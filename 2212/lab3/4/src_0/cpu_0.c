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



//The grayscale function transforms the values of the starting array (RGB) into gray values
//By applying the formula stated into the Haskell model, switched to bitwise
//operation to improve the speed of the program

void Grayscale(unsigned char* img){
	int maxX=img[0];  			// width of the picture
	int maxY=img[1];			//Height of the picture
	int total= maxY*maxX*3;    //Lenght of array
	int moving_index=3;  
	int basic_index=3;
	//The following lines of code overwrites on the starting array, since we won't
	//need the starting values anymore after grayscaling
	while(moving_index<total){
		img[basic_index]=(unsigned char)(((img[moving_index])>>2)+((img[moving_index])>>4)+((img[moving_index+1])>>1)+((img[moving_index+1])>>4)+((img[moving_index+2])>>3));
		basic_index++;  
		moving_index+=3;
		
	}
}

void taskAsciiSDF(unsigned char* img)
{
	int maxX=img[0];  			// width of the picture
	int maxY=img[1];			//Height of the picture
  int maxX_0=img[0]*3;
  printAscii(img, img[0], img[1]);
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
			Grayscale(image_sequence[current_image]);
			taskAsciiSDF(image_sequence[current_image]);
			resize(image_sequence[current_image]);
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
