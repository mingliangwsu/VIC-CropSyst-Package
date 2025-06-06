#include "CAFE_directory.h"
namespace CS
{
//______________________________________________________________________________
FARM_Directory::FARM_Directory(const wchar_t *dairy_project_name)
: CS::Application_directory(dairy_project_name)
, dairy_dir(0)
{}
//_FARM_Directory:constructor_______________________________________2014-11-10_/
FARM_Directory::FARM_Directory(const CS::Suite_Directory &suite_dir,const wchar_t * directory_name)
: CS::Application_directory(&suite_dir,directory_name)                           //141010
, dairy_dir(0)                                                                   //140111
{}
//_FARM_Directory:constructor_______________________________________2014-11-10_/
void FARM_Directory::reset()                                          provision_
{  delete dairy_dir;    dairy_dir    = 0;
}
//_reset____________________________________________________________2014-11-10_/
//==============================================================================
CAFE_Directory::CAFE_Directory()
: FARM_Directory(L"CAFE")                                                        //141110
, cropland_dir(0)
, cropland_energy_estimation_xls_file(0)
, cropland_energy_estimation_xlsm_file(0)
{}
//_CAFE_Directory:constructor_______________________________________2011-10-10_/
CAFE_Directory::CAFE_Directory(const CS::Suite_Directory &suite_dir,const wchar_t * directory_name)
: FARM_Directory(suite_dir,directory_name)
, cropland_dir(0)
, cropland_energy_estimation_xls_file(0)
, cropland_energy_estimation_xlsm_file(0)
{}
//_CAFE_Directory:constructor_______________________________________2014-01-11_/
FARM_Directory::Dairy_directory::Dairy_directory
(const FARM_Directory &path_name                                                 //141010
,const wchar_t *directory_name)
: CS::Application_directory(path_name,directory_name)                            //141010
, scenario_editor_exe(0)
, field_editor_exe(0)
, scenario_run_exe(0)
{}
//_Dairy_directory:constructor______________________________________2011-05-11_/
FARM_Directory::Dairy_directory::~Dairy_directory()
{ reset(); }
//_Dairy_directory:destructor_______________________________________2011-05-11_/
void CAFE_Directory::reset()                                          provision_
{  delete cropland_energy_estimation_xls_file; cropland_energy_estimation_xls_file =0;
   delete cropland_energy_estimation_xlsm_file;cropland_energy_estimation_xlsm_file=0;
   delete cropland_dir; cropland_dir = 0;
   FARM_Directory::reset();                                                      //141110
   //141110    delete dairy_dir;    dairy_dir    = 0;
}
//_2011-10-10___________________________________________________________________
void FARM_Directory::Dairy_directory::reset()                         provision_
{  CS::Application_directory::reset();
   if (scenario_editor_exe) delete scenario_editor_exe; scenario_editor_exe= 0;
   if (field_editor_exe)    delete field_editor_exe;    field_editor_exe   = 0;
}
//_reset____________________________________________________________2011-05-11_/
provide_directory_name_implementation(CAFE_Directory,cropland,cropland_dir,"cropland");
provide_subdir_file_name_implementation(CAFE_Directory,cropland_energy_estimation_xls  ,cropland_energy_estimation_xls_file,cropland(),"2011-09-12_cropland_energy_estimation_tool.xls");
provide_subdir_file_name_implementation(CAFE_Directory,cropland_energy_estimation_xlsm ,cropland_energy_estimation_xls_file,cropland(),"2011-09-12_cropland_energy_estimation_tool.xlsm");
provide_subdir_file_name_implementation(CAFE_Directory,cropland_energy_estimation_exe  ,cropland_energy_estimation_exe_file,cropland(),"cropland_energy_CropSyst_aggregator.exe");
//______________________________________________________________________________
const FARM_Directory::Dairy_directory &FARM_Directory::Dairy()         provision_//140111
{  if (!dairy_dir)
      dairy_dir = new FARM_Directory::Dairy_directory(*this,L"Dairy");
   return *dairy_dir;
}
//_Dairy_______________________________________________________________________/
const CORN::OS::File_name &FARM_Directory::Dairy_directory::field_editor_executable() provision_
{  if (!field_editor_exe)
   {  CORN::OS::Directory_name_concrete dairy_dir(this->w_str());
      field_editor_exe=new CORN::OS::File_name_concrete(dairy_dir,"field_editor.exe");
   }
   return *field_editor_exe;
}
//_field_editor_executable_____________________________________________________/
const CORN::OS::File_name &FARM_Directory::Dairy_directory::scenario_editor_executable() provision_
{  if (!scenario_editor_exe)
   {
      CORN::OS::Directory_name_concrete dairy_dir(this->w_str());
     scenario_editor_exe=new CORN::OS::File_name_concrete(dairy_dir,"farm_scenario_editor.exe");
   }
   return *scenario_editor_exe;
}
//_scenario_editor_executable__________________________________________________/
const CORN::OS::File_name &FARM_Directory::Dairy_directory::scenario_run_executable() provision_
{  if (!scenario_run_exe)
   {
      CORN::OS::Directory_name_concrete dairy_dir(this->w_str());
     scenario_run_exe=new CORN::OS::File_name_concrete(dairy_dir,"scenario_run.exe");
   }
   return *scenario_run_exe;
}
//_scenario_run_executable_____________________________________________________/
NIFA_Directory::NIFA_Directory()
: FARM_Directory(L"NIFA")                                                        //141110
{}
//_NIFA_Directory:constructor_______________________________________2011-10-10_/
NIFA_Directory::NIFA_Directory(const CS::Suite_Directory &suite_dir,const wchar_t * directory_name)
: FARM_Directory(suite_dir,directory_name)
{}
//_NIFA_Directory:constructor_______________________________________2014-11-11_/
NIFA_Directory::Dairy_directory::Dairy_directory
(const NIFA_Directory &path_name
,const wchar_t *directory_name)
: FARM_Directory::Dairy_directory(path_name,directory_name)
, nutrients_template_xlsm_file(0)
{}
//_Dairy_directory:constructor______________________________________2014-11-11_/
void NIFA_Directory::Dairy_directory::reset()                         provision_
{  delete nutrients_template_xlsm_file; nutrients_template_xlsm_file= 0;
   FARM_Directory::Dairy_directory::reset();
}
//_reset____________________________________________________________2014-11-11_/
const CORN::OS::File_name &NIFA_Directory::Dairy_directory::nutrients_template_xlsm() provision_
{  if (!nutrients_template_xlsm_file)
   {
      CORN::OS::Directory_name_concrete dairy_dir(this->w_str());
      nutrients_template_xlsm_file=new CORN::OS::File_name_concrete(dairy_dir,"nutrients-template.xlsm");
   }
   return *nutrients_template_xlsm_file;
}
//_nutrients_template_xlsm_____________________________________________________/
}//_namespace_CS_______________________________________________________________/
