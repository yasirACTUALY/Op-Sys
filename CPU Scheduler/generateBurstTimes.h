#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

char **generateProcessIDs(int n);
double next_exp(double lambda, int bound);

// Function to generate CPU and IO burst times for a process. CPU bound
// processes have longer CPU burst times by a factor of 4 and shorter IO burst
// times by a factor of 8.
void generateBurstTimes(int n, int n_cpu, double lambda, int bound, char **pids)
{
    // Needed to calculate the averages
    int CPUBoundCPUBurstTimeSum = 0;
    int CPUBoundCPUBurstTimeCount = 0;
    int CPUBoundIOBurstTimeSum = 0;
    int CPUBoundIOBurstTimeCount = 0;

    int IOBoundCPUBurstTimeSum = 0;
    int IOBoundCPUBurstTimeCount = 0;
    int IOBoundIOBurstTimeSum = 0;
    int IOBoundIOBurstTimeCount = 0;

    int totalCPUBurstTimeSum = 0;
    int totalIOBurstTimeSum = 0;

    int CPUProcessesLeft = n_cpu;
    for (int i = 0; i < n; i++)
    {
        int arrivalTime = floor(next_exp(lambda, bound));
        int numBursts = ceil(drand48() * 32);
        // The process is a CPU-bound process
        if (CPUProcessesLeft > 0)
        {
            if (numBursts > 1)
                printf("CPU-bound process %s: arrival time %dms; %d CPU bursts:\n", *(pids + i), arrivalTime, numBursts);
            else
                printf("CPU-bound process %s: arrival time %dms; %d CPU burst:\n", *(pids + i), arrivalTime, numBursts);
            // Calculate the CPU and I/O burst times. If it is the last burst,
            // only calculate the CPU burst time.
            for (int j = 0; j < numBursts; j++)
            {
                int CPUBurstTime = ceil(next_exp(lambda, bound)) * 4;
                CPUBoundCPUBurstTimeSum += CPUBurstTime;
                totalCPUBurstTimeSum += CPUBurstTime;
                CPUBoundCPUBurstTimeCount++;
                if (j != numBursts - 1)
                {
                    int IOBurstTime = ceil(next_exp(lambda, bound));
                    printf("==> CPU burst %dms ==> I/O burst %dms\n", CPUBurstTime, IOBurstTime);

                    CPUBoundIOBurstTimeSum += IOBurstTime;
                    totalIOBurstTimeSum += IOBurstTime;
                    CPUBoundIOBurstTimeCount++;
                }
                else
                    printf("==> CPU burst %dms\n", CPUBurstTime);
            }
        }
        // The process is an I/O-bound process
        else
        {
            if (numBursts > 1)
                printf("I/O-bound process %s: arrival time %dms; %d CPU bursts:\n", *(pids + i), arrivalTime, numBursts);
            else
                printf("I/O-bound process %s: arrival time %dms; %d CPU burst:\n", *(pids + i), arrivalTime, numBursts);
            // Calculate the CPU and I/O-bound times. If it is the last burst,
            // only calculate the CPU burst time
            for (int j = 0; j < numBursts; j++)
            {
                int CPUBurstTime = ceil(next_exp(lambda, bound));
                IOBoundCPUBurstTimeSum += CPUBurstTime;
                totalCPUBurstTimeSum += CPUBurstTime;
                IOBoundCPUBurstTimeCount++;
                if (j != numBursts - 1)
                {
                    int IOBurstTime = ceil(next_exp(lambda, bound)) * 8;
                    printf("==> CPU burst %dms ==> I/O burst %dms\n", CPUBurstTime, IOBurstTime);

                    IOBoundIOBurstTimeSum += IOBurstTime;
                    totalIOBurstTimeSum += IOBurstTime;
                    IOBoundIOBurstTimeCount++;
                }
                else
                    printf("==> CPU burst %dms\n", CPUBurstTime);
            }
        }
        CPUProcessesLeft--;
    }

    int n_io = n - n_cpu;
    // Round the averages to the third decimal place using ceil
    double CPUBoundAvgCPUBurstTime = 0.0;
    double IOBoundAvgCPUBurstTime = 0.0;
    double totalCPUBurstTimeAvg = 0.0;
    double CPUBoundAvgIOBurstTime = 0.0;
    double IOBoundAvgIOBurstTime = 0.0;
    double totalIOBurstTimeAvg = 0.0;
    if (CPUBoundCPUBurstTimeCount != 0) CPUBoundAvgCPUBurstTime = ceil(((double)CPUBoundCPUBurstTimeSum / (double)CPUBoundCPUBurstTimeCount) * 1000.0) / 1000.0;
    if (IOBoundCPUBurstTimeCount != 0) IOBoundAvgCPUBurstTime = ceil(((double)IOBoundCPUBurstTimeSum / (double)IOBoundCPUBurstTimeCount) * 1000.0) / 1000.0;
    if ((CPUBoundCPUBurstTimeCount + IOBoundCPUBurstTimeCount) != 0) totalCPUBurstTimeAvg = ceil(((double)totalCPUBurstTimeSum / ((double)CPUBoundCPUBurstTimeCount + (double)IOBoundCPUBurstTimeCount)) * 1000.0) / 1000.0;
    if (CPUBoundIOBurstTimeCount != 0 ) CPUBoundAvgIOBurstTime = ceil(((double)CPUBoundIOBurstTimeSum / (double)CPUBoundIOBurstTimeCount) * 1000.0) / 1000.0;
    if (IOBoundIOBurstTimeCount != 0) IOBoundAvgIOBurstTime = ceil(((double)IOBoundIOBurstTimeSum / (double)IOBoundIOBurstTimeCount) * 1000.0) / 1000.0;
    if ((CPUBoundIOBurstTimeCount + IOBoundIOBurstTimeCount) != 0) totalIOBurstTimeAvg = ceil(((double)totalIOBurstTimeSum / ((double)CPUBoundIOBurstTimeCount + (double)IOBoundIOBurstTimeCount)) * 1000.0) / 1000.0;

    FILE *fptr;
    fptr = fopen("simout.txt", "w");
    fprintf(fptr, "-- number of processes: %d\n", n);
    fprintf(fptr, "-- number of CPU-bound processes: %d\n", n_cpu);
    fprintf(fptr, "-- number of I/O-bound processes: %d\n", n_io);
    fprintf(fptr, "-- CPU-bound average CPU burst time: %.3f ms\n", CPUBoundAvgCPUBurstTime);
    fprintf(fptr, "-- I/O-bound average CPU burst time: %.3f ms\n", IOBoundAvgCPUBurstTime);
    fprintf(fptr, "-- overall average CPU burst time: %.3f ms\n", totalCPUBurstTimeAvg);
    fprintf(fptr, "-- CPU-bound average I/O burst time: %.3f ms\n", CPUBoundAvgIOBurstTime);
    fprintf(fptr, "-- I/O-bound average I/O burst time: %.3f ms\n", IOBoundAvgIOBurstTime);
    fprintf(fptr, "-- overall average I/O burst time: %.3f ms\n", totalIOBurstTimeAvg);

    // Deallocate memory
    fclose(fptr);
}

/*
int main() {
    int n = 3, n_cpu = 1, bound = 1024;
    double lambda = 0.001;
    char** pids = generateProcessIDs(n);
    srand48(32);
    generateBurstTimes(n, n_cpu, lambda, bound, pids);

    return EXIT_SUCCESS;
}
*/