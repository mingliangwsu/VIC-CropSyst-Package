#include "CS_weather_daily_UED.h"
#include "UED/library/timeqry.h"
#include "UED/library/std_codes.h"
#include "UED/library/locrec.h"
namespace CS {
//______________________________________________________________________________
Weather_database_daily_UED::Weather_database_daily_UED
(const CORN::OS::File_name &UED_file_name
,const CORN::date32           &simdate_raw_                                        //170525
,bool                       extend_available_weather_
,CORN::Year                 substitute_year_base_                                //170323
,CORN::Year                 simulation_start_year_                               //130331
,int8                       substitute_year_offset_                              //161207
,CORN::date32               preclude_available_weather_date_)                    //170310
: Simulation_element_abstract(simdate_raw_)                                        //180402
, Weather_database_daily()
, database_file(new CS_UED::Database_file_indexed(UED_file_name.c_str(),std::ios::in))
, provider(0)
, today_override_raw(0)
, Tmax_yesterday(0)
{
   database_file->initialize();                                                  //170323
   if (substitute_year_base_ || extend_available_weather_)                       //170323
      database_file->enable_substitution
      (substitute_year_base_                                                     //170323
      ,simulation_start_year_ //reference_year                                   //170331
      ,substitute_year_offset_);                                                 //161207
   if (preclude_available_weather_date_ && substitute_year_base_)                //170407brc
      database_file->enable_preclusion(preclude_available_weather_date_);
}
//_constructor______________________________________________________2016-05-25_/
Weather_database_daily_UED::Weather_database_daily_UED
(CS_UED::Database_file_indexed   *database_file_
,const CORN::date32           &simdate_raw_                                        //170525
,bool                          extend_available_weather_
,CORN::Year                    substitute_base_year_                             //170323
,CORN::Year                    simulation_start_year_                            //170323
,int8                          substitute_year_offset_                           //161207
,CORN::date32                  preclude_available_weather_date_)                 //170310
: Simulation_element_abstract(simdate_raw_)                                        //180402
, Weather_database_daily()
, database_file(database_file_)
, provider(0)
, today_override_raw(0)
, Tmax_yesterday(0)
{
   if (substitute_base_year_ && extend_available_weather_)                       //170323
      database_file->enable_substitution
      (substitute_base_year_                                                     //170323
      ,simulation_start_year_ // substitute reference year                       //170331
      ,substitute_year_offset_);                                                 //161207
   if (preclude_available_weather_date_)                                         //170323
      database_file->enable_preclusion(preclude_available_weather_date_);
}
//_constructor______________________________________________________2016-07-27_/
bool Weather_database_daily_UED::start_day()                       modification_
{
   UED::Time_query_single_date time_qry
      (today_override_raw ? today_override_raw : simdate_raw                       //170215
      ,(UED_units_code)UT_day);                                                  //030714
   bool override_set_safety = false;                                             //991013
   CORN::Units_code actual_units;

   //110910 The elements used to need to be loaded (by Weather_base) in this order
   // but I don't know if that is still the case using the new weather provider
   actual_units = UC_meter;         provider->precipitation_actual.set_m                (database_file->get_no_create(UED::Record_type_distinguisher_interface::UED_data_set_year_var_units ,UED::STD_VC_precipitation.get()             ,time_qry,actual_units,provider->precipitation_actual  ),provider->precipitation_actual);
   actual_units = UC_Celcius;       provider->air_temperature_max.update_value_qualified(database_file->get_no_create(UED::Record_type_distinguisher_interface::UED_data_set_year_var_units ,UED::STD_VC_max_temperature.get()           ,time_qry,actual_units,provider->air_temperature_max   ),provider->air_temperature_max    );
   actual_units = UC_Celcius;       provider->air_temperature_min.update_value_qualified(database_file->get_no_create(UED::Record_type_distinguisher_interface::UED_data_set_year_var_units ,UED::STD_VC_min_temperature.get()           ,time_qry,actual_units,provider->air_temperature_min   ),provider->air_temperature_min    );
   actual_units = UC_Celcius;       provider->air_temperature_avg.update_value_qualified(database_file->get_no_create(UED::Record_type_distinguisher_interface::UED_data_set_year_var_units ,UED::STD_VC_avg_temperature.get()           ,time_qry,actual_units,provider->air_temperature_avg   ),provider->air_temperature_avg    );
/*
   continue here apply adjustments
   get_global_adjustments()->apply()
   weather provider needs to append respective adjustment association/references
*/

   actual_units = UC_MJ_per_m2;     provider->solar_radiation.set_MJ_m2                 (database_file->get_no_create(UED::Record_type_distinguisher_interface::UED_data_set_year_var_units ,UED::STD_VC_solar_radiation_global.get()    ,time_qry,actual_units,provider->solar_radiation       ),provider->solar_radiation); //151115
   #if (CS_VERSION < 6)
   if (!provider->solar_radiation.is_valid()) // This may be a V4 UED file
      provider->solar_radiation.set_MJ_m2                                               (database_file->get_no_create(UED::Record_type_distinguisher_interface::UED_data_set_year_var_units ,UED::STD_VC_solar_radiation_ERRONEOUS.get() ,time_qry,actual_units,provider->solar_radiation       ),provider->solar_radiation);
   #endif
   actual_units = UC_Celcius;       provider->dew_point_temperature_max.update_value_qualified(database_file->get_no_create(UED::Record_type_distinguisher_interface::UED_data_set_year_var_units ,UED::STD_VC_max_dew_point_temperature.get()  ,time_qry,actual_units,provider->dew_point_temperature_max  ) ,provider->dew_point_temperature_max);
   actual_units = UC_Celcius;       provider->dew_point_temperature_min.update_value_qualified(database_file->get_no_create(UED::Record_type_distinguisher_interface::UED_data_set_year_var_units ,UED::STD_VC_min_dew_point_temperature.get()  ,time_qry,actual_units,provider->dew_point_temperature_min  ) ,provider->dew_point_temperature_min);
   actual_units = UC_percent;       provider->relative_humidity_max.set_value_quality   (database_file->get_no_create(UED::Record_type_distinguisher_interface::UED_data_set_year_var_units ,UED::STD_VC_max_relative_humidity.get()            ,time_qry,actual_units,provider->relative_humidity_max      ),provider->relative_humidity_max );
   actual_units = UC_percent;       provider->relative_humidity_min.set_value_quality   (database_file->get_no_create(UED::Record_type_distinguisher_interface::UED_data_set_year_var_units ,UED::STD_VC_min_relative_humidity.get()            ,time_qry,actual_units,provider->relative_humidity_min      ),provider->relative_humidity_min  );

   actual_units = UC_kg_kg;         provider->humidity_specific.set_kg_kg/*set_value_quality*/       (database_file->get_no_create(UED::Record_type_distinguisher_interface::UED_data_set_year_var_units ,UED::STD_VC_humidity_specific.get()                ,time_qry,actual_units,provider->humidity_specific          )); // ,provider->humidity_specific );

   actual_units = UC_meters_per_second; provider->wind_speed.set_m_s                    (database_file->get_no_create(UED::Record_type_distinguisher_interface::UED_data_set_year_var_units ,UED::STD_VC_wind_speed.get()                       ,time_qry,actual_units,provider->wind_speed                 ),provider->wind_speed);
/*150603 ET model should be provided by the class that needs it
   actual_units = UC_m;             provider->ET_reference.set_m               (database_file->get_no_create(UED::Record_type_distinguisher_interface::UED_data_set_year_var_units ,UED::STD_VC_pot_evapotranspiration.get()    ,time_qry,actual_units,provider->ET_reference          )    ,provider->ET_reference);
*/
   actual_units = UC_Celcius;       provider->dew_point_temperature_avg          .update_value_qualified           (database_file->get_no_create(UED::Record_type_distinguisher_interface::UED_data_set_year_var_units ,UED::STD_VC_avg_dew_point_temperature.get()         ,time_qry,actual_units,provider->dew_point_temperature_avg          ),provider->dew_point_temperature_avg);
   actual_units = UC_kPa;           provider->vapor_pressure_deficit_max         .update_value_qualified/*set_kPa*/(database_file->get_no_create(UED::Record_type_distinguisher_interface::UED_data_set_year_var_units ,UED::STD_VC_max_vapor_pressure_deficit.get()        ,time_qry,actual_units,provider->vapor_pressure_deficit_max         ),provider->vapor_pressure_deficit_max);
   actual_units = UC_kPa;           provider->vapor_pressure_deficit_fullday_avg .update_value_qualified/*set_kPa*/(database_file->get_no_create(UED::Record_type_distinguisher_interface::UED_data_set_year_var_units ,UED::STD_VC_avg_fullday_vapor_pressure_deficit.get(),time_qry,actual_units,provider->vapor_pressure_deficit_fullday_avg ),provider->vapor_pressure_deficit_fullday_avg);
   actual_units = UC_kPa;           provider->vapor_pressure_deficit_daytime     .update_value_qualified/*set_kPa*/(database_file->get_no_create(UED::Record_type_distinguisher_interface::UED_data_set_year_var_units ,UED::STD_VC_avg_daytime_vapor_pressure_deficit.get(),time_qry,actual_units,provider->vapor_pressure_deficit_daytime     ),provider->vapor_pressure_deficit_daytime);
/*
   actual_units = UC_XXXX;                   provider->xxxxxx.set_UNITS                 (database_file->get_no_create(UED::Record_type_distinguisher_interface::UED_data_set_year_var_units ,UED::STD_VC_xxxx.get()             ,time_qry,actual_units,provider->xxx.quality),provider->xxx.quality);
*/

   CORN::Date_clad_32 provided_date_tomorrow(today_override_raw ? today_override_raw : simdate_raw);
   provided_date_tomorrow.inc_day();
   // Tmax_yesterday is just just copy today's value in end_day()
   UED::Time_query_single_date tomorrow_time_qry(provided_date_tomorrow.get_datetime64(),(UED_units_code)UT_day);
   bool got =
       database_file->get_parameter_on
      (Tmin_tomorrow
      ,UED::STD_VC_min_temperature.get()
      ,tomorrow_time_qry
      ,UED::Record_type_distinguisher_interface::UED_data_set_year_var_units);
   if (!got)                                                                     //161222
   {  // Note that got may be false on the last day of available data.
      // In this case just use today's min value.
      Tmin_tomorrow.set_Celcius(provider->air_temperature_min.Celcius(),CORN::forecasted_quality);
      got = true;
   }
   return got && Simulation_element_abstract::start_day();
}
//_start_day________________________________________________________2016-05-25_/
bool Weather_database_daily_UED::end_day()                         modification_
{
   Tmax_yesterday =  provider->air_temperature_max.Celcius();
   return Simulation_element_abstract::end_day();
}
//_end_day__________________________________________________________2016-05-25_/
bool Weather_database_daily_UED::get_parameter_on
(modifiable_ CS::Parameter &parameter
,nat32                     variable_code // Typically the variable code will be UED variable code
,const CORN::Date_const    &on_date)                              rectification_
{  UED::Time_query_single_date time_qry
      (on_date.get_datetime64(),(UED_units_code)UT_day);
   return database_file->get_parameter_on
      (parameter
      ,variable_code
      ,time_qry
      ,UED::Record_type_distinguisher_interface::UED_data_set_year_var_units);
}
//_get_parameter_on_____________________________________2016-05-25__2015-11-19_/
bool Weather_database_daily_UED::get_geocoordinate
(modifiable_ Geocoordinate &geocoordinate)                                 const
{  const UED::Geolocation_record  *geoloc
      = database_file->ref_geolocation_record();
   if (geoloc) geocoordinate.copy_from_geocoordinate(*geoloc);
   return geoloc;
}
//_get_geocoordinate________________________________________________2016-05-25_/
bool Weather_database_daily_UED::override_today(CORN::date32 new_date_raw)
{  today_override_raw = const_cast<const CORN::date32 &>(new_date_raw);
   return true;
}
//_override_today______________________________________________________________/
}//_namespace CS_______________________________________________________________/

