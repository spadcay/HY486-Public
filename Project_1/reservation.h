#ifndef res_h
#define res_h

typedef struct reservation {
    int agency_id;
    int reservation_number;
} reservation;

reservation reservation_create(int agency_id, int reservation_number);
void reservation_print(reservation);

#endif 