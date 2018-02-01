/******************************************************************
 * @authors Jonah Bukowsky, David Whitters
 * @date 2/1/18
 *
 * CIS 452
 * Dr. Dulimarta
 ****************************************************************/

#include <pthread.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

/** The maximum number of character that will be accepted from the user. */
#define MAX_NUM_INPUT_CHARS 1028

void *worker_thread(void *arg);

int main()
{
    /* Not used, handle to the latest thread created. */
    pthread_t latest_thread;
    int status;

    /* Holds the pointer to the user input. */
    char * str;

    /* Seed the random number generator. */
    srand(getpid());

    while(1)
    {
        /* Allocate memory for the user input. */
        str = (char *)malloc(MAX_NUM_INPUT_CHARS);
        /* Prompt the user for input. */
        printf("Please input a file name: ");
        /* Store the user input. */
        scanf("%s", str);

        /* Spawn a worker thread with the user input. */
        if ((status = pthread_create(&latest_thread, NULL, worker_thread, str)) != 0) {
            fprintf(stderr, "thread create error %d: %s\n", status, strerror(status));
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}

void *worker_thread(void *arg)
{

    return NULL;
}
