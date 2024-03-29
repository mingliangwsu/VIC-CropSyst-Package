#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vicNl.h>

static char vcid[] = "$Id: open_state_file.c,v 4.10 2006/10/18 20:58:59 vicadmin Exp $";


FILE *open_state_file(filenames_struct filenames)
/*********************************************************************
  open_state_file      Keith Cherkauer           April 15, 2000

  This subroutine opens the model state file for output.

  Modifications:
  04-10-03 Modified to open and write to a binary state file.    KAC
  06-03-03 modified to handle both ASCII and BINARY state files.  KAC
  2005-11-29 SAVE_STATE is set in global param file, not in user_def.h GCT
  2005-12-06 Moved setting of statename to get_global_param     GCT
  2006-08-23 Changed order of fread/fwrite statements from ...1, sizeof...
             to ...sizeof, 1,... GCT
  2006-Oct-16 Merged infiles and outfiles structs into filep_struct;
	      This included moving global->statename to filenames->statefile. TJB

*********************************************************************/
{
  extern option_struct options;
  FILE   *statefile;
  char    filename[MAXSTRING];
  double  Nsum;
  /* open state file */
  sprintf(filename,"%s", filenames.statefile);
  if ( options.BINARY_STATE_FILE )
    statefile = open_file(filename,"wb");
  else
    statefile = open_file(filename,"w");

  /* Write save state date information */
  if ( options.BINARY_STATE_FILE ) {
    fwrite( &global_param.stateyear, sizeof(int), 1, statefile );
    fwrite( &global_param.statemonth, sizeof(int), 1, statefile );
    fwrite( &global_param.stateday, sizeof(int), 1, statefile );
  }
  else {
    fprintf(statefile,"%i %i %i\n", global_param.stateyear,
        global_param.statemonth, global_param.stateday);
  }

  /* Write simulation flags */
  if ( options.BINARY_STATE_FILE ) {
    fwrite( &options.Nlayer, sizeof(int), 1, statefile );
    fwrite( &options.Nnode, sizeof(int), 1, statefile );
  }
  else {
    fprintf(statefile,"%i %i\n", options.Nlayer, options.Nnode);
  }

  return(statefile);

}

