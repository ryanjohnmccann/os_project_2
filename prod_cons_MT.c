
#include "prod_cons_MT.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

extern struct monitor m1;

void init_monitor() {
    // Dynamically allocate memory for shared_buffer based on user input
    m1.shared_buffer = malloc(sizeof(long) * m1.b_size);

    // Defines the current read/write positions in the queue
    m1.producer_pos = 0;
    m1.consumer_pos = 0;
    // Total amount of values to be produced
    m1.nums_produced = m1.n_producers * (m1.b_size * 2);

    // The divided number to calculate even value distribution between consumer threads (extra handled later)
    m1.divide = m1.nums_produced / m1.n_consumers;

    // Is empty set to true, is full set to false
    m1.is_empty = 1;
    m1.is_full = 0;
}

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
            // Reset to the front of the queue
            if (m1.producer_pos == (m1.b_size - 1)) {
                m1.producer_pos = 0;
            } else {
                m1.producer_pos += 1;
            }
            m1.is_empty = 0;
        }
        // Buffer is full
        else {
            m1.is_full = 1;
            pthread_cond_signal(&m1.empty);
            printf("P%ld: Blocked due to full buffer\n", tid);
            while (m1.is_full) {
                pthread_cond_wait(&m1.full, &m1.buffer_lock);
            }
            printf("P%ld: Done waiting on full buffer\b", tid);
        }
        pthread_mutex_unlock(&m1.buffer_lock);
    }

    printf("P%ld: Exiting\n", tid);
    pthread_exit((void *) t);
}

void *Consumer(void *t) {
    long tid, track_pos, tmp, will_consume;
    track_pos = 0;
    tid = (long) t;

    // TODO: Fix this
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
        track_pos += 1;
        pthread_mutex_lock(&m1.buffer_lock);
        // Buffer must be empty
        if (track_pos == m1.b_size) {
            track_pos = 0;
            m1.is_empty = 1;
            while (m1.is_empty) {
                printf("C%ld: Blocked due to empty buffer\n", tid);
                pthread_cond_wait(&m1.empty, &m1.buffer_lock);
                printf("C%ld: Done waiting on empty buffer\n", tid);
            }
        }
        if (m1.shared_buffer[m1.consumer_pos] != 0) {
            tmp = m1.shared_buffer[m1.consumer_pos];
            printf("C%ld Reading %ld from position %ld\n", tid, tmp, m1.consumer_pos);
            m1.shared_buffer[m1.consumer_pos] = 0;
            m1.is_full = 0;
            pthread_cond_signal(&m1.full);
            will_consume -= 1;
        }
        // Space was empty
        else {
            pthread_mutex_unlock(&m1.buffer_lock);
            continue;
        }
        if (m1.consumer_pos == (m1.b_size - 1)) {
            m1.consumer_pos = 0;
        }
        else {
            m1.consumer_pos += 1;
        }
        pthread_mutex_unlock(&m1.buffer_lock);
    }
}
