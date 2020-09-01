#include "CS_suite/application/CS_arguments.h"
#include "common/simulation/scenario.h"
#include "corn/data_source/vv_file.h"
#include "corn/OS/file_system_engine.h"
#include "corn/math/statistical/statistics_types.h"
#include "corn/seclusion.h"
#include "corn/OS/FS/recursion.h" //LML 200819
#include <string.h>
using namespace CORN;
using namespace CORN::OS;
using namespace CORN::OS::FS;
namespace CS
{
#if ((CS_VERSION>=4) && (CS_VERSION<6))
Path_discovery_struct FMT_discovery_raw
= { file_bit,{(ascendent|breadth_first),{to_extreme,from_CWD}},REQUIRE_SPECIFYING_EXISTING_OPTIONAL,"*.fmt"};
#endif


//______________________________________________________________________________
Arguments::Arguments
(modifiable_ CS::Identification &ID_
,const wchar_t                  *scenario_extension_)                            //200525
: CORN::Arguments                                                             () //160826
, ID                                                                       (ID_) //160222
, scenario_filename_discovery_raw                                            (0) //200813
, scenario_filename_discovery                                                (0) //200813
/*200813
, scenario_directory_name                                                    (0)

, scenario_filename_discovery
, scenario_file_name_deprecated                                              (0)
*/
, find_and_reset_missing_parameter_file_names                            (false) //170629
, desired_tally_statistics                 (STAT_BASIC_ELEMENTS_INIT_FINAL_bits) //170413
 #if ((CS_VERSION>=4) && (CS_VERSION<6))
, scenario_extension                                       (scenario_extension_) //200525
, V4_format_filename_discovery                                               (0) //200813
//200813 , V4_format_filename                                                         (0) //200525
 #endif
{
   //160222 moved for (int a = 0; a < 1000; a++) arg_recognized[a] = 0;
   // By default the CWD is assumed to be the scenario directory
   // If any directory name is specified on the command line
   // that will be assumed to be the scenario directory
     // When no command line arguments then use CWD as default scenario directory

   scenario_filename_discovery_raw = new Path_discovery_struct;                  //200813
   scenario_filename_discovery_raw->DE_type_mask         = file_bit;
      // In the new system this will be file or directory
   scenario_filename_discovery_raw->recursion.direction  = ascendent;            //200813
   scenario_filename_discovery_raw->recursion.depth.max  = to_extreme;           //200813
   scenario_filename_discovery_raw->recursion.depth.min  = from_CWD;             //200813
   scenario_filename_discovery_raw->require.specifying   = 0xFFFFFFFF;           //200813
   scenario_filename_discovery_raw->require.existing     = 1;                    //200813
   scenario_filename_discovery_raw->include_pattern      = 0;                    //200813
      // Warning need to check this, I dont think the pattern needs wild card

   scenario_filename_discovery = new                                             //200813
      CORN::OS::FS::Path_discovery_cowl                                          //200813
         (*scenario_filename_discovery_raw,"scenario");                          //200813
   scenario_filename_discovery->seclusion.include_pattern(scenario_extension_);   //200813
      // Warning need to check this: I dont think the pattern needs wild card
      // otherwise need to compose scenario filename pattern.
   path_discoveries.append(scenario_filename_discovery);       //200813
   #if ((CS_VERSION>=4) && (CS_VERSION<6))
   V4_format_filename_discovery = new                                            //200815
      CORN::OS::FS::Path_discovery_cowl(FMT_discovery_raw,"format");             //200815
   path_discoveries.append(V4_format_filename_discovery);      //200813
   #endif
   quiet = true;                                                                 //191104
}
//_Arguments:constructor____________________________________________2015-04-15_/
bool Arguments::get_end()
{  bool ended = CORN::Arguments::get_end();
   // Specifying the scenario directory name on the command line is deprecated
   // it should be simply the current working directory

   #if ((CS_VERSION>=4) && (CS_VERSION<6))
   File_name *V4_format_filename                                                 //200813
      = V4_format_filename_discovery->provide_primary();                         //200813
   /*200813
   V4_format_filename
      = dynamic_cast<CORN::OS::Directory_entry_name *>
      (discover_one
       (CORN::Inclusion("*.FMT",true)
       ,CORN::OS::file_entry
       ,CORN::OS::File_system::superdirectory_recursion_inclusive));
   */
   #endif

   /*200813 obsolete
   CORN::OS::Directory_name_concrete *arg_scenario_directory_name_optional =
      dynamic_cast<CORN::OS::Directory_name_concrete *>(discover_one
         (CORN::Inclusion("*",true)
         ,CORN::OS::directory_entry));
   if (arg_scenario_directory_name_optional)
   {  delete scenario_directory_name; // overriding default (CWD)
      scenario_directory_name = arg_scenario_directory_name_optional;
   }
   // Specifying the scenario directory name on the command line is deprecated
   // it should be simply the current working directory
   // because now there may be multiple the scenario file name discovered
   */

   /*200813 now using path Discovery and there may be multiple
     scenario filenames in composition
   std::wstring scenario_pattern (L"*.");
   scenario_pattern.append(scenario_extension);                                  //200525
      //200525 (get_scenario_filename_extension_wstr());
   CORN::OS::File_name *arg_scenario_filename_optional =
      dynamic_cast<CORN::OS::Directory_name *>(discover_one
         (CORN::Inclusion("*",true)
         ,CORN::OS::file_entry));
   if (arg_scenario_filename_optional)
   {
      scenario_file_name_deprecated = arg_scenario_filename_optional;
      delete scenario_directory_name;
      scenario_directory_name = new CORN::OS::Directory_name_concrete
               (scenario_file_name_deprecated->get_path_directory_name()->c_str()
               ,CORN::OS::directory_entry);
   }
   */
   return ended;
}
//_get_end__________________________________________________________2019-11-04_/
Arguments::~Arguments()
{
   delete scenario_filename_discovery_raw;                                       //200813
/*200813 now using Path_discovery
   delete scenario_directory_name;        //not needed scenario_directory_name = 0;
   delete scenario_file_name_deprecated;  //not needed scenario_file_name_deprecated = 0;
*/
}
//_Arguments:destructor_____________________________________________2015-04-15_/
/*200813 obsolete now using Path_discovery
const CORN::OS::Directory_name &Arguments::provide_scenario_directory_name() provision_
{  if (!scenario_directory_name)
   {  if (scenario_file_name_deprecated)
      {  const CORN::OS::Directory_name *scenario_file_path = scenario_file_name_deprecated->get_path_directory_name();
         if (scenario_file_path) // The scenario file is qualified
            scenario_directory_name = new CORN::OS::Directory_name_concrete(*scenario_file_path,CORN::OS::directory_entry); //161028
      } else
      // provide_scenario_file_name may identify the scenario directory if given the scenario filename
      if (!scenario_directory_name)
         // we still dont have the scenario directory name assume CWD
         scenario_directory_name = new CORN::OS::Directory_name_concrete(); //161028
   }
   return *scenario_directory_name;
}
//_provide_scenario_directory_name__________________________________2015-04-15_/
const CORN::OS::File_name &Arguments::provide_scenario_file_name()     provision_
{  if (!scenario_file_name_deprecated)
   { // scenario file was not specified on the command line search update the directory tree
      scenario_file_name_deprecated = dynamic_cast<CORN::OS::File_name_concrete*>
         (CORN::OS::file_system_engine.find_qualified_name_in_path
         (scenario_extension.c_str()                                             //20025
         //200525 get_scenario_filename_extension_wstr()
         ,provide_scenario_directory_name()));
   // Now we search for the scenario file in the scenario directory
   // or any parent directory (this is used for GIS and/or
   // other multiple runs where the template scenarios may be in a parent
   // directory. 140210
      if (!scenario_file_name_deprecated)                                        //160120
         scenario_file_name_deprecated = new CORN::OS::File_name_concrete
            (provide_scenario_directory_name()
            //200525 ,get_scenario_filename_extension_wstr()
            ,scenario_extension);                                                //200525
   }
   return *scenario_file_name_deprecated;
}
//_provide_scenario_file_name_______________________________________2015-04-15_/
*/
/*200813 obsolete: not used anymore
bool Arguments::invalidate_scenario_directory()                    modification_
{
   delete scenario_directory_name;        scenario_directory_name = 0;
   delete scenario_file_name_deprecated;  scenario_file_name_deprecated = 0;
   return true;
}
//_invalidate_scenario_directory____________________________________2016-01-14_/
*/
/*200813 obsolete/unused
bool Arguments::is_scenario_file_named_or_found()                   affirmation_
{  provide_scenario_file_name();
   return CORN::OS::file_system_engine.exists(*scenario_file_name_deprecated);
}
//_is_scenario_file_named_or_found__________________________________2015-04-15_/
*/
const CS::Identification &Arguments::ref_ID()                              const
{  return ID;                                                                    //160222
}
//_ref_ID___________________________________________________________2015-10-28_/
}//_namespace_CS_______________________________________________________________/

