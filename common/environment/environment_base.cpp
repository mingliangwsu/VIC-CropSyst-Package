#include "common/environment/environment_base.h"

#include "soil/soil_I.h"
//______________________________________________________________________________
Environment_abstract::Environment_abstract()
:soil(0)                                                                         //020620
,owns_soil(false)                                                                //030710
,weather_today(0)
{}
//_Environment_abstract:constructor____________________________________________/
Environment_abstract::~Environment_abstract()
{  if (soil && owns_soil) delete soil; soil = 0;
}
//_Environment_abstract:destructor__________________________________2003-07-10_/
void Environment_abstract::set_soil(Soil_interface *_soil,bool _owns_soil)
{  soil        = _soil;                                                          //020620
   owns_soil   = _owns_soil;                                                     //030710
}
//_set_soil_________________________________________________________2006-07-03_/
float32 Environment_abstract::get_precipitation_today_m()                  const
{  return weather_today                                                          //141016
   ? weather_today->precipitation_actual.get_m()                                 //141016
   : 0.0;
}
//_get_precipitation_today_m___________________________________________________/
float32 Environment_abstract::get_curr_wind_speed_m_s()                    const
{ return weather_today ? weather_today->wind_speed.get_m_s() : 0.0;              //141016
} // meters per second
//_get_curr_wind_speed_m_s_____________________________________________________/
float64 Environment_abstract::get_curr_wind_speed_m_s(float64 height)      const //151014BRC
{ return weather_today
   ? weather_today->wind_speed.get_at_height_m_per_s(height) : 0.0;              //141016
} // meters per second
//_get_curr_wind_speed_m_s_____________________________________________________/
float32 Environment_abstract::get_avg_temperature()                        const
{  return weather_today
   ? weather_today->air_temperature_avg.Celcius()
   : 20.0;
}
//_get_avg_temperature_________________________________________________________/
float32 Environment_abstract::get_vapor_pressure_deficit_daily_kPa()       const
{ return weather_today
  ? weather_today->vapor_pressure_deficit_daytime.get_kPa()
   : 0.0;
}
//_get_vapor_pressure_deficit_daily_kPa__________________________2015-10-14BRC_/
CS::Temperature indeterminate_temperature(20.0);
// indeterminate_temperature is a place holder for environments which are not
// necessarily directly influenced by extern environmental temperature
// Default temperature 20Celcius
const Physical::Temperature &Environment_abstract::ref_ambient_temperature()const
{   return (weather_today)
      ? weather_today->air_temperature_avg
      : indeterminate_temperature;
}
//_indeterminate_temperature________________________________________2015-02-17_/
Wind_speed indeterminate_wind_speed(3);
Wind_speed &Environment_abstract::ref_wind_speed()
{  return (weather_today)
         ? weather_today->wind_speed
         : indeterminate_wind_speed;
}
//_ref_wind_speed________________________________________________2015-11-09BRC_/
