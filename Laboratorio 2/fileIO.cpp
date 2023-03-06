#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <chrono> // include chrono library
#include <omp.h>
#include <stdio.h>
using namespace std;
using namespace std::chrono; // using chrono library
void par_qsort(int *data, int lo, int hi)
{
    if (lo > hi)
        return;
    int l = lo;
    int h = hi;
    int p = data[(hi + lo) / 2];
    while (l <= h)
    {
        while ((data[l] - p) < 0)
            l++;
        while ((data[h] - p) > 0)
            h--;
        if (l <= h)
        {
            // swap
            int tmp = data[l];
            data[l] = data[h];
            data[h] = tmp;
            l++;
            h--;
        }
    }
#pragma omp task if (h - lo > 150) mergeable
    {
        par_qsort(data, lo, h);
    }
    par_qsort(data, l, hi);
}
int main()
{
    int N; // number of random numbers to generate
    cout << "Enter the number of random numbers to generate: ";
    cin >> N;                        // number of random numbers to generate
    int *Array = new int[N];         // allocate memory in the heap
    ofstream outFile("numbers.csv"); // create output file stream
    double ti, tf, t;
    double t_sec = 4.35;
    int thread_count = 6;
    ti = omp_get_wtime();
    // generate N random numbers and write them to file
    srand(time(nullptr)); // set random seed based on current time
#pragma omp parallel for num_threads(thread_count)
    for (int i = 0; i < N; i++)
    {
        Array[i] = rand() % 100; // generate random number between 0 and 99
        outFile << Array[i];     // write number to file
        if (i < N - 1)
            outFile << ","; // separate numbers with comma
    }
    outFile.close(); // close output file stream
    // read numbers from file and store them in local memory
    ifstream inFile("numbers.csv"); // create input file stream
    string line;
    getline(inFile, line);
    const char *delim = ",";
    char *token = strtok(const_cast<char *>(line.c_str()), delim);
    int i = 0;
    while (token != nullptr)
    {
        Array[i++] = atoi(token); // convert string to int and store in Array
        token = strtok(nullptr, delim);
    }
    inFile.close(); // close input file stream
// sort numbers in ascending order
#pragma omp parallel num_threads(thread_count)
    {
#pragma omp single
        {
            par_qsort(Array, 0, N - 1);
        }
    }
    // write sorted numbers to file
    ofstream sortedFile("sorted_numbers.csv"); // create output file stream
#pragma omp parallel for num_threads(thread_count)
    for (int i = 0; i < N; i++)
    {
        sortedFile << Array[i]; // write number to file
        if (i < N - 1)
            sortedFile << ","; // separate numbers with comma
    }
    sortedFile.close(); // close output file stream
    tf = omp_get_wtime();
    t = tf - ti;
    printf("threads = %d\n", thread_count);
    printf("tiempo secuencial 10000000 %f segundos\n", t_sec);
    printf("tiempo %f segundos paralelo\n", t);
    printf("SpeedUp %f\n", t_sec / t);
    printf("Eficiencia %f\n", (t_sec / t) / thread_count);
    delete[] Array; // free memory allocated in the heap
    return 0;
}