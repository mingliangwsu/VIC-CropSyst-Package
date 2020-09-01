#ifndef thermal_time_dailyH
#define thermal_time_dailyH

#include "crop/thermal_time_common.h"

namespace CropSyst
{
//______________________________________________________________________________
class Thermal_time_daily_linear
: public Thermal_time_common // NYI Thermal_time_daily_common
{
public:
   virtual float64 calc_growing_degree_day_daily_resolution                      //000330
      (float64 adjusted_max_temp                                                 //030610
      ,float64 air_temp_min)                                       calculation_;
      // This function calculates the normal growing degree day                  //019723
      // not limited by vernalization or photoperiod                             //019723

 public: // Constructor
   Thermal_time_daily_linear
      (const CropSyst::Crop_parameters_struct::Thermal_time   &parameters_       //110218_080605
      ,const float64      &stress_adjusted_temperature_C_                        //190812
      ,const float64      &air_temperature_min_C_                                //190812
      ,bool                  is_fruit_tree_                                      //041212
      ,Vernalization        *vernalization_                                      //181116
      ,Photoperiodization   *photoperiodization_);                               //181116
};
//_Thermal_time_daily_linear________________________________________2002-06-28_/
class Thermal_time_daily_nonlinear
: public Thermal_time_common // NYI Thermal_time_daily_common
{
 public:
   Thermal_time_daily_nonlinear
      (const               Crop_parameters_struct::Thermal_time    &_parameters  //110218_080605
      ,const float64      &stress_adjusted_temperature_C_                        //190812
      ,const float64      &air_temperature_min_C_                                //190812
      ,bool                  is_fruit_tree_                                      //041212
      ,Vernalization        *vernalization_                                      //181116
      ,Photoperiodization   *photoperiodization_);                               //181116
   virtual float64 calc_growing_degree_day_daily_resolution                      //000330
      (float64 adjusted_max_temp                                                 //030610
      ,float64 air_temp_min)                                       calculation_;
};
//_Thermal_time_daily_nonlinear_____________________________________2015-11-09_/
}//_namespace CropSyst_________________________________________________________/
#endif

