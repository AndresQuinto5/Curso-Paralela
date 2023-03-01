#include <iostream>
#include <omp.h>
#include <chrono>
using namespace std;

int fibonacci(int n) {
    if (n <= 1)
        return n;

    int x, y;
#pragma omp task if (n > 20) shared(x)
    x = fibonacci(n-1);

#pragma omp task if (n > 20) shared(y)
    y = fibonacci(n-2);

#pragma omp taskwait
    return x + y;
}

int fibonacci_seq(int n) {
    if (n <= 1)
        return n;

    return fibonacci_seq(n-1) + fibonacci_seq(n-2);
}

int main() {
    int n;
    int fib;
    int num_threads = omp_get_max_threads();

    cout << "Enter the index of the Fibonacci number to compute: ";
    cin >> n;

    auto start_time = chrono::steady_clock::now();
    fib = fibonacci(n);
    auto end_time = chrono::steady_clock::now();
    auto parallel_time = chrono::duration_cast<chrono::microseconds>(end_time - start_time).count();

    start_time = chrono::steady_clock::now();
    int fib_seq = fibonacci_seq(n);
    end_time = chrono::steady_clock::now();
    auto sequential_time = chrono::duration_cast<chrono::microseconds>(end_time - start_time).count();

    double speedup = (double) sequential_time / parallel_time;
    double efficiency = speedup / num_threads;

    cout << "Fibonacci number at index " << n << " (parallel): " << fib << endl;
    cout << "Fibonacci number at index " << n << " (sequential): " << fib_seq << endl;
    cout << "Parallel time: " << parallel_time << " us" << endl;
    cout << "Sequential time: " << sequential_time << " us" << endl;
    cout << "Speedup: " << speedup << endl;
    cout << "Efficiency: " << efficiency << endl;
    cout << "Threads: " << num_threads << endl;


    return 0;
}
