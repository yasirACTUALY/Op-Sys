#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

// Function to help calculate the interarrival times of CPU bursts.
double next_exp(double lambda, int bound) {
    while (1) {
        // Uniform distribution from [0.00, 1.00)
        double r = drand48();
        // Formula to calculate uniform to exponential distribution
        double x = -log(r) / lambda;
        // If the value is within our bounds, then return the value. Otherwise,
        // compute for another value
        if (x <= bound) return x;
    }
}

/*
int main() {
    int bound = 1024;
    double lambda = 0.001;
    srand48(32);
    double a = next_exp(lambda, bound);
    printf("a: %f\n", a);
    return EXIT_SUCCESS;
}
*/