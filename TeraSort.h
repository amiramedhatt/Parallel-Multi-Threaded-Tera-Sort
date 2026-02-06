#ifndef TERASORT_H_
#define TERASORT_H_

#include <stdint.h>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <mutex>

#include "ThreadManager.h"
#include "Thread.h"
#include "globalcutpoints.h"
#include "sortingalgorithms.h"

typedef unsigned __int128 uint128_t;
using namespace std;

class Sortable
{

private:
public:
    Sortable();
    Sortable(Sortable &sortable);
    virtual uint128_t key() = 0;
    virtual void swap(Sortable *p_sortable) = 0;
    virtual bool operator>(Sortable *p_sortable) = 0;
    virtual bool operator<(Sortable *p_sortable) = 0;
    virtual bool operator>=(Sortable *p_sortable) = 0;
    virtual bool operator<=(Sortable *p_sortable) = 0;
    virtual bool operator==(Sortable *p_sortable) = 0;
    virtual bool operator!=(Sortable *p_sortable) = 0;
    virtual uint16_t hash(uint16_t p_reducers) = 0;
    virtual ~Sortable();
};

typedef struct
{
    uint8_t key[10];
    uint8_t b1[2];
    char rowid[32];
    uint8_t b2[4];
    uint8_t filler[48];
    uint8_t b3[4];
} teraitem_r;

class TeraSortItem : public Sortable
{
private:
    teraitem_r *teraitem;

public:
    TeraSortItem();
    TeraSortItem(teraitem_r *item);
    TeraSortItem(TeraSortItem *teraSortItem);
    TeraSortItem(TeraSortItem &teraSortItem);
    uint128_t key();
    void swap(Sortable *p_sortable);
    bool operator>(Sortable *p_sortable);
    bool operator<(Sortable *p_sortable);
    bool operator>=(Sortable *p_sortable);
    bool operator<=(Sortable *p_sortable);
    bool operator==(Sortable *p_sortable);
    bool operator!=(Sortable *p_sortable);
    uint16_t hash(uint16_t p_reducers);
    teraitem_r *getTeraItem();
    ~TeraSortItem();
};

class ShuffleBuffer
{
private:
    int size;
    int current_size;
    std::mutex mutex;

public:
    teraitem_r *teradata;
    ShuffleBuffer(int size);
    int getCurrentSize() { return current_size; }
    int getSize() { return size; }
    std::mutex &getMutex() { return mutex; }
    void incrementCurrentSize() { current_size++; }
    ~ShuffleBuffer();
};

class ShuffleBuffers
{
private:
    ShuffleBuffer **shuffleBuffers;
    int size;
    int count;

public:
    ShuffleBuffers(int count, int size);
    void append(int buffer_index, teraitem_r *teradata);
    void save(char *file_name);
    ShuffleBuffer *operator[](int index);
    ~ShuffleBuffers();
};

class Mapper : public Thread
{
private:
    teraitem_r *teradata;
    TeraSortItem **terasortItems;
    vector<uint128_t> mycutpoints;
    int p_index;
    int p_mapper_count;
    int p_reducer_counts;
    bool second_phase;
    char *p_input_filename;
    size_t file_portion_size;
    size_t sample_size;
    vector<TeraSortItem *> sample;
    vector<uint128_t> cut_points;
    ShuffleBuffers *shuffleBuffers;
    void readFilePortion();
    void buildSample(size_t sample_size);
    void sortSample();
    void createCutPoints();
    void Shuffle();
    bool operator>(Sortable *p_item);
    bool operator<(Sortable *p_item);

public:
    // FIX: Added sample_size parameter
    Mapper(int p_index, char *p_input_filename, int p_mapper_count, int p_reducer_counts, ShuffleBuffers *shuffleBuffers, int sample_size);
    void *threadMainBody(void *arg)
    {
        if (!second_phase)
        {
            readFilePortion();
            buildSample(sample_size);
            sortSample();
            createCutPoints();
            second_phase = true;
        }
        else
        {
            Shuffle();
        }
        return nullptr;
    }
    void setCutPoints(const std::vector<uint128_t> &cuts)
    {
        mycutpoints = cuts;
    }
    ~Mapper();
};

class Reducer : public Thread
{
private:
    ShuffleBuffer *shuffleBuffer;
    std::string algo;

public:
    Reducer(ShuffleBuffer *p_shuffleBuffer, string algo);
    void *threadMainBody(void *arg)
    {
        std::vector<TeraSortItem *> items;
        for (int i = 0; i < shuffleBuffer->getCurrentSize(); i++)
        {
            items.push_back(new TeraSortItem(&shuffleBuffer->teradata[i]));
        }
        if (algo == "quick")
        {
            quicksort(items, 0, (items.size() - 1));
        }
        if (algo == "heap")
        {
            heapsort(items);
        }
        if (algo == "insertion")
        {
            insertionSort(items, 0, (items.size() - 1));
        }
        for (int i = 0; i < items.size(); i++)
        {
            TeraSortItem *sortedItem = items[i];
            shuffleBuffer->teradata[i] = *(sortedItem->getTeraItem());
        }

        for (auto ptr : items)
        {
            delete ptr;
        }
        return nullptr;
    }
    ~Reducer();
};

class TeraSort
{
private:
    char *p_input_filename;
    char *p_output_filename;
    int p_mapper_count;
    int p_reducer_count;
    int sample_size;
    std::string algorithm;
    ShuffleBuffers *shuffleBuffer;

public:
    TeraSort(char *p_input_filename, char *p_output_filename, int p_mapper_count, int p_reducer_count, int sample_size, string algorithm)
    {
        this->p_input_filename = p_input_filename;
        this->p_output_filename = p_output_filename;
        this->p_mapper_count = p_mapper_count;
        this->p_reducer_count = p_reducer_count;
        this->sample_size = sample_size;
        this->algorithm = algorithm;
        shuffleBuffer = new ShuffleBuffers(p_reducer_count, 20000000);
    }
    void sort()
    {
        ThreadManager *mappersThreadManager = new ThreadManager();
        std::vector<Mapper *> mappers;
        for (int i = 0; i < p_mapper_count; i++)
        {
            // FIX: Pass sample_size to Mapper constructor
            Mapper *mapper = new Mapper(i, p_input_filename, p_mapper_count, p_reducer_count, shuffleBuffer, sample_size);
            mappers.push_back(mapper);
            mappersThreadManager->addThread(mapper);
        }
        
        /* phase 1 */
        mappersThreadManager->startRR();
        mappersThreadManager->barrier();
        
        int M = p_mapper_count;
        int R = p_reducer_count;
        std::vector<uint128_t> averageCutPoints(R - 1);

        for (int j = 0; j < R - 1; j++)
        {
            __uint128_t sum = 0;
            for (int m = 0; m < M; m++)
            {
                sum += globalcutpoints[m][j];
            }
            averageCutPoints[j] = sum / M;
        }

        for (int i = 0; i < M; i++)
        {
            mappers[i]->setCutPoints(averageCutPoints);
        }
        
        /* phase 2 */
        mappersThreadManager->startRR();
        mappersThreadManager->barrier();

        ThreadManager *reducersThreadManager = new ThreadManager();
        for (int i = 0; i < p_reducer_count; i++)
            reducersThreadManager->addThread(new Reducer((*shuffleBuffer)[i], algorithm));
        reducersThreadManager->startRR();

        reducersThreadManager->barrier();

        shuffleBuffer->save(p_output_filename);
        
        delete mappersThreadManager;
        delete reducersThreadManager;
    }
    ~TeraSort()
    {
        delete shuffleBuffer;
    }
};
#endif
