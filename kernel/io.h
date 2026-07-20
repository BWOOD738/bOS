#pragma once 

void outb(unsigned short int port, unsigned char value); /* Maybe move to a file like io.h?? */
unsigned char inb(unsigned short int port);

void ioWait();