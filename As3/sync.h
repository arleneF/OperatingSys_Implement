//Reference:
//https://docs.oracle.com/cd/E19683-01/806-6867/sync-12/index.html

/*
 * NAME, etc.
 *
 *
 * sync.h
 */
#include "atomic_ops.h"


#ifndef _my_SYNC1_H_
#define _my_SYNC1_H_

//it should be volatile, otherwise compilor will generate a note
struct my_mutex_struct {
    volatile unsigned long lock_value; // lock is free(0), lock is busy(1)
    //"We do not want the thread to deadlock on its own access.
    int count; //a counter for how many times the lock has been locked by the recursive function.
    int ownerID;//The owner is just the threadID so you just need to include a owner ID in the data structure
};
typedef struct my_mutex_struct my_mutex_t;

int my_mutex_init(my_mutex_t *lock);
int my_mutex_unlock(my_mutex_t *lock);
int my_mutex_destroy(my_mutex_t *lock);

int my_mutex_lock(my_mutex_t *lock);
int my_mutex_trylock(my_mutex_t *lock);



/*Spinlock Starts here*/
struct my_spinlock_struct {
    volatile unsigned long lock_value; // lock is free(0), lock is busy(1)
    //"We do not want the thread to deadlock on its own access.
    int count; //a counter for how many times the lock has been locked by the recursive function.
    int ownerID;//The owner is just the threadID so you just need to include a owner ID in the data structure
};
typedef struct my_spinlock_struct my_spinlock_t;

int my_spinlock_init(my_spinlock_t *lock);
int my_spinlock_destroy(my_spinlock_t *lock);
int my_spinlock_unlock(my_spinlock_t *lock);

int my_spinlock_lockTAS(my_spinlock_t *lock);
int my_spinlock_lockTTAS(my_spinlock_t *lock);
int my_spinlock_trylock(my_spinlock_t *lock);


/*queuelock Starts here*/
struct my_queuelock_struct {
    volatile unsigned long lock_value; // lock is free(0), lock is busy(1)
    //"We do not want the thread to deadlock on its own access.
    int count; //a counter for how many times the lock has been locked by the recursive function.
    volatile unsigned long current_ticket;
    volatile unsigned long next_ticket;
	int ownerID;
};
typedef struct my_queuelock_struct my_queuelock_t;

int my_queuelock_init(my_queuelock_t *lock);
int my_queuelock_destroy(my_queuelock_t *lock);
int my_queuelock_unlock(my_queuelock_t *lock);

int my_queuelock_lock(my_queuelock_t *lock);
int my_queuelock_trylock(my_queuelock_t *lock);


#endif
