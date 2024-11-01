# Multi-Threaded Network and Wordle
## Project OverView
This project implements a multi-threaded TCP server in C that allows multiple clients to play a concurrent Wordle game. Using POSIX threads, each client connection is managed by a separate thread, enabling independent game sessions where each client attempts to guess a hidden word with up to six attempts.

## Features.

1. TCP Server: Listens on a specified port, accepting connections from multiple clients.
2. Multi-Threading with POSIX Threads: Each client connection spawns a new thread for isolated gameplay.
3. Wordle Game Mechanics: The server evaluates each client’s five-letter word guess, returning feedback on the correct, misplaced, or absent letters based on a hidden word selected at random from a specified dictionary.
4. Synchronization: Global variables track the total number of games won, lost, and total guesses across all threads, with necessary synchronization to avoid data conflicts.
5. Graceful Shutdown: The server can handle signal SIGUSR1 to terminate gracefully, closing all threads and deallocating memory.

# How to compile and Run
```bash
gcc -Wall -Werror hw3-main.c hw3.c -pthread
./hw3.out <listener-port> <seed> <dictionary-filename> <num-words>
```

# Example Usage
To start the server on port 8192 with a dictionary file containing 5757 words:
```bash
./hw3.out 8192 111 dict.txt 5757
```
