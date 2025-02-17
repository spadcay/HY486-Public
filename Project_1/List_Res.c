#include <stdio.h> 
#include <stdlib.h> 
#include "List_Res.h"
#include "color.h"
#define TRUE 1
#define FALSE 0

static list_reservation *createRes() {
    list_reservation *new = (list_reservation *)malloc(sizeof(list_reservation));
    if (new == NULL) {
        printf("Failed allocate memory!\n");
        exit(1);
    }
    //new->reservation = res;
    new->next = NULL;
    new->marked = 0;
    pthread_mutex_init(&new->lock, NULL);

    return new;
}

void init_list(list **l) {
    (*l) = (list *)malloc(sizeof(list));

    if ((*l) == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return;
    }

    list_reservation *new = createRes();
    (*l)->head = new;
    (*l)->tail = new;

    return;
}


static int validate(list_reservation *pred, list_reservation *curr) {
    if (pred->marked == 0 && curr->marked == 0 && pred->next == curr)
        return 1;
    else
        return 0;
}


void insert(list *l, reservation res) {
    
    while(TRUE) {
        list_reservation *pred = l->head;
        list_reservation *curr = pred->next;

        pthread_mutex_lock(&l->head->lock);
        if(l->head->next == NULL) {
            list_reservation *new = createRes();
            new->reservation = res;
            l->head->next = new;
            l->tail = new;
            pthread_mutex_unlock(&l->head->lock);
            
            return;
        }
        // else if(curr->reservation.reservation_number > res.reservation_number){
        //     list_reservation *new = createRes();
        //     new->reservation = res;
        //     new->next = curr;
        //     l->head->next = new;
        //     pthread_mutex_unlock(&l->head->lock);
            
        //     return;
        // }
        
        pthread_mutex_unlock(&l->head->lock);

        while((curr != l->tail) && (curr->reservation.reservation_number < res.reservation_number)) {
            pred = curr;
            curr = curr->next;
        }

        pthread_mutex_lock(&pred->lock);
        pthread_mutex_lock(&curr->lock);

        if(validate(pred, curr) == TRUE) {
            if(res.reservation_number > curr->reservation.reservation_number){
                list_reservation *new = createRes();
                new->next = NULL;
                new->reservation = res;
                curr->next = new;
                l->tail = new;
            }
            else {
                list_reservation *new = createRes();
                new->next = curr;
                new->reservation = res;
                pred->next = new;
            }
            pthread_mutex_unlock(&pred->lock);
            pthread_mutex_unlock(&curr->lock);

            return;
        }

        pthread_mutex_unlock(&pred->lock);
        pthread_mutex_unlock(&curr->lock);
        
    }

}

reservation delete(list *l){
    
}

void print_list(list *l){
    list_reservation *curr = l->head;

    while(curr->next != NULL){
        reservation_print(curr->next->reservation);
        curr = curr->next;
    }
}