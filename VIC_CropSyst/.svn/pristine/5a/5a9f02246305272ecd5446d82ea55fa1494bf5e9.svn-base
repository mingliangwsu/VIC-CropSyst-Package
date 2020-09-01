#ifdef VIC_CROPSYST_VERSION
#  include "VIC_crop_C_interface.h"
#  if (VIC_CROPSYST_VERSION>=3)
#     include "global.h"
#  endif
/**
   \author Roger Nelson   Biological System Engineering  Washington State University
//#ifdef CROPSYST_ON
   \email rnelson@wsu.edu
   \date   2010-03-10
This code is a hybrid of C and C++ intended to provide a C interface
to the C++ crop model.
This follows C style coding conventions
This is intended for a C++ compiler that supports mixed mode C/C++.
*/
#  ifdef CROPSYST_PROVIDED_SOIL_HYDROLOGY
#     include "VIC_crop_weather_soil.h"
#  else
#     include "VIC_crop_weather.h"
#  endif
#  include "VIC_crop_variable_codes.h"

#  if (VIC_CROPSYST_VERSION>=3)
#     include "agronomic/VIC_land_unit_simulation.h"
#     include "common/simulation/log.h"
#     include "CropSyst/source/cs_scenario_directory.h"
#  endif
double VIC_CropSyst_get_return=0.0;
/*____________________________________________________________________________*/
namespace VIC_crop
{
#ifdef CROPSYST_PROVIDED_SOIL_HYDROLOGY
   Crop_with_weather_soil     *active_CropSyst      = 0;  // Global variable.
#else
   #if (VIC_CROPSYST_VERSION>=3)
   VIC::Land_unit_simulation                                                     //150116
   //150116 VIC_crop_land_unit_simulation
   #else
   Crop_with_weather
   #endif
    *active_CropSyst      = 0;  // Global variable.    //120619
#endif
}
using namespace VIC_crop;
/*____________________________________________________________________________*/
#ifdef USE_CROPSYST_SUBLAYERING
void VIC_CropSyst_know_soil_layer
(int     layer_number
,double  thickness   // meter
,double  sand  // percent
,double  clay  // percent
,double  preferred_field_capacity
,double  preferred_permanent_wilt_point
,int     subdivide
)
{  if (active_CropSyst)
      active_CropSyst->know_soil_layer
         ((nat8)   layer_number
         ,(float64) thickness
         ,(float64) sand
         ,(float64) clay
         ,(float64) preferred_field_capacity
         ,(float64) preferred_permanent_wilt_point
         ,(nat8)    subdivide);
}
/*_______________________________________________VIC_CropSyst_know_soil_layer_*/
#endif
#ifdef CROPSYST_PROVIDED_SOIL_HYDROLOGY
void VIC_know_soil_layer_thickness
(double horizon_1_thickness_m
,double horizon_2_thickness_m
,double horizon_3_thickness_m)
{  if (active_CropSyst)
      active_CropSyst->know_soil_layer_thickness
         ((float64)horizon_1_thickness_m
         ,(float64)horizon_2_thickness_m
         ,(float64)horizon_3_thickness_m);
}
/*_2011-03-02____________________________________VIC_CropSyst_know_soil_layer_*/
double  VIC_CropSyst_know_soil_water_content
(int     horizon_number
,double  water_content_volumetric)
{  return (active_CropSyst)
      ? active_CropSyst->know_soil_water_content
         ((nat8)horizon_number,(float64)water_content_volumetric)
      : 0.0;
}
/*_______________________________________VIC_CropSyst_know_soil_water_content_*/
double VIC_CropSyst_know_water_entering_soil_mm(double water_entering_soil_mm)
{  return (active_CropSyst)
   ? active_CropSyst->know_water_entering_soil_mm(water_entering_soil_mm)
   : 0.0;
}
/*_2010-11-01________________________VIC_CropSyst_know_water_entering_soil_mm_*/
#else
#endif
#ifndef CROPSYST_PROVIDED_ET
double VIC_CropSyst_update_ET_coef_full_shading()
{  double  crop_ET_coef_full_shading =
    (active_CropSyst)
      ? active_CropSyst->update_ET_coef_full_shading() : 0;
   VIC_CropSyst_get_return = crop_ET_coef_full_shading;
      // This line should not be needed but sometimes with the gcc compiler
      // C++ functions called from C code do not return the correct value.
      // I haven't been able to figure out why.
   return crop_ET_coef_full_shading;
}
/*_2012-11-29_________________________________________________________________*/
void VIC_CropSyst_know_psychrometrics
(double vapor_pressure_saturation_slope_kPa_C
,double psychrometric_constant_kPa_C
,double aerodynamic_resistance_ref_plants)                        modification_
{  if (active_CropSyst)
      active_CropSyst->know_pychrometrics
         (vapor_pressure_saturation_slope_kPa_C
         ,psychrometric_constant_kPa_C
         ,aerodynamic_resistance_ref_plants);
}
/*_2014-05-08_______________________________________________________________*/
void VIC_CropSyst_know_transpiration_max_mm(double crop_transpiration_max_mm)
{  if (active_CropSyst)
       active_CropSyst->know_transpiration_max_mm(crop_transpiration_max_mm);
}
#endif
/*_2012-11-29_________________________________________________________________*/
unsigned int VIC_CropSyst_is_perennial()
{  return (active_CropSyst && active_CropSyst->is_perennial());
}
/*_2011-05-04_______________________________________VIC_CropSyst_is_perennial_*/
unsigned long int VIC_CropSyst_start_crop
(unsigned int WSDA_crop_type_code)
{  return (active_CropSyst)
   ? (unsigned long int)active_CropSyst->start_crop
      (WSDA_crop_type_code)
   : 0;
}
/*_2011-06-14________________________________________VIC_CropSyst_start_crop__*/
int VIC_CropSyst_stop_crop()
{  return (active_CropSyst)
   ? (unsigned long int)active_CropSyst->stop_crop()
   : 0;
}
#if (VIC_CROPSYST_VERSION==2)
/*_2011-06-15__________________________________________VIC_CropSyst_stop_crop_*/
void VIC_CropSyst_know_air_temperature
(double air_temperature_maximum
,double air_temperature_minimum
)
{  if (active_CropSyst)
      active_CropSyst->know_air_temperature
         ((float64)air_temperature_maximum
         ,(float64)air_temperature_minimum);
}
/*_2010-11-22_______________________________VIC_CropSyst_know_air_temperature_*/
void VIC_CropSyst_know_solar_radiation(double daily_irradiance_MJ_m2)
{  if (active_CropSyst)
      active_CropSyst->know_solar_radiation((float64)daily_irradiance_MJ_m2);
}
#endif

/*_2011-04-13_________________________________________________________________*/
void VIC_CropSyst_know_relative_humdity(double RH_max, double RH_min)
{
/*140815 RLN doesn't appear to be used anymore in any version
   but it may be needed for other CropSyst elements in V3, so keeping the function for now
   if (active_CropSyst)
      active_CropSyst->know_relative_humdity((float64) RH_max, (float64)RH_min);
*/
}
/*_2011-04-13_________________________________________________________________*/
//windspeed is always needed to calculate transpiration adjustment #ifndef CROPSYST_PROVIDED_ET
#if (VIC_CROPSYST_VERSION==2)
void VIC_CropSyst_know_wind_speed_m_s
(double wind_speed_m_s)
{  if (active_CropSyst)
      active_CropSyst->know_wind_speed_m_s(wind_speed_m_s);
}

//#endif
/*_2010-11-22________________________________VIC_CropSyst_know_wind_speed_m_s_*/
void VIC_CropSyst_know_atmospheric_CO2_concentration
(double new_atmospheric_CO2_concentration_ppm)
{  if (active_CropSyst)
      active_CropSyst->know_atmospheric_CO2_concentration
         ((float64)new_atmospheric_CO2_concentration_ppm);
}
#endif
/*____________________________VIC_CropSyst_know_atmospheric_CO2_concentration_*/
int VIC_CropSyst_start_day(unsigned long date)
{  return (active_CropSyst) ? active_CropSyst->start_day((int32)date) :0;
}
/*_____________________________________________________VIC_CropSyst_start_day_*/
int VIC_CropSyst_end_day()
{  return (active_CropSyst) ? active_CropSyst->end_day() :0;
}
/*_2010-10-10____________________________________________VIC_CropSyst_end_day_*/
double VIC_CropSyst_harvest
(unsigned long int harvest_options_bitmask)
{  return (active_CropSyst)
   ? active_CropSyst->harvest(harvest_options_bitmask & HARVEST_TERMINATE)
   : 0.0;
}
/*_______________________________________________________VIC_CropSyst_harvest_*/
int VIC_CropSyst_process()
{
/* //140810  decided not to do this
   int result = 0;                                                               //140810
   if (active_CropSyst)
   {  if (active_CropSyst->process())
         result = active_CropSyst->get_crop_event_sequence();                     //140810
   }
   return result;                                                                //140810
*/
   return (active_CropSyst) ? active_CropSyst->process() : 0;
}
/*_______________________________________________________VIC_CropSyst_process_*/
double VIC_CropSyst_get
(unsigned long int variable_code
/*NYI ,unsigned long int units_code*/)
{  // For some unknown reason VIC_CropSyst_get is returning large integer values
   // under Linux (GNUC++). As a work around setting global VIC_CropSyst_get_return
   // which can be read after calling VIC_CropSyst_get();

   VIC_CropSyst_get_return =
   (active_CropSyst) // precondition
      ? (double)active_CropSyst->get
         ((nat32)variable_code/*NYI,(nat32)units_code*/)
      : (double)0.0;
   return VIC_CropSyst_get_return ;
}
/*____________________________________________________________________________*/
/*140810 obsolete
double VIC_CropSyst_get_uptake_mm(int horizon_number)
{
   VIC_CropSyst_get_return =
    (active_CropSyst) // precondition
      ? (double)active_CropSyst->get_uptake_mm
         ((nat8)horizon_number)
      : (double)0.0;
   return  VIC_CropSyst_get_return;
}
*/

#if (VIC_CROPSYST_VERSION>=3)
const char* getCropSystLogFileName()
{
    /*Generate log file name according global file names*/
    //assert(false);
    char logfilename[MAXSTRING];
    std::strcpy(logfilename,filenames.CropSyst_Output_dir);
    #ifdef __linux__
    std::strcat(logfilename,"/CropSyst_log.txt");
    #else
    std::strcat(logfilename,"/\CropSyst_log.txt");  //LML 141103 need double check!!
    #endif
    return logfilename;
}
#endif


/*____________________________________________________________________________*/
#if (VIC_CROPSYST_VERSION==2)
unsigned long int VIC_CropSyst_create
(char *database_directory_name
,double latitude_dec_deg
,double longitude_dec_deg
,double elevation_meter
,unsigned long int optional_submodels_bitmask
,char *label)
{
   active_CropSyst = new VIC_crop::
   #ifdef CROPSYST_PROVIDED_SOIL_HYDROLOGY
   Crop_with_weather_soil
   // Currently we are using VIC soil hydrology,
   // but we may need to revert or use CropSyst proper soil hydrology.
   #else
   Crop_with_weather
   #endif
      (database_directory_name
      ,latitude_dec_deg
      ,longitude_dec_deg
      ,elevation_meter
      ,optional_submodels_bitmask & HARVEST_AUTOMATICALLY
      ,optional_submodels_bitmask & CO2_RESPONSE_SUBMODEL
      ,false); // don't assume irrigation
   // The label is not currently used
   if (active_CropSyst && !active_CropSyst->database_valid)                      //110617_110209
   {  delete active_CropSyst;                                                    //110209
      active_CropSyst = 0;                                                       //110209
   }                                                                             //110209
   return ( unsigned long int)active_CropSyst;
}
#else
/*_LML 141024___________________________________________________________________________*/
unsigned long int VIC_CropSyst_create_V3
(crop_data_struct *_VIC_crop
 ,soil_con_struct *_VIC_soil_con
 ,veg_lib_struct *_VIC_veg_lib
 ,option_struct *_VIC_options
 ,veg_con_struct *_VIC_veg_con
 ,int _VIC_Crop_count_index)
 //,sint32                        _LBF_ID   // negative values indicate offsite
 //,CropSyst::Scenario           &_scenario_control_and_model_options
 //,const CORN::Date             &_today
 //,Common_simulation_log        &_event_log
 //,const CropSyst::Scenario_directory *_scenario_directory)
{

   const char* log_file_name = getCropSystLogFileName();
   Common_simulation_log *event_log = new Common_simulation_log(log_file_name);
   CORN::OS::Directory_name_concrete dir_name;
   const CropSyst::Scenario_directory *scenario_directory = 0;//new CropSyst::Scenario_directory(dir_name,true);
   //scenario_directory->set_str(filenames.VIC_CropSyst_scenario_dir);
   //scenario_directory

   //assert(false);           //LML 141024 provide scenario_directory name

   CropSyst::Scenario *crop_scenario = generate_scenario_for_crop_count(
               _VIC_crop
               ,_VIC_soil_con
               ,_VIC_veg_lib
               ,_VIC_options
               ,_VIC_veg_con
               ,_VIC_Crop_count_index
               );

   active_CropSyst = new VIC_crop::
   #ifdef CROPSYST_PROVIDED_SOIL_HYDROLOGY
   Crop_with_weather_soil
   // Currently we are using VIC soil hydrology,
   // but we may need to revert or use CropSyst proper soil hydrology.
   #else
   VIC_crop_land_unit_simulation
   //CropSyst::Land_unit_simulation_VX
   #endif
      (_VIC_crop
       ,_VIC_soil_con
       ,_VIC_veg_lib
       ,_VIC_options
       ,_VIC_veg_con
       ,_VIC_Crop_count_index
       ,true   // negative values indicate offsite
       ,*crop_scenario
       ,global_today
       ,*event_log
       ,scenario_directory);
   // The label is not currently used
   if (active_CropSyst && !active_CropSyst->database_valid)                      //110617_110209
   {  delete active_CropSyst;                                                    //110209
      active_CropSyst = 0;                                                       //110209
   }                                                                             //110209
   return ( unsigned long int)active_CropSyst;

}
#endif
/*___LML 141024_________________________________________________________________________*/
CropSyst::Scenario *generate_scenario_for_crop_count
(crop_data_struct *_VIC_crop
 ,soil_con_struct *_VIC_soil_con
 ,veg_lib_struct *_VIC_veg_lib
 ,option_struct *_VIC_options
 ,veg_con_struct *_VIC_veg_con
 ,int _VIC_Crop_count_index)
{
    /*Generate the scenario data structure for each cropping systems in each grid cell.
     *Each has its own rotation types and initial C & N pools.
    */
    CropSyst::Scenario *cs = 0;//new CropSyst::Scenario;
    return cs;
    //assert(false);  //LML NIY
}
/*____________________________________________________________________________*/
unsigned long int VIC_CropSyst_object_activate
(unsigned long int crop_handle)
{  if (crop_handle)
   {
      #ifdef CROPSYST_PROVIDED_SOIL_HYDROLOGY
      Crop_with_weather_soil *activating_CropSyst
         = (Crop_with_weather_soil *)crop_handle;
      #else
      #if (VIC_CROPSYST_VERSION>=3)
      VIC_crop_land_unit_simulation *activating_CropSyst
           = (VIC_crop_land_unit_simulation *)crop_handle;
      #else
      Crop_with_weather *activating_CropSyst
         = (Crop_with_weather *)crop_handle;
      #endif
      #endif
         active_CropSyst = activating_CropSyst;
   }
   return (unsigned long int)active_CropSyst;
}
/*____________________________________________________________________________*/
unsigned long int VIC_CropSyst_object_delete
(unsigned long int crop_handle)
{  unsigned long int result = 0;
   if (crop_handle)
   {
      #ifdef CROPSYST_PROVIDED_SOIL_HYDROLOGY
      Crop_with_weather_soil *deleting_CropSyst
         = (Crop_with_weather_soil *)crop_handle;
      #else
      #if (VIC_CROPSYST_VERSION>=3)
      VIC_crop_land_unit_simulation *deleting_CropSyst
          = (VIC_crop_land_unit_simulation *)crop_handle;
      #else
      Crop_with_weather *deleting_CropSyst
         = (Crop_with_weather *)crop_handle;
      #endif
      #endif
      if (deleting_CropSyst == active_CropSyst)
         active_CropSyst = 0;
      {  delete deleting_CropSyst;
         result = 1;
      }
   }
   return result;
}
/*____________________________________________________________________________*/
#endif


