#include "TeraSort.h"
#include "Thread.h"
#include "ThreadManager.h"
#include <functional>
#include "globalcutpoints.h"
#include "sortingalgorithms.h"

Mapper::Mapper(int p_index, char * p_input_filename, int p_mapper_count, int p_reducer_count, ShuffleBuffers * p_shuffleBuffer, int p_sample_size):Thread()
{
    this->p_index = p_index;
    this->p_input_filename = p_input_filename;
    this->p_mapper_count = p_mapper_count;
    this->p_reducer_counts = p_reducer_count;
    this->shuffleBuffers = p_shuffleBuffer;
    this->sample_size = p_sample_size;  // FIX: Initialize sample_size
    this->second_phase = false;
    this->teradata = NULL;
    this->terasortItems = NULL;
    this->file_portion_size = 0;
}

void Mapper::readFilePortion()
{
    fstream inputfile;
    inputfile.open(p_input_filename, ios::in | ios::binary);
    if (!inputfile)
    {
        cerr << "Error: Could not open input file " << p_input_filename << endl;
        exit(1);
    }
    inputfile.seekg(0, ios::end);
    size_t file_size = inputfile.tellg();
    size_t record_size = sizeof(teraitem_r);
    size_t record_count = file_size / record_size;
    size_t records_per_mapper = record_count / p_mapper_count;
    size_t start_record = p_index * records_per_mapper;
    size_t end_record = (p_index == p_mapper_count - 1) ? record_count : start_record + records_per_mapper;
    this->file_portion_size = (end_record - start_record) * record_size;
    teradata = new teraitem_r[file_portion_size / record_size];
    inputfile.seekg(start_record * record_size, ios::beg);
    inputfile.read((char *)teradata, file_portion_size);
    inputfile.close();
    
    size_t portion_size = file_portion_size / record_size;
    terasortItems = new TeraSortItem *[portion_size];
    for (size_t i = 0; i < portion_size; i++)
    {
        terasortItems[i] = new TeraSortItem(&teradata[i]);
    }
}

void Mapper::buildSample(size_t sample_size)
{
    size_t portion_size = file_portion_size / sizeof(teraitem_r);
    if (portion_size == 0) return;
    
    // FIX: Ensure we don't try to sample more than we have
    size_t actual_sample_size = (sample_size > portion_size) ? portion_size : sample_size;
    
    if (actual_sample_size == 0) return;
    
    size_t interval = portion_size / actual_sample_size;
    if (interval == 0) interval = 1;
    
    for (size_t i = 0; i < actual_sample_size && (i * interval) < portion_size; i++)
    {
        int index = i * interval;
        sample.push_back(terasortItems[index]);
    }
}

void Mapper::sortSample()
{
    size_t n = sample.size();
    if (n == 0) return;
    
    if (n < 10000)
        insertionSort(sample, 0, n - 1);
    else
        quicksort(sample, 0, n - 1);
}

void Mapper::createCutPoints()
{
    size_t n = sample.size();
    if (n == 0 || p_reducer_counts <= 1) return;
    
    for (int i = 1; i < p_reducer_counts; i++)
    {
        int index = i * n / p_reducer_counts;
        if (index >= n) index = n - 1;  // FIX: Boundary check
        cut_points.push_back(sample[index]->key());
    }
    
    {
        std::lock_guard<std::mutex> lock(globalcutpoints_mutex);
        if (globalcutpoints.size() <= p_index)
        {
            globalcutpoints.resize(p_index + 1);
        }
        globalcutpoints[p_index] = cut_points;
    }
}

void Mapper::Shuffle()
{
    size_t portion_size = file_portion_size / sizeof(teraitem_r);
    for (size_t i = 0; i < portion_size; ++i)
    {
        // extract the key for this item
        auto extracted_key = terasortItems[i]->key();

        int reducer_index = 0;

        if (!mycutpoints.empty()) {
            while (reducer_index < (int)mycutpoints.size() &&
                   extracted_key > mycutpoints[reducer_index]) {
                reducer_index++;
            }
        }

        // MUST CLAMP
        if (reducer_index >= p_reducer_counts)
            reducer_index = p_reducer_counts - 1;

        shuffleBuffers->append(reducer_index,
                               terasortItems[i]->getTeraItem());
    }
}

Mapper::~Mapper()
{
    if (teradata != NULL)
    {
        delete[] teradata;
        teradata = NULL;
    }
    
    if (terasortItems != NULL)
    {
        size_t portion_size = file_portion_size / sizeof(teraitem_r);
        for (size_t i = 0; i < portion_size; i++)
        {
            delete terasortItems[i];
        }
        delete[] terasortItems;
        terasortItems = NULL;
    }
}
