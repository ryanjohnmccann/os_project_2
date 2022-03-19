
#ifndef PROJECT_2_C_PROD_CONS_MT_H
#define PROJECT_2_C_PROD_CONS_MT_H

#include <pthread.h>

struct monitor {
    int b_size, n_consumers, n_producers;
    // TODO: Ask about this later
    int shared_buffer[100000];
    int producer_pos;
    int nums_produced, divide;

    pthread_mutex_t buffer_lock;
    pthread_cond_t full;
    pthread_cond_t empty;
};

void init_monitor();

void *Producer();

void *Consumer();

#endif //PROJECT_2_C_PROD_CONS_MT_H
