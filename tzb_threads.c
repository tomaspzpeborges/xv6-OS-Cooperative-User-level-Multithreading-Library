/*
Operating Systems Coursework 5 at University of Leeds
Cooperative User-level Multithreading Library
Tomás Zilhão Borges, 201372847
*/


#include "tzb_threads.h"


thread_list *list; //not allowing the creation of more than 1 list


//allows the user to start multithreading 
void exit_thread(){

    if(list->current != list->head){ //changing back to main thread before ending it all
        printf("change to main thread to exit threading\n");
        return;
    }
    thread_node * temp = 0;
    if(list->head->next != 0) {
        temp = list->head->next;
    }
    while(temp!= 0){
        delete_thread_node(temp);
        temp = list->head->next; 
    }

    free(list->head);
    free(list);

}

//allows the user to end multithreading 
void init_thread()
{
    list = malloc(sizeof(thread_list));
    list->head = 0;
    list->tail = 0;
    list->current = 0;
    list->size = 0;

    thread_node * thread_0 = new_thread_node(); //first/main thread
    thread_0->state = RUNNING;

    list->current = thread_0;
}

//switches context to the next thread in the list of available threads
void schedule_thread(){

    thread_node * temp;
    if(list->current->next != 0){
        temp = list->current->next;
    }else{ //can't go any further, go back to the beginning
        temp = list->head;
    }

    while(temp != list->current)
    { 
        if( (temp->state == RUNNABLE) ){ 
    
            break;
        }

        temp = temp->next; // Move to next node
    }

    if(temp != list->current){ //we found another thread, do context switch
    
        temp->state = RUNNING;
        thread_node * aux = list->current;
        list->current = temp;
        swtch(&aux->context, &list->current->context);


    }else{ //if no other thread and current still RUNNABLE (should i check for it?)
        list->current->state = RUNNING;

    }


}

//gives up the current thread, switches context to the next thread in the list of available threads
void yield_thread() {

  list->current->state = RUNNABLE;
  schedule_thread();
}

//allows the user to create a new thread with a respective function that should run in it
int create_thread(void (*func)()){

    thread_node * node = new_thread_node();

    node->context.sp = (uint64) (node->stack + STACK_SIZE); //sp to the top of the stack 
    node->context.ra = (uint64) func; //save return address

    return node->id;
      
}

//returns a new unique id
int new_id(){

    static int number = 0;
    return number++; //first return will be 0
}

//adds a new node to the list
void add_thread_node(thread_node * node){

	list->size++;
    
	if (list->head == 0)
	{
		list->head = list->tail = node;
	}
	else //adding at the end of list
	{
		node->previous = list->tail;
		list->tail->next = node;
		list->tail = node;
	}

}

//creates a new node and adds it to the list
//when you create, you add
thread_node * new_thread_node(){

    if(list== 0)
    {
        printf("initialize threading first\n"); 
        return 0;
    }

    thread_node * node = malloc(sizeof(thread_node));
    node->id = new_id();


    node->state = RUNNABLE;
    node->next =0;
    node->previous=0 ;
    
    add_thread_node(node);

    return node;

}

//removes a node from the list
void remove_thread_node(thread_node * node){

    
    thread_node * temp = list->head;
    while(temp != 0)
    { 
        if(node->id == temp->id){ 

            if (list->head->id == node->id) {
                list->head = node->next;
            }

            if (node->next) {
                node->next->previous = node->previous;
            }

            if (node->previous) {
                node->previous->next = node->next;
            }

            list->size--;
            break;
        }

        temp = temp->next; // Move to next node
    }


}

//removes a node from the list and frees memory
void delete_thread_node(thread_node *node){

    remove_thread_node(node);
    free(node);
}

//allows the user to deelete an existing threads from its id (a thread id is returned by spawn_thread())
void delete_by_id_thread(int id){

    if(id == 0){
        printf("can´t delete main thread\n");
        return;
    }
        
    thread_node * temp = list->head;
    while(temp != 0)
    { 
        if(id == temp->id){ 
            break;
        }

        temp = temp->next; // Move to next node
    }
    if(temp == 0){
        printf("id does not match any thread's id\n");
        return;
    }

    if(temp->state == RUNNING){ 
        printf("please yield before deleting this thread\n");
        return;
    }
    else{
        delete_thread_node(temp);
    }

}


void print_thread_node(thread_node * node){

    printf("thread id: %d\n", node->id );
    printf("thread state: %d\n", node->state);

}




void mythread()
{
    // TEST#2
    //delete_by_id_thread(list->current->id);


    int count = 0;
    int i;
    for (i = 0; i < 6; i++) {
        printf( "my thread running %d\n", list->current->id);
        printf( "counter %d\n", count++);

        yield_thread();
    }
    printf("my thread: exit\n");
    yield_thread();

}

int main(int argc, char *argv[]){


    // TEST#1
    init_thread();
    create_thread(mythread);
    create_thread(mythread);

    int count = 0;
    int i;
    //printf( "my thread running %d\n", list->current->id);
    for (i = 0; i < 7; i++) { 
        printf( "my thread running %d\n", list->current->id);
        printf( "counter %d\n", count++);

        yield_thread();
    }
    printf("my thread: exit\n"); 
    //printf("tail: %d\n", list->tail->id);
    exit_thread();


    // TEST#2, NEEDS CHANGES IN MYTHREAD
/*
    init_thread();
    int a_id = create_thread(mythread);
    create_thread(mythread);

    yield_thread(); 

    delete_by_id_thread(a_id);
    printf("SIZE: %d\n",list->size); 

*/

    printf("end\n");
    exit(0);

}

 