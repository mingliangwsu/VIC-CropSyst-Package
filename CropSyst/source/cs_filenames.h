#ifndef CS_filenamesH
#define CS_filenamesH

#include "corn/OS/directory_entry_name_concrete.h"
#include "corn/OS/filtered_filename.h"
using namespace CORN;

#include "CS_suite/file_system/CS_file_ext.hpp"
//______________________________________________________________________________
#define CS_filename(XXX_filename,default_name,extension,description,wildcard)  \
class XXX_filename                                                             \
: public CORN::Filtered_file_name                                           \
{ public:                                                                      \
    inline XXX_filename(const char *fname_)                                    \
      : CORN::Filtered_file_name(fname_ ? fname_ : default_name            \
         ,extension,description,wildcard)                                   {} \
    inline XXX_filename(const OS::File_name_concrete &fname_)                  \
      : CORN::Filtered_file_name(fname_.length()?fname_.c_str():default_name     \
         ,extension,description,wildcard)                                   {} \
};

//_XXX_filename________________________________________________________________/
CS_filename(CRP_filename,"noname",CS_crop_EXT_V4         ,"CropSyst Crop"              ,CS_crop_wildcard);
CS_filename(SIL_filename,"noname",CS_soil_EXT_V4         ,"CropSyst Soil"              ,CS_soil_wildcard);
CS_filename(ROT_filename,"noname",CS_rotation_EXT_V4     ,"CropSyst Cropping system (rotation)",CS_rotation_wildcard);
CS_filename(MGT_filename,"noname",CS_management_EXT_V4   ,"CropSyst Management"        ,CS_management_wildcard);
CS_filename(WTB_filename,"noname",CS_water_table_EXT_V4  ,"CropSyst Water table"       ,CS_water_table_wildcard);
CS_filename(CS_output_options_filename,"noname"       ,"CS_output_options" ,"CropSyst output options","*.cs_output_options");
CS_filename(Biomass_decomposition_parameters_filename ,"noname"            ,"CS_biomatter","CropSyst Biomatter","*.CS_biomatter");
//CS_filename(XXX_filename,"noname",extension,description,wildcard);
CS_filename(STATSGO_filename,"Optional (Currently not selected)","DBF","MAPUNIT.DBF","*.DBF");

//#if (CS_VERSION == 5)
CS_filename(CropSyst_scenario_filename,"noname",CS_scenario_EXT,"CropSyst scenario","*.CropSyst_scenario|*.csn");
#if ((CS_VERSION > 0) && (CS_VERSION < 5))
// V3 files that will be completely obsolete in V5
//CS_filename(CSN_filename,"noname",CS_scenario_EXT     ,"CropSyst scenario" ,"*.CropSyst_scenario");
CS_filename(SIM_filename,"noname","SIM","Simulation control","*.SIM");
CS_filename(CSN_filename,"noname",CS_scenario_EXT,"CropSyst scenario","*.CropSyst_scenario|*.csn");
CS_filename(CSP_filename,"noname","csp","CropSyst project","*.csp");
CS_filename(FMT_filename,"noname","FMT","Report format","*.FMT");
CS_filename(MGS_filename_V3,"noname","mgs","Fixed events","*.mgs");
#endif
//_filename____________________________________________________________________/
#define MGS_MGT_filename CORN::OS::File_name_concrete
//_MGS_MGT_filename_________________________________________________1999-02-28_/
#endif
//filename.h

