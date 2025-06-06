#include "CS_land_unit_meteorological.h"
#include "corn/OS/file_system_engine.h"
#include "common/weather/atmospheric_CO2_change_recalibration.h"
//#if (CS_VERSION <= 4)
#include "common/weather/atmospheric_CO2_change_linear.h"
//#endif
#include "common/weather/sky.h"
#include "common/weather/subdaily.h"
#include "common/weather/hour/weather_hours.h"
#include "common/weather/snow_pack.h"
#include "common/weather/storms.h"
#include "common/weather/day/sun_days.h"
#include "CS_suite/observation/CS_observation.h"
#include "UED/library/std_codes.h"
#include "csvc.h"

//______________________________________________________________________________
namespace CS
{
//______________________________________________________________________________
Land_unit_meteorological::Land_unit_meteorological
(const CORN::date32  &simdate_raw_                                               //170524
,const Geocoordinate &geocoordinate_
,Snow_pack_model      snow_pack_model_)                                          //151202

: CS::Simulation_element_abstract                                 (simdate_raw_)
, geocoordinate                                                 (geocoordinate_)
, solar_radiation_extraterrestrial                                           (0) //151130
,  solar_radiation_extraterrestrial_owned                                 (true) //151215
, sun                                                     (0) , sun_owned(false) //151027
, sky                                                     (0) , sky_owned(false) //151027
, sun_days                                            (0) , sun_days_owned(true) //160117
, sun_hours                                          (0) , sun_hours_owned(true) //160811_160117
, season                   (simdate,!geocoordinate_.is_in_northern_hemisphere()) //1700524_151029
, weather_provider                            (0) , weather_provider_owned(true) //151215
#if (CS_VERSION==5)
, ET_ref                                                                     (0) //260731
#endif
, weather_spanning                                                           (0) //151212
, subdaily                                             (0), subdaily_owned(true) //151212
, hourly                                               (0), hourly_owned  (true) //151212
, storms                                               (0), storms_owned  (true) //120916
, snow_pack                                                                  (0)
, snow_pack_model                                             (snow_pack_model_) //151202_151023
, atmospheric_CO2_change                                                     (0) //151208
{}
//_constructor_________________________________________________________________/
Land_unit_meteorological::~Land_unit_meteorological()
{
   if (sun_hours_owned)          delete sun_hours;                               //160811
   if (sun_days_owned)           delete sun_days;                                //160811
   if (sky_owned)                delete sky;
   if (sun_owned)                delete sun;
   delete snow_pack;
#ifdef DISABLED_CRASHES
// Not sure why this is crashing in the pest model.
   if (hourly_owned)             delete hourly;                                  //151212
#endif
   if (subdaily_owned)           delete subdaily;                                //151212
   if (weather_provider_owned)   delete weather_provider;                        //151215
   if (solar_radiation_extraterrestrial_owned)                                   //151215
      delete solar_radiation_extraterrestrial;
   if (ET_ref)                   delete ET_ref;                                  //201129LML
   // this is crashing delete storms;
   // dont delete atmospheric_CO2_change (no longer owned)                       //171207
}
//_~Land_unit_meteorological________________________________________2015-10-23_/
/*160811 Not implemented yet because need to be careful the order of instanciation
bool Land_unit_meteorological::provide_everything()                   provision_
{
   bool provided = true;

   provide_sun();
   provide_sky();
   provide_sun_hours();

   provided &= sun && sky && sun_days && sun_hours;
   provided &= provide_hourly() != 0; // provides subdaily
   #if (CS_VERSION==5)
   assert(weather_provider); // weather provided either needs to have been submitted or known
   provide_ET_reference(); // ET reference provides everything
   provided &= ET_ref != 0;
   #endif
   return provided;
}
*/
//_provide_everything_______________________________________________2016-08-11_/
bool Land_unit_meteorological::know_sun(Sun *sun_)                    cognition_
{
   #if (CS_VERSION==5)
   assert(!ET_ref);
      // The ET has already been instanciated with an existing sun
      // creating a new sun would cause error with ET
   #endif
   if (sun_owned && sun && sun_) delete sun;
   sun = sun_; sun_owned = false;
   if (!sun_) provide_sun(); // in the rare case the caller reset the sun by passing 0.
   return true;
}
//_know_sun_________________________________________________________2016-08-01_/
bool Land_unit_meteorological::know_sky(Sky *sky_)                    cognition_
{
   if (sky_owned/*190725  && sky && sky_*/) delete sky;
   sky = sky_; sky_owned = false;
   if (!sky_) provide_sky(); // in the rare case the caller reset the sky by passing 0.
   return true;
}
//_know_sky_________________________________________________________2016-08-01_/
const Sun &Land_unit_meteorological::provide_sun()                    provision_
{  if (!sun)
   {  sun = new Sun(simdate,geocoordinate);
      sun_owned = true;
   }
   return *sun;
}
//_provide_sun______________________________________________________2016-08-01_/
const Sky &Land_unit_meteorological::provide_sky()                    provision_
{  if (!sky)
   {  sky = new Sky
         (provide_solar_radiation_extraterrestrial()
         ,ref_air_temperature_avg()
         ,ref_solar_radiation()
         ,ref_precipitation_actual());
      sky_owned = true;
   }
   return *sky;
}
//_provide_sky______________________________________________________2016-08-01_/
const Sun_days &Land_unit_meteorological::provide_sun_days()          provision_
{  if (!sun_days)
   {  sun_days = new Sun_days
         (geocoordinate.get_longitude_dec_deg()
         ,geocoordinate.get_longitude_dec_deg());
      sun_days_owned = true;
   }
   return *sun_days;
}
//_provide_sun_days_________________________________________________2016-08-11_/
const Sun_hours &Land_unit_meteorological::provide_sun_hours()        provision_
{  if (!sun_hours)
   {  sun_hours = new  Sun_hours
         (provide_sun_days()
         ,geocoordinate.get_latitude_dec_deg());
      sun_hours_owned = true;
   }
   return *sun_hours;
}
//_provide_sun_hours________________________________________________2016-08-11_/
#if (CS_VERSION==5)
const ET_Reference &Land_unit_meteorological::provide_ET_reference()  provision_
{  if (!ET_ref)                                                                  //171229
      ET_ref = new ET_Reference_calculator                                       //160731
      (geocoordinate.get_elevation()
      ,provide_sun().ref_day_length_hours_today()
      ,*weather_provider);
   return *ET_ref;
}
#endif
//_provide_ET_reference_____________________________________________2016-08-01_/
bool Land_unit_meteorological::initialize()                      initialization_
{  provide_sun();                                                                //160801
   provide_sky();                                                                //160801
   #if (CS_VERSION==5)
   provide_ET_reference();                                                       //160801
   #endif
   return true; // sun && sky;                                                   //160801
}
//_initialize_______________________________________________________2015-10-27_/
bool Land_unit_meteorological::know_weather_spanning
(Weather_spanning_interface &weather_known)                           cognition_
{  weather_spanning = &weather_known;
   return true;
}
//_know_weather_spanning____________________________________________2015-12-15_/
bool Land_unit_meteorological::know_Atmospheric_CO2                                                    //04012021LML
   (Atmospheric_CO2_change_element *atmospheric_CO2_change_)          cognition_
{
    atmospheric_CO2_change = atmospheric_CO2_change_;
    return true;
}
//______________________________________________________________________________
bool Land_unit_meteorological::take_storms(meteorology::Storms *storms_given)
{  if (!storms_owned) return false;
   /*190725 if (storms)*/ delete storms;
   storms = storms_given;
   return storms != 0;
}
//_take_storms______________________________________________________2015-12-15_/
bool Land_unit_meteorological::submit_weather_provider
(Weather_provider *weather_provider_remitted)                         remission_
{  if (weather_provider_owned)
      delete weather_provider;
   weather_provider = weather_provider_remitted; weather_provider_owned = true;
   return weather_provider_remitted != 0;
}
//_submit_weather_provider__________________________________________2015-12-15_/
bool Land_unit_meteorological::know_weather_provider
(Weather_provider *weather_provider_known)                            cognition_
{  if (weather_provider_owned)
      delete weather_provider;
   weather_provider = weather_provider_known; weather_provider_owned = false;
   return weather_provider != 0;
}
//_know_weather_provider____________________________________________2015-12-22_/
bool Land_unit_meteorological::submit_solar_radiation_extraterrestrial
(Solar_radiation_extraterrestrial_hourly *solar_radiation_extraterrestrial_submitted)
{  solar_radiation_extraterrestrial = solar_radiation_extraterrestrial_submitted;
   solar_radiation_extraterrestrial_owned = true;
   return solar_radiation_extraterrestrial != 0;
}
//_submit_solar_radiation_extraterrestrial_________________________2015-12-15_/
bool Land_unit_meteorological::know_solar_radiation_extraterrestrial
(const Solar_radiation_extraterrestrial_hourly &solar_radiation_extraterrestrial_known) cognition_
{  solar_radiation_extraterrestrial = const_cast<Solar_radiation_extraterrestrial_hourly *>
      (&solar_radiation_extraterrestrial_known);
   solar_radiation_extraterrestrial_owned = false;
   return solar_radiation_extraterrestrial != 0;
}
//_know_solar_radiation_extraterrestrial___________________________2015-12-15_/
const Solar_radiation_extraterrestrial_hourly &
Land_unit_meteorological::provide_solar_radiation_extraterrestrial() provision_
{  if (!solar_radiation_extraterrestrial)
   {  solar_radiation_extraterrestrial = new Solar_radiation_extraterrestrial_hourly
       (geocoordinate,provide_sun_days(),provide_sun_hours(),simdate);
      solar_radiation_extraterrestrial_owned = true;
   } return *solar_radiation_extraterrestrial;
}
//_provide_solar_radiation_extraterrestrial________________________2016-08-11_/
Atmospheric_CO2_change_element *                                                 //171207
Land_unit_meteorological::instanciate_CO2                                        //171207
(const CORN::OS::File_name &recalibration_filename
,float64 initial_CO2_conc
,float64 annual_CO2_change
, const CORN::Date_const &start_date
, const CORN::Date_const &stop_date)                             initialization_ //150315
{
   {  // By default, we setup the linear parameters                              //090707
      // In version 5 CO2 data may be stored in UED file                         //090707
      if (recalibration_filename.get_name().length() &&
         CORN::OS::file_system_engine.exists(recalibration_filename))            //130121
      { // There may be CO2 data in the recalibration file.                      //090707
         bool CO2_recalibration_encountered = false;                             //090707
         Atmospheric_CO2_change_recalibration *CO2_change_with_recalibration     //090707
            = new Atmospheric_CO2_change_recalibration
               (recalibration_filename.c_str()                                   //121126
               ,start_date
               ,stop_date
               ,simdate_raw                                                      //171207_150315
               ,(float32)initial_CO2_conc// 365.0
               ,CO2_recalibration_encountered);
         if (CO2_recalibration_encountered)                                      //090707
            atmospheric_CO2_change = CO2_change_with_recalibration;              //151201
         else
            { delete CO2_change_with_recalibration;                              //151208
               CO2_change_with_recalibration = 0;}
      }
      // The linear model is deprecated, now use recalibration file
      else
      {  Atmospheric_CO2_change_linear *CO2_change_linear =                      //181026
            new Atmospheric_CO2_change_linear                                    //090316
               ((float32)initial_CO2_conc                                        //090316
               ,(float32)annual_CO2_change
               ,simdate_raw);                                                    //171207
         atmospheric_CO2_change = CO2_change_linear;                             //181026
      }
   }
   return atmospheric_CO2_change;                                                //171207
}
//_instanciate_CO2__________________________________________________2012-01-25_/
const Sun &Land_unit_meteorological::ref_sun()                             const
{  return *sun;
}
//_ref_sun__________________________________________________________2015-09-21_/
const Sun_days &Land_unit_meteorological::ref_sun_days()                   const
{  return *sun_days;
}
//_ref_sun_days_____________________________________________________2015-11-30_/
float64 Land_unit_meteorological::get_snow_pack_thickness()                const
{  return snow_pack                                                              //151124
      ? snow_pack->ref_snow_pack_thickness()                                     //151023
      : 0.0;
}
//_get_snow_pack_thickness__________________________________________2014-12-08_/
float64 Land_unit_meteorological::get_snow_water_equivelent_depth()        const
{  return snow_pack
      ? snow_pack->get_water_equivelent_depth()
      : 0.0;
}
//_get_snow_water_equivelent_depth__________________________________2015-10-26_/
const Precipitation                  &Land_unit_meteorological::ref_precipitation_actual()           const { return ref_weather_provider().precipitation_actual; }
const Precipitation                  &Land_unit_meteorological::ref_precipitation_liberated()        const { return liberated_water; } //170321
const Vapor_pressure_deficit_max     &Land_unit_meteorological::ref_vapor_pressure_deficit_max()     const { return ref_weather_provider().vapor_pressure_deficit_max; }
const Vapor_pressure_daytime         &Land_unit_meteorological::ref_vapor_pressure_daytime()         const { return ref_weather_provider().vapor_pressure_daytime; }
const Vapor_pressure_nighttime       &Land_unit_meteorological::ref_vapor_pressure_nighttime()       const { return ref_weather_provider().vapor_pressure_nighttime; }
const Air_temperature_maximum        &Land_unit_meteorological::ref_air_temperature_max()            const { return ref_weather_provider().air_temperature_max; }
const Air_temperature_minimum        &Land_unit_meteorological::ref_air_temperature_min()            const { return ref_weather_provider().air_temperature_min; }
const Air_temperature_average        &Land_unit_meteorological::ref_air_temperature_avg()            const { return ref_weather_provider().air_temperature_avg; }
const Solar_radiation                &Land_unit_meteorological::ref_solar_radiation()                const { return ref_weather_provider().solar_radiation; }
const Dew_point_temperature          &Land_unit_meteorological::ref_dew_point_temperature_max()      const { return ref_weather_provider().dew_point_temperature_max; }
const Dew_point_temperature          &Land_unit_meteorological::ref_dew_point_temperature_min()      const { return ref_weather_provider().dew_point_temperature_min; }
const Dew_point_temperature          &Land_unit_meteorological::ref_dew_point_temperature_avg()      const { return ref_weather_provider().dew_point_temperature_avg; }
const Relative_humidity              &Land_unit_meteorological::ref_relative_humidity_max()          const { return ref_weather_provider().relative_humidity_max; }
const Relative_humidity              &Land_unit_meteorological::ref_relative_humidity_min()          const { return ref_weather_provider().relative_humidity_min; }
const Vapor_pressure_deficit_fullday &Land_unit_meteorological::ref_vapor_pressure_deficit_fullday_avg() const { return ref_weather_provider().vapor_pressure_deficit_fullday_avg; }
const Vapor_pressure_deficit_daytime &Land_unit_meteorological::ref_vapor_pressure_deficit_daytime() const { return ref_weather_provider().vapor_pressure_deficit_daytime; }
const Wind_speed                     &Land_unit_meteorological::ref_wind_speed()                     const { return ref_weather_provider().wind_speed; }
const Aerodynamics                   &Land_unit_meteorological::ref_aerodynamics ()                  const { return ref_weather_provider().aerodynamics; }
const ET_Reference                   &Land_unit_meteorological::ref_ET_reference()                   const
#if (CS_VERSION==5)
{ return *ET_ref; }                                                              //160731
#else
{ return ref_weather_provider().ET_reference; }                                  //180806
#endif
const Sky                            &Land_unit_meteorological::ref_sky()                            const { return *sky; } //151027
      bool                            Land_unit_meteorological::is_winter_time()              affirmation_ { return season.is_winter_time(); }
//__________________________________________________________________2015-10-23_/
const Weather_subdaily *Land_unit_meteorological::provide_subdaily()  provision_
{
   if (!subdaily)
   {  subdaily_owned = true;
      subdaily = new Weather_subdaily
         (ref_weather_spanning()
         ,simdate
         ,ref_air_temperature_max()
         ,ref_air_temperature_min()
         ,ref_relative_humidity_max()
         ,ref_relative_humidity_min());
      subdaily->update();
         // because subdaily could be provided at any time even after
         // today's weather would have been updated
   }
   return subdaily;
};
//_provide_subdaily_________________________________________________2015-12-12_/
const CS::Weather_hours *Land_unit_meteorological::provide_hourly()   provision_
{  if (!hourly)
   {  hourly_owned = true;
      provide_subdaily();
      if (!subdaily)  return 0; // can't have hourly without subdaily
      hourly = new CS::Weather_hours_calculated
         (*subdaily // provide_subdaily()
         ,geocoordinate                                                          //151116
         ,*solar_radiation_extraterrestrial
         ,ref_solar_radiation()
         ,provide_sun_hours());                                                  //160811_160117
      hourly->update(simdate.get_DOY());
         // because hourly could be provided at any time even after
         // today's weather would have been updated.
   }
   return hourly;
}
//_provide_hourly___________________________________________________2015-10-23_/
const Snow_pack    *Land_unit_meteorological::get_snow_pack()              const
{ return snow_pack;                                                              //151023
}
//_get_snow_pack____________________________________________________2015-10-23_/

/* NYI render variable recorders

// Weather
#define CSVP_weather_precipitation
#define CSVP_weather_snow_storage
#define CSVP_weather_temperature
#define CSVP_weather_solar_radiation


// ET
#define CSVP_weather_ET_pot                                          0x20001
#define CSVP_weather_ET_act                                          0x20002
#define CSVP_weather_ET_ref                                          0x20003
*/
//______________________________________________________________________________
bool Land_unit_meteorological::start()                             modification_
{  bool started = CS::Simulation_element_abstract::start();
   if (solar_radiation_extraterrestrial_owned)
      started &=solar_radiation_extraterrestrial->start();
      // MicroBasin have a single E.T. and must start separately
   return started;
}
//_start____________________________________________________________2015-11-30_/
bool Land_unit_meteorological::start_day_subdaily()                modification_
{  bool started = true;
   if (subdaily && subdaily_owned) started &= subdaily->update();                //151212
   if (hourly   && hourly_owned)   started &= hourly->update(simdate.get_DOY()); //151212
   return started;
}
//_start_day_subdaily_______________________________________________2019-05-15_/
bool Land_unit_meteorological::start_day()                         modification_
{  bool started = true;                                                          //151201
   if (sun_owned)  started &= sun->update();                                     //151027
   if (sky_owned)  started &= sky->update_clouds();                              //151027
   weather_provider->update();                                                   //151202
   started &= start_day_subdaily();                                              //190515
   /*190515 moved to start_day_subdaily
   if (subdaily && subdaily_owned)started &= subdaily->update();                 //151212
   if (hourly   && hourly_owned)  started &= hourly->update(simdate.get_DOY());  //151212
   */
   liberated_water.update_value_qualified_code
      (ref_precipitation_actual(),simulated_quality,true);                       //151124
      // Liberated water
      // will be the precipitation unless it is absorbed by snow pack
      // which may take or release as melt.
   if (snow_pack && snow_pack->is_melted_completely())                           //160117
      { delete snow_pack; snow_pack = 0; }
   if (    (ref_air_temperature_min().Celcius() < 0.0)
        && !snow_pack
        && (ref_precipitation_actual().get_m() > 0.0) )
   {  // create snow pack
      switch (snow_pack_model)
      {  case ENERGY_SNOW_PACK :
            snow_pack = new Snow_pack_energy_based // Erin_Brooks
               (liberated_water
               ,ref_precipitation_actual()
               ,ref_air_temperature_avg()
               ,ref_dew_point_temperature_avg()
               ,ref_solar_radiation()
               ,ref_sky()
               ,ref_sun()
               ,ref_aerodynamics()
               ,86400 // time_step
               //,_soil_slope_percent   // Warning need to make these available again
               //,_site_aspect_deg
               //,_forest_vegetation
               );
         break;
         case SIMPLE_SNOW_PACK :
            snow_pack = new Snow_pack_temperature_based
               (liberated_water
               ,ref_precipitation_actual()
               ,ref_air_temperature_avg());
         break;
         default: case NO_SNOW_PACK :
            // snow_pack remains null
         break;
      }
   }
   reported_snow_storage = 0.0;                                                  //160629
   if (snow_pack)
   {
      started = snow_pack->update().is_valid();                                  //151201
      reported_snow_storage = get_snow_water_equivelent_depth();                 //160629
   }
   if (storms && storms_owned)    storms->start_day();                           //160817_160506
   #if (CS_VERSION==5)
   started &= ET_ref->update().is_valid();                                       //160731
   #endif
   return started;                                                               //151201
}
//_start_day________________________________________________________2015-10-23_/
CS::CO2_atmospheric_concentration CO2_not_applicable;                            //150507
const CS::CO2_atmospheric_concentration &Land_unit_meteorological
::ref_CO2_current_conc()                                                   const //150507
{  return atmospheric_CO2_change                                                 //090709
      ? atmospheric_CO2_change->ref_CO2_current_conc()
      : CO2_not_applicable;  // I am currently returning 0 this should trigger an obvious error
   // Caller will need to provide an atmospheric_CO2_change model.
} // ppm
//_ref_CO2_current_conc_________________________________2015-12-01__2003-11-22_/
const Weather_spanning_interface &Land_unit_meteorological::ref_weather_spanning() const
{  return *weather_spanning;
}
//_ref_weather_spanning_____________________________________________2015-12-01_/
const CS::Solar_radiation_extraterrestrial &Land_unit_meteorological
::ref_solar_radiation_extraterrestrial()                                  const
{  return *solar_radiation_extraterrestrial;
}
//_ref_solar_radiation_extraterrestrial____________________________2015-12-01_/
const meteorology::Storms  *Land_unit_meteorological::ref_storms()         const
{  return storms;
}
//_ref_storms_______________________________________________________2015-12-15_/
#define TS_VALUE value_statistic
RENDER_INSPECTORS_DEFINITION(Land_unit_meteorological)
{
   #ifdef CS_OBSERVATION

   CS::Emanator &emanator = CS::get_emanators().know                             //171115
      ((nat32)this,"meteorology",get_ontology(),CS_NOT_EPHEMERAL
      ,0 // Currently rendering is propogating though land unit sim but we dont want that as the context (otherwise it get replicated)
      // the Simulation
      ,simdate);                                                                 //170319
   inspectors.append(new CS::Inspector_scalar(ref_precipitation_actual()                ,emanator,/*check ontology*/"precipitation/actual"             ,TS_VALUE,UED::STD_VC_precipitation.get()));   // UED::STD_VC_precipitation
   inspectors.append(new CS::Inspector_scalar(ref_precipitation_liberated()             ,emanator,/*check ontology*/"precipitation/liberated"          ,TS_VALUE,GENERATE_VARIABLE_CODE));
   inspectors.append(new CS::Inspector_scalar(ref_vapor_pressure_deficit_max()          ,emanator,/*check ontology*/"vapor_pressure"                   ,TS_VALUE,UED::STD_VC_vapor_pressure_actual.get()));
   inspectors.append(new CS::Inspector_scalar(ref_vapor_pressure_daytime()              ,emanator,/*check ontology*/"vapor_pressure/daytime"           ,TS_VALUE,GENERATE_VARIABLE_CODE));  //avg? sum?
   inspectors.append(new CS::Inspector_scalar(ref_vapor_pressure_nighttime()            ,emanator,/*check ontology*/"vapor_pressure/nighttime"         ,TS_VALUE,GENERATE_VARIABLE_CODE));  //avg? sum?
   inspectors.append(new CS::Inspector_scalar(ref_air_temperature_max()                 ,emanator,/*check ontology*/"temperature/air/maximum"          ,TS_VALUE,UED::STD_VC_max_temperature.get()));
   inspectors.append(new CS::Inspector_scalar(ref_air_temperature_min()                 ,emanator,/*check ontology*/"temperature/air/minimum"          ,TS_VALUE,UED::STD_VC_min_temperature.get()));
   inspectors.append(new CS::Inspector_scalar(ref_air_temperature_avg()                 ,emanator,/*check ontology*/"temperature/air/average"          ,TS_VALUE,UED::STD_VC_avg_temperature.get()));
   inspectors.append(new CS::Inspector_scalar(ref_solar_radiation()                     ,emanator,/*check ontology*/"radiation/solar/global"          ,TS_VALUE,UED::STD_VC_solar_radiation_global.get()));
   inspectors.append(new CS::Inspector_scalar(ref_dew_point_temperature_max()           ,emanator,/*check ontology*/"temperature/dew_point"            ,TS_VALUE,UED::STD_VC_max_dew_point_temperature.get()));
   inspectors.append(new CS::Inspector_scalar(ref_dew_point_temperature_min()           ,emanator,/*check ontology*/"temperature/dew_point"            ,TS_VALUE,UED::STD_VC_min_dew_point_temperature.get()));
   inspectors.append(new CS::Inspector_scalar(ref_dew_point_temperature_avg()           ,emanator,/*check ontology*/"temperature/dew_point"            ,TS_VALUE,UED::STD_VC_avg_dew_point_temperature.get()));
   inspectors.append(new CS::Inspector_scalar(ref_relative_humidity_max()               ,emanator,/*check ontology*/"humidity/relative/maximum"        ,TS_VALUE,UED::STD_VC_max_relative_humidity.get()));
   inspectors.append(new CS::Inspector_scalar(ref_relative_humidity_min()               ,emanator,/*check ontology*/"humidity/relative/minimum"        ,TS_VALUE,UED::STD_VC_min_relative_humidity.get()));
   //NYN    inspectors.append(new CS::Inspector_scalar(ref_vapor_pressure_actual()      ,emanator,/*check ontology*/"vapor_pressure/actual",UNKNOWN_statistic));
   inspectors.append(new CS::Inspector_scalar(ref_vapor_pressure_deficit_fullday_avg()  ,emanator,/*check ontology*/"vapor_pressure_deficit/fullday"  ,TS_VALUE,UED::STD_VC_avg_daytime_vapor_pressure_deficit.get())); //avg? sum?
   inspectors.append(new CS::Inspector_scalar(ref_vapor_pressure_deficit_daytime()      ,emanator,/*check ontology*/"vapor_pressure_deficit/daytime"  ,TS_VALUE,UED::STD_VC_avg_fullday_vapor_pressure_deficit.get())); //avg? sum?
   inspectors.append(new CS::Inspector_scalar(ref_wind_speed()                          ,emanator,/*check ontology*/"wind_speed"                      ,TS_VALUE,UED::STD_VC_wind_speed.get()));
   inspectors.append(new CS::Inspector_scalar(ref_CO2_current_conc()                    ,emanator,/*check ontology*/"CO2/atmospheric"                 ,TS_VALUE,GENERATE_VARIABLE_CODE)); //171220

   #if (CS_VERSION==5)
   inspectors.append(new CS::Inspector_scalar(
       ref_ET_reference().amount
      ,UC_m   ,emanator,/*check ontology*/"biometeorology:ET/potential/reference"    ,TS_VALUE,CSVP_weather_ET_ref));
   #endif
   inspectors.append(new CS::Inspector_scalar(reported_snow_storage                ,UC_m,emanator,/*check ontology*/"snow_storage"                    ,sum_statistic  ,GENERATE_VARIABLE_CODE));
      //snow storage is already cumulative
   return &emanator;
   #else
   return 0;
   #endif
}
//_RENDER_INSPECTORS_DEFINITION_____________________________________2016-06-26_/
}//_namespace_CS_______________________________________________________________/

