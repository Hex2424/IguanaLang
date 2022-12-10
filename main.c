#include "stdio.h"
#include "stdint.h"

struct ss;

void printBits(struct ss num)
{
   // for(int bit=0;bit<(sizeof(unsigned int) * 8); bit++)
   // {
   //    printf("%i", num & 0x01);
   //    num = num >> 1;
   // }
   printf("\n");
}


int main()
{  
   struct ss s = {4, 5};
   printBits(s);
   printf("\n");;
}

// gcc -Wstack-usage=12 -o program main.c
