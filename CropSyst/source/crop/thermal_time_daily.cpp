#include "crop/thermal_time_daily.h"

namespace CropSyst
{
// ______________________________________________________________________________
Thermal_time_daily_linear::Thermal_time_daily_linear
(const CropSyst::Crop_parameters_struct::Thermal_time              &parameters_  //110218
,const float64                                  &stress_adjusted_temperature_C_  //190812
,const float64                                          &air_temperature_min_C_  //190812
,bool                                                     cropsyst_fruit_model_  //041212
,Vernalization                                                  *vernalization_  //181116
,Photoperiodization                                        *photoperiodization_) //181116
: Thermal_time_common
   (parameters_
   ,stress_adjusted_temperature_C_
   ,air_temperature_min_C_
   ,cropsyst_fruit_model_
   ,vernalization_
   ,photoperiodization_)
{}
//_Thermal_time_daily_linear:constructor_______________________________________/
float64 Thermal_time_daily_linear::calc_growing_degree_day_daily_resolution
(float64 adjusted_max_temp                                                       //030610
,float64 air_temp_min)                                              calculation_
{ // value for a single day
   float64 deg_day_base_temp   = parameters.temperature.base;
   float64 deg_day_cutoff_temp = parameters.temperature.cutoff;
   // During stress conditions the plant temperature may be a higher than air temperature
   float64 avg_plant_temp = (adjusted_max_temp + air_temp_min) / 2.0;
   avg_plant_temp = (avg_plant_temp < deg_day_base_temp)
      ? deg_day_base_temp : avg_plant_temp;
   avg_plant_temp = (avg_plant_temp > deg_day_cutoff_temp)
      ? deg_day_cutoff_temp : avg_plant_temp;
   float64 result_growing_degree_day = avg_plant_temp - deg_day_base_temp;
   return result_growing_degree_day;
}
//_calc_growing_degree_day_daily_resolution_________________________2000-03-30_/
Thermal_time_daily_nonlinear::Thermal_time_daily_nonlinear
( const CropSyst::Crop_parameters_struct::Thermal_time             &parameters_  //110218
,const float64         &stress_adjusted_temperature_C_                           //190812
,const float64         &air_temperature_min_C_                                   //190812
, bool                  cropsyst_fruit_model_                                    //041212
, Vernalization        *vernalization_                                           //181116
, Photoperiodization   *photoperiodization_                                      //181116
)
: Thermal_time_common
   (parameters_
   ,stress_adjusted_temperature_C_
   ,air_temperature_min_C_
   ,cropsyst_fruit_model_
   ,vernalization_
   ,photoperiodization_)
{}
//_Thermal_time_daily_nonlinear:constructor____________________________________/
float64 Thermal_time_daily_nonlinear::calc_growing_degree_day_daily_resolution
(float64 adjusted_max_temp                                                       //030610
,float64 air_temp_min)                                              calculation_
{ // value for a single day

/*
Simulation of Phenological Development of Wheat Crops
Enli Wang* & Thomas Engel
Informatics in Crop Production, Technical University of Munich, 85350 Freising, Germany
Agricultural Systems, Vol. 58, No. 1, pp. l-24, 1998PII: SO308-521X(98)00028-6
*/
   float64 Tmin = parameters.temperature.base;
   float64 Topt = parameters.temperature.opt;
   float64 Tmax = parameters.temperature.max;
   float64 avg_plant_temp = (adjusted_max_temp + air_temp_min) / 2.0;

   float64 WEDR = 0.0;
   float64 Topt_minus_Tmin = Topt - Tmin;
   if (avg_plant_temp > Tmin && avg_plant_temp < Tmax)
   {
      float64 Tavg_minus_Tmin = avg_plant_temp - Tmin;
      float64 beta = 1.0; // (Value of 1 is generally adequate for thermal time calculation
      float64 alpha = logl(2) / logl((Tmax - Tmin) / Topt_minus_Tmin);
      float64 alpha2 = 2.0 * alpha;
      float64 numerator = 2.0 * powl(Tavg_minus_Tmin, alpha) * powl
         (Topt_minus_Tmin, alpha) - powl(Tavg_minus_Tmin, alpha2);
      float64 denominator = powl(Topt_minus_Tmin, alpha2);
      WEDR = powl((numerator / denominator), beta);
   }
   float64 result_growing_degree_day = WEDR * Topt_minus_Tmin;
   return result_growing_degree_day;
}
//_calc_growing_degree_day_daily_resolution_________________________2015-11-06_/
}//_namespace CropSyst_________________________________________________________/

