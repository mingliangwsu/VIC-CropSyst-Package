#include "common/weather/day/weather_day.h"
#include "common/weather/day/sun_days.h"
#include "common/weather/database/weather_database.h"
//______________________________________________________________________________
Weather_day::Weather_day
(Weather_database             *weather_database_                                 //050307
,modifiable_ Weather_provider &provider_                                         //151201
,const Geocoordinate          &geocoordinate_                                    //150627
,float64                       screening_height_                                 //151128
,const CORN::Date_const       &today_                                            //150125
,bool                          treat_valid_data_as_real_                         //081109
,bool                          read_only_)                                       //000711

: Weather_with_solar_rad_estimation
   (weather_database_                                                            //050307
   ,provider_                                                                    //151201
   ,geocoordinate_                                                               //150702
   ,screening_height_                                                            //151120
   ,today_                                                                       //150125
   ,treat_valid_data_as_real_                                                    //081108
   ,read_only_)                                                                  //000711
,atmospheric_pressure_parameter(geocoordinate_.get_elevation())                  //150629
{}
//______________________________________________________________________________
void Weather_day::invalidate(bool absolutely)
{
   /*180201 didnt do anything anymore
   Weather_with_solar_rad_estimation ::invalidate(absolutely);
   */
   atmospheric_pressure_parameter     .invalidate(absolutely);
   curr_vapor_pressure_deficit_daytime.invalidate(absolutely);
}
//_invalidate_______________________________________________________2009-12-16_/
bool Weather_day::update()
{  return Weather_with_solar_rad_estimation::update();
}
//_update___________________________________________________________1999-10-12_/
float64 Weather_day::get_curr_act_vapor_pressure()                         const
{  return curr_vapor_pressure_actual.get_value_update_if_necessary();
}
//______________________________________________________________________________
float64 Weather_day::get_atmospheric_pressure_est_kPa()                    const
{  return atmospheric_pressure_parameter.get_value_update_if_necessary();        //150126
}//______________________________________________________________________________
float64 Weather_day::get_curr_avg_daytime_vapor_pressure_deficit()         const
{  return curr_vapor_pressure_deficit_daytime.get_value_update_if_necessary();   //150126
}
//_get_curr_avg_daytime_vapor_pressure_deficit______________________2005-08-01_/
//101216 this can be replaced by weather_provider
float64 Weather_day::get_curr_avg_fullday_vapor_pressure_deficit(Quality &quality) const
{  float64 value = curr_avg_fullday_vapor_pressure_deficit
      .get_value_update_if_necessary();                                          //150126
   quality.assume(curr_avg_fullday_vapor_pressure_deficit);
   return value;
}
//_get_curr_avg_fullday_vapor_pressure_deficit______________________2005-08-01_/

// This method should not be obsolete because it was using in ClimGen4
// but now ClimGen5 uses weather provider.  180315
float64 Weather_day::curr_vapor_pressure_actual_calc_saturation_kPa
(float64 at_temperature_C, float64 airtemp_C, bool over_water_only) modification_
{  return curr_vapor_pressure_actual.calc_saturation_kPa
      (at_temperature_C,airtemp_C,over_water_only);                              //081111
}
//_curr_vapor_pressure_actual_calc_saturation_kPa___________________2005-08-02_/
