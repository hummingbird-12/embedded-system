# Assignment 2 - Timer Device Driver

## Device specs

* Device driver name: `/dev/dev_driver`
* Device driver major number: `242`

## Instructions

1. Use the `Makefile` to build (and push) both app and module.
    ```bash
    $ make      # to compile

    $ make push # to compile and do `adb push`
    ```
    * Compiled app: `app/app`
    * Compiled module: `module/dev_driver.ko`
    
2. In `minicom`, do the following:
    ```bash
    echo "7 6 1 7" > /proc/sys/kernel/printk
    cd /data/local/tmp
    insmod dev_driver.ko
    mknod /dev/dev_driver c 242 0 # if there is no device file
    ./app TIMER_INTERVAL[1-100] TIMER_CNT[1-100] TIMER_INIT[0001-8000]
    ...
    rmmod dev_driver
    ```

## Developer

* ID: 20161577
* Name: Inho Kim
