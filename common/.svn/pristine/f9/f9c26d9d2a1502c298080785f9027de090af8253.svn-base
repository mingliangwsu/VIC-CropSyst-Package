#include "weather/weather_types.h"
#include "corn/math/compare.hpp"
//______________________________________________________________________________
const char *snow_pack_model_label_table[] =
{"none"
,LABEL_simple
,LABEL_energy
,0};
//_snow_pack_model_label_table______________________________________1999-10-04_/
const char *season_S_S_F_W_label_table[] =
{"unknown"
,"spring"
,"summer"
,"fall"
,"winter"
,"spring_or_fall"
,0};
//_season_S_S_F_W_label_table_______________________________________2004-01-15_/
const char *season_S_W_SF_label_table[] =
{"unknown"
,"summer"
,"winter"
,"spring_or_fall"
,0};
//_season_S_W_SF_label_table________________________________________2004-01-15_/
const char *wind_speed_class_label_table[] =
{LABEL_very_low
,LABEL_low
,LABEL_medium
,LABEL_medium_high
,LABEL_high
,LABEL_very_high
,LABEL_specified
,0};
//_wind_speed_class_label_table_____________________________________1997-07-23_/
namespace CS {
bool Annual_temperature::is_calibrated()                            affirmation_
{  return !
      (   CORN::is_approximately<float32>(avg,0.0,0.0000001)
        || CORN::is_approximately<float32>(avg_half_amplitude,0.0,0.0000001)
        || (phase == 0));
}
//_is_calibrated____________________________________________________2015-04-30_/
}//_namespace CS_______________________________________________________________/

