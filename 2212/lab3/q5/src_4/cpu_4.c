#include "sys/alt_stdio.h"

#define TRUE 1

extern void delay (int millisec);

int main()
{
  printf("Hello from cpu_4!\n");

  while (TRUE) { /* ... */ }
  return 0;
}
