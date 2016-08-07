#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "threadlib.h"

//#define DEBUG
/* uncomment when you are done! */


typedef struct tcb * Thread;
/* information about threads */
struct tcb { 
  
   long int * sp;

   void *bp; 
   /* 
   * Address of stack pointer. 
	* Keep this as first element would ease switch.S 
	* You can do something else as well. 
	*/  
   Thread pre;
   Thread next;
     
  /* you will need others stuff */ 
}tcb_t;



/**
 * assembly code for switching 
 * @sp -- new stack to switch 
 * return sp of the old thread
 * 
 * Switching 
*/
void machine_switch(Thread new_thread /* addr. of new TCB */, 
		    Thread current_thread /* addr. of old TCB */);

void switch_threads(Thread new_thread /* addr. of new TCB */, 
		    Thread old_thread /* addr. of old TCB */);
		    
void switch_to(Thread new_thread);
/** Data structures and functions to support thread control box */ 





/** end of data structures */



void switch_threads(Thread new_thread /* addr. of new TCB */, Thread current_thread /* addr. of old TCB */) {

   /* This is basically a front end to the low-level assembly code to switch. */
   machine_switch(new_thread,current_thread);

  // assert(!printf("Implement %s",__func__));

}


/*********************************************************
 *                 Thread creation etc 
 *********************************************************/

/* Notes: make sure to have sufficient space for the stack
 * also it needs to be aligned 
 */

#define STACK_SIZE (sizeof(void *) * 1024)
#define FRAME_REGS 48 // is this correct for x86_64?

#include <stdlib.h>
#include <assert.h>

/*
 * allocate some space for thread stack.
 * malloc does not give size aligned memory 
 * this is some hack to fix that.
 * You can use the code as is. 
 */
void * malloc_stack(void); 


Thread head=NULL;
Thread first_thread=NULL;
int thread_count=0;



void * malloc_stack() 
{
  /* allocate something aligned at 16
   */
   void *ptr = malloc(STACK_SIZE + 16);
   if (!ptr) return NULL;
  return ptr;
}

int create_thread(void (*ip)(void)) {
	
	long int * Stack; 
	Stack = malloc_stack();
	if(!Stack) return -1; /* no memory? */

   Thread new_thread= (Thread) malloc(sizeof(tcb_t));
   if(!new_thread) return -1;
   
   new_thread->bp=Stack;
   Stack=(void *) (((long int)Stack & (-1 << 4) ) + STACK_SIZE -1);
   *(Stack)= (long int) ip;
   Stack--;
   *(Stack)= (long int )Stack +16;
   new_thread->sp = Stack - 14;

   thread_count++;

   if(thread_count == 1){
      head= new_thread;
      first_thread= new_thread;
      new_thread->next= first_thread;
      new_thread->pre = first_thread;
   }
   else if(thread_count == 2){

      head->next=new_thread;
      head->pre=new_thread;
      new_thread->pre=head;
      new_thread->next=head;

      head=new_thread;


   }

    else{

      head->next=new_thread;
      new_thread->pre=head;
      head=new_thread;
      head->next=first_thread;
      first_thread->pre=new_thread;
   }


  /**
   * Stack layout: last slot should contain the return address and I should have some space 
   * for callee saved registers. Also, note that stack grows downwards. So need to start from the top. 
   * Should be able to use this code without modification Basic idea: C calling convention tells us the top 
   * most element in the stack should be return ip. So we create a stack with the address of the function 
   * we want to run at this slot. 
   */

	return 0;
}

void yield(){
  /* thread wants to give up the CPUjust call the scheduler to pick the next thread. */
  head= head->next;
  switch_threads(head,head->pre);
  #ifdef DEBUG
    printf("%p  and %p\n",head,head->pre );
  #endif
}


void delete_thread(void){

   free(head->bp);
   if(thread_count==1){
  
    free(head);
    thread_count=0;
    exit(0);

   }

   else{
      #ifdef DEBUG
        printf("%p  and %p\n",head,head->pre );
      #endif
      Thread temp_head=head;


      head->pre->next=head->next;
      head->next->pre=head->pre;
      head=head->next;

      free(temp_head);
      thread_count--;
      switch_to(head);
     

   }

  /* When a user-level thread calls this you should not 
   * let it run any more but let others run
   * make sure to exit when all user-level threads are dead */ 


 // assert(!printf("Implement %s",__func__));
}


void stop_main(void)
{ 
  #ifdef DEBUG
    printf("%p  and %p and %p\n",head,head->next,first_thread );
  #endif
	switch_to(head);


}
