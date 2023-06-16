/* DFA State Machine Program

 */

#include <string.h>
#include <stdio.h>
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



int
main(int argc, char *argv[])
{
  int c = 0;
  int vflag = 0;
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
  }

  exit(EXIT_SUCCESS);
}
