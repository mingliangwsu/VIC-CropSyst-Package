/*
 * SUMMARY:      water_under_ice.c 
 * USAGE:        
 *
 * AUTHOR:       Laura Bowling
 * ORG:          University of Washington, Department of Civil Engineering
 * E-MAIL:       nijssen@u.washington.edu
 * ORIG-DATE:     8-Oct-1996 at 08:50:06
 * LAST-MOD: Mon Nov 12 16:38:32 2001 by Keith Cherkauer <cherkaue@u.washington.edu>
 * DESCRIPTION:  
 * DESCRIP-END.
 * FUNCTIONS:   
 * COMMENTS:     
 */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <vicNl.h>

#define MAX_ITER 50

//static char vcid[] = "$Id: water_under_ice.c,v 5.5.2.4 2009/12/12 19:42:13 vicadmin Exp $";

/*****************************************************************************
  Function name: water_under_ice()

  Purpose      : 

  Required     :


  Comments     :

  Modifications:
  15-Jun-04 Initialize mixmax to 0.						TJB
  04-Oct-04 Merged with Laura Bowling's updated lake model code.		TJB
  2006-Nov-07 Removed LAKE_MODEL option.					TJB
  2007-Apr-03 Modified to handle grid cell errors by returning to the
	      main subroutine, rather than ending the simulation.		KAC via GCT
  2007-Nov-06 Replaced lake.fraci with lake.areai.  Added workaround for
	      non-convergence of temperatures.					LCB via TJB
  2009-Dec-11 Replaced "assert" statements with "if" statements.		TJB
*****************************************************************************/
int water_under_ice(const int     freezeflag,
            const double  sw_ice,
            const double  wind,
		    double *Ti,
            double *vic_water_density,
            const double  lat,
            const int numnod,
            const double  dz,
            const double  surfdz,
            const double  Tcutoff,
            double &qw,
		    double *surface,
            double &deltaH,
		    double *water_cp,
            const double  hice,
            const double  sdepth,
            const double  dt,
            int &mixdepth,
            double &energy_out_bottom)
	      
{
  double Tnew[MAX_LAKE_NODES];
  int k;
  double jouleold;
  double joulenew;
  double de[MAX_LAKE_NODES]; 
  const double epsilon = 0.0001;
  double qw_init, qw_mean, qw_final;
  double sw_underice_visible, sw_underice_nir;
  const double a1 = PERCENT_RADIATION_VISIBLE;                                        //LML 150408
  const double a2 = PERCENT_RADIATION_INFRARED;                                       //LML 150408

  int iterations = 0;
  for (int k = 0; k < numnod; k++)
    Tnew[k] = Ti[k];
  // compute the eddy diffusivity 
  eddy(freezeflag, wind, vic_water_density, lat, numnod, dz, surfdz, de);
    // estimate the flux out of the water
  qw_init =  0.57 * (Ti[0] - Tcutoff) / (surfdz / 2.);
  qw = qw_init;
  qw_mean = -999.;
  energycalc(Ti, numnod,dz, surfdz, surface, water_cp, vic_water_density, jouleold);
  while((fabs(qw_mean - qw) > epsilon) && iterations < MAX_ITER) {
    if(iterations == 0)
      qw = qw_init;
    else
      qw = qw_mean;
    // compute shortwave that transmitted through the lake ice 
    sw_underice_visible = a1 * sw_ice * exp(-1. * (lamisw * hice + lamssw * sdepth));
    sw_underice_nir     = a2 * sw_ice * exp(-1. * (lamilw * hice + lamslw * sdepth));
    /* --------------------------------------------------------------------
     * Calculate the lake temperatures at different levels for the
     * new timestep.
     * -------------------------------------------------------------------- */
	
    temp_area(sw_underice_visible, sw_underice_nir, -1.*qw , Ti,
           vic_water_density, de, dt, surface, numnod,
           dz, surfdz, water_cp, joulenew, energy_out_bottom, Tnew);

    // recompute storage of heat in the lake
    deltaH = (joulenew - jouleold) / (surface[0] * dt * SECPHOUR);
      
    /* --------------------------------------------------------------------
     * Do the convective mixing of the lake water.
     * -------------------------------------------------------------------- */

    tracer_mixer(freezeflag,
              surface,numnod, dz, surfdz, water_cp, Tnew, mixdepth);
    qw_final = 0.57 * (Tnew[0] - Tcutoff) / (surfdz / 2.);
    qw_mean = (qw_final + qw) / 2.;
    iterations++;
  }

  if(fabs(qw_mean - qw) <= epsilon) {
    // Temperature reached convergence
    for (int k = 0; k < numnod; k++)
      Ti[k] = Tnew[k];
    qw = qw_mean;
    return(0);
  } else {
//    fprintf(stderr, "Lake temps under ice failed to converge; temporary work around used.\n");
    qw = 0.0;
    for (int k = 0; k < numnod; k++)
      Ti[k] = Tcutoff;
    energycalc(Ti, numnod,dz, surfdz, surface, water_cp, vic_water_density, joulenew);
    deltaH = (joulenew - jouleold) / (surface[0] * dt * SECPHOUR);
    return(0);
    // return (ERROR);
  }
}
