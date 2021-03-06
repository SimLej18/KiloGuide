#include <kilolib.h>

void setup() {
    // Blinks red, yellow and green before startup
    set_color(RGB(1,0,0));  // red
    delay(1000);
    set_color(RGB(1,1,0));  // yellow
    delay(1000);
    set_color(RGB(0,1,0));  // green
}

void loop() {
    // Goes straight
    spinup_motors();
    set_motors(kilo_straight_left, kilo_straight_right);
    delay(2000);

    // Turns left
    spinup_motors();
    set_motors(kilo_turn_left, kilo_straight_right/2);
    delay(5000);

    // Goes straight again
    spinup_motors();
    set_motors(kilo_straight_left, kilo_straight_right);
    delay(4000);

    // Turns right
    spinup_motors();
    set_motors(kilo_straight_left/2, kilo_turn_right);
    delay(5000);

    // Goes straight to get back to the race starting point
    spinup_motors();
    set_motors(kilo_straight_left, kilo_straight_right);
    delay(2000);
}

int main() {
    // initialize hardware
    kilo_init();
    // start program
    kilo_start(setup, loop);

    return 0;
}
