#ifndef stack_h
#define stack_h

#include <pthread.h>
#include "reservation.h"

typedef struct stack_node {
    reservation reservation;
    struct stack_node *next;
} stack_node;

typedef struct stack {
    stack_node *top;
    pthread_mutex_t top_lock;
    int size;   // the current size of the stack
    int capacity;   // the max size of the stack
}stack;

void init_stack(stack **, int);
int stack_push(stack *, int agency_id, int reservation_number);
reservation stack_pop(stack *);
void stack_print(stack *);

#endif