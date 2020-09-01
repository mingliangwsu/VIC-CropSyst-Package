#include "crop/thermal_time_hourly.h"
#include "corn/math/statistical/stats.h"
#include "corn/math/compare.hpp"
namespace CropSyst
{
//______________________________________________________________________________
Thermal_time_hourly::Thermal_time_hourly
( const CropSyst::Crop_parameters_class::Thermal_time    &parameters_            //110218
, const float64      &stress_adjusted_temperature_C_                             //190812
, const float64      &air_temperature_min_C_                                     //190812
, bool                 cropsyst_fruit_model_                                     //041212
,Vernalization        *vernalization_optional_                                   //181116
,Photoperiodization   *photoperiodization_optional_)                             //181116
: Thermal_time_common
   (parameters_
   ,stress_adjusted_temperature_C_                                               //190812
   ,air_temperature_min_C_                                                       //190812
   ,cropsyst_fruit_model_
   ,vernalization_optional_
   ,photoperiodization_optional_)
{}
//_Thermal_time_hourly:constructor_____________________________________________/
float64 Thermal_time_hourly::calc_growing_degree_day_hourly_resolution
( const CORN::Dynamic_array<float32> &stress_adj_hourly_temperature) calculation_
{  // Adapted from Environmental BioPhysics (Gaylon Campbell)
   statistical::Descriptive_progressive hourly_thermal_time; // deg hours        //200701
   for (int hr = 0; hr < 24; hr++)
   {
      float64  plant_temperature = stress_adj_hourly_temperature.get(hr);        //030610
      float64  temperature_base  = parameters.temperature.base;                  //181205
      float64  temperature_opt   = parameters.temperature.opt;                   //181205
      float64  temperature_max   = parameters.temperature.max;                   //181205
      float64  thermal_time =   // Degree hours ?                                //130415
         (  (plant_temperature <= temperature_base)                              //130415
          ||(plant_temperature >= temperature_max) )                             //130415
         ? 0.0                                                                   //130415
         : (plant_temperature <= temperature_opt)                                //130415
            ? (plant_temperature - temperature_base)
            : (temperature_opt - temperature_base) *                             //130415
              (1.0-((plant_temperature - temperature_opt)                        //130415
                   /(temperature_max - temperature_opt)));                       //130415
      thermal_time = CORN::must_be_0_or_greater<float64>(thermal_time);          //151110
      hourly_thermal_time.append(thermal_time);
   }
   // Warning we need to do senescence reduction etc...!!!
   float64 result_growing_degree_day = hourly_thermal_time.get_mean();
   return result_growing_degree_day;
}
//_calc_growing_degree_day_hourly_resolution________________________2002-11-24_/
}//_namespace CropSyst_________________________________________________________/

