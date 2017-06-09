//AUTHOR:		Jianglin (Arlene) FU 
//EDIT DATE:		2017-02-03, 21:24
//STUDENT ID:		*** 
//COMPUTING ID:		***

#include <stdio.h>
#include <string.h>
#include <stdlib.h> // for malloc
#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <signal.h>
#define cmd_length 200

int prompt() {
	/*char buffer[cmd_length];
	char* curDir = getcwd(buffer, cmd_length);
	printf("%s >>", curDir);*/
	printf("arlene's shell~");
	return 0;
}

/* Returns a pointer to an array of strings containing tokens */
char** tokenize (char buffer[]) {
	int i = 0;
	char** token = malloc(200*sizeof(char));
	//char *token[100];
	char* temp = strtok(buffer,"  '\t\n\a\r") ;

	while (temp!=NULL) { //store new line
		//-------------------correct version1
		token[i] = temp;
		temp = strtok(NULL,"  '\t\n\a\r");
		/*printf("%d %s \n", i, token[i]);*/
		i++;
		//-------------------correct version2
		//token[i++] = temp;
		//token[i] = NULL;
		//temp = strtok(NULL," '\t") ;
	}
	//printf("test print");
	//token[i] = NULL;
	//for (int j=0; j<i ;j++){
	//	printf("%d %s", j, token[j]);
	//	printf("\n");}
	token[i] = NULL;
	return token;
}



int main() {
	//basic signals handling (disable)
	signal (SIGINT, SIG_IGN);  //ctrl-c
	signal (SIGTSTP, SIG_IGN); //ctrl-z
	signal (SIGQUIT, SIG_IGN); //ctrl-'\'

	//for history matrix info update		
	int inputLineIndex=0;
	char hisMatrix[200][200];

	while (true) {		
		pid_t childPid; 

		//for type_prompt, initialization
		prompt();

		

		char buffer[200];
		char** command;
		fgets(buffer, 200, stdin);

		if (strcmp(buffer, "\n")!=0){
			command = tokenize(buffer);
			/*//-----------------testing1----------------//
			printf("I got here\n");
			for (int j=0;command[j] != NULL ;j++)
				printf("%d %s\n", j, command[j]);
			printf("finish print testing\n");
			//-----------------testing1----------------//
			printf("%s\n",command[0]); 
//wo zhi dao le! zhe li zhi hou you ge NULL!!!!!!! ("\n")
//er qie! zhe shi yin wei ru guo zhi da yi ge word, na token[i++] jiu shi \n le!
			bool a = strcmp(command[0], "exit");
			printf("%d\n", a);
			//-----------------testing2----------------//*/
			if (strcmp(command[0], "exit")==0){
				printf("byebye\n");
				exit(0);}
			if(strcmp(command[0], "pwd")==0){
				char buffer[cmd_length];
				char* curDir = getcwd(buffer, cmd_length);
				//printf("to print the addr\n");
				printf("%s\n", curDir);}
				//printf("finished printing\n");
				//ni kan ma, di er bian chu lai de pwd address bu shi zhe li chu lai de 
			if (strcmp(command[0], "cd")==0)
				chdir(command[1]);
			//if (strcmp(command[0], "history") == 0)
				//10History(inputLineIndex, history);
			else{
				childPid = fork();
				if (childPid < 0) {
					perror("fork() failed: ");
					printf("Errno: %d\n", errno);
					exit(EXIT_FAILURE);}
				else if (childPid == 0) { // child process
				    	//execvp(command, args including command) 
					execvp(command[0], command);
					exit(0);}
				else { // parent process
					int status;
					if (waitpid(childPid, &status, WUNTRACED) == -1) {
						perror("waitpid() failed: ");
						printf("Errno: %d\n", errno);
						exit(EXIT_FAILURE);}
				     }
			     }
		   }
	  }
     return 0 ;
}




//version2
/*	while (true) {		
		pid_t childPid; 

		prompt();

		char buffer[200];
		fgets(buffer, 200, stdin);

		char** command;
		if (strcmp(buffer, "\n")!=0){
			command = tokenize(buffer);
			for (int j=0;command[j] != NULL ;j++){
				printf("%d %s", j, command[j]);
				printf("\n");}
			if (strcmp(command[0], "exit")==0){
				printf("byebye\n");
				exit(0);}
			if(strcmp(command[0], "pwd")==0){
				char buffer[cmd_length];
				char* curDir = getcwd(buffer, cmd_length);
				printf("%s", curDir);}
			else{
				childPid = fork();
				if (childPid < 0) {
					perror("fork() failed: ");
					printf("Errno: %d\n", errno);
					exit(EXIT_FAILURE);}
				else if (childPid == 0) { // child process
				    	//execvp(command, args including command) 
					execvp(command[0], command);
					exit(0);}
				else { // parent process
					int status;
					if (waitpid(childPid, &status, WUNTRACED) == -1) {
						perror("waitpid() failed: ");
						printf("Errno: %d\n", errno);
						exit(EXIT_FAILURE);}
				     }
			     }
		   }
	  }
*/



//version 1
		/*if (strcmp(buffer, "\n")!=0) //if buffer contains sth...
		{
		    char** command = tokenize(buffer);
		    if (strcmp(command[0], "exit")==0){
			printf("byebye\n");
			exit(0);}
		    else 
			{
			childPid = fork();
			if (childPid < 0) {
			    perror("fork() failed: ");
			    printf("Errno: %d\n", errno);
			    exit(EXIT_FAILURE);}
			else if (childPid == 0) { // child process
			    //execvp(command, args including command) 
			    execvp(command[0], command);
			    exit(0);}
			else 
			    { // parent process
			    int status;
			    if (waitpid(childPid, &status, WUNTRACED) == -1) {
				perror("waitpid() failed: ");
				printf("Errno: %d\n", errno);
				exit(EXIT_FAILURE);}
			     }
		    	}
		}*/	
