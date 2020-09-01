#ifndef compareHPP
#  include "corn/math/compare.hpp"
#endif
#include <math.h>
#include "corn/math/moremath.h"
#include "aerodynamics.h"
using namespace CORN;
//______________________________________________________________________________
Aerodynamics::Aerodynamics
(const Wind_speed &_wind_speed                                                   //150215
,float64  _wind_measurement_height
,float64  _temperature_measurement_height)
: wind_speed                     (_wind_speed)                                   //150215
, wind_measurement_height        (_wind_measurement_height)
, temperature_measurement_height (_temperature_measurement_height)
, resistance_of_plants_reference_s_m (INTENSIVE,1.0e-6,UC_second_per_meter,unknown_quality) //150531
//, resistance_of_plants_reference_d_m (INTENSIVE,0.0,UC_day_per_meter,CORN::unknown_quality) //150531
   //deprecated
, resistance_of_snow_s_m(INTENSIVE,0.0,UC_second_per_meter,unknown_quality)      //150123
{}
//_Aerodynamics:constructor____________________________________________________/
float64 Aerodynamics::calc_resistance_general_form
(float64 wind_speed // any units
,float64 momentum_roughness_parameter_num          // usually meters
,float64 momentum_roughness_parameter_denom        // usually meters
,float64 heat_and_vapor_roughness_parameter_num    // usually meters
,float64 heat_and_vapor_roughness_parameter_denom  // usually meters
,float64 zero_plane_displacement_height            // usually meters
,float64 von_Karmon_constant) const
{  float64 momentum_term = CORN_ln(( wind_measurement_height - zero_plane_displacement_height
          + momentum_roughness_parameter_num) / momentum_roughness_parameter_denom);
   float64 heat_and_vapor_term = CORN_ln((temperature_measurement_height - zero_plane_displacement_height
          + heat_and_vapor_roughness_parameter_num)
          / heat_and_vapor_roughness_parameter_denom);
   float64 num = ( momentum_term * heat_and_vapor_term);
   float64 aero_resist = 0.0;
   if (is_approximately<float64>(wind_speed,0.0,0.00001))
   { // Need to avoid division by zero  //000120_
      //170104 this results in huge numbers aero_resist = 0.000001;
      aero_resist = 0.0012;                                                      //171208
         // based on 2 m/s windspeed
      resistance_of_snow_s_m.update_value_qualified_code(aero_resist,interpolated_quality);
   } else
   {  float64 denom = (wind_speed * CORN_sqr(von_Karmon_constant));
      aero_resist =  num / denom;
   }
   return aero_resist; // the returned units will vary
}
//_calc_resistance_general_form_____________________________________2000-11-09_/
float64 Aerodynamics::calc_resistance_of_plants_s_m
(float64 wind_speed_m_s // meters/sec
,float64 plant_height_m // meters
) const
{  float64 zero_plane_displacement = 0.6666666 * plant_height_m;
   float64 momentum_rough_param = 0.123 * plant_height_m;// (Zom)
   float64 heat_vapor_rough_param = 0.1 * momentum_rough_param; // (Zoh)
   float64 aero_resist_plant = calc_resistance_general_form
      (wind_speed_m_s,0.0,momentum_rough_param,0.0
      ,heat_vapor_rough_param,zero_plane_displacement);
   return aero_resist_plant;
} // This needs to be checked
//_calc_resistance_of_plants_s_m____________________________________2015-05-31_/
float64 Aerodynamics::calc_resistance_of_plants_d_m
(float64 wind_speed_m_d // meters/day
,float64 plant_height_m // meters
) const
{  float64 zero_plane_displacement = 0.6666666 * plant_height_m;
   float64 momentum_rough_param = 0.123 * plant_height_m;// (Zom)
   float64 heat_vapor_rough_param = 0.1 * momentum_rough_param; // (Zoh)
   float64 aero_resist_plant = calc_resistance_general_form
      (wind_speed_m_d,0.0,momentum_rough_param,0.0
      ,heat_vapor_rough_param,zero_plane_displacement);
   return aero_resist_plant;
} // check 011010
//_calc_resistance_of_plants_d_m____________________________________2000-11-09_/

/*
#  wind_roughness = (ln((zu-d+zm)/zm)*ln((zt-d+zh)/zh))/(u*k^2)
#
#  where  zu = height of wind measurements (2 m)
#         zt = height of temperature measurements (2 m)
#         d  = height of zero plane displacement (0 m)
#         zm = momentum roughness parameter (0.001 m)
#         zh = heat and vapor roughness parameter (0.0002 m)
#         k  = von Karmon constant (0.41)
#         u  = windspeed (m/s)
*/
float64 Aerodynamics::calc_resistance_of_snow
(float64 wind_speed_m_s
,float64 momentum_roughness_parameter
,float64 heat_and_vapor_roughness_parameter
,float64 von_Karmon_constant)
{  static const float64 zero_plane_displacement_for_snow_pack           = 0.0;   // m  //011008
   static const float64 momentum_roughness_parameter_for_snow_pack      = 0.001; // m  //011008
   static const float64 heat_and_vapor_roughness_parameter_for_snow_pack= 0.0002;// m  //011008
   if (is_approximately<float64>(wind_speed_m_s ,0,0.00001))
   { // Need to avoid division by zero  //000120_
      //150123 wind_speed_m_s = 3.0;
      resistance_of_snow_s_m.update_value_qualified_code(3.0,estimated_quality);
      //150121 aerodynamic_resistance_of_snow_s_m./*150121 measure.quality.set*/set_quality_code(UED_estimated);
   }
   float64 aero_resist_snow  // s/m
   =  calc_resistance_general_form                                               //011009
      (wind_speed_m_s
      ,momentum_roughness_parameter_for_snow_pack,momentum_roughness_parameter_for_snow_pack
      ,heat_and_vapor_roughness_parameter_for_snow_pack,heat_and_vapor_roughness_parameter_for_snow_pack
      ,zero_plane_displacement_for_snow_pack);
   return aero_resist_snow;
}
//_calc_resistance_of_snow_____________________________________________________/
bool Aerodynamics::update()                                                      //150215
{
   Quality_clad est_quality(estimated_quality);

   float64 wind_speed_at_standard_height_m_s                                     //150531
      = wind_speed.normalized_m_per_timestep(SECONDS_PER_SECOND);                //150531
   resistance_of_plants_reference_s_m.update_amount_in_preset_units(calc_resistance_of_plants_s_m
      (wind_speed_at_standard_height_m_s));                                      //150531
   resistance_of_snow_s_m.update_value_qualified(calc_resistance_of_snow
      (wind_speed.get_m_s()),est_quality);                                       //150215
   return true;
}
//_update______________________________________________________________________/
float64 Aerodynamics::calc_normalized_wind_speed(float64 wind_speed_m_ts) const
{  // This will work for any time step
   float64 normalized_WS_ts
    = is_approximately<float64>(wind_measurement_height,2.0,0.00001)             //011009
    ? wind_speed_m_ts                                                            //011009
    : wind_speed_m_ts * (4.87 / CORN_ln(67.8 * wind_measurement_height - 5.42));
   return normalized_WS_ts;
}
//_calc_normalized_wind_speed__________________________________________________/

