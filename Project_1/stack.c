#include <stdio.h> 
#include <stdlib.h> 

#include "stack.h"
#include "color.h"

void init_stack(stack **s, int capacity){
    (*s) = (stack *)malloc(sizeof(stack));
    
    (*s)->top = NULL;
    pthread_mutex_init(&(*s)->top_lock, NULL);
    (*s)->size = 0;
    (*s)->capacity = capacity;

    return;
}

int stack_push(stack *s, int agency_id, int reservation_number) {
    
    pthread_mutex_lock(&s->top_lock);

    if (s->size >= s->capacity) {
        pthread_mutex_unlock(&s->top_lock);
        //printf("STACK IS FULL %d\n", agency_id);
    
        return -1;
    }
    stack_node *new = (stack_node *)malloc(sizeof(stack_node));
    if (new == NULL) {
        pthread_mutex_unlock(&s->top_lock);
        return 0;
    }

    new->reservation = reservation_create(agency_id, reservation_number);
    new->next = s->top;
    s->top = new;
    s->size++;

    // printf("Pushed: ");
    // reservation_print(new->reservation);
    
    pthread_mutex_unlock(&s->top_lock);

    return 1;
}

reservation stack_pop(stack *s) {
    pthread_mutex_lock(&s->top_lock);

    if (s->size <= 0) {
        // Stack is empty
        pthread_mutex_unlock(&s->top_lock);
        reservation empty = {0}; 
        return empty;
    }

    stack_node *top_node = s->top;
    reservation res = top_node->reservation;
    s->top = top_node->next;
    free(top_node);
    s->size--;

    printf("Poped: ");
    reservation_print(res);
    
    pthread_mutex_unlock(&s->top_lock);
    
    return res;
}

void stack_print(stack *s) {
    pthread_mutex_lock(&s->top_lock);
    green();
    printf("Stack contents:\n");
    reset();
    stack_node *curr = s->top;
    while (curr != NULL) {
        reservation_print(curr->reservation);
        curr = curr->next;
    }

    pthread_mutex_unlock(&s->top_lock);
    
    return;
}