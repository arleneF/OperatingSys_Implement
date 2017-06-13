/*
 * reference: 
 * https://people.mpi-sws.org/~druschel/courses/os/lectures/proc4.pdf
 * sync.c
 *
 *
 */

#define _REENTRANT
#define MIN_DELAY 1
#define MAX_DELAY 4096

#include "sync.h"
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>



/*
 * Spinlock routines
 */
/*//--------------need to put into .h file---------------//
//-------not sure whether lock_value should be volatile or not---------//
struct my_mutex_struct {
    unsigned long lock_value; // lock is free(0), lock is busy(1)
    //"We do not want the thread to deadlock on its own access.
    int count; //a counter for how many times the lock has been locked by the recursive function.
    int ownerID;//The owner is just the threadID so you just need to include a owner ID in the data structure
};*/

int my_spinlock_init(my_spinlock_t *lock)
{
    lock->lock_value = 0;
    lock->count = 0;
    lock->ownerID = 0;
    return 0;
}

int my_spinlock_destroy(my_spinlock_t *lock)
{//-------------not sure if it is right-----------------//
    //A2: It is possible that your function does not do much other then return success. Some implementations may use dynamic allocation, and thus need to do memory cleanup.
    return 0;
}

//reference: https://en.wikipedia.org/wiki/Reentrant_mutex
//1-Acquire the control condition, asserting that the owner is the releaser.
//2-Decrement the acquisition count, asserting that the count is greater than or equal to zero.
//3-If the acquisition count is zero, clear the owner information and notify the control condition.
//4-Release the control condition.
//Main goal: we do not want the thread to deadlock on its own access

//------------------version3----not working------------------//
/*int my_spinlock_unlock(my_spinlock_t *lock)
{
    //https://linux.die.net/man/2/gettid
    //http://stackoverflow.com/questions/6372102/what-is-the-difference-between-pthread-self-and-gettid-which-one-should-i-u
    //not sure to use gettid or pthread_self
    if(lock->ownerID== pthread_self()) //if it is its own access (by owner thread)
    {	//if the lock has been asked for the owner more than once
        if (lock->count > 1){
            lock->count=lock->count-1;
            return 0;}
        else if (lock->count == 1 || lock->count == 0){
            //else if the lock has been asked only for once
            //then if called unlock, just put lock to default value
            //if the lock has been not asked, also put value into default
            lock->lock_value = 0;
            lock->count = 0;
            lock->ownerID = 0;
            return 0;}
    }
    else
        return -1; //unlock cannot been called by the thread that not the owner
}*/
//------------------version3----not working------------------//

//----------------Working Version1----------------//
int my_spinlock_unlock(my_spinlock_t *lock)
{	if(lock->ownerID!= pthread_self() || (lock == NULL)) //different thread can unlock???
		return -1;
	//printf("here\n");
	//if( (lock == NULL)) //different thread can unlock???
		//return -1;
	//printf("owner: %d\n", lock->ownerID);//----------------??????????
	//printf("pthread: %lu\n", pthread_self());//----------------??????????
	//printf("syscall: %d\n", (pid_t) syscall(SYS_gettid));//--------?????????
	if (lock->count>1)
		lock->count=lock->count-1;
	else{
		lock->ownerID=-1;
		lock->count=0;
		lock->lock_value=0;
	}	
    return 0;
}

/*int my_spinlock_unlock(my_spinlock_t *lock)
{
	if( (lock == NULL)) //different thread can unlock???
		return -1;
	if (lock->count>1)
		lock->count=lock->count-1;
	else{
		lock->ownerID=0;
		lock->count=0;
		lock->lock_value=0;
	}	
    return 0;
}*/

//----------------Working Version1----------------//
int my_spinlock_lockTAS(my_spinlock_t *lock) 
{
    //while didnt get lock, keep spining
    while (tas(&(lock->lock_value)) && lock->ownerID != pthread_self()){}
    //if calling thread is asking for a lock
    if (lock->ownerID== pthread_self()){
        lock->count=lock->count+1;
        lock->ownerID = pthread_self();
        return 0;
    }
    lock->count=lock->count+1;
    lock->ownerID = pthread_self(); //assign the lock
    return 0;
}

/*int my_spinlock_lockTAS(my_spinlock_t *lock)
{
    while ( tas(&(lock->lock_value)) &&  (lock->ownerID) != pthread_self() ){  } 
    if (lock->ownerID == pthread_self() ){
      lock->count = (lock->count) +1  ;
      return 0;
    }
    lock->count=1 ;
    lock->ownerID = pthread_self() ; //got the lock
    return 0;
 }*/

//----------------Working Version2----------------//
/*int my_spinlock_lockTAS(my_spinlock_t *lock)
{    
	//while didnt get lock, keep spining
    while(tas(&(lock->lock_value))==1 && lock->ownerID != pthread_self()){}
    if(lock->ownerID == pthread_self()) 
		lock->count++;
    lock->ownerID = pthread_self();//assign the lock

  return 0;
}
*/
int my_spinlock_lockTTAS(my_spinlock_t *lock)
{
    while(1)
    {   //while is 1: keep spining (locked) //lurking state
        while(lock->lock_value){}
        //tas=0:get lock, tas=1: didnt get lock
        //go to pouncing state //now lock is free, reset all data
        if((!tas(&(lock->lock_value))) && (lock->ownerID!=pthread_self())){
            lock->count=1;
            lock->ownerID=pthread_self();
            return 0;}
        //---------------------not sure----------------------//
        if((!tas(&(lock->lock_value))) && (lock->ownerID=pthread_self())){
            lock->count=lock->count+1;
            lock->ownerID=pthread_self();
            return 0;}
    }
}

/*There are functions that try to acquire a lock only once and immediately return a value telling about success or failure to acquire the lock. They can be used if you need no access to the data protected with the lock when some other thread is holding the lock. You should acquire the lock later if you then need access to the data protected with the lock.
 
 spin_trylock() does not spin but returns non-zero if it acquires the spinlock on the first try or 0 if not. This function can be used in all contexts like spin_lock: you must have disabled the contexts that might interrupt you and acquire the spin lock.
*/
//--------------------------shen me dong xi?-------------------//
int my_spinlock_trylock(my_spinlock_t *lock)
{
	if (lock ==NULL)
		return -1;
	if (tas(&(lock->lock_value))) //if lock is busy
        return -1;
    lock->ownerID = pthread_self();//otherwise (lock is free)
    lock->count++;
    return 0;
}


/*
 * Mutex routines
 */

int my_mutex_init(my_mutex_t *lock)
{
    lock->lock_value = 0;
    lock->count = 0;
    lock->ownerID = 0;
    return 0;
}

int my_mutex_destroy(my_mutex_t *lock)
{
    return 0;
}

int my_mutex_unlock(my_mutex_t *lock)
{
	if(lock->ownerID!= pthread_self() || (lock == NULL)) 
		return -1;
	if (lock->count>1)
		lock->count=lock->count-1;
	else{
		lock->ownerID=1;
		lock->count=0;
		lock->lock_value=0;
	}	
    return 0;
}

//sleep(3) is a posix standard library method that attempts to suspend the calling thread for the amount of time specified in seconds. usleep(3) does the same, except it takes a time in microseconds instead. Both are actually implemented with the nanosleep(2) system call.

int my_mutex_lock(my_mutex_t *lock) //TTAS //also need an exponential backoff
{
    //should include <sys/types.h>
    useconds_t delay = MIN_DELAY;//Used for time in microseconds.
    while(1)
    {
        while(lock->lock_value){}
        if((!tas(&(lock->lock_value))) && (lock->ownerID!=pthread_self())){
            lock->count=1;
            lock->ownerID=pthread_self();
            return 0;}
        if((!tas(&(lock->lock_value))) && (lock->ownerID=pthread_self())){
            lock->count=lock->count+1;
            lock->ownerID=pthread_self();
            return 0;}
        usleep(rand() % delay);//---------------bu zhi dao neng bu neng yong
        if (delay<MAX_DELAY)
            delay=delay*2;
    }
}

//The mutex_trylock() function is the same as mutex_lock (), respectively, except that if the mutex object referenced by mp is locked (by any thread, including the current thread), the call returns immediately with an error.
// mutex_trylock() does not suspend your task but returns non-zero if it could lock the mutex on the first try or 0 if not. This function cannot be safely used in hardware or software interrupt contexts despite not sleeping.
int my_mutex_trylock(my_mutex_t *lock)
{//-------------so it should be a little different with spin trylock, but where?
	if (lock ==NULL)
		return -1;
	if (tas(&(lock->lock_value)))
        return -1;
    lock->ownerID = pthread_self();
    lock->count=lock->count+1;
    return 0;
}

/*
 * Queue Lock
 */
int my_queuelock_init(my_queuelock_t *lock)
{
    lock->lock_value=0;
    lock->count=0;
    lock->current_ticket=0;
    lock->next_ticket=0;
	lock->ownerID=0;
    return 0;
}

int my_queuelock_destroy(my_queuelock_t *lock)
{
     return 0;
}

int my_queuelock_unlock(my_queuelock_t *lock)
{
	if((lock == NULL) || (lock->ownerID != pthread_self())) return -1;

	lock->current_ticket++;
	lock->ownerID =0;
	lock->count = 0;
	lock->lock_value = 0; //=0 lock is free

	return 0;
}
/*int my_queuelock_unlock(my_queuelock_t *lock)
{
	if(lock->ownerID == pthread_self()) {
		if (lock->count > 0)
		    lock->count=lock->count-1;
		else if (lock->count = 0){
		    lock->current_ticket++;
			lock->ownerID =0;
			lock->count = 0;
			lock->lock_value = 0; }
	   	else
			return -1;} //unlock cannot been called by the thread that not the owner
}*/

/*int my_queuelock_unlock(my_queuelock_t *lock)
{
	if (lock->ownerID != pthread_self()) return -1;
	else{
		//lock->ownerID=0;
		printf("count: %d\n",lock->count);
		lock->count--;
		lock->lock_value=0;
	}printf("e");
	return 0;
}*/


//Reference: http://stackoverflow.com/questions/2140536/g-cas-sync-val-compare-and-swap-problem-needs-explaining
// __sync_val_compare_and_swap will always return the old value of the variable, even if no swap took place.
//static inline unsigned long cas(volatile unsigned long* ptr, unsigned long old, unsigned long _new)//Compare and Swap
//These builtins perform an atomic compare and swap. That is, if the current value of *ptr is oldval, then write newval into *ptr. 
int my_queuelock_lock(my_queuelock_t *lock)
{
	if(lock == NULL) return -1;
	while(1) {
			while(lock->lock_value == 1) {}; 
			if((tas(&lock->lock_value) == 0) && (lock->ownerID!=pthread_self())) {
				while(lock->current_ticket != cas(&lock->current_ticket,lock->next_ticket,lock->next_ticket++)) {}
				lock->count++; 
				lock->ownerID = pthread_self();
				return 0;
			}
	}
	return 0;
}

int my_queuelock_trylock(my_queuelock_t *lock)
{
	if (lock ==NULL)
		return -1;
	if (tas(&(lock->lock_value)))
        return -1;
    lock->ownerID = pthread_self();
	lock->current_ticket++;
	lock->next_ticket++;
    lock->count++;
    return 0;
}

