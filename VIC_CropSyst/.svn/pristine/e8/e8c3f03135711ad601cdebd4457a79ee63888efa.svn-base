#include <stdio.h>
#include <stdlib.h>
#include <vicNl.h>
 
static char vcid[] = "$Id: make_dist_prcp.c,v 5.4.2.1 2009/08/03 22:23:03 vicadmin Exp $";

dist_prcp_struct make_dist_prcp(int  nveg
                                //150929LML #if VIC_CROPSYST_VERSION>=3
                                //150929LML ,veg_con_struct *veg_con
                                //150929LML #endif
                                )
/**********************************************************************
	read_dist_prcp	Keith Cherkauer		May 21, 1996

  This routine creates an array of structures which will store 
  necessary information about the distribution of precipitation, moisture,
  evaporation, and dew.  Mu represents the fractional area of the grid 
  that receives precipitation (wet), while 1-mu is the corresponding 
  area that receives no precipitation.  The value of mu changes with
  the intensity of incoming precipitation, and is set in the routine
  dist_prec.

  modifications:
  11-18-02 Modified to allocate vegetation variables for the 
           wetland vegetation class.                             LCB
  01-Nov-04 Updated arglist to make_energy_bal() as part of fix for
	    QUICK_FLUX state file compatibility.		TJB
  2006-Nov-07 Removed LAKE_MODEL option.  TJB
  2009-Jul-31 Removed extra lake/wetland tile.			TJB
**********************************************************************/
{
  extern option_struct options;

  dist_prcp_struct temp;
  int              i;
  int              Nitems;

  Nitems = nveg + 1;

  temp.mu     = (double *)calloc(Nitems,sizeof(double));
  for ( i = 0; i < Nitems; i++ ) temp.mu[i] = 1;
  temp.snow   = make_snow_data(Nitems);
  temp.energy = make_energy_bal(Nitems);
  for ( i = 0; i < 2; i++ ) {
    temp.veg_var[i]  = make_veg_var(Nitems);
    temp.cell[i]     = make_cell_data(Nitems,options.Nlayer);
  }
  //150928LML
  //150929LML #if VIC_CROPSYST_VERSION>=3
  //150929LML temp.crops = new (crop_data_struct**)[Nitems];
  //150929LML for (i = 0; i < Nitems; i++) {
  //150929LML   temp.crops[i] = new (crop_data_struct*)[options.SNOW_BAND];
  //150929LML   for (int b = 0; b < options.SNOW_BAND; b++) {
  //150929LML     temp.crops[i][b] = 0;
  //150929LML     if (iscrop(veg_con[i].veg_class_code && veg_con[i].Cv > 1e-8))
  //150929LML       temp.crops[i][b] = new crop_data_struct;
  //150929LML   }
  //150929LML }
  //150929LML #endif

  return (temp);
}
