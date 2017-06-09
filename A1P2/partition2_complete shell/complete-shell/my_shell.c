//AUTHOR:		JIANGLIN FU 
//STUDENT ID:		888 
//COMPUTING ID:		888
//Reference: http://www.cs.loyola.edu/~jglenn/702/S2005/Examples/dup2.html
//	     http://stackoverflow.com/questions/8389033/implementation-of-multiple-pipes-in-c
//pipe read/stdin:   0  
//pipe write/stdout: 1

#include <string.h>	// for strtok, strcpy, strcmp
#include <stdlib.h>	// for malloc // for exit()
#include <errno.h>
#include <stdio.h> 	// for stdin/out, printf
#include <stdbool.h>	// for while(true)
#include <sys/types.h> 	// for pid_t
#include <unistd.h>	// for chdir, getcwd, execvp, char*.....
#include <sys/wait.h>	// for waitpid()


int prompt() {
	char buffer[200];
	char* curDir = getcwd(buffer, 200);
	printf("%s >>", curDir);
	//printf("arlene's shell>>");
	return 0;
}

     
//------------------------token the input command------------------------//
char** tokenize(char buffer[]) { //return ["cat", "README", "|", "pg"]
	int i = 0;
	char** token = malloc(200*sizeof(char));
	//char *token[100];
	char* temp = strtok(buffer,"  '\t\n\a\r") ;

	while (temp!=NULL) { //store new line
		token[i] = temp;
		temp = strtok(NULL,"  '\t\n\a\r");
		i++;
	}
	token[i] = NULL;
	return token;
}

//------------------------saperate token by piping------------------------//
char** parsePipe(char** command, int pipeIndex)
{
    char **partialPipe = malloc(sizeof(char *) *100);
    int i, j = 0;
    int finishedPipe = 0;
    for (i = 0;command[i] != NULL; i++)
    {
        if (strcmp(command[i], "|") == 0)
            finishedPipe++;
        else if (finishedPipe == pipeIndex){
            partialPipe[j] = command[i];
 	    j++;}
    }
    partialPipe[j] = NULL;//make last element be NULL
    return partialPipe;
}


//tenHistory(inputLineIndex, hisMatrix);
//------------------------Print the history matrix------------------------//
void tenHistory(int inputLineIndex, char hisMatrix[200][200]){
	int start=0;
	int end =0;

	if (inputLineIndex < 10)
		end = inputLineIndex - 1;
	else{
		start = inputLineIndex - 10;
		end = inputLineIndex - 1;}

	char** command1 = malloc(200*sizeof(char));
	for (int i = start; i<= end; i++){
		printf("%d: %s", i, hisMatrix[i]);
	}
	free (command1);
}


int main() {
	//basic signals handling (disable)
	signal (SIGINT, SIG_IGN);  //ctrl-c
	signal (SIGTSTP, SIG_IGN); //ctrl-z
	signal (SIGQUIT, SIG_IGN); //ctrl-'\'

	//for history matrix info update		
	int inputLineIndex=0;
	char hisMatrix[200][200];

	//pipe initializing 
	int pipeEnd[20]; //(assume has at most 10 pipes 11 commands in concatenation))
	//int pipeEnd[4];
	//pipe(pipeEnd); // sets up 1st pipeEnd
	//pipe(pipeEnd + 2); // sets up 2nd pipeEnd

	while (true) {		
		pid_t childPid; 

		//for type_prompt, initialization
		prompt();

		char buffer[200];
		char** command;

		//Ctrl+D on Unixy platforms is EOF. 
		//If you are getting each char, you can check to see if you got EOF.
		//EOF encountered, ctrl d pressed
		if ( fgets(buffer, 200, stdin) == NULL )
		{
			printf("CTRL-D countered, BYE~\n");
			exit(0);
		}
		else{ //sucessfully read input
			//add every input command into the history matrix
			inputLineIndex++; 
			strcpy(hisMatrix[inputLineIndex-1], buffer);
			command = tokenize(buffer);

			//count the number of pipe that needed
			int count = 0;
			for (int i=0; command[i]!=NULL; i++) 
				if (strcmp(command[i], "|")==0) 	
					count++;
			for (int j=0; j<count; j++) //we now have e.g. 4 fds: //pipeEnd[0] = read = pipeEnd[2] //pipeEnd[1] = write = pipeEnd[3]
				if (pipe(pipeEnd+2*j)<0){
					perror("Pipe Failure: ");
            				exit(EXIT_FAILURE);}

			if (command[0] == NULL) 
				continue;
			//here must be "else if"
			//otherwise internal command will run 2nd time at "execvp(command[0], command)"
			//--------------need to decide to consider in pipe or not----------//
			//--------------currently make them for commands dont need to associate with system call----------//
			else if (strcmp(command[0], "exit")==0){
				printf("byebye\n"); 
				exit(0);}
			else if(strcmp(command[0], "pwd")==0){ 
				char buffer[200]; 
				char* curDir = getcwd(buffer, 200); 
				printf("%s\n", curDir);}
			else if (strcmp(command[0], "cd")==0) 
				chdir(command[1]);
			else if (strcmp(command[0], "history") == 0) 
				tenHistory(inputLineIndex, hisMatrix);
			//--------------need to decide to consider in pipe or not----------//

			else{ 	int i,j;
				for (i=0; i<=count; i++){
					//int IndexStart = 0; int IndexEnd = 0;
					childPid = fork(); 
					char** commandPipe;
					commandPipe = parsePipe(command, i);
					//tokenize na ge new command1
					//wo xian ba kuang jia xie hao
					if (childPid < 0) {
							perror("fork() failed: ");printf("Errno: %d\n", errno);exit(EXIT_FAILURE);}
					else if (childPid == 0){
						if (count == 0){
						   	execvp(command[0], command);
							for (j=0;j<2*count; j++) close (pipeEnd[j]);						    
							exit(0); } 
						else if (i==0){
							if (dup2(pipeEnd[1],STDOUT_FILENO)<0)
								{perror("dup2"); exit(EXIT_FAILURE);}
							for (j=0;j<2*count; j++) close (pipeEnd[j]);	
							/*int temp; IndexStart = IndexEnd+2;
							for (; command[IndexEnd+1]!=NULL; IndexEnd++) 
								if (strcmp(command[IndexEnd], "|")==0) 
									temp = IndexEnd -1;
							IndexEnd = temp;
							char *commandPipe[IndexEnd-IndexStart+1];
							for (int l=0;l<IndexEnd-IndexStart+1;l++) 
								commandPipe[l]=command[IndexStart+l];*/
							execvp(commandPipe[0],commandPipe);
							exit(0); }
						else if (i>0 && i<count){ // command between pipes
							if (dup2(pipeEnd[(i-1)*2],STDIN_FILENO)<0)
								{perror("dup2"); exit(EXIT_FAILURE);}
							if (dup2(pipeEnd[2*i+1],STDOUT_FILENO)<0)
								{perror("dup2"); exit(EXIT_FAILURE);} 
							for (j=0;j<2*count; j++) close (pipeEnd[j]);	
							/*int temp; IndexStart = IndexEnd+2;
							for (; command[IndexEnd+1]!=NULL; IndexEnd++) 
								if (strcmp(command[IndexEnd], "|")==0) 
									temp = IndexEnd -1;
							IndexEnd = temp;
							char *commandPipe[IndexEnd-IndexStart+1];
							for (int l=0;l<IndexEnd-IndexStart+1;l++) 
								commandPipe[l]=command[IndexStart+l];*/
							execvp(commandPipe[0],commandPipe);
							exit(0); }
						else if (i==count){ // last command 
							if (dup2(pipeEnd[(i-1)*2],STDIN_FILENO)<0)
								{perror("dup2"); exit(EXIT_FAILURE);}
							for (j=0;j<2*count; j++) close (pipeEnd[j]);
							/*int temp; IndexStart = IndexEnd+2;
							for (; command[IndexEnd+1]!=NULL; IndexEnd++) 
								if (strcmp(command[IndexEnd], "|")==0) 
									temp = IndexEnd -1;
							IndexEnd = temp;
							char *commandPipe[IndexEnd-IndexStart+1];
							for (int l=0;l<IndexEnd-IndexStart+1;l++) 
								commandPipe[l]=command[IndexStart+l];*/	
							execvp(commandPipe[0],commandPipe);
							exit(0);}
						else continue;} //for else if (childPid == 0)
					free (commandPipe);
				}//for for (i=0; i<=count; i++)
				
				if (childPid >0) { // parent process
					int status, k;
					for (j=0;j<2*count; j++) close (pipeEnd[j]);
					for (k=0; k<=count; k++)
						waitpid(childPid, &status, WUNTRACED);
						//will generate errors but dont know why
						//if (waitpid(childPid, &status, WUNTRACED) == -1)
						//{perror("waitpid() failed: ");printf("Errno: %d\n", errno);exit(EXIT_FAILURE);}}
				     }//for if (childPid >0)
			     }//for else
		}//for else()
	free (command);
	}//for while(true)
return 0;
}

