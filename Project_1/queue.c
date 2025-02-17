#include <stdio.h> 
#include <stdlib.h> 
#include "queue.h"

#include "color.h"

void init_queue(queue** q) {
    (*q) = (queue*)malloc(sizeof(queue));
    if ((*q) == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return;
    }
    (*q)->head = NULL;
    (*q)->tail = NULL;
    pthread_mutex_init(&((*q)->head_lock), NULL);
    pthread_mutex_init(&((*q)->tail_lock), NULL);

    return;
}

void enq(queue *q, int agency_id, int reservation_number) {
    queue_node* new = (queue_node*)malloc(sizeof(queue_node));
    
    if (new == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return;
    }

    new->reservation = reservation_create(agency_id, reservation_number);
    new->next = NULL;

    pthread_mutex_lock(&(q->tail_lock));
    //not empty queue
    if (q->tail != NULL) {
        q->tail->next = new;
        q->tail = new;
    } 
    // empty queue
    else {
        pthread_mutex_lock(&(q->head_lock));
        q->head = new;
        q->tail = new;
        pthread_mutex_unlock(&(q->head_lock));
    }

    pthread_mutex_unlock(&(q->tail_lock));

    return;
}

reservation deq(queue *q){
    pthread_mutex_lock(&(q->head_lock));
    
    if (q->head == NULL) {
        pthread_mutex_unlock(&(q->head_lock));
        reservation empty = {0}; 
        return empty;
    }

    queue_node* temp = q->head;
    reservation res = temp->reservation;
    q->head = temp->next;
    
    if (q->head == NULL) {
        pthread_mutex_lock(&(q->tail_lock));
        q->tail = NULL;
        pthread_mutex_unlock(&(q->tail_lock));
    }
    
    free(temp);

    pthread_mutex_unlock(&(q->head_lock));
    
    // red();
    // printf("Dequeued: " ANSI_COLOR_RESET);
    // reset();
    //reservation_print(res);
    
    return res;
}


void queue_print(queue* q) {
    pthread_mutex_lock(&(q->head_lock));
    queue_node* current = q->head;
    red();
    printf("Queue contents: \n");
    reset();
    while (current != NULL) {
        reservation_print(current->reservation);
        current = current->next;
    }
    printf("\n");
    pthread_mutex_unlock(&(q->head_lock));
}