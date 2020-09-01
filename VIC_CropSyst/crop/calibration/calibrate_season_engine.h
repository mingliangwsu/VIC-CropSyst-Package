#ifndef calibrate_season_engineH
#define calibrate_season_engineH
//______________________________________________________________________________
#include "corn/math/statistical/stats.h"
#include "corn/primitive.h"
#include "corn/container/association.h"
#include "corn/file_sys/smart_dirname.h"
//170628LML #include "corn/datetime/date.hpp"
#define VIC_crop_adjust_ext "VIC_crop_adjust"
#include "VIC_crop_param.h"
namespace CORN
{
   class Binary_file_fstream;
};
//______________________________________________________________________________
class Season_calibrator
: public CORN::Association
{
   nat16 curr_inducement_duration;
   nat16 days_to_maturity;
   float64 curr_accum_deg_day;
   float64 test_curr_accum_deg_day;   //110908
   bool in_season;
   CORN::Year finished_season_year;
   bool found_inducible_period;
   VIC_crop::Crop_parameters &crop_parameter; // owned by VIC_Season_calibrator

// the following 2 are for testing only
CORN::Statistical_running_tally DOY_crossed; bool DOY_crossed_found;
nat16 sowing_doy;

public:
   CORN::Statistical_running_tally sowing_DOY_stats;
   CORN::Statistical_running_tally maturity_deg_days_stats;
   CORN::Statistical_running_tally maturity_DOY_stats;
   CORN::Statistical_running_tally perennial_bud_or_restart_deg_days_stats;
   CORN::Statistical_data_set window_avg_temperatures;
public:
   Season_calibrator(VIC_crop::Crop_parameters &_crop_parameters);
   bool process(const CORN::Date &, float64 Tmax, float64 Tmin);
   float64 get_phenology_adjustment_given_historical_maturity_deg_days(float64 historical_maturity_deg_days);
//   float64 get_phenology_adjustment_given_typical_days_to_maturity(nat16 typical_days_to_maturity);
   float64 get_phenology_adjustment();
   float64 get_start_season_adjustment();
   virtual const char *get_key() const;
 private:
   bool process_perennial_crop(const CORN::Date &date, float64 Tmax, float64 Tmin);
   bool process_annual_crop   (const CORN::Date &date, float64 Tmax, float64 Tmin);
   bool test_accum_output(const CORN::Date &date, float64 Tmax, float64 Tmin);
};
//______________________________________________________________________________
class Season_calibrations
: public CORN::Association_list               //List of Calibration (one for each crop)
{  class Calibration
   : public CORN::Association
   {  class Adjustment
      : public CORN::Item
      {public:
         float64 latitude;
         float64 longitude;
         float32 phenology_adjustment;
         float32 season_start_adjustment;
       public:
         inline virtual int   compare(const CORN::Item &other)             const
         {  const Adjustment &other_adj = dynamic_cast<const Adjustment &>(other);
            return (latitude > other_adj.latitude)       ?  1
                 : (latitude < other_adj.latitude)       ? -1
                 : (longitude > other_adj.longitude)     ?  1
                 : (longitude < other_adj.longitude)     ? -1 : 0;
         };
         bool read(CORN::Binary_file_fstream &binfstream);
      };
      //________________________________________________________________________
      std::string crop_ID;
      CORN::Bidirectional_list adjustments;
      // VIC cell numbers are arbitrary (necessarily count from 0 or 1)
      // so cell_number array is used to index the cell adjustments.
      // The index of the cell_number in the cell_number array is
      // the index of the cell in the adjustment array.
   public:
      inline Calibration(const std::string &_crop_ID)
         : CORN::Association(),crop_ID(_crop_ID)
         {};
      inline const char *get_key() const { return crop_ID.c_str(); };
      float32 set_adjustments(float64 latitude,float64 longitude
         ,float32 phenology_adjustment,float32 start_adjustment);
      float32 get_phenology_adjustment(float64 latitude,float64 longitude);
      float32 get_start_season_adjustment(float64 latitude,float64 longitude);   //110314
      nat32 read(const CORN::File_name &calibration_filename);
      nat32 write(const CORN::Directory_name &calibration_directory_name);
   };
   //___________________________________________________________________________
   const CORN::Smart_directory_name calibration_directory_name;
public:
   Season_calibrations(const CORN::Directory_name &season_calibrations_directory_name);
   Season_calibrations(const char *season_calibrations_directory_name);
   float32 set_adjustments
      (const char *crop_ID
      ,float64 latitude,float64 longitude
      ,float32 phenology_adjustment
      ,float32 start_season_adjustment);
   float32 get_phenology_adjustment
      (const char *crop_ID,float64 latitude,float64 longitude);
   float32 get_start_season_adjustment(const char *crop_ID,float64 latitude, float64 longitude); //110314
   nat32 read_calibration_files();
   nat32 write_calibration_files();
};
//______________________________________________________________________________
#endif

