#include "temperature_amplitude_phase_calibrator.h"
#include "UED/library/varcodes.h"
#include "UED/library/timeqry.h"
#include "corn/chronometry/date_32.h"
#include <assert.h>
namespace CS {
//______________________________________________________________________________
Temperature_amplitude_and_phase_calibrator
::Temperature_amplitude_and_phase_calibrator()
{}
//______________________________________________________________________________
bool Temperature_amplitude_and_phase_calibrator::submit_temperature_max_min
(CORN::DOY  doy
,float32    Tmax
,float32    Tmin)
{  float32 Tavg = (Tmax + Tmin)/2.0;
   submit_temperature_avg(doy,Tavg);
   return true;
}
//_submit_temperature_max_min__________________________________________________/
bool Temperature_amplitude_and_phase_calibrator::submit_temperature_avg
(CORN::DOY  doy
,float32    Tavg)
{  average_temperature.append(Tavg);
   daily_temperature[doy].append(Tavg);
   return true;
}
//_submit_temperature_avg______________________________________________________/
bool Temperature_amplitude_and_phase_calibrator::load_daily_temperature_from_UED
(const UED::Database_file_indexed &database_const)
{  bool loaded = true;
   UED::Variable_code_clad UED_STD_VC_max_temperature          (UED_climate_air_temperature,       UED_maximum_for_time_step,false);
   UED::Variable_code_clad UED_STD_VC_min_temperature          (UED_climate_air_temperature,       UED_minimum_for_time_step,false);
   UED::Database_file_indexed &database
      = const_cast<UED::Database_file_indexed &>(database_const);                //150911
   datetime64 earliest_Tmax_datetime   =  database.get_earliest_date_time  ( UED_FOR_ALL_RECORD_TYPES, UED_STD_VC_max_temperature.get());
   datetime64 earliest_Tmin_datetime   =  database.get_earliest_date_time  ( UED_FOR_ALL_RECORD_TYPES, UED_STD_VC_min_temperature.get());
   datetime64 latest_Tmax_datetime     =  database.get_latest_date_time    ( UED_FOR_ALL_RECORD_TYPES, UED_STD_VC_max_temperature.get());
   datetime64 latest_Tmin_datetime     =  database.get_latest_date_time    ( UED_FOR_ALL_RECORD_TYPES, UED_STD_VC_min_temperature.get());
   datetime64 earliest_datetime = std::max<float64>(earliest_Tmax_datetime,earliest_Tmin_datetime);
   datetime64 latest_datetime   = std::min<float64>(latest_Tmax_datetime ,latest_Tmin_datetime  );
   CORN::Date_clad_32 earliest_date   (earliest_datetime);
   CORN::Date_clad_32 latest_date     (latest_datetime);
   for (CORN::Date_clad_32 today(earliest_date)
     ;today <= latest_date
     ;today.inc_day())
   {  UED::Time_query_single_date time_qry
         (today.get_datetime64(),(UED_units_code)UT_day);                        //030714
      UED_units_code actual_units = UC_Celcius;
      CORN::Quality_clad attribute;
      float32 Tmax = database.get_no_create
         (UED::Record_type_distinguisher_interface::UED_data_set_year_var_units
         ,UED_STD_VC_max_temperature.get(),time_qry,actual_units,attribute);
      float32 Tmin = database.get_no_create
         (UED::Record_type_distinguisher_interface::UED_data_set_year_var_units
         ,UED_STD_VC_min_temperature.get(),time_qry,actual_units,attribute);
      loaded &= submit_temperature_max_min(today.get_DOY(),Tmax,Tmin);
   }
   return loaded;
}
//_load_daily_temperature_from_UED_____________________________________________/
bool Temperature_amplitude_and_phase_calibrator::load_daily_temperature_from_UED_cstr
(const char *UED_weather_filename_raw)
{  UED::Database_file_indexed database (UED_weather_filename_raw,std::ios::in,true);
   database.initialize();
   return load_daily_temperature_from_UED(database);

/* NYI

   const Geolocation  * geolocation =weather_DB.get_geolocation_constant();
   float64 longitude_radians =geolocation ?  geolocation->get_longitude_radians() : 0.0;
   float64 standard_meridian_radians =longitude_radians;
   // 071119 currently just using longitude for the standard meridian because
   // we don't have this value in geolocation
   Sun_days sun_days
      (longitude_radians
      ,standard_meridian_radians);
   Weather_day weather
      (&weather_DB
      ,sun_days
      ,true );          // _read_only
      // Don't care about the remaining options
*/
}
//_load_daily_temperature_from_UED_cstr_____________________________2019-06-26_/
bool Temperature_amplitude_and_phase_calibrator
::load_daily_temperature_from_UED_filename
(const CORN::OS::File_name &UED_weather_filename)
{  return load_daily_temperature_from_UED_cstr(UED_weather_filename.c_str());
}
//_load_daily_temperature_from_UED_filename____________________________________/
bool Temperature_amplitude_and_phase_calibrator::calibrate
(float32 &annual_temperature_avg
,int16   &calibrated_phase
,float32 &calibrated_half_applitude)
{  bool calibrated = true;
   if (average_temperature.get_count())
   {
   // Find where there is 7 days average window of peak temperature

   CORN::DOY found_maximum_doy = 0;
   float32   found_maximum_temp = -999;

   for (CORN::DOY doy = 1; doy < 365; doy++)
   {  // I use a window because the daily average temperature fluctuates
      int16 window_start_doy = doy - 3;
      int16 window_end_doy   = doy + 3;
      CORN::statistical::Sample_cumulative<float32,float32> window;
      for (int16 window_doy = window_start_doy; window_doy <= window_end_doy; window_doy++)
      {
         CORN::DOY actual_doy
            = (window_doy < 0)   ? 365 + window_doy
             :(window_doy > 365) ? window_doy - 365
             : window_doy;
         float32 temp_on_doy =  daily_temperature[actual_doy].get_mean();
            window.append(temp_on_doy);
      }
      float32 window_average_temp = window.get_mean();
      if (window_average_temp > found_maximum_temp)
      {
         found_maximum_temp = window_average_temp;
         found_maximum_doy = doy;
      }
   }
   found_maximum_temp = daily_temperature[found_maximum_doy].get_mean();
   annual_temperature_avg = average_temperature.get_mean();
   // First find the phase.
   // This will be where the DOY of the peak of the estimate cover
   // is the same as the found_maximum_DOY
   CORN::DOY found_phase = 0;
   float32 found_peak_estimate_DOY  = 0;
   float32 arbitrary_avg_half_amplitude = 10; // For this step use some arbitrary half_a
   for (CORN::DOY phase = 1; (phase < 365) && !found_phase; phase++)
   {
      float32 found_peak_estimate      = -999.9;
      found_peak_estimate_DOY  = 0;
      for (int doy = 1; doy <365; doy++)
      {
         float32 estimate = annual_temperature_avg
         + arbitrary_avg_half_amplitude * sin(2.0*CORN_pi/365.0 * (doy-phase));
         if (estimate > found_peak_estimate)
         {  found_peak_estimate = estimate;
            found_peak_estimate_DOY = doy;
         }
      }
      if (found_peak_estimate_DOY == found_maximum_doy)
         found_phase = phase;
   }
   calibrated_phase = found_phase;
   // Now find avg half amplitude
   float32 found_avg_half_amplitude = -999.9;
   float32 found_closest_estimate_diff = 999.9;
   for (float32 avg_half_amplitude = 10.0
       ; avg_half_amplitude < 20.0; avg_half_amplitude += 0.1)
   {  // At this point we only need to estimate the calibrated phase

      float32 estimate = annual_temperature_avg + avg_half_amplitude
         * sin(2.0*CORN_pi/365.0*(found_peak_estimate_DOY - calibrated_phase));
      float32 estimate_diff = fabs(found_maximum_temp - estimate);
      if (/*(estimate_diff > 0.0) &&*/ (estimate_diff < found_closest_estimate_diff))
      {
         found_closest_estimate_diff = estimate_diff;
         found_avg_half_amplitude = avg_half_amplitude;
      }
   }
   calibrated_half_applitude = found_avg_half_amplitude;
   } else
   {
/*
      provide_explainations_global().take
         (new Explaination
            (severity_warning
            ,"%PROG_DOC%/explainations/annual_temperature.html"
            ,""
            ,""
            ));
*/
      std::cerr << "Unable to calculate average annual temperature. UED file missing or incomplete." << std::endl;
   }
   return calibrated;
}
//_calibrate___________________________________________________________________/
float32 Temperature_amplitude_and_phase_calibrator::get_july_temperature_avg() const
{  CORN::statistical::Sample_cumulative <float32,float32>july_temperatures_daily_avg;
   for (CORN::DOY july_DOY = 182; july_DOY <=212 ; july_DOY++)
      july_temperatures_daily_avg.append(daily_temperature[july_DOY].get_mean());
   return  july_temperatures_daily_avg.get_mean();
}
//_get_july_temperature_avg____________________________________________________/
}//_namespace CS_______________________________________________________________/

