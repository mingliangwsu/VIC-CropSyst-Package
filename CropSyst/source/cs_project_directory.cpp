#include "cs_project_directory.h"
#include "database_dir.h"
#include "CS_suite/application/CS_suite_directory.h"
//______________________________________________________________________________
CropSyst_project_directory::CropSyst_project_directory
(const char *cropsyst_project_filename)
: Common_simulation_project_directory(cropsyst_project_filename)
, database_directory(0)                                                          //090518
#ifndef linux
, generate_scenarios_log_filename(0)                                             //101118
#ifndef LADSS_UNIX
, selected_scenarios("selected_scenarios.dat")                                   //060710
#endif
#endif
{
#ifndef LADSS_UNIX
// not needed for LADSS
   GIS_dir_qual.set_cstr("GIS");                                                 //130123_050203
   GIS_dir_qual.set_path(*this) ;                                                //050203
   GIS_aggregation_dir_qual.set_cstr("aggregation");                             //130123_050930
   GIS_aggregation_dir_qual.set_path(GIS_dir_qual);                              //050930
   // optimization models are at the same directory level as scenarios           //040806
   carbon_sequestration_optimization_dir_qual.set_cstr("Carbon_sequestration_optimization");//130123_040806
   carbon_sequestration_optimization_dir_qual.set_path(*this);                   //040806
   selected_scenarios.set_path(*this) ;                                          //060710
   scenario_comparison_dir_qual.set_cstr("scenario_comparison");                 //130123_070423
   scenario_comparison_dir_qual.set_path(*this);                                 //070423
#endif
}
//_CropSyst_project_directory__________________________________________________/
CropSyst_project_directory::CropSyst_project_directory
(const CORN::OS::File_name_concrete &dir_name)
: Common_simulation_project_directory(dir_name)
, database_directory(0)                                                          //090518
#ifndef linux
, generate_scenarios_log_filename(0)                                             //101118
#ifndef LADSS_UNIX
, selected_scenarios("selected_scenarios.dat")                                   //060710
#endif
#endif
{
#ifndef LADSS_UNIX
// not needed for LADSS
   GIS_dir_qual.set_str("GIS");                                                  //130123_050203
   GIS_dir_qual.set_path(*this) ;                                                //050203
   GIS_aggregation_dir_qual.set_cstr("aggregation");                             //130123_050930
   GIS_aggregation_dir_qual.set_path(GIS_dir_qual);                              //050930
   // optimization models are at the same directory level as scenarios           //040806
   carbon_sequestration_optimization_dir_qual.set_cstr("Carbon_sequestration_optimization");//130123_040806
   carbon_sequestration_optimization_dir_qual.set_path(*this);                   //040806
   selected_scenarios.set_path(*this) ;                                          //060710
   scenario_comparison_dir_qual.set_cstr("scenario_comparison");               //130123_070423
   scenario_comparison_dir_qual.set_path(*this);                                 //070423
#endif
}
//_constructor__________________________________________2010-02-09__2005-02-03_/
CropSyst_project_directory::~CropSyst_project_directory()
{  if (database_directory) delete database_directory; database_directory = 0;
   if (generate_scenarios_log_filename) delete generate_scenarios_log_filename; generate_scenarios_log_filename = 0; //101118
}
//_~CropSyst_project_directory______________________________________2009-05-18_/
const Database_directory &CropSyst_project_directory
::provide_database_directory(bool create_if_not_exists)               provision_
{  if (!database_directory)   database_directory = new Database_directory(*this);
   if (create_if_not_exists)  database_directory->create();
   return *database_directory;
}
//_provide_database_directory_______________________________________2009-05-18_/
#ifndef LADSS_UNIX
// not needed for LADSS
const CORN::OS::File_name_concrete &CropSyst_project_directory::get_GIS_dir_qual(bool create_if_necessary) const
{  if (create_if_necessary)   GIS_dir_qual.create();                             //051117
   return GIS_dir_qual;
}
//_get_GIS_dir_qual_________________________________________________2004-08-10_/
void CropSyst_project_directory::set_GIS_dir(const CORN::Ustring &GIS_dir_unqual)
{  // This was provided so the user can specify the name of the GIS dir
   GIS_dir_qual.set_name(GIS_dir_unqual);
}
//_set_GIS_dir______________________________________________________2009-11-05_/
const CORN::OS::File_name_concrete &CropSyst_project_directory
::get_GIS_aggregation_dir_qual(bool create_if_necessary)                   const
{  if (create_if_necessary)   GIS_aggregation_dir_qual.create();                 //051117
   return GIS_aggregation_dir_qual;
}
//_get_GIS_aggregation_dir_qual_____________________________________2004-08-10_/
const CORN::File_name &CropSyst_project_directory::get_GIS_polygon_scenarios_filename() const
{   return get_filename(true ,"GIS_scenarios.dbf",get_GIS_dir_qual(true));
}
//_get_GIS_polygon_scenarios_filename_______________________________2005-02-06_/
const CORN::OS::File_name_concrete &CropSyst_project_directory::get_carbon_sequestration_optimization_dir_qual(bool create_if_necessary) const
{  if (create_if_necessary) carbon_sequestration_optimization_dir_qual.create(); //051117
   return carbon_sequestration_optimization_dir_qual;
}
//_get_carbon_sequestration_optimization_dir_qual___________________2004-08-10_/
const CORN::OS::File_name_concrete &CropSyst_project_directory::get_scenario_comparison_dir_qual(bool create_if_necessary) const
{  if (create_if_necessary)   scenario_comparison_dir_qual.create();
   return scenario_comparison_dir_qual;
}
//_get_scenario_comparison_dir_qual_________________________________2007-04-23_/
#endif
bool CropSyst_project_directory
::get_associated_filename(CORN::Smart_file_name &return_filename)          const //101116
{  return_filename.set_name_ext(".CS_project");
   return_filename.set_path(*this);
   bool assoc_file_exists = return_filename.exists();
   return assoc_file_exists;
}
//_get_associated_filename__________________________________________2010-11-18_/
const CORN::Smart_file_name &CropSyst_project_directory
::generate_scenarios_log_file(const char *extension)                  provision_
{  if (!generate_scenarios_log_filename)
   {  generate_scenarios_log_filename = new CORN::Smart_file_name("generated_scenarios_log",extension);
      generate_scenarios_log_filename->set_path(*this);
   }
   return *generate_scenarios_log_filename;
}
//_generate_scenarios_log_file______________________________________2010-11-18_/

