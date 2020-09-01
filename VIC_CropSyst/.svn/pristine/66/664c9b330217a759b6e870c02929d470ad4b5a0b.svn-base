#ifndef VIC_crop_paramH
#define VIC_crop_paramH

#ifndef primitiveH
#  include "corn/primitive.h"
#endif
#if (CROPSYST_VERSION==4)
#  include "crop/crop_param_V4.h"
#else
#  ifdef CROP_PARAMETERS_HARDCODED
#  else
#     #if (defined(_WIN32)  || defined(_WIN64))
#        include "crop/crop_param_V5.h"
#     else
#        include "crop/crop_param_V5.h"
            //CropSyst/crop/crop_param_V5.h"
#     endif
#  endif
#endif

namespace VIC_crop
{
struct Season_correction_struct
{
   nat16 earliest_start_DOY;  // DOY of end dormancy
   nat16 duration;            // days
};
typedef struct Season_correction_struct         Season_correction;

#ifdef CROP_PARAMETERS_HARDCODED
//______________________________________________________________________________
enum Harvested_part_enum
{seed_part          // Actually this is Grain which would include Seed and Achene
,root_part
,tuber_part
,leaf_part
,fruit_part
,HARVEST_CANOPY               // essentially all above (95%) ground biomass
,HARVEST_COMPLETE              // All biomass including roots
,HARVEST_PART_COUNT
,UNKNOWN_PART
};
typedef enum  Harvested_part_enum Harvested_part;
//______________________________________________________________________________
struct Crop_parameters_struct
{                                                                                //110207

struct Phenology //110207 Phenology_parameters_struct
{
   bool        perennial;                                                        //101101
   nat32       emergence_deg_day;
   nat32       begin_flowering_deg_day;
   nat32       begin_filling_deg_day;                 //100315 Claudio is using the term yield_formation for filling
   nat32       end_vegetative_growth_deg_day;         //  ThermalTimeAtTheEndOfCanopyGrowth
   nat32       begin_senescence_deg_day;              //  ThermalTimeAtTheBeginningOfCanopySenescence;
   nat32       begin_maturity_deg_day;                //  GetThermalTimeForPhysiologicalMaturity
                  // needs to be float to allow very larger numbers for long term perennials such as trees
/*check needed
   int16       tuber_initiation_deg_day;

   int16       max_root_depth_deg_day;
   bool        maturity_significant;
               //  Some perennial crops Fruit trees do need physiological maturity even thought they are perennial
               //  Some crops although grown annually such as potatos and tomatos could keep growing indefinitely
               //  So, we now have an option for the user to select is physiological maturity is significant
               //  for growth equations.
   float32     begin_rapid_fruit_growth_deg_day;       // needs to be float to allow very larger numbers for long term perennials such as trees, (was vernalization)
   float32     WUE_change_deg_day;                     // Degree days to crop change in water use efficiency for WUE_changes mode.
   float32     peak_LAI_or_rapid_fruit_growth_deg_day();
   float32     phenologic_sensitivity_water_stress;
*/
};
#define PHENOLOGY_UNDEFINED      {false,0,0,0,0,0,0}
// {emerge_deg_day,begin_flower_deg_day,begin_fill_deg_day,end_veg_deg_day,senescence_deg_day,maturity_deg_day}
//______________________________________________________________________________
struct Thermal_time // 110207 Thermal_time_parameters_struct
{
   float32     base_temperature;       // both daily and hourly mode      was BaseTemperatureForDevelopment
   float32     cutoff_temperature;     // only for daily resolution mode
   float32     opt_temperature;        // only for hourly mode
   float32     max_temperature;        // only for hourly mode
};
#define THERMAL_TIME_UNDEFINED   {0.0,0.0,0.0,0.0}
// {base,cutoff,opt,max}
//______________________________________________________________________________
struct Transpiration // 110207 Transpiration_parameters_struct
{
   float32 ET_coef;                                                             // was CropETCoeff100PercentCover
   float32 wilt_LWP;                                                             // was GetWiltingLeafWaterPotential
   float32 stomatal_closure_LWP;                                                 //(110208 was stress_onset_LWP)
   // LWP = Leaf Water Potential
   float32 full_cover_water_uptake_max;                                          // was Full_Cover_Crop_Water_Uptake_Max
};
#define TRANSPIRATION_UNDEFINED {0.0,0.0,0.0,0.0}
// {ET_coef,LWP_wilt,LWP_stress,uptake_max}
//______________________________________________________________________________
struct Canopy_growth_cover_based //110208 Canopy  //110207 Canopy_parameters_struct
{
   float32 initial_cover;                                                        //was InitialCanopyCover
   float32 maximum_cover;                                                        //was MaximumCanopyCover;
   float32 mature_green_cover;                                                   //was GreenCanopyCoverAtMaturity
   float32 mature_total_cover;                                                   //was TotalCanopyCoverAtMaturity
   float32 stop_canopy_expansion_LWP;                                            //was MinimumLeafWaterPotentialForCanopyExpansion
   float32 reduce_canopy_expansion_LWP;                                          // was CriticalLeafWaterPotentialForCanopyExpansion
   float32 fract_season_to_reach_half_max_cover_obsolete;                                 // was FractionOfSeasonToReachHalfMaximumCanopyCover
               // This corresponds to CropSyst relative_thermal_time_at_half_canopy_cover_maximum
   #if (!VB_VERSION_DATE || VB_VERSION_DATE >= 20110110)
   float32 max_canopy_height;
   #else
   #endif
};
#if (!VB_VERSION_DATE || VB_VERSION_DATE >= 20110110)
#define CANOPY_UNDEFINED     {0.0,0.0,0.0,0.0,0.0,0.0,0.0 ,0.0}
#else
#define CANOPY_UNDEFINED     {0.0,0.0,0.0,0.0,0.0,0.0,0.0}
#endif

// {initial,maximum,green_mature,total_mature,minLWP,critLWP,fract_half}
//______________________________________________________________________________
struct Biomass_production              //110208 was Biomass_parameters_struct
{  // This is stored under the section [growth]
   float32  RUE_max; // was radiation_use_efficiency;                                            //   Radiation_Use_Efficiency

            // This may be the same as RUE_max

   float32  WUE_at_1_kPa_VPD;    // Water Use Efficiency                         // WaterUseEfficiency_at_1_kPa
   float32  slope_of_water_use_efficiency_function_of_VPD;                       // SlopeOfWaterUseEfficiencyFunctionOfVPD Slope_of_Water_Use_Efficiency_Function_of_VPD

            // I think rename to WUE_scaling_coef to be consistent with CropSyst,
            // but check sign

   float32  opt_mean_temperature_for_growth;                   // OptimumMeanTemperatureForGrowth

            // I Think rename to early_growth_limit_temp to be consistent with CropSyst, but

};
#define BIOMASS_UNDEFINED        {0.0,0.0,0.0,0.0}
// {RUE,WUE,slopeWUE,opttemp,basetemp}
//______________________________________________________________________________
struct Root_parameters_struct
{
   float32  sow_depth;           //110218 was planting_depth;
   float32  depth_emergence; // meter?
   float32  max_depth; // was depth_maximum;  // meter
   float32  density_distribution_curvature;                                      // was CurvatureRootDensityFunction

      //110203 Warning density_distribution_curvature this may be either now hard coded or obsolete
      // in V.B. version

   float32  sensitivity_to_water_stress;                                         // was RootGrowthSensitivityToStress
public:
   inline float64 get_depth_emergence_m() const { return depth_emergence; };
};
#define ROOT_UNDEFINED   {0.0,0.0,0.0,0.0,0.0}
// {plantdepth,emerge_depth,max_depth,curve,sensitivity}
//______________________________________________________________________________
struct Harvest_parameters_struct
{
   Harvested_part part;
   float32        index_unstressed;
   float32        translocation_fraction_max;                                    // was MaximumTranslocationFraction
};
#define HARVEST_UNDEFINED {seed_part,0.5}
// {part,ndxunstressed,transloc}
//______________________________________________________________________________
struct CO2_Response_parameters_struct
{
   float32    growth_ratio_elevated_to_baseline_ppm;
   float32    baseline_reference_conc;
   float32    elevated_reference_conc;
};
#define CO2_RESPONSE_UNDEFINED   {0.0,0.0,0.0}
// {ratio,elevated,baseline}
//______________________________________________________________________________
};

//______________________________________________________________________________
typedef struct Crop_parameters_struct::Phenology                        Phenology_parameters ;
typedef struct Crop_parameters_struct::Canopy_growth_cover_based        Canopy_parameters;
typedef struct Crop_parameters_struct::Canopy_growth_cover_based        Canopy_growth_common_parameters;
   // In hardcoded, the all the canopy growth parameters are in one structre
typedef struct Crop_parameters_struct::Thermal_time                     Thermal_time_parameters;
typedef struct Crop_parameters_struct::Transpiration                    Transpiration_parameters;
typedef struct Crop_parameters_struct::Biomass_production               Biomass_parameters;
typedef struct Crop_parameters_struct::Root_parameters_struct           Root_parameters;
typedef struct Crop_parameters_struct::Harvest_parameters_struct        Harvest_parameters;
typedef struct Crop_parameters_struct::CO2_Response_parameters_struct   CO2_Response_parameters;
//typedef struct Crop_parameters_struct::Season_correction_struct         Season_correction;

/*obs
typedef struct Phenology_parameters_struct Phenology_parameters ;
typedef struct Canopy_parameters_struct Canopy_parameters;
typedef struct Thermal_time_parameters_struct Thermal_time_parameters;
typedef struct Transpiration_parameters_struct Transpiration_parameters;
typedef struct Harvest_parameters_struct Harvest_parameters;
*/

struct Crop_parameters_hardcoded_struct
{
   Phenology_parameters     phenology;
   Thermal_time_parameters  thermal_time;
   Transpiration_parameters transpiration;
   Canopy_parameters        canopy_growth_cover_based;                           //110208 was canopy;
   Biomass_parameters       biomass_production;
   Root_parameters          root;
   Harvest_parameters       harvest;
   CO2_Response_parameters  CO2_response;
   Season_correction_struct season_correction;
   Season_correction_struct season_correction_WA;
   Season_correction_struct season_correction_ID;
   Season_correction_struct season_correction_OR;
   Season_correction_struct season_correction_BC;
public:
   inline bool is_perennial() const { return phenology.perennial; }
};
typedef struct Crop_parameters_hardcoded_struct Crop_parameters;

#define canopy_growth_common canopy_growth_cover_based
//______________________________________________________________________________

#else
class Crop_parameters
: public CropSyst::Crop_parameters
{
 public:
   Season_correction_struct season_correction;
      // season_correction will be assigned to the values current state
      // by setup_season_correction.
   Season_correction_struct season_correction_WA;
   Season_correction_struct season_correction_ID;
   Season_correction_struct season_correction_OR;
   Season_correction_struct season_correction_BC;
public:
   virtual void expect_structure(bool for_write = false);
   bool setup_season_correction(float64 latitude_dec_deg,float64 longitude_dec_deg) modification_;
   bool adjust_phenology(float32 phenology_multiplier,float32 season_start_mutiplier);
   // moved to CropSyst::Crop_parameters inline bool is_fruit_tree() const { return harvested_part_labeled.get() == fruit_part; };
};
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
typedef struct CropSyst::Crop_parameters_struct::Phenology                 Phenology_parameters ;
typedef struct CropSyst::Crop_parameters_struct::Transpiration             Transpiration_parameters ;
typedef struct CropSyst::Crop_parameters_struct::Biomass_production        Biomass_parameters;
typedef struct CropSyst::Crop_parameters_struct::Morphology                Canopy_growth_common_parameters;
typedef struct CropSyst::Crop_parameters_struct::Canopy_growth_cover_based Canopy_parameters;
typedef struct CropSyst::Crop_parameters_struct::Thermal_time              Thermal_time_parameters;
typedef struct CropSyst::Crop_parameters_struct::Root                      Root_parameters;
typedef struct CropSyst::Crop_parameters_struct::Harvest                   Harvest_parameters;
typedef struct CropSyst::Crop_parameters_struct::CO2_Response              CO2_Response_parameters;
typedef struct CropSyst::Crop_parameters_struct::Inactive_period           Inactive_period_parameters;  //110302
typedef struct CropSyst::Crop_parameters_struct::Dormancy                  Dormancy_parameters;  //110302
#endif
//______________________________________________________________________________
} // namespace VIC_crop

#define CROP_PARAMETERS_UNDEFINED {PHENOLOGY_UNDEFINED,THERMAL_TIME_UNDEFINED,TRANSPIRATION_UNDEFINED,CANOPY_UNDEFINED,BIOMASS_UNDEFINED,ROOT_UNDEFINED,HARVEST_UNDEFINED,CO2_RESPONSE_UNDEFINED}
//{PHENOLOGY_UNDEFINED,THERMAL_TIME_UNDEFINED,CANOPY_UNDEFINED,BIOMASS_UNDEFINED,ROOT_UNDEFINED,CO2_RESPONSE_UNDEFINED}
#endif

