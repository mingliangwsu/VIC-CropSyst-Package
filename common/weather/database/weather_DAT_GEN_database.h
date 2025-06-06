#ifndef weather_DAT_GEN_databaseH
#define weather_DAT_GEN_databaseH
#include "common/weather/database/weather_database.h"
#include "common/weather/database/wthrfile.h"
#include "common/weather/loc_param.h"
using namespace CORN;
using namespace CORN::OS;
using namespace CORN::OS::FS;
// included because it is a super class
//______________________________________________________________________________
class Weather_data_text   // rename this to Weather_DAT_GEN_database
: public Weather_database
{
   Location_parameters  &location_parameters;// Currently always owned by caller //050525
   CORN::Date_clad_32 current_date;                                              //091216
 protected:
   /*CORN::OS::*/File_name               &template_filename;                         //130826_000313
      // will include weather path reference so we can override after simulation starts
   bool                          for_write;
   Weather_text_file::Format_old format_to_write;                                //021216
   std::string                   pref_ext;                                       //040909
   bool                          allow_missing_dates;                            //040427
   // Two buffers are provided since values may need to be computed
   // for data spanning more that one year.
   mutable bool                  need_to_write_column_headers;                   //060221
   std::ofstream                     *single_outfile;                            //060221
      // When writing a single file (V4 text file) we need to keep the file open for each buffer)
   mutable Weather_text_file    *current;  // Needs to be public so we can use today macro
   mutable Weather_text_file    *aux;
   mutable bool override_set_safety;                                             //081117 Warning, this is not yet implemented in set methods, but probably not applicable since this is generally not an output format.
 public: // constructor
   Weather_data_text
      (Location_parameters &location_params                                      //000313
      ,CORN::OS::File_name      &template_filename);                                  //130827_000313
        // Now that we are passing location, we dont need to pass the template filename
   virtual ~Weather_data_text();
   virtual const Location_parameters *get_location_parameters()           const; //090303
   virtual const Geolocation         *get_geolocation_constant()          const; //050525
   virtual const char *get_description(std::string &return_buffer)        const; //050525
   inline virtual bool start()                                                   //050111
      {  reset_buffers(); return true; }                                         //000519
   inline void set_allow_missing_dates(bool _allow) { allow_missing_dates = _allow; }  //040427
   inline bool solar_radiation_is_real()
      {  return !for_write && current && current->solar_radiation_is_real(); }
   virtual void set_today(const CORN::Date_const &new_today);
   virtual void invalidate(bool absolutely);                                     //091216
   Weather_text_file *select_buffer(Year  _date)                       mutates_;
  // This function return a pointer to a weather_file buffer
  // containing the requested CORN::Date.
  // If i_date = 0, then the current buffer is returned and
  // select_DOY returns the current DOY
   void setup_current(Year year) mutates_;
   virtual void reset_buffers();
 public: // get recorded data for a date
   virtual bool get_to    (Weather_provider &provider);                          //170111
   virtual bool set_from  (Weather_provider &provider);                          //170111

   /*170111
   virtual float64 get_max_temperature            (weather_database_get_formals) const; //971013
   virtual float64 get_min_temperature            (weather_database_get_formals) const; //971013
   virtual float64 get_solar_radiation            (Solar_radiation &solar_rad, const CORN::Date_time &) const;   //140715_971013
   virtual float64 get_max_relative_humidity      (weather_database_get_formals) const; //980306
   virtual float64 get_min_relative_humidity      (weather_database_get_formals) const; //980306
   virtual float64 get_precipitation_m            (weather_database_get_formals) const; //980306
   virtual float64 get_wind_speed_m_d             (weather_database_get_formals) const; //980306
   virtual float64 get_max_dew_point_temperature  (weather_database_get_formals) const; //980305
   virtual float64 get_min_dew_point_temperature  (weather_database_get_formals) const; //980305
   virtual float64 get_avg_dew_point_temperature  (weather_database_get_formals) const; //050603
   virtual float64 get_cloud_cover_percent        (weather_database_get_formals) const; //991022
   virtual float64 get__isothermal_longwave_net_radiation(weather_database_get_formals) const { return 0; } //070204
   virtual float64 get_net_radiation              (weather_database_get_formals) const  { return 0; }      //070204
   virtual float64 get_pot_evapotranspiration_m   (weather_database_get_formals) const;                     //991109
   virtual float64 get_avg_daytime_vapor_pressure_deficit_kPa  (weather_database_get_formals) const ;       //991109
   virtual float64 get_avg_fullday_vapor_pressure_deficit_kPa  (weather_database_get_formals) const ;       //011009
 public: // set data methods
   virtual float64 set_precipitation_mm         (const CORN::Date_time &date_time,float64 precip_mm            ,CORN::Quality &attribute);              //990424
   virtual float64 set_max_temperature          (const CORN::Date_time &date_time,float64 degrees_C            ,CORN::Quality &attribute);              //990424
   virtual float64 set_min_temperature          (const CORN::Date_time &date_time,float64 degrees_C            ,CORN::Quality &attribute);              //990424
   virtual float64 set_avg_temperature          (const CORN::Date_time &date_time,float64 degrees_C            ,CORN::Quality &attribute) { return 0;} //990424// Does not apply to this file type
   virtual float64 set_solar_radiation          (const CORN::Date_time &date_time,float64 solar_rad_MJ_m2      ,CORN::Quality &attribute) mutates_ ;    //990424
   virtual float64 set_max_relative_humidity    (const CORN::Date_time &date_time,float64 max_rel_humid_percent,CORN::Quality &attribute);              //990424
   virtual float64 set_min_relative_humidity    (const CORN::Date_time &date_time,float64 min_rel_humid_percent,CORN::Quality &attribute);              //990424
   virtual float64 set_max_dew_point_temperature(const CORN::Date_time &date_time,float64 max_dew_point_temp   ,CORN::Quality &attribute);              //990424
   virtual float64 set_min_dew_point_temperature(const CORN::Date_time &date_time,float64 min_dew_point_temp   ,CORN::Quality &attribute);              //990424
   virtual float64 set_avg_dew_point_temperature(const CORN::Date_time &date_time,float64 min_dew_point_temp   ,CORN::Quality &attribute);              //050619
   virtual float64 set_wind_speed_m_d           (const CORN::Date_time &date_time,float64 wind_speed_m_d       ,CORN::Quality &attribute);              //990424
   virtual float64 set_cloud_cover_percent      (const CORN::Date_time &date_time,float64 cloud_cover_percent  ,CORN::Quality &attribute) { return cloud_cover_percent;} ; // NYI 990424_
   // The old text database did not store these:
   virtual float64 set__isothermal_longwave_net_radiation(weather_database_set_formals) { return 0;  }                              //070204
   virtual float64 set_net_radiation            (weather_database_set_formals)         { return 0;  }                              //070204

   virtual float64 set_pot_evapotranspiration_mm (const CORN::Date_time& date_time,float64 precip_mm            ,CORN::Quality& attribute);             //991109
   virtual float64 set_pot_evapotranspiration_FAO_Penman_Monteith_reference_mm(weather_database_set_formals) { return 0.0; } // does not apply to this file type                 //070525
   virtual float64 set_avg_daytime_vapor_pressure_deficit_kPa(const CORN::Date_time& date_time,float64 precip_mm,CORN::Quality& attribute);             //991109
   virtual float64 set_avg_fullday_vapor_pressure_deficit_kPa(const CORN::Date_time& date_time,float64 precip_mm,CORN::Quality& attribute);             //011009
   */

   virtual bool get_parameter_on                                                 //170111
      (modifiable_ CS::Parameter &parameter
      ,nat32                      variable_code // Typically the variable code will be UED variable code
      ,const CORN::Date_const    &on_date)                        rectification_
      { return false; }
      // Not yet needed.  Not implemented because this class is becoming obsolete
      // and it currently only used for UED to CropSyst convertor

   virtual void set_precipitation_intervals_mm                                   //990425
      (const CORN::Date_const &date
      ,const CORN::Time &time                                                    //990505
      ,const CORN::Dynamic_array<float32> &precip_in_intervals
      ,CORN::Minutes interval_time_minutes                                       //990425
      ,CORN::Quality &attribute);
   virtual void set_temperature_intervals                                        //021124
      (const CORN::Date_const &date                                              //021124
      ,const CORN::Dynamic_array<float32> &temperature_in_intervals              //021124
      ,CORN::Minutes interval_time_minutes                                       //990425
      ,CORN::Quality &attribute);
   virtual void setup_to_write(Weather_text_file::Format_old desired_format);    //021216
   virtual inline void set_preferred_ext(const char *pref_ext_)                  //040909
      {  pref_ext.assign(pref_ext_); }
   inline virtual void set_override_safety(bool _override_set_safety) modification_
      { override_set_safety = _override_set_safety;}                             //050524
   inline virtual CORN::date32 get_earliest_available_date()               const
      { assert(false); return 0; }                                               //140911
      // This is not currently needed, eventually this class will be obsolete
   inline virtual bool is_good() const { return true;} 
};
//______________________________________________________________________________
#endif

