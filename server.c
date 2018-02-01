#include <pthread.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

void *worker_thread(void *arg);

int main()
{
    pthread_t thread1;
    void *result1;
    int status;

	if ((status = pthread_create(&thread1, NULL, worker_thread, NULL)) != 0) {
	    fprintf(stderr, "thread create error %d: %s\n", status, strerror(status));
	    exit(EXIT_FAILURE);
    }

    //join with the threads(wait for them to terminate
    // get their return vals
    if ((status = pthread_join(thread1, &result1)) != 0) {
	    fprintf(stderr, "join error %d:%s\n", status, strerror(status));
	    exit(1);
    }

    return 0;
}

void *worker_thread(void *arg)
{
    /* get filename passed in by main thread */
    char * filename = (char *) arg;

    /* detach thread so join is not needed */
    pthread_detach(pthread_self());

    if ((rand() % 10) < 8) {
        /* 80% chance of happening */
        sleep(1);
    }else {
        /* 20% chance of happening */
        /* sleep 7 - 10 seconds */
        sleep(rand() % 4 + 7);
    }

    printf("Accessed file %s", filename);
    
    /* free memory allocated for filename */
    free(filename);

    pthread_exit(NULL);
}