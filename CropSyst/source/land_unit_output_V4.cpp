#include "land_unit_output_V4.h"
#include "soil/chemicals_profile.h"
#ifdef NITROGEN
#include "soil/nitrogen_profile.h"
#endif
#include "cs_simulation.h"
#include "cs_scenario_directory.h"
#include "corn/data_source/vv_file.h"
#include "common/weather/weather.h"
#include "crop/crop_interfaced.h"
#include "cs_ET.h"
#include "UED/library/timeqry.h"

#  include "crop/crop_N_common.h"
// Actually will be using crop_N_V5 but need to check that spitting the subclass worked
#  include "crop/crop_cropsyst.h"
#  include "crop/crop_fruit.h"
#  include "common/weather/database/weather_database.h"
#  include "static_phrases.h"
#  include "csprofss.h"
#  include "cs_accum.h"
#include <iomanip>
#  include "static_phrases.h"
#include "fmt_param.h"
#include "soil/chemical_balance.h"
using namespace std;
extern bool harvest_event_failed_today; // This is here only for Mike Rivington's spreadsheet
#ifdef _Windows
ofstream detail_txt_file;
#ifdef DEBUG_CROP_N_FATE
ofstream crop_N_fate_txt_file;
#endif
#endif
#include "csvc_special.h"
#include "soil/erosion_RUSLE.h"
#include "corn/chronometry/date_const.h"

//______________________________________________________________________________
namespace CropSyst {
bool Land_unit_with_output_V4::start_growing_season
(modifiable_ Crop_model_interface &crop_at_start_season)                         //130716
{  bool started = Land_unit_with_output::start_growing_season
      (crop_at_start_season);                                                    //130716
   #ifdef XLS_OUTPUT
   #ifdef NITROGEN
   if (GS_N_balance_report)
   {
   Soil::Chemicals_profile *soil_chemicals = get_soil_chemicals();
   Soil::NO3_Profile *NO3 = soil_chemicals ? soil_chemicals->NO3 : 0;
   Soil::NH4_Profile *NH4 = soil_chemicals ? soil_chemicals->NH4 : 0;
      GS_N_balance_report->set_reporters
      (NO3->GS_bal_accums_M, NH4->GS_bal_accums_M,GP_accumulators_active);
   }
   #endif
   #endif
   return started;
}
//_start_growing_season_____________________________________________2012-05-03_/
bool Land_unit_with_output_V4::output_growing_season
(const Crop_model_interface &crop_at_end_season_)                                //130717
{
   bool outputed = Land_unit_with_output::output_growing_season                  //170820
      (crop_at_end_season_);                                                     //130717
   #ifdef XLS_OUTPUT
   if (outputed && GP_accumulators_active)                                       //170821
   {  if (GS_report)             GS_report->write_detail_line();
      if (GS_N_balance_report)   GS_N_balance_report->write_detail_line
         (simdate,crop_at_end_season_.get_description());                        //160326
   }
   #endif

/*200722  abandoned decided to just pot this in the normal data record
   annual_UED_db. set_value  // was set
      (float32 value
      ,const Time_query &time_query                                              //990118
      ,CORN::Units_code      &units
      ,CORN::Quality  &attribute    // must be a returned value                  //000710
      ,const Data_record_creation_layout &creation_layout
      ,bool  &record_created
      ,bool override_safety // = false
      );

crop_at_end_season_
annual_UED_db
   GDDs  ,UC_Celcius_degree_days ,*context, "growing_degree_days/normal" ,sum_statistic,CSVP_crop_base_seasonal_thermal_time));
*/
   return outputed;
}
//_output_growing_season____________________________________________2010-07-04_/
#if ((CROPSYST_VERSION > 0) && (CROPSYST_VERSION < 5))
Land_unit_with_output_V4::Land_unit_with_output_V4
(const CS::Identification     &LBF_ID_                                           //151128
,nat32                         cycle_shift_                                      //151026
,CropSyst::Scenario           &scenario_control_and_model_options_               //130827
,const CORN::date32           &today_                                            //170525
,const Geocoordinate          &geocoordinate_                                    //151116
,
      #if (CROPSYST_VERSION==4)
         // temporarily remove const until I cleanup meteorology setup in cs_simulation
      #else
      const
      #endif
      CS::Land_unit_meteorological &meteorology_                                 //151128
#if (CS_VERSION==4)
,Common_simulation_log        &event_log_
#endif
,const CropSyst::Scenario_directory *scenario_directory_                         //150920_050619
,const CORN::OS::File_name *override_format_filename                             //120505
,bool                          output_daily_UED_)                                //130624
:Land_unit_with_output
   (LBF_ID_                                                                      //151128
   ,cycle_shift_                                                                 //151128
   ,scenario_control_and_model_options_                                          //130827
   ,today_                                                                       //131004
   ,geocoordinate_                                                               //15116
   ,meteorology_                                                                 //151128
   #if (CS_VERSION==4)
   ,event_log_
   #endif
   ,scenario_directory_)                                                         //050619_041021
#ifdef XLS_OUTPUT
, DLY_report(0)
//200122 #ifndef SOILR
, YLY_report(0)
, GS_report(0)
, DLY_N_balance_report(0)
, YLY_N_balance_report(0)
, GS_N_balance_report(0)
//200122 #endif
#endif

//#if (CROPSYST_PROPER == 4)
#  ifndef __linux
#    if (!defined(OFOOT_VERSION))
,        detail_txt_stream(0)                                                    //090803
#        ifdef DETAIL_DENITR
,          denitr_detail_stream  (0)                                             //160128
#        endif
#    endif
#  endif
#  ifdef RUSLE_DETAIL
,RUSLE_detail_txt_file(0)
#  endif
//#endif
#if ((CS_VERSION >= 1) && (CS_VERSION < 5))
, format_filename( override_format_filename?(*override_format_filename):
    dynamic_cast<const CORN::OS::File_name &>
   (scenario_control_and_model_options_.report_format_filename_rel))             //150415_120505
/* should restore this
, format_filename(override_format_filename?(*override_format_filename)
   :scenario_control_and_model_options_.report_format_filename_rel)              //150415_120505
*/
#endif
,reports(0)                                                                      //010202
#ifdef PRINT_PROFILE
,profile_spreadsheet_set(0)                                                      //000827
,record_soil_profile_today(false)
#endif                               land_unit_with_output.weather_filename
#ifndef __linux__
,tot_txt_file
   (scenario_directory                                                           //150210
   ? scenario_directory->CS_output_totals_txt_cstr                               //121121
   : "totals.txt")                                                               //150210
#endif
{
   #ifndef __linux
   CORN::OS::File_name_concrete detail_txt_filename
      (scenario_directory_->Output(),"detail.txt");                              //090803
   detail_txt_stream = new std::ofstream(detail_txt_filename.c_str());           //090803
   #endif
   #ifdef RUSLE_DETAIL
   CORN::OS::File_name_concrete RUSLE_detail_txt_filename
      (*scenario_directory_->output_dir,"RUSLE_detail.txt");                     //130818
   RUSLE_detail_txt_file = new std::ofstream(RUSLE_detail_txt_filename.c_str());
   #endif
}
//_Land_unit_with_output_V4:constructor_____________________________2012-05-04_/
bool Land_unit_with_output_V4::initialize()                                      //141220
{
   #if (((CS_VERSION >= 1) && (CS_VERSION < 5)))
   if (CORN::OS::file_system_engine.exists(format_filename))                     //121121
   {  // Load the report formats
         reports = new Report_formats_parameters();
         VV_File report_format_file(format_filename.c_str());
         report_format_file.get(*reports);
   }
   #ifndef __linux
   #if (!defined(OFOOT_VERSION))
   organic_matter_percents_file.open
      (scenario_directory->Output().organic_matter_percents_dat().c_str());      //141119
   organic_matter_percents_file << "horizon\torganic_matter_biomass(%)\tmicrobial_C(%)\tlabile_C(%)\tmetastable_C(%)\tpassive_C(%)\tall_residues_biomass(kg/ha)" << std::endl; //070314
   #endif
   #endif

   #if (defined(XLS_OUTPUT) || defined(UED_OUTPUT))
   if (reports)                                                                  //041014
      reports->list_required_variables(MAX_soil_sublayers);                      //041014
      // Currently I am setting all the possible sublayers
      // eventually I am going to create a soil parameter by sublayer
      // where the subdivision will occur earlier, when the number of sublayer is available,
      // I can specify here the actual number of sublayers
   #endif
   // Soil must be loaded before initializing LBF
   #endif
   bool initialized = Land_unit_with_output::initialize();                       //141220
   //We must load_reports_options BEFORE opening output files
   open_out_files();
   write_headers();
   output_organic_matter_partitioning(true);
   return initialized;
}
//_initialize_______________________________________________________2012-05-04_/
bool Land_unit_with_output_V4::start()
{
   bool started = Land_unit_with_output::start();
   //#if (CROPSYST_PROPER==4)
   #ifdef _Windows
      #ifndef USE_CS_SIMULATION_ROTATOR

         if (detail_txt_stream)
         {
//   const Crop_complete *crop_CropSyst = dynamic_cast<const Crop_complete *>(crop_active_or_intercrop);

           (*detail_txt_stream)
               << "date" << '\t'
               << "accum_deg_days" << '\t'
               << "chill_hours_remaining"<< '\t';
           (*detail_txt_stream)
               << std::endl;
         };
         #ifdef DUMP_CURVE_NUMBERS
         CORN::OS::File_name_concrete SCS_runoff_dat_filename(scenario_directory->Output(),"SCS_runoff.dat"); //131216
         CropSyst::SCS_runoff_dat_file = new std::ofstream(SCS_runoff_dat_filename.c_str());     //131216
         (*CropSyst::SCS_runoff_dat_file)                                        //131216
                     << "date" << '\t'
                     << "CN1" <<  '\t'
                     << "CN2" <<  '\t'
                     << "CN3" <<  '\t'
                     << "surface_storage";
         #endif
      #endif
      #ifdef RUSLE_DETAIL
         if (RUSLE_detail_txt_file)
            (*RUSLE_detail_txt_file)
               << "date" << '\t'
               << "precip_mm" << '\t'
               << "storm_energy" << '\t'
               << "storm_intensity" << '\t'
               << "C_factor" << '\t'
               << "mulch_factor" << '\t'
               << "moisture_factor" << '\t'
               << std::endl;
      #endif
   #endif
   //#endif
   CORN::OS::file_system_engine.provide_directory
      (scenario_directory->Output().Soil());                                     //131011

   Soil::Chemicals_profile *soil_chemicals = get_soil_chemicals();
   Soil::NO3_Profile *NO3 = soil_chemicals ? soil_chemicals->NO3 : 0;
   Soil::NH4_Profile *NH4 = soil_chemicals ? soil_chemicals->NH4 : 0;
   #ifdef XLS_OUTPUT
   if (DLY_N_balance_report) DLY_N_balance_report->set_reporters(NO3->CONT_bal_accums_M  ,NH4->CONT_bal_accums_M   ,CONTINUOUS_accumulators_active);           //990319
   if (YLY_N_balance_report) YLY_N_balance_report->set_reporters(NO3->ANNL_bal_accums_M  ,NH4->ANNL_bal_accums_M   ,ANNL_accumulators_active);                 //990319
   if (GS_N_balance_report)  GS_N_balance_report->set_reporters
      (NO3->GS_bal_accums_M
      ,NH4->GS_bal_accums_M
      ,GP_accumulators_active);                                                  //030516
   #endif
#ifdef PRINT_PROFILE
   bool generate_profile_spreadsheet = reports
      && ((reports->profile_spreadsheet_timestep > 0)
          || (reports->at_planting)                                              //000331
          || (reports->at_emergence)                                             //000331
          || (reports->at_flowering)                                             //000331
          || (reports->at_maturity)                                              //000331
          || (reports->at_harvest)                                               //000331
          ||  reports->num_profile_print_dates);                                 //000526
   if ( generate_profile_spreadsheet )                                           //000331
   {  profile_spreadsheet_set = new Soil_profile_spreadsheet_set                 //000827
      (scenario_directory->Output().Soil()                                       //130826
      ,*ref_soil()                                                               //161008_000331
      ,scenario_control_and_model_options.description.brief.c_str()              //150112_130827_000827
      ,scenario_control_and_model_options.run_nitrogen                           //130827_000331
      #ifdef SALINITY
      ,scenario_control_and_model_options.run_salinity                           //130827_000331
      #endif
      ,reports->SS_hydraulic_properties                                          //000911
      ,reports->SS_water_content                                                 //000911
      ,reports->SS_water_potential                                               //000911
      ,reports->SS_root_fraction                                                 //010322
      ,reports->SS_root_biomass                                                  //071214
      #ifdef NITROGEN
      ,reports->SS_ammonium
      ,reports->SS_nitrate
      ,reports->SS_denitrification
      ,reports->SS_nitrification
      ,reports->SS_NH4_mineralization
      ,reports->SS_organic_matter
      #endif
      #ifdef RESIDUES
      ,reports->SS_plant_residue
      ,reports->SS_manure_residue
      #endif
      #ifdef SOIL_TEMPERATURE
      ,reports->SS_temperature
      #endif
      #ifdef SALINITY
      ,reports->SS_salinity
      ,reports->SS_salt
      #endif
      );
   } // generate_profile_spreadsheet
#endif
   return started;
}
//_start____________________________________________________________2012-05-04_/
bool Land_unit_with_output_V4::start_year() modification_
{  bool started = Land_unit_with_output::start_year();
   #ifdef CHEM_PROFILE
      Soil::Chemicals_profile *soil_chemicals = get_soil_chemicals();
      #ifdef NITROGEN
      //120508 I haven't figured out yet why Qt gcc is not defining the reports
      Soil::NO3_Profile *NO3 = soil_chemicals ? soil_chemicals->NO3 : 0;
      Soil::NH4_Profile *NH4 = soil_chemicals ? soil_chemicals->NH4 : 0;
      #ifdef XLS_OUTPUT
      if (NO3 && NH4)                                                            //040115
      if (YLY_N_balance_report)
         YLY_N_balance_report->set_reporters
            (NO3->ANNL_bal_accums_M
            ,NH4->ANNL_bal_accums_M
            ,ANNL_accumulators_active);
      #endif
      #endif
   #endif
   return started;
}
//_start_year_______________________________________________________2012-05-04_/
void Land_unit_with_output_V4::output_organic_matter_partitioning(bool at_start)
{
   #if (CROPSYST_PROPER == 4)

   // Move this land_unit_output_V4
   // and add these outputs using inspection

   #ifndef __linux
   #if (!defined(OFOOT_VERSION))
   if (scenario_control_and_model_options.organic_matter_and_residue_model_clad.get() != multiple_SOM_multiple_residues)
   {  organic_matter_percents_file << "This output is only available with the 'Microbial, stable organic matter and residues with decomposition' submodel." << endl;
      return;
   }
   #endif
   #endif
   #endif
   if (!soil) return;
   const Soil::Layers_interface *layers = soil->ref_layers();                    //160412
   if (!layers) return;
   nat8 soil_num_layers = layers->count();
   nat8 last_horizon = 1;                                                        //150821
   nat8 horizon_start_layer = 1;
   nat8 horizon_end_layer = 1;
   float64 soil_mass_horizon = 0.0;
   #if (CROPSYST_PROPER == 4)
   // Move this land_unit_output_V4
   // and add these outputs using inspection
      #ifndef __linux
         if (at_start)  organic_matter_percents_file << "Start" << endl;
         else           organic_matter_percents_file << simdate.get_year() << "(Dec 31)" << endl;
      #endif
   for (nat8 layer = 1; layer <= soil_num_layers; layer++)
   {
      nat8 horizon = layers->get_horizon_at_layer(layer);
      if ((horizon != last_horizon) || (layer == soil_num_layers))
      {
         // output the horizon percentages
         float64 microbial_C_mass_horizon    = organic_matter_residues_profile_kg_m2->get_mass_from_layer_to_layer(C_MASS_TOTAL,INCLUDE_MICROBIAL_BM         ,horizon_start_layer,horizon_end_layer,1.0);
         float64 labile_C_mass_horizon       = organic_matter_residues_profile_kg_m2->get_mass_from_layer_to_layer(C_MASS_TOTAL,INCLUDE_LABILE_ACTIVE_SOM    ,horizon_start_layer,horizon_end_layer,1.0);
         float64 metasatable_C_mass_horizon  = organic_matter_residues_profile_kg_m2->get_mass_from_layer_to_layer(C_MASS_TOTAL,INCLUDE_METASTABLE_ACTIVE_SOM,horizon_start_layer,horizon_end_layer,1.0);
         float64 passive_C_mass_horizon      = organic_matter_residues_profile_kg_m2->get_mass_from_layer_to_layer(C_MASS_TOTAL,INCLUDE_PASSIVE_SOM          ,horizon_start_layer,horizon_end_layer,1.0);
         float64 organic_matter_C_mass         = microbial_C_mass_horizon + labile_C_mass_horizon + metasatable_C_mass_horizon+ passive_C_mass_horizon;
         if (organic_matter_C_mass > 0)
         {
         float64 microbial_percent           = microbial_C_mass_horizon   / organic_matter_C_mass * 100.0;
         float64 labile_percent              = labile_C_mass_horizon      / organic_matter_C_mass * 100.0;
         float64 metastable_percent          = metasatable_C_mass_horizon / organic_matter_C_mass * 100.0;
         float64 passive_percent             = passive_C_mass_horizon     / organic_matter_C_mass * 100.0;
         float64 organic_matter_biomass      = organic_matter_residues_profile_kg_m2->get_mass_from_layer_to_layer(BIOMASS_TOTAL,INCLUDE_SOM_AND_MICROBIAL ,horizon_start_layer,horizon_end_layer,1.0);
         float64 organic_matter_percent      = organic_matter_biomass      / soil_mass_horizon   * 100.0;
         float64 residue_mass_kg_ha          = per_m2_to_per_ha(organic_matter_residues_profile_kg_m2->get_mass_from_layer_to_layer(BIOMASS_TOTAL,INCLUDE_RESIDUE_OM          ,horizon_start_layer,horizon_end_layer,1.0));   //070316
         // Set up for next horizon
         horizon_start_layer  = layer;
         #ifdef __linux
         }
         #else
         #if (!defined(OFOOT_VERSION))
         organic_matter_percents_file << setw(8) << setprecision(4) << ((int)last_horizon) <<"\t" << organic_matter_percent <<"\t" << microbial_percent <<"\t" << labile_percent <<"\t" << metastable_percent <<"\t" << passive_percent<<"\t" << residue_mass_kg_ha << endl;
         } else
            organic_matter_percents_file << setw(8) << ((int)last_horizon) <<"\t" << "No SOM" << endl;
         #else
         }
         #endif
         #endif
         soil_mass_horizon = 0.0;
      }
      float64 soil_mass_layer = soil->get_mass_kg_m2(layer);
      soil_mass_horizon       += soil_mass_layer;
      horizon_end_layer    = layer;
      last_horizon = horizon;
   }
   #endif
}
//_output_organic_matter_partitioning_______________________________2007-03-14_/
bool Land_unit_with_output_V4::end_day()
{  bool ended = Land_unit_with_output::end_day();
   Crop_model_interface *crop = crop_active_or_intercrop;                        //130716_070227_050115_031119 //131008NS
        // not const because we adjust crop's biomatter fate
   const Soil_components* soil = ref_soil(); // for local reference              //161008_060803
   #ifdef DISPLAY_PROFILE
   if ((scenario_control_and_model_options.display_profile ))
   {
      #ifdef unix
         system("clear");
      #endif
      //200420 std::string growth_stage_description;                                      //190613
      std::wstring crop_nameW; std::string crop_name;                            //200420
      if (crop)                                                                  //200420
      {  crop->get_name(crop_nameW);                                             //200420
         CORN::wstring_to_string(crop_nameW,crop_name);                          //200420
      }
      print_profile(std::cout,true,simdate
         ,crop ? crop->get_description() : "fallow"
         ,crop_name.c_str()                                                      //200420
         ,crop ? crop->get_recorded_root_depth_m() : 0.0);
   }
   #endif

#ifdef CHECK_USED
//130729
#error reached
   Annual_accumulators *ANNL_accumulators = ANNL_accumulators_active;            //030912
   if (soil)                                                                     //060803
   {  ANNL_accumulators->last_pot_evaporation          = soil->pot_evaporation;
   }
#ifdef RUNOFF
      ANNL_accumulators->last_est_runoff              = runoff ? runoff->ref_estimated_runoff() : 0.0; //130308_020620
#endif

   ANNL_accumulators->last_avg_temp     = meteorology.ref_air_temperature_avg(); //151130
   ANNL_accumulators->last_precip         = weather->get_precipitation_m(today); //9909013 today probably not needed                  //010202
   if (crop)                                                                     //070227
   {                                                                             //070227
      ANNL_accumulators->last_canopy_biomass          = crop->get_canopy_biomass_kg_m2();
      ANNL_accumulators->last_root_biomass            = crop->get_act_root_biomass_kg_m2();           //010713
      ANNL_accumulators->last_LAI                     = crop->get_LAI(false);
      ANNL_accumulators->last_GAI                     = crop->get_GAI(include_vital|include_effete);
      ANNL_accumulators->last_act_transpiration       = crop->get_act_transpiration_m();
      ANNL_accumulators->last_pot_transpiration       = crop->get_pot_transpiration_m();

            rename transpiration demand pot

            add Transpiration uptake_water potential attainable


      ANNL_accumulators->last_fract_canopy_cover      = crop->get_fract_canopy_cover();
      ANNL_accumulators->last_pot_evapotranspiration  = ET->pot_crop_evapotransp_m;    //010202
      ANNL_accumulators->last_temperature_stress      = crop->get_temperature_stress();//010726
      ANNL_accumulators->last_water_stress            = crop->get_water_stress();      //010726
      {  const Crop_nitrogen_interface  *crop_n = crop->get_nitrogen();
         if (crop_n)  // could be null if fallow
         ANNL_accumulators->last_nitrogen_stress  = crop_n->get_stress();
      }
      ANNL_accumulators->last_total_stress  = crop->get_overall_growth_stress(); // overall_growth_stress;
   }
#endif
// check used

   // WARNING: NEEd TO IMPLEMENT. if (today.get_DOY() == 1) && separate_year { op spreadsheet and WKD for this year}
//to be relaced with UED
#ifdef PRINT_PROFILE
    if (profile_spreadsheet_set && record_soil_profile_today)                    //000827
       profile_spreadsheet_set->write_day
         (simdate,(crop ? crop->get_recorded_root_depth_m() : 0.0)               //000827
         ,(crop),organic_matter_residues_profile_kg_m2);
#endif
#ifdef XLS_OUTPUT
   {
    if ((DLY_report) && (DLY_report->report_options.time_step)
        && ((simdate.get_DOY() % DLY_report->report_options.time_step) == 0))
         DLY_report->write_detail_line();
    if (DLY_N_balance_report)DLY_N_balance_report->write_detail_line(simdate,0); //160326
         // currently not outputing crop name on every line.
   }
#endif
   return ended;
}
//_end_day__________________________________________________________2012-05-04_/
bool Land_unit_with_output_V4::end_year()
{  bool ended = Land_unit_with_output::end_year();                               //131010
   output_organic_matter_partitioning(false);                                    //070214

   //#if (CS_VERSION==4)
   #ifdef RUSLE_DETAIL
   if (RUSLE_detail_txt_file && soil && soil->erosion)                           //160506
      soil->erosion->dump_year_days(*RUSLE_detail_txt_file);                     //160506
   #endif
   //#endif

   #ifdef XLS_OUTPUT
   if (YLY_report)
   {  YLY_report->write_detail_line();
      last_YLY_report_detail_write_date.set(simdate);                            //131030
      // The last detail line was printing twice.
   }
   #ifdef NITROGEN
   if (YLY_N_balance_report)                                                     //050116
   {
      #ifdef CHEM_PROFILE
      Soil::Chemicals_profile *soil_chemicals = get_soil_chemicals();
      Soil::NO3_Profile *NO3 = soil_chemicals ? soil_chemicals->NO3 : 0;
      Soil::NH4_Profile *NH4 = soil_chemicals ? soil_chemicals->NH4 : 0;
      if (NO3 && NH4)                                                            //050116
      {  // Makesure reporters are setup    Note If we do this here, probably dont have to do it in start_year
         YLY_N_balance_report->set_reporters
            (NO3->ANNL_bal_accums_M
            ,NH4->ANNL_bal_accums_M
            ,ANNL_accumulators_active);
         // Not sure why crashing
         YLY_N_balance_report->write_detail_line(simdate,0);                     //160326
      }
      #endif
    }
    #endif
   #endif
   return ended;
}
//_end_year_________________________________________________________2012-05-04_/
bool Land_unit_with_output_V4::stop()
{
   #ifndef __GNUC__
   //120508 I haven't figured out yet why Qt gcc is not defining the reports
   #ifdef XLS_OUTPUT
   if (YLY_report
       && (simdate.get_DOY() != last_YLY_report_detail_write_date.get_DOY())
          // The last detail line was printing twice.
       && (simdate.get_DOY() != 1))
       // we already incremented DOY and may have printed end of year detail above
      YLY_report->write_detail_line();
   #endif      
   #endif
   #ifndef __linux
      #if (!defined(OFOOT_VERSION))
         delete detail_txt_stream; detail_txt_stream = 0;                        //131216
         #ifdef DUMP_CURVE_NUMBERS
         delete CropSyst::SCS_runoff_dat_file; CropSyst::SCS_runoff_dat_file=0;  //131216
         #endif
         #ifdef DETAIL_DENITR
         delete denitr_detail_stream;
         #endif
         #ifdef RUSLE_DETAIL
         //280907RLN if (RUSLE_detail_txt_file)
         delete RUSLE_detail_txt_file; RUSLE_detail_txt_file = 0;                //120529
         #endif
      #endif
   #endif
   bool stopped = Land_unit_with_output::stop();
   close_out_files();
   #ifdef PRINT_PROFILE
   if (profile_spreadsheet_set)                                                  //000827
      delete profile_spreadsheet_set;                                            //000827
   #endif
   return stopped;
}
//_stop_____________________________________________________________2012-05-04_/
void Land_unit_with_output_V4::harvest_event
(const Crop_model_interface &crop_at_harvest)                                    //130716
{  Land_unit_with_output::harvest_event(crop_at_harvest);                        //130716
#ifdef PRINT_PROFILE
#ifdef XLS_OUTPUT
   if (reports && reports->at_harvest) { record_soil_profile_today = true; }
#endif
#endif
   const Crop_model_interface *crop =  &crop_at_harvest; // for local reference
   // Currently this is the active crop because this is called in trigger syncs  //020308
   // This if is here only for Mike Rivington's spreadsheet                      //070416
   if (harvest_event_failed_today && crop_active_or_intercrop)
       crop = crop_active_or_intercrop;
   const CropSyst::Soil_components *soil = ref_soil();   // for local reference  //161008_060803
   Period_accumulators *GP_accumulators = GP_accumulators_active;                //030912
   const Crop_mass_fate *biomass_fate_today = crop->get_biomass_fate_today_record();
   if (!GP_accumulators) return;                                                 //031202
   float64 removed_for_harvest_and_use = 0;                                      //060316
   float64 tot_biomass = 0;  //kg/m2?                                            //980724
   if (biomass_fate_today)                                                       //060316
   {  tot_biomass = biomass_fate_today->canopy_before;                           //980724
      removed_for_harvest_and_use = biomass_fate_today->yield+biomass_fate_today->useful; //060316
      //190617 if (crop->get_crop_model() == CROPSYST_ORCHARD_MODEL)             //130408
      if (crop->is_fruit())                                                      //190707
         tot_biomass +=  biomass_fate_today->yield ;                             //030529
   } else                                                                        //200603
   {  // Fruit doesnt render biomass_fate_today
      removed_for_harvest_and_use = crop->get_latest_yield_kg_m2();              //200608
      tot_biomass = crop->get_live_green_canopy_biomass_kg_m2
         (include_vital|include_effete);                                         //200608
//                  + crop->get_latest_yield_kg_m2();                            //200603

   }
   CORN::OS::Directory_entry_name_concrete debug_dir;                            //121121
   const CropSyst::Phenology &phenology = crop->ref_phenology();                 //190617

   #ifndef __linux
   //191104 if (verbose) //progression                                                    //160311_050115
   { // total txt file name
      tot_txt_file  << setiosflags(ios::fixed|ios::showpoint);
      tot_txt_file << setw(4) << simdate.get_year();
      tot_txt_file << setw(5) << phenology.get_planting_DOY();                   //190617  crop->get_planting_date().get_DOY();            //041205
      //NYI   tot_txt_file << setw(5) << crop->get_restart_date().get_DOY();
      tot_txt_file << setw(5) << phenology.get_emergence_DOY();                  //190617 crop->get_emergence_date().get_DOY();
      tot_txt_file << setw(5) << phenology.get_maturity_DOY();                   //190617 crop->get_maturity_date().get_DOY();
      tot_txt_file << setw(7) << setprecision(3) << (removed_for_harvest_and_use* 10.0);              //060316
      tot_txt_file << setw(7) << setprecision(3) <<(tot_biomass * 10.0);                              //980724
      tot_txt_file << setw(6) << setprecision(1) << m_to_mm(GP_accumulators->balance_drainage),       //990215
      tot_txt_file << setw(6) << setprecision(1) << m_to_mm(GP_accumulators->evapotranspiration_act)
         << ' ' << crop->get_description()  << endl;
   }
   #endif
   if (biomass_fate_today)     // already output biomass_fate_today????          //200603_040716
   { // total htm file name
      std::ofstream harvest_htm_file(scenario_directory->CS_output_harvest_htm_cstr,ios::app);  //140422_121121_030516
      // Apparently needed to force the correct directory
      harvest_htm_file  << "<!--" << debug_dir.c_str() << "-->" <<endl;
      harvest_htm_file  << setiosflags(ios::fixed|ios::showpoint);
      harvest_htm_file << "<TR><TD>" << simdate.get_year();
      harvest_htm_file
            << "<TD ALIGN=RIGHT>" << phenology.get_planting_DOY()                //190617 crop->get_planting_date().get_DOY()         //041205
            << "<TD ALIGN=RIGHT>" << phenology.get_restart_DOY()                 //190617 crop->get_restart_date().get_DOY()          //041205      //041205
            << "<TD ALIGN=RIGHT>" << phenology.get_emergence_DOY()               //190617 crop->get_emergence_date().get_DOY()
            << "<TD ALIGN=RIGHT>" << phenology.get_maturity_DOY();               //190617 crop->get_maturity_date().get_DOY();
      harvest_htm_file << "<TD ALIGN=RIGHT>"<< simdate.get_DOY();
      harvest_htm_file << "<TD ALIGN=RIGHT>" << setprecision(3) << per_m2_to_per_ha(biomass_fate_today->canopy_before);  //070111
      harvest_htm_file << "<TD ALIGN=RIGHT>" << setprecision(3) << per_m2_to_per_ha(biomass_fate_today->yield);
      harvest_htm_file << "<TD ALIGN=RIGHT>" << setprecision(3) << per_m2_to_per_ha(biomass_fate_today->useful);
      harvest_htm_file << "<TD ALIGN=RIGHT>" << setprecision(3) << per_m2_to_per_ha(biomass_fate_today->get_removed_for_grazing());
      harvest_htm_file << "<TD ALIGN=RIGHT>" << setprecision(3) << per_m2_to_per_ha(biomass_fate_today->disposal);
      harvest_htm_file << "<TD ALIGN=RIGHT>" << setprecision(3) << per_m2_to_per_ha(biomass_fate_today->chaff_or_leaf_litter);
      harvest_htm_file << "<TD ALIGN=RIGHT>" << setprecision(3) << per_m2_to_per_ha(biomass_fate_today->stubble_dead);
      harvest_htm_file << "<TD ALIGN=RIGHT>" << setprecision(3) << per_m2_to_per_ha(biomass_fate_today->uncut_canopy);
      //We now just output harvested material, this is reported in the growing season reports
      harvest_htm_file << "<TD ALIGN=RIGHT>" << setprecision(1) << m_to_mm(GP_accumulators->balance_drainage),       //990215
      harvest_htm_file << "<TD ALIGN=RIGHT>" << setprecision(1) << m_to_mm(GP_accumulators->evapotranspiration_act);
      #ifdef NITROGEN
      const Crop_nitrogen_interface *crop_N_kg_m2 = crop->get_nitrogen();;
    if (crop_N_kg_m2)
     {
       float64 N_uptake =
                over_m2_to_ha *
                (  crop_N_kg_m2->get_mass_in_plant()
                 - crop_N_kg_m2->get_mass_of_seedling()
                 + GP_accumulators->gaseous_N_loss
                + crop_N_kg_m2->get_mass_removed_seasonal());                    //040913
        float64 N_leached =
         (soil &&  soil->chemicals && soil->chemicals->NO3 &&
               soil->chemicals->NO3->GS_bal_accums_M) ?
             per_m2_to_per_ha((soil->chemicals->NO3->GS_bal_accums_M->get_reported_leached(true))
                             +(soil->chemicals->NH4->GS_bal_accums_M->get_reported_leached(true))) : 0.0;
        harvest_htm_file << "<TD ALIGN=RIGHT>" <<setprecision(1) <<N_uptake;
        harvest_htm_file << "<TD ALIGN=RIGHT>" <<setprecision(6) <<N_leached;
      }
      #endif
      harvest_htm_file << "<TD>" << crop->get_description() << endl;             //030516
   }
   #ifndef __linux
/*200603
   if (
      //191104 verbose &&
      crop && crop->is_terminate_pending()) // currently only print on crop terminate dates  I think I only want to do this temporarily  //050114
*/
   {
      std::ifstream totfile(scenario_directory->CS_output_totals_txt_cstr);      //121121_040919_020503
      std::string totline;                                                       //121121
      if (totfile.good())                                                        //070513
      {
      #ifdef APPTYPE_CONSOLE
      cout << endl;
      cout << (TL_TOTALHEAD3) <<endl;
      cout << (TL_TOTALHEAD4) <<endl;
      cout << "様様塚様幼様様塚様幼様様様塚様様幼様様幼様様幼様様様様様様様様様様様様様様様" << endl;
      #endif
            while (! totfile.eof())
            {  getline(totfile,totline);
              #ifdef APPTYPE_CONSOLE
               cout << totline << endl;
              #endif
            }
         totfile.close();
      }
   }
   #endif
}
//_harvest_event____________________________________________________2012-05-04_/
void Land_unit_with_output_V4::write_headers()
{
   //120508 I haven't figured out yet why Qt gcc is not defining the reports
#ifdef XLS_OUTPUT
   if (DLY_report)            DLY_report->write_headers();
//200122 #  ifndef SOILR
   if (YLY_report)            YLY_report->write_headers();
   if (GS_report)             GS_report->write_headers();
   if (DLY_N_balance_report)  DLY_N_balance_report->write_headers(false);        //160326
   if (YLY_N_balance_report)  YLY_N_balance_report->write_headers(false);        //160326
   if (GS_N_balance_report)   GS_N_balance_report->write_headers(true);          //160326
//200122 #  endif
#endif
}
//_write_headers____________________________________________________2012-05-04_/
int harvest_row;

void Land_unit_with_output_V4::open_out_files()
{
   #if (defined(_DEBUG))
   scenario_directory->Output().create();                                        //141220
   #endif
   { // clear the totals.htm filename and write headers                          //000612
      const CropSyst::Soil_components *soil = ref_soil();                        //161008_060803
      std::string description                                                    //050916
         (scenario_control_and_model_options.description.brief);                 //151208
      std::ofstream totals_htm(scenario_directory->CS_output_harvest_htm_cstr);  //030516
      totals_htm<< "<H2>" << description <<  endl;                               //000630
      totals_htm<< "<BR>" << (soil                                               //030516
         ? (const char *) (soil->parameters_ownable->description.brief.c_str())  //030516
         : "soil not specified") << endl;                                        //030516
      totals_htm<< "<BR>" << description << "</H2>" << endl;                     //010202
      totals_htm<< "<TABLE BORDER=1>" << endl ;                                  //000613
      totals_htm<< "<TR><TH>" << (TL_Harvest) << "<BR>Year";
      totals_htm<< "<TH>" << (TL_Planting) ;                                     //041205
      totals_htm<< "<TH>" << "Restart" << "<BR>" << (TL_Dormancy_ends);          //041205
      totals_htm<< "<TH>"<< (TL_Emergence)    ;
      totals_htm<< "<TH>"<< (TL_Maturity)     ;
      totals_htm<< "<TH>"<< "Event<BR>date"    ;
      totals_htm<< "<TH>" << (TL_Dry_biomass) << "<BR>(before harvest or clipping)";   //070111
      totals_htm<< "<TH>" << "Yield Grain, tuber, leaf, fruit or root";
      totals_htm<< "<TH>" << "Removed and designated for beneficial use (clipped fodder, silage, etc.)";
      totals_htm<< "<TH>" << "Consumed by animals";
      totals_htm<< "<TH>" << "Removed from the field and disposed (unused)";
      totals_htm<< "<TH>" << "Remained in the field laying as surface residue<BR>(straw, chaff, etc.)";
      totals_htm<< "<TH>" << "Remained in the field as dead standing stubble/residue";
      totals_htm<< "<TH>" << "Remained live biomass<BR>(after harvest or clipping)";

//030517 now only in growing season report
      totals_htm<< "<TH>" << (TL_Soil_water_drainage);                           //990215
      totals_htm<< "<TH>" << ("ET act."/*190812 TL_ET_act*/);
#ifdef NITROGEN
      if (scenario_control_and_model_options.run_nitrogen)
      {  totals_htm <<"<TH>" << (TL_Total_nitrogen_uptake);
         totals_htm <<"<TH>" << (TL_Nitrogen_leached);
      }
#endif
      totals_htm<< "<TH>"  << endl;
      totals_htm
      << "<TR><TH>" << "year"
      << "<TH>" << "DOY"
      << "<TH>"<< "DOY"
      << "<TH>"<< "DOY"
      << "<TH>"<< "DOY"
      << "<TH>"<< "DOY"                                                          //070111
      << "<TH>" << (TU_kg_ha)
      << "<TH>" << (TU_kg_ha)
      << "<TH>" << (TU_kg_ha)
      << "<TH>" << (TU_kg_ha)
      << "<TH>" << (TU_kg_ha)
      << "<TH>" << (TU_kg_ha)
      << "<TH>" << (TU_kg_ha)
      << "<TH>" << (TU_kg_ha)
      << "<TH>" << (TU_mm)                                                       //990215
      << "<TH>" << (TU_mm);
#ifdef NITROGEN
    if (scenario_control_and_model_options.run_nitrogen)
      totals_htm<< "<TH>" << (TU_kgN_ha) << "<TH>" << (TU_kgN_ha);
#endif
      totals_htm<< "<TH>" << "Description" << endl;                              //040202
   }
// UED output database is created in the CropSyst simulation constructor
#if  defined(XLS_OUTPUT)
#  ifdef CROPSYST
   if (reports)                                                                  //020503
   {
      CORN::Date_const &start_date= scenario_control_and_model_options.start_date;
      CORN::Date_const &stop_date = scenario_control_and_model_options.stop_date;
      DLY_report =
      reports->DLY_options.num_vars_to_print                                     //000608
      ? new CropSyst_DLY_report
         (*this ,reports->DLY_options,                                           //020309
            scenario_directory->Output().daily_xls().c_str()                     //030110
         ,(nat16)(stop_date.days_between(start_date,true) + 5))                  //170525
      : 0;                                                                       //000608
      int year_detail_lines = (stop_date.get_year())-(start_date.get_year())+1;
      YLY_report =
      reports->YLY_options.num_vars_to_print                                     //000608
      ? new CropSyst_YLY_report
         (*this ,reports->YLY_options,(uint16)(year_detail_lines+6))             //020309
      : 0;                                                                       //000608
      GS_report =                                                                //030516
      reports->GS_options.num_vars_to_print                                      //030516
      ?new CropSyst_GS_report                                                    //030516
         (*this ,reports->GS_options,(uint16)(2*year_detail_lines+6))            //030516
      : 0;                                                                       //030516
      if (scenario_control_and_model_options.run_nitrogen)                       //990318
      {                                                                          //990318
         DLY_N_balance_report = 0;                                               //990318
         if (reports->DLY_options.nitrogen_budgets)                              //000608
            DLY_N_balance_report =new CropSyst_N_balance_report
            (scenario_directory->Output().daily_N_xls().c_str()                  //030110
            ,reports->DLY_options                                                //990318
            ,(nat16)(stop_date.days_between(start_date,true) + 5));              //990318
         YLY_N_balance_report = 0;                                               //990318
         if (reports->YLY_options.nitrogen_budgets)                              //000608
            YLY_N_balance_report = new CropSyst_N_balance_report
            (scenario_directory->Output().annual_N_xls().c_str()                 //030110
            ,reports->YLY_options                                                //990318
            ,(nat16)(2*year_detail_lines+6));                                    //990318
         GS_N_balance_report = 0;                                                //030516
         if (reports->GS_options.nitrogen_budgets)                               //000608
            GS_N_balance_report =new CropSyst_N_balance_report
            (scenario_directory->Output().season_N_xls().c_str()                 //030516
            ,reports->GS_options                                                 //990318
            ,(nat16)(2*year_detail_lines+6));                                    //990318
      }
   } // endif reports
// For RON H. dont delete yet
//   assign(chem_infiltrate_file,concat(extract_filename(filename),'.r'));
//   rewrite(chem_infiltrate_file);
//   assign(chem_et_file,concat(extract_filename(filename),'.et'));
//   rewrite(chem_et_file);

#ifdef SPATIAL
   assign(spatial_CN_file,concat(extract_filename(filename),'.CNP'));
   rewrite(spatial_CN_file);
#endif
#endif
#  endif
/*200122
#  ifdef SOILR
   DLY_report = new CropSyst_DLY_report_class (*this,report_format_filename,DLY_report_filename);
#  endif
*/
   harvest_row = 0;
   #ifdef _Windows
      detail_txt_file.open(                                                      //151128
         scenario_directory->Output().detail_txt().c_str());
      #ifdef DEBUG_CROP_N_FATE
         crop_N_fate_txt_file.open(scenario_directory->crop_N_fate_txt.c_str());
      #endif
   #endif
}
//_open_out_files______________________________________________________________/
void Land_unit_with_output_V4::close_out_files()
{  { std::ofstream harvest_htm                                                   //030516
      (scenario_directory->CS_output_harvest_htm_cstr,ios::app);
      harvest_htm << "</TABLE>" << endl;                                         //030516
   }
#if  defined(XLS_OUTPUT)
   delete DLY_report; DLY_report = 0;                                            //141220
#  ifdef CROPSYST
   delete DLY_N_balance_report;  DLY_N_balance_report = 0;                       //141220_990318
   delete YLY_N_balance_report;  YLY_N_balance_report = 0;                       //141220_990318
   delete GS_N_balance_report;   GS_N_balance_report  = 0;                       //141220_030516
   delete YLY_report;            YLY_report           = 0;                       //141220
   delete GS_report;             GS_report            = 0;                       //141220_030516
#     ifdef SPATIAL
  Close(spatial_CN_file);
#     endif
#  endif
#endif

   #ifdef _Windows
      detail_txt_file.close();                                                   //040604
      #ifdef DEBUG_CROP_N_FATE
         crop_N_fate_txt_file.close();                                           //070131
      #endif
   #endif
}
//_close_out_files__________________________________________________2012-05-04_/
#endif
#ifdef DETAIL_DENITR
std::ofstream &Land_unit_with_output_V4::provide_denitr_detail()      provision_
{
   if (!denitr_detail_stream)
   {
      CORN::OS::Directory_entry_name_concrete denitr_detail_filename
         (get_output_directory_name(),L"denitr_detail",L"dat");
      denitr_detail_stream = new std::ofstream(denitr_detail_filename.c_str());
   }
   return *denitr_detail_stream;
}
//_provide_denitr_detail____________________________________________2016-01-28_/
#endif
}//_namespace_CropSyst_________________________________________________________/

