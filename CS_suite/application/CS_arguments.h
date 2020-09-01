#ifndef CS_argumentsH
#define CS_argumentsH
#include "corn/OS/directory_entry_name_concrete.h"
#include "corn/OS/file_system_logical.h"
#include "corn/data_source/vv_datasrc.h"
#include "CS_suite/simulation/CS_identification.h"
#include "corn/application/arguments.h"
namespace CS
{
//______________________________________________________________________________
class Arguments               // Probably should rename to Arguments_scenario
: public extends_ CORN::Arguments
{
 protected:
   modifiable_ CS::Identification   &ID;
      // ref to Simulation_control::ID
 protected: // Canonical directories and filenames

   /*200813
   // These probably should be generalized as parameter_directory_name and parameter_file_name
   provided_ CORN::OS::Directory_name *scenario_directory_name;
   provided_ CORN::OS::File_name      *scenario_file_name_deprecated;
   */
   CORN::OS::FS::Path_discovery_struct *scenario_filename_discovery_raw; //owned //200813
   CORN::OS::FS::Path_discovery_cowl   *scenario_filename_discovery; //submitted //200813
      // With the new scenario composition, there can be may scenario filename.
 public:
   bool find_and_reset_missing_parameter_file_names;                             //170629
 public:
   nat16 desired_tally_statistics;                                               //170318
 #if ((CS_VERSION>=4) && (CS_VERSION<6))
 public:
   std::wstring scenario_extension;                                              //200525
   CORN::OS::FS::Path_discovery_cowl *V4_format_filename_discovery;              //200813_200525
   //200813 CORN::OS::File_name *V4_format_filename;                                      //200525
 #endif
 public:
   Arguments
      (modifiable_ CS::Identification &ID_
      ,const wchar_t *scenario_extension_ = 0);                                  //200817_200525
   virtual ~Arguments();
 public:
   /*200813 obsolete, now use Path_discovery
   virtual const CORN::OS::Directory_name &provide_scenario_directory_name()     provision_;
   virtual const CORN::OS::File_name      &provide_scenario_file_name()          provision_;
   */
   /*200813 obsolete not used anymore ?
   virtual bool invalidate_scenario_directory()                   modification_; //160114
   */
   /*200813 obsolete unused
   bool is_scenario_file_named_or_found()                          affirmation_;
   */
   const CS::Identification &ref_ID()                                     const; //151028
/*200525
 private:
   inline virtual const char    *get_scenario_filename_extension_cstr()   const { return 0; }
   inline virtual const wchar_t *get_scenario_filename_extension_wstr()   const { return 0; }
*/
 protected:
   //NYN virtual bool get_start();                                                     //200813
   virtual bool get_end();                                                       //191104
};
//_Arguments________________________________________________________2015-04-15_/
}//_namespace_CS_______________________________________________________________/
#endif

