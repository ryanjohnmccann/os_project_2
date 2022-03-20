
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

    long tid, track_pos, tmp, will_produce;
    tid = (long) t;

    track_pos = 0;
    will_produce = (m1.b_size * 2);

    printf("P%ld: Producing %ld values\n", tid, m1.b_size * 2);
    while (will_produce > 0) {
        track_pos += 1;
        pthread_mutex_lock(&m1.buffer_lock);
        if (track_pos == m1.b_size) {
            track_pos = 0;
            m1.is_full = 1;
            while (m1.is_full) {
                printf("P%ld: Blocked due to full buffer\n", tid);
                pthread_cond_wait(&m1.full, &m1.buffer_lock);
                printf("P%ld: Done waiting on full buffer\n", tid);
            }
        }
        if (m1.shared_buffer[m1.producer_pos] == 0) {
            tmp = (rand() % 10) + 1;
            printf("P%ld: Writing %ld to position %ld\n", tid, tmp, m1.producer_pos);
            m1.shared_buffer[m1.producer_pos] = tmp;
            m1.is_empty = 0;
            pthread_cond_signal(&m1.empty);
            will_produce -= 1;
        }
        else {
            pthread_mutex_unlock(&m1.buffer_lock);
            continue;
        }
        if (m1.producer_pos == (m1.b_size - 1)) {
            m1.producer_pos = 0;
        }
        else {
            m1.producer_pos += 1;
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
        // Values to be consumed by last consumer thread (Will handle extras)
        if (tid == (m1.n_consumers - 1)) {
            will_consume = m1.nums_produced - ((m1.n_consumers - 1) * m1.divide);
        } else {
            will_consume = m1.divide;
        }
    }

    printf("C%ld: Consuming %ld values\n", tid, will_consume);
    while (will_consume > 0) {
        // Our initial position may not be zero, so we know we've gone through the entire queue when tracked position
        // equals the size of the queue.
        track_pos += 1;
        pthread_mutex_lock(&m1.buffer_lock);
        // Buffer must be empty or all values read
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
        // Reset consumer position to the front of the queue
        if (m1.consumer_pos == (m1.b_size - 1)) {
            m1.consumer_pos = 0;
        }
        else {
            m1.consumer_pos += 1;
        }
        pthread_mutex_unlock(&m1.buffer_lock);
    }
    pthread_exit((void *) t);
}
