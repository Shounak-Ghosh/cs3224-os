# Simple Kernel Module Instructions

This README provides a quick guide on how to build and manage a simple kernel module. For detailed information, please refer to [LWN.net's Kernel Development](https://lwn.net/Kernel/LDD3/).

## Prerequisites

Install required packages with the following: 

```bash
sudo apt install linux-generic
sudo apt-get install gcc-12
sudo apt install make
```

## Building the Module

To compile the module run the following commands:

```bash
make
```

If you need to clean up the build artifacts, use:

```bash
make clean
```

## Inserting the Module

To insert the compiled module into the kernel, run:

```bash
sudo insmod simple_module.ko
```

After insertion, you can check the kernel log to see any messages from the module:

```bash
sudo dmesg | tail
```

## Removing the Module

When you need to remove the module from the kernel, use:

```bash
sudo rmmod simple_module
```

You can verify its removal by checking the kernel log again:

```bash
sudo dmesg | tail
```