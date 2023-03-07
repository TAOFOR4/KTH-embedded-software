#include <stdio.h>
#include "sys/alt_irq.h"
#include "sys/alt_alarm.h"
#include "system.h"
#include "io.h"
#include "sys/alt_stdio.h"

#define IMAGE_WIDTH 32
#define IMAGE_HEIGHT 32

#define FLAG_OFFSET 6000

/*pointer to shared memory*/
unsigned char* imgorigin;

/*Flag Signal on shared memory*/
unsigned int* flag_siganl = (unsigned int*)(SHARED_ONCHIP_BASE + FLAG_OFFSET);  

extern void delay (int millisec);

void Resize(unsigned char* img){
  int maxX = img[0];
  int maxY = img[1];
  int max_col = img[2];
  
  
  unsigned char* shared;
  shared = (unsigned char*) SHARED_ONCHIP_BASE;

  unsigned char* img_resized;

   int i, j, k, l, x, y;
    for (i = 0; i < maxY; i += 2) {
      for (j = 0; j < maxX; j += 2) {
        int sum = 0;
        for (k = i; k < i + 2; k++) {
          for (l = j; l < j + 2; l++) {
            sum += img[k * maxX + l + 3];
          }
        }
      img_resized[(i / 2) * (maxX / 2) + (j / 2)] = sum / 4;
      }
    }

   *shared++  = maxX/2;
   *shared++  = maxY/2;
   *shared++  = max_col;
   for(y = 0; y < maxY/2; y++)
   for(x = 0; x < maxX/2; x++)
	{
		*shared++ = *img_resized++; 	
	}
}

int main(void)
{
  printf("Hello from cpu_1!\n");
        
        unsigned char* img_resized;

        /* Allocate shared memory */
        
    	imgorigin = (unsigned char*) SHARED_ONCHIP_BASE;   
   
        /* Main loop */
        while(1){

             if(*flag_siganl == 1){
        
                printf("Read flag in loop=%d\n", *flag_siganl);

                Resize(imgorigin);

                *flag_siganl = 2;

                printf("Write flag=%d\n", *flag_siganl);
             
             }
    
        }
  return 0;
}
 
