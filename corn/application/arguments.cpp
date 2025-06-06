#include "corn/application/arguments.h"
#include "corn/application/progress.h"
#include "corn/OS/file_system_engine.h"
#include "corn/container/text_list.h"

#include "corn/data_source/vv_file.h"
#include "corn/data_source/command_options_datasrc.h"
#include "corn/seclusion.h"
#include "corn/string/strconv.hpp"
#include <iostream>
#ifdef _Windows
#  include <windows.h>
#endif

namespace CORN
{
using namespace OS;                                                              //200818
using namespace OS::FS;                                                          //200818
int8 global_verbosity_depth=0;                                                   //170317
//______________________________________________________________________________
Arguments::Arguments() // NYI (const char *program_path_name_)
: CORN::Data_record("arguments")
, manual_URI               ()
, program_filename         (0)
, verbose_flag             (false)                                               //191101
, verbosity                (0)                                                   //170217
, wizard                   (false)
, quiet                    (false)
, debug                                                                          //170317
   #ifdef DEBUG_
      (true)
   #else
      (false)
   #endif
, trial                    (false)                                               //170822
, force                    (false)
, replace                  (true)
, update                   (false)
/*200801 moved
, recursion                (CORN::File_system::none_recursion_inclusive)         //170616
, recursion_depth          (0)                                                   //180801
, depth_first              (false)                                               //191101
*/
, help                     (false)
, find_responses           (0)                                                   //170601
, preferred_response_filename ("")                                               //191104
, absolute_filenames       (false)                                               //170411
, collect_remaining_deprecated (0)                                               //170616
, success_filename
#ifdef __linux__
   ("success")                                                                   //180208
#else
   // Under Windows, not using the status file idiom by default
   ("")
#endif

, languages                ("")
, start_directory          () // defaults to CWD
// private
, encountered_count        (0)                                                   //200811
, prompts_shown            (false)
, required_arguments       ()                                                    //191104
, recurse                  (false)                                               //191104
/*200801
, recursion_raw            ()
*/
, progress_raw             ()
, unrecognized             ()                                                    //191104
, path_discoveries         ()                                                    //200807
/*200807
, discovery_source         (0)                                                   //200804
, discovery_target         (0)                                                   //200804
*/
{

//200807  NYI   I now want to used Path_discovery for reponse files

   response_filename_extension = L"rsp";                                         //171110
      // .rsp was used in CS V4
      // From V5 now using .response
      // I need to make sure that any utilities (such as UED import/export)
      // now use response.
      // This assignment is ONLY to support legacy CS suite utilties

   // setup for basic recursion (used if recurse)
   recursion.direction = descendent;                                             //200818
   recursion.depth.max = to_extreme;                                             //200818
   recursion.depth.min = from_CWD;                                               //200818
}
//_Arguments:constructor____________________________________________2016-08-25_/
Arguments::~Arguments()
{}
//__Arguments:destructor____________________________________________2019-02-13_/
class Expected_single_filename
: public extends_ CORN::Item
{
 public:
   std::wstring &ref_filename;
   Seclusion *seclusion; // owned
 public:
   inline Expected_single_filename
      (std::wstring &filename_ref
      ,Seclusion *seclusion_given)
      : CORN::Item()
      , ref_filename(filename_ref)
      , seclusion(seclusion_given)
      { }
   inline virtual ~Expected_single_filename()
      {  delete seclusion;
      }
};
//_class_Expected_single_filename___________________________________2019-04-09_/
const /*CORN::OS::*/File_name * Arguments::know_program
(const char    *program_path_name_)
{  delete program_filename;
   program_filename = new CORN::OS::Directory_name_concrete(program_path_name_);
   return program_filename;
}
//_know_program_____________________________________________________2017-04-03_/
const /*CORN::OS::*/File_name * Arguments::know_program
(const wchar_t *program_path_name_)
{  delete program_filename;
   program_filename = new CORN::OS::Directory_name_concrete(program_path_name_);
   return program_filename;
}
//_know_program_UnicodeZ____________________________________________2017-04-03_/
const /*CORN::OS::*/File_name *Arguments::get_program_filename()               const
{  return  program_filename;
}
//_get_program_filename_____________________________________________2017-04-03_/
const /*CORN::OS::*/Directory_name *Arguments::get_program_directory()         const
{  return program_filename
   ?  program_filename->get_path_directory_name()
   : 0;
}
//_get_program_directory____________________________________________2017-04-03_/
bool Arguments::expect_structure(bool for_write)
{
   set_current_section(primary_section_name.c_str());                            //200807                                                  //200807
   bool expected
   =  expect_switch(verbose_flag    ,"--verbose"   ,"-v","Verbose log")
   && expect_switch(recurse         ,"--recursive" ,"-r","Recurse","into subdirectories")
   && expect_bool("-R",recurse,VV_bool_entry::form_unary,"Recurse","into subdirectories")
   && expect_switch(quiet           ,"--quiet"     ,"-q","Quite","Don't show or stop for prompts")
   && expect_switch(debug           ,"--debug" ,"-debug","Show debugging information","in addition for verbose log")
   && expect_switch(absolute_filenames,"--absolute" ,"-a","Absolute filename","in outputs")
   && expect_switch(trial           ,"--dry-run" ,"-trial","Trial or practice","Do not create/modify files")
   && expect_switch(force           ,"--force" ,"-f","Force overwrite of target file(s)","replaces/rewrites" )
   && expect_switch(update          ,"--update" ,"-u","Update target file(s)","augments/appends")
   && expect_switch(replace         ,"--replace" ,0,"Replace target file(s)")
   && expect_switch(help            ,"--help" ,"-h","Show help")
   && expect_string  ("--success"      , success_filename,ANY_STRING_LENGTH,"Create success files","when using success files to identify run status")
      // doesn't need to be expect_assignment because not required
   && expect_string  ("--language"     , languages.get_SDF_for_data_record_expect(),ANY_STRING_LENGTH,"Preferred languages","listed in order of preferrence")
   && expect_string  ("--response"     , preferred_response_filename)
   && expect_nat8    ("--verbosity"    , verbosity,10,"Amount of log output")    //200216
   // for the basic recursion, it is always assumed to be descendent             //200818
   && expect_nat8    ("-maxdepth" , recursion.depth.max,10                       //200818
      ,"Descend at most levels of directories below the starting-points. 0 means only apply to start directory. 255 (default) means apply to extreme.")
   && expect_nat8    ("-mindepth" , recursion.depth.min,10                       //200818
      ,"Only processes levels at or below the specified level. 0 (default) is the start directory.")
   // Note that linux du command has -max-depth
   /* conceptual  (find uses single -)
   && expect_nat8    ("--maxdepth", recursion.depth.max,10                       //200818
      ,"Descend at most levels of directories below the starting-points. 0 means only apply to start directory. 255 (default) means apply to extreme.")
   && expect_nat8    ("--mindepth", recursion.depth.min,10                       //200818
      ,"Only processes levels at or below the specified level. 0 (default) is the start directory.")
   */
   /*
   && expect_switch(depth_first     ,"--depth","-d","Depth first recursion")
   && expect_string  ("--recursion"    , recursion_raw)
   && expect_nat8    ("--maxdepth"     , recursion_depth,10,"limit of subdirecties to recurse into","0 = only apply processing only to the start directory.")
   */
   ;
   // Need to reset the primary section for each because
   // the discovery parameters establish sections.
   set_current_section(primary_section_name.c_str());                            //200807                                                  //200807
   FOR_EACH_IN(path_discovery,/*OS::FS::*/Path_discovery,path_discoveries,discovery_iter)    //200807
   {  path_discovery->setup_structure(*this,for_write);                          //200807
      set_current_section(primary_section_name.c_str());                         //200807                                                  //200807
   } FOR_EACH_END(discovery_iter)
/*200807
   if (discovery_source)
   {  discovery_source->setup_structure(*this,for_write);
      set_current_section(primary_section_name.c_str());                            //200807                                                  //200807
   }
   if (discovery_target)
   {  discovery_target->setup_structure(*this,for_write);
      set_current_section(primary_section_name.c_str());                            //200807                                                  //200807
   }
*/
   expected &= Data_record::expect_structure(for_write);
   return expected;
}
//_expect_structure_________________________________________________2018-05-14_/
bool Arguments::take(Item_string *arg)                               submission_
{  if (!arg) return false;
   bool taken = false;
   FOR_EACH_IN(path_discovery,/*OS::FS::*/Path_discovery,path_discoveries,discovery_iter) //200807
   {  if (! taken)                                                               //200807
      {
         taken = path_discovery->take(arg);                                      //200807
         //200811 if (taken) return true;
      }
   } FOR_EACH_END(discovery_iter)                                                //200807
   if (!taken)                                                                   //200811
   if (collect_remaining_deprecated)                                             //170616
   {  // Must be done first because once we have identified that the remaining
      // args are to be collected, the remaining arguments are not further considered
      collect_remaining_deprecated->add_string(*arg);
      //191104 recognized = true;
      delete arg; arg = 0; taken = true; //taken = true;                          //200212
   }
   if (!taken)                                                                   //200811
   {  // do the switches here because the entry may have  -- or - prefix
      FIND_FOR_EACH_IN(found_section,section,VV_Section,sections,true,each_section)
      {  FIND_FOR_EACH_IN(found_entry,entry,VV_abstract_entry,section->entries,true,each_entry)
         {  if (entry->is_key_string(*arg))
            {  VV_bool_entry *as_bool_entry = dynamic_cast<VV_bool_entry *>(entry);
               if (as_bool_entry)
               {  as_bool_entry->set(true);
                  taken = true;                                                  //200212
                  found_entry = entry;                                           //200216
                  found_section = section;                                       //200216
               }
            }
         } FOR_EACH_END(each_entry)
      } FOR_EACH_END(each_section)
      if (taken) {delete arg; arg = 0; taken = true; /*200811 return true;*/}    //200216
   }
   if (!taken)                                                                   //200811
   {  // Wizard must not be in expect strcture because
      // we dont want wizard option itself to shown in wizard output
      if (arg && ((*arg) == "--wizard"))                                         //171020
      {  wizard = true;
         delete arg; arg = 0; taken = true;/*200811  return true; */             //200212
      }
   }

   /*200807 I think this can be obsolete,  simply specify response with Path discovery
   this was only used for scenario generation
   {  if ((*arg) == "--responses")
      {
         find_and_read_response_files();
         delete arg; arg = 0; return true;                                       //200212
      }
   }
   */
   // In some cases that may be multiple assignment style arguments
   // (with the same argument name)
   // For example, in seclusion, there may be multiple arguments
   // where the ordering of the occurance of the arguments builds up
   // a list
   if (!taken)                                                                   //200811
   {  std::size_t equal_pos = arg->find('=');
      bool assignment = equal_pos != std::string::npos;                          //171120
      if (assignment)
      {  // Appears to be an assignment argument
         std::string variable (*arg,0,equal_pos);
         std::string value    (*arg,equal_pos+1);
         taken = take_assignment(variable,value);
      }
      if (taken) { delete arg; arg=0; return true;}                              //200201
   }
   // standalone progress
   if (!taken)                                                                   //200811
   if ((*arg).find("-progress") != std::string::npos) // also recognizes --progress
   {  provide_progress_display_global();                                         //170519
      taken = true;                                                              //200811
      //200811 {delete arg; arg = 0; return true; }                                       //200212
   }
   //191103 Option arguments are now handled with expect_switch
   // with expect_structure idiom
   // once all these are done, we can delete this
   if (!taken)                                                                   //200811
   {  if ((*arg)[0] == '-')
      {  std::size_t option_pos = 1;
         if ((*arg)[1] == '-')  // -- form
            option_pos = 2;
         std::string option((*arg),option_pos);
         bool recognized = recognize_option(option);
         if (!recognized)                                                        //170308
         {  // multiple single character options may be strung together
            // I.e.  -xyz is equivelent to -x -y -z
            // So if -xyz is not a recognized specific option
            recognized = true;
            for (nat8 n = 0; n < option.length(); n++)
            {  std::string option_n(option,n,1);
               recognized &= recognize_option(option_n);
            }
         }
         if (recognized) taken = true;                                           //200811
            //200811 { delete arg; arg=0; return true; }                     //200212
      }
   }
   if (taken && arg) { delete arg; arg = 0; }                                    //200811
   encountered_count += taken;                                                   //200811
   return taken;
}
//_take_____________________________________________________________2019-11-03_/
bool Arguments::take_unrecognized(Item_string *arg)
{  return unrecognized.take(arg); // take(0) does nothing;
}
//_take_unrecognized________________________________________________2020-02-12_/
bool Arguments::take_assignment
(const std::string &variable
,const std::string &value)                                         modification_
{  bool recognized = false;

   //NYI I need better way to handle the different progress methods
   // probably would have [progress] section or progress:xxxx
   if (variable == "--progress")                                                 //170519
      recognized
      = provide_progress_display_global().parameter.set_indicator(value);        //170519
   else if (variable.find("--progress") != std::string::npos)                    //170316
      recognized
      = provide_progress_display_global()
         .parameter.recognize_assignment(variable,value);                        //170519
   return recognized;
}
//_recognize__assignment_____________________________________________2016-10-13_/
bool Arguments::read_response_file_potentially
(const /*CORN::OS::*/Directory_entry_name &param_DEN)                  modification_
{  bool recognized = false;
   bool response_filename_encountered =
      response_filename_extension.length() &&                                    //170725
      param_DEN.has_extension(response_filename_extension);
   if (response_filename_encountered)
      // Attempt to read the response file
      // (if it is not found it will not be read so considered unrecognized.
      recognized = read_response_file(param_DEN);
   return recognized;
}
//_read_response_file_potentially___________________________________2016-10-15_/
bool Arguments::read_response_file                                 modification_
(const /*OS::*/File_name &response_filename)
{  bool was_VV_format = false;                                                   //181001
   {  // first assume response file is in VV format                              //181001
   VV_File VV_response_file(response_filename.c_str());                          //181001
   was_VV_format = VV_response_file.get(*this);                                  //181001
   if (was_VV_format) return true;                                               //181001
   }                                                                             //181001
   // else the reponse file is simply a set of lines
   Command_options_data_source response_file(response_filename);
   return response_file.get(*this); // get arguments
}
//_read_response_file_______________________________________________2016-10-15_/

/*200807 obsolete   but dont delete yet,  replace this with Path_discovery

nat32 Arguments::find_and_read_response_files()
{  Unidirectional_list response_files_accessible;
   std::wstring response_file_pattern_raw(L"*.");                                //191104
   response_file_pattern_raw.append(get_response_file_extension());              //191104
   CORN::Inclusion response_inclusion(response_file_pattern_raw,true);           //191212
   nat32 response_file_count = discover_many                                     //191104
      (response_files_accessible
      ,response_inclusion                                                        //191212
      ,file_entry
      ,File_system::superdirectory_recursion_inclusive);
   nat8 hold_find_responses = find_responses;
   find_responses = false;
   // while reading the response files disable find otherwise infinite loop.
   if (verbosity > 1 && response_file_count)
      std::clog << "info: loading accessible response files." << std::endl;
   FOR_EACH_IN(response_file,File_name
              ,response_files_accessible,each_response_file)
   {  read_response_file(*response_file);
   } FOR_EACH_END(each_response_file)
   find_responses = hold_find_responses;
   return response_file_count;
}
//_find_and_read_response_files_____________________________________2016-10-21_/
*/

bool Arguments::get_start()
{
   /*200807 obsolete  derived classes will append Path_discoveries as needed
   discovery_source = provide_discovery_source();                                //200801
   discovery_target = provide_discovery_target();                                //200801
   */

   /*200807 reimplement  append response file path discovery

   derived classes can override extension

   append response file path discovery


   if (find_responses & FIND_RESPONSES_FIRST)
      find_and_read_response_files();

   */
   return true;
}
//_get_start________________________________________________________2016-10-21_/
bool Arguments::get_end()
{  bool ended = true;

   //200811 NYI encountered_count += Data_record::count_encountered();                        //200811
   {
   // Should read response file first inorder to have options
   // for following get_end processing
   if (preferred_response_filename.length())                                              //191101
   {  /*CORN::OS::*/File_name_concrete response_filename(preferred_response_filename);
      read_response_file(response_filename);
   }
   }
   //191101 continue here need also to discover (collect response file pattern) and
   // load discovered response files.
   if (verbose_flag && !verbosity ) verbosity = 127; // ?or 127;                                  //191101
   /*200801 moved
   {  // This is additive                                                        //191101
         if      (recursion_raw.find("subdirectory")   != std::string::npos) recursion |= FILE_SYSTEM_RECURSION_SUBDIRECTORY;
         else if (recursion_raw.find("superdirectory") != std::string::npos) recursion |= FILE_SYSTEM_RECURSION_SUPERDIRECTORY;
         else if (recursion_raw.find("exclusive")      != std::string::npos) recursion |= FILE_SYSTEM_RECURSION_EXCLUSIVE;
   }
   */


/*200807  reimplement   now using Path_discovery

   lookup reponse file discovery
   for each discovered file read




   if (find_responses & FIND_RESPONSES_LAST)                                     //161021
       find_and_read_response_files();

*/


   if (wizard) ended &= write_wizard(std::cout);                                 //180515

   #ifndef USE_EXPECT_STRUCTURE

   // Here we could process verbose progress recursion
/*200801
   if (recurse)                                                                  //180514
      recursion = CORN::OS::File_system::subdirectory_recursion_inclusive;
*/
   #endif
   if (help) write_help();
   // response files are processed as encountered
   if (!quiet
      // && has_interactive_prompts()
      // should be needed because prompting
      // for required unencountered arguments should be automatic
      && prompts_shown)
   {  // must be after loading response file
      interactive_prompt();
      std::string command_line;
      if (compose_command_line(command_line))
      {  std::string continue_enter;
         std::clog
            << "These options can be specified on the command line as:"
            << std::endl << command_line << std::endl
            << std::endl
            << "Press [Enter] to continue" << std::endl;
         std::getline(std::cin,continue_enter);
      }
   }
   if (!quiet
      && ((encountered_count == 0)
          //NYI may want to virtual function to query derived classes if responses required
         )
      )                                       //200811
   {  std::clog
      << "Write command line and prompted (and loaded response file) options to a response file?" << std::endl;
      if (verbosity & VERBOSE_FLAG_EXPLAINATION)
         std::clog
         << "This will allow restarting the program with the same set of options." << std::endl;
      std::clog << "Enter response file name:" << std::endl;
      std::string response_filename;
      std::getline(std::cin,response_filename);
      /* conceptual:
      if file_system_engine.exists(response_filename);
      if (!yes)
         std::clog << "Replace the response file that already exists (y/n)?"
         replace = get y/n yes/no
      if (replace)
      else response_filename.clear()
      */
      if (response_filename.length())
      {  std::string response_content;
         compose_command_line(response_content);
         std::ofstream response_file(response_filename.c_str());
         response_file << response_content << std::endl;

         // NYI Don't forget unary options
      }
   }

   /*NYI I need better way to handle the different progress methods
   provide_progress_display_global().parameter.set_indicator(progress_raw);        //170519
   */

   resolve_directory_entry_names(start_directory);                               //171107
   languages.update();                                                           //191101
   if (languages.is_empty())                                                     //180515
   {
      // https://msdn.microsoft.com/en-us/library/windows/desktop/dd318139(v=vs.85).aspx
      // https://docs.microsoft.com/en-us/previous-versions/windows/desktop/apps/jj244362(v=vs.105)
      // append the system language of the OS
      //181130 #ifdef _Windows
      #ifndef __GNUC__
      // sorry for the lapse into Windows API uglyness
      ULONG numLanguages = 0;
      DWORD cchLanguagesBuffer = 0;
      BOOL hr = GetUserPreferredUILanguages
         (MUI_LANGUAGE_NAME, &numLanguages, NULL, &cchLanguagesBuffer);
      if (hr)
      {  WCHAR* pwszLanguagesBuffer = new WCHAR[cchLanguagesBuffer];
         hr = GetUserPreferredUILanguages
            (MUI_LANGUAGE_NAME,&numLanguages,pwszLanguagesBuffer,&cchLanguagesBuffer);
         if (hr)
         {  char lang[20]; lang[0] = 0; nat32 lang_i = 0;
            for (nat16 i = 0; i < cchLanguagesBuffer; i++)
            {  wchar_t curr_char = pwszLanguagesBuffer[i];
               switch (curr_char)
               {  case L'-':
                  {  lang[lang_i] = 0;
                     languages.append(new CORN::Item_string(lang));
                  } break;
                  case L',':
                  {  lang_i = 0;
                     lang[0] = 0;
                  } break;
                  default :
                  {
                     lang[lang_i++] = (char)curr_char;
                  }  break;
               } // switch
            } // for i
         } // if hr
         delete pwszLanguagesBuffer;
      } // if hr
      #endif
      #ifdef _Linux
      // NYI
      #endif
   }
   /*200807 now using list
   if (discovery_source) ended &= discovery_source->get_end();                   //200801
   if (discovery_target) ended &= discovery_target->get_end();                   //200801
   */
   FOR_EACH_IN(path_discovery,/*OS::FS::*/Path_discovery,path_discoveries,discovery_iter) //200807
      path_discovery->get_end();                                                 //200807
   FOR_EACH_END(discovery_iter)                                                  //200807
   return ended;                                                                 //180515
}
//_get_end__________________________________________________________2016-08-25_/
bool Arguments::write_help()                                               const
{  std::string author_mailto_URI;
   if (get_author_mailto_URI(author_mailto_URI))
      std::clog
      << "author: " << author_mailto_URI                          << std::endl;
   std::string manual_URI;
   if (manual_URI.length())
      std::clog
      << "manual: " << manual_URI                                 << std::endl;
   std::clog
      << "general options:"                                       << std::endl
      << "-v or --verbose      enables verbose output"            << std::endl
      << "-q or --quite        disables prompts"                  << std::endl
      << "--trial or --dry-run display what would be done without actually alterning file system or files" << std::endl
      << "-h or --help         displays brief help"               << std::endl
      << "--response={filename} load arguments from response file"<< std::endl
      << "--responses find accessible response files and loads them"<< std::endl;
   std::clog
      << "variable assigments:"                                   << std::endl;
   FOR_EACH_IN(section,VV_Section,sections,each_section)
   {  std::clog << "[" << section->get_label() << "]" << std::endl;
      FOR_EACH_IN(entry,VV_base_entry,section->entries,each_entry)
      {  std::string value;
         entry->append_to_string(value);
         entry->write_key(std::clog);                                            //180820
         std::clog
            << "=" << value // at this point this should be the default value
            << " {"
            << data_type_label_table[entry->get_type()]
            << "}"
            << std::endl;
      } FOR_EACH_END(each_entry)
   } FOR_EACH_END(each_section)
   return true;
}
//_write_help_______________________________________________________2016-08-25_/
bool Arguments::write_wizard(std::ostream &wizard_stream)            stream_IO_
{  FOR_EACH_IN(section,VV_Section,sections,each_section)
   {  if (section->is_key_cstr("arguments"))                                     //180820
      {  // Normal/generic arguments are stored in the "argument" section
         FOR_EACH_IN(entry,VV_base_entry,section->entries,each_entry)
         {  entry->write_wizard(wizard_stream);

            /*NYI
               if is_required(entry key)
            wizard_stream << "required: true" << std::endl;
            */

         } FOR_EACH_END(each_entry)
      }
      else
      {
         // Currently VV wizard can't handle data record [section] arguments
      }
   } FOR_EACH_END(each_section)
   return true;
}
//_write_wizard_____________________________________________________2018-05-15_/
void Arguments::display_interactive_instructions()
{
   std::clog
   << "Expected arguments where not given on the command line or response file." << std::endl
   << "Interactive mode will prompt for required arguments." <<std::endl
   << "The command line option: --quite or -q will disabled prompts" << std::endl << std::endl
   << "A prompt is displayed for each option assignment with type and default value." << std::endl
   << "Press [Enter] to accept default value or enter a new value." << std::endl
   << "Enter [,] to accept the current entry even if it is flagged as invalid." << std::endl
   << "Enter [.] to stop prompts (the default values will be retained)";
   if (sections.count() > 1)
   {
      std::clog << " for the current section." << std::endl
      << "enter [:] to stop remaining prompts." << std::endl;
   }
   std::clog
   << std::endl
   << "Dates are entered with format YYYYDDD (year and day of year)" << std::endl
   << "Filenames may be fully qualified or omit the path in the working directory." << std::endl << std::endl;
}
//_display_interactive_instructions_________________________________2016-10-16_/
bool Arguments::interactive_prompt()                               modification_
{  if (quiet) return false;
   // Currently this only prompts for Data record assignments
   // but should provide a list of option
   bool interactive_instructions_displayed = false;
   FOR_EACH_IN(section,VV_Section,sections,each_section)
   {  bool skip_section = false;
      std::clog << "section: " << section->get_label() << std::endl;
      /* Conceptual
      if ((section->encountered) and any entries not encountered
      {
         std::clog << "Some of the parameter related to this section have "
         "already been encountered (on the command line or from a file)"

         prompt to display current values
         if (display_current_values)
            section->write(clog);


         prompt the user to skip this section

         skip_section = xxxx
      }
      */
      if (!skip_section)
      FOR_EACH_IN(entry,VV_base_entry,section->entries,each_entry)
      {  if (!entry->was_encountered())
         {  if (!interactive_instructions_displayed)
            {  display_interactive_instructions();
               interactive_instructions_displayed = true;
            }
            prompts_shown = true;
            if (entry->annotation1.length())
               std::clog << entry->annotation1 << std::endl;
            if (entry->annotation2.length())
               std::clog << entry->annotation2 << std::endl;
            std::string value;
            nat8 date_type = entry->get_type();
            if (date_type < DT_COUNT)
               std::clog << "data type:" << data_type_label_table[date_type] << std::endl;
            else
            { }
            // Should also display units
            bool validated = true;
            bool accepted = true;
            do
            {  entry->append_to_string(value);
               entry->write_key(std::clog); //810820
               std::clog
               //180820 << entry->get_key()
               << "=" << value << std::endl;
               std::getline(std::cin,value);
               CORN::strip_string(value,CORN::Both);
               if (value == ".") {each_entry->end(); break; }
               if (value == ":") {each_entry->end(); each_section->end(); break;}
               if (value == ",") { accepted = true; }
               if (value.length())
                  entry->set_str(value.c_str());
               else
               {  entry->set_encountered(true);
                  std::clog << "\r" << value << std::endl; // echo the current/default value
               }
               if (date_type == VV_DEN)
               {  // check file/directory existance
                  entry->append_to_string(value);
                  CORN::OS::Directory_entry_name_concrete value_DEN(value);
                  validated = /*CORN::OS::*/file_system_engine.exists(value_DEN);
               }
               /*
               conceptual entries may have validation
               valudation status and is not valid continue prompting
               {
                  show validation status
                  show curent value

               }
            */
            } while (!validated && !accepted);
         }
      } FOR_EACH_END(each_entry)
   } FOR_EACH_END(each_section)
   return true;
}
//_interactive_prompt_______________________________________________2016-10-14_/
nat16 Arguments::compose_command_line(std::string &command)
{  nat16 count = 0;
   FOR_EACH_IN(section,VV_Section,sections,each_section)
   {  if (sections.count() > 1)
      {  // We only need section tags if there are multiple sections
         command += "[" + section->get_label() + "]" + " ";
      }
      FOR_EACH_IN(entry,VV_base_entry,section->entries,each_entry)
      {  std::string value;
         entry->append_to_string(value);
         entry->append_variable(command);                                        //181115
         command += "=" + value + " ";
         count++;
      } FOR_EACH_END(each_entry)
   } FOR_EACH_END(each_section)
   return count;
}
//_compose_command_line_____________________________________________2016-10-14_/
bool Arguments::require(const std::string &argument_key_list)
{  required_arguments.set_unique_keys(true);
   CORN::Text_list newly_required(argument_key_list.c_str(),' ');
   return required_arguments.transfer_all_from(newly_required) > 0;
}
//_require__________________________________________________________2018-05-15_/
/*200808 obsolete replaced with Path_discovery
Item *Arguments::discover_one
(const Seclusion                 &seclusion
,Directory_entry_type         entry_type
,CORN::OS::File_system::Recursion recursion)                       renunciation_ //191103
{  Item *the_one = 0;
   CORN::Unidirectional_list matching_args;
   nat32 matches = discover_many
      (matching_args
      ,seclusion
      ,entry_type
      ,recursion
      //abandoned ,1
      );
   if (matches > 1)
   {  // Multiple files were found in the path.
      // List them and prompt user which one
         int32 selection = 0;
         do
         {  int f = 0;
            if (CORN::global_arguments->verbose_flag || !CORN::global_arguments->quiet)                                               //170223
            {  std::clog << "Multiple potential items found" << std::endl;
               FOR_EACH_IN(matching_file, CORN::OS::File_name, matching_args, each_matching)
               {  f++;
                  std::clog  << (int) f << '\t' << matching_file->c_str() << std::endl;
               } FOR_EACH_END(each_matching)
            }
            if (CORN::global_arguments->quiet)                                   //170223
               // select the last listed (closest) table file.
               selection = matches;                                  //170223
            else
            {  std::cout << "Enter item number (enter 0 if none of these)" << std::endl;
               std::cin >> selection;
            }
            if (selection > 0)
            {  nat32 selection_index = selection - 1;
               the_one =
                  (matching_args.get_at(selection_index));
               matching_args.detach(the_one);
            }
         } while (selection > (int) matches);

      // for the remaining items
      FOR_EACH_IN(arg,Item,matching_args,arg_iter)
      {
         CORN::OS::Directory_entry_name *as_DEN =
            dynamic_cast<CORN::OS::Directory_entry_name *>(arg);
         if (as_DEN) delete as_DEN; // The DEN would have been instanciated by discover_many
         else unrecognized.take(arg); // but unselected item back on the list
      } FOR_EACH_END(arg_iter)
   } else
   {  the_one = matching_args.pop_first();
      if (CORN::global_arguments->is_verbose(2))
      {
         if (the_one)
            std::clog
            << "info: one item found " << std::endl;
         else
            std::clog
            << "info: no item found" << std::endl;
      }
   }
   return the_one;
}
//_discover_one_____________________________________________________2019-11-03_/
nat32 Arguments::discover_many
(CORN::Container                 &list
,const Seclusion                 &seclusion
,OS::Directory_entry_type         entry_type
,CORN::OS::File_system::Recursion recursion)                       renunciation_ //191103
{  bool matches = false;
   FOR_EACH_IN(arg,Item_string,unrecognized,iter_arg)
   {
      matches = seclusion.is_included_string(*arg);
      if (matches)
      {
         if(entry_type == OS::unknown_entry)
         {  list.take(arg);
            iter_arg->detach_current(); // because moved to list
         } else
         {  CORN::OS::Directory_entry_name *as_DEN
               = dynamic_cast<CORN::OS::Directory_entry_name *>(arg);
            if (!as_DEN && (entry_type != OS::unknown_entry))
            {
               as_DEN = new CORN::OS::Directory_entry_name_concrete(*arg);
            }
            if (as_DEN)
            {
               if (as_DEN->is_qualified())
               {
                  if (CORN::OS::file_system_engine.exists(*as_DEN))
                  {
                     list.take(as_DEN);
                     iter_arg->delete_current(); // because listed as filename
                     matches = true;
                  }
                  else
                     matches = false;
               } // else not a qualified name
            }
            if (!matches) // not recognized qualified name
            {  // At this point arg would be unqualified and expected filename
               std::wstring argW;
               CORN::string_to_wstring(*arg,argW);
               CORN::OS::Directory_entry_name *found_DEN
               = CORN::OS::file_system_engine.find_DEN
                  (argW,entry_type,start_directory,recursion);
               if (is_verbose(2))
                  std::clog << "info: searching path directories:";
               if (found_DEN)
               {
                  list.take(found_DEN);
                  iter_arg->delete_current(); // because listed as qualified filename
                  matches = true;
               } else
                  if (is_verbose(2))
                  std::clog << "not " << std::endl;
               std::clog << "found" << std::endl;
            }
         }
      } // else do nothing, not an arg we are looking for
   } FOR_EACH_END(iter_arg)
   if (!matches && entry_type && (recursion != CORN::OS::File_system::none_recursion_exclusive)) // no found any arguments matching
   {  // search the context
      if (is_verbose(2))
      {
         std::clog << "info: searching context:"
            << start_directory.c_str() << std::endl;
         CORN::Seclusion &writable_seclusion = const_cast<CORN::Seclusion &>(seclusion);
         writable_seclusion.write(std::clog);
      }
      CORN::Container *list_filenames = (entry_type == CORN::OS::file_entry)      ? &list : 0;
      CORN::Container *list_dirnames  = (entry_type == CORN::OS::directory_entry) ? &list : 0;

//CORN::Inclusion tif_pattern("*.tif",true);

      nat32 found = OS::file_system_engine.list_DENs
         (start_directory,list_filenames,list_dirnames
         //,tif_pattern
         , seclusion
         ,recursion);
   }
   return list.count();
}
//_discover_many____________________________________________________2019-11-03_/
*/

/**  conceptual

   Arguments currently has a simple set of prompts but it would be nice
   to have a more complete editors like this.

   This could be a general data record editor
   that could be expanded for options/arguments/flags


      For each data entry item in the options record that
      has not yet been set either on the command line or
      response file(s) the user will be prompted for data item value.
      If selected_options cstring arrays are provided then
      only those items which match the selected options will be prompted
      (all others will retain the default value).
      The prompt displays the caption, units the hint, and an indicator if
      documentation/help text/URL is available.
      The user can either enter the data value, or press enter to accept the default value.
      The following control keys presses have special functions
      [Esc] to cancel (and return false)
         The caller would then likely exit the program.
      [Backspace] to display previous prompt (only one previous)
      [Tab] or [Space] or [Return] to accept default value and advance to next prompt
      [Home] to prompt all options
         (even if the had been entered on the command line or response file(s)).
      [End] or [Close] discontinue prompts and retain the remaining options with default values.

      [F1] (See help)
      [Ctrl C]  Copies the current item's (default) value to the clipboard.
      [Ctrl V]  Enters the current item values from the clipboard.

      // The following special keys may be avaiable on some keyboard
      [Help] (See help)
      [Open] prompts for a (resonse) file to load additional options.
      [New]  Reset/clears all values (including command line and response file
         and previously entered values) to defaults and starts prompting all options again.


      Help Displays the help context page document (If any) for the current parameter item.
**/

//_conceptual_______________________________________________________2016-10-16_/
Arguments *global_arguments = 0;
}//_namespace_CORN_____________________________________________________________/

