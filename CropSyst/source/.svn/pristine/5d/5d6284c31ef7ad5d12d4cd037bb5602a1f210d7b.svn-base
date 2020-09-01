#ifndef crop_param_V5H
#define crop_param_V5H

//#error  Remove crop_param_V5.cpp from your project instead use crop_param_V4.cpp

// I have reverted to V4 crop parameter classes because I don't quite have all parameters ready in V5.
// In any case V5 I intend to use the new YAML format files in V5.

#include "crop/crop_param_class.h"
#include "corn/parameters/parameter.h"

namespace CropSyst                                                               ///110208
{

//______________________________________________________________________________
class Crop_parameters      // Move some of these to Crop_classification_parameters
: public extends_    Crop_parameters_class                                       //110121
, public implements_ CORN::Item
{
public:
class Thermal_time                                                               //020628
: public Crop_parameters_class::Thermal_time
{
 public:                                                                         //090414
   CORN::Parameter_number p_base_temperature;
   CORN::Parameter_number p_cutoff_temperature;
   CORN::Parameter_number p_opt_temperature;
   CORN::Parameter_number p_max_temperature;
 public: // constructor
   Thermal_time();
   VIRTUAL_BOOL_SETUP_STRUCTURE;
   void get_end();
};
//_Thermal_time________________________________________________________________/
class Phenology
: public Crop_parameters_class::Phenology
{public:                                                                         //090414
/*
   // WUE_change_deg_day is only here temporarily, in V5 WUE will become
   // an temporally adjustable parameter.
    float32 WUE_change_deg_day;
      // Degree days to crop change in water use efficiency for WUE_changes mode.//080510
      // In version 5 WUE will be a temporaly adjustable parameters.
*/
   // Eventually these should be renamed to
   // to be consistent with initiation and culmination

   CORN::Parameter_number p_emergence_deg_day;
   CORN::Parameter_number p_tuber_initiation_deg_day;
//200125 obsolete   CORN::Parameter_number p_max_canopy_deg_day_obsolete;
      // Replaces p_max_canopy_deg_day


   //191121 CORN::Parameter_number p_max_root_depth_deg_day_obsolete;            //191128
   CORN::Parameter_number p_begin_flowering_deg_day;
   CORN::Parameter_number p_begin_filling_deg_day;
   CORN::Parameter_number p_begin_maturity_deg_day;

   CORN::Parameter_number p_canopy_accrescence_duration;                         //190627
   CORN::Parameter_number p_duration_canopy_culminescence;                       //191121
   //200125 obsolete CORN::Parameter_number p_initiation_canopy_senescence_obsolete;               //190721
   CORN::Parameter_number p_duration_canopy_senescence;                          //190721

   CORN::Parameter_number p_root_elongation_deg_day;                             //191121


   /*190721 no senescence is crop canopy thermal time
   CORN::Parameter_number p_begin_senescence_deg_day;                            //090414
   CORN::Parameter_number p_full_senescence_deg_day;                             //131101
   */
   CORN::Parameter_number p_begin_rapid_fruit_growth_deg_day;
   #ifndef YAML_PARAM_VERSION
   CORN::Parameter_number p_WUE_change_deg_day;
      // WUE_change_deg_day is only here temporarily, in V5 WUE will become
      // an temporally adjustable parameter.
   #endif
   CORN::Parameter_number p_phenologic_sensitivity_water_stress;
 public:  // constructor
   Phenology();
   VIRTUAL_BOOL_SETUP_STRUCTURE;
};
//_Phenology___________________________________________________________________/
class Transpiration
: public Crop_parameters_class::Transpiration
{public:
   CORN::Parameter_number p_ET_coef;
   CORN::Parameter_number p_max_water_uptake;
   CORN::Parameter_number p_stomatal_closure_LWP;
   CORN::Parameter_number p_wilt_LWP;
 public:  // constructor
   Transpiration();
   VIRTUAL_BOOL_SETUP_STRUCTURE;
};
//_Transpiration____________________________________________________2011-02-08_/
class Biomass_production
: public Crop_parameters_class::Biomass_production
{public:
   #ifndef YAML_PARAM_VERSION
   CORN::Parameter_number p_TUE_at_1kPa_VPD;
   CORN::Parameter_number p_TUE_at_1kPa_VPD_vegetative;
   CORN::Parameter_number p_TUE_scaling_coef;
   CORN::Parameter_number p_TUE_scaling_coef_vegetative;
   /*191026 AquaCrop and TannerSinclair now obsolete
   CORN::Parameter_number p_water_productivity;
   CORN::Parameter_number p_water_productivity_vegetative;
   CORN::Parameter_number p_biomass_transpiration_coef;
   CORN::Parameter_number p_biomass_transpiration_coef_vegetative;     // eventually rename these to V_Tanner_Sinclair_k_X
   */

   CORN::Parameter_number p_RUE_at_low_VPD;                                      //191026
   CORN::Parameter_number p_RUE_at_low_total_solar_rad;                          //191026
   CORN::Parameter_number p_RUE_efficiency_decrease_rate;                        //191026

   //191026 CORN::Parameter_number p_RUE_global;                                 //110825
   CORN::Parameter_number p_season_biomass_adjustment_2;
   CORN::Parameter_number p_season_biomass_adjustment_3;
   CORN::Parameter_number p_season_biomass_adjustment_4;
   CORN::Parameter_number p_season_biomass_adjustment_5;
   #endif
   CORN::Parameter_number p_temp_limit_min;                                      //161004
   CORN::Parameter_number p_temp_limit_low;                                      //161004
   CORN::Parameter_number p_temp_limit_high;                                     //161004
   CORN::Parameter_number p_temp_limit_max;                                      //161004
 public:  // constructor
   Biomass_production();
   VIRTUAL_BOOL_SETUP_STRUCTURE;
};
//_Biomass_production_______________________________________________2011-02-08_/
class Vernalization
: public Crop_parameters_class::Vernalization
{
      friend class Crop_thermal_time;
 public:                                                                         //090414
   CORN::Parameter_number p_low_temp;
   CORN::Parameter_number p_high_temp;
   CORN::Parameter_number p_start;
   CORN::Parameter_number p_end;
   CORN::Parameter_number p_min_factor;
 public:
   Vernalization();
   VIRTUAL_BOOL_SETUP_STRUCTURE;
};
//_Vernalization_______________________________________________________________/
class Photoperiod
: public Crop_parameters_class::Photoperiod
{
      friend class Crop_thermal_time;
 public:
   contribute_ CORN::Parameter_number p_stop;
   contribute_ CORN::Parameter_number p_unconstrain;
 public:
   Photoperiod();
   VIRTUAL_BOOL_SETUP_STRUCTURE;
};
//_Photoperiod_________________________________________________________________/
class Emergence_hydro_thermal
: public Crop_parameters_class::Emergence_hydro_thermal
{
 public:
   CORN::Parameter_number p_a;
   CORN::Parameter_number p_b;
   CORN::Parameter_number p_seedling_leaf_area;
   CORN::Parameter_number p_germination_base_temperature;
   CORN::Parameter_number p_mean_base_soil_water_pot;
   CORN::Parameter_number p_stdev_base_soil_water_pot;
public:
   Emergence_hydro_thermal();
   VIRTUAL_BOOL_SETUP_STRUCTURE;
};
//_Emergence_hydro_thermal_____________________________________________________/
class Quiescence                                                                 //160331
: public Crop_parameters_class::Quiescence                                       //160331
{
 public:  // Inactive period parameters
   CORN::Parameter_number  p_inducement_temperature;
   CORN::Parameter_number  p_consider_inactive_days;
   /*190607
   CORN::Parameter_number  p_start_DOY;                                          //20110207
   CORN::Parameter_number  p_end_DOY;                                            //20110207
   */
 public:
   Quiescence();
   VIRTUAL_BOOL_SETUP_STRUCTURE;
};
//_Quiescence__________________________________________________________________/
class Dormancy
: public Crop_parameters_class::Dormancy
{
 public:
   CORN::Parameter_number p_chill_hours;                                         //090414
   CORN::Parameter_number p_deg_day_bud_break_chill_req_sat;                     //090414
   CORN::Parameter_number p_deg_day_bud_break_chill_req_not_sat;                 //090414
   CORN::Parameter_number p_senesce_percent;                                     //090414
 public: //
   Dormancy();
   VIRTUAL_BOOL_SETUP_STRUCTURE;
};
//_Dormancy_________________________________________________________2002-04-08_/
class Root
: public Crop_parameters_class::Root
{
 public:
   CORN::Parameter_number p_max_depth;
   CORN::Parameter_number p_surface_density;
   CORN::Parameter_number p_density_distribution_curvature;
   CORN::Parameter_number p_sensitivity_to_water_stress;
   CORN::Parameter_number p_length_at_emergence;
   CORN::Parameter_number p_root_shoot_emergence_ratio;                          //130624
   CORN::Parameter_number p_root_shoot_full_ratio;                               //130624
 public:
   Root();
   VIRTUAL_BOOL_SETUP_STRUCTURE;
};
//_Root________________________________________________________________________/
class Morphology
: public Crop_parameters_class::Morphology
{public:
   CORN::Parameter_number p_reduce_canopy_expansion_LWP;
   CORN::Parameter_number p_stop_canopy_expansion_LWP;
   CORN::Parameter_number p_max_canopy_height;                                   //090414
   //191221 light extinction is actually a canopy parameters
   // it should eventually be moved
   CORN::Parameter_number p_light_extinction_coef;                               //191121
   //191121 CORN::Parameter_number p_kc; // rename to light_extinction_coef      //090414
 public:
   Morphology();
   VIRTUAL_BOOL_SETUP_STRUCTURE;
};
//_Morphology__________________________________________________________________/
class Canopy_growth_cover_based
: public Crop_parameters_class::Canopy_growth_cover_based
{
 public:
   CORN::Parameter_number p_cover_initial;
   CORN::Parameter_number p_cover_maximum;
   //#ifndef SENESCENCE_REDUCTION_200124
   CORN::Parameter_number p_cover_total_season_end;                              //171219
   //#endif
   CORN::Parameter_number p_cover_green_season_end;                              //171219_171009
   //200125 CORN::Parameter_number p_thermal_time_season_end_obsolete;           //190826_171219_171009
   CORN::Parameter_number p_fract_season_to_reach_half_max_cover;
   CORN::Parameter_number p_shape_coef_accrescent;                               //171221
   CORN::Parameter_number p_shape_coef_senescent;                                //171221
 public:
   Canopy_growth_cover_based();
   VIRTUAL_BOOL_SETUP_STRUCTURE;
};
//_Canopy_growth_cover_based___________________________________________________/
class Canopy_growth_LAI_based
: public Crop_parameters_class::Canopy_growth_LAI_based
{
 public:
   CORN::Parameter_number p_initial_GAI;
   CORN::Parameter_number p_regrowth_GAI;
   CORN::Parameter_number p_max_LAI;
   CORN::Parameter_number p_specific_leaf_area;
   CORN::Parameter_number p_fract_LAI_mature;
   CORN::Parameter_number p_stem_leaf_partition;
   CORN::Parameter_number p_leaf_duration_deg_day;
   CORN::Parameter_number p_leaf_duration_sensitivity_stress;
 public:
   Canopy_growth_LAI_based();
   VIRTUAL_BOOL_SETUP_STRUCTURE;
};
//_Canopy_growth_LAI_based_____________________________________________________/
class Tolerance // was Hardiness                                                 //150914
: public Crop_parameters_class::Tolerance
{
 public:  // Hardiness
   CORN::Parameter_number p_leaf_damage_cold_temp;
   CORN::Parameter_number p_leaf_lethal_cold_temp;
   CORN::Parameter_number p_fruit_damage_cold_temp;
   CORN::Parameter_number p_fruit_lethal_cold_temp;
   // NYI  Valid_float32 v_tuber_critical_temperature
public:
   Tolerance();
   virtual bool setup_structure(CORN::Data_record &record,bool for_write,Harvested_part harvested_part) modification_;
};
//_Tolerance________________________________________________________2011-01-21_/
class Yield //200423 was Harvest
: public Crop_parameters_class::Yield
{
public:
   CORN::Parameter_number p_index_unstressed;
   CORN::Parameter_number p_growth_sensitivity;
   CORN::Parameter_number p_tuber_init_sensitivity;
   CORN::Parameter_number p_tuber_growth_sensitivity;
   CORN::Parameter_number p_translocation_fraction_max;                          //110907
public:
   Yield();
   virtual bool setup_structure
      (CORN::Data_record  &record
      ,bool                for_write
      ,Harvested_part      harvested_part )                       modification_; //171124
   bool setup_structure_section
      (CORN::Data_record &record,bool for_write
      ,Harvested_part harvested_part)                             modification_;
};
//_Yield____________________________________________________________2002-04-08_/
class Nitrogen
: public Crop_parameters_class::Nitrogen
{
 public:
   CORN::Parameter_number p_max_uptake_daily_rate;
   CORN::Parameter_number p_residual_N_not_available_for_uptake;
   CORN::Parameter_number p_soil_N_conc_where_N_uptake_decreases;
   CORN::Parameter_number p_PAW_where_N_uptake_rate_decreases;
   CORN::Parameter_number p_stubble_max_conc;
   CORN::Parameter_number p_root_conc;
   CORN::Parameter_number p_emergence_max_conc;                                  //121219
   CORN::Parameter_number p_emergence_crit_conc;                                 //121219
   CORN::Parameter_number p_biomass_to_start_dilution_crit_N_conc;               //121219

   CORN::Parameter_number p_N_mature_max_conc;                                   //120505
   CORN::Parameter_number p_dilution_curve_slope;                                //121219_120505
   CORN::Parameter_number p_end_dilution_curve;                                  //131101
 public:
   Nitrogen();
   VIRTUAL_BOOL_SETUP_STRUCTURE;
};
//_Nitrogen_________________________________________________________2001-01-22_/
class Salinity
: public Crop_parameters_class::Salinity // currently no Salinity_class
{
public:
   CORN::Parameter_number p_osmotic_pot_50;                                      //090414
   CORN::Parameter_number p_salt_tolerance_P;                                    //090414
 public:
   Salinity();
};
//_Salinity_________________________________________________________2002-04-05_/
class Fruit
:public Crop_parameters_class::Fruit
{
public:
   CORN::Parameter_number p_fract_total_solids;                                  //090414
   CORN::Parameter_number p_max_fruit_load_kg_ha;                                //090414
   CORN::Parameter_number p_harvest_temperature;                                 //090414
   CORN::Parameter_number p_harvest_temperature_days;                            //090414
   CORN::Parameter_number p_harvest_DOY_earliest;                                //090414
   CORN::Parameter_number p_harvest_DOY_latest;                                  //090414
   CORN::Parameter_number p_initial_growth_fraction_to_fruit;                    //090414
   CORN::Parameter_number p_rapid_growth_fraction_to_fruit;                      //090414
   CORN::Parameter_number p_max_water_uptake_before_fruiting;                    //091202
   CORN::Parameter_number p_max_water_uptake_during_initial_fruit;               //091202
   CORN::Parameter_number p_max_water_uptake_during_radid_fruit;                 //091202
   CORN::Parameter_number p_max_water_uptake_during_veraison;                    //091202
   CORN::Parameter_number p_max_water_uptake_after_maturity;                     //091202
 public:
   Fruit();
   VIRTUAL_BOOL_SETUP_STRUCTURE;
};
//_Fruit____________________________________________________________2011-01-21_/
class Canopy_architecture //200403 Canopy_characteristics_hedge_row // vineyard       // 0701116 now for both orchards and vinyards
: public Crop_parameters_class::Canopy_architecture
{public: // RLN I believe theys are annual values.
   CORN::Parameter_number p_initial_width;
   CORN::Parameter_number p_final_width;
   CORN::Parameter_number p_initial_breadth;
   CORN::Parameter_number p_final_breadth;
   CORN::Parameter_number p_initial_height;
   CORN::Parameter_number p_final_height;
   CORN::Parameter_number p_branch_insertion_height;
 public:
   Canopy_architecture();
   // Canopy_characteristics_hedge_row();
};
//_Canopy_characteristics_hedge_row_________________________________2002-04-05_/
class Layout // rename to tree layout
: public Crop_parameters_class::Layout
{ public:
   CORN::Parameter_number p_row_spacing;
   CORN::Parameter_number p_row_azimuth;
   CORN::Parameter_number p_col_spacing;
 public:
   Layout();
};
//_Layout___________________________________________________________2002-04-05_/
class CO2_Response
: public Crop_parameters_class::CO2_Response
{public:
   CORN::Parameter_number p_TUE_reference_conc;                                  //131206
   CORN::Parameter_number p_RUE_reference_conc;                                  //131206
   CORN::Parameter_number p_alpha;                                               //131206
   CORN::Parameter_number p_theta;                                               //131206
   CORN::Parameter_number p_growth_ratio_asymptotic;                             //131206
 public:
   CO2_Response();
   VIRTUAL_BOOL_SETUP_STRUCTURE;
};
//_CO2_Response_____________________________________________________2002-04-08_/
 public:
   nat32                      show_section;
   Thermal_time               thermal_time;                                      //020628
   Phenology                  phenology;                                         //080722
   Transpiration              transpiration;                                     //110208
   Biomass_production         biomass_production;                                //110208
   Morphology                 morphology;                                        //080725
   Canopy_growth_LAI_based    canopy_growth_LAI_based;                           //080725
   Canopy_growth_cover_based  canopy_growth_cover_based;                         //080725
   Root                       root;                                              //061212
   Quiescence                 quiescence;                                        //160331-080401
   Dormancy                   dormancy;                                          //080331
   Tolerance                  tolerance;                                         //020408  //optional was pointer
   Residue_decomposition_parameters  residue_decomposition;                      //020408  //110124 was pointer
   Vernalization              vernalization;                                     //020405  //optional was pointer
   Photoperiod                photoperiod;                                       //020405  //optional was pointer
   Emergence_hydro_thermal    emergence;                                         //041201
   Salinity                   salinity;                                          //020405  //optional was pointer
   Layout                     layout;                                            //020405  //optional was pointer
   //200403 Canopy_characteristics_hedge_row canopy_characteristics_hedge_row;            //020405  //optional was pointer
   Canopy_architecture        canopy_architecture;                               //200403

//020505       Although Claudio doesn't want residues to show up in the file for orchard model
//             there has to be a residue in order to preserve the balance in the event crop terminates.
   Yield                      yield;                                             //020408  //optional was pointer
//200423   Harvest                    harvest;                                           //020408  //optional was pointer
   Fruit                      fruit;                                             //020408
   Nitrogen                   nitrogen;  //Nitrogen is optional (not used by fruit crops)   //optional was pointer
   CO2_Response               CO2_response;                                                    //optional was pointer

   CORN::Parameter_number p_linger_days;  // Might not need this with new abscission
 public: // reference modifiers
   inline virtual Crop_parameters_struct::Quiescence                  &mod_quiescence()                { return quiescence;}
   inline virtual Crop_parameters_struct::Thermal_time                &mod_thermal_time()              { return thermal_time;}
   inline virtual Crop_parameters_struct::Biomass_production          &mod_biomass_production()        { return biomass_production; }
   inline virtual Crop_parameters_struct::Vernalization               &mod_vernalization()             { return vernalization;}
   inline virtual Crop_parameters_struct::Photoperiod                 &mod_photoperiod()               { return photoperiod;}
   inline virtual Crop_parameters_struct::Transpiration               &mod_transpiration()             { return transpiration; }
   inline virtual Crop_parameters_class::Morphology                   &mod_morphology()                { return morphology; }
   inline virtual Crop_parameters_struct::Canopy_growth_LAI_based     &mod_canopy_growth_LAI_based ()  { return canopy_growth_LAI_based;}
   inline virtual Crop_parameters_struct::Canopy_growth_cover_based   &mod_canopy_growth_cover_based() { return canopy_growth_cover_based;}
   inline virtual Crop_parameters_struct::Root                        &mod_root()                      { return root;}
   inline virtual Crop_parameters_struct::Fruit                       &mod_fruit()                     { return fruit; }
   inline virtual Crop_parameters_struct::Dormancy                    &mod_dormancy()                  { return dormancy;}//110504
   inline virtual Crop_parameters_struct::Phenology                   &mod_phenology()                 { return phenology;}

 public: // reference accessors
   inline virtual const Crop_parameters_struct::Quiescence                 &ref_quiescence()                const { return quiescence;}
   inline virtual const Crop_parameters_struct::Thermal_time               &ref_thermal_time()              const { return thermal_time;}
   inline virtual const Crop_parameters_struct::Biomass_production         &ref_biomass_production()        const { return biomass_production; }
   inline virtual const Crop_parameters_struct::Vernalization              &ref_vernalization ()            const { return vernalization;}
   inline virtual const Crop_parameters_struct::Photoperiod                &ref_photoperiod ()              const { return photoperiod;}
   inline virtual const Crop_parameters_struct::Transpiration              &ref_transpiration()             const { return transpiration; }
   inline virtual const Crop_parameters_struct::Morphology                 &ref_morphology()                const { return morphology;}
   inline virtual const Crop_parameters_struct::Canopy_growth_LAI_based    &ref_canopy_growth_LAI_based ()  const { return canopy_growth_LAI_based;}
   inline virtual const Crop_parameters_struct::Canopy_growth_cover_based  &ref_canopy_growth_cover_based() const { return canopy_growth_cover_based;}
   inline virtual const Crop_parameters_struct::Root                       &ref_root()                      const { return root;}
   inline virtual const Crop_parameters_struct::Fruit                      &ref_fruit()                     const { return fruit; }
   inline virtual const Crop_parameters_struct::Dormancy                   &ref_dormancy()                  const { return dormancy;}//110504
   inline virtual const Crop_parameters_struct::Phenology                  &ref_phenology()                 const { return phenology;}
   inline virtual const Crop_parameters_struct::Emergence_hydro_thermal    &ref_emergence ()                const { return emergence;}
 public:
   inline virtual Crop_parameters_class::Emergence_hydro_thermal   &mod_emergence_class()           { return emergence;}
   inline virtual Crop_parameters_class::Tolerance                 &mod_tolerance_class()           { return tolerance; } //150914
   inline virtual Crop_parameters_class::Nitrogen                  &mod_nitrogen_class()            { return nitrogen;}
   inline virtual Crop_parameters_class::Transpiration             &mod_transpiration_class()       { return transpiration; } //190626
   //200423 inline virtual Crop_parameters_class::Harvest                   &mod_harvest_class()             { return harvest;}
   inline virtual Crop_parameters_class::Yield                     &mod_yield_class()               { return yield;} //200423
   inline virtual Crop_parameters_class::Morphology                &mod_morphology_class()          { return morphology;}
   inline virtual Crop_parameters_class::Phenology                 &mod_phenology_class()           { return phenology; } //190701
   inline virtual Crop_parameters_class::CO2_Response              &mod_CO2_response_class()        { return CO2_response;}
   inline virtual Crop_parameters_class::Fruit                     &mod_fruit_class()               { return fruit; }
   inline virtual Crop_parameters_class::Biomass_production        &mod_biomass_production_class()  { return biomass_production; }
 public:
   Crop_parameters
      (bool _nitrogen_simulation  = true
      ,bool _salinity_simulation_unused  = true
      ,bool _residue_simulation   = true
      ,bool _CO2_simulation       = true);                                       //020408
   inline virtual bool is_key_cstr(const std::string &key)          affirmation_ //180820
      { return description.brief == key; }
   virtual bool expect_structure(bool for_write = false);
   //200126   virtual bool get_end();                                                       //160531
   virtual bool has_residue_decomposition()        affirmation_ { return true; }
   virtual bool has_nitrogen()                     affirmation_ { return true; }
};
//_Crop_parameters_____________________________________________________________/
}//_namespace CropSyst_________________________________________________________/
#endif

