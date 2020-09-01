
//#error  Remove crop_param_V5.cpp from your project instead use crop_param_V4.cpp

// I have reverted to V4 crop parameter classes because I don't quite have all parameters ready in V5.
// In any case V5 I intend to use the new YAML format files in V5.

#include "crop_param_V5.h"
#include "corn/data_source/datarec.h"
//---------------------------------------------------------------------------
//  Parameter_properties XXX_param_props = {"key",field_width,"caption","hint","%PROG_HOME%:path/document.HTM#anchor",};  Parameter_number_properties XXX_param_props_numeric = {_units_code,"units_units_or_NULL",_min_warning,_max_warning,_min_error,_max_error,optional_precision,optional_radix};
//
//Note that Field widths are as used in LADSS
//if the field width has /*9!*/ 9 is the field with used in LADSS but this is not adequate
// - indicates not yet implemented in LADSS
// / indicates not needed in LADSS
// ? indicates not sure what field width to use.
#define NO_HINT 0
#define NoAltFieldName 0
#define NotYetSubsection "NotYetSubsection"
#include "static_phrases.h"

#ifdef LADSS_MODE
extern bool LADSS_mode;
#endif
using namespace CORN;
//______________________________________________________________________________
namespace CropSyst                                                               ///110208
{
#include "crop/crop_param_properties.c_cpp"
//______________________________________________________________________________
Crop_parameters::Thermal_time::Thermal_time()
: Crop_parameters_class::Thermal_time()
   // deprecated can be removed once the V4 crop editor is update
,  p_base_temperature                    (temperature.base     ,PP_base_temperature    ,PPR_base_temperature   ,PPF_F) // PPF_base_temperature)
,  p_cutoff_temperature                  (temperature.cutoff   ,PP_cutoff_temperature  ,PPR_cutoff_temperature ,PPF_F) // PPF_cutoff_temperature)
,  p_opt_temperature                     (temperature.opt      ,PP_opt_temperature     ,PPR_opt_temperature    ,PPF_F) // PPF_opt_temperature)
,  p_max_temperature                     (temperature.max      ,PP_max_temperature     ,PPR_max_temperature    ,PPF_F) // PPF_max_temperature)
{}
//_Thermal_time:constructor_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ __/
bool Crop_parameters::Thermal_time::setup_structure
(Data_record &record,bool for_write)                       modification_
{
   bool satup = Crop_parameters_class::Thermal_time::setup_structure(record,for_write);
   record.expect_parameter(p_base_temperature);                                  //021124
   record.expect_parameter(p_cutoff_temperature);                                //021124
   record.expect_parameter(p_opt_temperature);                                   //021124
   record.expect_parameter(p_max_temperature);                                   //021124
   record.set_current_section("phenology"); // Note in V5 this is moved to thermal_time
   return satup;
}
//_Thermal_time::setup_structure____________________________________2010-05-13_/
Crop_parameters::Transpiration::Transpiration()
: Crop_parameters_class::Transpiration()
, p_ET_coef             (ET_coef                   ,PP_ET_coef                            ,PPR_ET_coef                        ,PPF_F) // PPF_ET_coef)
, p_max_water_uptake    (normal_max_water_uptake_mm,PP_max_water_uptake                   ,PPR_max_water_uptake               ,PPF_F) // PPF_max_water_uptake)
, p_stomatal_closure_LWP(LWP_stomatal_closure      ,PP_stomatal_closure_leaf_water_pot    ,PPR_stomatal_closure_leaf_water_pot,PPF_F) // PPF_stomatal_closure_leaf_water_pot)
, p_wilt_LWP            (LWP_wilt_point            ,PP_wilt_LWP                           ,PPR_LWP                            ,PPF_F) // PPF_LWP)
{}
//_Transpiration:constructor________________________________________2011-02-08_/
bool Crop_parameters::Transpiration::setup_structure(Data_record &record,bool for_write)    modification_
{
   bool satup = Crop_parameters_class::Transpiration::setup_structure(record,for_write);
   // [transpiration]
   record.expect_parameter(p_ET_coef);
   record.expect_parameter(p_max_water_uptake);
   record.expect_parameter(p_stomatal_closure_LWP); // leaf water pot at onset of stomatal closure
   record.expect_parameter(p_wilt_LWP);
   return satup;
}
//_Transpiration::setup_structure___________________________________2011-02-08_/
Crop_parameters::Biomass_production::Biomass_production()
: Crop_parameters_class::Biomass_production()
//131101 now only V4 , p_early_growth_limit_temp               (early_growth_limit_temp         ,PP_early_growth_limit_temp            ,PPN_early_growth_limit_temp,PPN_early_growth_limit_temp)
#ifndef YAML_PARAM_VERSION
, p_season_biomass_adjustment_2           (season_biomass_adjustment[2]    ,PP_season_biomass_adjustment_2        ,PPR_season_biomass_adjustment,PPF_F) // PPF_season_biomass_adjustment)
, p_season_biomass_adjustment_3           (season_biomass_adjustment[3]    ,PP_season_biomass_adjustment_3        ,PPR_season_biomass_adjustment,PPF_F) // PPF_season_biomass_adjustment)
, p_season_biomass_adjustment_4           (season_biomass_adjustment[4]    ,PP_season_biomass_adjustment_4        ,PPR_season_biomass_adjustment,PPF_F) // PPF_season_biomass_adjustment)
, p_season_biomass_adjustment_5           (season_biomass_adjustment[5]    ,PP_season_biomass_adjustment_5        ,PPR_season_biomass_adjustment,PPF_F) // PPF_season_biomass_adjustment)
#endif
#ifndef YAML_PARAM_VERSION
, p_TUE_at_1kPa_VPD                       (TUE_at_1kPa_VPD              [0],PP_TUE_at_1kPa_VPD                    ,PPR_TUE_at_1kPa_VPD,            PPF_F) // PPF_TUE_at_1kPa_VPD)
, p_TUE_at_1kPa_VPD_vegetative            (TUE_at_1kPa_VPD              [1],PP_TUE_at_1kPa_VPD_vegetative         ,PPR_TUE_at_1kPa_VPD,            PPF_F) // PPF_TUE_at_1kPa_VPD)
, p_TUE_scaling_coef                      (TUE_scaling_coef             [0],PP_TUE_scaling_coef                   ,PPR_TUE_scaling_coef,           PPF_F) // PPF_TUE_scaling_coef)
, p_TUE_scaling_coef_vegetative           (TUE_scaling_coef             [1],PP_TUE_scaling_coef_vegetative        ,PPR_TUE_scaling_coef,           PPF_F) // PPF_TUE_scaling_coef)
/*191026 AquaCrop and TannerSinclair now obsolete
, p_water_productivity                    (water_productivity           [0],PP_water_productivity                 ,PPR_water_productivity,         PPF_F) // PPF_water_productivity)
, p_water_productivity_vegetative         (water_productivity           [1],PP_water_productivity_vegetative      ,PPR_water_productivity,         PPF_F) // PPF_water_productivity)
, p_biomass_transpiration_coef            (reference_biomass_transpiration_coef  [0],PP_biomass_transpiration_coef,PPR_biomass_transpiration_coef, PPF_F) // PPF_biomass_transpiration_coef)
, p_biomass_transpiration_coef_vegetative (reference_biomass_transpiration_coef  [1],PP_biomass_transpiration_coef_vegetative,PPR_biomass_transpiration_coef,PPF_F) // PPF_biomass_transpiration_coef)
*/


// compiler complais
, p_RUE_at_low_VPD               (RUE[at_low_VPD]            ,PP_RUE_at_low_VPD              ,PPR_RUE_global,PPF_F) // PPF_RUE_global)  //191026
, p_RUE_at_low_total_solar_rad   (RUE[at_low_total_solar_rad],PP_RUE_at_low_total_solar_rad  ,PPR_RUE_global,PPF_F) // PPF_RUE_global)  //191026
, p_RUE_efficiency_decrease_rate (RUE_efficiency_decrease_rate  ,PP_RUE_efficiency_decrease_rate  ,PPR_RUE_efficiency_decrease_rate,PPF_F) // PPF_RUE_efficiency_decrease_rate) //191026

//191026 , p_RUE_global                            (RUE_global                      ,PP_RUE_global ,PPR_RUE_global,PPF_RUE_global)
/*171219 now only using global basis
, p_RUE_PAR_deprecated                               (RUE_PAR_deprecated                         ,PP_RUE_PAR    ,PPR_RUE_PAR   ,PPF_RUE_PAR)
*/
#endif
/*
Parameter_properties_literal PP_min_limitation_temp     ={"min" ,NoAltFieldName,NotYetSubsection,"Minimum"   ,"for temperature limitation"              ,"biomass.htm",UC_Celcius,TU_degrees_C}; Parameter_range  PPR_min_tolerable_temp         = { 0.0,10.0, 0.0,15.0}; Parameter_format PPF_min_tolerable_temp        ={4,1};
Parameter_properties_literal PP_low_limitation_temp     ={"low" ,NoAltFieldName,NotYetSubsection,"Low"       ,"optimum range for temperature limitation","biomass.htm",UC_Celcius,TU_degrees_C}; Parameter_range  PPR_low_threshold_limit_temp   = { 8.0,14.0, 5.0,20.0}; Parameter_format PPF_low_threshold_limit_temp  ={4,1};
Parameter_properties_literal PP_high_limitation_temp    ={"high",NoAltFieldName,NotYetSubsection,"High"      ,"optimum range for temperature limitation","biomass.htm",UC_Celcius,TU_degrees_C}; Parameter_range  PPR_high_threshold_limit_temp  = {20.0,30.0,15.0,35.0}; Parameter_format PPF_high_threshold_limit_temp ={4,1};
Parameter_properties_literal PP_max_limitation_temp     ={"max" ,NoAltFieldName,NotYetSubsection,"Maximum"   ,"temperature limitation"                  ,"biomass.htm",UC_Celcius,TU_degrees_C}; Parameter_range  PPR_max_tolerable_temp         = {40.0,50.0,30.0,55.0}; Parameter_format PPF_max_tolerable_temp        ={4,1};
*/

, p_temp_limit_min   (temperature_limitation.min         ,PP_temp_limit_min   ,PPR_temp_limit_min  ,PPF_F) // PPF_temp_limit_min)
, p_temp_limit_low   (temperature_limitation.low_optimum ,PP_temp_limit_low   ,PPR_temp_limit_low  ,PPF_F) // PPF_temp_limit_low)
, p_temp_limit_high  (temperature_limitation.high_optimum,PP_temp_limit_high  ,PPR_temp_limit_high ,PPF_F) // PPF_temp_limit_high)
, p_temp_limit_max   (temperature_limitation.max         ,PP_temp_limit_max   ,PPR_temp_limit_max  ,PPF_F) // PPF_temp_limit_max)
/*191026
, p_min_tolerable_temp        (min_tolerable_temp        ,PP_min_tolerable_temp        ,PPR_min_tolerable_temp       ,PPF_min_tolerable_temp )
, p_max_tolerable_temp        (max_tolerable_temp        ,PP_max_tolerable_temp        ,PPR_max_tolerable_temp       ,PPF_max_tolerable_temp )
, p_low_threshold_limit_temp  (low_threshold_limit_temp  ,PP_low_threshold_limit_temp  ,PPR_low_threshold_limit_temp ,PPF_low_threshold_limit_temp )
, p_high_threshold_limit_temp (high_threshold_limit_temp ,PP_high_threshold_limit_temp ,PPR_high_threshold_limit_temp,PPF_high_threshold_limit_temp )
*/
{}
//_Biomass_production:constructor___________________________________2011-02-08_/
bool Crop_parameters::Biomass_production::setup_structure(Data_record &record,bool for_write)    modification_
{
   bool satup =Crop_parameters_class::Biomass_production::setup_structure(record,for_write);
   // [growth]

#ifndef YAML_PARAM_VERSION
   record.expect_parameter(p_TUE_scaling_coef);                                  //080505
   record.expect_parameter(p_TUE_scaling_coef_vegetative);                       //110224
   {  record.expect_parameter(p_TUE_at_1kPa_VPD);                                //080506
      record.expect_parameter(p_TUE_at_1kPa_VPD_vegetative);                     //080506
   }
   /*191025 AquaCrop and TannerSinclair now obsoleted
   {  record.expect_parameter(p_water_productivity);                             //080505
      record.expect_parameter(p_water_productivity_vegetative);                  //080505
   }

   {  record.expect_parameter(p_biomass_transpiration_coef);                     //040729
      record.expect_parameter(p_biomass_transpiration_coef_vegetative);          //040729
   }
   */
   record.expect_parameter(p_RUE_at_low_VPD);                                    //191026
   record.expect_parameter(p_RUE_at_low_total_solar_rad);                        //191026
   record.expect_parameter(p_RUE_efficiency_decrease_rate);                      //191026
   if (!for_write)
      record.expect_float32("RUE_global",RUE[at_low_VPD]);                       //191031

#endif
      record.expect_parameter(p_temp_limit_min);
      record.expect_parameter(p_temp_limit_low);
      record.expect_parameter(p_temp_limit_high);
      record.expect_parameter(p_temp_limit_max);

      if (!for_write)
      { // 191026 renamed support of old files (the have not been renamed).
      record.expect_float32("min_tolerable_temp"            ,temperature_limitation.min           );
      record.expect_float32("low_threshold_limit_temp"      ,temperature_limitation.low_optimum   );
      record.expect_float32("high_threshold_limit_temp"     ,temperature_limitation.high_optimum  );
      record.expect_float32("max_tolerable_temp"            ,temperature_limitation.max           );
      }
      /*191026 obsolete
      record.expect_parameter(p_min_tolerable_temp);
      record.expect_parameter(p_max_tolerable_temp);
      record.expect_parameter(p_low_threshold_limit_temp);
      record.expect_parameter(p_high_threshold_limit_temp);
      */
   return satup;
}
//_Biomass_production::setup_structure______________________________2011-02-08_/
Crop_parameters::Phenology::Phenology()
: Crop_parameters_class::Phenology()
, p_emergence_deg_day                   (initiation.emergence                ,PP_emergence_deg_day                   ,PPR_emergence_deg_day                 ,PPF_F) // PPF_emergence_deg_day)
, p_tuber_initiation_deg_day            (initiation.tuberization             ,PP_tuber_initiation_deg_day            ,PPR_tuber_initiation_deg_day          ,PPF_F) // PPF_tuber_initiation_deg_day)
//200125 obsolete , p_max_canopy_deg_day_obsolete         (culmination_obsolete.accrescence            ,PP_max_canopy_deg_day                  ,PPR_max_canopy_deg_day                ,PPF_max_canopy_deg_day)

, p_canopy_accrescence_duration         (duration.accrescence                ,PP_canopy_accrescence_duration         ,PPR_canopy_accrescence_duration       ,PPF_F) // PPF_canopy_accrescence_duration ) //190627
//200125 obsolete, p_initiation_canopy_senescence_obsolete(initiation.senescence              ,PP_canopy_senescence_initiation        ,PPR_canopy_senescence_initiation      ,PPF_canopy_senescence_initiation) //190721
, p_duration_canopy_senescence          (duration.senescence                 ,PP_canopy_senescence_duration          ,PPR_canopy_senescence_duration        ,PPF_F) // PPF_canopy_senescence_duration)//190721
, p_duration_canopy_culminescence       (duration.culminescence              ,PP_culminescence_duration               ,PPR_culminescence_duration     ,PPF_F) // PPF_culiminescence_duration) //191121
//191121 , p_max_root_depth_deg_day_obsolete     (culmination_obsolete.root_elongation         ,PP_max_root_depth_deg_day              ,PPR_max_root_depth_deg_day            ,PPF_max_root_depth_deg_day)
, p_root_elongation_deg_day             (duration.root_elongation            ,PP_root_elongation_deg_day,PPR_root_elongation_deg_day,PPF_F) // PPF_root_elongation_deg_day) //191121
, p_begin_flowering_deg_day             (initiation.flowering                ,PP_begin_flowering_deg_day             ,PPR_begin_flowering_deg_day           ,PPF_F) // PPF_begin_flowering_deg_day)
, p_begin_filling_deg_day               (initiation.filling                  ,PP_begin_filling_deg_day               ,PPR_begin_filling_deg_day             ,PPF_F) // PPF_begin_filling_deg_day)
, p_begin_maturity_deg_day              (initiation.maturity                 ,PP_begin_maturity_deg_day              ,PPR_begin_maturity_deg_day            ,PPF_F) // PPF_begin_maturity_deg_day)
, p_begin_rapid_fruit_growth_deg_day    (initiation.rapid_fruit_growth       ,PP_begin_rapid_fruit_growth_deg_day    ,PPR_begin_rapid_fruit_growth_deg_day  ,PPF_F) // PPF_begin_rapid_fruit_growth_deg_day)
#ifndef YAML_PARAM_VERSION
, p_WUE_change_deg_day                  (WUE_change                          ,PP_WUE_change_deg_day                  ,PPR_WUE_change_deg_day                ,PPF_F) // PPF_WUE_change_deg_day)
#endif
, p_phenologic_sensitivity_water_stress (phenologic_sensitivity_water_stress ,PP_phenologic_sensitivity_water_stress ,PPR_phenologic_sensitivity_water_stress,PPF_F) // PPF_phenologic_sensitivity_water_stress)
{}
//_Phenology:constructor_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _/
bool Crop_parameters::Phenology::setup_structure(Data_record &record,bool for_write)    modification_
{  bool satup = Crop_parameters_class::Phenology::setup_structure(record,for_write); //020408
   record.expect_parameter(p_emergence_deg_day);
   record.expect_parameter(p_begin_flowering_deg_day);

   if (!for_write)
      record.expect_float32("peak_LAI",culmination_obsolete.accrescence);        //200125
   //200125   record.expect_parameter(p_max_canopy_deg_day_obsolete);            //131101

   record.expect_parameter(p_canopy_accrescence_duration);                       //190627


//NYI/*020405_*/    if (!CROPSYST_FRUIT || UNKNOWN|| !for_write)
   {
      //191121 record.expect_parameter(p_max_root_depth_deg_day);                //070321
      //191121 record.expect_float32(max_root_depth_deg_day_obsolete);           //191121
   }
//NYI/*_______*/    if (TUBER_ROOT_OR_UNKNOWN || !for_write)
        record. expect_parameter(p_tuber_initiation_deg_day);
   record.expect_parameter(p_begin_filling_deg_day);
//NYI/*020405_*/    if (CROPSYST_FRUIT_OR_UNKNOWN|| !for_write)
   record.expect_parameter(p_begin_rapid_fruit_growth_deg_day);                  //050719
   record.expect_parameter(p_begin_maturity_deg_day);                            //050628
   record.expect_parameter(p_phenologic_sensitivity_water_stress);
   #ifndef YAML_PARAM_VERSION
   record.expect_parameter(p_WUE_change_deg_day);                                //080514
   #endif

   if (!for_write)                                                               //200125
      record.expect_float32("initiation.senescence",initiation.senescence);      //200125
      //200125record.expect_parameter(p_initiation_canopy_senescence_obsolete);  //080725

   record.expect_parameter(p_duration_canopy_senescence);                        //131024
   record.expect_parameter(p_duration_canopy_culminescence);                     //191121
   record.expect_parameter(p_duration_canopy_senescence);                        //200603
   return satup;
}
//_Phenology::setup_structure_______________________________________2010-05-13_/
Crop_parameters::Vernalization::Vernalization()
: Crop_parameters_class::Vernalization()
,p_low_temp   (low_temp  ,PP_vernalization_low_temp   ,PPR_vernalization_low_temp   ,PPF_F) // PPF_vernalization_low_temp)
,p_high_temp  (high_temp ,PP_vernalization_high_temp  ,PPR_vernalization_high_temp  ,PPF_F) // PPF_vernalization_high_temp)
,p_start      (start     ,PP_vernalization_start      ,PPR_vernalization_start      ,PPF_F) // PPF_vernalization_start)
,p_end        (end       ,PP_vernalization_end        ,PPR_vernalization_end        ,PPF_F) // PPF_vernalization_end)
,p_min_factor (min_factor,PP_vernalization_min_factor ,PPR_vernalization_min_factor ,PPF_F) // PPF_vernalization_min_factor)
{}
//_Vernalization:constructor_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _/
bool Crop_parameters::Vernalization::setup_structure(Data_record &record,bool for_write)    modification_
{  bool satup = Crop_parameters_class::Vernalization::setup_structure(record,for_write); //020408
   record.expect_parameter(p_low_temp);
   record.expect_parameter(p_high_temp);
   record.expect_parameter(p_start);
   record.expect_parameter(p_end);
   record.expect_parameter(p_min_factor);
   return satup;
}
//_Vernalization::setup_structure___________________________________2010-05-13_/
Crop_parameters::Photoperiod::Photoperiod()
: Crop_parameters_class::Photoperiod()
,p_stop           (stop_day_length          ,PP_photoperiod_stop          ,PPR_photoperiod_start_and_end,PPF_F) // PPF_photoperiod_start_and_end)
,p_unconstrain    (unconstrained_day_length ,PP_photoperiod_unconstrained ,PPR_photoperiod_start_and_end,PPF_F) // PPF_photoperiod_start_and_end)
{}
//_Photoperiod:constructor__________________________________________2010-05-12_/
bool Crop_parameters::Photoperiod::setup_structure(Data_record &record,bool for_write)    modification_
{  bool satup = Crop_parameters_class::Photoperiod::setup_structure(record,for_write); //020408
   record.expect_parameter(p_stop);                                              //100513
   record.expect_parameter(p_unconstrain);                                       //100513
   return satup;
}
//_Photoperiod::setup_structure_____________________________________2010-05-13_/
Crop_parameters::Emergence_hydro_thermal::Emergence_hydro_thermal()
: Crop_parameters_class::Emergence_hydro_thermal()
, p_seedling_leaf_area           (seedling_leaf_area_cm2          ,PP_seedling_leaf_area           ,PPR_seedling_leaf_area          ,PPF_F) // PPF_seedling_leaf_area)
, p_germination_base_temperature (germination_base_temperature    ,PP_germination_base_temperature ,PPR_germination_base_temperature,PPF_F) // PPF_germination_base_temperature)
, p_mean_base_soil_water_pot     (mean_base_soil_water_pot_J_kg   ,PP_mean_base_soil_water_pot     ,PPR_mean_base_soil_water_pot    ,PPF_F) // PPF_mean_base_soil_water_pot)
, p_stdev_base_soil_water_pot    (stdev_base_soil_water_pot_J_kg  ,PP_stdev_base_soil_water_pot    ,PPR_stdev_base_soil_water_pot   ,PPF_F) // PPF_stdev_base_soil_water_pot)
, p_a                            (a_slope                         ,PP_a,PPR_a,PPF_F) // PPF_a)
, p_b                            (b_intercept                     ,PP_b,PPR_b,PPF_F) // PPF_b)
{}
//_Emergence_hydro_thermal:constructor______________________________2004-12-01_/
bool Crop_parameters::Emergence_hydro_thermal
::setup_structure(Data_record &record,bool for_write)      modification_
{
   bool satup = Crop_parameters_class::Emergence_hydro_thermal::setup_structure(record,for_write);
   record.expect_parameter(p_a);                            // Emperical constant for computing  hydrothermal time
   record.expect_parameter(p_b);                            // Emperical constant for computing  hydrothermal time
   record.expect_parameter(p_seedling_leaf_area);           // leaf area of seedling on first day of emergence in cm2
   record.expect_parameter(p_germination_base_temperature); // degress 0
   record.expect_parameter(p_mean_base_soil_water_pot);     // J/kg Mean base soil water potential (50% emergence)
   record.expect_parameter(p_stdev_base_soil_water_pot);    // J/kg Std deviation base soil water potential (?for 50% emergence?)
   return satup;
}
//_Emergence_hydro_thermal::setup_structure_________________________2004-12-03_/
Crop_parameters::Quiescence::Quiescence()
: Crop_parameters_class::Quiescence()
, p_consider_inactive_days  (consider_inactive_days,PP_consider_inactive_days ,PPR_consider_inactive_days,PPF_N) // PPF_consider_inactive_days)
, p_inducement_temperature  (inducement_temperature,PP_inducement_temperature ,PPR_inducement_temperature,PPF_F) // PPF_inducement_temperature)
{}
//_Quiescence:constructor___________________________________________2008-04-01_/
bool Crop_parameters::Quiescence::setup_structure(Data_record &record,bool for_write)    modification_
{  bool satup = Crop_parameters_class::Quiescence::setup_structure(record,for_write);
   record.expect_parameter(p_consider_inactive_days);                            //080401
   record.expect_parameter(p_inducement_temperature);                            //970521
   return satup;
}
//_Quiescence::setup_structure______________________________________2010-05-10_/
Crop_parameters::Dormancy::Dormancy()
: Crop_parameters_class::Dormancy()
,p_chill_hours                         (chill_hours                           ,PP_chill_hours                        ,PPR_chill_hours                          ,PPF_F) // PPF_chill_hours)  //021202
,p_deg_day_bud_break_chill_req_sat     (deg_day_bud_break_chill_req_sat       ,PP_deg_day_bud_break_chill_req_sat    ,PPR_deg_day_bud_break_chill_req_sat      ,PPF_F) // PPF_deg_day_bud_break_chill_req_sat) //030515
,p_deg_day_bud_break_chill_req_not_sat (deg_day_bud_break_chill_req_not_sat   ,PP_deg_day_bud_break_chill_req_not_sat,PPR_deg_day_bud_break_chill_req_not_sat  ,PPF_F) // PPF_deg_day_bud_break_chill_req_not_sat)  //030515
,p_senesce_percent                     (senesce                               ,PP_senesce_percent                    ,PPR_senesce_percent                      ,PPF_F) // PPF_senesce_percent)   //060317
{}
//_Dormancy:constructor_____________________________________________2002-04-08_/
bool Crop_parameters::Dormancy::setup_structure(Data_record &record,bool for_write) modification_
{  bool satup = Crop_parameters_class::Dormancy::setup_structure(record,for_write);
   record.expect_parameter(p_senesce_percent);                                   //060317
   record.expect_parameter(p_chill_hours);                                       //030515
   record.expect_parameter(p_deg_day_bud_break_chill_req_sat);                   //030515
   record.expect_parameter(p_deg_day_bud_break_chill_req_not_sat);               //030515
   return satup;
}
//_Dormancy::setup_structure________________________________________2010-06-10_/
Crop_parameters::Root::Root()
: Crop_parameters_class::Root()
,p_max_depth                     (max_depth                       ,PP_max_depth                       ,PPR_max_depth                      ,PPF_F) // PPF_max_depth)
,p_surface_density               (surface_density                 ,PP_surface_density                 ,PPR_surface_density                ,PPF_F) // PPF_surface_density)
,p_density_distribution_curvature(density_distribution_curvature  ,PP_density_distribution_curvature  ,PPR_density_distribution_curvature ,PPF_F) // PPF_density_distribution_curvature)
,p_sensitivity_to_water_stress   (sensitivity_to_water_stress     ,PP_sensitivity_to_water_stress     ,PPR_sensitivity_to_water_stress    ,PPF_F) // PPF_sensitivity_to_water_stress)
,p_length_at_emergence           (length_at_emergence_cm          ,PP_length_at_emergence             ,PPR_length_at_emergence            ,PPF_F) // PPF_length_at_emergence)
,p_root_shoot_emergence_ratio    (root_shoot_emergence_ratio      ,PP_root_shoot_emergence_ratio      ,PPR_root_shoot_emergence_ratio     ,PPF_F) // PPF_root_shoot_emergence_ratio) //131101
,p_root_shoot_full_ratio         (root_shoot_full_ratio           ,PP_root_shoot_full_ratio           ,PPR_root_shoot_full_ratio          ,PPF_F) // PPF_root_shoot_full_ratio) //131101
{}
//_Root:constructor_________________________________________________2006-12-12_/
bool Crop_parameters::Root::setup_structure(Data_record &record,bool for_write) modification_
{  bool satup = Crop_parameters_class::Root::setup_structure(record,for_write);
   record.expect_parameter(p_sensitivity_to_water_stress);                       //061212
   record.expect_parameter(p_max_depth);
   record.expect_parameter(p_surface_density);                                   //030722
   record.expect_parameter(p_density_distribution_curvature);                    //030722
   record.expect_parameter(p_length_at_emergence);
   // Prior to version 4.06.03 root parameters were under morphology
   // In version 5 we will no longer import.
   record.expect_parameter(p_root_shoot_emergence_ratio);                        //131101
   record.expect_parameter(p_root_shoot_full_ratio);                             //131101
   return satup;
}
//_Root::setup_structure____________________________________________2010-06-10_/
Crop_parameters::Morphology::Morphology()
: Crop_parameters_class::Morphology()
, p_reduce_canopy_expansion_LWP  (reduce_canopy_expansion_LWP  ,PP_reduce_canopy_expansion_LWP  ,PPR_LWP,PPF_8_1) // PPF_LWP)
, p_stop_canopy_expansion_LWP    (stop_canopy_expansion_LWP    ,PP_stop_canopy_expansion_LWP    ,PPR_LWP,PPF_8_1) // PPF_LWP)
, p_max_canopy_height            (max_canopy_height            ,PP_max_canopy_height            ,PPR_max_canopy_height,PPF_F) // PPF_max_canopy_height)
, p_light_extinction_coef        (light_extinction_coef        ,PP_light_extinction_coef        ,PPR_light_extinction_coef,PPF_F) // PPF_light_extinction_coef)
/*abandoned
#if (PHENOLOGY_VERSION!=2018)
// canopy
, p_canopy_senescence_initiation (canopy_senescence_initiation,PP_canopy_senescence_initiation  ,PPR_canopy_senescence_initiation,PPF_canopy_senescence_initiation) //190627
, p_canopy_senescence_duration   (canopy_senescence_duration  ,PP_canopy_senescence_duration    ,PPR_canopy_senescence_duration  ,PPF_canopy_senescence_duration) //190627
#endif
*/
{}
//_Morphology:constructor___________________________________________2001-02-19_/
bool Crop_parameters::Morphology::setup_structure(Data_record &record,bool for_write)    modification_
{  bool satup = Crop_parameters_class::Morphology::setup_structure(record,for_write);
      record.expect_parameter(p_max_canopy_height);   // Used by LADSS and VIC  //030303
      // In version 5 this is moved to morphology
   record.set_current_section("growth");
      record.expect_parameter(p_reduce_canopy_expansion_LWP);                    //080506
      record.expect_parameter(p_stop_canopy_expansion_LWP);                      //080506
   if (!for_write) // moved to canopy_cover
   {
      record.set_current_section("transpiration");  // In version 4 kc was under transpiration but it is probably more suitable under canopy
      record.expect_float32("kc",light_extinction_coef);   //Extinction coefficient for solar radiation
   }
   if (!for_write) // moved to canopy_cover
   {
      record.set_current_section("morphology");  // In version 4 kc was under transpiration but it is probably more suitable under canopy
      record.expect_float32("light_extinction_coef",light_extinction_coef);   //Extinction coefficient for solar radiation
      record.expect_float32("kc",light_extinction_coef);   //Extinction coefficient for solar radiation
   }

   record.set_current_section("canopy_cover");                                          //200212
      record.expect_parameter(p_light_extinction_coef);   //Extinction coefficient for solar radiation

/*abandoned decided to keep in phenology
   record. set_current_section("canopy_cover");                                  //190627
      record.expect_parameter(p_canopy_senescence_initiation);                   //190627
      record.expect_parameter(p_canopy_senescence_duration);                     //190627
*/
  return satup;
}
//_Morphology::setup_structure______________________________________2011-02-19_/
Crop_parameters::Canopy_growth_cover_based::Canopy_growth_cover_based()
: Crop_parameters_class::Canopy_growth_cover_based()
, p_cover_initial         (cover_initial           ,PP_cover_initial             ,PPR_cover_initial      ,PPF_F) // PPF_cover_initial)
, p_cover_maximum         (cover_maximum_no_CO2_adj,PP_cover_maximum             ,PPR_cover_maximum      ,PPF_F) // PPF_cover_maximum)
//#ifndef SENESCENCE_REDUCTION_200124
, p_cover_total_season_end(cover_total_season_end   ,PP_cover_total_season_end   ,PPR_cover_total_season_end ,PPF_F) // PPF_cover_total_season_end)
//#endif
, p_cover_green_season_end         (cover_green_season_end   ,PP_cover_green_season_end         ,PPR_cover_green_season_end,PPF_F) // PPF_cover_green_season_end) //171219_171009
//200125 , p_thermal_time_season_end_obsolete(thermal_time_season_end_obsolete  ,PP_thermal_time_season_end        ,PPR_thermal_time_season_end,PPF_thermal_time_season_end) //171219_171009
, p_fract_season_to_reach_half_max_cover(thermal_time_fractional_at_half_cover_max,PP_fract_season_to_reach_half_max_cover,PPR_fract_season_to_reach_half_max_cover,PPF_F) // PPF_fract_season_to_reach_half_max_cover) //181215
, p_shape_coef_accrescent     (shape_coef_accrescent,PP_shape_coef_accrescent , PPR_shape_coef, PPF_F) // PPF_shape_coef)   //171221
, p_shape_coef_senescent      (shape_coef_senescent,PP_shape_coef_senescent   , PPR_shape_coef, PPF_F) // PPF_shape_coef)   //171221
{}
//_Canopy_growth_cover_based:constructor____________________________2008-07-25_/
bool Crop_parameters::Canopy_growth_cover_based::setup_structure(Data_record &record,bool for_write)    modification_
{  bool satup = Crop_parameters_class::Canopy_growth_cover_based::setup_structure(record,for_write);
   record.expect_parameter(p_cover_initial);                                     //080725
   record.expect_parameter(p_cover_maximum);                                     //080725
   #ifndef SENESCENCE_REDUCTION_200124
   record.expect_parameter(p_cover_total_season_end);                            //171221
   #endif
   record.expect_parameter(p_cover_green_season_end);                            //171221
   if (!for_write)                                                               //200125
      record.expect_float32("thermal_time_season_end" ,thermal_time_season_end_obsolete); //200125
      //200125 record.expect_parameter(p_thermal_time_season_end_obsolete);               //190627_171221
      // Warning thermal_time_season_end may be moved to to phenology   171221
   record.expect_parameter(p_shape_coef_accrescent);                             //171221
   record.expect_parameter(p_shape_coef_senescent);                              //171221
   record.set_current_section("canopy_cover");  // In version 4 kc was under transpiration but it is probably more suitable under canopy

   if (!for_write)
   {  // This is to allow import from earlier versions // was slated to be dropped in version 6 but still using as of 2017
      //record.expect_float32("initial_cover",cover_initial);                         //080725
      //record.expect_float32("maximum_cover",cover_maximum_no_CO2_adj);              //080725
      record.expect_float32("CC_initial",cover_initial);                         //080725
      record.expect_float32("CC_maximum",cover_maximum_no_CO2_adj);              //080725
      #ifndef SENESCENCE_REDUCTION_200124
      record.expect_float32("CC_total_mature",cover_total_season_end);           //080725
      #endif
   }

//   record.expect_float32("initial",cover_initial);                             //171221
//   record.expect_float32("maximum",cover_maximum_no_CO2_adj);                  //171221

   //??? record.expect_float32("green_senescence_full",green_senescence_full);   //171219

   record.expect_parameter(p_fract_season_to_reach_half_max_cover);              //080725
   return satup;
}
//_Canopy_growth_cover_based::setup_structure_______________________2010-06-10_/
Crop_parameters::Canopy_growth_LAI_based::Canopy_growth_LAI_based()
: Crop_parameters_class::Canopy_growth_LAI_based()
, p_initial_GAI          (initial_GAI           ,PP_initial_GAI          ,PPR_early_GAI,PPF_F) // PPF_early_GAI)  //000207
, p_regrowth_GAI         (regrowth_GAI          ,PP_regrowth_GAI         ,PPR_early_GAI,PPF_F) // PPF_early_GAI)  //080207
, p_max_LAI              (max_LAI               ,PP_max_LAI              ,PPR_max_LAI,PPF_F) // PPF_max_LAI)    //990228
, p_specific_leaf_area   (specific_leaf_area    ,PP_specific_leaf_area   ,PPR_specific_leaf_area,PPF_F) // PPF_specific_leaf_area)
, p_fract_LAI_mature     (fract_LAI_mature      ,PP_fract_LAI_mature     ,PPR_fract_LAI_mature,PPF_F) // PPF_fract_LAI_mature)
, p_stem_leaf_partition  (stem_leaf_partition   ,PP_stem_leaf_partition  ,PPR_stem_leaf_partition,PPF_F) // PPF_stem_leaf_partition)
, p_leaf_duration_deg_day(leaf_duration_deg_day ,PP_leaf_duration_deg_day,PPR_leaf_duration_deg_day,PPF_F) // PPF_leaf_duration_deg_day)  //050628
, p_leaf_duration_sensitivity_stress(leaf_duration_sensitivity_stress,PP_leaf_duration_sensitivity_stress,PPR_leaf_duration_sensitivity_stress,PPF_F) // PPF_leaf_duration_sensitivity_stress)
{}
//_Canopy_growth_LAI_based:constructor_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _/
bool Crop_parameters::Canopy_growth_LAI_based::setup_structure(Data_record &record,bool for_write)    modification_
{  bool satup = Crop_parameters_class::Canopy_growth_LAI_based::setup_structure(record,for_write);
   record.expect_parameter(p_max_LAI); // now for all crops

//NYI      if (NOT_ANY_FRUIT_OR_UNKNOWN)                                         //020405
      {  record.expect_parameter(p_fract_LAI_mature);
      }
   record.expect_parameter(p_initial_GAI);                                       //000413
   record.expect_parameter(p_regrowth_GAI);                                      //080207
   record.expect_parameter(p_specific_leaf_area);
   record.expect_parameter(p_stem_leaf_partition);
   record.expect_parameter(p_leaf_duration_deg_day    );                         //050611
   record.expect_parameter(p_leaf_duration_sensitivity_stress);
   return satup;
}
//_Canopy_growth_LAI_based::setup_structure_________________________2010-06-10_/
Crop_parameters::Tolerance::Tolerance()
: Crop_parameters_class::Tolerance()
, p_leaf_damage_cold_temp  (temperature_cold_leaf_damage ,PP_leaf_damage_cold_temp,PPR_hardiness_temperature,PPF_F) // PPF_hardiness_temperature)
, p_leaf_lethal_cold_temp  (temperature_cold_leaf_lethal ,PP_leaf_lethal_cold_temp,PPR_hardiness_temperature,PPF_F) // PPF_hardiness_temperature)
, p_fruit_damage_cold_temp (temperature_cold_fruit_damage,PP_fruit_damage_cold_temp,PPR_hardiness_temperature,PPF_F) // PPF_hardiness_temperature)
, p_fruit_lethal_cold_temp (temperature_cold_fruit_lethal,PP_fruit_lethal_cold_temp,PPR_hardiness_temperature,PPF_F) // PPF_hardiness_temperature)
{}
//_Tolerance:constructor_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _/
bool Crop_parameters::Tolerance::setup_structure
(Data_record &record,bool for_write,Harvested_part harvested_part) modification_
{
   bool satup = Crop_parameters_class::Tolerance::setup_structure(record,for_write,harvested_part);
// had note 131101  Replaced by harvest index sensitivity but still appears to be used
      record.expect_parameter(p_leaf_damage_cold_temp);                          //990129
      record.expect_parameter(p_leaf_lethal_cold_temp);                          //991029
      if ((harvested_part == fruit_part) || !for_write)
      {
         record.expect_parameter(p_fruit_damage_cold_temp);                      //081128
         record.expect_parameter(p_fruit_lethal_cold_temp);                      //081128
      }
   return satup;
}
//_Tolerance::setup_structure_______________________________________2011-03-25_/
Crop_parameters::Yield::Yield()
: Crop_parameters_class::Yield()
,p_index_unstressed        (harvest_index_unstressed        ,PP_unstressed                ,PPR_unstressed            ,PPF_F) // PPF_unstressed)
,p_growth_sensitivity      (stress_sensitivity_growth       ,PP_growth_sensitivity        ,PPR_stress_sensitivity    ,PPF_F) // PPF_stress_sensitivity)
,p_tuber_init_sensitivity  (stress_sensitivity_tuber_init   ,PP_tuber_init_sensitivity    ,PPR_stress_sensitivity    ,PPF_F) // PPF_stress_sensitivity)
,p_tuber_growth_sensitivity(tuber_growth_sensitivity        ,PP_tuber_growth_sensitivity  ,PPR_stress_sensitivity    ,PPF_F) // PPF_stress_sensitivity)
,p_translocation_fraction_max (translocation_fraction_max   ,PP_translocation_fraction    ,PPR_translocation_fraction,PPF_F) // PPF_translocation_fraction)
{}
//______________________________________________________________________________
bool Crop_parameters::Yield::setup_structure
(Data_record &record,bool for_write,Harvested_part harvested_part) modification_
{
//   #if (CROPSYST_VERSION < 6)
//   if (!for_write)
//   {  // To import V4
      setup_structure_section(record,for_write,harvested_part);                  //020405
//   }
//   #endif
//   setup_structure_section(record,for_write,harvested_part,"yield"/*171124 "harvest"*/);           //110405
   if (!for_write)
      record.expect_bool("heat_extreme",apply_temperature_extreme_response);              //200423
   record.expect_bool("temperature_extreme_response",apply_temperature_extreme_response); //200423
   record.expect_bool("water_response",apply_water_response_flowering);                   //200423
   record.expect_bool("temperature_response",apply_temperature_response_flowering);       //200423
   record.expect_float32
         ("water_stress_sensitivity",water_stress_sensitivity);                                                            //200424
   record.expect_float32
         ("temperature_stress_sensitivity",temperature_stress_sensitivity);                                                            //200424
   record.expect_float32
         ("response_temperature_optimum",response_temperature_optimum);                                                            //200424
   record.expect_float32
         ("response_temperature_damaging",response_temperature_damaging);                                                            //200424
   return true;
}
//______________________________________________________________________________
bool Crop_parameters::Yield::setup_structure_section
(Data_record &record,bool for_write,Harvested_part harvested_part) modification_
{  //NYN Crop_parameters_class::Harvest::setup_structure(record,for_write);
   record.set_current_section(LABEL_harvest_index);
   record.expect_parameter(p_index_unstressed);
   bool UNKNOWN = (harvested_part == UNKNOWN_PART);
   if ( UNKNOWN
         ||(harvested_part == root_part)                                         //020405
         ||(harvested_part == tuber_part)                                        //020405
         ||(!for_write))                                                         //051207
   { // Tuber and root crops
      record.expect_parameter(p_tuber_init_sensitivity);
      record.expect_parameter(p_tuber_growth_sensitivity);
   }
   record.expect_parameter(p_translocation_fraction_max);                        //110907
   if ( UNKNOWN
            ||(harvested_part == leaf_part)
            ||(!for_write))                                                      //020419
   { // leaf and crops
      record.expect_parameter(p_index_unstressed);                               //020419
      record.expect_parameter(p_growth_sensitivity);                             //020419
   }
   // The following are common to both V4 and V5
   record.expect_bool("adjust_HI",apply_adjustment_to_unstressed_harvest_index); //110909
   if (!for_write)                                                               //200423
   record.expect_bool("heat_extreme",apply_temperature_extreme_response);        //200423_150206
   record.expect_bool("temperature_extreme_response",apply_temperature_extreme_response); //200423
   record.expect_bool("water_response",apply_water_response_flowering);                   //200423
   record.expect_bool("temperature_response",apply_temperature_response_flowering);       //200423
   return true;
}
//_Harvest::setup_structure_section_________________________________2011-02-19_/
Crop_parameters::Fruit::Fruit()
: Crop_parameters_class::Fruit()
, p_fract_total_solids              (fract_total_solids                 ,PP_fract_total_solids              ,PPR_fract_total_solids              ,PPF_F) // PPF_fract_total_solids)
, p_max_fruit_load_kg_ha            (max_fruit_load_kg_ha               ,PP_max_fruit_load_kg_ha            ,PPR_max_fruit_load_kg_ha            ,PPF_F) // PPF_max_fruit_load_kg_ha)
, p_initial_growth_fraction_to_fruit(initial_growth_fraction_to_fruit   ,PP_initial_growth_fraction_to_fruit,PPR_initial_growth_fraction_to_fruit,PPF_F) // PPF_initial_growth_fraction_to_fruit)
, p_rapid_growth_fraction_to_fruit  (rapid_growth_fraction_to_fruit     ,PP_rapid_growth_fraction_to_fruit  ,PPR_rapid_growth_fraction_to_fruit  ,PPF_F) // PPF_rapid_growth_fraction_to_fruit)
, p_harvest_temperature             (harvest_temperature                ,PP_harvest_temperature             ,PPR_harvest_temperature             ,PPF_F) // PPF_harvest_temperature)
, p_harvest_temperature_days        (harvest_temperature_days           ,PP_harvest_temperature_days        ,PPR_harvest_temperature_days        ,PPF_N) // PPF_harvest_temperature_days)
, p_harvest_DOY_earliest            (harvest_DOY_earliest               ,PP_harvest_DOY_earliest            ,PPR_harvest_DOY_earliest            ,PPF_N) // PPF_harvest_DOY_earliest)
, p_harvest_DOY_latest              (harvest_DOY_latest                 ,PP_harvest_DOY_latest              ,PPR_harvest_DOY_latest              ,PPF_N) // PPF_harvest_DOY_latest)
, p_max_water_uptake_before_fruiting      (max_water_uptake_before_fruiting_mm      ,PP_max_water_uptake_before_fruiting      ,PPR_max_water_uptake_before_fruiting     ,PPF_F) // PPF_max_water_uptake_before_fruiting)
, p_max_water_uptake_during_initial_fruit (max_water_uptake_during_initial_fruit_mm ,PP_max_water_uptake_during_initial_fruit ,PPR_max_water_uptake_during_initial_fruit,PPF_F) // PPF_max_water_uptake_during_initial_fruit)
, p_max_water_uptake_during_radid_fruit   (max_water_uptake_during_radid_fruit_mm   ,PP_max_water_uptake_during_radid_fruit   ,PPR_max_water_uptake_during_radid_fruit  ,PPF_F) // PPF_max_water_uptake_during_radid_fruit)
, p_max_water_uptake_during_veraison      (max_water_uptake_during_veraison_mm      ,PP_max_water_uptake_during_veraison      ,PPR_max_water_uptake_during_veraison     ,PPF_F) // PPF_max_water_uptake_during_veraison)
, p_max_water_uptake_after_maturity       (max_water_uptake_after_maturity_mm       ,PP_max_water_uptake_after_maturity       ,PPR_max_water_uptake_after_maturity      ,PPF_F) // PPF_max_water_uptake_after_maturity)
{}
//_Fruit:constructor_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _/
bool Crop_parameters::Fruit::setup_structure(Data_record &record,bool for_write) modification_
{
   bool satup = Crop_parameters_class::Fruit::setup_structure(record,for_write);
   record.expect_parameter(p_fract_total_solids);                                //000630
   record.expect_parameter(p_max_fruit_load_kg_ha);                              //000616

   record.expect_parameter(p_initial_growth_fraction_to_fruit);                  //030521
   record.expect_parameter(p_rapid_growth_fraction_to_fruit);   // grapes        //030521

   record.expect_parameter(p_harvest_temperature);                               //081204
   record.expect_parameter(p_harvest_temperature_days);                          //081204
   record.expect_parameter(p_harvest_DOY_earliest);                              //081204
   record.expect_parameter(p_harvest_DOY_latest);                                //081204

   record.expect_parameter(p_max_water_uptake_before_fruiting);                  //091202
   record.expect_parameter(p_max_water_uptake_during_initial_fruit);             //091202
   record.expect_parameter(p_max_water_uptake_during_radid_fruit);               //091202
   record.expect_parameter(p_max_water_uptake_during_veraison);                  //091202
   record.expect_parameter(p_max_water_uptake_after_maturity);                   //091202
   return satup;
}
//_Fruit::setup_structure___________________________________________2011-03-26_/
#ifdef CHECK_NEEDED
110127 probably is but don't delete yet

Crop_parameters::Fruit::Fruit(const Fruit &copy_from)
: public Crop_parameters::Fruit(const Fruit &copy_from)
, p_initial_fruit_mass           (initial_fruit_mass           ,PP_initial_fruit_mass           ,PPR_initial_fruit_mass           ,PPF_initial_fruit_mass)
, p_initial_reserves_release_rate(initial_reserves_release_rate,PP_initial_reserves_release_rate,PPR_initial_reserves_release_rate,PPF_initial_reserves_release_rate)
, p_clusters_per_vine            (clusters_per_vine            ,PP_clusters_per_vine            ,PPR_clusters_per_vine            ,PPF_clusters_per_vine)
, p_berries_per_cluster          (berries_per_cluster          ,PP_berries_per_cluster          ,PPR_berries_per_cluster          ,PPF_berries_per_cluster)
, p_fruits_per_tree              (fruits_per_tree              ,PP_fruits_per_tree              ,PPR_fruits_per_tree              ,PPF_fruits_per_tree)

, p_harvest_temperature_days     (harvest_temperature_days     ,PP_harvest_temperature_days     ,PPR_harvest_temperature_days     ,PPF_harvest_temperature_days)
, p_harvest_temperature          (harvest_temperature          ,PP_harvest_temperature          ,PPR_harvest_temperature          ,PPF_harvest_temperature)
, p_harvest_DOY_earliest         (harvest_DOY_earliest         ,PP_harvest_DOY_earliest         ,PPR_harvest_DOY_earliest         ,PPF_harvest_DOY_earliest)
, p_harvest_DOY_latest           (harvest_DOY_latest           ,PP_harvest_DOY_latest           ,PPR_harvest_DOY_latest           ,PPF_harvest_DOY_latest)

, p_fract_total_solids                  (fract_total_solids                 ,PP_fract_total_solids                ,PPR_fract_total_solids              ,PPF_fract_total_solids)
, p_max_fruit_load_kg_ha                (max_fruit_load_kg_ha               ,PP_max_fruit_load_kg_ha              ,PPR_max_fruit_load_kg_ha            ,PPF_max_fruit_load_kg_ha)
, p_initial_growth_fraction_to_fruit    (initial_growth_fraction_to_fruit   ,PP_initial_growth_fraction_to_fruit  ,PPR_initial_growth_fraction_to_fruit,PPF_initial_growth_fraction_to_fruit)
, p_rapid_growth_fraction_to_fruit      (rapid_growth_fraction_to_fruit     ,PP_rapid_growth_fraction_to_fruit    ,PPR_rapid_growth_fraction_to_fruit  ,PPF_rapid_growth_fraction_to_fruit)

, p_max_water_uptake_before_fruiting      (max_water_uptake_before_fruiting      ,PP_max_water_uptake_before_fruiting      ,PPR_max_water_uptake_before_fruiting     ,PPF_max_water_uptake_before_fruiting)
, p_max_water_uptake_during_initial_fruit (max_water_uptake_during_initial_fruit ,PP_max_water_uptake_during_initial_fruit ,PPR_max_water_uptake_during_initial_fruit,PPF_max_water_uptake_during_initial_fruit)
, p_max_water_uptake_during_radid_fruit   (max_water_uptake_during_radid_fruit   ,PP_max_water_uptake_during_radid_fruit   ,PPR_max_water_uptake_during_radid_fruit  ,PPF_max_water_uptake_during_radid_fruit)
, p_max_water_uptake_during_veraison      (max_water_uptake_during_veraison      ,PP_max_water_uptake_during_veraison      ,PPR_max_water_uptake_during_veraison     ,PPF_max_water_uptake_during_veraison)
, p_max_water_uptake_after_maturity       (max_water_uptake_after_maturity       ,PP_max_water_uptake_after_maturity       ,PPR_max_water_uptake_after_maturity      ,PPF_max_water_uptake_after_maturity)
{}
#endif
//______________________________________________________________________________
Crop_parameters::Layout::Layout()
:Crop_parameters_class::Layout()
,p_row_spacing(spacing.row,PP_row_spacing,PPR_row_or_col_spacing,PPF_F) // PPF_row_or_col_spacing)
,p_col_spacing(spacing.col,PP_col_spacing,PPR_row_or_col_spacing,PPF_F) // PPF_row_or_col_spacing)
,p_row_azimuth(row_azimuth_true_north_deg ,PP_row_azimuth,PPR_row_azimuth,       PPF_F) // PPF_row_azimuth)
{}
//_Layout:constructor_______________________________________________2002-04-08_/
Crop_parameters::Nitrogen::Nitrogen()
:Crop_parameters_class::Nitrogen()
,p_max_uptake_daily_rate               (max_uptake_daily_rate_kg_ha                 ,PP_max_uptake_daily_rate                 ,PPR_max_uptake_daily_rate                 ,PPF_F) // PPF_max_uptake_daily_rate)
,p_stubble_max_conc                    (stubble_max_conc                            ,PP_stubble_max_conc                      ,PPR_stubble_max_conc                      ,PPF_F) // PPF_stubble_max_conc)
,p_root_conc                           (root_conc                                   ,PP_root_conc                             ,PPR_root_conc                             ,PPF_F) // PPF_root_conc)
,p_dilution_curve_slope                (dilution_curve_slope                        ,PP_dilution_curve_slope                  ,PPR_dilution_curve_slope                  ,PPF_F) // PPF_dilution_curve_slope)                   //130406
,p_residual_N_not_available_for_uptake (residual_N_not_available_for_uptake         ,PP_residual_N_not_available_for_uptake   ,PPR_residual_N_not_available_for_uptake   ,PPF_F) // PPF_residual_N_not_available_for_uptake)
,p_soil_N_conc_where_N_uptake_decreases(soil_N_conc_where_N_uptake_decreases        ,PP_soil_N_conc_where_N_uptake_decreases  ,PPR_soil_N_conc_where_N_uptake_decreases  ,PPF_F) // PPF_soil_N_conc_where_N_uptake_decreases)
,p_PAW_where_N_uptake_rate_decreases   (PAW_where_N_uptake_rate_decreases           ,PP_PAW_where_N_uptake_rate_decreases     ,PPR_PAW_where_N_uptake_rate_decreases     ,PPF_F) // PPF_PAW_where_N_uptake_rate_decreases)
,p_emergence_max_conc                  (emergence_max_conc                          ,PP_emergence_max_conc                    ,PPR_emergence_max_conc                    ,PPF_F) // PPF_emergence_max_conc)                     //121219
,p_emergence_crit_conc                 (emergence_crit_conc                         ,PP_emergence_crit_conc                   ,PPR_emergence_crit_conc                   ,PPF_F) // PPF_emergence_crit_conc)                    //121219
,p_biomass_to_start_dilution_crit_N_conc(biomass_to_start_dilution_crit_N_conc_T_ha ,PP_biomass_to_start_dilution_crit_N_conc ,PPR_biomass_to_start_dilution_crit_N_conc ,PPF_F) // PPF_biomass_to_start_dilution_crit_N_conc)  //121219
,p_N_mature_max_conc                   (dilution_curve_max_conc_mature ,PP_N_mature_max_conc                     ,PPR_N_mature_max_conc                     ,PPF_F) // PPF_N_mature_max_conc)                      //120406
,p_end_dilution_curve                  (end_dilution_curve_deg_day                  ,PP_end_dilution_curve                    ,PPR_end_dilution_curve                    ,PPF_N) // PPF_end_dilution_curve)                     //130408
{}
//_Nitrogen:constructor_____________________________________________2002-04-08_/
bool Crop_parameters::Nitrogen::setup_structure(Data_record &record,bool for_write)    modification_
{
   bool satup = Crop_parameters_class::Nitrogen::setup_structure(record, for_write);
   record.expect_parameter(p_max_uptake_daily_rate);
   record.expect_parameter(p_stubble_max_conc);
   record.expect_parameter(p_root_conc);
   record.expect_parameter(p_dilution_curve_slope);                              //130406
   record.expect_parameter(p_residual_N_not_available_for_uptake);
   record.expect_parameter(p_soil_N_conc_where_N_uptake_decreases);
   record.expect_parameter(p_PAW_where_N_uptake_rate_decreases);
   record.expect_parameter(p_emergence_max_conc);                                //130406
   record.expect_parameter(p_emergence_crit_conc);                               //130406
   record.expect_parameter(p_biomass_to_start_dilution_crit_N_conc);             //130406
   record.expect_parameter(p_N_mature_max_conc);                                 //130406
   record.expect_parameter(p_end_dilution_curve);                                //130408
   return satup;
}
//_Nitrogen::setup_structure________________________________________2011-03-25_/
Crop_parameters::Salinity::Salinity()
: Crop_parameters_class::Salinity()
,p_osmotic_pot_50    (osmotic_pot_50   ,PP_osmotic_pot_50,  PPR_osmotic_pot_50,  PPF_F) // PPF_osmotic_pot_50)
,p_salt_tolerance_P  (salt_tolerance_P ,PP_salt_tolerance_P,PPR_salt_tolerance_P,PPF_F) // PPF_salt_tolerance_P)
{}
//_Salinity:constructor_____________________________________________2002-04-08_/
Crop_parameters::CO2_Response::CO2_Response()
:Crop_parameters_class::CO2_Response()
,p_TUE_reference_conc      (CO2_concentration_when_measured.TUE,PP_TUE_reference_conc,PPR_use_efficiency,PPF_F) // PPF_use_efficiency) //140314
,p_RUE_reference_conc      (CO2_concentration_when_measured.RUE,PP_RUE_reference_conc,PPR_use_efficiency,PPF_F) // PPF_use_efficiency) //140314
,p_alpha                   (non_rectagular_hyperbola_solution.alpha           ,PP_alpha                                  ,PPR_unitless                               ,PPF_F) // PPF_unitless)   //131206
,p_theta                   (non_rectagular_hyperbola_solution.theta           ,PP_theta                                  ,PPR_unitless                               ,PPF_F) // PPF_unitless)   //131206
,p_growth_ratio_asymptotic (non_rectagular_hyperbola_solution.growth_ratio_asymptotic  ,PP_growth_ratio_asymptotic       ,PPR_growth_ratio_asymptotic                ,PPF_F) // PPF_growth_ratio_asymptotic)  //131206
{}
//_CO2_Response:constructor_________________________________________2002-04-08_/
bool Crop_parameters::CO2_Response::setup_structure(Data_record &record,bool for_write)    modification_
{
   bool satup = Crop_parameters_class::CO2_Response::setup_structure(record,for_write);
   record.expect_parameter(p_TUE_reference_conc);                                //170524
   record.expect_parameter(p_RUE_reference_conc);                                //170524
   record.expect_parameter(p_alpha);                                             //131206
   record.expect_parameter(p_theta);                                             //131206
   record.expect_parameter(p_growth_ratio_asymptotic);                           //131206
   return satup;
}
//_CO2_Response::setup_structure____________________________________2011-03-26_/
Crop_parameters::Canopy_architecture::Canopy_architecture()
:Crop_parameters_class::Canopy_architecture()
,p_initial_width              (initial_width          ,PP_initial_width    ,PPR_canopy_dimension,PPF_F) // PPF_canopy_dimension)
,p_final_width                (final_width            ,PP_final_width      ,PPR_canopy_dimension,PPF_F) // PPF_canopy_dimension)
,p_initial_breadth            (initial_breadth        ,PP_initial_breadth  ,PPR_canopy_dimension,PPF_F) // PPF_canopy_dimension)
,p_final_breadth              (final_breadth          ,PP_final_breadth    ,PPR_canopy_dimension,PPF_F) // PPF_canopy_dimension)
,p_initial_height             (initial_height         ,PP_initial_height   ,PPR_canopy_dimension,PPF_F) // PPF_canopy_dimension)
,p_final_height               (final_height           ,PP_final_height     ,PPR_canopy_dimension,PPF_F) // PPF_canopy_dimension)
,p_branch_insertion_height    (branch_insertion_height,PP_branch_insertion_height,PPR_branch_insertion_height,PPF_F) // PPF_branch_insertion_height)
{}
//_Canopy_characteristics_hedge_row:constructor_____________________2002-04-08_/
Crop_parameters::Crop_parameters(
bool _nitrogen_simulation
,bool _salinity_simulation_unused
,bool _residue_simulation
,bool _CO2_simulation
)
: Crop_parameters_class()                                                        //110121
, show_section(0xFFFF)                                                           //110605
, thermal_time()                                                                 //020628
, phenology()                                                                    //080722
, morphology()                                                                   //110224
, canopy_growth_LAI_based()
, canopy_growth_cover_based()
, root()
, quiescence()                                                                   //160331
, dormancy()
//200423, harvest                ()                                                      //020409
, yield                  ()                                                      //200423
, fruit()
, nitrogen               ()                                                      //020408
, CO2_response           ()                                                      //020408
, p_linger_days          (linger_days,PP_linger_days,PPR_linger_days,PPF_N)
, tolerance              ()                                                      //020409
, emergence              ()
, vernalization          ()                                                      //020409
, photoperiod            ()                                                      //020409
, residue_decomposition  ()
, salinity               ()
, canopy_architecture    ()                                                      //020409
, layout                 ()                                                      //020409
{}
//_Crop_parameters:constructor______________________________________2011-01-21_/
bool Crop_parameters::expect_structure(bool for_write)
{
   set_current_section("GUI");
      expect_nat32("show",show_section,16);
   Crop_parameters_class::expect_structure(for_write);
   structure_defined = false;                                                    //120314
   bool UNKNOWN = !for_write || (harvested_part_clad.get() == UNKNOWN_PART);     //190130_020715

   //unused bool NOT_ANY_FRUIT_OR_UNKNOWN =(!(harvested_part_clad.get()==fruit_part)||UNKNOWN); //020405

   expect_parameter(p_linger_days);
   #if (VIC_CROPSYST_VERSION != 2)

   // Actually we will want this in all versions VIC, but this came after VIC2
   // and the code::Blocks compiler is complaining about a multiple inheritence

   EXPECT_ENUMERATED_SECTION("grain_stress_period",grain_stress_sensitivity_period_section_VV,yield.grain_stress_sensitivity_periods); //130428
   #endif

//130406  Warning I think this can be moved to crop_param_class:expect_structure or_setup_structure
   set_current_section("salinity");
      expect_parameter(salinity.p_osmotic_pot_50);
      expect_parameter(salinity.p_salt_tolerance_P);

   set_current_section(LABEL_canopy_characteristics);                            //000330
         expect_parameter(canopy_architecture.p_initial_width);                  //020405
         expect_parameter(canopy_architecture.p_final_width);                    //020405
         expect_parameter(canopy_architecture.p_initial_breadth);                //071116
         expect_parameter(canopy_architecture.p_final_breadth);                  //071116
         expect_parameter(canopy_architecture.p_initial_height);                 //020405
         expect_parameter(canopy_architecture.p_final_height);                   //030520
         expect_parameter(canopy_architecture.p_branch_insertion_height);        //071116
      set_current_section(LABEL_layout);                                         //020405
         expect_parameter(layout.p_row_spacing);                                 //020405
         expect_parameter(layout.p_row_azimuth);                                 //020405
         expect_parameter(layout.p_col_spacing);                                 //020405

   canopy_growth_cover_based.setup_structure (*this,for_write);                  //100730
   canopy_growth_LAI_based.setup_structure   (*this,for_write);                  //100730
   phenology.setup_structure                 (*this,for_write);                  //100611
   thermal_time.setup_structure              (*this,for_write);                  //100730
               // The following are actually thermal time accumulation parameters
               // in version 5 we will have a separate section
   root.setup_structure(*this,for_write);                                        //100730

   quiescence.setup_structure(*this,for_write);                                  //160331_100610
   bool CROPSYST_FRUIT = is_fruit_tree();                                        //190625_071121
   if (CROPSYST_FRUIT || !for_write) // ORCHARD_VINEYARD_MODEL_OR_UNKNOWN        //020418
      dormancy       .setup_structure(*this,for_write);                          //100610
      // Currently dormancy only applied to the fruit tree model,
      // but potentially any crop could have dormancy
   // Residue

   residue_decomposition.setup_structure(*this,for_write);                       //110126
   if (!CROPSYST_FRUIT)                                                          //110606
       vernalization.setup_structure(*this,for_write);                           //100730
   if (!CROPSYST_FRUIT)
       photoperiod.setup_structure(*this,for_write);                             //100513
   if ( (CROPSYST_FRUIT || !for_write))                                          //020405
   { // fruit trees and grape harvest parameters
      set_current_section(LABEL_fruit);                                          //020405
         expect_enum("harvest_condition",fruit.harvest_condition_mode_cowl);     //081204
   }
#ifdef NYI
110126 CropGro not yet implemented in V5
   if ((!for_write || (crop_model_labeled.get() == CROPGRO_MODEL))  && cropgro)  //031121
   {  expect_filename("model_input",cropgro->model_input_file,255);
   }
#endif
   #ifndef YAML_PARAM_VERSION
   #if (VIC_CROPSYST_VERSION != 2)
   //151020 Code::Blocks was having problems with multiple inheritence,
   // I may have corrected that problem
   set_current_section(LABEL_crop);
   EXPECT_ENUMERATED_SECTION("grain_stress_period",grain_stress_sensitivity_period_section_VV,yield.grain_stress_sensitivity_periods); //130428
   #endif
   #endif
   structure_defined = true;                                                     //120314
   return true;
}
//_Crop_parameters::expect_structure___________________________________________/

/* Importing V4 files

Not yet implemented:
Added 'abscission'
   this replaces senesced_biomass_fate
   and deciduous

harvested_part enumeration replaced 'seed' with 'grain'
   (shouldn't need to do anything as unrecognized 'seed' will default to 'grain'

Implemented:
Section [harvest_index] now [harvest]
*/
//______________________________________________________________________________
}//_namespace CropSyst_________________________________________________________/


