#ifndef crop_param_V4H
#define crop_param_V4H
#include "crop/crop_param_class.h"
#include "corn/validate/validtyp.h"
#include "corn/OS/filtered_filename.h"
#include "common/residue/residue_decomposition_param.h"
#include "crop/crop_types.hpp"
// These classes are used only in version 4.
// Do not include this in version 5 projects.
using namespace CORN;
//______________________________________________________________________________
namespace CropSyst {                                                             //140318
//______________________________________________________________________________
class Crop_parameters      // Move some of these to Crop_classification_parameters
: public Crop_parameters_class                                                   //110121
{
public:
class Thermal_time
: public Crop_parameters_class::Thermal_time
{
 public:                                                                         //020629
   Valid_float32  v_base_temperature;
   Valid_float32  v_cutoff_temperature;
   Valid_float32  v_opt_temperature;
   Valid_float32  v_max_temperature;
 public: // constructor
   Thermal_time();
   VIRTUAL_BOOL_SETUP_STRUCTURE;
};
//_Crop_parameters__________________________________________________2002-06-28_/
class Phenology
: public Crop_parameters_class::Phenology
{
 public:                                                                         //020629
   //keep this for legacy import
   Valid_float32  v_emergence_deg_day;
   Valid_float32  v_tuber_initiation_deg_day;
   Valid_float32  v_max_root_depth_deg_day_obsolete;                             //191126
   Valid_float32  v_begin_flowering_deg_day;
   Valid_float32  v_begin_filling_deg_day;
   Valid_float32  v_begin_maturity_deg_day;
   Valid_float32  v_begin_rapid_fruit_growth_deg_day;

   // These are actually now relative to canopy initiation
   Valid_float32  v_canopy_accrescence_duration;                                 //190628
   //200125 obsolete Valid_float32  v_canopy_senescence_initiation;                                //190627
   Valid_float32  v_canopy_senescence_duration;                                  //190627
   Valid_float32  v_root_elongation_duration;                                    //191121

   Valid_float32  v_WUE_change_deg_day;
   Valid_float32  v_phenologic_sensitivity_water_stress;
 public:  // constructor
   Phenology();
   VIRTUAL_BOOL_SETUP_STRUCTURE;
};
//_Phenology________________________________________________________2008-07-22_/
class Transpiration
: public Crop_parameters_class::Transpiration
{public:
   Valid_float32  v_ET_coef;
   Valid_float32  v_max_water_uptake;
   Valid_float32  v_LWP_stomatal_closure;
   Valid_float32  v_LWP_wilt_point;    // rename to v_wilt_LWP
 public:  // constructor
   Transpiration();
   VIRTUAL_BOOL_SETUP_STRUCTURE;
};
//_Transpiration____________________________________________________2011-02-08_/
class Biomass_production
: public Crop_parameters_class::Biomass_production
{public:

/*140317 Eventually these will be in V4 only but VIC crop it still using
this so I am still providing this in crop_param_struct
We are going to have a meeting with Jenny to decide if the crop files need to be redone

   float32 opt_mean_temperature_for_growth;                // C
         //Optimum mean daily temperature for growth (was temp limit).           //080725
*/
   #define  early_growth_limit_temp  opt_mean_temperature_for_growth_V4
      // In CropSyst  early_growth_limit_temp used to be opt_mean_temperature_for_growth
      // VIC uses opt_mean_temperature_for_growth
      // Need to check with Claudio what he would prefer.

   Valid_float32 v_TUE_at_1kPa_VPD;
   Valid_float32 v_TUE_at_1kPa_VPD_vegetative;
   Valid_float32 v_TUE_scaling_coef;               Valid_float32 v_TUE_scaling_coef_vegetative;
   /*191026 AquaCrop and Tanner Sinclair now obsolete
   Valid_float32 v_water_productivity;             Valid_float32 v_water_productivity_vegetative;
   Valid_float32 v_biomass_transpiration_coef;     Valid_float32 v_biomass_transpiration_coef_vegetative;     // eventually rename these to V_Tanner_Sinclair_k_X
   */

//continue_here new parameters

   Valid_float32 v_RUE_at_low_VPD;                                               //191026
   Valid_float32 v_RUE_at_low_total_solar_rad;                                   //191026
   Valid_float32 v_RUE_efficiency_decrease_rate;                                 //191026
   //191026 Valid_float32 v_RUE_global;                                          //110825
   //191025 obs Valid_float32 v_RUE_PAR_deprecated;                              //110825

   Valid_float32 v_season_biomass_adjustment_2;
   Valid_float32 v_season_biomass_adjustment_3;
   Valid_float32 v_season_biomass_adjustment_4;
   Valid_float32 v_season_biomass_adjustment_5;

   //191025 The following should be renamed   temperature_limiation_ min max low high
   Valid_float32  v_min_tolerable_temp;                                          //130531
   Valid_float32  v_low_threshold_limit_temp;                                    //130508
   Valid_float32  v_high_threshold_limit_temp;                                   //130508
   Valid_float32  v_max_tolerable_temp;                                          //130508

 public:  // constructor
   Biomass_production();
   VIRTUAL_BOOL_SETUP_STRUCTURE;
};
//_Biomass_production_______________________________________________2011-02-08_/
class Vernalization
: public Crop_parameters_class::Vernalization
{                                                                
   friend class Crop_thermal_time;
 public:  // Vernalization
   Valid_float32  v_low_temp;
   Valid_float32  v_high_temp;
   Valid_float32  v_start;
   Valid_float32  v_end;
   Valid_float32  v_min_factor;
 public:
   Vernalization();
   VIRTUAL_BOOL_SETUP_STRUCTURE;
};
//_Vernalization____________________________________________________2002-04-05_/
class Photoperiod
: public Crop_parameters_class::Photoperiod
{
 public:
   contribute_ Valid_float32  v_stop;
   contribute_ Valid_float32  v_unconstrain;
 public:
   Photoperiod();
   VIRTUAL_BOOL_SETUP_STRUCTURE;
};
//_Photoperiod_________________________________________________________________/
class Emergence_hydro_thermal
: public Crop_parameters_class::Emergence_hydro_thermal
{
public:
   Valid_float32  v_a;
   Valid_float32  v_b;
   Valid_float32  v_seedling_leaf_area;
   Valid_float32  v_germination_base_temperature;
   Valid_float32  v_mean_base_soil_water_pot;
   Valid_float32  v_stdev_base_soil_water_pot;
public:
   Emergence_hydro_thermal();
   VIRTUAL_BOOL_SETUP_STRUCTURE;
};
//_Emergence_hydro_thermal_____________________________________________________/
class Quiescence
: public Crop_parameters_class::Quiescence
{public:  // Inactive period parameters
   Valid_float32  v_inducement_temperature;
   Valid_int16    v_consider_inactive_days;
 public:
   Quiescence();
   VIRTUAL_BOOL_SETUP_STRUCTURE;
};
//_Quiescence__________________________________________________________________/
class Dormancy
: public Crop_parameters_class::Dormancy
{  // Only applies to fruit trees
 public:  // Dormancy (specific)
   Valid_float32  v_chill_hours;
   Valid_float32  v_deg_day_bud_break_chill_req_sat;
   Valid_float32  v_deg_day_bud_break_chill_req_not_sat;
   Valid_float32  v_senesce_percent;
 public:
   Dormancy();
   VIRTUAL_BOOL_SETUP_STRUCTURE;
};
//_Dormancy_________________________________________________________2002-04-08_/
class Root
: public Crop_parameters_class::Root
{
 public:
   Valid_float32  v_max_depth;
   Valid_float32  v_surface_density;
   Valid_float32  v_density_distribution_curvature;
   Valid_float32  v_sensitivity_to_water_stress;
   Valid_float32  v_length_at_emergence;
   Valid_float32  v_root_shoot_emergence_ratio;                                  //130624
   Valid_float32  v_root_shoot_full_ratio;                                       //130624

   // Deprecated in 4.18.10
   Valid_float32  v_length_per_unit_mass;
 public:
   Root();
   VIRTUAL_BOOL_SETUP_STRUCTURE;
};
//_Root________________________________________________________________________/
class Morphology
: public Crop_parameters_class::Morphology
{
 public:
   Valid_float32 v_reduce_canopy_expansion_LWP;
   Valid_float32 v_stop_canopy_expansion_LWP;
   Valid_float32 v_max_canopy_height;
   Valid_float32 v_light_extinction_coef;                                        //191217
//191217 moved   Valid_float32 v_kc;    now as light_extinction_coef             //110528
 public:
   Morphology();
   VIRTUAL_BOOL_SETUP_STRUCTURE;
};
//_Morphology__________________________________________________________________/
class Canopy_growth_cover_based
: public Crop_parameters_class::Canopy_growth_cover_based
{
 public:
   Valid_float32  v_canopy_cover_initial;
   Valid_float32  v_canopy_cover_maximum;
   //#ifndef SENESCENCE_REDUCTION_200124
   Valid_float32  v_canopy_cover_total_season_end;
   //#endif
   Valid_float32  v_thermal_time_fractional_at_half_cover_max;                   //181017
   Valid_float32  v_thermal_time_fractional_at_half_cover_senescence;            //181017
 public:
   Canopy_growth_cover_based();
   VIRTUAL_BOOL_SETUP_STRUCTURE;
};
//______________________________________________________________________________
class Canopy_growth_LAI_based
: public Crop_parameters_class::Canopy_growth_LAI_based
{
 public:
   Valid_float32  v_initial_GAI;
   Valid_float32  v_regrowth_GAI;
   Valid_float32  v_max_LAI;
   Valid_float32  v_specific_leaf_area;
   Valid_float32  v_fract_LAI_mature;
   Valid_float32  v_stem_leaf_partition;
   Valid_float32  v_leaf_duration_deg_day;
   Valid_float32  v_leaf_duration_sensitivity_stress;
 public:
   Canopy_growth_LAI_based();
   VIRTUAL_BOOL_SETUP_STRUCTURE;
};
//______________________________________________________________________________
class Tolerance
: public Crop_parameters_class::Tolerance
{
 public:  // Hardiness
   Valid_float32  v_leaf_damage_cold_temp;
   Valid_float32  v_leaf_lethal_cold_temp;
   Valid_float32  v_fruit_damage_cold_temp;
   Valid_float32  v_fruit_lethal_cold_temp;
   // NYI  Valid_float32 v_tuber_critical_temperature
 public:
   Tolerance();
   virtual bool setup_structure(CORN::Data_record &record,bool for_write,Harvested_part harvested_part) modification_;
};
//_Tolerance________________________________________________________2002-04-08_/
class Yield //200423 was Harvest
: public Crop_parameters_class::Yield
// in this case we dont have Harvest_index_class superclass
{
 public:
   Valid_float32  v_unstressed;
   Valid_float32  v_growth_sensitivity;
   Valid_float32  v_tuber_init_sensitivity;
   Valid_float32  v_tuber_growth_sensitivity;
   Valid_float32  v_translocation_fraction;                                      //110907

   // Currently these are only used in the parameter editor for editing the current stress period
   Valid_float32  v_cold_stress_no_stress_threshold_temperature;                 //120416
   Valid_float32  v_cold_stress_max_stress_threshold_temperature;                //120416
   Valid_float32  v_cold_stress_sensitivity;                                     //120416
   Valid_float32  v_cold_stress_exposure_damaging_hours;                         //120416
   Valid_int16    v_cold_exposure_initial_hour;                                  //130628

   Valid_float32  v_heat_stress_no_stress_threshold_temperature;                 //120416
   Valid_float32  v_heat_stress_max_stress_threshold_temperature;                //120416
   Valid_float32  v_heat_stress_sensitivity;                                     //120416
   Valid_float32  v_heat_stress_exposure_damaging_hours;                         //120416
   Valid_int16    v_heat_exposure_initial_hour;                                  //130628
 public:
   Yield();
   virtual bool setup_structure
      (CORN::Data_record &record,bool for_write
      ,Harvested_part harvested_part)                             modification_; //171122
};
//_Yield____________________________________________________________2002-04-08_/
class Fruit
:public Crop_parameters_class::Fruit
{
 public:
   Valid_float32  v_fract_total_solids;
   Valid_float32  v_max_fruit_load_kg_ha;
   Valid_float32  v_harvest_temperature;
   Valid_int16    v_harvest_temperature_days;
   Valid_int16    v_harvest_DOY_earliest;
   Valid_int16    v_harvest_DOY_latest;
   Valid_float32  v_initial_growth_fraction_to_fruit;
   Valid_float32  v_rapid_growth_fraction_to_fruit;
   Valid_float32  v_max_water_uptake_before_fruiting;
   Valid_float32  v_max_water_uptake_during_initial_fruit;
   Valid_float32  v_max_water_uptake_during_radid_fruit;
   Valid_float32  v_max_water_uptake_during_veraison;
   Valid_float32  v_max_water_uptake_after_maturity;
 public:
   Fruit();
   VIRTUAL_BOOL_SETUP_STRUCTURE;
};
//_Fruit____________________________________________________________2011-01-21_/
class Nitrogen
: public Crop_parameters_class::Nitrogen
{
 public:
   Valid_float32  v_max_uptake_daily_rate;
   Valid_float32  v_residual_N_not_available_for_uptake;
   Valid_float32  v_soil_N_conc_where_N_uptake_decreases;
   Valid_float32  v_PAW_where_N_uptake_rate_decreases;

   // Deprecated in V 4.17
   #if (CROPSYST_VERSION==4)
   Valid_float32  v_demand_adjust;
   Valid_float32  v_biomass_to_start_dilution_max_N_conc;                        //120505
   #endif


   Valid_float32  v_emergence_max_conc;                                          //121219
   Valid_float32  v_emergence_crit_conc;                                         //121219
   Valid_float32  v_biomass_to_start_dilution_crit_N_conc;                       //121219
   Valid_float32  v_stubble_max_conc;
   Valid_float32  v_root_conc;
   Valid_float32  v_N_mature_max_conc;                                           //120505
   Valid_float32  v_dilution_curve_slope;                                        //121219_120505 was conc_slope
   Valid_int16    v_end_dilution_curve;                                          //130408
 public:
   Nitrogen();
   VIRTUAL_BOOL_SETUP_STRUCTURE;
};
//_Nitrogen_________________________________________________________2001-01-22_/
class Salinity
: public Crop_parameters_class::Salinity // currently no Salinity_class
{public:
   Valid_float32 v_osmotic_pot_50;
   Valid_float32 v_salt_tolerance_P;
 public:
   Salinity();
};
//_Salinity_________________________________________________________2002-04-05_/
class Canopy_architecture //200403 Canopy_characteristics_hedge_row // vineyard       // 0701116 now for both orchards and vinyards
: public Crop_parameters_class::Canopy_architecture ///200403 Canopy_characteristics_hedge_row
{public: // RLN I believe theys are annual values.
   Valid_float32 v_initial_width;
   Valid_float32 v_final_width;
   Valid_float32 v_initial_breadth;
   Valid_float32 v_final_breadth;
   Valid_float32 v_initial_height;
   Valid_float32 v_final_height;
   Valid_float32 v_branch_insertion_height;
 public:
   Canopy_architecture();
};
//_Canopy_characteristics_hedge_row_________________________________2002-04-05_/
struct Layout         // rename to tree layout
: public Crop_parameters_class::Layout
{ public: //
   Valid_float32 v_row_spacing;
   Valid_float32 v_row_azimuth;
   Valid_float32 v_col_spacing;
 public:
   Layout();
};
//_Layout___________________________________________________________2002-04-05_/
class CO2_Response
: public Crop_parameters_class::CO2_Response
{public:  // CO2 specific parameters
   Valid_float32  v_TUE_reference_conc;                                          //131206
   Valid_float32  v_RUE_reference_conc;                                          //131206
   Valid_float32  v_alpha;                                                       //131206
   Valid_float32  v_theta;                                                       //131206
   Valid_float32  v_growth_ratio_asymptotic;                                     //131206
 public:
   CO2_Response();
   VIRTUAL_BOOL_SETUP_STRUCTURE;
};
//_CO2_Response_____________________________________________________2002-04-08_/
class CropGro
{
 public:  // CropGro specific parameters
   CORN::Filtered_file_name model_input_file;
 public:
   CropGro();
};
//_CropGro__________________________________________________________2003-11-21_/
 public:
   Thermal_time                thermal_time;                                     //020628
   //#if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
   Phenology                   phenology;                                        //080722
   //#endif
   Transpiration               transpiration;                                    //110208
   Biomass_production          biomass_production;                               //110218
   Morphology                  morphology;                                       //110219
   Canopy_growth_LAI_based     canopy_growth_LAI_based;                          //080725
   Canopy_growth_cover_based   canopy_growth_cover_based;                        //080725
   Root                        root;                                             //061212
   Quiescence                  quiescence;                                       //160326
   // quiescence was inactive_period                                             //160326_080401
   Dormancy                    dormancy;                                         //080331
   Tolerance                   tolerance;                                        //150913_020408_110606 was pointer
   Residue_decomposition_parameters residue_decomposition;                       //020408 //110606 was pointer
   Vernalization               vernalization;                                    //020405 //110606 was pointer
   Photoperiod                 photoperiod;                                      //020405 //110606 was pointer
   Emergence_hydro_thermal     emergence;                                        //041201
   Salinity                    salinity;                                         //020405 //110606 was pointer
   Layout                      layout;                                           //020405 //110606 was pointer
   Yield                       yield;                                            //200423_020408_110218 was Harvest_index //110606 was pointer
   //200423 Harvest                     harvest;                                          //020408_110218 was Harvest_index //110606 was pointer
   Fruit                       fruit;                                            //020408
   Nitrogen                    nitrogen;  //Nitrogen is optional (not used by fruit crops) //110606 was pointer
   Canopy_architecture         canopy_architecture;                              //020405 //110606 was pointer
   //200403 Canopy_characteristics_hedge_row canopy_characteristics_hedge_row;            //020405 //110606 was pointer
   CO2_Response                CO2_response;
   CropGro                     cropgro;
public:  // The following apply only to version 4
   float32 reference_light_to_biomass_kg_MJ_old;
   bool                        deciduous_obsoleteX;  //141121                     //020405
   Senesced_biomass_fate_clad senesced_biomass_fate_clad_obsolete;               //060817
      // Retained until V5
 public: // validators

// This mode is for compatibility with previous versions
// These parameters are deprecated and is be removed in version 5

   Valid_int16    v_linger_days;                                                 //080910
    // This is the amount of time scenesced leaves linger with (remain attached to) the plant.
//020505       Although Claudio doesn't want residues to show up in the file for orchard model
//             there has to be a residue in order to preserve the balance in the event crop terminates.
 public: // Data record overrides
 public: // Constructor and initialization
   Crop_parameters
      (bool nitrogen_simulation  = true
      ,bool salinity_simulation  = true
      ,bool residue_simulation   = true
      ,bool CO2_simulation       = true);                                        //020408
   virtual ~Crop_parameters();                                                   //020409
   virtual void initialize_fallow();                                             //990217
//             CropSyst Crop class needs to be initialized after loading,
//             Other crop file/parameter users may not need to initialize
//             So this method does not have to be abstract.
 public:
   // Enabled for parameter editor
   bool validate(std::ofstream &validation_file);
   virtual bool expect_structure(bool for_write = false);                        //161023
   virtual bool set_start();                                                     //161023
      // Override VV file because we want to setup the stages set.               //980801
   virtual bool get_end();                                                       //161023_980801
   void setup_optional_parameters();                                             //020419
public:
   inline virtual Crop_parameters_struct::Quiescence                  &mod_quiescence()                { return quiescence; }
   inline virtual Crop_parameters_struct::Thermal_time                &mod_thermal_time()              { return thermal_time;}
   inline virtual Crop_parameters_struct::Biomass_production          &mod_biomass_production()        { return biomass_production; }
   inline virtual Crop_parameters_struct::Vernalization               &mod_vernalization ()            { return vernalization;}
   inline virtual Crop_parameters_struct::Photoperiod                 &mod_photoperiod ()              { return photoperiod;}
   inline virtual Crop_parameters_struct::Transpiration               &mod_transpiration()             { return transpiration; }
   inline virtual Crop_parameters_struct::Morphology                  &mod_morphology()                { return morphology;}
   inline virtual Crop_parameters_struct::Canopy_growth_LAI_based     &mod_canopy_growth_LAI_based ()  { return canopy_growth_LAI_based;}
   inline virtual Crop_parameters_struct::Canopy_growth_cover_based   &mod_canopy_growth_cover_based() { return canopy_growth_cover_based;}
   inline virtual Crop_parameters_struct::Root                        &mod_root()                      { return root;}
   inline virtual Crop_parameters_struct::Fruit                       &mod_fruit()                     { return fruit; }
   inline virtual Crop_parameters_struct::Dormancy                    &mod_dormancy()                  { return dormancy; } //110504
   inline virtual Crop_parameters_struct::Phenology                   &mod_phenology()                 { return phenology; } //190606

   inline virtual const Crop_parameters_struct::Quiescence                 &ref_quiescence()        const { return quiescence; }
   inline virtual const Crop_parameters_struct::Thermal_time               &ref_thermal_time()      const { return thermal_time;}
   inline virtual const Crop_parameters_struct::Biomass_production         &ref_biomass_production()const { return biomass_production; }
   inline virtual const Crop_parameters_struct::Vernalization              &ref_vernalization ()    const { return vernalization;}
   inline virtual const Crop_parameters_struct::Photoperiod                &ref_photoperiod ()      const { return photoperiod;}
   inline virtual const Crop_parameters_struct::Transpiration              &ref_transpiration()     const { return transpiration; }
   inline virtual const Crop_parameters_struct::Morphology                 &ref_morphology()        const { return morphology;}
   inline virtual const Crop_parameters_struct::Canopy_growth_LAI_based    &ref_canopy_growth_LAI_based ()  const { return canopy_growth_LAI_based;}
   inline virtual const Crop_parameters_struct::Canopy_growth_cover_based  &ref_canopy_growth_cover_based() const { return canopy_growth_cover_based;}
   inline virtual const Crop_parameters_struct::Root                       &ref_root()                      const { return root;}
   inline virtual const Crop_parameters_struct::Fruit                      &ref_fruit()               const { return fruit; }
   inline virtual const Crop_parameters_struct::Dormancy                   &ref_dormancy()            const { return dormancy; } //110504
   inline virtual const Crop_parameters_struct::Phenology                  &ref_phenology()           const { return phenology; } //190606

   //#if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
//   inline  virtual Crop_parameters_struct::Phenology                   &mod_phenology()                 { return phenology;}
//   inline virtual const Crop_parameters_struct::Phenology              &ref_phenology()           const { return phenology;}
   //#endif
   //NYI inline  virtual Crop_parameters_struct::Transpiration               &ref_transpiration()             { return transpiration; }

//NYN inline virtual const Crop_parameters_struct::Inactive_period             &ref_inactive_period()           const { return inactive_period;}

//NYN inline virtual const Crop_parameters_struct::Emergence_hydro_thermal     &ref_emergence ()                const { return emergence;}

#ifdef CROPSYST_PROPER
   inline virtual Crop_parameters_struct::Emergence_hydro_thermal     &mod_emergence ()                { return emergence;}
   //ref_emergence
#endif

 public: // I think these XXX_class names are unused oboslete (see methods above)
   inline virtual Crop_parameters_class::Transpiration       &mod_transpiration_class()       { return transpiration;}
   inline virtual Crop_parameters_class::Biomass_production  &mod_biomass_production_class()  { return biomass_production;}
   inline virtual Crop_parameters_class::Tolerance           &mod_tolerance_class()           { return tolerance; }
   inline virtual Crop_parameters_class::Morphology          &mod_morphology_class()          { return morphology; }
   inline virtual Crop_parameters_class::Phenology           &mod_phenology_class()           { return phenology; } //190701
   inline virtual Crop_parameters_class::Yield               &mod_yield_class()               { return yield;} //200423
   //200423 inline virtual Crop_parameters_class::Harvest             &mod_harvest_class()             { return harvest;}
   inline virtual Crop_parameters_class::Nitrogen            &mod_nitrogen_class()            { return nitrogen;}
   inline virtual Crop_parameters_class::CO2_Response        &mod_CO2_response_class()        { return CO2_response;}
   inline virtual Crop_parameters_class::Fruit               &mod_fruit_class()               { return fruit; }
   inline virtual Crop_parameters_class::Emergence_hydro_thermal &mod_emergence_class()       { return emergence;}
 public:
   inline virtual float64 param_base_temp()                                const { return thermal_time.temperature.base;}    //181111
   inline virtual float64 param_cutoff_temp()                              const { return thermal_time.temperature.cutoff;}  //181111
   inline virtual float64 param_opt_temp()                                 const { return thermal_time.temperature.opt;}     //181111_021124
   inline virtual float64 param_max_temp()                                 const { return thermal_time.temperature.max;}     //181111_021124

// 020628 these needed to be made relative to Thermal_time_parameters
//    inline virtual float64 param_phenologic_sensitivity_water_stress()   const { return phenologic_sensitivity_water_stress}#endif
// eliminating param_XXX() methods because now passing the parameter data structures to the classes where used

   inline virtual float64 param_max_canopy_height()                        const { return morphology.max_canopy_height; } // 110218  //990213
   inline virtual float64 param_fruit_damage_cold_temp()                   const { return tolerance.temperature_cold_fruit_damage; }  //991029
   inline virtual float64 param_fruit_lethal_cold_temp()                   const { return tolerance.temperature_cold_fruit_lethal; }  //991029
   inline virtual float64 param_wilt_leaf_water_pot()                      const { return transpiration.LWP_wilt_point;}   //110218
   inline virtual Land_use  param_land_use()                               const { return (Land_use)land_use_clad.get(); }   //020324_
   virtual bool is_deciduous()                                             const
      { return abscission_clad.get() == deciduous; }                             //141121
};
//_Crop_parameters_____________________________________________________________/
}//_namespace CropSyst_________________________________________________________/
#endif

