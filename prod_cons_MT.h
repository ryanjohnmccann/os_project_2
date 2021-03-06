
#ifndef PROJECT_2_C_PROD_CONS_MT_H
#define PROJECT_2_C_PROD_CONS_MT_H

#include <pthread.h>

struct monitor {
    long b_size, n_consumers, n_producers, producer_pos, consumer_pos;
    long *shared_buffer;
    long nums_produced, divide;
    long is_full, is_empty;

    pthread_mutex_t buffer_lock;
    pthread_cond_t full;
    pthread_cond_t empty;
};

void init_monitor();

void *Producer();

void *Consumer();

#endif //PROJECT_2_C_PROD_CONS_MT_H
