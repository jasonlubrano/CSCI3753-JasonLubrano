Author: Jason Lubrano
Class: CSCI3753
Assignment: Programming Assignment #3
Collab: Kyle Ward
Doc: README.md

Files included in the repo:
    multi-lookup.c #my multi-lookup source file
    multi-lookup.h #my multi-lookup header file
    util.c #dnslookup file from professor knox
    util.h #dnslookup file from professor knox
    namesx.txt #input files from professor knox
    performance.py #performance file from professor knox
    multi-lookup.o #my multilookup object
    util.o #object for the util
    pa3.pdf #what the lab was abbout
    Makefile #make file

To run the program:
In the terminal type these commands:
    To make:
        $ make all
    To run:
        $ ./multi-lookup < # requester threads > < # resolver threads > < input files > < output file >
        - do it without < >, commas not necessary between input files
        - ex:
        $ 2 2 names1.txt names2.txt results.txt
    make clean
        $ make clean