#include <stdio.h>
//190131 not needed #include <strings.h>
#include <stdlib.h>
#include <vicNl.h>

static char vcid[] = "$Id: compress_files.c,v 5.1 2001/08/15 23:44:57 cherkaue Exp $";

void compress_files(char string[])
/**********************************************************************
  compress_files.c	Keith Cherkauer		September 10, 1997

  This subroutine compresses the file "string" using a system call.

**********************************************************************/
{

  char command[MAXSTRING];

  /** uncompress and open zipped file **/
#if VERBOSE
  fprintf(stderr,"zipping \"%s\".\n",string);
#endif

  sprintf(command,"nice gzip -f %s &",string);
  system(command);

}
