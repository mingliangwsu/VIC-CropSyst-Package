#include <stdio.h>
#include <stdlib.h>
#include "vicNl.h"
#ifndef __BCPLUSPLUS__
//190131 I dont think unistd in any version
#include <unistd.h>
#endif
#ifdef VIC_CROPSYST_VERSION
#include "crop/VIC_crop_C_interface.h"
static char vcid[] = "$Id: free_crop.c,v 4.1.1 2011/05/03 KJC WSU $";

void free_crop(crop_data_struct *crop)
/**********************************************************************
  free_vegcon.c	            KEYVAN
  This subroutine frees all components of the CROP_CON structure.
**********************************************************************/
{
  for(int i = 0; i < crop[0].no_of_crops_in_cell; i++){
    if (crop[i].CropSystHandle) {
      VIC_CropSyst_object_delete(crop[i].CropSystHandle);
      crop[i].CropSystHandle = 0;
    };
  }
  free((char *) (crop));
}
#endif

