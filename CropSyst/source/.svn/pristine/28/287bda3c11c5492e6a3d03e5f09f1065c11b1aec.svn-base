#include "crop/crop_param_class.h"
#include "corn/math/compare.hpp"
#include "biomatter/decomposition_const.h"
#include "residue/residue_const.h"
#include "corn/metrology/units_code.hpp"
#include "static_phrases.h"

#ifndef VIC_CROPSYST
#include "common/simulation/event.h"
#endif
#include "csvc.h"

//______________________________________________________________________________
using namespace CORN;
extern bool LADSS_mode;
namespace CropSyst {
//#ifndef YAML_PARAM_VERSION
bool Crop_parameters_class::Adjustment::
setup_structure(CORN::Data_record &data_rec,bool for_write)
{  data_rec.expect_string("parameter",parameter,255);
   data_rec.expect_string("value",value,255);
   return true;
}
//#endif
//______________________________________________________________________________
const char *Crop_parameters_class::Adjustment_event::label_string(std::string &buffer) const
{
   begin_sync.append_to_string(buffer _ISO_FORMAT_DATE_TIME);
   buffer += ":";
   buffer += parameter;
   buffer += "=";
   buffer += value;
   return buffer.c_str();
}
//_Adjustment_event::label_string___________________________________2017-04-23_/
// once I implement parameter adjustments idiom (V5) restore this back only V4
DECLARE_CONTAINER_ENUMERATED_SECTION(Crop_parameters_class::Adjustment_event, adjustment_section_VV,true);
//______________________________________________________________________________
//#endif
//______________________________________________________________________________
Crop_parameters_class::Thermal_time::Thermal_time()
: Crop_parameters_struct::Thermal_time()                                         //021124
, thermal_response_cowl(thermal_response,linear)                                 //151109
{
   temperature.base     = 3.0;
   temperature.cutoff   = 22.0;                                                  //990227
   temperature.opt      = 22.0;                                                  //021124
   temperature.max      = 35.0;                                                  //021124
   thermal_response_cowl.set(linear);                                            //151109
}
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
bool Crop_parameters_class::Thermal_time::setup_structure(Data_record &record,bool for_write)    modification_
{  record.set_current_section("phenology");
   record.expect_enum("thermal_response",thermal_response_cowl);                 //151109
   return true;
}
//_Thermal_time::setup_structure________________________________________________
Crop_parameters_class::Transpiration::Transpiration()
: Crop_parameters_struct::Transpiration()
{  normal_max_water_uptake_mm =   10.0;
   LWP_wilt_point             =-1600.0;
   ET_coef                    =    1.0;
   LWP_stomatal_closure       = -700.0;                                          //020418 (110208 was stomatal_closure_leaf_water_pot)
   /*190411 since Jordi has passed on, these will probably never be used again
   a1 = 0;  //121114 special temporary paramters for Jordi to test
   a2 = 0;  // 0.0 values disable this.
   */
}
//_Transpiration:constructor________________________________________2011-02-08_/
bool Crop_parameters_class::Transpiration::setup_structure(Data_record &record,bool for_write)    modification_
{
/*190411 since Jordi has passed on, these will probably never be used again
   //121114 special temporary paramters for Jordi to test
   record.set_current_section("Jordi");
      record.expect_float32("a1",a1);
      record.expect_float32("a2",a2);
*/
   record.set_current_section("transpiration");
   return true;
}
//_Transpiration::setup_structure______________________________________________/
Crop_parameters_class::Biomass_production::Biomass_production()
: Crop_parameters_struct::Biomass_production()
#if (CROPSYST_VERSION< 6)
,TUE_equation_cowl_obsolete(TUE_equation_obsolete,TUE_curve)                     //191108
#endif
   //191108,TUE_Tanner_Sinclair)  // Default to the original model.         //080408

#if (CROPSYST_VERSION==4)
, RUE_basis_cowl_obsolete   (RUE_basis_obsolete,unspecified_RUE_basis)           //080728
#endif

{  WUE_changes                =false;                                            //080508
   temperature_limitation.min          =  3.0;                                   //191025
   temperature_limitation.low_optimum  = 10.0;                                   //191025
   temperature_limitation.high_optimum = 25.0;                                   //191025
   temperature_limitation.max          = 45.0;                                   //191025
/*191025
   min_tolerable_temp         =  3.0;                                            //130531
   max_tolerable_temp         = 45.0;                                            //130508
   low_threshold_limit_temp   = 10.0;                                            //130508
   high_threshold_limit_temp  = 25.0;                                            //130508
   RUE_PAR_deprecated         =3.0;                                              //110829
*/

// WARNING need new default values from Claudio   191026
   efficiency_model_raw = "TUE";                                                 //191026
   efficiency_model = TUE_efficiency;                                            //191026
   RUE[at_low_VPD] = RUE[at_low_total_solar_rad] = 4.0;                          //191026
   RUE_efficiency_decrease_rate = 0.0;                                           //191026
   //191031 RUE_global_obsolete                 =4.0;                                     //110829_040428
   continuous_cultivation_impact_factor = 1.0;

   #ifdef YAML_PARAM_VERSION
   // eventually all all versions will use the new adjustments
   #else
   for (int j = 0; j < 2; j++)                                                   //080506
    {  for (int season = 0; season <= 5; season++)                               //080428
         season_biomass_adjustment[season] = 1.0;                                //080731
      TUE_at_1kPa_VPD                     [j] = 5.0;                             //080505
      /*191026 Aquacrop and TannerSinclair now obsolete
      water_productivity                  [j] =20.0;  // this is not a good default use 20 for c3, 30 for c4  //080505
      reference_biomass_transpiration_coef[j] = 5.0;                             //040729
      */
      TUE_scaling_coef                    [j] = 0.45;                            //091201
   }
   #endif
}
//_Biomass_production:constructor___________________________________2011-02-08_/
bool Crop_parameters_class::Biomass_production::setup_structure
(Data_record &record,bool for_write)                               modification_
{  record.set_current_section(LABEL_growth);                                     //020405
   record.expect_bool("WUE_changes",WUE_changes);                                //080508
      // Note that the thermal time for the WUE change is in thermal time parameters  080514
   record.expect_string("efficiency_model",efficiency_model_raw,32);             //191026

   // Support for reading OLD versions
   #if ((CS_VERSION > 0) && (CS_VERSION < 5))
   record.expect_enum("TUE_equation",TUE_equation_cowl_obsolete);                //080505
   #if (CROPSYST_VERSION==4)
   record.expect_enum("RUE_basis",RUE_basis_cowl_obsolete);                      //110825
   #endif
   if (!for_write)
   {
      record.expect_float32("RUE_global"    ,RUE[at_low_VPD]);                   //191031
      record.expect_float32("RUE_global"    ,RUE[at_low_total_solar_rad]);       //191031
      // record.expect_float32("RUE_global"    ,RUE_global_obsolete);            //191030
   }
   record.expect_float32("min_tolerable_temp"         ,temperature_limitation.min);
   record.expect_float32("low_threshold_limit_temp"   ,temperature_limitation.low_optimum);
   record.expect_float32("high_threshold_limit_temp"  ,temperature_limitation.high_optimum);
   record.expect_float32("max_tolerable_temp"         ,temperature_limitation.max);
   #endif

   return true;
}
//_Biomass_production::setup_structure______________________________2011-02-08_/
Crop_parameters_class::Vernalization::Vernalization()
:Crop_parameters_struct::Vernalization()
{  low_temp    = 3.0;
   high_temp   =10.0;
   start       =10.0;                                                            //011016
   end         =50.0;
   min_factor  = 0.0;
   enabled     =false;                                                           //020718
}
//_Vernalization:constructor________________________________________2002-04-08_/
bool Crop_parameters_class::Vernalization::setup_structure(Data_record &record,bool for_write)    modification_
{  record.set_current_section(LABEL_vernalization);                              //020405
   record.expect_bool(LABEL_enabled,enabled );                                   //021015
   return true;
}
//_Vernalization::setup_structure______________________________________________/
Crop_parameters_class::Phenology::Phenology()
: Crop_parameters_struct::Phenology()
{
   initiation.emergence            =    (100);                                   //990228
   initiation.tuberization         =   (1000);
   initiation.flowering            =   (1000);
   initiation.filling              =   (1020);
   initiation.maturity             = (1500.0);                                   //990228

   culmination_obsolete.accrescence         =      (0); //190627 (1040);
   culmination_obsolete.senescence_obsoleteX =      (0); //190627 (1501.0);                //131024
   culmination_obsolete.root_elongation     =   (1040);                                   //070321
   culmination_obsolete.filling             =   (1300);                                   //030521
   maturity_significant            =   (true);                                   //080319
   clipping_resets                 =  (false);                                   //040719

   // accrescence and senescence are now relative to emergance not planting

   initiation.senescence_obsoleteX = 0;                                          //200125
   initiation.senescence            =  9999999; //200125 obsolete    980;        //190630
      // will be removed when new period specification is fully implemented

   duration.accrescence             =        0;                                   //190630
   duration.culminescence           =      100;                                   //191121
   duration.senescence              =    500.0;                                   //190630
   duration.root_elongation         =     1040;                                   //191121

      // clipping_resets may be true for all crops, but I am
     // leaving it false for compatibility with old alfalfa.
   //181110 #endif

   #ifdef YAML_PARAM_VERSION
   // eventually all versions will use parameter adjustments
   #endif
   #if (CROPSYST_VERSION==4)
   WUE_change                          =   (1000);                               //080514
   #endif
   phenologic_sensitivity_water_stress =    (1.0);                               //020410
}
//_Crop_parameters_class::Phenology::Phenology______________________2008-07-22_/
#if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
void Crop_parameters_class::Phenology::offset_by(int16 offset_deg_day)
{
   initiation.emergence            += offset_deg_day;
   // This is currently alias, dont increment begin_shoot_growth_deg_day        //020405
   initiation.tuberization                += offset_deg_day;
   culmination_obsolete.accrescence         += offset_deg_day;
   culmination_obsolete.root_elongation     += offset_deg_day;                   //070321
   initiation.flowering            += offset_deg_day;
   initiation.filling              += offset_deg_day;          // eventually remove the f32 suffix
   initiation.maturity             += (float32)offset_deg_day; // needs to be float to allow very larger numbers for long term perennials such as trees  //050511
   culmination_obsolete.filling             += (float32)offset_deg_day; // needs to be float to allow very larger numbers for long term perennials such as trees
}
//_Phenology::offset_by_____________________________________________2003-05-21_/
#endif
bool Crop_parameters_class::Phenology::setup_structure
(Data_record &record,bool for_write)                               modification_
{  record.set_current_section(LABEL_phenology);                                  //020405
   bool satup = true;
   record.expect_bool("maturity_significant",maturity_significant);              //080319
   record.expect_bool("clipping_resets",     clipping_resets);                   //040719
   if (!for_write)
   {
      record.expect_float32("peak_LAI"   ,culmination_obsolete.accrescence);     //190628
      record.expect_float32("senescence"     ,initiation.senescence_obsoleteX);  //190628
      record.expect_float32("full_senescence",culmination_obsolete.senescence_obsoleteX); //190628
   }
   record.expect_float32("duration.accrescence" ,duration  .accrescence);        //190627
   record.expect_float32("duration.culminescence",duration .culminescence);      //191121
   record.expect_float32("duration.senescence"  ,duration  .senescence);         //190627


   record.expect_float32("duration.root_elongation",duration.root_elongation);   //200603

   return satup;
}
//_Phenology::setup_structure__________________________________________________/
Crop_parameters_class::Photoperiod::Photoperiod()
: Crop_parameters_struct::Photoperiod()                                          //100512
, photoperiodism_cowl(photoperiodism)                                            //151122_100512
{  photoperiodism = no_photoperiod;                                              //151122_100512
   stop_day_length          = 0.0;                                               //120516
   unconstrained_day_length = 0.0;                                               //120516
}
//_Photoperiod_constructor__________________________________________2002-04-08_/
bool Crop_parameters_class::Photoperiod::setup_structure(Data_record &record,bool for_write)    modification_
{  record.set_current_section(LABEL_photoperiod);                                //020405
   record.expect_enum("photoperiodism",photoperiodism_cowl);                     //151122_100513
   if (!for_write)
      // Support or old files
      record.expect_enum("consideration",photoperiodism_cowl);                   //100513
   return true;
}
//_Photoperiod::setup_structure_____________________________________2010-05-13_/
Crop_parameters_class::Emergence_hydro_thermal::Emergence_hydro_thermal()
: emergence_model_cowl(emergence_model)
{  emergence_model = EMERGENCE_THERMAL_TIME;
   seedling_leaf_area_cm2        =     2;
   germination_base_temperature  =    -1;  // WARNING need reasonable default
   mean_base_soil_water_pot_J_kg = -1000;  // WARNING need reasonable default
   stdev_base_soil_water_pot_J_kg=   100;  // WARNING need reasonable default
   a_slope                       = 22.35;
   b_intercept                   =129.66;
}
//_Emergence_hydro_thermal_parameters_class:constructor_____________2004-12-01_/
bool Crop_parameters_class::Emergence_hydro_thermal::setup_structure
(Data_record &record,bool for_write)                               modification_
{  record.set_current_section("emergence");
   record.expect_enum("model",emergence_model_cowl);                             //100520
   return true;
}
//_Crop_parameters_class::Emergence_hydro_thermal::setup_structure__2004-12-01_/
Crop_parameters_class::Quiescence::Quiescence()
{  consider_inactive_days  =7;                                                   //080401
   inducement_temperature  =10;
}
//_Quiescence:constructor___________________________________________2008-04-01_/
bool Crop_parameters_class::Quiescence::setup_structure
(Data_record &record,bool for_write)                               modification_
{  // End season or dormancy inactivity considieration
   record.set_current_section("inactive_period");                                //080331
   return true;
}
//_Quiescence::setup_structure______________________________________2010-05-11_/
Crop_parameters_class::Dormancy::Dormancy()
{  //050715 Note I eventually want to create a degree_day unit (see not in units.h)
   chill_hours                         = 400.0;
   senesce                             = 100.0;
   deg_day_bud_break_chill_req_sat     = 100;                                    //030515
   deg_day_bud_break_chill_req_not_sat = 150;                                    //030515
}
//_Dormancy:constructor_____________________________________________2002-04-08_/
bool Crop_parameters_class::Dormancy::setup_structure
(Data_record &record,bool for_write)                               modification_
{  record.set_current_section(LABEL_dormancy);                                   //020408
   return true;
}
//_Crop_parameters_class::Dormancy::setup_structure_________________2010-06-10_/
Crop_parameters_class::Root::Root()
: Crop_parameters_struct::Root()                                                 //100720
{  max_depth                        = 1.5;
   surface_density                  = 3.0;                                       //030722
   density_distribution_curvature   = 0.1;                                       //030722
   sensitivity_to_water_stress      = 0.0;                                       //061212
   #if (CS_VERSION==4)
   length_per_unit_mass_km_kg_V4    =90.0;                                       //060612
   length_at_emergence_cm           = 2.0;                                       //110218
   #else
   length_at_emergence_cm           = 1.2;                                       //110218
   #endif
   root_shoot_emergence_ratio       = 2.0;                                       //130624
   root_shoot_full_ratio            = 0.1;                                       //130624
//180727    sow_depth                        = 0.08;                                      //110218 currently only by VIC?
   initial_depth_deprecated         = 0; //180728 non 0 now indicates file uses deprecated value  = 0.2;  //120817 The depth of the roots at surface biomass emergence
}
//_Dormancy::setup_structure________________________________________2006-12-12_/
bool Crop_parameters_class::Root::setup_structure
(Data_record &record,bool for_write)                               modification_
{  record.set_current_section("root");                                           //020408
      if (!for_write)
         record.expect_float32("initial_depth", initial_depth_deprecated);          //120817
            // "cm (depth at emergence)"
   return true;
}
//_Root::setup_structure____________________________________________2010-06-10_/
Crop_parameters_class::Morphology::Morphology()
: Crop_parameters_struct::Morphology()
,canopy_growth_cowl(canopy_growth_model,leaf_area_index_based)                   //080725
{  max_canopy_height             =1.5;                                           //990213
   reduce_canopy_expansion_LWP   =-800.0;                                        //080506
   stop_canopy_expansion_LWP     =-1200.0;                                       //080506
   light_extinction_coef/*191106 kc */ =0.5;                                     //110527_990228 was under transpiration
   canopy_architecture = false;
}
//_Morphology:constructor___________________________________________2010-02-19_/
bool Crop_parameters_class::Morphology::setup_structure(Data_record &record,bool for_write) modification_
{  record.set_current_section("crop");
      record.expect_enum("canopy_growth",canopy_growth_cowl);                    //080725
   record.set_current_section("morphology");
   record.expect_bool("canopy_architecture",canopy_architecture);                //190627
   return true;
}
//_Morphology::setup_structure______________________________________2010-02-19_/
Crop_parameters_class::Canopy_growth_cover_based::Canopy_growth_cover_based()
: Crop_parameters_struct::Canopy_growth_cover_based()
{  cover_initial           = 0.01;                                               //080723
   cover_maximum_no_CO2_adj= 0.9;                                                //080723
   cover_total_season_end  = 0.1;                                                //171219_080723
   cover_green_season_end  = 0.0;                                                //171219_171009
   #if (!defined(YAML_PARAM_VERSION) || (CROPSYST_VERSION==4))
   thermal_time_season_end_obsolete = 0; //1080.0;                                    //171009
   #endif
      //to be compatible with previous crop file X was scenescence
   thermal_time_fractional_at_half_cover_max = 0.5;                              //181017
   thermal_time_fractional_at_half_cover_senescence = 0.5;                       //181017
   shape_coef_accrescent = 9.0;                                                  //171221
   shape_coef_senescent  = 9.0;                                                  //171221
}
//_Canopy_growth_cover_based:constructor____________________________2010-07-30_/
bool Crop_parameters_class::Canopy_growth_cover_based::setup_structure
(Data_record &record,bool for_write)                               modification_
{  record.set_current_section("canopy_cover");                                   //020408
   //moved to V4 and V5
   /*
   record.expect_float32("green_season_end",cover_green_season_end);             //171219
   record.expect_float32("thermal_time_season_end",thermal_time_season_end  ); //171219
      // Warning thermal_time_season_end may be moved to to phenology   171221
   */
   if (!for_write)
   { // These have been renamed (allow import from old files)
   record.expect_float32("CC_green_X",cover_green_season_end);                   //171219_171009
   #if (!defined(YAML_PARAM_VERSION) || (CROPSYST_VERSION==4))
   record.expect_float32("TT_X",thermal_time_season_end_obsolete);               //171009
   #endif
   }
   return true;
}
//_Canopy_growth_cover_based::setup_structure_______________________2010-07-30_/
Crop_parameters_class::Canopy_growth_LAI_based::Canopy_growth_LAI_based()
: Crop_parameters_struct::Canopy_growth_LAI_based()
{  initial_GAI                      =0.011;                                      //000207
   regrowth_GAI                     =0.011;                                      //080207
   max_LAI                          =5.0;                                        //990228
   specific_leaf_area               =22.0;
   fract_LAI_mature                 =0.8;
   stem_leaf_partition              =3.0;
   leaf_duration_deg_day            =800;                                        //050628
   leaf_duration_sensitivity_stress =1.0;
      // Default senescence_power_coef is linear (as prior to V 4.8.10)
   clipping_removes_newest_first    =true;                                       //071126
}
//_Canopy_growth_LAI_based:constructor_________________________________________/
bool Crop_parameters_class::Canopy_growth_LAI_based::setup_structure(Data_record &record,bool for_write) modification_
{  record.set_current_section("leaf");                                           //020408
      record.expect_bool("clip_newest",clipping_removes_newest_first);           //071126
   return true;	
}
//_Canopy_growth_LAI_based::setup_structure_________________________2010-07-30_/
Crop_parameters_class::Yield::Yield()
{  harvest_index_unstressed          =(0.50);                                    //110218
//   #ifdef YAML_PARAM_VERSION
//   stress_sensitivity        =(0.0);                                           //150914
//   #else
   stress_sensitivity_growth        =(0.0);                                      //020419
   stress_sensitivity_tuber_init    =(0.0);
   tuber_growth_sensitivity  =(0.0);
//   #endif
   translocation_fraction_max = 0.4;                                             //110907
   apply_adjustment_to_unstressed_harvest_index = true;                          //110907
   //200423 apply_heat_extreme_adjustment_to_unstressed_harvest_index = false;            //150206
   apply_temperature_extreme_response     = false;                               //200423
   apply_temperature_response_flowering   = false;                               //200423
   apply_water_response_flowering         = false;                               //200423
   water_stress_sensitivity               =1.0;                                  //200424
   temperature_stress_sensitivity         =1.0;                                  //200424
   this->response_temperature_optimum  = 25.0; //200424 claudio needs to provide
   this->response_temperature_damaging = 35.0; //200424 claudio needs to provide
   editor_grain_stress_sensitivity.set_default();                                //130425
}
//_Harvest:constructor_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _/
#if (VIC_CROPSYST_VERSION != 2)
bool Crop_parameters_class::Yield::Grain_stress_sensitivity_period
::setup_structure(CORN::Data_record &data_rec,bool for_write)
{
   bool satup = Simple_event::setup_structure(data_rec,for_write);
   data_rec.expect_float32("cold_stress_no_stress_threshold_temperature"  ,cold.no_stress_threshold_temperature );
   data_rec.expect_float32("cold_stress_max_stress_threshold_temperature" ,cold.max_stress_threshold_temperature);
   data_rec.expect_float32("cold_stress_sensitivity"                      ,cold.sensitivity                     );
   data_rec.expect_float32("cold_stress_exposure_damaging_hours"          ,cold.exposure_damaging_hours         );
   data_rec.expect_int16  ("cold_stress_exposure_initial_hour"            ,cold.exposure_initial_hour           ); //130628

   data_rec.expect_float32("heat_stress_no_stress_threshold_temperature"  ,heat.no_stress_threshold_temperature );
   data_rec.expect_float32("heat_stress_max_stress_threshold_temperature" ,heat.max_stress_threshold_temperature);
   data_rec.expect_float32("heat_stress_sensitivity"                      ,heat.sensitivity                     );
   data_rec.expect_float32("heat_stress_exposure_damaging_hours"          ,heat.exposure_damaging_hours         );
   data_rec.expect_int16("heat_stress_exposure_initial_hour"              ,heat.exposure_initial_hour           ); //130628
   return satup;
}
#endif
//_Harvest::Grain_stress_sensitivity_period::setup_structure________2013-04-29_/
Crop_parameters_class::Fruit::Fruit()
: harvest_condition_mode_cowl(harvest_condition_mode,management_scheduled_harvest)
{  fract_total_solids                     =0.35;                                 //000635
   max_fruit_load_kg_ha                   =20000;                                //000616
   /*180601 was for Jordi but never implemented
   initial_fruit_mass                     =0.02;                                 //071127
   initial_reserves_release_rate          =0.02;                                 //071127
   clusters_per_vine                      =30;                                   //071127
   berries_per_cluster                    =25;                                   //071127
   fruits_per_tree                        =250;                                  //071127
   */
//moved above   harvest_condition_mode_labeled         =management_scheduled_harvest;         //081204
   harvest_temperature                    =15;                                   //081204
   harvest_temperature_days               =5;                                    //081204
   harvest_DOY_earliest                   =258;                                  //081204
   harvest_DOY_latest                     =275;                                  //081204
   #ifdef YAML_PARAM_VERSION
   growth_fraction_to_fruit       =0.9;                                          //150914
   #else
   initial_growth_fraction_to_fruit       =0.9;                                  //030521
   rapid_growth_fraction_to_fruit         =0.65;                                 //030521
   max_water_uptake_before_fruiting_mm       =10.0;                              //091202
   max_water_uptake_during_initial_fruit_mm  =10.0;                              //091202
   max_water_uptake_during_radid_fruit_mm    =10.0;
   max_water_uptake_during_veraison_mm       =10.0;                              //091202
   max_water_uptake_after_maturity_mm        =10.0;                              //091202
   #endif
   grape=false;                                                                  //021206
}
//_Fruit:constructor________________________________________________2002-04-08_/
bool Crop_parameters_class::Fruit::setup_structure(Data_record &record,bool for_write) modification_
{  record.set_current_section(LABEL_fruit);                                      //020405
   return true;
}
//_Fruit::setup_structure___________________________________________2011-03-26_/
#ifdef CHECK_NEEDED
110127 probably is not but dont delete yet
Crop_parameters::Fruit::Fruit(const Fruit &copy_from)
{
fract_total_solids                    =copy_from.fract_total_solids;
max_fruit_load_kg_ha                  =copy_from.max_fruit_load_kg_ha; // Warning need good value
grape                                 =copy_from.grape;
initial_fruit_mass                    =copy_from.initial_fruit_mass;
initial_reserves_release_rate         =copy_from.initial_reserves_release_rate;
clusters_per_vine                     =copy_from.clusters_per_vine;
berries_per_cluster                   =copy_from.berries_per_cluster;
fruits_per_tree                       =copy_from.fruits_per_tree;
initial_growth_fraction_to_fruit      =copy_from.initial_growth_fraction_to_fruit;
rapid_growth_fraction_to_fruit        =copy_from.rapid_growth_fraction_to_fruit;

harvest_condition_mode_labeled        =copy_from.harvest_condition_mode_labeled.get();
harvest_temperature_days              =copy_from.harvest_temperature_days)
harvest_temperature                   =copy_from.harvest_temperature)
harvest_DOY_earliest                  =copy_from.harvest_DOY_earliest)
harvest_DOY_latest                    =copy_from.harvest_DOY_latest)

max_water_uptake_before_fruiting_mm       =copy_from.max_water_uptake_before_fruiting_mm)
max_water_uptake_during_initial_fruit_mm  =copy_from.max_water_uptake_during_initial_fruit_mm)
max_water_uptake_during_radid_fruit_mm    =copy_from.max_water_uptake_during_radid_fruit_mm)
max_water_uptake_during_veraison_mm       =copy_from.max_water_uptake_during_veraison_mm)
max_water_uptake_after_maturity_mm        =copy_from.max_water_uptake_after_maturity_mm)
}

#endif
 //_____________________________________________________________________________
Crop_parameters_class::Crop_parameters_class()
:Common_parameters_data_record(NO_ASSOCIATED_DIRECTORY,LABEL_crop,CS_VERSION)    //180626 CS_VERSION_NUMBERS) //050323
, sowing_event_ID                                                           ("")
   // sowing_event_ID Will be assigned for LADSS when parameters are instanciated for sowing event   //051129
//TEMP DISABLED /*031121_*/ ,cropgro                (new CropGro_parameters)
,crop_model_clad                                           (CROPSYST_CROP_MODEL) //020508
,harvested_part_clad                                              (UNKNOWN_PART) // Must be unknown else we can't load option parameters  //020508
,photosynthetic_pathway_clad                                                (C3) //110504
,life_cycle_clad                                             (annual_life_cycle) //020716
,senesces                                                                 (true) //040808
,land_use_clad                                                       (ROW_CROPS)
// I think these defaults will be effectively the same as the original
//050715  Note, I haven't assigned units codes to all these.  It is not critical yet
// Warning I need a range of values from Claudio
,parameter_editor_mode                                                   (false) //030520
   //Note these are grams of biomass per kg water (NOT PPM)
,linger_days                                                                (90)
#ifndef VIC_CROPSYST
,has_max_water_uptake_adjustments                                        (false) //120725
#endif
/* abandoned
#ifdef LIU_XIAO_CROP
,parameters_Liu_Xiao(0)                                                          //180116
#endif
*/
{}
//_Crop_parameters_class:constructor___________________________________________/
Crop_parameters_class::~Crop_parameters_class()
{
/*abandoned
   #ifdef LIU_XIAO_CROP
   delete parameters_Liu_Xiao;
   #endif
*/
}
//_~Crop_parameters_class___________________________________________2018-01-16_/
#ifndef YAML_PARAM_VERSION
float64 Crop_parameters_class::param_TUE_at_1kPa_VPD(bool before_WUE_change) const
{  int vegetative_growth_index =
      ref_biomass_production().WUE_changes && before_WUE_change ? 1 : 0;
   return ref_biomass_production().TUE_at_1kPa_VPD[vegetative_growth_index];
}
//_param_TUE_at_1kPa_VPD____________________________________________2008-05-28_/
float64 Crop_parameters_class::param_TUE_scaling_coef( bool before_WUE_change ) const
{  int vegetative_growth_index =
      ref_biomass_production().WUE_changes && before_WUE_change ? 1 : 0;
   return ref_biomass_production().TUE_scaling_coef[vegetative_growth_index];
}
//_param_TUE_scaling_coef___________________________________________2009-12-01_/
#endif
float64 Crop_parameters_class::param_base_temp()                           const
{ return  ref_thermal_time().temperature.base;
}
//_param_base_temp__________________________________________________2011-08-23_/
bool Crop_parameters_class::is_perennial()                          affirmation_
{  Life_cycle life_cycle = (Life_cycle)life_cycle_clad.get();
   return life_cycle >= perennial_life_cycle;                                    //080722
}
//_is_perennial_____________________________________________________2002-07-16_/
bool Crop_parameters_class::is_annual()                             affirmation_
{  Life_cycle life_cycle = (Life_cycle)life_cycle_clad.get();
   return (life_cycle == annual_life_cycle);
}
//_is_annual________________________________________________________2002-07-16_/
bool Crop_parameters_class::is_fruit_tree()                         affirmation_
{  return
   (crop_model_clad.get() == CROPSYST_FRUIT_MODEL);
}
//_is_fruit_tree____________________________________________________2011-08-24_/
bool Crop_parameters_class::applies_harvest_index()                 affirmation_
{  bool apply = (crop_model_clad.get() == CROPSYST_CROP_MODEL);
   if (harvested_part_clad.get() >= HARVEST_CANOPY)   apply = false;
   if (land_use_clad.get() == PASTURE)                apply = false;
   return apply;
}
//_applies_harvest_index____________________________________________2004-06-14_/
bool Crop_parameters_class::has_leaf_senescence()                   affirmation_
{  bool any_senescence = is_deciduous() || senesces                              //040808
      || (life_cycle_clad.get() == annual_life_cycle) ;
     // May move to parameter editor
   return any_senescence;
}
//_has_leaf_senescence______________________________________________2003-05-21_/
bool Crop_parameters_class::expect_structure(bool for_write)
{  bool expected = Common_parameters_data_record::expect_structure(for_write);   //161023_021015
   structure_defined = false;                                                    //120314
   bool CROPSYST_ORCHARD = is_fruit_tree();                                      //190625
   set_current_section(LABEL_crop);
#ifdef LADSS_MODE
   expect_int32(LABEL_LADSS_land_use_ID,LADSS_land_use_ID);                      //021220
#endif
   expect_string("feed_name_immature"  ,feed_name_immature_early,50);            //021114
   expect_string("feed_name_midmature" ,feed_name_midmature_headed_normal,50);   //021114
   expect_string("feed_name_mature"    ,feed_name_mature_normal,50);             //021114
   expect_enum("model",crop_model_clad);                                         //020408
   expect_enum(LABEL_harvested_part,harvested_part_clad);                        //020405
   expect_string("RUSLE2_name"  ,RUSLE2_name,100);                               //110831
   expect_string("RUSLE2_description"  ,RUSLE2_description,100);                 //110812
   expect_enum("abscission",abscission_clad);                                    //141121

   // Options
   #if (CS_VERSION<6)
      // C_species label will be obsolete in V6
   #if (CS_VERSION==5)
      // Continue to recognized C_species when reading in V5
   if (!for_write)
   #endif
   expect_enum(LABEL_C_species,photosynthetic_pathway_clad);                     //110504_020324
   #endif
      // Eventually rename to photosynthetic_pathway
   expect_enum("photosynthetic_pathway",photosynthetic_pathway_clad);            //110504_020324
   expect_enum(LABEL_life_cycle,life_cycle_clad);                                //020716
   expect_enum(LABEL_land_use,land_use_clad);                                    //020324
   if (!for_write)                                                               //200212
      expect_enum("season_dormancy_mode",continuation_mode_clad);                //200212_080331
   expect_enum("continuation_mode",continuation_mode_clad);                      //200212_080331
   #ifndef YAML_PARAM_VERSION
   // replaced with continuous_cultivation_impact_factor 151101
   set_current_section("water_use_efficiency");                                  //080818
      expect_float32("season2_adj",mod_biomass_production().season_biomass_adjustment[2],"");   //060720
      expect_float32("season3_adj",mod_biomass_production().season_biomass_adjustment[3],"");   //060720
      expect_float32("season4_adj",mod_biomass_production().season_biomass_adjustment[4],"");   //060720
      expect_float32("season5_adj",mod_biomass_production().season_biomass_adjustment[5],"");   //060720
   #endif

// In version 5 the season adjustments will be 0 indexed (season offset)  [Need to verify this ]

   set_current_section(LABEL_leaf);                                              //020405
      expect_bool(LABEL_senesces,senesces);                                      //040808
   mod_transpiration_class()     .setup_structure(*this,for_write);              //110327
   mod_biomass_production_class().setup_structure(*this,for_write);              //110327
   mod_morphology_class()        .setup_structure(*this,for_write);              //110325
      // the harvest parameter section was previously named [harvest_index]
   mod_yield_class()             .setup_structure                                //110606_110325
      (*this,for_write,harvested_part_clad.get());

   #ifdef CROPSYST_PROPER
   mod_emergence_class()         .setup_structure(*this,for_write);              //110327
   #else
   // if there is no emergence section the VV data source is
   // falling back to [crop] section when it is encountered in the file
   set_current_section("emergence");                                             //170307
   #endif
   mod_tolerance_class()         .setup_structure(*this,for_write,harvested_part_clad.get()); //110606_110325_020717
   if ((!CROPSYST_ORCHARD || !for_write))                                        //020405
      mod_nitrogen_class().setup_structure(*this,for_write);                     //110325
   mod_fruit_class().setup_structure(*this,for_write);                           //110512
   mod_CO2_response_class().setup_structure(*this,for_write);                    //110606

#ifdef LADSS_MODE
   if (LADSS_mode)                                                               //021220
   {
      expect_int16("EMERGE_DD"         ,ref_phenology().emergence_deg_day);
      expect_int16("TUBER_INIT_DD"     ,ref_phenology().tuber_initiation_deg_day);
      expect_int16("FLOWER_DD"         ,ref_phenology().begin_flowering_deg_day);
      expect_int16("PEAK_LAI_DD"       ,ref_phenology().max_canopy_deg_day);
      expect_int16("MAX_ROOT_DEPTH_DD" ,ref_phenology().max_root_depth_deg_day);
      expect_int16("FILLING_DD"        ,ref_phenology().begin_filling_deg_day);
      expect_float32("MATURITY_DD"     ,ref_phenology().begin_maturity_deg_day); //050628
      expect_float32("RAPID_GROWTH_DD" ,ref_phenology().begin_rapid_fruit_growth_deg_day);   //061006
   }
#endif
   if ( (CROPSYST_ORCHARD || !for_write))                                        //020405
   { // fruit trees and grape harvest parameters
      set_current_section(LABEL_fruit);
      expect_bool("grape",mod_fruit().grape);                                    //170524_021206
   }
   #ifndef YAML_PARAM_VERSION
   // adjustments and grain_stress_period will not be in V5
   // When I have implemented the parameter adjustment idiom this
   // will be enabled only in V4
   set_current_section(LABEL_crop);
   EXPECT_ENUMERATED_SECTION("adjustment",adjustment_section_VV,adjustments);    //120726
   #endif
   structure_defined = true;                                                     //120314
   return expected;                                                              //161023
}
//_Crop_parameters_class::expect_structure__________________________2001-01-09_/
Crop_parameters_class::Nitrogen::Nitrogen()
//200529 : nitrogen_leaf_stress_mode_cowl(nitrogen_leaf_stress_mode,no_leaf_stress)
{  fixating                            =false;
   legume_fraction                     =1.0; //default must be 1.0 (non pasture) //070105
   max_uptake_daily_rate_kg_ha         =5.0;                                     //061129
   #if (CROPSYST_VERSION == 4)
   demand_adjust                       =1.0;                                     //061213
   if (dilution_curve_based_on_max_conc)                                         //121219
   {  demand_adjust                       =1.0;                                  //061213
      biomass_to_start_dilution_max_N_conc_T_ha = CropSyst::biomass_to_start_dilution_max_N_conc_default[C3];   //default for C3 species
   }
   #endif
   biomass_to_start_dilution_crit_N_conc_T_ha = CropSyst::biomass_to_start_dilution_crit_N_conc_default[C3];   //default for C3 species  121219
   emergence_max_conc = 0.007;
   emergence_crit_conc = 0.007;
   stubble_max_conc                    =0.007;                                   //061214
   root_conc                           =0.007;                                   //061214
   //200529 nitrogen_leaf_stress_mode           =no_leaf_stress; // default no_leaf_stress if for compatibility with previous version //070227
   leaf_expansion_sensitivity          = 1.0;                                    //200528
   residual_N_not_available_for_uptake =1.0;                                     //070209
   soil_N_conc_where_N_uptake_decreases=5.0;                                     //070209
   PAW_where_N_uptake_rate_decreases   =0.5;                                     //070209
   dilution_curve_max_conc_mature = CropSyst::N_mature_max_conc_default[C3];//default for C3 species
   dilution_curve_slope                = 0.45;                                   //150901_120620
   end_dilution_curve_deg_day          = 0;
   TUE_stress_sensitivity              = 0.5;                                    //151117
}
//_Nitrogen:constructor _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _/
bool Crop_parameters_class::Nitrogen::setup_structure(Data_record &record,bool for_write)    modification_
{  record.set_current_section(LABEL_nitrogen);                                   //020405
   record.expect_bool(LABEL_fixation,fixating);
   record.expect_float32("legume_fraction",legume_fraction);                     //070105
   //200529 record.expect_enum("nitrogen_leaf_stress_mode",nitrogen_leaf_stress_mode_cowl);//070227
   record.expect_float32("leaf_expansion_sensitivity",leaf_expansion_sensitivity);//200529
   record.expect_float32("TUE_stress_sensitivity",TUE_stress_sensitivity);       //151117
   return true;
}
//_Nitrogen::setup_structure________________________________________2011-03-25_/
void Crop_parameters_class::Nitrogen::set_fallow_conditions()
{  max_uptake_daily_rate_kg_ha   = 0.0;                                          //061129
   // Currently fallow is not allowed to uptake
   stubble_max_conc              =0.007;
   root_conc                     =0.01;
}
//_set_fallow_conditions____________________________________________2002-04-08_/
float32 Crop_parameters_class::Nitrogen::provide_concentration_slope
(Photosynthetic_pathway photosynthetic_pathway)                       provision_
{  if (CORN::is_zero<float32>(dilution_curve_slope/*200127 ,0.0,0.000001*/))     //121219
   {
      #if (CROPSYST_VERSION == 4)
      //121219 These became deprecated in version 4.17.00 because dilution curves are now based on critical curve not maximum curve. obsolete in V5
      if (dilution_curve_based_on_max_conc)                                      //121219
      {  float64 slope_adjustment  =  1.0 / demand_adjust;
         dilution_curve_slope = CropSyst::N_dilution_curve_slope_default[photosynthetic_pathway] * slope_adjustment;
      } else
      #endif
         dilution_curve_slope = CropSyst::N_dilution_curve_slope_default[photosynthetic_pathway];//121219
   }
   return dilution_curve_slope;
}
//_provide_concentration_slope______________________________________2012-05-07_/
Crop_parameters_class::Salinity::Salinity()
{  osmotic_pot_50      =-0.0000001; // was -200.0
   salt_tolerance_P    = 0.0000001;
}
//_Salinity_________________________________________________________2002-04-08_/
Crop_parameters_class::CO2_Response::CO2_Response()
{
   CO2_concentration_when_measured.RUE = 350.0;                                  //131206
   CO2_concentration_when_measured.TUE = 350.0;                                  //131206
   non_rectagular_hyperbola_solution.alpha                  = 0.0032725;         //151028
   non_rectagular_hyperbola_solution.theta                  = 0.941574;          //151028
   non_rectagular_hyperbola_solution.growth_ratio_asymptotic= 1.2771338;         //151028

/* C3               C4
alpha 0.0032725     0.0038
theta 0.941574      0.977
GRA   1.2771338     1.0627
*/
}
//_CO2_Response:constructor_________________________________________2002-04-08_/
bool Crop_parameters_class::CO2_Response::setup_structure(Data_record &record,bool for_write)    modification_
{  record.set_current_section(LABEL_CO2);                                        //980223
   return true;
}
//_CO2_Response::setup_structure____________________________________2011-03-26_/
//200403Crop_parameters_class::Canopy_characteristics_hedge_row::Canopy_characteristics_hedge_row()
Crop_parameters_class::Canopy_architecture::Canopy_architecture()
{  initial_width                =1.0;                                            //040715
   final_width                  =1.0;                                            //040715
   initial_breadth              =2.0;                                            //071116
   final_breadth                =2.0;                                            //071116
   initial_height               =1.2;                                            //040715
   final_height                 =1.2;                                            //040715
   branch_insertion_height      =0.6;                                            //071116
}
//_Canopy_characteristics_hedge_row:constructor_____________________2002-04-08_/
Crop_parameters_class::Tolerance::Tolerance()
{  enabled                           =false;                                     //021016
   temperature_cold_leaf_damage     = -5;
   temperature_cold_leaf_lethal     =-10;
   temperature_cold_fruit_damage    = -5;                                        //081126
   temperature_cold_fruit_lethal    =-10;                                        //081126
   temperature_heat_tuber_critical  = 17;                                        //081109
}
//_Tolerance:constructor____________________________________________2002-04-08_/
bool Crop_parameters_class::Tolerance::setup_structure
(Data_record &record,bool for_write,Harvested_part harvested_part) modification_
{  record.set_current_section(LABEL_hardiness);                                  //000330
   record.expect_bool(LABEL_enabled,enabled);                                    //021015
   if ((harvested_part == tuber_part) || !for_write)
      record.expect_float32("tuber_crit_temperature",temperature_heat_tuber_critical);//081108
      // the remaining parameters are setup in the derived classes
   return true;
}
//_Tolerance::setup_structure_______________________________________2011-03-25_/
Crop_parameters_class::Layout::Layout()
{
   spacing.row =2.7;                                                             //180109
   spacing.col =2.5;                                                             //180109
   row_azimuth_true_north_deg=0;
}
//_Layout:constructor_______________________________________________2002-04-08_/
#if ((PHENOLOGY_VERSION==2018))
Crop_parameters_class::Phenologic_period *Crop_parameters_class::render_phenologic_period
(const VV_Section &phenologic_period_section_pot)                     rendition_
{
   Crop_parameters_class::Phenologic_period *period = 0;
   std::string period_name_pot;
   phenologic_period_section_pot.label_string(period_name_pot);
   size_t /*nat32*/ phenology_pos =period_name_pot.find("phenology/");
   if (phenology_pos != std::string::npos)
   {
      period = new Crop_parameters_class::Phenologic_period;
      const VV_unexpected_entry *duration_entry          = dynamic_cast<const VV_unexpected_entry *>(phenologic_period_section_pot.entries.find_string("duration"));
      const VV_unexpected_entry *relevant_entry   = dynamic_cast<const VV_unexpected_entry *>(phenologic_period_section_pot.entries.find_string("relevant"));
      const VV_unexpected_entry *basis_entry             = dynamic_cast<const VV_unexpected_entry *>(phenologic_period_section_pot.entries.find_string("initiation/basis"));
      const VV_unexpected_entry *relative_to_entry       = dynamic_cast<const VV_unexpected_entry *>(phenologic_period_section_pot.entries.find_string("initiation/relative_to"));
      const VV_unexpected_entry *thermal_time_accum_entry= dynamic_cast<const VV_unexpected_entry *>(phenologic_period_section_pot.entries.find_string("initiation/thermal_time_accum"));

      const VV_unexpected_entry *thermal_response_entry  = dynamic_cast<const VV_unexpected_entry *>(phenologic_period_section_pot.entries.find_string("thermal_response"));
      const VV_unexpected_entry *temperature_base_entry  = dynamic_cast<const VV_unexpected_entry *>(phenologic_period_section_pot.entries.find_string("temperature/base"));
      const VV_unexpected_entry *temperature_cutoff_entry= dynamic_cast<const VV_unexpected_entry *>(phenologic_period_section_pot.entries.find_string("temperature/cutoff"));
      const VV_unexpected_entry *temperature_opt_entry   = dynamic_cast<const VV_unexpected_entry *>(phenologic_period_section_pot.entries.find_string("temperature/optimal"));
      const VV_unexpected_entry *temperature_max_entry   = dynamic_cast<const VV_unexpected_entry *>(phenologic_period_section_pot.entries.find_string("temperature/maximum"));
                                    period->ontology = period_name_pot;
      if (duration_entry)           period->duration_GDDs = CORN::cstr_to_float32(duration_entry->get_value().c_str());

      if (relevant_entry)
      {  std::string relevance(relevant_entry->get_value());
         if (relevance.find("vernalization") != std::string::npos) period->relevant |= RELEVANT_VERNALIZATION;
         if (relevance.find("pheotoperiod")  != std::string::npos) period->relevant |= RELEVANT_PHOTOPERIOD;
         if (relevance.find("clipping")      != std::string::npos) period->relevant |= RELEVANT_CLIPPING;
      }
      if (basis_entry)              period->initiation.basis = (basis_entry->get_value() == "initiation")
         ? Crop_parameters_struct::Phenologic_period::initiation_basis
         : Crop_parameters_struct::Phenologic_period::culmination_basis;
      if (relative_to_entry)        period->initiation.relative_to = relative_to_entry->get_value();
      if (thermal_time_accum_entry) period->initiation.thermal_time_accum  = CORN::cstr_to_float32(thermal_time_accum_entry->get_value().c_str());

      period->thermal_time = new Crop_parameters_struct::Thermal_time;
      if (thermal_response_entry)
         period->thermal_time->thermal_response = thermal_response_entry->get_value() == "linear" ? linear : nonlinear;
      if (temperature_base_entry)      period->thermal_time->temperature.base    = CORN::cstr_to_float32(temperature_base_entry   ->get_value().c_str());
      if (temperature_cutoff_entry)    period->thermal_time->temperature.cutoff  = CORN::cstr_to_float32(temperature_cutoff_entry ->get_value().c_str());
      if (temperature_opt_entry)       period->thermal_time->temperature.opt     = CORN::cstr_to_float32(temperature_opt_entry    ->get_value().c_str());
      if (temperature_max_entry)       period->thermal_time->temperature.max     = CORN::cstr_to_float32(temperature_max_entry    ->get_value().c_str());

   } // else section is not phenology/period
   return period;
}
//_Crop_parameters_class::render_phenologic_period__________________2018-11-09_/
#endif
bool Crop_parameters_class::get_end()
{  bool ended = Common_parameters_data_record::get_end();                        //170524_161025_020408
   // Note that the thermal time for the WUE change is in thermal time parameters  080514
   #if (CROPSYST_VERSION < 6)
   // removed this after 2021
   if (ref_biomass_production().TUE_equation_obsolete != TUE_curve)
   {
      std::clog << "warning: The selected transpiration use efficiency model: "
      // << TUE_equation_cowl.get_label()
      << " is obsolete, now always using TUE curve." << std::endl;
   }
   #endif

   #if (CROPSYST_VERSION==4)
   // remove this after 2020
   if (ref_biomass_production().RUE_basis_obsolete == photosynthetically_active_radiation)
   {  std::clog << "warning: The selected RUE basis (PAR) is obsolete, "
      << " now always using total radiation" << std::endl;
   }
   #endif

   Crop_parameters_struct::Biomass_production  biomass_production = mod_biomass_production();

   biomass_production.efficiency_model =                                         //191031
      (ref_biomass_production().efficiency_model_raw == "TUE")
      ? TUE_efficiency
      : RUE_efficiency;
   #ifndef VIC_CROPSYST
   // Was in V4 but also applicable to V5

   //180702 This should be applicable to all versions
   FOR_EACH_IN(adjustment,Adjustment,adjustments,each_adj)
   {  if (adjustment->parameter==LABEL_max_water_uptake)
         has_max_water_uptake_adjustments = true;
   } FOR_EACH_END(each_adj)
   #endif
   bool has_phenologic_period_parameters = false;


   #if ((PHENOLOGY_VERSION==2018))
   FOR_EACH_IN(phenologic_period_section_pot,CORN::VV_Section,sections,section_iter)   //181109
   {
      Phenologic_period *phenologic_period_pot = render_phenologic_period(*phenologic_period_section_pot);
      if (phenologic_period_pot) // may be 0 if not applicable
      {  has_phenologic_period_parameters = true;
         mod_phenology().phenologic_periods.take(phenologic_period_pot);
      }
   } FOR_EACH_END(section_iter)
   #else
   {  Crop_parameters_struct::Phenology &phenology =mod_phenology();
      if (phenology.initiation.accrescence == 0)                                 //200126
         phenology.initiation.accrescence = phenology.initiation.emergence;      //200126

      if (   (phenology.culmination_obsolete.accrescence > 0)
          && (phenology.duration.accrescence == 0))
         phenology.duration.accrescence = phenology.culmination_obsolete.accrescence - phenology.initiation.accrescence;//190630
      if (phenology.initiation.senescence_obsoleteX> 0)
      {
         // upgrading: now senescence initiation is relative to canopy initiation emergence
         phenology.initiation.senescence =  phenology.initiation.senescence_obsoleteX - phenology.initiation.emergence;
         phenology.duration.culminescence = phenology.initiation.senescence_obsoleteX - phenology.culmination_obsolete.accrescence; //191121
         phenology.duration.senescence = phenology.culmination_obsolete.senescence_obsoleteX - phenology.initiation.senescence_obsoleteX;
      }
      #if (!defined(YAML_PARAM_VERSION) || (CROPSYST_VERSION==4))
      float32 thermal_time_season_end_obsolete
         = ref_canopy_growth_cover_based().thermal_time_season_end_obsolete;
      // Termal time season end was for a time essentially duplicate of
      // senescence end, but TTSE was priority over the old senesence end.

      if (thermal_time_season_end_obsolete > 0)
         phenology.duration.senescence
         = thermal_time_season_end_obsolete - phenology.initiation.senescence_obsoleteX;
      #endif

//std::clog << phenology.duration.canopy_senescencex << std::endl;
   }
   #endif
   #if (CROPSYST_VERSION < 6)

   // instanciate phenologic periods given legacy phenology
   if (!has_phenologic_period_parameters)
   {
      // as of 200123 V4 and V5 crop parameter files havent implemented
      // the new phenologic periods but are now using
      // phenologic periods for canopy curve evolution,
      // so we need to compose the period parameters.

      Crop_parameters_struct::Phenology  &phenology  = mod_phenology();
      Crop_parameters_struct::Morphology &morphology = mod_morphology();
      Phenology  &phenology_class = mod_phenology_class();

      //std::clog << "Upgrading phenology parameters" << std::endl;
      // previously it was assumed growth started at emergence
      //200126 moved above phenology.initiation.accrescence = phenology.initiation.emergence;
      // Previously it was assumed root growth started at emergence
      // At this point accresence and senescence initiation and duration should be setup
      if (CORN::is_zero<float32>(phenology.duration.accrescence))                //200312
         phenology.duration.accrescence = 10; // avoid div 0

      if (CORN::is_zero<float32>(phenology.duration.culminescence))              //200312
         phenology.duration.culminescence = 10; // avoid div 0

      if (CORN::is_zero<float32>(phenology.duration.senescence))                 //200312
         phenology.duration.senescence = 10; // avoid div 0

      phenology_class.accrescence_period = new Phenologic_period
         ("phenology/accrescence"
         ,PHENOLOGY_SEASON_ONTOLOGY
         ,Crop_parameters_struct::Phenologic_period::initiation_basis
         ,phenology.initiation.accrescence
         ,phenology.duration.accrescence                                         //200123
         ,CONSECUTIVE_PERIOD
         ,   (ref_vernalization().enabled                          ? RELEVANT_VERNALIZATION : 0)
          | ((ref_photoperiod  ().photoperiodism != no_photoperiod)? RELEVANT_PHOTOPERIOD   : 0)
          |  (phenology.clipping_resets                            ? RELEVANT_CLIPPING      : 0)
         ,&mod_thermal_time(),CSVC_crop_base_restart_date_YD );

      phenology_class.culminescence_period = new Phenologic_period
         ("phenology/culminescence"
         ,"phenology/accrescence"                                                //200123
         ,Crop_parameters_struct::Phenologic_period::culmination_basis           //200123
         ,0 // culminescence starts immediately after the culmination of accrescence 200123
         ,phenology.duration.culminescence
         ,CONSECUTIVE_PERIOD
         ,   (ref_vernalization().enabled                          ? RELEVANT_VERNALIZATION : 0)
          | ((ref_photoperiod  ().photoperiodism != no_photoperiod)? RELEVANT_PHOTOPERIOD   : 0)
          |  (phenology.clipping_resets                            ? RELEVANT_CLIPPING      : 0)
         ,&mod_thermal_time(),CSVP_crop_base_peak_LAI_date_YD );

      phenology_class.senescence_period = new Phenologic_period
         ("phenology/senescence"
         ,"phenology/culminescence"
         ,Crop_parameters_struct::Phenologic_period::culmination_basis
         ,0 // senescence starts immediately after the culmination of culminescence
         ,phenology.duration.senescence
         ,CONSECUTIVE_PERIOD
         ,   (ref_vernalization().enabled                          ? RELEVANT_VERNALIZATION : 0)
          | ((ref_photoperiod  ().photoperiodism != no_photoperiod)? RELEVANT_PHOTOPERIOD   : 0)
          |  (phenology.clipping_resets                            ? RELEVANT_CLIPPING      : 0)
         ,&mod_thermal_time(),0/*xxxxxxUED_var_code*/ );

      phenology_class.root_elongation_period = new Phenologic_period             //191121
         ("phenology/root_elongation" // need to check ontology!
         ,"phenology/accrescence"
         ,Crop_parameters_struct::Phenologic_period::initiation_basis
         ,0 // starts a emergence phenology.initiation.root_elongation
         ,phenology.duration.root_elongation
         ,CONSECUTIVE_PERIOD
         ,   (ref_vernalization().enabled                          ? RELEVANT_VERNALIZATION : 0)
          | ((ref_photoperiod  ().photoperiodism != no_photoperiod)? RELEVANT_PHOTOPERIOD   : 0)
          |  (phenology.clipping_resets                            ? RELEVANT_CLIPPING      : 0)
         ,&mod_thermal_time(),0/*xxxxxxUED_var_code*/ );
      #if ((PHENOLOGY_VERSION==2018))
      mod_phenology().phenologic_periods.take(phenology_class.accrescence_period);
      mod_phenology().phenologic_periods.take(phenology_class.culminescence_period);
      mod_phenology().phenologic_periods.take(phenology_class.senescence_period);
      mod_phenology().phenologic_periods.take(phenology_class.root_elongation_period); //191121

      phenology.initiation.root_elongation = phenology.initiation.emergence;

      // Note in the following vernalization photoperiod and clipping relevence
      // are set in all periods only to match previous behaviour
      // however in reality, these are probably not relevent in all periods.     181128

      Phenologic_period *germination_period = new Phenologic_period
         ("phenology/germination"
         ,PHENOLOGY_SEASON_ONTOLOGY
         ,Crop_parameters_struct::Phenologic_period::initiation_basis
         ,0,phenology.initiation.emergence
         ,CONSECUTIVE_PERIOD
         ,   (ref_vernalization().enabled                          ? RELEVANT_VERNALIZATION : 0)
          | ((ref_photoperiod  ().photoperiodism != no_photoperiod)? RELEVANT_PHOTOPERIOD   : 0)
          |  (phenology.clipping_resets                            ? RELEVANT_CLIPPING      : 0)
         ,&mod_thermal_time()
         ,CSVP_crop_base_planting_date_YD );
      mod_phenology().phenologic_periods.take(germination_period);

      // Previously emergence wasnt considered a period (more of an event)
      // So it doesn't have any duration (or thermal time accumulation)
      // Now the user could have multiple day emergence period if so desired
      Phenologic_period *emergence_period = new Phenologic_period
         ("phenology/emergence"
         ,PHENOLOGY_SEASON_ONTOLOGY
         ,Crop_parameters_struct::Phenologic_period::initiation_basis
         ,phenology.initiation.emergence
         ,0 // phenology.initiation.emergence+1
         // previously there was the emergence period (assumed emergence was essentially instantaneous
         // phenology.initiation.accrescence - phenology.initiation.emergence     // check
         ,INDEPENDENT_PERIOD // actually this is an event
         ,   (ref_vernalization().enabled                          ? RELEVANT_VERNALIZATION : 0)
          | ((ref_photoperiod  ().photoperiodism != no_photoperiod)? RELEVANT_PHOTOPERIOD   : 0)
          |  (phenology.clipping_resets                            ? RELEVANT_CLIPPING      : 0)
         ,0 // &mod_thermal_time()
         ,CSVP_crop_base_emergence_date_YD );
      mod_phenology().phenologic_periods.take(emergence_period);

      Phenologic_period *tuberization_period = new Phenologic_period
         ("phenology/tuberization"
         ,PHENOLOGY_SEASON_ONTOLOGY
         ,Crop_parameters_struct::Phenologic_period::initiation_basis
         ,phenology.initiation.tuberization
         ,phenology.initiation.senescence - phenology.initiation.tuberization // duration //Need to check
         ,INDEPENDENT_PERIOD
         ,   (ref_vernalization().enabled                          ? RELEVANT_VERNALIZATION : 0)
          | ((ref_photoperiod  ().photoperiodism != no_photoperiod)? RELEVANT_PHOTOPERIOD   : 0)
          |  (phenology.clipping_resets                            ? RELEVANT_CLIPPING      : 0)
         ,&mod_thermal_time(),CSVP_crop_base_tuber_init_date_YD);
      mod_phenology().phenologic_periods.take(tuberization_period);

      float32 anthesis_duration = phenology.initiation.filling - phenology.initiation.flowering;
      if (anthesis_duration < 0) anthesis_duration=1;
         // in some crops anthesis is not relevent, so just avoid negative duration
      Phenologic_period *anthesis_period = new Phenologic_period
         ("phenology/anthesis"
         ,PHENOLOGY_SEASON_ONTOLOGY
         ,Crop_parameters_struct::Phenologic_period::initiation_basis
         ,phenology.initiation.flowering
         ,anthesis_duration // duration
         ,(harvested_part_clad.get() == tuber_part) ? INDEPENDENT_PERIOD :CONSECUTIVE_PERIOD //181228
         ,   (ref_vernalization().enabled                          ? RELEVANT_VERNALIZATION : 0)
          | ((ref_photoperiod  ().photoperiodism != no_photoperiod)? RELEVANT_PHOTOPERIOD   : 0)
          |  (phenology.clipping_resets                            ? RELEVANT_CLIPPING      : 0)
         ,&mod_thermal_time(),CSVP_crop_base_flowering_date_YD);
      mod_phenology().phenologic_periods.take(anthesis_period);

      Phenologic_period *filling_period = new Phenologic_period
         ("phenology/filling"
         ,PHENOLOGY_SEASON_ONTOLOGY
         ,Crop_parameters_struct::Phenologic_period::initiation_basis
         ,phenology.initiation.filling
         ,phenology.duration.filling
         /*200506
         ,// maturity_significant ? phenology.initiation.maturity :
            phenology.culmination.accrescence
           -phenology.initiation.filling // duration
         */
         ,(harvested_part_clad.get() == tuber_part) ? INDEPENDENT_PERIOD :CONSECUTIVE_PERIOD
         ,   (ref_vernalization().enabled                          ? RELEVANT_VERNALIZATION : 0)
          | ((ref_photoperiod  ().photoperiodism != no_photoperiod)? RELEVANT_PHOTOPERIOD   : 0)
          |  (phenology.clipping_resets                            ? RELEVANT_CLIPPING      : 0)
         ,&mod_thermal_time()
         ,CSVP_crop_base_grain_filling_date_YD );
      mod_phenology().phenologic_periods.take(filling_period);
      // filling is fructescence for fruit trees

      if (is_fruit_tree())
      {
      Phenologic_period *rapid_fruit_growth_period = new Phenologic_period
         ("phenology/rapid_fruit_growth"
         ,PHENOLOGY_SEASON_ONTOLOGY
         ,Crop_parameters_struct::Phenologic_period::initiation_basis
         ,phenology.initiation.rapid_fruit_growth
         ,phenology.initiation.maturity - phenology.initiation.rapid_fruit_growth // need to check duration
         ,INDEPENDENT_PERIOD
         ,   (ref_vernalization().enabled                          ? RELEVANT_VERNALIZATION : 0)
          | ((ref_photoperiod  ().photoperiodism != no_photoperiod)? RELEVANT_PHOTOPERIOD   : 0)
          |  (phenology.clipping_resets                            ? RELEVANT_CLIPPING      : 0)
         ,&mod_thermal_time(),CSVC_crop_base_rapid_fruit_growth_date_YD);
      mod_phenology().phenologic_periods.take(rapid_fruit_growth_period);

      Phenologic_period *veraison_period = new Phenologic_period
         ("phenology/veraison"
         ,PHENOLOGY_SEASON_ONTOLOGY
         ,Crop_parameters_struct::Phenologic_period::initiation_basis
         ,phenology.initiation.veraison
         ,100 // duration is probably not
         ,INDEPENDENT_PERIOD
         ,   (ref_vernalization().enabled                          ? RELEVANT_VERNALIZATION : 0)
          | ((ref_photoperiod  ().photoperiodism != no_photoperiod)? RELEVANT_PHOTOPERIOD   : 0)
          |  (phenology.clipping_resets                            ? RELEVANT_CLIPPING      : 0)
         ,&mod_thermal_time(),0 /*xxxxxxUED_var_code*/ );
      mod_phenology().phenologic_periods.take(veraison_period);
      }
      if (phenology.maturity_significant)
      {
         Phenologic_period *maturity_period = new Phenologic_period
            ("phenology/maturity"
            ,PHENOLOGY_SEASON_ONTOLOGY
            ,Crop_parameters_struct::Phenologic_period::initiation_basis
            ,phenology.initiation.maturity  // didnt previously have maturity culimination
            ,phenology.duration.maturity //200508 culmination.senescence_obsolete - phenology.initiation.maturity // duration
            //200506 ,phenology.culmination.senescence_obsolete - phenology.initiation.maturity // duration
            ,INDEPENDENT_PERIOD // I think because the consecutive period is for canopy development model
            ,   (ref_vernalization().enabled                          ? RELEVANT_VERNALIZATION : 0)
             | ((ref_photoperiod  ().photoperiodism != no_photoperiod)? RELEVANT_PHOTOPERIOD   : 0)
             |  (phenology.clipping_resets                            ? RELEVANT_CLIPPING      : 0)
            ,&mod_thermal_time(),CSVP_crop_base_maturity_date_YD );
         mod_phenology().phenologic_periods.take(maturity_period);
      }
      #endif
   }
   #endif
   return ended;
}
//_get_end__________________________________________________________2012-07-25_/
#ifndef YAML_PARAM_VERSION
float64 Crop_parameters_class::param_max_water_uptake_m
(Normal_crop_event_sequence growth_stage)                                  const
{  float64 max_water_uptake_mm = ref_transpiration().normal_max_water_uptake_mm; //120724
   if (is_fruit_tree()                                                           //190625
      && !has_max_water_uptake_adjustments)                                      //120724
   {
      const Crop_parameters_struct::Fruit &fruit = ref_fruit();
      switch (growth_stage)
      {
         case NGS_FILLING        : max_water_uptake_mm  = fruit.max_water_uptake_during_initial_fruit_mm;break;
         case FGS_RAPID_FRUIT_DEVELOPMENT : max_water_uptake_mm  = fruit.max_water_uptake_during_radid_fruit_mm;  break;
         case FGS_VERAISON       : max_water_uptake_mm  = fruit.max_water_uptake_during_veraison_mm;     break;
         case NGS_MATURITY       :
         case NGS_HARVESTABLE    :
         case NGS_HARVEST        : max_water_uptake_mm  = fruit.max_water_uptake_after_maturity_mm;      break;
         case NGS_GERMINATION    :
         case NGS_EMERGENCE      :
         case NGS_RESTART        : // or FGS_BUD_BREAK

         case NGS_ACTIVE_GROWTH  :
         case NGS_ANTHESIS       :
         default                 : max_water_uptake_mm  = fruit.max_water_uptake_before_fruiting_mm;     break;
      }
   }
   return mm_to_m(max_water_uptake_mm);
}    // mm/day
#endif
//_param_max_water_uptake_m_________________________________________1998-12-03_/
}//_namespace CropSyst_________________________________________________________/

