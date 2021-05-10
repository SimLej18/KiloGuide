# Rush Hour

[*source code of this tutorial*](../resources/sources/probe.c)

---

## Summary

Here we are! This is the last tutorial of this guide. It's now time to wrap up everything we have learnt in one program.

In this tutorial, a great number of kilobots will move around randomly. They will communicate with their neighbors, trying to count how many they are. If the kilobot has none or few neighbors, it will turn its LED *green*. If it has a lot of neighbors on the other hand, it will turn *red*. The goal is to produce a heat-map of some kind, where red regions correlate with a high kilobot density and green region with a low kilobot density.

---

## What you will learn

* How to use `kilo_ticks`
* How to use `kilo_uid`
* Summarize everything we have learnt through this guide

---

## Probe's program

Each kilobot will run the same program : `probe.c`. The goal of the program is to register up to 5 neighbors and change LED accordingly.

### Macros

Our program will define the following macros :

```c
#define TIMEOUT 50
#define MAX_DIST 100
#define MAX_NEIGHBOUR_NB 5
```

The goal of each macro will be clarified later in this tutorial.

### `kilo_uid` and structures

During calibration, you can give each kilobot a *unique ID*. This ID is stored in the kilobot memory and can be accessed via the `kilo_uid` variable. In this tutorial, we will use it to differentiate every neighbor.

!!! Note
    Be sure to give each kilobot a **unique** ID at calibration. If two kilobots have the same ID, the final collective behavior will be altered.

Each kilobot must also be able to forget a neighbor once it hasn't had contact with it for some time. To do so, we are going to define a structure :

```c
struct kilobot {
    int uid;
    int timer;
};
```

This [C structure](https://www.tutorialspoint.com/cprogramming/c_structures.htm) stores the neighbor unique ID and the elapsed time since the last contact.

To store the information of all neighbors, we can create an **array of structures** :

```c
struct kilobot neighbours[MAX_NEIGHBOUR_NB];
int nb_neighbours = 0;
```

You can easily deduct the role of `MAX_NEIGHBOUR_NB` : it tells how many neighbors can be stored at the same time.

The `nb_neighbours` variable will be used in two ways at the same time. The first way is to know how many neighbors are registered. The second is to know at which index to write in the `neighbours` array when registering a new neighbor. (If there are 3 neighbors, the following neighbor must be registered at index 3.)

### See if a neighbor is already registered

When it receives a message, the kilobot must be able to know if it comes from a known neighbor or a new one. If it comes from a known neighbor, it must know at which index to find this neighbor in the `neighbours` array. This is what this simple algorithm does :

```c
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
```

### Message and `setup()`

The only data we need to send to our neighbors is our unique ID (`kilo_uid`). We can therefore define the message in `setup()`, knowing it won't change during the execution.

As usual, `message_tx()` does nothing extraordinary.

```c
message_t message;

void setup() {
    // Set message content
    message.type = NORMAL;
    message.data[0] = kilo_uid;
    message.crc = message_crc(&message);

}

message_t* message_tx() {
    return &message;
}
```

### The `message_rx()` function

This function is a bit more complicated.

First we must see if the kilobot sending the message is close enough to consider it a neighbor. This is the goal of the `MAX_DIST` macro : it tells the maximum distance for a kilobot to be considered a neighbor.

Then we can use the function we defined previously to know if the kilobot sending the message is already registered in `neighbours`. If it is not, we register it (assuming we have an available index in `neighbours`). If it is, we reset its timer.

```c
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
```

### The `loop()` function

The `loop()` function has quite a few things to do.

First, it **increments all of our neighbors' timer**.

```c
// Increment timer of each kilobot
for (int i = 0 ; i < nb_neighbours ; i++) {
    neighbours[i].timer++;
}
```

Second, it **checks for neighbors in timeout**. This is the goal of the `TIMEOUT` macro : it tells the limit for a neighbor's timer to be considered too big. If a kilobot is in timeout, we use a simple algorithm to shift all following neighbors to the left in `neighbours` and decrement `nb_neighbours`. This way, `nb_neighbours` still represents the number of neighbors and the index where we can register a new neighbor at the same time.

```c
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
```

Here is where we **set the LED** to a shade between green and red depending on `nb_neighbours`:

```c
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
```

We still have to **make the kilobots move randomly**. We want each kilobot to chose a movement between "straight", "turn left" and "turn right" and execute it for one second, every second. We can't really use `delay()` as it would slow down the `loop()` considerably, therefore slowing the timers of all neighbors. One solution is to use `kilo_ticks`.

The `kilo_ticks` variable is used to know the time elapsed since the execution started. It is incremented roughly 32 times per second. By using the `%` operator, we can then execute a piece of code every second :

```c
// Set movement
if (kilo_ticks % 32 == 0) {  // Roughly every second
    switch (rand_hard() % 3) {  // Choses randomly
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
```

### Put it all together !

You probably know the `main()` function by heart at this point.

```c
int main() {
    // initialize hardware
    kilo_init();

    kilo_message_tx = message_tx;
    kilo_message_rx = message_rx;

    // start program
    kilo_start(setup, loop);

    return 0;
}
```

---

## We are done !

This was by far the most complete and complex program of this guide. Congratulations on keeping up to this point.

There are still a few minor functionalities of kilobots we have not covered, such as their ability to sense ambient light, measure their board temperature, indicate their voltage or the `kilo_message_tx_success` callback. You can learn about those in the [kilolib documentation](https://www.kilobotics.com/docs/kilolib_8h.html).  

Anyway, we hope this guide was of use. May you have a lot of fun coding for kilobot!

---
