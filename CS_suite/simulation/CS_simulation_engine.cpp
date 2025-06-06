#include "CS_suite/simulation/CS_simulation_engine.h"
#include "CS_suite/observation/CS_recording.h"
#include "CS_suite/observation/CS_UED_recorder.h"
#include "CS_suite/file_system/CS_file_ext.hpp"
#include "corn/OS/file_system_engine.h"
#include "corn/application/progress.h"
#include "CS_suite/observation/CS_optation.h"

#ifdef _Windows
#  define dynamic_library_file_extension "DLL"
#else
#  define dynamic_library_file_extension "so"
   // I think linux dynamic objects have so extension
#endif

CORN::Date_format ISO_date_with_DOY_format
(CORN_date_DEFAULT_ORDERING
,CORN_date_DEFAULT_STYLES
,CORN_date_DEFAULT_SEPARATOR
,true);

#define _ISO_date_with_DOY_no_time ,&ISO_date_with_DOY_format,0

namespace CS {
int32 simdate_debug;
//______________________________________________________________________________
Simulation_engine::Simulation_engine
//200815 (Command_options_data_source &command_line_options_data_source_)                 //160826
(CS::Arguments &arguments_)                                                      //200815
: Simulation_element_composite(today_driving_raw)                                //170524_150612
, arguments(arguments_)                                                          //200815
//200815 , CWD                         ()                                                 //160902
, control_filename_extension  (L"CS_control")                                    //160505
, context_discovery           (0)                                                //170627_161118
, alteration                  (0)                                                //181015
, today_driving_raw           (1)                                                //170524_090309
, simulation_units            (sub_elements)
, databases                   (0)
//200815 , command_line_options_data_source(command_line_options_data_source_)            //160826
, can_continue(true)                                                             //180208
{
   simdate.prefer_date_format(&CORN::ISO_8601_date_format);                      //170524
}
//_Simulation_engine:constructor____________________________________2013-06-18_/
Simulation_engine::~Simulation_engine()
{  delete databases;
}
//_Simulation_engine:destructor_____________________________________2017-02-25_/
bool Simulation_engine::is_key_string(const std::string &key)       affirmation_ //
{
   // Currently there is not an occasion where the key of the
   // simulation scenario is used, so this method could be reimplemented
   // if something more useful is envisioned.
   // Currently the CWD (ultimate simulation scenario directory)
   // effectively uniquely identifies a scenario.
   return key.compare(arguments.start_directory.c_str()) == 0;                   //200815
   //200815 return key.compare(CWD.c_str()) == 0;
}
//_is_key_string____________________________________________________2018-08-20_/
Context_discovery &Simulation_engine::provide_context_discovery()     provision_ //180118
{
   if (!context_discovery)
   {
      context_discovery =
         new Context_discovery(arguments.start_directory);                        //200813
      /*200815
      const CS::Arguments *args = ref_arguments();                               //180118
      context_discovery = args
         ? new Context_discovery(args->start_directory)                          //200813
         //200813 ? new Context_discovery(args->provide_scenario_directory_name())
         : new Context_discovery();
      */
   }
   return *context_discovery;
}
//_provide_context_discovery________________________________________2017-06-27_/
bool Simulation_engine::preinitialize()                          initialization_
{  bool preinitalized = true;                                                    //170313
   // Set up desired variables

   // Assuming the current working directory is the scenario directory
   // Later we can override with scenario specified on command line
   // (As in Version 4)
   // file discover must be setup before setup of control and scenario options.
   /*200815 now arguments are always submitted
   apply_command_line_arguments();

   // Scenario directory could be specified on the command line.
   // although apply_command_line_arguments returns bool if any args applied
   // args are optional so not a condition for preinitalized.
   const CS::Arguments *args = ref_arguments();                                  //180118_170313
   CORN::global_arguments = args;                                                //191015_180305
   */
   Context_discovery &context_discovery = provide_context_discovery();           //170627
   /*200815
   context_discovery.verbose =
      CORN::global_arguments->                                                   //200317
      verbosity;                                        //170607
   */
   context_discovery.verbose = arguments.verbosity;                              //200815

//context_discovery.verbose=127;

   CS::Examination &examination                                                  //171116
      = instantiate_examination_global
         (today_driving_raw,get_output_directory_name());                        //190124
   examination.desire_tally_statistics(arguments.desired_tally_statistics);      //200818
   /*200815
   if (args)                                                                     //170313
      examination.desire_tally_statistics(args->desired_tally_statistics);
   */

   instantiate_emanators(simdate);                                               //171116
   alteration = new Alteration(today_driving_raw);                               //181016
   sub_elements.take(alteration); /*actually submit */                           //181016
   CORN::OS::file_system_engine.set_current_working_directory                    //170313
      (context_discovery.provide_leaf());                                        //170627_170313

   if (CORN::global_arguments->verbosity > 2)    //discovery                     //170313
      std::clog << "scenario directory:";                                        //170313
   #ifdef __linux__
   if (CORN::global_arguments->verbosity > 2)    //discovery                     //170313
   // else in Windows always print the scenario directory
   #endif
      std::clog << context_discovery.provide_leaf().c_str() << std::endl;        //170313
   discover_TDF_and_optate();                                                    //190119


   /*190119  I think this is obsolete, it was added for Harsimran's project
   Now just define TDF files.
   CORN::Unidirectional_list inspection_journal_files;
   context_discovery.extract_accessible_files_with_extension                     //170628
      (L"inspection_journal",inspection_journal_files);
   desired_variables_daily->include_from_files(inspection_journal_files);        //170225

   */

/* NYI/Conceptual                                                                //170227
Discover TDF files and load desired variables
(for columns that have daily time step.
*/
   designate_accessible_files();                                                 //170629
   define_scenario_from_scenario_path();                                         //170629

   sub_elements.append(new Recording_system(simdate_raw));                       //170528
   // Adding the recording system before simulation elements
   // ensures that the actual simulation elements are available to the scope
   // of the examination and recording

   CORN::Unidirectional_list *adjustment_files = new CORN::Unidirectional_list;  //181016
   context_discovery.extract_accessible_files_with_extension                     //181016
      (L"CS_scenario_adjustment",*adjustment_files);                             //181016
   alteration->take_adjustment_files(adjustment_files); adjustment_files = 0;    //181016
   return preinitalized;
}
//_preinitialize____________________________________________________2017-02-27_/
bool Simulation_engine::initialize()                             initialization_
{  bool initialized = Simulation_element_composite::initialize();                //150929
   /*NYI
   if (find_and_load_plugins())
      // Inspectors provide access the variable of other Simulation elements
      simulation_control.inspection_levels &= INSPECTION_CONTINUOUS_BIT;
   */

   #if (CS_VERSION >= 5)

   Examination &examination = get_examination_global();                          //170319
   // For now I am just passing the key but this would probably
   // be the scenario description.
   //160606 for (nat8 level = 0; level < COUNT_inspection; level++)
   //160606       if (inspection.inspector_tallies[level])
   // At this point component elements should be available
   // (I.e. CropSyst soil and meteorology)

   render_inspectors
            (examination.inspectors                                              //161202
            ,0 //We dont need to identify the instance but it would be the scenario name
            ,ref_simulation_control().description.brief.c_str()); //The engine is the upper most context
/* currently not global because each simulation element can maintain
their own alteration, but may want to be global to allow
simulation elements to share their alterable elements

   Alteration &alteration = get_alteration_global();                             //181015
   render_modifiers(alteration.modifiers
            ,0
            ,ref_simulation_control().description.brief.c_str());                //181015
*/
   #endif
   return initialized;                                                           //160222
}
//_initialize_______________________________________________________2015-09-19_/
bool Simulation_engine::start()                                    modification_
{  bool started = true;
   if (ref_simulation_control().UED_recording)                                   //160621
   {
      provide_global_UED_recorder                                                //160614
         (get_output_directory_name());
   }
   // We need intialized done first in start so the output directory has been setup

   // Note that the start date is always the target start date
   const CORN::Date_const &start_date = ref_start_date();                        //160222
   const CORN::Date_const &stop_date  = ref_stop_date();                         //160222
   today_driving_raw = start_date.get_date32();                                  //170524_160222_040510

   Simulation_element_composite::start();
   {  Synchronization rel_to_start_date_sync(SIMULATION_BEGIN_DATE,0);           //030605_110505 moved from run()
      if (event_scheduler)
      event_scheduler->synchronize(rel_to_start_date_sync,start_date);           //160222_131101_031010
   }
   {  Synchronization rel_to_end_date_sync(SIMULATION_END_DATE,0);               //030605
      if (event_scheduler)
      event_scheduler->synchronize(rel_to_end_date_sync,stop_date);              //160222_131101_031010
   }
   if (start_date.get_DOY() != 1)                                                //150701
      started &= start_year();
      // The simulation period might not actually start Jan 1
      // but we typically will want annual setup

   // At this point all subelements existing throughout the duration
   // of the simulation should have been instanciated so we can setup
   // their inspection.
   // Subelements may instanciate ephemeral objects and would be responsible
   // for initiating inspector rendition of those objects.
   // Ephemeral objects (I.e. CropSyst Crop)
   // generally will not be subject to annual inspection.

   // NYI need to inform inspectors of desired_variables_daily  //170225

   // NYI inform plugins of inspectors

   get_emanators().start(simdate.get_datetime64());                              //171115
   //200815 const CS::Arguments *args = ref_arguments();                                  //180118_170316
   return started;
}
//_start____________________________________________________________2009-04-09_/
bool Simulation_engine::stop()                                     modification_
{  bool stopped = true;
   #if (CS_VERSION >= 5)
   stopped &= get_examination_global().stop();                                   //170319
   #endif
   return stopped && Simulation_element_composite::stop();
}
//_stop_____________________________________________________________2019-01-24_/
/*200815 Now arguments are provided external to this
bool Simulation_engine::apply_command_line_arguments()             modification_
{  bool applied = true;
   Arguments *arguments = mod_arguments();
   if (arguments)
   {
      CORN::global_arguments = arguments;                                        //191104
      command_line_options_data_source.get(*arguments);                          //160826
   }
   return applied;
}
//_apply_command_line_arguments_____________________________________2016-02-22_/
*/

// The plugin system is currently intended for version 5
// but I may add it also to V4
nat8 Simulation_engine::find_and_load_plugins()                       provision_
{
   // plugin could be in the scenario directory or any parent directory
   // so it is discovered.
   // (I.e. all scenarios in a project might want to apply the the same plugin(s).)

   nat8 plugins_found = 0;
   CORN::Unidirectional_list DLL_filenames;
   nat32 DLLs_found = 0;
   #ifdef _Windows
   // In _Windows DLLs for CS simulations are likely to be shortcuts,
   // In this case we will resolve. (The resolution could be moved to discovery)
   CORN::Unidirectional_list DLL_shortcut_filenames;
   CORN::Inclusion DLL_pattern("*.DLL*",true);  // will find shortcuts
   nat32 DLL_shortcuts_found = CORN::OS::file_system_engine.list_DENs
      (provide_scenario_directory_name()
      ,&DLL_shortcut_filenames,0,DLL_pattern
      ,CORN::OS::File_system::superdirectory_recursion_inclusive);
   FOR_EACH_IN(DLL_shortcut,CORN::OS::File_name,DLL_shortcut_filenames,each_shortcut)
   {
      CORN::OS::File_name *actual_DLL_name = CORN::OS::file_system_engine.render_resolved_shortcut_or_assume_actual(*DLL_shortcut);
      DLL_filenames.append(actual_DLL_name);
   } FOR_EACH_END(each_shortcut)
   #endif
   // In Linux, dynamic libraries for CS simulations will generally be soft or hard links
   // so it is not necessary to resolve links.
   // In both Windows and Linux the user could have put the actual DLL file
   // in the target directory tree although this is not recommended.
  #ifndef __linux__

   // I have not yet checked pluggins for Linux
   // This does compile with Qt compiler but gcc has problems linking
   // on the ThinkMates so I have disabled this until I needed it.               //151218
   CORN::Inclusion DLL_pattern_linux("*.so*",false);
   DLLs_found += CORN::OS::file_system_engine.list_DENs
      (provide_scenario_directory_name(),&DLL_filenames,0
      ,DLL_pattern_linux
      ,CORN::OS::File_system::superdirectory_recursion_inclusive);
         // plugin could be in the scenario directory or any parent directory
         // (I.e. all scenarios is a project might want to apply the
         //  the same plugin(s).)
   FOR_EACH_IN(DLL_filename,CORN::OS::File_name,DLL_filenames,each_DLL)
   {  Simulation_element_plugin *plugin = new Simulation_element_plugin
         (*DLL_filename,&get_examination_global().inspectors,simdate_raw);       //171114_170524_161202_150915
         // NYI modifiers
      if (plugin->is_valid())
      {  simulation_units.take(plugin);
         plugins_found++;
      } else delete plugin;
         //The DLL might not be a valid simulation plugin so dont install it.
   } FOR_EACH_END(each_DLL)
   #endif
   return plugins_found;
}
//_find_and_load_plugins____________________________________________2015-09-08_/
std::ofstream *Simulation_engine::status_restart()
{
   std::ofstream *status = 0;
   CORN::OS::File_name_concrete started_filename("started");
   //200815 if (ref_arguments() && ref_arguments()->success_filename.length())
   if (arguments.success_filename.length())                                      //299815
   {
      //200815 const CS::Arguments &arguments = *ref_arguments();
      if (CORN::OS::file_system_engine.exists(started_filename))
      {  // The program has already started
         std::clog << "Already started, if you need to rerun then delete:" << started_filename.c_str() << std::endl;
         exit(EXIT_SUCCESS); // not an error just dont start again.
      }
      CORN::OS::File_name_concrete success_filename(arguments.success_filename);
      if (CORN::OS::file_system_engine.exists(success_filename))
      {  // the program has already been run
         std::clog << "Already completed" << std::endl;
         if (arguments.quiet)
            exit(EXIT_SUCCESS); // not an error just dont restart
         else
         {  std::string continue_enter;
            std::clog
               << "Rerun?" << std::endl
               << "Enter 'n' or press [Ctrl][C] to cancel." << std::endl
               << "Enter 'y' or press [Enter] to continue." << std::endl;
            std::getline(std::cin,continue_enter);
            if ((continue_enter[0] == 'n') || (continue_enter[0] == 'N'))
            {  std::clog << "No rerun" << std::endl;
               exit(EXIT_SUCCESS); // not an error just dont start again.
            } else
               CORN::OS::file_system_engine.recycle(success_filename,true);
         }
      }
      status = new std::ofstream(started_filename.c_str(),std::ios::app);
      CORN::Date_time_clad_64 now;
      (*status) << now.as_string() << ": { +run";
   }
   return status;
}
//_status_restart___________________________________________________2018-02-08_/
Simulation_run_result Simulation_engine::run()
{
   std::ofstream *status=status_restart();                                       //180208
   can_continue &= preinitialize();                                              //180208_170227
   if (status)                                                                   //180208
   {  (*status) << ", preinitialization" << ((can_continue) ? '+' : '-');
   }
   bool stopped_OK = false;
   bool started_OK = can_continue;                                               //150930
   CORN::Date_cowl_32 today_driving(today_driving_raw);                          //170524
   today_driving_raw = ref_start_date().get_date32();                            //170524_160120
   // today must be set before calling initilize                                 //170220
   started_OK &= initialize();                                                   //150930
   if (status)                                                                   //180208
   {  (*status) << ", initialization" << ((started_OK) ? '+' : '-');
   }
   Progress_display *progress = get_progress_display_global();                   //170520
   if (progress)                                                                 //170528
   {  nat32 simulation_days = ref_start_date().days_to(ref_stop_date());
      progress->know_steps_to_complete(simulation_days);
   }
   started_OK &= start();                                                        //180208_170612_150930_150429
   if (status)                                                                   //180208
   {  (*status) << ", start" << ((started_OK) ? '+' : '-')
         << " }" << std::endl;
      // At this point we are done with the started status file
      // we don't need to keep it open continuously
      delete status; status = 0;
   }
   if (started_OK)
   {  if (progress) progress->start();                                           //170520
      for (today_driving.set(ref_start_date())                                   //181026
          ;today_driving <=  ref_stop_date()
            && can_continue &&
            (!event_scheduler || !event_scheduler->is_aborted() )
          ;today_driving.inc_day())
      {  bool stream_progress = progress && progress->step();                    //170316
         if (stream_progress)                                                    //170316
         {  std::string today_string;                                            //181026
            simdate.append_to_string(today_string _ISO_date_with_DOY_no_time);   //181026
            progress->know_specific_string(today_string);
         }
         if (simdate.get_DOY() == 1)
            can_continue &= start_year();                                        //150941
         bool day_started   =   start_day();                                     //150617
         bool day_processed = process_day();                                     //050115
         bool day_ended     =     end_day();
         can_continue = day_started && day_processed && day_ended;
         if (simdate.is_last_DOY())                                              //140778_070228_040524
            can_continue &= end_year();                                          //170420
         if (stream_progress) progress->stream_to(std::clog);                    //170316
      }
      stopped_OK = stop();                                                       //140728
   }
   if (progress) progress->stop(std::clog);                                      //170528_170523_170316
      stopped_OK &= finalize();                                                  //181223
   Simulation_run_result result                                                  //040806
      = (today_driving >= ref_start_date()) && stopped_OK                        //160222
      ? RUN_SUCCESS : RUN_FAILURE;
   //200815 if (ref_arguments() && ref_arguments()->success_filename.length())            //180208
   if (arguments.success_filename.length())                                      //200815_180208
   {  CORN::OS::File_name_concrete result_filename
         ((result == RUN_SUCCESS) ? arguments.success_filename.c_str() : "failure");
         //200815 ((result == RUN_SUCCESS) ? ref_arguments()->success_filename.c_str() : "failure");
      CORN::OS::File_name_concrete started_filename("started");
      CORN::OS::file_system_engine.rename(started_filename,result_filename);
   }
   if (!can_continue)result = RUN_TERMINATE;                                     //150616_040806
   if (event_scheduler)                                                          //150930
   {
   if (!event_scheduler->is_viable()) result = RUN_NOT_VIABLE;                   //040816
   if (event_scheduler->is_aborted()) result = RUN_ABORT;
   }
   return result;                                                                //040806
}
//_run______________________________________________________________2013-06-18_/
bool Simulation_engine::is_rotation_shifting_enabled()              affirmation_
{  return ref_simulation_control().rotation_cycling;                             //160222
}
//_is_rotation_shifting_enabled_____________________________________2015-10-08_/
const char *Simulation_engine::get_ontology()                              const
{  return ref_simulation_control().description.brief.c_str();
}
//_get_ontology_____________________________________________________2016-06-06_/
Databases &Simulation_engine::provide_databases()
{  if (!databases)
      databases = new CS::Databases(provide_scenario_directory_name()
         ,CORN::global_arguments->verbosity);                                    //200317_170217
   return *databases;
}
//_provide_databases________________________________________________2016-09-16_/
nat16 Simulation_engine::designate_accessible_scenario_parameter_filenames()
{  provide_context_discovery();
   return
      context_discovery->reset_data_record_missing_with_accessible
         (mod_simulation_scenario())
    + context_discovery->reset_data_record_missing_with_accessible
         (mod_simulation_control());
}
//_designate_accessible_scenario_parameter_filenames________________2017-06-29_/
nat8 Simulation_engine::define_scenario_from_scenario_path()       modification_
{  nat8 identified = 0;
/*200815 now arguments are always provided
   const CS::Arguments *arguments = ref_arguments();                             //180118
   if (!arguments) return 0;
*/
   //NYI CORN::Text_list_wstring path_items; CORN::Text_list_wstring path_items_not_in_scenarios;
   CORN::Text_list path_items; CORN::Text_list path_items_not_in_scenarios;
   arguments.start_directory                                                     //200815
   //200813arguments->provide_scenario_directory_name()
      .append_path_list_to                                                       //170314
      (path_items
      ,CORN::OS::Directory_entry_name::include_qualification
      |CORN::OS::Directory_entry_name::include_name_extension);
   bool in_scenarios = false;
   while (!in_scenarios && !path_items.is_empty())
   {
      Item_wstring *path_item = dynamic_cast<Item_wstring *>(path_items.pop_first());
      path_items_not_in_scenarios.take(path_item);
      in_scenarios = (*path_item) == L"Scenarios";
   }
   if (!in_scenarios)
   {  // We didn't find the "Scenarios" landmark we are probably in
      // In some arbitrary directory so we have to do a more exhaustive search
      // Not linking: path_items_not_in_scenarios.give_to(path_items);
      FOR_EACH_IN(path_item,Item_wstring,path_items_not_in_scenarios,each_item)
      {  path_items.add_wstring((*path_item));                                   //180115
      } FOR_EACH_END(each_item)
   }
   /* dont delete (this generally works but is expensive as it does a lot of
      file lookups in the database)                                              //171117

   FOR_EACH_IN(path_item,CORN::Text_list::Item,path_items,each_path_name)
   {
      identified +=
         find_file_ext_in_database_and_assign_to_any_parameter_filename
            (*path_item,path_items); // used to be most closely match
   } FOR_EACH_END(each_path_name)

      */
   identified += resolve_parameter_filenames_kadigans(path_items);               //171117
   return identified;
}
//_define_scenario_from_scenario_path_______________________________2016-10-31_/
nat32 Simulation_engine::resolve_parameter_filenames_kadigans
(const CORN::Text_list &path_items)
{  nat32 resolved = 0;
   Simulation_control &scenario_record = mod_simulation_scenario();
   FOR_EACH_IN(section,VV_Section,scenario_record.sections,each_section)
   {
      FOR_EACH_IN(entry,VV_abstract_entry,section->entries,each_entry)
      {  const CORN::VV_directory_entry_entry *entry_as_DEN =
            dynamic_cast<const CORN::VV_directory_entry_entry *>(entry);
         if (entry_as_DEN)
         {
            CORN::OS::Directory_entry_name &param_filename = entry_as_DEN->ref_value();
            // At this point the parameter file names are fully qualified
            if (!param_filename.has_name(true) // NYI is_valid()
                || !CORN::OS::file_system_engine.exists(param_filename))
               resolved += resolve_parameter_filename_kadigans
                  (param_filename,path_items);
         }
      } FOR_EACH_END(each_entry)
   } FOR_EACH_END(each_section)
   return resolved;
}
//_resolve_parameter_filenames_kadigans_____________________________2017-11-17_/
bool Simulation_engine::resolve_parameter_filename_kadigans
(CORN::OS::Directory_entry_name &param_filename // both input and output
,const CORN::Text_list &path_items)                                        const
{
   // param_filename may be qualified or unqualified and relative or absolute
   CORN::Text_list potential_kadigans; // The filename with potential kadigans decomposed
   param_filename.append_path_list_to
      (potential_kadigans
      ,CORN::OS::Directory_entry_name::include_all_except_extension);
   // CORN::Text_list replaced_kadigans_candidate_filenames;
   CORN::OS::Directory_entry_name *param_filename_resolved
   = resolve_kadigans(potential_kadigans,path_items,param_filename.get_extension());
   if (param_filename_resolved)
   {
      if (CORN::global_arguments->verbosity >= 3)                                //200317
      {  std::clog << "resolved: " << param_filename.c_str() << std::endl
                   << "    with: " << param_filename_resolved->c_str() << std::endl;
      }
      param_filename.set_DEN(*param_filename_resolved);
   }
   return param_filename_resolved != 0;
}
//_resolve_parameter_filename_kadigans______________________________2017-11-17_/
CORN::OS::File_name_concrete * // bool
Simulation_engine::resolve_kadigans
(const CORN::Text_list &potential_kadigans
,const CORN::Text_list &path_items
,const std::wstring                    &extension)                        const
{
   nat32 kadigan_index = 0;
   nat32 potential_kadigan_count = potential_kadigans.count();
   FOR_EACH_IN(potential_kadigan,Item_wstring,potential_kadigans,each_kadigan)
   {
      bool is_actual_kadigan
         = potential_kadigan->c_str()[0] == L'_';
            // NYN potentail_kadigan.c_str()[potentail_kadigan.length()-1] = '_'
      if (is_actual_kadigan)
      FOR_EACH_IN(path_item,Item_wstring,path_items,each_path_item)
      {
         CORN::Text_list partially_resolved_potential_kadigans;
         partially_resolved_potential_kadigans.clone_from(potential_kadigans,0,kadigan_index);
         CORN::OS::Directory_entry_name_concrete DEN(partially_resolved_potential_kadigans,CORN::OS::Directory_entry_name::include_all_except_extension);
         if (!CORN::OS::file_system_engine.exists(DEN)) { break;} // if DEN exists
         partially_resolved_potential_kadigans.add_wstring(*path_item);          //180115
         partially_resolved_potential_kadigans.clone_from(potential_kadigans,kadigan_index + 1); // remaining
         {
            partially_resolved_potential_kadigans.add_wstring(extension);
            CORN::OS::File_name_concrete *resolved= new CORN::OS::File_name_concrete(partially_resolved_potential_kadigans);
            if (!CORN::OS::file_system_engine.exists(*resolved))
            {  delete resolved;
               resolved = 0;
            } else return resolved;
         }
         {  CORN::OS::File_name_concrete *resolved =
               resolve_kadigans
               (partially_resolved_potential_kadigans
               ,path_items,extension);
            if (resolved) return resolved;
         }
      } FOR_EACH_END(each_path_item);
      kadigan_index++;
   } FOR_EACH_END(each_kadigan);
   return 0;
}
//_resolve_kadigans_________________________________________________2017-11-17_/
nat16 Simulation_engine::designate_accessible_files()
{  provide_context_discovery();
   return
     context_discovery->compose_accessible_parameter_files
      (get_control_data_rec(),get_control_file_name_extension(),0)
   + context_discovery->compose_accessible_parameter_files
      (get_scenario_data_rec(),get_scenario_file_name_extension(),0)
   + designate_accessible_scenario_parameter_filenames();
}
//_designate_accessible_files_______________________________________2017-06-29_/
nat32 Simulation_engine::discover_TDF_and_optate()
{  nat32 desired_properties = 0;
   provide_context_discovery();
   CORN::Unidirectional_list TDF_filenames;
   nat16 TDF_filename_count
   = context_discovery->
      extract_accessible_files_with_pattern
      (L"*.TDF",TDF_filenames);
   desired_properties= optation_global.optate_TDFs(TDF_filenames);
   return desired_properties;
}
//_discover_TDF_and_optate__________________________________________2019-01-19_/
}//_namespace_CS_______________________________________________________________/

