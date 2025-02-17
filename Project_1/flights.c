#include <stdlib.h>
#include <stdio.h>
#include "flights.h"

void print_flight(flight *f){
    if(f == NULL) {
        printf("Empty flight.\n");
        return;
    }

    stack_print(f->completed_reservations);
    queue_print(f->pending_reservations);

    return;
}