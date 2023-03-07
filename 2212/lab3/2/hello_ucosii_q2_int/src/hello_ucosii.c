#include <stdio.h>
#include "includes.h"
#include "altera_avalon_pio_regs.h"
#include "sys/alt_irq.h"
#include "sys/alt_alarm.h"

#include "imageProcessing.h"
#include "images.h"
#include "system.h"
#include "ascii_gray.h"
#include "os_cfg.h"

/* Definition of Semaphores */
#define N_RESOURCES 15
OS_EVENT *QSem;
void *QMsgTbl[N_RESOURCES];

#define DEBUG 1

#define HW_TIMER_PERIOD 100 /* 100ms */

/* Definition of Task Stacks */
#define   TASK_STACKSIZE       2048
OS_STK    task1_stk[TASK_STACKSIZE];
OS_STK    task2_stk[TASK_STACKSIZE];
OS_STK    task3_stk[TASK_STACKSIZE];
OS_STK    StartTask_Stack[TASK_STACKSIZE]; 

/* Definition of Task Priorities */

#define STARTTASK_PRIO      1
#define TASK1_PRIORITY      10
#define TASK2_PRIORITY      11
#define TASK3_PRIORITY      12

/* Definition of Task Periods (ms) */
#define TASK1_PERIOD 400
#define TASK2_PERIOD 500
#define TASK2_PERIOD 500

/*
 * Global variables
 */
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
// OS_EVENT *Task1Semaphore;
// OS_EVENT *Task2Semaphore;
OS_EVENT *Task1Semaphore;
OS_EVENT *Task2Semaphore;
OS_EVENT *Task3Semaphore;

// SW-Timer
OS_TMR *Task1Tmr;
OS_TMR *Task2Tmr;


extern unsigned char *g_shared;
extern unsigned char *grayShared;
//unsigned char* g_shared;
// void sram2sm_p32(unsigned char* base)
// {
// 	int x, y;
// 	unsigned char* shared;

// 	shared = (unsigned char*) SHARED_ONCHIP_BASE ;
//     g_shared = shared;

// 	int size_x = *base++;
// 	int size_y = (*base++)/2;
// 	int max_col= *base++;
//   base+= size_x*size_y; 
// 	*shared++  = size_x;
// 	*shared++  = size_y;
// 	*shared++  = max_col;
// 	printf("The image is: %d x %d!! \n", size_x, size_y);
// 	for(y = 0; y < size_y; y++)
// 	for(x = 0; x < size_x; x++)
// 	{ 
// 		*shared++ = *base++; 	// R   
// 		*shared++ = *base++;	// G 
// 		*shared++ = *base++;	// B
// 	}
// }





/* Timer Callback Functions */ 
void Task1TmrCallback (void *ptmr, void *callback_arg){
  OSSemPost(Task1Semaphore);
  printf("OSSemPost(Task1Sem);\n");
}

void Task2TmrCallback (void *ptmr, void *callback_arg){
  OSSemPost(Task2Semaphore);
  printf("OSSemPost(Task2Sem);\n");
}



void task1(void* pdata)
{
  INT8U err;
  while (1)
  { 
    printf("Hello from task1\n");
    OSSemPend(Task1Semaphore, 0, &err);
  

  printf("\n1st char image = %d",test_ppm_1[0]);

    //read image
    sram2sm_p32(image_sequence[0]);
    unsigned char *msg = "1357924680";
    unsigned char *data = (unsigned char*) SHARED_ONCHIP_BASE ;
    //int data= (int)g_shared;

    printf("\nsize of g_shared = %d",sizeof(g_shared));
    printf("\nsize of data = %d",sizeof(data));
    //printf("\nmessage= %s",data);
    OSQPost(QSem, (void *)data);

    OSSemPost(Task2Semaphore);
    
  }
}


void task2(void* pdata)
{
  INT8U err;
  while (1)
  { 
    printf("Hello from task2\n");
    OSSemPend(Task2Semaphore, 0, &err);

    //ascii operation
    // printAscii(grayShared+3, (int)*grayShared, (int)*(grayShared+1));
    unsigned char *msg_recvd = (unsigned char *)OSQPend(QSem, 0, &err);
    //printf("\nmesg recvd = %s",msg_recvd);

    graySDF(msg_recvd);

    //grayShared = (unsigned char*)(SHARED_ONCHIP_BASE + ONCHIP_SECTION_1);
    if(grayShared==NULL)
    {
      printf("\n NULL");
    }
    else
    {
      //printf("\nsizeof = %d, 1=%c 2=%c",sizeof(grayShared),grayShared[0],grayShared[1]);
    printf("\nmesg recvd t2=%d %d %d ",grayShared[0],grayShared[2],grayShared[3]);
    //printf("\nmesg recvd t2=%s ",grayShared);

    }
    OSQPost(QSem, (void *)grayShared);

    OSSemPost(Task3Semaphore);
  }
}

//ignored for problem 1
void task3(void* pdata)
{
  INT8U err;
  while (1)
  { 
    printf("Hello from task3\n");
    OSSemPend(Task3Semaphore, 0, &err);

    //grayscale operation
    unsigned char *msg_recvd = (unsigned char *)OSQPend(QSem, 0, &err);
    //printf("\n======> w= %d, h= %d",(int)msg_recvd[0],(int)msg_recvd[1]);
    printf("\n======> w= %d, h= %d",msg_recvd[0],msg_recvd[1]);
    
    printf("\nmesg recvd t3= %s",msg_recvd);
    printAscii(msg_recvd+3,(int)msg_recvd[0],(int)msg_recvd[1]);
    

    //OSSemPost(Task1Semaphore);
  }
}


void StartTask(void* pdata)
{
  INT8U err;
  void* context;

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
  //  Task1Tmr = OSTmrCreate(0, //delay
  //                           TASK1_PERIOD/HW_TIMER_PERIOD, //period
  //                           OS_TMR_OPT_PERIODIC,
  //                           Task1TmrCallback, //OS_TMR_CALLBACK
  //                           (void *)0,
  //                           "Task1Tmr",
  //                           &err);
                            
   if (DEBUG) {
    if (err == OS_ERR_NONE) { //if creation successful
      printf("Task1Tmr created\n");
    }
   }
   
   //Create Task1 Timer
  //  Task2Tmr = OSTmrCreate(0, //delay
  //                           TASK2_PERIOD/HW_TIMER_PERIOD, //period
  //                           OS_TMR_OPT_PERIODIC,
  //                           Task2TmrCallback, //OS_TMR_CALLBACK
  //                           (void *)0,
  //                           "Task2Tmr",
  //                           &err);
                            
   if (DEBUG) {
    if (err == OS_ERR_NONE) { //if creation successful
      printf("Task2Tmr created\n");
    }
   }

   /*
    * Start timers
    */
   
   //start Task1 Timer
   OSTmrStart(Task1Tmr, &err);
   
   if (DEBUG) {
    if (err == OS_ERR_NONE) { //if start successful
      printf("Task1Tmr started\n");
    }
   }

   //start Task2 Timer
   OSTmrStart(Task2Tmr, &err);
   
   if (DEBUG) {
    if (err == OS_ERR_NONE) { //if start successful
      printf("Task2Tmr started\n");
    }
   }  

   /*
   * Creation of Kernel Objects
   */

  Task1Semaphore = OSSemCreate(1);   
  Task2Semaphore = OSSemCreate(0); 
  Task3Semaphore = OSSemCreate(0);  

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

  if (DEBUG) {
     if (err == OS_ERR_NONE) { //if start successful
      printf("Task1 created\n");
    }
   }  

  err=OSTaskCreateExt(task2,
                  NULL,
                  (void *)&task2_stk[TASK_STACKSIZE-1],
                  TASK2_PRIORITY,
                  TASK2_PRIORITY,
                  task2_stk,
                  TASK_STACKSIZE,
                  NULL,
                  0);

  if (DEBUG) {
     if (err == OS_ERR_NONE) { //if start successful
      printf("Task 2 created\n");
    }
   }  


   err=OSTaskCreateExt(task3,
                  NULL,
                  (void *)&task3_stk[TASK_STACKSIZE-1],
                  TASK3_PRIORITY,
                  TASK3_PRIORITY,
                  task3_stk,
                  TASK_STACKSIZE,
                  NULL,
                  0);

  if (DEBUG) {
     if (err == OS_ERR_NONE) { //if start successful
      printf("Task 3 created\n");
    }
   }  

  printf("All Tasks and Kernel Objects generated!\n");

  /* Task deletes itself */

  OSTaskDel(OS_PRIO_SELF);
}


int main(void) {


  printf("MicroC/OS-II-Vesion: %1.2f\n", (double) OSVersion()/100.0);

  //Create Message Queue
  OSInit();

  QSem = OSQCreate(&QMsgTbl[0], N_RESOURCES);
  int i;
  //for(i=0;i<N_RESOURCES;i++){
    //OSQPost(QSem, (void *)1);
  //}


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
