#include <stdio.h>
#include "sys/alt_irq.h"
#include "sys/alt_alarm.h"
#include "system.h"
#include "io.h"
#include "sys/alt_stdio.h"
#include "altera_avalon_performance_counter.h"
#include "image.h"

#define IMAGE_WIDTH 32
#define IMAGE_HEIGHT 32

#define FLAG_OFFSET 7000

#define SECTION_1 1

unsigned char* imgorigin;
unsigned char current_image=0;

/*Flag Signal on shared memory*/
unsigned int* flag_signal = (unsigned int*)(SHARED_ONCHIP_BASE + FLAG_OFFSET);


extern void delay (int millisec);

void sram2sm_p3(unsigned char* base)
{
	int x, y;
	unsigned char* shared;

	shared = (unsigned char*) SHARED_ONCHIP_BASE;
	int size_x = *base++;
	int size_y = *base++;
	int max_col= *base++;
	*shared++  = size_x;
	*shared++  = size_y;
	*shared++  = max_col;
	printf("The image is: %d x %d!! \n", size_x, size_y);
	for(y = 0; y < size_y; y++)
	for(x = 0; x < size_x; x++)
	{
		*shared++ = *base++; 	// R
		*shared++ = *base++;	// G
		*shared++ = *base++;	// B
	}
}

void sram2smwithgray(unsigned char* base)
{
	int x, y;
	unsigned char* shared;

	shared = (unsigned char*) SHARED_ONCHIP_BASE;
	int size_x = *base++;
	int size_y = *base++;
	int max_col= *base++;
	*shared++  = size_x;
	*shared++  = size_y;
	*shared++  = max_col;
	printf("The image is: %d x %d!! \n", size_x, size_y);
    printf("The MAX COL is: %d ! \n", max_col);
	for(y = 0; y < size_y; y++)
	for(x = 0; x < size_x; x++)
	{
		*shared++ = *base++; 	
	}
}

void graySDF(unsigned char* orig,unsigned char* after)
{
	//PERF_BEGIN(PERFORMANCE_COUNTER_0_BASE, graySDF1);
	int i,j;
  //float* after;
	//unsigned char grayscale_img[fullsize+3];
	unsigned char* imgP;
	unsigned char* share;
	after[0]=orig[0];
	after[1]=orig[1];
	after[2]=orig[2];
  int fullsize=after[0]*after[1];
  //float after;
	for(i=0;i<fullsize;i++)
	{
		after[i + 3]=  ( (float) orig[3 * i + 3]*0.3125 +   (float) orig[3 * i + 4]*0.5625 + (float) orig[3 * i + 5]*0.125);
	}
	//ERF_END(PERFORMANCE_COUNTER_0_BASE, graySDF1);
}

int main(void){

        printf("Hello from cpu_0!\n");        
        unsigned char* graydata; 
        graydata = malloc (IMAGE_WIDTH * IMAGE_HEIGHT * sizeof(unsigned char));        

        sram2sm_p3(sequence1[current_image]);
        PERF_RESET(PERFORMANCE_COUNTER_0_BASE);
		PERF_START_MEASURING (PERFORMANCE_COUNTER_0_BASE);
		PERF_BEGIN(PERFORMANCE_COUNTER_0_BASE, SECTION_1);
        /* Allocate shared memory */
    	imgorigin = (unsigned char*) SHARED_ONCHIP_BASE;
        printf("%s\n", imgorigin);
        
        *flag_signal = 0; 
        printf("Read flag=%d\n", *flag_signal);
        
    	/* Main loop */
        while(1){

            if(*flag_signal == 0){

           graySDF(imgorigin, graydata);
            sram2sm_gray(graydata);
            printf("%s\n", imgorigin);
            *flag_signal = 1;  
            printf("Write flag=%d\n", *flag_signal);
            current_image = (current_image+1)%sequence1_length;
                      
           }

        }
		PERF_END(PERFORMANCE_COUNTER_0_BASE, SECTION_1);  

		/* Print report */
		perf_print_formatted_report
		(PERFORMANCE_COUNTER_0_BASE,            
		ALT_CPU_FREQ,        // defined in "system.h"
		1,                   // How many sections to print
		"Section 1"        // Display-name of section(s).
		);   

  return 0;
}