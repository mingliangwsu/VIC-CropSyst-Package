#include "crop/yield.h"
#include "corn/math/compare.hpp"
namespace CropSyst {
//______________________________________________________________________________
float64 Yield_abstract::sensitivity_effect
(float64 mean_period_stress
,float64 harvest_ndx_period_sensitivity_index)                      calculation_
{  static const float64  Stress_Threshold = 0.75;
   if (CORN::is_zero(mean_period_stress,0.0001))//avoid overflow                 //090514
      mean_period_stress = 0.0000001;                                            //010726
   return (CORN::is_zero<float64>(harvest_ndx_period_sensitivity_index) ||       //090514
           (mean_period_stress >= Stress_Threshold))
   ? 1.0   //then no stress effect
   : pow(mean_period_stress / Stress_Threshold,harvest_ndx_period_sensitivity_index);
}
//_sensitivity_effect_______________________________________________1998-08-24_/
float64 Yield_tuber::sensitivity_effect_B
(float64 mean_period_stress
, float64 harvest_ndx_period_sensitivity)                           calculation_
{  return (CORN::is_zero<float64>(harvest_ndx_period_sensitivity))               //090514
   ? 1.0   //then no stress effect
   : pow(mean_period_stress ,harvest_ndx_period_sensitivity);
}
//_sensitivity_effect_B_____________________________________________1998-08-24_/
float64  Yield_tuber::initiation_temperature_stress_factor()        calculation_
{  // Note we are currently passing the air temperature to represent the soil temperature
   float64 tolerance_heat_critcal_temperature = param_tuber_critical_temperature;
   float64 soil_temp = soil_temperature.Celcius();
   float64 stress_factor = (soil_temp <= 8.0)              // if low temperature stress condition
      ?  (1.0 - 0.05 * pow((soil_temp - 8.0) , 2.0))       // then low temperature stress curve
      :  (soil_temp > tolerance_heat_critcal_temperature)           // else if high temperature stress condition
         ? (soil_temp >= tolerance_heat_critcal_temperature + 8.0)  //      then if extreme temperature
            ? 0.0  // then full stress
            : (1.0 - pow(0.0156 * (soil_temp- tolerance_heat_critcal_temperature),2.0))
               // else high stress curve
         : 1.0; // else no stress.
   return CORN::must_be_0_or_greater<float64>(stress_factor); // low limit of 0
}
//_initiation_temperature_stress_factor_____________________________2008-11-09_/
float64 Yield_tuber::bulking_temperature_stress_factor()                   const
{   // Note we are currently passing the air temperature to represent the soil temperature
   static const float64 Tb    =  2.0;
   static const float64 TOpt1 = 15.0;
   static const float64 TOpt2 = 19.0;
   static const float64 Tmax  = 33.0;
   float64 slope1 = 1.0 / (TOpt1 - Tb);
   float64 slope2 = 1.0 / (Tmax - TOpt2);
   float64 soil_temp = soil_temperature.Celcius();
   float64 stress_factor =
    (soil_temp > Tb && soil_temp <= TOpt1)        ? (soil_temp - Tb) * slope1
      : (soil_temp >= TOpt2 && soil_temp <= Tmax) ? 1.0 - (soil_temp - TOpt2) * slope2
      : (soil_temp >  TOpt1 && soil_temp <  TOpt2)? 1.0
      : (soil_temp <= Tb)                         ? 0.0
      : (soil_temp >= Tmax)                       ? 0.0
      : 0.0;
   return stress_factor;
}
//_bulking_temperature_stress_factor________________________________2008-11-25_/
bool Yield_tuber::know_stress(float64 water_and_or_nitrogen_stress) assignation_
{  bool known = true;
   // tuber is temperature based so water and nitrogen stress is not used
   if (phenology.is_in_tuberization())
   {  float64 temperature_stress = initiation_temperature_stress_factor();
      tuber_init_temperature_stress_stats.append(temperature_stress);
   }
   else if (phenology.is_in_yield_formation())
   {  float64 temperature_stress = bulking_temperature_stress_factor();
      tuber_growth_temperature_stress_stats.append(temperature_stress);
   }
   return known;
}
//_know_stress______________________________________________________2008-11-25_/
float64 Yield_simple_harvest_index::harvest_index_1_stress_period
(float64 harvest_ndx_unstressed
,float64 mean_period_stress
,float64 harvest_ndx_period_sensitivity)                            calculation_
{   return harvest_ndx_unstressed
      * sensitivity_effect(mean_period_stress,harvest_ndx_period_sensitivity);
}
//_harvest_index_1_stress_period____________________________________1998-08-24_/
float64 Yield_tuber::calculate(float64 canopy_biomass_kg_m2)        calculation_
{  float64 stress_effect_initiation =  sensitivity_effect_B
            (tuber_init_temperature_stress_stats.get_mean()
            ,param_tuber_init_sensitivity);                                      //130406
   float64 stress_effect_bulking = sensitivity_effect_B
            (tuber_growth_temperature_stress_stats .get_mean()
            ,param_tuber_growth_sensitivity);                                    //130406
   float64 yield_now = canopy_biomass_kg_m2 * param_harvest_ndx_unstressed
      *  stress_effect_initiation * stress_effect_bulking;
   reported_harvest_index =
            canopy_biomass_kg_m2 > 0.00001                                       //150211
            ? yield_now / canopy_biomass_kg_m2                                   //120117
            : 0.0;                                                               //150211
            // reported_harvest_index added by RLN for completeness,
            // Claudio originally intended it only for seed.
   return yield_now;
}
//_calculate________________________________________________________2015-08_25_/
float64 Yield_canopy::calculate(float64 canopy_biomass_kg_m2)       calculation_
{  // Conceptual
    // used for silage
         float64 actual_yield = canopy_biomass_kg_m2 ;
         return actual_yield;
}
//_Yield_canopy:calculate___________________________________________2015-08-25_/
float64 Yield_simple_harvest_index::calculate(float64 canopy_biomass_kg_m2) calculation_
{  float64 stressed_harvest_biomass =
      param_apply_adjustment_to_unstressed_harvest_index
      ? canopy_biomass_kg_m2
          * harvest_index_1_stress_period
            (param_harvest_ndx_unstressed
            ,stress_since_emergence_stats.get_mean()                             //010726
            ,param_growth_sensitivity)
      : canopy_biomass_kg_m2;
   float64 yield_now =                                                           //030516
      std::min<float64>
         (canopy_biomass_kg_m2* (float64)param_harvest_ndx_unstressed
         ,stressed_harvest_biomass);
   if (canopy_biomass_kg_m2 > 0.0)
      reported_harvest_index = yield_now / canopy_biomass_kg_m2;                 //120117
   else
   {  reported_harvest_index = 0.0;
      std::cout << "No biomass production" << std::endl;
   }
      // reported_harvest_index added by RLN for completeness,
      // Claudio intended it only for seed.
   return yield_now;
}
//_Yield_simple_harvest_index::calculate____________________________2015-08-25_/
Yield_grain::Yield_grain
(float_P param_harvest_ndx_unstressed_
,bool    param_apply_adjustment_to_unstressed_harvest_index_
,bool    apply_heat_extreme_adjustment_to_unstressed_harvest_index_
,float_P param_translocation_fraction_max_
,Yield_grain_water_response          *flowering_water_response_submitted   //200423
,Yield_grain_temperature_response    *flowering_heat_response_submitted    //200423
,const Phenology &phenology_
,const CORN::Local_time &ref_sun_rise_time_
,const CORN::Dynamic_array<float32> &stress_adj_hourly_temperature_)
: Yield_abstract
                                                 (param_harvest_ndx_unstressed_
                           ,param_apply_adjustment_to_unstressed_harvest_index_
                                                                    ,phenology_)
, param_translocation_fraction_max           (param_translocation_fraction_max_)
, temperature_extreme_reponse                                                (0)
, flowering_water_response                  (flowering_water_response_submitted)
, flowering_heat_response                    (flowering_heat_response_submitted)
{
   if (apply_heat_extreme_adjustment_to_unstressed_harvest_index_)
      temperature_extreme_reponse
         = new Yield_grain_temperature_extreme_response
         (stress_adj_hourly_temperature_
         ,ref_sun_rise_time_);
}
//_Yield_grain:constructor__________________________________________2015-08-25_/
Yield_grain::~Yield_grain()
{  delete flowering_water_response;
   delete flowering_heat_response;
}
//__Yield_grain:destructor__________________________________________2020-04-23_/
bool Yield_grain::know_flowering()
{  return
      ((flowering_water_response && flowering_water_response->know_flowering()) || !flowering_water_response)
   && ((flowering_heat_response  && flowering_heat_response ->know_flowering()) || !flowering_heat_response);
}
//_Yield_grain::know_flowering______________________________________2020-04-23_/
bool Yield_grain::know_filling()
{  return
      ((flowering_water_response)   ? flowering_water_response->know_filling() : true)
   && ((flowering_heat_response)    ? flowering_heat_response ->know_filling() : true);
}
//_Yield_grain::know_filling________________________________________2020-04-23_/
bool Yield_grain::end_day()                                    modification_
{  return
       ((flowering_water_response && flowering_water_response->end_day()) || !flowering_water_response)
   &&  ((flowering_heat_response  && flowering_heat_response ->end_day()) || !flowering_heat_response);
}
//_Yield_grain::update______________________________________________2020-04-23_/
#include "harvest_grain_asymptotic.c"
float64 Yield_grain::calculate(float64 canopy_biomass_kg_m2)        calculation_
{  float64 yield_now = 0.0;
   float64 actual_yield = 0;                                                     //010322
   float64 actual_harvest_index = param_harvest_ndx_unstressed;                  //110903
   if (param_apply_adjustment_to_unstressed_harvest_index)
   {  float64 temperature_extreme_stress_factor =
           temperature_extreme_reponse
         ? temperature_extreme_reponse->get_factor() : 1.0;
      float64 flowering_water_stress_factor = flowering_water_response           //200422
         ? flowering_water_response->get_factor() : 1.0;                         //200422
      float64 flowering_heat_stress_factor  = flowering_heat_response            //200422
         ? flowering_heat_response->get_factor()  : 1.0;                         //200422
      float64 stress_factor =                                                    //200422
         std::min<float64>(temperature_extreme_stress_factor,                    //200422
         std::min<float64>(flowering_water_stress_factor                         //200422
                          ,flowering_heat_stress_factor));                       //200422
         float64 sink_strength_based_harvest_index
            =  param_harvest_ndx_unstressed
               //200423 * temperature_extreme_stress_effect                              //130424
               * stress_factor;                                                  //200423
         float64 fraction_biomass_produced_during_yield_formation
            = biomass_at_maturity > 0.000001
            ?  (biomass_at_maturity - biomass_at_filling)
             / biomass_at_maturity
            : 0;
#ifdef VIC_CROPSYST_VERSION
         //180703LML in case of the GDD cannot reach to maturity
         float64 biomass_at_harvest = biomass_at_maturity;
         if (biomass_at_harvest < biomass_at_filling) {
             biomass_at_harvest = canopy_biomass_kg_m2;
             fraction_biomass_produced_during_yield_formation
                = (biomass_at_harvest - biomass_at_filling) / biomass_at_harvest;
         }
#endif
         float64 source_limited_harvest_index =                                  //150519
            calc_source_limited_harvest_index                                    //130513
            (fraction_biomass_produced_during_yield_formation                    //161010
            , sink_strength_based_harvest_index                                  //150519_130510
                  // under certain conditions the harvest index can be slightly above (upto 10%) the parameter
            ,(float64)param_translocation_fraction_max);                         //120117
         actual_yield  = canopy_biomass_kg_m2 * source_limited_harvest_index;    //150519
         reported_harvest_index = source_limited_harvest_index;                  //150826
   } else
   {  reported_harvest_index = actual_harvest_index;                             //120117
      actual_yield = canopy_biomass_kg_m2 * actual_harvest_index;                //110907
   }
   yield_now = actual_yield;                                                     //081125
   return yield_now;                                                             //071120
}
//_Yield_grain::calculate___________________________________________2007-11-20_/
bool Yield_grain::know_stress(float64 water_and_or_nitrogen_stress) assignation_
{  bool in_veg_growth = phenology.is_in_accrescence();
   bool in_flowering  = phenology.is_flowering();
   if (temperature_extreme_reponse &&
       (in_veg_growth||in_flowering))
           temperature_extreme_reponse->update();                                //130425
   return true;
}
//_Yield_grain::know_stress_________________________________________2015-05-25_/
}//_namespace CropSyst_________________________________________________________/

