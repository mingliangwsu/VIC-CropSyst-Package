#ifndef VIC_weather_providerH
#define VIC_weather_providerH
//___________________________________________________________________________
#ifdef CROPSYST_PROVIDED_ET
// Using full weather provider
#else
#include "weather/parameter/WC_air_temperature.h"
#include "weather/parameter/WP_solar_radiation.h"
#include "weather/parameter/WP_psychrometric_const.h"
#include "weather/parameter/WP_vapor_pressure_deficit.h"
#include "weather/parameter/WP_vapor_pressure_saturation_slope.h"
#include "weather/parameter/WP_wind_speed.h"
#include "weather/parameter/WP_relative_humidity.h"
namespace VIC //141207RLN _crop
{
//___________________________________________________________________________
class Weather_provider
{
 public:
   Air_temperature_maximum                   air_temperature_max;
   Air_temperature_minimum                   air_temperature_min;
   Air_temperature_average_calculator        air_temperature_avg;
   Solar_radiation                           solar_radiation;
   Psychrometric_constant                    psychrometric_constant;
   Vapor_pressure_deficit_max                vapor_pressure_deficit_max;
   Vapor_pressure_deficit_daytime            vapor_pressure_deficit_daytime;
   Vapor_pressure_saturation_slope           vapor_pressure_saturation_slope;
   float64                                   atmospheric_CO2_conc_ppm;
   Wind_speed                                wind_speed;
   Relative_humidity                         relative_humidity_max;
   Relative_humidity                         relative_humidity_min;
   Relative_humidity                         relative_humidity_avg;
 public:
   inline bool update()                                         modification_
      {  return air_temperature_avg.update().is_valid();
      }
   inline virtual void invalidate(bool absolutely = true)
      {  air_temperature_avg.invalidate();
      }
};
//_2014-08-15________________________________________________________________
} // namespace VIC
#endif
#endif
