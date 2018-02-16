Jason Lubrano
jason.lubrano@colorado.edu
jalu7098@colorado.edu


This folder contains the files in their respective location:
/home/kernel/linux-hwe-4.13.0/arch/x86/kernel/simple_add.c
	- program that makes the system call to add numbers
/home/kernel/linux-hwe-4.13.0/arch/x86/kernel/Makefile
	- makefile
/home/kernel/linux-hwe-4.13.0/include/linux/syscalls.h
	- header for the system calls
/home/kernel/linux-hwe-4.13.0/arch/x86/entry/syscalls/syscall_64.tbl
	- Table for the system calls that has hello world and add
/home/kernel/linux-hwe-4.13.0/var/log/syslog
	- log of everything that was done.
/home/user/Dropbox/test_add_number.c
	- test to add numbers

To run the program:
	add the simple_add to the specified location in the kernel
	modify the systemcall_64 table to add the new system call
	add the new system call header to the header file
	edit the makefile program to link the two together
	in terminal:
		gcc /home/user/Dropbox/test_add_number.c -o test_add_number
	 	./test_add_number
	then you can use dmesg to check if it did right.