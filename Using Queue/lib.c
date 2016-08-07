#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "threadlib.h"

#define DEBUG
/* uncomment when you are done! */

#ifdef DEBUG
 #define PRINT   printf
#else 
 #define PRINT(...)
#endif



/* information about threads */
struct tcb { 
	long int *sp;  /* Address of stack pointer. 
	      * Keep this as first element would ease switch.S 
	      * You can do something else as well. 
	      */  
	void *malloc_add;
	struct tcb * next; 
	
};

typedef struct tcb tcb_t;
typedef struct tcb *TCB;

// Two glboal variables to store address of front and rear nodes. 
struct tcb* front = NULL;
struct tcb* rear = NULL;

/**
 * assembly code for switching 
 * @sp -- new stack to switch 
 * return sp of the old thread
 * 
 * Switching 
*/
void machine_switch(tcb_t *newthread /* addr. of new TCB */, 
		    tcb_t *oldthread /* addr. of old TCB */);

void switch_threads(tcb_t *newthread /* addr. of new TCB */, 
		    tcb_t *oldthread /* addr. of old TCB */);
void remove_this(tcb_t *front);
void Enqueue(tcb_t *newTCB);
tcb_t * Dequeue(void);
		    

tcb_t * running=NULL;

tcb_t * runningnext=NULL;
tcb_t * runningtemp=NULL;
/** Data structures and functions to support thread control box */ 

void switch_threads(tcb_t *newthread /* addr. of new TCB */, tcb_t *oldthread /* addr. of old TCB */) {

  /* This is basically a front end to the low-level assembly code to switch. */
 
 machine_switch(newthread,oldthread);
 
	//assert(!printf("Implement %s",__func__));

}


// To Enqueue a tcb
void Enqueue(tcb_t * newTCB) {
	
	 
	
	if(front == NULL && rear == NULL){
		newTCB -> next = NULL;
		front = newTCB;
		rear = newTCB;
		
		return;
	}
	newTCB -> next = rear;
	
	rear = newTCB;
}

// To Dequeue a tcb.
tcb_t * Dequeue() {
	tcb_t * temp_rear = rear;
	tcb_t * temp_front = front;
	if(front == NULL) {
		printf("Queue is Empty\n");
		return front;
	}
	if(front == rear) {
		front = rear = NULL;
	}
	else {
		while(temp_rear -> next != front){
			temp_rear = temp_rear -> next;
		}
		front = temp_rear;
	}
	return temp_front;
}



/** end of data structures */


/*********************************************************
 *                 Thread creation etc 			 *
 *********************************************************/

/* Notes: make sure to have sufficient space for the stack
 * also it needs to be aligned 
 */

#define STACK_SIZE (sizeof(void *) * 1024)
#define FRAME_REGS 48 // is this correct for x86_64?

#include <stdlib.h>
#include <assert.h>

TCB head = NULL;
int no_of_threads = 0;


/*
 * allocate some space for thread stack.
 * malloc does not give size aligned memory 
 * this is some hack to fix that.
 * You can use the code as is. 
 */
void * malloc_stack(void); 

void * malloc_stack() 
{
	/* allocate something aligned at 16
	*/
	void *ptr = malloc(STACK_SIZE + 16);
	ptr = (void *)(((long int)ptr & (-1 << 4)) + STACK_SIZE);
	if (!ptr) return NULL;

	return ptr;
}

int create_thread(void (*ip)(void)) {

/**
   * Stack layout: last slot should contain the return address and I should have some space 
   * for callee saved registers. Also, note that stack grows downwards. So need to start from the top. 
   * Should be able to use this code without modification Basic idea: C calling convention tells us the top 
   * most element in the stack should be return ip. So we create a stack with the address of the function 
   * we want to run at this slot. 
   */

	
	long int  *stack; 
	stack = malloc_stack();
	if(!stack) return -1; /* no memory? */

	tcb_t *newTCB = (tcb_t *)malloc(sizeof(tcb_t));
	
	stack--;
	*(stack) = (long int)ip;
	newTCB -> sp = stack-8;
	no_of_threads++;
	Enqueue(newTCB);

	#ifdef DEBUG
		printf("front: %p\n",front);
		printf("rear: %p\n",rear);
		printf("t1->next: %p\n",front->next);
		
	#endif


	return 0;
}

void yield(){
  /* thread wants to give up the CPUjust call the scheduler to pick the next thread. */
Enqueue(runningtemp);
runningnext=Dequeue();
running=runningtemp;
runningtemp=runningnext;


switch_threads(runningnext, running);
}


void delete_thread(void){

	/* When a user-level thread calls this you should not 
	* let it run any more but let others run
	* make sure to exit when all user-level threads are dead */ 

	if(no_of_threads == 1){
		free(running);
		exit(0);

	}

	else {
		no_of_threads--;
		runningnext=Dequeue();
		running=runningtemp;
		runningtemp=runningnext;
		switch_threads(runningnext, running);
		free(running);
	}

}


void stop_main(void)
{ 
  /* Main function was not created by our thread management system. 
   * So we have no record of it. So hijack it. 
   * Do not put it into our ready queue, switch to something else.*/
	running = Dequeue();
	runningtemp=running;
	
	if(running != NULL){

		remove_this(running);

	}
	//assert(!printf("Implement %s",__func__));

}
