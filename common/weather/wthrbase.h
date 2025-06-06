#ifndef wthrbaseH
#define wthrbaseH
#include "corn/chronometry/date_I.h"
// This is the base weather abstract class
#ifndef REACCH_VERSION
#  include "common/weather/loc_param.h"
#endif

#  include "common/weather/weather_provider.h"
#  include "common/weather/weather_interface.h"
#  include "common/weather/subdaily.h"
#  include "common/weather/seasons.h"
#include "common/weather/sky.h"
#include "corn/container/enumlist.h"

//          The following are included because they are super classes
#if ((CS_VERSION>=1) && (CS_VERSION<=4))
#include "common/weather/wind.h"
#else
#endif
#include "common/biometeorology/aerodynamics.h"
#ifndef measuresH
#  include "corn/measure/measures.h"
#endif
#ifndef constH
#  include "corn/const.h"
#endif
#ifndef dynamic_array_TH
#  include"corn/dynamic_array/dynamic_array_T.h"
#  define Dynamic_float_array Dynamic_array<float32>
#endif

#define WEATHER_GENERATION
#ifdef FOR_CANMS_DAIRY
#undef WEATHER_GENERATION
#endif
#if (defined(__linux__) || defined(__GNUC__))
//LML 150309 added "__GNUC__"
#undef WEATHER_GENERATION
#endif
#ifdef OFOOT_VERSION
#undef WEATHER_GENERATION
#endif
#undef WEATHER_GENERATION
//__Forward_declarations________________________________________________________
class Weather_database;
class Weather_with_solar_rad_estimation;   // Forward declaration
class CropSyst_ET;                         // Forward declaration
class Reference_ET_FAO;                                                          //050619
class Atmospheric_CO2_change_element;                                            //171207_090709
class Reference_evapotranspiration_model_set;                                    //090928
class ClimGen_Parameters;                                                        //091214
//______________________________________________________________________________
class Weather_base
: public Weather_interface                                                       //031122
, public CS::Weather_subdaily                                                    //151212
#if ((CS_VERSION>=1) && (CS_VERSION<=4))
, public Wind                                                                    //991001
#else
  //110104 now member Aerodynamics
#endif
, public CS::Seasons                                                             //140813
{
   friend class Reference_ET_FAO;                                                //050619
   friend class Reference_evapotranspiration_model_set;                          //050619
 public:
   Weather_provider &provider;                                                   //151201
      // Not const because we load
 private:  // I/O
   bool                 database_read_only;                                      //010726
    // This mode is intended for ClimGen
    // Data loaded from the database that is marked worse than generated quality will be
    // treated as missing so it will be regenerated.
   bool                 treat_valid_data_as_real;                                //081109
      // This is used by CropSyst to assume all the valid quality data is as measured data
      // This prevents the non-measured (I.e. ClimGen Generated) weather elements from being recomputed in CropSyst
      // May also use treat_valid_data_as_real in ClimPar when using the include generated data site analysis mode
 public:
   const Geocoordinate        &geocoordinate;                                    //151027
   mutable Weather_database   *weather_data;        // This will be a pointer to either weather_data_UED or weather_data_text (don't delete this) 061218
 public:  // This is temporarily public I would like to create accessors for the location
   DOY      avg_DOY_with_min_annual_temperature;                                 //060427
   // Average day of the year with minimum yearly temperature
   // This is used for the hourly soil temperature submodel.
   // It could be calculated for each year.
   // RLN becareful with southern hemisphere
 private: // Annual calculations
   Year     updated_for_year;                                                    //041026
    // WARNING CropSystVB only looks and the first year of the simulation for these,
    // this is looking at the current year (each year).
    // RLN: Probably should be done for all available years!!!
   float64  annual_temperature_avg;                                              //041026
 public:
   //___________________________________________________________________________
 public: // constructors
   Weather_base
      (Weather_database    *weather_database_
      ,modifiable_  Weather_provider &provider_                                  //151201
      ,const CORN::Date_const &today_                                            //150125
      ,const Geocoordinate &geocoordinate_                                       //150629
      ,float64              screening_height                                     //151128
      ,bool                treat_valid_data_as_real_                             //081109
      ,bool                read_only_                                            //000711
      ,CORN::Seconds       time_step_seconds_ = 86400);
   virtual ~Weather_base();                                                      //030711
   virtual bool done_with_dates_before(const CORN::Date_const &earliest_date) const;//141103
   friend class Weather_with_solar_rad_estimation;
   friend class CropSyst_ET;
 public:
   virtual CORN::date32 get_earliest_available_date()                     const; //150209
   virtual bool start_day();                                                     //091216
   virtual bool update()                           /*should be rectification_*/; //151212
   virtual void load();
   virtual void commit( const Date_const &);                                     //050616
//180131   virtual void invalidate(bool absolutely);                                     //091216
 protected:
   mutable  Season_S_S_F_W season_S_S_F_W;
 protected: // snow related
   float64 precipitate();      // returns liquid total water entering the system //991004
 protected: friend class ClimGen_Parameter_generator;
   // Stored values for the current time step  (these are mutable so get_XXX() can update as needed).
 public:  // CropSyst version3 needs direct access for I/O
   #define curr_max_vapor_pressure_deficit provider.vapor_pressure_deficit_max
   #define curr_vapor_pressure_max         provider.vapor_pressure_daytime
   #define curr_vapor_pressure_nighttime   provider.vapor_pressure_nighttime
   #define curr_max_temperature            provider.air_temperature_max
   #define curr_min_temperature            provider.air_temperature_min
   #define curr_air_temperature_avg        provider.air_temperature_avg
   #define curr_solar_radiation            provider.solar_radiation
   #define curr_max_dew_point_temperature  provider.dew_point_temperature_max
   #define curr_min_dew_point_temperature  provider.dew_point_temperature_min
   #define curr_avg_dew_point_temperature  provider.dew_point_temperature_avg
   #define curr_max_relative_humidity      provider.relative_humidity_max
   #define curr_min_relative_humidity      provider.relative_humidity_min
   #define curr_vapor_pressure_actual      provider.vapor_pressure_actual
   #define curr_avg_fullday_vapor_pressure_deficit provider.vapor_pressure_deficit_fullday_avg
   #define curr_vapor_pressure_deficit_daytime  provider.vapor_pressure_deficit_daytime
   #define curr_wind_speed                 provider.wind_speed
   #define curr_precipitation              provider.precipitation_actual
   mutable float64 curr_pot_evapotranspiration_m; mutable CORN::Quality_clad pot_evapotranspiration_quality;   //991109
 public:  // Weather interface implementations
#if ((CS_VERSION>=1) && (CS_VERSION<=4))
   inline virtual float64 calc_normalized_wind_speed(float64 wind_speed_m_ts)    const
      { return Wind::calc_normalized_wind_speed(wind_speed_m_ts); }
   inline virtual float64 calc_aerodynamic_resistance_of_plants                  //100421
      (float64 wind_speed_m_d // meters/day
      ,float64 plant_height_m = 0.12)                                      const
      { return Wind::calc_aerodynamic_resistance_of_plants(wind_speed_m_d,plant_height_m); }
   inline virtual float64 calc_slope_of_saturation(float64 avg_air_temperature) const //100419
      { return curr_max_vapor_pressure_deficit.calc_slope_of_saturation(avg_air_temperature); }
#else
   inline virtual float64 calc_normalized_wind_speed(float64 wind_speed_m_ts)    const
      { return provider.aerodynamics.calc_normalized_wind_speed(wind_speed_m_ts); } //110104
   inline virtual float64 calc_aerodynamic_resistance_of_plants                 //100421
      (float64 wind_speed_m_d // meters/day                                     //100421
      ,float64 plant_height_m = 0.12)                                     const //100421
      { return provider.aerodynamics.calc_resistance_of_plants_d_m(wind_speed_m_d,plant_height_m); }      //100421
    inline virtual float64 calc_slope_of_saturation(float64 avg_air_temperature) const                //100419
      { return provider.vapor_pressure_deficit_max.calc_slope_of_saturation(avg_air_temperature); }
#endif
 public:  // weather data accessors
   // These will all need renaming to get_XXXXX_today_UNITS()
   inline virtual const Solar_radiation &ref_solar_radiation()             const { return provider.solar_radiation; }       //140715
   inline virtual const Precipitation &ref_precipitation_actual()                 const { return provider.precipitation_actual;} //160414_151028
   inline virtual float64 get_curr_wind_speed_m_d()                        const { return provider.wind_speed.get_m_d(); }    // m/d
   inline virtual float64 get_curr_wind_speed_m_s()                        const { return provider.wind_speed.get_m_s(); }    // m/s
   inline virtual const CORN::Quality &get_precipitation_quality()         const { return provider.precipitation_actual; }

   inline virtual const CORN::Quality &get_wind_speed_quality()            const { return provider.wind_speed; }
   inline virtual const Air_temperature_average &ref_air_temperature_avg() const { return provider.air_temperature_avg; } //140507
   inline virtual const Wind_speed              &ref_wind_speed()          const { return provider.wind_speed; }          //150521

   virtual float64 get_air_temperature_min_tomorrow()                      const;//151201
   virtual float64 get_air_temperature_max_yesterday()                     const;//151202


   inline virtual float64 get_curr_pot_evapotranspiration()                const { return curr_pot_evapotranspiration_m; } // m               //991109
   //200326        virtual float64 get_curr_max_air_temperature()                   const;// C  { return curr_max_temperature.get(); };   //  C   //100420
   inline virtual bool is_temperature_hourly_valid()                       const { return true;}                                             //100929

   // Currently CO2 data is not recorded, CropSyst may generate CO2 change
   // Otherwise the default CO2 concentration is returned

   inline virtual float64 get_aerodynamic_resistance_of_snow_s_m()         const //011008
#if ((CS_VERSION>=1) && (CS_VERSION<=4))
                  { return  get_aerodynamic_resistance_of_snow(SECONDS_PER_METER); }

#else
                  { return
                     provider.
                     aerodynamics.get_resistance_of_snow(SECONDS_PER_SECOND);    //150603
                  }
#endif                                                                                                                                                                                          //000612
   inline virtual float64 get_curr_max_vapor_pressure_deficit()                const { return curr_max_vapor_pressure_deficit       .get_value_update_if_necessary(); }//150126_081027
   inline virtual float64 get_curr_vapor_pressure_max(Quality &quality)        const { float64 value = curr_vapor_pressure_max      .get_value_update_if_necessary(); quality.assume(curr_vapor_pressure_max);       return value;}  //150126_081027
   inline virtual float64 get_curr_vapor_pressure_min(Quality &quality)        const { float64 value = curr_vapor_pressure_nighttime.get_value_update_if_necessary(); quality.assume(curr_vapor_pressure_nighttime); return value;}  //150126_991012
   /*200322 obsolete because properties have quality
   inline virtual const CORN::Quality &get_max_temperature_quality()           const { return curr_max_temperature.get_quality(); }                                                 //991012
   inline virtual const CORN::Quality &get_min_temperature_quality()           const { return curr_min_temperature.get_quality(); }                                                 //991012
   inline virtual const CORN::Quality &get_max_dew_point_temperature_quality() const { return curr_max_dew_point_temperature.get_quality(); }                                       //991012
   inline virtual const CORN::Quality &get_min_dew_point_temperature_quality() const { return curr_min_dew_point_temperature.get_quality(); }                                       //050610
   inline virtual const CORN::Quality &get_avg_dew_point_temperature_quality() const { return curr_avg_dew_point_temperature.get_quality(); }                                       //991012
   inline virtual const CORN::Quality &get_max_relative_humidity_quality()     const { return curr_max_relative_humidity.get_quality(); }                                           //991012
   inline virtual const CORN::Quality &get_min_relative_humidity_quality()     const { return curr_min_relative_humidity.get_quality(); }                                           //991109
   inline virtual const CORN::Quality &get_pot_evapotranspiration_quality()    const { return pot_evapotranspiration_quality; }                                                     //080617
   inline virtual const CORN::Quality &get_avg_fullday_vapor_pressure_deficit_quality()const { return curr_avg_fullday_vapor_pressure_deficit.get_quality(); }                      //081027
   inline virtual const CORN::Quality &get_vapor_pressure_max_quality()        const { return curr_vapor_pressure_max.get_quality(); }                                                      //081027
   inline virtual const CORN::Quality &get_vapor_pressure_min_quality()        const { return curr_vapor_pressure_nighttime.get_quality(); }                                                //100514
   inline virtual const CORN::Quality &get_curr_max_vapor_pressure_deficit_quality()   const { return curr_max_vapor_pressure_deficit.get_quality(); }
   */
   virtual bool any_valid_data_today();                                                                                                                                             //031122
 public: // File I/O The following load the data for a specific date

   bool load_weather_provider(Weather_provider &weather_provider) mutation_;
                  // note, shouldn't need to pass the date because it is already a member of weather_provider
#              define Weather_daily_save_formals const Date_time &i_date_time, float64 value,CORN::Quality &attribute
 public:
      // Attempt to provide the best element values for the specified date.
      // It is not guarented that the element can be provided for that date
      // So these methods may return 0.
      // The returned element is not relinquished to the caller.
 protected:
   void fix_up_relative_humidity();                                              //050616
 public:
   virtual bool get_parameter_on                                                 //151122
      (modifiable_ CS::Parameter &parameter
      ,nat32  variable_code // Typically the variable code will be UED variable code
      ,const CORN::Date_const &on_date)                          rectification_;

 public: // The following load the data for the current time step
   // load() now loads to weather provider
   inline virtual float64 load_curr_pot_evapotranspiration_m()        mutation_
    {pot_evapotranspiration_quality.assume_code(not_valid_quality); return 0.0;} //991109
 public: // overrides
   #if (CS_VERSION < 5)
   virtual const Location_parameters *get_location_parameters()           const; //090303
   #endif
   virtual const Geolocation         *get_geolocation_constant()          const; //050525

   // These are needed here because sun requires overrides

   virtual const char *get_description(std::string &return_buffer)        const; //041109
   const CORN::Quality &load_wind_speed_estimate_if_necessary
      (Wind_speed &wind_speed
      ,const CORN::Date_const *date = OPTIONAL_DATE);                            //150126
   /*200326 Now require/use monthly mean
   float64 forecast_wind_speed_m_d(CORN::DOY DOY);                               //970303
   */
 private: // The following methods are used by ClimGen to provide generation parameters                            //091214
   contribute_ const ClimGen_Parameters  *ClimGen_parameters;  // not owned                                       //091214
      // Will be 0 when not using ClimGen when not 0 assume Generation mode                                       //091214
      // Owned by caller who should ensure this exists for the duration of the scope of weather base.             //091214
 public:  // The following methods should only be used by ClimGen                                                  //091214
//#ifndef FOR_CAFE_DAIRY
#ifdef WEATHER_GENERATION
   virtual void know_generation_parameters
      (const ClimGen_Parameters  &_ClimGen_parameters)            contribution_; //091214
      // Optional used for generation
#endif
   inline void update_max_dew_point_temperature() { curr_max_dew_point_temperature.update();}  //091216
   inline void update_min_dew_point_temperature() { curr_min_dew_point_temperature.update();}  //091216
};
//_Weather_base________________________________________________________________/
#endif
// 060529 421 lines

