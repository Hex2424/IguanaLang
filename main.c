#include "stdio.h"
#include "stdint.h"

void printBits(unsigned int num)
{
   // for(int bit=0;bit<(sizeof(unsigned int) * 8); bit++)
   // {
   //    printf("%i", num & 0x01);
   //    num = num >> 1;
   // }
   printf("\n");
}

typedef struct
{
   int lol:5;
   int lol2:2;
}ss;

int main()
{  
   typedef struct
   {
      char lolz:5;
      char lols:1;
   }ss;

   uint32_t value = -500848;
   uint32_t valuei;
   int value2;

   ss l;
   valuei = value % (2 << (5 - 1));
   l.lolz = value;
   printf("%d %d\n", valuei, l.lolz);
}

// gcc -Wstack-usage=12 -o program main.c
