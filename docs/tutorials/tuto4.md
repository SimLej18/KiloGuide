# Morphogenetics

[*source code of this tutorial*](../resources/sources/morpho.zip)

---

## Summary

In most animals' bodies, some cells are able to feel their distance from other cells using chemicals propagation. The main use of this ability is to form complex shapes and tissues. The field of *[morphogenetic robotics](https://en.wikipedia.org/wiki/Morphogenetic_robotics)* takes inspiration from this to develop self-organized communities of robots.

In this tutorial, we are going to develop a *very simplified* morphogenesis algorithm. It works with one 'source' kilobot and multiple 'cell' kilobots. Each 'cell' kilobot will be able to deduct its distance from the 'source' kilobot by communicating with its neighbors. The kilobots won't move, but they will use their LED to indicate their distance level.

---

## What you will learn

* How to implement a basic collective behavior

---

## Source's program

The source kilobot is fairly simple. He will only send a message telling its level : 0. To recognize it from the other kilobots, we will set its LED to white.

### The `setup()` function

```c
message_t message;

int level = 0;  // 0 as this cell is the source

void setup() {
    // Set message content
    message.type = NORMAL;
    message.data[0] = level;
    message.crc = message_crc(&message);

    // Set light
    set_color(RGB(3, 3, 3));
}
```

### Other functions

As said above, the source kilobot really has nothing to do, so the `loop()` is just empty :

```c
void loop() {
    // Nothing to do
}
```

The `message_tx()` function is also quite common :

```c
message_t* message_tx() {
    return &message;
}
```

And once again, the `main()` function is nothing new :

```c
int main() {
    // initialize hardware
    kilo_init();

    kilo_message_tx = message_tx;

    // start program
    kilo_start(setup, loop);

    return 0;
}
```

---

## Cell's program

Here's the tough part of the tutorial. Just as the source, the cells will communicate their current level with their neighbors. However, if a cell receive a message from a cell with a level 2 or more times lower than its own, it must update its level.

### Macros, message and `setup()`

For a better configurability, we will define the `MAX_LEVEL` and `TIMEOUT` macros. `MAX_LEVEL` is the maximum distance level of all cells. `TIMEOUT` is used to know when to increase the cell's level : if a cell has no news from another cell with a lower level for a long time, it must deduct that it is now further away from the source and update its level accordingly. The `timer` variable will measure the time since last contact with a lower-level cell.

```c
#define MAX_LEVEL 5
#define TIMEOUT 30

message_t message;

int level = MAX_LEVEL;  // The cell starts at MAX_LEVEL
int timer = 0;

void setup() {
    message.type = NORMAL;
}
```

### Sending and receiving messages

In `message_rx()`, the cell analyses messages from its neighbors. If one of its neighbors' level is 2 or more times lower than its own level, it updates. If the level is just under its own, it resets its timer.

```c
void message_rx(message_t *m, distance_measurement_t *d) {
    if (m->data[0] < level-1) {
        // Change level
        level = m->data[0] + 1;
        timer = 0;
    }
    if (m->data[0] == level-1) {
        // Reset timer
        timer = 0;
    }
}
```

In `message_tx()`, we update the message content with the current level just before sending the message :

```c
message_t* message_tx() {
    message.data[0] = level;
    message.crc = message_crc(&message);
    return &message;
}
```

### The `loop()` function


Every time we enter the `loop()`, we increase the timer. If it is above `TIMEOUT`, we decrement the cell's level.

The cell will also display different shades from green to red depending on its level.

```c
void loop() {
    // Update timer
    timer++;
    if (timer >= TIMEOUT && level < MAX_LEVEL) {
        // Timeout
        level++;
        timer = 0;
    }

    // Set light
    switch (level) {
        case 1:
            set_color(RGB(0, 3, 0));
            delay(50);
            break;
        case 2:
            set_color(RGB(1, 3, 0));
            delay(50);
            break;
        case 3:
            set_color(RGB(3, 3, 0));
            delay(50);
            break;
        case 4:
            set_color(RGB(3, 1, 0));
            delay(50);
            break;
        case MAX_LEVEL:
            set_color(RGB(3, 0, 0));
            delay(50);
            break;
    }
}
```

!!! Note
    As the `TIMEOUT` is set to 30 and the `loop()` takes 50 milliseconds to execute, a timeout take roughly 1,5 seconds to occur.

### Put it all together

Don't forget the `main()` function!

```c
int main() {
    // initialize hardware
    kilo_init();

    kilo_message_rx = message_rx;
    kilo_message_tx = message_tx;

    // start program
    kilo_start(setup, loop);

    return 0;
}
```

---

## We are done !

You can now play with the kilobots, moving them around. Try and see what happens when you change the location of the source kilobot. You can also play around with `MAX_LEVEL` and `TIMEOUT`, seeing the effects of each change. If you want to go further, why not make the source/cells move randomly? What happens when you have multiple 'source' kilobots? The possibilities are endless.

---
