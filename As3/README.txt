//AUTHOR:		JIANGLIN FU 
//EDIT DATE:		2017-03-22
//STUDENT ID:		888 
//COMPUTING ID:		888

//Reference: http://stackoverflow.com/questions/2140536/g-cas-sync-val-compare-and-swap-problem-needs-explaining
//https://people.mpi-sws.org/~druschel/courses/os/lectures/proc4.pdf
//https://en.wikipedia.org/wiki/Reentrant_mutex
//https://docs.oracle.com/cd/E19683-01/806-6867/sync-12/index.html


Commands to run the program :

1. make
2. ./mylocks
3. option command: ./mylocks -t 8 -i 100000000 -c 1 -0 2	
step 3 is optional, this is use to mannually change the variables of "numThreads", "numItterations", "testID", "workOutsideCS" and "workInsideCS"

the sample result shown on terminal is like following:

jfa49@asb9840u-e06:~/Downloads/A3/base-a3$ ./mylocks 
Usage of: ./mylocks -t #threads -i #Itterations -o #OperationsOutsideCS -c #OperationsInsideCS -d testid
testid: 0=all, 1=pthreadMutex, 2=pthreadSpinlock, 3=mySpinLockTAS, 4=mySpinLockTTAS, 5=myMutexTAS, 6=myQueueLock, 
numThreads is: 4
numItterations is: 1000000
workOutsideCS is: 0
workInsideCS is: 1
testID is: 0
TEST1: Threaded Run Pthread (Mutex) Total Count: 1000000
       Pthread Mutex time(ms): 420
TEST2: Threaded Run Pthread (Spinlock) Total Count: 1000000
       Pthread Spinlock time(ms): 121
TEST3: Threaded Run My Spinlock TAS Total Count: 1000000
       MySpinlockTAS time(ms): 807
TEST4: Threaded Run My SpinLock TTAS Total Count: 1000000
       mySpinLockTTAS time(ms): 657
TEST5: Threaded Run My Mutex TAS Total Count: 1000000
       myMutexTAS time(ms): 62
TEST6: Threaded Run My Queue Lock CAS Total Count: 1000000
       myQueueLock time(ms): 867



