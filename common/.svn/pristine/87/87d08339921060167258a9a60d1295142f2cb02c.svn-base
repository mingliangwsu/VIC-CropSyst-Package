#ifndef CS_weather_daily_UEDH
#define CS_weather_daily_UEDH
#ifndef CLIMGEN_VERSION
#include "CS_suite/simulation/CS_simulation_element.h"
#endif
#include "CS_suite/UED/database_file_substitution.h"
#include "common/weather/weather_provider.h"
#include "common/weather/weather_interface.h"
namespace CS
{
//______________________________________________________________________________
interface_ Weather_database_daily
: public implements_ Weather_spanning_interface
{
   virtual bool get_geocoordinate
      (modifiable_ Geocoordinate &geolocation)                          const=0;
   virtual bool start_day()                                     modification_=0;
   virtual bool end_day()                                       modification_=0;
   virtual bool know_weather_provider
      (modifiable_ Weather_provider *provider_)                    cognition_=0;
   virtual const Weather_provider &ref_weather_provider()               const=0;
   virtual bool override_today(CORN::date32 new_date)                        =0;
};
//_Weather_database_daily___________________________________________2016-07-29_/
class Weather_database_daily_UED
: public extends_interface_   Weather_database_daily
#ifndef CLIMGEN_VERSION
, public extends_             Simulation_element_abstract                        //180402
#endif
{
 public: // temporarily
   mutable CS_UED::Database_file_indexed *database_file;
 public: // external references
   cognate_ Weather_provider *provider;
 private:
   CORN::date32 today_override_raw;
   float64 Tmax_yesterday;
   contribute_ Air_temperature_minimum Tmin_tomorrow;
 public:
   Weather_database_daily_UED
      (const CORN::OS::File_name    &UED_file_name
      ,const CORN::date32           &simdate_raw_                                //170525
      ,bool                          extend_available_weather_
      ,CORN::Year                    substitute_base_year_                       //170323
      ,CORN::Year                    simulation_start_year_                      //130331
      ,int8                          substitute_year_offset_                     //161207
      ,CORN::date32                  preclude_available_weather_date_);          //170310
   Weather_database_daily_UED
      (CS_UED::Database_file_indexed  *database_file
      ,const CORN::date32           &simdate_raw_                                //170525
      ,bool                          extend_available_weather_
      ,CORN::Year                    substitute_base_year_                       //170323
      ,CORN::Year                    simulation_start_year_                      //130331
      ,int8                          substitute_year_offset_                     //161207
      ,CORN::date32                  preclude_available_weather_date_);          //170310
   inline virtual bool know_weather_provider
      (modifiable_ Weather_provider *provider_)                       cognition_
      { provider = provider_; return provider != 0; }
   inline virtual const Weather_provider &ref_weather_provider()          const
      { return *provider; }
   virtual bool get_geocoordinate(modifiable_ Geocoordinate &geolocation) const;
 public: // Simulation_element implementations
   virtual bool start_day()                                       modification_;
   virtual bool end_day()                                         modification_;
 public: // Weather_spanning_interface implementations
   virtual bool get_parameter_on
      (modifiable_ CS::Parameter &parameter
      ,nat32  variable_code // Typically the variable code will be UED variable code
      ,const CORN::Date_const &on_date)                          rectification_;
   inline virtual float64 get_air_temperature_min_tomorrow()              const
      { return Tmin_tomorrow.Celcius(); }
   inline virtual float64 get_air_temperature_max_yesterday()             const
      { return Tmax_yesterday; }
   virtual bool override_today(CORN::date32 new_date);
 private:
   void check_abstract()
      {  CORN::date32 x=0;
         Weather_database_daily_UED(0,x,false,0,0,0,0); }
};
//_Weather_database_daily_UED_______________________________________2016-05-25_
}//_namespace CS_______________________________________________________________/
#endif

