
// for  CROPSYST_PROJECT_EXT
#include "cs_scenario_directory.h"

#  include "CS_suite/file_system/CS_database_directory.h"
using namespace CS;

#include "cs_filenames.h"
#ifdef __unix
#  include <fstream>
#endif
#include <string.h>
#include <stdlib.h>
namespace CropSyst                                                               //120112
{
//______________________________________________________________________________
void Scenario_directory::reset()                                      provision_
{  
/*200111  nolonger have Database under scenarios
	delete database_directory; database_directory = 0;
*/	
}
//_reset____________________________________________________________2012-02-07_/
/*200111  nolonger have Database under scenarios
Database_directory_logical &Scenario_directory::Database(bool create_now_if_needed) provision_
{  if (!database_directory)
      database_directory = new CS::Database_directory_logical(*this,create_now_if_needed,L"Database");
   return *database_directory;
}
//_Database_________________________________________________________2012-02-05_/
*/
#define PROVIDE_model_filename_OPTIONAL(provide_method,structure,varible,filename,extension) \
CORN::OS::File_name *Scenario_directory                                          \
::provide_method()                                                   provision_  \
{  if (!optional_resolved_filenames.structure.varible)                           \
   {  optional_resolved_filenames.structure.varible                              \
         = new CORN::OS::File_name_concrete(*this,filename,extension);           \
      if (!CORN::OS::file_system_engine.exists                                   \
          (*optional_resolved_filenames.structure.varible))                      \
      {  delete optional_resolved_filenames.structure.varible;                   \
         optional_resolved_filenames.structure.varible = 0;                      \
   }  }                                                                          \
   return optional_resolved_filenames.structure.varible;                         \
}
//_PROVIDE_model_filename_OPTIONAL__________________________________2015-04-16_/
#define PROVIDE_model_filename(provide_method,structure,varible,filename,extension) \
CORN::OS::File_name &Scenario_directory                                          \
::provide_method()                                                   provision_  \
{  if (!optional_resolved_filenames.structure.varible)                           \
   {  optional_resolved_filenames.structure.varible                              \
         = new CORN::OS::File_name_concrete(*this,filename,extension);           \
   }                                                                             \
   return *optional_resolved_filenames.structure.varible;                        \
}
//_PROVIDE_model_filename___________________________________________2015-04-16_/

#if (CROPSYST_VERSION >= 5)
PROVIDE_model_filename_OPTIONAL(provide_CPF_water_irrigation_management_filename_optional ,CPF, water_irrigation_management,"irrigation"     ,"CropSyst_management")
#else
PROVIDE_model_filename_OPTIONAL(provide_CPF_water_irrigation_management_filename_optional ,CPF, water_irrigation_management,"irrigation"     ,"mgt")
#endif
PROVIDE_model_filename         (provide_organic_matter_equilibrated_filename              ,organic_matter, equilibirated   ,"equilibirated"  ,"organic_matter")
//______________________________________________________________________________
Scenario_directory::Output_directory::Output_directory
(const CORN::OS::File_system::Directory &_parent_directory
,const std::wstring &_name)
: CORN::OS::File_system_logical::Directory(_parent_directory,_name,false)
,daily_UED_filename     (0)
,season_UED_filename    (0)
,annual_UED_filename    (0)
,harvest_UED_filename   (0)
,totals_txt_filename    (0)
,harvest_htm_filename   (0)
,schedule_txt_filename  (0)
#if (CROPSYST_VERSION == 4)
,daily_xls_filename        (0)
,annual_xls_filename       (0)
,season_xls_filename       (0)
,organic_matter_percents_dat_filename     (0)
,daily_N_xls_filename      (0)
,annual_N_xls_filename     (0)
,season_N_xls_filename     (0)
,C_balance_dat_filename    (0)
,detail_txt_filename       (0)
,SCS_runoff_dat_filename   (0)
#endif
,crop_N_fate_txt_filename  (0)
,Soil_dirname              (0)
{}
//_Output_directory:constructor_____________________________________2012-02-04_/
Scenario_directory::Output_directory::~Output_directory()
{  reset(); }
//_Output_directory:destructor______________________________________2012-02-04_/
void Scenario_directory::Output_directory::reset()                    provision_
{  delete daily_UED_filename;          daily_UED_filename   = 0;
   delete season_UED_filename;         season_UED_filename  = 0;
   delete annual_UED_filename;         annual_UED_filename  = 0;
   delete harvest_UED_filename;        harvest_UED_filename = 0;
   delete totals_txt_filename;         totals_txt_filename  = 0;
   delete harvest_htm_filename;        harvest_htm_filename = 0;
   delete schedule_txt_filename;       schedule_txt_filename= 0;
   CORN::OS::File_system_logical::Directory::reset();
}
//_reset____________________________________________________________2012-02-04_/
Scenario_directory::Scenario_directory
(const CORN::OS::Directory_name &scenario_dir_name_ref
,bool create_now_if_needed)                                                      //130126
:CORN::OS_FS::Parameters_directory                                               //200817
   (scenario_dir_name_ref
   ,CORN::OS::directory_entry
   ,create_now_if_needed)
//200817 :Common_scenario_directory(scenario_dir_name,create_now_if_needed)               //130126
/*200111
No longer having Database directory under scenario
 protected:
,database_directory(0)
*/
,output_dir(0)
{  optional_resolved_filenames.CPF.water_irrigation_management    = 0;           //150422
   optional_resolved_filenames.organic_matter.initial             = 0;           //150429
   optional_resolved_filenames.organic_matter.equilibirated       = 0;           //150429
}
//_Scenario_directory:constructor___________________________________2012-02-04_/
/*

As of CS6 output directories the extension 'output'
as a monad this will disable output directories as consituents.

The name is latest.output previous version would be (re)named
to have the creation DateTime for backup.
*/

const Scenario_directory::Output_directory &Scenario_directory::CS_output()  provision_ //130120
{  if (!output_dir)
      output_dir = new Scenario_directory::Output_directory
         #if (CS_VERSION < 6)
         (*this,L"Output");
         #else
         (*this,L"latest.output");
         #endif
   return *output_dir;
}
//_CS_output________________________________________________________2012-03-09_/
const Scenario_directory::Output_directory &Scenario_directory::Output()  provision_ //130120
{  if (!output_dir)
      output_dir = new Scenario_directory::Output_directory
         #if (CS_VERSION < 6)
         (*this,L"Output");
         #else
         (*this,L"latest.output");
         #endif
   return *output_dir;
}
//_Output___________________________________________________________2012-02-02_/
const CORN::OS::File_name &Scenario_directory::Output_directory::season_UED() provision_
{  if (!season_UED_filename)
      season_UED_filename = new CORN::OS::File_name_concrete(*this,"season.UED");
   return *season_UED_filename;
}
//_season_UED__________________________________________________________________/
provide_file_name_implementation(Scenario_directory::Output_directory,daily_UED     ,daily_UED_filename     ,"daily.UED")
//provide_file_name_implementation(Scenario_directory::Output_directory,season_UED,season_UED_filename,"season.UED")
provide_file_name_implementation(Scenario_directory::Output_directory,annual_UED    ,annual_UED_filename    ,"annual.UED")
provide_file_name_implementation(Scenario_directory::Output_directory,harvest_UED   ,harvest_UED_filename   ,"harvest.UED")
provide_file_name_implementation(Scenario_directory::Output_directory,totals_txt    ,totals_txt_filename    ,"totals.txt") //121121
provide_file_name_implementation(Scenario_directory::Output_directory,harvest_htm   ,harvest_htm_filename   ,"harvest.htm") //121121
provide_file_name_implementation(Scenario_directory::Output_directory,schedule_txt  ,schedule_txt_filename  ,"schedule.txt") //130808
#if (CROPSYST_VERSION == 4)
// In V5 we do away with xls output
provide_file_name_implementation(Scenario_directory::Output_directory,daily_xls                    ,daily_xls_filename     ,"daily.xls") //130827
provide_file_name_implementation(Scenario_directory::Output_directory,annual_xls                   ,annual_xls_filename    ,"annual.xls") //130827
provide_file_name_implementation(Scenario_directory::Output_directory,season_xls                   ,season_xls_filename    ,"season.xls") //130827
provide_file_name_implementation(Scenario_directory::Output_directory,organic_matter_percents_dat  ,organic_matter_percents_dat_filename,"organic_matter_percents.dat") //130827
provide_file_name_implementation(Scenario_directory::Output_directory,daily_N_xls                  ,daily_N_xls_filename   ,"daily_N.xls") //130827
provide_file_name_implementation(Scenario_directory::Output_directory,annual_N_xls                 ,annual_N_xls_filename  ,"annual_N.xls") //130827
provide_file_name_implementation(Scenario_directory::Output_directory,season_N_xls                 ,season_N_xls_filename  ,"season_N.xls") //130827
provide_file_name_implementation(Scenario_directory::Output_directory,C_balance_dat                ,C_balance_dat_filename ,"C_balance.dat") //130827
provide_file_name_implementation(Scenario_directory::Output_directory,detail_txt                   ,detail_txt_filename    ,"detail.txt") //130827
provide_file_name_implementation(Scenario_directory::Output_directory,SCS_runoff_dat               ,SCS_runoff_dat_filename,"SCS_runoff.dat")//131216
#endif
provide_file_name_implementation     (Scenario_directory::Output_directory,crop_N_fate_txt         ,crop_N_fate_txt_filename  ,"crop_N_fate_txt.dat") //130827
provide_directory_name_implementation(Scenario_directory::Output_directory,Soil                    ,Soil_dirname              ,"Soil") //130829
provide_directory_name_implementation(Scenario_directory    ,GIS                     ,GIS_dirname            ,"GIS") //140214

//_output_filenames_________________________________________________2012-02-04_/
Scenario_directory::~Scenario_directory()                                        //120112
{
/*200111
No longer having Database directory under scenario
 protected:
   if (database_directory) delete database_directory;
*/
   if (output_dir)         delete output_dir;                                    //120309
}
//_destructor_______________________________________________________2009-05-18_/
CORN::OS::Directory_name *Scenario_directory::render_crop_production_function_adjustment_percent
(nat8 adjustment_percent)                                             rendition_
{
   char percent_of_optimal_cstr[256];
   CORN::nat8_to_cstr(adjustment_percent,percent_of_optimal_cstr,10);            //150428
   CORN::OS::Directory_name_concrete CPF_directory(*this,"CPF");
   CORN::OS::Directory_name_concrete CPF_water_directory(CPF_directory,"water");
   CORN::OS::Directory_name_concrete *CPF_adjusment = new CORN::OS::Directory_name_concrete
      (CPF_water_directory,percent_of_optimal_cstr);
   return CPF_adjusment;
}
//_render_crop_production_function_adjustment_percent_______________2015-04-17_/
CORN::OS::Directory_name *Scenario_directory::render_crop_production_function_adjustment_schedule
(const std::wstring &irrigation_adjustment_name)                        rendition_
// unqual no ext
{
//   wstring_to_ASCIIZ(*irrigation_adjustment_filename,percent_of_optimal_cstr); //was fixed_adj
   CORN::OS::Directory_name_concrete CPF_directory(*this,"CPF");
   CORN::OS::Directory_name_concrete CPF_water_directory(CPF_directory,"water");
   CORN::OS::Directory_name_concrete *CPF_adjusment = new CORN::OS::Directory_name_concrete
      (CPF_water_directory,irrigation_adjustment_name);
   return CPF_adjusment;
}
//_render_crop_production_function_adjustment_schedule______________2015-04-17_/
}//_namespace CropSyst_________________________________________________________/                                                         //120112

