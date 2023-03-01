#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define A 1
#define B 40
#define N 10000000

double f(double x);
double trapezoides(double a, double b, int n);

int main(int argc, char* argv[]) {
    double integral;
    double a=A, b=B;
    int n=N;
    double start_time, end_time;
    int num_threads;

    if(argc > 2) {
        a = strtol(argv[1], NULL, 10);
        b = strtol(argv[2], NULL, 10);
    }

    start_time = omp_get_wtime();
    integral = trapezoides(a,b,n);
    end_time = omp_get_wtime();
    num_threads = omp_get_max_threads();

    printf("Con n = %d trapezoides, nuestra aproximacion \n",n);
    printf("de la integral de %f a %f es = %.10f\n", a,b,integral);
    printf("Tiempo transcurrido: %lf segundos\n", end_time - start_time);
    printf("Numero de hilos: %d\n", num_threads);

    return 0;
}

double trapezoides(double a, double b, int n) {
    double integral, h;
    int k;

    h = (b-a)/n;
    integral = (f(a) + f(b)) / 2.0;

#pragma omp parallel
    {
        #pragma omp single
        {
            for(k = 1; k <= n-1; k++) {
                #pragma omp task
                integral += f(a + k*h);
            }
        }
    }

    integral = integral * h;

    return integral;
}

double f(double x) {
    double return_val;

    return_val = x*x;

    return return_val;
}
