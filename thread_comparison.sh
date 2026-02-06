#!/bin/bash

# Simple: 1 Thread vs Multiple Threads Comparison

echo "==========================================="
echo "1 Thread vs Multiple Threads Comparison"
echo "==========================================="
echo ""

# Choose your dataset (change this to unsorted-1gb or unsorted-2gb)
DATASET="unsorted-1gb"

echo "Testing with dataset: $DATASET"
echo ""

# Test with 1 thread
echo "-------------------------------------------"
echo "Testing with 1 THREAD..."
echo "-------------------------------------------"
time ./terasort $DATASET output-1thread.dat 1 1 5000 heap
rm -f output-1thread.dat
echo ""

# Test with 2 threads
echo "-------------------------------------------"
echo "Testing with 2 THREADS..."
echo "-------------------------------------------"
time ./terasort $DATASET output-2threads.dat 2 2 5000 heap
rm -f output-2threads.dat
echo ""

# Test with 4 threads
echo "-------------------------------------------"
echo "Testing with 4 THREADS..."
echo "-------------------------------------------"
time ./terasort $DATASET output-4threads.dat 4 4 5000 heap
rm -f output-4threads.dat
echo ""

# Test with 8 threads
echo "-------------------------------------------"
echo "Testing with 8 THREADS..."
echo "-------------------------------------------"
time ./terasort $DATASET output-8threads.dat 8 8 5000 heap
rm -f output-8threads.dat
echo ""

echo "==========================================="
echo "Done! Compare the 'real' times above"
echo "==========================================="
