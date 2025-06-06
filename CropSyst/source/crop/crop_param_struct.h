#ifndef crop_param_structH
#define crop_param_structH
#include "corn/primitive.h"
#include "crop/crop_types.h"
#include "corn/chronometry/date_32.h"
#include "corn/enum.h"
#include "corn/measure/measures.h"
#include "corn/metrology/units_clad.h"
#include "corn/container/unilist.h"



#if ((PHENOLOGY_VERSION==2018))
#include "corn/data_source/datarec.h"
#endif
/* This module defines a crop parameters commonly used
in a number crop simulation models.
DO NOT DELETE any of these variable even though they dont appear
in a particular model without consulting Roger Nelson rnelson@wsu.edu
 */

//190630 #if ((PHENOLOGY_VERSION==2018))
#  if CROPSYST_VERSION < 6
#     define PHENOLOGY_SEASON_ONTOLOGY "phenology/season"
// This phenologic period is only to use legacy crop files.
#  else
#     define PHENOLOGY_SEASON_ONTOLOGY ""
#  endif
#  define CONSECUTIVE_PERIOD true
#  define INDEPENDENT_PERIOD false
//190630 #endif

namespace CropSyst
{
   #if (CROPSYST_VERSION == 4)
   // 121219 These became deprecated in version 4.17.00 because dilution curves are now based on critical curve not maximum curve
   extern const float biomass_to_start_dilution_max_N_conc_default
      [PHOTOSYNTHETIC_PATHWAY_COUNT]; // T/ha  was in crop_N as biomass_to_start_dilution_max_N_conc_species
   extern const float biomass_to_start_dilution_max_to_crit_mult
      [PHOTOSYNTHETIC_PATHWAY_COUNT];
   extern const float biomass_to_start_dilution_max_to_min_mult
      [PHOTOSYNTHETIC_PATHWAY_COUNT];
   #endif
   extern const float biomass_to_start_dilution_crit_N_conc_default
      [PHOTOSYNTHETIC_PATHWAY_COUNT]; // T/ha these are the defaults for the parameter
   extern const float biomass_to_start_dilution_crit_to_max_mult
      [PHOTOSYNTHETIC_PATHWAY_COUNT];
   extern const float biomass_to_start_dilution_crit_to_min_mult
      [PHOTOSYNTHETIC_PATHWAY_COUNT];

   extern const float N_mature_max_conc_default[PHOTOSYNTHETIC_PATHWAY_COUNT];
   // was in crop_N as N_max_conc_at_maturity_species
   extern const float N_mature_max_to_crit_mult[PHOTOSYNTHETIC_PATHWAY_COUNT];
   extern const float N_mature_max_to_min_mult[PHOTOSYNTHETIC_PATHWAY_COUNT];
   extern const float N_dilution_curve_slope_default
      [PHOTOSYNTHETIC_PATHWAY_COUNT]; // 121219 was N_conc_slope_default
};

//______________________________________________________________________________
namespace CropSyst
{
//______________________________________________________________________________
struct Crop_parameters_struct
{
   struct Thermal_time
   {
      Thermal_response thermal_response;                                          //151110
      struct Temperature
      {
         float32 base; // both daily linear, non-linear and hourly mode
         float32 cutoff; // only for daily linear mode
         float32 opt; // only for daily non-linear and hourly mode               //021124
         // The optimal temperature must not be lower than base_temperature
         float32 max; // only for daily linear and hourly mode                   //021124
      } temperature;
    public:
      inline Thermal_time()                                                   {} //150904
      inline Thermal_time(const Thermal_time & copy_from)                        //181119
      : thermal_response(copy_from.thermal_response)
      // NYI , temperature(copy_from.temperature)
      {  temperature.base     = copy_from.temperature.base;
         temperature.cutoff   = copy_from.temperature.cutoff;
         temperature.opt      = copy_from.temperature.opt;
         temperature.max      = copy_from.temperature.max;
      }
   };
   //_struct Thermal_time___________________________________________2002-06-28_/
   //190628 #if ((PHENOLOGY_VERSION==2018))
   #define RELEVANT_VERNALIZATION 1
   #define RELEVANT_PHOTOPERIOD   2
   #define RELEVANT_CLIPPING      4

   class Phenologic_period : public implements_ CORN::Item
   {
    public:
      enum Basis { event_basis, initiation_basis, culmination_basis };
      std::string ontology;

      // This would (generally) alias the more general the ID of the period
      //
      struct Initiation
      {  std::string relative_to;
         // I.e. phenology/emergence (default if omitted growing season)
         Basis basis; // I.e. initiation culmination
         float32 thermal_time_accum; // Thermal time  (offset)
      } initiation;
      float32 duration_GDDs; // Thermal time
      // duration may be 0 indicating an instanteous event
      // thermal time must also be 0 (nothing to accumulate to)
      bool consecutive;
      // Consecutive indicates that the periods accumulated thermal time
      // would be cumulative as in legacy thermal time accumulation.
      nat8 relevant;
      // bitmask flags
      // 0 vernalization
      // 1 photoperiod
      // 2 clipping
      // now using bitmask bool clipping_resets;
      // for alfalfa and grasses and probably other crops                        //040719
      Thermal_time *thermal_time;                                                //181118
      // NYI const Thermal_time &thermal_time;
      #if ((CS_VERSION>=4) && (CS_VERSION<=5))
      nat32 variable_code_UED;
      #endif
    public:
      Phenologic_period
         (const std::string &ontology
         ,std::string init_relative_to
         ,Basis init_basis = event_basis
         ,float32 init_thermal_time_accum = 0
         ,float32 duration_GDDs = 99999
         , bool consecutive = true
         , nat8 relevant = 0
         , /* const */ Thermal_time *thermal_time_ = 0
         #if ((CS_VERSION>=4) && (CS_VERSION<=5))
         , nat32 variable_code_UED = 0
         #endif
         );
      Phenologic_period();
      inline bool is_valid()                                        affirmation_
      {  return ontology.length(); }
         //181228 init thermal time can be 0  (I.e. emergence) initiation.thermal_time_accum &&
      inline virtual const std::string &append_to_string(std::string &buffer)
      {  buffer.append(ontology); return buffer; }
      inline virtual bool is_key_cstr(const char *key)              affirmation_
      {  return ontology == key; }
   };
   //_Phenologic_period_____________________________________________2018-11-07_/
   struct Phenology
   {
      float32 phenologic_sensitivity_water_stress;
      #if (!defined(YAML_PARAM_VERSION) || (CROPSYST_VERSION==4))
      float32 WUE_change;
      // Degree days to crop change in water use efficiency for WUE_changes mode.//080510
      // In version 5 WUE will be a temporaly adjustable parameters.
      #endif
      // 181110 #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))

      // The stages are for Phenology_2013, these are still loaded
      // to import earlier crop parameter files.
      struct Stage                                                               //150905_150831
      { // value in degree days
         float32 emergence;
         float32 accrescence; // active growth                                   //150913
            // as of 190627 we no longer use culmination.accrescence
            // we use duration
         float32 culminescence;                                                  //191121
         float32 senescence;                                                     //190630
         // Now scenescence is relative to canopy

         float32 tuberization;                                                   //181110 tuber;
         float32 root_elongation;                                                //150913_070321
         float32 flowering; // rename to anthesis
         float32 filling;                                                        //100315
         // Claudio is using the term yield_formation for filling
         float32 rapid_fruit_growth;                                             //150929
         float32 veraison;
         float32 maturity;                                                       //050511_100316
         // rename to begin_physiological_maturity_deg_day
         // needs to be float to allow very larger numbers for long term perennials such as trees

         float32 senescence_obsoleteX;
         #if (defined(YAML_PARAM_VERSION))
         float32 season; // (was in canopy )                                     //171219
         // when harvested before maturity or if no maturity
         #endif
       public:
         inline Stage()                                                          //160211
         : emergence                                                         (0)
         , accrescence                                                       (0)
         , culminescence                                                     (0) //191121
         , senescence                                                        (0)
         , tuberization                                                      (0)
         , root_elongation                                                   (0) //181109
         , flowering                                                         (0)
         , filling                                                           (0)
         , rapid_fruit_growth                                                (0)
         , veraison                                                          (0)
         , maturity                                                          (0)
         , senescence_obsoleteX                                              (0)
         #if (defined(YAML_PARAM_VERSION))
         , season                                                            (0) //171219
         #endif
         { } // Clear constructor
         inline Stage(const Stage & copy_from)                                   //160211
         : emergence                              (copy_from.emergence)
         , accrescence                            (copy_from.accrescence)
         , culminescence                          (copy_from.culminescence)      //191121
         , senescence                             (copy_from.senescence)
         , tuberization                           (copy_from.tuberization)
         , root_elongation                        (copy_from.root_elongation)    //181109
         , flowering                              (copy_from.flowering)
         , filling                                (copy_from.filling)
         , rapid_fruit_growth                     (copy_from.rapid_fruit_growth)
         , veraison                               (copy_from.veraison)
         , maturity                               (copy_from.maturity)
         , senescence_obsoleteX                   (copy_from.senescence_obsoleteX)
         #if (defined(YAML_PARAM_VERSION))
         , season                                 (copy_from.season)             //171219
         #endif
         { } // Copy constructor
      };
      Stage initiation;
      Stage duration;
         // currently only accrescence and senescence  durations are used

      Stage culmination_obsolete;
         // culmination is mostly deprecated and was the idiom for V4
         // Currently only root elongation uses this so I need to switch
         // over to use period initiation duration idiom.

      bool maturity_significant;                                                 //080319
      // Some perennial crops Fruit trees do need physiological maturity even thought they are perennial
      // Some crops although grown annually such as potatos and tomatos could keep growing indefinitely
      // So, we now have an option for the user to select is physiological maturity is significant
      // for growth equations.
      nat32 stages_relevent;                                                     //150905
      bool clipping_resets; // for alfalfa and grasses and probably other crops  //040719
      // 181110 #endif

   #if (CROPSYST_VERSION < 6)
   //190701 The period idiom was added for canopy growth in V4
   // phenology HENOLOGY_VERSION==2013
   // These will also be used PHENOLOGY_VERSION==2018 when upgrading.
   // But eventually these will be fully dynamically instanciated
   Phenologic_period *accrescence_period;
   Phenologic_period *culminescence_period;
   Phenologic_period *senescence_period;
   Phenologic_period *root_elongation_period;
   #endif

      #if (PHENOLOGY_VERSION==2018)
      CORN::Unidirectional_list phenologic_periods;                              //181109
      //#else
      //CORN::Unidirectional_list canopy_periods;                                  //190628
      #endif
    public:
      inline Phenology()
      : phenologic_sensitivity_water_stress(0)
      #if (!defined(YAML_PARAM_VERSION) || (CROPSYST_VERSION==4))
      , WUE_change(0)
      // Degree days to crop change in water use efficiency for WUE_changes mode.//080510
      // In version 5 WUE will be a temporaly adjustable parameters.
      #endif
      , initiation()
      , culmination_obsolete()
      , maturity_significant(false)
      , stages_relevent(0)
      // probably should initialize to something realistic, Parameter calss is currently setting this up
      , clipping_resets(false)
      { } // clear constructor
      inline Phenology(const Phenology & copy_from)
      : phenologic_sensitivity_water_stress
         (copy_from.phenologic_sensitivity_water_stress)
      #if (!defined(YAML_PARAM_VERSION) || (CROPSYST_VERSION==4))
      , WUE_change(copy_from.WUE_change)
      // Degree days to crop change in water use efficiency for WUE_changes mode.//080510
      // In version 5 WUE will be a temporaly adjustable parameters.
      #endif
      // 181110 #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
      , initiation(copy_from.initiation)
      , culmination_obsolete(copy_from.culmination_obsolete)
      , maturity_significant(copy_from.maturity_significant)
      , stages_relevent(copy_from.stages_relevent)
      , clipping_resets(copy_from.clipping_resets)
      // 181110 #endif
      { } // copy_constructor
   };
   //_Phenology_____________________________________________________2008-07-22_/
   struct Transpiration
   {
      float32 ET_coef; // at maximum canopy cover was ET_crop_coef
      float32 LWP_wilt_point; // J/kg    Wilting leaf water potential.
      float32 LWP_stomatal_closure; // J/kg    Leaf water potential at the onset of stomatal closure. (020418 was critical_leaf_water_pot) (110208 was stomatal_closure_leaf_water_pot)
      float32 normal_max_water_uptake_mm; // mm/day  (kg/m2) For normal crops, not that fruit tree crops have max uptake by growth stage
      /* 190411 since Jordi has passed on, these will probably never be used again
      float32  a1; //intercept
      float32  a2; //slope
      // a1 and a2 are parameters added specially for Jordi Marsal for testing. 121114
      // these are coefficients of a regression so.
      // Values of 0 effectively disable these.
       */
   };
   //_struct Transpiration__________________________________________2011-02-08_/
   struct Biomass_production
   { // This is stored under the section [growth] so may want to rename
      bool WUE_changes;                                                          //080506
      // Normally Water Use Efficiency coefficient is the same throughout
      // vegetative growth, but some users way want a separate
      // growth curve coefficients for active vegetative growth and
      // reproductive/yield growth (after active vegetative) growth.
      // When this is not enabled a single value (normal) TUE_at_1kPa_VPD,
      // water_productivity, or reference_biomass_transpiration_coef  is used.
      // The degree days at which there is a change in WUE is stored
      // in phenology thermal time parameters                                    //080514

      // This is only used in V4, in V5 all parameters are adjustable.

      #if (CROPSYST_VERSION< 6)
      //191025 now always using Transpiration_Use_Efficiency
      TUE_Equation_obsolete TUE_equation_obsolete;                               //191025_080505
      #endif
      #if (CROPSYST_VERSION==4)
      // 191025 now always total radiation basis all versions
      RUE_Basis_obsolete RUE_basis_obsolete;                                     //191025_080731
      #endif


      float32 continuous_cultivation_impact_factor;                              //151031
      // Adjustment to biomass production due to continuous cultivation
      // where there may occur soil compaction, pest damage and other
      // deleterious effects.
      // Was season_biomass_adjustment, now using parameter adjustment mechanism.
      #if (defined(YAML_PARAM_VERSION))
      // YAML version uses the new parameter adjustment mechanism
      float32 TUE_at_1kPa_VPD_adjustable; // g biomass /kg H20                   //080428
      // Transpiration Use Efficiency when VPD is at 1kPa.
      // Used by Tanner-Sinclair and TUE_curve TUE equations.
      float32 TUE_scaling_coef_adjustable; // unitless                           //080505
      // (May need to rename this slope)
      // Used by TUE_curve TUE_equation
      /*191026 Aquacrop no obsolete
      float32 water_productivity_adjustable; // g biomass / m2                   //080428
      // ?Transpiration Use Efficiency? when VPD is at 1kPa.
      // Use by AquaCrop TUE_equation
      */
      /*191026 tanner sinclair now obsolete
      // The following are for the Tanner Sinclair equation
      float32 reference_biomass_transpiration_coef_adjustable;
      // Pascal Above ground biomass-transpiration coef.                         //040729
      */
      #endif
      float32 season_biomass_adjustment[6];                                      //080731
      // replaced with continuous_cultivation_impact_factor

      // Used by Transpiration_dependent_growth_V5 (YAML)
      #if (!defined(YAML_PARAM_VERSION) || (CROPSYST_VERSION==4))
      // The following are for TUE curve mode    probably rename to WUE  water use VPD
      float32 TUE_at_1kPa_VPD // g biomass /kg H20                               //080428
         [2] // Index 0 is normal mode (and/or after vegetative growth)
         ; // Index 1 is only during vegetative growth (for WUE_changes mode).
      // Transpiration Use Efficiency when VPD is at 1kPa.
      float32 TUE_scaling_coef // May need to rename this slope  unitless        //080505
         [2] // Index 0 is normal mode (and/or after vegetative growth)          //091201
         ; // Index 1 is only during vegetative growth (for WUE_changes mode).
      /*191026 aquacrop now obsolete
      float32 water_productivity // g biomass / m2                               //080428
         [2] // Index 0 is normal mode (and/or after vegetative growth)
         ; // Index 1 is only during vegetative growth (for WUE_changes mode).
      // ?Transpiration Use Efficiency? when VPD is at 1kPa.
      */
      /*191026 Tanner Sinclair now obsolete
      // The following are for the Tanner Sinclair equation
      float32 reference_biomass_transpiration_coef // Pascal Above ground biomass-transpiration coef. //040729
         [2] // Index 0 is normal mode (and/or after vegetative growth)
         ; // Index 1 is only during vegetative growth (for WUE_changes mode).
      */
      #endif
      // RUE='Radiation use efficiency'

      // 191025 RUE now has two meanings/cases:
      // when using TUE efficiency with RUE efficiency limitation
      // when only using RUE efficiency

      std::string efficiency_model_raw;                                          //191026
      Efficiency_model efficiency_model;                                         //191026
      //#define TUE_EFFICIENCY 0
      //#define RUE_EFFICIENCY 1
         // TUE or RUE (TUE is most commonly used)

      //191026 float32 RUE_global; // g/MJ                                          //080428
      #define at_low_VPD             0
      #define at_low_total_solar_rad 1
      float32 RUE[2];   // g/MJ                                                         //191026

      //191031 float32 RUE_global_obsolete; // g/MJ                                       //191031
         // RUE[at_low_VPD] is analygous to the old RUE_global
      float32 RUE_efficiency_decrease_rate;   // (g m2 d)/ MJ2                                   //191026
         // rate of decrease per unit of daily solar radiation (total)

      // RUE global radiation basis at high relative humidity (low VPD)
      /*191025 haven't used PAR option for several year
      float32 RUE_PAR_deprecated; // g/MJ                                        //080428
      // RUE PAR basis at high relative humidity (low VPD)
      // V5 now uses only RUE_global                                             //171219
      */

      struct Temperature_limitation
      {  // For RUE
         float32 min;            //191025 was min_tolerable_temp
         float32 low_optimum;    //191025 was low_threshold_limit_temp
         float32 high_optimum;   //191025 was high_threshold_limit_temp
         float32 max;            //191025 was max_tolerable_temp
      } temperature_limitation;                                                  //191025

      // rename threshold to optimum I.e.  temperature_optimal_low
      #define WUE_at_1_kPa_VPD TUE_at_1kPa_VPD[0]
   };
   //_struct Biomass_production_____________________________________2011-02-08_/
   struct Vernalization
   {
      float32 low_temp;    // -4 - 5
      float32 high_temp;   // 7 - 12
      float32 start;       // 0 - vernReq C
      float32 end;         // 0 - 50 C
      float32 min_factor;  // 0 - 1
      bool enabled;                                                              //020718
   };
   //_struct Vernalization__________________________________________2002-04-05_/
   struct Photoperiod
   {
      Photoperiodism photoperiodism;                                             //151122
      float32 stop_day_length; // 0-24 hours
      float32 unconstrained_day_length; // 0-24 hours
   };
   //_struct Photoperiod____________________________________________2010-05-12_/
   struct Emergence_hydro_thermal // was Emergence_parameters                    //100325
   {
      Emergence_model emergence_model; // rename to germination
      float32 a_slope;
         // Emperical constant for computing  hydrothermal time
         // rename this to slope (although a is conventionally intercept it appears to be used as slope in the eqauation
      float32 b_intercept;
      // Emperical constant for computing  hydrothermal time  rename this to intercept
      // (although b is conventionally slope, it appears to be used as intercept in the equation.
      float32 seedling_leaf_area_cm2; // cm2
      // leaf area of seedling on first day of emergence
      float32 germination_base_temperature; // degress 0
      float32 mean_base_soil_water_pot_J_kg; // J/kg
      // Mean base soil water potential (50% emergence)
      float32 stdev_base_soil_water_pot_J_kg; // J/kg
      // Std deviation base soil water potential (?for 50% emergence?)
   };
   //_struct Emergence_hydro_thermal____________________2010-03-25__2004-12-01_/
   struct Quiescence
   {
      // Only use the DOY part
      float32 inducement_temperature; // 'C                                      //080401
      // was under dormancy
      int16 consider_inactive_days;                                              //080401
      // was hardcoded as 7 days  actually unsigned
   };
   //_struct Quiescence________________________________________________________/
   struct Dormancy
   {
      float32 chill_hours; // chill units 1 unit equal 1 hour exposure at 6'C    //021125
      // 081125 should rename this to chill_units?
      float32 deg_day_bud_break_chill_req_sat;                                   //030515
      float32 deg_day_bud_break_chill_req_not_sat;                               //030515
      float32 senesce;                                                           //060317
   };
   //_struct Dormancy__________________________________________________________/
   struct Root
   {
      float32 max_depth; // m maximum rooting depth
      float32 surface_density; // cm/cm3                                         //030722
      // RLN I believe this is maximum                                           //140817
      // For V5 this should probably be dynamic                                  //140817
      float32 density_distribution_curvature; // 0.0001-10                       //030722
      float32 length_per_unit_mass_km_kg_V4;                                     //060612
      // I don't see length_per_unit_mass_km_kg_V4 used anymore
      float32 root_shoot_emergence_ratio;                                        //130624
      float32 root_shoot_full_ratio;                                             //130624
      float32 sensitivity_to_water_stress;                                       //061212

      float32 length_at_emergence_cm;                                            //061212
         // In the future should be m for consistency (YAML version will allow the user to specify units

      float32 initial_depth_deprecated;                                          //120817
      // Root depth at above ground biomass emergence.
      // This includes the root length and the sow_depth
      // (In the case of CropSyst, the evaporative layer thickness)
      float32 at_pt_root_limit;
      // 0-1      Act/pot. transpiration ratio that limits root growth. (Used in CropSyst VB not used in CropSyst C++)
    public:
      inline float32 get_depth_emergence_m_deprecated()                    const
      {  return initial_depth_deprecated > 0.00000001
         ? initial_depth_deprecated                                              //120817
         : (cm_to_m(length_at_emergence_cm) /* + sow_depth */ );
      }
   };
   //_struct Root___________________________________________________2006-12-12_/
   struct Morphology
   {
      Canopy_growth_model canopy_growth_model;                                   //080725
      bool canopy_architecture;                                                  //190627
      float32 max_canopy_height; // m                                            //990213
      // maximum canopy height, not currently used.


      // The following are actually canopy parameters    200128
      // Claudio say LWP are actually only relevent to canopy curve model
      // but it appears to be general to canopy cover
      float32 reduce_canopy_expansion_LWP;   // J/kg                             //080506
      // was LWP_that_reduces_canopy_expansion
      float32 stop_canopy_expansion_LWP;     // J/kg                             //080506
      //// was LWP_that_stops_canopy_expansion


      // light_extinction_coef is relevent to all canopy models
      float32 light_extinction_coef;         // 0-1                              //191105
      // Extinction coefficient for (total or photosynthetically active) solar radiation
      // kc renamed to light_extinction_coef (move to canopy)
   };
   //_struct Morphology_____________________________________________2011-02-18_/
   struct Canopy_growth_cover_based
   { // The following are for the canopy cover based canopy growth
      float32 cover_initial; // fraction? // 0.01
         // total and green?
      float32 cover_maximum_no_CO2_adj; // fraction? // 0.9
         // total and green?

      // 140205 Do not use cover_maximum_no_CO2_adj directly when using
      // CO2, it needs to be adjusted for CO2
      //#ifndef SENESCENCE_REDUCTION_200124
      float32 cover_total_season_end; // fraction? // 0.1 total Canopy Cover at Maturity
      //#endif
      float32 cover_green_season_end; // fract green canopy cover at end of season //171009
      float32 shape_coef_accrescent;                                             //171221
      float32 shape_coef_senescent;                                              //171221
      #if (!defined(YAML_PARAM_VERSION) || (CROPSYST_VERSION==4))
      float32 thermal_time_season_end_obsolete;                                  //171219
      // thermal_time_season_end (would be phenology.culmination.senescence)
      #endif
      // rename to X to whatever this represents (ask Claudio)
      // 181017 renamed float32 fract_season_to_reach_cover_max_half;
      // fraction (was 0.45 now use 0.5025 (value introduced in VIC))
      float32 thermal_time_fractional_at_half_cover_max;                         //181015
      float32 thermal_time_fractional_at_half_cover_senescence;                  //181015
      float32 cover_maximum_CO2_adj; // fraction?                                //140205
    public:
      Canopy_growth_cover_based();                                               //140205
      virtual bool initialize                                                    //140205
      (float64 actual_biomass_gain_ratio_in_response_to_CO2
         // NYI ,float64 extinction_coef
      )                                                         initialization_; //180111
   };
   //_struct Canopy_growth_cover_based______________________________2008-07-23_/
   struct Canopy_growth_LAI_based
   { // LAI = Leaf Area Index
      // GAI = Green (leaf) Area Index
      float32 initial_GAI;                                                       //080207
      float32 regrowth_GAI;                                                      //080207
      float32 max_LAI;
      float32 specific_leaf_area; // GAI m2/kg
      float32 fract_LAI_mature;
      float32 stem_leaf_partition; // GAI LAI curvature
      float32 leaf_duration_deg_day;                                             //050628
      // Needs to be float to allow very large numbers
      float32 leaf_duration_sensitivity_stress; // New GAI 1-2
      bool clipping_removes_newest_first;
   };
   //_struct Canopy_growth_LAI_based________________________________2008-07-25_/
   struct Temperature_sensitivity
   {
      struct Temperature
      {  float32 no_stress_threshold_temperature;
         float32 max_stress_threshold_temperature;
         float32 sensitivity;
         int16 /* CORN::Hour */ exposure_initial_hour;                           //130628
         float32 exposure_damaging_hours;
         inline void set_default()
         {  sensitivity = 1.0;
            exposure_initial_hour = 99; // non 24hour indicates sunrise          //130624
            exposure_damaging_hours = 4.0;
         }
         inline void copy_from(const Temperature &from_copy)
         {  no_stress_threshold_temperature  = from_copy.no_stress_threshold_temperature;
            max_stress_threshold_temperature = from_copy.max_stress_threshold_temperature;
            sensitivity                      = from_copy.sensitivity;
            exposure_initial_hour            = from_copy.exposure_initial_hour;  //130624
            exposure_damaging_hours          = from_copy.exposure_damaging_hours;
         }
      };
      //_struct_Temperature_________________________________________2013-04-16_/
      Temperature cold;
      Temperature heat;
      inline void set_default()
      {  cold.set_default();
         cold.no_stress_threshold_temperature = -0.5;
         cold.max_stress_threshold_temperature = -2.0;
         cold.exposure_initial_hour = 0;
         heat.set_default();
         heat.no_stress_threshold_temperature = 31.0;
         heat.max_stress_threshold_temperature = 44.0;
         heat.exposure_initial_hour = 99;
      }
      inline void copy_from(const Temperature_sensitivity &from_copy)
      {  cold.copy_from(from_copy.cold);
         heat.copy_from(from_copy.heat);
      }
   };
   //_struct Temperature_sensitivity________________________________2013-05-02_/
   struct Tolerance
   {
      bool enabled;                                                              //021015
      float32 temperature_cold_fruit_damage; // 'C  begins to damage crop.       //150913_081129
      float32 temperature_cold_fruit_lethal; // 'C  is lethal to crop.           //150913_081129
      // Cold temperature which:
      float32 temperature_cold_leaf_damage;  // 'C  begins to damage crop.       //150913_991116
      float32 temperature_cold_leaf_lethal;  // 'C  is lethal to crop.           //150913_991116
      float32 temperature_heat_tuber_critical;                                   //081109
   };
   //_struct Tolerance______________________________________________2011-01-21_/
   struct Yield // was Harvest_index   was also Harvest
   { // Common harvest parameter for specific crops
      float32 harvest_index_unstressed; // 0-1 (rename to index_unstressed_or_fixed)
      bool apply_adjustment_to_unstressed_harvest_index;
      //200423 renamed apply_temperature_extreme_response
      // bool apply_heat_extreme_adjustment_to_unstressed_harvest_index;            //150206
      bool apply_water_response_flowering;                                       //200423
      bool apply_temperature_response_flowering;                                 //200423
      bool apply_temperature_extreme_response;                                   //200423_150206

      // response parameter
      float32 water_stress_sensitivity;                                          //200424
      float32 temperature_stress_sensitivity;                                    //200424

      // temperature response parameters
      float32 response_temperature_optimum;                                      //200423
      float32 response_temperature_damaging;                                     //200423

      // Note in V5 new crop file will not store these flags
      // these options will be applied only if the
      // adjustment parameters enabling the option are introduced:
      // (translocation_fraction max) enables apply_adjustment_to_unstressed_harvest_index
      // Temperature sensitivity enables apply_heat_extreme_adjustment_to_unstressed_harvest_index

      float32 stress_sensitivity_growth; // 0-1  (for leaf crops)                //150915_020419
      // Harvest parameters for tuber crops
      float32 stress_sensitivity_tuber_init; // 0-1                              //150915
      float32 tuber_growth_sensitivity; // 0-1

      // The following apply to VIC_crop and now grain asymtotic mode

      float32 translocation_fraction_max;
      Temperature_sensitivity editor_grain_stress_sensitivity;                   //130428
      // We now have a list of these but this  is used by the parameter editor
      // It is not saved to the file
   };
   //_struct Harvest________________________________________________2002-04-09_/
   struct Fruit
   {
    public: // harvest parameters for all fruits
      bool grape; // if true then we have veraison                               //021206
      float32 fract_total_solids; // (0-1)                                       //000616
      float32 max_fruit_load_kg_ha; // (fresh weight)                            //000616
      /* 180601 was for Jordi but never used
      float32 initial_fruit_mass;                                                //071127
      float32 initial_reserves_release_rate;                                     //071127
      int16 clusters_per_vine;                                                   //071127
      int16 berries_per_cluster;                                                 //071127
      int16 fruits_per_tree;                                                     //071127
       */
      Harvest_condition_mode harvest_condition_mode /* labeled */ ;
      float32 harvest_temperature;                                               //081204
      int16 harvest_temperature_days;                                            //081204
      int16 harvest_DOY_earliest; // or fixed date                               //081204
      int16 harvest_DOY_latest;                                                  //081204
      #define harvest_DOY_fixed harvest_DOY_latest
      #ifdef YAML_PARAM_VERSION
      // using adjustable parameter idiom
      float32 growth_fraction_to_fruit;                                          //030521
      #endif
      #if (!defined(YAML_PARAM_VERSION))
      // The following are probably going to be obsolete with the new fruit model  071120
      float32 initial_growth_fraction_to_fruit;                                  //030521
      // was development_fraction_to_fruit
      float32 rapid_growth_fraction_to_fruit;                                    //030521
      // was veraison_fraction_to_fruit
      #endif
      #ifdef YAML_PARAM_VERSION
      // In V5 maximum water uptake will be adjustable parameters (using normal max_water_uptake)
      #endif
      #if (!defined(YAML_PARAM_VERSION))
      // In the YAML version we will have (step) interpolation.
      float32 max_water_uptake_before_fruiting_mm;                               //091202
      float32 max_water_uptake_during_initial_fruit_mm;                          //091202
      float32 max_water_uptake_during_radid_fruit_mm;                            //091202
      float32 max_water_uptake_during_veraison_mm;                               //091202
      float32 max_water_uptake_after_maturity_mm;                                //091202
      #endif
   };
   //_struct Fruit__________________________________________________2002-04-08_/
   struct Nitrogen
   { // Nitrogen is optional (not used by row crops)
      bool fixating; // all legumes fixate now
      // fixating is being replaced with fixation
      Nitrogen_fixation fixation;                                                //170117
      // this will be new V5 or YAML version
      float32 legume_fraction; // legume_fraction of crop mix is a special parameter for the CFF (temporary) for pasture crops that are a mixture of legume and gremilia. //070105
      float32 max_uptake_daily_rate_kg_ha; // kg/ha/day Maximum Nitrogen Uptake during rapid linear growth () //061129
      float32 residual_N_not_available_for_uptake;                               //070209
      float32 soil_N_conc_where_N_uptake_decreases;                              //070209
      float32 PAW_where_N_uptake_rate_decreases;                                 //070209
      float32 stubble_max_conc; // Max N content of standing stubble. kgN/kgDM
      float32 root_conc; // Standard N content of root.kgN/kgDM
      float32 dilution_curve_max_conc_mature; // was  N_mature_max_conc          //150913_120505
      provided_ float32 dilution_curve_slope;                                    //120507_121219
      // rename in parameter editor to dilution_curve_slope 121219

      #if (CROPSYST_VERSION == 4)
      // The N dilution was changed from max_N curve to crit_N curve
      float32 demand_adjust;                                                     //061213
      float32 biomass_to_start_dilution_max_N_conc_T_ha; /* T/ha */              //120505
      bool dilution_curve_based_on_max_conc;                                     //121219
      // This was always true prior to V4.17
      #endif
      float32 emergence_max_conc;                                                //121219
      float32 emergence_crit_conc;                                               //121219
      float32 biomass_to_start_dilution_crit_N_conc_T_ha;
      /* T/ha */                                                                 //121219
      // This is now entered as a positive number
      int16 end_dilution_curve_deg_day;                                          //130408
      /*200529
      Nitrogen_leaf_stress_mode nitrogen_leaf_stress_mode
      ;                       //070224
      */
      float32 leaf_expansion_sensitivity;                                        //200529
      float32 TUE_stress_sensitivity;                                            //151117
    public:
      inline float32 calc_N_mature_crit
         (Photosynthetic_pathway photosynthetic_pathway)            calculation_
      {  return dilution_curve_max_conc_mature
         * CropSyst::N_mature_max_to_crit_mult[photosynthetic_pathway];
      }
      inline float32 calc_N_mature_min
         (Photosynthetic_pathway photosynthetic_pathway)            calculation_
      {  return dilution_curve_max_conc_mature
            * CropSyst::N_mature_max_to_min_mult[photosynthetic_pathway];
      }
      #if (CROPSYST_VERSION == 4)
      // The N dilution was changed from max_N curve to crit_N curve
      inline float32 calc_biomass_to_start_dilution_crit_N_conc_T_ha
         (Photosynthetic_pathway photosynthetic_pathway)            calculation_ //120505
      {  return biomass_to_start_dilution_max_N_conc_T_ha * CropSyst::
            biomass_to_start_dilution_max_to_crit_mult[photosynthetic_pathway];
      }
      #endif
      inline float32 calc_biomass_to_start_dilution_min_N_conc_T_ha
         (Photosynthetic_pathway photosynthetic_pathway)            calculation_
      {  return
         #if (CROPSYST_VERSION == 4)
            dilution_curve_based_on_max_conc
            ? biomass_to_start_dilution_max_N_conc_T_ha *
               CropSyst::biomass_to_start_dilution_max_to_min_mult[photosynthetic_pathway]
            :
         #endif
            biomass_to_start_dilution_crit_N_conc_T_ha *
               CropSyst::biomass_to_start_dilution_crit_to_min_mult[photosynthetic_pathway]; //121219
      }
      inline float32 calc_biomass_to_start_dilution_max_N_conc_T_ha_obsolete
         (Photosynthetic_pathway photosynthetic_pathway)            calculation_
      {  return biomass_to_start_dilution_crit_N_conc_T_ha
            * CropSyst::biomass_to_start_dilution_crit_to_max_mult[photosynthetic_pathway];
      }
   };
   //_struct Nitrogen_______________________________________________2002-04-05_/
   struct Salinity
   {
      float32 osmotic_pot_50; // osmotic potential at 50% yield reduction KPa, J/Kg.For the Saturation extract
      // 150913 rename to  50percent_reduction_osmotic_pot
      float32 salt_tolerance_P; // p_value. p exponent for the Van-Genucht"salinity tolerance exp
      // 150913 rename to P_emperical_exponent
      // osmotic potential at 50% yield reduction KPa, J/Kg.For the Saturation extract
      // p_value. p exponent for the Van-Genuchten salinity tolerance exp
   };
   //_struct Salinity_______________________________________________2002-04-05_
   struct Canopy_architecture //200403 0701116 now for both orchards and vinyards
   {
      float32 initial_width; // perpendicular to the row
      float32 final_width;
      float32 initial_breadth; // along the row                                  //071116
      float32 final_breadth;                                                     //071116
      float32 initial_height; // tree / trellis height                           //020405
      float32 final_height;                                                      //020405
      float32 branch_insertion_height;                                           //071116
   };
   //_struct Canopy_characteristics_hedge_row_______________________2002-04-05_/
   struct Layout
   {
    public:
      float32 row_azimuth_true_north_deg;
      // decimal degrees  true north basis 0 = North 90= East
      struct Spacing
      {  float32 row; // m  distance between rows
         float32 col; // m spacing of orchard   along the rows
      } spacing;
   };
   //_struct Layout_________________________________________________2002-04-05_/
   struct CO2_Response
   {
    public: // CO2 specific parameters
      // Non-rectangular Hyperbola Solution for Estimated Relative Growth
      struct CO2_Concentration_when_measured                                     //131206
      {  float32 TUE;
         float32 RUE;
      } CO2_concentration_when_measured;
      // ppm
      struct Non_rectagular_hyperbola_solution                                   //131205
      {  float32 alpha;
         float32 theta;
         float32 growth_ratio_asymptotic;                                        //131206
      } non_rectagular_hyperbola_solution;
      // Use to determine ratio of gain (based on emperical measurements)
   };
   //_struct CO2_Response___________________________________________2002-04-08_/
};
//_Crop_parameters_struct______________________________________________________/
}//_namespace CropSyst__________________________________________________________
#endif

