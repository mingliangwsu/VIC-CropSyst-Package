#ifndef thermal_time_hourlyH
#define thermal_time_hourlyH

// This was removed from CropSyst 4  2015-11-09

#include "options.h"
#include "crop/thermal_time_common.h"
#include "CS_suite/observation/CS_inspection.h"
namespace CropSyst
{
//______________________________________________________________________________
class Thermal_time_hourly
: public implements_ Thermal_time_common
{
 public:
   virtual float64 calc_growing_degree_day_hourly_resolution                     //190130_021124
      (const CORN::Dynamic_array<float32> &stress_adj_hourly_temperature) calculation_;//030610
 public: // Constructor
   Thermal_time_hourly                                                           //080605
      (const CropSyst::Crop_parameters_class::Thermal_time    &parameters        //110218_080605
      , const float64 &stress_adjusted_temperature_C                             //190812
      , const float64 &air_temperature_min_C                                     //190812
      ,bool            cropsyst_fruit_model                                      //041212
      ,Vernalization        *vernalization_optional                              //181116
      ,Photoperiodization   *photoperiodization_optional );                      //181116
};
//_Thermal_time_hourly______________________________________________2002-06-28_/
}//_namespace CropSyst_________________________________________________________/
#endif

