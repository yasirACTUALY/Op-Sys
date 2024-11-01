# CPU Scheduling Simulation

## Overview
This algorithm simulates an operating system focused on managing processes in a CPU scheduling environment. It will simulate processes in various states and implement different CPU scheduling algorithms, allowing comparison of scheduling performance metrics.

## Simulation
The simulation emulates the following three states for processes:

* RUNNING: actively using the CPU
* READY: waiting in a queue for CPU allocation.
* WAITING: blocked on I/O or other events.

Processes are generated with random attributes based on defined parameters.

## Usage
Run the program with:

```bash
./project <number_of_processes> <cpu_bound_processes> <seed> <lambda> <upper_bound>

```

## Example
```bash
./project 3 1 32 0.001 1024
```
