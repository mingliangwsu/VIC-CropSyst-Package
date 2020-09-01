#include "crop/thermal_time_common.h"
#include "crop/crop_param.h"
#include "corn/math/statistical/stats.h"
#include "corn/math/compare.hpp"
#include "crop/growth_stages.h"
#include "common/weather/parameter/WP_temperature.h"
#include "CS_suite/observation/CS_inspector.h"
#include "csvc.h"
namespace CropSyst
{
//______________________________________________________________________________
Thermal_time_common::Thermal_time_common
( const Crop_parameters_struct::Thermal_time   &parameters_                      //110218
, const float64 &stress_adjusted_temperature_C_                                  //190812
, const float64 &air_temperature_min_C_                                          //190802
, bool                                          cropsyst_fruit_model_            //041212
,Vernalization                                 *vernalization_optional           //181116
,Photoperiodization                            *photoperiodization_optional)     //181116
:parameters                                                        (parameters_)
,cropsyst_fruit_model                                    (cropsyst_fruit_model_) //041212
,stress_adjusted_temperature_C                  (stress_adjusted_temperature_C_) //190812
,air_temperature_min_C                                  (air_temperature_min_C_) //190812
#ifdef THERMAL_TIME_OBSOLETE_2020
,clipping_recovery_deg_days_deprecated                                       (0) //040826
#endif
,growing_degree_day                                                          (0)
,consecutive_days_no_accumulation                                            (0) //030515
, vernalization                                         (vernalization_optional) //181116
, photoperiodization                               (photoperiodization_optional) //181116
{  clear_accum_degree_days();                                                    //140619
}
//_Thermal_time_common:constructor_____________________________________________/
void Thermal_time_common::clear_accum_degree_days()                modification_
{
 #ifdef THERMAL_TIME_OBSOLETE_2020
   #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
   for (int i = 0; i <= 1; i++)                                                  //140619
   {  accum_degree_days_normal [i] = 0.0;                                        //140619
      accum_degree_days_clipped[i] = 0.0;                                        //140619
   }
   #endif
   #if ((PHENOLOGY_VERSION==2018))
   GDDs = 0;                                                                     //181114
   #endif
   clipping_recovery_deg_days_deprecated = 0.0;                                  //040826
 #else
   GDDs = 0;                                                                     //200211
   GDDs_yesterday = 0;                                                           //200211
 #endif

   consecutive_days_no_accumulation = 0;                                         //030515
}
//_clear_accum_degree_days_____________________________________________________/
float64  Thermal_time_common::Vernalization::update()             rectification_
{  Vfactor =1.0;
   float64 temperature = temperature_with_est_night.Celcius();                   //181116
   float64 temp_low   = parameters.low_temp;                                     //020628
   float64 temp_high  = parameters.high_temp;                                    //020628
   float64 vern_start = parameters.start;                                        //020628
   float64 vern_end   = parameters.end;                                          //020628
   float64 min_factor = parameters.min_factor;                                   //160712
   float64 vern_day = (temperature > temp_high)
   ? (1.0 - (temperature - temp_high) / 7.0)
   : (temperature < temp_low)
      ? (1.0 - (temp_low - temperature) / 7.0)
      : 1.0;
   if (vern_day < 0.0)  vern_day = 0.0;
   vern_days += vern_day;
   if (vern_days >= vern_start)                                                  //160713
   {  Vfactor =
         min_factor + ( (1.0 - min_factor) *                                     //160712
           (vern_days - vern_start) / (vern_end - vern_start));
      if (status != VERNALIZATION_ACTIVE)
      {   // output vernalization starts to log was here
          status  = VERNALIZATION_START;                                         //020628
      }
   }
   if ((vern_days >= vern_end) && (status == VERNALIZATION_ACTIVE))
   {  Vfactor = 1.0;
      vern_days = 0.0;                                                           //021204
      status = VERNALIZATION_END;                                                //020628
   }
   Vfactor = CORN::must_be_between<float64>(Vfactor,0.0,1.0);                    //181116
   return Vfactor;
}
//_update________________________________________________________________1997?_/
Thermal_time_common::Photoperiodization::Photoperiodization
(const Crop_parameters_struct::Photoperiod &photoperiod_parameters_
,const float64                             &ref_day_length_hours_)
: photoperiod_parameters                               (photoperiod_parameters_)
, ref_day_length_hours                                   (ref_day_length_hours_)
, factor                                                                   (1.0)
{}
//_Photoperiodization::constructor__________________________________2018-11-18_/
float64 Thermal_time_common::Photoperiodization::update()         rectification_
{
   factor =1.0;  // 1 has no effect
   float64 stop_day_length          = photoperiod_parameters.stop_day_length;   // 0-24 hours
   float64 unconstrained_day_length = photoperiod_parameters.unconstrained_day_length; // 0-24 hours
   if (stop_day_length == unconstrained_day_length)
      return 1.0;     // Note: U = S not allowed
/*
Short-day crops
Daylength above which development towards flowering stops (hours)								14
Daylength below which development towards flowering is not constrained (hours)								10

Long-day crops
Daylength below which development towards flowering stops (hours)								10
Daylength above which development towards flowering is not constrained (hours)								14
*/
   float64 day_length_hours = ref_day_length_hours;                              //140812
   factor =
            (unconstrained_day_length >= stop_day_length)
            ?  // Long-day crop
               (day_length_hours <= stop_day_length)
               ? 0.0
               : day_length_hours >= unconstrained_day_length
                  ? 1.0
                  : 1.0 - ((day_length_hours - unconstrained_day_length)
                          /(stop_day_length  - unconstrained_day_length))
            :  // Short-day crop
               (day_length_hours >= stop_day_length)
               ? 0.0
               : (day_length_hours <= unconstrained_day_length)
                  ? 1.0
                  : 1.0 - ((day_length_hours - unconstrained_day_length )
                          /(stop_day_length - unconstrained_day_length));
  factor = CORN::must_be_between<float64>(factor,0.0,1.0);                       //181116
  return factor;
}
//_update_factor____________________________________________________2018-11-16_/
float64 Thermal_time_common::calc_growing_degree_day_hourly_resolution
( const CORN::Dynamic_array<float32> &stress_adj_hourly_temperature)calculation_
{  // Adapted from Environmental BioPhysics (Gaylon Campbell)
   CORN::statistical::Descriptive_progressive hourly_thermal_time; // deg hours  //200701
   for (int hr = 0; hr < 24; hr++)
   {
      float64  plant_temperature = stress_adj_hourly_temperature.get(hr);        //030610
      const Crop_parameters_struct::Thermal_time::Temperature &temperature
         = parameters.temperature;                                               //181111
      float64  thermal_time =   // Degree hours ?                                //130415
         (  (plant_temperature <= temperature.base)                              //130415
          ||(plant_temperature >= temperature.max ))                             //130415
         ? 0.0                                                                   //130415
         : (plant_temperature <= temperature.opt)                                //130415
            ? (plant_temperature - parameters.temperature.base)
            :  (temperature.opt - temperature.base) *                            //130415
               (1.0 - ( (plant_temperature - temperature.opt )                   //130415
                /(temperature.max - temperature.opt)));                          //130415
      thermal_time = CORN::must_be_0_or_greater<float64>(thermal_time);
      hourly_thermal_time.append(thermal_time);
   }
   // Warning we need to do senescence reduction etc...!!!
   float64 result_growing_degree_day = hourly_thermal_time.get_mean();
   return result_growing_degree_day;
}
//_calc_growing_degree_day_hourly_resolution________________________2002-11-24_/
float64 Thermal_time_common::calc_limited_growing_degree_day
(float64 growing_deg_day
#ifdef THERMAL_TIME_OBSOLETE_2020
#if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
,Normal_crop_event_sequence  growth_stage_sequence                               //130902
#endif
#endif
)                                       calculation_
{
   float64 limit_factor = 1.0;
  #ifdef THERMAL_TIME_OBSOLETE_2020
   #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
   float64 vernalization_factor = vernalization       ? vernalization->update() : 1.0;
   float64 photofactor          = photoperiodization  ? photoperiodization->update() : 1.0;
   #endif
   #if ((PHENOLOGY_VERSION==2018))
   float64 vernalization_factor = vernalization       ? vernalization->update() : 1.0;
   float64 photofactor          = photoperiodization  ? photoperiodization->update() : 1.0;
   #endif
  #else
   float64 vernalization_factor = vernalization       ? vernalization->update() : 1.0;
   float64 photofactor          = photoperiodization  ? photoperiodization->update() : 1.0;
  #endif


   {
   limit_factor = std::min<float64>(photofactor,vernalization_factor);
  #ifdef THERMAL_TIME_OBSOLETE_2020
   #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
   if ((growth_stage_sequence == NGS_GERMINATION)                                //130902
   ||  (growth_stage_sequence >  NGS_ANTHESIS))      limit_factor = 1.0;         //130902_041213
   #endif
   // in PHENOLOGY_2018 only specific periods will have vernalization or photoperiod
  #endif
   }
   float64 limited_growing_degree_day =  growing_deg_day * limit_factor;
   return  limited_growing_degree_day;
}
//_calc_limited_growing_degree_day__________________________________2000-03-30_/
double Thermal_time_common::accumulate_degree_days
#ifdef THERMAL_TIME_OBSOLETE_2020
(
#if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
 Normal_crop_event_sequence growth_stage_sequence                                //130911
#endif
)
#else
()
#endif

{  float64 vernalization_factor = 1.0;                                           //111109
   if (vernalization)                                                            //111109
   {  vernalization_factor = vernalization->update();                            //181116

      if (vernalization->is_satisfied())                                         //111109
      {
        #ifdef THERMAL_TIME_OBSOLETE_2020
         #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
         delete vernalization;
         #endif
        #endif
         vernalization = 0;                                                      //111109
      }
   }
   growing_degree_day =                                                          //010723
     calc_growing_degree_day_daily_resolution                                    //021125
     (stress_adjusted_temperature_C                                              //190812
     ,air_temperature_min_C);                                                    //190812
   float64 limited_grow_deg_day = calc_limited_growing_degree_day                //010723
      (growing_degree_day
      #ifdef THERMAL_TIME_OBSOLETE_2020
      #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
      , growth_stage_sequence
      #endif
      #endif
      );                             //130911
      //not sure if this is correct, it wasn't set up right, need to check if we still want to do this
   #ifdef THERMAL_TIME_OBSOLETE_2020
   if (clipping_recovery_deg_days_deprecated > 0.0)                                         //040826
   {
      #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
      accum_degree_days_normal[TT_TODAY]
      #endif
      #if ((PHENOLOGY_VERSION==2018))
      GDDs
      #endif
         += 0.001;                               //040826
         // Only slight accumulation to prevent any consecutive days of no accumulation problems
      clipping_recovery_deg_days_deprecated -= limited_grow_deg_day;             //040826
   } else                                                                        //040826
   #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
      accum_degree_days_normal[TT_TODAY] += limited_grow_deg_day;
   accum_degree_days_clipped[TT_TODAY] += limited_grow_deg_day;                  //040830
   #endif
   #if ((PHENOLOGY_VERSION==2018))
      GDDs += limited_grow_deg_day;                                              //181114
   #endif
   if (                                                                          //041213
       cropsyst_fruit_model &&   // Claudio decided again to restore this only for fruit, may need to eliminate it completely  //050816
     CORN::is_zero<float64>(growing_degree_day,0.001))
   {  // If there are a number of consecutive days of no thermal time accum,     //030515
      // then we throw away the accumulation (happens during winter).            //030515
      consecutive_days_no_accumulation +=1;                                      //030515
      if (consecutive_days_no_accumulation == 10)                                //030515
      {  clear_accum_degree_days();                                              //030515
         consecutive_days_no_accumulation = 0;                                   //030515
      }
   } else  consecutive_days_no_accumulation = 0;                                 //030515
   #else

      GDDs += limited_grow_deg_day;                                              //181114

   #endif
   return growing_degree_day;                                                    //010727
}
//_accumulate_degree_days______________________________________________________/
#ifdef THERMAL_TIME_OBSOLETE_2020
void Thermal_time_common::respond_to_clipping
(float64 new_thermal_time)                                         modification_
{
#ifdef disabled
this may be too long a period
clipping_recovery_deg_days_deprecated = 1.5 * new_thermal_time ;
#else
clipping_recovery_deg_days_deprecated = 0.0;
#endif
#if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
   accum_degree_days_clipped[TT_TODAY] = new_thermal_time;
#endif
}
//_respond_to_clipping______________________________________________2004-08-23_/
#endif

#if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
// in 2018 version we delete the affected periods
void Thermal_time_common::reset_to(float64 new_thermal_time)
{
  #ifdef THERMAL_TIME_OBSOLETE_2020
   #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
   accum_degree_days_clipped[TT_TODAY] = new_thermal_time ;
   #endif
   //200211 #if ((PHENOLOGY_VERSION==2018))
   //200211    GDDs = new_thermal_time;
   //200211    #endif
  #else
   GDDs = new_thermal_time;
   GDDs_yesterday = new_thermal_time;
  #endif
}
//_reset_to_________________________________________________________2004-07-19_/
#endif

#ifdef THERMAL_TIME_OBSOLETE_2020
float64 Thermal_time_common::get__accum_degree_days
(bool adjusted_for_clipping
,bool for_yesterday)
const
{
   return
   #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
      adjusted_for_clipping
      ? accum_degree_days_clipped[for_yesterday]
      : accum_degree_days_normal [for_yesterday];
   #endif
   #if ((PHENOLOGY_VERSION==2018))
   GDDs;
   #endif
}
//_get_accum_degree_days_______________________________________________________/
#else
float64 Thermal_time_common::get_accum_degree_days()
const
{  return GDDs;
}
//_get_accum_degree_days____________________________________________2020-02-11_/
#endif


Thermal_time_common::Vernalization::Vernalization
(const CropSyst::Crop_parameters_struct::Vernalization &parameters_
,const Physical::Temperature &temperature_with_est_night_)
:parameters                                                        (parameters_)
,temperature_with_est_night                        (temperature_with_est_night_)
,status                                                 (VERNALIZATION_INACTIVE)
,vern_days                                                                   (0)
,Vfactor                                                                   (1.0)
{}
//_Vernalization:constructor________________________________________2011-11-09_/
bool Thermal_time_common::end_day()                                modification_
{

   #ifdef THERMAL_TIME_OBSOLETE_2020
   #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
   //181114 yesterdays' value was only used to match thermal time,
   accum_degree_days_normal [TT_YESTERDAY]= accum_degree_days_normal [TT_TODAY];
   accum_degree_days_clipped[TT_YESTERDAY]= accum_degree_days_clipped[TT_TODAY]; //040823
   #endif
   // PHENOLOGY_2018
   // consecutive days of no accumulation should not be a problem in 2018 version
   // because new periods are instanciated.
   #else
   GDDs_yesterday = GDDs;
   #endif

   return true;
}
//_Thermal_time_common::end_day(____________________________________2014-06-19_/
RENDER_INSPECTORS_DEFINITION(Thermal_time_common)
{
   // uses crop emanator
   #ifdef THERMAL_TIME_OBSOLETE_2020
   #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
   inspectors.append(new CS::Inspector_scalar(accum_degree_days_clipped[0]  ,UC_Celcius_degree_days ,*context, "growing_degree_days/clipped",sum_statistic,CSVP_crop_base_growing_degree_days));
   inspectors.append(new CS::Inspector_scalar(accum_degree_days_normal[0]   ,UC_Celcius_degree_days ,*context, "growing_degree_days/normal" ,sum_statistic,CSVP_crop_base_seasonal_thermal_time));
   #endif
   #if ((PHENOLOGY_VERSION==2018))
   inspectors.append(new CS::Inspector_scalar(GDDs   ,UC_Celcius_degree_days ,*context, "growing_degree_days/normal" ,sum_statistic,CSVP_crop_base_seasonal_thermal_time));
   #endif
   #else

   inspectors.append(new CS::Inspector_scalar(GDDs  ,UC_Celcius_degree_days ,*context, "growing_degree_days/normal" ,sum_statistic,CSVP_crop_base_seasonal_thermal_time));

   #endif
   return 0;
}
//_RENDER_INSPECTORS_DEFINITION_____________________________________2013-09-09_/
}//_namespace CropSyst_________________________________________________________/

/*

Magnitude
Consequence
Significance
Weight

Scale
proportion
range
rate
ratio

scendence
ascendence
descendence

crescendo
decrescendo

curve shapes
linear up (ascending)
linear down (descending
trapazoid

(triangle)
nonlinear   (sinusoildal?)
*/
