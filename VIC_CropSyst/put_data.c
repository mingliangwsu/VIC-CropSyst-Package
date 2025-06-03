#include <stdio.h>
#include <stdlib.h>
#include <vicNl.h>
#include <iostream>

static char vcid[] = "$Id: put_data.c,v 5.14.2.47 2012/10/25 23:15:58 vicadmin Exp $";
#ifdef VIC_CROPSYST_VERSION
int initialize_crop_output(const veg_con_struct *veg_con,out_data_struct *out_data);
extern int get_cropping_system_numbers(const veg_con_struct *veg_con_array);     //150928LML
#endif
double get_soilprofile_water_from_output_mm(const out_data_struct *outdata,
                                            const soil_con_struct *soil_con);    //150930LML
bool calc_treeline_adjustment_factors(const int rec, const char* AboveTreeLine,const veg_con_struct *veg_con,
                                      const lake_con_struct &lake_con, const lake_var_struct &lake_var,
                                      double *TreeAdjustFactor);                 //160512LML
bool aggregate_output_in_temporal_dimention(const int out_step_ratio, out_data_struct *out_data); //160512LML
bool transform_for_ALMA_output(const int out_dt_sec, out_data_struct *out_data); //160512LML
bool store_veg_outputs(const double *AreaFract,
                const double *TreeAdjustFactor,
                const char *AboveTreeLine,
                const atmos_data_struct *atmos,
                const soil_con_struct   *soil_con,
                const veg_con_struct    *veg_con,
                const lake_con_struct   *lake_con,
                dist_prcp_struct  *prcp,
                out_data_struct   *out_data,
                int *Tfoliage_fbcount,
                int *Tcanopy_fbcount,
                int *Tsnowsur_fbcountf,
                int *Tsurf_fbcount,
                int *Tsoil_fbcount,
                double *cv_lake,
                double *cv_veg,
                double *cv_baresoil,
                double *cv_snow,
                double *cv_overstory
                );                                                               //160512LML

//______________________________________________________________________________
int  put_data(  dist_prcp_struct  *prcp,
                atmos_data_struct *atmos,
                soil_con_struct   *soil_con,
                veg_con_struct    *veg_con,
                lake_con_struct   *lake_con,
                out_data_file_struct   *out_data_files,
                out_data_struct   *out_data,
                save_data_struct  *save_data,
                dmy_struct        *dmy,
                #if VIC_CROPSYST_VERSION==2
                crop_data_struct  *crops,
                #endif
                int                rec)
/**********************************************************************
    put_data.c    Dag Lohmann        January 1996

  This routine converts data units, and stores finalized values
  in an array for later output to the output files.

  modifications:
  06-24-98  modified for new distributed presipitation data structures KAC
  01-20-00 modified to deal with simplified frozen soil moisture layers
           and frost depth / thaw depth accounting                 KAC
  03-08-00 modified to eliminate extra lines for storing bare
           soil variables.                                         KAC
  6-8-2000 modified to handle spatially distribute frozen soil     KAC
  10-6-2000 modified to handle partial snow cover                  KAC
  02-27-01 modified to output lake model variables                 KAC
  11-18-02 updated output of lake variables to reflect algorithm
           changes.  Also added output variables for blowing snow
           algorithm.                                              LCB
  03-12-03 modified to add additional energy balance variable storage
           when output of snow bands is selected.                  KAC
  03-12-03 Modifed to add AboveTreeLine to soil_con_struct so that
           the model can make use of the computed treeline.     KAC
  30-Oct-03 Snow_flux was incorrectly set to Tcanopy.  Fixed.   TJB
  25-Aug-04 Sub_snow was incorrectly set to blowing_flux.  Now it is
            set to vapor_flux.                                  TJB
  28-Sep-04 Now out_data->aero_resist stores the aerodynamic resistance
            used in flux calculations.                          TJB
  2005-Mar-24 Modified to compute ALMA output variables.        TJB
  2005-Apr-23 Now aero_cond is aggregated instead of aero_resist.       TJB
  2006-Sep-23 Implemented flexible output configuration; uses the new
              out_data and out_data_files structures; removed the
              OPTIMIZE and LDAS_OUTPUT options; uses the
          new save_data structure; implemented aggregation.        TJB
  2006-Oct-10 Shortened the names of output variables whose names were
          too long; fixed typos in others; created new OUT_IN_LONG
          variable.                            TJB
  2006-Nov-07 Added OUT_SOIL_TNODE.                    TJB
  2006-Nov-07 Assigned value to overstory.                TJB
  2006-Nov-07 Removed LAKE_MODEL option.                TJB
  2006-Nov-30 Added OUT_DELSURFSTOR.                    TJB
  2006-Nov-30 Convert pressure and vapor pressure to kPa for output.    TJB
  2006-Dec-20 Changed OUT_SURF_TEMP from average of T[0] and T[1] to
          direct assignment of T[0].                TJB
  2007-Apr-21 Moved initialization of tmp_fract to immediately before the
            #if SPATIAL_FROST
          block, so that it would be initialized in all cases.    TJB
  2007-Aug-17 Added EXCESS_ICE output variables.            JCA
  2007-Aug-22 Added OUT_WATER_ERROR as output variable.            JCA
  2007-Nov-06 Lake area is now the larger of lake.areai and lake.sarea.
          Added wetland canopyevap and canopy_vapor_flux to grid
          cell flux aggregation.                    LCB via TJB
  2008-Apr-21 Made computation of out_data[OUT_SURFSTOR] more robust.    TJB
  2008-Sep-09 Calculate sarea in order to output lake surface area at
          the end of the time step.  The stored variable
          lake->sarea represents the sarea from the beginning of
          the time step, not the updated value from the end of the
          time step.                        LCB via TJB
  2008-Sep-09 Added SOIL_TNODE_WL as an output variable, the soil
          temperature in the wetland fraction of the grid cell.    LCB via TJB
  2008-Sep-09 Allow output of wetland frost/thaw depths even if Clake
          is 1.0 since wetland energy balance is always computed.    LCB via TJB
  2008-Sep-09 Lake depth assignment moved up to precede sarea
          assignment.                        LCB via TJB
  2008-Sep-09 Check to make sure that area > 0.0 when checking to see
          if ice area > sarea.                    LCB via TJB
  2008-Oct-23 Changed data type of put_data() to be int so that it
          can return ErrorFlag.                    TJB
  2009-Jan-12 Added a final return of (0) since the data type of put_data()
          is int rather than void.                    TJB
  2009-Jan-16 Modified aero_resist_used and Ra_used to become arrays of
          two elements (surface and overstory); added
          options.AERO_RESIST_CANSNOW.                TJB
  2009-Jan-16 Added AERO_COND1&2 and AERO_RESIST1&2 to track
          surface and overstory values; changed AERO_COND
          and AERO_RESIST to track "scene" values.            TJB
  2009-Feb-09 Removed checks on PRT_SNOW_BAND option.            TJB
  2009-Feb-22 Added OUT_VPD.                        TJB
  2009-May-17 Added OUT_ASAT.                        TJB
  2009-Jun-09 Modified to use extension of veg_lib structure to contain
          bare soil information.                    TJB
  2009-Jun-09 Added OUT_PET_*, potential evap computed for various
          reference land cover types.                TJB
  2009-Jun-09 Cell_data structure now only stores final aero_resist
          values (called "aero_resist").  Preliminary uncorrected
          aerodynamic resistances for current vegetation and various
          reference land cover types for use in potential evap
          calculations is stored in temporary array aero_resist.    TJB
  2009-Jun-19 Added T flag to indicate whether TFALLBACK occurred.    TJB
  2009-Jul-31 Modified so that wetland veg is now included in main loop
          over veg tiles and aggregated the same way as all other
          veg tiles.                        TJB
  2009-Aug-28 OUT_LAKE_ICE_TEMP and OUT_LAKE_SURF_TEMP are [C].        TJB
  2009-Sep-19 Added T fbcount to count TFALLBACK occurrences.        TJB
  2009-Sep-28 Created collect_wb_terms and collect_eb_terms to handle
          summing of storages and fluxes from upland veg tiles,
          wetland veg tile, and lake.  Added logic to handle an
          initial (pre-simulation) call for purpose of initializing
          water and energy balance checks.                TJB
  2009-Sep-30 Miscellaneous fixes for lake model.            TJB
  2009-Oct-05 Modifications for taking changes in lake area into account.    TJB
  2009-Oct-08 Extended T fallback scheme to snow and ice T.        TJB
  2009-Nov-09 Changed definition of sarea to include ice extent.    LCB via TJB
  2009-Nov-15 Redirected T fallback messages to stderr.            TJB
  2009-Dec-11 Added logic for initialization of save_data structure.    TJB
  2010-Feb-14 Added OUT_LAKE_AREA_FRAC.                    TJB
  2010-Mar-31 Added OUT_RUNOFF_IN.                    TJB
  2010-Sep-24 Renamed RUNOFF_IN and OUT_RUNOFF_IN to CHANNEL_IN and
          OUT_LAKE_CHAN_IN, respectively.  Renamed OUT_EVAP_LAKE
          to OUT_LAKE_EVAP.  Added other lake water balance terms
          to set of output variables.  Added volumetric versions
          of these too.                        TJB
  2010-Nov-02 Added OUT_LAKE_RCHRG, OUT_LAKE_RCHRG_V, OUT_RO_IN,
          OUT_LAKE_RO_IN_V, OUT_LAKE_VAPFLX, and OUT_LAKE_VAPFLX_V.    TJB
  2010-Nov-02 Changed units of lake_var moisture fluxes to volume (m3).    TJB
  2010-Nov-11 Moved assignment of all OUT_LAKE* variables outside
          collect_wb_terms().  Added lakefactor to collect_wb_terms()
          arg list.                            TJB
  2010-Nov-21 Added OUT_LAKE_DSTOR, OUT_LAKE_DSTOR_V, OUT_LAKE_DSWE,
          OUT_LAKE_DSWE_V, OUT_LAKE_SWE, and OUT_LAKE_SWE_V.    TJB
  2010-Nov-26 Changed += to = in assignment of OUT_LAKE_* variables.    TJB
  2010-Dec-01 Added OUT_ZWT.                        TJB
  2011-Mar-01 Added OUT_ZWT2, OUT_ZWT3, and OUT_ZWTL.            TJB
  2011-Mar-31 Added frost_fract to collect_wb_terms() arglist.        TJB
  2011-Nov-04 Added OUT_TSKC.                        TJB
  2012-Jan-16 Removed LINK_DEBUG code                    BN
  2012-Feb-07 Removed OUT_ZWT2 and OUT_ZWTL; renamed OUT_ZWT3 to
          OUT_ZWT_LUMPED.                        TJB
  2012-Oct-25 Fixed sign errors in flux summations in call to calc_energy_balance_error().
          Changed calc_energy_balance_error() to return the error, and
          now out_data[OUT_ENERGY_ERROR].data[0] is assigned to this
          error.  Corrected the setting of rad_temp when there is snow
          in the canopy to Tfoliage (canopy snow temperature) instead
          of Tcanopy (canopy air temperature).            CL via TJB
2012-Nov-14 CropSyst related changes applied on water balance
2012-Nov-14 CropSyst outputs are implemented
**********************************************************************/
{
  extern global_param_struct global_param;
  extern option_struct    options;
  int                     index;
  int                     Ndist;
  char              *AboveTreeLine;
  double            *AreaFract;
  double            *depth;
  double            *dz;
#if SPATIAL_FROST
  double            *frost_fract;
  double             frost_slope;
#endif // SPATIAL_FROST
  double             dp;
  int                skipyear;
  double                  inflow;
  double                  outflow;
  double                  storage;
  double                  TreeAdjustFactor[MAX_BANDS];
  int                     dt_sec;
  int                     out_dt_sec;
  int                     out_step_ratio;
  static int              step_count;
  static int              Tfoliage_fbcount_total;
  static int              Tcanopy_fbcount_total;
  static int              Tsnowsurf_fbcount_total;
  static int              Tsurf_fbcount_total;
  static int              Tsoil_fbcount_total;
  double                  cv_lake;
  double                  cv_veg;
  double                  cv_baresoil;
  double                  cv_snow;
  double                  cv_overstory;
  cell_data_struct     ***cell;
  energy_bal_struct     **energy;
  lake_var_struct         lake_var;
  snow_data_struct      **snow;
  veg_var_struct       ***veg_var;

  //if (rec == 125) {
  //    std::clog << "DEBUGGING put_data rec = 125\n";
  //}

  #ifdef VIC_CROPSYST_VERSION
  int total_crop_counter = get_cropping_system_numbers(veg_con);
  #endif
  AboveTreeLine = soil_con->AboveTreeLine;
  AreaFract = soil_con->AreaFract;
  depth = soil_con->depth;
  dz = soil_con->dz_node;
#if SPATIAL_FROST
  frost_fract = soil_con->frost_fract;
  frost_slope = soil_con->frost_slope;
#endif // SPATIAL_FROST
  dp = soil_con->dp;
  skipyear = global_param.skipyear;
  dt_sec = global_param.dt*SECPHOUR;
  out_dt_sec = global_param.out_dt*SECPHOUR;
  out_step_ratio = (int)(out_dt_sec/dt_sec);
  if (rec >= 0) step_count++;
  if (rec == 0) {
    Tsoil_fbcount_total = 0;
    Tsurf_fbcount_total = 0;
    Tsnowsurf_fbcount_total = 0;
    Tcanopy_fbcount_total = 0;
    Tfoliage_fbcount_total = 0;
  }
  Ndist = options.NDIST;

  // Compute treeline adjustment factors
  calc_treeline_adjustment_factors(rec,AboveTreeLine,veg_con,*lake_con,lake_var,TreeAdjustFactor); //160512LML
  cv_baresoil = 0;                                                               //180402ML
  cv_veg = 0;                                                                    //180402LML
  cv_overstory = 0;                                                              //180402LML
  cv_snow = 0;                                                                   //180402LML

  // Initialize output data to zero
  zero_output_list(out_data);

  // Set output versions of input forcings
  out_data[OUT_AIR_TEMP].data[0]  = atmos->air_temp[NR];
  out_data[OUT_DENSITY].data[0]   = atmos->density[NR];
  out_data[OUT_LONGWAVE].data[0]  = atmos->longwave[NR];
  out_data[OUT_PREC].data[0]      = atmos->out_prec;                             // mm over grid cell
  out_data[OUT_PRESSURE].data[0]  = atmos->pressure[NR]/kPa2Pa;
  out_data[OUT_QAIR].data[0]      = EPS * atmos->vp[NR]/atmos->pressure[NR];
  out_data[OUT_RAINF].data[0]     = atmos->out_rain;                             // mm over grid cell
  out_data[OUT_REL_HUMID].data[0] = 100.*atmos->vp[NR]/(atmos->vp[NR]+atmos->vpd[NR]);
  if (options.LAKES && lake_con->Cl[0] > 0)
    out_data[OUT_LAKE_CHAN_IN].data[0] = atmos->channel_in[NR];                  // mm over grid cell
  else
    out_data[OUT_LAKE_CHAN_IN].data[0] = 0;
  out_data[OUT_SHORTWAVE].data[0] = atmos->shortwave[NR];
  out_data[OUT_SNOWF].data[0]     = atmos->out_snow;                             // mm over grid cell
  out_data[OUT_TSKC].data[0]      = atmos->tskc[NR];
  out_data[OUT_VP].data[0]        = atmos->vp[NR]/kPa2Pa;
  out_data[OUT_VPD].data[0]       = atmos->vpd[NR]/kPa2Pa;
  out_data[OUT_WIND].data[0]      = atmos->wind[NR];

  cell    = prcp->cell;
  energy  = prcp->energy;
  lake_var = prcp->lake_var;
  snow    = prcp->snow;
  veg_var = prcp->veg_var;

  /****************************************
    Store Output for all Vegetation Types (except lakes)
  ****************************************/
  store_veg_outputs(AreaFract, TreeAdjustFactor, AboveTreeLine,
                    atmos, soil_con, veg_con, lake_con,
                    prcp, out_data,
                    &Tfoliage_fbcount_total,
                    &Tcanopy_fbcount_total,
                    &Tsnowsurf_fbcount_total,
                    &Tsurf_fbcount_total,
                    &Tsoil_fbcount_total,
                    &cv_lake,
                    &cv_veg,
                    &cv_baresoil,
                    &cv_snow,
                    &cv_overstory);                                          //160512LML
  /*****************************************
    Aggregation of Dynamic Soil Properties
   *****************************************/
#if EXCESS_ICE
  for(index=0;index<options.Nlayer;index++) {
    out_data[OUT_SOIL_DEPTH].data[index]  = soil_con->depth[index];
    out_data[OUT_SUBSIDENCE].data[index]  = soil_con->subsidence[index];
    out_data[OUT_POROSITY].data[index]    = soil_con->effective_porosity[index];
  }
  for(index=0;index<options.Nnode;index++)
    out_data[OUT_ZSUM_NODE].data[index]   = soil_con->Zsum_node[index];
#endif // EXCESS_ICE

  /*****************************************
    Finish aggregation of special-case variables
   *****************************************/
  // Normalize quantities that aren't present over entire grid cell
  if (cv_baresoil > 0) {
    out_data[OUT_BARESOILT].data[0] /= cv_baresoil;
  }
  if (cv_veg > 0) {
    out_data[OUT_VEGT].data[0] /= cv_veg;
  }
  if (cv_overstory > 0) {
    out_data[OUT_AERO_COND2].data[0] /= cv_overstory;
  }
  if (cv_snow > 0) {
    out_data[OUT_SALBEDO].data[0] /= cv_snow;
    out_data[OUT_SNOW_SURF_TEMP].data[0] /= cv_snow;
    out_data[OUT_SNOW_PACK_TEMP].data[0] /= cv_snow;
  }

  // Radiative temperature
  out_data[OUT_RAD_TEMP].data[0] = pow(out_data[OUT_RAD_TEMP].data[0],0.25);

  // Aerodynamic conductance and resistance
  if (out_data[OUT_AERO_COND1].data[0] > SMALL) {
    out_data[OUT_AERO_RESIST1].data[0] = 1 / out_data[OUT_AERO_COND1].data[0];
  } else {
    out_data[OUT_AERO_RESIST1].data[0] = param.HUGE_RESIST;
  }
  if (out_data[OUT_AERO_COND2].data[0] > SMALL) {
    out_data[OUT_AERO_RESIST2].data[0] = 1 / out_data[OUT_AERO_COND2].data[0];
  } else {
    out_data[OUT_AERO_RESIST2].data[0] = param.HUGE_RESIST;
  }
  if (out_data[OUT_AERO_COND].data[0] > SMALL) {
    out_data[OUT_AERO_RESIST].data[0] = 1 / out_data[OUT_AERO_COND].data[0];
  } else {
    out_data[OUT_AERO_RESIST].data[0] = param.HUGE_RESIST;
  }

  /*****************************************
    Compute derived variables
   *****************************************/
  // Water balance terms
  out_data[OUT_DELSOILMOIST].data[0] = 0;
  for (index=0; index<options.Nlayer; index++) {
    out_data[OUT_SOIL_MOIST].data[index]    = out_data[OUT_SOIL_LIQ].data[index]+out_data[OUT_SOIL_ICE].data[index];
    out_data[OUT_DELSOILMOIST].data[0]     += out_data[OUT_SOIL_MOIST].data[index];
    out_data[OUT_SMLIQFRAC].data[index]     = out_data[OUT_SOIL_LIQ].data[index]/out_data[OUT_SOIL_MOIST].data[index];
    out_data[OUT_SMFROZFRAC].data[index]    = 1 - out_data[OUT_SMLIQFRAC].data[index];
  }
  if (rec >= 0) {
    out_data[OUT_DELSOILMOIST].data[0] -= save_data->total_soil_moist;
    out_data[OUT_DELSWE].data[0]        = out_data[OUT_SWE].data[0] + out_data[OUT_SNOW_CANOPY].data[0] - save_data->swe;
    out_data[OUT_DELINTERCEPT].data[0]  = out_data[OUT_WDEW].data[0] - save_data->wdew;
    out_data[OUT_DELSURFSTOR].data[0]   = out_data[OUT_SURFSTOR].data[0] - save_data->surfstor;
  }

  // Energy terms
  out_data[OUT_REFREEZE].data[0]        = (out_data[OUT_RFRZ_ENERGY].data[0]/Lf)*dt_sec;
  out_data[OUT_R_NET].data[0]           = out_data[OUT_NET_SHORT].data[0] + out_data[OUT_NET_LONG].data[0];

  // Save current moisture state for use in next time step
  save_data->total_soil_moist = 0;
  for (index=0; index<options.Nlayer; index++) {
    save_data->total_soil_moist        += out_data[OUT_SOIL_MOIST].data[index];
  }
  save_data->surfstor                   = out_data[OUT_SURFSTOR].data[0];
  save_data->swe                        = out_data[OUT_SWE].data[0] + out_data[OUT_SNOW_CANOPY].data[0];
  save_data->wdew                       = out_data[OUT_WDEW].data[0];

  /********************
    Check Water Balance
    ********************/
  #ifdef VIC_CROPSYST_VERSION //21/SEP/2012 Keyvan, added to check the water balance in coupled model
  double total_grid_irrig         = 0;
  double total_grid_irrig_evap    = 0;
  double total_grid_irrig_runoff  = 0;
  if(total_crop_counter > 0){
    for (int crop_count = 0; crop_count < total_crop_counter; crop_count++) {
        total_grid_irrig += out_data[OUT_CROP_IRRI_WAT].data[crop_count];
        total_grid_irrig_evap += out_data[OUT_EVAP_FROM_IRRIG].data[crop_count];
        total_grid_irrig_runoff += out_data[OUT_RUNOFF_IRRIG].data[crop_count];
    }
    out_data[OUT_EVAP].data[0] += total_grid_irrig_evap;                         //190507 
    out_data[OUT_RUNOFF].data[0] += total_grid_irrig_runoff;                     //190507
    inflow  = out_data[OUT_PREC].data[0]
              //170924LML + out_data[OUT_CELL_IRRIGATION].data[0]                /*150929LML crops*///150930LML prcp->grid_cell_crop_output.cell_irrig_water
              //170925LML + out_data[OUT_CROP_IRRI_WAT].data[0]                              //170924LML
              + total_grid_irrig                                                 //170925LML
              + out_data[OUT_LAKE_CHAN_IN].data[0];
    outflow = out_data[OUT_EVAP].data[0]
              + out_data[OUT_RUNOFF].data[0]
              + out_data[OUT_BASEFLOW].data[0]
              //170924LML + out_data[OUT_CELL_EVAP_FROM_IRRIG].data[0]                       /*150929LML crops*///150930LML prcp->grid_cell_crop_output.cell_evap_from_irrigation
              //170924LML + out_data[OUT_CELL_RUNOFF_IRRIG].data[0]                          /*150929LML crops*///150930LML prcp->grid_cell_crop_output.cell_irrigation_runoff ;//- out_data[OUT_EVAP_CANOP_IRRIG].data[0];//+crops->cell_evap_from_irrigation_intercepted;//+crops->cell_avg_transp;
              //170924LML + out_data[OUT_CELL_EVAP_CANOP_IRRIG].data[0];
              //170925LML + out_data[OUT_EVAP_FROM_IRRIG].data[0]                           //170924LML
              //170925LML + out_data[OUT_RUNOFF_IRRIG].data[0];                             //170924LML
              //190507 + total_grid_irrig_evap                                            //170925LML
              //190507+ total_grid_irrig_runoff;                                         //170925LML
              ;
    //printf("cell_irrigation_runoff(%f)\n",crops->cell_irrigation_runoff);
  } else{
    inflow  = out_data[OUT_PREC].data[0] + out_data[OUT_LAKE_CHAN_IN].data[0];//+out_data[OUT_CROP_IRRI_WAT].data[0];
    outflow = out_data[OUT_EVAP].data[0] + out_data[OUT_RUNOFF].data[0] + out_data[OUT_BASEFLOW].data[0];//+out_data[OUT_CROP_TRANSPIR].data[0];
  }
  #else
  inflow  = out_data[OUT_PREC].data[0]+out_data[OUT_LAKE_CHAN_IN].data[0];//+out_data[OUT_CROP_IRRI_WAT].data[0];
  outflow = out_data[OUT_EVAP].data[0] + out_data[OUT_RUNOFF].data[0] + out_data[OUT_BASEFLOW].data[0];//+out_data[OUT_CROP_TRANSPIR].data[0];
  #endif
  static double last_storage = 0;                                                //150930LML
  storage = 0.;
  for (int index = 0; index < options.Nlayer; index++) {
    if(options.MOISTFRACT)
      storage += (out_data[OUT_SOIL_LIQ].data[index] + out_data[OUT_SOIL_ICE].data[index])
                 * depth[index] * 1000;
    else
      storage += out_data[OUT_SOIL_LIQ].data[index] + out_data[OUT_SOIL_ICE].data[index];
  }
  storage += out_data[OUT_SWE].data[0] + out_data[OUT_SNOW_CANOPY].data[0] + out_data[OUT_WDEW].data[0] + out_data[OUT_SURFSTOR].data[0];
  out_data[OUT_WATER_ERROR].data[0] = calc_water_balance_error(rec,inflow,outflow,storage);
#ifndef OUTPUT_SIMPLE_FOR_FORECAST
  if (fabs(out_data[OUT_WATER_ERROR].data[0]) > 1e-5) {
      std::clog<<"##error:"     <<out_data[OUT_WATER_ERROR].data[0]
               <<"\nrec:"       <<rec
               <<"\nPPT:"       <<out_data[OUT_PREC].data[0]
               <<"\nSWE:"       <<out_data[OUT_SWE].data[0]
               <<"\nSNOW_CNOPY:"<<out_data[OUT_SNOW_CANOPY].data[0]
               <<"\nWDEW:"      <<out_data[OUT_WDEW].data[0]
               <<"\nsoil_evap:" <<out_data[OUT_EVAP_BARE].data[0]
               <<"\ncanop_evap:"<<out_data[OUT_EVAP_CANOP].data[0]
               <<"\ntransp:"    <<out_data[OUT_TRANSP_VEG].data[0]
               <<"\nsnow_sub:"  <<(out_data[OUT_SUB_SNOW].data[0] + out_data[OUT_SUB_SURFACE].data[0] + out_data[OUT_SUB_BLOWING].data[0])
               #ifdef VIC_CROPSYST_VERSION
               <<"\nIrrigation:"<<total_grid_irrig                               //out_data[OUT_CROP_IRRI_WAT].data[0]            //170924LML
               #endif
               <<"\nChan_In:"   <<out_data[OUT_LAKE_CHAN_IN].data[0]
               <<"\nEvap:"  <<out_data[OUT_EVAP].data[0]
               <<"\nRunoff:"    <<out_data[OUT_RUNOFF].data[0]
               <<"\nBaseflow:"  <<out_data[OUT_BASEFLOW].data[0]
               #ifdef VIC_CROPSYST_VERSION
               //170924LML <<"\tEvapIrig:"  <<out_data[OUT_CELL_EVAP_FROM_IRRIG].data[0]     //150930LML prcp->grid_cell_crop_output.cell_evap_from_irrigation
               //170924LML <<"\tRunoffIrig:"<<out_data[OUT_CELL_RUNOFF_IRRIG].data[0]        //150930LML prcp->grid_cell_crop_output.cell_irrigation_runoff
               <<"\nEvapIrig:"  <<total_grid_irrig_evap                          //out_data[OUT_EVAP_FROM_IRRIG].data[0]          //170924LML
               <<"\nRunoffIrig:"<<total_grid_irrig_runoff                        //out_data[OUT_RUNOFF_IRRIG].data[0]             //170924LML
               #endif
               <<"\n\nInflow:"          <<inflow
               <<"\nOutflow:"           <<outflow
               <<"\nNetflow:"           <<(inflow-outflow)
               <<"\nDelta_S:"           <<(storage-last_storage)
               <<"\nCurrent_Storage:"   <<storage
               <<"\nLast_Storage:"      <<last_storage
               <<"\nBalance_error:"     <<((storage-last_storage)
                                           - (inflow-outflow))
               <<std::endl;
  }
#endif
  last_storage = storage;                                                        //150930LML
  /********************
    Check Energy Balance
  ********************/
  if(options.FULL_ENERGY)
    out_data[OUT_ENERGY_ERROR].data[0] = calc_energy_balance_error(rec,
        out_data[OUT_NET_SHORT].data[0] + out_data[OUT_NET_LONG].data[0],
        out_data[OUT_LATENT].data[0] + out_data[OUT_LATENT_SUB].data[0],
        out_data[OUT_SENSIBLE].data[0] + out_data[OUT_ADV_SENS].data[0],
        out_data[OUT_GRND_FLUX].data[0] + out_data[OUT_DELTAH].data[0] + out_data[OUT_FUSION].data[0],
        out_data[OUT_ADVECTION].data[0] - out_data[OUT_DELTACC].data[0] + out_data[OUT_SNOW_FLUX].data[0] + out_data[OUT_RFRZ_ENERGY].data[0]);
  else
    out_data[OUT_ENERGY_ERROR].data[0] = 0;                                      // Perhaps this should be replaced with a NODATA value in this case
  /******************************************************************************************
    Return to parent function if this was just an initialization of wb and eb storage terms
  ******************************************************************************************/
  if (rec < 0) return(0);
  /********************
    Report T Fallback Occurrences
  ********************/
  #if VIC_CROPSYST_VERSION<3
  if (rec == global_param.nrecs-1) {
    fprintf(stderr,"Total number of fallbacks in Tfoliage: %d\n", Tfoliage_fbcount_total);
    fprintf(stderr,"Total number of fallbacks in Tcanopy: %d\n", Tcanopy_fbcount_total);
    fprintf(stderr,"Total number of fallbacks in Tsnowsurf: %d\n", Tsnowsurf_fbcount_total);
    fprintf(stderr,"Total number of fallbacks in Tsurf: %d\n", Tsurf_fbcount_total);
    fprintf(stderr,"Total number of fallbacks in soil T profile: %d\n", Tsoil_fbcount_total);
  }
  #endif

  /********************
    Temporal Aggregation
    ********************/
    aggregate_output_in_temporal_dimention(out_step_ratio,out_data); //160512LML
  if (step_count == out_step_ratio) {
    //***********************************************
    //  Change of units for ALMA-compliant output
    //***********************************************
    if (options.ALMA_OUTPUT) {
      transform_for_ALMA_output(out_dt_sec,out_data);                            //160512LML
    } //ALMA_OUTPUT
    /*************
      Write Data
    *************/
    if (rec >= skipyear) {
      if (options.BINARY_OUTPUT) {
        for (int v = 0; v < N_OUTVAR_TYPES; v++) {
          for (int i = 0; i < out_data[v].nelem; i++) {
            out_data[v].aggdata[i] *= out_data[v].mult;
          }
        }
      }
      write_data(out_data_files, out_data, dmy, global_param.out_dt);
    }
    // Reset the step count
    step_count = 0;
    // Reset the agg data
    for (int v = 0; v < N_OUTVAR_TYPES; v++)
      for (int i = 0; i < out_data[v].nelem; i++)
        out_data[v].aggdata[i] = 0;
  } // End of output procedure
  return (0);
}
//______________________________________________________________________________
void collect_wb_terms(const cell_data_struct &cell,
                      const veg_var_struct   &veg_var,
                      const snow_data_struct &snow,
                      const double            mu,
                      const double            Cv,
                      const double            AreaFract,
                      const double            TreeAdjustFactor,
                      const int               HasVeg,
                      const double            lakefactor,
                      const int               overstory,
                      const double           *depth,
#if SPATIAL_FROST
                      double           *frost_fract,
#endif // SPATIAL_FROST
                      out_data_struct  *out_data)
{
  extern option_struct    options;
  double tmp_cond1;
  double tmp_cond2;
  double tmp_moist;
  double tmp_ice;
  double AreaFactor = Cv * mu * AreaFract * TreeAdjustFactor * lakefactor;
  /** record evaporation components **/
  double tmp_evap = 0.0;
  for (int index = 0; index < options.Nlayer; index++)
    tmp_evap += cell.layer[index].evap;
  #ifdef VIC_CROPSYST_VERSION
  out_data[OUT_TRANSP_VEG].data[0] += tmp_evap * AreaFactor;                     //150625LML
  out_data[OUT_EVAP_BARE].data[0]  += cell.layer[0].VCS.evap_bare_soil * AreaFactor; //150625LML
  tmp_evap                         += cell.layer[0].VCS.evap_bare_soil;              //150713LML
  #else
  if (HasVeg)
    out_data[OUT_TRANSP_VEG].data[0] += tmp_evap * AreaFactor;
  else
    out_data[OUT_EVAP_BARE].data[0] += tmp_evap * AreaFactor;
  #endif //VIC_CROPSYST_VERSION
  tmp_evap += snow.vapor_flux * 1000.;
  out_data[OUT_SUB_SNOW].data[0] += snow.vapor_flux * 1000. * AreaFactor;
  out_data[OUT_SUB_SURFACE].data[0] += snow.surface_flux * 1000. * AreaFactor;
  out_data[OUT_SUB_BLOWING].data[0] += snow.blowing_flux * 1000. * AreaFactor;
  if (HasVeg) {
    tmp_evap += snow.canopy_vapor_flux * 1000.;
    out_data[OUT_SUB_CANOP].data[0] += snow.canopy_vapor_flux * 1000. * AreaFactor;
  }
  if (HasVeg) {
    tmp_evap += veg_var.canopyevap;
    out_data[OUT_EVAP_CANOP].data[0] += veg_var.canopyevap * AreaFactor;
  }
  out_data[OUT_EVAP].data[0] += tmp_evap * AreaFactor;                           // mm over gridcell
  /** record potential evap **/
  out_data[OUT_PET_SATSOIL].data[0] += cell.pot_evap[0] * AreaFactor;
  out_data[OUT_PET_H2OSURF].data[0] += cell.pot_evap[1] * AreaFactor;
  out_data[OUT_PET_SHORT].data[0]   += cell.pot_evap[2] * AreaFactor;
  out_data[OUT_PET_TALL].data[0]    += cell.pot_evap[3] * AreaFactor;
  out_data[OUT_PET_NATVEG].data[0]  += cell.pot_evap[4] * AreaFactor;
  out_data[OUT_PET_VEGNOCR].data[0] += cell.pot_evap[5] * AreaFactor;
  out_data[OUT_ASAT].data[0] += cell.asat * AreaFactor;
  out_data[OUT_RUNOFF].data[0]   += cell.runoff * AreaFactor;
  out_data[OUT_BASEFLOW].data[0] += cell.baseflow * AreaFactor;
  out_data[OUT_INFLOW].data[0] += (cell.inflow) * AreaFactor;
  if (HasVeg)
    out_data[OUT_WDEW].data[0] += veg_var.Wdew * AreaFactor;
  if (cell.aero_resist[0] > SMALL) {
    tmp_cond1 = (1/cell.aero_resist[0]) * AreaFactor;
  } else {
    tmp_cond1 = param.HUGE_RESIST;
  }
  out_data[OUT_AERO_COND1].data[0] += tmp_cond1;
  if (overstory) {
    if (cell.aero_resist[1] > SMALL) {
      tmp_cond2 = (1/cell.aero_resist[1]) * AreaFactor;
    } else {
      tmp_cond2 = param.HUGE_RESIST;
    }
    out_data[OUT_AERO_COND2].data[0] += tmp_cond2;
  }
  if (overstory) {
    out_data[OUT_AERO_COND].data[0] += tmp_cond2;
  } else {
    out_data[OUT_AERO_COND].data[0] += tmp_cond1;
  }
  /** record layer moistures **/
  for (int index = 0; index < options.Nlayer; index++) {
    tmp_moist = cell.layer[index].moist;
    #if SPATIAL_FROST
    tmp_ice = 0;
    for (int frost_area = 0; frost_area < FROST_SUBAREAS; frost_area++ )
      tmp_ice  += (cell.layer[index].ice[frost_area] * frost_fract[frost_area]);
    #else
    tmp_ice   = cell.layer[index].ice;
    #endif //SPATIAL_FROST
    tmp_moist -= tmp_ice;
    if (options.MOISTFRACT) {
      tmp_moist /= depth[index] * 1000.;
      tmp_ice /= depth[index] * 1000.;
    }
    out_data[OUT_SOIL_LIQ].data[index] += tmp_moist * AreaFactor;
    out_data[OUT_SOIL_ICE].data[index] += tmp_ice * AreaFactor;
  }
  out_data[OUT_SOIL_WET].data[0] += cell.wetness * AreaFactor;
  out_data[OUT_ROOTMOIST].data[0] += cell.rootmoist * AreaFactor;
  out_data[OUT_ZWT].data[0] += cell.zwt * AreaFactor;
  out_data[OUT_ZWT_LUMPED].data[0] += cell.zwt_lumped * AreaFactor;
  for (int index = 0; index < options.Nlayer; index++) {
    out_data[OUT_SOIL_TEMP].data[index] += cell.layer[index].T * AreaFactor;
  }
  /*****************************
    Record Snow Pack Variables
  *****************************/
  out_data[OUT_SWE].data[0] += snow.swq * AreaFactor * 1000.;
  out_data[OUT_SNOW_DEPTH].data[0] += snow.depth * AreaFactor * 100.;
  if (snow.swq> 0.0) {
    out_data[OUT_SALBEDO].data[0] += snow.albedo * AreaFactor;
    out_data[OUT_SNOW_SURF_TEMP].data[0] += snow.surf_temp * AreaFactor;
    out_data[OUT_SNOW_PACK_TEMP].data[0] += snow.pack_temp * AreaFactor;
  }
  if (HasVeg)
    out_data[OUT_SNOW_CANOPY].data[0] += (snow.snow_canopy) * AreaFactor * 1000.;
  out_data[OUT_SNOW_MELT].data[0] += snow.melt * AreaFactor;
  out_data[OUT_SNOW_COVER].data[0] += snow.coverage * AreaFactor;
}
//______________________________________________________________________________
void collect_eb_terms(const energy_bal_struct &energy,
                      const snow_data_struct  &snow,
                      const cell_data_struct  &cell_wet,
                      const double            Cv,
                      const double            AreaFract,
                      const double            TreeAdjustFactor,
                      const int               HasVeg,
                      const int               IsWet,
                      const double            lakefactor,
                      const int               overstory,
                      const int               band,
                      int *Tfoliage_fbcount,
                      int *Tcanopy_fbcount,
                      int *Tsnowsur_fbcountf,
                      int *Tsurf_fbcount,
                      int *Tsoil_fbcount,
                      out_data_struct  *out_data
#if SPATIAL_FROST
                      ,double           *frost_fract
                      ,double            frost_slope
#endif // SPATIAL_FROST
                      )
{

  extern option_struct    options;
  double rad_temp;
  double surf_temp;
  double AreaFactor = Cv * AreaFract * TreeAdjustFactor * lakefactor;
  /**********************************
    Record Frozen Soil Variables
  **********************************/
  /** record freezing and thawing front depths **/
  if (options.FROZEN_SOIL) {
    for (int index = 0; index < MAX_FRONTS; index++) {
      if(energy.fdepth[index] != MISSING)
        out_data[OUT_FDEPTH].data[index] += energy.fdepth[index] * AreaFactor * 100.;
      if(energy.tdepth[index] != MISSING)
        out_data[OUT_TDEPTH].data[index] += energy.tdepth[index] * AreaFactor * 100.;
    }
  }
  double tmp_fract = 0;
#if SPATIAL_FROST
  for (int frost_area = 0; frost_area < FROST_SUBAREAS; frost_area++ )
    if ( cell_wet.layer[0].ice[frost_area] )
      tmp_fract  += frost_fract[frost_area];
#else
  if ( cell_wet.layer[0].ice > 0 )
    tmp_fract   = 1.;
#endif
  out_data[OUT_SURF_FROST_FRAC].data[0] += tmp_fract * AreaFactor;
  //160512LML   tmp_fract = 0;
  //160512LML #if SPATIAL_FROST
  //160512LML   if ( (energy.T[0] + frost_slope / 2.) > 0 ) {
  //160512LML     if ( (energy.T[0] - frost_slope / 2.) <= 0 )
  //160512LML       tmp_fract += linear_interp( 0, (energy.T[0] + frost_slope / 2.), (energy.T[0] - frost_slope / 2.), 1, 0) * AreaFactor;
  //160512LML   }
  //160512LML   else
  //160512LML     tmp_fract += 1. * AreaFactor;
  //160512LML #else
  //160512LML   if ( energy.T[0] <= 0 )
  //160512LML     tmp_fract = 1. * AreaFactor;
  //160512LML #endif
  /**********************************
    Record Energy Balance Variables
  **********************************/
  if ( overstory && snow.snow && !(options.LAKES && IsWet))
    rad_temp = energy.Tfoliage + KELVIN;
  else
    rad_temp = energy.Tsurf + KELVIN;
  surf_temp = energy.Tsurf;
  if(!HasVeg) {
    // landcover is bare soil
    out_data[OUT_BARESOILT].data[0] += (rad_temp-KELVIN) * AreaFactor;
  } else {
    // landcover is vegetation
    if ( overstory && !snow.snow )
      // here, rad_temp will be wrong since it will pick the understory temperature
      out_data[OUT_VEGT].data[0]    += energy.Tfoliage * AreaFactor;
    else
      out_data[OUT_VEGT].data[0]    += (rad_temp-KELVIN) * AreaFactor;
  }
  out_data[OUT_SURF_TEMP].data[0]   += surf_temp * AreaFactor;
  for (int index = 0; index < options.Nnode; index++)
    out_data[OUT_SOIL_TNODE].data[index] += energy.T[index] * AreaFactor;
  if (IsWet) {
    for (int index = 0; index < options.Nnode; index++)
      out_data[OUT_SOIL_TNODE_WL].data[index] = energy.T[index];
  }
  out_data[OUT_SURFT_FBFLAG].data[0]        += energy.Tsurf_fbflag * AreaFactor;
  *Tsurf_fbcount                 += energy.Tsurf_fbcount;
  for (int index = 0; index < options.Nnode; index++) {
    out_data[OUT_SOILT_FBFLAG].data[index]  += energy.T_fbflag[index] * AreaFactor;
    *Tsoil_fbcount               += energy.T_fbcount[index];
  }
  out_data[OUT_SNOWT_FBFLAG].data[0]    += snow.surf_temp_fbflag * AreaFactor;
  *Tsnowsur_fbcountf         += snow.surf_temp_fbcount;
  out_data[OUT_TFOL_FBFLAG].data[0]     += energy.Tfoliage_fbflag * AreaFactor;
  *Tfoliage_fbcount          += energy.Tfoliage_fbcount;
  out_data[OUT_TCAN_FBFLAG].data[0]     += energy.Tcanopy_fbflag * AreaFactor;
  *Tcanopy_fbcount           += energy.Tcanopy_fbcount;
  out_data[OUT_NET_SHORT].data[0]       += energy.NetShortAtmos * AreaFactor;
  out_data[OUT_NET_LONG].data[0]        += energy.NetLongAtmos * AreaFactor;
  if ( snow.snow && overstory )
    out_data[OUT_IN_LONG].data[0]       += energy.LongOverIn * AreaFactor;
  else
    out_data[OUT_IN_LONG].data[0]       += energy.LongUnderIn * AreaFactor;
  if ( snow.snow && overstory )
    out_data[OUT_ALBEDO].data[0]        += energy.AlbedoOver * AreaFactor;
  else
    out_data[OUT_ALBEDO].data[0]        += energy.AlbedoUnder * AreaFactor;
  out_data[OUT_LATENT].data[0]          -= energy.AtmosLatent * AreaFactor;
  out_data[OUT_LATENT_SUB].data[0]      -= energy.AtmosLatentSub * AreaFactor;
  out_data[OUT_SENSIBLE].data[0]        -= energy.AtmosSensible * AreaFactor;
  out_data[OUT_GRND_FLUX].data[0]       -= energy.grnd_flux * AreaFactor;
  out_data[OUT_DELTAH].data[0]          -= energy.deltaH * AreaFactor;
  out_data[OUT_FUSION].data[0]          -= energy.fusion * AreaFactor;
//  /** record energy balance error **/
//  out_data[OUT_ENERGY_ERROR].data[0] += energy.error * AreaFactor;
  out_data[OUT_RAD_TEMP].data[0]        += ((rad_temp) * (rad_temp) * (rad_temp) * (rad_temp)) * AreaFactor;
  out_data[OUT_DELTACC].data[0]         += energy.deltaCC * AreaFactor;
  if (snow.snow && overstory)
    out_data[OUT_ADVECTION].data[0]     += energy.canopy_advection * AreaFactor;
  out_data[OUT_ADVECTION].data[0]       += energy.advection * AreaFactor;
  out_data[OUT_SNOW_FLUX].data[0]       += energy.snow_flux * AreaFactor;
  if (snow.snow && overstory)
    out_data[OUT_RFRZ_ENERGY].data[0]   += energy.canopy_refreeze * AreaFactor;
  out_data[OUT_RFRZ_ENERGY].data[0]     += energy.refreeze_energy * AreaFactor;
  out_data[OUT_MELT_ENERGY].data[0]     += energy.melt_energy * AreaFactor;
  if ( !overstory )
    out_data[OUT_ADV_SENS].data[0]      -= energy.advected_sensible * AreaFactor;
  //**********************************
  //  Record Band-Specific Variables
  //**********************************/
  out_data[OUT_SWE_BAND].data[band]             += snow.swq * Cv * lakefactor * 1000.;
  out_data[OUT_SNOW_DEPTH_BAND].data[band]      += snow.depth * Cv * lakefactor * 100.;
  if (HasVeg)
    out_data[OUT_SNOW_CANOPY_BAND].data[band]   += (snow.snow_canopy) * Cv * lakefactor * 1000.;
  out_data[OUT_SNOW_MELT_BAND].data[band]       += snow.melt * Cv * lakefactor;
  out_data[OUT_SNOW_COVER_BAND].data[band]      += snow.coverage * Cv * lakefactor;
  out_data[OUT_DELTACC_BAND].data[band]         += energy.deltaCC * Cv * lakefactor;
  out_data[OUT_ADVECTION_BAND].data[band]       += energy.advection * Cv * lakefactor;
  out_data[OUT_SNOW_FLUX_BAND].data[band]       += energy.snow_flux * Cv * lakefactor;
  out_data[OUT_RFRZ_ENERGY_BAND].data[band]     += energy.refreeze_energy * Cv * lakefactor;
  out_data[OUT_MELT_ENERGY_BAND].data[band]     += energy.melt_energy * Cv * lakefactor;
  out_data[OUT_ADV_SENS_BAND].data[band]        -= energy.advected_sensible * Cv * lakefactor;
  out_data[OUT_SNOW_SURFT_BAND].data[band]      += snow.surf_temp * Cv * lakefactor;
  out_data[OUT_SNOW_PACKT_BAND].data[band]      += snow.pack_temp * Cv * lakefactor;
  out_data[OUT_LATENT_SUB_BAND].data[band]      += energy.latent_sub * Cv * lakefactor;
  out_data[OUT_NET_SHORT_BAND].data[band]       += energy.NetShortAtmos * Cv * lakefactor;
  out_data[OUT_NET_LONG_BAND].data[band]        += energy.NetLongAtmos * Cv * lakefactor;
  if (snow.snow && overstory)
    out_data[OUT_ALBEDO_BAND].data[band]        += energy.AlbedoOver * Cv * lakefactor;
  else
    out_data[OUT_ALBEDO_BAND].data[band]        += energy.AlbedoUnder * Cv * lakefactor;
  out_data[OUT_LATENT_BAND].data[band]          -= energy.latent * Cv * lakefactor;
  out_data[OUT_SENSIBLE_BAND].data[band]        -= energy.sensible * Cv * lakefactor;
  out_data[OUT_GRND_FLUX_BAND].data[band]       -= energy.grnd_flux * Cv * lakefactor;
}
//______________________________________________________________________________
double get_soilprofile_water_from_output_mm(const out_data_struct *outdata,
                                            const soil_con_struct *soil_con)
{
    double storage = 0;
    for (int index = 0; index < options.Nlayer; index++)
      if(options.MOISTFRACT)
        storage += (outdata[OUT_SOIL_LIQ].data[index] + outdata[OUT_SOIL_ICE].data[index])
                   * soil_con->depth[index] * 1000;
      else
        storage += outdata[OUT_SOIL_LIQ].data[index] + outdata[OUT_SOIL_ICE].data[index];
    return storage;
}
//160512LML_____________________________________________________________________
bool calc_treeline_adjustment_factors(const int rec, const char* AboveTreeLine,const veg_con_struct *veg_con,
                                      const lake_con_struct &lake_con, const lake_var_struct &lake_var,
                                      double *TreeAdjustFactor)
{
  for (int band = 0; band < options.SNOW_BAND; band++ ) {
    if ( AboveTreeLine[band] ) {
      double Cv = 0;
      for (int veg = 0 ; veg < veg_con[0].vegetat_type_num ; veg++ ) {
        #ifdef VIC_CROPSYST_VERSION
        if ( veg_con[veg].VCS.veg_lib[band]->overstory ) {
        #else
        if ( veg_lib[veg_con[veg].veg_class].overstory ) {
        #endif
          if (options.LAKES && veg_con[veg].LAKE) {
            if (band == 0) {
              // Fraction of tile that is flooded
              double Clake = lake_var.sarea / lake_con.basin[0];
              Cv += veg_con[veg].Cv * (1. - Clake);
            }
          } else {
            Cv += veg_con[veg].Cv;
          }
        }
      }
      TreeAdjustFactor[band] = 1. / ( 1. - Cv );
    } else {
      TreeAdjustFactor[band] = 1.;
    }
    if ( TreeAdjustFactor[band] != 1 && rec == 0 )
      fprintf( stderr, "WARNING: Tree adjust factor for band %i is equal to %f.\n", band, TreeAdjustFactor[band] );
  } //band
  return true;
}
//160512LML_____________________________________________________________________
bool aggregate_output_in_temporal_dimention(const int out_step_ratio, out_data_struct *out_data)
{
  for (int v = 0; v < N_OUTVAR_TYPES; v++) {
    if (out_data[v].aggtype == AGG_TYPE_END) {
      for (int i = 0; i < out_data[v].nelem; i++) {
        out_data[v].aggdata[i] = out_data[v].data[i];
      }
    }
    else if (out_data[v].aggtype == AGG_TYPE_SUM) {
      for (int i = 0; i < out_data[v].nelem; i++) {
        out_data[v].aggdata[i] += out_data[v].data[i];
      }
    }
    else if (out_data[v].aggtype == AGG_TYPE_AVG) {
      for (int i = 0; i < out_data[v].nelem; i++) {
        out_data[v].aggdata[i] += out_data[v].data[i] / (double)out_step_ratio;
      }
    }
  }
  out_data[OUT_AERO_RESIST].aggdata[0]  = 1. / out_data[OUT_AERO_COND].aggdata[0];
  out_data[OUT_AERO_RESIST1].aggdata[0] = 1. / out_data[OUT_AERO_COND1].aggdata[0];
  out_data[OUT_AERO_RESIST2].aggdata[0] = 1. / out_data[OUT_AERO_COND2].aggdata[0];
  return true;
}
//160512LML_____________________________________________________________________
bool transform_for_ALMA_output(const int out_dt_sec, out_data_struct *out_data)
{
  double db_out_dt_sec = (double)out_dt_sec;
  out_data[OUT_BASEFLOW].aggdata[0] /= db_out_dt_sec;
  out_data[OUT_EVAP].aggdata[0] /= db_out_dt_sec;
  out_data[OUT_EVAP_BARE].aggdata[0] /= db_out_dt_sec;
  out_data[OUT_EVAP_CANOP].aggdata[0] /= db_out_dt_sec;
  out_data[OUT_INFLOW].aggdata[0] /= db_out_dt_sec;
  out_data[OUT_LAKE_BF_IN].aggdata[0] /= db_out_dt_sec;
  out_data[OUT_LAKE_BF_IN_V].aggdata[0] /= db_out_dt_sec;
  out_data[OUT_LAKE_BF_OUT].aggdata[0] /= db_out_dt_sec;
  out_data[OUT_LAKE_BF_OUT_V].aggdata[0] /= db_out_dt_sec;
  out_data[OUT_LAKE_CHAN_IN].aggdata[0] /= db_out_dt_sec;
  out_data[OUT_LAKE_CHAN_IN_V].aggdata[0] /= db_out_dt_sec;
  out_data[OUT_LAKE_CHAN_OUT].aggdata[0] /= db_out_dt_sec;
  out_data[OUT_LAKE_CHAN_OUT_V].aggdata[0] /= db_out_dt_sec;
  out_data[OUT_LAKE_DSTOR].aggdata[0] /= db_out_dt_sec;
  out_data[OUT_LAKE_DSTOR_V].aggdata[0] /= db_out_dt_sec;
  out_data[OUT_LAKE_DSWE].aggdata[0] /= db_out_dt_sec;
  out_data[OUT_LAKE_DSWE_V].aggdata[0] /= db_out_dt_sec;
  out_data[OUT_LAKE_EVAP].aggdata[0] /= db_out_dt_sec;
  out_data[OUT_LAKE_EVAP_V].aggdata[0] /= db_out_dt_sec;
  out_data[OUT_LAKE_ICE_TEMP].aggdata[0] += KELVIN;
  out_data[OUT_LAKE_PREC_V].aggdata[0] /= db_out_dt_sec;
  out_data[OUT_LAKE_RCHRG].aggdata[0] /= db_out_dt_sec;
  out_data[OUT_LAKE_RCHRG_V].aggdata[0] /= db_out_dt_sec;
  out_data[OUT_LAKE_RO_IN].aggdata[0] /= db_out_dt_sec;
  out_data[OUT_LAKE_RO_IN_V].aggdata[0] /= db_out_dt_sec;
  out_data[OUT_LAKE_VAPFLX].aggdata[0] /= db_out_dt_sec;
  out_data[OUT_LAKE_VAPFLX_V].aggdata[0] /= db_out_dt_sec;
  out_data[OUT_LAKE_SURF_TEMP].aggdata[0] += KELVIN;
  out_data[OUT_PREC].aggdata[0] /= db_out_dt_sec;
  out_data[OUT_RAINF].aggdata[0] /= db_out_dt_sec;
  out_data[OUT_REFREEZE].aggdata[0] /= db_out_dt_sec;
  out_data[OUT_RUNOFF].aggdata[0] /= db_out_dt_sec;
  out_data[OUT_SNOW_MELT].aggdata[0] /= db_out_dt_sec;
  out_data[OUT_SNOWF].aggdata[0] /= db_out_dt_sec;
  out_data[OUT_SUB_BLOWING].aggdata[0] /= db_out_dt_sec;
  out_data[OUT_SUB_CANOP].aggdata[0] /= db_out_dt_sec;
  out_data[OUT_SUB_SNOW].aggdata[0] /= db_out_dt_sec;
  out_data[OUT_SUB_SNOW].aggdata[0] += out_data[OUT_SUB_CANOP].aggdata[0];
  out_data[OUT_SUB_SURFACE].aggdata[0] /= db_out_dt_sec;
  out_data[OUT_TRANSP_VEG].aggdata[0] /= db_out_dt_sec;
  out_data[OUT_BARESOILT].aggdata[0] += KELVIN;
  out_data[OUT_SNOW_PACK_TEMP].aggdata[0] += KELVIN;
  out_data[OUT_SNOW_SURF_TEMP].aggdata[0] += KELVIN;
  for (int index=0; index < options.Nlayer; index++) {
    out_data[OUT_SOIL_TEMP].aggdata[index] += KELVIN;
  }
  for (int index=0; index < options.Nnode; index++) {
    out_data[OUT_SOIL_TNODE].aggdata[index] += KELVIN;
    out_data[OUT_SOIL_TNODE_WL].aggdata[index] += KELVIN;
  }
  out_data[OUT_SURF_TEMP].aggdata[0] += KELVIN;
  out_data[OUT_VEGT].aggdata[0] += KELVIN;
  out_data[OUT_FDEPTH].aggdata[0] /= 100.;
  out_data[OUT_TDEPTH].aggdata[0] /= 100.;
  out_data[OUT_DELTACC].aggdata[0] *= db_out_dt_sec;
  out_data[OUT_DELTAH].aggdata[0] *= db_out_dt_sec;
  out_data[OUT_AIR_TEMP].aggdata[0] += KELVIN;
  out_data[OUT_PRESSURE].aggdata[0] *= 1000.;
  out_data[OUT_VP].aggdata[0] *= 1000.;
  out_data[OUT_VPD].aggdata[0] *= 1000.;
  return true;
}
//160512LML_____________________________________________________________________
bool store_veg_outputs(const double *AreaFract,
                const double *TreeAdjustFactor,
                const char *AboveTreeLine,
                const atmos_data_struct *atmos,
                const soil_con_struct   *soil_con,
                const veg_con_struct    *veg_con,
                const lake_con_struct   *lake_con,
                dist_prcp_struct  *prcp,
                out_data_struct   *out_data,
                int *Tfoliage_fbcount,
                int *Tcanopy_fbcount,
                int *Tsnowsur_fbcountf,
                int *Tsurf_fbcount,
                int *Tsoil_fbcount,
                double *cv_lake,
                double *cv_veg,
                double *cv_baresoil,
                double *cv_snow,
                double *cv_overstory
                )
{
  cell_data_struct ***cell    = prcp->cell;
  energy_bal_struct **energy  = prcp->energy;
  lake_var_struct &lake_var   = prcp->lake_var;
  snow_data_struct **snow     = prcp->snow;
  veg_var_struct ***veg_var   = prcp->veg_var;
  const double *depth               = soil_con->depth;
  #if SPATIAL_FROST
  const double *frost_fract = soil_con->frost_fract;
  const double frost_slope = soil_con->frost_slope;
  #endif // SPATIAL_FROST

  for (int veg = 0 ; veg <= veg_con[0].vegetat_type_num ; veg++) {
    double Cv = veg_con[veg].Cv;
    double Clake = 0;
    int Nbands = options.SNOW_BAND;
    bool IsWet = false;
    bool HasVeg = false;
    if (veg < veg_con[0].vegetat_type_num)
      HasVeg = true;
    if ( Cv > 0) {
      // Check if this is lake/wetland tile
      if (options.LAKES && veg_con[veg].LAKE) {
        Clake = lake_var.sarea / lake_con->basin[0];
        Nbands = 1;
        IsWet = true;
      }
      //*********************************
      //  Store Output for all Bands
      //*********************************
      for (int band = 0; band < Nbands; band++) {
        char overstory =
            #ifdef VIC_CROPSYST_VERSION
            veg_con[veg].VCS.veg_lib[band]->overstory;
            #else
            veg_lib[veg_con[veg].veg_class].overstory;
            #endif
        double ThisAreaFract = AreaFract[band];
        double ThisTreeAdjust = TreeAdjustFactor[band];
        if (IsWet) {
          ThisAreaFract  = 1.;
          ThisTreeAdjust = 1.;
        }
        if((ThisAreaFract > 0.) &&
           ((veg == veg_con[0].vegetat_type_num) ||
            (!AboveTreeLine[band]) ||
            (AboveTreeLine[band] && !overstory)
            )
           ) {
          //*******************************************************
          //  Store Output for Wet and Dry Fractions
          //*******************************************************
          for (int dist = 0; dist < options.NDIST; dist++ ) {
            double mu = 0;
            if(dist == 0)
              mu = prcp[0].mu[veg];
            else
              mu = 1. - prcp[0].mu[veg];
            //** compute running totals of various landcovers **
            double fraction = Cv * mu * ThisAreaFract * ThisTreeAdjust;
            if (HasVeg)
              *cv_veg        += fraction;
            else
              *cv_baresoil   += fraction;
            if (overstory)
              *cv_overstory  += fraction;
            if (snow[veg][band].swq> 0.0)
              *cv_snow       += fraction;

        /*********************************
              Record Water Balance Terms
        *********************************/
            collect_wb_terms(cell[dist][veg][band], veg_var[dist][veg][band], snow[veg][band],
                             mu, Cv, ThisAreaFract, ThisTreeAdjust, HasVeg,
                             (1-Clake), overstory, depth,
                             #if SPATIAL_FROST
                             frost_fract,
                             #endif // SPATIAL_FROST
                             out_data);
          } // End wet/dry loop
          //**********************************
          //  Record Energy Balance Terms
          //**********************************
          collect_eb_terms(energy[veg][band], snow[veg][band], cell[WET][veg][band],
                           Cv, ThisAreaFract, ThisTreeAdjust,
                           HasVeg, 0, (1-Clake), overstory, band,
                           Tfoliage_fbcount,
                           Tcanopy_fbcount,
                           Tsnowsur_fbcountf,
                           Tsurf_fbcount,
                           Tsoil_fbcount,
                           out_data
                           #if SPATIAL_FROST
                           ,frost_fract
                           ,frost_slope
                           #endif // SPATIAL_FROST
                           );
          // Store Wetland-Specific Variables
          if (IsWet) {
            // Wetland soil temperatures
            for (int i = 0; i < options.Nnode; i++) {
              out_data[OUT_SOIL_TNODE_WL].data[i] = energy[veg][band].T[i];
            }
          }
          //**********************************
          //  Record Lake Variables
          //**********************************
          if (IsWet) {
            // Override some variables of soil under lake with those of wetland
            // This is for those variables whose lake values shouldn't be included
            // in grid cell average
            // Note: doing this for eb terms will lead to reporting of eb errors
            // this should be fixed when we implement full thermal solution beneath lake
            for (int i = 0; i < MAX_FRONTS; i++) {
              lake_var.energy.fdepth[i]      = energy[veg][band].fdepth[i];
              lake_var.energy.tdepth[i]      = energy[veg][band].fdepth[i];
            }
            for (int i = 0; i < options.Nnode; i++) {
              lake_var.energy.ice[i]         = energy[veg][band].ice[i];
              lake_var.energy.T[i]           = energy[veg][band].T[i];
            }
            for (int i = 0; i < N_PET_TYPES; i++) {
              lake_var.soil.pot_evap[i]      = cell[WET][veg][band].pot_evap[i];
            }
            lake_var.soil.rootmoist          = cell[WET][veg][band].rootmoist;
            lake_var.energy.deltaH           = energy[veg][band].deltaH;
            lake_var.energy.fusion           = energy[veg][band].fusion;
            lake_var.energy.grnd_flux        = energy[veg][band].grnd_flux;
            //*********************************
            //    Record Water Balance Terms
            //*********************************
            collect_wb_terms(lake_var.soil, veg_var[WET][0][0], lake_var.snow,
                             1.0, Cv, ThisAreaFract, ThisTreeAdjust, 0,
                             Clake, overstory, depth,
                             #if SPATIAL_FROST
                             frost_fract,
                             #endif // SPATIAL_FROST
                             out_data);
        /**********************************
          Record Energy Balance Terms
        **********************************/
            collect_eb_terms(lake_var.energy, lake_var.snow, lake_var.soil,
                             Cv, ThisAreaFract, ThisTreeAdjust,
                             0, 1, Clake, overstory, band,
                             Tfoliage_fbcount,
                             Tcanopy_fbcount,
                             Tsnowsur_fbcountf,
                             Tsurf_fbcount,
                             Tsoil_fbcount,
                             out_data
                             #if SPATIAL_FROST
                             ,frost_fract
                             ,frost_slope
                             #endif // SPATIAL_FROST
                             );
            // Store Lake-Specific Variables
            // Lake ice
            if (lake_var.new_ice_area > 0.0) {
              out_data[OUT_LAKE_ICE].data[0]        = (lake_var.ice_water_eq/lake_var.new_ice_area) * ice_density / RHO_W;
              out_data[OUT_LAKE_ICE_TEMP].data[0]   = lake_var.tempi;
              out_data[OUT_LAKE_ICE_HEIGHT].data[0] = lake_var.hice;
              out_data[OUT_LAKE_SWE].data[0]        = lake_var.swe/lake_var.areai;      // m over lake ice
              out_data[OUT_LAKE_SWE_V].data[0]      = lake_var.swe;                   // m3
            } else {
              out_data[OUT_LAKE_ICE].data[0]        = 0.0;
              out_data[OUT_LAKE_ICE_TEMP].data[0]   = 0.0;
              out_data[OUT_LAKE_ICE_HEIGHT].data[0] = 0.0;
              out_data[OUT_LAKE_SWE].data[0]        = 0.0;
              out_data[OUT_LAKE_SWE_V].data[0]      = 0.0;
            }
            out_data[OUT_LAKE_DSWE_V].data[0]       = lake_var.swe - lake_var.swe_save; // m3
            out_data[OUT_LAKE_DSWE].data[0]         = (lake_var.swe - lake_var.swe_save)*1000/soil_con->cell_area; // mm over gridcell
            // Lake dimensions
            out_data[OUT_LAKE_AREA_FRAC].data[0]    = Cv*Clake;
            out_data[OUT_LAKE_DEPTH].data[0]        = lake_var.ldepth;
            out_data[OUT_LAKE_SURF_AREA].data[0]    = lake_var.sarea;
            if (out_data[OUT_LAKE_SURF_AREA].data[0] > 0)
                out_data[OUT_LAKE_ICE_FRACT].data[0] = lake_var.new_ice_area/out_data[OUT_LAKE_SURF_AREA].data[0];
            else
                out_data[OUT_LAKE_ICE_FRACT].data[0]  = 0.;
            out_data[OUT_LAKE_VOLUME].data[0]       = lake_var.volume;
            out_data[OUT_LAKE_DSTOR_V].data[0]      = lake_var.volume - lake_var.volume_save;
            out_data[OUT_LAKE_DSTOR].data[0]        = (lake_var.volume - lake_var.volume_save)*1000/soil_con->cell_area; // mm over gridcell
            // Other lake characteristics
            out_data[OUT_LAKE_SURF_TEMP].data[0]    = lake_var.temp[0];
            if (out_data[OUT_LAKE_SURF_AREA].data[0] > 0) {
              out_data[OUT_LAKE_MOIST].data[0]      = (lake_var.volume / soil_con->cell_area) * 1000.; // mm over gridcell
              out_data[OUT_SURFSTOR].data[0]        = (lake_var.volume / soil_con->cell_area) * 1000.; // same as OUT_LAKE_MOIST
            } else {
              out_data[OUT_LAKE_MOIST].data[0]      = 0;
              out_data[OUT_SURFSTOR].data[0]        = 0;
            }
            // Lake moisture fluxes
            out_data[OUT_LAKE_BF_IN_V].data[0]      = lake_var.baseflow_in;      // m3
            out_data[OUT_LAKE_BF_OUT_V].data[0]     = lake_var.baseflow_out;     // m3
            out_data[OUT_LAKE_CHAN_IN_V].data[0]    = lake_var.channel_in;       // m3
            out_data[OUT_LAKE_CHAN_OUT_V].data[0]   = lake_var.runoff_out;       // m3
            out_data[OUT_LAKE_EVAP_V].data[0]       = lake_var.evapw;            // m3
            out_data[OUT_LAKE_PREC_V].data[0]       = lake_var.prec;             // m3
            out_data[OUT_LAKE_RCHRG_V].data[0]      = lake_var.recharge;         // m3
            out_data[OUT_LAKE_RO_IN_V].data[0]      = lake_var.runoff_in;        // m3
            out_data[OUT_LAKE_VAPFLX_V].data[0]     = lake_var.vapor_flux;       // m3
            out_data[OUT_LAKE_BF_IN].data[0]        = lake_var.baseflow_in*1000./soil_con->cell_area; // mm over gridcell
            out_data[OUT_LAKE_BF_OUT].data[0]       = lake_var.baseflow_out*1000./soil_con->cell_area; // mm over gridcell
            out_data[OUT_LAKE_CHAN_OUT].data[0]     = lake_var.runoff_out*1000./soil_con->cell_area; // mm over gridcell
            out_data[OUT_LAKE_EVAP].data[0]         = lake_var.evapw*1000./soil_con->cell_area; // mm over gridcell
            out_data[OUT_LAKE_RCHRG].data[0]        = lake_var.recharge*1000./soil_con->cell_area; // mm over gridcell
            out_data[OUT_LAKE_RO_IN].data[0]        = lake_var.runoff_in*1000./soil_con->cell_area; // mm over gridcell
            out_data[OUT_LAKE_VAPFLX].data[0]       = lake_var.vapor_flux*1000./soil_con->cell_area; // mm over gridcell
          } // End if options.LAKES etc.
        } // End if ThisAreaFract etc.
#ifdef VIC_CROPSYST_VERSION                                                      /*keyvan added 11142012*/
          //** Crop model output  KJC 08242010 **
          //**Note that even though the array length of a crop output variable equlas the number of crop in the
          //library it will store values for only those crops in the current grid cell and in the order of the
          //crop occurence in the vegparam file.
          //For example, if there are 10 crops in the crop library,
          //out_data[OUT_CROP_BIOM_CUR].nelem = 10 and out_data[OUT_CROP_BIOM_CUR].data[0 to 9]
          //
          //If the current cell has only 3 crops, 210, 218 and 1827 (crop codes in crop library) in that order, only
          //out_data[OUT_CROP_BIOM_CUR].data[0 to 2] will have values respectively to the crops 210,218 and 1827.
          //KJC 02162011 *
          for (int dist = 0; dist < options.NDIST; dist++ ) {
            double mu = 0;
            if(dist == 0)
              mu = prcp[0].mu[veg];
            else
              mu = 1. - prcp[0].mu[veg];
            int crop_counter = get_crop_counter(veg_con,veg_con[veg].VCS.veg_class_code);
            //150929LML for(crop_counter=0;crops && crop_counter<crops[0].no_of_crops_in_cell; crop_counter++){  //120824 RLN there could be no crops in the cell
            if (veg_var[dist][veg][band].crop_state) {
                crop_data_struct &current_crop = *veg_var[dist][veg][band].crop_state;  //150929LML crops[crop_counter];
                double fraction = mu * ThisAreaFract * ThisTreeAdjust;
                //170924LML double fraction_for_gridcell = fraction * veg_con[veg].Cv;
                fraction *= veg_con[veg].Cv;                                     //170924LML
                out_data[OUT_CROP_IRRI_WAT].data[crop_counter]   += fraction * current_crop.irrigation_water;

                //if (current_crop.irrigation_water > 100) {
                //    std::clog << "current_crop.irrigation_water:" << current_crop.irrigation_water << "\n";
                //}

                for (int index = 0; index < options.Nlayer-1; index++) {
                  out_data[OUT_CROP_TRANSPIR].data[crop_counter] += fraction * current_crop.transpiration[index];
                }
                out_data[OUT_CROP_TRANS0 ].data[crop_counter]    += fraction * current_crop.transpiration[0];
                out_data[OUT_CROP_TRANS1 ].data[crop_counter]    += fraction * current_crop.transpiration[1];
                out_data[OUT_CROP_TRANS2 ].data[crop_counter]    += fraction * current_crop.transpiration[2];
                out_data[OUT_CROP_LAI].data[crop_counter]        += fraction * current_crop.crop_leaf_area_index;
                out_data[OUT_CROP_WSI].data[crop_counter]        += fraction * current_crop.water_stress_index;
                out_data[OUT_CROP_GROWTHST].data[crop_counter]   += fraction * current_crop.growth_stage;
                out_data[OUT_CROP_BIOM_CUR].data[crop_counter]   += fraction * current_crop.biomass_current;
                out_data[OUT_CROP_BIOMYELD].data[crop_counter]   += fraction * current_crop.biomass_yield;
                out_data[OUT_CROP_BIOM_AHA].data[crop_counter]   += fraction * current_crop.biomass_after_harvest;
                #ifdef CROPSYST_PROVIDED_SOIL_HYDROLOGY
                //12-08-03
                out_data[OUT_LYR0_VW ].data[crop_counter]        += fraction * current_crop.soil_vol_wat_content[0];
                out_data[OUT_LYR1_VW ].data[crop_counter]        += fraction * current_crop.soil_vol_wat_content[1];
                out_data[OUT_LYR2_VW ].data[crop_counter]        += fraction * current_crop.soil_vol_wat_content[2];
                #endif
                out_data[OUT_SWD0 ].data[crop_counter]           += fraction * cell[dist][veg][band].layer[0].moist;
                out_data[OUT_SWD1 ].data[crop_counter]           += fraction * cell[dist][veg][band].layer[1].moist;
                out_data[OUT_SWD2 ].data[crop_counter]           += fraction * cell[dist][veg][band].layer[2].moist;
                out_data[OUT_CROP_EVAP ].data[crop_counter]      += fraction * current_crop.evaporation;
                out_data[OUT_INFILT ].data[crop_counter]         += fraction * cell[dist][veg][band].VCS.infiltration;//this should change to represent the right crop later
                //130503 keyvan added
                out_data[OUT_EVAP_FROM_IRRIG].data[crop_counter] += fraction * current_crop.irrigation_evap;/// needs to be checked
                out_data[OUT_EVAP_CANOP_IRRIG].data[crop_counter]+= fraction * current_crop.intercepted_irrigation;// + crops[crop_counter].intercepted_irrigation;/// need to be checked
                out_data[OUT_EVAP_SOIL].data[crop_counter]       += fraction * cell[dist][veg][band].layer[0].VCS.evap_bare_soil;//150930LML current_crop.soil_evap;
                out_data[OUT_RUNOFF_IRRIG].data[crop_counter]    += fraction * current_crop.irrigation_runoff;
                out_data[OUT_ET_POT_SHORT].data[crop_counter]    += fraction * current_crop.pot_evap_short;
                out_data[OUT_CROP_COEF_KC].data[crop_counter]    += fraction * current_crop.crop_coef;
                out_data[OUT_CROP_SOIL_MOIST].data[crop_counter] += fraction * current_crop.soil_moist;
                out_data[OUT_CROP_TRANS_POT].data[crop_counter]  += fraction * current_crop.potential_transpir;
                out_data[OUT_CANOPY_FRACT].data[crop_counter]    += fraction * current_crop.canopy_interception_fraction; //130709 keyvan
                out_data[OUT_DEL_T].data[crop_counter]           += fraction * current_crop.delta_T;
                out_data[OUT_DEL_VPD].data[crop_counter]         += fraction * current_crop.delta_VPD;
                //170924LML out_data[OUT_EVAP_FROM_IRRIG].data[crop_counter] += fraction * cell[dist][veg][band].VCS.evap_from_irrigation_syst;         //150930LML
                //170924LML out_data[OUT_EVAP_CANOP_IRRIG].data[crop_counter]+= fraction * cell[dist][veg][band].VCS.evap_from_irrigation_intercept;    //150930LML
                //170924LML out_data[OUT_RUNOFF_IRRIG].data[crop_counter]    += fraction * cell[dist][veg][band].VCS.irrigation_runoff;                 //150930LML
                //170924LML out_data[OUT_IRRIGATION].data[crop_counter]      += fraction * cell[dist][veg][band].VCS.irrigation_water;            //150930LML
                //170924LML out_data[OUT_CELL_EVAP_FROM_IRRIG].data[0]       += fraction_for_gridcell * cell[dist][veg][band].VCS.evap_from_irrigation_syst;         //150930LML
                //170924LML out_data[OUT_CELL_EVAP_CANOP_IRRIG].data[0]      += fraction_for_gridcell * cell[dist][veg][band].VCS.evap_from_irrigation_intercept;    //150930LML
                //170924LML out_data[OUT_CELL_RUNOFF_IRRIG].data[0]          += fraction_for_gridcell * cell[dist][veg][band].VCS.irrigation_runoff;                 //150930LML
                //170924LML out_data[OUT_CELL_IRRIGATION].data[0]            += fraction_for_gridcell * cell[dist][veg][band].VCS.irrigation_water;            //150930LML
                crop_counter++;
              } //end crop
          } //dist
          //the following lines until #endif were used for outputting variables realted to crop model debugging. KJC
          #ifdef VIC_CROPSYST_VERSION
          out_data[OUT_TMIN].data[0] = atmos->VCS.tmin;                              //130219[NR];
          out_data[OUT_TMAX].data[0] = atmos->VCS.tmax;                              //130219[NR];
          #endif
          int daytime_hour_count = 0;                                            //For crop testing..Kiran 10142010
          double daytime_vpd = 0;                                                //For crop testing..Kiran 10142010
          for (int vpd_hour = 0; vpd_hour < NR; vpd_hour++) {                    //For crop testing..Kiran 10142010
            if (atmos->shortwave[vpd_hour] > 0.0){
                daytime_vpd += atmos->vpd[vpd_hour];
                daytime_hour_count++;
            }
          }
          out_data[OUT_DAY_VPD].data[0]     = daytime_vpd / (1000. * daytime_hour_count); // dividing it by 1000 converts it to kPa
          out_data[OUT_SHRT_WAVE].data[0]   = 0.0864 * atmos->shortwave[NR];     //0.0864 is a conversion factor for converting W/m2 to MJ/m2/d
          out_data[OUT_RHMAX].data[0]       = atmos->VCS.relative_humidity_max;
          out_data[OUT_RHMIN].data[0]       = atmos->VCS.relative_humidity_min;
          out_data[OUT_WS].data[0]          = atmos->wind[NR];
#endif //VIC_CROPSYST_VERSION
      } // End loop over bands
    } // End if Cv > 0
  } // End loop over veg
  return true;
}
