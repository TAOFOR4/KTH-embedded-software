#include <stdio.h>
#include "altera_avalon_performance_counter.h"
#include "sys/alt_irq.h"
#include "sys/alt_alarm.h"
#include "system.h"
#include "io.h"
#include "images.h"
#include "ascii_gray.h"

#define DEBUG 0
#define PERFORMANCE 1


/* Definition of Task Periods (ms) */

#define SECTION_1 1
#define SECTION_2 2
#define SECTION_3 3
#define SECTION_4 4
#define SECTION_5 5

/*
 * Global variablesl
 */
static unsigned char imagematrix[31][31];		//Matrix of the img
static unsigned int matrixmix[27][27];			//Matrix after xcorr2
unsigned char current_image=0;					//Keeps track of what image are we working on
unsigned char objectmax[2];		//Variables used to send the position of the max value in the matrix
unsigned char finalresult[2];  //Stores the coordinate for every picture
int counter=0;   		//Used in performance mode
int max_calc_pos;	 	//Used in PosMax to calculate the 



void graySDF(unsigned char* orig,unsigned char* after)
{

	int sizeX=orig[0];
	int sizeY=orig[1];
	int fullsize=sizeX*sizeY;
	int i,j;
	//unsigned char grayscale_img[fullsize+3];
	unsigned char* imgP;
	unsigned char* share;
	after[0]=sizeX;
	after[1]=sizeY;
	after[2]=orig[2];
	for(i=0;i<fullsize;i++)
	{
		after[i + 3]= (unsigned char)( 0.3125 * orig[3 * i + 3 ] + 0.5625 * orig[3 * i + 4] + 0.125  * orig[3 * i + 5]);
	}
	imgP = after;
	share = (unsigned char*) SHARED_ONCHIP_BASE;
	for (i=0;i<fullsize+3;i++)
	{
		*share++ = *imgP++;
	}

}

void asciiSDF(unsigned char* orig ,unsigned char* ASCII)
{
int sizeX = orig[0], sizeY = orig[1];
int i,j;
int fullsize=sizeX*sizeY;
//unsigned char ASCII[sizeX * sizeY+3];
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
		//printf("%c ",ASCII[j+(i*sizeX)+3]);
		}
	printf("\n");
	}
		imgP = ASCII;
	share = (unsigned char*) SHARED_ONCHIP_BASE;
	for (i=0;i<fullsize+3;i++)
	{
		*share++ = *imgP++;
	}


}
void resize(unsigned char* img) {
   int maxX=img[0];// width of the picture
   int maxY=img[1];//Height of the picture
    unsigned char* output_image = (unsigned char*) malloc((maxX/2) * (maxY/2) * sizeof(unsigned char));
	int i, j, k, l;
	for (i = 0; i < maxY; i += 2) {
	for (j = 0; j < maxX; j += 2) {
	int sum = 0;
		for (k = i; k < i + 2; k++) {
		for (l = j; l < j + 2; l++) {
		sum += img[k * maxX + l];}
}
		output_image[(i / 2) * (maxX / 2) + (j / 2)] = sum / 4;
		
		
}
}
}
int main(void) {
/*
* PERFORMANCE MODE
*/
	 
			printf("PERFORMANCE MODE!! \n\n\n");
			PERF_RESET(PERFORMANCE_COUNTER_0_BASE);
			PERF_START_MEASURING (PERFORMANCE_COUNTER_0_BASE);
			PERF_BEGIN(PERFORMANCE_COUNTER_0_BASE, SECTION_1);
			char i = *image_sequence[current_image];
		    char j = *(image_sequence[current_image]+1);
			unsigned char* graydata = malloc((int)i * (int)j *sizeof(unsigned char));
			graySDF(image_sequence[current_image],graydata[current_image]);
			unsigned char* assiced = malloc((int)i * (int)j *sizeof(unsigned char));
			asciiSDF(graydata[current_image],assiced[current_image]);
			resize(assiced[current_image]);
			current_image++;
			current_image%=sequence_length;  
			PERF_END(PERFORMANCE_COUNTER_0_BASE, SECTION_1);  
			perf_print_formatted_report(PERFORMANCE_COUNTER_0_BASE,      
      ALT_CPU_FREQ,        // defined in "system.h"
			1,                   // How many sections to print
			"Total time"   // Display-name of section(s).
			);           
	  
	  return 0;
}
