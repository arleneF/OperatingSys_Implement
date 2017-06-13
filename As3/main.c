#include "main.h"


void *fnC(int numItterations)
{
    int i;
    for(i=0;i<numItterations;i++)
    {  
        c++;  
    }   
}

//-----------------initialization--------------//
pthread_mutex_t count_mutex;
pthread_spinlock_t count_spinlock;
my_mutex_t myMutexLock;
my_spinlock_t mySpinLock;
my_queuelock_t myQueueLock;

//-----------------TestID=1--------------//
void *pthreadMutexTest()
{
    int i;
	int j;
	int k;
	int localCount = 0;
	
    for(i=0;i<numItterations;i++)
    {
		for(j=0;j<workOutsideCS;j++)/*How much work is done outside the CS*/
			localCount++;
		pthread_mutex_lock(&count_mutex);
		for(k=0;k<workInsideCS;k++)/*How much work is done inside the CS*/
			c++;
		pthread_mutex_unlock(&count_mutex);
    }
}

//-----------------TestID=2--------------//
void *pthreadSpinlockTest()
{
	int i;
	int j;
	int k;
	int localCount = 0;
    
    for(i=0;i<numItterations;i++)
    {
        for(j=0;j<workOutsideCS;j++)/*How much work is done outside the CS*/
            localCount++;
        pthread_spin_lock(&count_spinlock);
        for(k=0;k<workInsideCS;k++)/*How much work is done inside the CS*/
            c++;
        pthread_spin_unlock(&count_spinlock);
    }
}


//-----------------TestID=3--------------//
void *mySpinlockTASTest()//TAS 
{
	int i;
	int j;
	int k;
	int localCount = 0;
    
    for(i=0;i<numItterations;i++)
    {
        for(j=0;j<workOutsideCS;j++)/*How much work is done outside the CS*/
            localCount++;
        my_spinlock_lockTAS(&mySpinLock);
        for(k=0;k<workInsideCS;k++)/*How much work is done inside the CS*/
            c++;
        my_spinlock_unlock(&mySpinLock);
    }
}

//-----------------TestID=4--------------//
void *mySpinlockTTASTest()//TTAS
{
	int i;
	int j;
	int k;
	int localCount = 0;
    
    for(i=0;i<numItterations;i++)
    {
        for(j=0;j<workOutsideCS;j++)//How much work is done outside the CS
            localCount++;
        my_spinlock_lockTTAS(&mySpinLock);
        for(k=0;k<workInsideCS;k++)//How much work is done inside the CS
            c++;
        my_spinlock_unlock(&mySpinLock);
    }
}

//-----------------TestID=5--------------//
void *myMutexLockTest()//with TTAS method
{
	int i;
	int j;
	int k;
	int localCount = 0;
    
    for(i=0;i<numItterations;i++)
    {
        for(j=0;j<workOutsideCS;j++)//How much work is done outside the CS
            localCount++;
        my_mutex_lock(&myMutexLock);
        for(k=0;k<workInsideCS;k++)//How much work is done inside the CS
            c++;
        my_mutex_unlock(&myMutexLock);
    }
}

//-----------------TestID=6--------------//
void *myQueueLockLockTest()//TTAS
{
	int i;
	int j;
	int k;
	int localCount = 0;
    
    for(i=0;i<numItterations;i++)
    {
        for(j=0;j<workOutsideCS;j++)//How much work is done outside the CS
            localCount++;
        my_queuelock_lock(&myQueueLock);
        for(k=0;k<workInsideCS;k++)//How much work is done inside the CS
            c++;
        my_queuelock_unlock(&myQueueLock);
    }
}

int runTest(int testID)
{
/*testid: 0=all, 1=pthreadMutex, 2=pthreadSpinlock, 3=mySpinLockTAS, 4=mySpinLockTTAS, 5=myMutexTAS, 6=myQueueLock*/
	int readIteraton = c;
	//printf("c: %lld\n",c);
    if (testID == 0 || testID == 1 ) /*Pthread Mutex*/
    {
        c=0;
        struct timespec start;
        struct timespec stop;
        unsigned long long result; //64 bit integer
        pthread_t *threads = (pthread_t* )malloc(sizeof(pthread_t)*numThreads);	
        int i;
        int rt;
        clock_gettime(CLOCK_MONOTONIC, &start);
        for(i=0;i<numThreads;i++)
             if(rt=(pthread_create( threads+i, NULL, &pthreadMutexTest, NULL))){
                printf("Thread creation failed: %d\n", rt);
                return -1;}
        for(i=0;i<numThreads;i++) //Wait for all threads to finish
             pthread_join(threads[i], NULL);
        clock_gettime(CLOCK_MONOTONIC, &stop);
		
		//printf("c2: %lld\n",c);
        printf("TEST1: Threaded Run Pthread (Mutex) Total Count: %d\n", readIteraton);
        result=timespecDiff(&stop,&start);
        printf("       Pthread Mutex time(ms): %llu\n",result/1000000);
    }

    if(testID == 0 || testID == 2) /*Pthread Spinlock*/
    {	
		c=0;
        struct timespec start;
        struct timespec stop;
        unsigned long long result; //64 bit integer

		//int pthread_spin_init(pthread_spinlock_t *lock, int pshared)
		pthread_spin_init(&count_spinlock,0); //from <pthread.h>

        pthread_t *threads = (pthread_t* )malloc(sizeof(pthread_t)*numThreads);	
        int i;
        int rt;

        clock_gettime(CLOCK_MONOTONIC, &start);
        for(i=0;i<numThreads;i++)
             if(rt=(pthread_create( threads+i, NULL, &pthreadSpinlockTest, NULL))){
                printf("Thread creation failed: %d\n", rt);
                return -1;}
        for(i=0;i<numThreads;i++) //Wait for all threads to finish
             pthread_join(threads[i], NULL);
        clock_gettime(CLOCK_MONOTONIC, &stop);

        printf("TEST2: Threaded Run Pthread (Spinlock) Total Count: %d\n", readIteraton);
        result=timespecDiff(&stop,&start);
        printf("       Pthread Spinlock time(ms): %llu\n",result/1000000);

		//int pthread_spin_destroy(pthread_spinlock_t *lock)
		pthread_spin_destroy(&count_spinlock);//from <pthread.h>
    }
   // printf("testID is: %d\n", testID);
    if(testID == 0 || testID == 3) /*MySpinlockTAS*/
    {
		c=0;
        struct timespec start;
        struct timespec stop;
        unsigned long long result; //64 bit integer

		my_spinlock_init(&mySpinLock);

        pthread_t *threads = (pthread_t* )malloc(sizeof(pthread_t)*numThreads);	
        int i;
        int rt;

        clock_gettime(CLOCK_MONOTONIC, &start);
        for(i=0;i<numThreads;i++)
             if(rt=(pthread_create( threads+i, NULL, &mySpinlockTASTest, NULL))){
                printf("Thread creation failed: %d\n", rt);
                return -1;}      
        for(i=0;i<numThreads;i++) //Wait for all threads to finish
            pthread_join(threads[i], NULL);
        clock_gettime(CLOCK_MONOTONIC, &stop);

        printf("TEST3: Threaded Run My Spinlock TAS Total Count: %d\n", readIteraton);
        result=timespecDiff(&stop,&start);
        printf("       MySpinlockTAS time(ms): %llu\n",result/1000000);

		my_spinlock_destroy(&mySpinLock);
    }

	if(testID == 0 || testID == 4) /*mySpinLockTTAS*/
    {
		c=0;
        struct timespec start;
        struct timespec stop;
        unsigned long long result; //64 bit integer

		my_spinlock_init(&mySpinLock);

        pthread_t *threads = (pthread_t* )malloc(sizeof(pthread_t)*numThreads);	
        int i;
        int rt;

        clock_gettime(CLOCK_MONOTONIC, &start);
        for(i=0;i<numThreads;i++)
             if(rt=(pthread_create( threads+i, NULL, &mySpinlockTTASTest, NULL))){
                printf("Thread creation failed: %d\n", rt);
                return -1;}        
        for(i=0;i<numThreads;i++) //Wait for all threads to finish
             pthread_join(threads[i], NULL);
        clock_gettime(CLOCK_MONOTONIC, &stop);

        printf("TEST4: Threaded Run My SpinLock TTAS Total Count: %d\n", readIteraton);
        result=timespecDiff(&stop,&start);
        printf("       mySpinLockTTAS time(ms): %llu\n",result/1000000);

		my_spinlock_destroy(&mySpinLock);
    }

	if(testID == 0 || testID == 5) /*myMutexTAS*/
    {
		c=0;
        struct timespec start;
        struct timespec stop;
        unsigned long long result; //64 bit integer

		my_mutex_init(&myMutexLock);

        pthread_t *threads = (pthread_t* )malloc(sizeof(pthread_t)*numThreads);	
        int i;
        int rt;

        clock_gettime(CLOCK_MONOTONIC, &start);
        for(i=0;i<numThreads;i++)
             if(rt=(pthread_create( threads+i, NULL, &myMutexLockTest, NULL))){
                printf("Thread creation failed: %d\n", rt);
                return -1;}        
        for(i=0;i<numThreads;i++) //Wait for all threads to finish
             pthread_join(threads[i], NULL);
        clock_gettime(CLOCK_MONOTONIC, &stop);

        printf("TEST5: Threaded Run My Mutex TAS Total Count: %d\n", readIteraton);
        result=timespecDiff(&stop,&start);
        printf("       myMutexTAS time(ms): %llu\n",result/1000000);

		my_mutex_destroy(&myMutexLock);
    }

	if(testID == 0 || testID == 6) /*myQueueLock*/
    {
		c=0;
        struct timespec start;
        struct timespec stop;
        unsigned long long result; //64 bit integer

		my_queuelock_init(&myQueueLock);

        pthread_t *threads = (pthread_t* )malloc(sizeof(pthread_t)*numThreads);	
        int i;
        int rt;

        clock_gettime(CLOCK_MONOTONIC, &start);
        for(i=0;i<numThreads;i++)
             if(rt=(pthread_create( threads+i, NULL, &myQueueLockLockTest, NULL))){
                printf("Thread creation failed: %d\n", rt);
                return -1;}      
        for(i=0;i<numThreads;i++) //Wait for all threads to finish
             pthread_join(threads[i], NULL);
        clock_gettime(CLOCK_MONOTONIC, &stop);

        printf("TEST6: Threaded Run My Queue Lock CAS Total Count: %d\n", readIteraton);
        result=timespecDiff(&stop,&start);
        printf("       myQueueLock time(ms): %llu\n",result/1000000);

		my_queuelock_destroy(&myQueueLock);
    }
    return 0;
}

int testAndSetExample()
{
    volatile long test = 0; //Test is set to 0
    printf("Test before atomic OP: %lu\n",test);
    tas(&test);
    printf("Test after atomic OP: %lu\n",test);
}

int processInput(int argc, char *argv[])
{
    /*testid: 0=all, 1=pthreadMutex, 2=pthreadSpinlock, 3=mySpinLockTAS, 4=mySpinLockTTAS, 5=myMutexTAS, 6=myQueueLock*/
        /*You must write how to parse input from the command line here, your software should default to the values given below if no input is given*/
//------------------------------------------not consider the default case
    numThreads=4;
    numItterations=1000000;
    testID=0;
    workOutsideCS=0;
    workInsideCS=1;

    int i=0;
    for (i=0;i<argc;i++){ //pass in command and cast string to int
        if (strcmp("-t", argv[i])==0)
            numThreads = atoi(argv[i+1]);//make thread number equal to value after -t
        else if (strcmp("-i", argv[i])==0)
            numItterations = atoi(argv[i+1]);
        else if (strcmp("-o", argv[i])==0)
            workOutsideCS = atoi(argv[i+1]);
        else if (strcmp("-c", argv[i])==0)
            workInsideCS = atoi(argv[i+1]);
        else if (strcmp("-d", argv[i])==0)
            testID = atoi(argv[i+1]);
    }
	fnC(numItterations);
    printf("numThreads is: %d\n", numThreads);
    printf("numItterations is: %d\n", numItterations);
    printf("workOutsideCS is: %d\n", workOutsideCS);
    printf("workInsideCS is: %d\n", workInsideCS);
    printf("testID is: %d\n", testID);
    return 0;
}


int main(int argc, char *argv[])
{
	printf("Usage of: %s -t #threads -i #Itterations -o #OperationsOutsideCS -c #OperationsInsideCS -d testid\n", argv[0]);
	printf("testid: 0=all, 1=pthreadMutex, 2=pthreadSpinlock, 3=mySpinLockTAS, 4=mySpinLockTTAS, 5=myMutexTAS, 6=myQueueLock, \n");	

	//testAndSetExample(); //Uncomment this line to see how to use TAS
	
	processInput(argc,argv);
	runTest(testID);
	return 0;

}
