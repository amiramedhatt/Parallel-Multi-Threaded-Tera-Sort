#!/bin/bash

# Ultra-Simple: Just time 1GB and 2GB, that's it!

echo "Testing 1GB dataset..."
time ./terasort unsorted-1gb output-1gb.dat 4 4 1000 quick
rm -f output-1gb.dat
echo ""

echo "Testing 2GB dataset..."
time ./terasort unsorted-2gb output-2gb.dat 4 4 1000 quick
rm -f output-2gb.dat
echo ""

echo "Done!"
