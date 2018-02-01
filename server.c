/******************************************************************
 * @authors Jonah Bukowsky, David Whitters
 * @date 2/1/18
 *
 * CIS 452
 * Dr. Dulimarta
 ****************************************************************/

#include <pthread.h>
#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

/** The maximum number of character that will be accepted from the user. */
#define MAX_NUM_INPUT_CHARS 1028

/** Set to 1 when the interrupt handler was called. */
int Int_Received = 0u;
/** The number of file requests that have been been completed. */
int Num_Successful_Requests = 0u;
/** The mutex for the shared data Num_Successful_Requests. */
pthread_mutex_t num_req_mutex = PTHREAD_MUTEX_INITIALIZER;
/** The total amount of time processing file requests. */
int Total_Time = 0u;
/** The mutex for the shared data Total_Time. */
pthread_mutex_t time_mutex = PTHREAD_MUTEX_INITIALIZER;

/**
    Signal handler for the SIGINT signal.

    @param sig_num
        The signal to handle.
*/
void Sig_Handler(int sig_num)
{
    /* sig_num is not used in this handler. */
    (void)sig_num;

    printf(" received.\n");
    Int_Received = 1u; /* Signal that this interrupt has occurred. */

    /* Unlock the mutexes so they can be destoyed. */
    pthread_mutex_unlock(&num_req_mutex);
    pthread_mutex_unlock(&time_mutex);
}

/**
    Delays a random amount of time to emulate a file lookup.
    Then reports what file was "accessed".

    @param arg
        Accepts any argument, in this case a character pointer to the
        user input is expected.

    @return
        Nothing in this case.
*/
void *worker_thread(void *arg)
{
    /* Lock the shared data. Waits here until a lock is acquired. */
    pthread_mutex_lock(&num_req_mutex);
    pthread_mutex_lock(&time_mutex);

    int sleep_time = 0;
    /* get filename passed in by main thread */
    char * filename = (char *) arg;

    /* detach thread so join is not needed */
    pthread_detach(pthread_self());

    if ((rand() % 10) < 8) /* Emulate disk cache access. */
    {
        /* 80% chance of happening */
        sleep_time = 1;
    }
    else /* Emulate hard drive access. */
    {
        /* 20% chance of happening */
        /* sleep 7 - 10 seconds */
        sleep_time = (rand() % 4) + 7;
    }

    sleep(sleep_time);

    printf("\nAccessed file: %s\n", filename);

    /* Update metrics. */
    Total_Time += sleep_time;
    ++Num_Successful_Requests;

    /* free memory allocated for filename */
    free(filename);

    /* Unlock the shared data to allow other threads to access it. */
    pthread_mutex_unlock(&num_req_mutex);
    pthread_mutex_unlock(&time_mutex);

    pthread_exit(NULL);
}

int main(void)
{
    /* Used for handling the interrupt signal. */
    struct sigaction act;
    act.sa_handler = &Sig_Handler; /* Set the handler. */
    act.sa_flags = 0;
    /* Not used, handle to the latest thread created. */
    pthread_t latest_thread;
    int status;
    /* Holds the pointer to the user input. */
    char * str;

    /* Setup mutexes. */
    if (pthread_mutex_init(&time_mutex, NULL) != 0) /* Returns 0 on success. */
    {
        perror("\n Time mutex init failed\n");
        exit(EXIT_FAILURE);
    }
    if (pthread_mutex_init(&num_req_mutex, NULL) != 0)
    {
        perror("\n Request mutex init failed\n");
        exit(EXIT_FAILURE);
    }

    /* Setup the signal handler for the interrupt signal. */
    if(sigaction(SIGINT, &act, NULL) < 0)
    {
        perror("SIGACTION ERROR\n");
        exit(EXIT_FAILURE);
    }

    /* Seed the random number generator. */
    srand(getpid());

    while(0 == Int_Received)
    {
        /* Allocate memory for the user input. */
        str = (char *)malloc(MAX_NUM_INPUT_CHARS);
        /* Prompt the user for input. */
        printf("Please input a file name: ");
        /* Store the user input. */
        fgets(str, MAX_NUM_INPUT_CHARS, stdin);
        /* Remove newline character. */
        str[strcspn(str, "\n")] = 0;

        /* Check for invalid input. */
        if(strlen(str) <= 0)
        {
            free(str);
            continue;
        }

        /* Spawn a worker thread with the user input. */
        if ((status = pthread_create(&latest_thread, NULL, worker_thread, str)) != 0) {
            fprintf(stderr, "thread create error %d: %s\n", status, strerror(status));
            exit(EXIT_FAILURE);
        }
    }

    /* Find average processing time. */
    float avg = (float)Total_Time / Num_Successful_Requests;

    printf("\n\nTotal Processing Time: %d\n", Total_Time);
    printf("Number of completed requests: %d\n", Num_Successful_Requests);
    printf("Average Processing Time: %.2f\n", avg);

    /* Uninitialize the mutexes. */
    if(pthread_mutex_destroy(&time_mutex) != 0)
    {
        perror("Time Mutex Destroy\n");
        exit(EXIT_FAILURE);
    }
    if(pthread_mutex_destroy(&num_req_mutex) != 0)
    {
        perror("Num Request Mutex Destroy\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}
