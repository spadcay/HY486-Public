#include "queue.h"
#include "stack.h"

typedef struct flight {
    stack *completed_reservations;
    queue *pending_reservations;
}flight;

void print_flight(flight *);
