#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vicNl.h>

static char vcid[] = "$Id: compute_pot_evap.c,v 1.1.2.2 2009/08/22 00:02:23 vicadmin Exp $";

void compute_pot_evap(int veg_class,
              const dmy_struct *dmy,
		      int rec,
              int dt,          /*hour*/
              double shortwave,                                                  //gross shortwave radiation 150624LML note
		      double net_longwave,
		      double tair,
		      double vpd,
		      double elevation,
              const double * const *aero_resist,
              double *pot_evap  /*mm/dt*/
              ,double* pot_evap_daily                                            //160922LML = 0 /*150608LML*/
              ,double run_time_hours                                             //160922LML  = 0                                        //150624LML
		      )

/****************************************************************************

  compute_pot_evap: computes potential evaporation for several different
                    reference land cover types (which are defined in
                    vicNl_def.h and global.h).

  modifications:
  2009-Aug-21 Fixed bug in assignment of albedo for natural vegetation.	TJB
  2013-Feb-27 added CropSyst here and then removed it for know later we can
              change this part of the code

****************************************************************************/
{
  extern veg_lib_struct *veg_lib;
  extern char ref_veg_ref_crop[];
  /************************************************
  Estimate and store potential evap estimates using penman equation
  ************************************************/
  int NVegLibTypes = veg_lib[0].NVegLibTypes;
  double ra,rs,rc,rad,rarc,RGL,lai,albedo;
  for (int i = 0; i < N_PET_TYPES; i++) {
    if (i < N_PET_TYPES_NON_NAT) {
      veg_lib_struct &vegLib = veg_lib[NVegLibTypes+i];
      rs = vegLib.rmin;
      rarc = vegLib.rarc;
      RGL = vegLib.RGL;
      lai = vegLib.LAI[dmy->month-1];
      albedo = vegLib.albedo[dmy->month-1];
    } else {
      veg_lib_struct &vegLib = veg_lib[veg_class];
      rs = vegLib.rmin;
      if (i == PET_VEGNOCR) rs = 0;
      rarc = vegLib.rarc;
      RGL = vegLib.RGL;
      lai = vegLib.LAI[dmy->month-1];
      albedo = vegLib.albedo[dmy->month-1];
    }
    double net_short = (1.0 - albedo) * shortwave;
    double gsm_inv = 1.0;
    char ref_crop = ref_veg_ref_crop[i];
    rc = calc_rc(rs, net_short, RGL, tair, vpd, lai, gsm_inv, ref_crop);
    if (i < N_PET_TYPES_NON_NAT || !veg_lib[veg_class].overstory)
      ra = aero_resist[i][0];
    else
      ra = aero_resist[i][1];
    rad = net_short + net_longwave;

#ifdef VIC_CROPSYST_VERSION
    if (i == PET_SHORT) {                                                        //The PET of short grass is used for driving CropSyst
        rc = 70.;
        rarc = 0;
    }
#endif

    double daily_et = penman(tair,elevation,rad,vpd,ra,rc,rarc);
    pot_evap[i] = daily_et * dt/HOURSPERDAY;
    if (pot_evap_daily != 0) {
      pot_evap_daily[i] += daily_et * run_time_hours / HOURSPERDAY;
    }
  }
}
