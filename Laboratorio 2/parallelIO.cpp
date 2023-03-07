#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <chrono> // include chrono library
#include <omp.h> // include OpenMP library

using namespace std;
using namespace std::chrono; // using chrono library

void quicksort(int *Array, int left, int right) {
    if (left < right) {
        int pivot = Array[(left + right) / 2];
        int i = left - 1;
        int j = right + 1;
        while (true) {
            do {
                i++;
            } while (Array[i] < pivot);
            do {
                j--;
            } while (Array[j] > pivot);
            if (i >= j) {
                break;
            }
            swap(Array[i], Array[j]);
        }
        #pragma omp task
        quicksort(Array, left, j);
        #pragma omp task
        quicksort(Array, j + 1, right);
    }
}

int main() {
    int N; // number of random numbers to generate
    cout << "Enter the number of random numbers to generate: ";
    cin >> N; // number of random numbers to generate
    int *Array = new int[N]; // allocate memory in the heap
    ofstream outFile("numbers.csv"); // create output file stream

    // generate N random numbers and write them to file
    srand(time(nullptr)); // set random seed based on current time
    for (int i = 0; i < N; i++) {
        Array[i] = rand() % 100; // generate random number between 0 and 99
        outFile << Array[i]; // write number to file
        if (i < N - 1) outFile << ","; // separate numbers with comma
    }

    outFile.close(); // close output file stream

    // read numbers from file and store them in local memory
    ifstream inFile("numbers.csv"); // create input file stream
    string line;
    getline(inFile, line);
    const char* delim = ",";
    char* token = strtok(const_cast<char*>(line.c_str()), delim);
    int i = 0;
    while (token != nullptr) {
        Array[i++] = atoi(token); // convert string to int and store in Array
        token = strtok(nullptr, delim);
    }

    inFile.close(); // close input file stream

    // time the execution of the parallel code
    auto start_time = high_resolution_clock::now(); // get start time
    
    #pragma omp parallel
    {
        int num_threads = omp_get_num_threads(); // get number of threads
        #pragma omp single
        {
            cout << "Using " << num_threads << " threads" << endl; // print number of threads
        }
        #pragma omp single nowait
        quicksort(Array, 0, N - 1);
    }
    auto end_time = high_resolution_clock::now(); // get end time

    // calculate the duration of the parallel code
    auto duration = duration_cast<microseconds>(end_time - start_time);

    cout << "Execution time of parallel code: " << duration.count() << " microseconds" << endl;

    // write sorted numbers to file
    ofstream sortedFile("sorted_numbers.csv"); // create output file stream
    for (int i = 0; i < N; i++) {
        sortedFile << Array[i]; // write number to file
        if (i < N - 1) sortedFile << ","; // separate numbers with comma
    }
    sortedFile.close(); // close output file stream

    delete[] Array; // free memory allocated in the heap

    return 0;
}
