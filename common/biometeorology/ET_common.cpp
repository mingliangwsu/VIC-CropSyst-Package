#include "biometeorology/ET_common.h"
#include "weather/loc_param.h"
#include "biometeorology/ET_common.h"
#include "weather/weather_types.h"
#include <math.h>

#  include "weather/parameter/WP_solar_radiation.h"
#  include "weather/parameter/WP_atmospheric_pressure.h"
#  include "common/solar/irradiance_extraterrestrial.h"
#  include "weather/parameter/WP_vapor_pressure_act.h"
#  include "weather/parameter/WP_air_temperature.h"
//______________________________________________________________________________
ET_Reference_common::ET_Reference_common //190812 ET_common::ET_common
(float32 elevation_                                                              //150603
,const float64                               &day_length_hours_today_            //150627
,const Aerodynamics                          &aerodynamics_                      //150617
,const CS::Solar_radiation_extraterrestrial &solar_radiation_extraterrestrial_   //150625
,const Air_temperature_maximum  &air_Tmax_
,const Air_temperature_minimum  &air_Tmin_
,const Atmospheric_pressure     &atmospheric_pressure_parameter_                 //100421
,const Vapor_pressure_actual    &vapor_pressure_actual_                          //100421
,const Solar_radiation                  &solar_radiation_parameter_
,const Vapor_pressure_saturation_slope  &vapor_pressure_saturation_slope_        //140512
,const Latent_heat_vaporization         &latent_heat_vaporization_               //140512
,const Psychrometric_constant           &psychrometric_constant_)                //140512

: elevation(elevation_)                                                          //100421
, solar_radiation_extraterrestrial         (solar_radiation_extraterrestrial_)   //150625
, day_length_hours_today                               (day_length_hours_today_) //150627
, atmospheric_pressure                         (atmospheric_pressure_parameter_) //100421
, vapor_pressure_actual                                 (vapor_pressure_actual_) //100421
, air_temperature_max                                                (air_Tmax_)
, air_temperature_min                                                (air_Tmin_)
, solar_radiation                                   (solar_radiation_parameter_)
, slope_saturated_vapor_pressure_plus_psychrometric_resistance_reference         //150617
   ( vapor_pressure_saturation_slope
   , psychrometric_constant
   , aerodynamics_
   ,canopy_resistance_constant_d_m   )
,aerodynamics                                                    (aerodynamics_) //150617
,vapor_pressure_saturation_slope              (vapor_pressure_saturation_slope_) //140512
,latent_heat_vaporization                            (latent_heat_vaporization_) //140512
,psychrometric_constant                                (psychrometric_constant_) //140512
{}
//_ET_common:constructor____________________________________________2010-04-21_/
/*101206 This is copied here temporarily.
(It is a duplicate of the function is weather/daily.h
 I need to have it as a common parameter/element in weather provider
*/
float64 ET_Reference_common::get_cloud_factor_for_net_rad(float64 clear_sky_solar_rad) const
{
   // { This is similar to what is in ET_model, perhaps could be consolidated }
   float64 solar_rad = solar_radiation.get_MJ_m2();                              //991009
   float64 rad_ratio = solar_rad/clear_sky_solar_rad;
   rad_ratio = CORN::must_be_between<float64>(rad_ratio,0.0,1.0);                //150505
   float64 cloud_fact = 1.35 * rad_ratio -0.35;
   cloud_fact = CORN::must_be_between<float64>(cloud_fact,0.0,1.0);              //150505
   return cloud_fact;
}
//_get_cloud_factor_for_net_rad_____________________________________2010-12-06_/
float64 ET_Reference_common
::calc_isothermal_long_wave_net_radiation()                                const
{  // Net outgoing longwave radiation  MJ/(m day)
   float64 humid_cloud_or_delta_emiss_cloud_factor = 1.0;                        //020723
   // Determine the factor applied to Stefan_Boltzmann_law depending on available data
   {
      float64 act_vapor_pressure = vapor_pressure_actual.get_kPa();              //110911
      float64 humidity_factor = 0.34 - 0.14 * sqrt(act_vapor_pressure);          //010202
      float64 Ra  // MJ/m2  (per day)                                            //011008
         = solar_radiation_extraterrestrial.get_MJ_per_m2_day();
         // Should be daily derived from houly when available.
      float64 elevation_adjustment =  2.0e-5 * elevation;                        //100421_020320
      float64 Rso = (0.75 + elevation_adjustment) * Ra;
         // For compatibility with Claudio's V.B. code disable the elevation_adjustment 020320
      float64 cloud_factor = 1.0;
      cloud_factor = get_cloud_factor_for_net_rad(Rso);
      humid_cloud_or_delta_emiss_cloud_factor = humidity_factor * cloud_factor;  //020723
   }
   float64 S_B_law =  Stefan_Boltzmann_law();
   float64 Rnl = S_B_law * humid_cloud_or_delta_emiss_cloud_factor;              //020723
   return Rnl;
}
//_calc_isothermal_long_wave_net_radiation_____________________________________/
float64 ET_Reference_common::calc_net_radiation(bool day_light_hours)      const
{ // MJ/(m2/day or daytime) Note that this net_radiation allows
   // for either full day or daylight hours only.
   float64 reference_grass_albedo = 0.23;
   float64 day_fraction = day_light_hours ? (day_length_hours_today/24.0) :1.0;  //150627_000731
   float64 iso_lw_net_rad = calc_isothermal_long_wave_net_radiation();           //991004
   isothermal_long_wave_net_radiation = iso_lw_net_rad;                          //070204
   float64 short_wave_net_rad =                                                  //041108
      ( 1.0 - reference_grass_albedo ) * solar_radiation.get_MJ_m2();            //101205
      // This equation is duplicated in calc_shortwave_net_radiation
   float64 net_rad = short_wave_net_rad - iso_lw_net_rad * day_fraction;         //990215
   net_radiation = net_rad;                                                      //070204
   return net_rad;                                                               //980205
}
//_calc_net_radiation_______________________________________________2002-03-20_/
float64 ET_Reference_common::Stefan_Boltzmann_law()                        const
{  // The rate of longwave energy emission is proportional
   // to the absolute temperature of the earths surface.
   // This will later be corrected by humidity and cloudiness.
   float64 max_temp_k = air_temperature_max.kelvin();                            //101006
   float64 min_temp_k = air_temperature_min.kelvin();                            //101006
   float64 S_B_law = Stephan_Boltzmann_MJ_K4_m2_day
     * (pow(max_temp_k,4.0) + pow(min_temp_k,4.0)) / 2.0;
   return  S_B_law;
}
//_Stefan_Boltzmann_law_____________________________________________2002-03-20_/
float64 ET_Reference_common::calc_atmospheric_density
(float64 temperature_kelvin)                                               const
{  float64 virtual_temperature_K  = 1.01 * (temperature_kelvin);// Kelvin
   float64 atmospheric_pressure_  = atmospheric_pressure.get_kPa();              //100421
   float64 atmospheric_density
      = 3.486 * (atmospheric_pressure_ / virtual_temperature_K); // kg/m3
   return atmospheric_density;
}
//_calc_atmospheric_density_________________________________________2001-10-08_/
void ET_Reference_common::dump_header(std::ofstream &strm)
{  strm << "Tmax\t"<< "Tmin\t"<< "Ra\t" << "Srad\t" << "AtmsPress\t" << "VPact\t"  << "LWRiso\t" << "RadNet\t";
}
//_dump_header_________________________________________________________________/
void ET_Reference_common::dump_detail(std::ofstream &strm)
{  strm
        << air_temperature_max.Celcius()
        << "\t" << air_temperature_min.Celcius()
        << "\t"<< solar_radiation_extraterrestrial.get_MJ_per_m2_day()          //150827
        << "\t"<< solar_radiation.get_MJ_m2()
        << "\t"<< atmospheric_pressure.get_kPa()
        << "\t"<< vapor_pressure_actual.get_kPa()
        << "\t"<< get_isothermal_long_wave_net_radiation()
        << "\t"<< get_net_radiation()
        << "\t";
}
//_dump_detail______________________________________________________2010-12-12_/
const float64 ET_Reference_common::inverse_latent_heat_flux_vaporization = 0.408;          //131105
//__________________________________________________________________2013-11-05_/

