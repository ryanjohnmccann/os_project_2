
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#include "prod_cons_MT.h"

struct monitor m1;

int main(int argc, char *argv[]) {

    void *status;
    long t;
    int rc;

    int buffer_size = atoi(argv[1]);
    int num_producers = atoi(argv[2]);
    int num_consumers = atoi(argv[3]);

    // Initialize struct values
    m1.b_size = buffer_size;
    m1.n_consumers = num_consumers;
    m1.n_producers = num_producers;

    // Initialize monitor
    init_monitor();

    // Handle producer threads
    pthread_t producer_threads[m1.n_producers];
    pthread_mutex_init(&m1.buffer_lock, NULL);
    pthread_cond_init(&m1.full, NULL);

    // Create threads
    for (t = 0; t < m1.n_producers; t++) {
        printf("Main: started producer %ld\n", t);
        rc = pthread_create(&producer_threads[t], NULL, Producer, (void *) t);
        if (rc) {
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }

    // Handle consumer threads
    pthread_t consumer_threads[m1.n_consumers];
    pthread_mutex_init(&m1.buffer_lock, NULL);
    pthread_cond_init(&m1.empty, NULL);

    // Create threads
    for (t = 0; t < m1.n_consumers; t++) {
        printf("Main: started consumer %ld\n", t);
        rc = pthread_create(&consumer_threads[t], NULL, Consumer, (void *) t);
        if (rc) {
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }

    // Destroy
    pthread_mutex_destroy(&m1.buffer_lock);
    pthread_cond_destroy(&m1.full);
    pthread_cond_destroy(&m1.empty);

    // Join producer threads
    for (t = 0; t < m1.n_producers; t++) {
        rc = pthread_join(producer_threads[t], &status);
        if (rc) {
            printf("ERROR; return code from pthread_join() is %d\n", rc);
            exit(-1);
        }
        printf("Main: producer %ld joined\n", t);
    }
    // Join consumer threads
    for (t = 0; t < m1.n_consumers; t++) {
        rc = pthread_join(consumer_threads[t], &status);
        if (rc) {
            printf("ERROR; return code from pthread_join() is %d\n", rc);
            exit(-1);
        }
        printf("Main: consumer %ld joined\n", t);
    }

    // Free allocated data and exit
    free(m1.shared_buffer);
    printf("Main: Program completed\n");
    pthread_exit(NULL);
}
