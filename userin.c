#include "smallsh.h"
#include <string.h>
#include <unistd.h>

// command arg1 arg2 arg3
static char inpbuf[MAXBUF], tokbuf[2 * MAXBUF],
*ptr = inpbuf, *tok = tokbuf;

int inarg(char c);

// This gets called by main function till it gets EOF
int userin(char *p)
{
	int c, count;
	ptr = inpbuf;
	tok = tokbuf;
	

	char cwd[200];
	// This stores the current working directory till 200 characters in this cwd array of characters
	getcwd(cwd, 200);

//	printf("%s", p);

	// Concatenating cwd with $ and space
	printf("%s", strcat(cwd,"$ "));
	
	count = 0;
	// Continuous while loop till user inputs EOF OR user is done with the input
	while (1)
	{
		// Reading character from std input
		// If that character is EOF; user terminates the execution, then return EOF to main method in main_smallsh
		if ((c = getchar()) == EOF)
			return(EOF);
		// If the count of characters are still less than MAXBUF which is 512, keep on appending the character
		// to inpbuf which is also of max length 512
		if (count < MAXBUF)
			inpbuf[count++] = c;
		// If user puts enter at the end and the count is still less than 512,
		// append null character in the inpbuf
		// Since its end of user input, return count to main function
		// We could have sent back anything technically, except EOF
		if (c == '\n' && count < MAXBUF)
		{
			// Check if inpbuf is quit
			// If it is, then terminate the program by returning EOF to main
			if (strcmp(inpbuf, "quit\n") == 0) {
				return EOF;
			}
			inpbuf[count] = '\0';
			return count;
		}
		
		// If its end of input for user, and none of above conditions worked, that means string length is more than 512
		// Put this message for user, reset the count and since the main function has still not received EOF, it will send the default prompt
		// string here, and thats what will get printed here
		// Again, EOF has not yet been received so this function will be called again and this time, the cwd$  will be seen on prompt
		if (c == '\n') 
		{
			printf("smallsh: input line too long\n");
			count = 0;
			printf("%s ", p);
		}
	}

//	printf("inpbuf=%s\n", inpbuf);
}

int gettok(char **outptr)
{
	int type;
	
	*outptr = tok;
	
	// Pointer starts reading the inpbuf (user input)
	// Keeps skipping till its receiving empty spaces
	// Goes to first non space character
	while (*ptr == ' ' || *ptr == '\t')
		ptr++;
	
	// The value that ptr has at its current position is being set at the current location of tok
	// tok is then incremented
	*tok++ = *ptr;
	
	switch (*ptr++) {
	// If the character at ptr is new line, set type EOL and break the switch here
	case '\n':
		type = EOL;
		break;
	// If the character at ptr is &, set type AMPERSAND and break the switch here
	case '&':
		type = AMPERSAND;
		break;
	// If the character at ptr is ; set type SEMICOLON and break the switch here
	case ';':
		type = SEMICOLON;
		break;
	// If PIPE, break the switch
	case '|':
		type = PIPE;
		break;
	// Char other than EOL, & and ;
	default:
		// Set type as ARG
		type = ARG;
		
		// Iterate till inarg returns 1
		// i.e Iterate till any special character has been seen in the char currently pointed by pts.
		// Ofcourse, tok is reading the characters from the current position pointer ptr is pointing to
		while (inarg(*ptr))
			// tok will keep on reading the characters from pointer ptr till any special character is recevied, and save it in itself
			// Special character as in characters in special[] 
			*tok++ = *ptr++;
	}
	// Once switch breaks, be it because of EOL, &, ;, or end of word or any special character received in string
	// append null character at the end of tok and return type
	*tok++ = '\0';
	return type;
}

//****************************************************************************************
static char special[] = { ' ', '\t', '&', ';', '\n', '\0' };
int inarg(char c)
{
	char *wrk;
	// Iterate through the characters in special array till either all the elements have been iterated
	// or the current character matches any of those special characters
	// Special char matched? return 0
	// Else 1
	for (wrk = special; *wrk; wrk++)
	{
		if (c == *wrk)
			return (0);
	}
	return (1);
}



