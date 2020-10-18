# Final Project - Hangman Game

## Device specs

* Device driver name: `/dev/hangman`
* Device driver major number: `242`

## Instructions

1. Use the root `Makefile` to build (and push) both app and module.
    ```bash
    $ make      # to compile

    $ make push # to compile and do `adb push`
    ```
    * Compiled app: `app/app`
    * Compiled module: `module/hangman.ko`

2. In `minicom`, do the following:
    ```bash
    echo "7 6 1 7" > /proc/sys/kernel/printk
    cd /data/local/tmp
    insmod hangman.ko

    # if there is no device file
    mknod /dev/hangman c 242 0

    ./app
    
    rmmod hangman
    ```

## Developer

* ID: 20161577
* Name: Inho Kim

## About the course

CSE4116 - Embedded System Software, Spring 2020,

Sogang University
