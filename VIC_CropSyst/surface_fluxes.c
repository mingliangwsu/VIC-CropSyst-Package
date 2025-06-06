#include <stdio.h>
#include <stdlib.h>
#include <vicNl.h>
#include <math.h>
#include <iostream>
#ifdef VIC_CROPSYST_VERSION
#include <irrigation_lib.h>
#include "VIC_crop_variable_codes.h"  /** 08132010 KJC the codes were not recognized without this*/
#include "crop/growth_stages.h" /**08172010 KJC **/
#if (VIC_CROPSYST_VERSION>=3)
#include "assert.h"
#include "agronomic/VIC_land_unit_C_interface.h"                                 //150616LML
#include "agronomic/VIC_land_unit_simulation.h"                                  //150702LML
#endif //VIC_CROPSYST_VERSION>=3
//RLN needed for alloc_2d_array
#endif //VIC_CROPSYST_VERSION

#include "pubtools.h"


static char vcid[] = "$Id: surface_fluxes.c,v 5.12.2.23 2012/10/25 23:15:58 vicadmin Exp $";
void update_aero_resist(const double iter_aero_resist_used[2], const double * const *aero_resist,
                        const int UnderStory, double **step_aero_resist);        //160518LML
#if CLOSE_ENERGY
#define MAX_ITER 10 /* Max number of iterations for total energy balance */
#else
#define MAX_ITER 0   /* No iterations */
#endif // CLOSE_ENERGY
#define GRND_TOL 0.001
#define OVER_TOL 0.001
int surface_fluxes(char                 overstory,
                   double               BareAlbedo,
                   double               height,
                   double               ice0,
                   double               moist0,
                   #if EXCESS_ICE
                   int                  SubsidenceUpdate,
                   double              *evap_prior_dry,
                   double              *evap_prior_wet,
                   #endif
                   double               mu,
                   double               surf_atten,
                   double              *Melt,
                   double              *Le,
                   double             **aero_resist,
                   double              *displacement,
                   double              *gauge_correction,
                   double              *out_prec,
                   double              *out_rain,
                   double              *out_snow,
                   double              *ref_height,
                   double              *roughness,
                   double              *snow_inflow,
                   double              *wind,
                   float               *root,
                   int                  Nbands,
                   int                  Ndist,
                   int                  Nlayers,
                   int                  Nveg,
                   int                  band,
                   int                  dp,
                   int                  iveg,
                   int                  rec,
                   int                  veg_class,
                   atmos_data_struct   *atmos,
                   dmy_struct          *dmy,
                   energy_bal_struct   *energy,
                   global_param_struct *gp,
                   cell_data_struct    *cell_dry,
                   cell_data_struct    *cell_wet,
                   snow_data_struct    *snow,
                   soil_con_struct     *soil_con,
                   veg_var_struct      *veg_var_dry,
                   veg_var_struct      *veg_var_wet,
                   float              lag_one,
                   float              sigma_slope,
                   float              fetch
#ifdef VIC_CROPSYST_VERSION
#if (VIC_CROPSYST_VERSION>=3)
                   ,veg_con_struct      *veg_con
#endif
#ifndef FULL_IRRIGATION
                   ,irrigation_pattern_struct *irrig_patt                         //keyvan 130604
#endif
#endif
                    )
/**********************************************************************
    surface_fluxes    Keith Cherkauer        February 29, 2000

  Formerly a part of full_energy.c this routine computes all surface
  fluxes, and solves the snow accumulation and ablation algorithm.
  Solutions are for the current snow band and vegetation type (these
  are defined in full_energy before the routine is called).

  modifications:
  10-06-00 modified to handle partial snow cover                KAC
  10-31-00 modified to iterate a solution for the exchange of
           energy between the snowpack and the ground surface.  KAC
  11-18-02 modified to add the effects of blowing snow.         LCB
  02-07-03 fixed indexing problem for sub-daily snow model within
           daily water balance VIC: hour (now hidx) is incremented
           by 1 rather than the sub-daily time step, so the atmospheric
           forcing data is now properly indexed.                KAC
  04-23-03 Indexing fix sent SNOW_STEP to calc_surf_energy_bal rather
           than the model time step, meaning that without snow the
           evaporation was computed for SNOW_STEP hours rather than a
           full day.  This was fixed by introducing step_inc to
           index the arrays, while step_dt keeps track of the correct
           time step.                                                  KAC
  10-May-04 Fixed initialization of canopyevap to initialize for every
        value of dist, rather than just dist 0.            TJB
  16-Jul-04 Added variables store_blowing_flux and store_surface_flux
        so that surface_flux and blowing_flux are calculated
        correctly over a model time step.                TJB
  16-Jul-04 Moved calculation of blowing_flux from this function
        into latent_heat_from_snow().                TJB
  05-Aug-04 Moved calculation of blowing_flux back into this function
        from latent_heat_from_snow().  Updated arg lists to
        calc_surf_energy_bal() and solve_snow() accordingly.    TJB
  28-Sep-04 Added aero_resist_used to store the aerodynamic resistance
        used in flux calculations.                    TJB
  04-Oct-04 Merged with Laura Bowling's updated lake model code.    TJB
  2006-Sep-23 Implemented flexible output configuration; moved tracking
              of rain and snow for output to this function.        TJB
  2006-Sep-26 Moved tracking of out_rain and out_snow to solve_snow.c.    TJB
  2006-Dec-20 Modified iteration loop variables to be more intuitive.    TJB
  2007-Apr-04 Modified to handle grid cell errors by returning to the
          main subroutine, rather than ending the simulation.    GCT/KAC
  24-Apr-07 Features included for IMPLICIT frozen soils option.        JCA
        (passing nrecs to  calc_surf_energy_bal)
  2007-Jul-03 Added iter_snow, iter_bare_energy, and iter_snow_energy
          structures so that canopy/understory iterations don't
          reset step_snow, etc.  This fixes a bug involving large
          water balance errors when model step = daily and snow
          step = sub-daily.                        TJB
  2007-Aug-17 Added features for EXCESS_ICE option.                     JCA
  2008-May-05 Changed moist from a scalar to an array (moist0).  Previously,
          when options.SNOW_BAND > 1, the value of moist computed
          for earlier bands was always overwritten by the value
          of moist computed for the final band (even if the final
          band had 0 area).                        KAC via TJB
  2008-Oct-23 In call to CalcBlowing(), replaced
            veg_lib[iveg].displacement[dmy[rec].month-1] and
            veg_lib[iveg].roughness[dmy[rec].month-1] with
          *displacement and *roughness.                LCB via TJB
  2009-Jan-16 Modified aero_resist_used and Ra_used to become arrays of
          two elements (surface and overstory); added
          options.AERO_RESIST_CANSNOW.                TJB
  2009-May-17 Added asat to cell_data.                    TJB
  2009-May-20 Changed "bare_*" to "soil_*", to make it clearer that
          these data structures refer to the energy balance at the
          soil surface, regardless of whether the surface is covered
          by snow or some veg or is totally bare.            TJB
  2009-Jun-09 Modified to use extension of veg_lib structure to contain
          bare soil information.                    TJB
  2009-Jun-09 Added call to compute_pot_evap() to compute potential
          evaporation for various land cover types.            TJB
  2009-Jun-09 Cell_data structure now only stores final aero_resist
          values (called "aero_resist").  Preliminary uncorrected
          aerodynamic resistances for current vegetation and various
          reference land cover types for use in potential evap
          calculations is stored in temporary array aero_resist.    TJB
  2009-Jun-19 Added T flag to indicate whether TFALLBACK occurred.    TJB
  2009-Jun-26 Simplified argument list of runoff() by passing all cell_data
          variables via a single reference to the cell data structure.    TJB
  2009-Sep-19 Added T fbcount to count TFALLBACK occurrences.        TJB
  2009-Nov-15 Removed ice0 and moist0 from argument list of solve_snow,
          since they are never used.                TJB
  2010-Apr-24 Added logic to handle case when aero_cond or aero_resist
          are very large or 0.                    TJB
  2010-Apr-26 Simplified argument lists for solve_snow() and
          snow_intercept().                        TJB
  2011-May-31 Removed options.GRND_FLUX.                TJB
  2012-Jan-16 Removed LINK_DEBUG code                    BN
  2012-Oct-25 Now call calc_atmos_energy_bal() whenever there is a canopy
          with snow, regardless of the setting of CLOSE_ENERGY.    CL via TJB
**********************************************************************/
{
  extern option_struct   options;
  extern global_param_struct global_param;

  #ifdef VIC_CROPSYST_VERSION
  #if VIC_CROPSYST_VERSION==2
  ,crops  //KJC 05272011
  #endif
  int veg_class_code = veg_con[iveg].VCS.veg_class_code;                         //180620LML fixed the index problem
  #endif

  int                    BISECT_OVER;
  int                    BISECT_UNDER;
  int                    ErrorFlag;
  int                    INCLUDE_SNOW = FALSE;
  int                    UNSTABLE_CNT;
  int                    UNSTABLE_SNOW = FALSE;
  int                    N_steps;
  int                    UnderStory;
  int                    dist;
  int                    hidx;     // index of initial element of atmos array
  int                    step_inc; // number of atmos array elements to skip per surface fluxes step
  int                    endhidx;  // index of final element of atmos array
  int                    step_dt;  // time length of surface fluxes step
  int                    lidx;
  int                    over_iter;
  int                    under_iter;
  int                    p,q;
  double                 Ls;
  double                 LongUnderIn; // inmoing LW to ground surface
  double                 LongUnderOut; // outgoing LW from ground surface
  double                 NetLongSnow; // net LW over snowpack
  double                 NetShortSnow; // net SW over understory
  double                 NetShortGrnd; // net SW over snow-free surface
  double                 OldTSurf; // previous snow surface temperature
  double                 ShortUnderIn; // incoming SW to understory
  double                 Tair; // air temperature
  double                 Tcanopy; // canopy air temperature
  double                 Tgrnd; // soil surface temperature
  double                 Tsurf; // ground surface temperature
  double                 VPDcanopy; // vapor pressure deficit in canopy/atmos
  double                 VPcanopy; // vapor pressure in canopy/atmos
  double                 coverage; // mid-step snow cover fraction
  double                 delta_coverage; // change in snow cover fraction
  double                 delta_snow_heat; // change in snowpack heat storage
  double                 last_Tcanopy;
  double                 last_snow_coverage; // previous snow covered area
  double                 last_snow_flux;
  double                 last_tol_under; // previous surface iteration tol
  double                 last_tol_over; // previous overstory iteration tol
  double                 ppt[2]; // precipitation/melt reaching soil surface
  double                 rainfall[2]; // rainfall
  double                 snowfall[2]; // snowfall
  double                 snow_flux; // heat flux through snowpack
  double                 snow_grnd_flux; // ground heat flux into snowpack
  double                 tol_under;
  double                 tol_over;
  double                *aero_resist_used;
  double                *pot_evap;
  double                *baseflow_dry;
  double                *baseflow_wet;
  double                *asat_dry;
  double                *asat_wet;
  double                *inflow_dry;
  double                *inflow_wet;
  double                *runoff_dry;
  double                *runoff_wet;
  layer_data_struct     *layer_dry;
  layer_data_struct     *layer_wet;

  // Step-specific quantities
  double                 step_Evap;
  double                 step_Wdew[2];
  double                 step_melt;
  double                 step_melt_energy;  /* energy used to reduce snow coverage */
  double                 step_out_prec;
  double                 step_out_rain;
  double                 step_out_snow;
  double                 step_ppt[2];
  double                 step_prec[2];

  // Quantities that need to be summed or averaged over multiple snow steps
  // energy structure
  double                 store_AlbedoOver;
  double                 store_AlbedoUnder;
  double                 store_AtmosLatent;
  double                 store_AtmosLatentSub;
  double                 store_AtmosSensible;
  double                 store_LongOverIn;
  double                 store_LongUnderIn;
  double                 store_LongUnderOut;
  double                 store_NetLongAtmos;
  double                 store_NetLongOver;
  double                 store_NetLongUnder;
  double                 store_NetShortAtmos;
  double                 store_NetShortGrnd;
  double                 store_NetShortOver;
  double                 store_NetShortUnder;
  double                 store_ShortOverIn;
  double                 store_ShortUnderIn;
  double                 store_advected_sensible;
  double                 store_advection;
  double                 store_canopy_advection;
  double                 store_canopy_latent;
  double                 store_canopy_latent_sub;
  double                 store_canopy_sensible;
  double                 store_canopy_refreeze;
  double                 store_deltaCC;
  double                 store_deltaH;
  double                 store_fusion;
  double                 store_grnd_flux;
  double                 store_latent;
  double                 store_latent_sub;
  double                 store_melt_energy;
  double                 store_refreeze_energy;
  double                 store_sensible;
  double                 store_snow_flux;
  // snow structure
  double                 store_canopy_vapor_flux;
  double                 store_melt;
  double                 store_vapor_flux;
  double                 store_blowing_flux;
  double                 store_surface_flux;
  // veg_var structure
  double                 store_canopyevap[2];
  double                 store_throughfall[2];
  // cell structure
  double                 store_layerevap[2][MAX_LAYERS];
  double                 store_ppt[2];
  double                 store_aero_cond_used[2];
  double                 store_pot_evap[N_PET_TYPES];

  #if (VIC_CROPSYST_VERSION>=3)
  double                 store_soilevap[2];
  double                 store_irrigation_netdemand[2];                          //180327LML
  double                 store_irrigation_water[2];                              //150702LML
  double                 store_net_irrigation_water[2];                          //150702LML
  double                 store_irrigation_actual[2];                             //150714LML
  double                 store_irrigation_evap[2];                               //150702LML
  double                 store_irrigation_runoff[2];                             //150702LML
  double                 store_irrigation_intercept[2];                          //150702LML
  double                 store_irrigation_intercept_evap[2];                     //150702LML
  double                 store_pot_tanspiration[2];                              //150702LML
#endif


  // Structures holding values for current snow step
  energy_bal_struct      snow_energy; // energy fluxes at snowpack surface
  energy_bal_struct      soil_energy; // energy fluxes at soil surface
  veg_var_struct         snow_veg_var[2]; // veg fluxes/storages in presence of snow
  veg_var_struct         soil_veg_var[2]; // veg fluxes/storages in soil energy balance
  snow_data_struct       step_snow;
  layer_data_struct      step_layer[2][MAX_LAYERS];

  // Structures holding values for current iteration
  energy_bal_struct      iter_snow_energy; // energy fluxes at snowpack surface
  energy_bal_struct      iter_soil_energy; // energy fluxes at soil surface
  veg_var_struct         iter_snow_veg_var[2]; // veg fluxes/storages in presence of snow
  veg_var_struct         iter_soil_veg_var[2]; // veg fluxes/storages in soil energy balance
  snow_data_struct       iter_snow;
  layer_data_struct      iter_layer[2][MAX_LAYERS];
  double                 iter_aero_resist[3];
  double                 iter_aero_resist_used[2];
  double                 iter_pot_evap[N_PET_TYPES];                             //mm per gp->dt

  // handle bisection of understory solution
  double store_tol_under;
  double A_tol_under;
  double B_tol_under;
  double A_snow_flux;
  double B_snow_flux;

  // handle bisection of overstory solution
  double store_tol_over;
  double A_tol_over;
  double B_tol_over;
  double A_Tcanopy;
  double B_Tcanopy;

  /***********************************************************************
    Set temporary variables for convenience
  ***********************************************************************/
  aero_resist_used = cell_wet->aero_resist;
  baseflow_dry = &(cell_dry->baseflow);
  baseflow_wet = &(cell_wet->baseflow);
  asat_dry = &(cell_dry->asat);
  asat_wet = &(cell_wet->asat);
  pot_evap = cell_wet->pot_evap;
  inflow_dry = &(cell_dry->inflow);
  inflow_wet = &(cell_wet->inflow);
  runoff_dry = &(cell_dry->runoff);
  runoff_wet = &(cell_wet->runoff);
  layer_dry = cell_dry->layer;
  layer_wet = cell_wet->layer;

  double **step_aero_resist = alloc_2d_array<double>(N_PET_TYPES, 2, "step_aero_resist");
  //***********************************************************************
  //  Set temporary variables - preserves original values until iterations
  //  are completed
  //***********************************************************************
  energy->advection       = 0;
  energy->deltaCC         = 0;
  if ( snow->swq > 0 ) snow_flux = energy->snow_flux;
  else                 snow_flux = -(energy->grnd_flux + energy->deltaH + energy->fusion);
  energy->refreeze_energy = 0;
  coverage                = snow->coverage;
  snow_energy             = (*energy);
  soil_energy             = (*energy);
  snow_veg_var[WET]       = (*veg_var_wet);
  snow_veg_var[DRY]       = (*veg_var_dry);
  soil_veg_var[WET]       = (*veg_var_wet);
  soil_veg_var[DRY]       = (*veg_var_dry);
  step_snow               = (*snow);
  for ( lidx = 0; lidx < Nlayers; lidx++ ) {
    step_layer[WET][lidx] = layer_wet[lidx];
    step_layer[DRY][lidx] = layer_dry[lidx];
  }
  for ( lidx = 0; lidx < Nlayers; lidx++ ) {
    step_layer[WET][lidx].evap = 0;
    step_layer[DRY][lidx].evap = 0;
  }

  #if (VIC_CROPSYST_VERSION>=3)
  int iscrop_return = iscrop(veg_con[iveg].VCS.veg_class_code);
  for (int dist = 0; dist < Ndist; dist++) step_layer[dist][0].VCS.evap_bare_soil = 0.0;                                    //150626LML
  #endif
  soil_veg_var[WET].canopyevap = 0;
  soil_veg_var[DRY].canopyevap = 0;
  snow_veg_var[WET].canopyevap = 0;
  snow_veg_var[DRY].canopyevap = 0;
  soil_veg_var[WET].throughfall = 0;
  soil_veg_var[DRY].throughfall = 0;
  snow_veg_var[WET].throughfall = 0;
  snow_veg_var[DRY].throughfall = 0;
  //********************************
  //  Set-up sub-time step controls
  //  (May eventually want to set this up so that it is also true
  //  if frozen soils are present)
  //********************************
  step_inc  = 1;
  if((snow->swq > 0 || snow->snow_canopy > 0 || atmos->snowflag[NR])
      #if VIC_CROPSYST_VERSION==2
      && !iscrop_return/*keyvan added from the && part*/
      #endif
      ) { //this is for implementing the first solution
                                                                                 //*** Keyvan
                                                                                 //here I see three solutions:
                                                                                 //1-when we have crop skip snow hourly algorithm
                                                                                 //2- we can call crop syst every hour and average outputs
                                                                                 //3- we can average values that we pass to the crop model and then call crop model once a day
                                                                                 //****
    hidx      = 0;
    endhidx   = hidx + NF;
    step_dt   = options.SNOW_STEP;
  } else {
    hidx      = NR;
    endhidx   = hidx + step_inc;
    step_dt   = global_param.dt;
  }
  //*******************************************
  //  Initialize sub-model time step variables
  //*******************************************
  // energy structure
  store_AlbedoOver        = 0;
  store_AlbedoUnder       = 0;
  store_AtmosLatent       = 0;
  store_AtmosLatentSub    = 0;
  store_AtmosSensible     = 0;
  store_LongOverIn        = 0;
  store_LongUnderIn       = 0;
  store_LongUnderOut      = 0;
  store_NetLongAtmos      = 0;
  store_NetLongOver       = 0;
  store_NetLongUnder      = 0;
  store_NetShortAtmos     = 0;
  store_NetShortGrnd      = 0;
  store_NetShortOver      = 0;
  store_NetShortUnder     = 0;
  store_ShortOverIn       = 0;
  store_ShortUnderIn      = 0;
  store_advected_sensible = 0;
  store_advection         = 0;
  store_canopy_advection  = 0;
  store_canopy_latent     = 0;
  store_canopy_latent_sub = 0;
  store_canopy_sensible   = 0;
  store_canopy_refreeze   = 0;
  store_deltaCC           = 0;
  store_deltaH            = 0;
  store_fusion            = 0;
  store_grnd_flux         = 0;
  store_latent            = 0;
  store_latent_sub        = 0;
  store_melt_energy       = 0;
  store_refreeze_energy   = 0;
  store_sensible          = 0;
  store_snow_flux         = 0;
  // snow structure
  last_snow_coverage      = snow->coverage;
  store_canopy_vapor_flux = 0;
  store_melt              = 0;
  store_vapor_flux        = 0;
  store_surface_flux      = 0;
  store_blowing_flux      = 0;
  // veg_var and cell structures
  for ( dist = 0; dist < Ndist; dist++ ) {
    store_throughfall[dist] = 0.;
    store_canopyevap[dist]  = 0.;
    for ( lidx = 0; lidx < options.Nlayer; lidx++ ) {
      store_layerevap[dist][lidx] = 0.;
    }
  }
  step_Wdew[WET]          = veg_var_wet->Wdew;
  step_Wdew[DRY]          = veg_var_wet->Wdew;
  // misc
  store_ppt[WET]          = 0;
  store_ppt[DRY]          = 0;
  step_prec[DRY]          = 0;
  store_aero_cond_used[0] = 0;
  store_aero_cond_used[1] = 0;
  (*snow_inflow)          = 0;
  for (p=0; p<N_PET_TYPES; p++)
    store_pot_evap[p] = 0;

#if (VIC_CROPSYST_VERSION>=3)
  for (dist = 0; dist < Ndist; dist ++) {
    store_soilevap[dist]                    = 0;
    store_irrigation_netdemand[dist]        = 0;
    store_irrigation_water[dist]            = 0;
    store_net_irrigation_water[dist]        = 0;
    store_irrigation_actual[dist]           = 0;
    store_irrigation_evap[dist]             = 0;
    store_irrigation_runoff[dist]           = 0;
    store_irrigation_intercept[dist]        = 0;
    store_irrigation_intercept_evap[dist]   = 0;
    store_pot_tanspiration[dist]            = 0;
  }
#endif




  N_steps = 0;
  /*************************
    Compute surface fluxes
  *************************/
  do {
    #ifdef VIC_CROPSYST_VERSION
    clear_cell_irrigation_water_fluxes(cell_wet);
    clear_cell_irrigation_water_fluxes(cell_dry);
    #endif
    /** Solve energy balance for all sub-model time steps **/
    /* set air temperature and precipitation for this snow band */
    Tair = atmos->air_temp[hidx] + soil_con->Tfactor[band];
    step_prec[WET] = atmos->prec[hidx] / mu * soil_con->Pfactor[band];
    Tgrnd = energy->T[0];
    // initialize canopy terms
    Tcanopy     = Tair;
    VPcanopy    = atmos->vp[hidx];
    VPDcanopy   = atmos->vpd[hidx];
    over_iter   = 0;
    tol_over    = 999;
    last_Tcanopy      = 999;
    last_snow_flux    = 999;
    // initialize bisection startup
    BISECT_OVER  = FALSE;
    //160526LML A_tol_over   = 999;
    //160526LML B_tol_over   = 999;
    // Compute mass flux of blowing snow
    if( !overstory && options.BLOWING && step_snow.swq > 0.) {
      double lag_one        = veg_con[iveg].lag_one;
      double fetch          = veg_con[iveg].fetch;
      double sigma_slope    = veg_con[iveg].sigma_slope;
      Ls = (677. - 0.07 * step_snow.surf_temp) * JOULESPCAL * GRAMSPKG;
      step_snow.blowing_flux = CalcBlowingSnow((double) step_dt, Tair,
                        step_snow.last_snow, step_snow.surf_water,
                        wind[2], Ls, atmos->density[hidx],
                        atmos->vp[hidx], roughness[2],
                        ref_height[2], step_snow.depth,
                        lag_one, sigma_slope,
                        step_snow.surf_temp, iveg, Nveg, fetch,
                        displacement[1], roughness[1],
                        &step_snow.transport);
      if ( (int)step_snow.blowing_flux == ERROR )
        return ( ERROR );
      step_snow.blowing_flux *= step_dt * SECPHOUR / RHO_W;                      /* m/time step */
    } else {
      step_snow.blowing_flux = 0.0;
    }

    do {
      /** Iterate for overstory solution **/
      over_iter++;
      last_tol_over  = tol_over;
      under_iter = 0;
      tol_under  = 999;
      UnderStory = 999;
      UNSTABLE_CNT = 0;
      // bisect understory
      BISECT_UNDER = FALSE;
      A_tol_under  = 999;
      //160526LML B_tol_under  = 999;
      store_tol_under = 999;
      //160526LML A_tol_over   = 999;
      //160526LML B_tol_over   = 999;
      store_tol_over = 999;
      do {
        /** Iterate for understory solution - itererates to find snow flux **/
        under_iter++;
        last_tol_under = tol_under;
        if ( last_Tcanopy != 999 ) Tcanopy = (last_Tcanopy + Tcanopy) / 2.;
        last_Tcanopy       = Tcanopy;
        //160526LML A_tol_over         = store_tol_over;
        //160526LML A_Tcanopy          = Tcanopy;
        // update understory energy balance terms for iteration
        if ( last_snow_flux != 999 ) {
          if ( ( fabs(store_tol_under) > fabs(A_tol_under)
                 && A_tol_under != 999
                 && fabs(store_tol_under - A_tol_under) > 1. )
               || tol_under < 0 ) { // stepped the correct way
            UNSTABLE_CNT++;
            if ( UNSTABLE_CNT > 3 || tol_under < 0 )
              UNSTABLE_SNOW = TRUE;
          } else if ( !INCLUDE_SNOW ) { // stepped the wrong way
            snow_flux = (last_snow_flux + iter_soil_energy.snow_flux) / 2.;
          }
        }
        last_snow_flux = snow_flux;
        A_tol_under    = store_tol_under;
        //160526LML A_snow_flux    = snow_flux;
        snow_grnd_flux = -snow_flux;
        // Initialize structures for new iteration
        iter_snow_energy        = snow_energy;
        iter_soil_energy        = soil_energy;
        iter_snow               = step_snow;
        for (int dist = 0; dist < Ndist; dist ++ ) {
          iter_snow_veg_var[dist]       = snow_veg_var[dist];
          iter_soil_veg_var[dist]       = soil_veg_var[dist];
          iter_snow_veg_var[dist].Wdew  = step_Wdew[dist];
          iter_soil_veg_var[dist].Wdew  = step_Wdew[dist];
          iter_snow_veg_var[dist].canopyevap = 0;
          iter_soil_veg_var[dist].canopyevap = 0;
          for (int lidx = 0; lidx < Nlayers; lidx ++ ) {
            iter_layer[dist][lidx] = step_layer[dist][lidx];
            iter_layer[dist][lidx].evap = 0;
          }
        }
        for (int q = 0; q < 3; q++) {
          iter_aero_resist[q] = aero_resist[N_PET_TYPES][q];
        }
        iter_aero_resist_used[0]    = aero_resist_used[0];
        iter_aero_resist_used[1]    = aero_resist_used[1];
        iter_snow.canopy_vapor_flux = 0;
        iter_snow.vapor_flux        = 0;
        iter_snow.surface_flux      = 0;
        /* iter_snow.blowing_flux has already been reset to step_snow.blowing_flux */
        LongUnderOut       = iter_soil_energy.LongUnderOut;
        /** Solve snow accumulation, ablation and interception **/
        /*double snowmelt_crop_class=
        #ifdef CROPSYST_ON
        iscrop_return ? (CROP_VEG_CLASS-1) :
        #endif
        veg_class;*/
        double dryFrac = -1;                                                     //160526LML
        step_melt = solve_snow(overstory, BareAlbedo, LongUnderOut,
                   global_param.MIN_RAIN_TEMP, global_param.MAX_SNOW_TEMP,
                   Tcanopy, Tgrnd, Tair, dp, mu,
                   step_prec[WET], snow_grnd_flux, global_param.wind_h,
                   &energy->AlbedoUnder, &step_Evap, Le,
                   &LongUnderIn, &NetLongSnow, &NetShortGrnd,
                   &NetShortSnow, &ShortUnderIn, &OldTSurf,
                   iter_aero_resist, iter_aero_resist_used,
                   &coverage, &delta_coverage,
                   &delta_snow_heat, displacement,
                   gauge_correction, &step_melt_energy,
                   &step_out_prec, &step_out_rain, &step_out_snow,
                   step_ppt, rainfall, ref_height,
                   roughness, snow_inflow, snowfall, &surf_atten,
                   wind, root, UNSTABLE_SNOW, options.Nnode,
                   Nveg, iveg, band, step_dt, rec, hidx, veg_class,
                   &UnderStory, &dryFrac, dmy, atmos, &(iter_snow_energy),
                   iter_layer[DRY], iter_layer[WET], &(iter_snow),
                   soil_con, &(iter_snow_veg_var[DRY]),
                   &(iter_snow_veg_var[WET])
#ifdef VIC_CROPSYST_VERSION
                   #if (VIC_CROPSYST_VERSION>=3)
                   ,veg_con
                   ,iter_soil_energy.NetLongAtmos                                //150624LML
                   #endif
                   ,cell_wet
                   ,cell_dry
                   //150929LML ,current_crop/*150702LML crops*/
                   ,veg_class_code
                   ,aero_resist
#endif
                    );
        // iter_snow_energy.sensible + iter_snow_energy.latent + iter_snow_energy.latent_sub + NetShortSnow + NetLongSnow + ( snow_grnd_flux + iter_snow_energy.advection - iter_snow_energy.deltaCC + iter_snow_energy.refreeze_energy + iter_snow_energy.advected_sensible ) * step_snow.coverage
        if ( step_melt == ERROR ) return (ERROR);
        //* Check that the snow surface temperature was estimated, if not
        //prepare to include thin snowpack in the estimation of the
        //snow-free surface energy balance
        if ( ( iter_snow.surf_temp == 999 || UNSTABLE_SNOW ) && (iter_snow.swq > 0 )) {
          INCLUDE_SNOW                  = UnderStory + 1;
          iter_soil_energy.advection    = iter_snow_energy.advection;
          iter_snow.surf_temp           = step_snow.surf_temp;
          step_melt_energy          = 0;
        } else {
          INCLUDE_SNOW = FALSE;
        }
        //**************************************************
        //  Solve Energy Balance Components at Soil Surface
        //**************************************************
        Tsurf = calc_surf_energy_bal((*Le), LongUnderIn, NetLongSnow,
                     NetShortGrnd, NetShortSnow, OldTSurf,
                     ShortUnderIn, iter_snow.albedo,
                     iter_snow_energy.latent,
                     iter_snow_energy.latent_sub,
                     iter_snow_energy.sensible,
                     Tcanopy, VPDcanopy,
                     VPcanopy, delta_coverage, dp,
                     ice0, step_melt_energy, moist0, mu,
                     iter_snow.coverage,
                     (step_snow.depth + iter_snow.depth) / 2.,
                     BareAlbedo, surf_atten,
                     iter_aero_resist, iter_aero_resist_used,
                     displacement, &step_melt, step_ppt,
                     rainfall, ref_height, roughness,
                     #if SPATIAL_SNOW
                     snowfall,
                     #endif
                     wind, root, INCLUDE_SNOW,
                     UnderStory, options.Nnode, Nveg, band,
                     step_dt, hidx, iveg,
                     (int)overstory, rec, veg_class, atmos,
                     dmy, &iter_soil_energy,
                     iter_layer[DRY], iter_layer[WET],
                     &(iter_snow), soil_con,
                     &iter_soil_veg_var[DRY], &iter_soil_veg_var[WET], global_param.nrecs
                     #ifdef VIC_CROPSYST_VERSION
                     #if (VIC_CROPSYST_VERSION>=3)
                     ,veg_con
                     #endif
                     ,cell_wet//Keyvan 11202012
                     ,cell_dry                                                 //150929LML
                     //150929LML ,current_crop/*150702 crops*/  //Keyvan 11202012
                     ,veg_class_code //Keyvan 130226
                     ,aero_resist
                     #ifndef FULL_IRRIGATION
                     ,irrig_patt
                     #endif
                     #endif //VIC_CROPSYST_VERSION
                     ); ///changed the veg class back to the original form
        if ( (int)Tsurf == ERROR ) return ( ERROR );                             // Return error flag to skip rest of grid cell
        if ( INCLUDE_SNOW ) step_ppt[WET] += step_melt;                  //* store melt from thin snowpack
        //*****************************************
        //  Compute energy balance with atmosphere
        //*****************************************
        if ( iter_snow.snow && overstory ) {
          // do this if overstory is active and energy balance is closed
          Tcanopy = calc_atmos_energy_bal(iter_snow_energy.canopy_sensible,
                                          iter_soil_energy.sensible,
                                          iter_snow_energy.canopy_latent,
                                          iter_soil_energy.latent,
                                          iter_snow_energy.canopy_latent_sub,
                                          iter_soil_energy.latent_sub,
                                          (*Le),
                                          iter_snow_energy.NetLongOver,
                                          iter_soil_energy.NetLongUnder,
                                          iter_snow_energy.NetShortOver,
                                          iter_soil_energy.NetShortUnder,
                                          iter_aero_resist_used[1],
                                          Tair,atmos->density[hidx],
                                          atmos->vp[hidx], atmos->vpd[hidx],
                                          &iter_soil_energy.AtmosError,
                                          &iter_soil_energy.AtmosLatent,
                                          &iter_soil_energy.AtmosLatentSub,
                                          &iter_soil_energy.NetLongAtmos,
                                          &iter_soil_energy.NetShortAtmos,
                                          &iter_soil_energy.AtmosSensible,
                                          &VPcanopy, &VPDcanopy,
                                          &iter_soil_energy.Tcanopy_fbflag,
                                          &iter_soil_energy.Tcanopy_fbcount);
          //* iterate to find Tcanopy which will solve the atmospheric energy
          // balance.  Since I do not know vp in the canopy, use the
          // sum of latent heats from the ground and foliage, and iterate
          // on the temperature used for the sensible heat flux from the
          // canopy air to the mixing level
          if ( (int)Tcanopy == ERROR ) return ( ERROR );                         // Return error flag to skip rest of grid cell
        } else {
          // else put surface fluxes into atmospheric flux storage so that the model will continue to function
          iter_soil_energy.AtmosLatent    = iter_soil_energy.latent;
          iter_soil_energy.AtmosLatentSub = iter_soil_energy.latent_sub;
          iter_soil_energy.AtmosSensible  = iter_soil_energy.sensible;
          iter_soil_energy.NetLongAtmos   = iter_soil_energy.NetLongUnder;
          iter_soil_energy.NetShortAtmos  = iter_soil_energy.NetShortUnder;
        }
        iter_soil_energy.Tcanopy = Tcanopy;
        iter_snow_energy.Tcanopy = Tcanopy;
        //*****************************************
        //  Compute iteration tolerance statistics
        //*****************************************
        // compute understory tolerance
        if ( INCLUDE_SNOW || ( iter_snow.swq == 0 && delta_coverage == 0 ) ) {
          store_tol_under = 0;
          tol_under       = 0;
        } else {
          store_tol_under = snow_flux - iter_soil_energy.snow_flux;
          tol_under       = fabs(store_tol_under);
        }
        if ( fabs( tol_under - last_tol_under ) < GRND_TOL && tol_under > 1. )
          tol_under = -999;
        // compute overstory tolerance
        if ( overstory && iter_snow.snow ) {
          store_tol_over = Tcanopy - last_Tcanopy;
          tol_over       = fabs( store_tol_over );
        } else {
          store_tol_over = 0;
          tol_over       = 0;
        }
      } while ( ( fabs( tol_under - last_tol_under ) > GRND_TOL )
                && ( tol_under != 0 ) && (under_iter < MAX_ITER) );
    } while ( ( fabs( tol_over - last_tol_over ) > OVER_TOL
                && overstory )
              && ( tol_over != 0 )
              && (over_iter < MAX_ITER) );
    //**************************************
    //  Compute Potential Evap
    //**************************************
    update_aero_resist(iter_aero_resist_used,aero_resist,UnderStory,step_aero_resist); //160518LML
    //compute pot_evap
    compute_pot_evap(veg_class, dmy, rec, global_param.dt, atmos->shortwave[hidx],
                     iter_soil_energy.NetLongAtmos, Tair, VPDcanopy,
                     soil_con->elevation, step_aero_resist, iter_pot_evap
                     #ifdef VIC_CROPSYST_VERSION
                     ,(iscrop_return?cell_wet->VCS.pot_evap_daily:0)               //150616LML
                     ,(double)step_dt
                     #endif
                     );
    //**************************************
    //  Store sub-model time step variables
    //**************************************
    snow_energy = iter_snow_energy;
    soil_energy = iter_soil_energy;
    for (int dist = 0; dist < Ndist; dist++) {
      snow_veg_var[dist] = iter_snow_veg_var[dist];
      soil_veg_var[dist] = iter_soil_veg_var[dist];


      //std::clog << "dist:" << dist
      //          << "\titer_soil_veg_var_canopy_evap:" << iter_soil_veg_var[dist].canopyevap
      //          << std::endl;

    }
    step_snow = iter_snow;
    for (int lidx = 0; lidx < options.Nlayer; lidx++) {
      for (int dist = 0; dist < Ndist; dist++)
        step_layer[dist][lidx] = iter_layer[dist][lidx];
    }
    for (int dist = 0; dist < Ndist; dist++) {
      cell_data_struct *cell = (dist == WET) ? cell_wet : cell_dry;                                      //150929LML
      if(iveg != Nveg) {
        if(step_snow.snow) {
          store_throughfall[dist] += snow_veg_var[dist].throughfall;
          store_canopyevap[dist]  += snow_veg_var[dist].canopyevap;
          soil_veg_var[dist].Wdew      = snow_veg_var[dist].Wdew;
        } else {
          store_throughfall[dist] += soil_veg_var[dist].throughfall;
          store_canopyevap[dist]  += soil_veg_var[dist].canopyevap;
          snow_veg_var[dist].Wdew      = soil_veg_var[dist].Wdew;
        }
        step_Wdew[dist] = soil_veg_var[dist].Wdew;
      }
      for (int lidx = 0; lidx < options.Nlayer; lidx++)
        store_layerevap[dist][lidx] += step_layer[dist][lidx].evap;
      #if (VIC_CROPSYST_VERSION>=3)
      store_soilevap[dist] += step_layer[dist][0].VCS.evap_bare_soil;
      if (iscrop_return) {
#ifdef __BCPLUSPLUS__
//190130 RLN compiler not finding irrigation_netdemand
#else
        if (N_steps==0) store_irrigation_netdemand[dist]        += cell->VCS.irrigation_netdemand;          //180327LML
#endif
        if (N_steps==0) store_irrigation_evap[dist]             += cell->VCS.evap_from_irrigation_syst;     //150702LML
        if (N_steps==0) store_irrigation_water[dist]            += cell->VCS.irrigation_water;        //150702LML
        //if (N_steps==0) store_net_irrigation_water[dist]        += cell->VCS.net_irrigation;
        if (N_steps==0) store_irrigation_actual[dist]           += cell->VCS.actual_irrigation_reach_ground_amount;      //150714LML
        if (N_steps==0) store_irrigation_runoff[dist]           += cell->VCS.irrigation_runoff;             //150702LML
        if (N_steps==0) store_irrigation_intercept[dist]        += cell->VCS.intercepted_irrigation;        //150702LML
        if (N_steps==0) store_irrigation_intercept_evap[dist]   += cell->VCS.evap_from_irrigation_intercept;//150702LML
        if (N_steps==0) store_pot_tanspiration[dist]            += cell->VCS.potential_transpir;            //150702LML
      }
      #endif
      store_ppt[dist] += step_ppt[dist];
    } //dist
    if (iter_aero_resist_used[0]>0) store_aero_cond_used[0] += 1. / iter_aero_resist_used[0];
    else                            store_aero_cond_used[0] += param.HUGE_RESIST;
    if (iter_aero_resist_used[1]>0) store_aero_cond_used[1] += 1. / iter_aero_resist_used[1];
    else                            store_aero_cond_used[1] += param.HUGE_RESIST;
    if(iveg != Nveg) store_canopy_vapor_flux += step_snow.canopy_vapor_flux;
    store_melt         += step_melt;
    store_vapor_flux   += step_snow.vapor_flux;
    store_surface_flux += step_snow.surface_flux;
    store_blowing_flux += step_snow.blowing_flux;
    out_prec[0] += step_out_prec * mu;
    out_rain[0] += step_out_rain * mu;
    out_snow[0] += step_out_snow * mu;
    if ( INCLUDE_SNOW ) {
      /* copy needed flux terms to the snowpack */
      snow_energy.advected_sensible  = soil_energy.advected_sensible;
      snow_energy.advection          = soil_energy.advection;
      snow_energy.deltaCC            = soil_energy.deltaCC;
      snow_energy.latent             = soil_energy.latent;
      snow_energy.latent_sub         = soil_energy.latent_sub;
      snow_energy.refreeze_energy    = soil_energy.refreeze_energy;
      snow_energy.sensible           = soil_energy.sensible;
      snow_energy.snow_flux          = soil_energy.snow_flux;
    }
    store_AlbedoOver        += snow_energy.AlbedoOver;
    store_AlbedoUnder       += soil_energy.AlbedoUnder;
    store_AtmosLatent       += soil_energy.AtmosLatent;
    store_AtmosLatentSub    += soil_energy.AtmosLatentSub;
    store_AtmosSensible     += soil_energy.AtmosSensible;
    store_LongOverIn        += snow_energy.LongOverIn;
    store_LongUnderIn       += LongUnderIn;
    store_LongUnderOut      += soil_energy.LongUnderOut;
    store_NetLongAtmos      += soil_energy.NetLongAtmos;
    store_NetLongOver       += snow_energy.NetLongOver;
    store_NetLongUnder      += soil_energy.NetLongUnder;
    store_NetShortAtmos     += soil_energy.NetShortAtmos;
    store_NetShortGrnd      += NetShortGrnd;
    store_NetShortOver      += snow_energy.NetShortOver;
    store_NetShortUnder     += soil_energy.NetShortUnder;
    store_ShortOverIn       += snow_energy.ShortOverIn;
    store_ShortUnderIn      += soil_energy.ShortUnderIn;
    store_canopy_advection  += snow_energy.canopy_advection;
    store_canopy_latent     += snow_energy.canopy_latent;
    store_canopy_latent_sub += snow_energy.canopy_latent_sub;
    store_canopy_sensible   += snow_energy.canopy_sensible;
    store_canopy_refreeze   += snow_energy.canopy_refreeze;
    store_deltaH            += soil_energy.deltaH;
    store_fusion            += soil_energy.fusion;
    store_grnd_flux         += soil_energy.grnd_flux;
    store_latent            += soil_energy.latent;
    store_latent_sub        += soil_energy.latent_sub;
    store_melt_energy       += step_melt_energy;
    store_sensible          += soil_energy.sensible;
    if ( step_snow.swq == 0 && INCLUDE_SNOW ) {
      if ( last_snow_coverage == 0 && step_prec ) last_snow_coverage = 1;
      store_advected_sensible += snow_energy.advected_sensible  * last_snow_coverage;
      store_advection         += snow_energy.advection          * last_snow_coverage;
      store_deltaCC           += snow_energy.deltaCC            * last_snow_coverage;
      store_snow_flux         += soil_energy.snow_flux          * last_snow_coverage;
      store_refreeze_energy   += snow_energy.refreeze_energy    * last_snow_coverage;
    } else if ( step_snow.snow || INCLUDE_SNOW ) {
      double newcov = step_snow.coverage + delta_coverage;
      store_advected_sensible += snow_energy.advected_sensible  * newcov;
      store_advection         += snow_energy.advection          * newcov;
      store_deltaCC           += snow_energy.deltaCC            * newcov;
      store_snow_flux         += soil_energy.snow_flux          * newcov;
      store_refreeze_energy   += snow_energy.refreeze_energy    * newcov;
    }
    for (int p = 0; p < N_PET_TYPES; p++) store_pot_evap[p] += iter_pot_evap[p];
    /* increment time step */
    N_steps ++;
    hidx += step_inc;

    /*std::clog << "hidx:"        << hidx
              << "\tebdidx:"    << endhidx
              << std::endl;*/

  } while (hidx < endhidx);
  //************************************************
  //  Store snow variables for sub-model time steps
  //************************************************
  (*snow)                 = step_snow;
  snow->vapor_flux        = store_vapor_flux;
  snow->blowing_flux      = store_blowing_flux;
  snow->surface_flux      = store_surface_flux;
  snow->canopy_vapor_flux = store_canopy_vapor_flux;
  (*Melt) = store_melt;
  snow->melt = store_melt;
  for (int dist = 0; dist < Ndist; dist++) ppt[dist] = store_ppt[dist];
  //******************************************************
  //  Store energy flux averages for sub-model time steps
  //******************************************************
  (*energy) = soil_energy;
  energy->AlbedoOver        = store_AlbedoOver / (double)N_steps;
  energy->AlbedoUnder       = store_AlbedoUnder / (double)N_steps;
  energy->AtmosLatent       = store_AtmosLatent / (double)N_steps;
  energy->AtmosLatentSub    = store_AtmosLatentSub / (double)N_steps;
  energy->AtmosSensible     = store_AtmosSensible / (double)N_steps;
  energy->LongOverIn        = store_LongOverIn / (double)N_steps;
  energy->LongUnderIn       = store_LongUnderIn / (double)N_steps;
  energy->LongUnderOut      = store_LongUnderOut / (double)N_steps;
  energy->NetLongAtmos      = store_NetLongAtmos / (double)N_steps;
  energy->NetLongOver       = store_NetLongOver / (double)N_steps;
  energy->NetLongUnder      = store_NetLongUnder / (double)N_steps;
  energy->NetShortAtmos     = store_NetShortAtmos / (double)N_steps;
  energy->NetShortGrnd      = store_NetShortGrnd / (double)N_steps;
  energy->NetShortOver      = store_NetShortOver / (double)N_steps;
  energy->NetShortUnder     = store_NetShortUnder / (double)N_steps;
  energy->ShortOverIn       = store_ShortOverIn / (double)N_steps;
  energy->ShortUnderIn      = store_ShortUnderIn / (double)N_steps;
  energy->advected_sensible = store_advected_sensible / (double)N_steps;
  energy->canopy_advection  = store_canopy_advection / (double)N_steps;
  energy->canopy_latent     = store_canopy_latent / (double)N_steps;
  energy->canopy_latent_sub = store_canopy_latent_sub / (double)N_steps;
  energy->canopy_refreeze   = store_canopy_refreeze / (double)N_steps;
  energy->canopy_sensible   = store_canopy_sensible / (double)N_steps;
  energy->deltaH            = store_deltaH / (double)N_steps;
  energy->fusion            = store_fusion / (double)N_steps;
  energy->grnd_flux         = store_grnd_flux / (double)N_steps;
  energy->latent            = store_latent / (double)N_steps;
  energy->latent_sub        = store_latent_sub / (double)N_steps;
  energy->melt_energy       = store_melt_energy / (double)N_steps;
  energy->sensible          = store_sensible / (double)N_steps;
  if (snow->snow || INCLUDE_SNOW) {
    energy->advection       = store_advection / (double)N_steps;
    energy->deltaCC         = store_deltaCC / (double)N_steps;
    energy->refreeze_energy = store_refreeze_energy / (double)N_steps;
    energy->snow_flux       = store_snow_flux / (double)N_steps;
  }
  energy->Tfoliage          = snow_energy.Tfoliage;
  energy->Tfoliage_fbflag   = snow_energy.Tfoliage_fbflag;
  energy->Tfoliage_fbcount  = snow_energy.Tfoliage_fbcount;
  //**********************************************************
  //  Store vegetation variable sums for sub-model time steps
  //**********************************************************
  if(iveg != Nveg) {
      veg_var_wet->throughfall = store_throughfall[WET];
      veg_var_dry->throughfall = store_throughfall[DRY];
      veg_var_wet->canopyevap  = store_canopyevap[WET];
      veg_var_dry->canopyevap  = store_canopyevap[DRY];
      if(snow->snow) {
        veg_var_wet->Wdew        = snow_veg_var[WET].Wdew;
        veg_var_dry->Wdew        = snow_veg_var[DRY].Wdew;
      }
      else {
        veg_var_wet->Wdew        = soil_veg_var[WET].Wdew;
        veg_var_dry->Wdew        = soil_veg_var[DRY].Wdew;
      }
  }
  //**********************************************************
  //  Store soil layer variables for sub-model time steps
  //**********************************************************
  for (int lidx = 0; lidx < Nlayers; lidx++) {
      layer_wet[lidx]      = step_layer[WET][lidx];
      layer_dry[lidx]      = step_layer[DRY][lidx];
      layer_wet[lidx].evap = store_layerevap[WET][lidx];
      layer_dry[lidx].evap = store_layerevap[DRY][lidx];
  #if EXCESS_ICE
      evap_prior_wet[lidx] = store_layerevap[WET][lidx];
      evap_prior_dry[lidx] = store_layerevap[DRY][lidx];
  #endif
  }
  #if (VIC_CROPSYST_VERSION>=3)
  layer_wet[0].VCS.evap_bare_soil = store_soilevap[WET];
  layer_dry[0].VCS.evap_bare_soil = store_soilevap[DRY];
  for (int dist = 0; dist < Ndist; dist++) {
    crop_data_struct *current_crop = 0;
    if (iscrop_return) {
        if (dist == WET) current_crop = veg_var_wet->crop_state;
        else current_crop = veg_var_dry->crop_state;
    }
    cell_data_struct *cell = (dist == WET) ? cell_wet : cell_dry;
    if (current_crop) {
      VIC_land_unit_activate(current_crop->CropSystHandle);
      current_crop->canopy_evap                   = store_canopyevap[dist];                 //150630LML


      //std::clog << "store_canopyevap:" << store_canopyevap[dist] << std::endl;

#ifdef __BCPLUSPLUS__
//190130 RLN compiler not finding irrigation_netdemand
#else
      current_crop->irrigation_netdemand          = store_irrigation_netdemand[dist];       //180327LML
#endif
      current_crop->irrigation_evap               = store_irrigation_evap[dist];            //150702LML
//190130RLN redundent      current_crop->irrigation_netdemand          = store_irrigation_netdemand[dist];
      current_crop->irrigation_water              = store_irrigation_water[dist];           //150702LML
      //current_crop->net_irrigation                = store_net_irrigation_water[dist];
      current_crop->irrigation_runoff             = store_irrigation_runoff[dist];          //150702LML
      current_crop->intercepted_irrigation        = store_irrigation_intercept[dist];       //150702LML
      current_crop->evap_intercepted_irrig_water  = store_irrigation_intercept_evap[dist];  //150702LML
      current_crop->canopy_interception_fraction  = VIC_CropSyst_get(VIC::CANOPY_COVER_FRACTION_TOTAL); //150702LML
      current_crop->potential_transpir            = store_pot_tanspiration[dist];           //150702LML
      cell->VCS.potential_transpir                = store_pot_tanspiration[dist];           //160620LML removed the irrigation evaporation

      //170925LML cell->VCS.irrigation_water                = current_crop->irrigation_water;             //150713LML
      //170925LML cell->VCS.evap_from_irrigation_syst             = current_crop->irrigation_evap;              //150713LML
      //170925LML cell->VCS.evap_from_irrigation_intercept        = current_crop->evap_intercepted_irrig_water; //150713LML
      //170925LML cell->VCS.irrigation_runoff                     = current_crop->irrigation_runoff;            //150713LML
      //170925LML cell->VCS.actual_irrigation_reach_ground_amount = store_irrigation_actual[dist];          //150714LML
    }
  }
  #endif
  if (store_aero_cond_used[0]>0 && store_aero_cond_used[0]<param.HUGE_RESIST)
    aero_resist_used[0] = 1. / (store_aero_cond_used[0]/(double)N_steps);
  else if (store_aero_cond_used[0]>=param.HUGE_RESIST) aero_resist_used[0] = 0;
  else                                               aero_resist_used[0] = param.HUGE_RESIST;
  if (store_aero_cond_used[1]>0 && store_aero_cond_used[1]<param.HUGE_RESIST)
    aero_resist_used[1] = 1./(store_aero_cond_used[1]/(double)N_steps);
  else if (store_aero_cond_used[1]>=param.HUGE_RESIST) aero_resist_used[1] = 0;
  else                                               aero_resist_used[1] = param.HUGE_RESIST;
  for (int p = 0; p < N_PET_TYPES; p++) pot_evap[p] = store_pot_evap[p] / (double)N_steps;
  delete_2d_array<double>(step_aero_resist,N_PET_TYPES);
  #if VIC_CROPSYST_VERSION>=3
  for (int dist = 0; dist < Ndist; dist++) {
    crop_data_struct *current_crop = 0;
    if (iscrop_return && (global_param.dt == 24.0)) {
      if (dist == WET) current_crop = veg_var_wet->crop_state;
      else             current_crop = veg_var_dry->crop_state;
      VIC_land_unit_activate(current_crop->CropSystHandle);
      
      //190721 LML  Claudio decided that CropSyst use its own reference ET
      //VIC_land_unit_update_PET();                                                //150616LML
    }
    if (current_crop) current_crop->pot_evap_short = pot_evap[PET_SHORT];        //150702LML
  }
  #endif
  /********************************************************
    Compute Runoff, Baseflow, and Soil Moisture Transport
  ********************************************************/
#if EXCESS_ICE
  if(SubsidenceUpdate != 2){
#endif
    (*inflow_wet) = ppt[WET];
    (*inflow_dry) = ppt[DRY];


    //07142022LML check if irrigation field
    bool irrigated_field = false;
#if (VIC_CROPSYST_VERSION>=3)
    int rotation_code = veg_con[iveg].VCS.veg_class_code;
    bool is_crop = iscrop(rotation_code);
    if (is_crop) {
        int rotation_or_crop_veg_class_code = -1;                                      //161212LML
        int rotation_cycle_index = -1;                                                 //161212LML
        DecomposeVegclassCode(veg_class_code,rotation_or_crop_veg_class_code,
                          rotation_cycle_index);                                   //161212LML
        Irrigation_Type irrigation_mode     = NO_IRRIGATION;
        bool full_irrigation;
        bool find_irrig_type = find_irrigation_type(
                    soil_con->VCS.irrigation_type_list,
                    rotation_or_crop_veg_class_code,irrigation_mode,
                    full_irrigation);
        if (irrigation_mode != NO_IRRIGATION && options.VCS.do_irrigate_crop)
            irrigated_field = true;
    }
#endif


    ErrorFlag = runoff(cell_wet, cell_dry, energy, soil_con, ppt,
                       #if EXCESS_ICE
                       SubsidenceUpdate,
                       #endif
                       #if SPATIAL_FROST
                       soil_con->frost_fract,
                       #endif // SPATIAL_FROST
                       mu, global_param.dt, options.Nnode, iveg
                       #if (VIC_CROPSYST_VERSION>=3)
                       ,veg_var_dry                                             //150929LML
                       ,veg_var_wet                                             //150929LML
                       ,irrigated_field                                         //07142022LML
                       #endif
                       );
    return( ErrorFlag );
#if EXCESS_ICE
  }
#endif
  return(0);
}
#undef MAX_ITER
#undef GRND_TOL
#undef OVER_TOL
//160518LML_____________________________________________________________________
void update_aero_resist(const double iter_aero_resist_used[2], const double * const *aero_resist,
                        const int UnderStory, double **step_aero_resist)
{
    // First, determine the stability correction used in the iteration
    double stability_factor[2];
    if (iter_aero_resist_used[0] == param.HUGE_RESIST) stability_factor[0] = param.HUGE_RESIST;
    else                                         stability_factor[0] = iter_aero_resist_used[0] / aero_resist[N_PET_TYPES][UnderStory];
    if (iter_aero_resist_used[1] == iter_aero_resist_used[0]) {
      stability_factor[1] = stability_factor[0];
    } else {
      if (iter_aero_resist_used[1] == param.HUGE_RESIST) stability_factor[1] = param.HUGE_RESIST;
      else                                         stability_factor[1] = iter_aero_resist_used[1] / aero_resist[N_PET_TYPES][1];
    }
    // Next, loop over pot_evap types and apply the correction to the relevant aerodynamic resistance
    for (int p = 0; p < N_PET_TYPES; p++) {
      if (stability_factor[0] == param.HUGE_RESIST) step_aero_resist[p][0] = param.HUGE_RESIST;
      else                                    step_aero_resist[p][0] = aero_resist[p][UnderStory] * stability_factor[0];
      if (stability_factor[1] == param.HUGE_RESIST) step_aero_resist[p][1] = param.HUGE_RESIST;
      else                                    step_aero_resist[p][1] = aero_resist[p][1]          * stability_factor[1];
    }
}
