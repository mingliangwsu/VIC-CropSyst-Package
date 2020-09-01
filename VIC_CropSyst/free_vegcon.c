#include <stdio.h>
#include <stdlib.h>
#include <vicNl.h>
 
static char vcid[] = "$Id: free_vegcon.c,v 3.1 1999/02/16 18:02:07 vicadmin Exp $";

void free_vegcon(veg_con_struct **veg_con)
/**********************************************************************
  free_vegcon.c	            Keith Cherkauer	  September 25, 1998

  This subroutine frees all components of the veg_con structure.

**********************************************************************/
{
  for(int i=0;i<=veg_con[0][0].vegetat_type_num;i++) {
    #if VIC_CROPSYST_VERSION>=3
    if (iscrop(veg_con[0][i].VCS.veg_class_code)) {
      for (int band = 0; band < options.SNOW_BAND; band++)
         delete veg_con[0][i].VCS.veg_lib[band];
    }
    delete[] veg_con[0][i].VCS.veg_lib;
    #endif
    free((char *)veg_con[0][i].zone_depth);
    free((char *)veg_con[0][i].zone_fract);
  }
  free((char *)veg_con[0]);
}
