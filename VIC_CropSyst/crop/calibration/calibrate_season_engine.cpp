#include "calibrate_season_engine.h"
#include "crop/crop_param_V5.h"
#include "corn/format/binary/binary_file_fstream.h"
#include "corn/file_sys/filename.h"
#include "corn/math/compare.hpp"
//ofstream test_GDD_out("GDD.dat");
//______________________________________________________________________________
Season_calibrator::Season_calibrator(VIC_crop::Crop_parameters &_crop_parameter)
:sowing_DOY_stats()
,maturity_deg_days_stats()
,perennial_bud_or_restart_deg_days_stats()
,days_to_maturity(0)
,curr_inducement_duration(0)
,curr_accum_deg_day(0)
,in_season(false)
,found_inducible_period(false)
,crop_parameter(_crop_parameter)
,finished_season_year(0)
{}
//______________________________________________________________________________
const char *Season_calibrator::get_key() const { return crop_parameter.get_key();}
//______________________________________________________________________________
bool Season_calibrator::process(const CORN::Date &date, float64 Tmax, float64 Tmin)
{  bool processed = crop_parameter.is_perennial()
      ? process_perennial_crop (date,  Tmax,  Tmin)
      : process_annual_crop (date,  Tmax,  Tmin);
   test_accum_output(date,  Tmax,  Tmin);                                        //110908
   return   processed;
}
//______________________________________________________________________________
bool Season_calibrator::process_perennial_crop
(const CORN::Date &date, float64 Tmax, float64 Tmin)
{
   float64 Tavg = (Tmax+Tmin)/2.0;
   bool maturity_found = false;
   CORN::DOY doy = date.get_DOY();
   CORN::Year year = date.get_year();
   if ((year != finished_season_year))
   {
      if (doy == 1)
      {
         // begin accumulating deg days until day the earliest start DOY (I.e. day 90)
         // for the region (WA,ID,OR).
         curr_accum_deg_day = 0;
         test_curr_accum_deg_day = 0;                                            //110907
      } else
      if (doy == crop_parameter.season_correction.earliest_start_DOY)
      {  // earliest_start_DOY represents budbreak for trees, start of season for perennials
         perennial_bud_or_restart_deg_days_stats.append(curr_accum_deg_day);
         // Now begin accumulating deg das until maturity
         curr_accum_deg_day = 0;
         test_curr_accum_deg_day = 0;                                            //110907
         in_season = true;
         days_to_maturity = 0;
      }
      float64 deg_day_base_temp   = crop_parameter.thermal_time.base_temperature;
      float64 deg_day_cutoff_temp = crop_parameter.thermal_time.cutoff_temperature;
      float64 avg_plant_temp = Tavg;
      avg_plant_temp = (avg_plant_temp > deg_day_cutoff_temp)
               ? deg_day_cutoff_temp : avg_plant_temp;
      if (Tavg > deg_day_base_temp)
      {
         float64 growing_degree_day = avg_plant_temp - deg_day_base_temp;
         curr_accum_deg_day += growing_degree_day;
      }
      if (in_season)
      {
         if (days_to_maturity < crop_parameter.season_correction.duration)
            days_to_maturity++;
         else
         {  //We've reach the target days to maturity.
               maturity_deg_days_stats.append(curr_accum_deg_day);
               maturity_DOY_stats.append(doy);
               maturity_found = true;
               in_season = false;
               finished_season_year = year;
         }
      }
   }
   return maturity_found;
}
//______________________________________________________________________________
bool Season_calibrator::process_annual_crop
(const CORN::Date &date, float64 Tmax, float64 Tmin)
{  float64 Tavg = (Tmax+Tmin)/2.0;
   bool maturity_found = false;
   CORN::DOY doy = date.get_DOY();
   CORN::Year year = date.get_year();
   bool winter_crop = (crop_parameter.season_correction.earliest_start_DOY>182);
   if ((year != finished_season_year) || winter_crop)
   {  if (doy == 1)
      {  curr_inducement_duration = 0;
         found_inducible_period = false;
         test_curr_accum_deg_day = 0;        //110907
      } else
      {  if (in_season)
         {  window_avg_temperatures.clear();
            if (days_to_maturity < crop_parameter.season_correction.duration)
            {  float64 deg_day_base_temp   = crop_parameter.thermal_time.base_temperature;
               float64 deg_day_cutoff_temp = crop_parameter.thermal_time.cutoff_temperature;
               float64 Tavg = (Tmax + Tmin) / 2.0;
               float64 avg_plant_temp = Tavg;
               avg_plant_temp = (avg_plant_temp > deg_day_cutoff_temp)
               ? deg_day_cutoff_temp : avg_plant_temp;
               if (Tavg > deg_day_base_temp)
               {  float64 growing_degree_day = avg_plant_temp - deg_day_base_temp;
                  curr_accum_deg_day += growing_degree_day;
               }
               days_to_maturity++;
            } else
            {  //We've reach the target days to maturity.
               maturity_deg_days_stats.append(curr_accum_deg_day);
               maturity_DOY_stats.append(doy);
               maturity_found = true;
               in_season = false;
               finished_season_year = year;
            }
         } else // still looking for the start of the season.
         {  if (winter_crop)
            {  // Winter crop
               if (doy > crop_parameter.season_correction.earliest_start_DOY)
                  in_season = true;
            } else
            {  //Spring/summer crop
               window_avg_temperatures.append(Tavg);
               if (window_avg_temperatures.get_count() == 7 /*110911  crop_parameter.inactive_period.consider_inactive_days*/ )
               {  float64 mean_window_avg_temp = window_avg_temperatures.get_mean();
                  if (mean_window_avg_temp > crop_parameter.thermal_time.base_temperature)      //110911
                       found_inducible_period =  true;
                  else window_avg_temperatures.remove(0);
               }
               if (found_inducible_period && (doy >= crop_parameter
                     .season_correction.earliest_start_DOY))
               {  in_season = true;
                  sowing_doy = doy; DOY_crossed_found = false;
                   test_curr_accum_deg_day = 0; //110908
               }
            }
            if (in_season )
            {  sowing_DOY_stats.append(doy);
               days_to_maturity = 0;
               curr_accum_deg_day = 0.0;
            }
         }
      }
   }
   return maturity_found;
}
//______________________________________________________________________________
bool Season_calibrator::test_accum_output
(const CORN::Date &date, float64 Tmax, float64 Tmin)
{
   float64 Tavg = (Tmax+Tmin)/2.0;
   bool maturity_found = false;
   CORN::DOY doy = date.get_DOY();
   CORN::Year year = date.get_year();
   float64 deg_day_base_temp   = crop_parameter.thermal_time.base_temperature;
   float64 deg_day_cutoff_temp = crop_parameter.thermal_time.cutoff_temperature;
   float64 avg_plant_temp = Tavg;

   avg_plant_temp = (avg_plant_temp > deg_day_cutoff_temp)
   ? deg_day_cutoff_temp : avg_plant_temp;
   if (Tavg > deg_day_base_temp)
   {  float64 growing_degree_day = avg_plant_temp - deg_day_base_temp;
      test_curr_accum_deg_day += growing_degree_day;
   }
if ((test_curr_accum_deg_day > 2241) && !DOY_crossed_found)
{DOY_crossed_found = true;
DOY_crossed.append(doy - sowing_doy);
}

   return maturity_found;
}
//______________________________________________________________________________
float64 Season_calibrator
::get_phenology_adjustment_given_historical_maturity_deg_days(float64 historical_maturity_deg_days)
{
   float64 adjust = maturity_deg_days_stats.get_mean() / historical_maturity_deg_days;
//110902    if (adjust > 1.0) adjust = 1.0;                                               //110519  We decided to only adjust to shorten the season
   return adjust;
}
//______________________________________________________________________________
float64 Season_calibrator::get_phenology_adjustment()
{
   return  get_phenology_adjustment_given_historical_maturity_deg_days(crop_parameter.phenology.begin_maturity_deg_day);
}
//______________________________________________________________________________
float64 Season_calibrator::get_start_season_adjustment()
{
   float64 restart_degday_mean =  perennial_bud_or_restart_deg_days_stats.get_mean();
   float64 param_restart_degday = crop_parameter.phenology.emergence_deg_day;
   bool perennial =  crop_parameter.is_perennial();
   return (perennial && (param_restart_degday > 0.00001))
      ? restart_degday_mean / param_restart_degday
      : 1.0 ;

}
//_2011-03-14___________________________________________________________________
float32 Season_calibrations::Calibration::set_adjustments(
float64 at_latitude,float64 at_longitude
,float32 new_phenology_adjustment
,float32 new_season_start_adjustment)
{  FIND_FOR_EACH_IN(found_adjustment,adjustment,Season_calibrations::Calibration::Adjustment,adjustments,true,each_adj)
   {  if (CORN::is_approximately(adjustment->latitude,at_latitude,0.00001) && CORN::is_approximately(adjustment->longitude,at_longitude,0.00001))
         found_adjustment = adjustment;
   } FOR_EACH_END(each_adj);
   if (!found_adjustment)
   {  found_adjustment = new Season_calibrations::Calibration::Adjustment;
      found_adjustment->latitude  = at_latitude;
      found_adjustment->longitude = at_longitude;
      adjustments.append(found_adjustment);
   }
   found_adjustment->phenology_adjustment = new_phenology_adjustment;
   found_adjustment->season_start_adjustment = new_season_start_adjustment;
   return new_phenology_adjustment;
}
//_2011-03-14___________________________________________________________________
float32 Season_calibrations::set_adjustments
(const char *crop_ID
,float64 at_latitude,float64 at_longitude
,float32 phenology_adjustment
,float32 season_start_adjustment)
{  Calibration *calibration = dynamic_cast<Calibration *>(find(crop_ID));
   if (!calibration)
   {  calibration = new Calibration(crop_ID);
      append(calibration);
   }
   calibration->set_adjustments
      (at_latitude,at_longitude
      ,phenology_adjustment
      ,season_start_adjustment
      );
   return phenology_adjustment;
}
//______________________________________________________________________________
Season_calibrations::Season_calibrations(const CORN::Directory_name &season_calibrations_directory_name)
: calibration_directory_name(season_calibrations_directory_name)
{}
Season_calibrations::Season_calibrations(const char *season_calibrations_directory_name)
: calibration_directory_name(season_calibrations_directory_name)
{}
//______________________________________________________________________________
nat32 Season_calibrations::read_calibration_files()
{  nat32 files_read = 0;
   CORN::Association_list calibration_files;
   calibration_directory_name.list_contents_by_type
      (calibration_files
      ,false // not recursive
      ,true // quality
      ,true // with ext
      ,true // show path
      ,true //show extension
      ,CORN::Directory_entry_name::file_entry
      ,"CS_crop");
   FOR_EACH_IN(calibration_filename,CORN::File_name,calibration_files,each_calib_file)
   {  Calibration *calibration = new Calibration(calibration_filename->get_name(false));
      calibration->read(*calibration_filename);
      append(calibration);
      files_read ++;
   } FOR_EACH_END(each_calib_file);
   return files_read; //NYI
}
//______________________________________________________________________________
nat32 Season_calibrations::write_calibration_files()
{  nat32 written = 0;
   std::cout << "Writing crop calibrations" << std::endl;
   FOR_EACH(calibration,Calibration,each_calib)
   {  calibration->write(calibration_directory_name);
      std::cout << calibration->get_key() << std::endl;
      written++;
   } FOR_EACH_END(each_calib);
   return written;
}
//______________________________________________________________________________
bool Season_calibrations::Calibration::Adjustment::read(CORN::Binary_file_fstream &binfstream)
{  latitude  = binfstream.read_float64();
   longitude = binfstream.read_float64();
   phenology_adjustment = binfstream.read_float32();
   season_start_adjustment = binfstream.read_float32();
   return true;
}
//______________________________________________________________________________
nat32 Season_calibrations::Calibration::read(const CORN::File_name &calibration_filename)
{  CORN::Binary_file_fstream calib_file(calibration_filename.c_str(),false);
   bool at_eof = !calib_file.good();
   int cell_count = 0;
   while (!at_eof)
   {  Season_calibrations::Calibration::Adjustment *adjustment = new Season_calibrations::Calibration::Adjustment;
      adjustment->read(calib_file);
      adjustments.append(adjustment);
   }
   return 0; // NYI
}
//______________________________________________________________________________
nat32 Season_calibrations::Calibration::write(const CORN::Directory_name &calibration_directory_name)
{  nat32 written = 0;
   CORN::File_name crop_adjustment_file_name(crop_ID.c_str(),VIC_crop_adjust_ext);
   crop_adjustment_file_name.set_path(calibration_directory_name);
   std::ofstream calibration_file(crop_adjustment_file_name.c_str(),std::ofstream::binary);
   adjustments.sort();
   FOR_EACH_IN(adjustment,Season_calibrations::Calibration::Adjustment,adjustments,each_adj)
   {  calibration_file.write((char *)(&adjustment->latitude),sizeof(float64));
      calibration_file.write((char *)(&adjustment->longitude),sizeof(float64));
      calibration_file.write((char *)(&adjustment->phenology_adjustment),sizeof(float32));
      calibration_file.write((char *)(&adjustment->season_start_adjustment),sizeof(float32)); //110314
      written ++;
   } FOR_EACH_END(each_adj);
   return written;
}
//______________________________________________________________________________

