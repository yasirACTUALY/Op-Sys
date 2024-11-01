/* tcp-server-fork.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>

extern int total_guesses;
extern int total_wins;
extern int total_losses;
extern char **words;

int *client_socks;
int client_count = 0;
volatile int Stop = 0;
int num_words = 0;
int word_count;
char **valid_words;

pthread_mutex_t words_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t wins_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t losses_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t guess_mutex = PTHREAD_MUTEX_INITIALIZER;

// Check if the word is a valid word
int isValid(char *word)
{
    for (int i = 0; i < word_count; i++)
    {
        if (strcmp(*(valid_words + i), word) == 0)
        {
            return 1;
        }
    }
    return -1;
}

// checks word validity and turns it to capital
int valid(char *word)
{
    for (int i = 0; i < 5; i++)
    {
        if (!isalpha(*(word + i)))
        {
            // invalid number
            return -1;
        }
        else if (islower(*(word + i)))
        {
            // convert to uppercase
            *(word + i) = toupper(*(word + i));
        }
    }
    return 1;
}

// compares the guess with the word
char *comparason(char *word, char *guess, int *correct)
{
    *correct = 1;
    char *output = calloc(6, sizeof(char));
    // initialize the output
    for (int i = 0; i < 5; i++)
    {
        *(output + i) = '-';
    }
    *(output + 5) = '\0';

    // checks the corectness of the guess
    for (int i = 0; i < 5; i++)
    {
        char letter = *(word + i);
        if (letter == *(guess + i))
        {
            *(output + i) = letter;
        }
        else
        {
            *correct = 0;
            for (int j = 0; j < 5; j++)
            {
                // Ensure edge case incase duplicate letters
                if (letter == *(guess + j) && *(word + j) != letter)
                {
                    *(output + j) = letter + 32;
                    break;
                }
            }
        }
    }
    // if debug is enabled
    return output;
}

// parse the file into words
int parse_words(int fileFD)
{
    valid_words = calloc(word_count, sizeof(char *));
    // int file_size = 6 * word_count;
    // char *buffer = calloc(file_size, sizeof(char));

    // reads entire file into buffer then splits it into words
    // int temp_size = read(fileFD, buffer, file_size);

    // parse the file that is made  word_count 5 letter words ending wit a endline
    int c = 0;
    while (c < word_count)
    {
        char *buffer = calloc(6, sizeof(char));
        int tmp = read(fileFD, buffer, 6);
        if (tmp == 6 || tmp == 5)
        {
            *(valid_words + c) = calloc(6, sizeof(char));
            for (int i = 0; i < 5; i++)
            {
                if (islower(*(buffer + i)))
                {
                    *(buffer + i) = toupper(*(buffer + i));
                }
            }
            strcpy(*(valid_words + c), buffer);
            *(*(valid_words + c) + 5) = '\0';
        }
        else
        {
            free(buffer);
            perror("Error: File read error\n");
            return -1;
        }
        free(buffer);
        c++;
    }

    // if (temp_size == file_size)
    // {
    //     // saves the words into the dict skipping the "\n" chars
    //     for (int i = 0; i < word_count; i++)
    //     {
    //         *(valid_words + i) = calloc(6, sizeof(char));
    //         for (int j = 0; j < 5; j++)
    //         {
    //             *(*(valid_words + i) + j) = *(buffer + (i * 6) + j);
    //         }
    //         valid(*(valid_words + i));
    //         *(*(valid_words + i) + 5) = '\0';
    //     }
    //     free(buffer);
    // }
    // else
    // {
    //     free(valid_words);
    //     free(buffer);
    //     printf("ERROR: File read error\n");
    //     return -1;
    // }
    return 1;
}

// adds the hidden to words and increment client count
void add_word(char *word)
{

    pthread_mutex_lock(&words_mutex);
    *(words + num_words) = calloc(6, sizeof(char));
    strcpy(*(words + num_words++), word);
    pthread_mutex_unlock(&words_mutex);
}

void signal_handler(int signo)
{
    if (signo == SIGUSR1)
    {
        printf("MAIN: SIGUSR1 rcvd; Wordle server shutting down...\n");
        Stop = 1;
        // close all sockets
        for (int i = 0; i < client_count; i++)
        {
            close(*(client_socks + i));
        }
    }
#ifdef DEBUG

    if (signo == SIGTERM || signo == SIGINT)
    {
        printf("MAIN: SIGUSR1 rcvd; Wordle server shutting down...\n");
        Stop = 1;
        // close all sockets
        for (int i = 0; i < client_count; i++)
        {
            close(*(client_socks + i));
        }
    }
#endif
    free(client_socks);
}

void *client_game(void *arg)
{
    int newsd = *(int *)arg;
    uint16_t guesses = 6;
    char *hidden_word = *(valid_words + (rand() % word_count));
    add_word(hidden_word);

    // TO DO: add the word to the GLOBAL list of words "words"

    while (guesses > 0)
    {
        guesses--;
        printf("THREAD %lu: waiting for guess\n", (long unsigned int)pthread_self());
        char *guess = calloc(6, sizeof(char));
        char peek;
        if (recv(newsd, &peek, 1, MSG_PEEK) == 0)
        {
            // server dissconeected
            printf("THREAD %lu: client gave up; closing TCP connection...\n", (unsigned long)pthread_self());
            printf("THREAD %lu: game over; word was %s!\n", (unsigned long)pthread_self(), hidden_word);
            pthread_mutex_lock(&losses_mutex);
            total_losses++;
            pthread_mutex_unlock(&losses_mutex);
            // printf("freeing\n");
            free(guess);
            close(newsd);
            pthread_exit(NULL);
        }
        recv(newsd, guess, 5, MSG_WAITALL);
        *(guess + 5) = '\0';
        printf("THREAD %lu: rcvd guess: %s\n", (long unsigned int)pthread_self(), guess);
        char *response = calloc(8, sizeof(char));
        int valid_word = valid(guess);
        int valid_guess = isValid(guess);
        if (valid_word == -1 || valid_guess == -1)
        {
            guesses++;
            //  invalid guess
            *(response) = 'N';
            //*(response + 1) = htons(guesses);
            uint16_t guesses_left = htons((uint16_t)(guesses));
            *(uint16_t *)(response + 1) = guesses_left;
            for (int i = 0; i < 5; i++)
            {
                *(response + i + 3) = '?';
            }
            if (guesses == 1)
            {
                printf("THREAD %lu: invalid guess; sending reply: ????? (%d guess left)\n", (long unsigned int)pthread_self(), guesses);
                // printf("THREAD %lu: sending reply: %s (%d guess left)\n", (long unsigned int)pthread_self(), response, guesses);
            }
            else
            {
                printf("THREAD %lu: invalid guess; sending reply: ????? (%d guesses left)\n", (long unsigned int)pthread_self(), guesses);
                // printf("THREAD %lu: sending reply: %s (%d guesses left)\n", (long unsigned int)pthread_self(), response, guesses);
            }
            if (send(newsd, response, 8, 0) == -1)
            {
                perror("Error: send() failed\n");
                free(response);
                free(guess);
                break;
            }
            // printf("freeing\n");
            free(response);
            free(guess);
            continue;
        }
        // increase guesses count
        pthread_mutex_lock(&guess_mutex);
        total_guesses++;
        pthread_mutex_unlock(&guess_mutex);
        int correct = 0;
        char *result = comparason(hidden_word, guess, &correct);
        *(response) = 'Y';
        uint16_t guesses_left = htons((uint16_t)(guesses));
        *(uint16_t *)(response + 1) = guesses_left;
        // printf("Result: %d\n", guesses);
        for (int i = 0; i < 5; i++)
        {
            *(response + i + 3) = *(result + i);
        }
        if (guesses == 1)
        {
            printf("THREAD %lu: sending reply: %s (%d guess left)\n", (long unsigned int)pthread_self(), result, guesses);
        }
        else
        {
            printf("THREAD %lu: sending reply: %s (%d guesses left)\n", (long unsigned int)pthread_self(), result, guesses);
        }
        if (send(newsd, response, 8, 0) == -1)
        {
            perror("Error: send() failed\n");
            free(result);
            free(guess);
            free(response);
            break;
        }
        if (correct == 1)
        {
            // correct guess
            printf("THREAD %lu: game over; word was %s!\n", (long unsigned int)pthread_self(), hidden_word);
            // increment the wins
            pthread_mutex_lock(&wins_mutex);
            total_wins++;
            pthread_mutex_unlock(&wins_mutex);
            // printf("freeing\n");
            free(result);
            free(guess);
            free(response);
            close(newsd);
            pthread_exit(NULL);
        }
        else if (guesses == 0)
        {
            // out of guesses
            printf("THREAD %lu: game over; word was %s!\n", (long unsigned int)pthread_self(), hidden_word);
            pthread_mutex_lock(&losses_mutex);
            total_losses++;
            pthread_mutex_unlock(&losses_mutex);
        }
        free(result);
        free(guess);
        free(response);
    }

    close(newsd);
    pthread_exit(NULL);
}

void free_valid()
{
    for (int i = 0; i < word_count; i++)
    {
        free(*(valid_words + i));
    }
    free(valid_words);
}
int wordle_server(int argc, char **argv)
{

    total_guesses = 0;
    total_wins = 0;
    total_losses = 0;

    setvbuf(stdout, NULL, _IONBF, 0);

    // ignore signals
    signal(SIGINT, SIG_IGN);
    signal(SIGTERM, SIG_IGN);
    signal(SIGUSR2, SIG_IGN);
    signal(SIGUSR1, signal_handler);

#ifdef DEBUG
    signal(SIGINT, signal_handler);
#endif
    // validate the arguments
    if (argc != 5)
    {
        fprintf(stderr, "ERROR: Invalid argument(s)\nUSAGE: hw3.out <listener-port> <seed> <dictionary-filename> <num-words>\n");
        return EXIT_FAILURE;
    }
    int port = atoi(*(argv + 1));
    int seed = atoi(*(argv + 2));
    const char *dict = *(argv + 3);
    word_count = atoi(*(argv + 4));

    if (port <= 0 || seed <= 0 || word_count <= 0)
    {
        fprintf(stderr, "ERROR: Invalid argument(s)\nUSAGE: hw3.out <listener-port> <seed> <dictionary-filename> <num-words>\n");
        return EXIT_FAILURE;
    }

    int fileFD = open(dict, O_RDONLY);
    if (fileFD == -1)
    {
        perror("Error: open()1 failed\n");
        return EXIT_FAILURE;
    }

    // parse the dictionary words
    if (parse_words(fileFD) == -1)
    {
        return EXIT_FAILURE;
    }
    close(fileFD);
    printf("MAIN: opened %s (%d words)\n", dict, word_count);

    srand(seed);
    printf("MAIN: seeded pseudo-random number generator with %d\n", seed);

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));

    sa.sa_handler = SIG_IGN;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);

    // Set up signal handler for SIGUSR1
    // int client_sock;

    sa.sa_handler = signal_handler;
    // WILL CHANGE TO SIGUSR1
    sigaction(SIGUSR1, &sa, NULL);
#ifdef DEBUG
    sigaction(SIGINT, &sa, NULL);

#endif

    /* Create the listener socket as TCP socket (SOCK_STREAM) */
    int listener = socket(AF_INET, SOCK_STREAM, 0);

    if (listener == -1)
    {
        perror("Error: socket() failed\n");
        return EXIT_FAILURE;
    }

    // setup the server
    struct sockaddr_in tcp_server;
    // zero out the structure
    memset(&tcp_server, 0, sizeof(tcp_server));
    tcp_server.sin_family = AF_INET;
    tcp_server.sin_addr.s_addr = htonl(INADDR_ANY);
    tcp_server.sin_port = htons(port);
    words = calloc(word_count, sizeof(char *));

    if (bind(listener, (struct sockaddr *)&tcp_server, sizeof(tcp_server)) == -1)
    {
        // free(words);
        free_valid();
        perror("Error: bind() failed\n");
        return EXIT_FAILURE;
    }
    if (listen(listener, 5) == -1)
    {
        perror("Error: listen() failed\n");
        return EXIT_FAILURE;
    }
    printf("MAIN: Wordle server listening on port {%d}\n", port);
    client_socks = calloc(20, sizeof(int));
    int newsd;
    while (!Stop)
    {
        newsd = accept(listener, NULL, NULL);
        if (newsd == -1)
        {
            if (errno == EINTR)
            {
                // Interrupted by signal
                continue;
            }
            perror("Error: accept() failed\n");
            continue;
        }
        *(client_socks + client_count++) = newsd;
        pthread_t thread;
        // creates a thread to deal with client
        printf("MAIN: rcvd incoming connection request\n");
        if (pthread_create(&thread, NULL, client_game, (void *)&newsd) != 0)
        {
            perror("Error: pthread_create() failed\n");
            close(newsd);
            client_count--;
            continue;
        }
        pthread_detach(thread);
    }

    close(listener);

    free_valid();
    // free(words);
    return EXIT_SUCCESS;
}
