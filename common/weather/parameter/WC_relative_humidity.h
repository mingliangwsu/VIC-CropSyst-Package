#ifndef WC_relative_humidityH
#define WC_relative_humidityH
#include "weather/parameter/WP_relative_humidity.h"
#include "weather/weather_types.h"
#include "corn/chronometry/time_types.hpp"
#include "weather/parameter/WP_dew_point_temperature.h"
#include "weather/parameter/WP_air_temperature.h"

#ifdef ALREADY_INCLUDED
#include "weather/parameter/weather_parameter.h"

   // for Season_S_W_SF
#include "UED/library/codes.h"
#include "corn/const.h"
#endif
// Forward declarations:
class Precipitation;                                                             //101215
class Humidity_specific;                                                         //180808
class Atmospheric_pressure;                                                      //180808
class Vapor_pressure_deficit_max;
class Vapor_pressure_nighttime;                                                  //140220
struct Relative_humidity_extremes;                                               //091216
//______________________________________________________________________________
class Relative_humidity_calculator
: public Relative_humidity
{
 protected:
   Dew_point_temperature_average   *dew_point_temperature_avg;   // May be 0 if unknown
   Vapor_pressure_deficit_max      *vapor_pressure_deficit_max;                  //081016
   Air_temperature_maximum         *air_temperature_max;  // May be 0 if unknown   081016
   contribute_ const Relative_humidity_extremes *generation_parameters;          //091216
      // Only applies to ClimGen generation
   contribute_ const Season_S_W_SF              *current_season_known;           //091217
      // reference the weather season
 public:
   inline Relative_humidity_calculator                                           //070212
      (float64 _value, const CORN::Quality &_quality)                            //070212
      : Relative_humidity()                                                      //140220
      , dew_point_temperature_avg(0)                                             //081016
      , vapor_pressure_deficit_max(0)                                            //081016
      , air_temperature_max(0)                                                   //081016
      , generation_parameters(0)                                                 //091216
      {  set_value_quality(_value,_quality,true); }                              //151001
   Relative_humidity_calculator                                                  //140220
      (Dew_point_temperature_average   *dew_point_temperature_avg
      ,Vapor_pressure_deficit_max      *vapor_pressure_deficit_max               //081016
      ,Air_temperature_maximum         *air_temperature_max); // May be 0 if unknown //081016
   virtual float64 set_value_quality                                             //150121
      (float64 _value
      ,const CORN::Quality &_quality
      ,bool override_safety = false)                                  mutation_; //090312
   void know_generation_parameters                                               //091216
      (const Relative_humidity_extremes &generation_parameters
      ,const Season_S_W_SF &current_season_known)                    cognition_; //180810 contribution_;
};
//_Relative_humidity_calculator________________________________________________/
class Relative_humidity_extreme_calculator // not really extreme need better name
: public extends_ Relative_humidity_calculator
{
 protected: // When humidity_specific is available
   const Humidity_specific       *humidity_specific;
   const Atmospheric_pressure    *atmospheric_pressure;
 public: // structors
   inline Relative_humidity_extreme_calculator
      (float64 value_, const CORN::Quality &quality_)
      : Relative_humidity_calculator(value_,quality_)
      {}
    inline Relative_humidity_extreme_calculator
      (Dew_point_temperature_average   *dew_point_temperature_avg_
      ,Vapor_pressure_deficit_max      *vapor_pressure_deficit_max_
      ,Air_temperature_maximum         *air_temperature_max_)  // May be 0 if unknown //081016
      : Relative_humidity_calculator
         (dew_point_temperature_avg_
         ,vapor_pressure_deficit_max_
         ,air_temperature_max_)
      , humidity_specific(0)
      {}
   inline Relative_humidity_extreme_calculator
      (const Humidity_specific       *humidity_specific_
      ,const Atmospheric_pressure    *atmospheric_pressure_)
      : Relative_humidity_calculator(0,0,0)
      , humidity_specific     (humidity_specific_)
      , atmospheric_pressure  (atmospheric_pressure_)
      {}
 public:
   float64 calc_from_humidity_specific(CORN::Quality &RH_quality)      const;
 private:
   virtual const Air_temperature &ref_temperature_air_associated()      const=0;
};
//_class Relative_humidity_extreme_calculator_______________________2018-08-10_/
class Relative_humidity_maximum_calculator
: public Relative_humidity_extreme_calculator                                    //140220
{  Air_temperature_minimum          *air_temperature_min;
      // May be 0 if it relative humidity is known.
      // required if calculating relative humidity with either method.
   Dew_point_temperature_minimum   *dew_point_temperature_min;   // May be 0 if unknown
   Vapor_pressure_nighttime        *vapor_pressure_nighttime;                    //081030
   Precipitation                   *precipitation;                               //101215
   bool precipitation_today;                                                     //100119
   contribute_ CORN::Days           unlimited_RHmax_window;                      //100119
   bool                             when_precip_set_100;
      // Warning this would be an option for ClimGen It currently isnt set
      // This was the default option but was disabled for CropSyst 200228
public:
   Relative_humidity_maximum_calculator
      (float64 _value, const CORN::Quality & _quality);
      // Use this constructor when relative humidity is known.
   Relative_humidity_maximum_calculator
      (Air_temperature_maximum         *air_temperature_max                      //081016
      ,Air_temperature_minimum         *air_temperature_min
      ,Dew_point_temperature_minimum   *dew_point_temperature_min
      ,Vapor_pressure_deficit_max      *vapor_pressure_deficit_max               //081016
      ,Dew_point_temperature_average   *dew_point_temperature_avg
      ,Vapor_pressure_nighttime        *vapor_pressure_min
      ,Precipitation                   *precipitation = 0);                      //101215
      // Use this constructor when there is a potential that either average dew point temperature or VDP max may be available
   Relative_humidity_maximum_calculator
      (Air_temperature_maximum         *air_temperature_max                      //081016
      ,Air_temperature_minimum         *air_temperature_min
      ,Dew_point_temperature_minimum   *dew_point_temperature_min
      ,Vapor_pressure_deficit_max      *vapor_pressure_deficit_max               //081016
      ,Precipitation                   *precipitation = 0);                      //101215
      // Use this constructor when it is known that only minimum dew point temperature is available
      // Currently the calculation based on minimum dew point temperature
      // appears to provide the best results, it should preferred of
      // the use of average dew point temperature
   Relative_humidity_maximum_calculator
      (Air_temperature_minimum         *air_temperature_min
         // Required  (070212 I had noted that this could be 0 if unknown, but that doesn't appear to be the case.)
      ,Dew_point_temperature_average   *dew_point_temperature_avg  // May be 0 if unknown
      ,Precipitation                   *precipitation = 0);                      //101215
      // Use this constructor when it is known that only average dew point temperature is available
   Relative_humidity_maximum_calculator                                          //180810
      (Air_temperature_minimum       *air_temperature_min_ // required
      ,const Humidity_specific       *humidity_specific_
      ,const Atmospheric_pressure    *atmospheric_pressure_);
      // use this constructor whene specific humidity is available
  virtual const CORN::Quality &update()                          rectification_; //080908
 public:
   virtual float64 calc_from_DewPt_min                (CORN::Quality &calced_max_rel_humidity_quality)   const;
           float64 est_from_vapor_pressure_nighttime  (CORN::Quality &RHmax_quality)                     const;//091212
   virtual float64 est_from_avg_dew_point_temperature (CORN::Quality &calced_max_rel_humidity_quality)   const;
 private:
   inline virtual const Air_temperature &ref_temperature_air_associated()  const //180808
      { return *air_temperature_min; }
};
//_Relative_humidity_maximum_calculator________________________________________/
class Relative_humidity_minimum_calculator
: public Relative_humidity_extreme_calculator
{  Dew_point_temperature_maximum   *dew_point_temperature_max;   // May be 0 if unknown
public:
   Relative_humidity_minimum_calculator                                          //140220
      (float64 value_, const CORN::Quality &quality_);
      // This constructor is used when the value is known
   Relative_humidity_minimum_calculator                                          //140220
      (Air_temperature_maximum         *air_temperature_max_     // May be 0 if unknown
      ,Dew_point_temperature_maximum   *dew_point_temperature_max_               //081028
      ,Vapor_pressure_deficit_max      *vapor_pressure_deficit_max_
      ,Dew_point_temperature_average   *dew_point_temperature_avg_);   // May be 0 if unknown
      // This constructor is used when any of these parameters are potentially available
      // (I am pretty sure  _air_temperature_max is required for the calculation mode.)
   Relative_humidity_minimum_calculator                                          //180810
      (Air_temperature_maximum       *air_temperature_max_  // required
      ,const Humidity_specific       *humidity_specific_
      ,const Atmospheric_pressure    *atmospheric_pressure_);
      // use this constructor when specific humidity is available
   // NYI could add additional constructors when certain parameters are known, I just need to trace this down
  virtual const CORN::Quality &update()                          rectification_; //080908
public:  // I suppose the following could be public
   virtual float64 calc_from_VPsat_at_max_dew_point(CORN::Quality &calced_min_rel_humidity_quality) const;
   virtual float64 est_from_avg_dew_point_temperature(CORN::Quality &calced_min_rel_humidity_quality) const;
   // Calculation from avg dew pt should be considered less preferable than calc_from_VPsat_at_max_dew_point
   virtual float64 est_from_VPD_max(CORN::Quality &est_min_rel_humidity_quality) const;//081016
 private:
   inline virtual const Air_temperature &ref_temperature_air_associated()  const //180808
      { return *air_temperature_max; }
};
//_Relative_humidity_minimum_calculator________________________________________/
#endif


