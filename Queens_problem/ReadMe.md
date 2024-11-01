# Queens Problem in (m,n) grid

## Overview
This is an implementation of a multi-process solution in C to solve a variant of the **n-Queens Problem**. Instead of a square board, this problem generalizes to an `m x n` chessboard, where the goal is to:
1. Find the maximum number of non-attacking Queens that can be placed.
2. Determine the number of distinct solutions if they exist.
3. Track the number of "dead ends" (where no valid move is possible).

The program uses `fork()`, `waitpid()`, and `pipe()` for process synchronization and communication, generating a process tree that explores possible Queen placements through a brute-force approach.
## Execution Example

To run the program on a 3x3 board, use:

```bash
./hw2.out 3 3
```

## Compilation Modes
* Quit Mode : To display minimal output, use the following compile command:
```bash
gcc -Wall -Werror -D QUIET -o hw2.out hw2.c
```
* No Parallel Mode: To run child processes sequentially for easier debugging:
```bash
gcc -Wall -Werror -D NO_PARALLEL -o hw2.out hw2.c
```
* Debug Mode: To display additional debugging information, compile as follows:
```bash
gcc -Wall -Werror -D DEBUG_MODE hw2.c
```





