#include <stdio.h>
#include "reservation.h"

reservation reservation_create(int agency_id, int reservation_number){
    reservation node;

    node.agency_id = agency_id;
    node.reservation_number = reservation_number;

    return node;
}

void reservation_print(reservation node){

    printf("agency_id: %d, reservation_number: %d\n", node.agency_id, node.reservation_number);

    return;
}