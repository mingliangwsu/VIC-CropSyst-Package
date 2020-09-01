#include "common/weather/daily.h"
#include "common/weather/hour/sun_hours.h"
namespace CS
{
//______________________________________________________________________________
Weather_daily::Weather_daily
(const Geocoordinate              &geocoordinate_
,const CORN::Date_const           &today_
)
: geocoordinate   (geocoordinate_)
, today           (today_)
, weather_provider(0)
, sun_days        (0)
, sun_hours       (0)
, ET_solar_irrad  (0)
{}
//_Weather_daily:constructor________________________________________2016-07-29_/
CS::Solar_radiation_extraterrestrial_hourly &Weather_daily
::provide_ET_solar_irrad()                                           provision_
{
   if (!ET_solar_irrad)
   {
        ET_solar_irrad
         = new CS::Solar_radiation_extraterrestrial_hourly
            (geocoordinate,provide_sun_days(),provide_sun_hours(),today);
   }
   return *ET_solar_irrad;
}
//_provide_ET_solar_irrad___________________________________________2016-07-29_/
Sun_days &Weather_daily::provide_sun_days()                           provision_
{
   if (!sun_days)
   {
      float64 longitude_radians = geocoordinate.get_longitude_radians();          //151208_071116
      float64 standard_meridian_radians = longitude_radians;
      // Currently we do not have standard meridian in geolocation so simply using the longitude. 071116
      sun_days = new Sun_days( longitude_radians, standard_meridian_radians);
   }
   return *sun_days;
}
//_provide_sun_days_________________________________________________2016-07-29_/
Sun_hours &Weather_daily::provide_sun_hours()                         provision_
{
   if (!sun_hours)
      sun_hours = new Sun_hours
      (provide_sun_days(),geocoordinate.get_latitude_dec_deg());
   return *sun_hours;
}
//_provide_sun_hours________________________________________________2016-07-29_/
Weather_provider &Weather_daily::provide_weather_provider()           provision_
{
   if (!weather_provider)
      weather_provider =
         new Weather_provider
            (today,geocoordinate,provide_ET_solar_irrad());
            // currently no generation ,weather_database->get_location_parameters()
   return *weather_provider;
}
//_provide_weather_provider_________________________________________2016-07-29_/
bool Weather_daily::start()                                        modification_
{  bool started = true;
   started &= ET_solar_irrad->start();
   return started;
}
//_start____________________________________________________________2016-07-31_/
}//_namespace CS_______________________________________________________________/

