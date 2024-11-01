#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
void print(char **arr, int n)
{
    printf("Cache:\n");
    for (int i = 0; i < n; i++)
    {
        if (*(arr + i) != NULL)
        {
            printf("%c%d%c ==> \"", 91, i, 93);
            for (int j = 0; *(*(arr + i) + j) != '\0'; j++)
            {
                printf("%c", *(*(arr + i) + j));
            }
            printf("\"\n");
        }
    }
}
int main(int argc, char const **argv)
{
    // setvbuf(stdout, NULL, _IONBF, 0);
    if (argc < 3) // checks if the number of arguments is correct
    {
        fprintf(stderr, "ERROR: <Invalid number of arguments>\n");
        exit(1);
    }

    int n = atoi(*(argv + 1)); // n rows for the table size
    if (n < 1)                 // checks if n is a positive integer
    {
        fprintf(stderr, "ERROR: <Not a positive integer>\n");
        exit(1);
    }

    char **arr = calloc(n, sizeof(char *)); // hashmap to store n words

    char *buffer = calloc(1, sizeof(char));
    int currentRead = 0; // size of current word being read
    char *word = calloc(130, sizeof(char));
    for (int k = 2; k < argc; k++)
    {
        const char *filename = *(argv + k);  // name of file to parse words from
        int file = open(filename, O_RDONLY); // opens file
        if (file == -1)
        {
            fprintf(stderr, "ERROR: <File not found>\n");
            exit(1);
        }
        while (1)
        {
            int readChar = read(file, buffer, 1); // reads a char from the file
            // if a current read char is a letter and the read is succesful
            if (isalnum(*buffer) && readChar != 0)
            {
                *(word + currentRead) = *buffer;
                currentRead++; // increments the size of the word
            }
            else
            {
                if (currentRead > 2)
                {
                    *(word + currentRead) = '\0'; // ends the word
                    // calculates the hash
                    int hash = 0;
                    for (int i = 0; i < currentRead; i++)
                    {
                        hash += *(word + i);
                    }
                    hash = hash % n;
                    int re = 2; // 0 = alloc, 1 = realloc, 2 = nop
                    if (*(arr + hash) == NULL)
                    {
                        *(arr + hash) = calloc(currentRead + 1, sizeof(char));

                        strcpy(*(arr + hash), word);
                        re = 0;
                    }
                    else
                    {
                        int len = strlen(*(arr + hash));
                        if (len != currentRead)
                        {
                            re = 1;
                            *(arr + hash) = realloc(*(arr + hash), currentRead + 1);
                        }
                        strcpy(*(arr + hash), word);
                    }
                    if (re == 0)
                    {
                        printf("Word \"%s\" ==> %d (calloc)\n", word, hash);
                    }
                    else if (re == 1)
                    {
                        printf("Word \"%s\" ==> %d (realloc)\n", word, hash);
                    }
                    else
                    {
                        printf("Word \"%s\" ==> %d (nop)\n", word, hash);
                    }
                }
                currentRead = 0;
            }
            if (readChar == 0)
            {
                break;
            }
        }
        close(file);
    }

    printf("\n");
    print(arr, n);
    free(word);
    free(buffer);
    for (int i = 0; i < n; i++)
    {
        if (*(arr + i) != NULL)
        {
            free(*(arr + i));
        }
    }
    free(arr);

    return 0;
}