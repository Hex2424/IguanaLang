#include "stdio.h"
#include "stdint.h"

void printBits(unsigned int num)
{
   for(int bit=0;bit<(sizeof(unsigned int) * 8); bit++)
   {
      printf("%i", num & 0x01);
      num = num >> 1;
   }
   printf("\n");
}


int main()
{

   //      float ss = 3.3;
   //      uint32_t* h = (void*)&ss;
   //      uint32_t l = *h;
    
   //  printf("Leading text %f %f\n\n", ss, l);
   //  printBits(l);

   
    // for(float i = 1.1;;i++)
    // {
    //     unsigned int ii = i;
    //     int iii = i;
    //     printf("Leading text %d %f %u\n",iii, i, ii);
    // }

}

