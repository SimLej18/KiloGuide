# Coding for kilobots
---

Programs for kilobots are coded in C language, then compiled as hex instructions for the kilobot to read.

!!! Note
    If you are not familiar with the C language, your priority should be to learn its basics. Here is a [great tutorial](https://www.programiz.com/c-programming) to get you started.

---

## Default template

The default template for the kilobot code looks like this :

```c
#include <kilolib.h>

void setup() {
    // Put your setup code here, to be run only once
}

void loop() {
    // Put your main code here, to be run repeatedly
}

int main() {
    // Initializes hardware
    kilo_init();
    // Starts program
    kilo_start(setup, loop);

    return 0;
}
```

This code shows the two main parts of a kilobot program : the `setup` and `loop` functions.

The `setup()` function is executed only once, at the very start of the execution. It's a great place to put the code that initialize some variables or constants.

The `loop()` function is run repeatedly during the whole execution. It's where the common behavior of the kilobot is implemented.

---

## Basic functions

Here are some of the most useful functions of the [kilolib library](https://www.kilobotics.com/docs/index.html) :

Function | Effects
------------ | -----------------
`kilo_init()` | Initializes kilobot's hardware
`kilo_start(setup, loop)` | Starts the execution of the program
`RGB(r, g, b)` | Converts RGB code to a parameter used by `set_color()`
`set_color(color)` | Sets the kilobot's LED color
`set_motors(left, right)` | Sets each motor speed, enabling movement
`delay(millis)` | Pauses the execution for a given amount of milliseconds

---
