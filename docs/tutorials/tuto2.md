# Full Metal Kilobot

[*source code of this tutorial*](../resources/sources/fmk.zip)

---

## Summary

Now that we know the basics, let's tackle the real interesting thing about kilobots : make them communicate.

This tutorial is inspired by the military training scenes in [Kubrick's "Full Metal Jacket" movie](https://en.wikipedia.org/wiki/Full_Metal_Jacket). We will use two kilobots : one will be the instructor, the other will be the rookie.

Therefore, we are going to write two distinct programs :

* the instructor's program, generating and sending random training instructions
* the rookie's program, executing instructions as fast as possible

---

## What you will learn

* How to create a message with the `message_t` structure
* How to generate random numbers with the `rand_hard()` function
* How to send messages with `kilo_message_tx`
* How to receive and read messages with `kilo_message_rx`

---

## Create messages

Messages for kilobots are contained in the `message_t` structure, which has 3 fields :

* `type`, from 0 to 127, describing the type of the message. For this tutorial, we will only use the macro `NORMAL`
* `data`, an array of length 9, storing the real content of the message
* `crc`, an hash code, computed with the `message_crc()` function, used to check that data was not corrupted during transmission.

We can define a message like this :

```c
message_t message;  // This message will contain the answer to the ultimate question of life, universe and everything.
message.type = NORMAL;
message.data[0] = 42;
message.crc = message_crc(&message);
```

---

## Create random numbers

Kilobots have two functions to generate random numbers :

* `rand_soft()` generates a random number from a seed. You can provide a specific seed using the `rand_seed()` function.
* `rand_hard()` generates a random number from the kilobot's hardware, namely its battery precise voltage.

As `rand_hard()` is slower than `rand_soft()`, it is common to use `rand_soft()` when we need random numbers multiple times per second. In that situation, we can use `rand_hard()` inside `rand_seed()` to initialize the RNG.

In this tutorial, we will only use the `rand_hard()` function as we do not need efficiency.

---

## Instructor's program

Let's get into the real code !

### Instructions

The first step is to define a protocol used by the two kilobots to communicate. It seems difficult but it really is fairly simple.

We have 3 different possible instructions :

* "**Show me your war face!**" ⟶ Light the led
* "**Turn on yourself x times!**" ⟶ Set motors to do x turns on itself
* "**Run x cm and come back!**" ⟶ Set motors to run x cm, turn around and come back

A simple protocol would be : "Each instruction has an ID, from 1 to 3, stored in the first byte of `data`. Instructions 2 and 3 have a parameter, stored in the second byte of `data`."

So for example, "Turn on yourself 3 times!" would translate to the following message :

```c
message_t message;
message.type = NORMAL;
message.data[0] = 2;  // Turn on yourself...
message.data[1] = 3;  // ...3 times!
message.crc = message_crc(&message);
```

We can now write a simple program that will generate a random instruction :

```c
message_t instruction;
int instructionID;
int instructionParam;

void makeInstruction() {
    // Creates an instruction to be given to the rookie
    instruction.type = NORMAL;
    instruction.data[0] = instructionID;
    instruction.data[1] = instructionParam;
    instruction.crc = message_crc(&instruction);
}

void generateInstruction() {
    // Gives an instruction to the rookie

    // Choses the instruction
    instructionID = (rand_hard() % 3) + 1;  // Instructions have a specific number, from 1 to 3

    // Depending on the instruction, generates a random parameter
    switch (instructionID) {
        case 1:
            // Rookie must show his war face ! (Turn on his led)
            instructionParam = 0;
            break;

        case 2:
            // Rookie must run x centimeters, then come back.
            instructionParam = 3 + (rand_hard() % 8); // Cm to be runned, from 3 to 10
            break;

        case 3:
            // Rookie must turn around on himself x times.
            instructionParam = 1 + (rand_hard() % 3); // number of turns, from 1 to 3
            break;
    }
    makeInstruction();
}
```

!!! note
    Note how we put `instruction`, `instructionID` and `instructionParam` as global variables. It makes it possible to access them from anywhere in the code, though we must be careful with the way each part of the program manipulates them.

### Message handling

To handle communication, we need to define two functions. One *(commonly named `message_tx()`)* will be called when sending a message, the other *(commonly named `message_rx()`)* will be called when receiving a message.

We also have to define a flag, which is a variable with value 0 or 1 describing the "state" of the robot. In our case, the `instructionGiven` flag tells wether or not an instruction has been given to the rookie. If its value is 0, the instructor should generate and send a new instruction. If it's 1, the instructor waits for the rookie to finish.

```c
int instructionGiven = 0;

message_t* message_tx() {
    // This function returns the instruction to send to the rookie
    return &instruction;
}

void message_rx(message_t *msg, distance_measurement_t *dist) {
    // This function is called when the rookie has finished executing his order and is ready for a new one.
    instructionGiven = 0;
}
```

!!!note
    Notice the two parameters of the message_rx function. They are mandatory to define the function, but of no use for the instructor.


### Setup and loop

The setup function for the instructor is rather minimalistic, it's even empty in fact. The instructor does not care about anything except yelling orders repeatedly.

In the loop function, we will either generate a new instruction or wait for the rookie. When generating a new instruction, the instructor will indicate the instruction chosen by turning on its LED with a specific color. When waiting, the instructor blinks yellow.

```c
void setup() {
}

void loop() {
    if (!instructionGiven) {
        // Gives an order. Blinks a specific color depending on the given order
        generateInstruction();

        switch (instructionID) {
            case 1:
                set_color(RGB(1,0, 0)); // Blinks red
                break;

            case 2:
                set_color(RGB(0, 1, 0)); // Blinks green
                break;

            case 3:
                set_color(RGB(0, 0, 1)); // Blinks green to
                break;
        }
        delay(500);
        set_color(RGB(0,0,0));
        instructionGiven = 1;  // Changes the flag
    }
    else {
        // Waits for the rookie. Blinks yellow
        delay(500);
        set_color(RGB(1,1,0));
        delay(500);
        set_color(RGB(0,0,0));
    }
}
```

!!!note
    Even if the `setup()` function is empty, we must define it, as it must be passed to the `kilo_start()` function.

### Put it together

The only thing left to do is to define the main function. `kilo_message_tx` and `kilo_message_rx` are system variables used to specify which function to call when sending/receiving a message.

```c
int main() {
    // initialize hardware
    kilo_init();

    // Registers the function to call when sending a instruction
    kilo_message_tx = message_tx;

    // Registers the function to call when rookie says he is ready
    kilo_message_rx = message_rx;

    // start program
    kilo_start(setup, loop);

    return 0;
}
```

---

## Rookie's program

Most of the aspects of the rookie's program have already been seen in the instructor's program. In this section, we will only focus on the new things.

### Message handling

#### Sending messages

The rookie must only send a message to say that he is ready, so the message doesn't need to contain any data : the mere fact that he is sending a message already tells the purpose of the message. *And as you can see above, the instructor never cares about the content of the message.*

The rookie will also use a flag to tell wether he is ready for an instruction or not.

These facts give the following code :

```c
#include <stddef.h>  // Defines 'NULL' macro

message_t ready;
int waitingForInstruction = 0;

void setup() {
    // Waits to be called by the instructor
    waitingForInstruction = 1;

    // Initialize the message
    ready.type = NORMAL;
    ready.crc = message_crc(&ready);
}

message_t* message_tx() {
	// Sends a message if the rookie is ready. If not, sends nothing.
    if (waitingForInstruction) {
        return &ready;
    }
    return NULL;
}
```

#### Receiving messages

As for the instructor's program, the rookie will receive message with the `message_rx()` function. Here, the `msg` parameter is useful to retrieve the content of the message :

```c
int instructionID = 0;
int instructionParam = 0;

void message_rx(message_t *msg, distance_measurement_t *dist) {
    // This function is called when the rookie receives an instruction
    instructionID = msg->data[0];
    instructionParam = msg->data[1];
    waitingForInstruction = 0;
}
```

### The loop

In the loop function, the rookie will either execute an instruction, or wait to receive one. When waiting, it blinks yellow.

```c
int straightSpeedCalibrator = 1000; // Delay in milliseconds to go straight on 1cm
int turningSpeedCalibrator = 5000; // Delay in milliseconds to make 1 turn around

void loop() {
    if (waitingForInstruction) {
        // Waits for orders. Blinks yellow
        delay(500);
        set_color(RGB(1,1,0));
        delay(500);
        set_color(RGB(0,0,0));
    }
    else {
        switch(instructionID) {
            case 1:
                // Rookie shows his war face
                set_color(RGB(0, 1, 1));
                delay(500);
                set_color(RGB(0,0,0));
                break;
            case 2:
                // Rookie runs x centimeters, then comes back
                for (int i = 0 ; i < 2 ; i++) {
                    spinup_motors();
              	    set_motors(kilo_straight_left, kilo_straight_right);
                    delay(instructionParam * straightSpeedCalibrator);
                    set_motors(kilo_turn_left, 0);
                    delay(turningSpeedCalibrator);
                    set_motors(0, 0);
                }
                break;
            case 3:
                // Rookie turns around on himself x times
                for (int i = 0 ; i < instructionParam ; i++) {
                    spinup_motors();
                    set_motors(kilo_turn_left, 0);
                    delay(turningSpeedCalibrator);
                    set_motors(0, 0);
                }
                break;
        }
        waitingForInstruction = 1;
        delay(2000);  // Waits two seconds before executing the next instruction
    }
}
```

!!!note
    Notice the use of `straightSpeedCalibrator` and `turningSpeedCalibrator`. These variables are used to indicate the time the kilobot takes to do a specific movement. Set them to the most fitting values depending on your kilobot specificities.

### Put it together

The `main()` function is nothing new at this point:

```c
int main() {
    // Initializes hardware
    kilo_init();

    // Registers the function to call when receiving a instruction
    kilo_message_rx = message_rx;

    // Registers the function to call when sending a message
    kilo_message_tx = message_tx;

    // Starts program
    kilo_start(setup, loop);

    return 0;
}
```

---

## We are done !

You can now admire two kilobots in a tough military training. You've mastered the art of communication and are ready to get into the [next tutorial](tuto3.md)!

---
