/* DFA State Machine Program

 */

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>

/* For my hash map implementation, I relied on Ben Hoyt's
   blog and associated code for guidance. You can see his
   implementation at benhoyt.com/writings/hash-table-in-c/.

   See hm.h for functions and structs 
 */

#include "hm.h"


/* usage - prints instructions to screen */

void usage()
{
  const char *usage_str = "\n\
Note to self: this is the usage message that Prof. Seetharaman\
used in the initial assignment paper.\n\n\
./dfa-h -d <dfafile> -v\n\n\
This is a simple DFA Implementation. The DFA definition\n\
is specified via .dfa file; input string is read from stdin.\n\
In non-verbose mode, print ACCEPT or NOT ACCEPT as the case may be.\n\n\
-h\n\
        print usage\n\
-d <dfafile>\n\
        DFA definition file\n\
-v\n\
        verbose mode; display machine definition, transitions etc.\n\n\
Example:\n\
./dfa -d m1.dfa\n\
Example dfa definition file m1.dfa\n\n\
# DFA M1 from Page 36 of ITC Text;\n\
states: q1 q2 q3\n\
alphabet: 0 1\n\
startstate: q1\n\
finalstate: q2\n\
transition: q1 0 q1\n\
transition: q1 1 q2\n\
transition: q2 0 q3\n\
transition: q2 1 q2\n\
transition: q3 0 q2\n\
transition: q3 1 q2\n\n\
Example run in interactive mode:\n\
$ ./dfa -d m1.dfa\n\
00011\n\
00011 --> ACCEPT\n\
00100\n\
00100 --> ACCEPT\n\
00000\n\
00000 --> NOT ACCEPT\n\
00000 --> NOT ACCEPT\n\
";

  printf("%s\n", usage_str);
}

/* Here, we define a function read_input() to read in a
   .dfa file as input and store in different data structures.
   
 */

void
rm_newline(char *line) {
  for (int i = 0, j; line[i] != '\0'; ++i) {
      while (line[i] == '\n') {

         for (j = i; line[j] != '\0'; ++j) {
	   line[j] = line[j + 1];
         }

         line[j] = '\0';
      }
  }
}

/* Define verbose printing functions depending
   on user input
   
 */
int vflag = 0;

int quietprint(const char *format, ...) {
  return 0;
}

int verboseprint(const char *format, ...) {
  va_list arg;
   int done;

   va_start (arg, format);
   done = vfprintf (stdout, format, arg);
   va_end (arg);

   return done;
}

int (*fx[2])(const char *, ...) = {quietprint, verboseprint};

int
main(int argc, char *argv[])
{
  int c = 0;
  char dfa_file[512];
  strcpy(dfa_file, "");    

  while((c = getopt(argc, argv, "hd:v")) != -1) {
    switch(c)
      {
      case 'd':
	strcpy(dfa_file, optarg);
	break;
      case 'h':
	usage();
	break;
      case 'v':
	vflag = 1;
	break;
      case '?':
	usage();
	break;
      default:
	usage();
      }
  }
  
  /* Including from the getopt code provided */
  printf("dfa_file=%s verbose=%d\n", dfa_file, vflag);

  if (strcmp(dfa_file, "") == 0) {
    fprintf(stderr, "-d is a required parameter\n");
    exit(EXIT_FAILURE);
  } else {
    // Read input from the .dfa file
    int buff_size = 1024;
    char buff[buff_size];
    char *line;

    // Define the DFA
    #define SPACE_SIZE 128
    
    char **states;
    char **alphabet;
    char *start_state;
    char **final_states;
    hm *transition_map = hm_create();

    states = malloc(sizeof(char *) * SPACE_SIZE);
    alphabet = malloc(sizeof(char *) * SPACE_SIZE);
    start_state = malloc(sizeof(char) * SPACE_SIZE);
    final_states = malloc(sizeof(char *) * SPACE_SIZE);
    
    int state_idx = 0;
    int alpha_idx = 0;
    int final_idx = 0;
    
    FILE *file = fopen(dfa_file, "r");
    while(fgets(buff, buff_size, file) != NULL) {
      line = strtok(buff, " ");

      // Cases for each possible first line of .dfa
      if (strcmp(line, "states:") == 0)
	{
	  line = strtok(NULL, " ");
	  while (line != NULL) {
	    rm_newline(line);
	    states[state_idx] = malloc(sizeof(char) * strlen(line));
	    strcpy(states[state_idx], line);
	    state_idx++;
	    
	    line = strtok(NULL, " ");
	  }
	}
      else if (strcmp(line, "alphabet:") == 0)
	{
	  line = strtok(NULL, " ");
	  while (line != NULL) {
	    rm_newline(line);
	    alphabet[alpha_idx] = malloc(sizeof(char) * strlen(line));
	    strcpy(alphabet[alpha_idx], line);
	    alpha_idx++;
	    
	    line = strtok(NULL, " ");
	  }
	}
      else if (strcmp(line, "startstate:") == 0)
	{
	  line = strtok(NULL, " ");
	  rm_newline(line);
	  strcpy(start_state, line);
	}
      else if (strcmp(line, "finalstate:") == 0)
	{
	  line = strtok(NULL, " ");
	  while (line != NULL) {
	    rm_newline(line);
	    final_states[final_idx] = malloc(sizeof(char) * strlen(line));
	    strcpy(final_states[final_idx], line);
	    final_idx++;
	    
	    line = strtok(NULL, " ");
	  }
	}
      else if (strcmp(line, "transition:") == 0)
	{
	  /* We are expecting three entries here. We add the first and the
	     second together (e.g., "q1 1"), and assign that as the key.
	     We then assign the third value (e.g., "q3") as the value.
	   */
	  char *key_1;
	  char *key_2;
	  
	  key_1 = strtok(NULL, " ");
	  key_2 = strtok(NULL, " ");
	  char *key = malloc(sizeof(char) * (strlen(key_1) + strlen(key_2) + 1));
	  char *value = malloc(sizeof(char) * (strlen(key_1) + 1));
	  
	  strcpy(key, key_1);
	  strcat(key, key_2);

	  /* Note to self: draw memory diagrams soon to understand why you need to
	     create a val_temp pointer and then copy to value, rather than just
	     assigning the next string to value. Is it because value is still
	     pointing to the previous value??
	   */
	  
	  char *val_temp = strtok(NULL, " ");
	  strcpy(value, val_temp);
	  rm_newline(value);
	  
	  hm_set(transition_map, key, value);
	}
      else if (strcmp(line, "#") == 0)
	{
	  continue;
	}
      else
	{
	  printf("Error in your input file, exiting program");
	  exit(EXIT_FAILURE);
	}
    }
    
    fclose(file);
    
    /* If verbose mode is enabled, print DFA definition */
    fx[vflag]("--- BEGIN DFA Definition ---\nStates:\n   ");
    for (int i = 0; i < state_idx; i++) {
      fx[vflag]("%s ", states[i]);
    }	

    fx[vflag]("\nAlphabet:\n   ");
    for (int i = 0; i < alpha_idx; i++) {
      fx[vflag]("%s ", alphabet[i]);
    }

    fx[vflag]("\nStart State: %s\n", start_state);
    fx[vflag]("Final State(s): ");
    for (int i = 0; i < final_idx; i++) {
      fx[vflag]("%s ", final_states[i]);
    }
    
    fx[vflag]("\nTransitions\n");
    for (int i = 0; i < state_idx; i++) {
      if (states[i]) {
	for (int j = 0; j < alpha_idx; j++) {
	  if (alphabet[j]) {
	    char *key_1 = states[i];
	    char *key_2 = alphabet[j];
	    
	    char *key = malloc(sizeof(char) * (strlen(key_1) + strlen(key_2) + 1));
	    strcpy(key, key_1);
	    strcat(key, key_2);
 
	    fx[vflag]("   %s %s %s\n", states[i], alphabet[j],
		      hm_get(transition_map, key));
	  }
	}
      }
    }
    fx[vflag]("--- END DFA Definition ---\n\n");
    
    /* Take user input, then apply transitions */
    while (1) {
      int accepted = 0;
      
      char user_input[SPACE_SIZE];
      scanf("%s", user_input);
    
      char *curr_state = malloc(sizeof(char) * (strlen(start_state) + 1));
      strcpy(curr_state, start_state);

      for (int i = 0; i < strlen(user_input); i++) {
	fx[vflag]("Current state: %s   ", curr_state);
        char user_str[2] = {user_input[i], '\0'};
	fx[vflag]("Symbol: %s ", user_str);
	strcat(curr_state, user_str);

        char *temp = (char *)hm_get(transition_map, curr_state);
        strcpy(curr_state, temp);
	fx[vflag]("--> New state: %s\n", curr_state);
      }
    
      for (int j = 0; j < SPACE_SIZE; j++) {
        if (final_states[j] != NULL && strcmp(curr_state, final_states[j]) == 0) {
	  accepted = 1;
	  break;
        }
      }

      if (accepted == 1) {
        printf("%s --> ACCEPTED\n", user_input);
      } else {
        printf("%s --> REJECTED\n", user_input);
      }
      
    }
    free(states);
    free(alphabet);
    free(final_states);
    
    exit(EXIT_SUCCESS);
  }
}
