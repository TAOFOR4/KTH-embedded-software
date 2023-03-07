// Skeleton for lab 2
//
// Task 1 writes periodically RGB-images to the shared memory
//
// No guarantees provided - if bugs are detected, report them in the Issue tracker of the github repository!

#include <stdio.h>
#include "altera_avalon_performance_counter.h"
#include "includes.h"
#include "altera_avalon_pio_regs.h"
#include "sys/alt_irq.h"
#include "sys/alt_alarm.h"
#include "system.h"
#include "io.h"
#include "images.h"

#define DEBUG 1

#define HW_TIMER_PERIOD 100 /* 100ms */

/* Definition of Task Stacks */
#define   TASK_STACKSIZE       2048
OS_STK    task1_stk[TASK_STACKSIZE];
OS_STK    task2_stk[TASK_STACKSIZE];
OS_STK    StartTask_Stack[TASK_STACKSIZE];
OS_EVENT *CommQ;

/* Definition of Task Priorities */

#define STARTTASK_PRIO      1
#define TASK1_PRIORITY      10
#define TASK2_PRIORITY      11

/* Definition of Task Periods (ms) */
#define TASK1_PERIOD 50000
#define TASK2_PERIOD 50000

#define graySDF1 1
#define asciiSDF2 2


OS_EVENT *Task1TmrSem;
OS_EVENT *Task2TmrSem;

// Message Queue
OS_EVENT *CommQ;
OS_TMR *Task1Tmr;
OS_TMR *Task2Tmr;


void Task1TmrCallback (void *ptmr, void *callback_arg){
  OSSemPost(Task1TmrSem);
}

void Task2TmrCallback (void *ptmr, void *callback_arg){
  OSSemPost(Task2TmrSem);
}


void graySDF(unsigned char* orig,unsigned char* after)
{
	PERF_BEGIN(PERFORMANCE_COUNTER_0_BASE, graySDF1);
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
	imgP = after;
	share = (unsigned char*) SHARED_ONCHIP_BASE;
	for (i=0;i<fullsize+3;i++)
	{
		*share++ = *imgP++;
	}
	PERF_END(PERFORMANCE_COUNTER_0_BASE, graySDF1);
}

void asciiSDF(unsigned char* orig)
{
PERF_BEGIN(PERFORMANCE_COUNTER_0_BASE, asciiSDF2);

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
		imgP = ASCII;
	share = (unsigned char*) SHARED_ONCHIP_BASE;
	for (i=0;i<fullsize+3;i++)
	{
		*share++ = *imgP++;
	}

	PERF_END(PERFORMANCE_COUNTER_0_BASE, asciiSDF2);
}
int delay; // Delay of HW-timer

/*
 * ISR for HW Timer
 */
alt_u32 alarm_handler(void* context)
{
  OSTmrSignal(); /* Signals a 'tick' to the SW timers */

  return delay;
}

// Semaphores
OS_EVENT *Task1Sem;
OS_EVENT *Task2Sem;

// SW-Timer
OS_TMR *Task1Tmr;
OS_TMR *Task2Tmr;

/* Timer Callback Functions */

void task1(void* pdata)
{
	INT8U err;
	//INT8U value=0;
	INT8U current_image=0;
  	unsigned char* graydata;
	while (1)
	{
		char i = *image_sequence[current_image];
		char j = *(image_sequence[current_image]+1);
		 graydata = malloc((int)i * (int)j *sizeof(unsigned char));
		PERF_RESET(PERFORMANCE_COUNTER_0_BASE);
		PERF_START_MEASURING (PERFORMANCE_COUNTER_0_BASE);
		PERF_BEGIN(PERFORMANCE_COUNTER_0_BASE, 1);
		graySDF(image_sequence[current_image],graydata);
		err = OSQPost(CommQ, (void *)graydata);
    free(graydata);
		OSSemPend(Task1TmrSem, 0, &err);
		current_image=(current_image+1) % sequence_length;

	}
}

void task2(void* pdata) {
	INT8U err;
	void* msg;
	unsigned char* gray;
	while(1)
	{
	msg = OSQPend(CommQ, 0, &err);
		gray = (unsigned char*) msg;
	asciiSDF(gray);
    PERF_END(PERFORMANCE_COUNTER_0_BASE, 1);  

		/* Print report */
		perf_print_formatted_report
		(PERFORMANCE_COUNTER_0_BASE,
		ALT_CPU_FREQ,        // defined in "system.h"
		1, 
		"Section 1"               
		);
	OSSemPend(Task2TmrSem, 0, &err);
	}
}

void StartTask(void* pdata)
{
  INT8U err;
  void* context;
   void* CommMsg[1024];
  static alt_alarm alarm;     /* Is needed for timer ISR function */

  /* Base resolution for SW timer : HW_TIMER_PERIOD ms */
  delay = alt_ticks_per_second() * HW_TIMER_PERIOD / 1000;
  printf("delay in ticks %d\n", delay);

  /*
   * Create Hardware Timer with a period of 'delay'
   */
  if (alt_alarm_start (&alarm,
      delay,
      alarm_handler,
      context) < 0)
      {
          printf("No system clock available!n");
      }

  /*
   * Create and start Software Timer
   */

   //Create Task1 Timer
   Task1Tmr = OSTmrCreate(0, //delay
                            TASK1_PERIOD/HW_TIMER_PERIOD, //period
                            OS_TMR_OPT_PERIODIC,
                            Task1TmrCallback, //OS_TMR_CALLBACK
                            (void *)0,
                            "Task1Tmr",
                            &err);
   Task2Tmr = OSTmrCreate(0, //delay
                            TASK2_PERIOD/HW_TIMER_PERIOD, //period
                            OS_TMR_OPT_PERIODIC,
                            Task2TmrCallback, //OS_TMR_CALLBACK
                            (void *)0,
                            "Task2Tmr",
                            &err);
   
   OSTmrStart(Task1Tmr, &err);
   OSTmrStart(Task2Tmr, &err);
   if (DEBUG) {
    if (err == OS_ERR_NONE) { //if start successful
      printf("Task1Tmr started\n");
    }
   }


   /*
   * Creation of Kernel Objects
   */

  Task1TmrSem = OSSemCreate(0);
  Task2TmrSem = OSSemCreate(0);
	
    CommQ = OSQCreate(&CommMsg[0], 1024); 
  /*
   * Create statistics task
   */

  OSStatInit();

  /*
   * Creating Tasks in the system
   */

  err=OSTaskCreateExt(task1,
                  NULL,
                  (void *)&task1_stk[TASK_STACKSIZE-1],
                  TASK1_PRIORITY,
                  TASK1_PRIORITY,
                  task1_stk,
                  TASK_STACKSIZE,
                  NULL,
                  0);
err=OSTaskCreateExt(task2,
                  NULL,
                  (void *)&task2_stk[TASK_STACKSIZE-1],
                  TASK2_PRIORITY,
                  TASK2_PRIORITY,
                  task2_stk,
                  TASK_STACKSIZE,
                  NULL,
                  0);



  printf("All Tasks and Kernel Objects generated!\n");

  /* Task deletes itself */

  OSTaskDel(OS_PRIO_SELF);
}


int main(void) {

  printf("MicroC/OS-II-Vesion: %1.2f\n", (double) OSVersion()/100.0);

  OSTaskCreateExt(
	 StartTask, // Pointer to task code
         NULL,      // Pointer to argument that is
                    // passed to task
         (void *)&StartTask_Stack[TASK_STACKSIZE-1], // Pointer to top
						     // of task stack
         STARTTASK_PRIO,
         STARTTASK_PRIO,
         (void *)&StartTask_Stack[0],
         TASK_STACKSIZE,
         (void *) 0,
         OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

  OSStart();

  return 0;
}
