#include "smallsh.h"
#include <unistd.h>
#include <stdlib.h>

int runcommand(char **cline, int where)
{
	pid_t pid;
	int status;
	// Forking the parent because we want the child process to execute the commands
	switch (pid = fork()) {
		case -1:
			// Forking failed, terminate the process
			perror("smallsh");
			return (-1);
		case 0:
			// Execute the commands sent by procline
			execvp(cline[0], cline);
			perror(*cline);
			exit(1);
	}
	//code for parents
	// If user is chilling, and the process is in background
	// However, this entire process will never reach this state
	if (where == BACKGROUND)
	{
		printf("[process id %d]\n", pid);
		return (0);
	}
	
	// Wait for the child process to complete execution of commands given by user
	// If it fails, terminate the program
	// Else, return status, not that it matters in procline
	if (waitpid(pid, &status, 0) == -1)
		return (-1);
	else
		return (status);
}

