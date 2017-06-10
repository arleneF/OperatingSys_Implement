#define _GNU_SOURCE //to avoid warning like implicit declaration of 'cpu_zero'
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <stdlib.h>	// for malloc // for exit()
#include <sys/types.h> 	// for pid_t
#include <unistd.h>	// for chdir, getcwd, execvp, char*.....
#include <sys/wait.h>	// for waitpid()
#include <pthread.h>
#include <errno.h>
#define MAX 100000
//pipe read/stdin:   0
//pipe write/stdout: 1
/*-----------------------------------------------------------
reference: 
 http://stackoverflow.com/questions/280909/cpu-affinity
https://computing.llnl.gov/tutorials/pthreads/samples/condvar.c
-----------------------------------------------------------*/

void funcion_Call_Cost();
void system_Call_Cost();
void process_Switch_Cost();
void set_cpu_affinity();
void bare_function();
void *thread1();
void *thread2();
void thread_Switch_Cost();
//timespec is a struct specified in time.h
//this function using subtraction to find the difference between 2 time structs, and return the result in nanoseconds
double timespecDiff(struct timespec *timeA_p, struct timespec *timeB_p)
{
  	return ((timeA_p->tv_sec * 1000000000) + (timeA_p->tv_nsec)) - ((timeB_p->tv_sec * 1000000000) + (timeB_p->tv_nsec));
}

unsigned long long loopOverheadinPS(struct timespec *start, struct timespec *stop)
{
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, start); 
	int i=0;
	for(i=0; i<MAX; i++){}
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, stop);
	unsigned long long result = 1000*timespecDiff(stop, start)/MAX;
	return result;
}

unsigned long long loopOverhead(struct timespec *start, struct timespec *stop)
{
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, start); 
	int i=0;
	for(i=0; i<MAX; i++){}
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, stop);
	unsigned long long result = timespecDiff(stop, start)/MAX;
	return result;
}

/*--------------------calculate function call cost---------------------------------------*/
void bare_function() {}

void funcion_Call_Cost()
{
	struct timespec start;
	struct timespec stop;
	unsigned long long result = 0;

	/*int i=0;
	for (i=0;i<MAX;i++){
		clock_gettime(CLOCK_MONOTONIC, &start); 
		bare_function();
		clock_gettime(CLOCK_MONOTONIC, &stop);
		result = result+ timespecDiff(&stop, &start);
	}*/
	clock_gettime(CLOCK_MONOTONIC, &start); 
	int i=0;
	for (i=0;i<MAX;i++){
		bare_function();
	}
	clock_gettime(CLOCK_MONOTONIC, &stop);
	result =timespecDiff(&stop, &start)*1000;
	result=result/MAX;
	unsigned long long overhead=loopOverheadinPS(&start,&stop);
	printf("----------part2 function call-----------\n");
	printf("current iteration number: %d\n", MAX);
	printf("Measured overhead cost: %llu ps\n", overhead); 
	printf("Measured function call cost: %llu ps\n", result); 	
	if (overhead>result){
		overhead=result;
		printf("overhead is bigger than result, make larger iteration!\n");}
	else{
		printf("Measured function call cost with overhead: %llu ps\n", result-overhead); }
	printf("\n");
	//llu stands for long long unsigned
}
/*--------------------calculate function call cost---------------------------------------*/


/*--------------------calculate system call cost---------------------------------------*/
void system_Call_Cost()
{	struct timespec start;
	struct timespec stop;
	unsigned long long result= 0;
	unsigned long long overhead=loopOverheadinPS(&start,&stop);

	int i=0;
	for (i=0;i<MAX;i++){
		clock_gettime(CLOCK_MONOTONIC, &start);
		getpid();
		clock_gettime(CLOCK_MONOTONIC, &stop);
		result+= timespecDiff(&stop, &start);}
	result=result/MAX;
	printf("----------part3 system call-----------\n");
	printf("current iteration number: %d\n", MAX);
	//printf("Measured overhead cost: %llu ps\n", overhead); 
	printf("Measured system call cost: %llu ns\n", result);
	//printf("Measured system call cost: %llu ps\n", result-overhead);
	printf("\n");
}
/*--------------------calculate system call cost---------------------------------------*/



/*--------------------calculate cost of process switch---------------------------------------*/
void set_cpu_affinity()
{
	cpu_set_t mask;
	CPU_ZERO(&mask);
	CPU_SET(6, &mask);
	//if (sched_setaffinity(0, sizeof(mask), &mask))
	//	perror("sched_setaffinity Error");
	if(sched_setaffinity(0, sizeof(mask), &mask) < 0)
		perror("sched_setaffinity Error");
	if(pthread_setaffinity_np(pthread_self(), sizeof(mask), &mask) < 0)
		perror("sched_setaffinity Error");
}

void process_Switch_Cost()
{
	set_cpu_affinity();
	struct timespec start;
	struct timespec stop;
	unsigned long long result =0;
	unsigned long long overhead=loopOverhead(&start,&stop);
	pid_t childPid;
	char buffer[20];
	char singleByte = '1'; //signed char is 1 byte
	int fdP[2];
	int fdC[2];
	int timer[2];//---------method3
	pipe(fdP);
	pipe(fdC);
	pipe(timer);//----------method3
	childPid = fork();
	if (childPid < 0) {
		perror("fork() failed: ");
		printf("Errno: %d\n", errno);
		exit(EXIT_FAILURE);}
	else if (childPid==0){ //child process
		close(fdC[0]); // closes the output side of its pipe
		close(fdP[1]); // also closes the parents input side
		int i=0;
		for (i=0;i<MAX;i++){
			/* -------------method1
			//step2
			read(fdP[0], buffer, sizeof(buffer)); //put msg in fdP[0] into buffer
			//clock_gettime(CLOCK_MONOTONIC, &stop); //once finish read, jsut record the time
			//step3
			write(fdC[1],&singleByte,sizeof(singleByte)); //write stuff in singleByte to fdC[1]*/
			/*--------------method2		    	
			while (read(fdP[0],buffer,sizeof(buffer)))
				write(fdC[1],&singleByte,sizeof(singleByte));*/
			//--------------method3
			read(fdP[0],buffer,sizeof(buffer));
			write(fdC[1],&singleByte,sizeof(singleByte));
		}
		clock_gettime(CLOCK_MONOTONIC, &stop); //--------------method3
		write(timer[1],&stop,sizeof(stop));//--------------method3
		close(fdC[1]);
		close(fdP[0]);
		exit(0);
	}
	else{ //parent process
		int status;
		close(fdC[1]);
		close(fdP[0]);
		clock_gettime(CLOCK_MONOTONIC, &start); //--------------method1&2&3
		int i=0;		
		for ( i=0;i<MAX;i++){
			write(fdP[1],&singleByte,sizeof(singleByte));//step1
			//clock_gettime(CLOCK_MONOTONIC, &stop); // --------------method1&2
			read(fdC[0], buffer, sizeof(buffer)); //step4
		}
		read(timer[0],&stop,sizeof(stop)); // --------------method3
		//clock_gettime(CLOCK_MONOTONIC, &stop);//--------------method1&2
		//must divide by twice MAX, 
		//otherwise the result is the time which the switching go the child process and then go back
		result = timespecDiff(&stop, &start)/(MAX*2);//--------------method1&2&3

		printf("----------part4 process switch-----------\n");
		printf("current iteration number: %d\n", MAX);
		printf("Measured overhead cost: %llu ns\n", overhead); 
		printf("Measured switching process cost: %llu ns\n", result);
		printf("Measured switching process cost without overhead: %llu ns\n", result-2*overhead);
		//since we have 2 threads, so subtract twice of the overhead
		printf("\n");
		close(fdC[0]);
		close(fdP[1]);
		waitpid(childPid, &status, WUNTRACED); //wait(NULL);
	}
}
/*--------------------calculate cost of process switch---------------------------------------*/



/*--------------------calculate cost of thread switch---------------------------------------*/
//initialize all the variable here with "thread" to make difference with previous task
struct timespec start_thread;
struct timespec stop_thread;
unsigned long long result_thread =0;
pthread_mutex_t region;
pthread_cond_t cond1,cond2;
int buffer=0;
void *thread2()
{
	int i=0;
	for (i=0;i<MAX;i++){
	//while(i<MAX){
		/*although create thread1 first and should run thread1's critical region first
		  however initailly buffer is 0, so enter thread1's critical region, do nothing
		  and then exit thread1's critical region and enter thread2's critical region*/
		pthread_mutex_lock(&region);
			//printf("7\n");
		if (buffer==0){
			//printf("8\n");
			buffer=1;
			//clock_gettime(CLOCK_MONOTONIC, &start_thread);//way1
			pthread_cond_wait(&cond2,&region);
			//printf("8\n");
			//clock_gettime(CLOCK_MONOTONIC, &stop_thread);//way1
			/*unlocks the mutex(region) and wait for the cond to 
			  be signaled (cond2 is signaled in thread1)*/
			//printf("9\n");
			pthread_cond_signal(&cond1);
			//printf("10\n");
			//printf("i'm in thread2\n");
			/*shall unblock at least one of the threads that are blocked on 
			the specified condition variable cond (if any threads are blocked on cond).*/
			//i++;		
		}
		pthread_mutex_unlock(&region);
	}
	pthread_exit(0);
}

void *thread1()
{
	int i=0;
	for (i=0;i<MAX;i++){
	//while (i<MAX){
		pthread_mutex_lock(&region);
			//printf("4\n");
		if (buffer==1){
			buffer=0;
			//printf("5\n");
			pthread_cond_signal(&cond2);//sent signal cond2
			//printf("11\n");
			pthread_cond_wait(&cond1,&region);
			//printf("12\n");
			//printf("i'm in thread1\n");
			//i++;
		}
		pthread_mutex_unlock(&region);
			//printf("6\n");
	}
	pthread_exit(0);
}

void *threadOverhead()
{	int i=0;
	for (i=0;i<MAX;i++){}
	pthread_exit(0);
}


unsigned long long createThreadCost()
{
	int rt3;
	pthread_t overhead;
	unsigned long long result_overhead =0;
	struct timespec start_overhead;
	struct timespec stop_overhead;

	clock_gettime(CLOCK_MONOTONIC, &start_overhead);
	if (rt3=pthread_create(&overhead,NULL,threadOverhead,NULL))
		printf("Thread creation failed: %d\n", rt3);
	pthread_join(overhead,NULL);
	clock_gettime(CLOCK_MONOTONIC, &stop_overhead);//way4
	result_overhead=timespecDiff(&stop_overhead,&start_overhead)/MAX;
	//printf("Measured overhead costlalalalala: %llu ns\n", result_overhead); 
	return result_overhead;
}

void thread_Switch_Cost()
{
	int rt1,rt2;
	set_cpu_affinity();
	pthread_t prod,cons;
	pthread_mutex_init(&region,NULL);
	pthread_cond_init(&cond1,NULL);
	pthread_cond_init(&cond2,NULL);
	unsigned long long result_overhead =0;

	clock_gettime(CLOCK_MONOTONIC, &start_thread);//way3&4
	//way3 is wrong since 2 pthread_create will run first then go into the thread functions
	if (rt1=pthread_create(&cons,NULL,thread1,NULL))
		printf("Thread creation failed: %d\n", rt1);
			//printf("2\n");
	if (rt2=pthread_create(&prod,NULL,thread2,NULL))
        	printf("Thread creation failed: %d\n", rt2);
			//printf("3\n");
	//clock_gettime(CLOCK_MONOTONIC, &stop_thread);//way3
	pthread_join(prod,NULL);
	pthread_join(cons,NULL);
	clock_gettime(CLOCK_MONOTONIC, &stop_thread);//way4
	pthread_cond_destroy(&cond1);
	pthread_cond_destroy(&cond2);
	pthread_mutex_destroy(&region);
	result_thread=timespecDiff(&stop_thread,&start_thread)/(MAX*2);
	result_overhead=createThreadCost();
	printf("----------part5 thread switch-----------\n");
	printf("current iteration number: %d\n", MAX);
	printf("Measured overhead of creating single thread: %llu ns\n", result_overhead); 
	printf("Threaded Run total count: %llu ns\n", result_thread);
	printf("Measured Threaded switch cost with overhead: %llu ns\n", result_thread-2*result_overhead); 
	printf("\n");
	//printf("Threaded Run total count: %llu\n", result_thread);
}
/*--------------------calculate cost of thread switch---------------------------------------*/




/*--------------------call all functions in the main func---------------------------------------*/
int main()
{
	struct timespec start;
	struct timespec stop;
	unsigned long long result= 0;
	funcion_Call_Cost();
	system_Call_Cost();
	process_Switch_Cost();
	thread_Switch_Cost();
	return 0;
}

/*
question1: if i change the MAX, the result will be very different. e.g. change to 1000 then process switch time will be 4000ns rather than 1000ns, is that normal?
question2: is thread time be 16ns is that normal? (MAX=100000), if change to 10000, it will be 100~400*/

