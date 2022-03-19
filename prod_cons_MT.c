
#include "prod_cons_MT.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

// TODO: Signals

extern struct monitor m1;

void init_monitor() {
    int i;
    for (i = 0; i < m1.b_size; i++) {
        m1.shared_buffer[i] = -1;
    }
    m1.producer_pos = 0;
    m1.nums_produced = m1.n_producers * (m1.b_size * 2);
    m1.divide = m1.nums_produced / m1.n_consumers;
}

void *Producer(void *t) {

    int i;
    long tid;
    tid = (long) t;
    printf("P%ld: Producing %d values\n", tid, m1.b_size * 2);

    // TODO: This may not work, if not, search entire queue until original position has been reached
    //  (curr position is "zero")
    for (i = 0; i < (m1.b_size * 2); i++) {
        int rand_int = (rand() % 10) + 1;
        pthread_mutex_lock(&m1.buffer_lock);
        // Entry is empty
        if (m1.shared_buffer[m1.producer_pos] == -1) {
            printf("P%ld: Writing %d to position %d\n", tid, rand_int, m1.producer_pos);
            m1.shared_buffer[m1.producer_pos] = rand_int;
            if (m1.producer_pos == (m1.b_size - 1)) {
                m1.producer_pos = 0;
            } else {
                m1.producer_pos += 1;
            }
        }
            // Buffer is full
        else {
            printf("P%ld: Blocked due to full buffer\n", tid);
            pthread_cond_wait(&m1.full, &m1.buffer_lock);
        }
        pthread_mutex_unlock(&m1.buffer_lock);
    }

    printf("P%ld: Exiting\n", tid);
    pthread_exit((void *) t);
}

void *Consumer(void *t) {
    long tid, start_ind, end_ind, i;

    tid = (long) t;

    // TODO: All threads will get one and the last will be skipped
    if (m1.divide == 0) {
        printf("DIVIDE IS ZERO FIX THIS!");
        exit(1);
    } else {
        start_ind = (tid * m1.divide);
    }

    // Our extra values thread
    if (tid == (m1.n_consumers - 1)) {
        end_ind = m1.nums_produced;
    } else {
        end_ind = start_ind + m1.divide;
    }

    printf("C%ld: Consuming %ld values\n", tid, (end_ind - start_ind));
    for (i = start_ind; i < end_ind; i++) {

    }
}
