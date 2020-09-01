#include <stdio.h>
#include <stdlib.h>
#include <vicNl.h>

#ifdef __linux__
#include <unistd.h>     //LML 140812
#endif
static char vcid[] = "$Id: nrerror.c,v 3.2 1999/08/23 23:59:06 vicadmin Exp $";

void nrerror(char error_text[])
/* Numerical Recipes standard error handler */
{
	fprintf(stderr,"Model run-time error...\n");
	fprintf(stderr,"%s\n",error_text);
	fprintf(stderr,"...now exiting to system...\n");
    #ifdef __cplusplus
    exit(1);
    #else
    _exit(1);
    #endif
}
