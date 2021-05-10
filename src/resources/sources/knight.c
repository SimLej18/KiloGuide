#include <kilolib.h>

#define MIN_DISTANCE 35
#define MAX_DISTANCE 70

#define CHARGING 0
#define FREAKING_OUT 1
#define WON 2
#define LOST 3

int distanceLimit;
int state;

message_t message;

void setup() {
    // Set the distance limit.
    distanceLimit = (rand_hard() % (MAX_DISTANCE - MIN_DISTANCE)) + MIN_DISTANCE;

    // Set the state
    state = CHARGING;

    // Set the message
    message.type = NORMAL;
    message.data[0] = state;
    message.crc = message_crc(&message);

    // Set light and movement
    set_color(RGB(3, 0, 0));
    spinup_motors();
    set_motors(kilo_straight_left, kilo_straight_right);
}

void loop() {
    if (state == CHARGING) {
        // The kilobot is charging : blink red
        set_color(RGB(3, 0, 0));
        delay(100);
        set_color(RGB(0, 0, 0));
        delay(100);
    }
    else if (state == FREAKING_OUT) {
        // The kilobot is freaking out : fastly blink white for 2 seconds
        for (int i = 0 ; i < 2000 ; i += 200) {
            set_color(RGB(3, 3, 3));
            delay(100);
            set_color(RGB(0, 0, 0));
            delay(100);
        }
        // The kilobot is calming down, he lost
        set_motors(0, 0);
        state = LOST;
    }
    else if (state == WON) {
        // The kilobot won the fight : blink blue, green and yellow
        set_color(RGB(0, 0, 3));
        delay(100);
        set_color(RGB(0, 3, 0));
        delay(100);
        set_color(RGB(3, 3, 0));
        delay(100);
    }
    else {
        // The kilobot lost the fight : slowly, shamefully blinks white
        set_color(RGB(1, 1, 1));
        delay(2000);
        set_color(RGB(0, 0, 0));
        delay(2000);
    }
}

message_t* message_tx() {
    return &message;
}

void message_rx(message_t *m, distance_measurement_t *d) {
    if (state == CHARGING && m->data[0] == CHARGING) {
        // The kilobot and his opponent are still charging each other
        if (estimate_distance(d) < distanceLimit) {
            // The kilobot freaks out and surrender
            state = FREAKING_OUT;

            // Change message
            message.data[0] = state;
            message.crc = message_crc(&message);

            // Change movement
            spinup_motors();
            set_motors(0, kilo_turn_right);
        }
    }
    else if (m->data[0]) {
        // The oponnent is freaking out
        state = WON;

        // Stop charging
        spinup_motors();
        set_motors(0, 0);
    }
}

int main()
{
    kilo_init();

    kilo_message_tx = message_tx;
    kilo_message_rx = message_rx;

    kilo_start(setup, loop);

    return 0;
}
