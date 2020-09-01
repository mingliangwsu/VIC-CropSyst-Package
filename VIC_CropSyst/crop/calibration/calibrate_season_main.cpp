#include "calibrate_season_engine.h"
//170628LML #include "corn/datetime/date.h"
#include "corn/file_sys/smart_dirname.h"
#include "VIC_crop_param.h"

#include "corn/data_source/vv_file.h"
#include "corn/format/binary/binary_file_fstream.h"
#include "corn/container/text_list.h"
#ifdef _Windows
#include "UED/convert/VIC/VIC_forcing_reader.h"
#else
#include "VIC_forcing_reader.h"
#endif
#include <assert.h>
//______________________________________________________________________________
class VIC_Season_calibrator
: public Season_calibrations
{
   nat32 cell_number_current;

   CORN::Association_list crops_parameters;   //list of names crop parameters
   CORN::Association_list crop_season_calibrators; //list of Season_calibrator

   const CORN::Directory_name &season_calibration_directory;
   const CORN::Smart_directory_name &crop_parameter_file_directory;
public:
   VIC_Season_calibrator
      (const CORN::Smart_directory_name &_crop_parameter_file_directory
      ,const CORN::Directory_name &_season_calibration_directory)
   : Season_calibrations(_season_calibration_directory)
   , cell_number_current(0)
   , crop_season_calibrators()
   , crop_parameter_file_directory  (_crop_parameter_file_directory)
   , season_calibration_directory   (_season_calibration_directory)
   {
      CORN::Association_list crop_file_names;

      crop_parameter_file_directory.list_contents_by_type
         (crop_file_names
         ,false // not recursive
         ,true //qualify
         ,true //with_extension
         ,true //show_path
         ,true //i_show_extension
         ,CORN::Directory_entry_name::file_entry
         ,"CS_crop");

      FOR_EACH_IN(crop_file_name,CORN::File_name,crop_file_names,each_crop_file)
      {
         // open the file,
         //
         VIC_crop::Crop_parameters *params = new VIC_crop::Crop_parameters;
         CORN::VV_File crop_file(crop_file_name->c_str());
         crop_file.get(*params);
         crops_parameters.append(params);
         params->description.assign(crop_file_name->get_name(false));
            // temporarily using the description field to index the crop.
      } FOR_EACH_END(each_crop_file);
   };

   bool start_cell(float64 latitude,float64 longitude,const CORN::Text_list &crops_in_this_cell,CORN::Text_list &missing_crops)
   {
      crop_season_calibrators.delete_all();
      FOR_EACH_IN(crop_name, CORN::Text_list::Item, crops_in_this_cell, each_crop)
      //      FOR_EACH_IN(crop_params,VIC_crop::Crop_parameters,crops_parameters,each_crop)
      {
         VIC_crop::Crop_parameters *crop_params = dynamic_cast<VIC_crop::Crop_parameters *>(crops_parameters.find(crop_name->c_str()));
         if (crop_params)
         {  crop_params->setup_season_correction(latitude,longitude);
            Season_calibrator *crop_season_calibrator = new Season_calibrator(*crop_params);
            crop_season_calibrators.append(crop_season_calibrator);
         }
         else
         {  missing_crops.add(crop_name->c_str(),true);
            // std::cout << "Unable to find crop:" << (*crop_name) << std::endl;
         };
      } FOR_EACH_END(each_crop);
      return true;
   };
   bool process(const CORN::Date &date, float32 Tmax, float32 Tmin)
   {
      FOR_EACH_IN(crop_season_calibrator, Season_calibrator,crop_season_calibrators,each_calibrator)
      {
         crop_season_calibrator->process(date,Tmax,Tmin);
      } FOR_EACH_END(each_calibrator);
      return true;
   };
   bool commit_cell(float64 latitude, float64 longitude)
   {
      FOR_EACH_IN(crop_season_calibrator, Season_calibrator,crop_season_calibrators,each_calibrator)
      {
         float64 phenology_adjustment = crop_season_calibrator->get_phenology_adjustment();
         float64 start_season_adjustment = crop_season_calibrator->get_start_season_adjustment();
         /*110314
         set_phenology_adjustment(crop_season_calibrator->get_key(),latitude,longitude, phenology_adjustment);
         set_start_season_adjustment(crop_season_calibrator->get_key(),latitude,longitude, start_season_adjustment);
         */
         set_adjustments(crop_season_calibrator->get_key(),latitude,longitude, phenology_adjustment,start_season_adjustment);
      } FOR_EACH_END(each_calibrator)
      return true;
   };
};
//______________________________________________________________________________
int calibrate_given_VIC_forcing_file_directory
(const CORN::Smart_directory_name &VIC_foring_directory
,const CORN::Smart_directory_name &crop_parameter_file_directory
,const CORN::Smart_directory_name &season_calibration_directory
,const CORN::Smart_file_name      &cell_crop_fraction_filename
,CORN::Year earliest_filter_year
,CORN::Year latest_filter_year)
{
   CORN::Year forcing_file_earliest_year = 1915;

   VIC_Season_calibrator season_calibrator(crop_parameter_file_directory,season_calibration_directory);
   std::ifstream cell_crop_fraction_file(cell_crop_fraction_filename.c_str());
   std::string header_crop_names_str;
   std::getline(cell_crop_fraction_file,header_crop_names_str);
   //std::cout << header_crop_names_str << std::endl;
   CORN::Text_list header_crop_names(header_crop_names_str.c_str(),'\t');
   delete header_crop_names.pop_at_head(); // Cellno
   delete header_crop_names.pop_at_head(); // Lat
   delete header_crop_names.pop_at_head(); // Long
   CORN::Text_list missing_crops;
   while (!cell_crop_fraction_file.eof())
   {  nat32 cell_number;
      float64 latitude;
      float64 longitude;
      cell_crop_fraction_file >> cell_number >> latitude >> longitude;
      CORN::Text_list crops_in_this_cell;

      FOR_EACH_IN(crop_name,CORN::Text_list::Item,header_crop_names,each_crop)
      {  if (crop_name->length())
         {
            float32 fraction;
            cell_crop_fraction_file >> fraction;
            if (fraction > 0.01)
            {
               //std::cout << (*crop_name) << '\t';
               crops_in_this_cell.add(crop_name->c_str());
            };
         };
      } FOR_EACH_END(each_crop);

      if (crops_in_this_cell.count() > 0)
      {
//         char latitude_cstr[20];
//         CORN_float64_to_str_formated(latitude, latitude_cstr,20, 5,'.',false);
//         char longitude_cstr[20];
//         CORN_float64_to_str_formated(longitude, longitude_cstr,20, 5,'.',false);
         std::string forcing_name("data_");
         forcing_name.append(CORN_float64_to_str(latitude,5));
         forcing_name.append("_");
         forcing_name.append(CORN_float64_to_str(longitude,5));
         #ifdef _Windows
         std::cout << forcing_name;
         #endif
         CORN::Smart_file_name forcing_filename(forcing_name);
         forcing_filename.set_path(VIC_foring_directory);

         if (forcing_filename.exists())
         {
            CORN::Binary_file_fstream forcing_file(forcing_filename.c_str(),false);

            if (forcing_file.good())
            {
               CORN::Date record_date(forcing_file_earliest_year,(CORN::DOY)1);
               float32 precip;
               float32 tmax;
               float32 tmin;
               float32 wind;
               //bool in_time_range = true;
               season_calibrator.start_cell(latitude,longitude,crops_in_this_cell,missing_crops);
               #ifdef _Windows
               std::cout <<"\r                    " << forcing_name << ' ';
               #endif
//               while (in_time_range && !forcing_file.at_eof())
               while (!forcing_file.at_eof())
               {
                  if (read_VIC_forcing_file_line(forcing_file,record_date,earliest_filter_year,latest_filter_year,precip,tmax,tmin,wind))
                  {
                     bool in_time_range  = in_time_range
                     =  (record_date.get_year() >= earliest_filter_year)
                     && (record_date.get_year() <= latest_filter_year);
                     if (in_time_range)
                        season_calibrator.process(record_date,tmax,tmin);

                  #ifdef _Windows
                  std::cout << '\r' << record_date.get_date32()/* << '\t' << tmax<< '\t' <<tmin*/;
                  #endif
                  };
                  record_date.inc();
               }; // while
               bool committed = season_calibrator.commit_cell(latitude,longitude/*cell_number*/);
               if (!committed)
                  std::cout << forcing_name << " not calibrated";
            } else
            {
               std::cout << "\r file not good:" << forcing_filename << std::endl;
            };
         } else
         {
            std::cout << forcing_name << " not found" << std::endl;
         };
      } else
      {
         std::cout << "\r No crops in cell:" << latitude << " " << longitude << std::endl;
      };
   };
   nat32 calibrations_written = season_calibrator.write_calibration_files();
   std::cout << "Calibrations written: " << calibrations_written << std::endl;
   if (missing_crops.count())
   {
      std::cout << "Missing crops:\n";
      missing_crops.write(std::cout);
   };
   return EXIT_SUCCESS;
};
//______________________________________________________________________________
#ifdef _Windows
#include <weather/database/weather_UED_database.h>
void calibration_test(const char *weather_UED_filename, const char *crop_param_filename)
{
   Weather_UED_database weather_database(weather_UED_filename,true,CORN::measured_quality);
   VIC_crop::Crop_parameters crop_parameters;
   VV_File crop_file(crop_param_filename);
   crop_file.get(crop_parameters);
   Season_calibrator season_calibrator(crop_parameters);
   for (CORN::Date date((CORN::Year)1978 ,(CORN::DOY)1);date.get() <= 2008365;date.inc())
   {
      bool maturity_found = false;
      std::cout
         << ((int)(date.get_date32()) / 1000)
         << ' ' << ((int)(date.get_date32()) % 1000) << '\t';
      CORN::Quality tmax_quality;
      CORN::Quality tmin_quality;
      float64 Tmax = weather_database.get_max_temperature(tmax_quality, date);
      float64 Tmin = weather_database.get_min_temperature(tmin_quality, date);

      maturity_found = season_calibrator.process(date,Tmax,Tmin);

      if (maturity_found)
         std::cout
         << " sowing DOY :\t"       << season_calibrator.sowing_DOY_stats.get_current()
         << " maturity degdays:\t" << season_calibrator.maturity_deg_days_stats.get_current()
         //110126 obs << " days to_maturity :\t" << season_calibrator.days_to_maturity_stats.get_current()
         << std::endl;
      std::cout << '\r';
   };
   std::cout
         << std::endl
         << " sowing DOY historical earliest (param):\t" <<
            //crop_parameters.phenology.VIC_earliest_sowing_DOY_historical
            crop_parameters.season_correction.earliest_start_DOY
            << std::endl
         << " sowing DOY mean:\t"            << season_calibrator.sowing_DOY_stats.get_mean()  << std::endl
         << " maturity DOY mean:\t"          << season_calibrator.maturity_DOY_stats.get_mean()  << std::endl
         << " maturity degdays mean:\t"      << season_calibrator.maturity_deg_days_stats.get_mean()  << std::endl
         << " maturity degdays stddev:\t"    << season_calibrator.maturity_deg_days_stats.get_standard_deviation()  << std::endl
         << " maturity degdays ceofvarv:\t"  << season_calibrator.maturity_deg_days_stats.get_coefficient_of_variation()  << std::endl

         << " maturity degdays historical (param):\t"  << crop_parameters.phenology.begin_maturity_deg_day<< std::endl
         //110126 << " days to_maturity mean:\t" << season_calibrator.days_to_maturity_stats.get_mean()  << std::endl
         << " days to_maturity historical (param):\t"  <<
            //crop_parameters.phenology.VIC_typical_days_to_maturity_historical
            crop_parameters.season_correction.earliest_start_DOY
            << std::endl
         << " phenology adj:\t"           << season_calibrator.get_phenology_adjustment()    << std::endl
         << " budbreak or emerge adj:\t"  << season_calibrator.get_start_season_adjustment() << std::endl
         << std::endl;
};
#endif
//______________________________________________________________________________
//#define QUICK_TEST
int main(int argc, char *argv[])
{
#ifdef QUICK_TEST
   calibration_test
      ("C:\\Projects\\VIC\\validation\\crop\\324885.UED"
      //,"G:\\Simulation\\Database\\weather\\completed\\300122 - WSU Othello.UED"
      ,"C:\\Projects\\VIC\\database\\crop\\name\\potato.CS_crop"
    //  ,"J:\\CFF_longrun\\Database\\Crop\\SC.crp"
    //,"J:\\CFF_longrun\\Database\\Crop\\WW.crop"
      );
#else
   if (argc == 0)
   {
      std::cerr << "This program can take three arguments:" << std::endl;
      std::cerr << "1. path name of the directory containing VIC forcing files." << std::endl;
      std::cerr << "2. path name of the directory containing CropSyst parameter files." << std::endl;
      std::cerr << "3. path name of the output directory to store the generated season adjustment files."  << std::endl;
      std::cerr << "4. fully qualified filename of cell crop fractions."  << std::endl;
      std::cerr << "The current working directory is assumed as the default." << std::endl;
   };
   #ifdef _Windows
   CORN::Smart_directory_name VIC_forcing_directory         ("C:\\Projects\\VIC\\database\\weather");
   CORN::Smart_directory_name crop_parameter_file_directory ("C:\\Projects\\VIC\\database\\crop\\WSDA");
   CORN::Smart_directory_name season_calibration_directory  ("C:\\Projects\\VIC\\database\\crop\\WSDA");
   CORN::Smart_file_name cell_crop_fraction_filename        ("C:\\Projects\\VIC\\database\\crop\\WSDA\\VegPercentages.txt");
   #else
   CORN::Smart_directory_name VIC_forcing_directory         ("/media/RAID/Data/Kiran/Data/VIC/VICUWData/data/vic_inputdata0625_pnw_combined_05142008");
   CORN::Smart_directory_name crop_parameter_file_directory ("/media/RAID/Data/Kiran/Data/VIC/crop/WSDA");
   CORN::Smart_directory_name season_calibration_directory  ("/media/RAID/Data/Kiran/Data/VIC/crop/WSDA");
   CORN::Smart_file_name cell_crop_fraction_filename        ("/media/RAID/Data/Kiran/Data/crop_selection_USDA_WSDA/VegPercentages.txt");
   #endif
   if (argc >= 2) VIC_forcing_directory         .set(argv[1]);
   if (argc >= 3) crop_parameter_file_directory .set(argv[2]);
   if (argc >= 4) season_calibration_directory  .set(argv[3]);
   if (argc >= 5) cell_crop_fraction_filename   .set(argv[4]);
   return calibrate_given_VIC_forcing_file_directory
      (VIC_forcing_directory,crop_parameter_file_directory,season_calibration_directory
      ,cell_crop_fraction_filename
      ,1970,2006
     // ,1975,2008
      );
#endif
   //obsolete return calibrate_given_weather(crop_parameter_file_directory,season_calibration_directory);
};
//______________________________________________________________________________

