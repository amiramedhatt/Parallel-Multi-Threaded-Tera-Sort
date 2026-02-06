#include <iostream>
#include "TeraSort.h"
#include "globalcutpoints.h"

using namespace std;

int main(int argc, char *argv[])
{
    if (argc != 7)
    {
        cerr << "Usage: " << argv[0] << " <input_file> <output_file> <num_mappers> <num_reducers> <sample_size> <algorithm>" << endl;
        cerr << "  algorithm: 'quick', 'heap', or 'insertion'" << endl;
        return 1;
    }
    
    char *input_filename = argv[1];
    char *output_filename = argv[2];
    int num_mappers = atoi(argv[3]);
    int num_reducers = atoi(argv[4]);
    int sample_size = atoi(argv[5]);
    string algorithm = argv[6];
    
    // Validate inputs
    if (num_mappers <= 0)
    {
        cerr << "Error: num_mappers must be positive" << endl;
        return 1;
    }
    if (num_reducers <= 0)
    {
        cerr << "Error: num_reducers must be positive" << endl;
        return 1;
    }
    if (sample_size <= 0)
    {
        cerr << "Error: sample_size must be positive" << endl;
        return 1;
    }
    if (algorithm != "quick" && algorithm != "heap" && algorithm != "insertion")
    {
        cerr << "Error: algorithm must be 'quick', 'heap', or 'insertion'" << endl;
        return 1;
    }
    
    cout << "Starting TeraSort..." << endl;
    cout << "Input: " << input_filename << endl;
    cout << "Output: " << output_filename << endl;
    cout << "Mappers: " << num_mappers << endl;
    cout << "Reducers: " << num_reducers << endl;
    cout << "Sample size: " << sample_size << endl;
    cout << "Algorithm: " << algorithm << endl;
    
    TeraSort teraSort(input_filename, output_filename, num_mappers, num_reducers, sample_size, algorithm);
    teraSort.sort();
    
    cout << "TeraSort completed successfully!" << endl;
    return 0;
}
