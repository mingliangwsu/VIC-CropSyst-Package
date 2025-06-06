//180118 restored #ifndef CS_INSPECTOR_VERSION
#if ((CROPSYST_PROPER > 0) && (CROPSYST_PROPER <= 5))


// V4 only, in V6 UED output will use inspectors                                 //151005

#  include "csvc.h"
#  include "csvc_special.h"

#  include <stdlib.h>
#  include "cs_ET.h"
#  include "cs_mgmt.h"
#  include "cs_UED_daily.h"
#  include "land_unit_sim.h"
#  include "crop/crop_interfaced.h"
#  include "crop/crop_N_interface.h"
#  include "soil.h"
#  include "soil/chemical_balance.h"
#  include "soil/chemical_profile.h"
#  include "cs_accum.h"
#  include "soil/chemicals_profile.h"
#  ifdef NITROGEN
#     include "soil/nitrogen_profile.h"
#  endif
#  include "soil/erosion_RUSLE.h"
#  include "soil/runoff.h"
#  include "soil/soil_evaporator.h"
#  include "organic_matter/OM_residues_profile_abstract.h"
#  include "common/residue/residues_interface.h"
#  include "common/weather/weather.h"
#  include "crop/crop_cropsyst.h"
#  include "CS_suite/observation/CS_optation.h"
namespace CropSyst
{
//______________________________________________________________________________
struct Variable_field_registration
{  UED::Variable_code    CSVC_var_code
;  const char *         section_name
;  const char *         field_name
;  Units_code           preferred_units
;  Units_code           stored_units
; };
//______________________________________________________________________________
// Note that the section and variable name must match those used in xxx:setup_structure() methods.
#define daily_variable_fields_count 111
Variable_field_registration daily_variable_fields_registration [daily_variable_fields_count] =
{//{var_code,"section_name","field_name",preferred_units,stored_units}
   {CSVC_crop_base_root_biomass_current                        ,"crop_base",    "rootbiomass",                    UC_kg_ha          ,UC_kg_m2}     // This had previously been output preferred as UC_kg_ha might not be necessary anymore and can probably be UC_kg_m2. Waiting to test this.  // was /*CSVC_crop_base_root_biomass
,  {CSVC_crop_base_water_stress_factor                         ,"crop_base",    "waterstress",                    UC_decimal_percent,UC_decimal_percent} // Actually I think I want these units to be UC_index
,  {CSVC_crop_base_temperature_stress_factor                   ,"crop_base",    "tempstress",                     UC_decimal_percent,UC_decimal_percent}// Actually I think I want these units to be UC_index
,  {CSVC_crop_base_growing_degree_days_period_accum            ,"crop_base",    "degdays",                        UC_Celcius_degree_days            ,UC_Celcius_degree_days}
,  {CSVC_crop_base_seasonal_thermal_time_period_sum            ,"crop_base",    "thermal_time",                   UC_Celcius_degree_days            ,UC_Celcius_degree_days}
,  {CSVC_crop_biomass_canopy                                   ,"crop_base",    "canopy_BM",                      UC_kg_ha          ,UC_kg_m2}     // This had previously been output preferred as UC_kg_ha might not be necessary anymore and can probably be UC_kg_m2. Waiting to test this.  // was CSVC_crop_base_dry_biomass
,  {CSVC_crop_biomass_yield                                    ,"crop_base",    "yield_BM",                       UC_kg_ha          ,UC_kg_m2} //181031

,  {CSVC_crop_base_green_biomass                               ,"crop_base",    "green_canopy_BM",                UC_kg_ha          ,UC_kg_m2}     // This had previously been output preferred as UC_kg_ha might not be necessary anymore and can probably be UC_kg_m2. Waiting to test this. // was CSVC_crop_base_live_biomass
,  {CSVC_crop_base_fruit_biomass                               ,"crop_base",    "fruit_BM",                       UC_kg_m2          ,UC_kg_m2}
,  {CSVC_crop_base_fruit_canopy_biomass                        ,"crop_base",    "non-fruit_canopy_BM",            UC_kg_m2          ,UC_kg_m2}
,  {CSVC_crop_base_leaf_area_index                             ,"crop_base",    "LAI",                            UC_index          ,UC_index}
,  {CSVC_crop_base_green_area_index                            ,"crop_base",    "GAI",                            UC_index          ,UC_index}
,  {CSVC_crop_base_root_depth                                  ,"crop_base",    "root_depth",                     UC_meter          ,UC_meter}
,  {CSVC_crop_base_water_stress_index                          ,"crop_base",    "water_stress_index",             UC_index          ,UC_index}
,  {CSVC_crop_base_temperature_stress_index                    ,"crop_base",    "temperature_stress_index",       UC_index          ,UC_index}
,  {CSVC_crop_base_VPD_daytime                                 ,"crop_base",    "VPD_daytime",                    UC_kPa            ,UC_kPa}   // check units
,  {CSVC_crop_base_intercepted_PAR                             ,"crop_base",    "intercepted_PAR",                UC_MJ_m2          ,UC_MJ_m2}   // need to check units
,  {CSVC_crop_base_intercepted_PAR_accum_to_date_time          ,"crop_base",    "intercepted_PAR_accum",          UC_MJ_m2          ,UC_MJ_m2}         //  need to check units
,  {CSVC_crop_base_fract_canopy_cover                          ,"crop_base",    "canopy_cover_green",             UC_decimal_percent,UC_decimal_percent}   // Actually I think the units need to be a faction, when changed, check fast graph also
,  {CSVC_crop_base_fract_canopy_cover_total                    ,"crop_base",    "canopy_cover_total",             UC_decimal_percent,UC_decimal_percent}   // 200229 Actually I think the units need to be a faction, when changed, check fast graph also
,  {CSVC_crop_base_leaf_water_pot                              ,"crop_base",    "leaf_water_pot",                 UC_J_per_kg       ,UC_J_per_kg}
,  {CSVC_crop_base_transp_pot_time_step_sum                    ,"crop_base",    "transp_pot",                     UC_mm             ,UC_meter}     // This had previously been output preferred as UC_mm might not be necessary anymore and can probably be UC_m. Waiting to test this.
,  {CSVC_crop_base_transp_act_time_step_sum                    ,"crop_base",    "transp_act",                     UC_mm             ,UC_meter}     // This had previously been output preferred as UC_mm might not be necessary anymore and can probably be UC_m. Waiting to test this.
,  {CSVC_crop_base_water_intrcpt_time_step_sum                 ,"crop_base",    "crop_water_intrcpt",             UC_mm             ,UC_meter}     // This had previously been output preferred as UC_mm might not be necessary anymore and can probably be UC_m. Waiting to test this.
,  {CSVC_crop_N_stress_factor                                  ,"crop_N",       "N_stress",                       UC_factor         ,UC_factor}
,  {CSVC_crop_N_stress_index                                   ,"crop_N",       "N_stress_index",                 UC_index          ,UC_index}
,  {CSVC_crop_N_stress_index_period_avg                        ,"crop_N",       "N_stress_index_avg",             UC_index          ,UC_index}
,  {CSVC_crop_N_uptake_accum_to_date_time                      ,"crop_N",       "N_uptake_accum",                 UC_kg_m2          ,UC_kg_m2}
,  {CSVC_crop_N_mass_canopy_current                            ,"crop_N",       "N_mass_canopy_current",          UC_kg_m2          ,UC_kg_m2}
,  {CSVC_crop_N_mass_canopy_production_accum_to_date_time      ,"crop_N",       "N_mass_canopy_production",       UC_kg_m2          ,UC_kg_m2}
,  {CSVC_crop_N_conc_canopy                                    ,"crop_N",       "N_canopy_concentration",         UC_kg_kg          ,UC_kg_kg}  // kgN/kgBM
,  {CSVC_crop_N_conc_roots_live                                ,"crop_N",       "N_root_concentration",           UC_kg_kg          ,UC_kg_kg}  // kgN/kgBM
,  {CSVC_crop_N_automatic_uptake_time_step_sum                 ,"crop_N",       "N_auto_uptake",                  UC_kg_m2          ,UC_kg_m2}
,  {CSVC_crop_N_fixation                                       ,"crop_N",       "N_fixation",                     UC_kg_m2          ,UC_kg_m2}

,  {CSVC_crop_N_conc_min                                       ,"crop_N",       "N_plant_min_concentration",      UC_kg_kg          ,UC_kg_kg}  // kgN/kgBM
,  {CSVC_crop_N_conc_max                                       ,"crop_N",       "N_plant_max_concentration",      UC_kg_kg          ,UC_kg_kg}  // kgN/kgBM
,  {CSVC_crop_N_conc_critical                                  ,"crop_N",       "N_plant_critical_concentration", UC_kg_kg          ,UC_kg_kg}  // kgN/kgBM

,  {CSVC_crop_P_mass_uptake_accum_to_date_time                 ,"crop_P",       "P_uptake_accum",                 UC_kg_m2          ,UC_kg_m2} //110801
,  {CSVC_mgmt_irrig_applied_time_step_sum                      ,"management",   "irrigation",                     UC_mm             ,UC_meter}     // This had previously been output preferred as UC_mm might not be necessary anymore and can probably be UC_m. Waiting to test this.
,  {CSVC_mgmt_biomass_unsatisfied_time_step_sum                ,"management",   "biomass_unsatisfied",            UC_kg_m2          ,UC_kg_m2}
,  {CSVC_water_table_depth                                     ,"water_table",  "water_table_depth",              UC_meter          ,UC_meter}
,  {CSVC_soil_pond_water_depth                                 ,"soil_runoff",  "pond_water",                     UC_meter          ,UC_meter}
,  {CSVC_soil_runoff_surface_water_runoff_time_step_sum        ,"soil_runoff",  "surface_water_runoff",           UC_mm             ,UC_meter}     // This had previously been output preferred as UC_mm might not be necessary anymore and can probably be UC_m. Waiting to test this.
,  {CSVC_soil_erosion_clod_rate                                ,"soil_erosion", "soil_clod_rate",                 UC_m              ,UC_m}         // Not sure about units!!!
,  {CSVC_soil_base_evap_pot_time_step_sum                      ,"soil_base",    "soil_evap_pot",                  UC_mm             ,UC_meter}     // This had previously been output preferred as UC_mm might not be necessary anymore and can probably be UC_m. Waiting to test this.
,  {CSVC_soil_base_evap_act_time_step_sum                      ,"soil_base",    "soil_evap_act",                  UC_mm             ,UC_meter}     // This had previously been output preferred as UC_mm might not be necessary anymore and can probably be UC_m. Waiting to test this.
,  {CSVC_soil_base_water_entering_time_step_sum                ,"soil_base",    "soil_water_entering",            UC_mm             ,UC_meter}     // This had previously been output preferred as UC_mm might not be necessary anymore and can probably be UC_m. Waiting to test this.
,  {CSVC_soil_base_water_drainage_time_step_sum                ,"soil_base",    "soil_water_drainage",            UC_mm             ,UC_meter}     // This had previously been output preferred as UC_mm might not be necessary anymore and can probably be UC_m. Waiting to test this.
,  {CSVC_soil_base_water_depletion_balance_accum_to_date_time  ,"soil_base",    "soil_water_depletion_balance",   UC_meter          ,UC_meter} //131007
,  {CSVC_soil_base_water_depletion_seasonal_period_sum         ,"soil_base",    "soil_water_depletion_seasonal",  UC_meter          ,UC_meter} //131007
,  {CSVC_soil_base_water_balance_accum_to_date_time            ,"soil_base",    "soil_water_balance",             UC_meter          ,UC_meter}
//190812,  {CSVC_weather_ET_pot_time_step_sum                          ,"weather",      "ET_pot",                         UC_mm             ,UC_meter}     // This had previously been output preferred as UC_mm might not be necessary anymore and can probably be UC_m. Waiting to test this.
,  {CSVC_weather_ET_max_time_step_sum                          ,"weather",      "ET_max",                         UC_mm             ,UC_meter}     // This had previously been output preferred as UC_mm might not be necessary anymore and can probably be UC_m. Waiting to test this.
,  {CSVC_weather_ET_act_time_step_sum                          ,"weather",      "ET_act",                         UC_mm             ,UC_meter}     // This had previously been output preferred as UC_mm might not be necessary anymore and can probably be UC_m. Waiting to test this.
,  {CSVC_weather_ET_ref_time_step_sum                          ,"weather",      "ET_ref",                         UC_mm             ,UC_meter}     // This had previously been output preferred as UC_mm might not be necessary anymore and can probably be UC_m. Waiting to test this.  //130723
,  {CSVC_weather_precipitation_time_step_sum                   ,"weather",      "precipitation",                  UC_mm             ,UC_meter}     // This had previously been output preferred as UC_mm might not be necessary anymore and can probably be UC_m. Waiting to test this.
,  {CSVC_weather_temperature_time_step_min                     ,"weather",      "temperature_min",                UC_Celcius        ,UC_Celcius}
,  {CSVC_weather_temperature_time_step_max                     ,"weather",      "temperature_max",                UC_Celcius        ,UC_Celcius}
,  {CSVC_weather_temperature_time_step_avg                     ,"weather",      "temperature_avg",                UC_Celcius        ,UC_Celcius} //160522
,  {CSVC_weather_solar_radiation                               ,"weather",      "solar_radiation",                UC_MJ_per_m2_solar_radiation,UC_MJ_per_m2_solar_radiation}
,  {CSVC_weather_snow_storage                                  ,"weather",      "snow_storage",                   UC_meter          ,UC_meter}
,  {CSVC_residue_pot_evap_time_step_sum                        ,"residue",      "residue_evap_pot",               UC_meter          ,UC_meter}
,  {CSVC_residue_act_evap_time_step_sum                        ,"residue",      "residue_evap_act",               UC_meter          ,UC_meter}
,  {CSVC_residue_ground_cover                                  ,"residue",      "residue_ground_cover",           UC_decimal_percent,UC_decimal_percent}
,  {CSVC_residue_water_storage                                 ,"residue",      "residue_water_storage",          UC_meter          ,UC_meter}
,  {CSVC_residue_surface_biomass                               ,"residue",      "residue_surface_BM",             UC_kg_m2          ,UC_kg_m2}
,  {CSVC_residue_plant_incorporated                            ,"residue",      "residue_straw_incorp",           UC_kg_m2          ,UC_kg_m2}
,  {CSVC_residue_manure_incorporated                           ,"residue",      "residue_manure_incorp",          UC_kg_m2          ,UC_kg_m2}
,  {CSVC_residue_water_intrcpt_time_step_sum                   ,"residue",      "residue_water_intrcpt",          UC_meter          ,UC_meter}

,  {CSVC_mgmt_N_applied_time_step_sum                          ,"management"  , "N_applied",                      UC_kg_m2          ,UC_kg_m2}  // kgN/m2
,  {CSVC_crop_N_uptake_time_step_sum                           ,"crop_N"      , "N_uptake",                       UC_kg_m2          ,UC_kg_m2}  // kgN/m2
,  {CSVC_soil_N_leached_time_step_sum                          ,"soil_N"      , "N_leached",                      UC_kg_m2          ,UC_kg_m2}  // kgN/m2
,  {CSVC_residue_mineralized_time_step_sum                     ,"residue"     , "N_straw_mineralization",         UC_kg_m2          ,UC_kg_m2}  // kgN/m2
,  {CSVC_residue_manure_mineralized_time_step_sum              ,"residue"     , "N_manure_mineralization",        UC_kg_m2          ,UC_kg_m2}  // kgN/m2
,  {CSVC_organic_matter_mineralized_time_step_sum             ,"organic_matter","N_mineralization",               UC_kg_m2          ,UC_kg_m2}  // kgN/m2
,  {CSVC_mgmt_N_organic_applied_time_step_sum                  ,"management"  , "N_OM_applied",                   UC_kg_m2          ,UC_kg_m2}  // kgN/m2
,  {CSVC_soil_N_immobilization_time_step_sum                   ,"soil_N"      , "N_immobilization",               UC_kg_m2          ,UC_kg_m2}  // kgN/m2
,  {CSVC_soil_N_immobilization_period_sum                      ,"soil_N"      , "N_immobilization_GP",            UC_kg_m2          ,UC_kg_m2}  // kgN/m2 //110918
,  {CSVC_soil_N_immobilization_accum_to_date_time              ,"soil_N"      , "N_immobilization_accum",         UC_kg_m2          ,UC_kg_m2}  // kgN/m2 //110918

,  {CSVC_soil_N_N_amount                                       ,"soil_N"      , "N_amount",                       UC_kg_m2          ,UC_kg_m2}  // kgN/m2 //181031

,  {CSVC_crop_P_mass_uptake_time_step_sum                      ,"crop_P"      , "P_uptake",                       UC_kg_m2          ,UC_kg_m2}  // kgN/m2 // 110801

//  The following are accumulations
,  {CSVC_weather_ET_max_accum_to_date_time                     ,"weather"     , "ET_max_accum",                   UC_m           ,UC_m}
//190812 ,  {CSVC_weather_ET_pot_accum_to_date_time                     ,"weather"     , "ET_pot_accum",                   UC_m           ,UC_m}
,  {CSVC_weather_ET_act_accum_to_date_time                     ,"weather"     , "ET_act_accum",                   UC_m           ,UC_m}
,  {CSVC_weather_ET_ref_accum_to_date_time                     ,"weather"     , "ET_ref_accum",                   UC_m           ,UC_m}   //130723
,  {CSVC_weather_precipitation_accum_to_date_time              ,"weather"     , "precipitation_accum",            UC_m           ,UC_m}
,  {CSVC_crop_base_transp_pot_accum_to_date_time               ,"crop_base"   , "transp_pot_accum",               UC_m           ,UC_m}
,  {CSVC_crop_base_transp_act_accum_to_date_time               ,"crop_base"   , "transp_act_accum",               UC_m           ,UC_m}
,  {CSVC_crop_base_water_intrcpt_accum_to_date_time            ,"crop_base"   , "crop_water_intrcpt_accum",       UC_m           ,UC_m}
,  {CSVC_soil_base_evap_pot_accum_to_date_time                 ,"soil_base"   , "soil_evap_pot_accum",            UC_m           ,UC_m}
,  {CSVC_soil_base_evap_act_accum_to_date_time                 ,"soil_base"   , "soil_evap_act_accum",            UC_m           ,UC_m}
,  {CSVC_soil_base_water_entering_accum_to_date_time           ,"soil_base"   , "soil_water_entering_accum",      UC_m           ,UC_m}
,  {CSVC_soil_base_water_drainage_accum_to_date_time           ,"soil_base"   , "soil_water_drainage_accum",      UC_m           ,UC_m}
,  {CSVC_residue_pot_evap_accum_to_date_time                   ,"residue"     , "residue_evap_pot_accum",         UC_m           ,UC_m}
,  {CSVC_residue_act_evap_accum_to_date_time                   ,"residue"     , "residue_evap_act_accum",         UC_m           ,UC_m}
,  {CSVC_residue_water_intrcpt_accum_to_date_time              ,"residue"     , "residue_water_intrcpt_accum",    UC_m           ,UC_m}
,  {CSVC_mgmt_irrig_applied_accum_to_date_time                 ,"management"  , "irrigation_accum",               UC_m           ,UC_m}
,  {CSVC_soil_runoff_surface_water_runoff_accum_to_date_time   ,"soil_runoff" , "surface_water_runoff_accum",     UC_m           ,UC_m}
,  {CSVC_mgmt_N_applied_accum_to_date_time                     ,"management"  , "N_applied_accum",                UC_kg_m2          ,UC_kg_m2}  // kgN/m2
,  {CSVC_soil_N_leached_accum_to_date_time                     ,"soil_N"      , "N_leached_accum",                UC_kg_m2          ,UC_kg_m2}  // kgN/m2
,  {CSVC_organic_matter_mineralized_accum_to_date_time         ,"organic_matter", "N_mineralization_accum",         UC_kg_m2          ,UC_kg_m2}  // kgN/m2
,  {CSVC_organic_matter_mineralized_period_sum                 ,"organic_matter", "N_mineralization_GP",            UC_kg_m2          ,UC_kg_m2}  // kgN/m2  //110918
,  {CSVC_mgmt_N_inorganic_gaseous_loss_accum_to_date_time      ,"management"  , "N_inorganic_gaseous_loss_accum", UC_kg_m2          ,UC_kg_m2}  // kgN/m2   //110918
,  {CSVC_mgmt_N_inorganic_gaseous_loss_period_sum              ,"management"  , "N_inorganic_gaseous_loss_GP",    UC_kg_m2          ,UC_kg_m2}  // kgN/m2   //110918

,  {CSVC_residue_mineralized_accum_to_date_time                ,"residue"     , "N_straw_mineralization_accum",   UC_kg_m2          ,UC_kg_m2}  // kgN/m2
,  {CSVC_residue_manure_mineralized_accum_to_date_time         ,"residue"     , "N_manure_mineralization_accum",  UC_kg_m2          ,UC_kg_m2}  // kgN/m2
,  {CSVC_mgmt_N_organic_applied_accum_to_date_time             ,"management"  , "N_OM_applied_accum",             UC_kg_m2          ,UC_kg_m2}  // kgN/m2
,  {CSVC_crop_N_automatic_uptake_accum_to_date_time            ,"crop_N"      , "N_auto_uptake_accum",            UC_kg_m2          ,UC_kg_m2}  // kgN/m2
,  {CSVC_crop_N_fixation_time_step_sum                         ,"crop_N"      , "N_fixation_accum",               UC_kg_m2          ,UC_kg_m2}  // kgN/m2
,  {CSVC_organic_matter_CO2_C_loss_OM_time_step_sum            ,"organic_matter"          , "OM_gaseous_CO2_C_loss",          UC_kg_m2          ,UC_kg_m2}  // kgC/m2

,  {CSVC_crop_WUE                            ,"crop_base","WUE"               ,UC_kg_kg      ,UC_kg_kg}     //160522
,  {CSVC_crop_degree_hours_above_30          ,"crop_base","DegHr_above_30"    ,UC_Celcius_degree_hours,UC_Celcius_degree_hours}  //160522
,  {CSVC_crop_degree_hours_above_34          ,"crop_base","DegHr_above_34"    ,UC_Celcius_degree_hours,UC_Celcius_degree_hours}  //160522

};

#define daily_profile_variable_fields_count 14
Variable_field_registration daily_profile_variable_fields_registration [daily_profile_variable_fields_count] =
{ // {var_code,"section_name","field_name",preferred_units,stored_units}
   {CSVC_soil_base_water_content             ,"soil_base","WC"                ,UC_m3_m3      ,UC_m3_m3}
,  {CSVC_soil_base_plant_avail_water         ,"soil_base","PAW"               ,UC_decimal_percent,UC_decimal_percent}
,  {CSVC_soil_base_temperature_time_step_avg ,"soil_temperature_profile","soil_temperature"  ,UC_Celcius,UC_Celcius}
,  {CSVC_soil_N_N_amount                     ,"soil_chemical_profile"   ,"N_amount"          ,UC_kg_ha      ,UC_kg_m2}     // preferred was originally UC_kg_ha, I think I can simply output UC_kg_m2 but need to check fastgraph
,  {CSVC_soil_base_water_potential           ,"soil_base","water_potential"   ,UC_J_kg       ,UC_J_kg}
,  {CSVC_soil_N_NO3_amount                   ,"soil_chemical_profile"   ,"N_NO3"             ,UC_kg_ha      ,UC_kg_m2}     // preferred was originally UC_kg_ha, I think I can simply output UC_kg_m2 but need to check fastgraph
,  {CSVC_soil_N_NH4_amount                   ,"soil_chemical_profile"   ,"N_NH4"             ,UC_kg_ha      ,UC_kg_m2}     // preferred was originally UC_kg_ha, I think I can simply output UC_kg_m2 but need to check fastgraph
,  {CSVC_soil_N_denitrification_time_step_sum,"soil_chemical_profile"   ,"N_denitrification" ,UC_kg_ha      ,UC_kg_m2}     // preferred was originally UC_kg_ha, I think I can simply output UC_kg_m2 but need to check fastgraph
,  {CSVC_soil_N_nitrification_time_step_sum  ,"soil_chemical_profile"   ,"N_nitrification"   ,UC_kg_ha      ,UC_kg_m2}     // preferred was originally UC_kg_ha, I think I can simply output UC_kg_m2 but need to check fastgraph
,  {CSVC_soil_N_mineralization_time_step_sum ,"soil_chemical_profile"   ,"N_mineralization"  ,UC_kg_ha      ,UC_kg_m2}     // preferred was originally UC_kg_ha, I think I can simply output UC_kg_m2 but need to check fastgraph
,  {CSVC_residue_plant_incorporated          ,"residue_profile"  ,"plant_incorp"      ,UC_kg_ha      ,UC_kg_m2}     // preferred was originally UC_kg_ha, I think I can simply output UC_kg_m2 but need to check fastgraph
,  {CSVC_residue_manure_incorporated         ,"residue_profile"  ,"manure_incorp"     ,UC_kg_ha      ,UC_kg_m2}    // preferred was originally UC_kg_ha, I think I can simply output UC_kg_m2 but need to check fastgraph
,  {CSVC_soil_base_salinity                  ,"soil_chemical_profile","salinity"          ,UC_dS_m       ,UC_dS_m}
//NYI,  {CSVC_organic_matter_percent_time_step_sum,'organic_matter','field_name',preferred_units,stored_units}
,  {CSVC_crop_base_root_fraction             ,"crop_base","root_fraction"     ,UC_fraction   ,UC_fraction}
};
//_daily_variable_fields_registration__________________________________________/
CS_UED_tuple_data_source::CS_UED_tuple_data_source(UED::Database_file_indexed *database_ )
: UED::Tuple_data_source
   (database_
   ,UED::Record_type_distinguisher_interface::UED_data_set_year_var_units
   ,UT_day,simulated_quality)
{}
//_constructor_________________________________________________________________/
CS_daily_data_record::CS_daily_data_record
(CropSyst::Land_unit_simulation &_land_unit_sim                                  //141206
//190121 now using optation_global, const CS::Desired_variables &_desired_vars                                    //170225
)
: CORN::Data_record("LBF")
, land_unit_sim(_land_unit_sim)
//190121 , desired_vars(_desired_vars)
, crop_act_root_biomass(0)
, crop_water_stress_stats(1.0)      //180409
, crop_temperature_stress_stats(1.0) //180410 (0)
, crop_growing_degree_days(0)
, crop_seasonal_thermal_time(0)
, crop_dry_biomass_kg_m2(0)
, crop_yield_biomass_kg_m2(0) //for AgMIP
, crop_live_biomass_kg_m2(0)
, crop_fruit_biomass_kg_m2(0)
, crop_fruit_canopy_biomass_kg_m2(0)
, crop_leaf_area_index(0)
, crop_green_area_index(0)
, crop_root_depth_m(0)
, crop_water_stress_index(0)
, crop_temperature_stress_index(0)
, crop_VPD_daytime(0)
, crop_intercepted_PAR(0)
, crop_intercepted_PAR_accum(0)
, crop_canopy_cover_green(0)
, crop_canopy_cover_total(0)
, crop_leaf_water_pot(0)
, residue_ground_cover(0)
, residue_water_storage_m(0)
, residue_surface_biomass_kg_m2(0)
, residue_plant_incorporated_kg_m2(0)
, residue_manure_incorporated_kg_m2(0)
, soil_clod_rate(0)
, ET_max_m(0)
//190812, ET_pot_m(0)
, ET_act_m(0)
, ET_ref_m(0)                                                                    //130723
, crop_transp_pot_m(0)
, crop_transp_act_m(0)
, soil_evap_pot_m(0)
, soil_evap_act_m(0)
, residue_evap_pot_m(0)
, residue_evap_act_m(0)
, management_irrigation_m(0)
, biomass_unsatisfied_kg_m2(0)

//NYI here I could output biomass_grazed

, weather_precipitation_m(0)
, weather_temperature_max(0)
, weather_temperature_min(0)
, weather_solar_rad_MJ_m2(0)
, crop_water_inter_m(0)
, residue_water_inter_m(0)
, soil_surface_water_runoff_m(0)
, soil_water_entering_m(0)
, soil_water_drainage_m(0)
               // eventually we will have a water balance report which will use balance_drainage
               // we will restore here the reported drainage.
, weather_snow_storage_m(0)
, soil_water_depletion_balance_m(0)                                              //131007
, soil_water_depletion_seasonal_m(0)                                             //131007
, soil_water_balance_m(0)
#ifdef NITROGEN
, nitrogen_applied_kg_m2(0)
, nitrogen_uptake_kg_m2(0)
, nitrogen_leached_kg_m2(0)
, nitrogen_amount_kg_m2(0)                                                       //181031
, residue_mineralizated_straw_kg_m2(0)
, residue_mineralizated_manure_kg_m2(0)
, OM_mineralization_orig_kg_m2(0)                                                //110918
, OM_mineralization_new_kg_m2(0)                                                 //110918
, nitrogen_organic_N_applied_kg_m2(0)
, nitrogen_N_immobilization_kg_m2(0)
#endif
#ifdef NITROGEN
, nitrogen_stress_factor(1.0)                                                    //200529
, nitrogen_stress_index(0)
, nitrogen_avg_stress_index(0)
, nitrogen_uptake_accum_kg_m2(0)
, nitrogen_mass_canopy_current_kg_m2(0)                                          //131007
, nitrogen_mass_canopy_production_kg_m2(0)                                       //131007
, nitrogen_plant_N_concentration_deprecated(0)
, nitrogen_canopy_concentration(0)
, nitrogen_root_concentration(0)
, nitrogen_automatic_N_in_uptake_kg_m2(0)
, nitrogen_fixation_kg_m2(0)
, nitrogen_plant_N_concentration_min(0)
, nitrogen_plant_N_concentration_max(0)
, nitrogen_plant_N_concentration_critical(0)
#endif
#ifdef PHOSPHORUS
, phosphorus_applied_kg_m2(0)                                                    //110801
, phosphorus_uptake_kg_m2(0)                                                     //110801
, phosphorus_applied_accum_kg_m2(0)                                              //110801
, phosphorus_uptake_accum_kg_m2(0)                                               //110801
#endif

// growth period accumulation
, ET_pot_accum_GP_m(0)
, ET_act_accum_GP_m(0)
, ET_ref_accum_GP_m(0)                                                           //130723
, crop_transp_pot_accum_GP_m(0)
, crop_transp_act_accum_GP_m(0)
, soil_evap_pot_accum_GP_m(0)
, soil_evap_act_accum_GP_m(0)
, residue_evap_pot_accum_GP_m(0)
, residue_evap_act_accum_GP_m(0)
, management_irrig_accum_GP_m(0)
, weather_precip_accum_GP_m(0)
, crop_water_intrcpt_accum_GP_m(0)
, residue_water_intrcpt_accum_GP_m(0)
, surface_water_runoff_accum_GP_m(0)
, ponded_water_GP_m(0)
, soil_water_enter_accum_GP_m(0)
, soil_water_drain_accum_GP_m(0)
  // eventually we will have a water balance report which will use balance_drainage
  // we will restore here the reported drainage.
#ifdef NITROGEN
, nitrogen_applied_accum_GP_kg_m2(0)
, nitrogen_leached_accum_GP_kg_m2(0)
, mineralizated_residue_accum_GP_kg_m2(0)
, mineralizated_manure_accum_GP_kg_m2(0)
, OM_mineralization_orig_accum_GP_kg_m2(0)
, OM_mineralization_new_accum_GP_kg_m2(0)
, N_immobilization_accum_GP_kg_m2(0)
, automatic_N_in_uptake_accum_GP_kg_m2(0)
, organic_N_applied_accum_GP_kg_m2(0)
, N_Fixation_accum_GP_kg_m2(0)
, N_inorganic_gaseous_loss_GP(0)                                                 //110918
#endif
, OM_gaseous_CO2_C_loss(0)                                                       //160516
// continuous is the default mode case CONTINUOUS_ACCUM :
, ET_max_accum_cont_m(0)                                                         //190812
, ET_act_accum_cont_m(0)
, ET_ref_accum_cont_m(0)                                                         //130723
, crop_transp_pot_accum_cont_m(0)
, crop_transp_act_accum_cont_m(0)
, soil_evap_pot_accum_cont_m(0)
, soil_evap_act_accum_cont_m(0)
, residue_evap_pot_accum_cont_m(0)
, residue_evap_act_accum_cont_m(0)
, management_irrig_accum_cont_m(0)
, management_biomass_unsatisfied_accum_cont_m(0)
, weather_precip_accum_cont_m(0)
, crop_water_intrcpt_accum_cont_m(0)                                             //160419
, residue_water_intrcpt_accum_cont_m(0)
, surface_water_runoff_accum_cont_m(0)
, ponded_water_cont_m(0)
, water_table_depth_m(0)
, soil_water_enter_accum_cont_m(0)
, soil_water_drain_accum_cont_m(0)
#ifdef NITROGEN
, nitrogen_applied_accum_cont_kg_m2(0)
, nitrogen_leached_accum_cont_kg_m2(0)
, OM_mineralization_orig_accum_cont_kg_m2(0)
, OM_mineralization_new_accum_cont_kg_m2(0)
, mineralizated_residue_accum_cont_kg_m2(0)
, mineralizated_manure_accum_cont_kg_m2(0)
, N_immobilization_accum_cont_kg_m2(0)                                           //110918
, automatic_N_in_uptake_accum_cont_kg_m2(0)
, organic_N_applied_accum_cont_kg_m2(0)
, N_inorganic_gaseous_loss_cont(0)                                               //110918
, N_Fixation_accum_cont_kg_m2(0)

// contributes
, last_NO3_applied_E  (0)
, last_NH4_applied_E  (0)
, last_NO3_uptake_E   (0)
, last_NH4_uptake_E   (0)
, last_NO3_leached_E  (0)
, last_NH4_leached_E  (0)
#endif
, WUE                                                                        (0) //160521
, DegHr_above_30                                                             (0) //160521
, DegHr_above_34                                                             (0) //160521
{}
//_constructor_________________________________________________________________/
bool CS_daily_data_record::expect_structure(bool for_write)
{
   structure_defined = false;                                                    //120314
   CS::Optation_temporal &optation_day = CS::optation_global.provide_temporal(UT_day); //190119

   CropSyst::Soil_components *soil = land_unit_sim.mod_soil();                   //150411_060803
   #ifdef NITROGEN
   Soil::NO3_Profile *NO3 = (soil && soil->chemicals) ? soil->chemicals->NO3 : 0;
   Soil::NH4_Profile *NH4 = (soil && soil->chemicals) ? soil->chemicals->NH4 : 0;
   Chemical_balance_accumulators_molecular *NO3_CONT_bal_accums = NO3 ? NO3->CONT_bal_accums_M : 0;
   Chemical_balance_accumulators_molecular *NH4_CONT_bal_accums = NH4 ? NH4->CONT_bal_accums_M : 0;
   #endif

   Organic_matter_residues_profile_abstract *organic_matter_residues_profile_kg_m2= land_unit_sim.organic_matter_residues_profile_kg_m2;
   const Residues_interface *residue_pools_kg_m2 = organic_matter_residues_profile_kg_m2 ? organic_matter_residues_profile_kg_m2->ref_residues() : 0;

   Period_accumulators *GP_accumulators = 0;
   Annual_accumulators *ANNL_accumulators = 0;
   Period_accumulators *CONTINUOUS_accumulators= 0;

   GP_accumulators = land_unit_sim.GP_accumulators_active;
   ANNL_accumulators = land_unit_sim.ANNL_accumulators_active;
   #if (defined (CROPSYST_PROPER))
   CONTINUOUS_accumulators= land_unit_sim.CONTINUOUS_accumulators_active;
   #endif
   if (soil ) soil->setup_structure(*this,for_write);                            //190119
   CropSyst::Soil_runoff_common *runoff =                                        //200316
      dynamic_cast<CropSyst::Soil_runoff_common *>(land_unit_sim.runoff);        //130308
   #if (CROPSYST_VERSION==4)
   // May want this in V5
   if (runoff)    runoff   ->setup_structure(*this,for_write);                   //190119_130308
   #endif
   #if (CROPSYST_PROPER==4)
   CropSyst_ET *ET = dynamic_cast<CropSyst_ET *>(land_unit_sim.ET_ref);          //180805
      // weather must be defined before soil because soil initialization requires weather info!
   if (ET   ) ET  ->setup_structure(*this,for_write);                            //190119
   #endif
   set_current_section("management");
   #ifdef USE_VAR_CODES
   if (optation_day.is_desired(CSVC_mgmt_irrig_applied_time_step_sum))		   expect_float64("irrigation",land_unit_sim.total_irrigation_today);
   if (optation_day.is_desired(CSVC_mgmt_biomass_unsatisfied_time_step_sum))	expect_float64("biomass_unsatisfied",land_unit_sim.unsatisfied_harvest_BM_today_kg_m2);
   #endif
//NYI Not sure where I stored the biomass_grazed, this wasn't completed from when I originally added the grazing model, so I am not sure if it is still needed.
//NYI /*040225_*/    if (desired_vars.is_desired(CSVC_mgmt_biomass_grazed_time_step_sum))	   data_rec.expect_float64("biomass_grazed",land_unit_sim.xxxxx_biomass_grazed);

   crop_act_root_biomass               = 0.0;
   crop_water_stress_stats             = 1.0; //180409 0.0;
   crop_temperature_stress_stats       = 1.0; //180410 0.0;
   crop_growing_degree_days            = 0.0;
   crop_seasonal_thermal_time          = 0.0;
   crop_dry_biomass_kg_m2              = 0.0;
   crop_yield_biomass_kg_m2            = 0.0; //181030 AgMIP
   crop_live_biomass_kg_m2             = 0.0;
   crop_fruit_biomass_kg_m2            = 0.0;
   crop_fruit_canopy_biomass_kg_m2     = 0.0;
   crop_leaf_area_index                = 0.0;
   crop_green_area_index               = 0.0;
   crop_root_depth_m                   = 0.0;
   crop_water_stress_index        = 0.0;
   crop_temperature_stress_index       = 0.0;
   crop_VPD_daytime                    = 0.0;
   crop_intercepted_PAR                = 0.0;
   crop_intercepted_PAR_accum          = 0.0;
   crop_canopy_cover_green            = 0.0;
   crop_canopy_cover_total             = 0.0;
   crop_leaf_water_pot                 =0.0;

   crop_transp_pot_m                   = 0.0;
   crop_transp_act_m                   = 0.0;
   crop_water_inter_m                  = 0.0;

   nitrogen_stress_factor              = 1.0;                                    //200529
   nitrogen_stress_index               = 0.0;
   nitrogen_avg_stress_index           = 0.0;
   nitrogen_uptake_accum_kg_m2         = 0.0;
   nitrogen_mass_canopy_current_kg_m2  = 0.0;                                    //131007
   nitrogen_mass_canopy_production_kg_m2  = 0.0;                                 //131007_110915
   nitrogen_canopy_concentration       = 0.0;
   nitrogen_root_concentration         = 0.0;
   nitrogen_automatic_N_in_uptake_kg_m2= 0.0;
   nitrogen_fixation_kg_m2             = 0.0;
   nitrogen_plant_N_concentration_min  = 0.0;
   nitrogen_plant_N_concentration_max  = 0.0;
   nitrogen_plant_N_concentration_critical= 0.0;
   #ifdef PHOSPHORUS
      phosphorus_applied_kg_m2            = 0.0;
      phosphorus_uptake_kg_m2             = 0.0;
   #endif

   #ifdef USE_VAR_CODES
   {  set_current_section("crop_base");

      if (optation_day.is_desired(CSVC_crop_base_root_biomass_current))             { expect_float32("rootbiomass",crop_act_root_biomass);}  // was CSVC_crop_base_root_biomass
      if (optation_day.is_desired(CSVC_crop_base_water_stress_factor))              { expect_float32("waterstress",crop_water_stress_stats);}
      if (optation_day.is_desired(CSVC_crop_base_temperature_stress_factor))        { expect_float32("tempstress",crop_temperature_stress_stats);}
      if (optation_day.is_desired(CSVC_crop_base_growing_degree_days_period_accum)) { expect_float32("degdays",crop_growing_degree_days);}
      if (optation_day.is_desired(CSVC_crop_base_seasonal_thermal_time_period_sum)) { expect_float32("thermal_time",crop_seasonal_thermal_time);}
      if (optation_day.is_desired(CSVC_crop_biomass_canopy))                        { expect_float32("canopy_BM",crop_dry_biomass_kg_m2 );} // 070207 CSVC_crop_base_dry_biomass
      if (optation_day.is_desired(CSVC_crop_biomass_yield))                         { expect_float32("yield_BM",crop_yield_biomass_kg_m2);} //181031
      if (optation_day.is_desired(CSVC_crop_base_green_biomass ))                   { expect_float32("canopy_live_BM",crop_live_biomass_kg_m2);}  // was  /*070206 CSVC_crop_base_live_biomass*/
   //done above I think we need to output the act root biomass /*041021_*/    if (desired_vars.is_desired(CSVC_crop_base_root_biomass)                      { expect_float32("rootbiomass",crop_root_biomass_kg_m2);}
      if (optation_day.is_desired(CSVC_crop_base_fruit_biomass))                 { expect_float32("fruit_BM",crop_fruit_biomass_kg_m2);}                     //041021
      if (optation_day.is_desired(CSVC_crop_base_fruit_canopy_biomass))          { expect_float32("non_fruit_canopy_BM",crop_fruit_canopy_biomass_kg_m2);}   //041021
      if (optation_day.is_desired(CSVC_crop_base_fruit_reserves_biomass))        { expect_float32("reserves_BM",crop_fruit_reserves_biomass_kg_m2);}   //200608

      if (optation_day.is_desired(CSVC_crop_base_leaf_area_index))               { expect_float32("LAI",crop_leaf_area_index);}                                    //041021
      if (optation_day.is_desired(CSVC_crop_base_green_area_index))              { expect_float32("GAI",crop_green_area_index);}  // 060921 was only vital         //041021
      if (optation_day.is_desired(CSVC_crop_base_root_depth))                    { expect_float32("root_depth",crop_root_depth_m);}                       //061005
      if (optation_day.is_desired(CSVC_crop_base_water_stress_index))            { expect_float32("water_stress_index",crop_water_stress_index);}             //041021
      if (optation_day.is_desired(CSVC_crop_base_temperature_stress_index))      { expect_float32("temperature_stress_index",crop_temperature_stress_index);}      //041021

      //NYI    virtual float64 get_tuber_init_temperature_stress_index()   const = 0;  //081111
      //NYI    virtual float64 get_tuber_fill_temperature_stress_index()   const = 0;  //081111

      if (optation_day.is_desired(CSVC_crop_base_VPD_daytime))                   { expect_float32("VPD_daytime",crop_VPD_daytime);}
      if (optation_day.is_desired(CSVC_crop_base_intercepted_PAR))               { expect_float32("intercepted_PAR",crop_intercepted_PAR);}
      if (optation_day.is_desired(CSVC_crop_base_intercepted_PAR_accum_to_date_time))                  { expect_float32("intercepted_PAR_accum",crop_intercepted_PAR_accum);}
      if (optation_day.is_desired(CSVC_crop_base_fract_canopy_cover))
                                                                                 { expect_float32("canopy_cover_green",crop_canopy_cover_green);} //200127
      if (optation_day.is_desired(CSVC_crop_base_fract_canopy_cover_total))
                                                                                 { expect_float32("canopy_cover_total",crop_canopy_cover_total);} //200213


      if (optation_day.is_desired(CSVC_crop_base_leaf_water_pot    ))
                                                                                 { expect_float32("leaf_water_pot",crop_leaf_water_pot);}
//NO daily soil_loss/*041021_*/    soil_loss                           : num_value= 0.0;                    break; //{over_m2_to_ha * daily_RUSLE.soil_loss not currently availabled}
     if (optation_day.is_desired(CSVC_crop_base_transp_pot_time_step_sum))       { expect_float32("transp_pot",crop_transp_pot_m);}
     if (optation_day.is_desired(CSVC_crop_base_transp_act_time_step_sum))       { expect_float32("transp_act",crop_transp_act_m);}
     if (optation_day.is_desired(CSVC_crop_base_water_intrcpt_time_step_sum))    { expect_float32("crop_water_intrcpt",crop_water_inter_m);}
               //NYI here I could output biomass_grazed
     #ifdef NITROGEN

      if (land_unit_sim.scenario_control_and_model_options.run_nitrogen)          //130827_120426
      {
      set_current_section("crop_N");
      if (optation_day.is_desired(CSVC_crop_N_stress_factor))                    { expect_float32("N_stress",nitrogen_stress_factor);}
      if (optation_day.is_desired(CSVC_crop_N_stress_index))                     { expect_float32("N_stress_index",nitrogen_stress_index);}
      if (optation_day.is_desired(CSVC_crop_N_stress_index_period_avg))          { expect_float32("N_stress_index_avg",nitrogen_avg_stress_index);}
      if (optation_day.is_desired(CSVC_crop_N_uptake_accum_to_date_time))        { expect_float32("N_uptake_accum",nitrogen_uptake_accum_kg_m2);}
      if (optation_day.is_desired(CSVC_crop_N_mass_canopy_production_accum_to_date_time))    { expect_float32("N_mass_canopy_production",nitrogen_mass_canopy_production_kg_m2);} //131007_110915
      if (optation_day.is_desired(CSVC_crop_N_mass_canopy_current))                          { expect_float32("N_mass_canopy_current",nitrogen_mass_canopy_current_kg_m2);} //131007
      if (optation_day.is_desired(CSVC_crop_N_conc_canopy))                     { expect_float32("N_canopy_concentration",nitrogen_canopy_concentration );}
      if (optation_day.is_desired(CSVC_crop_N_conc_roots_live))                 { expect_float32("N_root_concentration",nitrogen_root_concentration );}
      if (optation_day.is_desired(CSVC_crop_N_automatic_uptake_time_step_sum))  { expect_float32("N_auto_uptake",nitrogen_automatic_N_in_uptake_kg_m2);}
      if (optation_day.is_desired(CSVC_crop_N_fixation))                        { expect_float32("N_fixation",nitrogen_fixation_kg_m2);}

      if (optation_day.is_desired(CSVC_crop_N_conc_min))                        { expect_float32("N_plant_min_concentration",nitrogen_plant_N_concentration_min);}
      if (optation_day.is_desired(CSVC_crop_N_conc_max))                        { expect_float32("N_plant_max_concentration",nitrogen_plant_N_concentration_max);}
      if (optation_day.is_desired(CSVC_crop_N_conc_critical))                   { expect_float32("N_plant_critical_concentration",nitrogen_plant_N_concentration_critical);}
      }
      #endif
   #ifdef PHOSPHORUS
   set_current_section("crop_P");
   if (optation_day.is_desired(CSVC_crop_P_mass_uptake_accum_to_date_time))
   {
      expect_float32("P_uptake_accum",phosphorus_uptake_accum_kg_m2);}
   #endif
   }
   { // Simulation
   set_current_section("management");
   if (optation_day.is_desired(CSVC_mgmt_irrig_applied_time_step_sum))           { expect_float32("irrigation",management_irrigation_m);}
   if (optation_day.is_desired(CSVC_mgmt_biomass_unsatisfied_time_step_sum))     { expect_float32("biomass_unsatisfield",biomass_unsatisfied_kg_m2);}
   set_current_section("soil_runoff");
   if (optation_day.is_desired(CSVC_soil_pond_water_depth))                      { expect_float32("pond_water",ponded_water_cont_m);}
   }
   if (soil)
   {
   #ifdef RUNOFF
   set_current_section("soil_runoff");
   if (optation_day.is_desired(CSVC_soil_runoff_surface_water_runoff_time_step_sum)) { expect_float32("surface_water_runoff",soil_surface_water_runoff_m);}
   #endif
   #ifdef EROSION
   set_current_section("soil_erosion");
   if (optation_day.is_desired(CSVC_soil_erosion_clod_rate))                     expect_float32("soil_clod_rate",soil_clod_rate);
   #endif
   set_current_section("soil_base");
   if (optation_day.is_desired(CSVC_soil_base_evap_pot_time_step_sum))           expect_float32("soil_evap_pot",soil_evap_pot_m);
   if (optation_day.is_desired(CSVC_soil_base_evap_act_time_step_sum))           expect_float32("soil_evap_act",soil_evap_act_m);
   if (optation_day.is_desired(CSVC_soil_base_water_entering_time_step_sum))     expect_float32("soil_water_entering",soil_water_entering_m);
   if (optation_day.is_desired(CSVC_soil_base_water_drainage_time_step_sum))     expect_float32("soil_water_drainage",soil_water_drainage_m);
   }
   //160731if (ET)
   {  set_current_section("weather");
      if (optation_day.is_desired(CSVC_weather_ET_max_time_step_sum))            expect_float32("ET_max",ET_max_m);
      //190812 if (optation_day.is_desired(CSVC_weather_ET_pot_time_step_sum))            expect_float32("ET_pot",ET_pot_m);
      if (optation_day.is_desired(CSVC_weather_ET_act_time_step_sum))            expect_float32("ET_act",ET_act_m);
      /* if (optation_day.is_desired(CSVC_weather_ET_ref_time_step_sum)) */      expect_float32("ET_ref",ET_ref_m); //130723

   }
   { // weather
      set_current_section("weather");
      if (optation_day.is_desired(CSVC_weather_precipitation_time_step_sum))     expect_float32("precipitation",weather_precipitation_m);
      if (optation_day.is_desired(CSVC_weather_snow_storage))                    expect_float32("snow_storage",weather_snow_storage_m);
      if (optation_day.is_desired(CSVC_weather_temperature_time_step_max))       expect_float32("temperature_max",weather_temperature_max);
      if (optation_day.is_desired(CSVC_weather_temperature_time_step_min))       expect_float32("temperature_min",weather_temperature_min);
      if (optation_day.is_desired(CSVC_weather_solar_radiation))                 expect_float32("solar_radiation",weather_solar_rad_MJ_m2);
   }

   set_current_section("water_table");
   if (optation_day.is_desired(CSVC_water_table_depth))                          expect_float32("water_table_depth",water_table_depth_m);

   if (residue_pools_kg_m2)
   { // Residue
      set_current_section("residue");
      if (optation_day.is_desired(CSVC_residue_pot_evap_time_step_sum))          { expect_float32("residue_evap_pot",residue_evap_pot_m);}
      if (optation_day.is_desired(CSVC_residue_act_evap_time_step_sum))          { expect_float32("residue_evap_act",residue_evap_act_m);}
      if (optation_day.is_desired(CSVC_residue_ground_cover))                    { expect_float32("residue_ground_cover",residue_ground_cover);}
      if (optation_day.is_desired(CSVC_residue_water_storage))                   { expect_float32("residue_water_storage",residue_water_storage_m);}
      if (optation_day.is_desired(CSVC_residue_surface_biomass))                 { expect_float32("residue_surface_BM",residue_surface_biomass_kg_m2);}
      #ifdef TEMP_DISAB
      if (optation_day.is_desired(CSVC_residue_plant_incorporated))              { expect_float32("residue_straw_incorp",residue_plant_incorporated_kg_m2);}
      #endif
      if (optation_day.is_desired(CSVC_residue_manure_incorporated))             { expect_float32("residue_manure_incorp",residue_manure_incorporated_kg_m2);}
      if (optation_day.is_desired(CSVC_residue_water_intrcpt_time_step_sum))     { expect_float32("residue_water_intrcpt",residue_water_inter_m);}
   }
   #ifdef NITROGEN
   if (NO3_CONT_bal_accums && NH4_CONT_bal_accums)
   {  if (optation_day.is_desired(CSVC_mgmt_N_applied_time_step_sum))
      {  set_current_section("management");
         expect_float32("N_applied",nitrogen_applied_kg_m2);
      }
      if (optation_day.is_desired(CSVC_crop_N_uptake_time_step_sum))
      {  set_current_section("crop_N");
         expect_float32("N_uptake",nitrogen_uptake_kg_m2);
      }
      if (optation_day.is_desired(CSVC_soil_N_leached_time_step_sum))
      {  set_current_section("soil_N");
         expect_float32("N_leached",nitrogen_leached_kg_m2);
      }
      set_current_section("residue");
      if (organic_matter_residues_profile_kg_m2)                                 //060628
      {
         expect_float32("N_straw_mineralization",residue_mineralizated_straw_kg_m2);
         expect_float32("N_manure_mineralization",residue_mineralizated_manure_kg_m2);//041021
         {
            {
               set_current_section("organic_matter");                            //110918_041031
               expect_float32("N_mineralization",OM_mineralization_new_kg_m2);   //110918_041021
            }
         }
      }
      set_current_section("management");                                         //041031
      if (optation_day.is_desired(CSVC_mgmt_N_organic_applied_time_step_sum))
      {
         expect_float32("N_OM_applied",nitrogen_organic_N_applied_kg_m2);}       //041021
      set_current_section("soil_N");                                             //041031
      if (optation_day.is_desired(CSVC_soil_N_immobilization_time_step_sum))
      {
         expect_float32("N_immobilization",nitrogen_N_immobilization_kg_m2);}   //041021
   }
   if (optation_day.is_desired(CSVC_soil_N_N_amount))                            //181031
      {  set_current_section("soil_N");
         expect_float32("N_amount",nitrogen_amount_kg_m2);
      }

   #endif

   #ifdef PHOSPHORUS

   #endif
   set_current_section("soil_base");
   if (optation_day.is_desired(CSVC_soil_base_water_depletion_balance_accum_to_date_time)){ expect_float32("soil_water_depletion_balance",soil_water_depletion_balance_m);}
   if (optation_day.is_desired(CSVC_soil_base_water_depletion_seasonal_period_sum))       { expect_float32("soil_water_depletion_seasonal",soil_water_depletion_seasonal_m);}
   //131007 if (optation_day.is_desired(CSVC_soil_base_water_depletion_accum_to_date_time)) { expect_float32("soil_water_depletion",soil_water_depletion_m);}
   if (optation_day.is_desired(CSVC_soil_base_water_balance_accum_to_date_time))   { expect_float32("soil_water_balance",soil_water_balance_m);}
#ifdef KEEP
041022 Currently only outputting annual accumulation
In version 3 we had an option for growth period or annual accumulation in the daily report.
In version 4 these accumulated variables are temporary, they will be replaced by
accumulation performed by the report generator,
(version 4 has not had the option to select growth period accumulation, so I am only outputting the annual accumulation for now.
// growth period accumulation

would need to set current section()

   if (optation_day.is_desired(CSVC_weather_ET_pot_accum_to_date_time))        { expect_float32(x"xxxxxx",ET_pot_accum_GP_m);}
   if (optation_day.is_desired(CSVC_weather_ET_act_accum_to_date_time))        { expect_float32(x"xxxxxx",xxxxxxxx);}
   if (optation_day.is_desired(CSVC_weather_ET_ref_accum_to_date_time))        { expect_float32(x"xxxxxx",xxxxxxxx);} //130723
   if (optation_day.is_desired(CSVC_crop_base_transp_pot_accum_to_date_time))  { expect_float32(x"xxxxxx",xxxxxxxx);}
   if (optation_day.is_desired(CSVC_crop_base_transp_act_accum_to_date_time))  { expect_float32(x"xxxxxx",xxxxxxxx);}
   if (optation_day.is_desired(CSVC_soil_base_evap_pot_accum_to_date_time))    { expect_float32(x"xxxxxx",xxxxxxxx);}
   if (optation_day.is_desired(CSVC_soil_base_evap_act_accum_to_date_time))    { expect_float32(x"xxxxxx",xxxxxxxx);}
   if (optation_day.is_desired(CSVC_residue_pot_evap_accum_to_date_time))      { expect_float32(x"xxxxxx",xxxxxxxx);}
   if (optation_day.is_desired(CSVC_residue_act_evap_accum_to_date_time))      { expect_float32(x"xxxxxx",xxxxxxxx);}
   if (optation_day.is_desired(CSVC_mgmt_irrig_applied_accum_to_date_time))    { expect_float32(x"xxxxxx",xxxxxxxx);}
                 also add unsatisfied_harvest_biomass_kg_m2
   if (optation_day.is_desired(CSVC_weather_precipitation_accum_to_date_time)) { expect_float32(x"xxxxxx",xxxxxxxx);}
   if (optation_day.is_desired(CSVC_crop_base_water_intrcpt_accum_to_date_time)){ expect_float32(x"xxxxxx",xxxxxxxx);}
   if (optation_day.is_desired(CSVC_residue_water_intrcpt_accum_to_date_time)) { expect_float32(x"xxxxxx",xxxxxxxx);}
   if (optation_day.is_desired(CSVC_soil_runoff_surface_water_runoff_accum_to_date_time))     { expect_float32(x"xxxxxx",xxxxxxxx);}
   if (optation_day.is_desired(CSVC_soil_pond_water_depth))                    { expect_float32(x"xxxxxx",xxxxxxxx);}
   if (optation_day.is_desired(CSVC_soil_base_water_entering_accum_to_date_time)){ expect_float32(x"xxxxxx",xxxxxxxx);}
   if (optation_day.is_desired(CSVC_soil_base_water_drainage_accum_to_date_time)){ expect_float32(x"xxxxxx",xxxxxxxx);}
               // eventually we will have a water balance report which will use balance_drainage
               // we will restore here the reported drainage.
               // eventually we will have a water balance report which will use balance_drainage
               // we will restore here the reported drainage.
   #ifdef NITROGEN
   if (NO3 && NH4 && NO3->GP_bal_accums)
   {  if (optation_day.is_desired(CSVC_mgmt_N_applied_accum_to_date_time))
      {   expect_float32(x"xxxxxx",nitrogen_applied_accum_GP_kg_m2);
      }
      if (desired_vars.is_desired(DLY_nitrogen_leached_accum))
      {  expect_float32(x"xxxxxx",nitrogen_leached_accum_GP_kg_m2);
      }
   }
   if (desired_vars.is_desired(CSVC_residue_mineralized_accum_to_date_time))         { expect_float32(x"xxxxxx",xxxxxxxx);}
   if (desired_vars.is_desired(CSVC_residue_manure_mineralized_accum_to_date_time))  { expect_float32(x"xxxxxx",xxxxxxxx);}
   if (desired_vars.is_desired(CSVC_organic_matter_mineralized_accum_to_date_time))  { expect_float32(x"xxxxxx",xxxxxxxx);}
   if (desired_vars.is_desired(DLY_automatic_N_in_uptake_accum))                     { expect_float32(x"xxxxxx",xxxxxxxx);}
   if (desired_vars.is_desired(DLY_organic_N_applied_accum))                         { expect_float32(x"xxxxxx",xxxxxxxx);}
   if (desired_vars.is_desired(DLY_N_Fixation_accum))                                { expect_float32(x"xxxxxx",xxxxxxxx);}
   #endif

   #endif
   #ifdef NITROGEN
   set_current_section("soil_N");                                                //110918
   if (optation_day.is_desired(CSVC_soil_N_immobilization_period_sum))           //110918
   {
      expect_float32("N_immobilization_GP",N_immobilization_accum_GP_kg_m2);}    //110918
   if (optation_day.is_desired(CSVC_soil_N_immobilization_accum_to_date_time))   //110918
   {
      expect_float32("N_immobilization_accum",N_immobilization_accum_cont_kg_m2);} //110918
   #endif

   // continuous is the default mode case CONTINUOUS_ACCUM :
   set_current_section("weather");
   if (optation_day.is_desired(CSVC_weather_ET_max_accum_to_date_time))          { expect_float32("ET_max_accum",ET_max_accum_cont_m);}

   if (optation_day.is_desired(CSVC_weather_ET_act_accum_to_date_time))          { expect_float32("ET_act_accum",ET_act_accum_cont_m);}
   /*                                                    always output       */  { expect_float32("ET_ref_accum",ET_ref_accum_cont_m);} //130723

   if (optation_day.is_desired(CSVC_weather_precipitation_accum_to_date_time))   { expect_float32("precipitation_accum",weather_precip_accum_cont_m);}
   set_current_section("crop_base");
   if (optation_day.is_desired(CSVC_crop_base_transp_pot_accum_to_date_time))    { expect_float32("transp_pot_accum",crop_transp_pot_accum_cont_m);}
   if (optation_day.is_desired(CSVC_crop_base_transp_act_accum_to_date_time))    { expect_float32("transp_act_accum",crop_transp_act_accum_cont_m);}
   if (optation_day.is_desired(CSVC_crop_base_water_intrcpt_accum_to_date_time)) { expect_float32("crop_water_intrcpt_accum",
      //160414 crop_water_intrcpt_accum_cont_m
      //160419 crop_water_intrcpt_accum_cont_m_stored
      crop_water_intrcpt_accum_cont_m                                            //160419
      );}

   if (optation_day.is_desired(CSVC_crop_biomass_yield))                         { expect_float32("yield_biomass",   crop_yield_biomass_kg_m2); } //181030

   set_current_section("soil_base");
   if (optation_day.is_desired(CSVC_soil_base_evap_pot_accum_to_date_time))      { expect_float32("soil_evap_pot_accum",soil_evap_pot_accum_cont_m);}
   if (optation_day.is_desired(CSVC_soil_base_evap_act_accum_to_date_time))      { expect_float32("soil_evap_act_accum",soil_evap_act_accum_cont_m);}
   if (optation_day.is_desired(CSVC_soil_base_water_entering_accum_to_date_time)){ expect_float32("soil_water_entering_accum",soil_water_enter_accum_cont_m);}
   if (optation_day.is_desired(CSVC_soil_base_water_drainage_accum_to_date_time)){ expect_float32("soil_water_drainage_accum",soil_water_drain_accum_cont_m);}
   set_current_section("residue");
   if (optation_day.is_desired(CSVC_residue_pot_evap_accum_to_date_time))        { expect_float32("residue_evap_pot_accum",residue_evap_pot_accum_cont_m);}
   if (optation_day.is_desired(CSVC_residue_act_evap_accum_to_date_time))        { expect_float32("residue_evap_act_accum",residue_evap_act_accum_cont_m);}
   if (optation_day.is_desired(CSVC_residue_water_intrcpt_accum_to_date_time))   { expect_float32("residue_water_intrcpt_accum",residue_water_intrcpt_accum_cont_m);}
   set_current_section("management");
   if (optation_day.is_desired(CSVC_mgmt_irrig_applied_accum_to_date_time))      { expect_float32("irrigation_accum",management_irrig_accum_cont_m);}
   if (optation_day.is_desired(CSVC_mgmt_biomass_unsatisfied_accum_to_date_time)){ expect_float32("biomass_unsatisfied_accum",management_biomass_unsatisfied_accum_cont_m);}
   set_current_section("soil_runoff");
   if (optation_day.is_desired(CSVC_soil_runoff_surface_water_runoff_accum_to_date_time))          { expect_float32("surface_water_runoff_accum",surface_water_runoff_accum_cont_m);}
               // eventually we will have a water balance report which will use balance_drainage
               // we will restore here the reported drainage.
      set_current_section("organic_matter");                                     //160516
         expect_float32("OM_gaseous_CO2_C_loss",OM_gaseous_CO2_C_loss);          //160516
/*181031 abanonded specified aboved
   if (desired_vars.is_desired(CSVC_soil_N_N_amount))                            //181031
      {  set_current_section("soil_N");
         expect_float32("N_amount",nitrogen_amount_kg_m2);
      }
*/

   #ifdef NITROGEN
   if (NO3_CONT_bal_accums && NH4_CONT_bal_accums)
   {
      if (optation_day.is_desired(CSVC_mgmt_N_applied_accum_to_date_time))
      {  set_current_section("management");
         expect_float32("N_applied_accum",nitrogen_applied_accum_cont_kg_m2);
      }
      if (optation_day.is_desired(CSVC_soil_N_leached_accum_to_date_time))
      {  set_current_section("soil_N");
         expect_float32("N_leached_accum",nitrogen_leached_accum_cont_kg_m2);
      }


      if (optation_day.is_desired(CSVC_organic_matter_mineralized_period_sum))   //110918
      {  set_current_section("organic_matter");                                  //041031
         expect_float32("N_mineralization_GP",OM_mineralization_new_accum_GP_kg_m2); //110918
      }
      if (optation_day.is_desired(CSVC_organic_matter_mineralized_accum_to_date_time)) //041021
      {  set_current_section("organic_matter");                                  //041031
            expect_float32("N_mineralization_accum",OM_mineralization_new_accum_cont_kg_m2); //110918
/*
                     mineralizated_OM_accum_cont_kg_m2= NH4_CONT_bal_accums->get_transformed_from(true);
                     set_current_section("organic_matter");                                  //041031
                        expect_float32("N_mineralization_accum",mineralizated_OM_accum_cont_kg_m2);
*/
      }



      if (optation_day.is_desired(CSVC_mgmt_N_inorganic_gaseous_loss_period_sum))   //110918
      {  set_current_section("management");
            expect_float32("N_inorganic_gaseous_loss_GP",N_inorganic_gaseous_loss_GP);
      }
      //NYI
      if (optation_day.is_desired(CSVC_mgmt_N_inorganic_gaseous_loss_accum_to_date_time))   //110918
      {  set_current_section("management");
            expect_float32("N_inorganic_gaseous_loss_accum",N_inorganic_gaseous_loss_cont);
      }
   }
#ifdef CHECK_OBSOLETE
090818
Dont delete yet
I dont think this is really obsolete because we still have V4.1 model
//050118 we now only have mineralization from organic matter
   set_current_section("residue");
   if (optation_day.is_desired(CSVC_residue_mineralized_accum_to_date_time))        { expect_float32("N_straw_mineralization_accum",mineralizated_residue_accum_cont_kg_m2);}
   if (optation_day.is_desired(CSVC_residue_manure_mineralized_accum_to_date_time)) { expect_float32("N_manure_mineralization_accum",mineralizated_manure_accum_cont_kg_m2);}
#endif
         set_current_section("management");
   if (optation_day.is_desired(CSVC_mgmt_N_organic_applied_accum_to_date_time))     { expect_float32("N_OM_applied_accum",organic_N_applied_accum_cont_kg_m2);}
         set_current_section("crop_N");
   if (optation_day.is_desired(CSVC_crop_N_automatic_uptake_accum_to_date_time))    { expect_float32("N_auto_uptake_accum",automatic_N_in_uptake_accum_cont_kg_m2);}
   if (optation_day.is_desired(CSVC_crop_N_fixation_time_step_sum))                 { expect_float32("N_fixation_accum",N_Fixation_accum_cont_kg_m2);}
   #endif
   structure_defined = true;                                                     //120314
   expect_float32("N_fixation_accum",N_Fixation_accum_cont_kg_m2);

   //special output
   set_current_section("crop");
   expect_float32("WUE",WUE);
   expect_float32("DegHr_above_30",DegHr_above_30 ); //     ,UC_degree_hour,UC_deg_hour}
   expect_float32("DegHr_above_34",DegHr_above_34);                              //180907
   #else
   {
      //200017  continue here I am pretty sure I have already provided
      // mechanism for recording selected desired variable
      // make sure that optation is setup with
      // desired variable and that
   }
   #endif
   // USE_VAR_CODES
   return true;
}
//_expect_structure_________________________________________________2012-04-26_/
bool CS_daily_data_record::set_start()                                           //161023
{
   Crop_model_interface *crop =                                                  //131008
      (land_unit_sim.crop_active_or_intercrop);
   Soil_components *soil =land_unit_sim.mod_soil();                              //161008
   Soil_runoff_common *runoff = dynamic_cast<Soil_runoff_common *>               //200316
      (land_unit_sim.runoff);                                                    //131101
   #ifdef NITROGEN
   Soil::NO3_Profile *NO3 = (soil && soil->chemicals) ? soil->chemicals->NO3 : 0;
   Soil::NH4_Profile *NH4 = (soil && soil->chemicals) ? soil->chemicals->NH4 : 0;
   Chemical_balance_accumulators_molecular *NO3_CONT_bal_accums_M = NO3 ? NO3->CONT_bal_accums_M : 0;
   Chemical_balance_accumulators_molecular *NH4_CONT_bal_accums_M = NH4 ? NH4->CONT_bal_accums_M : 0;
   #endif
   #if (CROPSYST_PROPER==4)
   //180805   CropSyst_ET
   ET_Reference                                                                  //180805
      *ET_ref = land_unit_sim.ET_ref;     // weather must be defined before soil because soil initialization requires weather info!
   #endif
   Organic_matter_residues_profile_abstract *organic_matter_residues_profile_kg_m2= land_unit_sim.organic_matter_residues_profile_kg_m2;
   Residues_interface *residue_pools_kg_m2 = organic_matter_residues_profile_kg_m2 ? organic_matter_residues_profile_kg_m2->mod_residues() : 0;
   Period_accumulators *GP_accumulators = 0;
   Annual_accumulators *ANNL_accumulators = 0;
   #if (defined(CROPSYST_PROPER))
   Period_accumulators *CONTINUOUS_accumulators= 0;
   #endif

   GP_accumulators = land_unit_sim.GP_accumulators_active;
   ANNL_accumulators = land_unit_sim.ANNL_accumulators_active;
   #if (defined (CROPSYST_PROPER) /*|| defined(OFOOT_VERSION)*/)
   CONTINUOUS_accumulators= land_unit_sim.CONTINUOUS_accumulators_active;
   #endif
   crop_act_root_biomass               = 0.0;
   crop_water_stress_stats             = 1.0; //180409 0.0;
   crop_temperature_stress_stats       = 1.0; //180410 0.0;
   crop_growing_degree_days            = 0.0;
   crop_seasonal_thermal_time          = 0.0;
   crop_dry_biomass_kg_m2              = 0.0;
   crop_yield_biomass_kg_m2            = 0.0; //181030
   crop_live_biomass_kg_m2             = 0.0;
   crop_fruit_biomass_kg_m2            = 0.0;
   crop_fruit_canopy_biomass_kg_m2     = 0.0;
   crop_leaf_area_index                = 0.0;
   crop_green_area_index               = 0.0;
   crop_root_depth_m                   = 0.0;
   crop_water_stress_index             = 0.0;
   crop_temperature_stress_index       = 0.0;
   crop_VPD_daytime                    = 0.0;
   crop_intercepted_PAR                = 0.0;
   crop_intercepted_PAR_accum          = 0.0;
   crop_canopy_cover_green             = 0.0;
   crop_canopy_cover_total             = 0.0;  //200127
   crop_leaf_water_pot                 = 0.0;

   crop_transp_pot_m                   = 0.0;
   crop_transp_act_m                   = 0.0;
   crop_water_inter_m                  = 0.0;

   nitrogen_stress_factor              = 1.0;                                    //200529
   nitrogen_stress_index               = 0.0;
   nitrogen_avg_stress_index           = 0.0;
   nitrogen_uptake_accum_kg_m2         = 0.0;
   nitrogen_mass_canopy_production_kg_m2  = 0.0;                                 //131007_110915
   nitrogen_mass_canopy_current_kg_m2  = 0.0;                                    //131007

   nitrogen_canopy_concentration       = 0.0;
   nitrogen_root_concentration         = 0.0;
   nitrogen_automatic_N_in_uptake_kg_m2= 0.0;
   nitrogen_fixation_kg_m2             = 0.0;
   nitrogen_plant_N_concentration_min  = 0.0;
   nitrogen_plant_N_concentration_max  = 0.0;
   nitrogen_plant_N_concentration_critical= 0.0;
#ifdef PHOSPHORUS
      phosphorus_applied_kg_m2            = 0.0;
      phosphorus_uptake_kg_m2             = 0.0;
#endif

   if (crop)  //140320 This would be the predominant crop
   {
      crop_act_root_biomass                  = crop->get_act_root_biomass_kg_m2();
      crop_water_stress_stats                = crop->get_water_stress();
      crop_temperature_stress_stats          = crop->get_temperature_stress();
      #ifdef THERMAL_TIME_OBSOLETE_2020
      crop_growing_degree_days            = crop->get_accum_degree_days(true);
      crop_seasonal_thermal_time          = crop->get_accum_degree_days(false);
      #else
      crop_growing_degree_days            = crop->get_accum_degree_days();       //200211
      crop_seasonal_thermal_time          = crop->get_accum_degree_days();       //200211
      #endif
      crop_dry_biomass_kg_m2              = per_ha_to_per_m2(crop->get_dry_biomass_kg_ha());
      crop_yield_biomass_kg_m2            = crop->get_latest_yield_kg_m2();      //181030
      crop_live_biomass_kg_m2             = per_ha_to_per_m2(crop->get_live_green_canopy_biomass_kg_ha());
   //done above I think we need to output the act root biomass /*041021_*/   crop_root_biomass_kg_m2             = over_ha_to_m2(crop->get_act_root_biomass_kg_ha());

      #ifdef CROP_ORCHARD
      crop_fruit_biomass_kg_m2            = per_ha_to_per_m2(crop->get_fruit_dry_biomass_kg_ha());
      #endif
      crop_fruit_canopy_biomass_kg_m2     = crop->get_canopy_biomass_kg_m2();
      //if (crop->canopy_growthX) // may be able to check this in the caller 190423
      {
      crop_leaf_area_index                = crop->get_LAI(false /*true*/);       //130515
      crop_green_area_index               = crop->get_GAI(include_vital|include_effete);
      }
      crop_root_depth_m                   = crop->update_root_depth(soil->get_rooting_depth(),soil->layers->get_depth_m());

      crop_water_stress_index        = crop->get_water_stress_index();
      crop_temperature_stress_index       = crop->get_temperature_stress_index();

      //NYI    virtual float64 get_tuber_init_temperature_stress_index()   const = 0;  //081111
      //NYI    virtual float64 get_tuber_fill_temperature_stress_index()   const = 0;  //081111

      crop_VPD_daytime                    = crop->get_VPD_daytime();
      crop_intercepted_PAR                = crop->get_intercepted_PAR_MJ_m2();
      crop_intercepted_PAR_accum          = crop->get_intercepted_PAR_accum_MJ_m2();
      crop_canopy_cover_green             = crop->get_canopy_interception_global_green(); //200228  get_fract_canopy_cover_green(); //180328
      crop_canopy_cover_total             = crop->get_canopy_interception_global_total(); //200228 get_fract_canopy_cover_total(); //200127
      crop_leaf_water_pot                 = crop->get_leaf_water_pot();
      crop_transp_pot_m                   = crop->get_pot_transpiration_m(CO2_NONE); //
     // Conceptual add crop_transp_pot CO2 adjusted //151031
      crop_transp_act_m                   = crop->ref_reported().daily.transpiration_act_m;
      crop_water_inter_m                  = crop->get_intercepted_precipitation_m(); //160419
               //NYI here I could output biomass_grazed
#ifdef NITROGEN
   const Crop_nitrogen_interface *crop_N_kg_m2 = crop->get_nitrogen();
   if (crop_N_kg_m2)
   {

      nitrogen_stress_factor              = crop_N_kg_m2->get_stress();
      nitrogen_stress_index               = crop_N_kg_m2->get_stress_index();
      nitrogen_avg_stress_index           = crop_N_kg_m2->get_stress_index_mean();
      nitrogen_uptake_accum_kg_m2         =
         (crop_N_kg_m2->get_mass_in_plant() - crop_N_kg_m2->get_mass_of_seedling()
       + (GP_accumulators != NULL ? GP_accumulators->gaseous_N_loss: 0.0)
       + crop_N_kg_m2->get_mass_removed_seasonal()) ;                            //120516
      nitrogen_mass_canopy_production_kg_m2= crop_N_kg_m2->get_mass_canopy_production(); //131007
      nitrogen_mass_canopy_current_kg_m2  = crop_N_kg_m2->get_mass_canopy_current();    //131007
      nitrogen_canopy_concentration       = crop_N_kg_m2->get_canopy_concentration(include_vital | include_effete);
      nitrogen_root_concentration         = crop_N_kg_m2->get_root_concentration(include_vital | include_effete);
      nitrogen_automatic_N_in_uptake_kg_m2= crop_N_kg_m2->get_fertilization_requirements();
      nitrogen_fixation_kg_m2             = crop_N_kg_m2->get_fixation();

      nitrogen_plant_N_concentration_min  = crop_N_kg_m2->get_concentration_min();
      nitrogen_plant_N_concentration_max  = crop_N_kg_m2->get_concentration_max();
      nitrogen_plant_N_concentration_critical= crop_N_kg_m2->get_concentration_critical();
   }
#endif
#ifdef PHOSPHORUS
               if (desired_vars.is_desired(CSVC_crop_P_mass_uptake_accum_to_date_time))
                  { phosphorus_uptake_accum_kg_m2         =
                     6.0 /*nitrogen_phosphorus_ratio_plant*/ *
                     (crop_N_kg_m2->get_mass_in_plant()  - crop_N_kg_m2->get_mass_of_seedling()
                     //+ (GP_accumulators? (GP_accumulators->gaseous_N_loss): 0.0)
                     + crop_N_kg_m2->get_mass_removed_seasonal()) ;
                  }
#endif
   }
   { // Simulation
   management_irrigation_m    = land_unit_sim.total_irrigation_today;
   biomass_unsatisfied_kg_m2  = land_unit_sim.unsatisfied_harvest_BM_today_kg_m2;
   ponded_water_cont_m        = land_unit_sim.pond.get_depth();
   }
   if (soil)
   {
   #ifdef EROSION
   soil_clod_rate             = (soil->erosion) ? soil->erosion->clod_factor_rate : 0;
   #endif

   soil_evap_pot_m = soil->mod_evaporator()->output_evaporation_potential_daily; //160719
   soil_evap_act_m = soil->mod_evaporator()->output_evaporation_actual_daily;    //160719
   soil_water_entering_m = soil->get_act_water_entering_soil();                  //150925
   soil_water_drainage_m = soil->get_reported_drainage();
   }
   #ifdef RUNOFF
   soil_surface_water_runoff_m   = (runoff) ?  runoff->estimated_runoff : 0.0;
   #endif
      ET_max_m = land_unit_sim.output_crop_evapotransp_max_m;                    //151218
      ET_act_m = land_unit_sim.output_act_crop_evapotransp_m;                    //151218

   const CS::Land_unit_meteorological  &meteorology = land_unit_sim.meteorology; //180107
   #if (CROPSYST_PROPER==4)
   ET_ref_m                            = ET_ref->get_m();                        //180416
   #else
   // Note eventually in V5 ETref will be moved and obtained from a UED file .
   ET_ref_m                            = meteorology.ref_ET_reference().get_m();
   #endif

//std::clog << "ET_ref" << ET_ref_m << std::endl;

   { // weather
      weather_precipitation_m = meteorology.ref_precipitation_actual().get_m();  //160414_151130_130402
      weather_temperature_max = meteorology.ref_air_temperature_max().Celcius(); //151130_130402
      weather_temperature_min = meteorology.ref_air_temperature_min().Celcius(); //151130_130402
      weather_snow_storage_m  = meteorology.get_snow_water_equivelent_depth();   //151026
      weather_solar_rad_MJ_m2 = meteorology.ref_solar_radiation().get_MJ_m2();   //151130_140715 get_curr_solar_radiation_MJ_m2();
   }
   #ifdef SOIL_INFILTRATION
   water_table_depth_m=soil->infiltration_submodel->get_water_table_depth();
   #endif
   if (residue_pools_kg_m2)
   { // Residue
      residue_evap_pot_m                  = residue_pools_kg_m2->get_pot_evaporation();
      residue_evap_act_m                  = residue_pools_kg_m2->get_act_evaporation();
      residue_ground_cover                = residue_pools_kg_m2->get_total_fract_ground_cover(INCLUDE_POSITION_FLAT_SURFACE| INCLUDE_POSITION_STUBBLE_SURFACE);
      residue_water_storage_m             =
         residue_pools_kg_m2->get_curr_water_hold()
            * residue_pools_kg_m2->get_surface_all_biomass();

      residue_surface_biomass_kg_m2       = residue_pools_kg_m2->get_surface_all_biomass();
      #ifdef TEMP_DISAB
      residue_plant_incorporated_kg_m2    =
            residue_plant_incorporated_kg_m2 = residue_pools_kg_m2->get_subsurface_plant_biomass();
      #endif
      //residue_manure_incorporated_kg_m2   =
      residue_manure_incorporated_kg_m2= residue_pools_kg_m2->get_subsurface_manure_biomass();
      residue_water_inter_m               = residue_pools_kg_m2->get_interception();
   }
#ifdef NITROGEN
   if (NO3_CONT_bal_accums_M && NH4_CONT_bal_accums_M)
   {  {  float32 curr_NO3_applied_E = NO3_CONT_bal_accums_M->get_applied(true);
         float32 curr_NH4_applied_E = NH4_CONT_bal_accums_M->get_applied(true);
         nitrogen_applied_kg_m2 = ( (curr_NO3_applied_E - last_NO3_applied_E)   +  (curr_NH4_applied_E - last_NH4_applied_E) );
         last_NO3_applied_E = curr_NO3_applied_E;
         last_NH4_applied_E = curr_NH4_applied_E;
      }
      {  float32 curr_NO3_uptake_E = NO3->molecular_to_elemental*(NO3_CONT_bal_accums_M->uptake);
         float32 curr_NH4_uptake_E = NH4->molecular_to_elemental*(NH4_CONT_bal_accums_M->uptake);
         nitrogen_uptake_kg_m2= ((curr_NO3_uptake_E - last_NO3_uptake_E) + (curr_NH4_uptake_E - last_NH4_uptake_E)  );
         last_NO3_uptake_E = curr_NO3_uptake_E;
         last_NH4_uptake_E = curr_NH4_uptake_E;
      }
      {  float32 curr_NO3_leached_E = NO3->molecular_to_elemental*(NO3_CONT_bal_accums_M->reported_leached);
         float32 curr_NH4_leached_E = NH4->molecular_to_elemental*(NH4_CONT_bal_accums_M->reported_leached);
         nitrogen_leached_kg_m2   =( (curr_NO3_leached_E - last_NO3_leached_E)+(curr_NH4_leached_E - last_NH4_leached_E) );
         last_NO3_leached_E = curr_NO3_leached_E;
         last_NH4_leached_E = curr_NH4_leached_E;
      }
      if (organic_matter_residues_profile_kg_m2)                                 //120503
      {  residue_mineralizated_straw_kg_m2   = organic_matter_residues_profile_kg_m2->get_total_straw_mineralization() ;
         residue_mineralizated_manure_kg_m2  = organic_matter_residues_profile_kg_m2->get_total_manure_mineralization();
               OM_mineralization_new_kg_m2   = organic_matter_residues_profile_kg_m2->get_soil_organic_matter_mineralization_profile_N(); //110918
      }
      /*NYI
      if (desired_vars.is_desired(CSVC_mgmt_N_inorganic_applied_time_step_sum))
         nitrogen_inorganic_N_applied_kg_m2    = land_unit_sim.nitrogen_daily_output.applied_inorganic; //131004
      */
      CS::Optation_temporal &optation_day = CS::optation_global.provide_temporal(UT_day); //190119
         nitrogen_organic_N_applied_kg_m2    = land_unit_sim.nitrogen_daily_output.applied_organic; //131004
         nitrogen_applied_kg_m2 = land_unit_sim.nitrogen_daily_output.applied_total;//160323
         nitrogen_N_immobilization_kg_m2     = organic_matter_residues_profile_kg_m2 ? organic_matter_residues_profile_kg_m2-> get_N_immobilization_profile() : 0;
      {  nitrogen_amount_kg_m2 = soil->chemicals->total_N_E[LAYERS_SUM_INDEX];
      }
#endif
   }
#ifdef PHOSPHORUS

#endif
   soil_water_depletion_balance_m      = ANNL_accumulators->water_depletion;     //131007
   soil_water_depletion_seasonal_m     =
      GP_accumulators ? GP_accumulators->water_depletion : 0.0;                  //131007
   soil_water_balance_m                = ANNL_accumulators->water_balance;
#ifdef KEEP
041022 Currently only outputting annual accumulation
In version 3 we had an option for growth period or annual accumulation in the daily report.
In version 4 these accumulated variables are temporary, they will be replaced by
accumulation performed by the report generator,
(version 4 has not had the option to select growth period accumulation, so I am only outputting the annual accumulation for now.
 // growth period accumulation

would need to set current section()

   ET_pot_accum_GP_m                  = GP_accumulators->pot_evapotranspiration;
   ET_act_accum_GP_m                  = GP_accumulators->act_evapotranspiration;
   ET_ref_accum_GP_m                  = GP_accumulators->ref_evapotranspiration; //130723
   crop_transp_pot_accum_GP_m         = GP_accumulators->pot_transpiration;
   crop_transp_act_accum_GP_m         = CORN::must_be_0_or_greater<float64>(GP_accumulators->act_transpiration));
   soil_evap_pot_accum_GP_m           = GP_accumulators->pot_soil_evaporation;
   soil_evap_act_accum_GP_m           = GP_accumulators->act_soil_evaporation;
   residue_evap_pot_accum_GP_m        = GP_accumulators->pot_residue_evaporation;
   residue_evap_act_accum_GP_m        = GP_accumulators->act_residue_evaporation;
   management_irrig_accum_GP_m        = GP_accumulators->irrigation;
                 also add unsatisfied_harvest_biomass_kg_m2
   weather_precip_accum_GP_m          = GP_accumulators->precip_m;
   crop_water_intrcpt_accum_GP_m      = GP_accumulators->crop_intercept;
   residue_water_intrcpt_accum_GP_m   = GP_accumulators->residue_interception;
   surface_water_runoff_accum_GP_m    = GP_accumulators->runoff;
   ponded_water_GP_m                  = simulation.pond.get_depth();
   soil_water_enter_accum_GP_m        = GP_accumulators->act_water_entering_soil;
   soil_water_drain_accum_GP_m        = GP_accumulators->reported_drainage;
               // eventually we will have a water balance report which will use balance_drainage
               // we will restore here the reported drainage.
               // eventually we will have a water balance report which will use balance_drainage
               // we will restore here the reported drainage.
#ifdef NITROGEN
   if (NO3 && NH4 && NO3->GP_bal_accums)
   {  if (desired_vars.is_desired(CSVC_mgmt_N_applied_accum_to_date_time))
      { nitrogen_applied_accum_GP_kg_m2       =
                           ( NO3->GP_bal_accums->get_applied(true)
                           + NH4->GP_bal_accums->get_applied(true));
      }
      if (desired_vars.is_desired(DLY_nitrogen_leached_accum))
      { nitrogen_leached_accum_GP_kg_m2    =
                           (NO3->GP_bal_accums->get_reported_leached(true)
                           +NH4->GP_bal_accums->get_reported_leached(true) );
      }
   }
   mineralizated_residue_accum_GP_kg_m2   = GP_accumulators->straw_mineralization;
   mineralizated_manure_accum_GP_kg_m2    = GP_accumulators->manure_mineralization;
   mineralizated_OM_accum_GP_kg_m2        = GP_accumulators->NH4_mineralization;
   automatic_N_in_uptake_accum_GP_kg_m2   = GP_accumulators->N_fert_requirements;
   organic_N_applied_accum_GP_kg_m2       = GP_accumulators->organic_N_applied;
   N_Fixation_accum_GP_kg_m2              = GP_accumulators->N_fixation;
#endif

#endif
#ifdef NITROGEN
    N_immobilization_accum_GP_kg_m2              = GP_accumulators
      ? GP_accumulators->N_immobilization :0.0;                                  //110918
    #if defined(CROPSYST_PROPER)
    N_immobilization_accum_cont_kg_m2 = (CONTINUOUS_accumulators != NULL)
      ? CONTINUOUS_accumulators->N_immobilization:0.0;                           //110918
    #endif
#endif
   // continuous is the default mode case CONTINUOUS_ACCUM :
   #if defined(CROPSYST_PROPER)
   ET_max_accum_cont_m                     =CONTINUOUS_accumulators->evapotranspiration_max;
   //190812 ET_pot_accum_cont_m                     =CONTINUOUS_accumulators->pot_evapotranspiration;
   ET_act_accum_cont_m                     =CONTINUOUS_accumulators->evapotranspiration_act;
   ET_ref_accum_cont_m                     =CONTINUOUS_accumulators->evapotranspiration_ref; //130723
   weather_precip_accum_cont_m             =CONTINUOUS_accumulators->precip_m;
   crop_transp_pot_accum_cont_m            =CONTINUOUS_accumulators->pot_transpiration;
   crop_transp_act_accum_cont_m            =CORN::must_be_0_or_greater<float64>(CONTINUOUS_accumulators->act_transpiration);
   crop_water_intrcpt_accum_cont_m         =CONTINUOUS_accumulators->crop_intercept; //160419
   soil_evap_pot_accum_cont_m              =CONTINUOUS_accumulators->pot_soil_evaporation;
   soil_evap_act_accum_cont_m              =CONTINUOUS_accumulators->act_soil_evaporation;
   soil_water_enter_accum_cont_m           =CONTINUOUS_accumulators->act_water_entering_soil;
   soil_water_drain_accum_cont_m           =CONTINUOUS_accumulators->reported_drainage;

   residue_evap_pot_accum_cont_m           =CONTINUOUS_accumulators->pot_residue_evaporation;
   residue_evap_act_accum_cont_m           =CONTINUOUS_accumulators->act_residue_evaporation;
   residue_water_intrcpt_accum_cont_m      =CONTINUOUS_accumulators->residue_interception;

   management_irrig_accum_cont_m           =CONTINUOUS_accumulators->irrigation;
   management_biomass_unsatisfied_accum_cont_m=CONTINUOUS_accumulators->biomass_unsatisfied;

   surface_water_runoff_accum_cont_m=CONTINUOUS_accumulators->runoff;
   #endif
               // eventually we will have a water balance report which will use balance_drainage
               // we will restore here the reported drainage.
   #ifdef NITROGEN
   if (NO3_CONT_bal_accums_M && NH4_CONT_bal_accums_M)
   {  {  nitrogen_applied_accum_cont_kg_m2 =
                      ( NO3->molecular_to_elemental*(NO3_CONT_bal_accums_M->applied)
                      + NH4->molecular_to_elemental*(NH4_CONT_bal_accums_M->applied));
      }
      {  nitrogen_leached_accum_cont_kg_m2 =
                           (NO3->molecular_to_elemental*(NO3_CONT_bal_accums_M->reported_leached)
                           +NH4->molecular_to_elemental*(NH4_CONT_bal_accums_M->reported_leached) );
      }
      {  OM_mineralization_new_accum_GP_kg_m2= GP_accumulators!=NULL ? GP_accumulators->OM_N_mineralization : 0;  //120516 110918
      }
      {
         #if defined(CROPSYST_PROPER)
         OM_mineralization_new_accum_cont_kg_m2= CONTINUOUS_accumulators->OM_N_mineralization;  //110918
         #endif
/*
                     mineralizated_OM_accum_cont_kg_m2= NH4_CONT_bal_accums->get_transformed_from(true);
                     set_current_section("organic_matter");                                  //041031
                        expect_float32("N_mineralization_accum",mineralizated_OM_accum_cont_kg_m2);
*/
      }
      {
         N_inorganic_gaseous_loss_GP = GP_accumulators != NULL ? GP_accumulators->inorganic_NH4_to_atmosphere:0;  //120516
      }
      //NYI
      {
         #if defined(CROPSYST_PROPER)
         N_inorganic_gaseous_loss_cont = CONTINUOUS_accumulators->inorganic_NH4_to_atmosphere;
         #endif
      }
   }
   #endif
   OM_gaseous_CO2_C_loss =                                                       //160516
      organic_matter_residues_profile_kg_m2 ?                                    //171111
      organic_matter_residues_profile_kg_m2->get_gaseous_CO2_C_loss_reported_daily
         (INCLUDE_SOM_AND_MICROBIAL)
      : 0.0;

   // Special output 160522
   const Crop_complete *crop_CropSyst=dynamic_cast<const Crop_complete *>(crop);
   if (crop_CropSyst)
   {
      WUE            = crop_CropSyst->report_transpiration_use_eff;
      DegHr_above_30 = crop_CropSyst->report_degree_hours_above_30_today;
      DegHr_above_34 = crop_CropSyst->report_degree_hours_above_34_today;
   }else
   {
      WUE            = 0;
      DegHr_above_30 = 0;
      DegHr_above_34 = 0;
   }
   return CORN::Data_record::set_start();
}
//_set_start________________________________________________________2012-04-26_/
#define YVU UED::Record_type_distinguisher_interface::UED_data_set_year_var_units
#define UED_CL UED::Data_record_creation_layout
void CS_UED_tuple_data_source::register_layer_field
(UED::Variable_code variable_code,nat8 num_sublayers,const char *section
,const char *base_field_name,Units_code preferred_units
,Units_code stored_units,Units_code time_stamp_step)
{
   for (nat8 sublayer = 1; sublayer <= num_sublayers; sublayer++)
   {  UED::Variable_code_clad layer_variable_code(variable_code); layer_variable_code.set_layer(sublayer);
      char field_name[50];
      strcpy(field_name,base_field_name);
      strcat(field_name,"_");
      char sublayer_str[10];
      CORN::int8_to_cstr(sublayer,sublayer_str,10);
      strcat(field_name,sublayer_str);
      add_field(section,field_name,new UED_CL
         (YVU,layer_variable_code.code,preferred_units,UT_day,time_stamp_step
          ,simulated_quality), true,stored_units);
   }
}
//_register_layer_field_____________________________________________2004-10-02_/
void CS_UED_tuple_data_source::register_fields
(nat8 num_sublayers,Units_code time_stamp_step)
{
   CS::Optation_temporal &optation_day = CS::optation_global.provide_temporal(UT_day); //190119

   for (int i = 0; i < daily_variable_fields_count; i++)
      if (optation_day.is_desired(daily_variable_fields_registration[i].CSVC_var_code))
         add_field(daily_variable_fields_registration[i].section_name
                  ,daily_variable_fields_registration[i].field_name
                  ,new UED_CL(YVU,daily_variable_fields_registration[i].CSVC_var_code
                     ,daily_variable_fields_registration[i].preferred_units
                     ,UT_day,time_stamp_step,simulated_quality)
                  ,true,daily_variable_fields_registration[i].stored_units);
   for (int i = 0; i < daily_profile_variable_fields_count; i++)
      if (optation_day.is_desired(daily_profile_variable_fields_registration[i].CSVC_var_code))
         register_layer_field(daily_profile_variable_fields_registration[i].CSVC_var_code
            ,num_sublayers
            ,daily_profile_variable_fields_registration[i].section_name
            ,daily_profile_variable_fields_registration[i].field_name
            ,daily_profile_variable_fields_registration[i].preferred_units
            ,daily_profile_variable_fields_registration[i].stored_units,time_stamp_step);
}
//_register_fields__________________________________________________2004-10-31_/
}//_namespace CropSyst_________________________________________________________/
#endif

