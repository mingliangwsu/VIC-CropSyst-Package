#include "options.h"
#include "csvc.h"
#include "cs_vars.h"
#include "cs_vars_abbrv.h"
#include "static_phrases.h"
//______________________________________________________________________________
CS_output_variable_layout CS_output_variables[]=
{//status      var_code                            ,pref units             ,mnwrn,mxwrn,prcsn,desc
 /*compout  */ {CSVC_crop_base_growing_degree_days_period_accum      ,UT_day           ,0    ,10000/*,1*/                           ,TL_Growing_degree_days,NO_ABBRV_NO_COLOR}
,/*compout  */ {CSVC_crop_biomass_canopy                             ,UC_kg_ha        ,0    ,1000  /*max warn will be adjusted*/    ,TL_Dry_biomass              ,NO_ABBRV_NO_COLOR}  // 070207 wsa CSVC_crop_base_dry_biomass
,/*compout  */ {CSVC_crop_base_root_biomass_current                  ,UC_kg_ha        ,0    ,1000  /*max warn will be adjusted*/    ,TL_Root_biomass             ,NO_ABBRV_NO_COLOR}  // 070207 was  CSVC_crop_base_root_biomass
,/*compout  */ {CSVC_crop_base_leaf_area_index                       ,UC_index              ,0    ,4   /*max warn will be adjusted*/,TL_Leaf_area_index          ,NO_ABBRV_NO_COLOR}
,/*compout  */ {CSVC_crop_base_green_area_index                      ,UC_index              ,0    ,4   /*max warn will be adjusted*/,TL_Green_area_index         ,NO_ABBRV_NO_COLOR}
,/*compout  */ {CSVC_crop_base_root_depth                            ,UC_meter     ,0    ,3    /*max warn will be adjusted*/        ,TL_Root_depth               ,NO_ABBRV_NO_COLOR}
,/*compout  */ {CSVC_crop_base_water_stress_factor                   ,UC_factor             ,0    ,1    /*,2*/    ,TL_Crop_water_stress        ,NO_ABBRV_NO_COLOR}
,/*compout  */ {CSVC_crop_base_water_stress_index                    ,UC_index              ,0    ,1    /*,2*/    ,"Crop water stress index"   ,NO_ABBRV_NO_COLOR}
,/*compout  */ {CSVC_crop_base_temperature_stress_index              ,UC_index              ,0    ,1    /*,2*/    ,TL_Temperature_stress       ,NO_ABBRV_NO_COLOR}
,/*compout  */ {CSVC_crop_base_fract_canopy_cover                    ,UC_decimal_percent    ,0    ,1    /*,2*/    ,TL_Canopy_ground_cover      ,NO_ABBRV_NO_COLOR}
,/*compout  */ {CSVC_crop_base_transp_pot_time_step_sum              ,UC_mm   ,0    ,15   /*,2*/    ,"Transpiration potential" ,NO_ABBRV_NO_COLOR}
,/*compout  */ {CSVC_crop_base_transp_pot_accum_to_date_time         ,UC_mm   ,0    ,15   /*,2*/    ,"Transpiration potential" ,NO_ABBRV_NO_COLOR}
,/*compout  */ {CSVC_crop_base_transp_act_time_step_sum              ,UC_mm  ,0    ,15   /*,2*/    ,"Transpiration actual" ,NO_ABBRV_NO_COLOR}
,/*compout  */ {CSVC_crop_base_transp_act_accum_to_date_time         ,UC_mm  ,0    ,15   /*,2*/    ,"Transpiration actual" ,NO_ABBRV_NO_COLOR}
,/*compout  */ {CSVC_crop_base_water_intrcpt_time_step_sum           ,UC_mm  ,0    ,30   /*,2*/    ,TL_Crop_water_interception   ,NO_ABBRV_NO_COLOR}
,/*compout  */ {CSVC_crop_base_water_intrcpt_accum_to_date_time      ,UC_mm  ,0    ,30   /*,2*/    ,TL_Crop_water_interception   ,NO_ABBRV_NO_COLOR}
,/*compout  */ {CSVC_weather_ET_max_time_step_sum                    ,UC_mm  ,0    ,15   /*,2*/    ,"Evapotranspiration maximum" ,NO_ABBRV_NO_COLOR}
,/*compout  */ {CSVC_weather_ET_act_time_step_sum                    ,UC_mm  ,0    ,15   /*,2*/    ,"Evapotranspiration actual"  ,NO_ABBRV_NO_COLOR}
,/*compout  */ {CSVC_weather_ET_ref_time_step_sum                    ,UC_mm  ,0    ,15   /*,2*/    ,"Reference evapotranspiration",NO_ABBRV_NO_COLOR}  //130723
,/*compout  */ {CSVC_weather_precipitation_time_step_sum             ,UC_mm  ,0    ,30   /*,2*/    ,"Precipitation",NO_ABBRV_NO_COLOR} //130723
,/*compout  */ {CSVC_weather_temperature_time_step_min               ,UC_Celcius    ,-20    ,40   /*,2*/    ,TL_Temperature,NO_ABBRV_NO_COLOR}
,/*compout  */ {CSVC_weather_temperature_time_step_max               ,UC_Celcius    ,-20    ,40   /*,2*/    ,TL_Temperature,NO_ABBRV_NO_COLOR}
,/*compout  */ {CSVC_weather_solar_radiation                         ,UC_MJ_per_m2_solar_radiation    ,0    ,35   /*,2*/    ,"Evapotranspiration actual",NO_ABBRV_NO_COLOR}
,/*compout  */ {CSVC_soil_runoff_surface_water_runoff_time_step_sum  ,UC_mm    ,0    ,30   /*,2*/    ,TL_Surface_water_runoff     ,NO_ABBRV_NO_COLOR}
,/*compout  */ {CSVC_soil_base_water_entering_time_step_sum          ,UC_mm    ,0    ,30   /*,2*/    ,TL_Water_entering_soil      ,NO_ABBRV_NO_COLOR}
,/*compout  */ {CSVC_soil_base_water_drainage_time_step_sum          ,UC_mm    ,0    ,30   /*,2*/    ,TL_Soil_water_drainage      ,NO_ABBRV_NO_COLOR}
,/*compout  */ {CSVC_soil_base_evap_act_time_step_sum                ,UC_mm    ,0    ,15   /*,2*/    ,TL_Actual_soil_evaporation  ,NO_ABBRV_NO_COLOR}
#ifdef NITROGEN
,/*compout  */ {CSVC_crop_N_stress_factor                            ,UC_factor             ,0    ,1    /*,2*/    ,"Nitrogen stress"         ,NO_ABBRV_NO_COLOR}
,/*compout  */ {CSVC_crop_N_stress_index                             ,UC_index              ,0    ,1    /*,2*/    ,"Nitrogen stress index"   ,NO_ABBRV_NO_COLOR}

,/*notoutyet*/ {CSVC_crop_N_conc_canopy                              ,UC_kg_kg /*UC_kg_solid_kg_liquid*/        ,0    ,20   /*,2*/    ,"Canopy N concentration",NO_ABBRV_NO_COLOR}

,/*notoutyet*/ {CSVC_crop_N_fixation_time_step_sum                   ,UC_kg_ha/*N*/   ,0    ,20   /*,2*/    ,TL_N_fixation              ,NO_ABBRV_NO_COLOR}
,/*notoutyet*/ {CSVC_crop_N_fixation_period_sum                      ,UC_kg_ha/*N*/   ,0    ,20   /*,2*/    ,TL_N_fixation              ,NO_ABBRV_NO_COLOR}
,/*notoutyet*/ {CSVC_crop_N_fixation_accum_to_date_time              ,UC_kg_ha/*N*/   ,0    ,20   /*,2*/    ,TL_N_fixation              ,NO_ABBRV_NO_COLOR}
#endif

,/*compout  */ {CSVC_water_table_depth                               ,UC_meter     ,0    ,3    /*max warn will be adjusted*/    ,"Water table depth"               ,NO_ABBRV_NO_COLOR}

// The following are by soil layer
,/*compout  */ {CSVC_soil_base_water_content                         ,UC_m3_m3 /*UC_m3_liquid_m3_solid*/   ,0  ,  1,TL_Water_content         ,NO_ABBRV_NO_COLOR}
,/*compout  */ {CSVC_soil_base_plant_avail_water                     ,UC_decimal_percent   ,0  ,  1,TL_Plant_available_water2 /*181030 TL_Water_content */         ,NO_ABBRV_NO_COLOR}
,/*compout  */ {CSVC_soil_base_water_potential                       ,UC_J_kg /*UC_J_kg_liquid*/  ,0  ,  1,TL_Water_potential       ,NO_ABBRV_NO_COLOR}
,/*compout  */ {CSVC_soil_base_temperature_time_step_avg             ,UC_Celcius      ,-20, 40,TL_Temperature           ,NO_ABBRV_NO_COLOR}
,/*compout  */ {CSVC_soil_base_salinity                              ,UC_decisiemens      ,0, 20,"Salinity"          ,NO_ABBRV_NO_COLOR}
#ifdef NITROGEN

,/*compout  */ {CSVC_soil_N_N_amount                                 ,UC_kg_ha/*N*/   ,  0, 50,TL_Nitrogen              ,NO_ABBRV_NO_COLOR}
,/*compout  */ {CSVC_soil_N_NO3_amount                               ,UC_kg_ha/*N*/   ,  0, 50,TL_Nitrate               ,NO_ABBRV_NO_COLOR}
,/*compout  */ {CSVC_soil_N_NH4_amount                               ,UC_kg_ha/*N*/   ,  0, 50,TL_Ammonium              ,NO_ABBRV_NO_COLOR}
,/*compout  */ {CSVC_soil_N_mineralization_time_step_sum             ,UC_kg_ha/*N*/   ,  0, 50,TL_Mineralization        ,NO_ABBRV_NO_COLOR}
,/*compout  */ {CSVC_soil_N_nitrification_time_step_sum              ,UC_kg_ha/*N*/   ,  0, 50,TL_Nitrification         ,NO_ABBRV_NO_COLOR}
,/*compout  */ {CSVC_soil_N_nitrification_time_step_sum              ,UC_kg_ha/*N*/   ,  0, 50,TL_Denitrification       ,NO_ABBRV_NO_COLOR}
,/*compout  */ {CSVC_organic_matter_percent_time_step_sum            ,UC_percent            ,  0, 50,TL_Percent_organic_matter,NO_ABBRV_NO_COLOR}
#endif
,              {0 /* End of list */,0,0,0,0,NO_ABBRV_NO_COLOR}
};
//_CS_output_variables_________________________________________________________/
float64 CS_UED_max_root_biomass_encountered     = 0.0;
float64 CS_UED_max_canopy_biomass_encountered   = 0.0;
//______________________________________________________________________________
void set_CS_output_variable_max_warning_range(CS_UED_variable_code code,float highest_value)
{  // Lookup the layout for the variable of the range to change.
   CS_output_variable_layout *layout_to_change = 0;
   for (int i = 0
       ; CS_output_variables[i].code
       ; i++)
      if (CS_output_variables[i].code == code)
      {  layout_to_change = &CS_output_variables[i];
         break;
      }
   // Find the power of 10 just greater than the highest value.
   float max_factor = 1.0;
   for (max_factor = 1.0; max_factor < highest_value ; max_factor *= 10.0);
   float dec_factor = max_factor / 10;
   while (max_factor >= highest_value)
      max_factor -= dec_factor;
   max_factor += dec_factor;
   if (max_factor > layout_to_change->max_warning)
      layout_to_change->max_warning = max_factor;
}
//_set_CS_output_variable_max_warning_range____________________________________/
// These are variable selection bitmasks
// There is one corresponding to each CropSyst class
// thus each CropSyst class can have up to 256 output variables
// (actually 64k, if no layered variables are used)

nat32 sel_vars_weather_file      = 0;    // CSVC_weather_file
nat32 sel_vars_weather_VPD       = 0;     // CSVC_VPD_weather
nat32 sel_vars_weather_ET        = 0;      //

nat32 sel_vars_soil_base         = 0;
nat32 sel_vars_soil_chemical     = 0;
nat32 sel_vars_soil_N            = 0;
nat32 sel_vars_soil_freezing     = 0;
nat32 sel_vars_soil_erosion      = 0;
nat32 sel_vars_soil_runoff       = 0;
nat32 sel_vars_soil_complete     = 0;

nat32 sel_vars_crop_base         = 0;
nat32 sel_vars_crop_nitrogen     = 0;
nat32 sel_vars_crop_complete     = 0;

nat32 sel_vars_residue           = 0;
nat32 sel_vars_residue_straw     = 0;
nat32 sel_vars_residue_manure    = 0;

nat32 sel_vars_water_table       = 0;

nat32 sel_vars_management        = 0;
nat32 sel_vars_management_irrigation = 0;
nat32 sel_vars_management_N  = 0;
nat32 sel_vars_management_NH4       = 0;
nat32 sel_vars_management_NO3       = 0;

nat32 sel_vars_computed_for_output  = 0;
//______________________________________________________________________________
const char *CSVC_atmospheric_descriptions_to_be_translated[] =
{"Weather"                 // 00
,"vapor"                   // 01
,"evapotranspiration"      // 02
,"date"                       // 03
/*
,"-"                       // 04 reserved
,"-"                       // 05 reserved
,"-"                       // 06 reserved
,"-"                       // 07 reserved
,"-"                       // 08 reserved
,"-"                       // 09 reserved
,"-"                       // 0A reserved
,"-"                       // 0B reserved
,"-"                       // 0C reserved
,"-"                       // 0D reserved
,"-"                       // 0E reserved
,"-"                       // 0F reserved
*/
,0
};
//_CSVC_atmospheric_descriptions_to_be_translated______________________________/
const char *CSVC_soil_descriptions_to_be_translated[] =
{"general"                       // 10 reserved
,"-"                       // 11 reserved
,"carbon"             // 12
,"nitrogen"           // 13
,"freezing"           // 14
,"erosion"            // 15
,"runoff"             // 16
,"complete"           // 17
/*
,"-"                       // 18 reserved
,"-"                       // 19 reserved
,"-"                       // 1A reserved
,"-"                       // 1B reserved
,"-"                       // 1C reserved
,"-"                       // 1D reserved
,"-"                       // 1E reserved
,"-"                       // 1F reserved
*/
,0
};
//_CSVC_soil_descriptions_to_be_translated_____________________________________/
const char *CSVC_crop_descriptions_to_be_translated[] =
{"general"                 // 20
,"base"                    // 21 crop base
,"-"                       // 22 reserved
,"nitrogen"                // 23
,"-"                       // 24 reserved
,"-"                       // 25 reserved
,"-"                       // 26 reserved
,"-"                       // 27 reserved
,"-"                       // 28 reserved
,"-"                       // 29 reserved
,"specific"                // 2A
/*
,"-"                       // 2B reserved
,"-"                       // 2C reserved
,"-"                       // 2D reserved
,"-"                       // 2E reserved
,"-"                       // 2F reserved
*/
,0
};
//_CSVC_crop_descriptions_to_be_translated_____________________________________/
const char *CSVC_residue_descriptions_to_be_translated[] =
{"general"                 // 30
,"plant"                   // 31   I.e. straw
,"manure"                  // 32
,"-"                       // 33 reserved
,"-"                       // 34 reserved
,"organic_matter"          // 35
/*

,"-"                       // 36 reserved
,"-"                       // 37 reserved
,"-"                       // 38 reserved
,"-"                       // 39 reserved
,"-"                       // 3A reserved
,"-"                       // 3B reserved
,"-"                       // 3C reserved
,"-"                       // 3D reserved
,"-"                       // 3E reserved
,"-"                       // 3F reserved
*/
,0
};
//_CSVC_residue_descriptions_to_be_translated__________________________________/
const char *CSVC_water_table_descriptions_to_be_translated[] =
{"general"                       // 40 reserved
,"depth"                       // 41 reserved
/*

,"-"                       // 42 reserved
,"-"                       // 43 reserved
,"-"                       // 44 reserved
,"-"                       // 45 reserved
,"-"                       // 46 reserved
,"-"                       // 47 reserved
,"-"                       // 48 reserved
,"-"                       // 49 reserved
,"-"                       // 4A reserved
,"-"                       // 4B reserved
,"-"                       // 4C reserved
,"-"                       // 4D reserved
,"-"                       // 4E reserved
,"-"                       // 4F reserved
*/
,0
};
//_CSVC_water_table_descriptions_to_be_translated______________________________/
const char *CSVC_management_descriptions_to_be_translated[] =
{"general"              // 50 reserved
,"irrigation"           // 51 reserved
,"nitrogen"             // 52 reserved
/*
,"-"                       // 53 reserved
,"-"                       // 54 reserved
,"-"                       // 55 reserved
,"-"                       // 56 reserved
,"-"                       // 57 reserved
,"-"                       // 58 reserved
,"-"                       // 59 reserved
,"-"                       // 5A reserved
,"-"                       // 5B reserved
,"-"                       // 5C reserved
,"-"                       // 5D reserved
,"-"                       // 5E reserved
,"-"                       // 5F reserved
*/
,0
};
//_CSVC_management_descriptions_to_be_translated_______________________________/
const char *CSVC_xxx_descriptions_to_be_translated[] =
{"-"                       // 60 reserved
/*
,"-"                       // 61 reserved
,"-"                       // 62 reserved
,"-"                       // 63 reserved
,"-"                       // 64 reserved
,"-"                       // 65 reserved
,"-"                       // 66 reserved
,"-"                       // 67 reserved
,"-"                       // 68 reserved
,"-"                       // 69 reserved
,"-"                       // 6A reserved
,"-"                       // 6B reserved
,"-"                       // 6C reserved
,"-"                       // 6D reserved
,"-"                       // 6E reserved
,"-"                       // 6F reserved
*/
,0
};
//_CSVC_xxx_descriptions_to_be_translated______________________________________/
const char *CSVC_other_descriptions_to_be_translated[] =
{"computed"                       // 70 reserved
/*
,"-"                       // 71 reserved
,"-"                       // 72 reserved
,"-"                       // 73 reserved
,"-"                       // 74 reserved
,"-"                       // 75 reserved
,"-"                       // 76 reserved
,"-"                       // 77 reserved
,"-"                       // 78 reserved
,"-"                       // 79 reserved
,"-"                       // 7A reserved
,"-"                       // 7B reserved
,"-"                       // 7C reserved
,"-"                       // 7D reserved
,"-"                       // 7E reserved
,"-"                       // 7f reserved
*/
,0
};
//_CSVC_other_descriptions_to_be_translated____________________________________/
const char *CSVCG_descriptions_to_be_translated[] =
{"Atmospheric"             // 0
,"Soil"                    // 1
,"Crop"                    // 2
,"Residue"                 // 3
,"Water table"             // 4
,"Management"              // 5
,"Chemical"                // 6
,"Other"                   // 7
,0
};
//_CSVCG_descriptions_to_be_translated_________________________________________/
nat32 CropSyst_Variable_code_clad::get_class_color() const
{
   nat32 class_color = 0xFFFFFF;
   switch (get_class())
   { case CSC_weather                :
     case CSC_weather_VPD            :
     case CSC_weather_ET             : class_color = 0xFEEFEF;       break;
/*NYI
     case CSVC_soil_base              :
     case CSVC_soil_chemical          :
     case CSVC_soil_nitrogen          :
     case CSVC_soil_freezing          :
     case CSVC_soil_erosion           :
     case CSVC_soil_complete          :
          class_color = 0xFF99FF;
     break;
*/
     case CSC_crop_base              :
     case CSC_crop_nitrogen_misc     :
     case CSC_crop_nitrogen_mass     :
     case CSC_crop_complete          : class_color = 0xCCFFBB;       break;
     case CSC_residue                :
     case CSC_residue_plant          :
     case CSC_residue_manure         : class_color = 0xFFFFDB;       break;
     case CSC_water_table            : class_color = 0xAAAAFF;       break;
     case CSC_management             :
     case CSC_management_irrigation  :
     case CSC_management_N           :
     case CSC_management_NH4         :
     case CSC_management_NO3         : class_color = 0xFFDDCC;       break;
     // NYI case CSC_miscellaneous_N
     // NYI case CSC_other_output
     default : class_color = 0xFFFFFF;
     break;
   };
   return class_color;
}
//_get_class_color_____________________________________________________________/
std::string &CropSyst_Variable_code_clad::get_class_description(std::string &buffer)
{
   nat8  class_group       = get_class_group();
   nat8  class_distinction = get_class_distinction();
   //170217unused nat16 csv               = get_class_variable();
   buffer.assign(CSVCG_descriptions_to_be_translated[class_group]);
   buffer.append(": ");
   const char *distinction_label = 0;
   switch(class_group)
   {  case CSVCG_weather      : distinction_label = CSVC_atmospheric_descriptions_to_be_translated[class_distinction];  break;
      case CSVCG_soil         : distinction_label = CSVC_soil_descriptions_to_be_translated[class_distinction];         break;
      case CSVCG_crop         : distinction_label = CSVC_crop_descriptions_to_be_translated[class_distinction];         break;
      case CSVCG_residue      : distinction_label = CSVC_residue_descriptions_to_be_translated[class_distinction];      break;
      case CSVCG_water_table  : distinction_label = CSVC_water_table_descriptions_to_be_translated[class_distinction];  break;
      case CSVCG_management   : distinction_label = CSVC_management_descriptions_to_be_translated[class_distinction];   break;
//    case CSVCG_xxxxxxx      : distinction_label = CSVC_xxxxxxxxxxx_descriptions_to_be_translated[class_distinction];  break;
      case CSVCG_other        : distinction_label = CSVC_other_descriptions_to_be_translated[class_distinction];        break;
   };
   if (distinction_label)
      buffer.append(distinction_label);
   return buffer;
}
//_get_class_description_______________________________________________________/
const char *CropSyst_Variable_code_clad::get_abbreviation() const
{  for (int i = 0;csvc_abbreviations[i].code; i++)
      if (csvc_abbreviations[i].code == get()) return csvc_abbreviations[i].abbr;
   return "unknown";
}
//_get_abbreviation____________________________________________________________/

