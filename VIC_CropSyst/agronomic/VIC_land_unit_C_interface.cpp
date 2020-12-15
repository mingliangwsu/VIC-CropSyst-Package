/* RLN This may be needed for crop_data_struct
#include "VCS_Nl_def.h"
*/
#include "VCS_user_def.h"
#if (VIC_CROPSYST_VERSION>=3)
#ifdef OUTPUT_FULLIRRIGATION_PATTERN
#include "VIC_crop_output.h"
#endif
#include "VIC_land_unit_C_interface.h"
#include "global.h"
#include "irrigation_lib.h"
#include <stdlib.h>                                                              //LML 150527
#ifdef VCS_V5
#include "vic_driver_shared_all.h"
#include "VCS_Nl_v5.h"
#include "VCS_global.h"
#else
#include "vicNl.h"                                                               //150723LML
#include "global.h"
#endif
#include <iomanip>                                                               //160524LML

#include "crop/phenology_I.h"

#include "soil/chemicals_profile.h"                                              //150722LML
#ifdef PRINT_VIC_SOIL_LAYERS_FOR_DEBUG

#include "organic_matter/OM_residues_profile_abstract.h"                         //150727LML
#endif
/**
   \author Roger Nelson   Biological System Engineering
   Washington State University
//#ifdef CROPSYST_ON
   \email rnelson@wsu.edu
   \date   2010-03-10
This code is a hybrid of C and C++ intended to provide a C interface
to the C++ CropSyst model.
This follows C style coding conventions
This is intended for a C++ compiler that supports mixed mode C/C++.
*/
#include "common/simulation/log.h"
#include "agronomic/VIC_land_unit_simulation.h"
#include "corn/OS/file_system_engine.h"
#include "corn/OS/directory_entry_name_concrete.h"
#include "corn/data_source/vv_file.h"
#if defined(CROP_DAILY_OUTPUT_MEMFIRST) || defined (OUTPUT_FULLIRRIGATION_PATTERN)
#include <list>
#include "VIC_crop_output.h"
#endif
#include "crop/crop_interfaced.h"


double VIC_land_unit_get_return=0.0;

extern CORN::date32 global_simdate_raw; //LML 200819

/*____________________________________________________________________________*/
namespace VIC
{
   // Global variables
   VIC::Land_unit_simulation *active_land_unit = 0;
   //Within each grid cell, each cropping system keep its own land unit through
   //"CropSystHandle" in crops
   CORN::OS::Directory_name *project_directory = 0;
   CORN::Enumeration_list rotations_parameters;
      // List of rotation parameters which have already been loaded in memory
      // to avoid reopenning rotation parameter files multiple times.
}
using namespace VIC;
/*____________________________________________________________________________*/
void VIC_land_unit_know_psychrometrics
(double vapor_pressure_saturation_slope_kPa_C
,double psychrometric_constant_kPa_C
,double aerodynamic_resistance_ref_plants)                 
{  if (active_land_unit)
      active_land_unit->know_pychrometrics
         (vapor_pressure_saturation_slope_kPa_C
         ,psychrometric_constant_kPa_C
         ,aerodynamic_resistance_ref_plants);
}
/*_2014-12-08_______________________________________________________________*/
void VIC_land_unit_know_transpiration_max_mm(double crop_transpiration_max_mm)
{  if (active_land_unit)
       active_land_unit->know_transpiration_max_mm(crop_transpiration_max_mm);
} 
/*_VIC_land_unit_know_transpiration_max_mm_________________________2014-12-08_*/
unsigned long int VIC_land_unit_start()
{  return (active_land_unit)
   ? (unsigned long int)active_land_unit->start()
   : 0;
} 
/*______________________________________________________________2016-05-04LML_*/
unsigned long int VIC_land_unit_start_year()
{  return (active_land_unit)
   ? (unsigned long int)active_land_unit->start_year()
   : 0;
}
/*_VIC_land_unit_start_year________________________________________2014-12-08_*/
int VIC_land_unit_start_day()
{
   return (active_land_unit) ? active_land_unit->start_day() :0;
}
//_VIC_land_unit_start_day______________________________________2017-12-01LML_*/
int VIC_land_unit_before_start_day()
{
   return (active_land_unit) ? active_land_unit->initialize_at_start_of_day():0;
}
/*_VIC_land_unit_before_start_day_______________________________2015-07-18LML_*/
nat16 VIC_land_unit_get_irrigation_type()
{
   nat16 irritation_type = NO_IRRIGATION;
   if (active_land_unit) {
       if (active_land_unit->get_irrigation_operation_for_VIC_next_day())
           irritation_type = active_land_unit->get_irrigation_operation_for_VIC_next_day()->irrigation_type;
       else if (active_land_unit->curr_automatic_irrigation_mode)
           irritation_type = active_land_unit->curr_automatic_irrigation_mode->irrigation_type;
   }
   return irritation_type;
}
/*____________________________________________________VIC_land_unit_start_day_*/
int VIC_land_unit_end_day(const double cell_fraction,int dist, int band)
{
    //bool end_day = (active_land_unit) ? active_land_unit->end_day() :0;
#ifndef NOT_PRINT_CROP_DAILY_OUTPUTS
    VIC_land_unit_print_end_day_outputs(0,cell_fraction,dist,band);                            //150722LML
#endif
    bool end_day = (active_land_unit) ? active_land_unit->end_day() :0;
    return end_day;
}
/*_VIC_land_unit_start_day____________________________________________________*/
int VIC_land_unit_print_end_day_outputs(int growth_season_only,
                                        const double cell_fraction,
                                        int dist, int band)
{
    extern bool created_head;

    bool print_status = true;
    bool doprint      = false;
    int lys = options.Nlayer;
//#ifndef CROP_DAILY_OUTPUT_MEMFIRST
//    extern std::ofstream debugout;
//#endif
    //180404LML static bool newed(false);
    int runtime_veg_code =
        active_land_unit->ref_VIC_veg_con().VCS.veg_class_code;
    int rotation_code(0);
    int rotation_cycle_index(0);
    DecomposeVegclassCode(runtime_veg_code,rotation_code,rotation_cycle_index);
#ifndef CROP_DAILY_OUTPUT_MEMFIRST
    char out_file_name[300];
    #ifdef __linux__
    sprintf(out_file_name,"%s/%s_%.5f_%.5f.asc",filenames.result_dir,
            filenames.VCS.crop_output_file_name,
            active_land_unit->ref_VIC_soil_con().lat,
            active_land_unit->ref_VIC_soil_con().lng);
    #else
    sprintf(out_file_name,"%s\\%s_%.5f_%.5f.asc",filenames.result_dir,
            filenames.VCS.crop_output_file_name,
            active_land_unit->ref_VIC_soil_con().lat,
            active_land_unit->ref_VIC_soil_con().lng);
    #endif
#endif
    //if (!newed) {
    if(global_rec == 0 && !created_head) {                                                        //180404LML
#ifndef CROP_DAILY_OUTPUT_MEMFIRST
        created_head = true;
        debugout.open(out_file_name,std::ofstream::out);
        debugout <<"cell_id,"
                 <<"lon,"
                 <<"lat,"
                 <<"Year,"
                 <<"Month,"
                 <<"Day,"
                 <<"DOY,"
#ifndef OUTPUT_SIMPLE_FOR_FORECAST
                 <<"Dist,"
                 <<"Band,"
#endif
                 <<"Cell_fract,"
                 <<"CroppingSyst_code,"
                 <<"Crop_code,"
                 <<"Crop_name,"
#ifndef OUTPUT_SIMPLE_FOR_FORECAST
                 <<"Accum_DD,"
                 <<"Accum_DD_clip,"
                 <<"Grow_Stage,"
                 //<<"VIC_LAI,"
                 //<<"LAI,"
                 <<"GAI,"
#endif
                 <<"Green_Canopy_Cover,"
                 <<"Biomass_kg_m2,"
                 <<"Yield_kg_m2,"
#ifndef OUTPUT_SIMPLE_FOR_FORECAST
                 <<"Root_depth_mm,"
#ifdef VCS_V5
                 <<"VIC_PET_mm,"
#else
                 <<"VIC_PET_shortgrass_mm,"
#endif
                 //<<"CropSyst_Pot_Transp_mm,"
#endif
                 <<"CropSyst_refET_mm,"
#ifndef OUTPUT_SIMPLE_FOR_FORECAST
                 <<"Act_Transp_mm,"                                              //180523LML include canopy evap ifdef CROPSYST_HANDLE_CANOPY_EVAP
#endif
                 <<"VIC_final_transp_mm,"
#ifndef OUTPUT_SIMPLE_FOR_FORECAST
                 <<"irrig_netdemand_mm,"
#endif
                 <<"irrig_total_mm,"
#ifndef OUTPUT_SIMPLE_FOR_FORECAST
                 <<"irrig_evap_mm,"
                 <<"irrig_runoff_mm,"
                 <<"irrig_intcpt_mm,"                                            //intercepted by canopy (before & after canopy evap processes)
                 //<<"irrig_intcpt_evap_mm,"                                       //to meet evap demand
#endif
                 <<"Soil_E_mm,"
#ifndef OUTPUT_SIMPLE_FOR_FORECAST
                 <<"Crop_Canopy_E_mm,"
                 <<"snow_sublimation,"
                 <<"ET_mm,"
#endif
                 <<"VIC_ET_mm,"
                 <<"water_stress_index,"
                 <<"Runoff,"
                 <<"Baseflow,"
                 <<"PPT,"
                 <<"Tair_avg,"
#ifndef OUTPUT_SIMPLE_FOR_FORECAST
                 <<"Tmax,"
                 <<"Tmin,"
                 <<"SWRAD_w_m2,"
                 <<"VP_kPa,"
                 <<"SPH_kg_kg,"
                 <<"RHUM_avg_%,"
                 <<"RHUM_max_%,"
                 <<"RHUM_min_%,"
                 <<"Snow_dep_mm,"
                 <<"Crop_Biomass_kg_m2,"
#endif
#ifdef PRINT_SOM_FOR_DEBUG
                 <<"Surface_Residue_C_kg_m2,"
                 <<"Soil_Residue_C_kg_m2,"
                 <<"Soil_SOM_C_kg_m2,"
                 <<"Canopy_Biomass_N_kg_m2,"
                 <<"Surface_Residue_N_kg_m2,"
                 <<"Soil_Residue_N_kg_m2,"
                 <<"Soil_SOM_N_kg_m2,"
                 //<<"Soil_NO3_N_kg_m2,"
                 //<<"Soil_NH4_N_kg_m2,"
                 <<"NPP_C_kg_m2,"
                 <<"CO2_C_loss_Residue_kg_m2,"
                 <<"CO2_C_loss_SOM_kg_m2,"
                 <<"N_applied_inorg_kg_m2,"
                 <<"N_applied_org_kg_m2,"
                 <<"volatilization_loss_NH3_kg_m2,"
                 <<"volatilization_total_kg_m2,"
                 <<"N_uptake_kg_m2,"
                 <<"N_stress_index,"
                 <<"Nitrification_N_kg_m2,"
                 <<"DeNitrification_N_kg_m2,"
                 <<"Nitrification_N2O_N_kg_m2,"
                 <<"DeNitrification_N2O_N_kg_m2,"
#endif
                 <<"Profile_soil_liquid_water_mm,";
        #ifdef PRINT_VIC_SOIL_LAYERS_FOR_DEBUG
        for (int i = 0; i < lys; i++) debugout << "VWC["           << i << "],";
        for (int i = 0; i < lys; i++) debugout << "Moist_mm["      << i << "],";
        for (int i = 0; i < lys; i++) debugout << "Ice_mm["        << i << "],";
        for (int i = 0; i < lys; i++) debugout << "Depth_mm["      << i << "],";
        for (int i = 0; i < lys; i++) debugout << "T["             << i << "],";
        for (int i = 0; i < lys; i++) debugout << "NO3_kgN_m2["    << i << "],";
        for (int i = 0; i < lys; i++) debugout << "NH4_kgN_m2["    << i << "],";
        if (active_land_unit->organic_matter_residues_profile_kg_m2) {
        for (int i = 0; i < lys; i++) debugout << "Resid_kgC_m2["  << i << "],";
        for (int i = 0; i < lys; i++) debugout << "SOM_kgC_m2["    << i << "],";
        }
        #endif
        debugout << std::endl;
        //debugout.close();
        //180404LML newed = true;
#else
#endif
    }
#ifndef CROP_DAILY_OUTPUT_MEMFIRST
    //debugout.open(out_file_name,std::ofstream::out | std::ofstream::app);
#else
    if(global_rec == 0 && created_head == false) {
        extern std::stringstream crop_output_head;
        crop_output_head <<"cell_id,"
             <<"lon,"
             <<"lat,"
             <<"Year,"
             <<"Month,"
             <<"Day,"
             <<"DOY,"
#ifndef OUTPUT_SIMPLE_FOR_FORECAST
             <<"Dist,"
             <<"Band,"
#endif
             <<"Cell_fract,"
             <<"CroppingSyst_code,"
             <<"Crop_code,"
             <<"Crop_name,"
#ifndef OUTPUT_SIMPLE_FOR_FORECAST
             <<"Accum_DD,"
             <<"Accum_DD_clip,"
             <<"Grow_Stage,"
             //<<"VIC_LAI,"
             //<<"LAI,"
             <<"GAI,"
#endif
             <<"Green_Canopy_Cover,"
             <<"Biomass_kg_m2,"
             <<"Yield_kg_m2,"
#ifndef OUTPUT_SIMPLE_FOR_FORECAST
             <<"Root_depth_mm,"
#ifdef VCS_V5
             <<"VIC_PET_mm,"
#else
             <<"VIC_PET_shortgrass_mm,"
#endif
             //<<"CropSyst_Pot_Transp_mm,"
#endif
             <<"CropSyst_refET_mm,"
             <<"crop_evapotransp_max_mm,"
#ifndef OUTPUT_SIMPLE_FOR_FORECAST
             <<"Act_Transp_mm,"                                                  //180523LML include canopy evap ifdef CROPSYST_HANDLE_CANOPY_EVAP
#endif
             <<"VIC_final_transp_mm,"
#ifndef OUTPUT_SIMPLE_FOR_FORECAST
             <<"irrig_netdemand_mm,"
#endif
             <<"irrig_total_mm,"
#ifndef OUTPUT_SIMPLE_FOR_FORECAST
             <<"irrig_evap_mm,"
             <<"irrig_runoff_mm,"
             <<"irrig_intcpt_mm,"                                            //intercepted by canopy (before & after canopy evap processes)
             //<<"irrig_intcpt_evap_mm,"                                       //to meet evap demand
#endif
             <<"Soil_E_mm,"
#ifndef OUTPUT_SIMPLE_FOR_FORECAST
             <<"Crop_Canopy_E_mm,"
             <<"snow_sublimation,"
             <<"ET_mm,"
#endif
             <<"VIC_ET_mm,"
             <<"water_stress_index,"
             <<"Runoff,"
             <<"Baseflow,"
             <<"PPT,"
             <<"Tair_avg,"
#ifndef OUTPUT_SIMPLE_FOR_FORECAST
             <<"Tmax,"
             <<"Tmin,"
             <<"SWRAD_w_m2,"
             <<"VP_kPa,"
             <<"SPH_kg_kg,"
             <<"RHUM_avg_%,"
             <<"RHUM_max_%,"
             <<"RHUM_min_%,"
             <<"Snow_dep_mm,"
             <<"Crop_Biomass_kg_m2,"
#endif
             <<"SoilProfileMoisture_mm,";
#ifndef OUTPUT_SIMPLE_FOR_FORECAST
        for (int layer = 0; layer < options.Nlayer; layer++)
             crop_output_head <<"soil_liq_vwc[" << layer << "].liq,";
#endif
        crop_output_head << std::endl;
        created_head = true;
    }
#endif
    CropSyst::Crop_model_interface *active_crop = active_land_unit ?
        active_land_unit->crop_active_or_intercrop : 0;
    if (growth_season_only && !active_crop) doprint = false;
    else if (global_simdate /*200727RLN global_today*/.get_year() >= global_param.startyear
             && global_simdate /*200727RLN global_today*/.get_year() <= global_param.endyear) doprint = true;

    //Crop_output_struct::last_yield = 0;
    if (doprint) {                                                               //For DEBUG!!!
        std::wstring wactive_crop_name(L"NOCROP");
        crop_data_struct &temp_vic_crop = active_land_unit->ref_VIC_crop();
        snow_data_struct &snow          = active_land_unit->ref_VIC_snow();
        bool cropnum = active_crop ?
                       active_crop->get_name(wactive_crop_name)
                       : false;
        double transpiration_mm = m_to_mm(active_crop ?
            active_crop->get_act_transpiration_m() : 0);
        double VIC_transpiration_mm = 0;
        for (int index = 0; index < options.Nlayer; index++)
          VIC_transpiration_mm += active_land_unit->ref_VIC_cell().layer[index].evap;
        
        double CropSyst_ref_et_m = active_land_unit->meteorology.ref_ET_reference().get_m(); //190721LML (m)
        double crop_evapotransp_max_m = active_land_unit->output_crop_evapotransp_max_m;     //20201021LML
        //190507 double canopy_evap_mm = temp_vic_crop.canopy_evap;
        double canopy_evap_mm = active_land_unit->ref_VIC_veg_var().canopyevap;     //190507
        double soil_evap_mm = active_land_unit->ref_VIC_cell().layer[0].VCS.evap_bare_soil;
        double irrigation_evap_mm = temp_vic_crop.irrigation_evap;
        double total_evap = canopy_evap_mm + soil_evap_mm
                          + irrigation_evap_mm
                          //190507 + temp_vic_crop.intercepted_irrigation    //190507
                          + snow.vapor_flux * 1000.0                //190507
                          + snow.canopy_vapor_flux * 1000.0;        //190507
        double total_et = transpiration_mm + total_evap; 
        double total_VIC_et = VIC_transpiration_mm + total_evap;    //190507 there is one day shift of transpiration and also an adjustment due to soil moisture limitation and water vertical flow
        double profile_liq_water = 0;
        for (int i = 0; i < options.Nlayer; i++)
            profile_liq_water += active_land_unit->ref_VIC_cell().layer[i].moist;
        std::string active_crop_name(wactive_crop_name.begin(),wactive_crop_name.end());
        Soil::Chemicals_profile *temp_chemical = active_land_unit->get_soil_chemicals(); //200208RLN
        int16 day_since_start_of_harvest = (active_crop? active_crop->ref_phenology().get_days_since_start_of(NGS_HARVEST) : 0);
 
        //std::clog << "VIC_shortgrass_ET:" << (active_land_unit->ref_VIC_cell().VCS.pot_evap_daily[PET_SHORT]) << std::endl;
#ifndef CROP_DAILY_OUTPUT_MEMFIRST
        std::string growth_stage_description;                                    //200208RLN
        /*200817RLN  uncomment if really needed
        nat8 periods = active_crop->describe_periods(growth_stage_description);
         if (periods)                                                            //200420
            period_descriptions.string_items(growth_stage_description,' ');                                //200420

        */

        if ( (debugout.rdstate() & std::ofstream::failbit ) != 0 ) {
              std::cerr << "Error write file:" << out_file_name << "\n";
              return 1;
        }


        debugout <<active_land_unit->ref_VIC_soil_con().gridcel
                 <<","<<std::setprecision(8)<<(double)active_land_unit->ref_VIC_soil_con().lng
                 <<","<<std::setprecision(7)<<(double)active_land_unit->ref_VIC_soil_con().lat
                 <<","<<(int)global_simdate.get_year()
                 <<","<<(int)global_simdate.get_month()
                 <<","<<(int)global_simdate.get_DOM()
                 <<","<<(int)global_simdate.get_DOY()
#ifndef OUTPUT_SIMPLE_FOR_FORECAST
                 <<","<<dist
                 <<","<<band
#endif
                 <<","<<cell_fraction
                 <<","<<(active_land_unit->ref_VIC_veg_con().VCS.veg_class_code)
                 <<","<<VIC_land_unit_get_current_crop_code()
                 <<","<<active_crop_name.c_str()
#ifndef OUTPUT_SIMPLE_FOR_FORECAST
                 <<","<<(active_crop ? active_crop->get_accum_degree_days(/*200323RLN false*/) : 0)
                 <<","<<(active_crop ? active_crop->get_accum_degree_days(/*200323RLN true*/) : 0)
                 //<<","<<(active_crop ? active_crop->get_d (true) : 0)
                 <<","<<(active_crop ?
                            growth_stage_description
                 //200817RLN active_crop->describe_growth_stage(growth_stage_description)
                         : "N/A")
                 //<<","<<VIC_land_unit_get(VIC::LAI)
                 //<<","<<(active_crop ? active_crop->get_LAI(true)                : 0)
                 <<","<<(active_crop ? active_crop->get_GAI(true)                : 0)
#endif
                 <<","<<(active_crop ? active_crop->get_canopy_interception_global_green() : 0)
                    //200323RLN get_fract_canopy_cover_green()     : 0)
                 <<","<<(active_crop ? active_crop->get_canopy_biomass_kg_m2() : 0)
                 //<<","<<(active_crop ? (active_crop->get_canopy_biomass_kg_m2() + active_crop->get_act_root_biomass_kg_m2()) : 0)
                 //<<","<<(active_crop ? active_crop->get_latest_yield_kg_m2()     : 0)
                 <<","<<(active_crop? ((day_since_start_of_harvest==0) ? active_crop->get_latest_yield_kg_m2() : 0.0) : 0.0)
#ifndef OUTPUT_SIMPLE_FOR_FORECAST
                 <<","<<m_to_mm(active_crop ? active_crop->get_recorded_root_depth_m()   : 0)
#ifdef VCS_V5
                 <<","<<(active_land_unit->ref_VIC_cell().VCS.pot_evap_total_daily)
#else
                 <<","<<active_land_unit->ref_VIC_cell().VCS.pot_evap_daily[PET_SHORT]
#endif
                 //<<","<<m_to_mm(active_crop ? active_crop->get_pot_transpiration_m(false) : 0)
#endif
                 <<","<<m_to_mm(CropSyst_ref_et_m)
#ifndef OUTPUT_SIMPLE_FOR_FORECAST
                 <<","<<m_to_mm(active_crop ? active_crop->get_act_transpiration_m()     : 0)
#endif
                 <<","<<VIC_transpiration_mm
#ifndef OUTPUT_SIMPLE_FOR_FORECAST
                 <<","<<temp_vic_crop.irrigation_netdemand
#endif
                 <<","<<temp_vic_crop.irrigation_water
#ifndef OUTPUT_SIMPLE_FOR_FORECAST
                 <<","<<temp_vic_crop.irrigation_evap
                 <<","<<temp_vic_crop.irrigation_runoff
                 <<","<<temp_vic_crop.intercepted_irrigation
                 //<<","<<temp_vic_crop.evap_intercepted_irrig_water
#endif
                 <<","<<soil_evap_mm
#ifndef OUTPUT_SIMPLE_FOR_FORECAST
                 <<","<<canopy_evap_mm
                 <<","<<((snow.vapor_flux + snow.canopy_vapor_flux) * 1000.0)        //190507
                 <<","<<total_et
#endif
                 <<","<<total_VIC_et
                 <<","<<(active_crop ? active_crop->get_water_stress_index() : 0)
                 <<","<<active_land_unit->ref_VIC_cell().runoff
                 <<","<<active_land_unit->ref_VIC_cell().baseflow
#ifdef VCS_V5
                 <<","<<active_land_unit->get_weather_provider()->precipitation_actual.get_mm()
                 <<","<<active_land_unit->get_weather_provider()->air_temperature_avg.Celcius()
#else
                 <<","<<active_land_unit->ref_VIC_atmos()[global_rec].prec[options.VCS.NR]
                 <<","<<active_land_unit->ref_VIC_atmos()[global_rec].air_temp[options.VCS.NR]
#endif
#ifndef OUTPUT_SIMPLE_FOR_FORECAST
                 <<","<<active_land_unit->ref_VIC_atmos()[global_rec].VCS.tmax
                 <<","<<active_land_unit->ref_VIC_atmos()[global_rec].VCS.tmin
                 <<","<<active_land_unit->ref_VIC_atmos()[global_rec].shortwave[options.VCS.NR]
                 <<","<<active_land_unit->ref_VIC_atmos()[global_rec].vp[options.VCS.NR]
#ifdef __BCPLUSPLUS__
// RLN  compiler not finding sph
#else
                 <<","<<active_land_unit->ref_VIC_atmos()[global_rec].VCS.sph[options.VCS.NR]
#endif
                 <<","<<active_land_unit->ref_VIC_atmos()[global_rec].VCS.relative_humidity[options.VCS.NR]
                 <<","<<active_land_unit->ref_VIC_atmos()[global_rec].VCS.relative_humidity_max
                 <<","<<active_land_unit->ref_VIC_atmos()[global_rec].VCS.relative_humidity_min
                 <<","<<m_to_mm(snow.depth)
                 //<<","<<(active_crop ? (active_crop->get_canopy_biomass_kg_m2() + active_crop->get_act_root_biomass_kg_m2()) : 0)
                 <<","<<(active_crop ? active_crop->get_canopy_biomass_kg_m2() : 0)
#endif
#ifdef PRINT_SOM_FOR_DEBUG
                 <<","<<VIC_land_unit_get(VIC::SURFACE_RESIDUE_C_MASS)
                 <<","<<VIC_land_unit_get(VIC::SUBSURFACE_RESIDUE_C_MASS)
                 <<","<<VIC_land_unit_get(VIC::PROFILE_SOM_C_MASS)
                 <<","<<(active_crop ? active_crop->get_N_mass_canopy_kg_m2(true) : 0)
                 <<","<<VIC_land_unit_get(VIC::SURFACE_RESIDUE_N_MASS)
                 <<","<<VIC_land_unit_get(VIC::SUBSURFACE_RESIDUE_N_MASS)
                 <<","<<VIC_land_unit_get(VIC::PROFILE_SOM_N_MASS)
                 //<<","<<VIC_land_unit_get(VIC::PROFILE_NO3_N_MASS)
                 //<<","<<VIC_land_unit_get(VIC::PROFILE_NH4_N_MASS)
                 <<","<<(active_crop ? (active_crop->get_C_mass_canopy() + active_crop->get_C_mass_roots()) : 0)
                 <<","<<VIC_land_unit_get(VIC::CO2_C_LOSS_RESIDUE)
                 <<","<<VIC_land_unit_get(VIC::CO2_C_LOSS_SOM)
                 <<","<<active_land_unit->nitrogen_daily_output.applied_inorganic           //170131LML
                 <<","<<active_land_unit->nitrogen_daily_output.applied_organic             //170131LML
                 <<","<<active_land_unit->nitrogen_daily_output.volatilization_loss_NH3     //170131LML
                 <<","<<active_land_unit->nitrogen_daily_output.volatilization_total        //170131LML
                 <<","<<(active_crop ? (active_crop->get_nitrogen()? active_crop->get_nitrogen()->get_daily_uptake() : 0) : 0)
                 <<","<<(active_crop ? (active_crop->get_nitrogen()? active_crop->get_nitrogen()->get_stress_index() : 0) : 0)
                 <<","<<(temp_chemical ? temp_chemical->nitrification_N_E_output[0] : -9999)
                 <<","<<(temp_chemical ? temp_chemical->denitrification_N_E_output[0] : -9999)
                 <<","<<(temp_chemical ? (temp_chemical->NH4 ? temp_chemical->NH4->get_N2O_loss() : -9999) : -9999)     //150925LML VIC_land_unit_get(VIC::NITRIFICATION_N2O_N)
                 <<","<<(temp_chemical ? (temp_chemical->NO3 ? temp_chemical->NO3->get_N2O_loss() : -9999) : -9999)
#endif
                 <<","<<profile_liq_water
                 ;
        #ifdef PRINT_VIC_SOIL_LAYERS_FOR_DEBUG
        for (int i = 0; i < lys; i++)
            debugout<<","<<(active_land_unit->ref_VIC_cell().layer[i].moist
                / m_to_mm(active_land_unit->ref_VIC_soil_con().depth[i]));
        for (int i = 0; i < lys; i++)
            debugout<<","<<active_land_unit->ref_VIC_cell().layer[i].moist;
        for (int i = 0; i < lys; i++)
            debugout<<","<<active_land_unit->ref_VIC_cell().layer[i].ice;
        for (int i = 0; i < lys; i++)
            debugout<<","<<m_to_mm(active_land_unit->ref_VIC_soil_con().depth[i]);
        for (int i = 0; i < lys; i++)
            debugout<<","<<active_land_unit->ref_VIC_cell().layer[i].T;
        for (int i = 0; i < lys; i++)
            debugout<<","<<(active_land_unit->get_soil_chemicals() ?
                            active_land_unit->get_soil_chemicals()->get_NO3_N_mass_kg_m2(i+1)
                            : 0.0);
        for (int i = 0; i < lys; i++)
            debugout<<","<<(active_land_unit->get_soil_chemicals() ?
                            active_land_unit->get_soil_chemicals()->get_NH4_N_mass_kg_m2(i+1)
                            : 0.0);
        if (active_land_unit->organic_matter_residues_profile_kg_m2) {
        for (int i = 0; i < lys; i++)
            debugout<<","<<active_land_unit->organic_matter_residues_profile_kg_m2->get_mass_from_layer_to_layer(C_MASS_TOTAL,INCLUDE_RESIDUE_OM,nat8(i+1),nat8(i+1),1);
        for (int i = 0; i < lys; i++)
            debugout<<","<<active_land_unit->organic_matter_residues_profile_kg_m2->get_mass_from_layer_to_layer(C_MASS_TOTAL,INCLUDE_SOM_AND_MICROBIAL,nat8(i+1),nat8(i+1),1);
        }
        #endif
        debugout<<std::endl;
        //debugout.close();
#else
         extern std::list<Crop_output_struct> crop_output_list;
         Crop_output_struct temp;
         temp.cell_id       = active_land_unit->ref_VIC_soil_con().gridcel;
         temp.lon           = (double)active_land_unit->ref_VIC_soil_con().lng;
         temp.lat           = (double)active_land_unit->ref_VIC_soil_con().lat;
         temp.Year          = (int)global_simdate/*200827 global_today*/.get_year();
         temp.Month         = (int)global_simdate/*200827 global_today*/.get_month();
         temp.Day           = (int)global_simdate/*200827 global_today*/.get_DOM();
         temp.DOY           = (int)global_simdate/*200827 global_today*/.get_DOY();
#ifndef OUTPUT_SIMPLE_FOR_FORECAST
         temp.Dist = dist;
         temp.Band = band;
#endif
         temp.Cell_fract = cell_fraction;
         temp.CroppingSyst_code = (active_land_unit->ref_VIC_veg_con().VCS.veg_class_code);
         temp.Crop_code = VIC_land_unit_get_current_crop_code();
         strcpy(temp.Crop_name,active_crop_name.c_str());
         //temp.Crop_name = active_crop_name.c_str()
#ifndef OUTPUT_SIMPLE_FOR_FORECAST
         temp.Accum_DD = (active_crop ? active_crop->get_accum_degree_days() : 0);
         temp.Accum_DD_clip = (active_crop ? active_crop->get_accum_degree_days() : 0);
         //std::string t = (active_crop ? active_crop->describe_growth_stage()      : "N/A");
#ifdef LIU_DEBUG
         if (active_crop) {
             CORN:Text_list t;
             std::string str;
             active_crop->describe_periods(t);
             t.string_items(str,'&');
             str.erase(std::remove(str.begin(),str.end(),' '), str.end());
             strcpy(temp.Grow_Stage, str.c_str());
         } else
#endif
             temp.Grow_Stage[0] = 0;
#endif

         //CORN::Text_list t;


         //if (active_crop) {
         //    double tgai = active_crop->get_GAI(true);
         //    double tgdd = active_crop->param_emergence_deg_day();
         //    //Crop_complete::describe_periods(CORN::Text_list &descriptions)
         //
         //    bool p = active_crop->get_root_biomass();
         //    //strcpy(temp.Grow_Stage, t.f);
         //    bool ig = active_crop->is_at_season_end();
         //    double cm = active_crop->get_C_mass_canopy();
         //    std::cerr << "before describe_periods" << "\n";
         //    nat8 c = active_crop->describe_periods(t);
         //    float64 ci = active_crop->get_canopy_interception_global_total();
         //    std::cerr << "after describe_periods" << "\n";
         //    Item_string *s = dynamic_cast<Item_string *>(t.pop_first());
         //    if(s) std::cerr << "Stage:" << s->c_str() << "\n";
         //}
         //std::cerr << "TBD LML 200518!!!\n";

         //int16 day_since_start_of_harvest = (active_crop? active_crop->ref_phenology().get_days_since_start_of(NGS_HARVEST) : 0);
#ifndef OUTPUT_SIMPLE_FOR_FORECAST
         temp.GAI = (active_crop ? active_crop->get_GAI(true)                : 0);
#endif
         temp.Green_Canopy_Cover = (active_crop ? active_crop->get_canopy_interception_global_green()     : 0);
         //temp.Biomass_kg_m2 = (active_crop ? active_crop->get_canopy_biomass_kg_m2()   : 0);
         //temp.Biomass_kg_m2 = (active_crop ? (active_crop->get_canopy_biomass_kg_m2() + active_crop->get_act_root_biomass_kg_m2()) : 0);
         temp.Biomass_kg_m2 = active_crop ? active_crop->get_canopy_biomass_kg_m2() : 0;
         temp.Yield_kg_m2 = active_crop? ((day_since_start_of_harvest==0) ? active_crop->get_latest_yield_kg_m2() : 0.0) : 0.0;
         // temp.Yield_kg_m2 = (active_crop ? active_crop->get_latest_yield_kg_m2()     : 0);
#ifndef OUTPUT_SIMPLE_FOR_FORECAST
         temp.Root_depth_mm = m_to_mm(active_crop ? active_crop->get_recorded_root_depth_m()   : 0);
         temp.VIC_PET_shortgrass_mm = active_land_unit->ref_VIC_cell().VCS.pot_evap_daily[PET_SHORT];
#endif
         temp.CropSyst_refET_mm = m_to_mm(CropSyst_ref_et_m);
         temp.crop_evapotransp_max_mm = m_to_mm(crop_evapotransp_max_m);
#ifndef OUTPUT_SIMPLE_FOR_FORECAST
         temp.Act_Transp_mm = m_to_mm(active_crop ? active_crop->get_act_transpiration_m()     : 0);
#endif
         temp.VIC_final_transp_mm = VIC_transpiration_mm;
#ifndef OUTPUT_SIMPLE_FOR_FORECAST
         temp.irrig_netdemand_mm = temp_vic_crop.irrigation_netdemand;
#endif
         temp.irrig_total_mm = temp_vic_crop.irrigation_water;
#ifndef OUTPUT_SIMPLE_FOR_FORECAST
         temp.irrig_evap_mm = temp_vic_crop.irrigation_evap;
         temp.irrig_runoff_mm = temp_vic_crop.irrigation_runoff;
         temp.irrig_intcpt_mm = temp_vic_crop.intercepted_irrigation;
#endif
         temp.Soil_E_mm = soil_evap_mm;
#ifndef OUTPUT_SIMPLE_FOR_FORECAST
         temp.Crop_Canopy_E_mm = canopy_evap_mm;
         temp.snow_sublimation = ((snow.vapor_flux + snow.canopy_vapor_flux) * 1000.0);        //190507
         temp.ET_mm = total_et;
#endif
         temp.VIC_ET_mm = total_VIC_et;
         temp.water_stress_index = (active_crop ? active_crop->get_water_stress_index() : 0);
         temp.Runoff = active_land_unit->ref_VIC_cell().runoff;
         temp.Baseflow = active_land_unit->ref_VIC_cell().baseflow;
         temp.PPT = active_land_unit->ref_VIC_atmos()[global_rec].prec[options.VCS.NR];
         temp.Tair_avg = active_land_unit->ref_VIC_atmos()[global_rec].air_temp[options.VCS.NR];
#ifndef OUTPUT_SIMPLE_FOR_FORECAST
         temp.Tmax = active_land_unit->ref_VIC_atmos()[global_rec].VCS.tmax;
         temp.Tmin = active_land_unit->ref_VIC_atmos()[global_rec].VCS.tmin;
         temp.SWRAD_w_m2 = active_land_unit->ref_VIC_atmos()[global_rec].shortwave[options.VCS.NR];
         temp.VP_kPa = active_land_unit->ref_VIC_atmos()[global_rec].vp[options.VCS.NR];
         temp.SPH_kg_kg = active_land_unit->ref_VIC_atmos()[global_rec].VCS.sph[options.VCS.NR];
         temp.RHUM_avg_per = active_land_unit->ref_VIC_atmos()[global_rec].VCS.relative_humidity[options.VCS.NR];
         temp.RHUM_max_per = active_land_unit->ref_VIC_atmos()[global_rec].VCS.relative_humidity_max;
         temp.RHUM_min_per = active_land_unit->ref_VIC_atmos()[global_rec].VCS.relative_humidity_min;
         temp.Snow_dep_mm = m_to_mm(snow.depth);
#endif
         //temp.Biomass_kg_m2 = (active_crop ? (active_crop->get_canopy_biomass_kg_m2() + active_crop->get_act_root_biomass_kg_m2()) : 0);
         temp.profile_liq_water = profile_liq_water;
#ifndef OUTPUT_SIMPLE_FOR_FORECAST
         temp.soil_liq_vwc.resize(options.Nlayer);
         for (int layer = 0; layer < options.Nlayer; layer++) {
             temp.soil_liq_vwc[layer] = active_land_unit->ref_VIC_cell().layer[layer].moist
                                        / m_to_mm(active_land_unit->ref_VIC_soil_con().depth[layer]);
         }
#endif

     crop_output_list.push_back(temp);
#endif

#ifdef OUTPUT_FULLIRRIGATION_PATTERN
     if (active_crop) {
         if (temp_vic_crop.irrigation_water >= 0.01) {
             Out_irrigation_pattern_struct irrip;
             irrip.cell_id = active_land_unit->ref_VIC_soil_con().gridcel;
             irrip.lon           = (double)active_land_unit->ref_VIC_soil_con().lng;
             irrip.lat           = (double)active_land_unit->ref_VIC_soil_con().lat;
             irrip.Year          = (int)global_simdate.get_year();
             irrip.DOY           = (int)global_simdate.get_DOY();
             irrip.rotation_code = (active_land_unit->ref_VIC_veg_con().VCS.veg_class_code);
             irrip.irrigation_amount = temp_vic_crop.irrigation_water;
             out_irrigation_pattern_output_list.push_back(irrip);
         }
     }
#endif

    }

    return print_status;
}
/*_2010-10-10___________________________________________VIC_land_unit_end_day_*/
int VIC_land_unit_process_day()                                                  //150611RLN
{  return (active_land_unit) ? active_land_unit->process_day() : 0;              //150611RLN
}
/*______________________________________________________VIC_land_unit_process_*/
const CropSyst::Irrigation_operation_struct *VIC_land_unit_get_irrigation_parameters() //190128RLN
{  return (active_land_unit) ? active_land_unit->get_irrigation_parameters() : 0;
}
/*____________________________________________________________________________*/
double VIC_land_unit_get_irrigation_target()                                     //201028LML
{
   return (active_land_unit) ? active_land_unit->irrig_appl_target : 0;          //(m)
}
/*____________________________________________________________________________*/
int VIC_land_unit_chemical_transport_and_exchange
   (unsigned long int at_time
   ,unsigned long int duration
   ,double           *old_liquid_water_content
   ,double           *new_liquid_water_content
   ,double            water_entering_soil
   ,double           *water_flow)
{
   return (active_land_unit)
      ? active_land_unit->chemical_transport_and_exchange
         ((Seconds)             at_time
         ,(Seconds)             duration
         ,old_liquid_water_content
         ,new_liquid_water_content
         ,water_entering_soil
         ,water_flow)
      : 0;
}
/*_VIC_land_unit_chemical_transport_and_exchange_______________2015-07-26_RLN_*/
/*150701LML
Irrigation_Type VIC_land_unit_get_irrigation_type(const CropSyst::Irrigation_operation_struct &irrigation)
{
  assert(false);
  return(IrrigTP_CP_impact_14VH_RainBird); //LML 150504 Need talk with Roger
}
*/
/*_VIC_land_unit_get_irrigation_parameters_________________________2015-04-13_*/
double VIC_land_unit_get
(unsigned long int variable_code
/*NYI ,unsigned long int units_code*/)
{  // For some unknown reason VIC_land_unit_get is returning large integer values
   // under Linux (GNUC++). As a work around setting global VIC_land_unit_get_return
   // which can be read after calling VIC_land_unit_get();

   VIC_land_unit_get_return =
   (active_land_unit) // precondition
      ? (double)active_land_unit->get
         ((nat32)variable_code)
      : (double)0.0;
   return VIC_land_unit_get_return ;
}
/*_VIC_land_unit_get_______________________________________________2015-04-15_*/
/* LML 150423
double VIC_CropSyst_get_transpiration(double layerevap[MAX_LAYERS])
{
    //Get modeled transpiration (for each layer) from CropSyst
    //layerevap has VIC layers from 0 (the first layer) to VIC_options->Nlayer-1
    return active_land_unit->vic_soil_for_CropSyst.hydrology.extract_water(layerevap,1);
}
*/
/*____________________________________________________________________________*/
double VIC_CropSyst_get
(unsigned long int variable_code
/*NYI ,unsigned long int units_code*/)
{  // For some unknown reason VIC_CropSyst_get is returning large integer values
   // under Linux (GNUC++). As a work around setting global VIC_CropSyst_get_return
   // which can be read after calling VIC_CropSyst_get();

   return VIC_land_unit_get(variable_code);
}
/*____________________________________________________________________________*/
/*150415 The log file is option and not really needed for VIC
const char* getCropSystLogFileName()
{
    // Generate log file name according global file names
    //assert(false);
    char logfilename[MAXSTRING];
    //strcpy(logfilename,filenames.CropSyst_Output_dir);
    strcpy(logfilename,filenames.CropSyst_Working_dir);
    #ifdef __linux__
    strcat(logfilename,"/CropSyst_log.txt");
    #else
    strcat(logfilename,"/\CropSyst_log.txt");  //LML 141103 need double check!!
    #endif
    return logfilename;
}
*/
/*____________________________________________________________________________*/
unsigned long int VIC_land_unit_create
(int cell_ID  //gridcell 150415RLN
, int rotation_or_crop_veg_class_code
, int rotation_cycle_index
, crop_data_struct  &VIC_crop_
, soil_con_struct   &VIC_soil_con_
, option_struct     &VIC_options_
, veg_con_struct    &VIC_veg_con_
, atmos_data_struct *VIC_atmos_
, cell_data_struct  &VIC_cell_
, snow_data_struct  &VIC_snow_
, veg_var_struct    &VIC_veg_var_)
{   //rotation_number = _VIC_veg_con->veg_class_code
    //CS::Identification *cellid = new CS::Identification((int32)cell_ID);
    CS::Identification cellid((int32)cell_ID);
    wchar_t test;  //20200821LML
    //CS::Arguments *arguments_relinquished = new CS::Arguments(cellid,&test);          //200817

    CS::Arguments arguments_relinquished(cellid,&test);          //200817

    /*200817
    CropSyst::Arguments *arguments_relinquished = new CropSyst::Arguments
       (*cellid,cell_ID,rotation_cycle_index);
    */
    //160421LML obs. arguments_relinquished->initialize();
   /*150415RLN obsolete, VIC land unit will maintain its own log
   const char* log_file_name = getCropSystLogFileName();
   Common_simulation_log *event_log = new Common_simulation_log(log_file_name);
   CORN::OS::Directory_name_concrete dir_name(filenames.CropSyst_Working_dir);
   const CropSyst::Scenario_directory *scenario_directory = new CropSyst::Scenario_directory(dir_name,true);

   //scenario_directory->set_str(filenames.VIC_land_unit_scenario_dir);
   //scenario_directory
   */

   //assert(false);           //LML 141024 provide scenario_directory name
/*150415 RLN obsolete unused
   CropSyst::Scenario *crop_scenario = generate_scenario_for_crop_count(
               _VIC_crop
               ,_VIC_soil_con
               ,_VIC_veg_lib
               ,_VIC_options
               ,_VIC_veg_con
               ,_VIC_Crop_count_index
               );
               */
    /***************************************************************************
    * LML 150414 This block should be construct the scenario for this cropping
    * .identify scenario file name and read this file to crop_scenario
    * .identify rotation type and construct the rotation & management
    ***************************************************************************/
/*
   sint32 LBF_ID = 0; //(sint32)(nat32)(*crop_scenario) // Just an arbitrary unique ID get unique ID
*/
   active_land_unit = new VIC::Land_unit_simulation
      (
      &arguments_relinquished
       ,(nat32)rotation_or_crop_veg_class_code                                   //150521
       ,(nat8) rotation_cycle_index                                              //150521
       ,global_simdate.ref_date32() //200828RLN global_simdate_raw //190130 global_today.get_date32()
       ,VIC_crop_
       ,VIC_soil_con_
       ,VIC_options_
       ,VIC_veg_con_
       ,VIC_atmos_
       ,VIC_cell_
       ,VIC_snow_ /*150608LML*/
       ,VIC_veg_var_

      );

   // The label is not currently used
   //LML 150422 active_land_unit->know_VIC_environment
   //LML 150422    (_VIC_crop
   //LML 150422     ,_VIC_soil_con
   //LML 150422     ,_VIC_veg_lib
   //LML 150422     ,_VIC_options
   //LML 150422     ,_VIC_veg_con
   //LML 150422     ,_VIC_atmos
   //LML 150422     ,_VIC_cell
   //LML 150422     ,_VIC_Crop_count_index
   //LML 150422    // ,true   // negative values indicate offsite
   //LML 150422    );
   if (active_land_unit && !active_land_unit->database_valid)                    //110617_110209
   {  delete active_land_unit;                                                   //110209
      active_land_unit = 0;                                                      //110209
   }                                                                             //110209
   return ( unsigned long int)active_land_unit;
}
/*_VIC_land_unit_create________________________________________2014-10-24_LML_*/
/*150415RLN obsolete
 The VIC::Land_unit will maintaing its own scenario control options

CropSyst::Scenario *generate_scenario_for_crop_count
(crop_data_struct *_VIC_crop
 ,soil_con_struct *_VIC_soil_con
 ,veg_lib_struct *_VIC_veg_lib
 ,option_struct *_VIC_options
 ,veg_con_struct *_VIC_veg_con
 ,int _VIC_Crop_count_index)
{
    //Generate the scenario data structure for each cropping systems in each grid cell.
    //Each has its own rotation types and initial C & N pools.
    CropSyst::Scenario *cs = new CropSyst::Scenario();
    return cs;
    //assert(false);  //LML NIY
}
*/
/*____________________________________________________________________________*/
unsigned long int VIC_land_unit_activate
(unsigned long int land_unit_handle)
{  if (land_unit_handle)
   {
      VIC::Land_unit_simulation *activating_land_unit
           = (VIC::Land_unit_simulation *)land_unit_handle;
      active_land_unit = activating_land_unit;
   }
   return (unsigned long int)active_land_unit;
}
/*_VIC_land_unit_activate___________________________________________150714LML_*/
unsigned long int VIC_land_unit_deactivate()
{  if (active_land_unit)
   {
      active_land_unit = 0;
   }
   return (unsigned long int)active_land_unit;
}
/*____________________________________________________________________________*/
unsigned long int VIC_CropSyst_object_activate
(unsigned long int land_unit_handle)
{ return VIC_land_unit_activate(land_unit_handle); }
//RLN Not sure what happened to the handle parameter, dont know how Gcc compiles this
//RLN 191030 { return VIC_land_unit_activate; }
/*____________________________________________________________________________*/
unsigned long int VIC_land_unit_delete
(unsigned long int land_unit_handle)
{  unsigned long int result = 0;
   if (land_unit_handle)
   {
      VIC::Land_unit_simulation *deleting_CropSyst
          = (VIC::Land_unit_simulation *)land_unit_handle;
      if (deleting_CropSyst == active_land_unit)
         active_land_unit = 0;
      {  delete deleting_CropSyst;
         result = 1;
      }
   }
   return result;
}
/*____________________________________________________________________________*/
unsigned long int VIC_CropSyst_object_delete
(unsigned long int land_unit_handle)
{  return VIC_land_unit_delete(land_unit_handle); }
//190130 {  return VIC_land_unit_delete; }
/*____________________________________________________________________________*/
int VIC_know_project_directory(const char *project_directory_name)
{// returns 1 (true) if the directory actually exists else 0 (false)
   delete VIC::project_directory;
   VIC::project_directory = new CORN::OS::Directory_name_concrete(
       project_directory_name);
   return CORN::OS::file_system_engine.exists(*VIC::project_directory);
}
/*_VIC_know_project_directory______________________________________2015-05-20_*/
int VIC_get_rotation_cycle_years(int rotation_or_crop_veg_class_code)
{  nat8 rotation_cycle_years = 0;
   // First check if we have already loaded rotation parameters
   Rotation_parameters_enumerated *rotation_params =
      dynamic_cast<Rotation_parameters_enumerated *>
      (rotations_parameters.find(rotation_or_crop_veg_class_code));
   if (rotation_params)
   {
      rotation_cycle_years = rotation_params->years;
   } else
   {  // Cell cropping system may have a single crop (I.e. fruit trees)
      // with no rotation.  In this case there is only one cycle year.
      VIC_know_project_directory(filenames.VCS.CropSyst_Working_dir);            //150527
      char rotation_or_crop_veg_class_code_cstr[10];
      if (rotation_or_crop_veg_class_code > 10000) rotation_or_crop_veg_class_code -= 10000;                  //200909LML nonirrigated crops
      CORN::int32_to_cstr(rotation_or_crop_veg_class_code,
                          rotation_or_crop_veg_class_code_cstr,10,10,'\0');      //LML 150527 added '\0'
      CORN::OS::Directory_name_concrete project_database_directory
         (*VIC::project_directory,"Database");
      std::clog << "project_database_directory:"
                << project_database_directory.c_str() << std::endl;
      /*170223LML
      CORN::OS::Directory_name_concrete crop_directory
         (project_database_directory,"Crop");
      std::clog << "crop_directory:" << crop_directory.c_str() << std::endl;
      CORN::OS::File_name_concrete crop_filename
         (crop_directory,rotation_or_crop_veg_class_code_cstr,".CS_crop");
      if (CORN::OS::file_system_engine.exists(crop_filename))
         rotation_cycle_years = 1;
      else
      {  */
          CORN::OS::Directory_name_concrete rotation_directory
             (project_database_directory,"Rotation");
          std::clog << "rotation_directory:" << rotation_directory.c_str()
                    << std::endl;
          CORN::OS::Directory_name_concrete rotation_code_directory
             (rotation_directory,"Code");                                        //150527 TEST ONLY!!!
         CORN::OS::File_name_concrete *rotation_filename
            = new CORN::OS::File_name_concrete
               (rotation_code_directory,
                rotation_or_crop_veg_class_code_cstr,"CS_rotation");
         Rotation_parameters_enumerated *rotation_params
               = new Rotation_parameters_enumerated
               (rotation_or_crop_veg_class_code,rotation_filename);
         CORN::VV_File rotation_file(rotation_filename->c_str());
         std::clog << "Rotation file name:" << rotation_filename->c_str()
                   <<std::endl;
         if (rotation_file.get(*rotation_params))                                //170220RLN
         {
            rotations_parameters.append(rotation_params);
            rotation_cycle_years = rotation_params->years;
         } else // not a rotation (preestablished crop)                          //170220RLN
         {  delete rotation_params;
            rotation_cycle_years = 1;                                            //170221RLN

            // ROGER: Continue here should maintain a list of crop codes that
            // dont have rotation so we dont have to keep checking
            // for rotation file existance.  170212
         }
      //170223LML}
   }
   return rotation_cycle_years;
}
/*_VIC_get_rotation_cycle_years____________________________________2015-05-20_*/
int find_index_in_sorted_array(const int *data, const int key,
                               int imin, int imax)
{
    // continue searching while [imin,imax] is not empty
    while (imax >= imin)
    {
        // calculate the midpoint for roughly equal partition
        int imid = (imin + imax) / 2;
        if(data[imid] == key)
          return imid;           // key found at index imid
        else if (data[imid] < key)// determine which subarray to search
          imin = imid + 1;// change min index to search upper subarray
        else
          imax = imid - 1;// change max index to search lower subarray
    }
    // key was not found
    return -1;
}
//_find_index_in_sorted_array_______________________________________2015-04-13_/
int get_veg_lib_index(const int veg_class_code)      //LML 141015
{
    extern veg_lib_struct *veg_lib;
    //crop_data_struct crop_local;
    int num_veg_types = veg_lib[0].NVegLibTypes + N_PET_TYPES_NON_NAT;
    int *veg_codes = (int *) calloc(num_veg_types, sizeof(int));
    for (int i = 0; i < num_veg_types; i++)
        veg_codes[i] = veg_lib[i].VCS.veg_class_code;
    int crop_rotation_code_without_sowing = veg_class_code;                      //LML 150428
    if (veg_class_code >= VEG_CLASS_CODE_START_SPECIFIC_SOWING)
      crop_rotation_code_without_sowing = veg_class_code
                                          / MULTIPLIER_FOR_SOWING_CODE;          //LML 150428
    int index = find_index_in_sorted_array(veg_codes,
                                           crop_rotation_code_without_sowing,
                                           0,
                                           num_veg_types - 1);
    free(veg_codes);
    return index;
}
/*_get_veg_lib_index___________________________________________2015-04-14_LML_*/
int get_irrig_lib_index(const int irrig_code)
{
    extern veg_lib_struct *veg_lib;
    //crop_data_struct crop_local;
    int num_types = IRRIGATION_TYPE_ACCOUNTS;                                    //160504LML IRRIGATION_TYPES;
    int *codes = (int *) calloc(num_types, sizeof(int));
    for (int i = 0; i < num_types; i++) {
        codes[i] = (int)Irrigation_library[i].IrrigationType_code;
    }
    int index = find_index_in_sorted_array(codes,irrig_code,0,
                                           IRRIGATION_TYPE_ACCOUNTS-1);          //160504LML IRRIGATION_TYPES-1);
    free(codes);
    return index;
}
/*____________________________________________________________________________*/
//LML 150526 int get_years_in_rotation(int rotation_code)
//LML 150526 {
//LML 150526     //assert(false);    //Roger TODO 150424
//LML 150526     return 2;
//LML 150526 }
//160520LML____________________________________________________________________/
int get_code_from_name(const std::string crop_name)
{
    int code = -1;
    #if VIC_CROPSYST_VERSION>=3
    extern CropCodeLib *cropcode_lib;                                            //161102LML
    extern int account_crop_types;                                               //161102LML
    for (int i = 0; i < account_crop_types; i++) {
        if (std::string(cropcode_lib[i].cropname) == crop_name) {
            code = cropcode_lib[i].code;
            break;
        }
    }
    #endif
    return code;
}
/*_get_code_from_name______________________________________________2015-05-05_*/
int VIC_land_unit_get_current_crop_code()
{
    int code = 0;
    std::wstring wactive_crop_name(L"NOCROP");
    CropSyst::Crop_model_interface //200208RLN Crop_interfaced
    *active_crop = active_land_unit ?
        active_land_unit->crop_active_or_intercrop : 0;
    bool cropnum = active_crop ?
                   active_crop->get_name(wactive_crop_name)
                   : false;
    std::string active_crop_name(wactive_crop_name.begin(),
                                 wactive_crop_name.end());
    if (is_digits(active_crop_name)) code = atoi(active_crop_name.c_str());
    else code = get_code_from_name(active_crop_name);
    //if (code == -1) std::cerr << "Can't find code for crop:"
    //                          << active_crop_name
    //                          << std::endl;
    return code;
}
/*_VIC_land_unit_get_current_crop_code_____________________________2015-06-16_*/
int VIC_land_unit_get_current_crop_name(char *crop_name)
{  int crop_name_length = 0;
   crop_name[0] = 0;
   if (active_land_unit)
   {  std::wstring current_crop_name;
      if (active_land_unit->get_active_crop_name(current_crop_name))
      {
         CORN::wstring_to_ASCIIZ(current_crop_name,crop_name);                   //160520LML seems doesn't work!!!
         crop_name_length=current_crop_name.length();
      }
   }
   return crop_name_length;
}
/*_VIC_land_unit_get_current_crop_name_____________________________2015-07-20_*/
int VIC_land_unit_is_current_crop_fruit_tree()                                   //150720RLN
{  return (active_land_unit)
      ? active_land_unit->is_current_crop_fruit_tree()
      : false;
}
/*_VIC_land_unit_is_current_crop_fruit_tree________________________2015-07-20_*/
double VIC_land_unit_update_PET()
{
    return (active_land_unit) ? active_land_unit->update_PET_from_VIC() : 0.0;
}
/*_VIC_land_unit_update_PET___________________________________________________*/

/*_2015-07-20_RLN_____________________________________________________________*/
//160531LML____________________________________________________________________/
bool is_digits(const std::string &str)
{
    return str.find_first_not_of("0123456789") == std::string::npos;
}
#endif



