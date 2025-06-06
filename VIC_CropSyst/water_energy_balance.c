/*
 * SUMMARY:      water_energy_balance.c - Calculate snow accumulation and melt for the lake model
 * USAGE:        
 *
 * AUTHOR:       Laura Bowling
 * ORG:          University of Washington, Department of Civil Engineering
 * E-MAIL:       nijssen@u.washington.edu
 * ORIG-DATE:     8-Oct-1996 at 08:50:06
 * LAST-MOD: Wed Nov  7 15:45:45 2001 by Keith Cherkauer <cherkaue@u.washington.edu>
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

//static char vcid[] = "$Id: water_energy_balance.c,v 5.5.2.4 2009/12/12 19:42:13 vicadmin Exp $";

/*****************************************************************************
  Function name: water_energy_balance()

  Purpose      : 

  Required     :


  Comments     :

  Modifications:
  04-Oct-04 Merged with Laura Bowling's updated lake model code.		TJB
  2006-Sep-23 Replaced redundant STEFAN constant with STEFAN_B.			TJB
  2006-Nov-07 Removed LAKE_MODEL option.					TJB
  2007-Apr-03 Modified to handle grid cell errors by returning to the
	      main subroutine, rather than ending the simulation.		KAC via GCT
  2007-Nov-06 Replaced lake.fraci with lake.areai.  Added workaround for
	      non-convergence of temperatures.					LCB via TJB
  2008-Mar-01 Added assignments for Tcutk and Le to ensure that they are always
	      assigned a value before being used.				TJB
  2009-Dec-11 Replaced "assert" statements with "if" statements.		TJB
*****************************************************************************/
int water_energy_balance(int     numnod,
			 double *surface,
			 double *evapw,
			 int     dt,
			 int     freezeflag,
			 double  dz,
			 double  surfdz,	
			 double  lat,
			 double  Tcutoff,
			 double  Tair,
			 double  wind,
			 double  pressure,
			 double  vp,
			 double  air_density,
			 double  longwave,
			 double  shortwave,
			 double  sumjoulb,
			 double  wind_h,
			 double *Qh,
			 double *Qle,
			 double *LWnet,
			 double *T,
             double *vic_water_density,
			 double *deltaH,
			 double *energy_ice_formation,
			 double  fracprv,
			 double *new_ice_area,
			 double *cp,
			 double *new_ice_height,
			 double *energy_out_bottom,
			 double *new_ice_water_eq,
			 double  lvolume)
	      
{
  double Ts;
  double Tcutk;
  double Tskin;
  double Tnew[MAX_LAKE_NODES];
  int k;
  double sumjouli;
  
  double Tmean;
  int iterations;
  double Le;
  double jouleold;
  double joulenew;
  double error;
  double Told;
  double Tupper, Tlower;
  
  double de[MAX_LAKE_NODES]; 
  double epsilon = 0.0001;
  double a1 = PERCENT_RADIATION_VISIBLE;                                        //LML 150408
  double a2 = PERCENT_RADIATION_INFRARED;                                       //LML 150408
  /* Calculate the surface energy balance for water surface temp = 0.0 */
 
  Tmean = -999.;
  error = -999.;
  Ts    = T[0];
  iterations = 0;
  for(k=0; k<numnod; k++)
    Tnew[k] = T[k];
 
  energycalc(T, numnod,dz, surfdz, surface, cp, vic_water_density,jouleold);
 
  while((fabs(Tmean - Ts) > epsilon) && iterations < MAX_ITER) {
 
    if(iterations == 0)
      Ts=T[0];
    else
      Ts=Tmean;

    /* ....................................................................
     * Pass the skin temperature of the lake in Kelvin since the
     * Stefan-Boltzmann formula uses K.
     * ....................................................................*/
 
    Tskin = Ts + KELVIN;
    Tcutk = Tcutoff + KELVIN;

    /* ....................................................................
     * Send an ice height of 0 meters to latsens for the calculation
     * of latent and sensible heat over liquid water.
     * ....................................................................*/
 
    latsens (Tskin, Tcutk, 0.0, Tair, wind, pressure, vp, air_density, wind_h,
         *evapw, *Qh);
 
    /**********************************************************************
      Compute the Latent Heat Flux 
    **********************************************************************/
 
    Le = (2.501 - 0.002361 * Tair) * 1.0e6;   /*J/kg  */
 
    *Qle = -1.*(*evapw)*Le;                          /* W/m^2 */

    /* --------------------------------------------------------------------
     * Calculate the outgoing long wave fluxes, positive downwards.
     * -------------------------------------------------------------------- */

    *LWnet = longwave -EMH2O*STEFAN_B*Tskin*Tskin*Tskin*Tskin;
 
    /*************************************************************
      Use a Triadiagonal Matric to Explicitly Solve for 
      Temperatures at Water Thermal Nodes
    *************************************************************/

    /* --------------------------------------------------------------------
     * Calculate the eddy diffusivity.
     * -------------------------------------------------------------------- */
    eddy(1, wind, vic_water_density, lat, numnod, dz, surfdz, de);
    /* --------------------------------------------------------------------
     * Calculate the lake temperatures at different levels for the
     * new timestep.
     * -------------------------------------------------------------------- */
    temp_area(shortwave*a1, shortwave*a2, *Qle+*Qh+*LWnet, T,
          vic_water_density, de, dt, surface, numnod,
          dz, surfdz, cp, joulenew, *energy_out_bottom, Tnew);
 
    /* Surface temperature < 0.0, then ice will form. */
    if(Tnew[0] <  Tcutoff) {
      iceform(Tcutoff, fracprv, numnod, dt, dz, surfdz, cp, surface, lvolume,
           Tnew, *energy_ice_formation, *new_ice_area, *new_ice_height, *new_ice_water_eq);
      energycalc(Tnew, numnod, dz, surfdz, surface, cp, vic_water_density,sumjouli);
      *deltaH = (sumjouli-jouleold)/(surface[0]*dt*SECPHOUR);
    }
    else {
      *deltaH = (joulenew-jouleold)/(surface[0]*dt*SECPHOUR);
      *energy_ice_formation = 0.0;
    }

    Tmean = (Tnew[0] + T[0])/2.;  

    error = *LWnet + shortwave - *energy_out_bottom + *Qh + *Qle  - *deltaH + *energy_ice_formation;
    iterations += 1;
  }
 
  if(fabs(Tmean - Ts) <= epsilon) {
    // Temperature reached convergence
    for(k=0; k<numnod; k++)
      T[k] = Tnew[k];
    return(0);
  }
  else {
    //  fprintf(stderr, "Lake temperatures in open water fraction failed to converge; temporary work around used.\n");
    Tskin = T[0] + KELVIN;
    Tcutk = Tcutoff + KELVIN;
    latsens (Tskin, Tcutk, 0.0, Tair, wind, pressure, vp, air_density, wind_h,
             *evapw, *Qh);

    Le = (2.501 - 0.002361 * Tair) * 1.0e6;     /*J/kg  */
    *Qle = -1.*(*evapw)*Le;                          /* W/m^2 */

    *LWnet = longwave -EMH2O*STEFAN_B*Tskin*Tskin*Tskin*Tskin;

    if(T[0] <  Tcutoff) {
      iceform(Tcutoff, fracprv, numnod, dt, dz, surfdz, cp, surface, lvolume,
              T, *energy_ice_formation, *new_ice_area, *new_ice_height, *new_ice_water_eq);
    }
    else {
      *energy_ice_formation = 0.0;
    }

    *deltaH = 0.0;
    //return (ERROR);
    return(0);

  }

}
