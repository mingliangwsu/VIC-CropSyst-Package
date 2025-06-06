#include "management_param_V4.h"
#include "static_phrases.h"
#include "corn/data_source/vv_file.h"

#if (MANAGEMENT==4)
namespace CropSyst {
//______________________________________________________________________________
Inorganic_nitrogen_operation::Inorganic_nitrogen_operation()
:Inorganic_nitrogen_operation_class()
,v_NO3_N                 (NO3_N              ,0 /*UC_kg_ha */ ,"NO3_N",3,0, 0.0,1000.0, 0.0,1000.0,TU_kg_ha  ,TL_Amount_N_form_of_nitrate)
,v_NH4_N                 (NH4_N              ,0 /*UC_kg_ha */ ,"NH4_N",3,0, 0.0,1000.0, 0.0,1000.0,TU_kg_ha  ,TL_Amount_N_form_of_ammonium)
   //120710  The units have been disabled because they are either kg/ha or fixed inorganic application or % for split automatic N
,v_NH4_volatilization    (NH4_volatilization ,UC_percent,LABEL_NH4_volatilization,3,0, 0.0, 100.0, 0.0, 100.0,TU_percent,TL_Volatilization_loss,TL_CropSyst_will_calculate)
{}
//_Inorganic_nitrogen_operation:constructor____________________________________/
void Inorganic_nitrogen_operation::setup_parameters(Data_record &data_rec,bool for_write)
{  Inorganic_nitrogen_operation_class::setup_parameters(data_rec, for_write);
   data_rec.expect_float32(LABEL_NO3_amount,NO3_N/*_kg_ha*/);   // Obsolete provided for auto upgrade
   data_rec.expect_float32(LABEL_NH4_amount,NH4_N/*_kg_ha*/);   // obsolete
   data_rec.expect_float32("application_depth",injection_depth_cm);              //150120

   // V5 uses parameter V4 simply expect the value
   data_rec.expect_float32("N_soil_critical" ,condition.N_soil_critical);        //170630
   data_rec.expect_float32("sampling_depth"  ,condition.sampling_depth);         //170630

}
//_Inorganic_nitrogen_operation::setup_parameters___________________1998-10-07_/
void Inorganic_nitrogen_operation::log(std::ostream &log_file)             const
{  Inorganic_nitrogen_operation_class::log(log_file);
   char buffer[255];
   log_file << v_NO3_N.log_cstr(buffer);                                         //030123
   log_file << v_NH4_N.log_cstr(buffer);
   log_file << v_NH4_volatilization.log_cstr(buffer);
}
//_Inorganic_nitrogen_operation::log___________________________________________/
}//_namespace CropSyst_________________________________________________________/

//#include "corn/data_source/vv_file.cpp"
#include "common/biomatter/biomass_decomposition_param.h"
#include "mgmtevnt.h"
// Labels from V3
#define LABEL_automatic_clipping    "automatic_clipping"
#define LABEL_latest_date_to_harvest   "latest_date_to_harvest"
#define LABEL_automatic_nitrogen    "automatic_nitrogen"
#define LABEL_auto_N_1 "auto_N_1"
#define LABEL_mode                              "mode"
#define LABEL_depth_obsolete                    "sampling_depth"
namespace CropSyst {
//______________________________________________________________________________

#if (MANAGEMENT==4)
DECLARE_CONTAINER_ENUMERATED_SECTION(Residue_event            ,residue_operation_section_VV,true);
#endif
#if ((CROPSYST_VERSION > 0) && (CROPSYST_VERSION < 5))
DECLARE_CONTAINER_ENUMERATED_SECTION(V3_clipping_event       ,V3_clipping_operation_section_VV,true);
DECLARE_CONTAINER_ENUMERATED_SECTION(Organic_nitrogen_event   ,organic_nitrogen_operation_section_VV,true);
#endif
//______________________________________________________________________________

Tillage_operation::Tillage_operation()
: Tillage_operation_class()
,v_dust_mulch_depth_cm        (dust_mulch_depth_cm       ,UC_cm        ,"dust_mulch_depth"       ,1,10.0,0,100,0,300,"cm"  ,"Dust mulching depth")
,v_dust_mulch_effective_days  (dust_mulch_effective_days ,              "dust_mulch_effective_days",30  ,0, 60,0,365,"days","Number of days until mulch effect subsides","Without precipitation")
,v_dust_mulch_intensity       (dust_mulch_intensity      ,UC_adjustment,"dust_mulch_intensity"   ,2,0.0,0,0.9,0,1.0,"intensity"  ,"Dust mulching intensity","(0.0 = no effect, 1.0 = sever)")
,v_oxidation_effect_sand      (oxidation_effect.sand     ,UC_multiplier,"sand_oxidation"        ,3,1.5,0.0,10.0,0.0,100.0,"multiplier","sand","tillage oxidation effect")
,v_oxidation_effect_clay      (oxidation_effect.clay     ,UC_multiplier,"clay_oxidation"        ,3,0.5,0.0,10.0,0.0,100.0,"multiplier","clay","tillage oxidation effect")
{
//190221 abandoned, should be needed   op_clad.set(CS_OP_MGMT_TILLAGE);                                              //190221
}
//_Tillage_operation:constructor____________________________________2011-01-13_/
void Tillage_operation::setup_parameters(CORN::Data_record &data_rec,bool for_write)
{  Tillage_operation_class::setup_parameters(data_rec,for_write);
   data_rec.expect_valid_float32(v_dust_mulch_depth_cm);                         //051206
   data_rec.expect_valid_int16(v_dust_mulch_effective_days);                     //051206
   data_rec.expect_valid_float32(v_dust_mulch_intensity);                        //051212
   data_rec.expect_valid_float32(v_oxidation_effect_sand);                       //100121
   data_rec.expect_valid_float32(v_oxidation_effect_clay);                       //100121
//190221 abandoned   op_clad.set(CS_OP_MGMT_TILLAGE);                                              //190221
}
//_Tillage_operation::setup_parameters______________________________1998-10-07_/
// This mode is for compatibility with previous versions
// I was planning on making these deprecated replaced with
// Organic_matter_composition_decomposition in version 5,
// but Claudio is still using this set of parameter in the VB version,
// also for CAFE dairy.
//
Organic_nitrogen_operation_abstract::Organic_nitrogen_operation_abstract(CropSyst_Op cs_op)
: Organic_nitrogen_operation_abstract_class(cs_op)
,v_org_N                   (org_N_kg_ha            ,UC_kg_ha                     ,LABEL_org_N            ,3    ,0  , 0.0, 400.0, 0.0,2000.0,"kg/ha"/*TU_kg_ha */ ,"Nitrogen mass in organic form"/*120304 TL_Amount_N_form_of_organic_manure*/)
,v_NH3_N                   (NH3_N_kg_ha            ,UC_kg_ha                     ,"NH3_N"/*110818 LABEL_NH3_amount*/       ,3    ,0  , 0.0,1000.0, 0.0,1000.0,"kg/ha"/*TU_kg_ha */  ,TL_Amount_N_form_of_ammonia)
,v_dry_matter              (dry_matter_percent     ,UC_percent                   ,"dry_matter"           ,1    ,13 ,3.0,  20.0, 0.0, 100.0,"%"       ,"Dry matter of fresh manure (percent not water)")
,v_long_term_org_N_volatilization_loss_percent(long_term_org_N_volatilization_loss_percent,UC_percent ,LABEL_long_term_org_N_volatilization  ,3,0, 0.0, 100.0, 0.0, 100.0,"%"/*TU_percent*/,"Long term NH3 volatilization (including mineralized organic N)") //020525x
{}
//_Organic_nitrogen_operation_abstract:constructor__________________2011-01-13_/
Residue_operation::Residue_operation()
:Tillage_operation()
{  op_clad.set(CS_OP_MGMT_RESIDUE);                                              //030824
}
//_Residue_operation:constructor_______________________________________________/
const char *Residue_operation::label_string(std::string &result)           const
{   CropSyst_field_operation_VX::label_string(result);
   return result.c_str();
}
//_Residue_operation:label_string___________________________________2017-04-23_/
void Residue_operation::log(std::ostream &log_file)                        const
{  CropSyst_field_operation_VX::log(log_file);
}
//_Residue_operation:log_______________________________________________________/
void Organic_nitrogen_operation_abstract::setup_parameters(CORN::Data_record &data_rec,bool for_write)
{
   CropSyst_field_operation_VX::setup_parameters(data_rec, for_write);
   data_rec.expect_valid_float32(v_org_N);
   data_rec.expect_valid_float32(v_NH3_N);                                       //981226
   if (!for_write)                                                               //110818
      data_rec.expect_float32(LABEL_NH3_amount,NH3_N_kg_ha); /*this is to read older V4 and V3 */ //110818
   data_rec.expect_valid_float32(v_dry_matter);                                  //981226
   data_rec.expect_float32(LABEL_long_term_org_N_volatilization
                          ,long_term_org_N_volatilization_loss_percent);         //020525
   data_rec.expect_float32("total_solids",total_solids_kg_ha);
   data_rec.expect_float32("water_depth",water_depth_mm);                        //110816
   data_rec.expect_float32("water_volume",water_volume_m3);                      //110816
   data_rec.expect_float32("application_depth",injection_depth_cm);              //150120
}
//_Organic_nitrogen_operation_abstract::setup_parameters____________1998-12-28_/
void Organic_nitrogen_operation_abstract::log(std::ostream &log_file) const
{  Organic_nitrogen_operation_abstract_class::log(log_file);
   char buffer[255];                                                             //030123
   log_file << v_org_N.log_cstr(buffer);
   log_file << v_NH3_N.log_cstr(buffer);                                         //030123
   log_file << v_dry_matter.log_cstr(buffer);                                    //021105
   log_file << v_long_term_org_N_volatilization_loss_percent.log_cstr(buffer);   //030123
}
//_Organic_nitrogen_operation_abstract::log_________________________2011-01-13_/
nat32 Management_parameters::fertilization_nitrogen_multiply_by(float multiplier) modification_
{  uint32 count = Management_parameters_class::fertilization_nitrogen_multiply_by(multiplier);
#if ((CROPSYST_VERSION > 0) && (CROPSYST_VERSION < 5))
// This mode is for compatibility with previous versions
// I was planning on making these deprecated replaced with
// Organic_matter_composition_decomposition in version 5,
// but Claudio is still using this set of parameter in the VB version,
// also for CAFE dairy.
//
   FOR_EACH_IN(org_N,Organic_nitrogen_operation,organic_nitrogen_operations,each_org_N)
       count += org_N->multiply_amount_by(multiplier);
   FOR_EACH_END(each_org_N)
#endif

//130904  Warning I might need to apply this multiplication to the new Biomatter application

   return count;
}
//_fertilization_nitrogen_multiply_by_______________________________2007-07-16_/
#if ((CROPSYST_VERSION > 0) && (CROPSYST_VERSION < 5))
// This mode is for compatibility with previous versions
// I was planning on making these deprecated replaced with
// Organic_matter_composition_decomposition in version 5,
// but Claudio is still using this set of parameter in the VB version,
// also for CAFE dairy.
//
Organic_nitrogen_operation::Organic_nitrogen_operation                        ()
:Organic_nitrogen_operation_abstract               (CS_OP_MGMT_ORGANIC_NITROGEN) //080902
,org_N_volatilization_calculated                                          (true) //020525
,carbon_fraction                                                           (0.5) //080225
//200220 ,decomposition_time_calculated(true)                                             //990227
//200220 ,decomposition_time_63     (0)
,halflife                                                                    (0) //040628
,solid_liquid_form_clad                                             (solid_form) //020322
,v_carbon_fraction         (carbon_fraction,UC_fraction  ,"carbon_fraction"      ,3    ,0.5, 0.0,1.0   , 0.0,1.0   ,"fraction","Carbon fraction of biomass")   //080225
//200220obs ,v_decomposition_time_63   (decomposition_time_63        ,LABEL_decomposition_time     ,60 , 0  ,1000  ,   0,3000  ,"Days"    ,TL_Decomposition_time_constant)
,v_halflife   (halflife,"halflife"  ,80 , 0  ,1000  ,   0,3000  ,"Days"    ,"Time to decompose 50%")        //040826
{}
//_Organic_nitrogen_operation:constructor______________________________________/
bool Organic_nitrogen_operation::is_liquid()  const
{  bool liquid = false;
   if ((dry_matter_percent > 0.0) || (dry_matter_percent < 100.0))               //080903
      liquid = Organic_nitrogen_operation_abstract::is_liquid();                 //080903
   else                                                                          //080903
   {  // if we dont have a value for dry_matter_percent
      // The parameter file is earlier the version 4.12.04
      // In earlier version the user selected the form of manure not the DryMatter %  080903
      liquid =  solid_liquid_form_clad.get() > solid_form;
   }
   return liquid;
}
//_is_liquid()______________________________________________________2002-03-22_/
void Organic_nitrogen_operation::setup_parameters(CORN::Data_record &data_rec,bool for_write)
{
   Organic_nitrogen_operation_abstract::setup_parameters(data_rec,for_write);    //080902
   data_rec.expect_float32("carbon_fraction",carbon_fraction);                   //080225
   /*200220obs
   data_rec.expect_bool(LABEL_decomposition_time_calculated
      ,decomposition_time_calculated,CORN::VV_bool_entry::form_true );           //990227
   */
   /*200220obs
   data_rec.expect_int16(LABEL_decomposition_time,decomposition_time_63);
   */
   if (!for_write) // renamed to halflife
      data_rec.expect_int16("decomposition_time_50",halflife);                   //040826
   data_rec.expect_int16("halflife",halflife);                                   //040826
   data_rec.expect_bool(LABEL_org_N_volatilization_calculated
      ,org_N_volatilization_calculated,CORN::VV_bool_entry::form_true );         //020525
   data_rec.expect_enum(LABEL_liquid,solid_liquid_form_clad);                    //020322
}
//_setup_parameters_________________________________________________1998-12-28_/
const char *Organic_nitrogen_operation::label_string(std::string &result)  const
{  CropSyst_field_operation_VX::label_string(result);
   result.append(TL_Organic_N);
   result += ":";
   CORN::append_float32_to_string(org_N_kg_ha,3,result);
   result += " kg N/ha";
   return result.c_str();
}
//_label_string_____________________________________________________2017-04-23_/
void Organic_nitrogen_operation::log(std::ostream &log_file)          stream_IO_
{  Organic_nitrogen_operation_abstract::log(log_file);                           //080902
   char buffer[255];                                                             //030123
   log_file << LABEL_org_N_source << '=';                                        //021105
      organic_matter_source_cowl.stream_label(log_file);                         //190120
      log_file << std::endl;
   log_file << LABEL_org_N_volatilization_calculated <<'='  << org_N_volatilization_calculated << std::endl;
   log_file << "carbon fraction" <<'=' << carbon_fraction << std::endl;
   /*200220obs
   if (!decomposition_time_calculated)
   log_file << v_decomposition_time_50.log_cstr(buffer);                         //030123
   */
   log_file << "form" << '=' << solid_liquid_form_clad.stream_label (log_file)<<std::endl; //021105
}
//_log______________________________________________________________20??-??-??_/
#endif

#define HARVEST_CLIP_CONSTRUCTOR_INITIALIZATION                                                                                                                                                                                                           \
, v_min_LAI_required             (min_LAI_required, U_units_not_specified,     LABEL_min_LAI              ,2,0.0,0.0,6.0,0.0,30.0,TU_m2_m2,TL_Minimum_LAI_required_for_clipping)                                                                          \
, v_min_biomass_required_kg_ha  (min_biomass_required_kg_ha  ,   UC_kg_ha,   LABEL_min_biomass    ,2,0.0,0.0,30000.0,0.0,100000.0,TU_kg_ha,"Minimum biomass required for clipping")                                                                       \
, v_reserve_biomass_kg_ha        (reserve_biomass_kg_ha,UC_kg_ha             ,"reserve_biomass",2,0.0,0.0,10000.0,0.0,100000.0,TU_kg_ha,"Reserve biomass (unavailable for clipping)")                                                                     \
, v_grazing_percent_N_as_OM      (grazing_percent_N_as_OM ,LABEL_percent_N_as_OM        ,50,0,100,0,100,TU_percent,"Percent of manure nitrogen as organic matter")                                                                                        \
, v_grazing_percent_N_as_NH3     (grazing_percent_N_as_NH3,LABEL_percent_N_as_NH3       ,50,0,100,0,100,TU_percent,"Percent of manure nitrogen as ammonia (NH3)")                                                                                         \
, v_remove_amount_kg_ha                   (remove_amount_kg_ha,UC_kg_ha ,"remove_amount"  ,  2,0.0,0.0,10000.0,0.0,1000000.0,TU_kg_ha,"Fixed amount of biomass to remove")                                                                                \
, v_min_retain_GAI                        (min_retain_GAI           ,U_units_not_specified     ,"min_retain_GAI"   ,2,  0.0,0.01,  30.0,0.0, 40.0,TU_m2_m2  ,"Minimum GAI to be retained")                                                                \
, v_remove_for_use_percent                (biomass_fate_percents.remove_for_use               ,"use"            ,  0,0,100,0,100,TU_percent,"Removed and designated for beneficial use (clipped fodder, silage, etc.)","portion of above ground biomass") \
, v_remove_for_grazing_percent            (biomass_fate_percents.remove_for_grazing           ,"graze"          ,  0,0,100,0,100,TU_percent,"Removed for grazing or other animal consumption","portion of above ground biomass")                          \
, v_remove_for_disposal_percent           (biomass_fate_percents.remove_for_disposal          ,"dispose"        ,  0,0,100,0,100,TU_percent,"Removed from the field and disposed (unused)","portion of above ground biomass")                             \
, v_remain_as_residue_percent             (biomass_fate_percents.remain_as_residue            ,"residue"        ,  0,0,100,0,100,TU_percent,"Remains in the field laying as surface residue","portion of above ground biomass")                           \
, v_remain_standing_live_percent          (biomass_fate_percents.remain_as_live               ,"live"           ,100,0,100,0,100,TU_percent,"Remains as live standing plant tissue ","portion of above ground biomass")                                   \
, v_remain_standing_dead_percent          (biomass_fate_percents.remain_as_dead               ,"dead"           ,100,0,100,0,100,TU_percent,"Remains in the field as dead standing stubble/residue","portion of above ground biomass")                    \
, v_remain_roots_as_dead_residue_percent  (biomass_fate_percents.remove_roots_to_dead_residue ,"root_dead"      ,100,0,100,0,100,TU_percent,"Remains as dead root residue","portion of subsurface biomass")                                               \
, v_remove_roots_as_loss_percent          (biomass_fate_percents.remove_roots_as_loss         ,"root_loss"      ,100,0,100,0,100,TU_percent,"Removed as loss","portion of subsurface biomass (usually 0.0)")                                              \
, v_remain_roots_live_percent             (biomass_fate_percents.remain_roots_live            ,"root_live"      ,100,0,100,0,100,TU_percent,"Remains as live roots","portion of subsurface biomass")                                                      \
, v_biomass_returned_as_manure            (biomass_fate_percents.return_as_manure      ,"return_as_manure",80,0,100,0,100,TU_percent,"Percent of grazed biomass returned to field as manure","portion of grazing manure production")

Harvest_or_clipping_operation::Harvest_or_clipping_operation()
: Harvest_or_clipping_operation_class()                                          //120419
#ifndef __linux__
, retain_GAI_obsolete_V4_3(0)                                                    //050825
, max_retain_GAI_obsolete        (20.0) // large number disabled the max         //051105
, V3_trim_biomass_removed        (0)
, V3_biomass_fate_clad        (BIOMASS_TO_HARVEST)                            //031014
, V3_trim_removed_to_surface     (0)
, v_V3_trim_biomass_removed      (V3_trim_biomass_removed    ,LABEL_trim_biomass_removed,0,0,100,0,100,TU_percent,TL_Percent_biomass_to_remove) // WARNING need translation
, v_V3_trim_removed_to_surface   (V3_trim_removed_to_surface ,LABEL_trim_removed_to_surface,0,0,100,0,100,TU_percent,"Percent of the removed biomass add to surface residue") // WARNING need translation
, v_retain_GAI_obsolete                   (retain_GAI_obsolete_V4_3  ,U_units_not_specified     ,"retain_GAI"   ,2,  0,0,  8,0, 20,TU_m2_m2  ,"GAI to leave remaining on the field after harvest")             //050822
, v_max_retain_GAI_obsolete               (max_retain_GAI_obsolete   ,U_units_not_specified     ,"max_retain_GAI"   ,2, 20.0,5.0,   30.0,0.0, 40.0,TU_m2_m2  ,"Maximum GAI retained after biomass is removed") //050822
#endif
HARVEST_CLIP_CONSTRUCTOR_INITIALIZATION
{}
//______________________________________________________________________________
#define HARVEST_CLIP_SETUP                                                     \
   data_rec.expect_valid_float32(v_min_LAI_required);                          \
   data_rec.expect_valid_float32(v_min_biomass_required_kg_ha);                \
   data_rec.expect_valid_float32(v_reserve_biomass_kg_ha);                     \
   data_rec.expect_valid_int16(v_grazing_percent_N_as_OM);                     \
   data_rec.expect_valid_int16(v_grazing_percent_N_as_NH3);                    \
   data_rec.expect_valid_float32(v_remove_amount_kg_ha);                       \
   data_rec.expect_valid_float32(v_min_retain_GAI);                            \
   data_rec.expect_valid_int16(v_remove_for_use_percent);                      \
   data_rec.expect_valid_int16(v_remove_for_grazing_percent);                  \
   data_rec.expect_valid_int16(v_biomass_returned_as_manure);                  \
   data_rec.expect_valid_int16(v_remove_for_disposal_percent);                 \
   data_rec.expect_valid_int16(v_remain_as_residue_percent);                   \
   data_rec.expect_valid_int16(v_remain_standing_live_percent);                \
   data_rec.expect_valid_int16(v_remain_standing_dead_percent);                \
   data_rec.expect_valid_int16(v_remain_roots_as_dead_residue_percent);        \
   data_rec.expect_valid_int16(v_remove_roots_as_loss_percent);                \
   data_rec.expect_valid_int16(v_remain_roots_live_percent);

void Harvest_or_clipping_operation::setup_parameters(CORN::Data_record &data_rec,bool for_write)
{   Harvest_or_clipping_operation_class::setup_parameters(data_rec,for_write);
#ifndef __linux__
   if (!for_write)                                                               //050822
   {                                                                             //050822
      data_rec.expect_valid_int16(v_V3_trim_biomass_removed);
      data_rec.expect_enum(LABEL_V3_clipping_fate,V3_biomass_fate_clad);         //020322
      data_rec.expect_valid_int16(v_V3_trim_removed_to_surface);
   }
   if (!for_write) data_rec.expect_valid_float32(v_retain_GAI_obsolete);
      // becoming obsolete 4.3.0  keeping this only to import old files          //060310
      data_rec.expect_valid_float32(v_max_retain_GAI_obsolete);                  //051105
#endif
   HARVEST_CLIP_SETUP                                                            //120502
}
//_Harvest_or_clipping_operation::setup_parameters__________________2003-10-14_/
Auto_clip_biomass_mode::Auto_clip_biomass_mode()
: Auto_clip_biomass_mode_class()
HARVEST_CLIP_CONSTRUCTOR_INITIALIZATION
, v_LAI_forces_clipping                      (LAI_forces_clipping       ,UC_kg_ha                  ,LABEL_max_LAI          ,2,5.0, 2.0,10.0, 0.0,15.0,TU_m2_m2,"Maximum LAI that forces clipping")
, v_biomass_forces_clipping                  (biomass_forces_clipping   ,UC_kg_ha                  ,LABEL_max_biomass      ,2,4000.0,2000.0,6000.0,0.0,100000.0,TU_kg_ha,TL_Maximum_biomass_that_forces_clipping)
, v_flowering_forces_clipping                (flowering_forces_clipping /*UC_Days*/                ,LABEL_after_flowering  ,0,0,365,0,366,"Days","Number of days after flowering forces clipping")
, v_adjust_relative_growth_rate_for_clipping (adj_relative_growth_rate  ,U_units_not_specified     ,LABEL_adj_rel_growth   ,2,1.0,0.5,1.5,0.0,2.0,TU_0_2,TL_Clipping_relative_growth_rate_adjustment)
{}
//_Auto_clip_biomass_mode:constructor_______________________________2001-10-27_/
void Auto_clip_biomass_mode::setup_parameters(Data_record &data_rec,bool for_write)
{
   Harvest_or_clipping_operation_class::setup_parameters(data_rec,for_write);    //150404
   HARVEST_CLIP_SETUP                                                            //120502
   data_rec.expect_valid_float32(v_adjust_relative_growth_rate_for_clipping);
   data_rec.expect_valid_float32(v_LAI_forces_clipping);
   data_rec.expect_valid_float32(v_biomass_forces_clipping);
   data_rec.expect_valid_int16(v_flowering_forces_clipping);
}
//_Auto_clip_biomass_mode::setup_parameters_________________________2002-03-14_/

// Warning, not sure why I moved log here and not to '_class
void Auto_clip_biomass_mode::log(std::ostream &log_file) const
{
   char buffer[255];                                                             //031023
   log_file << v_adjust_relative_growth_rate_for_clipping.log_cstr(buffer);      //031023
   log_file << v_biomass_forces_clipping.log_cstr(buffer);                       //031023
   log_file << v_flowering_forces_clipping.log_cstr(buffer);                     //040830
}
//_Auto_clip_biomass_mode::log______________________________________2008-08-10_/
#define IRRIGATION_CONSTRUCTOR_INITIALIZATION                                  \
,v_amount                  (amount_mm              ,UC_mm                  ,LABEL_amount              ,3,0   , 0.0,1000.0, 0.0,1000.0,TU_mm   ,TL_Total_irrigation)                           \
,v_net_irrigation_mult     (net_irrigation_mult    ,U_units_not_specified  ,LABEL_net_irrigation_mult ,2,1.0 , 0.0,2.0, 0.0,10.0,TU_unitless,TL_Net_irrigation_multiplier)                    \
,v_refill_point            (refill_point           ,UC_decimal_percent     ,LABEL_refill_point        ,2,1.0 ,0.0,1.0,0.0,2.0,TU_0_1,"Refill point")                                          \
,v_NO3_N_concentration     (NO3_N_concentration       ,UC_kg_m3               ,"NO3_N_concentration"     ,3,0.0 ,0.0,99999.9,0.0,99999.9,TU_kg_m3,"Nitrate NO3 N concentration",TL_Irrigation )  \
,v_NH4_N_concentration     (NH4_N_concentration       ,UC_kg_m3               ,"NH4_N_concentration"     ,3,0.0 ,0.0,99999.9,0.0,99999.9,TU_kg_m3,"Ammonium NH4 N concentration",TL_Irrigation ) \
,v_salinity                (salinity               ,UC_dS_m                ,LABEL_salinity            ,3,0.0 ,0.0,20.0,0.0,50.0 ,TU_dS_m,TL_Salinity,TL_Irrigation )                          \
,v_ECw_to_TDS              (ECw_to_TDS                ,UC_factor              ,"ECw_to_TDS"              ,3,0.64,0.0, 1.0,0.0, 2.0 ,"factor","ECw (dS/m) -> TDS (kg/m�) conversion factor", "Eletrical conductivity in water (dS/m) to total dissolved solids (l/g or kg/m�). Usually about 0.64." )   \
,v_max_allowable_depletion (max_allowable_depletion,U_units_not_specified  ,LABEL_max_allowable_depletion ,3,0.5, 0.0,1.0, 0.0,1.0 ,TU_0_1,TL_Maximum_allowable_depletion)                    \
,v_depletion_observe_depth (depletion_observe_depth_m ,UC_meter               ,LABEL_depletion_observe_depth ,2,1.0, 0.0,2.0, 0.0,4.0,TU_m ,TL_Depletion_observation_depth)                      \
,v_depletion_observe_root_zone_fract(depletion_observe_root_zone_fract,UC_fraction,"depletion_observe_root_zone_fract",2,1.0,0.0,2.0,0.0,4.0,"TU_0_2","Fraction of current root depth")          \
,v_min_application         (min_application_mm        ,UC_mm                  ,"min_application"         ,2,    0.0,0.0,200.0,0.0,1000.0,TU_mm,"Irrigation system minimum application capacity") \
,v_max_application         (max_application_mm        ,UC_mm                  ,LABEL_max_application     ,2,99999.0,0.0,200.0,0.0,1000.0,TU_mm,"Irrigation system maximum application capacity") \
,v_leaf_water_potential    (leaf_water_potential      ,UC_J_kg                ,"leaf_water_potential"    ,1,-1000.0,-5000.0,0.0,-10000.0,0.0,"-J/kg","Leaf water potential")


/* Hint:  Irrigation will occur when the leaf water potential becomes more negative than this value. */

//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
Irrigation_operation::Irrigation_operation(CropSyst_Op  irrig_or_auto_irrig_code )
: Irrigation_operation_class(irrig_or_auto_irrig_code)
IRRIGATION_CONSTRUCTOR_INITIALIZATION
{}
//_Irrigation_operation:constructor_________________________________2004-05-11_/
Automatic_irrigation_mode::Automatic_irrigation_mode()                           //990224
:Automatic_irrigation_mode_class()
IRRIGATION_CONSTRUCTOR_INITIALIZATION                                            //120503
{}
//_Automatic_irrigation_mode________________________________________2012-05-03_/
#define IRRIGATION_SETUP_VALID_PARAMETERS                                      \
   data_rec.expect_float32(LABEL_amount,amount_mm);                            \
   data_rec.expect_valid_float32(v_net_irrigation_mult);                       \
   data_rec.expect_valid_float32(v_refill_point);                              \
   data_rec.expect_valid_float32(v_salinity);                                  \
   data_rec.expect_valid_float32(v_NO3_N_concentration);                       \
   data_rec.expect_valid_float32(v_NH4_N_concentration);                       \
   data_rec.expect_valid_float32(v_leaf_water_potential);                      \
   data_rec.expect_valid_float32(v_max_allowable_depletion);                   \
   data_rec.expect_valid_float32(v_depletion_observe_depth);                   \
   data_rec.expect_valid_float32(v_depletion_observe_root_zone_fract);         \
   data_rec.expect_valid_float32(v_min_application);                           \
   data_rec.expect_valid_float32(v_max_application);                           \

/*200225
   data_rec.expect_valid_float32(v_surface_evaporation_area_fraction);         \
   data_rec.expect_valid_float32(v_profile_volume_fraction);
*/

//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
void Irrigation_operation::setup_parameters(Data_record &data_rec,bool for_write)
{  Irrigation_operation_class::setup_parameters(data_rec, for_write);
   IRRIGATION_SETUP_VALID_PARAMETERS                                             //120503
   data_rec.expect_float32("wetting_surface",wetting_area_surface_fraction);     //200226
}
//_Irrigation_operation::setup_parameters___________________________1998-10-07_/
void Automatic_irrigation_mode::setup_parameters(Data_record &data_rec,bool for_write)
{  Automatic_irrigation_mode_class::setup_parameters(data_rec, for_write);
   IRRIGATION_SETUP_VALID_PARAMETERS;
}
//_Irrigation_operation::setup_parameters___________________________2012-05-03_/
void Irrigation_operation::log(std::ostream &log_file)                     const
{  Irrigation_operation_class::log(log_file);
   char buffer[512];
   if (amount_mm > 0.000001)
      log_file << v_amount.log_cstr(buffer);                                     //030123
   log_file << v_salinity.log_cstr(buffer);                                      //030123
   log_file << v_NO3_N_concentration.log_cstr(buffer);                           //080213
   log_file << v_NH4_N_concentration.log_cstr(buffer);                           //080213
   log_file << v_net_irrigation_mult.log_cstr(buffer);                           //030123
   log_file << v_refill_point.log_cstr(buffer);                                  //051228
}
//_Irrigation_operation::log___________________________________________________/
void Automatic_irrigation_mode::log(std::ostream &log_file)                const
{  Automatic_irrigation_mode_class::log(log_file);
   char buffer[255];
   { log_file << v_max_allowable_depletion.log_cstr(buffer);
     log_file << v_depletion_observe_depth.log_cstr(buffer);
   }
   log_file << v_min_application.log_cstr(buffer);
   log_file << v_max_application.log_cstr(buffer);
}
//_Automatic_irrigation_mode::log___________________________________2002-08-10_/
N_application_soil_observation_mode::N_application_soil_observation_mode()
: N_application_soil_observation_mode_class()
,v_target_yield_N_uptake             (target_yield_N_uptake_kg_ha            ,UC_kg_ha    ,LABEL_target_yield_N_uptake              ,2,  0.0,0.0,99999.9,0.0,99999.9,TU_kg_ha,TL_Crop_N_uptake_for_target_yield)  //   kg/ha  Crop N uptake for target yield //990208
,v_estimated_mineralization          (estimated_mineralization_kg_ha         ,UC_kg_ha    ,"estimated_mineralization"               ,2,  0.0,0.0,99999.9,0.0,99999.9,TU_kg_ha,"Estimated mineralization")  //   kg/ha  Crop N uptake for target yield //120712
#if ((CROPSYST_VERSION > 0) && (CROPSYST_VERSION <=4))
,v_critical_soil_N_for_no_response   (critical_soil_N_for_no_response_kg_ha  ,UC_kg_ha    ,LABEL_critical_soil_N_for_no_response  ,2,  0.0,0.0,99999.9,0.0,99999.9,TU_kg_ha,TL_Critical_soil_N)//{ kg/ha } //990208_
#endif
   //120710 this is only in V4
,v_fertilizer_use_efficiency         (fertilizer_use_efficiency              ,UC_percent  ,LABEL_fertilizer_use_efficiency          ,1,100.0,0.0,  100.0,0.0,  100.0,TU_percent,TL_Fertilizer_use_efficiency)//        % 0-100  /990208
,v_soil_N_sampling_depth             (soil_N_sampling_depth                  ,UC_meter    ,LABEL_sampling_depth                     ,2,  1.0,0.0,    3.0,0.0,    4.0,TU_m,TL_Soil_N_sampling_depth)//            m  //040715
{}
//_N_application_soil_observation_mode:constructor_____________________________/
void N_application_soil_observation_mode::setup_parameters(CORN::Data_record &data_rec,bool for_write)
{  N_application_soil_observation_mode_class::setup_parameters(data_rec,for_write);
   data_rec.expect_valid_float32(v_target_yield_N_uptake);                       //990208
   data_rec.expect_valid_float32(v_estimated_mineralization);                    //120712
   #if ((CROPSYST_VERSION > 0) && (CROPSYST_VERSION <=4))
   data_rec.expect_valid_float32(v_critical_soil_N_for_no_response);             //990208
   #endif
   data_rec.expect_valid_float32(v_fertilizer_use_efficiency);                   //990208
}
//_N_application_soil_observation_mode::setup_parameters____________1997-07-09_/
void N_application_soil_observation_mode::log(std::ostream &log_file)    const
{  N_application_soil_observation_mode_class::log(log_file);
   char buffer[255];
   log_file << v_target_yield_N_uptake.log_cstr(buffer);
   log_file << v_estimated_mineralization.log_cstr(buffer);
   #if ((CROPSYST_VERSION > 0) && (CROPSYST_VERSION <=4))
   log_file << v_critical_soil_N_for_no_response.log_cstr(buffer);  //<- This is only in V4
   #endif
   log_file << v_soil_N_sampling_depth.log_cstr(buffer);
   log_file << v_fertilizer_use_efficiency.log_cstr(buffer);
}
//_N_application_soil_observation_mode::log_________________________2002-03-11_/
Management_parameters::Management_parameters()
: Management_parameters_class()
#ifndef __linux__
#ifdef NYI
,V3_auto_irrig_enabled                 (0)                                       //020922
,V3_clipping_operations                ()                                        //040524
,V3_harvest_date(0) // used only for V3 import (will be 0 if not V3)             //040524
,V3_auto_clip_biomass_event(0)                                                   //040701
,V3_latest_date_to_harvest_relative((Year)0,(CORN::DOY)0,D_YMD,D_YY| D_lead_zero //990421| D_Mmm|D_relative,'/')
#endif
#endif
,organic_nitrogen_operations           ()                                        //981228
,v_soil_conservation_factor         (soil_conservation_factor,UC_factor ,  LABEL_soil_conservation_factor ,3,DEFAULT_soil_conservation_factor , 0.0,1.0, 0.0,1.0 ,TU_0_1,TL_RUSLE_soil_conservation_practice_factor)
{}
//_Management_parameters:constructor________________________________2011-01-XX_/
bool Management_parameters::expect_structure(bool for_write)
{  bool expected = Management_parameters_class::expect_structure(for_write);     //161025_120314
   structure_defined = false;                                                    //120314
   set_current_section(LABEL_management);
   expect_valid_float32(v_soil_conservation_factor);
// g++ compiler was having problems with friendship so prior version importing is not supported in Linux
      set_current_section("automatic_nitrogen");                                                         //040712
         expect_valid_float32(N_application_soil_observation_event->v_target_yield_N_uptake);            //040712
         expect_valid_float32(N_application_soil_observation_event->v_estimated_mineralization);         //120712
         #if ((CROPSYST_VERSION > 0) && (CROPSYST_VERSION <=4))
         expect_valid_float32(N_application_soil_observation_event->v_critical_soil_N_for_no_response);  //040712
         // This is only in V4
         #endif
         expect_valid_float32(N_application_soil_observation_event->v_fertilizer_use_efficiency);        //040712

      set_current_section("soil_N_sampling");                                                            //040712
         expect_valid_float32(N_application_soil_observation_event->v_soil_N_sampling_depth);            //040712
   set_current_section(LABEL_management);

   EXPECT_ENUMERATED_SECTION(LABEL_residue,residue_operation_section_VV,residue_operations); //020617
   // I am not sure why I had residue here,
   // It looks like I may have been planing to make residue ops obsolete
   // in V5 but I am not sure why, the residue_operation
   // were still in the class
   #if ((CROPSYST_VERSION > 0) && (CROPSYST_VERSION < 5))
   EXPECT_ENUMERATED_SECTION(LABEL_organic_fertilization ,organic_nitrogen_operation_section_VV    ,organic_nitrogen_operations);//020617
   #endif
   structure_defined = true;                                                     //120314
   return expected;
}
//_Management_parameters::expect_structure__________________________2011-01-XX_/
Chemical_operation::Chemical_operation()
:Chemical_operation_class()
,v_chemical_concentration  (chemical_concentration,UC_kg_m3,LABEL_concentration     ,3,0, 0.0,  10.0, 0.0, 100.0,TU_kg_m3,TL_Concentration  )
{  name = "";                                                                    //120419
}
//_Chemical_operation:constructor______________________________________________/
bool Management_parameters::get_end()
{
#ifndef __linux__
// g++ compiler is having problems with friendship so prior version importing is not supported in Linux

//             Take version 3 auto irrig parameters to create a new event
//             compatible with version 4.
#ifdef NYI
   if (V3_auto_irrig_enabled && V3_auto_irrig)
   {  auto_irrigation_mode_operations.append(V3_auto_irrig);
//                Do not delete V3_auto_irrig we just dont keep the global pointer set.
   }
   V3_auto_irrig = 0;
#endif
#if (CROPSYST_VERSION >= 5)
/*
When we go to version 5 SCS codes will be obsolete but
but I need to
convert the SCS_code to NRCS field operation
I haven't implemented this yet because I dont yet know what are the
corresponding operations.

if there is SCS_code and new NRCS field operation
*/
#endif
   FOR_EACH_IN(harvest_or_clipping_event,Harvest_or_clipping_event,harvest_or_clipping_operations,each_clip_event) //060310
   {  // To read parameters from old param files (These are all from V4)
         if (harvest_or_clipping_event->harvest_amount_mode == HARVEST_RETAIN_GAI_obsolete)
            if (harvest_or_clipping_event->retain_GAI_obsolete_V4_3!=0.0)
               harvest_or_clipping_event->min_retain_GAI = harvest_or_clipping_event->retain_GAI_obsolete_V4_3;
   } FOR_EACH_END(each_clip_event)
#endif
   return true;
}
//_Management_parameters::get_end___________________________________2001-01-09_/
DEFINE_Biomatter_application_operation
}//_namespace CropSyst_________________________________________________________/
#endif

