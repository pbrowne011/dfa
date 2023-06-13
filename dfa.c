/* DFA State Machine Program

 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void usage()
{
  const char *usage_str = "\
Defining usage for the DFA simulator"

int
main(int argc, char *argv[])
{
  int c = 0;
  int vflag = 0;
  char dfa_file[512];
  strcpy(dfa_file, "");

  while((c = getopt(argc, argv, "d:hv")) != -1) {
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
	break;
      }

  }
}
