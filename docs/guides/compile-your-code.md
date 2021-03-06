# Compile your code
---

There are many ways to compile code for kilobots. This guide presents
two of them.

!!! note
    The following methods have mostly been tested on **MacOS Big Sur**.

    If you experience any problem, please consider visiting [this quick-start wiki](https://diode.group.shef.ac.uk/kilobots/index.php/Getting_Started:_How_to_work_with_kilobots#Compile_your_own_control_software) or [this guide from the official website](https://www.kilobotics.com/documentation#advanceduse) (go to 'advanced use' in the 'documentation' section).

    Both of those websites where used to write this guide.

---

## Compiling your code - the easy way

The [kilobotics website](https://www.kilobotics.com) provides a simple online editor to write code for kilobots.

Once logged in with your Dropbox account, you can access the editor, create new programs and save them.

To compile your program, just click the green `compile` button. A `.hex` file with the same name as your `.c` file should appear in the KiloEdit directory of your dropbox.

!!! warning
    Kilobotics' online editor has aged and is not really maintained anymore. Depending on your operating system, browser or dropbox account and configurations, the editor may not work properly. If this is your case, please use the alternative method presented below.

---

## Compiling your code - the less-easy way

If the kilobotics' online editor doesn't work or doesn't suit your needs, you can still go for the tough method : compiling the code manually. The goal of this tutorial is to get you a simple command-line utility to easily compile your different programs.

!!! warning
    The following method may not work with Windows operating system.  
    We suggest that Windows users use a virtual machine running Linux.

### Step 1 : install AVR compilator

#### For Linux using `apt`

Run the following command in the terminal :
```
sudo apt-get install avr-libc gcc-avr avrdude
```

#### For MacOS using `homebrew`

Run the following command in the terminal :
```
brew tap osx-cross/avr
brew install avr-gcc
```


### Step 2 : download the template code

To get the task simpler, download [this file](https://diode.group.shef.ac.uk/kilobots/images/8/8a/TemplateForCompilingKilobotCode.zip).

It contains a shell script that will take care of library dependencies and allow us to compile code with just one command.


### Step 3 : place your files inside the template directory

Place your files inside the 'TemplateForCompiling' directory. They should now be in the same directory as `compileCode.sh`.

### Step 4 : compile with one command

Open a terminal window and navigate inside the 'TemplateForCompiling' directory with the `cd` command.

Type the following command :

```
./compileCode.sh your-file
```

!!! note
    Replace 'your-file' by the real name of the file you want to compile.

!!! note
    Note how we didn't put the `.c` extension at the end of the file name.


### We are done !

The `.hex` file corresponding to your program should now appear inside the 'TemplateForCompiling/build/' directory.

If anything went wrong, refer to the error message returned by the `compileCode.sh` script. If your code contains syntaxical errors, they will be listed there.

---
