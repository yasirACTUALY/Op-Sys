/*
Part I: CPU Scheduling and Processes
The program will be given 5 command line arguments:
    - n, the number of processes
    - n_cpu, the number of processes that are CPU-bound. CPU-bound processes
      have CPU burst times that are longer by a factor of 4. I/O-bound processes
      have CPU burst times that are shorter by a factor of 8.
    - seed, the seed for the pseudo-random number generator
    - lambda, used to determine interarrival times
    - bound, the upper bound for valid pseudo-random numbers.
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#include "generateProcessIDs.h"
#include "generateBurstTimes.h"
#include "next_exp.h"

char **generateProcessIDs(int n);
double next_exp(double lambda, int bound);
void generateBurstTimes(int n, int n_cpu, double lambda, int bound, char **pids);

int main(int argc, char **argv)
{
    if (argc != 6)
    {
        fprintf(stderr, "ERROR: USAGE <n> <n_cpu> <seed> <lambda> <bound>\n");
        return EXIT_FAILURE;
    }

    // Get n, n_cpu, seed, lambda, and the upper bound
    int n = atoi(*(argv + 1));
    if (n <= 0 || n > 260)
    {
        fprintf(stderr, "ERROR: Invalid number of processes (n)\n");
        return EXIT_FAILURE;
    }
    int n_cpu = atoi(*(argv + 2));
    int seed = atoi(*(argv + 3));
    if (n_cpu < 0 || seed < 0) {
        fprintf(stderr, "n_cpu and/or seed must be greater than 0.\n");
        return EXIT_FAILURE;
    }
    if (n_cpu > n) {
        fprintf(stderr, "ERROR: Number of CPU processes cannot be greater than n.\n");
        return EXIT_FAILURE;
    }
    double lambda = atof(*(argv + 4));
    int bound = atof(*(argv + 5));
    if (lambda <= 0 || bound <= 0)
    {
        fprintf(stderr, "ERROR: Invalid lambda or bound\n");
        return EXIT_FAILURE;
    }

    printf("<<< PROJECT PART I\n");
    if (n_cpu == 1)
        printf("<<< -- process set (n=%d) with %d CPU-bound process\n", n, n_cpu);
    else
        printf("<<< -- process set (n=%d) with %d CPU-bound processes\n", n, n_cpu);
    printf("<<< -- seed=%d; lambda=%f; bound=%d\n", seed, lambda, bound);

    // Get the process ID's and set the seed for the pseudo-random number
    // generator
    char **pids = generateProcessIDs(n);
    srand48(seed);
    generateBurstTimes(n, n_cpu, lambda, bound, pids);

    // Deallocate memory used
    for (int i = 0; i < n; i++)
    {
        free(*(pids + i));
    }
    free(pids);

    return EXIT_SUCCESS;
}