#include <kilolib.h>
#define DEBUG
#include <debug.h>

void setup() {
    printf("Hello world!");
}

void loop() { return; }

int main() {
    kilo_init();
    debug_init();
    kilo_start(setup, loop);
    return 0;
}
