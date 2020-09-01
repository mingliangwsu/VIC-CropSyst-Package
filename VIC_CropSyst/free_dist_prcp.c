#include <stdio.h>
#include <stdlib.h>
#include <vicNl.h>
 
static char vcid[] = "$Id: free_dist_prcp.c,v 4.1.4.5 2009/08/03 22:23:03 vicadmin Exp $";

void free_dist_prcp(dist_prcp_struct *prcp, int Nveg)
/**********************************************************************
	free_dist_prcp	Keith Cherkauer		March 1998

  This routine frees all memory allocated down the distributed 
  precipitation data structure.  This include all grid cell specific
  variables (soil, vegetation, energy, snow).

  modifications:
  06-24-98 modified to account for redesign of distributed precipitation
           data structures                                          KAC
  2007-Apr-21 Replaced loop over Nveg to loop over Nitems, so that lake-
	      specific veg tiles could be freed.			TJB
  2009-Jul-31 Removed extra veg tile for lake/wetland.			TJB

**********************************************************************/
{
  extern option_struct options;
  int Ndist = 2;
  int Nitems = Nveg + 1;
  for (int i = 0; i < Ndist; i++) {
    for (int j = 0; j < Nitems; j++) {
      free((char *)prcp[0].cell[i][j]);
    }
    free((char *)prcp[0].cell[i]);
    for(int j = 0; j < Nitems; j++) {
      //150929LML
      #if (VIC_CROPSYST_VERSION>=3)
      for (int b = 0; b < options.SNOW_BAND; b++) {
        if ((*prcp).veg_var[i][j][b].crop_state) {
            VIC_land_unit_delete((*prcp).veg_var[i][j][b].crop_state->CropSystHandle); //180402LML
            delete (*prcp).veg_var[i][j][b].crop_state;
        }
      }
      #endif
      free((char *)(*prcp).veg_var[i][j]);
    }
    free((char *)(*prcp).veg_var[i]);
  }
  for (int j = 0; j < Nitems; j++) {
    free((char *)prcp[0].energy[j]);
  }
  free((char *)prcp[0].energy);
  for (int i = 0; i < Nitems; i++)
    free((char *)prcp[0].snow[i]);
  free((char *)prcp[0].snow);
  free((char *)prcp[0].mu);
}
