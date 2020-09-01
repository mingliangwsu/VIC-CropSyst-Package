#include "CS_suite/CropSyst/extreme/stress_ascertaination.h"
#include "CS_suite/observation/CS_inspector.h"
#include "crop/phenology_I.h"
#include "CS_varderiv.h"
#if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
#include "crop/phenology_2013.h"
#endif
namespace CS
{
//_________________________________________________________________________
Stress_ascertaination::Stress_ascertaination
(const Parameters                      &parameters_
,const CropSyst::Phenology   &phenology_
,const CORN::Dynamic_array<float32>    &hourly_temperature_
         // For CropSyst flowering heat stress this is stress adjusted hourly
         // plant temperature, otherwise it can be simply air temperature.
,const CORN::date32 &simdate_raw_
)
: CS::Simulation_element_abstract                                   (simdate_raw_)
, parameters                                                       (parameters_)
, window_size_days_acheived                                              (false)
, phenology                                                         (phenology_)
, hourly_temperature                                       (hourly_temperature_)
, degree_hours_of_extreme_today                                              (0)
, degree_hours_of_extreme                                                  (366)
{
   nat32 abs_extreme_temperature = abs(parameters_.extreme_temperature);
   nat32 sign_extreme_temperature = parameters_.extreme_temperature < 0;
   nat32 qualifier = UED_period_sum;
   variable_code
      =  (CSC_crop_extreme_temperature  << 16)
         // CSV composition
      //NYN | (parameters_.stage_finalization  << 12)
         //#if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
         | (parameters_.stage_initiation  << 8)
         //#endif
         //#if ((PHENOLOGY_VERSION==2018))
         //| ( (parameters.during_active_growth? NGS_ACTIVE_GROWTH  : NGS_ANTHESIS) << 8)
         //#endif
         | (parameters_.high              << 7)
         | (sign_extreme_temperature      << 6 )
         | (abs_extreme_temperature)
      | ((nat32 )1 << 31) // non standard UED
      | ((nat32 )qualifier << 24)
      ;

//debug_stress_output <<  "stage" <<'\t' << "extreme" << '\t' <<  "date" << '\t'<< "hour" << '\t' << "temperature" << '\t' << "degree_hours_above_extreme" << std::endl;
}
//_Stress_ascertaination:constructor________________________________2018-08-14_/
bool Stress_ascertaination::end_day()
{
   #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
   const CropSyst::Phenology_2013 &phenology_2013 = dynamic_cast<const CropSyst::Phenology_2013 &>   //181103
      (phenology);                                                               //181103
   nat16 days_since_start_of_period
      = phenology_2013
         .get_days_since_start_of(parameters.stage_initiation);
   #endif
   #if ((PHENOLOGY_VERSION==2018))
   const CropSyst::Phenology &phenology_2018 = dynamic_cast<const CropSyst::Phenology/*_2018*/ &>   //181103
      (phenology);                                                               //181103
   nat16 days_since_start_of_period = 0;                                         //181114
   if (parameters.stage_initiation == NGS_ANTHESIS)
   {  const CS::Period *anthesis_period =                                        //181103
           phenology_2018.get_anthesis_period_active();                          //181103
      if (anthesis_period)                                                       //181114
         days_since_start_of_period =anthesis_period->get_lapse();               //200506_181103
   } else
      days_since_start_of_period = phenology_2018.day_of_season();               //181114
   #endif
   nat16 half_days_before_period_init = (parameters.window_size_days/2);
   if (days_since_start_of_period <= half_days_before_period_init)
   {
      // In the case of grain extreme damage we specify 7 day window on
      // either side of start of flowering
      // (include period before and after start of flowering)
      {
      degree_hours_of_extreme_today = 0;
      for (CORN::Hour hour = 0; hour < 24; hour++)
      {  float64 temperature_hour = 0;
         // temperature_hour = hourly_weather->get_air_temperature(hour);
         temperature_hour = hourly_temperature.get(hour);
         float64 degree_hours_above_extreme =
            (parameters.high ? (temperature_hour < parameters.extreme_temperature)
                     ? 0.0
                     : (temperature_hour - parameters.extreme_temperature)
                  : (temperature_hour > parameters.extreme_temperature)
                     ? 0.0
                     : 99999.99 // quite_NAN
                        // conceptual, low mode is not currently used.
                        // I am not sure how to characterise low extreme cases.
            );
         degree_hours_of_extreme_today += degree_hours_above_extreme;
      }
      degree_hours_of_extreme.append(degree_hours_of_extreme_today);
      }
   } else terminate(); // window satisfied
   if (days_since_start_of_period)
      terminate();
   return CS::Simulation_element_abstract::end_day();
}
//_end_day__________________________________________________________2018-09-14_/
RENDER_INSPECTORS_DEFINITION(Stress_ascertaination)
{
   #ifdef CS_OBSERVATION
   std::string ontology_def ("crop/tolerance/extreme/above_");
   CORN::append_nat16_to_string(parameters.extreme_temperature,ontology_def);
   ontology_def.append
      ((parameters.stage_initiation == NGS_ANTHESIS)
       ? "_anthesis"
       : "_season");
   inspectors.append(new CS::Inspector_scalar
      (degree_hours_of_extreme_today
      ,UC_Celcius_degree_hours
      ,*context, ontology_def.c_str()
      ,CORN::sum_statistic,0 /*variable_code*/));
   #endif
   return 0;
}
//_RENDER_INSPECTORS_DEFINITION_____________________________________2018-09-14_/
}//_namespace_CS_______________________________________________________________/

