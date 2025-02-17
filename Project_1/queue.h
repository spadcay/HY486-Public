#ifndef queue_h
#define queue_h

#include <pthread.h> 
#include "reservation.h"

typedef struct queue_node {
    reservation reservation;
    struct queue_node *next;
}queue_node;

typedef struct queue {
    queue_node *head;
    queue_node *tail;
    pthread_mutex_t head_lock;
    pthread_mutex_t tail_lock;
}queue;


void init_queue(queue **);
void enq(queue *, int , int);
reservation deq(queue *);
void queue_print(queue *);

#endif