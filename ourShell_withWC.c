#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <ctype.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/shm.h>

static char *my_argv[100], *my_envp[100], *fileContents[100];
static char *search_path[10];
static char *buff;

void free_buff_and_cmd(char *buff, char* cmd){
	bzero(buff, 255);
	bzero(cmd, 34);
	buff = NULL;
	cmd = NULL;
	free(buff);
	free(cmd);
	buff = (char *)malloc(256 * sizeof(char));
	cmd = (char *)malloc(35 * sizeof(char));
}

void free_echo(char *buffer){
	bzero(buffer, 19);
	buffer = NULL;
	free(buffer);
	buffer = (char *)malloc(20 * sizeof(char));
}

void free_redirected_info(char *info){
	bzero(info, 9999);
	info = NULL;
	free(info);
	info = (char *)malloc(10000 * sizeof(char));
}

void free_echoFeed(char *buffer){
	bzero(buffer, 255);
	buffer = NULL;
	free(buffer);
	buffer = (char *)malloc(256 * sizeof(char));
}

/************************************************
	This command fills the my_envp array
	Lifted straight from ysh.c
*************************************************/
void copy_envp(char **envp)
{
    int index = 0;
    for(;envp[index] != NULL; index++) {
        my_envp[index] = (char *)
		malloc(sizeof(char) * (strlen(envp[index]) + 1));
        memcpy(my_envp[index], envp[index], strlen(envp[index]));
    }
}
/* ******************************************************************
*	This command fills the my_argv global array for the command.    *
*	so the my_argv[0] will be for example 'ls' and then every option*
*	after that is in my_argv[0+i], so ls -l -t -g -a would produce  *
*	my_argv[0] = ls, my_argv[1] = -l and so on.						*
*	It returns the number of arguments passed to the shell so that  *
*	you can use it as a limiter for looping through the argv array  *
*********************************************************************/
int fill_argv(char *buffer)
{
	int numArgs = 0;
	int i = 0, j = 0, k = 0;
	char c;


	i = 0;
	while(c != '\n'){
		c = buffer[i];
		if( c != ' ' && c != '\n'){
			my_argv[j][k] = c;
			k++;
		}
		else if( c == ' '){
			my_argv[j][k] = '\0';
			numArgs++;
			j++;
			k = 0;
		}
		i++;
	}
	numArgs++;

	i = 0;
	return numArgs;
}

void free_argv(){
	int index;
    for(index=0;my_argv[index]!=NULL;index++) {
        bzero(my_argv[index], strlen(my_argv[index])+1);
        my_argv[index] = NULL;
		free(my_argv[index]);
    }
	for(index=0; index < 100; index++) {
		my_argv[index] = (char *)malloc(20 * sizeof(char));
	}
}

void free_cmd(char *cmd) {
	bzero(cmd, 34);
	cmd = NULL;
	free(cmd);
	cmd = (char *)malloc(35 * sizeof(char));
}

void free_buff(char *buffer){
	bzero(buffer, 255);
	buffer = NULL;
	free(buffer);
	buffer = (char *)malloc(256 * sizeof(char));
}

void free_pipe(char *left, char *right){
	bzero(left, 49);
	bzero(right, 49);
	right = NULL;
	left = NULL;
	free(right);
	free(left);
	right = (char *)malloc(50 * sizeof(char));
	left = (char *)malloc(50 * sizeof(char));
}

void free_pipeFeed(char *buffer){
	bzero(buffer, 9999);
	buffer = NULL;
	free(buffer);
	buffer = (char *)malloc(10000 * sizeof(char));
}


int main(int argc, char *argv[], char *envp[])
{
	char *buffer = (char *)malloc(256*sizeof(char));
	char *cmd = (char *)malloc(35*sizeof(char));
	char *path_string = (char *)malloc(30*sizeof(char));
	int i = 0;
	int j = 0;
	int numArgs;
	int forkCheck, bgCheck, PID, parentPID;
	char *leftOfRedirect = (char*)malloc(50*sizeof(char));
	char *rightOfRedirect = (char*)malloc(50*sizeof(char));
	int fd[2];
	char stdoutBuffer[50];
	int true = 1;
	int copyTo, redirect = 0;
	int numRedirectArgs = 0;
	int redirectSymbolSpot;
	int rightRedirectLength;
	char *redirectedInformation = (char *)malloc(10000*sizeof(char));
	FILE *file;
	char c;
	int redirectArgVSpot;
	char *echoFunctionPrint = (char *)malloc(20 * sizeof(char));
	int echoParentProcessID;
	int echoProcessID;
	int echoFile;
	int bufferLength;
	int standardOut, standardIn;
	char *echoFeed = (char *)malloc( 256 * sizeof(char));
	int pipeSymbolSpot, pipeArgVSpot;
	int isPipe = 0;
	char *leftOfPipe = (char *)malloc(50 * sizeof(char));
	char *rightOfPipe = (char *)malloc(50 * sizeof(char));
	int rightPipeLength;
	char *pipeFeed = (char *)malloc(10000 * sizeof(char));	
	 
	i = 0;
	for( i; i < 100; i++ ){
		my_argv[i] = (char *)malloc(20*sizeof(char));
	}
	i=0;
	for( i; i < 100; i++ ){
		my_envp[i] = (char *)malloc(128*sizeof(char));
	}
	
	copy_envp(envp);
	
   	/****************************************************
		This spawns a fork to clear away the previous shell
		then prints [OUR_SHELL} to take the command
	*****************************************************/
	if(fork() == 0) {
        execve("/usr/bin/clear", argv, my_envp);
        exit(1);
    } else {
        wait(NULL);
    }
    fflush(stdout);
	
	while(true == 1)
	{//**********Everything from this point until the next 10 star comment executes until the shell encounters an error or you manually kill it
		redirect = 0;
		isPipe = 0;
		
		if( buffer == NULL ) { buffer = (char *)malloc(256*sizeof(char)); }
		if( cmd == NULL ) { cmd = (char *)malloc(35*sizeof(char)); }
		
		pipe(fd);//start the pipe before a fork so that parent and child can communicate
		printf("[OUR_SHELL ] ");
		fgets(buffer, 255, stdin);
		
		i = 0;
		if( strcmp(buffer, "\n\0") == 0 ) { continue; }
		for( i; i < 255; i++ ){//check to see if the full command string given to the shell has > OR < in it, if so redirection is true and remembers where > or < is in the command
			if( buffer[i] == '>' || buffer[i] == '<'){
				redirectSymbolSpot = i;
				redirect = 1;
			}
			if(buffer[i] == '|'){
				pipeSymbolSpot = i;
				isPipe = 1;
			}
		}
		
		numArgs = fill_argv(buffer);//Fills the my_argv array and sets numArgs to the number of arguments passed
		my_argv[numArgs] = NULL;//execve requires a NULL at the end of the arguments for argv
		i = 0;
		for( i; i < numArgs; i++ ){
			printf("%s ", my_argv[i]);
		}
		printf("\n");
		for( i=0; i < numArgs; i++){
			if( strcmp(my_argv[i], ">\0") == 0 || strcmp(my_argv[i], "<\0") == 0 ){
				redirectArgVSpot = i;
			}
			if( strcmp(my_argv[i], "|\0") == 0 ){
				pipeArgVSpot = i;
			}
		}
		
		
		strcpy(cmd, my_argv[0]);//sets the command to the first argument of argv, because execve needs (commandWithPath, arrayOfCommandAndOptions, my_envp)
		bgCheck = numArgs - 1;//sets the space to check for & for background processes
		
		parentPID = getpid();//gets the parent's id before the fork to check which process we're in
		forkCheck = fork();
		PID = getpid();//get the current processes PID to allow for control of separate processes
		i = 0;
		if( PID != parentPID){//what the child does
			printf("Child starting\n");
			if(redirect == 1) {
				strncpy(leftOfRedirect, buffer, redirectSymbolSpot);//copies from 0 until the </> symbol
				strcat(leftOfRedirect, "\0");//makes it a string
				printf("Program is redirecting: Redirect symbol at %d, left command of %s, redirect string at %d in my_argv[]\n", redirectSymbolSpot, leftOfRedirect, redirectArgVSpot);
				if( buffer[redirectSymbolSpot] == '>' ) {
					free_redirected_info(redirectedInformation);
					close(fd[0]);//close the read section of the pipe
					dup2(fd[1], 1);//duplicate stdout to the write end of the pipe and close stdout
					for( i=redirectArgVSpot; i < numArgs; i++ ) {
						bzero(my_argv[i], strlen(my_argv[i])+1);
						if( i == redirectArgVSpot ){
							my_argv[i] = NULL;
						}
					}
					if(cmd[0] == '.' ){//if the first character of the command is ., this means the path is the current path so you don't need to preface with /bin
						i = execve(cmd, my_argv, my_envp); 
						if( i < 0 ){
							printf("execve() error. Exiting.\n");
							exit(1);
						} 
					} else{//this path is taken if the command is one of the system commands i.e. ls, echo
						strcpy(path_string, "/bin/");//preface the command with bin
						strcat(path_string, cmd);
						strcpy(cmd, path_string);
						i = execve(cmd, my_argv, my_envp);//execute the command
						if( i < 0 ){
							printf("execve() error. Exiting.\n");
							exit(1);
						}
					}	
//					system(leftOfRedirect);//I wanted to see if I could get the pipe working so I didn't fill my_argv or use execve, but with system and the command it works fine.
					close(fd[1]);
				
				} else if( buffer[redirectSymbolSpot] == '<' ) { 
					close(fd[1]);//close the input side of the pipe
					dup2(fd[0], 0);//redirect the output side of the pipe to stdin
					for( i=redirectArgVSpot; i < numArgs; i++ ) {
						bzero(my_argv[i], strlen(my_argv[i])+1);
						my_argv[i] = NULL;
					}
					if(cmd[0] == '.' ){//if the first character of the command is ., this means the path is the current path so you don't need to preface with /bin
						i = execve(cmd, my_argv, my_envp); 
						if( i < 0 ){
							printf("execve() error. Exiting.\n");
							exit(1);
						} 
					} else{//this path is taken if the command is one of the system commands i.e. ls, echo
						strcpy(path_string, "/bin/");//preface the command with bin
						strcat(path_string, cmd);
						strcpy(cmd, path_string);
						i = execve(cmd, my_argv, my_envp);//execute the command
						if( i < 0 ){
							printf("execve() error. Exiting.\n");
							exit(1);
						}
					}	
//					system(leftOfRedirect);//call system() with leftOfRedirect, ie cat < files.txt, leftOfRedirect = cat
					
					close(fd[0]);//close the other side of the pipe because its good practice
				}
				
			} else if( isPipe == 1){//if there is a pipe symbol in the command line, i.e. ls | 
				strncpy(leftOfPipe, buffer, pipeSymbolSpot - 1);
				strcat(leftOfPipe, "\0");
				printf("Program is piping output of command %s to input of right of pipe\n", leftOfPipe);
				close(fd[0]);
				dup2(fd[1], 1);
				for(i=pipeArgVSpot; i < numArgs; i++){
					bzero(my_argv[i], 19);
					if( i == pipeArgVSpot ){
						my_argv[i] = NULL;
					}
				}
				
				if (strcmp(cmd, "wc\0") == 0)			// wc has a different path
					strcpy(path_string, "/usr/bin/");	//preface the command with usr/bin 
					
				else				
					strcpy(path_string, "/bin/");//preface the command with bin
					
				strcat(path_string, cmd);
				strcpy(cmd, path_string);
				i = execve(cmd, my_argv, my_envp);//execute the command
				if( i < 0 ){
					printf("execve() error. Exiting.\n");
					exit(1);
				}
				free_pipe(leftOfPipe, rightOfPipe);
			} else {
				if(cmd[0] == '.' ){//if the first character of the command is ., this means the path is the current path so you don't need to preface with /bin
					i = execve(cmd, my_argv, my_envp); 
					if( i < 0 ){
						printf("execve() error. Exiting.\n");
						exit(1);
					} 
				}  if( strcmp(cmd, "echo\0") == 0 ) {//if the command is echo, take this path
					if(my_argv[1][0] == '`'){//if the data to be echo'd is in back quotes, take this path to execute the command in back quotes and feed the executed program stdout to the pipe for the parent to read and echo
						close(fd[0]);
						bufferLength = strlen(buffer);
						strncpy(echoFunctionPrint, buffer + 6, bufferLength );
						strcat(echoFunctionPrint, "\0");
						bufferLength = strlen(echoFunctionPrint);
						echoFunctionPrint[bufferLength - 2] = '\0';
						printf("Echo called with %s\n", echoFunctionPrint);
						dup2(fd[1], 1);
						free_argv();
						numArgs = fill_argv(echoFunctionPrint);
						my_argv[numArgs] = NULL;
						strcpy(cmd, my_argv[0]);
						strcpy(path_string, "/bin/");
						strcat(path_string, cmd);
						strcpy(cmd, path_string);	
						i = execve(cmd, my_argv, my_envp);//execute the command
						if( i < 0 ){
							printf("execve() error. Exiting.\n");
							exit(1);
						}						
					
					}  else if( my_argv[1][0] == '\'' || my_argv[1][0] == '"') {//if the echo data is either single or double quotes, remove the quotes and echo simply the message inside the quotes
						free_echoFeed(echoFeed);
						bufferLength = strlen(buffer);
						strncpy(echoFeed, buffer + 6, bufferLength);
						strcat(echoFeed, "\0");
						bufferLength = strlen(echoFeed);
						echoFeed[bufferLength - 2] = '\0';
						printf("%s", echoFeed);
						printf("\n");
					
					} else {						
						strcpy(path_string, "/bin/");//preface the command with bin
						strcat(path_string, cmd);
						strcpy(cmd, path_string);
						i = execve(cmd, my_argv, my_envp);//execute the command
						if( i < 0 ){
							printf("execve() error. Exiting.\n");
							exit(1);
						}
					}				
				} else if (strcmp(cmd, "wc\0") == 0) {		// we want to use the wc command
					strcpy(path_string, "/usr/bin/");//preface the command with usr/bin (according to the which command the path for 
					strcat(path_string, cmd);        //		wc is /usr/bin)
					strcpy(cmd, path_string);
					
					for (i = numArgs; i < 100; i++)
						my_argv[i] = NULL;
						
					i = execve(cmd, my_argv, my_envp);//execute the command
					
					if( i < 0 ){
						printf("execve() error. Exiting.\n");
						exit(1);
					}	
							
				} else {//this path is taken if the command is one of the system commands i.e. ls, echo
					strcpy(path_string, "/bin/");//preface the command with bin
					strcat(path_string, cmd);
					strcpy(cmd, path_string);
					i = execve(cmd, my_argv, my_envp);//execute the command
					if( i < 0 ){
						printf("execve() error. Exiting.\n");
						exit(1);
					}
				}
			}
			exit(0);
		} 
		else if( PID == parentPID && strcmp(my_argv[bgCheck], "&\0") == 0){//this path is taken if the & symbol is at the end of the command signaling a background process
			printf("Parent returning\n");
			free_argv();
			close(fd[0]); close(fd[1]);
		} 

		else if( PID == parentPID && redirect == 1){//parent needs to read the output of the redirected command so that it can write to a file
			rightRedirectLength = strlen(buffer) - redirectSymbolSpot;//find the length of the filename
			strncpy(rightOfRedirect, buffer + redirectSymbolSpot + 2, rightRedirectLength);//copy from the </> symbol to the end of the string
			if( rightOfRedirect[rightRedirectLength - 3] == '\n' ){ rightOfRedirect[rightRedirectLength - 3] = '\0'; }//if theres a newline character in that string replace it with the string terminator
			printf("File name for redirection: %s\n", rightOfRedirect);
			if(buffer[redirectSymbolSpot] == '>' ){
				free_redirected_info(redirectedInformation);
				close(fd[1]);//close the write portion of the pipe
				wait(NULL);//wait for the child to write to the pipe
				i = read(fd[0], redirectedInformation, 10000);//read from the input side of the pipe
				close(fd[0]);//close the input side of the pipe
//				printf("Child wrote: %s\n", redirectedInformation);
				file = fopen(rightOfRedirect, "w");
				fprintf(file, "%s", redirectedInformation);
				fclose(file);
			}
			else if( buffer[redirectSymbolSpot] == '<'){
				free_redirected_info(redirectedInformation);//make sure redirectedInformation is empty
				close(fd[0]);//close the output side of the pipe
				file = fopen(rightOfRedirect, "r");//open the filename, which is always right of the redirect symbol, for reading. So in 'cat < files.txt' rightOfRedirect = files.txt
				i = 0;
				while( fscanf(file, "%c", &c ) != EOF) {//go through the characters in the opened file and build the redirected information needed
					redirectedInformation[i] = c;
					i++;
				}
				fclose(file);//close the file
				strcat(redirectedInformation, "\0");//make the redirected information a string
				//printf("Parent wrote:\n%s\n", redirectedInformation);//debugging command
				i = write(fd[1], redirectedInformation, 10000);//write the redirected information to the input side of the pipe
				close(fd[1]);//close the pipe
				wait(NULL);//wait for the child to be done
			}
			free_argv();
		} else if( PID == parentPID && strcmp(cmd, "echo\0") == 0 && my_argv[1][0] == '`') {//if there is an echo call with a function in back quotes, this branch is taken by the parent
			free_redirected_info(redirectedInformation);
			close(fd[1]);
			wait(NULL);
			i = read(fd[0], redirectedInformation, 10000);
			close(fd[0]);
			for(i=0; i < strlen(redirectedInformation); i++){
				if(redirectedInformation[i] == '\n'){
					printf(" ");
				} else {
					printf("%c", redirectedInformation[i]);
				}
			}
			printf("\n");
			free_argv();
			free_echo(echoFunctionPrint);
		} else if( PID == parentPID && strcmp(cmd, "echo\0") == 0 && (my_argv[1][0] == '\'' || my_argv[1][0] == '"') ){//if there is an echo call with single or double quotes, this path is taken by the parent
			wait(NULL);
			free_echo(echoFunctionPrint);
			free_argv();
		} else if( PID == parentPID && isPipe == 1){
			free_redirected_info(redirectedInformation);
			rightPipeLength = strlen(buffer) - pipeSymbolSpot;
			strncpy(rightOfPipe, buffer + pipeSymbolSpot + 2, rightPipeLength);
			if( rightOfPipe[rightPipeLength - 3] == '\n' ){ rightOfPipe[rightPipeLength - 3] = '\0'; }//if theres a newline character in that string replace it with the string terminator
//			printf("Parent is waiting for child to finish, then using child's program's output as input for %s\n", rightOfPipe);
			close(fd[1]);
			wait(NULL);
			i = read(fd[0], redirectedInformation, 10000);
			close(fd[0]);
//			printf("Redirected output from child was: %s\n", redirectedInformation);
			pipe(fd);
			forkCheck = fork();
			if( forkCheck < 0 ) { printf("Fork failed, exiting.\n"); }
			PID = getpid();
			if(PID == parentPID){
				close(fd[0]);
				i = write(fd[1], redirectedInformation, 10000);
				close(fd[1]);
				wait(NULL);
				free_pipe(leftOfPipe, rightOfPipe);
				free_argv();
				
			} else if( PID != parentPID ){
				free_redirected_info(redirectedInformation);
				close(fd[1]);
				dup2(fd[0], 0);
//				printf("Right command of pipe is: %s, cmd is: %s\n", rightOfPipe, cmd);
				strcpy(my_argv[0], my_argv[pipeArgVSpot + 1]);
				for( i = 1; i < numArgs; i++ ){
					if(i == 1){ 
						bzero(my_argv[i], 19);
						my_argv[i] = NULL;
					} else{
						bzero(my_argv[i], 19);
					}
				}
				strcpy(cmd, my_argv[0]);
				
				if (strcmp(cmd, "wc\0") == 0)			// wc has a different path
					strcpy(path_string, "/usr/bin/");	//preface the command with usr/bin 
					
				else				
					strcpy(path_string, "/bin/");//preface the command with bin
				
				//strcpy(path_string, "/bin/");//preface the command with bin
				strcat(path_string, cmd);
				strcpy(cmd, path_string);
				i = execve(cmd, my_argv, my_envp);//execute the command
				if( i < 0 ){
					printf("execve() error. Exiting.\n");
					exit(1);
				}
				close(fd[0]);
				exit(0);
			}		
		} else if (PID == parentPID && (strcmp(cmd, "wc\0") == 0)) { // we used the wc command		
			wait(NULL);			
			free_argv();	
			close(fd[0]); close(fd[1]);						
					
		 } else {//if the command is not a background process, wait for the ran process to be killed.
			printf("Parent waiting\n");
			wait(NULL);
			free_argv();
			close(fd[0]); close(fd[1]);
		}
		
		free_buff_and_cmd(buffer, cmd);
	}	//**********this encompasses the shell's query responses and commands	
	
	return 0;
} 