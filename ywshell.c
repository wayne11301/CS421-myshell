#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>


#define MAX_LINE 80 /* 80 chars per line, per command, should be enough. */

#define BUFFER_SIZE 50
char *args[(MAX_LINE/2)+1];  /* command line (of 80) has max of 40 arguments */
char *history[10]; /*for history array*/
char historytoken[80]; /* for history array token*/
char numchar[10];
int k = 1, command_count=0, l=0;

static char buffer[BUFFER_SIZE];
char inputBuffer[MAX_LINE];      /* buffer to hold the command entered */
int background;              /* equals 1 if a command is followed by '&' */
int status;


/**
 * setup() reads in the next command line, separating it into distinct tokens
 * using whitespace as delimiters. setup() sets the args parameter as a
 * null-terminated string.
 */

void setup(char inputBuffer[], char *args[],int *background, int repeat,int temp)
{
    int length, /* # of characters in the command line */
        i,      /* loop index for accessing inputBuffer array */
        start,  /* index where beginning of next command parameter is */
        ct;     /* index of where to place the next parameter into args[] */
   
    ct = 0;

    /* read what the user enters on the command line if not repeating (r command)*/
    /* if executing r x command, copy command from history array to run*/
    if(repeat == 0){
    	length = read(STDIN_FILENO, inputBuffer, MAX_LINE); 
	}else{
		if(temp==-1){
			length = strlen(history[command_count-2])+1;
			strcpy(inputBuffer, history[command_count-2]);
			strcat(inputBuffer,"\n");
		}else{
			length = strlen(history[temp])+1;
			strcpy(inputBuffer, history[temp]);
			strcat(inputBuffer,"\n");
		}

	}
    start = -1;
    if (length == 0)
        exit(0);            /* ^d was entered, end of user command stream */
    if (length < 0){
        perror("error reading the command");
        exit(-1);           /* terminate with error code of -1 */
    }

    /* examine every character in the inputBuffer */
    for (i=0;i<length;i++) {
        switch (inputBuffer[i]){
          case ' ':
          case '\t' :               /* argument separators */
            if(start != -1){
                    args[ct] = &inputBuffer[start];    /* set up pointer */
                ct++;
            }
            inputBuffer[i] = '\0'; /* add a null char; make a C string */
            start = -1;
            break;
          case '\n':                 /* should be the final char examined */
            if (start != -1){
                    args[ct] = &inputBuffer[start];    
                ct++;
            }
                inputBuffer[i] = '\0';
                args[ct] = NULL; /* no more arguments to this command */
            break;
          default :             /* some other character */
            if (start == -1)
                start = i;
            if (inputBuffer[i] == '&'){
                *background  = 1;
                start = -1;
                inputBuffer[i] = '\0';
            }
          }
     }   
     args[ct] = NULL; /* just in case the input line was > 80 */
}

/* string combine function*/
char* concatenation(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1) + strlen(s2) + 1);
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

/* the signal handler function */
void handle_SIGTSTP() {
      	if(command_count<=0){
      		write(STDOUT_FILENO,"no command in history array\n",30);
      		return;
      	}
      	int counter=1;
      	int number;
      	if(command_count<=10){
      		number = counter;
      	}else{
      		number = command_count-9;
      	}
      	/* print each line of previous commands*/
      	while(history[counter-1]){
      		write(STDOUT_FILENO,"\n",2);
      		sprintf(numchar,"%d. ",number);
		write(STDOUT_FILENO,numchar,strlen(numchar));
      		write(STDOUT_FILENO,history[counter-1],strlen(history[counter-1]));
      		counter++;
      		number++;
      	}
      	write(STDOUT_FILENO,"\n",2);
      	sprintf(buffer,"ywshell[%d]: \n",command_count);
      	write(STDOUT_FILENO,buffer,15);
      	
      	return;
}
 

/* for yell command*/
void to_upper(char* temp)
{
	char* tokenstring;
	tokenstring = strtok(temp,":");

	char *token = tokenstring;
	while (*token) {
		*token = toupper((unsigned char) *token);
		token++;
	}
}


int main(void)
{

	pid_t childpid,parentpid;
	int i = 1,j=0,repeat=0,temp=-1;
	char repeat_token[20];
	char* repeat_out[1];
	/* set up the signal handler */
	
	struct sigaction handler;
	handler.sa_handler = handle_SIGTSTP;
	handler.sa_flags = SA_RESTART;
	sigaction(SIGTSTP, &handler, NULL);
	
	printf("Welcome to ywshell, my PID is %d \n", getpid());
	
	while (1){            /* Program terminates normally inside setup */
		
		background = 0;
		printf("ywshell[%d]: \n", command_count);
		setup(inputBuffer,args,&background,repeat,temp);       /* get next command */
		
		/* for r command output*/
		if(repeat==1){
			strcpy(historytoken,args[k-1]);
			repeat_out[0]=concatenation(historytoken, "");
			while(args[k]){
				strcpy(historytoken,repeat_out[0]);
				repeat_out[0] = concatenation(concatenation(historytoken," "),args[k]);
				k++;
			}
			printf("Now executing command: %s\n", repeat_out[0]);
			k=1;
		}
		
		
		
		if(args[0]==NULL){
			printf("No input detected, try again\n");
			continue;
		}
		command_count++;
		temp=-1;
		
		/*history array, if size exceed 10, push every element forward and insert the last recond to 10th place*/
		if(repeat != 1){
		if(command_count<=10){
			strcpy(historytoken,args[k-1]);
			history[command_count-1]=concatenation(historytoken, "");
			while(args[k]){
				strcpy(historytoken,history[command_count-1]);
				history[command_count-1]=concatenation(concatenation(historytoken," "),args[k]);
				k++;
				}
				k=1;
		}
		else{
			while(k<10){
				history[k-1]=history[k];
				k++;
			}
			k=1;
			strcpy(historytoken,args[k-1]);
			history[9]=concatenation(historytoken, "");
			while(args[k]){
				strcpy(historytoken,history[9]);
				history[9]=concatenation(concatenation(historytoken," "),args[k]);
				k++;
			}
			k=1;
		}
		}
		repeat=0;
		
		/* command handler*/
		if(strcmp(args[0],"yell")==0){
			while(args[i]){
				to_upper(args[i]);
				printf("%s ",args[i]);
				i++;
			}
			i=1;
			printf("\n");
		}else if(strcmp(args[0],"exit")==0){
			system("ps -o pid,ppid,pcpu,pmem,etime,user,command");
			exit(0);
		}else if(strcmp(args[0],"r")==0){
			if(args[1]==NULL){
				
				repeat = 1;
				strcpy(historytoken,history[command_count-2]);
				history[command_count-1]=concatenation(historytoken, "");
				continue;
				
			}else{
				repeat = 1;
				if(command_count<10){
					temp = command_count-1;
				}else{
					temp = atoi(args[1]);
					temp = command_count - temp;
					temp = 9-temp;
				}
				
				strcpy(historytoken,history[temp]);
				history[temp]=concatenation(historytoken, "");

				continue;
				
			
			}
		
		/* default system call*/
		}else{
			childpid = fork();
			if(childpid==0){
				execvp(args[0],args);
				exit(0);
			}else{
				if(background == 0){
					printf("[Child pid = %d, background = FALSE] \n", childpid);
					childpid = waitpid(childpid, &status, WCONTINUED);
					printf("Child process complete \n");
				}else {
					printf("[Child pid = %d, background = TRUE] \n", childpid);
				}
				
			}
	
		}
	
    }
}
