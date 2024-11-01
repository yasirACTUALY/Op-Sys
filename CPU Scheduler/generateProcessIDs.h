#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Function to generate process IDs for the total number of processes. Process
// IDs are assigned in order from A0, A1, A2, ..., A9, B0, B1, ..., Z9.
char** generateProcessIDs(int n) {
    // Return value. Holds all process IDs
    char** IDs = calloc(n, sizeof(char*));

    // Assign a letter and number based on n, the number of processes.
    for (int i = 0; i < n; i++) {
        char letter = (char)(65 + (i / 10));
        char number = (i % 10) + '0';
        *(IDs + i) = calloc(3, sizeof(char));
        *(*(IDs + i) + 0) = letter;
        *(*(IDs + i) + 1) = number;
        *(*(IDs + i) + 2) = '\0';
    }
    return IDs;
}

/*
int main() {
    int n = 3;
    char** IDs = generateProcessIDs(n);
    for (int i = 0; i < n; i++) {
        printf("ID: %s\n", *(IDs + i));
        free(*(IDs + i));
    }
    free(IDs);

    return EXIT_SUCCESS;
}
*/