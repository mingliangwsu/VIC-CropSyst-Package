#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <vicNl.h>
#include <math.h>
/*170413LML
#if (VIC_CROPSYST_VERSION>=3)
#include "agronomic/VIC_land_unit_C_interface.h"
#include "agronomic/VIC_land_unit_simulation.h"
int set_average_veglib_for_crop(veg_lib_struct &veglib,
                                const veg_var_struct * const * const *veg_var,
                                const dist_prcp_struct *prcp,
                                const soil_con_struct *soil_con,
                                const int veg_index,
                                const int current_month);                        //150929LML
#endif
*/
#include <iostream>
static char vcid[] = "$Id: full_energy.c,v 5.8.2.30 2012/08/29 00:10:56 vicadmin Exp $";
//double **accessable_aero_resistance; ///121129 keyvan
int  full_energy(char                 NEWCELL,
                 int                  gridcell,
                 int                  rec,
                 atmos_data_struct   *atmos,
                 dist_prcp_struct    *prcp,
                 dmy_struct          *dmy,
                 global_param_struct *gp,
                 const lake_con_struct     *lake_con,
                 soil_con_struct     *soil_con,
                 veg_con_struct      *veg_con
#ifdef VIC_CROPSYST_VERSION
                 #if VIC_CROPSYST_VERSION==2
                 ,crop_data_struct   *crops
                 #endif
                 #ifndef FULL_IRRIGATION
                 ,std::map<int,std::map<int,irrigation_pattern_struct>> &irrig_patt //keyvan 130604
                 #endif
#endif
                 )
/**********************************************************************
    full_energy    Keith Cherkauer        January 8, 1997

  This subroutine controls the model core, it solves both the energy
  and water balance models, as well as frozen soils.

  modifications:
  07-98 restructured to fix problems with distributed precipitation,
        and to add the ability to solve the snow model at different
    elevation bands within a single grid cell.                 KAC
  01-19-00 modified to work with the new atmosphere data structure
           implemented when the radiation forcing routines were
       updated.  Also modified to use the new simplified
       soil moisture storage for the frozen soil algorithm.    KAC
  12-01-00 modified to include the lakes and wetlands algorithm.   KAC
  11-18-02 Modified to handle blowing snow.  Also added debugging
           output for lake model.                                  LCB
  05-27-03 Updated passing of veg_con parameters for blowing snow
           to surface_fluxes.  Original did not account for the fact
           that veg_con is not allocated for veg = Nveg (bare soil)
           case.  This eliminates a memory error.                  KAC
  28-Sep-04 Added aero_resist_used to store the aerodynamic resistance
        used in flux calculations.                TJB
  2006-Sep-23 Implemented flexible output configuration; now computation
          of soil wetness and root zone soil moisture happens here.        TJB
  2006-Nov-07 Removed LAKE_MODEL option.                    TJB
  2007-Apr-04 Modified to handle grid cell errors by returning to the
          main subroutine, rather than ending the simulation.        GCT/KAC
  2007-May-01 Added case of SPATIAL_FROST = TRUE in modifications
          from 2006-Sep-23.                            GCT
  2007-Aug-10 Added features for EXCESS_ICE option.
          Including calculating subsidence for each layer and
          updating soil depth, effective porosity,
          bulk density, and soil moisture and fluxes by calling
          runoff function if subsidence occurs.                JCA
  2007-Sep-07 No longer resets ice content to previous time-step ice content if
          subsidence has occurred.                        JCA
  2007-Sep-19 Added MAX_SUBSIDENCE parameter to EXCESS_ICE option.        JCA
  2007-Sep-19 Fixed bug in subsidence calculation.                JCA
  2007-Nov-06 Added veg_con to parameter list of lakemain().  Replaced
          lake.fraci with lake.areai.                    LCB via TJB
  2008-Jan-23 Changed ice0 from a scalar to an array.  Previously,
          when options.SNOW_BAND > 1, the value of ice0 computed
          for earlier bands was always overwritten by the value
          of ice0 computed for the final band (even if the final
          band had 0 area).                            JS via TJB
  2008-May-05 Changed moist from a scalar to an array (moist0).  Previously,
          when options.SNOW_BAND > 1, the value of moist computed
          for earlier bands was always overwritten by the value
          of moist computed for the final band (even if the final
          band had 0 area).                            KAC via TJB
  2009-Jan-16 Modified aero_resist_used and Ra_used to become arrays of
          two elements (surface and overstory); added
          options.AERO_RESIST_CANSNOW.                    TJB
  2009-May-17 Added asat to cell_data.                        TJB
  2009-Jun-09 Modified to use extension of veg_lib structure to contain
          bare soil information.                        TJB
  2009-Jun-09 Modified to compute aero_resist for all potential evap
          landcover types.                            TJB
  2009-Jun-09 Cell_data structure now only stores final aero_resist
          values (called "aero_resist").  Preliminary uncorrected
          aerodynamic resistances for current vegetation and various
          reference land cover types for use in potential evap
          calculations is stored in temporary array aero_resist.        TJB
  2009-Jun-26 Simplified argument list of runoff() by passing all cell_data
          variables via a single reference to the cell data structure.    TJB
  2009-Jul-22 Fixed error in assignment of cell.aero_resist.            TJB
  2009-Jul-31 Wetland portion of lake/wetland tile is now processed in
          full_energy() instead of wetland_energy().  Lake funcions are
          now called directly from full_energy instead of lakemain().    TJB
  2009-Sep-28 Moved lake_snow and lake_energy into lake_var structure.
          Removed calls to initialize_prcp and update_prcp.            TJB
  2009-Sep-30 Miscellaneous fixes for lake model.                TJB
  2009-Oct-05 Miscellaneous fixes for lake model, including updating/
          rescaling of lake and wetland storages and fluxes to account
          for changes in lake area.                        TJB
  2009-Nov-09 Changed definition of lake->sarea to include ice extent; other
          changes to handle case when lake fraction goes to 0.        LCB via TJB
  2010-Mar-31 Added runoff_in.                            TJB
  2010-Sep-24 Changed atmos.runoff_in to atmos.channel_in.  Added
          lake_var.channel_in to store it.                    TJB
  2010-Nov-02 Changed units of lake_var moisture fluxes to volume (m3).        TJB
  2010-Nov-26 Changed argument list of water_balance().                TJB
  2011-May-31 Prepare_full_energy() is now always called.            TJB
  2011-Jun-03 Added options.ORGANIC_FRACT.  Soil properties now take
          organic fraction into account.                    TJB
  2012-Jan-01 Modified condition for determining whether to simulate lakes
          to check whether lake_idx >= 0.                    TJB
  2012-Jan-16 Removed LINK_DEBUG code                        BN
  2012-Aug-28 Added accumulation of rain and snow over lake to grid cell
          totals.                                TJB
  2012-NOV-12 VIC_CropSyst functions are added              KM

**********************************************************************/
{
  extern veg_lib_struct *veg_lib;
  extern option_struct   options;
  int                    ErrorFlag;
#if SPATIAL_FROST
  int                    frost_area;
#endif // SPATIAL_FROST
  double                 out_prec[2*MAX_BANDS];
  double                 out_rain[2*MAX_BANDS];
  double                 out_snow[2*MAX_BANDS];
  double                 ice0[MAX_BANDS];
  double                 moist0[MAX_BANDS];
  double                 surf_atten;
  double                 wind_h;
  double                 displacement[3];                                        //160519LML note: If overstory exit, [1]: overstory [0]: understory
                                                                                 //Otherwise, [1]&[0]: same and they are for canopy
  double                 roughness[3];
  double                 ref_height[3];
  double                 tmp_wind[3];
  double                 Le;
  double                 Melt[2*MAX_BANDS];
  double                 bare_albedo;
  double                 snow_inflow[MAX_BANDS];
  double                 rainonly;
  double                 sum_runoff;
  double                 sum_baseflow;
  double                 tmp_mu;
  float                  lag_one;
  float                  sigma_slope;
  float                  fetch;
  int                    pet_veg_class;
  double                 lakefrac;
  double                 fraci;
  double                 wetland_runoff;
  double                 wetland_baseflow;
  double                 oldsnow;
  double                 snowprec;
  double                 rainprec;
  int                    current_month = dmy->month - 1;                         //0-11 LML 150505
  veg_var_struct        *wet_veg_var;
  veg_var_struct        *dry_veg_var;
  #ifdef VIC_CROPSYST_VERSION                                                    /*keyvan 11162012*/
  int                    iscrop_return  = 0;
  #endif
#if EXCESS_ICE
  int                    SubsidenceUpdate = 0;
  int                    index;
  char                   ErrStr[MAXSTRING];
  double                 max_ice_layer; //mm/mm
  double                 ave_ice_fract; //mm/mm
  double                 ave_ice, tmp_ice; //mm
  double                 ice_layer; //mm
  double                 subsidence[MAX_LAYERS]; //mm
  double                 total_subsidence; //m
  double                 tmp_subsidence; //mm
  double                 total_meltwater; //mm
  double                 tmp_depth, tmp_depth_prior; //m
  double                 ppt[2];
  double                 moist_prior[2][MAX_VEG][MAX_BANDS][MAX_LAYERS]; //mm
  double                 evap_prior[2][MAX_VEG][MAX_BANDS][MAX_LAYERS]; //mm

#endif //EXCESS_ICE

  /* Allocate aero_resist array */
  int num_veg_for_pet = N_PET_TYPES + 1;
  double **aero_resist = alloc_2d_array<double>(num_veg_for_pet,3,"aero_resist");
  //160519LML
  cell_data_struct ***cell     = prcp->cell;
  energy_bal_struct **energy   = prcp->energy;
  lake_var_struct    *lake_var = &prcp->lake_var;
  snow_data_struct  **snow     = prcp->snow;
  veg_var_struct   ***veg_var  = prcp->veg_var;
  int Ndist = options.NDIST;
  int Nbands = options.SNOW_BAND;
  int Nveg   = veg_con[0].vegetat_type_num;
  double dp  = soil_con->dp;
  // Compute gauge undercatch correction factors
  //   - this assumes that the gauge is free of vegetation effects, so gauge
  //   correction is constant for the entire grid cell
  double gauge_correction[2] = {1,1};
  if( options.CORRPREC && atmos->prec[NR] > 0 )
    correct_precip(atmos->wind[NR], global_param.wind_h, soil_con->rough, soil_con->snow_rough,gauge_correction);
  atmos->out_prec = atmos->out_rain = atmos->out_snow = 0;
  #if VIC_CROPSYST_VERSION==2
  //should be set to zero at the begining of a day
  if (crops) {
    //150713LML for (int i = 0; i < crops[0].no_of_crops_in_cell; i++) { //LML 150528 added loop.
      crops[0].cell_irrig_water                         = 0.0; //water balance terms keyvan oct2012
      crops[0].cell_avg_transp                          = 0.0; //water balance term keyvan oct2012
      crops[0].cell_evap_from_irrigation                = 0.0;  //water balance term keyvan oct2012
      crops[0].cell_evap_from_irrigation_intercepted    = 0.0;
      crops[0].cell_irrigation_runoff                   = 0.0;
    //}
  }
  #endif

  /* initialize prior moist and ice for subsidence calculations */
  #if EXCESS_ICE
  for(iveg = 0; iveg <= Nveg; iveg++){
    for ( band = 0; band < Nbands; band++ ) {
      for ( dist = 0; dist < Ndist; dist++ ) {
        for(lidx=0;lidx<options.Nlayer;lidx++) {
          moist_prior[dist][iveg][band][lidx] = cell[dist][iveg][band].layer[lidx].moist;
          evap_prior[dist][iveg][band][lidx]  = 0; //initialize
        }
      }
    }
  }
  #endif //EXCESS_ICE
  ///**************************************************
  //  Solve Energy and/or Water Balance for Each
  //  Vegetation Type
  //**************************************************
  #ifdef CHECK_WATER_BALANCE
  double init_total_water[2][MAX_BANDS];                                         //(mm) 150714LML
  #endif
  for (int iveg = 0; iveg <= Nveg; iveg++){
    //** Solve Veg Type only if Coverage Greater than 0% **
    if (veg_con[iveg].Cv > 0.0) {
      char overstory;
      double Cv = veg_con[iveg].Cv;
      //** Define vegetation class number **
      #if (VIC_CROPSYST_VERSION>=3)
      if (dmy->day_in_year == 1) veg_con[iveg].VCS.this_year_start_irrigated = false; //190918LML
      int veg_code_runtime          = veg_con[iveg].VCS.veg_class_code;              //LML 150325
      iscrop_return                 = iscrop(veg_code_runtime);
      int current_veg_type_code(-1);                                             //LML 150325 vegetation code in veg lib file
      int rotation_cycle_index(0);                                               //LML 150526
      if (iscrop_return) {
        int rotation_or_crop_veg_class_code;                                     //LML 150526
        DecomposeVegclassCode(veg_code_runtime,rotation_or_crop_veg_class_code,rotation_cycle_index); //LML 150526
        for (int dist = 0; dist < Ndist; dist++) {                               //150929LML
          for (int band = 0; band < Nbands; band++) {                            //150929LML
            crop_data_struct *current_crop = veg_var[dist][iveg][band].crop_state;
            if (rec == 0 && current_crop) {
              current_crop->CropSystHandle = VIC_land_unit_create
                (gridcell                                                        //150415RLN
                 ,rotation_or_crop_veg_class_code                                // actual crop or rotation code                    //150521RLN
                 ,rotation_cycle_index                                           //150415RLN
                 ,*current_crop
                 ,*soil_con
                 ,options
                 ,veg_con[iveg]
                 ,atmos
                 ,cell[dist][iveg][band]
                 ,snow[iveg][band]
                 ,veg_var[dist][iveg][band]
                 );
              VIC_land_unit_start();
              if (dmy->day_in_year != 1) VIC_land_unit_start_year();             //160504LML
            } else if (current_crop){
              VIC_land_unit_activate(current_crop->CropSystHandle);
            }
            if (dmy->day_in_year == 1 && (global_param.dt == 24 || dmy->hour == 0))
              VIC_land_unit_start_year();                                        //160504LML
            if (global_param.dt == 24 || dmy->hour == 0)
              //171201LML VIC_land_unit_start_day();                                         //150325LML
              VIC_land_unit_before_start_day();                                  //171201LML
            if (current_crop) current_crop->code = VIC_land_unit_get_current_crop_code();
            VIC_land_unit_deactivate();
          } //band
        } //dist
        current_veg_type_code = rotation_or_crop_veg_class_code;
      } else {
        current_veg_type_code = veg_code_runtime;
      }
      int veg_class = get_veg_lib_index(current_veg_type_code);                  //LML 141017 vegetation index in veg_lib
                                                                                 //LML 150414 I think so. Could be rotation type NIY 150522
                                                                                 //veg_con[iveg].veg_class; //LML 150413 get_veg_lib_index(current_veg_type_code);
                                                                                 //LML 141017 all functions use this veg_class as veg index in veg_lib
      int veg_class_code = veg_con[iveg].VCS.veg_class_code;                             //130226Keyvan
      #elif VIC_CROPSYST_VERSION==2
      iscrop_return = iscrop(veg_class_code);
      veg_class = veg_con[iveg].veg_class; /* this part looks different in VIC_4.0.7 */
      #else
      veg_class = veg_con[iveg].veg_class; /* this part looks different in VIC_4.0.7 */
      #endif
      /** Lake-specific processing **/
      if (veg_con[iveg].LAKE) {
        /* Update areai to equal new ice area from previous time step. */
        lake_var->areai = lake_var->new_ice_area;
        /* Compute lake fraction and ice-covered fraction */
        if (lake_var->areai < 0) lake_var->areai = 0;
        if (lake_var->sarea > 0) {
          fraci = lake_var->areai/lake_var->sarea;
          if(fraci > 1.0) fraci = 1.0;
        } else {
            fraci = 0.0;
        }
        lakefrac = lake_var->sarea/lake_con->basin[0];
        Nbands   = 1;
        Cv      *= (1. - lakefrac);
        if (Cv == 0)
          continue;
      }
      /**************************************************
        Initialize Model Parameters
      **************************************************/
      for (int band = 0; band < Nbands; band++) {
        if(soil_con->AreaFract[band] > 0) {
          /* Initialize energy balance variables */
          energy[iveg][band].shortwave = 0;
          energy[iveg][band].longwave  = 0.;
          /* Initialize snow variables */
          snow[iveg][band].vapor_flux        = 0.;
          snow[iveg][band].canopy_vapor_flux = 0.;
          snow_inflow[band]                  = 0.;
          Melt[band*2]                       = 0.;
        }
      }
      /* Initialize precipitation storage */
      for (int j = 0; j < 2*MAX_BANDS; j++ )
        out_prec[j] = out_rain[j] = out_snow[j] = 0;
      //* In most instances the wind speed reference height
      //is fixed and the wind_h from the vegetation library
      //should not be used //KJC
      // The exception is when running in point mode, in which
      //case this might be overridden*
      // if ( iveg < Nveg ) wind_h = veg_lib[veg_class].wind_h;
      // else wind_h = gp->wind_h; */
      // /** Assign wind_h **/
      // /** Note: this is ignored below **

      //wind_h = current_veglib.wind_h; //it's not the same as old coupling we may change it latter
       //#endif
      //** Compute Surface Attenuation due to Vegetation Coverage **
      /*151001LML moved to band loop
      if(iveg < Nveg){
        #if (VIC_CROPSYST_VERSION>=3)
        if(iscrop_return){
            set_average_veglib_for_crop(current_veglib,veg_var,prcp,soil_con,iveg,current_month);  //150929LML
        }
        #endif
        #if VIC_CROPSYST_VERSION==2
        if(iscrop_return == 1)surf_atten = exp(-veg_lib[CROP_VEG_CLASS-1].rad_atten
                                * veg_lib[CROP_VEG_CLASS-1].LAI[current_month]);
        else
        #endif
        surf_atten = exp(-current_veglib.rad_atten
                                * current_veglib.LAI[current_month]);
      } else {
        surf_atten = 1.;
      }
      */
      /* Initialize soil thermal properties for the top two layers */
      prepare_full_energy(iveg, Nveg, options.Nnode, prcp, soil_con, moist0, ice0);
      /*151001LML moved to band loop.
      //Compute Bare Soil (free of snow) Albedo
      if (iveg != Nveg) {
        bare_albedo = current_veglib.albedo[current_month];
        #if VIC_CROPSYST_VERSION==2
        if (iscrop_return == 1) bare_albedo = veg_lib[CROP_VEG_CLASS-1].albedo[current_month];
        //else bare_albedo = veg_lib[veg_class].albedo[current_month];
        #endif
      } else {
        bare_albedo = BARE_SOIL_ALBEDO;
      }
      */
      /*************************************
       Compute the aerodynamic resistance
       for current veg cover and various
       types of potential evap
      *************************************/
      /* Loop over types of potential evap, plus current veg */
      /* Current veg will be last */
      for (int p = 0; p < num_veg_for_pet; p++) {
        /* Initialize wind speeds */
        tmp_wind[0] = atmos->wind[NR];
        tmp_wind[1] = -999.;
        tmp_wind[2] = -999.;
        /* Set surface descriptive variables */
        if (p < N_PET_TYPES_NON_NAT) pet_veg_class = veg_lib[0].NVegLibTypes + p;
        else                         pet_veg_class = veg_class;
        displacement[0] = veg_lib[pet_veg_class].displacement[current_month];
        roughness[0]    = veg_lib[pet_veg_class].roughness[current_month];
        overstory       = veg_lib[pet_veg_class].overstory;
        if ((p >= N_PET_TYPES_NON_NAT) && is_approximately(roughness[0],0))
          roughness[0] = soil_con->rough;
        /* Estimate vegetation height */
        double height = calc_veg_height(displacement[0]);
        #if (VIC_CROPSYST_VERSION>=3)
        if (iscrop_return && p >= N_PET_TYPES_NON_NAT) {
          //171201LML Should calculate real ra for each band.
          crop_data_struct *current_crop = veg_var[WET][iveg][0].crop_state;
          VIC_land_unit_activate(current_crop->CropSystHandle);
          height = VIC_land_unit_get(VIC::CANOPY_HEIGHT);                        //LML 150414 in meter
          VIC_land_unit_deactivate();
        }
        #endif
        wind_h = veg_lib[pet_veg_class].wind_h;                                  //180402LML
        /* Estimate reference height */
        if(displacement[0] < wind_h) ref_height[0] = wind_h;
        else                         ref_height[0] = displacement[0] + wind_h + roughness[0];
        /* Compute aerodynamic resistance over various surface types */
        /* Do this not only for current veg but also all types of PET */
        ErrorFlag = CalcAerodynamic(overstory,
                                    height,
                                    veg_lib[pet_veg_class].trunk_ratio,
                                    soil_con->snow_rough,
                                    soil_con->rough,
                                    veg_lib[pet_veg_class].wind_atten,
                                    aero_resist[p],
                                    tmp_wind,
                                    displacement,
                                    ref_height,
                                    roughness
                                    );
        if ( ErrorFlag == ERROR ) return ( ERROR );
        #ifdef VIC_CROPSYST_VERSION
        for (int surface = 0; surface <= 1; surface++) {
            if (p < N_PET_TYPES) {
              if (dmy->hour == 0) cell[WET][iveg][0].VCS.aero_resist_daily[p][surface] = 0.0;
              cell[WET][iveg][0].VCS.aero_resist_daily[p][surface] += aero_resist[p][surface] * (global_param.dt / 24.0);
            }
        }
        #endif
      }  //end p

      /* Initialize final aerodynamic resistance values */
      for (int band = 0; band < Nbands; band++ ) {
        if( soil_con->AreaFract[band] > 0 )
          for (int cov = 0; cov < 2; cov++)
            cell[WET][iveg][band].aero_resist[cov] = aero_resist[N_PET_TYPES][cov];
      }
      //******************************
      //  Solve ground surface fluxes
      //******************************
      for (int band = 0; band < Nbands; band++ ) {
        //std::clog << "iveg:" << iveg << "\tband:" << band << std::endl;
        if( soil_con->AreaFract[band] > 0 ) {
          wet_veg_var = &(veg_var[WET][iveg][band]);
          dry_veg_var = &(veg_var[DRY][iveg][band]);
          lag_one     = veg_con[iveg].lag_one;
          sigma_slope = veg_con[iveg].sigma_slope;
          fetch       = veg_con[iveg].fetch;
          for (int dist = 0; dist < Ndist; dist++) {
            #ifdef CHECK_WATER_BALANCE
            if (dist == WET)
              init_total_water[WET][band] = get_total_soil_moisture(
                                                cell[WET][iveg][band].layer)
                                            + m_to_mm(snow[iveg][band].swq)
                                            + m_to_mm(snow[iveg][band].snow_canopy)
                                            + wet_veg_var->Wdew;
            else
              init_total_water[DRY][band] = get_total_soil_moisture(
                                                cell[DRY][iveg][band].layer)
                                            + dry_veg_var->Wdew;
            #endif
            /* Initialize pot_evap */
            for (int p = 0; p < N_PET_TYPES; p++)
              cell[WET][iveg][band].pot_evap[p] = 0;
          }
          //151001LML
          veg_lib_struct &current_veglib =
              #if (VIC_CROPSYST_VERSION>=3)
              iveg < Nveg ? *veg_con[iveg].VCS.veg_lib[band] : veg_lib[veg_lib->NVegLibTypes];
              #else
              iveg < Nveg ? veg_lib[veg_class] : veg_lib[veg_lib->NVegLibTypes];
              #endif
          if (iveg < Nveg) {
            #if (VIC_CROPSYST_VERSION>=3)
            if(iscrop_return)
                //const veg_var_struct ***veg_var_temp = const_cast<veg_var_struct ***>(veg_var);                  //160923LML
                set_average_veglib_for_crop(current_veglib,veg_var,prcp,soil_con,iveg,current_month);  //150929LML
            #endif
            #if VIC_CROPSYST_VERSION==2
            if(iscrop_return == 1)surf_atten = exp(-veg_lib[CROP_VEG_CLASS-1].rad_atten
                                    * veg_lib[CROP_VEG_CLASS-1].LAI[current_month]);
            else
            #endif
            surf_atten = exp(-current_veglib.rad_atten
                                    * current_veglib.LAI[current_month]);
          } else {
            surf_atten = 1.;
          }


          //std::clog << "surf_atten:"        << surf_atten
          //          << "\trad_atten:"       << current_veglib.rad_atten
          //          << "\tLAI:"             << current_veglib.LAI[current_month]
          //          << std::endl;

          //Compute Bare Soil (free of snow) Albedo
          if (iveg != Nveg) {
            bare_albedo = current_veglib.albedo[current_month];
            #if VIC_CROPSYST_VERSION==2
            if (iscrop_return == 1) bare_albedo = veg_lib[CROP_VEG_CLASS-1].albedo[current_month];
            //else bare_albedo = veg_lib[veg_class].albedo[current_month];
            #endif
          } else {
            bare_albedo = BARE_SOIL_ALBEDO;
          }
          //150930LML
          #if VIC_CROPSYST_VERSION>=3
          for (int dist = 0; dist < Ndist; dist ++) {
            crop_data_struct *crop = (dist == WET) ? wet_veg_var->crop_state
                                                   : dry_veg_var->crop_state;
            if (crop) {
              crop->canopy_evap                   = 0;
              crop->irrigation_evap               = 0;
              crop->irrigation_netdemand          = 0;
              crop->irrigation_water              = 0;
              //crop->net_irrigation                = 0;
              crop->irrigation_runoff             = 0;
              crop->intercepted_irrigation        = 0;
              crop->evap_intercepted_irrig_water  = 0;
              crop->canopy_interception_fraction  = 0;
              crop->potential_transpir            = 0;
            }
          }  //dist
          #endif
          //171127LML Note: check height and tmp_wind, why use the last PET
          //vegetation type?
          double height = 0;                                                     //160516LML need check!!!
          #ifndef FULL_IRRIGATION
          irrigation_pattern_struct *irripattern = 0;
          if (irrig_patt.find(veg_class_code) != irrig_patt.end() ) {
              if (irrig_patt[veg_class_code].find(rec) != irrig_patt[veg_class_code].end()) {
                  irripattern = &irrig_patt[veg_class_code][rec];
              }
          }
          #endif
          ErrorFlag = surface_fluxes(//iveg, band, prcp->mu[iveg], rec, dmy, surface_flux_input);
                                     overstory, bare_albedo, height, ice0[band], moist0[band],
                                     #if EXCESS_ICE
                                     SubsidenceUpdate, evap_prior[DRY][iveg][band],
                                     evap_prior[WET][iveg][band],
                                     #endif
                                     prcp->mu[iveg], surf_atten, &(Melt[band*2]), &Le,
                                     aero_resist,
                                     displacement, gauge_correction,
                                     &out_prec[band*2],
                                     &out_rain[band*2], &out_snow[band*2],
                                     ref_height, roughness,
                                     &snow_inflow[band],
                                     tmp_wind, veg_con[iveg].root, Nbands, Ndist,
                                     options.Nlayer, Nveg, band, dp, iveg, rec, veg_class,
                                     atmos, dmy, &(energy[iveg][band]), gp,
                                     &(cell[DRY][iveg][band]),
                                     &(cell[WET][iveg][band]),
                                     &(snow[iveg][band]),
                                     soil_con, dry_veg_var, wet_veg_var,
                                     lag_one, sigma_slope, fetch
#ifdef VIC_CROPSYST_VERSION
#if (VIC_CROPSYST_VERSION>=3)
                                    ,veg_con
#endif
#ifndef FULL_IRRIGATION
                                    ,irripattern
#endif
#endif
                            );
          if ( ErrorFlag == ERROR ) return ( ERROR );
          atmos->out_prec += out_prec[band*2] * Cv * soil_con->AreaFract[band];
          atmos->out_rain += out_rain[band*2] * Cv * soil_con->AreaFract[band];
          atmos->out_snow += out_snow[band*2] * Cv * soil_con->AreaFract[band];
          //********************************************************
          //  Compute soil wetness and root zone soil moisture
          //********************************************************
          // Loop through distributed precipitation fractions
          for (int dist = 0; dist < Ndist; dist++ ) {
            cell_data_struct &cur_cell = cell[dist][iveg][band];
            cur_cell.rootmoist = 0;
            cur_cell.wetness   = 0;
            for (int lidx=0; lidx < options.Nlayer; lidx++) {
              if (veg_con->root[lidx] > 0)
                cur_cell.rootmoist += cur_cell.layer[lidx].moist;
              #if EXCESS_ICE
              cur_cell.wetness += (cur_cell.layer[lidx].moist - soil_con->Wpwp[lidx])/(soil_con->effective_porosity[lidx]*soil_con->depth[lidx]*1000 - soil_con->Wpwp[lidx]);
              #else
              cur_cell.wetness += (cur_cell.layer[lidx].moist - soil_con->Wpwp[lidx])/(soil_con->porosity[lidx]*soil_con->depth[lidx]*1000 - soil_con->Wpwp[lidx]);
              #endif
            }
            cur_cell.wetness /= options.Nlayer;
            #if (defined(VIC_CROPSYST_VERSION) && defined (DEBUG_SPECIFIC_VEG_BAND))
            if (soil_con->AreaFract[band] > 1e-6) {
              print_state_flux_specific_veg_band(dist,
                                                 prcp->mu[iveg],
                                                 band,
                                                 #ifdef CHECK_WATER_BALANCE
                                                 init_total_water[dist][band],
                                                 #endif
                                                 current_veglib,
                                                 &cur_cell,
                                                 &snow[iveg][band],
                                                 &veg_var[dist][iveg][band],
                                                 &veg_con[iveg],
                                                 //150929LML current_crop,
                                                 soil_con,
                                                 atmos);                               //150710LML
            }
            #endif
          } //dist
        } //if /** End Loop Through Elevation Bands **
      } //band ** End Full Energy Balance Model **
      #if (VIC_CROPSYST_VERSION>=3)
      if (iscrop_return && (dmy->hour == (24 - global_param.dt))) {
        for (int dist = 0; dist < Ndist; dist++ ) {
          for (int band = 0; band < Nbands; band++) {
            crop_data_struct *current_crop = (dist == WET ? veg_var[WET][iveg][band].crop_state : veg_var[DRY][iveg][band].crop_state);
            double tmp_mu = 0;
            if (dist == WET) tmp_mu = prcp->mu[iveg];
            else             tmp_mu = 1. - prcp->mu[iveg];
            double area_fraction = Cv * soil_con->AreaFract[band] * tmp_mu;
            if (current_crop) {
              VIC_land_unit_activate(current_crop->CropSystHandle);

              VIC_land_unit_start_day();                                         //171201LML
              VIC_land_unit_process_day();                                       //LML 150325
              VIC_land_unit_end_day(area_fraction,dist,band);                                             //LML 150325
              VIC_land_unit_deactivate();                                          //150714LML
            }
          } //band
        } //dist
      }
      #endif
    } /** end current vegetation type **/
  } /** end of vegetation loop **/
  delete_2d_array<double>(aero_resist,num_veg_for_pet);
  /****************************
     Calculate Subsidence
  ****************************/
#if EXCESS_ICE
  total_subsidence = 0;
  total_meltwater = 0; //for lake model only
  for(lidx=0;lidx<options.Nlayer;lidx++) {//soil layer
    subsidence[lidx] = 0;
    if(soil_con->effective_porosity[lidx]>soil_con->porosity[lidx]){

      /* find average ice/porosity fraction and sub-grid with greatest ice/porosity fraction */
      ave_ice = 0;
      max_ice_layer = 0;
      for(iveg = 0; iveg <= Nveg; iveg++){ //iveg
        if (veg_con[iveg].Cv  > 0.) {
      Cv = veg_con[iveg].Cv;
          Nbands = options.SNOW_BAND;
          if (veg_con[iveg].LAKE) {
            Cv *= (1-lakefrac);
            Nbands = 1;
          }
          for(band = 0; band < Nbands; band++) {//band
        if(soil_con->AreaFract[band] > 0) {
          for ( dist = 0; dist < Ndist; dist++ ) {// wet/dry
        if(dist==0)
          tmp_mu = prcp->mu[iveg];
        else
          tmp_mu = 1. - prcp->mu[iveg];
#if SPATIAL_FROST
        tmp_ice = 0;
        for ( frost_area = 0; frost_area < FROST_SUBAREAS; frost_area++ ) {//frost area
          tmp_ice += (cell[dist][iveg][band].layer[lidx].ice[frost_area]
                  * soil_con->frost_fract[frost_area]);
          ice_layer = cell[dist][iveg][band].layer[lidx].ice[frost_area];
          if(ice_layer>=max_ice_layer)
            max_ice_layer = ice_layer;
        } // frost area
#else //SPATIAL_FROST
        tmp_ice = cell[dist][iveg][band].layer[lidx].ice;
        ice_layer = cell[dist][iveg][band].layer[lidx].ice;
        if(ice_layer>=max_ice_layer)
          max_ice_layer = ice_layer;
#endif //SPATIAL_FROST
        ave_ice += tmp_ice * Cv * tmp_mu * soil_con->AreaFract[band];
          }// wet/dry
        }
      }//band
    }
      } //iveg
      ave_ice_fract = ave_ice/soil_con->max_moist[lidx];

      /*check to see if threshold is exceeded by average ice/porosity fraction*/
      if(ave_ice_fract <= ICE_AT_SUBSIDENCE) {
    SubsidenceUpdate = 1;

    /*calculate subsidence based on maximum ice content in layer*/
    /*constrain subsidence by MAX_SUBSIDENCE*/
    tmp_depth_prior = soil_con->depth[lidx];//m
    tmp_subsidence = (1000.*tmp_depth_prior - max_ice_layer);//mm
    if(tmp_subsidence > MAX_SUBSIDENCE)
      tmp_subsidence = MAX_SUBSIDENCE;
    tmp_depth = tmp_depth_prior - tmp_subsidence/1000.;//m
    if(tmp_depth <= soil_con->min_depth[lidx])
      tmp_depth = soil_con->min_depth[lidx];
    soil_con->depth[lidx] = (float)(int)(tmp_depth * 1000 + 0.5) / 1000;//m
    subsidence[lidx] = (tmp_depth_prior - soil_con->depth[lidx])*1000.;//mm
    total_subsidence += (tmp_depth_prior - soil_con->depth[lidx]);//m

    if(subsidence[lidx] > 0 ){
#if VERBOSE
      fprintf(stderr,"Subsidence of %.3f m in layer %d:\n",subsidence[lidx]/1000.,lidx+1);
      fprintf(stderr,"\t\tOccurred for record=%d: year=%d, month=%d, day=%d, hour=%d\n",rec,dmy->year,dmy->month,dmy->day, dmy->hour);
      fprintf(stderr,"\t\tDepth of soil layer decreased from %.3f m to %.3f m.\n",tmp_depth_prior,soil_con->depth[lidx]);
#endif

      /*update soil_con properties*/
#if VERBOSE
      fprintf(stderr,"\t\tEffective porosity decreased from %.3f to %.3f.\n",soil_con->effective_porosity[lidx],1.0-(1.0-soil_con->effective_porosity[lidx])*tmp_depth_prior/soil_con->depth[lidx]);
#endif
      soil_con->effective_porosity[lidx]=1.0-(1.0-soil_con->effective_porosity[lidx])*tmp_depth_prior/soil_con->depth[lidx];
      if(tmp_depth <= soil_con->min_depth[lidx])
        soil_con->effective_porosity[lidx]=soil_con->porosity[lidx];
#if VERBOSE
      fprintf(stderr,"\t\tBulk density increased from %.2f kg/m^3 to %.2f kg/m^3.\n",soil_con->bulk_density[lidx],(1.0-soil_con->effective_porosity[lidx])*soil_con->soil_density[lidx]);
#endif
      soil_con->bulk_dens_min[lidx] *= (1.0-soil_con->effective_porosity[lidx])*soil_con->soil_density[lidx]/soil_con->bulk_density[lidx];
      if (soil_con->organic[layer] > 0)
        soil_con->bulk_dens_org[lidx] *= (1.0-soil_con->effective_porosity[lidx])*soil_con->soil_density[lidx]/soil_con->bulk_density[lidx];
      soil_con->bulk_density[lidx] = (1.0-soil_con->effective_porosity[lidx])*soil_con->soil_density[lidx]; //adjust bulk density
      total_meltwater += soil_con->max_moist[lidx] - soil_con->depth[lidx] * soil_con->effective_porosity[lidx] * 1000.; //for lake model (uses prior max_moist,
                                                                                                                         //so must come before new max_moist calculation
      soil_con->max_moist[lidx] = soil_con->depth[lidx] * soil_con->effective_porosity[lidx] * 1000.;

    }//subsidence occurs
      }//threshold exceeded

    }//excess ice exists
  } //loop for each soil layer
  if(total_subsidence>0){

    /********update remaining soil_con properties**********/
#if VERBOSE
    fprintf(stderr,"Damping depth decreased from %.3f m to %.3f m.\n",soil_con->dp,soil_con->dp-total_subsidence);
#endif
    soil_con->dp -= total_subsidence;  //adjust damping depth

#if VERBOSE
    fprintf(stderr,"More updated parameters in soil_con: max_infil, Wcr, and Wpwp\n");
#endif
    /* update Maximum Infiltration for Upper Layers */
    if(options.Nlayer==2)
      soil_con->max_infil = (1.0+soil_con->b_infilt)*soil_con->max_moist[0];
    else
      soil_con->max_infil = (1.0+soil_con->b_infilt)*(soil_con->max_moist[0]+soil_con->max_moist[1]);

    /* Soil Layer Critical and Wilting Point Moisture Contents */
    for(lidx=0;lidx<options.Nlayer;lidx++) {//soil layer
      soil_con->Wcr[lidx]  = soil_con->Wcr_FRACT[lidx] * soil_con->max_moist[lidx];
      soil_con->Wpwp[lidx] = soil_con->Wpwp_FRACT[lidx] * soil_con->max_moist[lidx];
      if(soil_con->Wpwp[lidx] > soil_con->Wcr[lidx]) {
    sprintf(ErrStr,"Updated wilting point moisture (%f mm) is greater than updated critical point moisture (%f mm) for layer %d.\n\tIn the soil parameter file, Wpwp_FRACT MUST be <= Wcr_FRACT.\n",
        soil_con->Wpwp[lidx], soil_con->Wcr[lidx], lidx);
    nrerror(ErrStr);
      }
      if(soil_con->Wpwp[lidx] < soil_con->resid_moist[lidx] * soil_con->depth[lidx] * 1000.) {
    sprintf(ErrStr,"Updated wilting point moisture (%f mm) is less than updated residual moisture (%f mm) for layer %d.\n\tIn the soil parameter file, Wpwp_FRACT MUST be >= resid_moist / (1.0 - bulk_density/soil_density).\n",
        soil_con->Wpwp[lidx], soil_con->resid_moist[lidx] * soil_con->depth[lidx] * 1000., lidx);
    nrerror(ErrStr);
      }
    }

    /* If BASEFLOW = NIJSSEN2001 then convert ARNO baseflow
       parameters d1, d2, d3, and d4 to Ds, Dsmax, Ws, and c */
    if(options.BASEFLOW == NIJSSEN2001) {
      lidx = options.Nlayer-1;
      soil_con->Dsmax = soil_con->Dsmax_orig *
    pow((double)(1./(soil_con->max_moist[lidx]-soil_con->Ws_orig)), -soil_con->c) +
    soil_con->Ds_orig * soil_con->max_moist[lidx];
      soil_con->Ds = soil_con->Ds_orig * soil_con->Ws_orig / soil_con->Dsmax_orig;
      soil_con->Ws = soil_con->Ws_orig/soil_con->max_moist[lidx];
#if VERBOSE
      fprintf(stderr,"More updated parameters in soil_con: Dsmax, Ds, Ws\n");
#endif
    }

    /*********** update root fractions ***************/
    fprintf(stderr,"Updated parameter in veg_con: root\n");
    calc_root_fractions(*soil_con,veg_con);

    /**********redistribute soil moisture (call runoff function)*************/
    /* If subsidence occurs, recalculate runoff, baseflow, and soil moisture
       using soil moisture values from previous time-step; i.e.
       as if prior runoff call did not occur.*/
    for(iveg = 0; iveg <= Nveg; iveg++){
      if (veg_con[iveg].Cv  > 0.) {
        Nbands = options.SNOW_BAND;
        if (veg_con[iveg].LAKE) {
          Nbands = 1;
        }
        for ( band = 0; band < Nbands; band++ ) {
      for ( dist = 0; dist < Ndist; dist++ ) {
        for(lidx=0;lidx<options.Nlayer;lidx++) {
          cell[dist][iveg][band].layer[lidx].moist = moist_prior[dist][iveg][band][lidx];
          cell[dist][iveg][band].layer[lidx].evap = evap_prior[dist][iveg][band][lidx];
        }
      }
        }
      }
    }
    for(iveg = 0; iveg <= Nveg; iveg++){
      if (veg_con[iveg].Cv  > 0.) {
        Nbands = options.SNOW_BAND;
        if (veg_con[iveg].LAKE) {
          Nbands = 1;
        }
    for ( band = 0; band < Nbands; band++ ) {
      if( soil_con->AreaFract[band] > 0 ) {

        //set inflow for runoff call
        ppt[WET]=cell[WET][iveg][band].inflow;
        ppt[DRY]=cell[DRY][iveg][band].inflow;

        ErrorFlag = runoff(&(cell[WET][iveg][band]), &(cell[DRY][iveg][band]), &(energy[iveg][band]),
                   soil_con, ppt,
                   SubsidenceUpdate,
#if SPATIAL_FROST
                   soil_con->frost_fract,
#endif // SPATIAL_FROST
                   prcp->mu[iveg], gp->dt, options.Nnode, band, rec, iveg);
        if ( ErrorFlag == ERROR ) return ( ERROR );

      }
    }//band
      }
    }//veg

    /**********interpolate nodal temperatures to new depths and recalculate thermal properties***********/
    ErrorFlag = update_thermal_nodes(prcp, Nveg, options.Nnode, Ndist, soil_con, veg_con);
    if ( ErrorFlag == ERROR ) return ( ERROR );

  }//subsidence occurs

  /********************************************
    Save subsidence for output
  ********************************************/
  for(lidx=0;lidx<options.Nlayer;lidx++)
    soil_con->subsidence[lidx] = subsidence[lidx];

#endif //EXCESS_ICE
  /****************************
     Run Lake Model
  ****************************/
  /** Compute total runoff and baseflow for all vegetation types
      within each snowband. **/
  if ( options.LAKES && lake_con->lake_idx >= 0 ) {
    wetland_runoff = wetland_baseflow = 0;
    sum_runoff = sum_baseflow = 0;
    // Loop through all vegetation tiles
    for (int iveg = 0; iveg <= Nveg; iveg++) {
      /** Solve Veg Tile only if Coverage Greater than 0% **/
      if (veg_con[iveg].Cv  > 0.) {
        double Cv = veg_con[iveg].Cv;
        Nbands = options.SNOW_BAND;
        if (veg_con[iveg].LAKE) {
          Cv *= (1. - lakefrac);
          Nbands = 1;
        }
        // Loop through snow elevation bands
        for (int band = 0; band < Nbands; band++ ) {
          double area_fract = soil_con->AreaFract[band];
          if ( area_fract > 0 ) {
            // Loop through distributed precipitation fractions
            for (int dist = 0; dist < 2; dist++ ) {
              cell_data_struct &cur_cell = cell[dist][iveg][band];
              if ( dist == 0 ) tmp_mu = prcp->mu[iveg];
              else             tmp_mu = 1. - prcp->mu[iveg];
              double area = tmp_mu * Cv * area_fract;
              if (veg_con[iveg].LAKE) {
                wetland_runoff   += ( cur_cell.runoff   * area);
                wetland_baseflow += ( cur_cell.baseflow * area);
                cur_cell.runoff   = 0;
                cur_cell.baseflow = 0;
              } else {
                sum_runoff   += ( cur_cell.runoff   * area);
                sum_baseflow += ( cur_cell.baseflow * area);
                cur_cell.runoff   *= (1. - lake_con->rpercent);
                cur_cell.baseflow *= (1. - lake_con->rpercent);
              }
            } //dist
          }
        } //band
      }
    } //iveg
    /** Run lake model **/
    int iveg = lake_con->lake_idx;
    int band = 0;
    lake_var->runoff_in   = (sum_runoff   * lake_con->rpercent + wetland_runoff)   * soil_con->cell_area * 0.001; // m3
    lake_var->baseflow_in = (sum_baseflow * lake_con->rpercent + wetland_baseflow) * soil_con->cell_area * 0.001; // m3
    lake_var->channel_in  = atmos->channel_in[NR] * soil_con->cell_area * 0.001; // m3
    lake_var->prec        = atmos->prec[NR] * lake_var->sarea * 0.001; // m3
    rainonly = calc_rainonly(atmos->air_temp[NR], atmos->prec[NR],
                             global_param.MAX_SNOW_TEMP, global_param.MIN_RAIN_TEMP, 1);
    if ( (int)rainonly == ERROR ) {
      return( ERROR );
    }
    /**********************************************************************
       Solve the energy budget for the lake.
     **********************************************************************/
    oldsnow  = lake_var->snow.swq;
    snowprec = gauge_correction[SNOW] * (atmos->prec[NR] - rainonly);
    rainprec = gauge_correction[SNOW] * rainonly;
    atmos->out_prec += (snowprec + rainprec) * lake_con->Cl[0] * lakefrac;
    atmos->out_rain += rainprec * lake_con->Cl[0] * lakefrac;
    atmos->out_snow += snowprec * lake_con->Cl[0] * lakefrac;

    ErrorFlag = solve_lake(snowprec, rainprec, atmos->air_temp[NR],
                           atmos->wind[NR], atmos->vp[NR] / 1000.,
                           atmos->shortwave[NR], atmos->longwave[NR],
                           atmos->vpd[NR] / 1000.,
                           atmos->pressure[NR] / 1000.,
                           atmos->density[NR], lake_var, *lake_con,
                           *soil_con, global_param.dt, rec, global_param.wind_h, *dmy/*150609 [rec]*/, fraci);
    if ( ErrorFlag == ERROR ) return (ERROR);
    /**********************************************************************
       Solve the water budget for the lake.
     **********************************************************************/
    ErrorFlag = water_balance(*lake_con, global_param.dt, rec, iveg, band,
                              lakefrac, *soil_con,
#if EXCESS_ICE
                              SubsidenceUpdate, total_meltwater,
#endif
                              prcp, *veg_con, *lake_var);
    if ( ErrorFlag == ERROR ) return (ERROR);
  } // end if (options.LAKES && lake_con->lake_idx >= 0)
  return (0);
}

/*170413LML moved to VCS_Nl.c
//*150710LML                                                                   *
#if (defined(VIC_CROPSYST_VERSION) && defined (DEBUG_SPECIFIC_VEG_BAND))
void print_state_flux_specific_veg_band(int dist,
                                        double mu,
                                        int band,
                                        #ifdef CHECK_WATER_BALANCE
                                        double init_total_water_mm,
                                        #endif
                                        const veg_lib_struct   &veglib_rec,
                                        const cell_data_struct *cell,
                                        const snow_data_struct *snow,
                                        const veg_var_struct *veg_var,
                                        const veg_con_struct *veg_con,
                                        //150929LML const crop_data_struct *crop,
                                        const soil_con_struct *soil,
                                        const atmos_data_struct *atmos)
{
    int month_index = (int)global_today.get_month() - 1;
    std::ofstream debugout;
    static bool newed(false);
    std::string filename = "c:\\temp\\vic_veg_band.csv";
    if (!newed) {
        debugout.open(filename.c_str(),std::ofstream::out);
        debugout <<"Year,"
                 <<"Month,"
                 <<"Day,"
                 <<"DOY,"
                 <<"rec,"
                 <<"CELL-ID,"
                 <<"Veg_Code,"
                 <<"dist,"
                 <<"band,"
                 <<"Active_crop,"
                 <<"Accum_DD,"
                 <<"Grow_Stage,"
                 <<"LAI,"
                 <<"GAI,"
                 <<"Total_Canopy_Cover,"
                 <<"Biomass_kg_m2,"
                 <<"Yield_kg_m2,"
                 <<"Root_depth_mm,"
                 <<"PET_SHORT_mm,"
                 <<"Transp_VIC_mm,"
                 <<"Transp_CS_mm,"
                 <<"irrig_total_mm,"
                 <<"irrig_evap_mm,"
                 <<"irrig_runoff_mm,"
                 <<"irrig_intcpt_mm,"
                 <<"irrig_intcpt_evap_mm,"
                 <<"Soil_E_mm,"
                 <<"Canopy_E_mm,"
                 <<"Snow_sub_mm,"
                 <<"Canopy_sub_mm,"
                 <<"Runoff,"
                 <<"Baseflow,"
                 <<"PPT,"
                 <<"SWE_mm,"
                 <<"Profile_moisture_mm,"
                 <<"Profile_thickness_mm,"
                 <<"Profile_VWC,"
                 #ifdef CHECK_WATER_BALANCE
                 <<"init_total_water_mm,"
                 <<"final_total_water,"
                 <<"inflow,"
                 <<"outflow,"
                 <<"balance,"
                 #endif
                 ;
        #ifdef PRINT_VIC_SOIL_LAYERS_FOR_DEBUG
        for (int i = 0; i < options.Nlayer; i++) debugout<<"VWC["<<i<<"],";
        for (int i = 0; i < options.Nlayer; i++) debugout<<"Moist_mm["<<i<<"],";
        for (int i = 0; i < options.Nlayer; i++) debugout<<"Depth_mm["<<i<<"],";
        for (int i = 0; i < options.Nlayer; i++) debugout<<"FC_VWC["<<i<<"],";
        for (int i = 0; i < options.Nlayer; i++) debugout<<"SAT_VWC["<<i<<"],";
        for (int i = 0; i < options.Nlayer; i++) debugout<<"T["<<i<<"],";
        #endif
        debugout<<std::endl;
        debugout.close();
        newed = true;
    }
    debugout.open(filename.c_str(),std::ofstream::out | std::ofstream::app);
    crop_data_struct *crop = veg_var->crop_state;
    if (crop)
      VIC_land_unit_activate(crop->CropSystHandle);

    //if (veg_con->veg_class_code == 300200) {
      CropSyst::Crop_interfaced *CropSyst_crop                = VIC::active_land_unit ? VIC::active_land_unit->crop_active_or_intercrop : 0;
      //150722 Organic_matter_residues_profile_abstract *CropSyst_soil = VIC::active_land_unit ? VIC::active_land_unit->organic_matter_residues_profile_kg_m2 : 0;
      bool active_crop = CropSyst_crop ? true : false;
      double total_transpiration = 0.0;
      double total_transpiration_daily_from_CropSyst = 0.0;
      double profile_moisture   = get_total_soil_moisture(*cell);
      double profile_thickness  = get_total_soil_thickness_mm(*soil);
      double profile_vwc        = profile_moisture / profile_thickness;
      for (int i = 0; i < options.Nlayer; i++) {
          total_transpiration                     += cell->layer[i].evap;
          total_transpiration_daily_from_CropSyst += CropSyst_crop ? VIC::active_land_unit->ref_VIC_cell().layer[i].transpiration_daily : 0.0;
      }
      #ifdef CHECK_WATER_BALANCE
      double final_total_water = profile_moisture
                                 + m_to_mm(snow->swq)
                                 + m_to_mm(snow->snow_canopy)
                                 + veg_var->Wdew;
      double inflow            = (mu > 1e-6 ? (atmos->prec[options.NR] / mu * soil->Pfactor[band]) : 0.0)
                                 + (crop ? crop->irrigation_water : 0);
      double outflow           = cell->runoff
                                 + cell->baseflow
                                 + cell->layer[0].evap_bare_soil
                                 + veg_var->canopyevap
                                 + total_transpiration
                                 + (crop ? crop->irrigation_evap                : 0)
                                 + (crop ? crop->irrigation_runoff              : 0)
                                 + (crop ? crop->evap_intercepted_irrig_water   : 0)
                                 + m_to_mm(snow->vapor_flux + snow->canopy_vapor_flux);
      double balance           = (final_total_water - init_total_water_mm)
                                 - (inflow - outflow);
      #endif
      debugout<<(int)global_today.get_year()
              <<","<<(int)global_today.get_month()
              <<","<<(int)global_today.get_DOM()
              <<","<<(int)global_today.get_DOY()
              <<","<<global_rec
              <<","<<(int)soil->gridcel
              <<","<<veg_con->veg_class_code
              <<","<<dist
              <<","<<band
              <<","<<active_crop
              <<","<<(active_crop ? CropSyst_crop->get_accum_degree_days(false)         : 0)
              <<","<<(active_crop ? CropSyst_crop->describe_growth_stage()              : "N/A")
              <<","<<(active_crop ? CropSyst_crop->get_LAI(true)                        : 0)
              <<","<<(active_crop ? CropSyst_crop->get_GAI(true)                        : 0)
              <<","<<(active_crop ? CropSyst_crop->get_canopy_interception_global_total()             : 0)
              <<","<<(active_crop ? CropSyst_crop->get_canopy_biomass_kg_m2()           : 0)
              <<","<<(active_crop ? CropSyst_crop->get_latest_yield_kg_m2()             : 0)
              <<","<<(active_crop ? m_to_mm(CropSyst_crop->get_recorded_root_depth_m()) : 0)
              <<","<<cell->pot_evap[PET_SHORT]//(active_crop ? m_to_mm(CropSyst_crop->get_pot_transpiration_m())   : 0)
              <<","<<total_transpiration
              <<","<<(active_crop ? m_to_mm(CropSyst_crop->get_act_transpiration_m())   : 0)
              <<","<<(crop ? crop->irrigation_water               : 0)
              <<","<<(crop ? crop->irrigation_evap                : 0)
              <<","<<(crop ? crop->irrigation_runoff              : 0)
              <<","<<(crop ? crop->intercepted_irrigation         : 0)
              <<","<<(crop ? crop->evap_intercepted_irrig_water   : 0)
              <<","<<cell->layer[0].evap_bare_soil
              <<","<<veg_var->canopyevap
              <<","<<m_to_mm(snow->vapor_flux)
              <<","<<m_to_mm(snow->canopy_vapor_flux)
              <<","<<cell->runoff
              <<","<<cell->baseflow
              <<","<<(mu > 1e-6 ? (atmos->prec[options.NR] / mu * soil->Pfactor[band]) : 0.0) //150930LML atmos->prec[options.NR]
              <<","<<m_to_mm(snow->swq + snow->snow_canopy)
              <<","<<profile_moisture
              <<","<<profile_thickness
              <<","<<profile_vwc
              #ifdef CHECK_WATER_BALANCE
              <<","<<init_total_water_mm
              <<","<<final_total_water
              <<","<<inflow
              <<","<<outflow
              <<","<<balance
              #endif
              ;
        #ifdef PRINT_VIC_SOIL_LAYERS_FOR_DEBUG
        for (int i = 0; i < options.Nlayer; i++)
            debugout<<","<<cell->layer[i].moist / m_to_mm(soil->depth[i]);
        for (int i = 0; i < options.Nlayer; i++)
            debugout<<","<<cell->layer[i].moist;
        for (int i = 0; i < options.Nlayer; i++)
            debugout<<","<<m_to_mm(soil->depth[i]);
        for (int i = 0; i < options.Nlayer; i++)
            debugout<<","<<soil->Wcr[i] / m_to_mm(soil->depth[i]);
        for (int i = 0; i < options.Nlayer; i++)
            debugout<<","<<soil->max_moist[i] / m_to_mm(soil->depth[i]);
        for (int i = 0; i < options.Nlayer; i++)
            debugout<<","<<cell->layer[i].T;
        #endif
        debugout<<std::endl;
    //}
    debugout.close();
}
#endif //(defined(VIC_CROPSYST_VERSION) && defined (DEBUG_SPECIFIC_VEG_BAND))
*/
/*170413LML moved to VCS_Nl.c
//150929LML_____________________________________________________________________
int set_average_veglib_for_crop(veg_lib_struct &veglib,
                                const veg_var_struct * const * const *veg_var,
                                const dist_prcp_struct *prcp,
                                const soil_con_struct *soil_con,
                                const int veg_index,
                                const int current_month)
{
    //set the average over dist and snow bands
    int Ndist = options.NDIST;
    veglib.LAI[current_month]            = 0;
    veglib.albedo[current_month]         = 0;
    veglib.displacement[current_month]   = 0;
    veglib.roughness[current_month]      = 0;
    veglib.Wdmax[current_month]          = 0;
    veglib.emissivity[current_month]     = 0;
    veglib.rad_atten                     = 0;
    veglib.rmin                          = 0;
    veglib.rarc                          = 0;
    veglib.trunk_ratio                   = 0;
    veglib.wind_atten                    = 0;
    veglib.wind_h                        = 0;
    veglib.RGL                           = 0;
    for (int dist = 0; dist < Ndist; dist++ ) {                                  //150929LML
      for (int band = 0; band < options.SNOW_BAND; band++) {                                //150929LML
        crop_data_struct *current_crop = veg_var[dist][veg_index][band].crop_state;
        if (current_crop) {
          VIC_land_unit_activate(current_crop->CropSystHandle);                  //151001LML time consuming!!!
          double mu = (dist == WET ? prcp->mu[veg_index] : (1.0 - prcp->mu[veg_index]));
          double snow_zone_frac = soil_con->AreaFract[band];
          veglib.LAI[current_month]            += mu * snow_zone_frac * VIC_land_unit_get(VIC::LAI);
          veglib.albedo[current_month]         += mu * snow_zone_frac * VIC_land_unit_get(VIC::CANOPY_ALBEDO);
          veglib.displacement[current_month]   += mu * snow_zone_frac * VIC_land_unit_get(VIC::DSPLACEMENT);
          veglib.roughness[current_month]      += mu * snow_zone_frac * VIC_land_unit_get(VIC::ROUGHNESS);
          veglib.Wdmax[current_month]          += mu * snow_zone_frac * LAI_WATER_FACTOR * veglib.LAI[current_month];
          veglib.emissivity[current_month]     += mu * snow_zone_frac * VIC_land_unit_get(VIC::EMISSIVITY); //LML 150505 seemd never used
          veglib.rad_atten                     += mu * snow_zone_frac * VIC_land_unit_get(VIC::RAD_ATTEN);
          veglib.rmin                          += mu * snow_zone_frac * VIC_land_unit_get(VIC::MIN_STOMATA_CONDUCTANCE);
          veglib.rarc                          += mu * snow_zone_frac * VIC_land_unit_get(VIC::RA_ARCHITECTURAL);
          veglib.trunk_ratio                   += mu * snow_zone_frac * VIC_land_unit_get(VIC::TRUNK_RATIO);
          veglib.wind_atten                    += mu * snow_zone_frac * VIC_land_unit_get(VIC::WIND_ATTEN);
          veglib.wind_h                        += mu * snow_zone_frac * VIC_land_unit_get(VIC::WIND_H);
          veglib.RGL                           += mu * snow_zone_frac * VIC_land_unit_get(VIC::RGL);
        }
      } //band
    } //dist
}
*/
