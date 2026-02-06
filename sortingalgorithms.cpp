#include <vector>
#include <algorithm>
#include "TeraSort.h"
#include "sortingalgorithms.h"

void quicksort(std::vector<TeraSortItem*> & items, int left, int right)
{
    if (left < right) {
        if (right - left < 10) {
            insertionSort(items, left, right);
        } else {
            int pivotIndex = left + (right - left) / 2;
            TeraSortItem* pivotValue = items[pivotIndex];
            std::swap(items[pivotIndex], items[right]); // Move pivot to end
            int storeIndex = left;
            for (int i = left; i < right; i++) {
                if (items[i]->key() < pivotValue->key()) {
                    std::swap(items[i], items[storeIndex]);
                    storeIndex++;
                }
            }
            std::swap(items[storeIndex], items[right]); // Move pivot to its final place
            quicksort(items, left, storeIndex - 1);
            quicksort(items, storeIndex + 1, right);
        }
    }
}
void insertionSort(std::vector<TeraSortItem*> & items, int left, int right)
{
    for (int i = left + 1; i <= right; i++) {
        TeraSortItem* key = items[i];
        int j = i - 1;
        while (j >= left && items[j]->key() > key->key()) {
            items[j + 1] = items[j];
            j--;
        }
        items[j + 1] = key;
    }
}
void heapify(std::vector<TeraSortItem*> & items, int n, int i)
{
    int largest = i; // Initialize largest as root
    int left = 2 * i + 1; // left = 2*i + 1
    int right = 2 * i + 2; // right = 2*i + 2
    // If left child is larger than root
    if (left < n && items[left]->key() > items[largest]->key())
        largest = left;
    // If right child is larger than largest so far
    if (right < n && items[right]->key() > items[largest]->key())
        largest = right;
    // If largest is not root
    if (largest != i) {
        std::swap(items[i], items[largest]);
        // Recursively heapify the affected sub-tree
        heapify(items, n, largest);
    }
}
void heapsort(std::vector<TeraSortItem*> & items)
{
    int n = items.size();
    // Build heap (rearrange array)
    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(items, n, i);
    // One by one extract an element from heap
    for (int i = n - 1; i > 0; i--) {
        std::swap(items[0], items[i]); // Move current root to end
        heapify(items, i, 0); // call max heapify on the reduced heap
    }
}
