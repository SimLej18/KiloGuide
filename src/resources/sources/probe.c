#include <kilolib.h>

#define TIMEOUT 50
#define MAX_DIST 100
#define MAX_NEIGHBOUR_NB 5

struct kilobot {
    int uid;
    int timer;
};

struct kilobot neighbours[MAX_NEIGHBOUR_NB];
int nb_neighbours = 0;

message_t message;

int idOfNeighbour(int uid) {
    /*
    Looks for kilobot with 'uid' as uid in neighbours.
    If the kilobot exists, return its index in neighbours.
    Else, return -1.
    */
    for (int i = 0 ; i < nb_neighbours ; i++) {
        if (neighbours[i].uid == uid) {
            return i;
        }
    }
    return -1;
}

void setup() {
    // Set message content
    message.type = NORMAL;
    message.data[0] = kilo_uid;
    message.crc = message_crc(&message);

}

void loop() {
    // Increment timer of each kilobot
    for (int i = 0 ; i < nb_neighbours ; i++) {
        neighbours[i].timer++;
    }

    // Check for kilobot timeout
    for (int i = 0 ; i < nb_neighbours ; i++) {
        if (neighbours[i].timer >= TIMEOUT) {
            // Timeout. Remove this kilobot
            for (int j = i ; j < nb_neighbours-1 ; j++) {
                neighbours[j] = neighbours[j+1];
            }
            nb_neighbours--;
        }
    }

    // Set light
    switch (nb_neighbours) {
        case 0:
            set_color(RGB(0, 3, 0));
            delay(50);
            break;
        case 1:
            set_color(RGB(1, 3, 0));
            delay(50);
            break;
        case 2:
            set_color(RGB(1, 2, 0));
            delay(50);
            break;
        case 3:
            set_color(RGB(2, 1, 0));
            delay(50);
            break;
        case 4:
            set_color(RGB(3, 1, 0));
            delay(50);
            break;
        case MAX_NEIGHBOUR_NB:
            set_color(RGB(3, 0, 0));
            delay(50);
            break;
    }

    // Set movement
    if (kilo_ticks % 32 == 0) {  // Roughly every second
        switch (rand_hard() % 3) {
            case 0:
                // Goes straight
                spinup_motors();
                set_motors(kilo_straight_left, kilo_straight_right);
                break;
            case 1:
                // Turn right
                spinup_motors();
                set_motors(0, kilo_turn_right);
                break;
            case 2:
                // Turn left
                spinup_motors();
                set_motors(kilo_turn_left, 0);
                break;
        }
    }
}

message_t* message_tx() {
    return &message;
}

void message_rx(message_t *m, distance_measurement_t *d) {
    if (estimate_distance(d) < MAX_DIST) {  // The kilobot is close enough
        int id = idOfNeighbour(m->data[0]);
        if (id == -1) {  // New neighbour, not yet registered
            if (nb_neighbours < MAX_NEIGHBOUR_NB) {  // Can register it
                neighbours[nb_neighbours].uid = m->data[0];
                neighbours[nb_neighbours].timer = 0;
                nb_neighbours++;
            }
        }
        else {  // Neighbour already registered
            neighbours[id].timer = 0;
        }
    }
}

int main() {
    // initialize hardware
    kilo_init();

    kilo_message_tx = message_tx;
    kilo_message_rx = message_rx;

    // start program
    kilo_start(setup, loop);

    return 0;
}
