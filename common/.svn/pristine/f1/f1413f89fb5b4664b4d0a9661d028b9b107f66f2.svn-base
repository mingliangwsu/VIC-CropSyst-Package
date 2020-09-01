
#include "common/solar/irradiance_extraterrestrial.h"
#include "common/weather/hour/solar_irrad_hour.h"
#include "common/weather/parameter/WP_solar_radiation.h"
#include <math.h>
#include "corn/math/moremath.h"
#include "corn/math/compare.h"
#include "corn/chronometry/time_types.hpp"
//______________________________________________________________________________
Solar_radiation_hour::Solar_radiation_hour
(const Solar_radiation &_solar_radiation_daily                                   //140715
,const CS::Solar_radiation_extraterrestrial_hourly &_extraterrestrial_solar_radiation)
: solar_radiation_daily(_solar_radiation_daily)                                  //140715
, extraterrestrial_solar_radiation(_extraterrestrial_solar_radiation)            //150627
{  for (int h = 0; h < 24; h++)
   {  diffuse_solar_radiation[h] = 0;
      direct_solar_radiation[h] = 0;
      solar_radiation[h] = 0;
   }
}
//_Solar_radiation_hour:constructor____________________________________________/
bool Solar_radiation_hour::update()
{  // move the following to act_solar_rad
   if (!solar_radiation_daily.is_valid()) return false;                          //140715
   float64 act_solar_radiation_daily  = solar_radiation_daily.get_MJ_m2();       //140715
   float64 ET_irradiance_daily         =
      extraterrestrial_solar_radiation.get_MJ_per_m2_day();                      //150627
   float64 fraction_diffuse_irradiance = 1.0 - exp(1.0 - 0.96 / (std::min<float64>(0.8, act_solar_radiation_daily / ET_irradiance_daily)));
   for (CORN::Hour hour= 0; hour < 24; hour++)
   {
      solar_radiation        [hour] = act_solar_radiation_daily *
         extraterrestrial_solar_radiation.get_insolation_fraction_today(hour);   //150627
      diffuse_solar_radiation[hour] = solar_radiation[hour] * fraction_diffuse_irradiance;
      direct_solar_radiation [hour] = solar_radiation[hour] - diffuse_solar_radiation[hour];
   }
   return true;                                                                  //140715
}
//_update______________________________________________________________________/

