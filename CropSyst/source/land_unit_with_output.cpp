
#include "organic_matter/OM_residues_profile_abstract.h"
#include "land_unit_with_output.h"
#include "UED/library/controls.h"
#include "crop/crop_interfaced.h"
#include "soil/chemicals_profile.h"
#include "soil.h"
#include "common/weather/weather.h"
#include "corn/data_source/vv_file.h"
#include "cs_scenario_directory.h"
#if (CROPSYST_VERSION==4)
// Actually probably obsolete all version,
// Now TDF files are discovered and CS Explorer offers export functions
#include "output.h"
#endif
#include "cs_ET.h"
#include "cs_simulation.h"
#include "crop/crop_cropsyst.h"
#ifdef NITROGEN
#include "soil/nitrogen_profile.h"
#endif
#include <iomanip>
#ifdef RUSLE_DETAIL
#  include "soil/erosion_RUSLE.h"
#include "weather/storms.h"
#endif
#include "csvc.h"
#include "soil/chemical_balance.h"
#include "CS_suite/observation/CS_optation.h"

using namespace std;
extern ofstream dbg_nitr;
extern float64 N2O_from_nitrification_daily;
#include "crop/canopy_growth_cover_based.h"
using namespace CropSyst;  // Not eventually everything will be under Cropsyst

#ifdef DUMP_CURVE_NUMBERS
namespace CropSyst
{
   extern std::ofstream *SCS_runoff_dat_file;                                    //131216
}
#endif
extern CS_UED_variable_code fast_graph_variables[];                              //160303
#ifdef CROPSYST_PROPER
extern bool harvest_event_failed_today ; // this is a check for Mike Rivingtons spreadsheet
#endif

namespace CropSyst {
//______________________________________________________________________________
Land_unit_with_output::Land_unit_with_output
(const CS::Identification           &LBF_ID_                                     //151026
, nat32                              cycle_shift_                                //151026
, CropSyst::Scenario                &scenario_control_and_model_options_         //130827
,const CORN::date32 &today_                                                      //170525
,const Geocoordinate                &geocoordinate_                              //151116
,const CS::Land_unit_meteorological &meteorology_                                //151128
#if (CS_VERSION==4)
,Common_simulation_log              &event_log_
#endif
,const CropSyst::Scenario_directory *scenario_directory_)                        //050619

: Land_unit_simulation
   (LBF_ID_
   ,cycle_shift_
   ,scenario_control_and_model_options_                                          //130827
   ,today_                                                                       //130619
   ,geocoordinate_                                                               //151116
   ,meteorology_
   #if (CS_VERSION==4)
   ,event_log_
   #endif
   ,scenario_directory_)
#ifdef EXTENDS_LAND_UNIT_OUTPUT
, Land_unit_output
   (*this
   ,LBF_ID_
   ,cycle_shift_
   ,scenario_control_and_model_options_
   ,today_
   ,geocoordinate_
   ,meteorology_
   #if (CS_VERSION==4)
   ,event_log_
   #endif
   ,scenario_directory_)
#else
//170315 The following have been moved to Land_unit_output
,harvest_UED_db         (0)
,harvest_data_src_rec   (0)
,season_UED_db          (0)
,season_data_src_rec    (0)
,annual_UED_db          (0)
,annual_data_src_rec    (0)
#ifndef CS_INSPECTOR_VERSION
,daily_UED_db           (0)  // NYI get this from the scenario                   //030715
,daily_UED_data_src     (0)                                                      //030715
,daily_data_rec         (*this)                                                  //030715
#endif
,output_flags           (OUTPUT_COMPLETE)                                        //151227
#endif // EXTENDS_LAND_UNIT_OUTPUT

, AgMIP_soil_water_observed                                                  (0) //181101
, AgMIP_soil_water_rootzone                                                  (0) //181101
, AgMIP_soil_N_observed                                                      (0) //181101
, AgMip_soil_N_rootzone                                                      (0) //181101
   // Currently only outputing to UED file
{
   if (!optation_global.has_any_daily())                                         //190204
   {  output_flags ^= OUTPUT_DAILY; }                                            //151227
   #if (CROPSYST_PROPER == 5)
   //160530 temporarily output everything for debugging
   output_flags |= OUTPUT_DAILY;
   #endif
}
//_Land_unit_with_output____________________________________________2004-10-21_/
bool Land_unit_with_output::stop()                                 modification_ //160306
{  bool stopped = Land_unit_simulation::stop();                                  //160306
   #ifdef EXTENDS_LAND_UNIT_OUTPUT
   //obsolete
   stopped &= Land_unit_output::stop();                                          //170315
   #else
   //170315 The following is moved to Land_unit_output
   std::string event_description("Simulation period");
   // if (simdate.get_date32/*latest_simulated_date*/ < scenario_control_and_model_options.stop_date.get_date32()) //110104
   if (simdate < scenario_control_and_model_options.stop_date)                   //110104
      event_description.append(" (ended prematurely)");
   datetime64 start_date64 = scenario_control_and_model_options
      .start_date .get_datetime64();                                             //110104
   datetime64 end_date64 = simdate.get_date32();                                 //181230_110104
   UED::Period_record simulation_period                                          //041012
      (event_description.c_str()
      ,(nat32 )(UED_period_start_date_option | UED_period_end_date_option |UED_period_application_code_option)
      ,start_date64
      ,end_date64 // use current date because the simulation may be prematurely terminated
      ,SIMULATION_PERIOD // appl code
      ,0 // no enumeration
      ,0 // no index
      ,0 // no variable code
      );
   #ifndef CS_INSPECTOR_VERSION
   if (daily_UED_db)
   {  daily_UED_db   ->set_period(simulation_period);
      daily_UED_db->finalize();
      delete daily_UED_db;
   }
   delete daily_UED_data_src;
   #endif
   if (harvest_UED_db)
   {  harvest_UED_db ->set_period(simulation_period);                            //041012
      delete harvest_UED_db;        harvest_UED_db = 0;
   }
   //200701 if (harvest_data_src_rec)
   delete harvest_data_src_rec;harvest_data_src_rec=0;
   if (season_UED_db)
   {  season_UED_db  ->set_period(simulation_period);                            //041012
      delete season_UED_db;         season_UED_db = 0;
   }
   delete season_data_src_rec;   season_data_src_rec = 0;
   if (annual_UED_db)
   {  annual_UED_db  ->set_period(simulation_period);                            //041012
      delete annual_UED_db;         annual_UED_db = 0;
   }
   delete annual_data_src_rec;   annual_data_src_rec = 0;
   #endif
   return stopped;                                                               //160306
}
//_stop________________________________________________________________________/
bool Land_unit_with_output::initialize()                         initialization_ //151227
{  bool inited = CropSyst::Land_unit_simulation::initialize();
   #ifdef EXTENDS_LAND_UNIT_OUTPUT
   inited = Land_unit_output::initialize();                                      //170315
   #else
   //170315 The following is moved to Land_unit_output
   #ifndef linux
   #if (defined(XLS_OUTPUT) || defined(UED_OUTPUT))
   if (scenario_control_and_model_options.fast_graph)                            //160303
   {  Optation_temporal &daily = optation_global.provide_temporal(UT_day);       //190204
      daily.variable_codes.append_array_null_terminated(fast_graph_variables);   //160303
         // null terminated                                                      //100702
   }
   #endif
   #endif
   if (!inited) std::cerr << "CropSyst::Land_unit_simulation::initialize failed" << std::endl;
   #if (CROPSYST_VERSION==4)
   // V5 now discovers TDF files and lists variables                             //190121
   {  // Open output options and get desired variables                           //041031
      Scenario_output_options output_options;                                    //041031
      VV_File output_opts_file                                                   //041031
       (scenario_control_and_model_options.output_options_filename_rel.c_str()); //130827_041031
         // NYI In version 5 the output options should be (optionally?)
         // specified on the command line (or taken from the project).
      output_opts_file.get(output_options);                                      //041031
      #if (defined(XLS_OUTPUT) || defined(UED_OUTPUT))
      output_options.list_required_variables();                                  //190204
      #endif
   }
   if (output_flags & OUTPUT_DAILY)                                              //170227_151227
   {
      CORN::OS::File_name_concrete daily_UED_filename_qual
         (get_output_directory_name(),"daily","UED");                            //160309
      #ifndef CS_INSPECTOR_VERSION
      daily_UED_db         = new CropSyst_UED_database
            (daily_UED_filename_qual.c_str()
            ,(std::ios_base::out)
            ,scenario_control_and_model_options.start_date);
      daily_UED_data_src   = new CS_UED_tuple_data_source(daily_UED_db);
      daily_UED_db->write_generating_application((nat16)CS_VERSION_CODE,"CropSyst"); //140105
      #endif
   }
   #endif
   #ifdef USE_CS_SIMULATION_ROTATOR
   // The simulation rotator doesn't write annual UED files for each scenario
   // The UED records are stored in memory and will be aggregated.               //140119
   annual_UED_db        = new UED::Database_file_indexed(0,(std::ios_base::out),false);
   CORN::OS::File_name_concrete season_UED_filename   (get_output_directory_name()  ,"season.UED"); //160305
   CORN::OS::File_name_concrete harvest_UED_filename  (get_output_directory_name() ,"harvest.UED"); //160305
   CORN::OS::File_name_concrete annual_UED_filename   (get_output_directory_name()  ,"annual.UED"); //160305

   harvest_UED_db = new UED::Database_file_indexed(harvest_UED_filename.c_str(),(std::ios_base::out),false); //160305
   season_UED_db  = new UED::Database_file_indexed(season_UED_filename .c_str(),(std::ios_base::out),false); //160305
   annual_UED_db  = new UED::Database_file_indexed(annual_UED_filename .c_str(),(std::ios_base::out),false); //160305

   #else
   harvest_UED_db       = new UED::Database_file_indexed(scenario_directory->CS_output_harvest_UED_cstr,(std::ios_base::out),false);
   season_UED_db        = new UED::Database_file_indexed(scenario_directory->CS_output_season_UED_cstr ,(std::ios_base::out),false);
   annual_UED_db        = new UED::Database_file_indexed(scenario_directory->CS_output_annual_UED_cstr ,(std::ios_base::out),false);
   #endif
   harvest_UED_db ->write_generating_application((nat16)CS_VERSION_CODE,"CropSyst");     //140105
   season_UED_db  ->write_generating_application((nat16)CS_VERSION_CODE,"CropSyst");     //140105
   annual_UED_db  ->write_generating_application((nat16)CS_VERSION_CODE,"CropSyst");     //140105

   harvest_data_src_rec = new Harvest_data_source(harvest_UED_db);
   season_data_src_rec  = new Season_data_source(season_UED_db);
   annual_data_src_rec  = new Annual_data_source(annual_UED_db);
   #ifdef NITROGEN
   Soil::Chemicals_profile *soil_chemicals = get_soil_chemicals();
   if (soil_chemicals)                                                           //161206
   {
      Chemical_balance_accumulators_molecular *NO3_GP_bal_accums                 //050114
         = (soil_chemicals->NO3)
         ? soil_chemicals->NO3->GS_bal_accums_M: 0;
      Chemical_balance_accumulators_molecular *NH4_GP_bal_accums                 //050114
         = (soil_chemicals->NH4)
         ? soil_chemicals->NH4->GS_bal_accums_M: 0;
      season_data_src_rec->N_soil_profile_initial                                //161130
         = NO3_GP_bal_accums->get_current_profile_content(true)
         + NH4_GP_bal_accums->get_current_profile_content(true);
   }
   #endif
   season_data_src_rec->soil_water_profile_init
      = soil->ref_hydrology()->provide_water_depth_profile(true);                //160412_151227
      // This is actually only needed for REACCH
   season_data_src_rec->soil_available_water_15_init
      = soil->ref_hydrology()->calc_available_water_depth_to(1.5);               //160601
      // Special output
   std::string weather_description;                                              //050916
/*NYI actually should get from weather file is possible  151208
       (scenario_control_and_model_options.provide_geolocation()->get_station_ID_code())
*/
   // Should be able to move this to land_unit_output_V4                         //160529
   #ifndef CS_INSPECTOR_VERSION
   #if (defined(XLS_OUTPUT) || defined(UED_OUTPUT))
   if (daily_UED_data_src)                                                       //170227_140105_111221
       daily_UED_data_src->register_fields
       (soil->layers->get_number_sublayers(),UT_arbitrary_date);                 //140105_041014
   #endif
   if (daily_UED_db)                                                             //111221
       daily_UED_db->initialize_with_descriptions                                //030715
         (scenario_control_and_model_options.description.brief                   //150112_130827
         ,weather_description
         ,soil->parameters_ownable->description.brief                            //150112
         ,scenario_control_and_model_options.fast_graph);
   #endif
   #endif  // EXTENDS_LAND_UNIT_OUTPUT
   return inited;
}
//_initialize__________________________________________________________________/
void Land_unit_with_output::harvest_event
(const CropSyst::Crop_model_interface &_crop_at_harvest)           modification_ //131008
{
   #ifdef EXTENDS_LAND_UNIT_OUTPUT
   // I think this is valid for V5
   Land_unit_output::harvest_event(_crop_at_harvest);
   #else
   // This is still used in V4
   //170315 The following is moved to Land_unit_output
   harvest_data_src_rec->update(_crop_at_harvest);                               //070228 was crop crop_active
   harvest_data_src_rec->record((datetime64)get_today_date32());
   #endif
}
//_harvest_event_______________________________________________________________/
#if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
nat32 Land_unit_with_output::perform_triggered_synchronizations_today()
{  nat32 performed = 0;
 #ifdef EXTENDS_LAND_UNIT_OUTPUT
   performed += 1;                                                               //200404
      //200404 Land_unit_output::perform_triggered_synchronizations_today();
 #else


//#ifdef CROPSYST_PROPER
/*141222 RLN this was broken (although corrected now)
 * It is not necessary to check every day if period changes
 * We can now output the periods (as recorded in phenology)
 * Once at the end of the season.
 * This hasn't been implemented yet so dont delete this until reimplemented
*/
   modifiable_ Crop_model_interface *crop_active = crop_active_or_intercrop;     //130719_131008
   if (crop_active)
   {
      const CropSyst::Phenology &phenology = crop_active->ref_phenology();
      bool planted_today =
         phenology.started_today((Normal_crop_event_sequence)NGS_PLANTING);      //151005

      if (planted_today) latest_growth_stage_entered = NGS_PLANTING;
      if ((!latest_sync_date) || planted_today)
         latest_sync_date = (datetime64)get_today_date32();
         // If we haven't had a period yet, then today is just the start of the period.
      nat32  period_options = UED_period_start_date_option | UED_period_end_date_option |UED_period_application_code_option|UED_period_index_option|UED_period_enumeration_option|UED_period_variable_code_option;
      if (!latest_sync_date) latest_sync_date = (datetime64)get_today_date32();
      // If we haven't had a period yet, then today is just the start of the period.
      Normal_crop_event_sequence current_growth_stage
         = phenology.get_growth_stage_sequence();                                //141222
      if ((latest_growth_stage_entered!=NGS_NONE)
           && (current_growth_stage != latest_growth_stage_entered))
      {
         std::string period_description(get_growth_stage_description
            (latest_growth_stage_entered,false,false));
         period_description.append(":");
         period_description.append(crop_active->get_description());
         UED::Period_record growthstage_period
            (period_description.c_str(),period_options,latest_sync_date
            ,simdate.get_datetime64(),GROWTH_STAGE_PERIOD
            ,latest_growth_stage_entered
            ,phenology.get_restart_date()                                        //181113
            ,CSVC_crop_base_growth_stage);                                       //140114_050208
         latest_sync_date = (datetime64)get_today_date32();
         #ifndef CS_INSPECTOR_VERSION
         if (daily_UED_db)                                                       //111221
             daily_UED_db->set_period(growthstage_period);
         #endif
         harvest_UED_db->set_period(growthstage_period);                         //041012
         season_UED_db->set_period(growthstage_period);                          //041012
      }
      latest_growth_stage_entered = current_growth_stage;                        //141222
   }
 #endif
   #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
   performed += 1;                                                               //200404
      //200404 CropSyst::Land_unit_simulation_VX::
      //200404 perform_triggered_synchronizations_today();
   #endif
   return performed;
}
//_perform_triggered_synchronizations_today_________________________2014-01-03_/
#endif
bool Land_unit_with_output::start()                                              //130620
{
   bool started = Land_unit_simulation_VX::start();                              //080929
   #ifdef EXTENDS_LAND_UNIT_OUTPUT
   started &= Land_unit_output::start();                                         //170315
   #endif
#ifdef NYI
080306 This was an attempt to output the initial data for annual reports,
but the output manager or export utility is not setup to process this.
   CORN::Date day_before_start(today); day_before_start.dec();
   DOY doy = today.get_DOY();
   if (doy == 1)
   {  // If the the simulation start date in Jan 1 then we can
      // output the initial values as if for the previous year
      // When the simulation is not started on Jan 1 outputting
      // initial values as if for the previous year would be confusing an misleading
      Year year = day_before_start.get_year();
      if (annual_data_src_rec)
      {  annual_data_src_rec->update(*get_soil() ,*ANNL_accumulators_active
            ,organic_matter_residues_profile_kg_m2 ? organic_matter_residues_profile_kg_m2->ref_residues() : 0
         ,organic_matter_residues_profile_kg_m2
               ,pond,LBF_ID,year );
         annual_data_src_rec->record(day_before_start.get_datetime64());
      }
   }
   // Currently only outputing initial values for annual outputs
   // Eventually we may add others.
#endif
   return started;                                                               //080929
}
//_start____________________________________________________________2008-03-06_/
// WARNING need var codes for these

UED::Data_record_creation_layout AgMIP_soil_water_observed_creation   (UED::Record_type_distinguisher_interface::UED_data_set_year_var_units,0xFFFFFF01,UC_m      ,UT_day,UT_year,simulated_quality);
UED::Data_record_creation_layout AgMIP_soil_water_rootzone_creation   (UED::Record_type_distinguisher_interface::UED_data_set_year_var_units,0xFFFFFF02,UC_m      ,UT_day,UT_year,simulated_quality);
// This soil water it total (not available)
UED::Data_record_creation_layout AgMIP_soil_N_observed_creation       (UED::Record_type_distinguisher_interface::UED_data_set_year_var_units,0xFFFFFF03,UC_kg_m2  ,UT_day,UT_year,simulated_quality);
UED::Data_record_creation_layout AgMip_soil_N_rootzone_creation       (UED::Record_type_distinguisher_interface::UED_data_set_year_var_units,0xFFFFFF04,UC_kg_m2  ,UT_day,UT_year,simulated_quality);

// These creations only only temporary until I have inspection
// in the old daily
//_AgMIP____________________________________________________________2018-11-01_/
bool Land_unit_with_output::end_day()
{  // We call Land_unit_simulation_VX:: end_day first because it update accumulators
   bool result = true;
   result &= Land_unit_simulation_VX::end_day();
   #ifdef EXTENDS_LAND_UNIT_OUTPUT
   result &= Land_unit_output::end_day();                                        //170315
   #else
   //170315 The following is moved to Land_unit_output
   #ifndef CS_INSPECTOR_VERSION
   if (daily_UED_data_src)                                                       //170227_111228
   {
      daily_UED_data_src->set_date(simdate.get_datetime64());                    //070228
      daily_UED_data_src->set(daily_data_rec);                                   //070228
      #if (CROPSYST_VERSION==4)
      // This was a special output for AgMIP project
      // If we want to retain it in V5 then use inspectors
      float32 AgMIP_soil_water_observed = 0.0;
      float32 AgMIP_soil_N_observed     = 0.0;
      float32 AgMIP_soil_water_rootzone = 0.0;                                   //181101
      float32 AgMip_soil_N_rootzone     = 0.0;
      float32 rootzone_depth = crop_active_or_intercrop
          ? crop_active_or_intercrop->param_max_root_depth_m()
          : 0;
      if (soil)
      {
         AgMIP_soil_water_observed = soil->ref_hydrology()->
            calc_water_depth_to_depth
            //181105 calc_available_water_depth_to
               (scenario_control_and_model_options.observation.H2O_depth);
         AgMIP_soil_water_rootzone =
            crop_active_or_intercrop
            ? soil->ref_hydrology()->calc_water_depth_to_depth(rootzone_depth)
            : 0.0;
         unmodifiable_ Soil::Nitrogen *soil_N = soil->ref_nitrogen();
         if (soil_N)
         {  nat8 observe_soil_N_layer = soil->layers->get_layer_at_depth
               (scenario_control_and_model_options.observation.N_depth);
            nat8 rootzone_soil_N_layer = soil->layers->get_layer_at_depth
               (rootzone_depth);
            nat8 layer_count = soil->layers->count();
            for (nat8 layer = 1; layer <= layer_count; layer++)
            {  float32 N_layer = soil_N->get_N_mass_kg_m2(layer);
               if (layer <= observe_soil_N_layer)
                  AgMIP_soil_N_observed += N_layer;
               if (crop_active_or_intercrop)
                  if (layer <= rootzone_soil_N_layer)
                     AgMip_soil_N_rootzone += N_layer;
            } // for layer
         } // if soil_Na
      } // if soil
      bool record_created = false;
      UED::Time_query_single_date time_query(simdate_raw);                       //181101
      CORN::Quality_clad  simulated_attribute(simulated_quality);
      CORN::Units_code AgMIP_soil_water_observed_units   = UC_m;
      CORN::Units_code AgMIP_soil_N_observed_units       = UC_kg_m2;
      CORN::Units_code AgMIP_soil_water_rootzone_units   = UC_m;
      CORN::Units_code AgMip_soil_N_rootzone_units       = UC_kg_m2;
      daily_UED_db->set_value                                                    //181101
         (AgMIP_soil_water_observed ,time_query ,AgMIP_soil_water_observed_units
         ,simulated_attribute ,AgMIP_soil_water_observed_creation
         ,record_created,true);
      daily_UED_db->set_value
         (AgMIP_soil_N_observed     ,time_query ,AgMIP_soil_N_observed_units
         ,simulated_attribute ,AgMIP_soil_N_observed_creation,record_created,true);
      daily_UED_db->set_value
         (AgMIP_soil_water_rootzone ,time_query ,AgMIP_soil_water_rootzone_units
         ,simulated_attribute ,AgMIP_soil_water_rootzone_creation,record_created,true);
      daily_UED_db->set_value
         (AgMip_soil_N_rootzone     ,time_query ,AgMip_soil_N_rootzone_units
         ,simulated_attribute ,AgMip_soil_N_rootzone_creation
         ,record_created,true);
      #endif
   }
   #endif
   #endif
   return result;
}
//_end_day__________________________________________________________2007-02-27_/
bool Land_unit_with_output::start_growing_season                                 //160324
(modifiable_ Crop_model_interface &crop_at_start_season)
{  bool started = Land_unit_simulation_VX::start_growing_season
      (crop_at_start_season);
   #ifdef EXTENDS_LAND_UNIT_OUTPUT
   started &= Land_unit_output::start_growing_season(crop_at_start_season);      //170315
   #else
   //170315 The following is moved to Land_unit_output
   if (season_data_src_rec && soil)                                              //160324
   {
      #ifdef NITROGEN
      Soil::Chemicals_profile *soil_chemicals = get_soil_chemicals();            //161130
      if (soil_chemicals)                                                        //161206
      {  Chemical_balance_accumulators_molecular *NO3_GP_bal_accums = (soil_chemicals->NO3) ? soil_chemicals->NO3->GS_bal_accums_M: 0;  //050114
         Chemical_balance_accumulators_molecular *NH4_GP_bal_accums = (soil_chemicals->NH4) ? soil_chemicals->NH4->GS_bal_accums_M: 0;  //050114
         season_data_src_rec->N_soil_profile_initial                             //161130
            = NO3_GP_bal_accums->get_current_profile_content(true)
            + NH4_GP_bal_accums->get_current_profile_content(true);
      }
      #endif
      season_data_src_rec->soil_water_profile_init                               //160324
         = soil->ref_hydrology()->provide_water_depth_profile(true);             //160412_160324
      season_data_src_rec->soil_available_water_15_init
         = soil->ref_hydrology()->calc_available_water_depth_to(1.5);            //160601
   }
   #endif
   return started;
}
//_start_growing_season_____________________________________________2016-03-23_/
bool Land_unit_with_output::end_growing_season
(Crop_model_interface &crop_at_end_season_)                                      //131008
{  bool result = true;
   #ifdef EXTENDS_LAND_UNIT_OUTPUT
   result &= Land_unit_output::end_growing_season
      #if ((CROPSYST_VERSION >= 5) || defined(INTERCROPPING)|| defined(OFOOT_VERSION))
      (crop_at_end_season_);
      #else
      ();
      #endif
   #else
   output_growing_season(crop_at_end_season_);                                   //130716
   #endif
   result &= Land_unit_simulation_VX::end_growing_season(crop_at_end_season_);   //130716
   #ifdef CROPSYST_PROPER
   harvest_event_failed_today = false;                                           //16020 This was in CS_mod.cpp
   latest_growth_stage_entered = NGS_NONE;                                       //041205
   #endif
   return result;                                                                //070321
}
//_end_growing_season__________________________________________________________/
bool Land_unit_with_output::output_growing_season
(const Crop_model_interface &crop_at_end_season_)                                //131008
{  bool only_output_growing_season_on_harvest_date                               //170820
      = crop_at_end_season_.get_harvested_part()==tuber_part;                    //170820
   if (only_output_growing_season_on_harvest_date                                //170820
       && !(crop_at_end_season_.ref_phenology().is_harvested())) return false;   //170820
#ifdef EXTENDS_LAND_UNIT_OUTPUT
   Land_unit_output::output_growing_season(crop_at_end_season_);
#else
   datetime64 planting_or_restart_date                                           //041205
      = crop_at_end_season_.ref_phenology().get_planting_date();                                                    //181113

   /* NYI Output each of the stages
   for each growth_stage
   if the stage start date of growth_stage is valid (started)
   {
      nat32  period_options = UED_period_start_date_option | UED_period_end_date_option |UED_period_application_code_option|UED_period_index_option|UED_period_enumeration_option|UED_period_variable_code_option;
      //if (!latest_sync_date) latest_sync_date = (datetime64)get_today_date32(); // If we haven't had a period yet, then today is just the start of the period.
      //Normal_crop_event_sequence current_growth_stage = crop_active->ref_phenology().get_growth_stage_sequence(); //141222
      {
         std::string period_description(get_growth_stage_description(latest_growth_stage_entered,false)); //140103 NYI crop_active-> parameters_ownable->is_fruit_tree()));
         period_description.append(":");
         period_description.append(crop_active->get_description());
         UED::Period_record growthstage_period
            (period_description.c_str()
            ,period_options
            ,growth stage start date
            ,growth stage end date
            ,GROWTH_STAGE_PERIOD,latest_growth_stage_entered,crop_active->get_restart_date().get_date32(),CSVC_crop_base_growth_stage); //140114_050208
         latest_sync_date = (datetime64)get_today_date32();
         #if (defined(_DEBUG))
         if (daily_UED_db)                                                       //111221
         daily_UED_db->set_period(growthstage_period);
         #endif
         harvest_UED_db->set_period(growthstage_period);                         //041012
         season_UED_db->set_period(growthstage_period);                          //041012
      }
   }
   */

   UED::Period_record season_period                                              //041012
      (crop_at_end_season_.get_description() // "Growing season"
      ,UED_period_start_date_option      |UED_period_end_date_option
      |UED_period_application_code_option|UED_period_index_option
      |UED_period_variable_code_option   |UED_period_variable_code_option
      ,planting_or_restart_date                                                  //041205
      ,simdate.get_datetime64()                                                  //140105_070227
      ,GROWING_SEASON_PERIOD
      ,0 // no enumeration
      ,crop_at_end_season_.ref_phenology().get_restart_date()                    //181113
      ,CSVC_crop_base_name);  // NO CSVC
   //output growing season period to UED files
   #if (CROPSYST_VERSION >= 5)
   // I want to output the  management file/designation
   #endif
   // Not sure why this is crashing for REACCH //160309
   #ifndef CS_INSPECTOR_VERSION
   if (daily_UED_db)                                                             //111221
       daily_UED_db->set_period(season_period);
   #endif
   harvest_UED_db->set_period(season_period);                                    //041012
   season_UED_db->set_period(season_period);                                     //041012
   annual_UED_db->set_period(season_period);                                     //041012
   #ifdef CHEM_PROFILE
   Soil::Chemicals_profile *soil_chemicals = get_soil_chemicals();
   #endif
   #ifdef NITROGEN
   Chemical_balance_accumulators_molecular *NO3_GP_bal_accums                    //050114
      = (soil_chemicals && soil_chemicals ->NO3)
       ? soil_chemicals->NO3->GS_bal_accums_M: 0;
   Chemical_balance_accumulators_molecular *NH4_GP_bal_accums                    //050114
      = (soil_chemicals && soil_chemicals ->NH4)
       ? soil_chemicals->NH4->GS_bal_accums_M: 0;
   #endif
   season_data_src_rec->update
      ((crop_at_end_season_)
      ,*soil
      ,GP_accumulators_active
      #ifdef NITROGEN
      ,NO3_GP_bal_accums                                                         //050114
      ,NH4_GP_bal_accums                                                         //050114
      #endif
      ,pond
      ,(organic_matter_residues_profile_kg_m2
         ? organic_matter_residues_profile_kg_m2->mod_residues() : 0)
      ,ID.number);                                                               //151026
   season_data_src_rec->record(simdate_raw);
#endif
   return true;                                                                  //170820
}
//_output_growing_season____________________________________________2005-01-14_/
bool Land_unit_with_output::end_year()
{  bool result = Land_unit_simulation_VX::end_year();
   #ifdef EXTENDS_LAND_UNIT_OUTPUT
   result &= Land_unit_output::end_year();
   #else
   if (annual_data_src_rec)                                                      //050114
   {  annual_data_src_rec->update(soil,*ANNL_accumulators_active                 //050114
         ,organic_matter_residues_profile_kg_m2
            ? organic_matter_residues_profile_kg_m2->mod_residues() : 0          //050726
         ,organic_matter_residues_profile_kg_m2
         ,pond,ID.number,simdate.get_year());                                    //151026
      annual_data_src_rec->record(simdate.get_datetime64());                     //140224_050114
   }
   #endif
   return result;                                                                //111031
}
//_end_year_________________________________________________________2001-02-02_/
bool Land_unit_with_output::start_year()                           modification_
{  // Note that this isn't necessarily Jan 1, so we need to use the current date
   bool started = true;
   #ifdef EXTENDS_LAND_UNIT_OUTPUT
   started &= Land_unit_output::start_year();                                    //170315
   #else
   #ifndef CS_INSPECTOR_VERSION
   if (daily_UED_db)                                                             //111221
   {  soil->output_layering            (*daily_UED_db);
      soil->output_hydraulic_properties(*daily_UED_db); // This was added for Marcello and Andrea Di Guardo  //070209
   }
   #endif
   #ifdef CHEM_PROFILE
   Soil::Chemicals_profile *soil_chemicals = get_soil_chemicals();
   #ifdef NITROGEN
   Soil::NO3_Profile *NO3 = soil_chemicals ? soil_chemicals->NO3 : 0;
   Soil::NH4_Profile *NH4 = soil_chemicals ? soil_chemicals->NH4 : 0;
   if (NO3 && NH4)                                                               //020529
   {  if (NO3) NO3->update_YLY_bal_reporter();
      if (NH4) NH4->update_YLY_bal_reporter();
   }
   // reporters must be updated before we reset accumulators in start_of_year
   #endif
   #endif
   #endif
   started &= Land_unit_simulation_VX::start_year();
   return started;
}
//_start_year_______________________________________________________2004-10-14_/
}//_namespace CropSyst_________________________________________________________/

