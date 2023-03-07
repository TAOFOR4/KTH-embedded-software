#include <stdio.h>
#include "sys/alt_irq.h"
#include "sys/alt_alarm.h"
#include "system.h"
#include "altera_avalon_performance_counter.h"
#include "io.h"
#include "sys/alt_stdio.h"

#define IMAGE_WIDTH 32
#define IMAGE_HEIGHT 32
#define FLAG_OFFSET 6000

/*pointer to shared memory*/
unsigned char* imgorigin;

/* Global variables */
unsigned char* imgresized;

/*Flag Signal on shared memory*/
unsigned int* flagsignal = (unsigned int*)(SHARED_ONCHIP_BASE + FLAG_OFFSET);



extern void delay (int millisec);

void asciiSDF(unsigned char* orig)
{
int sizeX = orig[0];
int sizeY = orig[1];
int i,j;
int fullsize=sizeX*sizeY;
unsigned char ASCII[sizeX * sizeY+3];
char asciiLevels[16] = {' ','.',':','-','=','+','/','t','z','U','w','*','0','#','%','@'};
unsigned char* imgP;
unsigned char* share;
ASCII[0] = sizeX ;
ASCII[1] = sizeY ;
ASCII[2] = orig[2] ;
for( i = 0; i < sizeX * sizeY; i++)
{
ASCII[i+3] = asciiLevels[((orig[i+3])/16)];
}
for( i = 0; i < sizeY; i++)
	{
	for( j=0; j< sizeX;j++)
		{
		printf("%c ",ASCII[j+(i*sizeX)+3]);
		}
	printf("\n");
	}
		
}

int main(void)
{
  printf("Hello from cpu_2!\n");
        
        /* Allocate shared memory */
        imgorigin = (unsigned char*) SHARED_ONCHIP_BASE;
        printf("Read flag=%d\n", *flagsignal);

        while(1){

          if(*flagsignal == 2){
             


             asciiSDF(imgorigin);
             
        
             *flagsignal = 0;
              
             printf("flag information=%d\n", *flagsignal);
          }
        }
  return 0;
}
 
