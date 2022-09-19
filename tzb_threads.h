/*
Operating Systems Coursework 5 at University of Leeds
Cooperative User-level Multithreading Library
Tomás Zilhão Borges, 201372847
*/

#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

#define STACK_SIZE  8192 //page aligned!! 2 pages (4096 bytes), does the second page work as page guard somewhow??

//States of a thread
#define RUNNING     1
#define RUNNABLE    0


// Saved registers for context switches.
typedef struct thread_context_struct {
  uint64 ra;//return address: holds the return address from which swtch was called          
  uint64 sp; //stack pointer

  // callee-saved
  uint64 s0;
  uint64 s1;
  uint64 s2;
  uint64 s3;
  uint64 s4;
  uint64 s5;
  uint64 s6;
  uint64 s7;
  uint64 s8;
  uint64 s9;
  uint64 s10;
  uint64 s11;

} thread_context;

//a node for each thread
typedef struct thread_node_struct {
	
    int id; 
    char stack[STACK_SIZE]; //8192
    thread_context context;
    int state; //RUNNING, RUNNABLE

    struct thread_node_struct *next;
    struct thread_node_struct *previous;

} thread_node;

//list of all threads
typedef struct thread_list_struct {
	thread_node *head;
	thread_node *tail;

    thread_node *current;

	int size;

} thread_list;


extern void swtch( thread_context*, thread_context*) asm("swtch");


/* FUNCTIONALITY */

//returns a new unique id
int new_id(); 

//adds a new node to the list
void add_thread_node(thread_node * node);

//creates a new node and adds it to the list
//when you create, you add
thread_node * new_thread_node();

//removes a node from the list
void remove_thread_node(thread_node * node);

//removes a node from the list and frees memory
//when you delete, you remove
void delete_thread_node(thread_node * node);

//switches context to the next thread in the list of available threads
void schedule_thread();

/* USER */

//allows the user to start user-level multithreading 
void init_thread();

//allows the user to end user-level multithreading 
void exit_thread();

//gives up the current thread, switches context to the next thread in the list of available threads
void yield_thread();

//allows the user to create a new thread with a respective function that should run in it
int create_thread(void (*func)());

//allows the user to delete an existing threads from its id (a thread id is returned by spawn_thread())
void delete_by_id_thread(int id);

/* TESTING */
void mythread();
int main(int argc, char *argv[]);
void print_thread_node(thread_node * node);
