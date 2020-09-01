#include <stdio.h>
#include <stdlib.h>
#include <vicNl.h>

#include "global.h"

static char vcid[] = "$Id: arno_evap.c,v 5.3.2.7 2012/02/05 00:15:43 vicadmin Exp $";

/****************************************************************************
                                                                           
  ARNO/ARNO Model of Evaporation                                           
                                                                           
  Routine to compute evaporation based on the assumption that              
  evaporation is at the potential for the area which is saturated,         
  and at some percentage of the potential for the area which is partial    
  saturated.                                                               
                                                                           
  Evaporation from bare soil calculated only from uppermost layer.         
                                                                           
  Evaporation is in mm/(time step)  --> usually 1 day or 1 hour            

  modifications:
  6-8-2000  modified to make use of spatially distributed frost   KAC
  2-24-03   moved unit conversion of moist_resid outside of distributed
            precipitation loop.  Moist_resid was being multiplied by
            D1 * 1000 twice for the dry fraction.                 KAC
  04-Jun-04 Moved unit conversion of moist_resid back inside distributed
	    precipitation loop in such a way that it does not get multiplied
	    by D1 * 1000 twice.					TJB
  04-Jun-04 Changed logic of evap limit check to avoid creating spurious
	    condensation.  Previously, when liquid moisture < residual
	    moisture, (liquid moisture - residual moisture) would be
	    negative.  Any non-negative evap would be greater than this,
	    resulting in evap getting set to (liquid moisture - residual
	    moisture), which would be negative (i.e. condensation).
	    This artificially created condensation in whatever amount
	    necessary to bring liquid moisture up to residual, causing
	    1) large latent heat flux, 2) incorrect surface temperatures,
	    3) occasional inability for calc_surf_energy_bal to converge
	    in root_brent, and 4) spuriously high runoff and baseflow.
	    Now there is an added condition that liquid moisture > residual
	    moisture for evap to be capped at (liquid moisture - residual
	    moisture).	In addition, the previous logic for capping evap
	    involved an incorrect calculation of the soil's ice content.
	    Since the new logic doesn't require calculation of ice content,
	    this calculation has been removed altogether.		TJB
  2007-Apr-06 Modified to handle grid cell errors by returning ERROR
	      status that can be trapped by calling routines.			GCT/KAC
  2009-Mar-16 Modified to use min_liq (minimum allowable liquid water
	      content) instead of resid_moist.  For unfrozen soil,
	      min_liq = resid_moist.						TJB
  2009-Jun-09 Moved computation of canopy resistance rc out of penman()
	      and into separate function calc_rc().				TJB
  2009-Nov-15 Changed D1 to depth1 to avoid confusion with the D1 used in
	      func_surf_energy_bal() (the parent function), which refers
	      to the depth of the 1st soil thermal node in some cases.		TJB
  2009-Dec-11 Removed min_liq and options.MIN_LIQ.				TJB
  2010-Apr-28 Removed net_short, displacement, roughness, and ref_height from
	      arg list as they are no longer used.				TJB
  2012-Jan-16 Removed LINK_DEBUG code						BN
****************************************************************************/

//160509LML rephased source code
double arno_evap(const soil_con_struct &soil_con
                 ,const double          rad
                 ,const double          air_temp
                 ,const double          vpd
                 ,const double          ra
                 ,const double          delta_t                                  //seconds
                 ,const double          orig_mu
                 #if SPATIAL_FROST
                 ,const double      *frost_fract
                 #endif
                 ,layer_data_struct *layer_wet
                 ,layer_data_struct *layer_dry
                 )
{
  extern option_struct options;
  int    Ndist = options.NDIST;
  double total_evap = 0;
  for (int dist = 0; dist < Ndist; dist++) {
    layer_data_struct *layer;
    double mu = orig_mu;
    if (dist > 0) {
      mu = (1. - mu);
      layer = layer_dry;
    } else {
      layer = layer_wet;
    }
    //* Calculate the potential bare soil evaporation (mm/time step) *
    //PenmanInputParameter penman_parameter(air_temp, soil_con.elevation, rad, vpd, ra, 0, 0);

    //190718 V5 parameter 
    double SOIL_RARC = 100.0;
    
    double evap = calc_soil_evap(*layer, soil_con
                                 ,air_temp
                                 ,soil_con.elevation
                                 ,rad
                                 ,vpd
                                 ,ra
                                 ,0
                                 ,SOIL_RARC
                                #if SPATIAL_FROST
                                ,frost_fract
                                #endif
                                )
                                * delta_t * 1000.;                               //(mm/T)
    //std::clog << "delta_t:" << delta_t << "\tevap:" << evap << std::endl;
    #ifdef VIC_CROPSYST_VERSION
    layer[0].VCS.evap_bare_soil = evap;
    //std::clog << "delta_t:" << delta_t
    //          << "\tlayer[0].VCS.evap_bare_soil:" << evap
    //          << std::endl;
    //if (evap > 10) {
    //    std::clog << "evap > 10!!!\n";
    //}

    //std::cerr << "\trad:\t" << rad << "\t evap:\t" << evap << std::endl;
    #else
    layer[0].evap = evap;
    #endif
    total_evap += evap / 1000. / delta_t * mu;                                   //(m/s)
  } //dist
  return(total_evap);
}
//160509LML soil evaporation (m/s)
double calc_soil_evap(const layer_data_struct &layer
    ,const soil_con_struct &soil_con
    ,const double tair
    ,const double elevation
    ,const double rad
    ,const double vpd
    ,const double ra
    ,const double rc
    ,const double rarc
    #if SPATIAL_FROST
    ,const double *frost_fract
    #endif
    )
{
    double ratio;
    double moist;
    double evap = 0;
    #if SPATIAL_FROST
    moist = 0;
    for (int frost_area = 0; frost_area < FROST_SUBAREAS; frost_area++ ) {
      moist += (layer.moist - layer.ice[frost_area])
               * frost_fract[frost_area];
    }
    #else
    moist = layer.moist - layer.ice;
    #endif
    if ( moist > soil_con.max_moist[0]) moist = soil_con.max_moist[0];
    //* Calculate the potential bare soil evaporation (mm/time step) *
    double pot_soil_evap = penman(tair, elevation, rad, vpd, ra, rc, rarc);
    double tmp = 0;
    //**********************************************************************
    //*  Compute temporary infiltration rate based on given soil_moist.    *
    //**********************************************************************
    double max_infil = (1.0 + soil_con.b_infilt) * soil_con.max_moist[0];
    if (soil_con.b_infilt == -1.0) {
      tmp = max_infil;
    } else {
      ratio = 1.0 - moist / soil_con.max_moist[0];

      //std::clog << "\nratio:" << ratio << std::endl;

      //*****if(ratio < SMALL && ratio > -SMALL) ratio = 0.;*****
      if(ratio > 1.0) {
        printf("\n  ERROR: SOIL RATIO GREATER THAN 1.0\n");
        printf("moisture %f   max_moisture %f -> ratio = %f\n",
           moist,soil_con.max_moist[0],ratio);
        return( ERROR );
      } else {
        if(ratio < 0.0) {
          printf("\n  ERROR: SOIL RATIO LESS THAN 0.0\n");
          printf("moisture %f   max_moisture %f -> ratio = %e\n",
             moist,soil_con.max_moist[0],ratio);
          return( ERROR );
        } else {
          ratio = pow(ratio,(1.0 / (soil_con.b_infilt + 1.0)));
        }
      }
      tmp = max_infil*(1.0 - ratio);
    }
    //**********************************************************************
    //* Evaporate at potential rate, i.e., Eq.(10) in Liang's derivation.  *
    //**********************************************************************
    if (tmp >= max_infil) {
      evap = pot_soil_evap;
    } else {
      //********************************************************************
      //*  Compute As. 'As' is % area saturated, '1-As' is % area          *
      //*  that is unsaturated.                                            *
      //********************************************************************
      ratio = tmp/max_infil;
      ratio = 1.0 - ratio;
      if(ratio > 1.0) {
        printf("\n ARNO ERROR: EVAP RATIO GREATER THAN 1.0");
        return ( ERROR );
      } else {
        if(ratio < 0.0) {
          printf("\n ARNO ERROR: EVAP RATIO LESS THAN 0.0");
          return( ERROR );
        } else {
          if(ratio != 0.0)
            ratio = pow(ratio,soil_con.b_infilt);
        }
      }
      double as = 1 - ratio;
      //********************************************************************
      //*  Compute the beta function in the ARNO evaporation model using   *
      //*  the first 30 terms in the power expansion expression.           *
      //********************************************************************
      ratio = pow(ratio,(1.0/soil_con.b_infilt));
      double dummy = 1.0;
      for (int num_term = 1; num_term <= 30; num_term++) {
        double tmpsum = ratio;
        for (int i = 1; i < num_term; i++ ) tmpsum *= ratio;
        dummy += soil_con.b_infilt * tmpsum / (soil_con.b_infilt + num_term);
      }
      double beta_asp = as+(1.0-as)*(1.0-ratio)*dummy;

      //std::clog << "\npot_soil_evap:" << pot_soil_evap
      //          << "\nratio:"         << ratio
      //          << "\nas:"            << as
      //          << "\nbeta_asp:"      << beta_asp
      //          << "\ndummy:"         << dummy
      //          << "\n";

      evap = pot_soil_evap*beta_asp;

      //std::clog << "pot_soil_evap:" << pot_soil_evap << "\tbeta_asp:" << beta_asp << std::endl;
    }
    //***********************************************************************
    //*  Evaporation cannot exceed available soil moisture.                 *
    //*  Evaporation second soil layer = 0.0                                *
    //***********************************************************************
    //* only consider positive evaporation; we won't put limits on condensation *
    if (evap > 0.0) {
      if (moist > soil_con.resid_moist[0] * soil_con.depth[0] * 1000.) {
        //* there is liquid moisture available; cap evap at available liquid moisture *
        if (evap > moist -  soil_con.resid_moist[0] * soil_con.depth[0] * 1000.) {
          evap = moist -  soil_con.resid_moist[0] * soil_con.depth[0] * 1000.;
        }
      } else {
        //* no moisture available; cap evap at 0 *
        evap = 0.0;
      }
    }
    evap /= 1000. * SEC_PER_DAY;                                                 //mm/day to m/s
    return(evap);
}
