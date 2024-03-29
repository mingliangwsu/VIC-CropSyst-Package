#include "weather/weather_provider.h"
#include "UED/library/std_codes.h"
#include <assert.h>
#include "weather/loc_param.h"
#include "corn/math/compare.hpp"
#include "solar/irradiance_extraterrestrial.h"
#include "biometeorology/ET_reference.h"
using namespace UED;
//______________________________________________________________________________
Weather_provider::Weather_provider
(const CORN::Date_const &date_
,const Geocoordinate    &geocoordinate_                                          //150629
,const CS::Solar_radiation_extraterrestrial  &ET_solar_radiation_                //151022
      // actually daily or hourly (haven't defined an interface yet)
,const Location_parameters    *location_)
:date(date_)
,geocoordinate(geocoordinate_)                                                   //150629
,season_S_W_SF(WINTER_ONLY_SEASON)//(_season_S_W_SF)
,ET_solar_radiation(ET_solar_radiation_)                                         //150625
,location(location_ ? location_ : new Location_parameters
   (*(new Geolocation)))
,location_owned(location_==0)
,VPDmax_parameters
   (const_cast<Vapor_pressure_deficit_max_calculator::Parameters &>
    (location->VPDmax_parameters))                                               //170804
,solar_radiation_generation_parameters (location->solar_radiation_parameters)
,air_temperature_max()
,air_temperature_min()
,air_temperature_avg(air_temperature_max,air_temperature_min)                    //160204
,vapor_pressure_deficit_max
   (season_S_W_SF
   ,&dew_point_temperature_max
   ,&relative_humidity_min
   ,&air_temperature_max
   ,&air_temperature_min)
,vapor_pressure_daytime
   (&dew_point_temperature_max
   ,&relative_humidity_min
   ,&air_temperature_max)
,vapor_pressure_nighttime
   (&dew_point_temperature_min
   ,&dew_point_temperature_max
   ,&air_temperature_min
   ,&vapor_pressure_daytime
   ,&relative_humidity_max)
,solar_radiation
(solar_radiation_generation_parameters
,ET_solar_radiation_
,air_temperature_max,air_temperature_min,air_temperature_avg)
,dew_point_temperature_max
   (&relative_humidity_min
   ,&air_temperature_max
   ,&vapor_pressure_deficit_max
   ,&dew_point_temperature_avg)
,dew_point_temperature_min
   (&relative_humidity_max
   ,&air_temperature_min
   ,&dew_point_temperature_max
   ,location
   ,&season_S_W_SF
   ,&vapor_pressure_nighttime // curr_vapor_pressure_min
   ,&dew_point_temperature_avg)//season
,dew_point_temperature_avg(&air_temperature_avg,&relative_humidity_avg)          //130218
,relative_humidity_max
   (&air_temperature_max    // May be 0 if unknown
   ,&air_temperature_min       // Required (070212 I had noted that this could be 0 if unknown, but that doesn't appear to be the case.)
   ,&dew_point_temperature_min
   ,&vapor_pressure_deficit_max // May be 0 if unknown
   ,&dew_point_temperature_avg
   ,&vapor_pressure_nighttime //curr_vapor_pressure_min
   ,&precipitation_actual)
,relative_humidity_min
   (&air_temperature_max     // May be 0 if unknown
   ,&dew_point_temperature_max
   ,&vapor_pressure_deficit_max
   ,&dew_point_temperature_avg)   // May be 0 if unknown
,relative_humidity_avg(0,0,0)                                                    //130218
,vapor_pressure_actual
               // 080617 Not sure which constructor to use
               // currently vapor_pressure_actual is only used to
               // instanciate curr_avg_fullday_vapor_pressure_deficit
               // at the time I added this, curr_avg_fullday_vapor_pressure_deficit
               // is intended to be provided in order to load from a file.
               // So I am setting the pointers to 0
               // but may eventually need to pass the respective parameters that are
               // currently commented out
    (&vapor_pressure_deficit_max
    ,&vapor_pressure_deficit_fullday_avg
    ,&dew_point_temperature_max
    ,&dew_point_temperature_min
    ,&relative_humidity_max
    ,&relative_humidity_min
    ,&air_temperature_max
    ,&air_temperature_min
    ,&dew_point_temperature_avg  ) // May be 0 if unkown
,vapor_pressure_deficit_fullday_avg
    (&vapor_pressure_actual
    ,&vapor_pressure_deficit_max
    ,&dew_point_temperature_max   // May be 0 if unknown  // used by Vapor_pressure_deficit_parameter and Vapor_pressure_actual_parameter
    ,&relative_humidity_min      // May be 0 if unknown  // used by Vapor_pressure_deficit_parameter and Vapor_pressure_actual_parameter
    ,&air_temperature_max  // May be 0 if unknown
   ,&air_temperature_min  )     // May be 0 if unknown
,vapor_pressure_deficit_daytime
    (&vapor_pressure_deficit_max,&vapor_pressure_deficit_fullday_avg)
,wind_speed                      ()
,atmospheric_pressure
 (geocoordinate_.get_elevation())                                                //150629
,latent_heat_vaporization        (air_temperature_avg)                           //140512
,vapor_pressure_saturation_slope (air_temperature_avg)                           //140512
,psychrometric_constant          (latent_heat_vaporization,atmospheric_pressure) //140512
,aerodynamic_resistance_ref_plants(0)                                            //150411
,aerodynamics                    (wind_speed)                                    //150215
,atmospheric_CO2_conc_ppm        (360)
,ET_reference                    ()                                              //180806
{  for (uint8 e = 0; e < 255 ; e++) element[e] = 0;
   element[0] = &precipitation_actual;
   element[1] = &air_temperature_max;
   element[2] = &air_temperature_min;
   element[3] = &air_temperature_avg;
   element[4] = &relative_humidity_max;
   element[5] = &relative_humidity_min;
   element[6] = &solar_radiation;
   element[7] = &dew_point_temperature_max;
   element[8] = &dew_point_temperature_min;
   element[9] = &dew_point_temperature_avg;
   element[10] = &wind_speed;
//NYI /*990802_*/    mutable float64 curr_precipitation_m;            mutable CORN::Quality precipitation_quality;
   element[11] = &vapor_pressure_deficit_max;
   element[12] = &vapor_pressure_daytime;
   element[13] = &vapor_pressure_nighttime;
   element[14] = &vapor_pressure_actual;
   element[15] = &vapor_pressure_deficit_fullday_avg;
   element[16] = &vapor_pressure_deficit_daytime;
   element[17] = &atmospheric_pressure;
   element[18] = &latent_heat_vaporization;                                      //140512
   element[19] = &psychrometric_constant; // depends on latent_heat_vaporization //140512

/* Can't seem to reference this (may be because of multiple inheritence
   element[21] = &vapor_pressure_saturation_slope;                               //140512
*/
   element[20] = &ET_reference;                                                  //180806
   element[21] = 0;
   vapor_pressure_deficit_max.know_generation_parameters(VPDmax_parameters);
}
//_Weather_provider:constructor_____________________________________2010-12-08_/
Weather_provider::~Weather_provider()
{  if (location_owned && location) delete location; location = 0;
}
//_Weather_provider:destructor______________________________________2010-12-09_/
void Weather_provider::invalidate(bool absolutely)
{  for (nat8 e = 0; e < 255; e++)
      if (element[e])
          element[e]->invalidate(absolutely);
}
//_invalidate_______________________________________________________2010-12-08_/
bool Weather_provider::update()                                     computation_
{  bool update_successful = true;
   CORN::DOY doy = date.get_DOY();                                               //101208
   if (geocoordinate.is_in_southern_hemisphere())                                //151215_110414_101208
   {  // Southern hemisphere
      if      ((doy > 357) || (doy < 81)) season_S_W_SF = SUMMER_ONLY_SEASON;    //081029 was ((doy > 334) || (doy < 91))
      else if ((doy > 151) && (doy <244)) season_S_W_SF = WINTER_ONLY_SEASON;    //040115
      else                                season_S_W_SF = SPRING_OR_FALL_SEASON; //040115
   } else
   {  // Northern hemisphere
      if      ((doy > 173) && (doy <265)) season_S_W_SF = SUMMER_ONLY_SEASON;    //081029 was  ((doy > 151) && (doy <274))
      else if ((doy > 334) || (doy < 60)) season_S_W_SF = WINTER_ONLY_SEASON;    //040115
      else                                season_S_W_SF = SPRING_OR_FALL_SEASON; //040115
   }
   // curr_max_vapor_pressure_deficit requires dewpt and rel humidity
   // see if we can get at least estimated values                                  050604
   // curr_max_relative_humidity and curr_min_dew_point_temperature now both can be calculated
   //   from VPD_max (aridity factor) and Tmax I am updating curr_max_vapor_pressure_deficit_kPa first
   //   so it will be ready for use by curr_max_relative_humidity and  curr_min_relative_humidity.
   //  This is kind of a hack doing this here, I should use a provide idiom.     //081016
   //151202 unused vapor_pressure_nighttime.know_DOY_unused (doy);               //081030
   // Update minimum relative humidity first because
   // it has the most stable estimation
   // and we need it first if we need to use the regression parameters
   // to produce vapor_pressure_minimum.
//NYI
   bool precipitation_valid = false; // NYI
   nat8 valid_element_count = 0;                                                 //151028
   for (nat8 e = 0; (e < 255) &&  element[e]; e++)
      if (element[e])
         valid_element_count += element[e]->update().is_valid();
   update_successful = valid_element_count;
      // Not all elements need to be valid (I.e. Dewpt avg);
      // I am not currently checking if critical elements are valid.
/*
   // We may have estimated value for dew point temperature to derive relative humidity
   // we will throw away estimated values and recalculate DewPoint from relative humidity
   relative_humidity_min.update();
   relative_humidity_max.update();
   dew_point_temperature_max.invalidate(false);
   dew_point_temperature_min.invalidate(false);
   dew_point_temperature_max.update(); // May calculate/estimate dew point depending on available data
   dew_point_temperature_min.update(); // so it may be available
*/

/* NYI from weather_base
   season_S_S_F_W=(Season_S_S_F_W)get_season_S_S_F_W();                          //050801
*/
/* NYI from weather_base
   static const float64 rain_to_snow_temperature = -1.5;
   bool snowed = (air_temperature_avg.get_Celcius() < rain_to_snow_temperature) && (precipitation.get() > 0.0);

   // WARNING snowed is only temporary for VIC,  it was previously determined by the snow pack

   solar_radiation.Sky::update_clouds                                            //991004
   (air_temperature_avg.get_Celcius()
    ,get_extraterrestrial_radiation(local_date_time.get_DOY())                   //011009
    ,solar_radiation.get_MJ_m2()
    ,snowed   // snowed()
   );
*/

//NYI   curr_effective_precipitation = precipitate();                            //991005
#ifdef NYI
This is from weather base

/*040116_*/    // Update all temperature estimators
/*060501_*/    FOR_EACH_IN(variable_est,Time_step_variable_estimator,time_step_temperature_estimators,each_est)
/*040116_*/    {
                  Temperature_estimator *temperature_est = (Temperature_estimator *)variable_est->estimator;
                  float64 Tmax_today = get_curr_max_air_temperature();
/*040116_*/       float64 Tmin_tomorrow = get_next_min_temperature();
/*040116_*/       temperature_est->set_max_today_min_tomorrow(Tmax_today,Tmin_tomorrow);
/*040116_*/    } FOR_EACH_END(each_est);
/*060501_*/    FOR_EACH_IN(variable_est,Time_step_variable_estimator,time_step_relative_humidity_estimators,each_est)
/*060501_*/    {
/*060501_*/       Relative_humidity_estimator *rel_humid_est = (Relative_humidity_estimator *)variable_est->estimator;
/*081106_*/       Quality RHmin_quality;
/*081106_*/       float64 RHmin_today = get_curr_min_relative_humidity(RHmin_quality);
/*081106_*/       float64 RHmax_tomorrow =  get_next_max_relative_humidity();
/*060511_*/       rel_humid_est->set_min_today_max_tomorrow(RHmin_today, RHmax_tomorrow);
/*060501_*/    } FOR_EACH_END(each_est);

    CORN::DOY doy = get_now().get_DOY();                                         //050610

   // actually should check days in year, but this is trivial
   if (atmospheric_CO2_change && (doy == 365))                                   //090316
      atmospheric_CO2_change->end_year(get_now().get_year());      // This used to be done by CS_ET and the land_unit_sim::end_year() //090316_
#endif
   update_successful &= vapor_pressure_saturation_slope.update().is_valid();     //151028_140512
   update_successful &= aerodynamics.update();                                   //150817RLN
      // Not currently in the array because couldnot get reference
   return update_successful;
}
//_update___________________________________________________________2010-12-08_/
const Vapor_pressure_saturation_slope &Weather_provider                          //140507
::ref_vapor_pressure_saturation_slope()                                    const
{  return vapor_pressure_saturation_slope;
}
//_ref_vapor_pressure_saturation_slope______________________________2014-05-07_/

