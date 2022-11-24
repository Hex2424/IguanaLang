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
      int lolz:5;
      int lols:1;
   }ss;

   ss s;
   while (1)
   {
      ss s;
   }
   

   printf("%d", s.lols);
}

// gcc -Wstack-usage=12 -o program main.c
