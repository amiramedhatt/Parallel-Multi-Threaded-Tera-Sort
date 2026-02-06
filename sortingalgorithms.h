#ifndef SORTINGALGORITHMS_H
#define SORTINGALGORITHMS_H

#include <vector>
class TeraSortItem;
void quicksort(std::vector<TeraSortItem*> & items, int left, int right);
void insertionSort(std::vector<TeraSortItem*> & items, int left, int right);
void heapify(std::vector<TeraSortItem*> & items, int n, int i);
void heapsort(std::vector<TeraSortItem*> & items);

#endif
