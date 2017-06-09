//AUTHOR:		JIANGLIN FU 
//STUDENT ID:		*** 
//COMPUTING ID:		***

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
	return 0;
}

//------------------------token the input command------------------------//
char** tokenize(char buffer[]) {
	int i = 0;
	char** token = malloc(200*sizeof(char));
	char* temp = strtok(buffer,"  '\t\n\a\r") ;

	while (temp!=NULL) {
		token[i] = temp;
		temp = strtok(NULL,"  '\t\n\a\r");
		i++;}
	return token;
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
	for (int i = start; i<= end; i++)
		printf("%d: %s", i, hisMatrix[i]);
	//printf("inputLineIndex: %d\n", inputLineIndex);
	//printf("start: %d\n", start);
	//printf("end:   %d\n", end);
	/*for (int i = start; i <= end; i++){
		command1 = tokenize(hisMatrix[i]);
		//printf("%d: ", i);
		for (int j=0; command1[j] != NULL; j++){
			//-------------------WHYYYYYYYY's NOT Working！！！------------------//
			printf("%d : %s\n", j, command1[j]); //j won't go up! don't know why
			//printf("%s\n", command1[j]);
			//printf("lalala\n");
			printf("j: %d\n", j);}
		//printf("second i: %d: ", i);
	}*/
	
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

		//Ctrl+D on Unixy platforms is EOF. 
		//If you are getting each char, you can check to see if you got EOF.
		//EOF encountered, ctrl d pressed
		if ( fgets(buffer, 200, stdin) == NULL )
		{
			printf("CTRL-D countered, BYE~\n");
			exit(0);
		}
		else{
			if (strcmp(buffer, "\n")!=0){
				//add every input command into the history matrix
				inputLineIndex++;
				strcpy(hisMatrix[inputLineIndex-1], buffer);

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

				if (command[0] == NULL)
					continue;
				//here must be "else if"
				//otherwise the internal command will run second time at "execvp(command[0], command)"
				else if (strcmp(command[0], "exit")==0){
					printf("byebye\n");
					exit(0);}
				else if(strcmp(command[0], "pwd")==0){
					char getDir[200];
					char* curDir = getcwd(getDir, 200);
					printf("%s\n", curDir);}
				else if (strcmp(command[0], "cd")==0)
					chdir(command[1]);
				else if (strcmp(command[0], "history") == 0)
					tenHistory(inputLineIndex, hisMatrix);
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
			}//for if (strcmp(buffer, "\n")!=0)
		}//for else()
	}//for while(true)
	return 0 ;
}

