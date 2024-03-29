#include "CropSyst_engine_V5.h"
#include "csvc.h"
#include "common/simulation/log.h"
#include "UED/library/locrec.h"
#include "common/weather/storms.h"
#include "common/weather/temperature_amplitude_phase_calibrator.h"
#include "corn/application/explaination/explainations.h"
#include "common/weather/atmospheric_CO2_change.h"
#include "CS_suite/observation/CS_optation.h"
//______________________________________________________________________________
namespace CropSyst {
namespace version5 {
//______________________________________________________________________________
Simulation_engine::Simulation_engine
(CS::Arguments &arguments_                                                       //200817
//200817 Command_options_data_source &command_line_options_
)                             //160908
: CS::Cycler
   (arguments_
   //200817    command_line_options_
   ,ID,false)                                              //160513_160908
   // Warning need to provide CPF_function option                                  160515
,model_name                         ("CropSyst_V5")
,output_dir                         (0)
, control_filename_extension        (L"CS_control")                              //160308
, scenario_filename_extension       (L"CropSyst_scenario")                       //160219
, template_rotation_file_name       (0) // deprecated                            //151010
, ID                                (0)
//, arguments                         (ID)                                         //160908_160312
, arguments                         (arguments_)                                 //200817
, scenario_directory                (arguments_.start_directory,false)           //200817
, weather_daily                     (0)                                          //160525
, stores                            (0)
{}
//_Simulation_engine:constructor____________________________________2015-10-08_/
Simulation_engine::~Simulation_engine()
{  delete output_dir;
   delete weather_daily;
}
//_Simulation_engine:destructor_____________________________________2015-10-08_/
const CORN::OS::Directory_name &Simulation_engine
::get_output_directory_name()                                              const
{  if (!output_dir)
      output_dir = new CORN::OS::Directory_entry_name_concrete
         (arguments.start_directory                                              //200525
            //050525 .provide_scenario_directory()
         ,"Output");
   return *output_dir;
}
//_get_output_directory_name________________________________________2015-10-08_/
const CORN::OS::Directory_name &Simulation_engine::provide_scenario_directory_name() provision_
{ return arguments.start_directory;                                              //200525
   //200525 provide__scenario_directory();
}
// This could be moved to CS_simulation_engine
//_provide_scenario_directory_name__________________________________2015-10-08_/
const CORN::OS::File_name &Simulation_engine::provide_template_rotation_file_name() provision_
{  return template_scenario.rotation_filename;                                   //160529
}
//_provide_template_rotation_file_name______________________________2015-10-11_/
bool Simulation_engine::initialize()                             initialization_
{
   bool initialized = true;
   // Now that we know the scenario directory we can purge output

   /*200817 I dont think this actually purged anything because
   if (CORN::global_arguments->verbosity > 3) // actually deleting option
   //200115 if (verbose > 3) // deletion
      std::clog << "info: purging output from previous run." << std::endl;
     In cycle mode, there would not be Output subdir in the scenario
   arguments.provide_scenario_directory().purge_output();
   */
   if (arguments.start_directory.has_in_path_cstr("AgMIP"))                      //200817_160311
   //200817 if (arguments.provide_scenario_file_name().has_in_path_cstr("AgMIP"))         //160311
      initialized &= desire_AgMIP_variables();

      // NYI:  In  desired variables should selected with format files
      // (discovered in the context) so desired_AgMIP_variables is obsolete


   std::string ISO_6709_unused;                                                  //160111
   if (CORN::global_arguments->verbosity > 3) // actually discovery option
   //200115 if (verbose > 3)  //discovery
      std::clog << "info: discovering geocoordinate." << std::endl;
   geodesy::Geocoordinate_record geoloc_rec("location");                         //200117
   geoloc_rec.know(&geolocation);                                                //200117
   provide_context_discovery().get_geolocation                                   //170627_151207
      (arguments.start_directory                                                 //200817
       //200817 arguments.provide_scenario_directory()
      ,"location"
      ,geoloc_rec,ISO_6709_unused);                                              //200117_160111

#ifdef DEBUG_ANNUAL_CARBON_MASS
annual_carbon_mass << "year\t SOM_C_mass_30cm \t SOM_C_mass_profile \t  microbial_C_mass_30cm \t microbial_C_mass_profile \t labile_C_mass_30cm \t labile_C_mass_profile  \t metastable_C_mass_30cm  \t metastable_C_mass_profile  \t passive_C_mass_30cm \t passive_C_mass_profile" << endl;
#endif
/*V5 requires full compement of weather elements
   Location_parameters *location_loaded_by_LADSS = load_location_parameters();   //030911
*/

/* For CS_VERSION 5 GIS is not handled with scenario_generator I.e. scenario composition and command line argument options
   if (GIS_override_table && location_loaded_by_LADSS)                           //010516
      GIS_override_table->get(*location_loaded_by_LADSS); // Get the override of location parameters  //010516
*/
   if (CORN::global_arguments->verbosity > 3)  // actually file open option
   //200115   if (verbose > 3)  //open
      std::clog << "info: instanciating meteorology from file: "
      << template_scenario.weather_filename.c_str()
      << std::endl;
   if (CORN::OS::file_system_engine.exists(template_scenario.weather_filename))  //170612
      provide_weather_database_daily();                                          //170311
   else                                                                          //170612
   {  provide_explainations_global().take
      (new Explaination
         (CORN::Explaination::severity_error
         ,"%SUITE_DOC%/common/explaination/file_not_found.html"
         ,"Weather file is required"
         ,template_scenario.weather_filename));
      std::cerr << "error: weather not found:"
         << template_scenario.weather_filename.c_str() << std::endl;
      return initialized = false;
   }

// std::clog << "WARNING: annual temperature calibration (soil)" << std::endl;

   if (!template_scenario.annual_temperature.is_calibrated())                    //170219_150430
   {  // Do this before enable substitute dates because we dont need
      // to calibrated using substituted date [The calibrator wouldn't actually do that].
      CS::Temperature_amplitude_and_phase_calibrator annual_temperature_calibrator; //150430
      annual_temperature_calibrator.load_daily_temperature_from_UED
         (*(weather_DB_daily->database_file));                                   //170219_161212_150430
      annual_temperature_calibrator.calibrate                                    //150430
         (template_scenario.annual_temperature.avg
         ,template_scenario.annual_temperature.phase
         ,template_scenario.annual_temperature.avg_half_amplitude);
   }

   if (geolocation.get_elevation() < 0.0)
       geolocation.set_elevation_meter(global_elevation_mean_meter);
          // negative elevation is deemed invalid.
          // also AgMIP uses -99 as missing indicator.
   if (!geolocation.is_valid())
   {  // If a the geolocation is not specified for the site or is not valid
      // use the geolocation specified in the weather UED file (if any)
      weather_DB_daily->get_geocoordinate(geolocation);
   }
   meteorology = new CS::Land_unit_meteorological(simdate_raw,geolocation);      //170525
   weather_daily = new CS::Weather_daily(geolocation,simdate);                   //200817
   Weather_provider &weather_provider = weather_daily->provide_weather_provider();
   weather_DB_daily->know_weather_provider(&weather_provider);
   meteorology->sun_days   = &weather_daily->provide_sun_days();
   meteorology->sun_hours  = &weather_daily->provide_sun_hours();
   meteorology->know_weather_provider(&(weather_daily->provide_weather_provider())); //151215
/* NYI  160525
   UED_File_name storms_filename("storms.UED");
      //we can always check for existing storms file even in LADSS
      // This is temporary, should use file discovery
*/
   Minutes water_entering_soil_timestep_minutes = 60;                            //160511 070116
   const float32*  mean_peak_rainfall
         = template_scenario.mean_peak_rainfall;
   // Note that Storms could be simulation element
   CS::meteorology::Storms *storms =
/* NYI
        (CORN::OS::file_system_engine.exists(storms_filename))
         ? (CS::meteorology::Storms *)new CS::meteorology::Storms_database
               (today
               ,weather_provider.precipitation_actual                            //170218
               //170218 ,weather_extension->curr_precipitation                            //151027
               ,mean_peak_rainfall
               ,storms_filename)
         : (weather_extension->weather_data->get_location_parameters() && mean_peak_rainfall != 0)
*/
            template_scenario.has_mean_peak_rainfall()
            ? (CS::meteorology::Storms *)new CS::meteorology::Storms_hyeotographic
               (simdate_raw                                                      //170525
               ,weather_provider.precipitation_actual                            //170218
               ,mean_peak_rainfall
               ,(Minutes)template_scenario.default_event_duration_minutes
               ,false)
            : (CS::meteorology::Storms *)new CS::meteorology::Storms_uniform
               (simdate_raw                                                      //170525
               ,weather_provider.precipitation_actual                            //170218
               ,mean_peak_rainfall
               ,false
               ,water_entering_soil_timestep_minutes
               ,(Minutes)template_scenario.default_event_duration_minutes);
   meteorology->take_storms(storms);                                             //151215
   meteorology->know_weather_spanning(*weather_DB_daily);                        //151215
      // meteorology will provide hourly weather as needed
   if (template_scenario.simulate_CO2)
   {
      Atmospheric_CO2_change_element *atmospheric_CO2_change                     //171207
      = meteorology->instanciate_CO2                                             //151130
         (template_scenario.recalibration_filename
         ,template_scenario.initial_CO2_conc                                     //160531
         ,template_scenario.annual_CO2_change                                    //160531
         ,ref_start_date()                                                       //151203
         ,ref_stop_date());                                                      //151203
      simulation_units.take(atmospheric_CO2_change);                             //171207
   }
   know_meteorological(meteorology);
   initialized &= CS::Cycler::submit(meteorology);

// V5 will use explainations
   // can't list the required variables until we know the number of sublayers

   // This schedule output is temporary
   // and all rotator cycle are written to the same file.
   // Eventually there will be an explaination file for each cycle.

/* Special output
#ifdef TXT_OUTPUT
                           output_file << (T_date_Date) << ',';
               if (crop)   crop->txt_header_output(output_file);
                           ET_weather.txt_header_output(output_file);
                           soil->txt_header_output(output_file);
                           output_file << endl;
                           output_file << soil->num_sublayers << endl;
#endif
*/
/* special output
#ifdef DETAIL_DENITR
//061024 This is a special output wanted by Javier it is only temporary
//CORN::OS::Directory_entry_name_concrete scenario_output_dir(provide_scenario_directory_name(),"Output");
CORN::OS::File_name_concrete detail_denitr_dates_fname      (provide_scenario_directory_name(), "detail_denitr_dates.txt");
// detail_denitr_dates_fname.set_path(land_unit_with_output.scenario_directory);
if (CORN::OS::file_system_engine.exists(detail_denitr_dates_fname))
{
   ifstream detail_denitr_dates_file(detail_denitr_dates_fname.c_str());
   while ( !detail_denitr_dates_file.eof())
   {
      Date32 period_start = 0;
      Date32 period_end = 0;
      detail_denitr_dates_file >> period_start >> period_end;
      if (period_start && period_end)
         detail_denitr_dates.append(new Detail_denitr_date_range(period_start,period_end));
   }
}
#endif
*/

   if (CORN::global_arguments->verbosity > 3) // actually program-entry option
   //200115 if (arguments.is_verbose(3)) // program-entry
      std::clog << "info: CS::Cycler::initialize." << std::endl;
   CORN::OS::File_name *equilibrated_OM                                          //170420
      = find_equilibrated(template_scenario.equilibration,"equilibrated.organic_matter");  //170420
   if (equilibrated_OM)                                                          //170420
   {
      if (CORN::global_arguments->verbosity > 3) // actually discovery option
      //200115 if (arguments.is_verbose(2)) //discovery
         std::clog << "found: equilibrated organic matter=" << equilibrated_OM->c_str() << std::endl;
      template_scenario.organic_matter_initial_filename.set_DEN((*equilibrated_OM)); //170420
      delete equilibrated_OM; equilibrated_OM = 0;                               //170420
   }
   initialized &= CS::Cycler::initialize();
   return initialized;
}
//_initialize_______________________________________________________2016-05-15_/
CORN::OS::File_name *Simulation_engine::find_equilibrated
(const std::string &equilibration //  name of equilibration directory (unqual)
,const char *equilibrated_filename_unqual)                            rendition_
{
   if (equilibration.empty()) return 0;
   // search for the corresponding equilibration file
   /*
   Given that there exists a file from previously run simulation
   C:\Simulation\Projects\REACCH\GHG_SOC\Scenarios\2017-02-20\equilibration\MoscowMt\RT_WS-SP-WW_high\Output\0\equilibriated
   If the current scenario directory is
   C:\Simulation\Projects\REACCH\GHG_SOC\Scenarios\2017-02-20\historical\MoscowMt\RT_WS-SP-WW_high
   Of if the current scenario directory is
   C:\Simulation\Projects\REACCH\GHG_SOC\Scenarios\2017-02-20\future\MoscowMt\RT_WS-SP-WW_high\RCP4.5\bcc-csm1-1
   */
   CORN::OS::File_name *found_equilibrated = 0;
   CORN::Text_list scenario_path_list
      (arguments.start_directory.c_str()                                        //200817
      //200817 provide_scenario_directory()->c_str()
      ,DEFAULT_DIRECTORY_SEPARATORa_chr);
      // Excludes directories upto and include Scenarios
      // includes scenario levels and output
   CORN::Text_list Scenarios_path_list;
   CORN::Item_wstring *parent_dir = 0;
   do
   {  parent_dir = dynamic_cast<Item_wstring *>
         (scenario_path_list.pop_at_head());
      Scenarios_path_list.append(parent_dir);
   } while (parent_dir && (*parent_dir) != L"Scenarios");
   // scenario_paths_list now should be something like
   // 2017-02-20 historical MoscowMt RT_WS-SP-WW_high and output directories
   // or
   // 2017-02-20 future MoscowMt RT_WS-SP-WW_high RCP4.5 bcc-csm1-1 and output directories
   // and
   // Scenarios_path_list should be something like
   // C: Simulation Projects REACCH GHG_SOC Scenarios
   nat8 p1 = 0;
   CORN::OS::Directory_name *found_equilibration_dir = 0;
   CORN::Text_list remaining_dirs;
   FIND_FOR_EACH_IN
      (found_equilibration,level_or_output_name
      ,CORN::Item_wstring ,scenario_path_list,true,each_name)
   {  CORN::Text_list equilibration_scenario_paths_list(Scenarios_path_list); //copy
      for (nat8 p2 = 0; p2 <= p1; p2++)
      {  if (p1 == p2)
         {  equilibration_scenario_paths_list.add_string(equilibration);
            CORN::OS::Directory_name_concrete *potential_equilibration_qual = new
               CORN::OS::Directory_name_concrete
                  (equilibration_scenario_paths_list
                  ,CORN::OS::Directory_entry_name::include_all_except_extension
                  ,CORN::OS::Directory_entry_name::include_all);
            if (CORN::OS::file_system_engine.exists(*potential_equilibration_qual))
            {  found_equilibration = level_or_output_name;
               found_equilibration_dir = potential_equilibration_qual;
               for (nat8 p3 = p2+1; p3 < scenario_path_list.count(); p3++)
               {  const CORN::Item_wstring *dir_p3 =
                     dynamic_cast<const CORN::Item_wstring *>
                     (scenario_path_list.get_at(p3));
                  if (dir_p3)
                     remaining_dirs.add_wstring(*dir_p3);
               }
            } else delete potential_equilibration_qual;
         }
         else
         {  const CORN::Item_wstring *dir_p2 =
               dynamic_cast<const CORN::Item_wstring *>
               (scenario_path_list.get_at(p2));
            if (dir_p2)
               equilibration_scenario_paths_list.add_wstring(*dir_p2);
         }
      }
      p1 ++;
   } FOR_EACH_END(each_name)
   if (!found_equilibration_dir) return 0;
   CORN::OS::Directory_name_concrete *found_corresponding_equilibration_dir = 0;
   CORN::OS::Directory_name_concrete *best_corresponding_equilibration_dir = 0;
   CORN::Text_list corresponding_equilibration_path_list;
   found_equilibration_dir->append_path_list_to
         (corresponding_equilibration_path_list
         ,CORN::OS::Directory_entry_name::include_all);
   FOR_EACH_IN(remaining_dir,CORN::Item_wstring ,remaining_dirs,each_remaining)
   {  corresponding_equilibration_path_list.add_wstring(*remaining_dir);
      CORN::OS::Directory_name_concrete *pot_corresponding_equilibration_dir =
         new CORN::OS::Directory_name_concrete
            (corresponding_equilibration_path_list
            ,CORN::OS::Directory_entry_name::include_all_except_extension
            ,CORN::OS::Directory_entry_name::include_all);
      if (CORN::OS::file_system_engine.exists(*pot_corresponding_equilibration_dir))
         best_corresponding_equilibration_dir = pot_corresponding_equilibration_dir;
      else
      {  delete pot_corresponding_equilibration_dir;
         pot_corresponding_equilibration_dir = 0;
      }
   } FOR_EACH_END(each_remaining)
   found_corresponding_equilibration_dir = best_corresponding_equilibration_dir;
   if (found_corresponding_equilibration_dir)
   {  std::wstring equilibrated_filename_unqual_wstring;
      CORN::ASCIIZ_to_wstring
         (equilibrated_filename_unqual,equilibrated_filename_unqual_wstring);
      found_equilibrated = CORN::OS::file_system_engine.find_file_name
         (equilibrated_filename_unqual_wstring
         ,*found_corresponding_equilibration_dir
         ,CORN::OS::File_system::subdirectory_recursion_inclusive);
   }
   return found_equilibrated;
}
//_find_equilibrated________________________________________________2017-04-20_/
bool Simulation_engine::start()                                    modification_
{  return weather_daily->start()
          && CS::Cycler::start();
}
//_start____________________________________________________________2016-07-31_/

// In version 5 this will be obsolete
// We will add AgMIP.inspection_journal file to AgMIP projects.
// This is all setup, but I need to create such a file and link/reference
// it when creating AgMIP projects; so I haven't deleted this until
// I have implemented it.  170225


nat16 Simulation_engine::desire_AgMIP_variables()                initialization_
{
   CS::Optation_temporal &optation_day = CS::optation_global.provide_temporal(UT_day);
   return
      optation_day.know_variable_code(CSVC_crop_biomass_yield_period_sum)
    + optation_day.know_variable_code(CSVC_crop_biomass_produced_above_ground_period_sum)
    + optation_day.know_variable_code(CSVC_crop_base_peak_LAI_time_step_max)
    + optation_day.know_variable_code(CSVC_crop_base_flowering_date_YD)
    + optation_day.know_variable_code(CSVC_crop_base_maturity_date_YD)
    + optation_day.know_variable_code(CSVC_soil_N_leached_period_sum)
    + optation_day.know_variable_code(CSVC_soil_base_water_drainage_period_sum)
    + optation_day.know_variable_code(CSVC_crop_N_mass_yield_period_sum)
    + optation_day.know_variable_code(CSVC_weather_ET_act_period_sum)
    + optation_day.know_variable_code(CSVC_weather_ET_ref_period_sum)
    + optation_day.know_variable_code(CSVC_soil_N_mineralization_period_sum)
      //        + optation_day.know_variable_code(CSVC_mgmt_N_organic_gaseous_loss_period_sum)
    + optation_day.know_variable_code(CSVC_mgmt_N_inorganic_gaseous_loss_period_sum)
    + optation_day.know_variable_code(CSVC_soil_N_immobilization_period_sum)
    + optation_day.know_variable_code(CSVC_organic_matter_mineralized_period_sum)
    + optation_day.know_variable_code(CSVC_soil_N_denitrification_period_sum)

      // daily
    + optation_day.know_variable_code(CSVC_crop_biomass_yield)
    + optation_day.know_variable_code(CSVC_crop_biomass_canopy)
    + optation_day.know_variable_code(CSVC_crop_base_leaf_area_index)
    + optation_day.know_variable_code(CSVC_soil_N_leached_accum_to_date_time)
    + optation_day.know_variable_code(CSVC_crop_N_mass_canopy_production_accum_to_date_time)
    + optation_day.know_variable_code(CSVC_crop_N_mass_canopy_current)
    + optation_day.know_variable_code(CSVC_soil_base_water_drainage_accum_to_date_time)
    + optation_day.know_variable_code(CSVC_weather_ET_act_accum_to_date_time)
    + optation_day.know_variable_code(CSVC_weather_ET_ref_accum_to_date_time)

    + optation_day.know_variable_code(CSVC_organic_matter_mineralized_accum_to_date_time)
    + optation_day.know_variable_code(CSVC_mgmt_N_inorganic_gaseous_loss_accum_to_date_time)
    + optation_day.know_variable_code(CSVC_soil_N_immobilization_accum_to_date_time)
    + optation_day.know_variable_code(CSVC_soil_N_denitrification_time_step_sum)
    + optation_day.know_variable_code(CSVC_soil_base_plant_avail_water)
    + optation_day.know_variable_code(CSVC_organic_matter_N_mineralization_profile_time_step_sum)
    + optation_day.know_variable_code(CSVC_organic_matter_mineralized_time_step_sum);

/*190119 now using optation_global
   nat16 vars_appended = 31;
      // NYI eventually append should return bool and then we simply sum the count of appends
   if (desired_variables_daily)
   {
      desired_variables_daily->append(CSVC_crop_biomass_yield_period_sum);
      desired_variables_daily->append(CSVC_crop_biomass_produced_above_ground_period_sum);
      desired_variables_daily->append(CSVC_crop_base_peak_LAI_time_step_max);
      desired_variables_daily->append(CSVC_crop_base_flowering_date_YD);
      desired_variables_daily->append(CSVC_crop_base_maturity_date_YD);
      desired_variables_daily->append(CSVC_soil_N_leached_period_sum);
      desired_variables_daily->append(CSVC_soil_base_water_drainage_period_sum);
      desired_variables_daily->append(CSVC_crop_N_mass_yield_period_sum);               //120117
      desired_variables_daily->append(CSVC_weather_ET_act_period_sum);
      desired_variables_daily->append(CSVC_weather_ET_ref_period_sum);                  //130724
      desired_variables_daily->append(CSVC_soil_N_mineralization_period_sum);
      //desired_variables_daily->append(CSVC_mgmt_N_organic_gaseous_loss_period_sum);
      desired_variables_daily->append(CSVC_mgmt_N_inorganic_gaseous_loss_period_sum);
      desired_variables_daily->append(CSVC_soil_N_immobilization_period_sum);
      desired_variables_daily->append(CSVC_organic_matter_mineralized_period_sum);
      desired_variables_daily->append(CSVC_soil_N_denitrification_period_sum);

      // daily
      desired_variables_daily->append(CSVC_crop_biomass_yield);
      desired_variables_daily->append(CSVC_crop_biomass_canopy);
      desired_variables_daily->append(CSVC_crop_base_leaf_area_index);
      desired_variables_daily->append(CSVC_soil_N_leached_accum_to_date_time);
      desired_variables_daily->append(CSVC_crop_N_mass_canopy_production_accum_to_date_time); //131007
      desired_variables_daily->append(CSVC_crop_N_mass_canopy_current);                 //131007
      desired_variables_daily->append(CSVC_soil_base_water_drainage_accum_to_date_time);
      desired_variables_daily->append(CSVC_weather_ET_act_accum_to_date_time);
      desired_variables_daily->append(CSVC_weather_ET_ref_accum_to_date_time);          //130724

      desired_variables_daily->append(CSVC_organic_matter_mineralized_accum_to_date_time);
      desired_variables_daily->append(CSVC_mgmt_N_inorganic_gaseous_loss_accum_to_date_time);
      desired_variables_daily->append(CSVC_soil_N_immobilization_accum_to_date_time);
      desired_variables_daily->append(CSVC_soil_N_denitrification_time_step_sum);
      desired_variables_daily->append(CSVC_soil_base_plant_avail_water);
      desired_variables_daily->append(CSVC_organic_matter_N_mineralization_profile_time_step_sum);
      desired_variables_daily->append(CSVC_organic_matter_mineralized_time_step_sum);
   }

   return vars_appended;
*/
}
//_desire_AgMIP_variables___________________________________________2016-03-12_/
CS::Parameter_stores  &Simulation_engine::provide_stores()
{  if (!stores)
        stores = new CS::Parameter_stores(provide_databases());
   return *stores;
}
//_provide_stores___________________________________________________2016-09-16_/
const CropSyst::Scenario_directory *Simulation_engine::provide_scenario_directory() provision_
{  return
      &scenario_directory;                                                       //200525
   //200524 &arguments.provide_scenario_directory();
}
//_provide_scenario_directory_______________________________________2016-05-30_/
}}//_namespace CropSyst::version5______________________________________________/
//______________________________________________________________________________

