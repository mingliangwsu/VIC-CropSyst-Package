#include <options.h>
#include "crop/crop_param.h"
#include "crop/crop_common.h"
#include "soil/soil_I.h"
#include "soil/hydrology_I.h"
#include "crop/growth_stages.hpp"
#include "crop/transpiration.h"
#include <math.h>
#include "corn/math/compare.h"
#include "crop/transpiration.h"
#include "corn/measure/measures.h"
#include "common/weather/parameter/WC_vapor_pressure_deficit.h"
#include "crop/CO2_response.h"

// For inspection:
#include "CS_suite/observation/CS_inspector.h"
#include "csvc.h"
#include "csvc_special.h"
#define CSVP_crop_base_transp_pot_CO2_adjusted                          0x21FFFA

#include "mgmt_param.h"
#include "phenology_I.h"

namespace CropSyst {

#ifdef DEBUG_INTERCEPTION
ofstream debug_interception("debug_interception.dat");
#endif
//______________________________________________________________________________
Crop_common::Crop_common
(Crop_parameters                       *parameters_
,bool                                   owns_parameters_
,const Vapor_pressure_deficit_daytime  &daytime_vapor_pressure_deficit_          //140812
,Soil::Soil_interface                  *soil_                                    //160415
,modifiable_ Crop_CO2_response         *Crop_CO2_response_given_optional         //160418
,const CORN::date32                    &today_                                   //170525
,CORN::Unidirectional_list             &performable_operations_)
: Crop_element                         (today_)                                  //171212
, parameters                           (parameters_)
, owns_parameters                      (owns_parameters_)
, soil                                 (soil_)                                   //160415
, daytime_vapor_pressure_deficit       (daytime_vapor_pressure_deficit_)         //140812
, CO2_response                         (Crop_CO2_response_given_optional)        //160418
, intercepted_precipitation_m_today    (0)                                       //160416
, intercepted_precipitation_m_stored   (0)
// transpiration
, ET_ref                               (0)                                       //171031
, act_transpiration_m                  (0)                                       //171031
, transpiration                        (0)
, water_stress_stats(STAT_arithmetic_mean_bit)                                   //160803
{
   delete event_scheduler;                                                       //151127
   event_scheduler = new Event_scheduler_crop(simdate,performable_operations_);  //151127
   pot_transpiration_m_CO2_adj = pot_transpiration_m_unadj = 0;                  //190826
}
//_Crop_common:constructor__________________________________________2004-01-21_/
Crop_common::~Crop_common()
{
// this is crashing   delete transpiration; transpiration = 0;                   //141103
   parameters = 0;
   delete transpiration;                                                         //201129LML
}
//_~Crop_common__________________________________________2004-01-21_2003-07-10_/
bool Crop_common::initialize()                                   initialization_
{  return Crop_element::initialize();
}
//_initialize_______________________________________________________2019-07-21_/
bool Crop_common::clear_day()                                      modification_
{
   act_transpiration_m = 0;                                                      //190707
   reported.daily.transpiration_act_m = 0.0;                                     //190707
   intercepted_precipitation_m_today = 0;                                        //160414
   return true;
}
//_clear_day________________________________________________________2002-04-13_/
bool Crop_common::start_day()                                      modification_ //150930
{  bool started = true;
   /* Dont clear these because they are setup in update_evapotranspiration_max
    before the call to this but could put them in end_day
   pot_transpiration_m_CO2_adj = pot_transpiration_m_unadj = 0;                  //190826
   pot_transpiration_mX
   */
   started &= CS::Simulation_element_composite::start_day();
   return started;
}
//_start_day________________________________________________________2005-10-28_/
float64 Crop_common::update_evapotranspiration_max
(float64 ET_ref_)                                                 rectification_
{  ET_ref = ET_ref_;  // meter
   //200127 now member for output float64
   adjusted_ET_coef = update_adjusted_ET_crop_coefficient();                     //200127_080811
      //200127 was get
   /*190411 since Jordi has passed on, these will probably never be used again
   // This is a special test for Jordi Marsal for fruits, this method should
   //  be reimplemented with this in Crop_fruit if it is decided to keep it.
   if (!CORN::is_zero<float64>(parameters->transpiration.a2)) //121115
   {  float64 vapor_pressure_deficit_daytime_avg
      = daytime_vapor_pressure_deficit.get_kPa();//140812
      adjusted_ET_coef += parameters->transpiration.a1
         + parameters->transpiration.a2 * vapor_pressure_deficit_daytime_avg;
   }
   */
   float64 ET_max_m = ET_ref * adjusted_ET_coef;

   // If there is no crop adjusted_ET_coef should be 1 not near the parameter crop coef
   // ranges between 1 and ~1.2

   if (ET_max_m < 0.0)  ET_max_m = 0.0;

   float64 green_canopy_interception = get_canopy_interception_global_green();         //200228
   float64 pot_transpiration_m_unadj_local = (green_canopy_interception * ET_max_m);   //051028
   pot_transpiration_m_CO2_adj = pot_transpiration_m_unadj = pot_transpiration_m_unadj_local; //190826
   // ET_pot_m is actually more like potential atmospheric demand for ET???      180122
   reported.daily.ET_max_m = ET_max_m;                                           //190705_171031

/*
   Continue here add this output

   float64 total_canopy_interception = get_canopy_interception_global_total();    //200626
   reported.daily.heating_energy = ET_max_m * (total_canopy_interception - green_canopy_interception); //200626
*/
   return ET_max_m;
}
//_update_pot_evapotranspiration____________________________________2001-09-10_/
float64 Crop_common::process_transpiration_m
( float64 &limited_pot_transpiration)                              modification_
{
   soil_sublayer_array_64(water_uptake_demand);                                  //010724
   clear_sublayer_array(water_uptake_demand);                                    //010724
   float64 water_limited_act_transpiration = 0.0;                                //011118
   if (transpiration)                                                            //191011
      // wouldnt be transpiring unless there was canopy.                         //190707
      // transpiration is deleted at maturity                                    //191011


   //190826  Not sure why transpiration is stopped at maturity
   // wouldnt it continue has long as there is canopy leaf?
   // I think there would only be certain crops this would apply to.
   {
      {                                                                          //080205
         float64 water_uptake_max_mm =
            #if (CROPSYST_VERSION == 4)
            m_to_mm(parameters->param_max_water_uptake_m
                (get_growth_stage_sequence()));                                  //130902
            #else
            parameters->transpiration.normal_max_water_uptake_mm;
            //130406  Warning we need to make sure that
            // normal_max_water_uptake_mm is updated dynamically as needed
            // especially for fruit trees
            #endif
         float64 interception_green = get_canopy_interception_global_green();
         act_transpiration_m =                                                   //011022
           transpiration->calc_transpiration                                     //050331
               (limited_pot_transpiration
               ,interception_green                                               //081105
               ,water_uptake_max_mm                                              //130406_091222
               ,intercepted_precipitation_m_stored                               //160414
               ,get_total_fract_root_length()                                    //050331
               ,water_uptake_demand
               , CO2_response
                ?CO2_response->get_transpiration_adjustment_factor() : 1.0);     //160418
      }
      reported.daily.transpiration_act_m                                         //190705
         = water_limited_act_transpiration =
            CORN::must_be_0_or_greater<float64>(act_transpiration_m);            //011022
   }
   else
   {  act_transpiration_m = 0.0;                                                 //000323
      reported.daily.transpiration_act_m                                         //190705
      = water_limited_act_transpiration = 0.0;                                   //011117
   }
   float64 water_stress = (limited_pot_transpiration > 0.0)                      //011117
      ? water_limited_act_transpiration
         / limited_pot_transpiration                                             //011117
      : 1.0;
   water_stress_stats.append((float32)water_stress);                             //011023


   //if ((1-water_stress) > 0.05)
   //    printf("water_stress:%lf water_limited_act_transpiration:%lf limited_pot_transpiration:%lf \n",
   //       (1-water_stress),water_limited_act_transpiration * 1000.,limited_pot_transpiration * 1000.);




   /* NYI if water_stress
   canopy_warming_equiv_water_depth =  limited_pot_transpiration
      - water_limited_act_transpiration;
   This would be an output column in the report (we dont do anything with this)
   */
   float64 act_extracted_unused = // the result not specifically used            //051013
   (soil)                                                                        //020711
   ? soil->mod_hydrology()->extract_water(water_uptake_demand,2)                 //160412_010724
   : act_transpiration_m;
   // act_transpiration_m is the case for crop calibrator
   // (or when soil is not a limitation).                                        //060615
   reported.daily.transpiration_act_m = act_transpiration_m;                     //190705_051014
   return act_transpiration_m;                                                   //190705_031201
}
//_process_transpiration_m_____________________________________________________/
float64 Crop_common::intercept_precipitation(float64 water_reaching_plant)
{  // water_reaching_plant in meters
   float64 water_leaving_plant = 0.0;
   float64 intercepted_precipitation_m_old = intercepted_precipitation_m_stored;
   float64 on_plant = water_reaching_plant + intercepted_precipitation_m_stored;
   float64 max_store = 0.001 * get_canopy_interception_global_total();           //200228 get_fract_canopy_cover_total();
   if (on_plant > max_store)
   {  intercepted_precipitation_m_stored = max_store;
      water_leaving_plant = on_plant - max_store;
   }
   else intercepted_precipitation_m_stored =  on_plant;
   intercepted_precipitation_m_today
      = intercepted_precipitation_m_stored - intercepted_precipitation_m_old;
   return  water_leaving_plant;
}
//_intercept_precipitation_______________________________2016-04-14_2014-03-20_/
bool Crop_common::get_name(modifiable_ std::wstring &active_crop_name)     const
{ active_crop_name = parameters->name; return active_crop_name.length() != 0; }  //150720
//______________________________________________________________________________
RENDER_INSPECTORS_DEFINITION(Crop_common)
{
   #ifdef CS_OBSERVATION
   KNOW_EMANATOR(simdate,true);                                                  //171114
   emanator_known = &emanator;                                                   //160613

   inspectors.append(new CS::Inspector_scalar(pot_transpiration_m_unadj          ,UC_meter,  emanator,"transpiration/potential/unadjusted"   ,value_statistic,CSVP_crop_base_transp_pot              /*171222 CSVC_crop_base_transp_pot_time_step_sum*/));
   inspectors.append(new CS::Inspector_scalar(pot_transpiration_m_CO2_adj        ,UC_meter,  emanator,"transpiration/potential/CO2_adjusted" ,value_statistic,CSVP_crop_base_transp_pot_CO2_adjusted /*171222 CSVC_crop_base_transp_pot_CO2_adjusted_time_step_sum*/));
   inspectors.append(new CS::Inspector_scalar(reported.daily.transpiration_act_m ,UC_meter,  emanator,"transpiration/actual"                 ,value_statistic,CSVC_crop_base_transp_act_time_step_sum)); //190705
   inspectors.append(new CS::Inspector_scalar(water_stress_stats.ref_current()   ,UC_factor, emanator,"water/stress/factor"                  ,value_statistic,CSVP_crop_base_water_stress_factor));

   inspectors.append(new CS::Inspector_scalar(water_stress_stats.index           ,UC_index  ,emanator,"water/stress/index"                   ,value_statistic,CSVP_crop_base_water_stress_index)); //check ontology
   //inspectors.append(new CS::Inspector_scalar(water_stress_index_test        ,UC_index  ,emanator,/*check ontology*/"water/stress/index"   ,value_statistic,CSVP_crop_base_water_stress_index));

   inspectors.append(new CS::Inspector_scalar(adjusted_ET_coef                   ,UC_coefficient,emanator,"transpiration/coef/adjusted"      ,value_statistic,CSVC_crop_transp_coef_adj)); //200128

   return emanator_known;
   #else
   return 0;
   #endif
}
//_RENDER_INSPECTORS_DEFINITION_____________________________________2013-07-30_/
}//_namespace CropSyst_________________________________________________________/

