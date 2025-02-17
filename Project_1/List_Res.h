#ifndef list_h
#define list_h

#include "reservation.h"
#include <pthread.h>

typedef struct list_reservation {
    reservation reservation;
    int marked;
    pthread_mutex_t lock; 
    struct list_reservation *next;
}list_reservation;

typedef struct list {
    list_reservation *head;
    list_reservation *tail;
}list;

void init_list(list **);
void insert(list *, reservation );
reservation delete(list *);
void print_list(list *);


#endif