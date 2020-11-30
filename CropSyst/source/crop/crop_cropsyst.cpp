
#include "crop/crop_cropsyst.h"
#  include "static_phrases.h"
#   include "corn/measure/measures.h"
#   include <math.h>
#   include "corn/math/moremath.h"
//200813 #   include "cs_dvlp.h"
#   include <float.h>
#include "crop/crop_biomass.h"
#include "crop/canopy_growth_LAI_based_V4.h"
#include "crop/canopy_growth_LAI_based_V5.h"
#include "crop/transpiration.h"

#ifndef CROP_CALIBRATION
#  include "crop/canopy_growth_cover_based.h"
#endif
#include "mgmt_param.h"
#  include "soil/soil_I.h"
#include "soil/soil_evaporator.h"

#ifdef QND
#include <Q_N_D.h>
#endif

#  include "crop/crop_param.h"
#  include "crop/crop_emergence.h"

#if (CROPSYST_VERSION==4)
#include "common/weather/hour/weather_hours.h"
#  include "common/weather/loc_param.h"
// loc_param has Season
#  include "common/weather/weather.h"
#endif
#  include "common/simulation/log.h"
#ifdef NITROGEN
#  include "crop/crop_N_V4.h"
#  include "crop/crop_N_V5.h"
#endif
#ifdef CO2_CHANGE
#  include "crop/CO2_response.h"
#endif
#   ifdef EVENT_PROCESSING
#      include "cs_event.h"
#   endif
#ifndef compareHPP
#  include "corn/math/compare.hpp"
#endif
#include "crop/growth_stages.hpp"

#define common_translate(x) x

#ifndef ET_reference_FAOH
#  include "biometeorology/ET_reference_FAO.h"
#endif
#include "crop/yield.h"
#include "corn/chronometry/date_32.h"
#include "crop/biomass_growth_RUE_TUE.h"
#include "UED/library/std_codes.h"

#ifdef DEBUG_N_CONC
extern ofstream debug_N_conc_harvest_residue_root;
#endif
extern float64 CS_UED_max_root_biomass_encountered;
extern float64 CS_UED_max_canopy_biomass_encountered;

// Keep the LAI and biomass dumps for Mike
//debug ofstream dump_lAI("LAI.dat");
#ifdef DUMP_BIOMASS
ofstream dump_biomass("biomass.dat");
#endif
#include "crop/yield_temperature_extreme_response.h"
#ifdef CS_INSPECTOR_VERSION
#  include "UED/library/database_file.h"
#  include "UED/library/datasetrec_creation.h"
#  include "UED/library/datasetrec.h"
#  include "UED/library/controls.h"
#  include "CS_suite/observation/CS_observation.h"
#  include "corn/quality.hpp"
#endif

#include "common/residue/residues_interface.h"

#include "csvc.h"
#include "csvc_special.h"


// For inspection:

#include "CS_suite/observation/CS_inspector.h"
#define CSVC_crop_degree_hours_above_30_today                        0x8021FFFE
#define CSVC_crop_degree_hours_above_34_today                        0x8021FFFD
#define CSVP_crop_transpiration_use_eff                              0x21FFFF
#define CSVP_crop_ET_act                                             0x21FFFC
#define CSVP_crop_ET_pot                                             0x21FFFB

#define CSVC_crop_duration_season                                    0x8021FFF8

#include "CS_suite/observation/CS_examination.h"

namespace CropSyst {
float64 no_CO2_adjustment = 1.0;                                                 //160314

#ifdef CROPSYST_PROPER
#if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
CS::Stress_ascertaination::Parameters parameters_above_30C_season={0xFFFF,30,true,NGS_ACTIVE_GROWTH};    //180814
CS::Stress_ascertaination::Parameters parameters_above_35C_season={0xFFFF,35,true,NGS_ACTIVE_GROWTH};    //180814
CS::Stress_ascertaination::Parameters parameters_above_30C_flowering={14 ,30,true,NGS_ANTHESIS};  //180814
CS::Stress_ascertaination::Parameters parameters_above_34C_flowering={14 ,34,true,NGS_ANTHESIS};  //180814
#endif
#if ((PHENOLOGY_VERSION==2018))
CS::Stress_ascertaination::Parameters parameters_above_30C_season={0xFFFF,30,true,true};    //180814
CS::Stress_ascertaination::Parameters parameters_above_35C_season={0xFFFF,35,true,true};    //180814
CS::Stress_ascertaination::Parameters parameters_above_30C_flowering={14 ,30,true,false};  //180814
CS::Stress_ascertaination::Parameters parameters_above_34C_flowering={14 ,34,true,false};  //180814
#endif
#endif

//______________________________________________________________________________
float64 calc_stress_adjusted_temperature
(float64 crop_param_phenologic_sensitivity_water_stress
,float64 air_temp                // Could be max temperatue (or hourly temp?)    //030610
,float64 senescence_reduction_factor  // may be 0 if not yet achieved peak_LAI   //030610
,float64 water_stress_index
,float64 vapor_pressure_deficit_max   //check if this is VPD max
//181108 ,Normal_crop_event_sequence growth_stage_sequence
,bool started_maturity                                                           //181108
,bool          has_leaf_senescence) /* was perennial but that didn't account for perennials that lost leaves */ //020628
{  float64 senescence_reduction =                                                //970123
      (has_leaf_senescence  && ((!started_maturity)                              //181108
         && !is_zero(senescence_reduction_factor/*200127 ,0.0,0.00001*/) ))      //030610
        ? senescence_reduction_factor  : 1.0;                                    //030609
   senescence_reduction = std::max<float64>(0.0,std::min<float64>(senescence_reduction,1.0)); // should not needed guarenteed to be between 0-1 //970123
   float64 stress_increase_temperature = air_temp
      + ((1.0 + 1.5
                * vapor_pressure_deficit_max                                     //010202
                * senescence_reduction)
          * water_stress_index)                                                  //010726
        * crop_param_phenologic_sensitivity_water_stress;                        //030610
   return stress_increase_temperature;
}
//_calc_stress_adjusted_temperature____________________________________________/
}//_namespace CropSyst_________________________________________________________/

namespace CropSyst {                                                             //141206
//____________________________________________________________2002-11-25_1997?_/
Crop_complete::Crop_complete
(
 Crop_parameters     *parameters_                                                //020409
,float64 param_dry_soil_root_activity_coef_                                      //170518
,float64 param_saturated_soil_root_activity_coef_                                //170518
,bool                                   owns_parameters_                         //040121
,const CORN::date32 &today_                                                      //170525
,CORN::Unidirectional_list             &performable_operations_                  //151004
,const CS::CO2_atmospheric_concentration *atmospheric_CO2_conc_ppm_optional      //180122
,modifiable_ Crop_CO2_response         *Crop_CO2_response_given_optional         //151201
,const Air_temperature_maximum         &air_temperature_max_                     //140813
,const Air_temperature_minimum         &air_temperature_min_                     //140813
,const Air_temperature_average         &air_temperature_avg_                     //140813
,const Solar_radiation                 &solar_radiation_                         //140813
,const Vapor_pressure_deficit_daytime  &daytime_vapor_pressure_deficit_          //140813
,const Vapor_pressure_deficit_max      &vapor_pressure_deficit_max_              //140813
,const Psychrometric_constant          &psychrometric_constant_                  //140508
,const Weather_spanning_interface      &weather_                                 //151201
,const CORN::Local_time                &sun_rise_time_                           //150907
,const float64                         &ref_day_length_hours_                    //140812
,Seeding_parameters                    *seeding_parameters_ // may be 0          //041201
,Soil::Soil_interface                  *soil_                                    //050721
,Crop_nitrogen_model                    nitrogen_model_                          //050722
#ifdef NITROGEN
,const Slope_saturated_vapor_pressure_plus_psychrometric_resistance //referenced
      *slope_saturated_vapor_pressure_plus_psychrometric_resistance_reference
#endif
)
:Crop_common
   (parameters_,owns_parameters_                                                 //040121
   ,daytime_vapor_pressure_deficit_                                              //140813
   ,soil_
   ,Crop_CO2_response_given_optional                                             //160418
   ,today_                                                                       //150915
   ,performable_operations_)                                                      //151004
,weather                         (weather_)
,sun_rise_time                   (sun_rise_time_)                                //150907
,snow_pack_thickness             (0)                                             //161111_141205
, air_temperature_max       (air_temperature_max_)                               //140813
, air_temperature_min       (air_temperature_min_)                               //140813
, air_temperature_avg       (air_temperature_avg_)                               //140813
, solar_radiation           (solar_radiation_)                                   //140813
, vapor_pressure_deficit_max        (vapor_pressure_deficit_max_)                //140813
, psychrometric_constant            (psychrometric_constant_)                    //140508
, soil_salinity(soil_ ? soil_->ref_salinity():0)                                 //160412_140812
, canopy_leaf_growth(0) // set below                                             //050823
, water_canopy_expansion_factor(1.00)                                            //080806
, stress_adj_hourly_temperature_estimator       (60) // 60 minute intervals
, stress_adj_hourly_temperature                 (24) // not sure best initialial size
, stress_adj_hourly_temperature_not_applicable  (24) // not sure best initialial size
, seeding_parameters    (seeding_parameters_)                                    //041202
, estimate_root_biomass (false)                                                  //060612
, yesterday_deg_day     (0)                                                      //161110
, vapor_pressure_deficit_stats(STAT_arithmetic_mean_bit)                         //160803_030530
, intercepted_radiation             (0)                                          //084029
, intercepted_radiation_accum       (0)                                          //080429
, intercepted_radiation_season_accum(0)                                          //080429
, attainable_top_growth             (0)                                          //161110
, biomass_growth                    (0)                                          //180111
, biomass_at_filling                (0)                                          //161110
, biomass_at_maturity               (0)                                          //161110
, unclipped_biomass                 (0)                                          //071004

// Reporting and output
, plant_height                      (0)
, roots_current                     (0)                                          //070131
#ifndef CROP_CALIBRATION
, roots_dormant                     (0)                                          //070131
#endif
, roots_vital                       (0)                                          //070131

, pot_root_biomass                  (0)                                          //161110
, act_root_biomass                  (0)                                          //161110

, report_temperature_stress_stats(STAT_arithmetic_mean_bit)                      //160803_010726
, overall_growth_stress             (0)                                          //161110
, yield                             (0)                                          //150825
, initialized                       (false)                                      //031121
, emergence_count                   (0)                                          //190411

, fall_sowing                       (false)                                      //040914
, harvests_this_season              (0)                                          //161110
, no_more_clippings_this_season     (0)                                          //161110
/*200420 moved
, linger_sowing_label               (0)                                          //030521
, linger_tuber_init_label           (0)                                          //081108
, linger_veraison_label             (0)                                          //030521
, linger_harvest_label              (0)                                          //030521
, linger_chill_satisfied_label      (0)                                          //030529
, linger_anthesis_label             (0)                                          //191022
*/
, inactive_end_date                 (0)                                          //170525
, limited_pot_transpiration_m       (0)                                          //161110
, transpiration_dependent_growth                                                 //080721
   (
   #ifdef YAML_PARAM_VERSION
    parameters_->ref_biomass_production()
   #else
   *parameters_
   #endif
   ,daytime_vapor_pressure_deficit
   ,CO2_TUE_adjustment                                                           //151030
   ,before_WUE_change                                                            //180111
   )
, latest_harvest_date               (0)                                          //170525
, unexpected_termination_date       (0) //must be 0                              //170525

, biomass_fate_today                (0)                                          //NEW_BF
, biomass_fate_season               (0)                                          //NEW_BF
, terminate_pending                 (0)                                          //161110
//200603moved, fruit_harvested                   (0)                                          //161110
, days_of_expected_dormancy_remaining  (0)                                       //161110
, C_mass_canopy_est                 (0)                                          //110915_060818
, C_mass_root_est                   (0)                                          //110915_060818

, CO2_current_conc_optional                  (atmospheric_CO2_conc_ppm_optional) //180119
, CO2_TUE_adjustment                (1.0)                                        //150907
, stress_adjusted_temperature       (0.0)                                        //161110
#if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
, vernalization                                                                  //181118
   (parameters_->vernalization.enabled                                           //181118
      ? new Thermal_time_common::Vernalization                                   //181118
         (parameters_->vernalization
         ,temperature_with_est_night)
      : 0)                                                                       //181118
, photoperiodization                                                             //181118
   ((parameters_->photoperiod.photoperiodism_cowl.get() > no_photoperiod)        //181118
      ? new Thermal_time_common::Photoperiodization                              //181118
         (parameters_->photoperiod,ref_day_length_hours_)                        //181118
      : 0)                                                                       //181118
, thermal_time
  ( (parameters->thermal_time.thermal_response ==linear)                         //151109
    ? ((Thermal_time_common *)(new Thermal_time_daily_linear
      (parameters_->thermal_time
      ,stress_adjusted_temperature.ref_Celcius()                                 //190812
      ,air_temperature_min.ref_Celcius()                                         //190812
      ,parameters_->is_fruit_tree()                                              //190625
      ,vernalization                                                             //181118
      ,photoperiodization                                                        //181118
      )))
   : ((Thermal_time_common *)(new Thermal_time_daily_nonlinear
      (parameters_->thermal_time
      ,stress_adjusted_temperature.ref_Celcius()                                 //190812_140626
      ,air_temperature_min.ref_Celcius()                                         //190812_140626
      ,parameters_->is_fruit_tree()
      ,vernalization                                                             //181118
      ,photoperiodization                                                        //181118
      )))
   )
#endif
//181116 in PHENOLOGY_2018 each period has its own thermal time
#if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
, phenology                                                                      //130904
   (parameters_->phenology
   ,thermal_time->ref_thermal_time_cumulative()
   ,stress_adjusted_temperature.ref_Celcius()                                    //190812_190701
   ,air_temperature_min.ref_Celcius()                                            //190812_190701
   ,parameters->vernalization.enabled                                            //190701
      ? new Thermal_time_common::Vernalization
         (parameters_->vernalization
         ,temperature_with_est_night)
      : 0
   ,(parameters->photoperiod.photoperiodism != no_photoperiod)                   //190701
      ? new Thermal_time_common::Photoperiodization
         (parameters_->photoperiod
         ,ref_day_length_hours_)
      : 0
   ,parameters_->is_fruit_tree()                                                 //190701
   ,parameters_->is_perennial()
   ,simdate_raw)                                                                 //181108
#endif
#if ((PHENOLOGY_VERSION==2018))
, phenology                                                                      //130904
   (parameters_->phenology
//    #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
///   ,thermal_time->ref_thermal_time_cumulative()
//   #endif
   ,stress_adjusted_temperature//_max
   ,air_temperature_min
   ,parameters->vernalization.enabled
      ? new Thermal_time_common::Vernalization
         (parameters_->vernalization
         ,temperature_with_est_night)
      : 0
   ,(parameters->photoperiod.photoperiodism != no_photoperiod)
      ? new Thermal_time_common::Photoperiodization
         (parameters_->photoperiod
         ,ref_day_length_hours_)
      : 0
   ,parameters_->is_fruit_tree()
   ,parameters_->is_perennial()
   ,simdate_raw)                                                                 //181108
#endif
, emergence                   (0)                                                //041201
, emergence_hydrothermal_time (0)                                                //041201
, quiescence_start_requirement(0)                                                //160326_080401
, quiescence_end_requirement  (0)                                                //160326_080401
   // will be set once dormancy (or end of season) start
, nitrogen(0)                                                                    //050722
, N_mass_canopy_recorded_before_harvest(0)                                       //070209
, ref_event_log(0)                                                               //021007
, ref_residues(0)  // Set by know residues (currently only applies to V4_4_ORGANIC_MATTER_AND_RESIDUE) //060531
, report_transpiration_use_eff         (0)                                       //161110
// Special
#ifdef CROPSYST_PROPER
, report_degree_hours_above_30_today   (0)                                       //161110
, report_degree_hours_above_34_today   (0)                                       //161110
,above_30C_season    (0)                                                         //180814
,above_35C_season    (0)                                                         //180814
,above_30C_flowering (0)                                                         //180814
,above_34C_flowering (0)                                                         //180814
#endif

,param_saturated_soil_root_activity_coef(param_saturated_soil_root_activity_coef_)//191011
,param_dry_soil_root_activity_coef          (param_dry_soil_root_activity_coef_) //191011
{
   #ifdef NITROGEN
   // We must instanciate nitrogen first because it is needed to instanciate canopy
   switch (nitrogen_model_)                                                      //050722
   {
      case CROP_NITROGEN_ORIGINAL :                                              //050722
         nitrogen = new
            Crop_nitrogen_V5                                                     //151106
            ((parameters->nitrogen)
            ,*this,soil_,soil_->mod_nitrogen()                                   //160412
            ,slope_saturated_vapor_pressure_plus_psychrometric_resistance_reference //150602
            ,transpiration_dependent_growth.ref_transpiration_use_efficiency_CO2_adjusted()
            #if ((PHENOLOGY_VERSION==2018))
            ,phenology.ref_accum_deg_days_adjusted_for_clipping()                //181118
            #endif
            );                                                                   //151104
      break;
      default : break; // just leave nitrogen 0 (disabled)
   }
   #endif
   // Nitrogen must exist before we create canopy
   {
      CORN::Date_clad_32 yesterday(simdate_raw); yesterday.dec_day();            //170525_030722
      float64 Tmax_yesterday  = weather.get_air_temperature_max_yesterday();     //151201
      float64 Tmin_tomorrow   = weather.get_air_temperature_min_tomorrow();      //151201
      // Must get yesterday first, it could have data quality different from today  //040510
      float64 Tmax_today      = air_temperature_max.Celcius();                   //151201
      float64 Tmin_today      = air_temperature_min.Celcius();                   //151201
      stress_adj_hourly_temperature_estimator.reset
         (Tmax_today, Tmax_yesterday, Tmin_today, Tmin_tomorrow);                //030610
   }
   switch (parameters->harvested_part_clad.get())
   {  case grain_part :
      {
         Yield_grain_water_response *water_reponse                               //200423
            = (parameters->yield.apply_water_response_flowering)                 //200423
            ? new Yield_grain_water_response
               (parameters->yield.water_stress_sensitivity                       //200424
               ,water_stress_stats.ref_current())                                //200423
            : 0;                                                                 //200423
         Yield_grain_temperature_response * temperature_response                 //200423
            = (parameters->yield.apply_temperature_response_flowering)           //200423
            ? new Yield_grain_temperature_response                               //200423
               (parameters->yield.temperature_stress_sensitivity                 //200424
               ,parameters->yield.response_temperature_optimum                   //200423
               ,parameters->yield.response_temperature_damaging                  //200423
               ,air_temperature_max,air_temperature_min)                         //200423
            : 0;                                                                 //200423
         yield = new Yield_grain
            (parameters->yield.harvest_index_unstressed
            ,parameters->yield.apply_adjustment_to_unstressed_harvest_index
            ,parameters->yield.apply_temperature_extreme_response
                  //200423 apply_heat_extreme_adjustment_to_unstressed_harvest_index
            ,parameters->yield.translocation_fraction_max
            ,water_reponse                                                       //200423
            ,temperature_response                                                //200423
            ,ref_phenology()
            ,sun_rise_time                                                       //150907
            ,stress_adj_hourly_temperature);
      } break;
      case tuber_part :
         yield = new Yield_tuber
            (parameters->yield.harvest_index_unstressed
            ,parameters->yield.apply_adjustment_to_unstressed_harvest_index
            ,parameters->yield.stress_sensitivity_tuber_init
            ,parameters->yield.tuber_growth_sensitivity
            ,parameters->tolerance.temperature_heat_tuber_critical
            ,ref_phenology()
            ,air_temperature_avg);
               // currently just using air temperature to represent soil temperature
               // In version 5 soil temperature should be available
      break;
      case HARVEST_CANOPY :  // essentially all above (95%) ground biomass       //031015
         yield = new Yield_canopy
            (parameters->yield.harvest_index_unstressed
            ,parameters->yield.apply_adjustment_to_unstressed_harvest_index
            ,ref_phenology());
      break;
      case HARVEST_COMPLETE :
         // All biomass including roots                                          //031015
         std::clog << parameters->description.brief << std::endl;
         std::clog << "Harvest complete yield model not yet implemented." << std::endl;
         std::clog << "This is not necessarily an error, I.e. when harvest management is clipping." << std::endl;
         yield = new Yield_complete;                                           //140929
      break;
      case UNKNOWN_PART :
         std::cerr << parameters->description.brief << std::endl;
         std::cerr << "Crop parameter file harvest part unknown, part must be selected" << std::endl;
         assert(false); //NYI
      break;
      case fruit_part :
      break;
      case root_part :
         // NYI In the past this has been using the harvest index against above ground biomass
         // but is should be the root biomass.
      case bulb_part :  // NYI
         // The bulb is considered above ground biomass even though
         // it is mostly subsurface (it is not root).
      case leaf_part :  // NYI
      default:
         yield = new  Yield_simple_harvest_index
            (parameters->yield.harvest_index_unstressed
            ,parameters->yield.apply_adjustment_to_unstressed_harvest_index
            ,parameters->yield.stress_sensitivity_growth
            ,ref_phenology());
      break;
   } // switch harvested_part
   if (event_scheduler) delete event_scheduler; event_scheduler = 0;             //201129LML
   event_scheduler = as_event_scheduler_crop = new Event_scheduler_crop_complete
      (*this,simdate,performable_operations_);
}
//_constructor_________________________________________________________________/
Crop_complete::~Crop_complete()
{
   if (emanator_known) emanator_known->set_status(CS::Emanator::deleted_status); //160614
   // Note that in V4 emanator_known will be 0 (NYI)
   // but should have been instanciated in V5

   delete quiescence_start_requirement;
   delete quiescence_end_requirement;
   delete biomass_fate_today;
   delete biomass_fate_season;
   delete nitrogen;                                                              //070221
   delete canopy_leaf_growth;                                                    //070626
   delete roots_current;                                                         //141103
   delete CO2_response;
   delete yield;                                                                 //201128LML
   #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
   delete thermal_time;                                                          //180522
   #endif
}
//_destructor_______________________________________________________2002-04-09_/
bool Crop_complete::initialize()                                 initialization_
{
   bool initialized = Crop_common::initialize();                                 //031130
   linger_label.planting   = -1;                                                 //200420
   //200420 linger_label.anthesis   = -1;                                                 //200420
   linger_label.tuber_init = -1;                                                 //200420
   linger_label.harvest    = -1;                                                 //200420

   #ifdef YAML_PARAM_VERSION
   // will be using new parameter adjustment idiom
   #else
   as_event_scheduler_crop->take_events(parameters->adjustments);                //151004
   as_event_scheduler_crop->take_events(parameters->yield.grain_stress_sensitivity_periods); //151004
   #endif
   as_event_scheduler_crop->take_events(parameters->associated_events);          //151004

         // Warning this does not allow crop parameters to be
         // shared/referenced                                                    //160916

   intercepted_radiation                  = 0;                                   //080428
   intercepted_radiation_accum            = 0;                                   //080428
   intercepted_radiation_season_accum     = 0;                                   //080428
   N_mass_canopy_recorded_before_harvest  = 0;                                   //110915

   plant_height                           = 0;                                   //990201
   pot_root_biomass                       = 0;                                   //010713
   act_root_biomass                       = 0;                                   //010713
   act_transpiration_m                    = 0;                                   //011022
   intercepted_precipitation_m_today      = 0;                                   //160414
   //200520 water_stress_stats                     .reset();                     //010726
   water_stress_stats                     .clear(1.0);                           //200529
   report_temperature_stress_stats        .reset();                              //010726
   // no need to reset because instanciated for each crop instance
   // however tubers could be perennial do may want to have Yield.reset()
   if (yield) yield->reset();                                                    //150825
   overall_growth_stress                  = 0;
   biomass_at_filling                     = 0;                                   //131022
   biomass_at_maturity = 0;                                                      //130510
   #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
   act_transpiration_m                    = 0;                                   //011022
   phenology.growth_stage = NGS_NONE;
   #endif
   phenology.reset();                                                            //131202
   latest_harvest_date        .set_YD(Year(0),CORN::DOY(0));
   delete biomass_fate_today;    biomass_fate_today = 0;  // note, should alway be 0
   delete biomass_fate_season;   biomass_fate_season = 0; // note, should alway be 0
   days_of_expected_dormancy_remaining = 0;                                      //980723
   terminate_pending = false;                                                    //040514
   //200603 moved fruit_harvested = false;                                                      //030529
   pot_transpiration_m_CO2_adj = pot_transpiration_m_unadj = 0;                  //190826
   #ifdef NITROGEN
   if (nitrogen) initialized &= nitrogen->initialize();
   #endif
      // set water stress to 0 because root calculation is occuring before stress
      // is calculated on first day.

   #ifdef CROPSYST_PROPER
   stress_ascertation_determination();                                           //180914
   #endif
   consecutive_low_growth_days_for_clipping = 0;                                 //190616_970219
   days_since_last_clipping = 0;
   max_daily_GAI_rate = 0.0;    //  for clipping                                 //190616_970219
   automatic_clipping_enabled = true;                                            //190616_980724
   return initialized;
}
//_initialize__________________________________________________________________/
#ifdef CROPSYST_PROPER
bool Crop_complete::stress_ascertation_determination()
{
   // These are for special outputs.
   // They are actually used to determine extreme tolerance parameters.
   // This procedure should actually be done outside of CropSyst
   // in a standalone utility
   above_30C_season = new CS::Stress_ascertaination
      (parameters_above_30C_season,ref_phenology()
      ,stress_adj_hourly_temperature,simdate_raw);

   above_35C_season = new CS::Stress_ascertaination
      (parameters_above_35C_season,ref_phenology()
      ,stress_adj_hourly_temperature,simdate_raw);

   above_30C_flowering = new CS::Stress_ascertaination
      (parameters_above_30C_flowering,ref_phenology()
      ,stress_adj_hourly_temperature,simdate_raw);

   above_34C_flowering = new CS::Stress_ascertaination
      (parameters_above_34C_flowering
      ,ref_phenology()
      ,stress_adj_hourly_temperature,simdate_raw);

   return submit(above_30C_season)
       && submit(above_35C_season)
       && submit(above_30C_flowering)
       && submit(above_34C_flowering);
}
//_stress_ascertation_determination_________________________________2018-09-14_/
#endif
bool Crop_complete::start()
{  bool started = true;                                                          //140619

   // this is correcting something wrong with efficiency_model resetting.
   parameters->biomass_production.efficiency_model =
      (parameters->biomass_production.efficiency_model_raw.find("TUE") != std::string::npos)
      //200317 (parameters->biomass_production.efficiency_model_raw == "TUE")
//      (parameters->biomass_production.efficiency_model_raw == "TUE")
      ? TUE_efficiency
      : RUE_efficiency;

   //NYN started &= phenology   .start();                                              //181227
   started &= Crop_common::start();                                              //181227
      // actually should have been called in PHENOLOGY_2013
   if (parameters->is_perennial()) started &= create_roots();                    //190102
   if (!quiescence_end_requirement // Crop_orchard_fruit may have already set this up
       && parameters->continuation_mode_clad.get() > single_season)              //200213
   {
      // This > will include both multiple_season and dormancy_season models
      quiescence_start_requirement = new Quiescence_start_requirement            //160326
         (parameters->quiescence,parameters->is_fruit_tree());                   //190625
   }
   if (is_established())                                                         //190708
   {  plant_height = parameters->morphology.max_canopy_height;                   //190708
   } else
   {
      plant_height       = 0.0;                                                  //990213
      linger_label.planting = 1; // 3;                                           //030521
      trigger_synchronization(NGS_PLANTING);                                     //030516
      phenology.activate_sowing();                                               //181108
      trigger_synchronization(NGS_GERMINATION);                                  //170406
      #if (CROPSYST_VERSION==4)
      switch (parameters->emergence.emergence_model_cowl.get())                  //041201
      {                                                                          //041201
         #ifdef SEED_EMERGENCE_MODEL
         case EMERGENCE_HYDROTHERMAL_TIME    :                                   //041201
            emergence = emergence_hydrothermal_time
            = new Crop_emergence_hydrothermal_time                               //041201
             (parameters->emergence
             ,parameters->canopy_growth_LAI_based.specific_leaf_area
             ,seeding_parameters);                                               //041201
         break;
       #endif
//conceptual I would like to make the original thermal time mode a separate model SEED /*041201_*/     default /*EMERGENCE_THERMAL_TIME*/  : emergence = emergence_thermal_time      = new Crop_emergence_thermal_time();      break;
      } // switch
      #endif
   } //else

   started &= start_season();                                                    //190826
   started &= start_day();                                                       //190812
   // Need to call start_day because normal elements start_day has already
   // been processed, but crop may be instanciated after this has occurred
   // but so we need to setup the crops daily stuff.
   return started;
}
//_start____________________________________________________________2007-11-20_/
bool Crop_complete::plant(bool fall_sowing_)
{  bool planted = true;                                                          //190703
   fall_sowing = fall_sowing_;                                                   //190621
   harvests_this_season = 0;                                                     //040624
   no_more_clippings_this_season = false;                                        //040721
   #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
   phenology.activate_sowing();                                                  //181108
   #endif
   terminate_pending = false;                                                    //070711
   planted &= start();                                                           //190703_071120
   as_event_scheduler_crop->synchronize                                          //151004
      (Synchronization(false, NGS_PLANTING),simdate);
   return planted;                                                               //190703_020711
}
//_plant____________________________________________________________2002-02-22_/
bool Crop_complete::in_growing_season()                             affirmation_
{  return phenology.is_in_growing_season() && (!biomass_fate_today);             //130903
}
//_in_growing_season___________________________________________________________/
nat8 Crop_complete::describe_periods(CORN::Text_list &descriptions)        const
{
    bool test = true;
    //std::cerr << "test=describe_periods " << test << "\n";


   #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
   descriptions.add_cstr(get_growth_stage_description
      (phenology.growth_stage,parameters->is_fruit_tree()                        //120820_200212_080910
      ,(parameters->continuation_mode_clad.get() == dormancy_season)));           //200421
   #endif

   if (linger_label.planting  >= 0) descriptions.add_cstr("planting");
   if (linger_label.tuber_init>= 0) descriptions.add_cstr("tuber(init)");
   if (linger_label.harvest   >= 0) descriptions.add_cstr("harvest");

   if (phenology.get_accrescence_period())   descriptions.add_cstr("accrescence");
   if (phenology.get_culminescence_period()) descriptions.add_cstr("culminescence");
   const Phenology_abstract::Period_thermal *senescence_period
      = phenology.get_senescence_period();
   if (senescence_period)
   {  std::string scence_phrase("senescence");
      if (senescence_period->has_expired())
         scence_phrase += "(expired)";
      descriptions.add_string(scence_phrase);
   }
   return descriptions.count();
}
//_describe_periods_________________________________________________2020-04-20_/
/*200420
const char *Crop_complete::__describe_growth_stage
(std::string &growth_stage_phrase)                                         const
{

   //abandoned nat16 phrase_length_start = growth_stage_phrase.length();
   #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
   if (!growth_stage_phrase.empty()) growth_stage_phrase += " & ";               //191022
   growth_stage_phrase +=
      get_growth_stage_description
      (phenology.growth_stage,
       parameters->is_fruit_tree());                                             //120820

   #endif
   std::string linger_phrase;
   if (linger_label.planting  >= 0) {linger_phrase= "planting";    }
   if (linger_label.tuber_init>= 0) {linger_phrase= "tuber init"; }
   if (linger_label.harvest   >= 0) {linger_phrase= "harvest";   }  // must follow fruit_harvested check
   // if (linger_label.anthesis  >= 0) {growth_stage_phrase += "anthesis";}             //191022
   if (!linger_phrase.empty())
   {
      if (!growth_stage_phrase.empty()) growth_stage_phrase += " & ";            //191022
      growth_stage_phrase += linger_phrase;
   }
   std::string scence_phrase;
   if (phenology.get_accrescence_period())   scence_phrase = "accrescence";      //191022
   if (phenology.get_culminescence_period()) scence_phrase = "culminescence";    //191022
   const Phenology_abstract::Period_thermal *senescence_period
      = phenology.get_senescence_period();                                       //200304
   if (senescence_period)                                                        //200304
   {  scence_phrase = "senescence";                                              //191022
      if (senescence_period->has_expired())                                      //200304
         scence_phrase += "expired";                                             //200304
   }
   if (!scence_phrase.empty())
   {
      if (!growth_stage_phrase.empty()) growth_stage_phrase += " & ";               //191022
      growth_stage_phrase += scence_phrase;
   }
   return growth_stage_phrase.c_str();
}
//_describe__growth_stage____________________________________________2003-05-21_/
*/
bool Crop_complete::start_inactive_period(bool apply_dormancy)                   //140625_080401
{
   bool started = true;
   trigger_synchronization(NGS_QUIESCENCE);                                      //130902_021217
   // full_crop class with set biomass to residue basic crop doesn't do this     //970219
   intercepted_radiation = 0.0;                                                  //080429
   intercepted_radiation_accum = 0.0;                                            //080429
   bool dormancy_is_applicable
      = (parameters->continuation_mode_clad.get() == dormancy_season);           //200212_080910
   float64 dormancy_senesce = (                                                  //970219
      dormancy_is_applicable ? parameters->dormancy.senesce : 0.0);              //080331
   if (canopy_leaf_growth)                                                       //190701
   {
   if (CORN::is_approximately<float64>(dormancy_senesce,100.0,0.00001)
       #if (CROPSYST_VERSION == 4)
       || !canopy_leaf_growth
       #endif
       )                                                                         //060317
      drop_leaves();                                                             //190625
   else canopy_leaf_growth->end_season(apply_dormancy);                          //080401
   }
//180615   #ifdef YAML_PARAM_VERSION
   started &= as_event_scheduler_crop->end_season();                             //150930
//   #endif
#ifdef NITROGEN
   if (nitrogen) nitrogen->start_dormancy();
      // This may need to apply to the inactive period, not just dormancy        //080321
#endif
   started &= end_season();                                                      //160618

   #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
   //It is necessary to clear thermal time at the start of the inactive
   //period otherwise, for fruit crops, chilling evaluation uses thermal time
   //and if there is residual thermal time, the chilling requirement will be
   //immediately satisfied.

   thermal_time->clear_accum_degree_days();  // <- IMPORTANT TO DO THIS          //000719
   #endif
   phenology.activate_quiescence();                                              //181108
   return started;
}
//_start_inactive_period____________________________________________1997-02-19_/
Canopy_leaf_growth &Crop_complete::provide_canopy()                   provision_ //190702
{
   {
   delete canopy_leaf_growth; canopy_leaf_growth = 0;
   switch (parameters->morphology.canopy_growth_model)                           //080725
   {  // NYI case  : canopy = new xxxxx(); break;
#ifndef CROP_CALIBRATION
      case canopy_cover_based_2017 :                                             //171219_080805
      {  Canopy_growth_cover_based *canopy_growth_cover_based =                  //110904
            new  Canopy_growth_cover_based
            (*parameters
            ,parameters->canopy_growth_cover_based
            ,ref_residues,nitrogen
            ,phenology
            ,water_stress_stats.index
            // WARNING need to check if this will be the previous days value when canopy cover is updated
            ,water_canopy_expansion_factor
            #ifdef CO2_CHANGE
            , CO2_response
            #endif
            );                                                                   //080805
         canopy_leaf_growth = canopy_growth_cover_based;                         //110904
      } break;
#endif
      #if (CROPSYST_VERSION==4)
      case leaf_area_index_based_duration:                                       //080725
      {
         canopy_leaf_growth = new
            CropSyst::Canopy_growth_leaf_area_index_based_V4
            (
            *parameters
            ,parameters->canopy_growth_LAI_based
            ,ref_residues,nitrogen                                               //060816
            ,reported.season.LAI_max                                             //190705
               // should be using reported variable
            ,CO2_response);                                                      //080728
      } break;
      #endif
      case leaf_area_index_based : default:                                      //080725
      {
         canopy_leaf_growth = new
            CropSyst::Canopy_growth_leaf_area_index_based_V5
            (*parameters
            ,parameters->canopy_growth_LAI_based
            ,ref_residues,nitrogen
            ,reported.season.LAI_max                                             //190705
               // should be using reported variable
            ,CO2_response);
      } break;
     } // switch  canopy_growth_labeled
     bool canopy_initialized = canopy_leaf_growth && canopy_leaf_growth->initialize(); //080805
   }
   return *canopy_leaf_growth;                                                   //190702
}
//_create_canopy____________________________________________________2006-05-31_/
bool Crop_complete::drop_leaves()                                                //190624
{
   delete canopy_leaf_growth; canopy_leaf_growth = 0;
   return true;
}
//_drop_leaves______________________________________________________2019-06-25_/
bool Crop_complete::create_roots()
{
   if (soil)                                                                     //061208
   {
      float64 initial_root_length                                                //180727
         = cm_to_m(parameters->root.length_at_emergence_cm);                     //180727
      if (parameters->root.initial_depth_deprecated > 0.00001)                   //180727
      {
         std::clog << "Warning: the crop parameter file uses old initial root depth, using old initial root length determination." << std::endl;

      float64 depth_emergence =
         parameters->root.get_depth_emergence_m_deprecated();                    //170105
      float64 soil_thickness_1 = soil->ref_layers()->get_thickness_m(1);         //170105
      float64 initial_root_length                                                //120817
         = depth_emergence - soil_thickness_1;                                   //170105_120817
      if ( initial_root_length < 0) initial_root_length = depth_emergence;       //170105
      }                                                                          //180727
      if (canopy_leaf_growth)                                                    //190701
          canopy_leaf_growth->get_biomass_current(include_vital);                //130624
      Phenology::Period_thermal *root_elongation_period
      = phenology.activate_root_elongation();                                    //190102_181108
      root_elongation_period->start_day();
      if (is_established())                                                      //190707
         initial_root_length  = parameters->root.max_depth;                      //190707
      roots_current = roots_vital =
            dynamic_cast<Crop_root_vital *>                                      //130625
            (new Crop_root_vital_V5
            (parameters->root
            ,*root_elongation_period                                             //181108
            ,*soil->ref_layers()
            ,initial_root_length));
   } else phenology.activate_root_elongation();                                  //191121

   #ifdef CS_OBSERVATION
   /* Need to reimplement
   roots_current  ->render_inspectors(inspectors,emanator,"root");
   */
   #endif

   return true;
}
//_create_roots_____________________________________________________2019-01-02_/
bool Crop_complete::senesce_at_dormancy()
 // This is truly only for dormancy, not inactive_period                         //080321
{
   float64 dormancy_senesce =                                                    //060317
      ((parameters->continuation_mode_clad.get() == dormancy_season)             //200212_080331
         ? parameters->dormancy.senesce : 100.0);                                //080331
   {  Biomass_fate_parameters biomass_fate =                                     //040621
      { 0//remove_for_consumption                                                //040815
      , 0//remove_as_grazing                                                     //040610
      ,90//remain_as_residue                                                     //040610
      , 0//remove_for_disposal                                                   //040610
      ,10//remain_standing_dead                                                  //040610
      , 0//remain_standing_live                                                  //040610
      ,50//remain_roots_as_residue // Claudio says to use 50% for now,           //040621
      ,50//remain_roots_live                                                     //040621
      , 0//remove roots as loss                                                  //160830
      , 0//return_as_manure                                                      //040908
      };                                                                         //040621
      if (!CORN::is_approximately<float64>(dormancy_senesce,100.0,0.00001))      //070313
      {  biomass_fate.remain_as_dead = parameters->dormancy.senesce;             //070313
         biomass_fate.remain_as_residue = 0.0;                                   //070313
         biomass_fate.remain_as_live = 100.0 - parameters->dormancy.senesce;     //070313
      }                                                                          //070313
      Crop_mass_fate_biomass_and_nitrogen *fate =                                //070625
      process_biomass_fate                                                       //040621
         (DORMANCY_CAUSE
         ,OTHER_BIOMASS_FATE,0.0,0.0,0.0  // This is not a harvest event
         ,biomass_fate
         ,false,0.0);
//070626          WARNING need to check that fate is accounted for
//                (at least the fate that removes
//                Don't delete this message until this if verified.
      if (fate) delete fate;                                                     //060323
   }
   return true;
}
//_start_dormancy___________________________________________________1997-02-19_/
float64 Crop_complete::est_root_biomass()                                  const
{  // Way want to revisit this, if we have an explicit daily total biomass
   // accumulation model with top and root partitioning.                         //010713
   const Phenology::Period_thermal *root_elongation_period
      = phenology.get_root_elongation_period();
      // may want to make root growth period a property (reference)
   float64 root_to_top_ratio
      = root_elongation_period
      ? (0.75 - 0.6 * root_elongation_period->get_thermal_time_relative_elapsed())
      : 1.0;
  float64 ested_root_biomass // kg/m2
     = std::max<float64>(act_root_biomass,get_canopy_biomass_kg_m2_including
         (include_vital) * root_to_top_ratio);
      // RLN Warning I think this need to be canopy biomass_produced (including the amount decomposed) and actually produced during the season (or ever?)
      // Because I dont think roots senesce or diminish from clipping
  return ested_root_biomass;
}
//_est_root_biomass_________________________________________________2006-06-12_/
extern float64 debug_Daily_Direct_Row_Int ;
extern float64 debug_Daily_Diffuse_Row_Int ;
//______________________________________________________________________________
void Crop_complete::update_lingers()                               modification_ // augments adjuncts
{  linger_label.planting--;                                                      //200420_030521
   //moved to fruit     linger_label.budbreak--;                                 //200420_030521
   linger_label.harvest--;                                                       //200420_030521
   linger_label.tuber_init--;                                                    //200420_081111
   //200420 linger_label.anthesis--;                                                      //200420
}
//_update_lingers___________________________________________________2007-11-20_/
bool Crop_complete::actual_growth()
{  bool completed = true;                                                        //190709
   //200420 moved to start_day update_lingers();
   float64 act_biomass_growth = 0.0;
   float64 GAI_related_biomass_growth = 0;                                       //980206

// we must have canopy now    if (canopy_leaf_growth)                                                           //190701
// caller already checks
   float64 evap_layer_thickness = soil ? soil->ref_layers()->get_thickness_m(1) : 0.05;
         // alias growth_stage here is just for convenience during my transition it should be growth_stage_sequence
      // actual_growth is only called if canopy_leaf_growth exists
      {  bool allow_fruit_crop_growth_during_maturity
            =  (phenology.is_maturing())
            && parameters->is_fruit_tree();                                      //190625
         if (!phenology.has_matured_new()
             || allow_fruit_crop_growth_during_maturity)                         //011117
         { // For fruit model, we allow additional biomass growth during maturity 030523
            act_biomass_growth = calc_act_biomass_growth                         //011022
               (evap_layer_thickness                                             //010202
               ,GAI_related_biomass_growth);                                     //011022
            C_mass_canopy_est = act_biomass_growth * get_carbon_fraction();      //110915_060818
         } else
         {  overall_growth_stress = 1.0; // will be updated if we call
            #ifdef NITROGEN
            if (nitrogen &&
               !phenology.is_in_quiescence())                                    //181108
                nitrogen->process_any_gaseous_losses();                          //120630
            #endif
            // Now N gaseous losses may continue throughout maturity (and dormancy)
            // Note that N gaseous loss during active growth is handled by actual biomass growth   //070202
         }
      }
      float32 min_water_or_N_stress =  (float32)get_min_water_N_stress();        //081109
         // use float32 because stats is float32
      if (yield) completed &= yield->know_stress(min_water_or_N_stress);         //190709_150825
         // This will use either the default straw_to_residue,
         // or the latest set by the management harvest operation 030808
      {  bool not_dormant_or_inactive  = !phenology.is_in_quiescence();           //181108
         float64 thermal_time_growing_degree_day = THERMAL_TIME_SOURCE get_growing_degree_day();    //080728
         completed &= canopy_leaf_growth->age_leaf                               //190709
            (thermal_time_growing_degree_day,water_stress_stats.index);          //080726
         float64 GAI_vital = 0;                                                  //200122
         if (not_dormant_or_inactive)                                            //080725
         {  GAI_vital = get_GAI_vital();                                         //200122 170818

         if (GAI_vital > reported.season.GAI_max)                                //190705_070320
         {  reported.season.GAI_max_date.set(simdate);                           //190705_180413
            reported.season.GAI_max = GAI_vital;                                 //190705_070320
         }
         float64 leaf_and_branch_growth
            // RLN - I think this should actually only be leaf                   //071120
            = distribute_todays_non_leaf_stem_growth(act_biomass_growth);        //071120
         /*std::clog << "act_biomass_growth:" << act_biomass_growth
                   << "\tleaf_and_branch_growth:" << leaf_and_branch_growth
                   <<std::endl;*/
         bool has_leaf_senescence = parameters->has_leaf_senescence();
         bool continue_leaf_growth =                                             //000616
               (phenology.is_in_accrescence()  // or accrescence?
               || !has_leaf_senescence)                                          //030521
               && !is_limit_max_LAI();                                           //081103

         /*std::clog << "continue_leaf_growth:" << continue_leaf_growth
                   << "\tGAI_related_biomass_growth:" << GAI_related_biomass_growth
                   <<std::endl;*/


         //170316LML may need to consider the possibility that GAI_related_biomass_growth
         //is greater than leaf_and_branch_growth (e.g. fruit trees)

         if (canopy_leaf_growth->develop_leaf
               (continue_leaf_growth,                                            //000407
               leaf_and_branch_growth                                            //071120
               ,GAI_related_biomass_growth
               ,thermal_time_growing_degree_day))                                //080726
            reported.season.biomass_production += act_biomass_growth;            //190705_080728
         }
      if (!phenology.has_matured_new()                                           //181108
          &&parameters->tolerance.enabled)                                       //120117
         check_hardiness();
      //NYI else allow_natural_expiration(today);  Not sure how to setup natural expiration of plant

         reported.daily.LAI_for_self_shading_response =
         canopy_leaf_growth->update_LAI_for_self_shading_response                //13022?
            (phenology.has_expired_accrescence()
             ||(  phenology.is_in_culminescence()
                ||phenology.is_in_senescence())
            ,!phenology.is_matured());                                           //181108
      }

      #ifdef THERMAL_TIME_OBSOLETE_2020
      //190423 Im already canopy_leaf_growth above, probably could
      // combine the if block, but need to see what I am doing in check hardiness
      #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
      //190702 caller already checks if (canopy_leaf_growth)                     //080725
         reported.daily.LAI_for_self_shading_response                            //190705
            = canopy_leaf_growth->update_LAI_for_self_shading_response           //13022?
            (phenology.has_expired_accrescence()                                 //190628
            ,(float32)(thermal_time->get__accum_degree_days(true)));
      #else
         //190423 moved into existing if block above
      #endif
      #endif
   // This is act_root_biomass because it is based on todays final canopy biomass.    //010713
     act_root_biomass                                                            //010713
         = (soil && roots_current)                                               //190102_060615
         ?  roots_current->get_biomass()                                         //060615
         : est_root_biomass();  // <- This is the case for the Crop Calibrator (actually root biomass is not used for Crop calibrator. //060615_
      // The following line allows fast graph to adapt the scale
   float64 root_biomass_produced_today
         = roots_current ? roots_current->get_growth_biomass() : 0.0;            //070130
   C_mass_root_est = root_biomass_produced_today * get_carbon_fraction();        //060818
   #if (defined (CROPSYST_PROPER) || defined(OFOOT_VERSION))
      CS_UED_max_root_biomass_encountered = std::max<float64>
         (CS_UED_max_root_biomass_encountered,act_root_biomass);
   #endif
   return completed;                                                             //190709
}
//_actual_growth_______________________________________________________________/
float64 Crop_complete::distribute_todays_non_leaf_stem_growth
(float64 todays_growth)                                            modification_
{  return todays_growth;// for normal crops everything goes to leaf stem(branch)
}
//_distribute_todays_non_leaf_stem_growth___________________________2007-11-20_/
void Crop_complete::check_hardiness()                                            //131104
{
   float64 snow_protection_factor = 0.0;                                         //140625
   #ifdef VIC_CROPSYST_VERSION
           // NYI Need snow pack thickness in weather provider
   #else
   float64 typical_canopy_height_m = 0.2; // could be calculated plant height or parameter
   snow_protection_factor = CORN::must_be_between<float64>
      ((1.0 - snow_pack_thickness / typical_canopy_height_m)
      ,0.0,1.0);
   #endif
   //std::cout << "SPF:" << snow_protection_factor << " SP:" << weather.get_snow_pack_thickness() <<std::endl;
   float64 leaf_damage_cold_temp = parameters->tolerance.temperature_cold_leaf_damage;  //130406
   float64 leaf_lethal_cold_temp = parameters->tolerance.temperature_cold_leaf_lethal;  //130406
   float64 min_temperature = air_temperature_min.Celcius();                      //140512
   if (min_temperature < leaf_damage_cold_temp)
   {     if (!parameters->is_perennial() && (min_temperature < leaf_lethal_cold_temp))
         {  if (!unexpected_termination_date.get_date32())                       //170525_000320
               unexpected_termination_date.set(simdate);                         //000320
            if (  (parameters->harvested_part_clad.get() != grain_part) )        //130502_000330
            {  log_event("Freezing death");
               terminate_pending=true;                                           //040514
               end_season();                                                     //160618
                 Biomass_fate_parameters freezing_fate =                         //040622
                 {  0 // remove for consumption                                  //040622
                 ,  0 // remove for grazing                                      //040815
                 ,  0 // remain_as_residue                                       //040622
                 ,  0 // remove_as_loss                                          //040622
                 ,  0 // remain_standing_dead                                    //040622
                 ,100 // remain_standing_live                                    //040622
                 ,100 // remain_roots_as_dead_residue                            //040622
                 ,  0 // remove_roots_as_loss                                    //040622
                 ,  0 // remain_roots_live                                       //040622
                 ,0 };
                  Crop_mass_fate_biomass_and_nitrogen * fate =                   //070625
                     process_biomass_fate(FREEZING_DEATH_CAUSE                   //040622
                        ,OTHER_BIOMASS_FATE,0,0,0,freezing_fate,true,0.0);
                  if (fate) delete fate;                                         //060323
                  trigger_synchronization(NGS_TERMINATED);                       //040506
                  #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
                     growth_stage_at_harvest=phenology.growth_stage;             //040604
                     phenology.growth_stage = NGS_TERMINATED;                    //030516
                  #endif
               } else                                                            //000330
               { // Grain crops may still be salvagable, so we dont clear biomass
                  canopy_leaf_growth->die_back(1.0);  // kill green area index //000330
                  log_event("Freezing death (grain salvagable)");
               }                                                                 //000330
         } else // not killed but still damagable  (this is always the case for perennials)  //000330
         {
               static const float64 damage_curve_adj = 2.0; // Could make this a parameter.  //081103
               float64 fract_leaf_reduction_due_to_freezing = // 1.0 is full damage
                  parameters->is_fruit_tree()                                    //190625
                     // For fruit crops we do not kill  the plant
                     // (it would have to be really cold (I.e. -30 for pomes))
                     // but we do drop the leaves.
                     // We also reduce the maximum potential fruit load
                     // (accounting for the loss of flowers and early fruit growth).
                  ? std::min<float64>                                            //081103
                        (1.0                                                     //081103
                        ,(min_temperature < leaf_damage_cold_temp)               //081103
                           ?pow( (leaf_damage_cold_temp - min_temperature)       //081103
                                /(leaf_damage_cold_temp - leaf_lethal_cold_temp) //081103
                               ,damage_curve_adj)
                           :0.0)                                                 //081103
                  :                                                              //081103
                     ((leaf_damage_cold_temp - min_temperature)
                     /(leaf_damage_cold_temp - leaf_lethal_cold_temp));
               fract_leaf_reduction_due_to_freezing
                  = CORN::must_be_between<float64>(fract_leaf_reduction_due_to_freezing,0.0,1.0);
               fract_leaf_reduction_due_to_freezing *= snow_protection_factor;   //140625
               log_event("Freezing leaf damage");                                //020913
               canopy_leaf_growth->die_back(fract_leaf_reduction_due_to_freezing);//050826
               // may want to remove dead canopy biomass and fate it.
         }
   }
}
//_check_hardiness__________________________________________________1999-11-19_/
float64 Crop_complete::damage_GAI(float64 GAI_damage)              modification_
{  return canopy_leaf_growth                                                     //190702
      ? canopy_leaf_growth->damage_GAI(GAI_damage)
      : 0.0;                                                                     //190702
}
//_damage_GAI_______________________________________________________2015-12-26_/
Crop_mass_fate_biomass_and_nitrogen *
   Crop_complete::harvest_crop
(Harvest_amount_mode harvest_amount_mode                                         //031015
,float64 remove_amount_kg_m2  // if harvest_amount_mode=HARVEST_REMOVE_FIXED_AMOUNT_BIOMASS  else 0.0  //050822
,float64 management_retain_GAI                       // if harvest_amount_mode=HARVEST_RETAIN_GAI else 0.0 //050822
,const Biomass_fate_parameters &harvest_biomass_fate_parameters
,bool terminate)
{  linger_label.harvest = 2; // 4;
   latest_harvest_date.set(simdate);                                             //171116
   /*200603 relevent only to fruits (moved)
   fruit_harvested
      = parameters->is_fruit_tree()                                              //190625
      && (parameters->harvested_part_clad.get() == fruit_part);                  //020409
      // needed to disable latest date to harvest when grapes were harvested
   */
   #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
   growth_stage_at_harvest = phenology.growth_stage;                             //130920_040526
   #endif
   phenology.activate_harvest();                                                 //181108
   trigger_synchronization(NGS_HARVEST);                                         //030516
   float64 biomass_for_harvest = biomass_at_maturity;                            //170817
   if (CORN::is_zero(biomass_for_harvest))               //140211
   {  // This is for a case where the crop isn't reaching
      // maturity with normal thermal time accumulation and
      // management event has forced harvest before maturity occurs.
      biomass_for_harvest = get_canopy_biomass_kg_m2_including                   //140211
         (include_vital|include_effete);                                         //170817
         // include also effete because potatoes can have
   }
   float64 yield_now = 0.0;
   float64 HI = 0.0;                                                             //200603
   if (yield)                                                                    //200603
   {  yield_now =                                                                //040621
         (harvest_amount_mode == HARVEST_CROP_PARAMETER_BASED)                   //101101
         ? yield->calculate(biomass_for_harvest)                                 //150825_071120
         : 0.0;
      HI = yield->get_reported_harvest_index();                                  //160628
      // Don't delete yield, it would be needed for success perennial years.
   }
   if (!CORN::is_zero<float64>(HI) && get_nitrogen())                            //160711
   {  float64 HI_N  = pow(HI,(1.0 - 2.0 * (1.0 - HI) * HI));                     //160628
      float64 canopy_N_mass =  get_nitrogen()->get_mass_canopy_current();        //160628
      reported.season.N_mass_yield = canopy_N_mass * HI_N;                       //190705_160628
   }
   Crop_mass_fate_biomass_and_nitrogen *fate =                                   //070625
   process_biomass_fate((Biomass_fated_cause)HARVEST_ONCE
      ,harvest_amount_mode,remove_amount_kg_m2,management_retain_GAI,0.0
      ,harvest_biomass_fate_parameters,terminate,yield_now);
   // total_season_yield is now biomass_fate_season.removed_as_harvest
   if (output_season_tally.transpiration_act > 0.0)                              //180117
   {  // special output                                                          //160520
      output_season_tally.water_use_efficiency =
            reported.season.biomass_production                                   //190705
          / output_season_tally.transpiration_act;
   }
   return fate;                                                                  //060323
}
//_harvest_crop_____________________________________________________2007-06-25_/
bool Crop_complete::start_season()
{  // This is common both to restart (after dormancy)
   // the the apparent start of the season when the crop does not have dormancy.
   bool started = true;                                                          //140822
   delete biomass_fate_season; biomass_fate_season = 0;                          //170628
   /*200529
   water_stress_stats.reset();                                                   //100202
   water_stress_stats.ref_current() = 1.0;                                       //180410_160803
   report_temperature_stress_stats.reset();                                      //100202
   report_temperature_stress_stats.ref_current() = 1.0;                          //180410
   */
   water_stress_stats.clear(1.0);                                                //200529
   report_temperature_stress_stats.clear(1.0);                                   //200529

   vapor_pressure_deficit_stats.reset();                                         //190708_030530
   harvests_this_season = 0;                                                     //040624
   no_more_clippings_this_season = false;                                        //040721
   unclipped_biomass= 0.0;                                                       //010328
   #ifdef NITROGEN
   if (nitrogen) started &= nitrogen->start_season();                            //140822
   #endif
   //moved to restart total_season_biomass_production_output =initial_biomass;  // May need this in start season
   intercepted_radiation_season_accum         = 0.0;                             //190703_080429
   start_period(0);                                                              //160609
   //200603 moved to fruit fruit_harvested = false;                              //190708_000626
   harvestable = false;                                                          //190710
   // check if fruit_harvested is needed (we have latest_harvest_date
   latest_harvest_date.clear();                                                  //190710
   output_season_tally.transpiration_act = 0;
   mod_phenology().clear_days_since_start_of_harvest();                          //200404
   // Do not call phenology.start_season();
   // bacause something is not cleared correctly
   /*200507 only works for fruit trees
   phenology.start_season();                                                     //200502
   */
   return started;                                                               //140822
}
//_start_season_____________________________________________________2005-08-26_/
bool Crop_complete::clear_day()
{
   reported.daily.clear();
   overall_growth_stress = 1.0;                                                  //190708
   attainable_top_growth = 0;                                                    //180111
   delete biomass_fate_today; biomass_fate_today = 0;                            //170628
   C_mass_canopy_est = 0;                                                        //110915
   C_mass_root_est = 0;                                                          //110915
   return Crop_common::clear_day();
}
//_clear_day________________________________________________________2020-04-13_/

bool Crop_complete::start_day()
{
   update_lingers();                                                             //200420
   bool started_day =                                                            //140717
      clear_day()                                                                //200413
//200413      Crop_common::start_day()                                           //190812_150930
   //190812 temperature must be updated before updating phenology and thermal time
   // when need to update temperatures before starting phenology
   // this probably could be applicable also to 2013 version
   && update_temperatures()
   && phenology.start_day()                                                       //140717_130903
   && thermal_time_event();                                                       //190821

   // Crop calibrator does not need roots                                          061212
   /*200413 moved to clear_day
   reported.daily.clear();
   overall_growth_stress = 1.0;                                                  //190708
   // Thermal time accumulation
   // for crop should be done here so we have thermal time for scheduling

   attainable_top_growth = 0;                                                    //180111
   */
   #ifdef CO2_CHANGE
   // weather maintains the atmospheric CO2 concentration
   if (CO2_response)                                                             //090316
       CO2_response->update();
       // this should only be done when CO2 changes (and first day of simulation)
      //  Note that calc_responses really only need to be called when CO2 changes
   #endif
   if (canopy_leaf_growth && transpiration)                                      //191011_190701
   {  float64 LWP_yesterday = transpiration->get_leaf_water_pot();
         // Since transpiration is updated in process_day transpiration is in yesterdays state
      canopy_leaf_growth->know_LWP_yesterday(LWP_yesterday);                     //110901
   }
   started_day &= roots_current ? roots_current->start_day() : true;             //150601
      // Nitrogen needs roots updated
   #ifdef NITROGEN
   if (nitrogen)
   {  nitrogen->start_day();
      N_mass_canopy_recorded_before_harvest= nitrogen->get_mass_canopy_current();//131007
   }
   #endif
   //  Biomatter fate is used for output, if nothing has
   //  used it by the end of the yesterday, we can dump it.                      //040623
   /*200413 moved to clear day
   delete biomass_fate_today; biomass_fate_today = 0;                            //170628
   C_mass_canopy_est = 0;                                                        //110915
   C_mass_root_est = 0;                                                          //110915
   */

   // Need to synchronize first so that the date is set
   // before going into scheduling (done in start day)
   as_event_scheduler_crop->synchronize_thermal_time                             //150930
      (THERMAL_TIME_SOURCE get_accum_degree_days(/*200211 now default true*/));  //190412
         // We need the value adjusted for clipping
         // because events may be rescheduled based on thermal time.
   float64 accum_deg_days_adjusted_for_clipping
      = THERMAL_TIME_SOURCE get_accum_degree_days();
   before_WUE_change                                                             //080721
      = !parameters->has_leaf_senescence()
         || (parameters->biomass_production.WUE_changes &&                       //110218_100729
             (accum_deg_days_adjusted_for_clipping
              <= parameters->phenology.WUE_change));                             //100730
   // Must call Crop_common::start_day last because we need to
   // first update synchronziation before starting scheduler.                    //200413
   return started_day && Crop_common::start_day();                               //200413
}
//_start_day________________________________________________________2004-02-06_/
bool Crop_complete::end_day()
{  bool ended = true;                                                            //200424
   commit_biomass_fate();                                                        //180530
      // was called by land_unit_sim 180530
   reported.daily.root_depth                                                     //190725
      = roots_current ? roots_current->get_apparent_depth() : 0.0;               //190102_070131
   if (canopy_leaf_growth)                                                       //190423
   {
      reported.daily.GAI = canopy_leaf_growth->get_GAI(include_vital|include_effete); //050823
      reported.daily.LAI = canopy_leaf_growth->get_LAI();                        //160627
      reported.daily.canopy_cover_green
         = (float32)get_canopy_interception_global_green();                      //200228
            //200228 get_fract_canopy_cover_green();                             //160629
      reported.daily.canopy_cover_total                                          //200127
         = (float32)get_canopy_interception_global_total();
         //200228 = (float32)get_fract_canopy_cover_total();
   //NYI reported_canopy_cover_total = (float32)get_fract_canopy_cover_total();  //160629
      reported.season.canopy_cover_max = std::max<float32>                       //180413
         (reported.daily.canopy_cover_green,reported.season.canopy_cover_max);   //180403
      canopy_leaf_growth->end_day();                                             //190821
   }
   #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
   thermal_time->end_day();                                                      //140619
   #endif
   phenology.end_day(); // actually should be in crop::common
#ifdef NITROGEN
   if (nitrogen) nitrogen->end_day();
#endif
/*NYN
#ifndef OLD_PHENOLOGY_SEQUENCER
   Phenology_sequencer::end_day();
#endif
*/
   // Special output 160520
      output_season_tally.Srad.append(solar_radiation.get_MJ_m2());
      output_season_tally.Tmax.append(air_temperature_max.Celcius());
      output_season_tally.Tmin.append(air_temperature_min.Celcius());
      output_season_tally.Tavg.append(air_temperature_avg.Celcius());
      //already have in GS accum output_season_tally.VPD_daytime.append(daytime_vapor_pressure_deficit.get_kPa());
      output_season_tally.transpiration_act += act_transpiration_m; //This should be actual transpriation
      // Special outputs                                                         //160519
  #ifdef CROPSYST_PROPER
   #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
   /*200520
      nat16 days_since_start_of_flowering =  ref_phenology()
            .get_days_since_start_of(NGS_ANTHESIS);
   */
   if (phenology.is_early(NGS_ANTHESIS,7))                                       //181108
   #endif
   #if ((PHENOLOGY_VERSION==2018))
   if (phenology.is_early(phenology.get_anthesis_period_active(),7))                    //181108
   #endif
   {
      if (!output_season_tally.window_14_days_achieved  )                        //160509
      {  // 7 day window on either side of start of flowering
         // (include period before and after start of flowering)
         //const CS::Weather_hours *hourly_weather = meteorology.provide_hourly();
         //if (hourly_weather)
         // stress_adj_hourly_temperature
         {
            report_degree_hours_above_30_today = 0;
            report_degree_hours_above_34_today = 0;
            for (CORN::Hour hour = 0; hour < 24; hour++)
            {
               float64 temperature_hour = 0;
               // temperature_hour = hourly_weather->get_air_temperature(hour);
               temperature_hour = stress_adj_hourly_temperature.get(hour);

//if (temperature_hour > 30.0)
//std::clog << "reached" << std::endl;

               float64 degree_hours_30 = (temperature_hour < 30.0) ? 0.0 : temperature_hour - 30.0;
               report_degree_hours_above_30_today += degree_hours_30;
               float64 degree_hours_34 = (temperature_hour < 34.0) ? 0.0 : temperature_hour - 34.0;
               report_degree_hours_above_34_today += degree_hours_34;
            }

//std::cout << "dh30:" << report_degree_hours_above_30_today << " dh34:" << report_degree_hours_above_34_today << std::endl;

            output_season_tally.degree_hours_above_30_flowering.append(report_degree_hours_above_30_today);
            output_season_tally.degree_hours_above_34_flowering.append(report_degree_hours_above_34_today);

            if (output_season_tally.degree_hours_above_30_flowering.get_count() >= 14)
            {
               output_season_tally.window_14_days_achieved = true;
//std::cout << std::endl <<"dh30:" << output_season_tally.degree_hours_above_30_flowering.sum_last(14) << std::endl;
//std::cout <<             "dh34:" << output_season_tally.degree_hours_above_34_flowering.sum_last(14) << std::endl;
            }
         }
      } // else window fullfilled
   }
  #endif
   float64 soil_evap_act = soil->ref_evaporator()
      ? soil->ref_evaporator()->get_evaporation_actual_day() : 0;
   reported.daily.ET_act_m                                                       //190705_171031
   = soil_evap_act                                                               //171031
     + (ref_residues                                                             //171031
       ?ref_residues->get_act_evaporation() : 0.0)                               //171031
     + reported.daily.transpiration_act_m;                                       //190705_171031

   // Update roots was previously specifically called by land_unit_sim           //180530
   #ifdef OLD_ROOTS
         CROP_ROOT update_root_depth(soil_rooting_depth,total_soil_depth );      //180530
   #else
   #ifndef CROP_CALIBRATION
         roots_current ? roots_current->update_root_depth(terminate_pending) : 0; //190102_180530
   #else
         0.0;
   #endif                                                                        //180530
   #endif
   ended &= (yield && yield->end_day()) || !yield;                               //200424
   return ended && Crop_common::end_day();                                       //200424_151016
}
//_end_day__________________________________________________________2007-02-27_/
bool Crop_complete::consider_inactive_period()
//200402(float64 avg_temperature)
// daily temp with est. night time temps.
{  bool considered = true;                                                       //190709
   if (quiescence_end_requirement)                                               //021202
   {  // Note that normally fruit crops use chill and bud break requirements,
      // and quiescence_end_requirement would be 0,
      // but we do provide a quiescence_end_requirement catch-all for cases
      // where simulation starts mid-late winter, preventing satisfaction.
      // quiescence_end_requirement will be 0 for all subsequent years.
      if (quiescence_end_requirement->is_satisfied())                            //021202
      {                                                                          //030512
         delete quiescence_end_requirement;                                      //030512
         quiescence_end_requirement = 0;
         inactive_end_date.set(simdate);                                         //081013
         {
            considered &= phenology.activate_accrescence();                      //181108
            /* I am not sure if root elongation actually continues
            considered &= phenology.activate_root_elongation();                  //191121
            */
            considered &= restart_after_inactive_period();                       //021202
         }
      } else quiescence_end_requirement->add_daily_temperature(air_temperature_avg); //200402
         //200420 (avg_temperature); //021202
      }
   if (   quiescence_start_requirement
       && canopy_leaf_growth                                                     //190707
      // canopy_leaf_growth check is needed, otherwise we may start looking
      // for end season before we are even growing.                              //080401
       && !phenology.is_in_accrescence())                                        //200420
   {  // We are at start of dormancy
      if (quiescence_start_requirement->is_satisfied())                          //021202
      {
         log_event("Dormancy or inactive period begins" );
         delete quiescence_start_requirement; quiescence_start_requirement = 0;  //030514
         bool dormancy_condition
            = (parameters->continuation_mode_clad.get()==dormancy_season);       //200212_080401
         if (dormancy_condition)                                                 //180701_080331
         {
               considered &= senesce_at_dormancy();                              //071120
            // we now no longer have specified dormancy inactive period
            // end of dormancy (budbreak) is the end of the inactive period.     //190424
         }
         // always setup quie200304 else
         if (parameters->continuation_mode_clad.get() >= multiple_season)        //200402
            considered &= setup_quiescence_end_requirement();                    //200402_071120
               //200402 (simdate.get_DOY());
         considered &= start_inactive_period(dormancy_condition);                //200402_080401
      } else                                                                     //021202
         quiescence_start_requirement->add_daily_temperature                     //160326_021202
            (simdate.get_DOY(),temperature_with_est_night.Celcius());            //150122
   }                                                                             //021202
   return considered;                                                            //190709
}
//_consider_inactive_period_________________________________________2002-06-26_/
bool Crop_complete::restart_after_inactive_period()
{  // Set up conditions to check for start of next dormancy period.
   bool restarted = start_season();
   delete quiescence_start_requirement;                                          //160326
   quiescence_start_requirement = new Quiescence_start_requirement               //160326_021202
      ((parameters->quiescence)                                                  //160326_080331
      , parameters->is_fruit_tree()                                              //190625
      );
      // we split the perennial growing period in to growing seasons (for output only).  990413
      log_event(TL_Dormancy_ends);
#ifndef CROP_CALIBRATION
// CropCalibrate currently does not handle perennials
   if (roots_dormant)  // Note that there are only roots_dormant in the case of dormancy 080401_070131
   {  // This must be done before we call restart becase N restart uses the new vital roots
      // Send the dormant roots to biomass fate
      Crop_mass_fate *fate_now = new Crop_mass_fate
         (DORMANCY_CAUSE,0/*no canopy*/,roots_dormant->get_biomass());
      for (nat8 layer = 1; layer <= MAX_soil_layers; layer++)                    //070131
      {
         fate_now->roots_dead[layer] = roots_dormant->get_biomass_in_layer(layer);
         // all these effet roots are now considered dead, no live roots recorded in fate
      }
      Crop_biomass no_canopy_biomass_removed; // None in this case, just roots   //070412
      //dont currently do anything with the total biomass removed
      record_biomass_fate(*fate_now,0/*,false*/);
      #ifdef NITROGEN
      if (nitrogen)                                                              //180615
      {
      Crop_mass_fate *N_mass_fate_now = nitrogen->render_N_fate(DORMANCY_CAUSE,0,0); //131005
      nitrogen->apportion_fate
         (*fate_now,N_mass_fate_now,no_canopy_biomass_removed,false,include_effete);
      }
      #endif
      delete fate_now;
      if (!roots_vital)                                                          //070131
      {
         // create a new roots_vital at the current dormant root depth.
         // Note that biomass will instantly be created in this case.
         // If we at some time we decide to have a detailed external mass balance of roots,
         // this will need to be adjusted here.
         // The root mass balance is otherwise maintained for the season only.
         roots_current = roots_vital =
            dynamic_cast<CropSyst::Crop_root_vital *>(new CropSyst::Crop_root_vital_V5
               (parameters->root                                                 //130625
               ,*phenology.get_root_elongation_period()                          //181108
               ,*soil->ref_layers()
               ,roots_dormant->get_length()));
         roots_vital->update(1.0,false); // Needed to get the new biomass by layer   //070312
      }  // else we already have vital roots then just keep them.
      delete roots_dormant; roots_dormant = 0;
      phenology.activate_root_elongation();                                      //200504
   }
#endif

   /*190703 Now canopy retains (effect portions) and if everything is senesced
      canopy is reestablished
   float64 restart_GAI = (fall_sowing && emergence_count == 1) ? 0.1 :  0.25;    //070320
   restart_GAI = std::max<float64>(restart_GAI,get_GAI_vital_plus_effete());     //070320


   // we do have restrage initial cover fraction,
   // probably should calculate restart_GAI from this instead of these
   // hard coded values.
   */


   // Moved from restart  190708
   float64 restart_biomass = 0.005; //190703 there should be crop parameter?
   reported.season.biomass_production = restart_biomass;
   trigger_synchronization(NGS_RESTART);                                         //030606
   //initiate accrescence does this trigger_synchronization(NGS_ACTIVE_GROWTH);  //030606
   phenology.activate_restart();                                                 //190703
      // moved from restart_after_inactive_period
   phenology.activate_accrescence();                                             //181108
   float64 restart_GAI = get_GAI_vital_plus_effete();                            //190703
   restarted &= yield ? yield->reset() : false;                                  //170221_150925
   phenology.reset(NGS_EMERGENCE);                                               //141202
   phenology.reset(NGS_RESTART);                                                 //200404

   #ifdef NITROGEN
   if (nitrogen)
       nitrogen->restart(restart_biomass,restart_GAI,false, true);               //190708_020505
         // use_clump_factor after_dormancy
   #endif

   #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
   thermal_time->clear_accum_degree_days();                                      //180522
   #endif
   #if ((PHENOLOGY_VERSION==2018))
      // Not sure if this is still needed because we do reset and we now have periods
      // which are instanciated as needed
   #endif


   //190708 restart stuff moved here restartX(true);                             //190703
      //190703 (0.005,restart_GAI,false,true);                                   //040914
      // Thermal time is now cleared at start of dormancy,
      // it is accumulated during dormancy inorder to get bud break.
      // about 20 days of consecutive 0 thermal time will reset the thermal time.//030517
      // Claudio does want thermal time reset at end of dormancy.                //030521

   restarted &= initiate_accrescence();                                          //190708_190216
   // PHENOLOGY_2018 each intanciated growth stage has its own accumulation.
   //May need to trigger also active growth                                      //040512
   return restarted;
}
//_restart_after_inactive_period____________________________________1997-02-19_/
bool Crop_complete::initiate_accrescence()
{
   emergence_count++; // for multiple seasons                                    //190411

//std::clog << "debugging pot_transpiration_m_CO2_adj" << std::endl;
//pot_transpiration_m_CO2_adj = 99999.9;

   // delete transpiration;    should be deleted (from previou season) but havent check if this will crash  200701
   transpiration = new Crop_transpiration_2                                      //191011 moved from Crop_common:constructor
      (parameters->transpiration
      ,soil                                                                      //160415
      ,parameters->salinity.osmotic_pot_50                                       //160415
      ,parameters->salinity.salt_tolerance_P                                     //160415
      ,param_dry_soil_root_activity_coef                                         //170518
      ,param_saturated_soil_root_activity_coef);                                 //170518
   provide_biomass_growth().start();                                             //180612
   provide_canopy().start();                                                     //190702

   if (!roots_current) create_roots();
   {
      log_event(TL_Active_Growth);                                               //000719
      float64 initial_biomass = parameters->canopy_growth_LAI_based.initial_GAI
         / parameters->canopy_growth_LAI_based.specific_leaf_area;               //000207
      canopy_leaf_growth->restart_with                                           //190703
         (initial_biomass,parameters->canopy_growth_LAI_based.initial_GAI,true); //190703

      roots_vital->know_above_ground_biomass(initial_biomass);                   //130625
      #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
      phenology.activate_emergence();                                            //181108
      phenology.activate_accrescence();                                          //190701
      #endif

      canopy_leaf_growth->know_accrescence(phenology.get_accrescence_period());  //181230
      trigger_synchronization(NGS_EMERGENCE);
      trigger_synchronization(NGS_ACTIVE_GROWTH);
   }
   days_since_last_clipping = 0;                                                 //190621_970305
   return true;                                                                  //190707
}
//_initiate_emergence_____________________________________________________2000_/
#if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
bool Crop_complete::initiate_tuber()
{
   if (parameters->harvested_part_clad.get()  == tuber_part)
   {  log_event(TL_Tuber_initiation);
      phenology.activate_tuberization();                                         //181108
   }
   return true;
}
//_initiate_tuber______________________________________________________________/
#endif
// PHENOLOGY_2018 Phenology determines tuberization

bool Crop_complete::initiate_flowering()
{
   bool initiated = true;
   trigger_synchronization(NGS_ANTHESIS);                                        //151020
   #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
   if (!phenology.is_in_yield_formation())                                       //181108

   {
      //200409 moved to phenology phenology.growth_stage = NGS_ANTHESIS;         //081110
      trigger_synchronization(NGS_ANTHESIS);
      phenology.activate_anthesis();                                             //190520
      log_event(TL_Begin_flowering);
   }
   //200420 linger_label.anthesis = 5;                                           //191022
   #endif
   if (yield) initiated &= yield->know_flowering();                              //200423
   return initiated;
}
//_initiate_flowering__________________________________________________________/
bool Crop_complete::initiate_senescence()
{
   #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
   phenology.activate_senescence();                                              //190701
   #endif
   bool inited = canopy_leaf_growth->know_senescence(phenology.get_senescence_period());//181111
   //NYN senescence_date.set(today);                                             //001115 ?* NYI */
   return inited;
}
//_initiate_senescence______________________________________________2012-12-21_/
bool Crop_complete::culminate_senescence()
{  return canopy_leaf_growth->know_senescence(0)
      &&  phenology.culminate_senescence();
}
//_culminate_scenescence____________________________________________2019-10-22_/
bool Crop_complete::initiate_culminescence()
{
   #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
   phenology.activate_culminescence();
   #endif
   bool inited = canopy_leaf_growth->know_culminescence(phenology.get_culminescence_period()); //181111
   trigger_synchronization(NGS_END_CANOPY_GROWTH);
   log_event("Maximum canopy");
   return inited;
}
//_initiate_max_canopy______________________________________________2013-04-49_/
bool Crop_complete::initiate_filling()
{  bool initiated = true;
   biomass_at_filling = get_canopy_biomass_kg_m2_including(include_vital) ;      //131022
   yield->know_biomass_at_filling(biomass_at_filling);                           //150826
   trigger_synchronization(NGS_FILLING);                                         //130902
   #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
   //200409 moved to active_yield_formation phenology.growth_stage = NGS_FILLING;                                         //191023
   log_event(TL_Grain_filling);
   phenology.activate_yield_formation();                                         //181113_181108
   #endif
   // PHENOLOGY_2018  Phenology identifies yield_formation (filling)
   if (yield) initiated &= yield->know_filling();                              //200423
   return initiated;
}
//_initiate_filling_________________________________________________2002-08-09_/
bool Crop_complete::initiate_maturity()
{
/*disabled need to check why crashing 191030
   delete transpiration;                                //191011                                                       //191011
*/
   /* 200701  Claudio decided that transpiration should not stop at maturity
    (this was in regards to fruit trees by he says it is generally applicable)
   transpiration = 0;
   */
   biomass_at_maturity  =  get_canopy_biomass_kg_m2_including(include_vital) ;
   yield->know_biomass_at_maturity(biomass_at_maturity);                         //150826
   trigger_synchronization(NGS_MATURITY);                                        //130902
   if (!parameters->is_fruit_tree())                                             //190625
   {  delete biomass_growth;                                                     //180111
      biomass_growth = 0;                                                        //180111
   }                                                                             //180111
   #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
   //movedphenology.growth_stage = NGS_MATURITY; //191023
   log_event(TL_Maturity);
   phenology.activate_maturity();                                                    //181108
   #endif

   // canopy growth needs to know the occurance of maturity
   if (canopy_leaf_growth)                                                       //200507
   canopy_leaf_growth->know_maturity_initiation();                               //200409

   // PHENOLOGY_2018 phenology identify maturity
   return true;
}
//_initiate_maturity___________________________________________________________/
Crop_mass_fate_biomass_and_nitrogen *
Crop_complete::harvest
(
 float64 remove_amount_kg_m2                                                     //050822
,const CropSyst::Harvest_or_clipping_operation_struct                            //140811
   *_harvest_or_clipping_params
,bool terminate                                                                  //140811
)
{  Crop_mass_fate_biomass_and_nitrogen *result_fate = 0;                         //070625
   CropSyst::Harvest_or_clipping_operation_struct                                //140811
      *harvest_or_clipping_params                                                //120419_060612
   =  _harvest_or_clipping_params
   ?  (CropSyst::Harvest_or_clipping_operation_struct *)                         //140811
      _harvest_or_clipping_params                                                //120419
   : new
      CropSyst::Harvest_or_clipping_operation_struct;                            //140811
   if (!_harvest_or_clipping_params)                                             //060612
   {  //  harvest_or_clipping_params were not specified, so use defaults
      harvest_or_clipping_params->biomass_fate_percents.remove_for_use                = 0;
      harvest_or_clipping_params->biomass_fate_percents.remove_for_grazing            = 0;
      harvest_or_clipping_params->biomass_fate_percents.remain_as_residue             = 0;
      harvest_or_clipping_params->biomass_fate_percents.remove_for_disposal           = 0;
      harvest_or_clipping_params->biomass_fate_percents.remain_as_dead                = 100.0;
      harvest_or_clipping_params->biomass_fate_percents.remain_as_live                = 0;
      harvest_or_clipping_params->biomass_fate_percents.remove_roots_to_dead_residue  = 100;
      harvest_or_clipping_params->biomass_fate_percents.remain_roots_live             = 0;
      harvest_or_clipping_params->biomass_fate_percents.remove_roots_as_loss          = 0;
      harvest_or_clipping_params->biomass_fate_percents.return_as_manure              = 0;
   }
   //200603 the method is reimplemented by Crop_orchard_fruit
   //200603 if (!fruit_harvested)                                                         //070711
   {
   Biomass_fated_cause biomass_fate_cause = (Biomass_fated_cause)HARVEST_CLIPPING;
   Harvest_amount_mode harvest_amount_mode
      = harvest_or_clipping_params->harvest_amount_mode;                         //051105
   float64 management_retain_GAI =
      (harvest_amount_mode== HARVEST_REMOVE_FIXED_AMOUNT_BIOMASS)                //150223
         ? harvest_or_clipping_params->min_retain_GAI                            //051105
         : 0.0;                                                                  //150223
      if (terminate )                                                            //040914
          biomass_fate_cause = (Biomass_fated_cause)HARVEST_LAST_CLIPPING;
      if (harvest_or_clipping_params->biomass_fate_percents.remove_for_grazing >  0)
          biomass_fate_cause = (Biomass_fated_cause)HARVEST_GRAZE;
   result_fate =                                                                 //070625
    ((parameters->applies_harvest_index()
     && (biomass_fate_cause != (Biomass_fated_cause)HARVEST_GRAZE))
         ||  (harvest_amount_mode==HARVEST_CROP_PARAMETER_BASED) )               //090922_050822_ I had this as 'or'
      ?  harvest_crop(harvest_amount_mode,remove_amount_kg_m2,management_retain_GAI //980924
         ,harvest_or_clipping_params->biomass_fate_percents                      //051105
         ,terminate)
      :  process_biomass_fate
         (biomass_fate_cause
         ,harvest_amount_mode,remove_amount_kg_m2,management_retain_GAI          //050823
         ,per_ha_to_per_m2(harvest_or_clipping_params->reserve_biomass_kg_ha)    //110603
         ,harvest_or_clipping_params->biomass_fate_percents                      //051105
         ,terminate,0.0); // no harvest index
   if (terminate)                                                                //030516
   {  // just trigger termination                                                //040506
      end_season();                                                              //160618
      trigger_synchronization(NGS_TERMINATED);                                   //040506
      terminate_pending=true;                                                    //040514
      reported.daily.LAI_for_self_shading_response = 0.0;                        //190705
      // we dont need to respond  to clipping, because we are going to terminate.
   } else                                                                        //040506
      if (!parameters->is_fruit_tree())                                          //190625
         //  Fruit crops dont loose leaves
         respond_to_clipping();                                                  //051103
   if (!_harvest_or_clipping_params)
   {
      delete harvest_or_clipping_params;                                         //060612
      harvest_or_clipping_params = 0;                                            //140430
   }
      // Delete these if we create a default set of parameters
   }
   harvestable=false;                                                            //190710
   return result_fate;                                                           //020311
}
//_harvest__________________________________________________________2007-06-25_/
bool Crop_complete::respond_to_clipping()                          modification_
{  harvests_this_season += 1;                                                    //040624
   latest_harvest_date.set(simdate);                                             //110824_001115
   trigger_synchronization(NGS_HARVEST);                                         //130902
// Reseting to emergence deg day is not going to work.
// because if we clip every day, leaves will not accumulate thermal time
//                   float64 reset_thermal_time = parameters->thermal_time_parameters.emergence_deg_day;
      //trying to reset some time between flowering and emergence
       //(parameters->thermal_time_parameters.emergence_deg_day + parameters->thermal_time_parameters.begin_flowering_deg_day)/2.0;

   if (!phenology.is_matured())                                                  //181108
   { // We dont activate  after maturity or dormancy //041221
      #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
      if (parameters->phenology.clipping_resets)                                 //041221
      {  float64 reset_thermal_time =parameters->phenology.initiation.emergence; //040830
         Canopy_growth_cover_based *canopy_growth_cover_based
            = dynamic_cast<Canopy_growth_cover_based *>(canopy_leaf_growth);     //130512
         if (canopy_growth_cover_based)                                          //130512
         {
            /*190405 cannot go back to start season (zeros and sets to season start value)
            Instead, yesterdays values should be set to values after clipping
            canopy_growth->start();                                              //180718
            */
            reset_thermal_time=canopy_growth_cover_based->get_thermal_time_for_reset();//130512
         }


         // 190405 Warning, not sure why not similarly
         // reset thermal using get_thermal_time_for_reset



         thermal_time->reset_to(reset_thermal_time);                             //151110_040830
         trigger_synchronization(NGS_ACTIVE_GROWTH);                             //130902
         phenology.growth_stage = NGS_ACTIVE_GROWTH;                             //130902_040719

         /*190405 abandoned because it had appeared that clipping wasnt
resetting things correctly but I think the correction I made
will correct the problem Fabio was seeing

         // make vital effete
         // This was added for Fabio's sugar cane where there is no dormancy.
         // We need to move vital to effete otherwise the recovery dynamics
         // show that there is vital GAI/biomass so recovery dynamics is low.

         canopy_growth->vital_becomes_effete(false);    //180718
         */
      } else
      {  // if this is an annual (or biannual)
         #ifdef THERMAL_TIME_OBSOLETE_2020
         thermal_time->respond_to_clipping                                       //151110
            (parameters->phenology.initiation.emergence);                        //080722
         #endif
         //  and during flowering, then
         if ((phenology.growth_stage == NGS_ANTHESIS)
          || (phenology.growth_stage == NGS_FILLING))                            //130902
         {  // clear deg day for flowering and deg day for filling
            // the flowering part 040826
            parameters->phenology.initiation.filling = 0;                        //040826
            parameters->phenology.initiation.flowering = 0;                      //040826
            trigger_synchronization(NGS_ACTIVE_GROWTH);                          //130902
            phenology.growth_stage = NGS_ACTIVE_GROWTH;                          //130902_040719
         } // else perennials are allowed to remain in their current growth stage, but the stage may be extended. 040826
      }
      #endif

      if (phenology.respond_to_clipping())                                       //181108
      {  // float64 reset_thermal_time =parameters->phenology.initiation.emergence; //040830

         canopy_leaf_growth->know_accrescence(phenology.get_accrescence_period());  //200210

      #if ((PHENOLOGY_VERSION==2018))
         Canopy_growth_cover_based *canopy_growth_cover_based
            = dynamic_cast<Canopy_growth_cover_based *>(canopy_leaf_growth);     //130512
         trigger_synchronization(NGS_ACTIVE_GROWTH);                             //130902
         phenology.activate_accrescence();                                       //181108
      #endif
      }
   }
   return (canopy_leaf_growth == NULL) ||                                        //190701
      canopy_leaf_growth->respond_to_clipping();                                 //110824_080515
}
//_respond_to_clipping______________________________________________2004-07-19_/
Biomass_growth &Crop_complete::provide_biomass_growth()               provision_
{
   if (!biomass_growth)
      biomass_growth = new Biomass_growth_RUE_TUE                                //180111
      (
       parameters->is_fruit_tree()                                               //191025
      ,parameters
      #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
      ,phenology.growth_stage
      #endif
      ,emergence_count
      ,pot_transpiration_m_CO2_adj                                               //191025
      #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
      ,thermal_time
      #endif
      #if ((PHENOLOGY_VERSION==2018))
      ,phenology.ref_accum_deg_days_adjusted_for_clipping()                      //181118
      #endif
      ,CO2_response
      ,canopy_leaf_growth
      ,air_temperature_avg
      ,solar_radiation
      ,intercepted_radiation
      #ifdef YAML_PARAM_VERSION
      // Using adjustable parameter idiom
      ,transpiration_dependent_growth
      #else
      ,transpiration_dependent_growth
      #endif
      ,report_temperature_stress_stats
      //200608 ,is_at_max_fruit_load()
      );
   return *biomass_growth;
}
//_provide_biomass_growth___________________________________________2018-06-12_/

void Crop_complete::set_automatic_nitrogen_mode(bool enable)
{
#ifdef NITROGEN
   if (nitrogen) nitrogen->set_automatic_mode(enable);
#endif
}
//_set_automatic_nitrogen_mode______________________________________2004-05-25_/
bool Crop_complete::update_temperatures()                         //rectification_
{
   temperature_with_est_night.set_Celcius
      (air_temperature_avg.Celcius(),air_temperature_avg.get_quality_code());    //140625_140507
   float64 senescence_reduction_factor
      = canopy_leaf_growth                                                       //190423
      ? canopy_leaf_growth->get_senescence_reduction_factor()                    //080807
      : 1.0;                                                                     //190423
      stress_adjusted_temperature.set_Celcius                                    //140624
         (calc_stress_adjusted_temperature                                       //030610
          (parameters->phenology.phenologic_sensitivity_water_stress
          ,air_temperature_max
          ,senescence_reduction_factor
          ,water_stress_stats.index
          ,vapor_pressure_deficit_max.get_kPa()                                  //140813
          ,!phenology.is_matured()                                               //181108
          ,parameters->has_leaf_senescence())
         ,CORN::calculated_quality);


   Date_clad_32 tomorrow(simdate_raw); tomorrow.inc_day();                       //170525_030713
   float64 min_temp_tomorrow = weather.get_air_temperature_min_tomorrow();       //151201
      // RLN note, this should now be done in the wthrbase//021124
   stress_adj_hourly_temperature_estimator.set_max_today_min_tomorrow(air_temperature_max, min_temp_tomorrow); //030610
   stress_adj_hourly_temperature_estimator.set_stress_adjusted_temperature( stress_adjusted_temperature);      //030610
   stress_adj_hourly_temperature_estimator.get_temperature_by_interval(stress_adj_hourly_temperature);         //030610
///* Idealy this would go here but biomass? is inconsistent with old version
   #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
//200404   if (!phenology.is_in_quiescence())                                            //181108
      thermal_time->accumulate_degree_days
               #ifdef THERMAL_TIME_OBSOLETE_2020
               (phenology.growth_stage);                                         //020629
               #else
               ();                                                               //200211
               #endif
   #endif
//*/

      #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
      #ifdef SEED_EMERGENCE_MODEL
      //190725 float64 accum_degree_days=THERMAL_TIME_SOURCE get_accum_degree_days(true); //151030
      //Update emergence last because we dont want to grow
      // what emerged today.
      if (emergence_hydrothermal_time)                                           //040112
      {  if (check_for_emergence())                                              //051205
         {  // adjust the phenology parameters if the hydrothermal time delayed longer than the target emergence thermal time //051205
            float64 accum_degree_days
               = THERMAL_TIME_SOURCE get_accum_degree_days(/*true*/);                //051205
                 // RLN not sure if should be adjusted for clipping need to check with Claudio
            int16 emergence_deg_day_param =
               parameters->phenology.initiation.emergence;                       //051205
            if (accum_degree_days > emergence_deg_day_param)                     //051205
            {  float64 deg_days_5_days_from_now = est_degree_days_X_days_from_now(simdate, 5);
               float64 adjustment = accum_degree_days
                  - emergence_deg_day_param + deg_days_5_days_from_now;
               parameters->phenology.offset_by(adjustment);
      }  }  }
      #endif
      #endif

   return true;
}
//_update_temperatures______________________________________________2018-12-28_/
bool Crop_complete::process_day()
{  bool processed = true;
   processed &= consider_inactive_period(); //200402 (air_temperature_avg);                   //190708_020626
   #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
   if (phenology.growth_stage == FGS_BUD_BREAK) phenology.growth_stage = FGS_ACTIVE_GROWTH;          //130902_030516
   if (phenology.growth_stage == FGS_VERAISON)                                   //130902_030521
        phenology.growth_stage = FGS_RAPID_FRUIT_DEVELOPMENT;
   // For fruits, we only allow one day of harvest indicator then we resume maturity //130902_030615
   if (phenology.growth_stage==NGS_HARVEST)                                      //130902_030615
       phenology.growth_stage = growth_stage_at_harvest;                         //040514

   #endif
   #if ((PHENOLOGY_VERSION==2018))
   // phenology maintains its own growth stage/period.
   processed &= phenology.process_day();                                         //190821

   #endif
   if (roots_vital)                                                              //130902_061212
   {  if (is_fruit())                                                            //120808
         roots_vital->root_length       = parameters->root.max_depth;            //120808
      else                                                                       //120808
      {
         // Warning water stress hasn't been calculated yet
         // so using yesterday's                                                 //160803
         float64 canopy_biomass_vital = get_canopy_biomass_kg_m2_including(include_vital);
         roots_vital->know_above_ground_biomass(canopy_biomass_vital);
         // May beable to reference the canopy biomass
         // it is not too important to roots(output only?)
         // so this would have lower overhead
         roots_vital->update(get_water_stress(),terminate_pending);              //191021_061208
      }
   }
   if (canopy_leaf_growth)                                                       //190423
   {
      // warning not sure if process_transpiration first
      processed &= canopy_leaf_growth->process_day();                            //190821
         plant_height =                                                          //990201
            canopy_leaf_growth->calc_plant_development(
                        #if (CROPSYST_VERSION == 4)
                        parameters->param_max_canopy_height()                    //990201
                        #else
                        parameters->morphology.max_canopy_height
                        #endif
                       ,plant_height ,overall_growth_stress);                    //080805
/*
std::cout << simdate_raw << "\t"
<< canopy_leaf_growth->get)_fract_canopy_cover()
<< "\t" << canopy_leaf_growth->get_interception_global_green()
<< std::endl;
*/
         #ifdef CO2_CHANGE
         if (CO2_response)
         {
            CO2_TUE_adjustment
            = (canopy_leaf_growth->get_interception_global_green()>0.0000001)    //151030
               ? CO2_response->get_TUE_adjustment()                              //151030
               : 1.0;
            float64 CO2_transpiration_adjustment_factor
            = CO2_response->get_transpiration_adjustment_factor();               //110913
            pot_transpiration_m_CO2_adj *= CO2_transpiration_adjustment_factor;  //190826_151031
         } // else no adjustments applied
         #endif

         // currently CO2_TUE_adjustment is needed for xxx
         // Move intercepted_radiation to start day
         float64 canopy_cover_green_fract                                        //080428
            = canopy_leaf_growth->get_interception_global_green();               //200228_190628
         float64 global_radiation = (solar_radiation);  // MJ/m2 ?               //080428
         intercepted_radiation = global_radiation * canopy_cover_green_fract;    //080428
         intercepted_radiation_accum += intercepted_radiation;
         intercepted_radiation_season_accum += intercepted_radiation;
         if (biomass_growth)
         {
            biomass_growth->process_day();                                       //180111
            attainable_top_growth=biomass_growth->calc_attainable_top_growth();  //180111
            vapor_pressure_deficit_stats.append(daytime_vapor_pressure_deficit.get_kPa());//180111
         }
         if (transpiration)                                                      //191011
         {
            #ifdef NITROGEN
            if (nitrogen)
            {
               Crop_nitrogen_V5 *N_V5 = dynamic_cast<Crop_nitrogen_V5 *>(nitrogen); //151030
               float64 N_limited_growth =
                  // Now always N_V5
                  N_V5->update_limited_growth                                    //151030
                  (attainable_top_growth
                  ,canopy_leaf_growth->get_influence_on_root_activity_factor
                     (!phenology.is_in_accrescence()));                          //181108

               if (parameters->biomass_production.efficiency_model==TUE_efficiency) //191030
                  limited_pot_transpiration_m                                    //160321
                  = nitrogen->update_limited_pot_transpiration();                //160321
               else                                                              //191030
               {  float64 N_pot_transp_adj =                                     //191030
                     //200318 (attainable_top_growth > -0.0000001) // usually not 0       //191030
                     (attainable_top_growth > 0.0000001) // usually not 0       //191030
                     ? N_limited_growth / attainable_top_growth                  //191030
                     : 1.0 ;                                                     //191030
                  limited_pot_transpiration_m =                                  //191030
                     pot_transpiration_m_CO2_adj * N_pot_transp_adj;             //191030
               }
            } else limited_pot_transpiration_m                                   //160321
                  = no_nitrogen_calculate_limited_pot_transpiration();           //160321
            #endif
            process_transpiration_m(limited_pot_transpiration_m);                //031201
            processed &= actual_growth();                                        //200402
         } // else probably maturity where transpiration is disabled
      //200402 requires dranspiration processed &= actual_growth();

      reported.daily.canopy_biomass                                              //190705
         = canopy_leaf_growth->get_biomass_current
         (include_vital|include_effete|include_canopy_dead); // for output only  //050823


//debugdump_lAI << today32 << '\t' << canopy->get_GAI(include_vital|include_effete) << '\t' << canopy->get_GAI(include_vital) << '\t'  << canopy->get_GAI(include_effete) << '\t' << canopy->get_LAI() << endl;
#ifdef DUMP_BIOMASS
dump_biomass << today32 << '\t'
         << canopy->get_live_and_or_dead_biomass(true ,true ,false) << '\t'   //vital_or_effete
         << canopy->get_live_and_or_dead_biomass(true ,false,false) << '\t'   //vital
         << canopy->get_live_and_or_dead_biomass(false,true ,false) << '\t'   //effete
         << canopy->get_live_and_or_dead_biomass(true ,true ,true)  << endl;   //vital_or_effete_or_dead
#endif
   }
   //now used processed &= (yield && yield->process_day()) || !yield;            //200423
   return processed;                                                             //111121
}
//_process_day_________________________________________________________________/
float64 Crop_complete::no_nitrogen_calculate_limited_pot_transpiration() modification_
{  float64 transpiration_use_eff                                                 //160521
      = transpiration_dependent_growth.ref_transpiration_use_efficiency_CO2_adjusted();
   #if (CROPSYST_VERSION==4)
   // special output
   report_transpiration_use_eff = (float32)transpiration_use_eff;                //150621
   #endif
   float64 pot_transpiration_mm
      =  attainable_top_growth
      / transpiration_use_eff;                                                   //160521
   float64 limited_pot_transpiration_m = mm_to_m(pot_transpiration_mm); // CO2 adjusted
   return limited_pot_transpiration_m;
}
//_no_nitrogen_calculate_limited_pot_transpiration__________________2016-03-21_/
bool Crop_complete::is_established()                                affirmation_
{  return !parameters->continuation_mode_clad.get() == single_season;            //200213
}
//_is_established___________________________________________________2019-07-08_/
float64  Crop_complete::calc_act_biomass_growth
(float64 planting_depth                                                          //010202
,float64 &GAI_related_biomass_growth)
// This method may be specific to the canopy growth model.                       //080805
{
   float64 act_biomass_growth = attainable_top_growth;                           //151030_011022
   float64 N_leaf_stress_factor = 1.0;                                           //200528
   if (nitrogen)                                                                 //200528
   {  N_leaf_stress_factor = nitrogen->calc_leaf_stress_factor();                   //200528
      //200529         (parameters->nitrogen.nitrogen_leaf_stress_mode);                 //200528

      //200529 std::clog << std::endl << "NSF:" << N_leaf_stress_factor << std::endl;

      canopy_leaf_growth->know_N_leaf_stress_factor(N_leaf_stress_factor);       //200528
   }
   if (limited_pot_transpiration_m > 0.0)                                        //160321
   {
      #if (defined(NITROGEN))
      float64 act_to_pot_transpiration_ratio  // name to water_stress_factor
         =  reported.daily.transpiration_act_m                                   //190705
            //190705 shouldnt be using the reported value
           / limited_pot_transpiration_m;
      float64 transpiration_limited_growth
         = attainable_top_growth                                                 //011117
         * act_to_pot_transpiration_ratio;                                       //011117
      act_biomass_growth                                                         //011022
         = std::min<float64>(transpiration_limited_growth,attainable_top_growth);//011117
      if (parameters->biomass_production.efficiency_model == TUE_efficiency)     //191030
      {
      #if (CROPSYST_VERSION==4)
      deprecated::Crop_nitrogen_V4 *N_V4 = dynamic_cast<deprecated::Crop_nitrogen_V4 *>(nitrogen);
      if (N_V4)
      {
         float64 accum_degree_days
            = THERMAL_TIME_SOURCE get_accum_degree_days();                       //080728_130822 confirmed adjusted
         float64 root_activity_factor = canopy_leaf_growth->
            get_influence_on_root_activity_factor                                //080805
               (phenology.has_expired_accrescence());                            //190628
         float64 N_limited_growth = // could rename this to the final_limited_growth //011214
         N_V4->calc_limited_growth
            (transpiration_limited_growth
            ,act_to_pot_transpiration_ratio
            ,root_activity_factor);                                              //020505
         act_biomass_growth                                                      //011022
            = std::min<float64>( transpiration_limited_growth,N_limited_growth); //011117
      }
      if (!N_V4)
      #endif
      {
         if (nitrogen)                                                           //191030
         {
            float64 N_limited_pot_transp=nitrogen->get_limited_pot_transpiration();
            float64 water_stress_factor
               = N_limited_pot_transp > 0.0
               ? reported.daily.transpiration_act_m
                  // probably shouldnt be using reported value
                  / N_limited_pot_transp
               : 1.0;
            // For N_V5 we use the N attainable growth previously calculated in processes
            float64 N_limited_growth = nitrogen->get_limited_growth();           //151105
            act_biomass_growth = water_stress_factor * N_limited_growth;         //151105
         }
      }
      #endif
      } // else in RUE model we have already adjust (potential) transpiration.

      // This canopy expansion factor is needed only
      // for LAI canopy it reduces canopy expansion .                            //110901
      float64 leaf_water_pot = transpiration->get_leaf_water_pot();              //080506
      water_canopy_expansion_factor =                                            //080506
         (leaf_water_pot - parameters->morphology.stop_canopy_expansion_LWP)     //080506_110218
        /(parameters->morphology.reduce_canopy_expansion_LWP
            // Once we are more negative than this value expansion is reduced    //110218
          - parameters->morphology.stop_canopy_expansion_LWP);                   //110218_080506
      water_canopy_expansion_factor = CORN::must_be_between<float64>(water_canopy_expansion_factor,0.0,1.0);   //080506
      GAI_related_biomass_growth =  act_biomass_growth * water_canopy_expansion_factor; //080506
      #ifdef NITROGEN

      // Check with Claudio if this is still needed/valid  151030

      /*std::clog << "act_biomass_growth:" << act_biomass_growth
                << "\tleaf_water_pot:" << leaf_water_pot
                << "\twater_canopy_expansion_factor:" << water_canopy_expansion_factor
                << "\ttranspiration_limited_growth:" << transpiration_limited_growth
                << "\trecorded_act_transpiration_m:" << recorded_act_transpiration_m
                << "\tact_to_pot_transpiration_ratio:" << act_to_pot_transpiration_ratio
                << "\tvital_root? " << (roots_vital ? "true" : "false")
                << std::endl;*/

      if (nitrogen)
      {
         GAI_related_biomass_growth *= N_leaf_stress_factor;
      }
      #endif
   } else act_biomass_growth = 0.0;                                              //200528
   overall_growth_stress = (attainable_top_growth > 0.000001)
      ? act_biomass_growth / attainable_top_growth : 1.0;
   return act_biomass_growth;
}
//_calc_act_biomass_growth__________________________________________2001-10-23_/
#if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
#ifdef THERMAL_TIME_OBSOLETE_2020
bool Crop_complete::thermal_time_matchs(float64 deg_day_event,bool clipping_adjusted) const
{
   float64 GDDs_yesterday = thermal_time->get_accum_degree_days(clipping_adjusted,TT_YESTERDAY);
   float64 GDDs_today     = thermal_time->get_accum_degree_days(clipping_adjusted,TT_TODAY);
   return (GDDs_yesterday <= deg_day_event)
       && (GDDs_today    >= deg_day_event);
}
#else
bool Crop_complete::thermal_time_matchs(float64 deg_day_event)             const
{
   float64 GDDs_yesterday = thermal_time->get_accum_degree_days_yesterday();
   float64 GDDs_today     = thermal_time->get_accum_degree_days();
   return (GDDs_yesterday <= deg_day_event)
       && (GDDs_today    >= deg_day_event);
}
#endif
#endif
//______________________________________________________________________________
#if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
bool Crop_complete::thermal_time_event()
{  bool done = true;
   if (phenology.is_in_quiescence())                                             //181108
      return done;                                                               //071113
   if (phenology.is_in_germination())                                            //181108
      if (!emergence_hydrothermal_time)                                          //041201
       if (thermal_time_matchs(parameters->phenology.initiation.emergence))      //041201
          initiate_accrescence();
    if (parameters->has_leaf_senescence())                                       //970130
         // Not sure why was looking an senescence for tuberization
      if (thermal_time_matchs(parameters->phenology.initiation.tuberization))    //970130
         initiate_tuber();                                                       //970130

   if (canopy_leaf_growth)                                                       //190719_121221
   {
      if ((parameters->phenology.initiation.flowering) // Clipping event may set this to 0 indicating flowering is cancelled
        && thermal_time_matchs(parameters->phenology.initiation.flowering))      //081102
         initiate_flowering();
      {
      const Phenology_abstract::Period_thermal *culminescence                    //191022
         = phenology.get_culminescence_period();                                 //191022
      if (culminescence&& culminescence->has_expired())                          //191022
         initiate_senescence();                                                  //121221
      }
      {  const Phenology_abstract::Period_thermal *accresence                    //191022
            = phenology.get_accrescence_period();                                //191022
         if (accresence && accresence->has_expired())                            //191022
            initiate_culminescence();                                            //130429
      }
      {  const Phenology_abstract::Period_thermal *senescence                    //191022
            = phenology.get_senescence_period();                                 //191022
         if (senescence && senescence->has_expired())                            //191022
            culminate_senescence();                                              //191022
      }
   } // if canopy_leaf_growth
   if (parameters->is_fruit_tree()                                               //190625
       && (parameters->harvested_part_clad.get() == fruit_part))                 //030521
   {
   } else // Now all non fruit crops, not just those harvest for seed can have grain (seed) filling
       if ((parameters->phenology.initiation.filling) // Clipping event may set this to 0 indicating filling is cancelled  //030521
             && thermal_time_matchs(parameters->phenology.initiation.filling))
          initiate_filling();
   bool maturity_significant = parameters->phenology.maturity_significant;       //200409_080319
   if (maturity_significant                                                      //080319
       && thermal_time_matchs(parameters->phenology.initiation.maturity))        //970130
      done &= initiate_maturity();
   return done;
}
//_thermal_time_event_______________________________________________1997-05-14_/
#endif

#if ((PHENOLOGY_VERSION==2018))

bool Crop_complete::thermal_time_event()
{
   if (phenology.is_in_quiescence()) return true;                                //181108
   if (!emergence_hydrothermal_time)                                             //041201
   {  if (phenology.was_realized("phenology/accrescence"))                       //181118
         initiate_accrescence();                                                 //130429
   }
   {  // these can now be initiated at any time                                  //181230
      if (phenology.was_realized("phenology/anthesis"))                          //181118
         initiate_flowering();
      if (phenology.was_realized("phenology/culminescence"))                     //181118
         initiate_culminescence();                                               //130429
      if (phenology.was_realized("phenology/senescence"))                        //181118
         initiate_senescence();                                                  //121221
   }
   {  const Phenology::Period_thermal *senescence                                //200506_191022
         = phenology.get_senescence_period();                                    //191022
      if (senescence && senescence->has_expired())                               //191022
         culminate_senescence();                                                 //191022
   }
   if (parameters->is_fruit_tree() &&                                            //190625
       (parameters->harvested_part_clad.get() == fruit_part))                    //030521
   { } else // Now all non fruit crops, not just those harvest for seed can have grain (seed) filling
      if (phenology.was_realized("phenology/yield_formation"))
         initiate_filling();
   if (phenology.was_realized("phenology/maturity"))
      initiate_maturity();
   return true;
}
//_thermal_time_event_______________________________________________2018-11-18_/
#endif
bool Crop_complete::allow_natural_expiration()
{  bool ok_to_naturally_expire = true;
   if (parameters->is_annual() && CORN::is_zero<float64>
      (get_GAI_vital_plus_effete(),0.01)  )
      natural_expiration();
   else ok_to_naturally_expire  =false;                                          //040824
   return ok_to_naturally_expire;                                                //040824
}
//_allow_natural_expiration_________________________________________2004-08-05_/
bool Crop_complete::natural_expiration()
{  bool expired = true;
   log_event("Natural expiration");
   terminate_pending=true;                                                       //040514
   expired &= end_season();                                                      //160618
   Biomass_fate_parameters freezing_fate =                                       //040622
      {  0 // remove_for_consumption
      ,  0 // remove_as_grazing
      ,  0 // remain_as_residue
      ,  0 // remove_as_loss
      ,100 // remain_standing_dead
      ,  0 // remain_standing_live
      ,100 // remain_roots_as_dead_residue
      ,  0 // remain_roots_live
      ,  0 // remove_roots_as_loss
      ,0 };
   Crop_mass_fate_biomass_and_nitrogen *fate =                                   //070625
   process_biomass_fate(NATURAL_EXPIRATION_CAUSE,OTHER_BIOMASS_FATE
                     ,0,0,0,freezing_fate,true,0.0);                             //110603_040622
   //070625  Warning need to make sure that all the fate is accounted for
   //        once checked, delete this message
   if (fate) delete fate;                                                        //060323
   trigger_synchronization(NGS_TERMINATED); // End of season                     //040506
   return expired;
}
//_natural_expiration_______________________________________________2004-08-05_/
// The following are daily_history total accessors
float64 Crop_complete::get_GAI_today_vital()                               const
{ return canopy_leaf_growth ? canopy_leaf_growth->get_GAI(include_vital) : 0.0;} //190701
//_get_GAI_today_vital______________________________________________2005-08-23_/
float64 Crop_complete::get_GAI(nat8 include_GAI)                           const
{ return canopy_leaf_growth ? canopy_leaf_growth->get_GAI(include_GAI) : 0.0; }  //190701
//_get_GAI__________________________________________________________2005-08-23_/
float64 Crop_complete::get_GAI_vital_plus_effete()                         const
{  return (canopy_leaf_growth)
   ? canopy_leaf_growth->get_GAI(include_vital|include_effete)                   //050824
   : 0.0;
}
//_get_GAI_vital_plus_effete___________________________________________________/
float64 Crop_complete::get_GAI_vital()                                     const
{  return canopy_leaf_growth ? canopy_leaf_growth->get_GAI(include_vital) : 0.0; //190701
}
//_get_GAI_vital_______________________________________________________________/
float64 Crop_complete::get_LAI(bool from_canopy)                           const
{  return canopy_leaf_growth ? canopy_leaf_growth->get_LAI(from_canopy) : 0.0;   //190423_160627_060824
}
//_get_LAI_____________________________________________________________________/
float64 Crop_complete::get_peak_LAI(bool for_report)                       const
{ return
   canopy_leaf_growth ?                                                          //190423
      for_report
      ? reported.season.GAI_max                                                  //200122
      //200122 ? canopy_leaf_growth->get_reported_peak_LAI()
      : canopy_leaf_growth->get_peak_LAI()
   : 0.0 ;                                                                       //190423
}
//_get_peak_LAI_____________________________________________________2014-12-03_/
float64 Crop_complete::get_canopy_biomass_kg_m2_including(nat8 include_biomass) const
{  float64 sum_daily_canopy_biomass=
      canopy_leaf_growth ?                                                       //190423
      canopy_leaf_growth->get_biomass_current(include_biomass)                   //050823
      : 0.0;
   // The following line allows fastgraph to adapt the scale
   #if (defined (CROPSYST_PROPER) || defined(OFOOT_VERSION))
   if (include_biomass == (include_vital |include_effete))                       //070321
      CS_UED_max_canopy_biomass_encountered = std::max<float64>
         (CS_UED_max_canopy_biomass_encountered,sum_daily_canopy_biomass);
   #endif
  return sum_daily_canopy_biomass;
}
//_get_canopy_biomass_kg_m2_including__________________________________________/
float64 Crop_complete::get_live_green_canopy_biomass_kg_m2(nat8 include_biomass) const
{  float64 sum_daily_live_canopy_biomass =
      canopy_leaf_growth ?  // may be able to check this in the caller           //190423
      canopy_leaf_growth->get_live_green_biomass(true,true)                      //050823
      : 0.0;
  return sum_daily_live_canopy_biomass;
}
//_get_live_green_canopy_biomass_kg_m2_________________________________________/
#if (CROPSYST_VERSION < 5)
//191025 get_radiation_use_efficiency_PAR was here (moved to biomass_growth_legacy)
#endif
//_get_radiation_use_efficiency_PAR_________________________________2008-04-29_/
float64 Crop_complete::update_evapotranspiration_max(float64 ET_ref_)
{  // calculation of pot_crop_ET and GAI moved to Crop_common.
   float64 pot_crop_ET = Crop_common::update_evapotranspiration_max(ET_ref_);    //190812
   return pot_crop_ET;
}
//_update_transpiration_max_________________________________________2001-09-10_/
float64 Crop_complete::get_canopy_resistance()                             const
{  return
   #ifdef CO2_CHANGE
      CO2_response ? CO2_response->get_canopy_resistance() :
   #endif
      canopy_resistance_constant_d_m;
}
//_get_canopy_resistance____________________________________________2002-04-09_/
float64 Crop_complete::get_min_water_N_stress()                            const
{  float64 min_water_or_n_stress = water_stress_stats.get_current();
#ifdef NITROGEN
   if (nitrogen && !nitrogen->get_automatic_mode())                              //050722
   {  float64 N_stress = nitrogen->get_stress();
      min_water_or_n_stress =
         std::min<float64>(water_stress_stats.get_current(),N_stress);
   }
#endif
   return  min_water_or_n_stress;
}
//_get_min_water_N_stress___________________________________________2001-07-26_/
// Moved to Phenology_sequencer
static nat16 crop_event_count = 0;
void Crop_complete::log_event(const char *description)                     const
{  crop_event_count++;
   #ifndef MICROBASIN_VERSION
//std::ostream *ref_event_log = &std::cout; //190715
   if (ref_event_log)                                                            //050725
   {  std::string model_buffer;                                                  //190109
      (*ref_event_log)
      << "[crop_event_" << crop_event_count << "]" << std::endl
      << "date=" << simdate.get_date32()
            << " (" << simdate.as_string() << ")" << std::endl                   //030108
      << "type=" << description << std::endl // Currently using description for the type
      << "description=" << description << std::endl
      << "model=CropSyst:" <<
            parameters->crop_model_clad.append_label(model_buffer)               //190109
            <<std::endl
      << "RUSLE2_crop_name=" << parameters->RUSLE2_name << std::endl             //110831_110817
      << "RUSLE2_crop_description=" <<parameters->RUSLE2_description<<std::endl; //110831
   }
   #endif
}
//_log_event________________________________________________________2002-09-12_/
void Crop_complete::commit_biomass_fate()
{  if (biomass_fate_today)
   { log_event("biomass_fate");
      if (ref_event_log)
      {  biomass_fate_today->log(*ref_event_log);                                //050725
         (*ref_event_log) << "RUSLE2_crop_name=" << parameters->RUSLE2_name      //110831_110817
            << std:: endl;
      }
      if (biomass_fate_today->get_total_harvested() > 0.0)
      {  set_latest_harvest_date(simdate.get_date32());                          //130715
         trigger_synchronization(NGS_HARVEST);                                   //051021
         // Note this does not mark clipping events (may need to check get_total benificial use)
}  }  }
//_commit_biomass_fate______________________________________________2004-06-10_/
Crop_mass_fate_biomass_and_nitrogen *
Crop_complete::process_biomass_fate
(Biomass_fated_cause fated_cause
,Harvest_amount_mode harvest_amount_mode                                         //050822
,float64 remove_amount_kg_m2                                                     //050822
,float64 management_retain_GAI                                                   //050822
,float64 management_retain_biomass_kg_m2                                         //110603
,const Biomass_fate_parameters  &biomass_fate_parameters
,bool terminate                                                                  //040817
,float64 yield_now)
{  // returns the amount of material removed from the plant (for compatibility with remove_biomass_X)
   // This moves plant biomass to various pools, harvest yield, residue, loss etc.. //070802
   if (!canopy_leaf_growth) return 0;                                            //200414
   if (terminate) canopy_leaf_growth->terminate();                               //080222
/*190411 I am not sure why I invalidate this first,
it we need the GAI for the canopy to respond to clipping and reset the value for yesterday
in remove biomass (remove biomass will invalidate)
   canopy_growth->invalidate();                                                         //070802
*/
   float64 curr_canopy_biomass_kg_m2 =  get_canopy_biomass_kg_m2_including
      (include_vital|include_effete|include_canopy_dead);                        //031015
#ifdef NITROGEN
   N_mass_canopy_recorded_before_harvest                                         //070209
      = nitrogen                                                                 //070209
      ? nitrogen->get_mass_canopy_current()                                      //131006_110915
      : 0.0;                                                                     //070209
   Crop_mass_fate *N_mass_fate_now = nitrogen ? nitrogen->render_N_fate(fated_cause,0,0) : 0; //131005
#endif
   Crop_mass_fate *fate_now = new Crop_mass_fate(fated_cause,curr_canopy_biomass_kg_m2,act_root_biomass);
   float64 unharvested_canopy_biomass_kg_m2 = curr_canopy_biomass_kg_m2;         //040610
   float32 total_act_biomass_removed = 0;
   Crop_biomass canopy_removed;                                                  //070412
   if (!parameters->is_fruit_tree()                                              //200129_190625
       && (yield_now > 0.0000001))                                               //060103
   {  // In fruit crops, the yield amount is deducted from the canopy biomass every day so dont double delete

//071120 Note that could probably moved this to determine_yield

      unharvested_canopy_biomass_kg_m2 -= yield_now;
      // first remove any biomass that was taken for yield.
      total_act_biomass_removed = (float32)canopy_leaf_growth->remove_biomass
         (yield_now,management_retain_GAI,true,!terminate,canopy_removed);       //050823
   }
//cout  <<  get_canopy_biomass_kg_m2_including(include_vital|include_effete|include_canopy_dead) << endl;
   fate_now->yield = yield_now;
   #ifdef NITROGEN
   if (N_mass_fate_now)
   {
    N_mass_fate_now->yield = canopy_removed.get_nitrogen_mass();                 //131006
   }
   #endif
   int32 actual_remove_to_standing_dead = (int32)biomass_fate_parameters.remain_as_dead;  //050720
   int32 actual_remain_as_standing_live = (int32)biomass_fate_parameters.remain_as_live;  //050720
   if (terminate)                                                                //040817
   {  // we terminate the crop, so everything goes
      actual_remove_to_standing_dead += actual_remain_as_standing_live;
      actual_remove_to_standing_dead = std::min<int32>(actual_remove_to_standing_dead,100);
      actual_remain_as_standing_live = 0;
   }
   // Determine biomass removed
   float64 act_biomass_removed = 0;                                              //050824
   float64 act_N_removed = 0;                                                    //131006
   float64 removable_biomass =  unharvested_canopy_biomass_kg_m2-management_retain_biomass_kg_m2; // 110613
   switch (harvest_amount_mode)                                                  //050822
   {  case HARVEST_REMOVE_FIXED_AMOUNT_BIOMASS:                                  //050822
      {  remove_amount_kg_m2
            = std::min<float64>(remove_amount_kg_m2,removable_biomass);          //110613
         act_biomass_removed +=
            canopy_leaf_growth->remove_biomass
               (remove_amount_kg_m2,management_retain_GAI
               ,parameters->canopy_growth_LAI_based.clipping_removes_newest_first
               ,!terminate,canopy_removed);                                      //050823
         act_N_removed = canopy_removed.get_nitrogen_mass();                     //131006
      } break;
      case HARVEST_CROP_PARAMETER_BASED:// also applies to OTHER_BIOMATTER_FATE  //050822
      case HARVEST_PERCENT_BIOMASS :                                             //051003
      default:
      {  fate_now->useful           = unharvested_canopy_biomass_kg_m2 * (float32)biomass_fate_parameters.remove_for_use/100.0;
         // Special option, all biomass grazed is converted to manure, but some is applied to the field and some is removed because the animal might not be on the field all the time
         fate_now->grazing_to_manure   = unharvested_canopy_biomass_kg_m2 * (float32)biomass_fate_parameters.remove_for_grazing/100.0 *((float32)(biomass_fate_parameters.return_as_manure)/100.0);
         fate_now->grazing_to_disposal = unharvested_canopy_biomass_kg_m2 * (float32)biomass_fate_parameters.remove_for_grazing/100.0 *(1.0-(float32)(biomass_fate_parameters.return_as_manure)/100.0);
         fate_now->disposal            = unharvested_canopy_biomass_kg_m2 * (float32)biomass_fate_parameters.remove_for_disposal/100.0;
         fate_now->chaff_or_leaf_litter             = unharvested_canopy_biomass_kg_m2 * (float32)biomass_fate_parameters.remain_as_residue/100.0;

         float64 canopy_biomass_to_remove = 0.0; // We have already removed the yielded canopy biomass 040610
         canopy_biomass_to_remove += fate_now->useful;
         canopy_biomass_to_remove += fate_now->grazing_to_manure;
         canopy_biomass_to_remove += fate_now->grazing_to_disposal;
         canopy_biomass_to_remove += fate_now->disposal;
         canopy_biomass_to_remove += fate_now->chaff_or_leaf_litter;
         // The killed by the harvest procedure is needed because the
         // user can specify a percent that goes to dead standing stubble. 070802
         // Note that additional residue may be accumulated if the crop is terminated.
         // For HARVEST_CROP_PARAMETER_BASED normally all the residues with be taken in this case here.
         float64 killed_by_harvest_procedure = unharvested_canopy_biomass_kg_m2
            * (float32)actual_remove_to_standing_dead/100.0;                     //070802
         canopy_biomass_to_remove += killed_by_harvest_procedure;                //070802
         fate_now->stubble_dead    = killed_by_harvest_procedure;
         fate_now->uncut_canopy    = unharvested_canopy_biomass_kg_m2
            * (float32)actual_remain_as_standing_live/100.0;                     //051203
         if (fate_now->uncut_canopy < management_retain_biomass_kg_m2)           //110613
         {  fate_now->uncut_canopy  = management_retain_biomass_kg_m2;
            canopy_biomass_to_remove = removable_biomass;
         }
         act_biomass_removed += (float32)
         canopy_leaf_growth->remove_biomass
            (canopy_biomass_to_remove,management_retain_GAI
            ,parameters->canopy_growth_LAI_based.clipping_removes_newest_first
            ,!terminate,canopy_removed);                                         //050823

         #ifdef NITROGEN
         if (N_mass_fate_now)
         {
            act_N_removed += canopy_removed.get_nitrogen_mass();                 //131006
            N_mass_fate_now->useful                   = act_N_removed * (float64)biomass_fate_parameters.remove_for_use/100.0;
            N_mass_fate_now->grazing_to_manure        = act_N_removed * (float64)biomass_fate_parameters.remove_for_grazing/100.0 *((float64)(biomass_fate_parameters.return_as_manure)/100.0);
            N_mass_fate_now->grazing_to_disposal      = act_N_removed * (float64)biomass_fate_parameters.remove_for_grazing/100.0 *(1.0-(float64)(biomass_fate_parameters.return_as_manure)/100.0);
            N_mass_fate_now->disposal                 = act_N_removed * (float64)biomass_fate_parameters.remove_for_disposal/100.0;
            N_mass_fate_now->chaff_or_leaf_litter     = act_N_removed * (float64)biomass_fate_parameters.remain_as_residue/100.0;
            // NYIN_mass_fate_now->stubble_dead             = act_N_removed * (float64)
            // Any left over is presumed to be uncut canopy
            N_mass_fate_now->uncut_canopy             =  act_N_removed
               -  (N_mass_fate_now->useful
                  +N_mass_fate_now->grazing_to_manure
                  +N_mass_fate_now->grazing_to_disposal
                  +N_mass_fate_now->disposal
                  +N_mass_fate_now->chaff_or_leaf_litter
                  +N_mass_fate_now->yield);
         }
         #endif
      } break;
   } // end switch
   if ((harvest_amount_mode == HARVEST_REMOVE_FIXED_AMOUNT_BIOMASS)
    || (harvest_amount_mode == HARVEST_RETAIN_GAI_obsolete))                     //050824
   {  // Remove_biomass_history only removes as much as is available.
      fate_now->useful              = act_biomass_removed * (float32)biomass_fate_parameters.remove_for_use/100.0;   // additional harvested material that is not grain or fruit
      // Special option, all biomass grazed is converted to manure, but some is applied to the field and some is removed because the animal might not be on the field all the time
      float32 removed_for_grazing   = (float32)(act_biomass_removed * biomass_fate_parameters.remove_for_grazing/100.0);   //040806
      fate_now->grazing_to_manure   = removed_for_grazing * ((float32)(biomass_fate_parameters.return_as_manure)/100.0);       // will be available for the harvest model.
      // Note: grazing_to_manure is only used to determine the N that is cycled
      // in N_fate. Removed_for_grazing_to_manure in biomass fate otherwise has no meaning and should not be used for output
      fate_now->grazing_to_disposal = removed_for_grazing - fate_now->grazing_to_manure; // removed_for_grazing  // will be available for the harvest model. //040815
      fate_now->disposal            = act_biomass_removed * (float32)biomass_fate_parameters.remove_for_disposal/100.0;           // (I.e. from harvest or post harvest processing where material is not applied to the soil)
      fate_now->chaff_or_leaf_litter= act_biomass_removed * (float32)biomass_fate_parameters.remain_as_residue/100.0;
      fate_now->stubble_dead        = act_biomass_removed * actual_remove_to_standing_dead/100.0;
      fate_now->uncut_canopy        = unharvested_canopy_biomass_kg_m2 - act_biomass_removed;    // standing material //070221
      // Note that we dont set live standing to standing dead it terminate crop is specfied. 040806
      // The terminate crop will be triggered and processed latter.

      #ifdef NITROGEN
      if (N_mass_fate_now)                                                       //131006
      {
         N_mass_fate_now->useful              = act_N_removed * (float32)biomass_fate_parameters.remove_for_use/100.0;   // additional harvested material that is not grain or fruit
         // Special option, all biomass grazed is converted to manure, but some is applied to the field and some is removed because the animal might not be on the field all the time
         float32 removed_for_grazing   = (float32)(act_N_removed * biomass_fate_parameters.remove_for_grazing/100.0);   //040806
         N_mass_fate_now->grazing_to_manure   = removed_for_grazing * ((float32)(biomass_fate_parameters.return_as_manure)/100.0);       // will be available for the harvest model.
         // Note: grazing_to_manure is only used to determine the N that is cycled
         // in N_fate. Removed_for_grazing_to_manure in biomass fate otherwise has no meaning and should not be used for output
         N_mass_fate_now->grazing_to_disposal = removed_for_grazing - N_mass_fate_now->grazing_to_manure; // removed_for_grazing *((float32)(biomass_fate_parameters.manure_applied_to_field)/100.0);       // will be available for the harvest model. //040815
         N_mass_fate_now->disposal            = act_N_removed * (float32)biomass_fate_parameters.remove_for_disposal/100.0;           // (I.e. from harvest or post harvest processing where material is not applied to the soil)
         N_mass_fate_now->chaff_or_leaf_litter = act_N_removed * (float32)biomass_fate_parameters.remain_as_residue/100.0;
         N_mass_fate_now->stubble_dead         = act_N_removed * actual_remove_to_standing_dead/100.0;
         N_mass_fate_now->uncut_canopy         = unharvested_canopy_biomass_kg_m2 - act_N_removed;    // standing material //070221
         // Note that we dont set live standing to standing dead it terminate crop is specfied. 040806
         // The terminate crop will be triggered and processed latter.
      }
      #endif
   }  // else  HARVEST_CROP_PARAMETER_BASED and OTHER fate modes handled above
   total_act_biomass_removed += (float32)act_biomass_removed;
#ifdef QND
QND_daily_grazed_BM_kg_m2 += fate_now->grazing_to_manure;
QND_inc_ungrazed_BM_kg_m2 += fate_now->grazing_to_disposal + fate_now->removed_for_disposal;
#endif
   bool kill_everything
      = (fated_cause == FREEZING_DEATH_CAUSE)
      ||(fated_cause == NATURAL_EXPIRATION_CAUSE) || terminate;                  //060612
   if (kill_everything)
   {  float64 killed_biomass=canopy_leaf_growth
         ->remove_all_biomass(canopy_removed);                                   //070627
      fate_now->stubble_dead += killed_biomass;                                  //070626
      fate_now->uncut_canopy = 0;                                                //070201
      #ifdef NITROGEN
      if (N_mass_fate_now)                                                       //131006
      {  N_mass_fate_now->stubble_dead += canopy_removed.get_nitrogen_mass();    //070626
         N_mass_fate_now->uncut_canopy = 0;                                      //070201
      }
      #endif
      total_act_biomass_removed += (float32)killed_biomass;                      //070626
   }
   // The following are for roots
   if (estimate_root_biomass)                                                    //060612
      //  estimate_root_biomass is the old root biomass calculation
   {  float64 new_root_biomass = 0.0;                                            //040908
      for (short int sublayer = 1; sublayer <= MAX_soil_sublayers; sublayer++)   //980724
      {  float64 fract_roots_dead
         = (float64)biomass_fate_parameters.remove_roots_to_dead_residue/100.0;  //040908
         float64 total_fract_root_length_layer                                   //190102
            = (roots_current
               ? roots_current->get_total_fract_length_m()[sublayer] : 0.0);     //109102
         fate_now->roots_dead[sublayer]                                          //010713
            = total_fract_root_length_layer
             * act_root_biomass *fract_roots_dead;
         float64 fract_roots_live = 1.0 - fract_roots_dead;                      //040908
         float64 roots_live = fate_now->roots_live[sublayer]
            = total_fract_root_length_layer //190102 CROP_ROOT get_total_fract_root_length_m()[sublayer]
             * act_root_biomass * fract_roots_live;                              //040908
         // NYI currently not accounting for root loss                           //040908
         new_root_biomass += roots_live;                                         //040908
      }
      act_root_biomass = new_root_biomass;                                       //040908
   } else
   {  soil_layer_array64(root_biomass_before);
      clear_layer_array64(root_biomass_before);                                  //060612
#ifndef CROP_CALIBRATION
      if (soil && roots_current)roots_current->calc_biomass(root_biomass_before);//190102_060615
      else                                                                       //060615
#endif
         est_root_biomass(); // <- This is the case for the crop calibrator // But also if the crop is run with no soil (unlimited water) 060612
      if (kill_everything)                                                       //060612
      {  for (nat8 lyr = 1; lyr <= MAX_soil_sublayers; lyr++)                    //060612
            fate_now->roots_dead[lyr] = root_biomass_before[lyr];                //060612
            // note that fate_now->roots_live should already all be 0.           //070131
         if (roots_current) roots_current->kill();                               //190102_061208
         reported.daily.root_depth = 0;                                          //190705_070131
         //190705 recorded_root_depth = 0;                                       //070131
         fate_now->root_after = 0.0;   // actually should recalc current root mass to make sure   //070221
         // Don't set act_root_biomass to zero otherwise we won't know the value for the season report                  //070221
      } else                                                                     //060612
      {  // In the case of dormancy where we have some dead roots                //060612
         // and some live roots                                                  //070131
         if ((biomass_fate_parameters.remove_roots_to_dead_residue > 0) && roots_vital && fated_cause==DORMANCY_CAUSE)  //070131
         {  float64 fract_roots_dead = (float32)biomass_fate_parameters.remove_roots_to_dead_residue/100.0; //070131
            float64 fract_roots_live = 1.0 - fract_roots_dead;                   //070131
            act_root_biomass = 0.0;                                              //070131
            for (nat8 layer = 1; layer <= MAX_soil_layers; layer++)              //070131
            {  fate_now->roots_live[layer]
                  = root_biomass_before[layer] * fract_roots_live;               //070131
               act_root_biomass += fate_now->roots_live[layer];                  //070131
               fate_now->roots_dead[layer]
                  = root_biomass_before[layer] * fract_roots_dead;               //070131
            }                                                                    //070131
            float64 dormant_root_length = roots_vital->get_apparent_depth();     //070131
            delete roots_vital; roots_vital = 0;                                 //070131
            roots_current =                                                      //070131
#ifndef CROP_CALIBRATION
// Crop calibrator does not handle perennials                                    //070711
            roots_dormant =                                                      //070131
#endif
                  new CropSyst::Crop_root_dormant
                    (dormant_root_length,act_root_biomass,fate_now->roots_live); //070131
            fate_now->root_after = act_root_biomass;                             //070221
         } else                                                                  //070131
         { // else roots not touched.                                            //070205
            copy_layer_array64(fate_now->roots_live,root_biomass_before);        //070205
         }                                                                       //070205
      }                                                                          //060612
   }                                                                             //040908
/* debug
std::cout << "canopy_BM_before:" << curr_canopy_biomass_kg_m2 << std::endl;
float64 after_curr_canopy_biomass_kg_m2 =  get_canopy_biomass_kg_m2_including(include_vital|include_effete|include_canopy_dead);
std::cout << "canopy_BM_after:" << after_curr_canopy_biomass_kg_m2 << std::endl;
*/
#ifdef DEBUG_N_CONC
debug_N_conc_harvest_residue_root << biomass_fated_cause_label_table[fated_cause] << end;;
#endif
   fate_now->canopy_after -= (fate_now->useful + fate_now->grazing_to_manure
      +fate_now->grazing_to_disposal + fate_now->disposal
      + fate_now->chaff_or_leaf_litter + fate_now->stubble_dead);                //070221
   // Warning this canopy_biomass_after appears to be missing something,
   // So just setting it to the current biomass below
   fate_now->canopy_after =  get_canopy_biomass_kg_m2_including
      (include_vital|include_effete|include_canopy_dead);                        //070802
   record_biomass_fate(*fate_now,curr_canopy_biomass_kg_m2);
   #ifdef NITROGEN
   if (nitrogen)
   {
      N_mass_fate_now = nitrogen->apportion_fate
      (*fate_now
      ,N_mass_fate_now                                                           //131005
      ,canopy_removed
      ,harvest_amount_mode == HARVEST_CROP_PARAMETER_BASED
      ,include_vital/*roots*/ );
   }
   #endif

// WARNING!!!
// At this point the fate_now returned should have only masses that are
// to be redistributed, but I need to check every where this is returned to
// to make sure the fate returned is not used some other way.
// Before I add any code to clear fate
   return new Crop_mass_fate_biomass_and_nitrogen                                //060323
      #ifdef NITROGEN
      (fate_now,N_mass_fate_now);                                                //060323
      #else
      (fate_now,0);                                                              //140923
      #endif
}
//_process_biomass_fate_____________________________________________2007-06-25_/
float64 Crop_complete::record_biomass_fate
(const Crop_mass_fate &fate_now
,float64 curr_canopy_biomass_kg_m2)
{  if (!biomass_fate_today)  biomass_fate_today
      = new Crop_mass_fate(fate_now.cause,curr_canopy_biomass_kg_m2,get_act_root_biomass());
   biomass_fate_today->add(fate_now);
   if (!biomass_fate_season)  biomass_fate_season
      = new Crop_mass_fate(MULTIPLE_CAUSES,curr_canopy_biomass_kg_m2,get_act_root_biomass()); //040719
   biomass_fate_season->add(fate_now);                                           //040719
   return fate_now.get_total_removed_from_plant();
}
//_record_biomass_fate______________________________________________2004-08-09_/
#ifdef SEED_EMERGENCE_MODEL
bool Crop_complete::check_for_emergence()
{  bool first_day_of_emergence = emergence_hydrothermal_time->is_first_day(); // first cohort
   provide_canopy().start();                                                     //190703
   emergence_hydrothermal_time->process(*soil);
   if (first_day_of_emergence)
   {  log_event(TL_Emergence);
      trigger_synchronization(NGS_EMERGENCE);                                    //130925
      canopy_leaf_growth->restart_with // actually start with                    //190703
         (emergence_hydrothermal_time->get_cotyledon_biomass()
             ,emergence_hydrothermal_time->get_cotyledon_GAI(),true/*190713 ,false*/);
      phenology.activate_emergence();                                                //181108
   } else
   {  // additional initial portions
      float64 new_cotalydon_GAI = emergence_hydrothermal_time->get_cotyledon_GAI();
      if (new_cotalydon_GAI > 0.0)
      {  float64 new_cotalydon_BM =  emergence_hydrothermal_time->get_cotyledon_biomass();
         canopy_leaf_growth->emerge_cotyledon(new_cotalydon_GAI,new_cotalydon_BM);           //080728
         reported.season.biomass_production += new_cotalydon_BM;                 //190705_040719
   }  }
   if (emergence_hydrothermal_time->is_complete())
   {
      phenology.activate_emergence();                                                //181108
      trigger_synchronization(NGS_ACTIVE_GROWTH);                                //130925
      delete emergence_hydrothermal_time; emergence_hydrothermal_time = 0;
      emergence = 0;
   }
   return first_day_of_emergence;                                                //051205
}
//_check_for_emergence______________________________________________2004-12-01_/
#endif
bool Crop_complete::end_season_if_no_dormancy()
{  // Usually perennial crops go into dormancy,
   // and the restart after dormancy increments the season counter.
   // But this is not necessarily the case that dormancy is enabled
   // for perennial crops (We saw this with Elisenda's Spanish alfalfa scenario).
   // [Guillaumes Cullell, Elisenda]
   // In this case we need to identify the end of the season.
   // This is triggered by the clipping management event marked as last clipping.

//080401 I have redone season processing for non dormancy crops using inactive_periods
// this method is now probably obsolete

   if (parameters->continuation_mode_clad.get() != dormancy_season)              //200213_080331
   {
      /*190411 Now 'emergence_count' is incremented at the initiation of emergence
      season_count += 1;                                                               //050826
      */
      trigger_synchronization(NGS_QUIESCENCE);                                  //050826
         // 130902 I have removed the END_SEASON growth stage
         // Now I am simply assuming the crop will go into an inactive period
         // which is essentially what we presumed for end of season
         // (which we didn't have at the time). 
   }
   return end_season();                                                          //160629
}
//_end_season_if_no_dormancy________________________________________2005-08-18_/
float64 Crop_complete::get_carbon_fraction()                               const
{  float64 carbon_fraction = default_straw_residue_fraction_carbon;
   carbon_fraction = parameters->residue_decomposition.carbon_fraction;
   return carbon_fraction;
}
//_get_carbon_fraction______________________________________________2006-08-18_/
#ifdef NITROGEN
float64 Crop_complete::get_N_mass_canopy_kg_m2(bool before_harvest)        const
{  float64 N_mass_canopy = 0;
   if (nitrogen)
      N_mass_canopy = before_harvest
      ?  N_mass_canopy_recorded_before_harvest
      : nitrogen->get_mass_canopy_current();                                     //131006
   return  N_mass_canopy;
}
//_get_N_mass_canopy_kg_m2__________________________________________2007-02-09_/
#endif
bool Crop_complete::setup_quiescence_end_requirement()
{  // Note that this method is overridden for fruit crops
   quiescence_end_requirement = new Quiescence_end_requirement                   //030514
       (parameters->quiescence);                                                 //080331
   return quiescence_end_requirement != NULL;                                    //190709
}
//_setup_quiescence_end_requirement_____________________2016-03-16__2007-11-20_/
float64 Crop_complete::get_canopy_interception_global_total()              const
{ return canopy_leaf_growth
   ? canopy_leaf_growth->get_interception_global_total():0.0;}
//__________________________________________________________________2020-02-28_/
float64 Crop_complete::get_canopy_interception_global_green()              const
{ return canopy_leaf_growth
      ? canopy_leaf_growth->get_interception_global_green() : 0.0;}              //200228
//__________________________________________________________________2020-02-28_/
float64 Crop_complete::update_adjusted_ET_crop_coefficient()      rectification_
//200127 was get...
{  float64 param_ET_coef = parameters->transpiration.ET_coef;                    //110218
   //200127 now member because Claudio wants output float64
   adjusted_ET_coef = (param_ET_coef == 0.0)
   ? 1.0  // <- If no crop
   :  parameters->is_fruit_tree()                                                //190625
      ? param_ET_coef  // Note that Claudio wants to look into just how to interpret the ET crop coef. for fruit trees //060808
      : canopy_leaf_growth                                                       //1907XX
         ? canopy_leaf_growth->calc_adjusted_ET_crop_coefficient()               //200127
         : 1.0; // when no canopy  param_ET_coef; // Other crops may be adjusted by LAI
   return adjusted_ET_coef;
}
//_update_adjusted_ET_crop_coefficient______________________________2008-08-11_/
void Crop_complete::know_residues(Residues_interface *_residues)
{  ref_residues = _residues;
   if (canopy_leaf_growth)
      canopy_leaf_growth->know_residues(_residues);}
//_know_residues____________________________________________________2006-05-31_/
float64 Crop_complete::get_thermal_time_cumulative()                       const
{  return THERMAL_TIME_SOURCE get_accum_degree_days();
}
//_get_thermal_time_cumulative______________________________________2011-09-02_/
void Crop_complete::trigger_synchronization(Normal_crop_event_sequence event)
{
   #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
   phenology.days_since_start_of_0  [event] = 0; //200404 = 1;
   phenology.duration_of          [event] = 1;
   #endif
   bool fruit_crop_model = get_parameters()->is_fruit_tree();                    //190625
   Synchronization sync(fruit_crop_model,event);
   as_event_scheduler_crop->synchronize(sync,simdate);
}
//_trigger_synchronization__________________________________________2013-09-03_/
#ifdef CS_INSPECTOR_VERSION
bool Crop_complete::take_crop_ID_directory
(CORN::OS::Directory_name *crop_ID_directory_)                                   //160609
{
   crop_ID_directory = crop_ID_directory_;
   return true;
}
//_take_crop_ID_directory___________________________________________2016-06-09_/
#endif
bool Crop_complete::end_season()
{
   bool ended = end_period(0);
   #ifdef CS_INSPECTOR_VERSION
   // continue here add inspectors  to examination_global.singles
   #endif
   return ended;
}
//_end_season_______________________________________________________2016-06-09_/
bool Crop_complete::check_for_automatic_clipping
(const CropSyst::Auto_clip_biomass_mode &clip_biomass_op                         //051105
,float64 remove_amount_kg_m2                                                     //050822
,bool clip_now_or_never)                                                         //050929_980724
{
   #define clipping_low_GAI_rate  0.05
   bool clipping_performed = false;                                              //990913
   days_since_last_clipping++;
   if ((clip_biomass_op.biomass_fate_percents.remove_for_use                     //980723
       +clip_biomass_op.biomass_fate_percents.remove_for_disposal
       +clip_biomass_op.biomass_fate_percents.remain_as_residue) > 0)
   {  float64 quick_canopy_biomass =
            get_canopy_biomass_kg_m2_including(include_vital|include_effete);
      float64 GAI_today = get_GAI_today_vital();
         // this was only vital may may be want also effect
      float64 LAI = get_LAI(false);
      // RLN this is returning LAI for self shading response
      // I think we what the actual LAI  so simply call get_LAI();

      max_daily_GAI_rate = std::max<float64>(max_daily_GAI_rate,GAI_today);
      consecutive_low_growth_days_for_clipping +=
       ((GAI_today <= clipping_low_GAI_rate *clip_biomass_op.adj_relative_growth_rate)  //970521
          && (GAI_today < max_daily_GAI_rate)
          && (!air_temperature_avg || //normally there will always be air_temperature_avg
              (air_temperature_avg.Celcius() >
               get_parameters()->param_base_temp())))
         ? (nat16)1
         : (nat16)0;
      Biomass_fated_cause fated_cause =BIOMASS_FATE_COUNT;                       //050822
      float64   minimum_biomass_required_for_clipping_kg_m2 = per_ha_to_per_m2(clip_biomass_op.min_biomass_required_kg_ha); //051105
      float64   biomass_forces_clipping_kg_m2               = per_ha_to_per_m2(clip_biomass_op.biomass_forces_clipping); //051105
      bool sufficient_biomass_for_clipping = (quick_canopy_biomass >= minimum_biomass_required_for_clipping_kg_m2);   // I think minimum biomass is already handled by clip biomass 110428
      bool sufficient_LAI =  (LAI >= clip_biomass_op.min_LAI_required);          //110428
      bool try_to_get_in_one_more_harvest =                                      //980724
         (clip_now_or_never
          &&(quick_canopy_biomass>=minimum_biomass_required_for_clipping_kg_m2));//980724
      if (try_to_get_in_one_more_harvest)    fated_cause = CLIP_NOW_OR_NEVER;
      if (consecutive_low_growth_days_for_clipping > 14)
      {  if (sufficient_biomass_for_clipping && sufficient_LAI)                  //110428
         {  fated_cause = CLIPPING_DUE_TO_LOW_GROWTH;
            clipping_performed = sufficient_biomass_for_clipping;                //990913
         } else  //dry and dead:
            fated_cause = INSUFFICIENT_BIOMASS_FOR_FINAL_CLIPPING;
      }
      if (!(sufficient_biomass_for_clipping && sufficient_LAI)) return false;    //110502
      // for the next two options 0.0 essentially disabled the mode
      if (   (clip_biomass_op.LAI_forces_clipping > 0.0000001)
          && (LAI > clip_biomass_op.LAI_forces_clipping))
         fated_cause = CLIPPING_DUE_TO_MAX_LAI;
      if (   (biomass_forces_clipping_kg_m2 > 0.0000001)
          && (quick_canopy_biomass > biomass_forces_clipping_kg_m2))
         fated_cause = CLIPPING_DUE_TO_MAX_BIOMASS;
      #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
      int16 days_after_flowering =
         dynamic_cast<const Phenology_2013 &>(ref_phenology())
            .get_days_since_start_of(NGS_ANTHESIS);                              //181108
      #endif
      #if ((PHENOLOGY_VERSION==2018))
      const CS::Period *flowering = ref_phenology().get_anthesis_period_active();
         // actually this could be after flowering ends also, but in practice
         // clipping would be soon after flowering (for alfalfa)
      int16 days_after_flowering = flowering ? flowering->get_lapse() : 0;       //200508
      #endif
      int16 days_after_flowering_forces_clipping
         = clip_biomass_op.flowering_forces_clipping;                            //051105
      if ((days_after_flowering_forces_clipping )
       && (days_after_flowering >= days_after_flowering_forces_clipping))        //110502_040830
         fated_cause = CLIPPING_DUE_TO_DAYS_AFTER_FLOWERING;                     //040830
      const Biomass_fate_parameters *fate_parameters_to_use =  0;
      const Biomass_fate_parameters &biomass_fate_percents
         = clip_biomass_op.biomass_fate_percents;                                //051105
      Biomass_fate_parameters dry_and_dead_fate_parameters =                     //040610
            {  // All that would have been taken as harvest (any other loss from the harvest processing is sent to residue.
               0 // remove for consumption
               ,0 // remove as grazing
               ,(int16)(biomass_fate_percents.remain_as_residue
                  + biomass_fate_percents.remove_for_disposal
                  + biomass_fate_percents.remove_for_use)
               ,0 // remove_as_loss
               ,biomass_fate_percents.remain_as_dead
               ,biomass_fate_percents.remain_as_live
               ,biomass_fate_percents.remove_roots_to_dead_residue
               ,biomass_fate_percents.remove_roots_as_loss
               ,biomass_fate_percents.remain_roots_live
            };
      switch (fated_cause)
      {  case CLIPPING_DUE_TO_MAX_BIOMASS  :
         case CLIPPING_DUE_TO_MAX_LAI :
         case CLIPPING_DUE_TO_DAYS_AFTER_FLOWERING :                             //040830
         case CLIP_NOW_OR_NEVER           :
         {  clipping_performed = true;
            fate_parameters_to_use = &biomass_fate_percents;
         } break;
         case  CLIPPING_DUE_TO_LOW_GROWTH :
         {  fate_parameters_to_use = &biomass_fate_percents;
         } break;
         case INSUFFICIENT_BIOMASS_FOR_FINAL_CLIPPING :
         {  fate_parameters_to_use = &dry_and_dead_fate_parameters;
         } break;
         // By default fate_parameters_to_use=&biomass_fate_percents (set above) //050825
         default: break;                                                         //170217
      }
      if (fate_parameters_to_use)
      {  float64 biomass_removed = 0.0;                                          //060323
         Crop_mass_fate_biomass_and_nitrogen *fate_now                           //070625
            = clip_biomass
            (clip_biomass_op.harvest_amount_mode_cowl.get()                      //120419
            ,remove_amount_kg_m2
            ,clip_biomass_op.min_retain_GAI
            ,per_ha_to_per_m2(clip_biomass_op.reserve_biomass_kg_ha)
            ,*fate_parameters_to_use
            ,fated_cause);
         if (fate_now)                                                           //060323
         {  biomass_removed = fate_now->biomass->get_total_removed_from_plant();
            delete fate_now;
         }
         /* eventually all versions will use explainations

         #if (CS_VERSION==4)
         if (log)                                                                //050929
         {  std::string cause_description(harvest_mode_and_biomass_fated_cause_label_table[fated_cause]); //050929
            CORN::replace_character(cause_description,'_',' ');                  //050929
            log->record(
               #if (__LP64__ || _M_X64)
                  (uint64_t)
               #else
                  (nat32 )
               #endif
			this
			,"event",CropSyst_operation_codes[CS_OP_MGMT_AUTO_CLIP_BIOMASS],cause_description.c_str());  //050929
            (*log) << "biomass_removed=" << per_m2_to_per_ha(biomass_removed) << " kg/ha" << std::endl;  //050929
            (*log) << "note=See harvest report for biomass fate in this date." << std::endl; //050929
         }
         */
      }
      if (try_to_get_in_one_more_harvest)                                        //980724
            automatic_clipping_enabled = false;                                  //980724
   }
   return clipping_performed;
}
//_check_for_automatic_clipping_________________________2019-06-16__1999-09-13_/
Crop_mass_fate_biomass_and_nitrogen *                                            //070625
 Crop_complete::clip_biomass
(Harvest_amount_mode harvest_amount_mode                                         //050822
,float64 remove_amount_kg_m2                                                     //050822
,float64 min_retain_GAI                                                          //051103
,float64 min_retain_biomass_kg_m2                                                //110603
,const Biomass_fate_parameters &biomass_fate_percents
,Biomass_fated_cause fated_cause)                                                //050822
{  Crop_mass_fate_biomass_and_nitrogen *result                                   //070625
      = process_biomass_fate(fated_cause,harvest_amount_mode,remove_amount_kg_m2
      ,min_retain_GAI // retain_GAI
      ,min_retain_biomass_kg_m2                                                  //110603
      ,biomass_fate_percents,false,0.0);
   consecutive_low_growth_days_for_clipping = 0;
   max_daily_GAI_rate = 0.0;
   days_since_last_clipping = 0;
   respond_to_clipping();                                                        //040831
   return result;
}
//_clip_biomass_________________________________________2019-06-16__1997-06-22_/
float64 Crop_complete::param_emergence_deg_day()                           const
{
   float64 emerge_deg_day = 0; // warning currently 0 indicates not provided and cannot do computed planting
   #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
   emerge_deg_day = get_parameters()->phenology.initiation.emergence;
   #endif
   #if ((PHENOLOGY_VERSION==2018))
   Crop_parameters_struct::Phenologic_period *emerge_period_param =
      dynamic_cast<Crop_parameters_struct::Phenologic_period *>
      (parameters->phenology.phenologic_periods.find_cstr("phenology/emergence"));
   if (emerge_period_param)
      emerge_deg_day =emerge_period_param->initiation.thermal_time_accum;
   #endif
   return emerge_deg_day;
}
//_param_emergence_deg_day__________________________________________2019-06-17-/
float64 Crop_complete::est_degree_days_X_days_from_now                           //000330
(const CORN::Date_const & today, CORN::Days days_from_now)                 const //010202
{  // This is used when looking for planting conditions.
   float64 accum_deg_days_from_now = 0;
   accum_deg_days_from_now = THERMAL_TIME_SOURCE get_accum_degree_days(/*actually true was false*/);   //040830
   CORN::Date_clad_32 date_from_now(today.get_date32());                         //170530
    Air_temperature_maximum air_temp_max;                                        //190619
    Air_temperature_minimum air_temp_min;                                        //190619
   for (nat16 day_from_now = 0
       ;day_from_now <= (nat16)days_from_now
       ;day_from_now++)
   {
      weather.get_parameter_on(air_temp_max,UED::STD_VC_max_temperature.get(),date_from_now); //151124
      weather.get_parameter_on(air_temp_min,UED::STD_VC_min_temperature.get(),date_from_now); //151124
      float64 air_temp_max_forecasted = air_temp_max.Celcius();                  //151124
      float64 air_temp_min_forecasted = air_temp_min.Celcius();                  //151124

      #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))

      float64 deg_day_x = thermal_time->calc_growing_degree_day_daily_resolution
        (air_temp_max_forecasted
        ,air_temp_min_forecasted);                                               //010202
      float64 limited_deg_day_x = thermal_time->calc_limited_growing_degree_day  //010723
        (deg_day_x
        #ifdef THERMAL_TIME_OBSOLETE_2020
         // No vernalization factor because we dont want to start vernalization period.
         #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
         ,phenology.growth_stage
         #endif
         #endif
         );                                                                      //020629

      #endif
      #if ((PHENOLOGY_VERSION==2018))
      //181116 Need to implement
      assert(false);
      float64 limited_deg_day_x = 0;
      #endif

      accum_deg_days_from_now +=  limited_deg_day_x;
      date_from_now.inc_day();                                                   //170525
   }
   return accum_deg_days_from_now;
}
// moved from Crop_interfaced_completeX
//_est_degree_days_X_days_from_now__________________________________2019-06-17_/
bool Crop_complete::track_nitrogen_mass_balance
(CropSyst::Crop_nitrogen_mass_balancer *_mass_balance)                 const //200715LML mutation_
{
   bool test = true;
   std::cerr << "test=track_nitrogen_mass_balance " << test << "\n";


   return nitrogen ? nitrogen->track_mass_balance(_mass_balance) : 0;


}
//_track_nitrogen_mass_balance______________________________________2007-02-20_/
float64 Crop_complete::update_root_depth
(float64 soil_rooting_depth,float64 total_soil_depth)                            //981023
{  UNUSED_arg(soil_rooting_depth); UNUSED_arg(total_soil_depth);
   return
   #ifndef CROP_CALIBRATION
   Crop_complete::roots_current ?
   Crop_complete::roots_current->update_root_depth(terminate_pending)
   : 0.0;
   #else
   0.0;
   #endif
}
//_update_root_depth_____________________________________2019-06-19_1998-10-23_/
float64 Crop_complete::get_root_biomass
   (float64 *output_root_biomass_by_layer)                                 const
{  return (roots_current) ? roots_current->calc_biomass
      (output_root_biomass_by_layer) : 0.0;
}
//_get_root_biomass________________________________________________2007-12-15_/
#ifdef VIC_CROPSYST_VERSION
#ifdef CROPSYST_HANDLE_CANOPY_EVAP
float64 Crop_complete::get_intercepted_precipitation_m_stored()            const
{ return intercepted_precipitation_m_stored; }
bool Crop_complete::set_intercepted_precipitation_m_stored
(float64 intercepted_water_m)                                     modification_
{
   intercepted_precipitation_m_stored = intercepted_water_m;
   return true;
}
#endif
#endif
//_____________________________________________________________________________/
bool Crop_complete::is_at_season_start()                            affirmation_
{  return phenology.get_season_duration_days() == 1;
}
//_is_at_season_start_______________________________________________2020-04-04_/
bool Crop_complete::is_at_season_end()                              affirmation_
{  return is_terminate_pending()
       || phenology.is_at_quiescence();
}
//_is_at_end_season_________________________________________________2020-04-04_/
RENDER_INSPECTORS_DEFINITION(Crop_complete)
{
   #ifdef CS_OBSERVATION
   CS::Emanator *emanator =
       Crop_common::render_inspectors(inspectors,context,instance);
   phenology       .render_inspectors(inspectors,emanator,"phenology");
   if (nitrogen) nitrogen->render_inspectors(inspectors,emanator,"nitrogen");

   inspectors.append(new CS::Inspector_scalar(reported.daily.canopy_biomass,UC_kg_m2 ,*emanator,/*check ontology*/"canopy/biomass/cum"    ,sum_statistic,CSVC_crop_biomass_canopy));
      // biomass is already a cumulated value
/* NYI
   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_xxxxx   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_crop_base_green_biomass                                     ,PP_crop_base_green_biomass                                            ,));
   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_xxxxx   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_crop_base_fruit_biomass                                     ,PP_crop_base_fruit_biomass                                            ,));
   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_xxxxx   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_crop_base_fruit_canopy_biomass                              ,PP_crop_base_fruit_canopy_biomass                                     ,));
   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_xxxxx   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_crop_base_orchard_solid_canopy_interception                 ,PP_crop_base_orchard_solid_canopy_interception                        ,));
   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_xxxxx   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_crop_base_orchard_shaded_area_GAI                           ,PP_crop_base_orchard_shaded_area_GAI                                  ,));
   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_xxxxx   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_crop_base_orchard_canopy_porosity                           ,PP_crop_base_orchard_canopy_porosity                                  ,));
*/
   inspectors.append(new CS::Inspector_scalar(reported.daily.LAI   ,UC_index   ,*emanator,/*check ontology*/"leaf/area/index/LAI"    ,value_statistic,CSVP_crop_base_leaf_area_index));
   inspectors.append(new CS::Inspector_scalar(reported.daily.GAI   ,UC_index   ,*emanator,/*check ontology*/"leaf/area/index/GAI"    ,value_statistic,CSVP_crop_base_green_area_index));

   inspectors.append(new CS::Inspector_scalar(report_temperature_stress_stats.ref_current() ,UC_factor  ,*emanator,/*check ontology*/"stress/temperature/factor"  ,value_statistic,CSVP_crop_base_temperature_stress_factor));
   inspectors.append(new CS::Inspector_scalar(report_temperature_stress_stats.index         ,UC_index   ,*emanator,/*check ontology*/"stress/temperature/index"   ,value_statistic,CSVP_crop_base_water_stress_index));

/*NYI
   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_index   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_crop_base_tuber_init_temp_stress_index
   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_index   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_crop_base_tuber_fill_temp_stress_index
   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_index   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_crop_base_flower_temperature_stress_index
   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_index   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_crop_base_flower_water_stress_index
   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_index   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_crop_base_flower_N_stress_index
   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_index   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_crop_base_filling_duration_index
*/
   //100127   inspectors.append(new CS::Inspector_scalar(reported.daily.canopy_cover_green      ,UC_fraction,*emanator,/*check ontology*/"ground_cover_green"        ,value_statistic,CSVC_canopy_cover_green));
   inspectors.append(new CS::Inspector_scalar(reported.daily.canopy_cover_green      ,UC_fraction,*emanator,"canopy_cover_green"        ,value_statistic,CSVC_crop_base_fract_canopy_cover)); //200229 CSVC_canopy_cover_green));
   inspectors.append(new CS::Inspector_scalar(reported.daily.canopy_cover_total      ,UC_fraction,*emanator,"canopy_cover_total"        ,value_statistic,CSVC_crop_base_fract_canopy_cover_total)); //200229 CSVC_canopy_cover_total));
   //NYI inspectors.append(new CS::Inspector_scalar(reported_canopy_cover      ,UC_fraction,*emanator,/*check ontology*/"ground_cover_green"        ,value_statistic,CSVP_crop_base_fract_canopy_cover));
   inspectors.append(new CS::Inspector_scalar(intercepted_precipitation_m_today,UC_meter   ,*emanator,/*check ontology*/"water/interception"  ,value_statistic /*171031 sum_statistic*/,CSVP_crop_base_water_intrcpt/*171222 CSVC_crop_base_water_intrcpt_time_step_sum*/));

   // NYI inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_xxxxx   ,emanator,/*check ontology*/"XXX/XXX"    ,XXX_statistic,CSVP_crop_base_growth_stage

   // seasonal

   //180111 using final_statistic because these dates are set one time and shouldn't be tallied daily

   #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
   // actually this could be moved to phenology.know_observation(inspectors,emanator); as with 2018 version

   inspectors.append(new CS::Inspector_scalar(phenology.ref_planting_date()      ,UT_date   ,*emanator, /*need ontology*/"management/planting"        ,final_statistic,CSVP_crop_base_planting_date_YD));
   inspectors.append(new CS::Inspector_scalar(phenology.ref_emergence_date()     ,UT_date   ,*emanator,"phenology/emergence"                 ,final_statistic,CSVP_crop_base_emergence_date_YD));
   inspectors.append(new CS::Inspector_scalar(phenology.ref_anthesis_date()      ,UT_date   ,*emanator,"phenology/anthesis"                  ,final_statistic,CSVP_crop_base_flowering_date_YD));

   if (parameters->harvested_part_clad.get() == tuber_part)                      //171115
   inspectors.append(new CS::Inspector_scalar(phenology.ref_tuberization_date()  ,UT_date   ,*emanator,"phenology/tuber"                     ,final_statistic,CSVP_crop_base_tuber_init_date_YD));
   else
   inspectors.append(new CS::Inspector_scalar(phenology.ref_yield_formation_date(),UT_date   ,*emanator,"phenology/filling"                  ,final_statistic,CSVP_crop_base_grain_filling_date_YD));

   inspectors.append(new CS::Inspector_scalar(phenology.ref_culminescence_date() ,UT_date   ,*emanator,"phenology/accrescence"               ,final_statistic,CSVP_crop_base_peak_LAI_date_YD));
   if (parameters->phenology.maturity_significant)
   inspectors.append(new CS::Inspector_scalar(phenology.ref_maturity_date()      ,UT_date   ,*emanator,"phenology/maturity"                  ,final_statistic,CSVP_crop_base_maturity_date_YD));
   if (is_perennial())
   inspectors.append(new CS::Inspector_scalar(phenology.get_restart_date()       ,UT_date   ,*emanator,/*check ontology*/"phenology/restart" ,final_statistic,CSVP_crop_base_restart_date_YD));
   #endif

   #if ((PHENOLOGY_VERSION==2018))
   phenology.know_observation(&inspectors,emanator);                             //181109
   #endif
   inspectors.append(new CS::Inspector_scalar(latest_harvest_date                .ref_date32()   ,UT_date   ,*emanator, /*need ontology*/"management/harvest"              ,final_statistic/*180111 value_statistic*/,CSVP_crop_base_harvest_date_YD));

   /*NYI
   if (has_quiesence())
   {
   inspectors.append(new CS::Inspector_scalar(phenology.start_date[xxxx].ref_date32()   ,UT_date   ,*emanator, "phenology/quiescence/initiation" ,value_statistic,CSVP_crop_base_inactive_begin_date_YD));
   inspectors.append(new CS::Inspector_scalar(phenology.start_date[xxxx].ref_date32()   ,UT_date   ,*emanator, "phenology/quiescence/completion" ,value_statistic,CSVP_crop_base_inactive_end_date_YD));
   }
   */

   inspectors.append(new CS::Inspector_scalar(intercepted_radiation   ,UC_MJ_m2   ,*emanator,/*need ontology*/"biomass/production/RUE/PAR/intercepted"    ,value_statistic,CSVP_crop_base_intercepted_PAR));
/* NYI
   // NYI inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_xxxxx   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_crop_P_mass_uptake
   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_xxxxx   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_crop_C_mass_canopy
   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_xxxxx   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_crop_C_mass_roots_live
*/

// BIG WARNING these are just quick and dirty codes
// They have not been derived using my derivation conventions
// because eventually variable definitions will be based on the new ontology
// system, but currently UED still needs

   // Special outputs
   inspectors.append(new CS::Inspector_scalar(report_transpiration_use_eff       ,UC_kg_kg                  ,*emanator,/*check ontology*/"WUE"               ,value_statistic,CSVP_crop_transpiration_use_eff         )); //CSVC_crop_WUE));
   #ifdef CROPSYST_PROPER
   inspectors.append(new CS::Inspector_scalar(report_degree_hours_above_30_today ,UC_Celcius_degree_hours   ,*emanator,/*check ontology*/"DegHr_above_30"    ,sum_statistic  ,CSVC_crop_degree_hours_above_30_today   )); // already sum so use CVSC_
   inspectors.append(new CS::Inspector_scalar(report_degree_hours_above_34_today ,UC_Celcius_degree_hours   ,*emanator,/*check ontology*/"DegHr_above_34"    ,sum_statistic  ,CSVC_crop_degree_hours_above_34_today   )); // already sum so use CVSC_
   #endif

   inspectors.append(new CS::Inspector_scalar(reported.daily.ET_max_m            ,UC_m,*emanator,/*check ontology*/"ET/crop/max",value_statistic,CSVP_weather_ET_max )); //190705_171031
   inspectors.append(new CS::Inspector_scalar(reported.daily.ET_act_m            ,UC_m,*emanator,/*check ontology*/"ET/crop/act",value_statistic,CSVP_weather_ET_act)); //190705_171031

//disabled to see why reported_ET_pot_m is not output
   inspectors.append(new CS::Inspector_scalar(ref_phenology().ref_day_of_season(),UT_day ,*emanator,/*check ontology*/"duration_season" ,count_statistic,CSVC_crop_duration_season)); // already statistice so use CSVC
   #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
   thermal_time->render_inspectors(inspectors,emanator,"thermal_time");
   #endif
   #if ((PHENOLOGY_VERSION==2018))
   phenology.render_inspectors(inspectors,emanator,"thermal_time");
   #endif
   #ifdef CROPSYST_PROPER
   above_30C_season     ->render_inspectors(inspectors,emanator,"ascertaination_temperature_extreme"); //180917
   above_35C_season     ->render_inspectors(inspectors,emanator,"ascertaination_temperature_extreme"); //180917
   above_30C_flowering  ->render_inspectors(inspectors,emanator,"ascertaination_temperature_extreme"); //180917
   above_34C_flowering  ->render_inspectors(inspectors,emanator,"ascertaination_temperature_extreme"); //180917
   #endif
   return emanator;
   #else
   return 0;
   #endif
}
//_render_inspectors________________________________________________2013-07-30_/
}//_namespace CropSyst_________________________________________________________/

