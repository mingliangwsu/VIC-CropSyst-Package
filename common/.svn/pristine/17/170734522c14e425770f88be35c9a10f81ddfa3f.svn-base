#ifndef temperature_amplitude_phase_calibratorH
#define temperature_amplitude_phase_calibratorH
#include "corn/math/statistical/sample_cumulative_T.h"
#include "corn/OS/directory_entry_name.h"
#include "UED/library/database_file.h"
namespace CS {
//______________________________________________________________________________
class Temperature_amplitude_and_phase_calibrator
{
 public:
   CORN::statistical::Sample_cumulative <float32,float32>daily_temperature[367];
   CORN::statistical::Sample_cumulative <float32,float32>average_temperature;
      // Average of all years all days is the same as averaging the
      // value for all days for each year then averaging this for all years.
 public:
   Temperature_amplitude_and_phase_calibrator();
   bool load_daily_temperature_from_UED_filename(const CORN::OS::File_name &UED_weather_filename);
   bool load_daily_temperature_from_UED_cstr(const char *UED_weather_filename);
   bool load_daily_temperature_from_UED(const UED::Database_file_indexed &database);
   bool calibrate(float32 &annual_temperature_avg, int16 &phase, float32 &half_applitude);
   float32 get_july_temperature_avg()                                     const; //150202
      // get_july_temperature_avg added for VIC soil file generation
 protected:
   bool submit_temperature_avg(CORN::DOY  doy,float32    Tavg);
   bool submit_temperature_max_min(CORN::DOY doy,float32 Tmax,float32 Tmin);
};
//_Temperature_amplitude_and_phase_calibrator__________________________________/
}//_namespace CS_______________________________________________________________/
#endif

