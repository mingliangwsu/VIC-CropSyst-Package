#include "scenarios_generator_engine.h"
#include "corn/OS/file_system_engine.h"
#include "corn/data_source/generic_datarec.h"
#include "corn/data_source/vv_file.h"
#include "CS_suite/file_system/CS_context_discovery.h"
#if (defined (_Windows) || defined(_WIN32) || defined(_WIN64))
#   include "corn/OS/file_system_engine_Windows.h"
#endif
namespace CS {
//______________________________________________________________________________
Scenarios_generator_engine::Scenarios_generator_engine(Arguments &arguments_)
: databases          (0)
, arguments          (arguments_)
, predefined_combinations   (arguments_)
{}
//_Scenarios_generator_engine:constructor______________________________________/
Scenarios_generator_engine::~Scenarios_generator_engine()
{}
//_Scenarios_generator_engine:destructor____________________________2017-02-23_/
bool Scenarios_generator_engine::initialize()                    initialization_
{  bool initialized = true;
   // Exclude files and directories that the user
   // might be hidden or not consider.

/* This doesn't work here because arguments processing already copies template to target
   CORN::OS::file_system_engine.exists(arguments.generate_directory);
   if (already_generated)
      std::cerr << "error: Target senarios directory already generated.\n"
         << "(either use a different target name, or delete/rename the existing directory).\n"
         << arguments.generate_directory << std::endl;
   initialized = !already_generated;
*/


   arguments.seclusion.exclude_pattern("desktop");
   arguments.seclusion.exclude_pattern("desktop.ini");
   arguments.seclusion.exclude_pattern(".directory");
   arguments.seclusion.exclude_pattern(".svn/");
   arguments.seclusion.exclude_pattern(".svn/*");
   if (initialized)  // so far viable
      databases = new Databases(arguments.start_directory,arguments.verbose);
         // now allow the the target to be a subscenario to use any accessible database
         //  scenario(*scenarios_directory);
   if (arguments.verbose > 2) //level? program-entry?
      std::clog << "info: Loading predefined scenarios" << std::endl;
   predefined_combinations.load();
   if (arguments.verbose)  //creation
      std::clog << "info: generating " << arguments.generate_directory.c_str() << std::endl;

   /*171215 Disabled, not really needed (Linux scheduling now uses ls_scenario techinque
   ultimates_file = new std::ofstream(arguments.ultimates_filename.c_str());
   */
   return initialized;
}
//_initialize__________________________________________________________________/
nat32 Scenarios_generator_engine::provide_scenario_levels()           provision_
{  provide_database_subdirectories();

   FOR_EACH_IN(subdir,CORN::OS::Directory_name ,database_subdirectories,each_subdir)
   {  if (arguments.seclusion.is_included_DEN(*subdir))
         //170306 result not used Scenario_level *scenario_level =
            provide_new_scenario_level(*subdir);
   } FOR_EACH_END(each_subdir)
   return DB_scenario_levels_primary.count();
}
//_provide_scenario_levels_____________________________________________________/
Scenarios_generator_engine::Scenario_level *Scenarios_generator_engine
::provide_new_scenario_level
(const CORN::OS::Directory_name &dir_name)                           provision_
{  Scenario_level *provided = 0;
   CORN::Text_list path_list;
   dir_name.append_path_list_to(path_list, CORN::OS::Directory_entry_name::include_all);
   while (path_list.find_cstr("Database"))
   {  // strip off the path upto and including Database
      path_list.delete_at(0);
   }
   CORN::Item_wstring *primary_scenario_level_name =
      dynamic_cast<CORN::Item_wstring *>(path_list.pop_tail());
   // the name is actually guarenteed at this point
   if (is_already_leveled(*primary_scenario_level_name))
   {
      delete primary_scenario_level_name;
      return 0;
   }
   if (arguments.scenario_level_recognition.is_included_wstring(*primary_scenario_level_name)) //180118_170205
   {
      path_list.append(primary_scenario_level_name);
         // put the name back on the list.
      provided = DB_scenario_levels_primary.provide_new_scenario_level
         (*primary_scenario_level_name
         ,dir_name // path_list_relinquished
         ,arguments.seclusion
         ,arguments.scenario_level_recognition);
   } else
   {
      if (arguments.is_verbose(1)) //elimitation
      {  std::clog
         << "info: derecognized scenario level: "
         << primary_scenario_level_name->c_str()                                 //180118
        //180118 << (*primary_scenario_level_name)
         << std::endl
         << "dir: " << dir_name << std::endl;
      }
      delete primary_scenario_level_name;
   }
   return provided;
}
//______________________________________________________________________________
bool Scenarios_generator_engine::is_already_leveled(const std::wstring &level_name) affirmation_ //180118
{  return DB_scenario_levels_primary.is_already_leveled(level_name);
}
//_2017-02-05___________________________________________________________________
bool Scenarios_generator_engine::Scenario_levels::is_already_leveled(const std::wstring &level_name) affirmation_ //180118
{
   bool leveled = false;
   FOR_EACH(scenario_level,Scenario_level,each_scenario_level)
   {
      if (scenario_level->name == level_name)
         return leveled = true;
   } FOR_EACH_END(each_scenario_level)
   return leveled;
}
//_is_already_leveled_______________________________________________2017-02-05_/
Scenarios_generator_engine::Scenario_level *
Scenarios_generator_engine::Scenario_levels::provide_new_scenario_level
(const std::wstring             &level_name                                      //180119
,const CORN::OS::Directory_name &dir_name
,const CORN::Seclusion          &seclusions
,const CORN::Seclusion          &scenario_level_recognition)
{

   Scenario_level *scenario_level
      = dynamic_cast<Scenario_level *>(find_wstring(level_name));                //180119
   if (!scenario_level)
   {  scenario_level = new Scenario_level(level_name);
      if (scenario_level->initialize
         (dir_name,seclusions,scenario_level_recognition))
         append(scenario_level);
      else
         { delete scenario_level; scenario_level = 0; }
   }
   return scenario_level;
}
//_provide_new_scenario_level__________________________________________________/
bool Scenarios_generator_engine::Scenario_level::initialize
(const CORN::OS::Directory_name &dir_name
,const CORN::Seclusion &seclusions
,const CORN::Seclusion &scenario_level_recognition)              initialization_
{
   bool inited = true;
   added_subitems(dir_name,seclusions,scenario_level_recognition);
   return inited;
}
//_initialize_______________________________________________________2017-02-05_/
nat32 Scenarios_generator_engine::Scenario_level::added_subitems
(const CORN::OS::Directory_name &dir_name
,const CORN::Seclusion          &seclusions
,const CORN::Seclusion          &scenario_level_recognition)
{
   CORN::Unidirectional_list potential_subscenario_directories;
   nat32 DEN_items = CORN::OS::file_system_engine.
      list_DENs
      (dir_name,&scenario_files,&potential_subscenario_directories
      ,seclusions
      ,CORN::OS::File_system::none_recursion_inclusive);
      // note when there are no scenario directories
      // all files are assumed to be scenarios
      // we dont move files to scenarios list yet
      // because we have to wait till all the directories
      // in all the Databases have been collected.

   for (CORN::OS::Directory_name *pot_sub_scenario_dir
         = dynamic_cast<CORN::OS::Directory_name *>(potential_subscenario_directories.pop_first())
       ;pot_sub_scenario_dir
       ;pot_sub_scenario_dir
        = dynamic_cast<CORN::OS::Directory_name *>(potential_subscenario_directories.pop_first()))
   {
      std::wstring name_with_extW;
      pot_sub_scenario_dir->get_name_with_extension(name_with_extW);
      //180119 std::string name_with_ext; CORN::wstring_to_string(name_with_extW,name_with_ext);
      if (scenario_level_recognition.is_included_wstring(name_with_extW))         //180119
      {
         sublevels.provide_new_scenario_level
            (name_with_extW                                                      //180119
            ,*pot_sub_scenario_dir
            ,seclusions
            ,scenario_level_recognition);
      }
      else delete pot_sub_scenario_dir;
   }
   return DEN_items;
}
//_added_subitems______________________________________________________________/
nat32 Scenarios_generator_engine::provide_database_subdirectories()   provision_
{  return databases->render_subdirectories
      (database_subdirectories,arguments.seclusion
      ,CORN::OS::File_system::subdirectory_recursion_inclusive);
}
//______________________________________________________________________________
nat32 Scenarios_generator_engine::provide_database_parameter_files()  provision_
{  nat32 param_file_count = database_parameter_files.count();
   if (!param_file_count)
   FOR_EACH_IN(database_subdir,CORN::OS::Directory_name
      ,database_subdirectories,each_database_subdir)
   {  param_file_count = CORN::OS::file_system_engine.list_DENs
         (*database_subdir
         ,&database_parameter_files,0
         ,arguments.seclusion
         ,CORN::OS::File_system::subdirectory_recursion_exclusive);
   } FOR_EACH_END(each_database_subdir)
   return  param_file_count;
}
//_provide_database_parameter_files____________________________________________/
nat32 Scenarios_generator_engine
::provide_preestablished_subscenario_directories()                    provision_
{
   CORN::Seclusion generate_dir_seclusion;
   generate_dir_seclusion.exclude_pattern(".svn/");
   generate_dir_seclusion.exclude_pattern("Output/");
   generate_dir_seclusion.exclude_pattern("collate/");
   generate_dir_seclusion.include_pattern("*/");

   nat32 existing_count = existing_subscenarios_leaves.count();
   if (!existing_count)
   {  CORN::Unidirectional_list all_subscenarios;
      CORN::OS::file_system_engine.list_DENs //entry_names
         (arguments.generate_directory
         ,0,&all_subscenarios
         ,generate_dir_seclusion
         ,CORN::OS::File_system::subdirectory_recursion_inclusive);
      // all_subscenarios may include generate scenarios which we now omit.
      #define all_subscenarios_pop_first dynamic_cast<CORN::OS::Directory_name *>(all_subscenarios.pop_first())
      for (CORN::OS::Directory_name *subscenario = all_subscenarios_pop_first
          ;subscenario
          ;subscenario = all_subscenarios_pop_first)
      {  // If there are any subdirectories, this not a preexisting leaf scenario
         CORN::Unidirectional_list any_sub_dirs;
         CORN::OS::file_system_engine.list_DENs
            (*subscenario,0,&any_sub_dirs,CORN::Inclusion("*/",false)
            ,CORN::OS::File_system::subdirectory_recursion_inclusive);
         if (any_sub_dirs.count())
            delete subscenario; // not a leaf subscenario
         else existing_subscenarios_leaves.append(subscenario);
      }
      existing_count = existing_subscenarios_leaves.count();
   } // else we have already provided
   return existing_count;
}
//_provide_preestablished_subscenario_directories______________________________/
bool Scenarios_generator_engine::Arguments::is_scenarios_directory
(const CORN::OS::Directory_name &directory)                  affirmation_
{  // Eventually Scenarios directory will be identified by landmark
   return directory.get_name() == L"Scenarios";
}
//_is_scenarios_directory______________________________________________________/
bool Scenarios_generator_engine::generate()
{  bool generated = true;

   // Get any subscenario directories that the user has defined by hand
   // Any parameter files discovered in these scenarios will be
   // excluded from the combinitorics because the have already been situated
   // in these scenario directories.

   // the target directory is now assumed to be fully
   // generated it is up to the user to decided if generated
   // subdirectories are to be considered permenent
   // the user should always create a new directory inwhich to generate
   // senarios using the --template option to preserve predefine scenario structure
   // gendir_parameter_file_seclusion.exclude_pattern("*.generated");
   // Output directories will not have parameter files.

   if (arguments.REACCH_mode)                                                    //170512
      return generate_REACCH_scenarios();                                        //170512

   if (arguments.is_verbose(2)) //(verbose > 2)
      std::clog << "info: identifying preestablished scenario parameter file selections" << std::endl;
   CORN::Seclusion gendir_parameter_file_seclusion;
   gendir_parameter_file_seclusion.exclude_pattern("Output/");
   gendir_parameter_file_seclusion.exclude_pattern("collate/");

   gendir_parameter_file_seclusion.include_pattern_cstr("*/");
   gendir_parameter_file_seclusion.include_pattern_cstr("*");
   CORN::Unidirectional_list preestablished_scenario_used_parameter_files;
   CORN::OS::file_system_engine.list_DENs //entry_names
         (arguments.generate_directory
         ,&preestablished_scenario_used_parameter_files,0
         ,gendir_parameter_file_seclusion
         ,CORN::OS::File_system::subdirectory_recursion_inclusive);
   if (arguments.is_verbose(4)) // verbose > 3)
      preestablished_scenario_used_parameter_files.write(std::clog);
   if (arguments.verbose > 1) //selection? discovery?
      std::clog << "info: identifying preestablished scenarios." << std::endl;
   provide_preestablished_subscenario_directories();
   // we should now have only leaf directories
   if (arguments.verbose > 2) // selection? discovery?
      existing_subscenarios_leaves.write(std::clog);

   FOR_EACH_IN(used_param_file,CORN::OS::File_name,preestablished_scenario_used_parameter_files,each_used_param_file)
   {
      std::wstring used_param_file_nameW; std::string used_param_file_name;
      used_param_file->get_name_with_extension(used_param_file_nameW);
      CORN::wstring_to_string(used_param_file_nameW,used_param_file_name);
      if (arguments.is_verbose(1)) //rejection
         std::clog << "exclude: " << used_param_file_name.c_str() << std::endl;
      arguments.seclusion.exclude_pattern(used_param_file_name.c_str());
   } FOR_EACH_END(each_used_param_file)

   // We must provides scenario levels have excluding predefined scenario
   // used parameter files because we need to exclude these from
   // from potential scenario levels

   arguments.seclusion.include_pattern("*"); // Include all other files
   arguments.seclusion.include_pattern("*/"); // Include all other directories
   provide_scenario_levels();
   if (existing_subscenarios_leaves.count())
   {  FOR_EACH_IN(preestablished_scenario_leaf,CORN::OS::File_name
                 ,existing_subscenarios_leaves,each_preestablished_scenario)
      {
         if (arguments.verbose > 1) //inclusion
           std::clog << "preestablished scenario: "
              << preestablished_scenario_leaf->c_str() << std::endl;
         if (predefined_combinations.count())
              generate_predefined_combination_scenario_level
                 (DB_scenario_levels_primary,*preestablished_scenario_leaf);
         else generate_scenario_level
            (DB_scenario_levels_primary,0,*preestablished_scenario_leaf);        //170204
      } FOR_EACH_END(each_preestablished_scenario)
   } else
      if (predefined_combinations.count())
           generate_predefined_combination_scenario_level
              (DB_scenario_levels_primary,(arguments.generate_directory));
      else generate_scenario_level(DB_scenario_levels_primary,0,(arguments.generate_directory)); //170204
   CORN::OS::file_system_engine.set_current_working_directory(arguments.start_directory);

   #ifndef __linux__
   write_run_scenarios(arguments.generate_directory,all_scenario_dirs,"5");
   write_run_scenarios(arguments.generate_directory,all_scenario_dirs,"4");
   #endif

   return generated;
}
//_generate____________________________________________________________________/
bool Scenarios_generator_engine::generate_predefined_combination_scenario_level
(const Scenario_levels &scenario_levels
,const CORN::OS::Directory_name &specific_scenario_path )
{
   nat32 generated = 0;
   FOR_EACH_IN(predefined_scenario,Predefined_scenario,predefined_combinations,each_predefined)
   {
       std::string predefined_scenario_name;
      // scenario combination tables dont necessarily have geocoordinates (defining scenarios)
      // it could be simply a set of subscenario combinations
      if (predefined_scenario->Geocoordinate::is_valid())
      {
         // scenario_name is simply the predefined_scenario geocoordinate
         if (predefined_combinations.fields_dBase->geocoordinate_field) // use user specified geocoordinate (code)
         {  // If there is an actual geocoordinate (ISO formatted) field
            predefined_scenario
               ->know_format(predefined_combinations.fields_dBase->geocoordinate_format);
            predefined_scenario->label_string(predefined_scenario_name);
         }
         else // There may be latitude and longitude fields
            // Conceptual: the format options could be specified as command line arguements
            predefined_scenario->
               label_decimal_degrees_with_format
                  (predefined_scenario_name
                  , true // use N S E W
                  ,arguments.precision_latitude    // was 4
                  ,arguments.precision_longitude   // was 4
                  ,'0' // zero_fill_char   (need to check this)
                  ,0,0);  // no elevation no separator
      } else
      {  if (predefined_combinations.fields_dBase->ID_field)
         {  predefined_scenario_name = predefined_scenario->ID;
         } else
         {
            std::cerr << "error: dbf file must have geocoordinate fields or unique ID field" << std::endl;
            return false;
            //Conceptual
            //compose unique name from field_names (that are actual Database context names)
         }
      }
      CORN::OS::Directory_name_concrete scenario_directory(specific_scenario_path,predefined_scenario_name);
      if (decodings_match(predefined_scenario->decodings,scenario_directory))
      {
         bool directory_created = false;                                         //170511
         //170511 only create if actually generated CORN::OS::file_system_engine.create_directory(scenario_directory);
         FOR_EACH_IN(pot_scenario_element
            ,Predefined_scenario::Potential_scenario_element
            ,predefined_scenario->other_potential_elements,each_pot_element)
         {
            std::wstring database_context_name;
            CORN::string_to_wstring(pot_scenario_element->field_name,database_context_name);
            if (database_context_name == L"MUKEY")
               database_context_name = L"Soil";
            CORN::OS::Directory_entry_name *parameter_filename_name_qual
               = databases->render_find_in_context_string_any_extension
               (database_context_name
               ,pot_scenario_element->scenario_value);
            if (parameter_filename_name_qual)
            {
               if (!directory_created)                                           //170511
               {  // Only need to create directory once.
                  CORN::OS::file_system_engine.provide_directory/*180321 create_directory*/(scenario_directory);   //170511
                  directory_created = true;                                      //170511
               }
               std::wstring param_file_name_unqual;
               parameter_filename_name_qual->get_name_with_extension(param_file_name_unqual);
               CORN::OS::File_name_concrete link_file_name(scenario_directory,param_file_name_unqual);
               create_best_kind_of_link(*parameter_filename_name_qual,link_file_name);
            }  // field is not a Database context_name (not error)
         } FOR_EACH_END(each_pot_element)
         generated += generate_scenario_level(scenario_levels,0,scenario_directory); //170204
      } else
      {  // else decodings dont match don't generate scenario
         if (arguments.is_verbose(1)) //exclusion
            std::clog << "info: Not to be simulated:" << std::endl
            << scenario_directory.c_str() << std::endl;
      }
   } FOR_EACH_END(each_predefined)
   return generated > 0;
}
//_generate_predefined_combination_scenario_level___________________2016-11-21_/
bool Scenarios_generator_engine::generate_scenario_level
(const Scenario_levels &scenario_levels
,nat16 scenario_level_index                                                      //170204
,const CORN::OS::Directory_name &specific_scenario_path )
{
/*
don't forget if this scenario level database directory
has both files and folders
we need to make links to each of the (included) files at this generated scenario directory.

for leaf (Database scenarios) directories,
all files are assumed to be scenarios
*/

   bool generated = true;
   if (arguments.is_verbose(1))  //progress?
      std::clog << "ssp:" << specific_scenario_path.c_str() << '\r';

   nat16 scenario_level_count = scenario_levels.count();

   for (nat16 curr_scenario_level_index = scenario_level_index
       ; curr_scenario_level_index < scenario_level_count
       ; curr_scenario_level_index++)
   {
      Scenario_level *scenario_level = dynamic_cast<Scenario_level *>(scenario_levels.get_at(curr_scenario_level_index));//170204
      nat16 next_scenario_level_index = curr_scenario_level_index + 1;
      Scenario_level *next_scenario_level = dynamic_cast<Scenario_level *>(scenario_levels.get_at(next_scenario_level_index));//170204
      CORN::OS::Directory_name_concrete curr_scenario_level_path(specific_scenario_path,scenario_level->name);

      if (scenario_level->directories.count())
      {  // This level (directory) contains subdirectories which are presumed to be subscenarios
         FOR_EACH_IN(source_scenario,CORN::OS::Directory_name,scenario_level->directories,each_scenario)
         {
               std::wstring name_with_extW;
               source_scenario->get_name_with_extension(name_with_extW);
               std::string name_with_ext; CORN::wstring_to_string(name_with_extW,name_with_ext);

               CORN::OS::Directory_name_concrete sub_level_name
                  (specific_scenario_path,name_with_ext);
               //170204 moved below CORN::OS::file_system_engine.create_directory(sub_level_name);   //161209
               // Get and copy source scenario ancillary files
               // we do want to apply any user specified seclusions
               CORN::Unidirectional_list ancillary_files;
               CORN::Unidirectional_list directory_names_would_be_scenarios;
               CORN::OS::file_system_engine.list_DENs                            //161209
                  (*source_scenario,&ancillary_files,&directory_names_would_be_scenarios,arguments.seclusion
                  ,CORN::OS::File_system::none_recursion_inclusive);
               if (directory_names_would_be_scenarios.count())
               {  // When the source scenario has subdirectories (subscenarios) then
                  // any files also in this directory are assumed to be
                  // ancillary files which we now link to in the target.
                  FOR_EACH_IN(ancillary_file,CORN::OS::File_name,ancillary_files,each_ancil_file) //161209
                  {  std::wstring ancillary_file_name_with_ext;
                     CORN::OS::file_system_engine.provide_directory/*180321 create_directory*/(sub_level_name);   //170204
                     CORN::OS::File_name_concrete target_ancillary_filename
                        (sub_level_name,ancillary_file->get_name_with_extension(ancillary_file_name_with_ext));
                     create_best_kind_of_link(*ancillary_file,target_ancillary_filename);
                  } FOR_EACH_END(each_ancil_file)
               }  // else if there are no subdiretories (subscenarios)
                  // the files in this directory represent subscenario
                  // so these will be handled elsewhere
               generate_scenario_level(scenario_level->sublevels,0,sub_level_name); //170204
         } FOR_EACH_END(each_scenario)
      }
      else
      {  // file names are scenario names

         CORN::Container &potential_UED_files = scenario_level->scenario_files;
         CORN::OS::File_name *any_potential_UED_file =
            dynamic_cast<CORN::OS::File_name *>(potential_UED_files.get_at(0));
         if (any_potential_UED_file && any_potential_UED_file->has_extension_ASCII("UED",false))
         {  //if there are any (UED) weather files in the scenario level (source) directory.
            // (which may be excluded from scenarios) as is the case in N2O_emit project
            std::string geocoordinate_ISO_6709;
            CS::Context_discovery discovery(specific_scenario_path);             //161116
            discovery.know_root(discovery.provide_Scenarios());                  //161116
            // It is expected that this program is run in/under a project's
            // Scenarios directory, if not an alternate default root chosen
            // this would likely be a project directory.

               // actually the root is scenario and it is known now
               // but it currently isn't accessible
            Geolocation geolocation;
            bool found_geocoordinate = discovery.get_geolocation
               (specific_scenario_path,"geocoordinate"
               ,geolocation,geocoordinate_ISO_6709);
            if (found_geocoordinate)
            {
               std::wstring weather_name_ext;
               CORN::append_string_to_wstring(geocoordinate_ISO_6709,weather_name_ext);
               weather_name_ext.append(L".");
               weather_name_ext.append(L"UED");

               CORN::Text_list appropriates;
               // I really should propogate the items in the path
               // to be more specific,
               // rather than specific_scenario_path->append_path_list_to
               // but this should work.
               specific_scenario_path.append_path_list_to
                  (appropriates
                  ,CORN::OS::Directory_entry_name::include_all);
               appropriates.add_wstring(weather_name_ext);
               const CORN::OS::File_name &most_appropriate_filename
                  = find_most_appropriate_filename
                     (appropriates,potential_UED_files);
               CORN::OS::File_name_concrete link_file_name(specific_scenario_path,weather_name_ext);
               create_best_kind_of_link(most_appropriate_filename,link_file_name);
            }
         } else
         FOR_EACH_IN(filename_specifies_potential_scenario,CORN::OS::File_name,scenario_level->scenario_files,each_scenario_file)
         {  CORN::OS::Directory_name_concrete scenario_dir_name(specific_scenario_path,filename_specifies_potential_scenario->get_name());
            CORN::OS::file_system_engine.provide_directory/*180321 create_directory*/(scenario_dir_name/*180321 ,true*/);
            {  // All other files would be VV files so we can use inheritance (or symbolic links for linux)
               std::wstring name_ext; filename_specifies_potential_scenario->get_name_with_extension(name_ext);
               CORN::OS::File_name_concrete link_file_name(scenario_dir_name,name_ext);
               create_best_kind_of_link(*filename_specifies_potential_scenario,link_file_name);
            }
            //nat16 next_scenario_level_index = scenario_level_index+1;
            //Scenario_level *next_scenario_level = dynamic_cast<Scenario_level *>(scenario_levels.get_at(next_scenario_level_index));//170204
            if (next_scenario_level)
            {
            generated &= generate_scenario_level                                       //170204
               (scenario_levels
               ,next_scenario_level_index
               ,scenario_dir_name);
            // Since the next level is a predefined (not preestablished) subscenario
            // we need to skip
            curr_scenario_level_index++;
            } else
            {
               /*171215 Disabled, not really needed (Linux scheduling now uses ls_scenario techinque
               if (ultimates_file )
               {  // This is an ultimate scenario.
               // Write the ultimate scenario directory name to ultimates file
               // for use with scheduler.

               //std::clog << "ultimate:" << scenario_dir_name.c_str() << std::endl;

                  std::wstring scenario_dir_name_unqualW
                     (scenario_dir_name.w_str()
                     ,arguments.generate_directory.length() +1
                     ,std::wstring::npos);
                  std::string scenario_dir_name_unqual;
                  CORN::wstring_to_string(scenario_dir_name_unqualW, scenario_dir_name_unqual);;
                  (*ultimates_file) << scenario_dir_name_unqual.c_str() << std::endl;
                  //delete scenario_dir_name_unqual;
               }
               */
               #ifndef __linux__
                  CORN::OS::File_name_concrete run_V5_bat_filename(scenario_dir_name,"run_V5.bat");
                  std::ofstream run_V5_bat_file(run_V5_bat_filename.c_str());
                  run_V5_bat_file << "cmd.exe /c \"C:\\Program Files (x86)\\CS_Suite_5\\CropSyst\\CropSyst_5.exe\"" << std::endl;

                  CORN::OS::File_name_concrete run_V4_bat_filename(scenario_dir_name,"run_V4.bat");
                  std::ofstream run_V4_bat_file(run_V4_bat_filename.c_str());
                  run_V4_bat_file << "cmd.exe /c \"C:\\Program Files (x86)\\CS_Suite_4\\CropSyst\\CropSyst_4.exe\"" << std::endl;
               #endif
            }
         } FOR_EACH_END(each_scenario_file)
      }

   //NYI  for terminal scenario add link(s) to parameter files
   // not already specified by scenario level (I.e. soil file)
   } // for curr_scenario_level_index

//std::cerr << "SSP:" << specific_scenario_path.c_str() << std::endl;

   CORN::Text_list scenario_dirs;
   nat32 scenario_dir_count = CORN::OS::file_system_engine.list_names_in
      (specific_scenario_path,0,&scenario_dirs);
/*this is only writing penultimate
   if (ultimates_file && !scenario_dir_count)
   {  // This is an ultimate scenario.
      // Write the ultimate scenario directory name to ultimates file
      // for use with scheduler.
      std::wstring *specific_scenario_path_unqual = specific_scenario_path
           .render_relative_to_current_working_directory
               (CORN::OS::Directory_entry_name::none_qualification);
      (*ultimates_file) << specific_scenario_path_unqual->c_str() << std::endl;
   }
*/
   #ifndef __linux__
   write_run_scenarios(specific_scenario_path,scenario_dirs,"5");
   write_run_scenarios(specific_scenario_path,scenario_dirs,"4");

   all_scenario_dirs.add_string(specific_scenario_path.c_str());

   /*moved to write_run_scenarios

   CORN::OS::File_name_concrete run_bat_filename(specific_scenario_path,"run.bat");
   std::ofstream run_bat_file(run_bat_filename.c_str());
   if (scenario_dir_count)
   {  FOR_EACH_IN(scenario_dir,CORN::Text_list::Item,scenario_dirs,each_scenario)
      {
         run_bat_file << "PUSHD \""  << scenario_dir->c_str() << "\"" << std::endl;
         run_bat_file << "CALL \"" << "run.bat\"" << std::endl;
         run_bat_file << "POPD " << std::endl;
      } FOR_EACH_END(each_scenario)
   } else
   { // This is just temporary until I get the scenario scheduler running
      run_bat_file << "cmd.exe /c \"C:\\Program Files (x86)\\CS_Suite_5\\CropSyst\\CropSyst_5.exe\"" << std::endl;
   }
   */
   #endif
   return generated;
}
//_generate_scenario_level_____________________________________________________/
#ifndef __linux__
void Scenarios_generator_engine::write_run_scenarios
(const CORN::OS::Directory_name &specific_scenario_path
,const CORN::Text_list scenario_dirs
,const char *V)
{
   std::string run_Vx("run_V"); run_Vx.append(V);

   CORN::OS::File_name_concrete run_bat_filename(specific_scenario_path,run_Vx,"bat");
   std::ofstream run_bat_file(run_bat_filename.c_str());
   if (!scenario_dirs.is_empty())
   {  FOR_EACH_IN(scenario_dir,CORN::Item_wstring/*180119CORN::Text_list::Item*/,scenario_dirs,each_scenario)
      {
         CORN::OS::Directory_name_concrete scenario_dir_absolute(*scenario_dir); //180119
         CORN::OS::Directory_entry_name *scenario_dir_relative
            = CORN::OS::file_system_engine.render_relative_DEN
               (scenario_dir_absolute,arguments.generate_directory);

         run_bat_file << "PUSHD \""  << scenario_dir_relative->c_str() << "\"" << std::endl;
         //run_bat_file << "CALL \"" << "run.bat\"" << std::endl;
         run_bat_file << "cmd.exe /c \"C:\\Program Files (x86)\\CS_Suite_" << V << "\\CropSyst\\CropSyst_"<<V<<".exe\"" << std::endl;
         run_bat_file << "POPD " << std::endl;

         delete scenario_dir_relative;
      } FOR_EACH_END(each_scenario)
   } else
   { // This is just temporary until I get the scenario scheduler running
      run_bat_file << "cmd.exe /c \"C:\\Program Files (x86)\\CS_Suite_" << V << "\\CropSyst\\CropSyst_"<<V<<".exe\"" << std::endl;
   }
}
#endif

//_write_run_scenarios_________________________________________________________/
bool Scenarios_generator_engine::decodings_match
(const CORN::Unidirectional_list &scenario_decodings
,const CORN::OS::Directory_name  &scenario_to_be_generated)         affirmation_
{
   if (!arguments.expected_decodings_count)
      return true; // if there are not specific decodings then match everything.
   bool match = arguments.expected_decodings_count == scenario_decodings.count();
   if (match)
   {  // so far there are the same number of decodings expected has found in the predefined scenario record
      // now see if the decoding is recognized
   FOR_EACH_IN
      (scenario_decoding
      ,Predefined_scenario::Potential_scenario_element
      ,scenario_decodings
      ,each_scenario_decoding)
   {
      CORN::Text_list // should be wstring text list but NYI
            decoded_elements(scenario_decoding->scenario_value.c_str(),'/');
            // I.e. scenario value has the form scenario/subscenario/subsubscenario
      bool scenario_elements_match = true;
      FOR_EACH_IN(decoded_element,CORN::Item_string,decoded_elements,each_decoded_element)
      {
         if (!scenario_to_be_generated.has_in_path_cstr(decoded_element->c_str()
            ,CORN::OS::Directory_entry_name::include_qualification_designated,false))
               scenario_elements_match = false;
      } FOR_EACH_END(each_decoded_element)
      if (!scenario_elements_match) match = false;
   } FOR_EACH_END(each_scenario_decoding)
   }
   return match;
}
//_decodings_match__________________________________________________2017-01-30_/
const CORN::OS::File_name &Scenarios_generator_engine::find_most_appropriate_filename
(const CORN::Text_list &appropriates
,const CORN::Container &canidate_filenames)                                const
{  const CORN::OS::File_name *most_appropriate_candidate = 0; nat8 mosts_appropriateness = 0;
   FOR_EACH_IN(candidate,const CORN::OS::File_name,canidate_filenames,each_candidate)
   {  CORN::Text_list candidate_path_items;
      candidate->append_path_list_to(candidate_path_items,CORN::OS::Directory_entry_name::include_all);
      nat8 appropriateness = 0;
      FOR_EACH_IN(appropriate,CORN::Item_wstring,appropriates,each_approp)
      {  FOR_EACH_IN(dir_name,CORN::Item_wstring,candidate_path_items, each_dir_name)
         {  if ((*dir_name) == (*appropriate))
               appropriateness++;
         } FOR_EACH_END(each_dir_name)
      } FOR_EACH_END(each_approp)
      if (appropriateness >= mosts_appropriateness)
      {  // The >= would select the matching file closest to the scenario directory
         most_appropriate_candidate = candidate;
         mosts_appropriateness = appropriateness;
      }
   } FOR_EACH_END(each_candidate)
   return *most_appropriate_candidate;
}
//_find_most_appropriate_filename______________________________________________/
bool Scenarios_generator_engine::create_best_kind_of_link
(const CORN::OS::File_name &source_filename_absolute
,const CORN::OS::File_name &target_filename)
{  bool linked = true;
   if (CORN::OS::file_system_engine.exists(target_filename)) return false;


   continue here use new source and target arguments

   CORN::OS::File_name *source_filename_relative                                 //170412
      =  arguments.absolute_filenames ? 0 :
         CORN::OS::file_system_engine.render_relative_DEN
            (source_filename_absolute
            ,target_filename.get_parent_directory_name_qualified());
   const CORN::OS::File_name &source_filename = source_filename_relative
      ? (*source_filename_relative) : source_filename_absolute;
   if (is_VV_file(source_filename_absolute))
      // Not currently checking if actually file type does support iheritance
      // as most CS Suite VV (parameter) files support inheritance linking
   {
      std::ofstream target(target_filename.c_str());
      target
         << "[inherit]"                      << std::endl
         << "1=" << source_filename.c_str()  << std::endl;
   } else
   {  // file format not recognized as having inheritance linking
      // so default to soft links
      // prefer soft links,
      #ifdef __unix__
      CORN::OS::file_system_engine.create_file_link
         (source_filename,target_filename,LINK_SYMBOLIC);
      #endif
      #ifdef _Windows
      {
         CORN::OS::File_system_engine_Windows *file_sys_engine_Windows
            = dynamic_cast<CORN::OS::File_system_engine_Windows *>
               (&CORN::OS::file_system_engine);
                  // in Windows softlinks are poorly implemented
                  // so use hard links or shortcuts.
         if (file_sys_engine_Windows)
         {  std::wstring file_and_ext;
            linked = file_sys_engine_Windows->create_shortcut
               (source_filename_absolute // The file to link to
                  // currently windows only allows fully qualified names!
                  // [It may take relative names but I haven't tested this]
               ,target_filename.get_parent_directory_name_qualified()
               ,target_filename.get_name_with_extension(file_and_ext)
               ,L"Link");
         } else linked = false;
      }
      #endif
   }
   delete source_filename_relative;                                              //170412
   return linked;
}
//_create_best_kind_of_link____________________________________________________/
bool Scenarios_generator_engine::is_VV_file
(const CORN::OS::File_name &target_filename)                        affirmation_
{
   std::ifstream target(target_filename.c_str());
   std::string line;
   // read first few lines
   for (nat8 c = 0; c <20; c++)
   {
      getline(target,line);
      // if has [ and ] then this appears to be VV file return true;
      size_t l_brace_pos = line.find_first_of('[');
      size_t r_brace_pos = line.find_first_of(']');
      if ((l_brace_pos == 0)                                                     //161117
        &&(l_brace_pos != std::string::npos)
        &&(r_brace_pos != std::string::npos))
         return true;
   }
   return false;
}
//_is_VV_file__________________________________________________________________/
Scenarios_generator_engine::Arguments::Arguments()
: ARGUMENTS_SUPERCLASS()
, generate_directory()
// seclusion defaults to none
// scenario_level_recognition defaults to none
//, target_name_template_composed("")
//, target_name_specified("")
, expected_decodings       ("")                                                  //170131
, expected_decodings_count (0)                                                   //170511
, ultimates_filename       ("ultimates")                                         //170223
, REACCH_mode              (false)                                               //170512
//, scenario_level_recognition("*")                                                //171117
{  generate_directory=new CORN::OS::Directory_name_concrete;                     //161205
   find_responses = FIND_RESPONSES_LAST;
}
//______________________________________________________________________________
Scenarios_generator_engine::Arguments::~Arguments()
{}
//______________________________________________________________________________
bool Scenarios_generator_engine::Arguments::write_help()                         const
{  std::clog
      << "All arguments are optional and there is no fixed order." << std::endl << std::endl;
   return ARGUMENTS_SUPERCLASS::write_help();
}
//______________________________________________________________________________
bool Scenarios_generator_engine::Arguments::get_author_mailto_URI(std::string &email_address) const
{  email_address = "mailto:rnelson@wsu.edu";
   return true;
}
//______________________________________________________________________________
bool Scenarios_generator_engine::Arguments::recognize_option(const std::string &paramstring)
{  bool recognized = false;
   if (!recognized)
   {
      if ((paramstring=="REACCH"))
      {
         REACCH_mode = true;
         recognized = true;
      }
   }
   return recognized || CORN::Arguments::recognize_option(paramstring);
}
//_recognize_option_________________________________________________2017-05-12_/

//200206  redo: recognize_assignment is obsolete, use expect_structure and move these to get end


bool Scenarios_generator_engine::Arguments::recognize_assignment
(const std::string &variable
,const std::string &value)                                         modification_
{  bool recognized = false;
   if (variable == "--target")
   {
      CORN::string_to_wstring(value,target_name_specified);
      CORN::OS::Directory_name_concrete target_gen_dir(start_directory,target_name_specified);
      generate_directory.set_wstr(target_gen_dir.w_str());
      recognized = true;
   }
   if (variable == "--template")
   {
      CORN::string_to_wstring(value,template_scenario);
      CORN::OS::Directory_name *template_scenario_dirname_var_qual = new
         CORN::OS::Directory_name_concrete(template_scenario);
      CORN::OS::Directory_name *template_scenario_dirname_qual                   //171130
         = CORN::OS::file_system_engine.render_resolved_absolute_DEN
            (*template_scenario_dirname_var_qual
            ,start_directory);
      if (verbose > 3)  // inclusion
         std::clog << "info: template "
         << template_scenario_dirname_qual->c_str()
         << std::endl;
      if (CORN::OS::file_system_engine.exists(*template_scenario_dirname_qual))
      {
         if (verbose > 2) // progress?
            std::clog << "info: copying template." << std::endl; // << template_scenario->c_str() << std::endl;
         CORN::OS::file_system_engine.copy_directory_contents                    //161105
            (*template_scenario_dirname_qual,generate_directory,0,true,false);
      } else
      {  std::clog
            << "error: unable to find specified template scenario" << std::endl
            << "dir: " << value << std::endl;
      }
      delete template_scenario_dirname_qual;
      delete template_scenario_dirname_var_qual;
      recognized = true;
   } else if (variable == "--recognition")
   {
      std::wstring valueW;
      CORN::string_to_wstring(value,valueW);
      CORN::OS::File_name *found_recognition_file =
         CORN::OS::file_system_engine.find_qualified_name_in_path
            (valueW
            ,generate_directory //171117 start_directory
            );
      if (found_recognition_file)
      {
         if (verbose > 1) //discovery (inclusion)
            std::clog << "info: found and loading recognition file (specified on the command line)" << found_recognition_file->c_str() << std::endl;
         scenario_level_recognition.include_from(found_recognition_file->c_str());
         recognized = true;
      } else
      {  std::cerr << "warn: recognition file not found: " << value.c_str() << std::endl;
      }
   } else if (variable == "--decode")                                            //170130
   {  CORN::OS::File_name_concrete decode_file(value);
      if (decode_file.is_qualified())
         load_expected_decodings(decode_file);
      else
      {  std::wstring decode_filename_unqual;
         CORN::string_to_wstring(value,decode_filename_unqual);
         CORN::OS::File_name *found_decode_file =
            CORN::OS::file_system_engine.find_file_name                          //150412
               (decode_filename_unqual
               ,start_directory
               ,CORN::OS::File_system::superdirectory_recursion_inclusive
               //NYI may want to specify logical root
               );
         if (found_decode_file)
         {  load_expected_decodings(*found_decode_file);
            delete found_decode_file;
         }
      }
   } else if (variable == "--ultimates")
   {  ultimates_filename = value;
      recognized=true;
   } 
   return recognized || ARGUMENTS_SUPERCLASS::recognize_assignment(variable,value);
}
//_recognize_assignment________________________________________________________/
bool Scenarios_generator_engine::Arguments::load_expected_decodings
(const CORN::OS::File_name &decode_filename)                       modification_
{
   expected_decodings_count = 0;
   // This should append to decodings so we must preserve structure
   CORN::VV_File decoding_file(decode_filename.c_str(),true);
   bool loaded = decoding_file.get(expected_decodings);
   if (loaded)
   {  FOR_EACH_IN
         (variable_section,CORN::VV_Section
         ,expected_decodings.sections,each_expected_decoding_section)
      {  expected_decodings_count += (variable_section->entries.count() > 0);
      } FOR_EACH_END(each_expected_decoding_section)
   }
   return loaded;
}
//_load_expected_decodings__________________________________________2017-01-30_/
bool Scenarios_generator_engine::Arguments::get_end()
{
   // At this point we should have complied template scenario
   // so set the generate directory as CWD this will allow the
   // get_end() to discover any  seclusion files copied from the template.
   CORN::OS::file_system_engine.set_current_working_directory(generate_directory);
   return ARGUMENTS_SUPERCLASS::get_end();
}
//______________________________________________________________________________
bool Scenarios_generator_engine::Predefined_scenarios::provide_fields_dBase
(CORN::Data_source_dBase &/*DBF_file*/)                               provision_
{  // DBF_file is not used here, but it may be in other implementations
   // Check as it may be obsolete.
   if (!fields_dBase)
      geocoord_fields_dBase = fields_dBase = new Predefined_scenario::Fields_dBase;
   return fields_dBase != 0;
}
//______________________________________________________________________________

190925 redo this now use Data_record (expect_structure)

bool Scenarios_generator_engine::Predefined_scenarios::setup_DBF_fields
(CORN::Data_source_dBase &DBF_file)                                   provision_
{
   bool satup = geodesy::Geocoordinates::setup_DBF_fields(DBF_file);
   // Recognize either SOIL or MUKEY column
   fields_dBase->soil_MUKEY_field = DBF_file.lookup_field("SOIL");
   if (!fields_dBase->soil_MUKEY_field)
      fields_dBase->soil_MUKEY_field = DBF_file.lookup_field("MUKEY");
   //if (!arguments.ID_field_name.is_empty())
   fields_dBase->ID_field
      = DBF_file.lookup_field(arguments->ID_fieldname.c_str());
         //171111 ("ID");
   if (!fields_dBase->ID_field) fields_dBase->ID_field
      = DBF_file.lookup_field("NAME"); // Name is currently alias for ID
   fields_dBase->soil_area_field = DBF_file.lookup_field("SOILAREA");            //170518
   // May need to  compose a list of remaining arbitrary fields

   return satup;
}
//______________________________________________________________________________
bool Scenarios_generator_engine::Predefined_scenarios
::table_has_a_geocoordinate_fields()                                affirmation_
{ // was DBF_has
   bool has_ID_field = fields_dBase->ID_field;
   // Old REACCH cell files ID encoded the geocoordinate
   return has_ID_field||Geocoordinates::table_has_a_geocoordinate_fields();
}
//_table_has_a_geocoordinate_fields____________________________________________/

// 190925 redo this

bool Scenarios_generator_engine::Predefined_scenarios::set_from_record
(CORN::Generic_data_record &scenarios_rec
,Geocoordinate &geocoord_)
{
   bool sat = true;
   Predefined_scenario *scenario = dynamic_cast<Predefined_scenario *>(&geocoord_);
   CORN::VV_Section *section = scenarios_rec.get_current_section();
   Predefined_scenario::Fields_dBase *as_scenario_fields_dBase
      = dynamic_cast<Predefined_scenario::Fields_dBase *>(fields_dBase);
   if (as_scenario_fields_dBase->soil_MUKEY_field)
   {
      CORN::VV_string_entry *soil_MUKEY_entry =
         dynamic_cast<CORN::VV_string_entry *>

         //171111 WARNING may need to be  CORN::Item as is the case for ID_field


            (section->entries.find_cstr(as_scenario_fields_dBase->soil_MUKEY_field->get_key(),false));
      if (soil_MUKEY_entry)
         soil_MUKEY_entry->append_to_string(scenario->soil_MUKEY);
   }
   if (as_scenario_fields_dBase->ID_field)
   {
      //171111 CORN::VV_string_entry *ID_entry = dynamic_cast<CORN::VV_string_entry *>
      CORN::Item *ID_entry = dynamic_cast<CORN::Item *>
            (section->entries.find_cstr(as_scenario_fields_dBase->ID_field->get_key(),false));
      if (ID_entry)
          ID_entry->append_to_string(scenario->ID);
   }
   if (as_scenario_fields_dBase->soil_area_field)                                //170518
   {
      CORN::VV_float32_entry *soil_area_entry = dynamic_cast<CORN::VV_float32_entry *>


         //171111 WARNING may need to be  CORN::Item as is the case for ID_field


            (section->entries.find_cstr(as_scenario_fields_dBase->soil_area_field->get_key(),false));
      if (soil_area_entry)
         scenario->soil_area = soil_area_entry->get_value();
   }

   // save all other fields so we can potentially compose from them
   FOR_EACH_IN(entry,CORN::VV_entry_item,section->entries,each_entry)
   {  std::string entry_key; entry->key_string(entry_key);
      bool entry_is_encoded = false;
      FOR_EACH_IN(variable_section,CORN::VV_Section
         ,generator_arguments.expected_decodings.sections,each_expected_decoding_section)
      {  if (entry_key == variable_section->get_section_label())
         {  std::string entry_value; entry->append_to_string(entry_value);
            const CORN::VV_entry_item *decoded_item
               = dynamic_cast<const CORN::VV_entry_item *>
                  (variable_section->entries.find_string(entry_value));
            if (decoded_item)
            {
               std::string decoded_value; decoded_item->append_to_string(decoded_value);
               scenario->decodings.take(new Predefined_scenario::Potential_scenario_element(entry_key,decoded_value));
               entry_is_encoded = true;
            } else entry_is_encoded = false;
         }
      } FOR_EACH_END(each_expected_decoding_section)
      if (!entry_is_encoded)
      {
         Predefined_scenario::Potential_scenario_element *pot_scenario_elem
               = new Predefined_scenario::Potential_scenario_element(*entry);
         scenario->other_potential_elements.take(pot_scenario_elem);
      }
   } FOR_EACH_END(each_entry)
   sat  &= Geocoordinates::set_from_record(scenarios_rec,geocoord_);
   if (!geocoord_.is_valid())  // Special case for REACCH                        //170510
   {
      CORN::VV_int32_entry *ID_entry = dynamic_cast<CORN::VV_int32_entry *>
         (section->entries.find_cstr(as_scenario_fields_dBase->ID_field->get_key(),false));
      if (ID_entry)
      {
         nat32 ID_raw = ID_entry->get_value();
         float32 latitude = (float32)((int32)(ID_raw / 100000)) / 100.0;
         float32 longitude = -((float32)(ID_raw % 100000) / 100.0);
         geocoord_.set_latitude_dec_deg_f32 (latitude);
         geocoord_.set_longitude_dec_deg_f32(longitude);
      }
   }
   return sat;
}
//_set_from_record_____________________________________________________________/
Scenarios_generator_engine::Predefined_scenario::Potential_scenario_element
::Potential_scenario_element(const CORN::VV_entry_item &item)
: CORN::Item()
, field_name      (item.get_key())
, scenario_value  ()
{  item.append_to_string(scenario_value);
}
//______________________________________________________________________________
const char *Scenarios_generator_engine::REACCH_Predominant_soil::key_string
(std::string &buffer)                                                      const
         { buffer.assign(ID); return buffer.c_str(); }
/*
const char *Scenarios_generator_engine::REACCH_Predominate_soil::get_key()                                 const
      { std::string buffer; return key_string(buffer); }                         //170517
*/
//______________________________________________________________________________

} // namespace CS

