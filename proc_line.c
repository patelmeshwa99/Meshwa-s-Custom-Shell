#include "smallsh.h"
#include <string.h>
#include <stdlib.h>

int gettok(char **outptr);
int runcommand(char **cline, int where);

// This method will be called once userin is done with the input and the input is valid (as in not EOF)
void procline(void) 
{
	// This stores all the command line arguments/tokens that needs to be send to runcommand
	char *arg[MAXARG + 1]; 
	int toktype; 
	int narg; 
	int type; 

	narg = 0;

	// Infinite loop
	for (;;) 
	{
		// Receive the type of token from gettok in userin.c
	   switch (toktype = gettok(&arg[narg])) {
		// If its PIPE, make sure that we are piping so that the output of first command can be sent to next command
		   case PIPE:
			// Add NULL at the end because execvp will understand that it has all the args it could have had
			arg[narg] = NULL;
			int pipeFileDesc[2];
			// Create pipe to share data between child and parent process
			if (pipe(pipeFileDesc) == -1) {
				perror("Uh oh! Piping error!");
				exit(1);
			}
			
			// Forking processes to share data between child and parent
			pid_t forkId = fork();
			if (forkId == -1) {
				perror("Error encountered while forking!");
				exit(1);
				// Close file descriptors for pipe
				close(pipeFileDesc[0]);
				close(pipeFileDesc[1]);
			} 
			// Child process execution
			else if (forkId == 0) {
				// Close reading end of pipe because we know the command already
				close(pipeFileDesc[0]);

				// Redirecting the std output by the execution of the command to write end of the pipe
				dup2(pipeFileDesc[1], STDOUT_FILENO);

				// Closing write end of pipe in child
				close(pipeFileDesc[1]);

				// Execute the commands
				execvp(arg[0], arg);
				perror("Error while executn");
				exit(1);
			} 
			// Parent process execution
			else {
				// CLosing write end of the pipe
				close(pipeFileDesc[1]);

				// Redirect standard input to read end of pipe because that's where we will get output of first command execution
				dup2(pipeFileDesc[0], STDIN_FILENO);

				// Close reading end as well, since its of no use
				close(pipeFileDesc[0]);

				// Once the child completes execution, the parent continues and executes the command after pipe
				waitpid(forkId, NULL, 0);

				// Reset narg because we want to 
				narg = 0;
			}
			break;
		// If toktype is ARG
		case ARG:
			// If the content length in arg pointer is still less than 512, ie MAXARG
			// increment narg
			if (narg < MAXARG) {
				//printf("arg[narg=%d]=%s\n", narg, arg[narg]); //added for lab 6
				narg++;
			}
			break;
		// If toktype is any of the EOL, ;, &
		case EOL:
		case SEMICOLON:
		case AMPERSAND:
			//printf("arg[narg=%d]=%s\n", narg, arg[narg]); //added for lab 6
			// If token received is &, means user wants to stop the current execution. Maybe the user wants to take time. Set type as BACKGROUND then, meaning, 
			// send this process in background till it receives the continue signal
			// Else, the execution continues, after setting type to FOREGROUND
			if (toktype == AMPERSAND)
				type = BACKGROUND;
			else
				type = FOREGROUND;
			// Okay, so if collectively narg/length of data stored in arg pointer is something (ie not equal to zero), run command
			// However, set final char to NULL so that runcommand knows when to stop
			if (narg != 0)
			{
				arg[narg] = NULL;
				runcommand(arg, type);
			}
			// If toktype is EOL, just return
			// This situation would never arrive though, because procline never gets called when user input is EOL
			if (toktype == EOL)
				return;
			// Start from beginning
			// narg 0, basically resetting data in arg
			narg = 0;
			break;
	  }
	}
}

