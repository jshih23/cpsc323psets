// mainParsnip.c                                Stan Eisenstat (09/19/18)
//
// Prompts for commands, expands environment variables, parses them into
// command structures, and dumps the command structures to stdout.
//
// Bash version based on token arrays and parsed token arrays

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "parsnip.h"

int main()
{
    int nCmd = 1;                   // Command number
    token *list;                    // Array of tokens returned by tokenize()
    token *cmdArray;                // Parsed array of tokens

    char *line = NULL;                          // Space for line read
    size_t nLine = 0;                           // #chars allocated
    for ( ; ; ) {
		printf ("(%d)$ ", nCmd);                // Prompt for command
		fflush (stdout);

		if (getline (&line,&nLine, stdin) <= 0) // Read line
		    break;                              //   Break on end of file

		if ((list = tokenize (line)) == NULL)   // Lex line into tokens
		    continue;

		if (getenv ("DUMP_LIST")) {             // Dump token list only if
		    dumpList (list);                    //   environment variable set
		    printf ("\n");
		}

		if ((cmdArray = parse (list))) {        // Parsed command?
		    dumpList (cmdArray);                //   Dump CMD as array to stdout
		    cmdArray = freeList (cmdArray);
		    nCmd++;                             //   Adjust prompt
		}

		if (getenv ("DUMP_LIST_AGAIN")) {
		    dumpList (list);
		    printf ("\n");
		}
		list = freeList (list);                 // Free token list
    }

    printf ("\n");                              // Add final newline
    free (line);
    return EXIT_SUCCESS;
}


// Free NONE-terminated array of tokens TOK and return NULL.
token *freeList (token *tok)
{
    for (int i = 0; tok[i].type != NONE;  i++) {
	if (tok[i].text)
	    free (tok[i].text);
	tok[i].type = NONE;
	tok[i].text = NULL;
    }
    free (tok);
    return NULL;
}


// Print list of tokens LIST
void dumpList (token *tok)
{
    for (int i = 0; tok[i].type != NONE; i++) { // Walk down linked list
		if (tok[i].type == SIMPLE)                  //   printing token and type
		    printf ("%s:%d ", tok[i].text, tok[i].type);
		else if (tok[i].type == RED_IN)
		    printf ("<%s ", tok[i].text);
		else if (tok[i].type == RED_IN_HERE)
		    printf ("<<BEGIN-HERE\n%sEND-HERE ", tok[i].text);
		else if (tok[i].type == RED_OUT)
		    printf (">%s ", tok[i].text);
		else if (tok[i].type == RED_OUT_APP)
		    printf (">>%s ", tok[i].text);
		else if (tok[i].type == RED_ERR_OUT)
		    printf ("2>&1 ");
		else if (tok[i].type == RED_ERR_CLS)
		    printf ("2>&- ");
		else if (tok[i].type == PIPE)
		    printf ("| ");
		else if (tok[i].type == SEP_AND)
		    printf ("&& ");
		else if (tok[i].type == SEP_OR)
		    printf ("|| ");
		else if (tok[i].type == SEP_END)
		    printf ("; ");
		else if (tok[i].type == SEP_BG)
		    printf ("& ");
		else if (tok[i].type == PAR_LEFT)
		    printf ("( ");
		else if (tok[i].type == PAR_RIGHT)
		    printf (") ");
		else if (tok[i].type == SUBCMD)
		    printf ("SUBCMD ");
		else if (tok[i].type == LOCAL)
		    printf ("LOCAL:%s ", tok[i].text);
		else if (tok[i].type == ARG)
		    printf ("ARG:%s ", tok[i].text);
		else
		    printf ("UNKNOWN:%d ", tok[i].type);
    }
    putchar ('\n');                             // Terminate line
}
