#include <math.h>
#include "corn/math/moremath.h"
#include "common/weather/loc_param.h"
#include "common/weather/wthr.h"
#include "common/weather/database/weather_database.h"
#ifdef WEATHER_GENERATION
#include "ClimGen/source/ClimGenF.h"
#endif
#ifndef compareHH
#  include "corn/math/compare.hpp"
#endif

#include "common/weather/parameter/WC_solar_radiation.h"

#define CURRENT_DAY CORN_date(0)
//______________________________________________________________________________
//          Constructor for text file format
Weather_with_solar_rad_estimation::Weather_with_solar_rad_estimation             //990301
(Weather_database    *_weather_database                                          //050307
,modifiable_ Weather_provider &provider_                                         //151201
,const Geocoordinate &_geocoordinate                                             //150627
,float64              screening_height_                                          //151128
,const CORN::Date_const &_today                                                  //150125
,bool                 _treat_valid_data_as_real                                  //081109
,bool                 _read_only)                                                //000711
:Weather_daily_deprecated
   (_weather_database                                                            //050307
   ,provider_                                                                    //151201
   ,_today                                                                       //150125
   ,_geocoordinate                                                               //150630
   ,screening_height_                                                            //151128
   ,_treat_valid_data_as_real                                                    //081109
   ,_read_only)                                                                  //000711
, generation_parameters(0)                                                       //091214
{}
//_Weather_with_solar_rad_estimation________________________________1999-03-01_/
float64 atmospheric_transmittance(float64 temperature, float64 pot_solar_rad)
// 991008 check if this is function is OK for non daily time step if it is we can move it to basewthr or sky
{  return CORN::must_be_greater_or_equal_to<float64>
     (0.7 * (1 - exp(-0.31 * CORN_sqr(temperature) /pot_solar_rad)),0);
}
//_atmospheric_transmittance________________________________________1998-10-08_/
float64 Weather_with_solar_rad_estimation::delta_emissivity()              const
   //991008 check if this is function is OK for non daily time step
   // if it is we can move it to basewthr or sky
{  float64 avg_air_temp =
      #ifdef WEATHER_PROVIDER_VERSION
      provider.air_temperature_avg.Celcius();
      #else
      ref_air_temperature_avg().Celcius();
      #endif
   float64 delta_emiss = -0.02 + 0.261 * exp (-7.8e-4 * CORN_sqr(avg_air_temp));
  return delta_emiss;
}
//_Weather_with_solar_rad_estimation________________________________1999-02-02_/
#ifndef WEATHER_PROVIDER_VERSION
float64 Weather_with_solar_rad_estimation::simple_atmospheric_transmissivity
( float64 temperature_range
, float64 day_30_pot_solar_rad
, float64 estimated_B_solar_rad                                                  //970711
, float64 C)                                                               const
{  float64 clear_sky_transm_coef = (generation_parameters)
      ? generation_parameters->clear_sky_transmission_coef
      : DEF_clear_sky_transmission_coef;                                         //091214
   return clear_sky_transm_coef
      * ( 1.0 - exp((-estimated_B_solar_rad * pow((temperature_range),C))
                    / day_30_pot_solar_rad));
}
#endif
//_simple_atmospheric_transmissivity___________________________________________/
float64 Weather_with_solar_rad_estimation::donatelli_atmospheric_transmissivity
( float64 temperature_range
, float64 fitted_B_solar_rad
, float64 adjusted_min_temp // fTnc
, float64 adjusted_avg_temp /* faTvg */ )                                  const
{  float64 clear_sky_transm_coef = (generation_parameters)
      ? generation_parameters->clear_sky_transmission_coef
      : DEF_clear_sky_transmission_coef;                                         //091214
   float64 to_power = -fitted_B_solar_rad * adjusted_avg_temp
      * CORN_sqr(temperature_range) * adjusted_min_temp;                         //040106
   float64 atmosph_transm = 0.8;                                                 //040106
/*
debug_sol_rad
<< local_date_time.get_year()<< '\t'
<< local_date_time.get_DOY()<< '\t'
<< fitted_B_solar_rad << '\t'
<<  adjusted_avg_temp <<  '\t'
<< temperature_range << '\t'
<< CORN_sqr(temperature_range) << '\t'
<< adjusted_min_temp << '\t'
<< clear_sky_transm_coef << '\t'
<< to_power << '\t';
*/
   atmosph_transm = clear_sky_transm_coef * ( 1.0 - exp(to_power));
   return atmosph_transm;
}
//_donatelli_atmospheric_transmissivity_____________________________1997-07-11_/
#ifndef WEATHER_PROVIDER_VERSION
#error check obsolete
float64 Weather_with_solar_rad_estimation::original_computed_solar_radiation
   (float64 temperature_range                             // rename to Bristow and Campbell
   ,float64 pot_solar_rad
   ,float64 day_30_pot_solar_rad
   ,float64 B_solar_rad)                                                   const
{  float64 simple_atmos_transm =   simple_atmospheric_transmissivity
    (temperature_range,day_30_pot_solar_rad,B_solar_rad);
   return pot_solar_rad * simple_atmos_transm;
}
#endif
//_original_computed_solar_radiation___________________________________________/
#ifndef WEATHER_PROVIDER_VERSION
#error check obsolete
float64 Weather_with_solar_rad_estimation::donatelli_campbell_computed_solar_radiation
(float64 temperature_range,float64 pot_solar_rad)                          const
{
/*
static  bool debug_sol_rad_printed = false;
if (!debug_sol_rad_printed)
{
debug_sol_rad
<< "year" << '\t'
<< "DOY" << '\t'
<< "fitted_B_solar_rad" << '\t'
<< "adjusted_avg_temp" <<  '\t'
<< "temperature_range" << '\t'
<< "temp_range^2" << '\t'
<< "adjusted_min_temp" << '\t'
<< "clear_sky_transm_coef" << '\t'
<< "to_power" << '\t'
<< "act_atmo_trans" << '\t'
<< "pot_solar_rad" << '\t'
<< "act_solar_rad" << '\t'
<< std::endl;
}
*/
   float64 Tnc = generation_parameters
      ? generation_parameters->Tnc
      : TYPICAL_Tnc_solar_rad;
   float64 adjusted_min_temp = exp(get_curr_min_air_temperature() / Tnc);  // was DEF_Tnc_solar_rad but Needed a value for Elisenda //050916
   float64 avg_temp = ref_air_temperature_avg().Celcius();
   float64 adjusted_avg_temp  = 0.017 * exp(exp(-0.053 * avg_temp));
   float64 fitted_B_solar_rad = generation_parameters
      ? generation_parameters->fitted_B : TYPICAL_fitted_B_solar_rad;            //070313
   float64 act_atmospheric_transmissivity = donatelli_atmospheric_transmissivity
     (temperature_range,fitted_B_solar_rad,adjusted_min_temp,adjusted_avg_temp);
   float64 act_solar_rad = pot_solar_rad * act_atmospheric_transmissivity;
/*
debug_sol_rad
<< act_atmospheric_transmissivity << '\t'
<< pot_solar_rad << '\t'
<< act_solar_rad << '\t'
<< std::endl;

debug_sol_rad_printed = true;
*/
  return  CORN::must_be_greater_or_equal_to<float64>(act_solar_rad,0.8);         //010609
}
//_donatelli_campbell_computed_solar_radiation______________________1998-03-31_/
#endif
#ifndef WEATHER_PROVIDER_VERSION
//101208 moving to WP_solar_radiation
#error check obsolete
bool Weather_with_solar_rad_estimation::summer_B_period(CORN::DOY doy)     const
{  return (geocoordinate.is_in_southern_hemisphere() )
      ? ((doy < 81) || (doy > 265)) // Southern hemisphere
      : ((doy > 81) && (doy < 265)); // Northern hemisphere
}
#endif
//_summer_B_period_____________________________________________________________/
#ifndef WEATHER_PROVIDER_VERSION
// Summer B estimaters moved from ClimPar                                        //970712
#error check obsolete
float64 Weather_with_solar_rad_estimation::estimate_summer_B_solar_rad()   const
{
   float64 zero_safe_lat_rad = geocoordinate.get_latitude_radians();             //151118
   if (zero_safe_lat_rad == 0.0)  zero_safe_lat_rad = 0.0001;
   return 0.282*pow(fabs(zero_safe_lat_rad),-0.431);

}
//_estimate_summer_B_solar_rad______________________________________1998-12-07_/
float64 Weather_with_solar_rad_estimation::estimate_winter_B_solar_rad()   const
{
   float64 zero_safe_lat_rad = geocoordinate.get_latitude_radians();             //151118
   if (zero_safe_lat_rad == 0.0)  zero_safe_lat_rad = 0.0001;
   return  0.170 * pow(fabs(zero_safe_lat_rad),-0.979);
}
//_estimate_winter_B_solar_rad______________________________________1998-12-07_/
//101208 moving to WP_solar_radiation
float64 Weather_with_solar_rad_estimation::estimate_solar_radiation
(CORN::Quality &quality,bool use_temperature_differential)                 const //980331
 // climgen always uses
{
   static float64 min_temp_adjacent_day = 0;                                     //000712
   float64 max_temp_date = get_max_temperature(today);                           //990902
   float64 min_temp_date = get_min_temperature(today);                           //990902
   float64 temperature_range = max_temp_date - min_temp_date;                    //970712
   float64 pot_solar_rad_today=
       ET_solar_irradiance                                                       //151128
         .CS::Solar_irradiance_extraterrestrial_daily::get_MJ_per_m2_day();      //150628
   float64 fitted_B = generation_parameters
      ? generation_parameters->fitted_B : MISSING_fitted_B_solar_rad;            //091214
   float64 sol_rad = 0.0;                                                        //070313
   if (generation_parameters && !CORN::is_approximately<float64>
       (fitted_B,MISSING_fitted_B_solar_rad,0.00001))                            //970712
      // if we have an fitted B parameter
   {  float64 temperature_differential
         = max_temp_date - (min_temp_date + min_temp_adjacent_day)/2.0;          //980331
      sol_rad =                                                                  //070313
         donatelli_campbell_computed_solar_radiation                             //970712
         (use_temperature_differential?temperature_differential:temperature_range
         ,pot_solar_rad_today);                                                  //980331
   } else                                                                        //070313
   {
      Solar_radiation_with_Campbell_estimation_parameter srad_estimator          //200322
         (xxx);
      srad_estimator.update();                                                   //200322
      sol_rad = srad_estimator.MJ_m2_day();                                      //200322
      /*200322 implemented by Solar_radiation_with_Campbell_estimation_parameter
      float64 summer_B_solar_rad = estimate_summer_B_solar_rad();                //070313
      float64 winter_B_solar_rad =  estimate_winter_B_solar_rad();               //980312
      // This B summer and winter are not really the summer, but rather what
      // half of the year summer falls in.
      float64 B_solar_rad = summer_B_period(today.get_DOY())
         ? summer_B_solar_rad
         : winter_B_solar_rad;
      CORN::Date date_minus_30(today); date_minus_30 -= 30;                      //970617
      float64 pot_solar_rad_last_month =
         ET_solar_irradiance_new.CS::Solar_irradiance_extraterrestrial_daily
            ::get_MJ_per_m2_day(date_minus_30.get_DOY());                        //150628
      sol_rad =                                                                  //070313
         original_computed_solar_radiation
         (temperature_range,pot_solar_rad_today,pot_solar_rad_last_month,B_solar_rad); //970712
      */
   }
   quality.assume_code(quality_estimated_from(quality.get_quality_code() == estimated_quality? generated_quality: quality.get_quality_code()));  //070313
      // This is even further estimated because B is estimated
   min_temp_adjacent_day = min_temp_date;
   return sol_rad;
}
#endif
//_estimate_solar_radiation_________________________________________1997-10-13_/
#ifndef WEATHER_PROVIDER_VERSION
float64 Weather_with_solar_rad_estimation::load_solar_radiation
(/* modified_*/ Solar_radiation &solar_rad, const Date_time  &_date_time)
mutates_ // yes only pass by value!
{  float64 result =  Weather_daily_deprecated::load_solar_radiation(solar_rad, _date_time);
   // The WP_solar radiation should now handle this, but not tested              //110905
   const CORN::Quality   &worst_temperature_quality                              //081024
      = get_max_temperature_quality().worst(get_min_temperature_quality());
   if (!solar_rad.is_valid()
      && worst_temperature_quality.is_valid())                                   //050609
   {  
      result = estimate_solar_radiation(solar_rad,false);
      solar_rad.set_MJ_m2(result,quality_estimated_from_quality(worst_temperature_quality)); //140723
      Weather_daily_deprecated::set_solar_radiation(result,solar_rad);
   }
   return result;
}
#endif
//_load_solar_radiation_____________________________________________2000-02-16_/
#ifdef WEATHER_PROVIDER_VERSION
//110913  WARNING this is used in CropSyst, we need to do something different
// CropSyst should store the rain it needs
#else
int Weather_with_solar_rad_estimation::rain_days_in_last_30(CORN::Date _date) // yes only pass by value
{
   /*150120 the weather database now automatically returns values for date dates prior to the data set start date
   _date.set(_date.get() ? _date.get() : get_today_date32() );                   //990504
   */
   CORN::Date days_ago(today);
   days_ago -= 30;
   int days_with_rain = 0;
   while (days_ago < _date)
   {  if (get_precipitation_m(days_ago) > 0.0)
		   days_with_rain += 1;
      days_ago += 1;
   };
   return days_with_rain;
}
//_rain_days_in_last_30________________________________________________________/
#endif
void Weather_with_solar_rad_estimation::set_precipitation_intervals_mm
(const CORN::Date_const &_date
,const CORN::Time &i_time
,const Dynamic_float_array &precip_in_intervals
,CORN::Minutes interval_time_minutes
,CORN::Quality  &attribute)
{  weather_data->set_precipitation_intervals_mm
      (_date,i_time,precip_in_intervals,interval_time_minutes,attribute);
}
//_set_precipitation_intervals_mm___________________________________1999-04-25_/
void Weather_with_solar_rad_estimation::set_temperature_intervals
(const CORN::Date_const &_date
,const Dynamic_float_array &temperature_in_intervals
,CORN::Minutes interval_time_minutes
,CORN::Quality  &attribute)
{  weather_data->set_temperature_intervals
      (_date,temperature_in_intervals,interval_time_minutes,attribute);
}
//_set_temperature_intervals________________________________________2002-11-24_/
#ifdef WEATHER_GENERATION
void Weather_with_solar_rad_estimation::know_generation_parameters
   (const ClimGen_Parameters  &_ClimGen_parameters)                contribution_
{  Weather_base::know_generation_parameters(_ClimGen_parameters);
   generation_parameters = &(_ClimGen_parameters.solar_radiation_parameters);
}
#endif
//_know_generation_parameters_______________________________________2009-12-14_/

