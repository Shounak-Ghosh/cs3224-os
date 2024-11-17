For reference: [LWN.net's Kernel Development](https://static.lwn.net/images/pdf/LDD3/ch03.pdf), [Miscellaneous Character Drivers](https://www.linuxjournal.com/article/2920)

To use this module:

Compile the kernel module using the Makefile: `make` to build and `make clean` to clear previous build files.

Load the module using `sudo insmod lab8_a.ko`.

Check if the device file was created: `ls /dev/lab8`.

Compile and run the test program using `gcc test2.c -o test2` and `sudo ./test2 <BUFFER_SIZE>`.

Unload the module using `sudo rmmod lab8_a`.