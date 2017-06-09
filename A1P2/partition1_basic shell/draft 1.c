#include <stdio.h>
#include <string.h>
#include <stdlib.h> // malloc
#include <dirent.h> //opendir, readdir
#include <unistd.h> // getcwd
#include <sys/wait.h>
#include <time.h>

char** parse(char * buffer) ;
/* BUILT-IN COMMANDS
     -- ls, ls -arg, echo, exit */

void prompt() {
    time_t rawtime ;
    struct tm *info ;
    const int bufsize = 10;
    char buffer[bufsize] ;

    time(&rawtime) ;
    info = localtime(&rawtime) ;

    strftime(buffer, bufsize, "%X", info) ;
    printf("%s# ", buffer) ;
    return  ;
}

/* echo with no arguments 
void echo(char** str) {
    int i = 1 ;
    while(str[i]!=NULL && strcmp(str[i], "\n")!=0) {
        if (i>1) printf (" ") ;
        printf("%s", str[i]) ;
        str[i] = '\0' ; // reset buffer
        i++ ;
    }
} */

/* ls 
void ls() {
    DIR *dir ;
    int length = sizeof(char)*100 ;
    struct dirent *entry ;
    
    char* pwd = (char*)malloc(length) ;
    //getcwd(pwd, length) ;
    
    while( getcwd(pwd, length)==NULL ){
        pwd = realloc(pwd, length*2) ;
    }     
    
    dir = opendir(pwd);
    while ((entry = readdir(dir)) != NULL) {
        printf("%s\n", entry->d_name);
    }
    
    closedir(dir);
} */

/* Returns a pointer to an array of strings containing tokens */
char** parse(char * buffer) {
        int i = 0;
        char** tokens = malloc(100*sizeof(char));
        char* tok = strtok(buffer, " \n") ;
        
        while (tok!='\0') {
            /* newline is also stored */
            tokens[i] = tok ;
            tok = strtok(NULL, " ") ;
            i++ ;
        }
        tokens[i] = NULL ;
        return tokens ;
}

int main( int argc, char **argv) {
    while (1) {
        size_t bufsize = 100 ;
        char* buffer ;
        //size_t length ;
        int childpid ;
    
        buffer = (char*)malloc(bufsize*sizeof(char)) ;
    
        if (buffer == NULL) {
            perror("Failed to allocate to buffer.\n") ;
            exit (1) ;
        }
    
        prompt() ;
        getline(&buffer, &bufsize, stdin) ; // stores size of read line
        
        char** command = parse(buffer) ;
        /* echo is a built-in command */
        if ( strcmp(command[0], "echo")==0 ) {
            echo(command) ;
        }
        else if ( strcmp(command[0], "exit")==0 ) {
            exit(0) ;
        }
        else {
            int childpid = fork() ;
            if (childpid == 0) { // child process
                /* execvp(command, args including command) */
                execvp(command[0], command) ;
                exit(0) ;
            }
            else if (childpid < 0) {
                perror("fork") ;
                exit(EXIT_FAILURE) ;
            }
            else { // parent process
                int status ;
                if (waitpid(childpid, &status, WUNTRACED) == -1) {
                    perror("waitpid") ;
                    exit(EXIT_FAILURE) ;
                }
                else {
                    continue ;
                }
            }
        }
    
    
        /* Deallocate memory allocated */
        free(buffer) ;
    }
     return 0 ;
}
