#include "cs_accum.h"
#include "static_phrases.h"
#include "cs_mod.h"
#include "model_options.h"
#include "cs_vars.h"
#include "csvc.h"
#include "options.h"
#include "crop/crop_interfaced.h"
#if (CROPSYST_VERSION==4)
#  include "land_unit_output_V4.h"
#else
#  include "land_unit_output.h"
#endif
#include "soil/erosion_RUSLE.h"
#ifndef LADSS_UNIX
#  include "corn/data_source/dbase_datasrc.h"          // needed
#endif
#  include "cs_operation.h"
#  ifdef CHEM_PROFILE
#     include "soil/chemical_profile.h"
#     include "soil/chemicals_profile.h"
#  endif
#  ifdef NITROGEN
#     include "soil/nitrogen_profile.h"
#  endif
# if (CROPSYST_VERSION==4)
#  include "cs_report_V4.h"
# endif
#  include "csprofss.h"
#  include "crop/growth_stages.hpp"
#  include "cs_scenario_directory.h"
#  include "common/simulation/log.h"
#  include "project/GIS/GIS_table.h"
#ifndef compareHPP
#  include "corn/math/compare.hpp"
#endif
#     include "soil/temperature_hourly.h"
#  include "organic_matter/OM_residues_profile_abstract.h"
#include "crop/phenology_I.h"
#include "CS_suite/observation/CS_optation.h"
#include "common/geodesy/geolocation_item.h"
#ifdef CROPSYST_PROPER
#ifdef DETAIL_DENITR
extern std::ofstream *denitr_detail_stream = 0; // options
#endif
#endif
//______________________________________________________________________________
#ifdef _Windows
/*WINCON  */
extern void click_close_message(bool also_abort);
#ifdef DETAIL_FILE
extern ofstream detail_txt_file;
#endif
#ifdef DUMP_CURVE_NUMBERS
// special output for Rolf Sommer
extern std::ofstream *SCS_runoff_dat_file;                                       //131216
#endif
#endif
#ifdef DEBUG_ANNUAL_CARBON_MASS
ofstream annual_carbon_mass("annual_carbon_mass.dat");
#endif
#ifdef SOIL_TEMP_DEBUG
ofstream soil_temp_hourly;
#endif

#if defined(__MSDOS__) || defined(_Windows)
#  include <conio.h>  // for kbhit
#endif

nat32 today_for_debug_output;

using namespace CORN;
using namespace std;

// 970509 Warning these need to go in to simfile!!!!!
/*
#ifdef SOILR
bool SoilR_trim_to_residue       = true;
#endif
*/

#ifdef _Windows
#define NEW_DATA_AVAILABLE 0xE101
// This is just an arbitrary message value
// If we change this here, we need to change it in UED viewer
#endif

//050118 The following are quick and dirty output only!!!!
#ifdef OUMAROU_GRAZING
ofstream QND;
float32 QND_daily_grazed_BM_kg_m2 = 0;
float32 QND_plant_N_grazed_kg_m2 = 0;
float32 QND_plant_N_ungrazed_kg_m2 = 0;
float32 QND_inc_ungrazed_BM_kg_m2 = 0;  // can't get this  (use Inc_plant BM)
float32 QND_inc_plant_N_kg_m2 = 0;
float32 QND_inc_manure_BM_kg_m2 = 0;
float32 QND_inc_manure_N_kg_m2 = 0;
float32 QND_manure_org_N_appl_kg_m2 = 0;
float32 QND_manure_ammonia_N_inc_kg_m2 = 0;
float32 QND_manure_ammonia_N_loss_kg_m2 = 0;
float32 QND_plant_N_uptake_kg_m2 = 0;
#endif
float32 QND_mineralization_N_kg_m2 = 0;
float32 QND_immobil_N_kg_m2 = 0;
#pragma data_seg(".CropSyst_UI_notification")
uint32   UI_notify_date;      //
#pragma data_set()
//______________________________________________________________________________
extern CS_UED_variable_code fast_graph_variables[];                              //090615
//______________________________________________________________________________
namespace CropSyst_V4 {                                                          //151003
//______________________________________________________________________________
CropSyst_Model::CropSyst_Model
(CS::Arguments &arguments_)                                                      //200815

//200815 (Command_options_data_source &command_line_options_)                             //041030
: CropSyst_V4::Cropping_system_simulation                                        //151003_030616
   (
   scenario_control_and_model_ops                                               //151203
   //200815    ,command_line_options_
   ,arguments_                                                                   //200815
   )                                                       //160829
//200815 , ID                                                                         (0) //160312

//200815 , arguments                                            (ID,L"CropSyst_scenario") //200525_160312
, scenario_directory                           (arguments.start_directory,false) //200525
, meteorology                                                                (0)
   // instanciated in initialize                       //160311
, land_unit_with_output                                                      (0)
   // instanciated in initialize
,control_file_name_unqual(L".CropSyst_scenario")                                 //151203
#ifdef LISTVARS
,list_variables(false)
#endif
, scenario_control_and_model_ops(false)                                          //160311
,active_model_options(scenario_control_and_model_ops)                            //020531
,days_not_viable(0)                                                              //040826
,geocoordinate()                                                                 //160311_151208
,GIS_override_table(0)                                                           //010516
,GIS_record(0)                                                                   //020310
{
#ifdef unix
#ifdef LADSS_MODE
   //Finite difference not currently implemented in LADSS unix version; //041212
   _active_model_options.infiltration_model_labeled.set(CASCADE_DAILY_INFILTRATION);
#endif
#endif
}
//_CropSyst_Model:constructor_______________________________________2001-02-02_/
Common_operation *CropSyst_Model::get_operation(const char *operation_type_str
      ,const std::string  &operation_ID  // filename may be updated with qualification so not const (does not apply to cropsyst)
      ,modifiable_ CORN::OS::File_name &operation_filename  // filename may be updated with qualification so not const (does not apply to cropsyst)
      ,int32 resource_set_ID,bool &relinquish_op_to_event)
{  return land_unit_with_output->scenario_control_and_model_options.get_operation(operation_type_str
      ,operation_ID  // filename may be updated with qualification so not const (does not apply to cropsyst)
      ,operation_filename                                                        //041215
      ,resource_set_ID,relinquish_op_to_event);
}
//_get_operation:constructor________________________________________2003-09-12_/
#ifdef TEMP_DISABLED
//130828  I Need reimplement this of USE_OS_FS
#ifndef __linux
// I haven't yet implemented this for __linux, anyway in V5 GIS methods will probably be implemented as a derived class
//LADSS_Cropsyst_model overrides this function in LADSS_mode
void CropSyst_Model::get_parameters_from_GIS_table
(const char *GIS_override_table_name,uint32 SCENARIO_or_LBF_ID)
{    // If we are loading data from an Arccs table we need to override
   // some  simulation parameters and some soil parameters
#ifndef LADSS_UNIX
   Dynamic_int32_indexed_dBase_data_source *dbase_GIS_override_table
      = new Dynamic_int32_indexed_dBase_data_source
      (GIS_override_table_name
      ,"scenario_ID"                                                             //050919
      ,std::ios_base::in);                                                       //111026
   GIS_override_table = dbase_GIS_override_table;
   if (dbase_GIS_override_table->find_record(SCENARIO_or_LBF_ID))                //050921
   {  Polygon_simulation_parameters GIS_parameter_overrides_default_options(true);
      GIS_record = new CropSyst::Simulation_override_record(SCENARIO_or_LBF_ID,GIS_parameter_overrides_default_options,0); //130621
      GIS_override_table->get(*GIS_record); // get the override simulation parameters
      land_unit_with_output.scenario_control_and_model_options.apply_simulation_override_record(*GIS_record);
   } else                                                                                                                   //050921
      cerr << "Unable to find scenario/LBF ID:" << SCENARIO_or_LBF_ID << " in table:" << GIS_override_table_name << endl;   //050921
   active_model_options.fast_graph = false; // We need to disable fast graph (Daily output slows things down).
// WARNING     Check to make sure the ET_weather weather filename directory and prefix update is working
#endif
}
#endif
//_get_parameters_from_GIS_table_________________________________________2001?_/
#endif
bool CropSyst_Model::initialize_soil()
{
   land_unit_with_output->load_soil();                                           //020620
   // Must be called after load_soil because we need soil num sublayer
   CropSyst::Soil_components *soil = land_unit_with_output->mod_soil();          //161008_060803
   Smart_soil_parameters *soil_parameters_modifiable                             //161008
      = const_cast<Smart_soil_parameters *>(soil->parameters_ownable);
   if (GIS_override_table)                                                       //010516
   {
      GIS_override_table->get(*soil_parameters_modifiable);
         // get the override soil parameters                                     //010516
      // This must be called after load_soil, because setup soil
      // loads the soil parameters (defaults)
   }
   #ifndef LADSS_UNIX
   if (GIS_record)                                                               //050921
   {  if (!CORN::is_zero<float32>(GIS_record->parameter_overrides.init_PAW))     //050921
         soil->initialize_water_content_from_PAW(GIS_record->parameter_overrides.init_PAW); //050921
//                NO3 setup must come before LBF::initialize
         #ifdef NITROGEN
         if (!CORN::is_zero<float32>(GIS_record->parameter_overrides.init_NO3))
         {  // If NO3 mapped, with assume the init NO3 is all in layer 2 (or one if only 1 layer)
            for (int layer = 1; layer <= MAX_soil_horizons; layer++)
               land_unit_with_output->scenario_control_and_model_options.horizon_N_NO3[layer] = 0;
            land_unit_with_output->scenario_control_and_model_options.horizon_N_NO3[(soil_parameters_modifiable->horizon_count > 1) ? 2 : 1] = GIS_record->parameter_overrides.init_NO3;
         }
         #endif
         #ifdef PHOSPHORUS
         110801 NYI
           for (int layer = 1; layer <= MAX_soil_horizons; layer++)
                 land_unit_with_output.horizon_P[layer] = 0;
           land_unit_with_output.horizon_P[(soil->parameters_ownable->number_horizons > 1) ? 2 : 1] = GIS_record->parameter_overrides.init_P;
         #endif
      // This must be called after load_soil, because setup soil
      // loads the soil parameters (defaults)
      // This was added for Manual Lopez Vicente
      // here we presume a single soil horizon (uniform soil texture) 050505
      if (GIS_record->parameter_overrides.uniform_sand != 0)   soil_parameters_modifiable->horizon_sand[1] = GIS_record->parameter_overrides.uniform_sand ; //050921
      if (GIS_record->parameter_overrides.uniform_clay != 0)   soil_parameters_modifiable->horizon_clay[1] = GIS_record->parameter_overrides.uniform_clay ; //050921
      if ((GIS_record->parameter_overrides.uniform_sand != 0) ||
         (GIS_record->parameter_overrides.uniform_clay != 0))  soil_parameters_modifiable->horizon_silt[1] = 100 - GIS_record->parameter_overrides.uniform_sand - GIS_record->parameter_overrides.uniform_clay ; //050505
      if (GIS_record->parameter_overrides.soil_depth_cm > 0)   soil_parameters_modifiable->horizon_thickness[1] =  cm_to_m(GIS_record->parameter_overrides.soil_depth_cm); //050505
      // Soil is initialized below so we should be able to simply modify the parameter here. 050505
      #ifdef STATSGO_DATABASE_obsolete
      // Don't delete may want to replace with STATSGO2
      if (GIS_record->parameter_overrides.MUID.length() && GIS_record->parameter_overrides.SEQNUM && STATSGO_database)  //050921
      {                                                                          //060213
         Smart_soil_parameters_with_STATSGO *soil_parameters = (Smart_soil_parameters_with_STATSGO *)soil->parameters_ownable;  //060213
         soil_parameters->read_available_STATSGO_data                            //060213
         (*STATSGO_database,                                                     //050921
          GIS_record->parameter_overrides.MUID.c_str()                           //050921
         ,GIS_record->parameter_overrides.SEQNUM                                 //050921
         ,100.0 // NYN COMPPCT                                                   //080303
         ,0.05  ); //m 070821 Now just a constant (so we dont have to load GIS parameters), //051201
            //may want to move this to scenario (template) project.GIS_project_paramsX.Xmax_evap_layer_thickness_m
      }                                                                         //060213
      #endif
  }
  #endif
  return true;
}
//_initialize_soil__________________________________________________2013-11-13_/
bool CropSyst_Model::initialize()                                initialization_
{  bool initialized = true;
   initialized &= Cropping_system_simulation::initialize();                      //151203
   // Now that we know the scenario directory we can purge output
   scenario_directory                                                            //200525
   //200525 arguments.provide_scenario_directory()
   .purge_output();
   if (arguments.
      //200813 provide_scenario_file_name()
      start_directory                                                            //200813
      .has_in_path_cstr("AgMIP"))                                                //160311
      initialized &= desire_AgMIP_variables();
   optation_global.desire(CSVC_weather_ET_ref_accum_to_date_time,UT_day);        //190204
   std::string ISO_6709_unused;                                                  //161129_160111
   provide_context_discovery();                                                  //170703

   geodesy::Geocoordinate_record geocoord_rec("???");                            //191010
   geocoord_rec.know(&geocoordinate);                                            //191010
   bool geocoordinate_discovered = context_discovery->get_geolocation            //151207
      (arguments.start_directory                                                 //200525
       //200525 arguments.provide_scenario_directory()
      ,"location"
      ,geocoord_rec
      ,ISO_6709_unused);                                                         //160111
   meteorology                                                                   //151202_151128
      = new CS::Land_unit_meteorological
      (simdate_raw                                                               //170525
      ,geocoordinate
      ,active_model_options.snow_pack_model_clad.get());
   land_unit_with_output = new CropSyst::Land_unit_with_output_V4                //151204
      (arguments.ref_ID()                                                        //151128
      ,(nat32)0 //V4 CropSyst proper does not have cycle shifting
      ,scenario_control_and_model_ops
      ,simdate_raw                                                               //170524
      ,geocoordinate
      ,*meteorology
      #if (CS_VERSION==4)
      ,CS_event_log
      #endif
      //200525 ,&(arguments.provide_scenario_directory())
      ,&scenario_directory                                                       //200525
      ,arguments.
         V4_format_filename_discovery->provide_primary()                            //200813
         //200813 V4_format_filename
      ,true);
   add_land_block_fragment(land_unit_with_output);                               //030616

#ifdef DEBUG_ANNUAL_CARBON_MASS
annual_carbon_mass << "year\t SOM_C_mass_30cm \t SOM_C_mass_profile \t  microbial_C_mass_30cm \t microbial_C_mass_profile \t labile_C_mass_30cm \t labile_C_mass_profile  \t metastable_C_mass_30cm  \t metastable_C_mass_profile  \t passive_C_mass_30cm \t passive_C_mass_profile" << endl;
#endif
   Location_parameters *location_loaded_by_LADSS = load_location_parameters();   //030911
   if (GIS_override_table && location_loaded_by_LADSS)                           //010516
      GIS_override_table->get(*location_loaded_by_LADSS);                        //010516
   #if (CS_VERSION==4)
   const char *location_or_UED_filename =                                        //070213
      #ifdef __linux__
      land_unit_with_output->scenario_control_and_model_options.weather_filename.c_str();//130808
      #else
      location_loaded_by_LADSS                                                   //070213
      ? location_loaded_by_LADSS->weather_filename_V3.c_str() :                  //030911
       (CORN::OS::file_system_engine.exists(land_unit_with_output->scenario_control_and_model_options.weather_filename) //130828
         && land_unit_with_output->scenario_control_and_model_options.weather_filename.has_extension_ASCII("UED",false)) //130828
         ? land_unit_with_output->scenario_control_and_model_options.weather_filename.c_str()   //050916
         : land_unit_with_output->scenario_control_and_model_options.location_filename.c_str(); //030911
      #endif
   #endif

//std::clog << "weather filename" << location_or_UED_filename << std::endl;
   initialized &= setup_location
      (land_unit_with_output                                                     //151204_030911
      ,geocoordinate                                                             //151208
      #if (CS_VERSION==4)
      ,location_or_UED_filename                                                  //030911
      #else
      ,land_unit_with_output.weather_filename                                    //110104
      #endif
      ,location_loaded_by_LADSS                                                  //030911
      ,active_model_options.extend_available_weather                             //150218
      ,(Snow_pack_model)active_model_options.snow_pack_model_clad.get()          //030911
      ,0 //131113 ,soil->parameters_ownable->get_steepness_percent()             //030911
         //131113 disabled, need to load soil parameter file files
      ,true
      // If any of the hourly simulation models are selected, we need hour weather. 041111
      );
   initialized                                                                   //170703
      &= initialize_soil()                                                       //131113
      && meteorology->initialize();                                              //151129

   // can't list the required variables until we know the number of sublayers
   CS_event_log.open(land_unit_with_output->scenario_directory->
      Output().schedule_txt().c_str());                                          //051019
#ifndef __linux
#ifdef SOIL_TEMP_DEBUG
   soil_temp_hourly.open(land_unit_with_output.scenario_directory->output().
        soil_temp_hourly_dat.c_str());                                           //041114
#endif
#endif
   if (!CORN::global_arguments->verbosity)                                       //200317
   {  active_model_options.fast_graph = false;
      active_model_options.display_profile = false;
      //191104 obs display_full = false;
   }
#ifdef TXT_OUTPUT
                           output_file << (T_date_Date) << ',';
               if (crop)   crop->txt_header_output(output_file);
                           ET_weather.txt_header_output(output_file);
                           soil->txt_header_output(output_file);
                           output_file << endl;
                           output_file << soil->num_sublayers << endl;
#endif
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
      CORN::date32 period_start = 0;
      CORN::date32 period_end = 0;
      detail_denitr_dates_file >> period_start >> period_end;
      if (period_start && period_end)
         detail_denitr_dates.append(new Detail_denitr_date_range(period_start,period_end));
   }
}
#endif
   initialized &= land_unit_with_output->initialize();                           //160311
   return initialized;                                                           //010207
}
//_initialize__________________________________________________________________/
void CropSyst_Model::setup_initial_soil_profile_recalibrations()
{
   // We now allow recalibration to initial soil profile values
   for (Year year =  land_unit_with_output->scenario_control_and_model_options.start_date.get_year()
       ;     year <= land_unit_with_output->scenario_control_and_model_options.stop_date .get_year()
       ;     year++)
   {
      if (land_unit_with_output->scenario_control_and_model_options.H2O_recalibrate_date.get_date32())   //130828
      {  CORN::Date_clad_32 act_recal_date(year,land_unit_with_output->scenario_control_and_model_options.H2O_recalibrate_date.get_DOY());
         CropSyst::Recalibration_operation *recalib_op = new CropSyst::Recalibration_operation(CropSyst::Recalibration_operation::WC_RECAL,land_unit_with_output->scenario_control_and_model_options.horizon_H2O);   //020310
         land_unit_with_output->scenario_event_list.append(new Common_event(act_recal_date,recalib_op,true));
      }
      if (land_unit_with_output->scenario_control_and_model_options.N_NO3_recalibrate_date.get_date32()) //130828
      {  CORN::Date_clad_32 act_recal_date(year,land_unit_with_output->scenario_control_and_model_options.N_NO3_recalibrate_date.get_DOY());
         CropSyst::Recalibration_operation *recalib_op = new CropSyst::Recalibration_operation(CropSyst::Recalibration_operation::NO3_RECAL,land_unit_with_output->scenario_control_and_model_options.horizon_N_NO3);   //020310
         land_unit_with_output->scenario_event_list.append(new Common_event(act_recal_date,recalib_op,true));
      }
      if (land_unit_with_output->scenario_control_and_model_options.N_NH4_recalibrate_date.get_date32())
      {  CORN::Date_clad_32 act_recal_date(year,land_unit_with_output->scenario_control_and_model_options.N_NH4_recalibrate_date.get_DOY()); //130828
         CropSyst::Recalibration_operation *recalib_op = new CropSyst::Recalibration_operation(CropSyst::Recalibration_operation::NH4_RECAL,land_unit_with_output->scenario_control_and_model_options.horizon_N_NH4);   //020310
         land_unit_with_output->scenario_event_list.append(new Common_event(act_recal_date,recalib_op,true));
      }
      #ifdef PHOSPHORUS
      /*110801 NYI
      if (land_unit_with_output.P_recalibrate_date.get())
      {  CORN::Date act_recal_date(year,land_unit_with_output.P_recalibrate_date.get_DOY());
         Recalibration_operation *recalib_op = new Recalibration_operation(Recalibration_operation::P_RECAL,land_unit_with_output.horizon_P);   //020310
         land_unit_with_output.event_list.append(new Common_event(act_recal_date,recalib_op,true));
      }
      */
      #endif
      if (land_unit_with_output->scenario_control_and_model_options.POM_recalibrate_date.get_date32())
      {  CORN::Date_clad_32 act_recal_date(year,land_unit_with_output->scenario_control_and_model_options.POM_recalibrate_date.get_DOY());
         CropSyst::Recalibration_operation *recalib_op = new CropSyst::Recalibration_operation
            (CropSyst::Recalibration_operation::POM_RECAL
            ,land_unit_with_output->scenario_control_and_model_options.horizon_percent_organic_matter); //020310
         land_unit_with_output->scenario_event_list.append(new Common_event(act_recal_date,recalib_op,true));
      }
      // Eventually we can add salinity
   }
}
//_setup_initial_soil_profile_recalibrations________________________2001-11-16_/
bool CropSyst_Model::start()                                       modification_
{ // Start recording to output file
//dbg_denitr <<  "Hour" << '\t' << "N_denitr"<< '\t' <<  "pot_denitr_const" << '\t' << "denitr_factor" << '\t' << "moist funct" << '\t' << "respr_rspns_funct" << '\t' << "NO3_response_funct" << endl;

#ifdef OUMAROU_GRAZING
CORN::File_name QND_txt_filename(land_unit_with_output.scenario_directory->detail_txt.c_str());
QND_txt_filename.set_file("qnd");         // was daily_biomass.txt
QND.open( QND_txt_filename.c_str());
// output header line
QND
<<"date\t"
<<"grazed BM (kg/ha)\t"
<<" N grazed (kg/ha)\t"
<<" N ungrazed (kg/ha)\t"
<<" incorp ungrazed_BM (kg/ha)\t"
<<" incorp plant_N (kg/ha)\t"
<<" incorp manure_BM (kg/ha)\t"
<<" incorp manure_N (kg/ha)\t"
<<" manure org_N (kg/ha)\t"
<<" manure ammonia N incorp (kg/ha)\t"
<<" manure ammonia N loss (kg/ha)\t"
<<" mineralization N (kg/ha)\t"
<<" immobil N (kg/ha)\t"
<<" plant N uptake (kg/ha)\t" << endl;
#endif

//Note, this start_date set is repeated from simulation::start because we need the date set to initialize which needs to be done first;
   bool started = CropSyst_V4::Cropping_system_simulation::start();              //151003_150501_070625
   if (started)                                                                  //030911
   {                                                                             //030911
         #ifdef PRINT_PROFILE
         Report_formats_parameters *reports = land_unit_with_output->reports;
         bool generate_profile_spreadsheet =
            reports
         && ((reports->profile_spreadsheet_timestep > 0)
             || (reports->at_planting)                                           //000331
             || (reports->at_emergence)                                          //000331
             || (reports->at_flowering)                                          //000331
             || (reports->at_maturity)                                           //000331
             || (reports->at_harvest)                                            //000331
             ||  reports->num_profile_print_dates);                              //000526

         /*151203 This should be reenabled for using the new CS::Simulation_engine
         if ( generate_profile_spreadsheet )                                     //000331
           setup_profile_print_events(reports->profile_print_date_data,start_date.get_year(),stop_date.get_year());   //000526
         */
         #endif
   } // if started
   // I would like to move this start_year special case to Common_simulation::start(), but need to check if I can always start year before start(), probably not
   // If the simulation is not started on Jan 1 go ahead and
   // process as the start of year for accumulator and report setup.             //040524
   started &= meteorology->start();                                              //151201
   return started;
}
//______________________________________________________________________________
//160515 This method is not needed because we simply call arguments.provide_scenario_directory();
bool CropSyst_Model::start_day()
{  bool started = Cropping_system_simulation::start_day();
   return started;
}
//_start_day________________________________________________________2015-11-28_/
bool CropSyst_Model::process_day()                                 modification_ //150611RLN
{
   UI_notify_date = simdate.get_date32();                                        //151203_060728
   today_for_debug_output = simdate_raw;
   meteorology->process_day();                                                   //151203_151202
#if (!defined(OFOOT_VERSION))
#ifdef DETAIL_DENITR
//061024 This is a special output wanted by Javier it is only temporary
// if   UI_notify_date is in any of the date ranges specified by the user then we enable the daily output for today
FOR_EACH_IN (detail_denitr_date_range, Detail_denitr_date_range , detail_denitr_dates,each_detail_denitr_date)
{
   if (detail_denitr_date_range->is_in_range(UI_notify_date))
      denitr_detail_stream = &land_unit_with_output->provide_denitr_detail();    //160128
      //160128 enable_denitr_detail =  true;
   /* not sure why removed or if still needed
   else if (detail_denitr_date_range->ends_before(UI_notify_date))
      each_detail_denitr_date->delete_current();
   */
} FOR_EACH_END(each_detail_denitr_date)
#endif
#endif

//dbg_denitr  << UI_notify_date << endl;
   #ifdef INTERCROPPING
   Crop_interfaced_model *primary_crop = 0;                                      //130715
   if (land_unit_with_output->crop_active_or_intercrop)                          //130715
      primary_crop = land_unit_with_output->crop_active_or_intercrop->get_predominant_crop(); //130715
   #else
      #define primary_crop land_unit_with_output->crop_active_or_intercrop
   #endif
   Normal_crop_event_sequence last_growth_stage = primary_crop
      ? primary_crop ->ref_phenology().get_growth_stage_sequence()
      : NGS_NONE;                                                                //130715_960829
#ifdef _Windows
   #ifdef DETAIL_FILE
   if (detail_txt_file.good()) detail_txt_file << '[' << today.get() <<']' << endl;
   #endif
   #ifdef DUMP_CURVE_NUMBERS
   if (SCS_runoff_dat_file && SCS_runoff_dat_file->good()) (*SCS_runoff_dat_file) << std::endl<< today.as_string(); //131216
   #endif
#endif
   bool processed = Cropping_system_simulation::process_day();                   //150611_030616
//             Objects such as the crop may trigger synchronization
//             events when this occurs we need to make sure to synchronize.
// Note, I think I put perform_triggered_synchronizations here instead of in land block
// fragment because crop_harvest_dormancy_termination_cleanup may reset some triggers
// I need to verify, otherwise I would prefer to move perform_triggered_synchronizations to land_bloc_frag.
//050115 moved to end day /*_______*/	   daily_output(today);

               // Perform output before triggers because triggers may delete crop.
//050115				This should no longer be true because we now use crop_for_output

   process_triggered_syncronizations();                                          //130926

   bool record_soil_profile_today = land_unit_with_output->record_soil_profile_today;

   //#ifdef PRINT_PROFILE
   //#ifdef XLS_OUTPUT
      // Added ability to print the soil profile on phenologic sync.

   Report_formats_parameters *reports = land_unit_with_output->reports;


      if (reports &&
         primary_crop && primary_crop->ref_phenology().get_growth_stage_sequence() != last_growth_stage)  //130925_130715_070227
         switch (primary_crop->ref_phenology().get_growth_stage_sequence())      //130925
         {
               case NGS_GERMINATION    : if (reports->at_planting)  record_soil_profile_today = true; break;  //010207
               case NGS_ACTIVE_GROWTH  : if (reports->at_emergence) record_soil_profile_today = true; break;
               case NGS_ANTHESIS       : if (reports->at_flowering) record_soil_profile_today = true; break;
               case NGS_MATURITY       : if (reports->at_maturity)  record_soil_profile_today = true; break;
         }
   if (!viable) {days_not_viable += 1; viable = true; }                          //040828
   if (primary_crop && primary_crop->ref_phenology().is_in_growing_season()
   // THIS NEEDS TO BE IMPLEMENTED
   // I have moved in_growing_season to Phenology_common::is_in_growing_season
   // but there was a check for (!biomass_fate_today)
   // that was needed we din't want to return true if the is fate to dispose
   // I am not sure if that condition is still relevent
   )
   season_grazing_req_stats_kg_ha.append(per_m2_to_per_ha(land_unit_with_output->grazing_BM_demand_today_kg_m2));  //050107
   return processed;
}
//_process__________________________________________________________2002-03-08_/
bool CropSyst_Model::end_day()                                     modification_
{  // Returns true if successful
   bool processed = Cropping_system_simulation::end_day();
   #ifdef PRINT_PROFILE

     Report_formats_parameters *reports = land_unit_with_output->reports;
     if (reports &&(reports->profile_spreadsheet_timestep)                       //000511
         && ((int)(simdate.get_DOY()%reports->profile_spreadsheet_timestep)==0)) //000511
        land_unit_with_output->record_soil_profile_today = true;                 //000511
   #endif
   #ifdef PRINT_PROFILE
   record_soil_profile_today = false;  // Reset for tomorrow
   #endif
#ifdef OUMAROU_GRAZING
XResidue_pools &residue_pools_kg_m2 = *land_unit_with_output.residue_pools_kg_m2;
QND_inc_plant_N_kg_m2   = residue_pools_kg_m2.get_N_output(1,0);
QND_inc_manure_BM_kg_m2 = residue_pools_kg_m2.get_biomass_output(1,1);
QND_inc_manure_N_kg_m2  = residue_pools_kg_m2.get_N_output(1,1);

QND
<< today.get()                                         << '\t'
<< per_m2_to_per_ha(QND_daily_grazed_BM_kg_m2        ) << '\t'
<< per_m2_to_per_ha(QND_plant_N_grazed_kg_m2         ) << '\t'
<< per_m2_to_per_ha(QND_plant_N_ungrazed_kg_m2       ) << '\t'
<< per_m2_to_per_ha(QND_inc_ungrazed_BM_kg_m2        ) << '\t'
<< per_m2_to_per_ha(QND_inc_plant_N_kg_m2            ) << '\t'
<< per_m2_to_per_ha(QND_inc_manure_BM_kg_m2          ) << '\t'
<< per_m2_to_per_ha(QND_inc_manure_N_kg_m2           ) << '\t'
<< per_m2_to_per_ha(QND_manure_org_N_appl_kg_m2      ) << '\t'
<< per_m2_to_per_ha(QND_manure_ammonia_N_inc_kg_m2   ) << '\t'
<< per_m2_to_per_ha(QND_manure_ammonia_N_loss_kg_m2  ) << '\t'
<< per_m2_to_per_ha(QND_mineralization_N_kg_m2       ) << '\t'
<< per_m2_to_per_ha(QND_immobil_N_kg_m2              ) << '\t'
<< per_m2_to_per_ha(QND_plant_N_uptake_kg_m2         ) << '\t'
<< endl;

QND_daily_grazed_BM_kg_m2 = 0;
QND_inc_ungrazed_BM_kg_m2 = 0;  // can't get this  (use Inc_plant BM)
QND_plant_N_grazed_kg_m2 = 0;
QND_plant_N_ungrazed_kg_m2 = 0;
QND_inc_plant_N_kg_m2 = 0;
QND_inc_manure_BM_kg_m2 = 0;
QND_inc_manure_N_kg_m2 = 0;
QND_manure_org_N_appl_kg_m2 = 0;
QND_manure_ammonia_N_inc_kg_m2 = 0;
QND_manure_ammonia_N_loss_kg_m2 = 0;
QND_mineralization_N_kg_m2 = 0;
QND_immobil_N_kg_m2 = 0;
QND_plant_N_uptake_kg_m2 = 0;
#endif
   return processed;
}
//_end_day__________________________________________________________2005-01-15_/
bool CropSyst_Model::stop()                                        modification_ // Stop recording to output file
{  bool stopped = true;

   // Dump the last year accumulators if simulation doesn't run to end of year   //000217
#ifdef GRAZING_SPECIAL_OUTPUT
   if (land_unit_with_output.active_model_options.total_grazed_biomass
    || land_unit_with_output.active_model_options.carbon_sequestered)            //040812
   {  ofstream grazing_regimen_dat(land_unit_with_output.scenario_directory->grazing_regimen_dat.as_string());  //051015
      grazing_regimen_dat << "[scenario]" << endl;
      grazing_regimen_dat << "exit_code=" << (aborted ? simulation_run_result_label_table[RUN_ABORT]:  !viable ? simulation_run_result_label_table[RUN_NOT_VIABLE]:simulation_run_result_label_table[RUN_SUCCESS]) << endl;
      grazing_regimen_dat << "days_not_viable=" << days_not_viable << endl;
      {  grazing_regimen_dat << "[optimization]" << endl;
         grazing_regimen_dat << "carbon_sequestration_profile=" << per_m2_to_per_ha(carbon_sequestration_stats_profile_kg_m2.get_current()) << " kg/ha (organic soil in the soil profile at the end of the run)" << endl;
         grazing_regimen_dat << "carbon_sequestration_30cm=" << per_m2_to_per_ha(carbon_sequestration_stats_30cm_kg_m2.get_current()) << " kg/ha (organic soil in the first 30cm at the end of the run)" << endl;
      }
      grazing_regimen_dat.setf(ofstream::fixed);                                 //050111
      grazing_regimen_dat.precision(3);                                          //050111
      seasonal_grazing_stats_kg_ha.dump_statistics(grazing_regimen_dat,STAT_SUM|STAT_MEAN,"seasonal_grazing");
      seasonal_grazing_stats_kg_ha.dump(grazing_regimen_dat,2, true, '\n',  0, 0);
      seasonal_ungrazed_stats_kg_ha.dump_statistics(grazing_regimen_dat,STAT_SUM|STAT_MEAN,"seasonal_ungrazed");
      seasonal_ungrazed_stats_kg_ha.dump(grazing_regimen_dat,2, true, '\n', 0,0);
      seasonal_biomass_production_stats_kg_ha.dump_statistics(grazing_regimen_dat,STAT_SUM|STAT_MEAN,"seasonal_biomass_production");
      seasonal_biomass_production_stats_kg_ha.dump(grazing_regimen_dat,2, true, '\n',  0, 0);
      seasonal_grazing_req_stats_kg_ha.dump_statistics(grazing_regimen_dat,STAT_SUM|STAT_MEAN,"seasonal_grazing_requirements");
      seasonal_grazing_req_stats_kg_ha.dump(grazing_regimen_dat,2, true, '\n',  0,0);
   }
   { // Special output for manual
      CORN::File_name special_filename("special.dat");
      special_filename.set_path(land_unit_with_output.scenario_directory->output_dir);
      ofstream special_dat(special_filename.c_str());
      special_dat << "[average_annual]" << endl;
      special_dat << "ETpot=" << annual_pot_ET.get_mean() << endl;
      special_dat << "ETact=" << annual_act_ET.get_mean() << endl;
      special_dat << "RUSLE_C_factor=" << annual_RUSLE_C_factor.get_mean() << endl;
   }
#endif
   stopped &= Cropping_system_simulation::stop();                                //151203
   return stopped;                                                               //010208
}
//_stop________________________________________________________________________/
bool CropSyst_Model::end_year()                                    modification_
{  bool successful = Cropping_system_simulation::end_year();                     //131010_040524
   // We must do land block fragment first so we have the soil erosion for output.

   // This is special output for Oumarou's carbonsequestration and grazing regimens model. 040815
   Organic_matter_residues_profile_abstract *residue_and_organic_matter_profile = land_unit_with_output->organic_matter_residues_profile_kg_m2;
   if (residue_and_organic_matter_profile)                                       //060628
   {  carbon_sequestration_stats_profile_kg_m2.append(residue_and_organic_matter_profile->sum_carbon_to_depth_kg_m2(INCLUDE_SOM_AND_MICROBIAL,ENTIRE_PROFILE_DEPTH_as_9999,true));  //040812
      carbon_sequestration_stats_30cm_kg_m2.append(residue_and_organic_matter_profile->sum_carbon_to_depth_kg_m2(INCLUDE_SOM_AND_MICROBIAL,0.3,true));                              //040812
   }
   grazed_biomass_stats_kg_ha_check_used.append(per_m2_to_per_ha(land_unit_with_output->ANNL_accumulators_active->biomass_grazed)); //050115_
#ifdef DISABLED
101014
// Temporary special output
   if (residue_and_organic_matter_profile)                                       //060628
   {
float64 SOM_C_mass_30cm    = residue_and_organic_matter_profile->sum_carbon_to_depth_kg_m2(INCLUDE_SOM_AND_MICROBIAL,0.3,true);
float64 SOM_C_mass_profile = residue_and_organic_matter_profile->sum_carbon_to_depth_kg_m2(INCLUDE_SOM_AND_MICROBIAL,ENTIRE_PROFILE_DEPTH_as_9999,true);

float64 microbial_C_mass_30cm    = residue_and_organic_matter_profile->sum_carbon_to_depth_kg_m2(INCLUDE_MICROBIAL_BM,0.3,true);
float64 microbial_C_mass_profile = residue_and_organic_matter_profile->sum_carbon_to_depth_kg_m2(INCLUDE_MICROBIAL_BM,ENTIRE_PROFILE_DEPTH_as_9999,true);

float64 labile_C_mass_30cm       = residue_and_organic_matter_profile->sum_carbon_to_depth_kg_m2(INCLUDE_LABILE_ACTIVE_SOM,0.3,true);
float64 labile_C_mass_profile    = residue_and_organic_matter_profile->sum_carbon_to_depth_kg_m2(INCLUDE_LABILE_ACTIVE_SOM,ENTIRE_PROFILE_DEPTH_as_9999,true);

float64 metastable_C_mass_30cm   = residue_and_organic_matter_profile->sum_carbon_to_depth_kg_m2(INCLUDE_METASTABLE_ACTIVE_SOM,0.3,true);
float64 metastable_C_mass_profile= residue_and_organic_matter_profile->sum_carbon_to_depth_kg_m2(INCLUDE_METASTABLE_ACTIVE_SOM,ENTIRE_PROFILE_DEPTH_as_9999,true);

float64 passive_C_mass_30cm      = residue_and_organic_matter_profile->sum_carbon_to_depth_kg_m2(INCLUDE_PASSIVE_SOM,0.3,true);
float64 passive_C_mass_profile   = residue_and_organic_matter_profile->sum_carbon_to_depth_kg_m2(INCLUDE_PASSIVE_SOM,ENTIRE_PROFILE_DEPTH_as_9999,true);

#ifdef DEBUG_ANNUAL_CARBON_MASS
      annual_carbon_mass << year  << '\t'
                                 << per_m2_to_per_ha(SOM_C_mass_30cm)   << '\t' << per_m2_to_per_ha(SOM_C_mass_profile)  << '\t'
                                 << per_m2_to_per_ha(microbial_C_mass_30cm)   << '\t' << per_m2_to_per_ha(microbial_C_mass_profile)  << '\t'
                                << per_m2_to_per_ha(labile_C_mass_30cm)       << '\t' << per_m2_to_per_ha(labile_C_mass_profile)     << '\t'
                                << per_m2_to_per_ha(metastable_C_mass_30cm)   << '\t' << per_m2_to_per_ha(metastable_C_mass_profile)   << '\t'
                                << per_m2_to_per_ha(passive_C_mass_30cm)      << '\t' << per_m2_to_per_ha(passive_C_mass_profile) << endl;
#endif
    }
   annual_pot_ET        .append(land_unit_with_output.ANNL_accumulators_active->pot_evapotranspiration); //050505
   annual_act_ET        .append(land_unit_with_output.ANNL_accumulators_active->act_evapotranspiration); //050505
   if (land_unit_with_output.get_soil()->erosion)                                                        //060712
      annual_RUSLE_C_factor.append(land_unit_with_output.get_soil()->erosion->RUSLE_annual_C_factor);    //050505
    // special for Manuel                                                                                 //050505
#endif
   return successful;                                                            //040524
}
//_end_year_________________________________________________________2001-02-02_/
void CropSyst_Model::record_growing_season                                       //131008
(CropSyst::Crop_model_interface &crop_to_output)                                 //190618
{
   //This is special output for Oumarou's carbon sequestration and grazing regimens model
   //Once I have implemented recording season in UED file, I can move this to an external program 040815
   Crop_mass_fate *biomass_fate_season=crop_to_output.get_biomass_fate_season(); //040815
   if (biomass_fate_season)                                                      //040815
   {  float32 removed_for_grazing_kg_m2 = biomass_fate_season->get_removed_for_grazing();
      seasonal_grazing_stats_kg_ha.append(per_m2_to_per_ha(removed_for_grazing_kg_m2));
      seasonal_ungrazed_stats_kg_ha.append(per_m2_to_per_ha(biomass_fate_season->get_total_sent_to_residue()));
      seasonal_biomass_production_stats_kg_ha.append
         (per_m2_to_per_ha(crop_to_output.ref_reported().season.biomass_production));  //190705
      seasonal_grazing_req_stats_kg_ha.append(season_grazing_req_stats_kg_ha.sum());   //050107
      season_grazing_req_stats_kg_ha.reset();                                    //050107
   }
}
//_record_growing_season____________________________________________2003-05-16_/
Event_status_indicator                                                           //151128
CropSyst_Model::perform_operation
(Common_operation &op
,void *associated_obj)                                                           //021109
{
   Event_status_indicator result = ES_FAIL;                                      //151128
   CropSyst::Operation &CS_op = (CropSyst::Operation &)op;
   switch (CS_op.get_type())
   {
      // NYI
      // case soil profile record event
      // case other events related to output
      default: result = land_unit_with_output->perform_operation(CS_op); break;  //030519
   }
   return result;                                                                //030519
}
//_perform_operation________________________________________________2002-03-08_/
bool CropSyst_Model::end_operation_period
(Common_operation &op, const Date_const &date_period_ends)
{  return land_unit_with_output->end_operation_period(op,date_period_ends);
}
//_end_operation_period_____________________________________________2002-05-04_/
CropSyst_Model::~CropSyst_Model()
{  delete GIS_override_table;
   delete GIS_record;                                                            //050921
}
//_CropSyst_Model:destructor________________________________________2001-05-16_/
const CORN::OS::Directory_name &CropSyst_Model::get_output_directory_name() const
{  return (
      //200525 arguments.provide_scenario_directory()
      scenario_directory                                                         //200525
      .Output());
}
//_get_output_directory_name________________________________________2012_03_20_/
const CORN::OS::Directory_name &CropSyst_Model::provide_scenario_directory_name() provision_
{  return arguments.
      start_directory;                                                           //200525
      //200525 provide_scenario_directory_name();
}
//_provide_scenario_directory_name__________________________________2015-12-03_/
const std::wstring &CropSyst_Model::get_control_file_name_unqual()         const
{  return control_file_name_unqual;
}
//_get_control_file_name_unqual_____________________________________2015-12-03_/
nat16 CropSyst_Model::desire_AgMIP_variables()                   initialization_
{  nat16 vars_appended = 31;
   // In version 5 this hardcoding of desired variables is obsolete
   // Instead use inspection_journal and or TDF files
   // to indicate what variables are to be recorded.
   // This functionality could be added to V4 and this function deleted.         //170227

   CS::Optation_temporal &optation_day = CS::optation_global.provide_temporal(UT_day); //190119

   //190204 CS::Desired_variables &desired_variables = *desired_variables_daily;          //181030
      // NYI eventually append should return bool and then we simply sum the count of appends
      optation_day.know_variable_code(CSVC_crop_biomass_yield_period_sum);
      optation_day.know_variable_code(CSVC_crop_biomass_produced_above_ground_period_sum);
      optation_day.know_variable_code(CSVC_crop_base_peak_LAI_time_step_max);
      optation_day.know_variable_code(CSVC_crop_base_flowering_date_YD);
      optation_day.know_variable_code(CSVC_crop_base_maturity_date_YD);
      optation_day.know_variable_code(CSVC_soil_N_leached_period_sum);
      optation_day.know_variable_code(CSVC_soil_base_water_drainage_period_sum);
      optation_day.know_variable_code(CSVC_crop_N_mass_yield_period_sum);        //120117
      optation_day.know_variable_code(CSVC_weather_ET_act_period_sum);
      optation_day.know_variable_code(CSVC_weather_ET_ref_period_sum);           //130724
      optation_day.know_variable_code(CSVC_soil_N_mineralization_period_sum);
      //optation_day.know_variable_code(CSVC_mgmt_N_organic_gaseous_loss_period_sum);
      optation_day.know_variable_code(CSVC_mgmt_N_inorganic_gaseous_loss_period_sum);
      optation_day.know_variable_code(CSVC_soil_N_immobilization_period_sum);
      optation_day.know_variable_code(CSVC_organic_matter_mineralized_period_sum);
      optation_day.know_variable_code(CSVC_soil_N_denitrification_period_sum);

      optation_day.know_variable_code(CSVC_crop_biomass_yield);
      optation_day.know_variable_code(CSVC_crop_biomass_canopy);
      optation_day.know_variable_code(CSVC_crop_base_leaf_area_index);
      optation_day.know_variable_code(CSVC_soil_N_leached_accum_to_date_time);
      optation_day.know_variable_code(CSVC_crop_N_mass_canopy_production_accum_to_date_time); //131007
      optation_day.know_variable_code(CSVC_crop_N_mass_canopy_current);          //131007
      optation_day.know_variable_code(CSVC_soil_base_water_drainage_accum_to_date_time);
      optation_day.know_variable_code(CSVC_weather_ET_act_accum_to_date_time);
      optation_day.know_variable_code(CSVC_weather_ET_ref_accum_to_date_time);   //130724

      optation_day.know_variable_code(CSVC_organic_matter_mineralized_accum_to_date_time);
      optation_day.know_variable_code(CSVC_mgmt_N_inorganic_gaseous_loss_accum_to_date_time);
      optation_day.know_variable_code(CSVC_soil_N_immobilization_accum_to_date_time);
      optation_day.know_variable_code(CSVC_soil_N_denitrification_time_step_sum);
      optation_day.know_variable_code(CSVC_soil_base_plant_avail_water);
      optation_day.know_variable_code(CSVC_organic_matter_N_mineralization_profile_time_step_sum);
      optation_day.know_variable_code(CSVC_organic_matter_mineralized_time_step_sum);
   return vars_appended;
}
//_desire_AgMIP_variables___________________________________________2016-03-12_/
}//_namespace CropSyst_V4_______________________________________________________

