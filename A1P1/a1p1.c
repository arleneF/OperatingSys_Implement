//ssh -p ** ***@csil-***
//copy a file from the local server to the remote one:
//scp FILENAME user@remote_server:/remote/path/FILENAME
//scp -P24 Makefile ***@csil-***:/home/***


//AUTHOR:       JIANGLIN (ARLENE) FU
//C＊M＊P＊T＊3＊0＊0-AS1 PART1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
	FILE *fp1, *fp2, *fp3, *fp4;
	//*fopen(const char *filename, const char *mode)
	//----------------------READ MODEL NAME------------------------------//
	char localBuffer1[100];
	char *buffer_location1;
	fp1 = fopen ("/proc/cpuinfo", "r"); 
	int i =0, j=0;
	while (i<5){
		fgets(localBuffer1, 100, fp1);
		i++;}
	buffer_location1 = localBuffer1;
	//After find the line recorded model name, try to eliminate the title and only return the info
	while (j<100 && (localBuffer1[j] != ':') )
	{ 
		buffer_location1++;
		j++;
	}
	buffer_location1++;
	printf("CPU Model Name:        %s", buffer_location1);
	fclose(fp1);

	//----------------------READ KERNEL VERSION------------------------------//
	char localBuffer2[200];
	fp2 = fopen ("/proc/version", "r");
	fgets(localBuffer2, 200, fp2);
	printf("Kernel Version:         %s", localBuffer2);
	fclose(fp2);

	//----------------------READ MEM TOTAL------------------------------//
	char localBuffer_1[100], localBuffer3_2[100], localBuffer3_3[100];
	fp3 = fopen ("/proc/meminfo", "r");
	fscanf(fp3, "%s %s %s", localBuffer_1, localBuffer3_2, localBuffer3_3);
	printf("Main Memory Total:      %s %s\n", localBuffer3_2, localBuffer3_3);
	fclose(fp3);

	//----------------------READ UPTIME------------------------------//
	//2 uptime: the uptime of the system, and the amount of time spent in idle process
	//we choose first uptime to print
	//fgets reads to a newline.  fscanf only reads up to whitespace.
	char uptime1[10];
	fp4 = fopen ("/proc/uptime", "r");
  	fscanf(fp4, "%s", uptime1);
	printf("Uptime:                 %s Seconds\n", uptime1);
	fclose(fp4);

	return 0;}
