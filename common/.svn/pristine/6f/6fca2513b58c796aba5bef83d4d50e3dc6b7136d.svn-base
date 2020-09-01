#ifndef WC_humidity_specificH
#define WC_humidity_specificH
#include "weather/parameter/WP_humidity_specific.h"

/* check needed
#include "weather/weather_types.h"
//170525 #include "corn/datetime/date.hpp"
#include "corn/chronometry/time_types.hpp"
#include "weather/parameter/WP_dew_point_temperature.h"

#ifdef ALREADY_INCLUDED
#include "weather/parameter/weather_parameter.h"

   // for Season_S_W_SF
#include "UED/library/codes.h"
#include "corn/const.h"
#endif
// Forward declarations:
class Precipitation;                                                             //101215
class Dew_point_temperature_minimum;
class Dew_point_temperature_average;
class Dew_point_temperature_maximum;
class Air_temperature_minimum ;
class Air_temperature_maximum ;
class Vapor_pressure_deficit_max;
//140220 class Vapor_pressure_nighttime_parameter;
class Vapor_pressure_nighttime;                                                  //140220

//struct Relative_humidity_extremes;                                             //091216
*/
//______________________________________________________________________________

class Humidity_specific_calculator
: public extends_ Humidity_specific
{
 protected:
 public:
   inline Humidity_specific_calculator                                           //070212
      (float64 value_, const CORN::Quality &quality_)                            //070212
      : Humidity_specific()                                                      //140220
      {  UNUSED_arg(value_); UNUSED_arg(quality_); }                             //151001

   Humidity_specific_calculator();                                               //140220
        // May be 0 if unknown //081016
  virtual const CORN::Quality &update()                          rectification_; //080908
};
//______________________________________________________________________________
#endif
