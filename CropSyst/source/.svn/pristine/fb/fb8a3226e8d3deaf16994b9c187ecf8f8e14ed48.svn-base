#include "yield_temperature_extreme_response.h"
#include "corn/chronometry/time_conversion.h"
#include "corn/math/statistical/sample_dataset_T.h"
namespace CropSyst
{
//______________________________________________________________________________
//200422 was Harvest_temperature_stress_factor_grain

bool Yield_grain_temperature_extreme_response::update()            rectification_
{  if (current_period_sensitivity)
   {
      CORN::statistical::Sample_dataset_clad<float32,float32,float32> TSF_cold;   //180408
      CORN::statistical::Sample_dataset_clad<float32,float32,float32> TSF_heat;   //180408
         // original Statistical_data_set was float32 but could now be float64   //180408
         // Also I think I can now use sample_cumulative because only need means

      float64 hour;                                                              //130628
      hour = current_period_sensitivity->parameters.cold.exposure_initial_hour;  //130628
      for (int8 hour_count = 1; hour_count <= current_period_sensitivity->parameters.cold.exposure_damaging_hours; hour_count ++) //130628
      {
         if (hour > 23) hour = 0;
         float64 air_temperature_hour = stress_adj_hourly_temperature.get(hour);
         float64 cold_TSF_this_hour = current_period_sensitivity->temperature_stress_function
            (air_temperature_hour
            ,current_period_sensitivity->parameters.cold.no_stress_threshold_temperature
            ,current_period_sensitivity->parameters.cold.max_stress_threshold_temperature
            ,current_period_sensitivity->parameters.cold.sensitivity);
         //std::cout << "hour:" << (int)hour <<  "TSFcold:" << cold_TSF_this_hour << std::endl;
         TSF_cold.append(cold_TSF_this_hour);
         hour++;                                                                 //140209
      }
      hour = current_period_sensitivity->parameters.heat.exposure_initial_hour;  //130628
      if (hour > 24) // If the parameter file has hour > 24 (i.e. 99) then we use todays sunrise has the initial hour
         hour = sun_rise_time.get_hour();
      for (int8 hour_count = 1; hour_count <= current_period_sensitivity->parameters.heat.exposure_damaging_hours; hour_count ++) //130628
      {
         if (hour > 23) hour = 0;                                                //130628
         float64 air_temperature_hour = stress_adj_hourly_temperature.get(hour);
         float32 heat_TSF_this_hour = current_period_sensitivity->temperature_stress_function
            (air_temperature_hour
            ,current_period_sensitivity->parameters.heat.no_stress_threshold_temperature
            ,current_period_sensitivity->parameters.heat.max_stress_threshold_temperature
            ,current_period_sensitivity->parameters.heat.sensitivity);
         TSF_heat.append(heat_TSF_this_hour);
         hour++;                                                                 //140209
      }
      float64 TFS_cold_mean = TSF_cold.get_mean();
      float64 TFS_heat_mean = TSF_heat.get_mean();
      //std::cout << "dlymeanTSFcold:" << TFS_cold_mean << std::endl;
      current_period_sensitivity->cold.append(TFS_cold_mean);
      current_period_sensitivity->heat.append(TFS_heat_mean);
   }
   return true;
}
//_update___________________________________________________________2013-04-24_/
float64 Yield_grain_temperature_extreme_response::get_factor()              const
{  float64 factor = 1;
   FOR_EACH_IN(period_factor, Temperature_factors,period_temperature_factors,each_period)
   {  factor *= period_factor->effective_factor();
   } FOR_EACH_END(each_period)
   return factor;
}
//_get_factor_______________________________________________________2013-04-24_/
bool Yield_grain_temperature_extreme_response::add_period
(const Crop_parameters_struct::Temperature_sensitivity &parameters)
{  current_period_sensitivity = new Temperature_factors(parameters);
   period_temperature_factors.append(current_period_sensitivity);
   return true;
}
//_add_period_______________________________________________________2013-04-24__
}//_namespace CropSyst_________________________________________________________/

