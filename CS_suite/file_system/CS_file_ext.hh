#ifndef CS_file_extHH
#define CS_file_extHH
//#include <string>
/*abanonded
namespace std
{  class string;
   class wstring;
}
*/
#  define CS_scenario_LEXT_V5       L"CropSyst_scenario"
                                 // Prefixed with CropSyst to distinguish from CANMS_scenario
#  define CS_crop_LEXT_V5           L"CS_crop"
#  define CS_soil_LEXT_V5           L"CS_soil"
#  define CS_water_table_LEXT_V5    L"CS_watertable"
#  define CS_rotation_LEXT_V5       L"CS_rotation"
#  define CS_recalibration_LEXT_V5  L"CS_recalibration"
#  define CS_management_LEXT_V5     L"CS_management"

#  define CS_scenario_LEXT_V4       L"csn"
#  define CS_crop_LEXT_V4           L"crp"
#  define CS_soil_LEXT_V4           L"sil"
#  define CS_management_LEXT_V4     L"mgt"
#  define CS_water_table_LEXT_V4    L"wtb"
#  define CS_rotation_LEXT_V4       L"rot"
#  define CS_recalibration_LEXT_V4  L"rcl"
#  define CS_project_LEXT_V4        L"csp"


#if (CS_VERSION >= 5)
#  define CS_scenario_LEXT       CS_scenario_LEXT_V5
#  define CS_crop_LEXT           CS_crop_LEXT_V5
#  define CS_soil_LEXT           CS_soil_LEXT_V5
#  define CS_water_table_LEXT    CS_water_table_LEXT_V5
#  define CS_rotation_LEXT       CS_rotation_LEXT_V5
#  define CS_recalibration_LEXT  CS_recalibration_LEXT_V5
#  define CS_management_LEXT     CS_management_LEXT_V5
#else
#  define CS_scenario_LEXT       CS_scenario_LEXT_V4
#  define CS_crop_LEXT           CS_crop_LEXT_V4
#  define CS_soil_LEXT           CS_soil_LEXT_V4
#  define CS_management_LEXT     CS_management_LEXT_V4
#  define CS_water_table_LEXT    CS_water_table_LEXT_V4
#  define CS_rotation_LEXT       CS_rotation_LEXT_V4
#  define CS_recalibration_LEXT  CS_recalibration_LEXT_V4
#  define CS_project_LEXT        CS_project_LEXT_V4
#endif


#if (defined(USE_OS_FS) || (__BCPLUSPLUS__ > 0x0600))
#  define CS_PROJECT_FILENAME   L".CS_project"
#  define CS_SCENARIO_FILENAME  L".CropSyst_scenario"
#else
#  define CS_PROJECT_FILENAME     ".CS_project"
#  define CS_SCENARIO_FILENAME    ".CropSyst_scenario"
#endif


#  define CS_scenario_EXT        "CropSyst_scenario"
// these will probably be abandoned The next version will used YAML documents where the filename extension correspond to primary key
#  define CS_crop_EXT_V5            "CS_crop"
#  define CS_soil_EXT_V5            "CS_soil"
#  define CS_water_table_EXT_V5     "CS_watertable"
#  define CS_rotation_EXT_V5        "CS_rotation"
#  define CS_recalibration_EXT_V5   "CS_recalibration"
#  define CS_management_EXT_V5      "CS_management"

//#  define CS_scenario_EXT_V4        "csn"
#  define CS_crop_EXT_V4            "crp"
#  define CS_soil_EXT_V4            "sil"
#  define CS_management_EXT_V4      "mgt"
#  define CS_water_table_EXT_V4     "wtb"
#  define CS_rotation_EXT_V4        "rot"
#  define CS_recalibration_EXT_V4   "rcl"
#  define CS_project_EXT_V4         "csp"

/*disabled 200604
#if (CS_VERSION >= 5)
#  define CS_scenario_EXT        CS_scenario_EXT_V5
#  define CS_crop_EXT            CS_crop_EXT_V5
#  define CS_soil_EXT            CS_soil_EXT_V5
#  define CS_water_table_EXT     CS_water_table_EXT_V5
#  define CS_rotation_EXT        CS_rotation_EXT_V5
#  define CS_recalibration_EXT   CS_recalibration_EXT_V5
#  define CS_management_EXT      CS_management_EXT_V5
#else
#  define CS_scenario_EXT        CS_scenario_EXT_V4
#  define CS_crop_EXT            CS_crop_EXT_V4
#  define CS_soil_EXT            CS_soil_EXT_V4
#  define CS_management_EXT      CS_management_EXT_V4
#  define CS_water_table_EXT     CS_water_table_EXT_V4
#  define CS_rotation_EXT        CS_rotation_EXT_V4
#  define CS_recalibration_EXT   CS_recalibration_EXT_V4
#  define CS_project_EXT         CS_project_EXT_V4

#endif
*/
#  define CS_biomatter_EXT          "CS_biomatter"
#  define CS_biomatter_LEXT        L"CS_biomatter"
#  define CS_biomatter_wildcard     "*." CS_biomatter_EXT
#  define CS_scenario_wildcard      "*." CS_scenario_EXT

/*abandoned
#  define CS_crop_wildcard_V5          "*." CS_crop_EXT_V5
#  define CS_soil_wildcard_V5          "*." CS_soil_EXT_V5
#  define CS_water_table_wildcard_V5   "*." CS_water_table_EXT_V5
#  define CS_rotation_wildcard_V5      "*." CS_rotation_EXT_V5
#  define CS_recalibration_wildcard_V5 "*." CS_recalibration_EXT_V5
#  define CS_management_wildcard_V5    "*." CS_management_EXT_V5
*/

#  define CS_crop_wildcard          "*." CS_crop_EXT_V4
#  define CS_soil_wildcard          "*." CS_soil_EXT_V4
#  define CS_water_table_wildcard   "*." CS_water_table_EXT_V4
#  define CS_rotation_wildcard      "*." CS_rotation_EXT_V4
#  define CS_recalibration_wildcard "*." CS_recalibration_EXT_V4
#  define CS_management_wildcard    "*." CS_management_EXT_V4


//______________________________________________________________________________
struct Alternate_extensions
{
   char    *section;  // Corresponding to the scenario [section] the variable is under
   char    *variable; // Corresponding the parameter file name variable in the scenario file
   wchar_t *extension[2];
};
extern Alternate_extensions alternate_extension[];

extern Alternate_extensions *lookup_alternate_extensions_by_variable
   (const char * variable);
//   (const std::string & /*180821 char * */variable);
extern Alternate_extensions *lookup_alternate_extensions_by_extension
   (const wchar_t *extension);
//   (const std::wstring &/*180828 wchar_t **/extension);
#endif
