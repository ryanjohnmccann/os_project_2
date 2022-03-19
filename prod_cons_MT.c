
#include "prod_cons_MT.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

extern struct monitor m1;

void init_monitor() {
    m1.shared_buffer = malloc(sizeof(long) * m1.b_size);

    m1.producer_pos = 0;
    m1.nums_produced = m1.n_producers * (m1.b_size * 2);

    m1.divide = m1.nums_produced / m1.n_consumers;
}

// TODO: P0 is the only one writing to my shared buffer
void *Producer(void *t) {

    int i;
    long tid;
    tid = (long) t;
    printf("P%ld: Producing %ld values\n", tid, m1.b_size * 2);

    for (i = 0; i < (m1.b_size * 2); i++) {
        int rand_int = (rand() % 10) + 1;
        pthread_mutex_lock(&m1.buffer_lock);
        // Entry is empty
        if (m1.shared_buffer[m1.producer_pos] == 0) {
            printf("P%ld: Writing %d to position %ld\n", tid, rand_int, m1.producer_pos);
            m1.shared_buffer[m1.producer_pos] = rand_int;
            if (m1.producer_pos == (m1.b_size - 1)) {
                m1.producer_pos = 0;
            } else {
                m1.producer_pos += 1;
            }
            pthread_mutex_unlock(&m1.buffer_lock);
            pthread_cond_signal(&m1.empty);
        }
            // Buffer is full
        else {
            printf("P%ld: Blocked due to full buffer\n", tid);
            pthread_cond_wait(&m1.full, &m1.buffer_lock);
            printf("P%ld: Done waiting on full buffer\b", tid);
        }
    }

    printf("P%ld: Exiting\n", tid);
    pthread_exit((void *) t);
}

void *Consumer(void *t) {
    long tid, i, tmp, will_consume;
    tid = (long) t;

    // TODO: Here
    if (m1.divide == 0) {
        printf("DIVIDE IS ZERO FIX THIS!");
        exit(1);
    } else {
        if (tid == (m1.n_consumers - 1)) {
            will_consume = m1.nums_produced - ((m1.n_consumers - 1) * m1.divide);
        } else {
            will_consume = m1.divide;
        }
    }

    printf("C%ld: Consuming %ld values\n", tid, will_consume);
    while (will_consume > 0) {
        for (i = 0; i < m1.b_size; i++) {
            pthread_mutex_lock(&m1.buffer_lock);
            // Position is empty
            if (m1.shared_buffer[i] == 0) {
                continue;
            } else {
                tmp = m1.shared_buffer[i];
                printf("C%ld Reading %ld from position %ld\n", tid, tmp, i);
                will_consume -= 1;
                m1.shared_buffer[i] = 0;
            }
            pthread_mutex_unlock(&m1.buffer_lock);
        }
        pthread_cond_signal(&m1.full);
        if (will_consume > 0) {
            printf("C%ld: Blocked due to empty buffer\n", tid);
            pthread_cond_wait(&m1.empty, &m1.buffer_lock);
            printf("C%ld: Done waiting on empty buffer\n", tid);
        }
    }
}
