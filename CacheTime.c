#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "cacheutils.h"

uint8_t array[10*4096];

int main(int argc, const char **argv) {
  uint64_t time1, time2;
  uint64_t threshold;
  int i;

  threshold = detect_flush_reload_threshold();

  printf("LLC miss threshold: %ld\n", threshold);
  
  // Initialize the array
  for(i=0; i<10; i++) array[i*4096]=1;

  // FLUSH the array from the CPU cache
  for(i=0; i<10; i++) flush(&array[i*4096]);

  // Access some of the array items
  array[3*4096] = 100;
  array[7*4096] = 200;

  for(i=0; i<10; i++) {
    time1 = rdtsc();                
    maccess(&array[i*4096]);
    time2 = rdtsc() - time1;       
    printf("Access time for array[%d*4096]: %d CPU cycles\n",i, (int)time2);
  }
  return 0;
}
