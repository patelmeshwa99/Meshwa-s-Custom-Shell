// Importing the smallsh header file that has all the global variables defined
#include "smallsh.h"

// Default prompt string
char *prompt = "Command>>> ";

int userin(char *p);
void procline(void);

int main()
{
	// Till the EOF character is not received, let userin do whatever its doing (added comments there)
	while (userin(prompt) != EOF)
		// Once userin is done and it returns, and if its not EOF,
		// call procline in proc_line C file
		procline();
}


