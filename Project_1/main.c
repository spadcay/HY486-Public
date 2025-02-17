#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "flights.h"
#include "stack.h"
#include "queue.h"
#include "color.h"
#include "List_Res.h"

#define DEBUG(state) state

unsigned long long int A;
int number_of_inserter_airlines = 0;
pthread_barrier_t barrier, barrier_start_1st_phase_checks, barrier_start_2st_phase_checks;
flight *flights;
list *centerOfRes;

#define MAX_CAPACITY(id) ((A*A*3)/2 - id*A)  //for stack 
#define nRes(i, id) ((i*A*A) + id) //reservation number
#define MAX_Reservations (A*A*A)
#define MAX_THREADS (A*A + A + 1)
#define NUM_AGENCIES (A*A)
#define TOTAL_RES_NUM ((A*A*A*A*A*A + A*A*A)/2)

typedef struct thread_arg{
    int thread_id;
} thread_arg;

void *flight_thread(void *arg){ 
    thread_arg tmp;
    tmp = *(thread_arg *)arg;
    int thread_id = tmp.thread_id;
    
    if( thread_id > NUM_AGENCIES && thread_id < MAX_THREADS) {
        int index = A - (thread_id - NUM_AGENCIES);
        if(DEBUG(0)) printf("%d capacity: %d\n",thread_id - NUM_AGENCIES - 1 , MAX_CAPACITY(index));
        init_stack(&flights[thread_id - NUM_AGENCIES -1].completed_reservations, MAX_CAPACITY(index));
        init_queue(&flights[thread_id -NUM_AGENCIES -1].pending_reservations);
    }
    
    pthread_barrier_wait(&barrier_start_1st_phase_checks);

    if( thread_id > 0 && thread_id <= NUM_AGENCIES){
        for(int i = 0; i < A; i++) {
            if(stack_push(flights[(thread_id - 1) % A].completed_reservations, thread_id, nRes(i, thread_id)) == -1){
                enq(flights[(thread_id - 1) % A].pending_reservations, thread_id, nRes(i, thread_id));
            }  
        }
    }

    // here is the point where all threads wait until others reach here too
    pthread_barrier_wait(&barrier_start_1st_phase_checks);

    
    if( thread_id > 0 && thread_id <= NUM_AGENCIES){
        pthread_exit(NULL);
    }

    if(thread_id == 0){

        if(DEBUG(1)) {
            for(int i = 0; i < A; i++){
                blue();
                printf("    --- Flight ID: %d ---   \n", (i +1));
                reset();
                print_flight(&flights[i]);
            }
        }

        yellow();
        printf("Controller starts the ckecks for Phase 1....\n\n");
        reset();
        int total_size = 0;
        unsigned long long int total_ResNum = 0;

        for(int i = 0; i < A; i++){
            int size = flights[i].completed_reservations->size;
            int capacity = flights[i].completed_reservations->capacity;
            blue();
            printf("    --- Flight ID: %d ---   \n", (i +1));
            if(size <= capacity){
                green();
                printf("Stack overflow check passed (capacity: %d, found: %d)\n", capacity, size); 
                reset();
            }
            else{
                red();
                printf("Stack overflow check failed (capacity: %d, found: %d)\n", capacity, size);
                reset();
                exit(1);
            }   

            total_size += size;
            if(flights[i].pending_reservations->head != NULL){
                number_of_inserter_airlines++;
                queue_node *curr = (queue_node *)malloc(sizeof(queue_node));
                curr = flights[i].pending_reservations->head;
                total_ResNum += curr->reservation.reservation_number;
                total_size++;
                while(curr->next != NULL) {
                    total_ResNum += curr->next->reservation.reservation_number;
                    curr = curr->next;
                    total_size++;
                }
            }

            stack_node *temp = (stack_node *)malloc(sizeof(stack_node));
            temp = flights[i].completed_reservations->top;
            total_ResNum += temp->reservation.reservation_number;
            while(temp->next != NULL){
                total_ResNum += temp->next->reservation.reservation_number;
                temp = temp->next;
            }
        }
        if(total_size == MAX_Reservations){
            green();
            printf("\nTotal size check passed (expected: %d, found: %d)\n", MAX_Reservations, total_size);
            reset();
        }
        else {
            red();
            printf("\nTotal size check failed (expected: %d, found: %d)\n", MAX_Reservations, total_size);
            reset();
            exit(1);
        }

        if(total_ResNum == TOTAL_RES_NUM){
            green();
            printf("\nTotal keysum check passed (expected: %llu, found: %llu)\n", TOTAL_RES_NUM, total_ResNum);
            reset();
        }
        else{
            red();
            printf("\nTotal keysum check failed (expected: %llu, found: %llu)\n", TOTAL_RES_NUM, total_ResNum);
            reset();
            exit(1);
        }

    }

    pthread_barrier_wait(&barrier);
    if(thread_id > NUM_AGENCIES){
        // airlines who dont have empty queueus
        if(thread_id <= NUM_AGENCIES + number_of_inserter_airlines){
            
            while(flights[thread_id - NUM_AGENCIES -1].pending_reservations->head != NULL){
                reservation tmp = deq(flights[thread_id - NUM_AGENCIES -1].pending_reservations);

                if(DEBUG(0)){
                    reservation_print(tmp);
                } 
                else {
                    insert(centerOfRes, tmp);
                }  
            }
        }
        // airlines who have empty queues
        else {
            
        }
        
    }

    pthread_barrier_wait(&barrier_start_2st_phase_checks);

    if(thread_id != 0) pthread_exit(NULL);
    yellow();
    printf("\nController starts the ckecks for Phase 2....\n\n");
    reset();

    
    if(DEBUG(1)) print_list(centerOfRes);

    pthread_exit(NULL);
}

int main(int argc, char **argv){
    if (argc != 2) {
        printf("Wrong Format.The correct format is \"a.out <integer>\".\n");
        return 1;
    }

    A = atoi(argv[1]);
    
    pthread_t threads[MAX_THREADS];
    flights = (flight *)malloc(sizeof(flight) * A);

    init_list(&centerOfRes);

    pthread_barrier_init(&barrier, NULL, MAX_THREADS - NUM_AGENCIES);
    pthread_barrier_init(&barrier_start_2st_phase_checks, NULL, A + 1);
    pthread_barrier_init(&barrier_start_1st_phase_checks, NULL, MAX_THREADS);
    
    for(int i = 0; i < MAX_THREADS; i++){
        thread_arg *arg = (thread_arg *)malloc(sizeof(thread_arg));
        arg->thread_id = i;
        if (pthread_create(&threads[i], NULL, flight_thread, arg) != 0) {
            fprintf(stderr, "Error creating thread\n");
            return 1;
        }
    }

    // Join all threads
    for (int i = 0; i < MAX_THREADS; ++i) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}