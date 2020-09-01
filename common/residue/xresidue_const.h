#ifndef residue_constH
#define residue_constH
#include "corn/primitive.h"

#define  MAX_RESIDUE_HOLD_m3_kg  0.003  /* m3/kg */
#define  MAX_RESIDUE_HOLD_m3_kg  0.003  /* m3/kg */
#define  manure_water_hold    0.004   // WARNING check this }
#define  manure_area_per_mass 5.0

//061101 renamed default_straw_residue_fraction_carbon #define  straw_fraction_carbon_residue        0.46
#define  default_straw_residue_fraction_carbon        0.46
#define  default_manure_fraction_carbon               0.5

#define  straw_carbon_nitrogen_ratio_V4_4          50.0
#define  straw_carbon_nitrogen_ratio_V4_9          50.0
// WARNING must rename this to   default_straw_carbon_nitrogen_ratio

#define DEFAULT_STANDING_STUBBLE_CONTACT_FRACTION  0.3
#define DEFAULT_SURFACE_ORGANIC_RESIDUE_CONTACT_FRACTION  0.7
#define DEFAULT_SURFACE_MANURE_CONTACT_FRACTION  1.0
//____________________________________________________________________________/
enum Residue_horizons
   {NULL_HORIZON
   , SURFACE_HORIZON
   , SHALLOW_HORIZON
   , DEEP_HORIZON/*,DEEPER_HORIZON*/
   , UNREACHABLE_HORIZON                                                        //970418
   ,ALL_HORIZON
   ,ALL_SUBSURFACE_HORIZON};
//_Residue_horizons________________________________________________1999-02-17_/
enum RUSLE2_Residue_type
   {UNKNOWN_RESIDUE_TYPE
   , FRAGILE_RESIDUE_TYPE
   , MODERATE_RESIDUE_TYPE
   , NONFRAGILE_RESIDUE_TYPE
   , TOUGH_RESIDUE_TYPE
   , STONE_RESIDUE_TYPE
   ,RUSLE2_RESIDUE_TYPE_COUNT};
extern const char *RUSLE2_residue_type_label_table[];
DECLARE_LABELED_ENUM(RUSLE2_Residue_type_labeled,RUSLE2_Residue_type,RUSLE2_residue_type_label_table);
// RLN I a presuming manure is fragile (need to verify)
#define     RUSLE2_MANURE_RESIDUE_TYPE FRAGILE_RESIDUE_TYPE
//_RUSLE2_Residue_type_____________________________________________2006-07-31_/
enum  Organic_matter_source
{  plant_residue  // Replace this with plant_residue
     //  { The rest are manure organic matter }
   ,poultry_layers_manure
   ,poultry_fresh_broilers_turkeys_manure
   ,poultry_aged_broilers_turkeys_manure
   ,swine_manure
   ,daily_fresh_manure
   ,daily_tank_storage_manure
   ,dairy_anaerobic_lagoon_storage_manure
   ,beef_fresh_manure
   ,beef_aged_2_5_manure
   ,beef_aged_1_5_manure
   ,beef_aged_1_0_manure
    // Added 080908
   ,municiple_waste_compost
   ,sewage_sludges
   ,corn_stover
   ,sorghum_stover
   ,wheat_straw
   ,oat_straw
   ,soybean_residue
   ,organic_matter_source_COUNT
};
//_Organic_matter_source___________________________________________2008-09-08_/
extern const Label_description organic_matter_source_label_table[];
DECLARE_DESCRIPTIVE_LABELED_ENUM(organic_matter_source_label_table_labeled,Organic_matter_source,organic_matter_source_label_table);
//____________________________________________________________________________/
struct Organic_matter_composition_decomposition
{
//   Residue_source source
   float32 nitrogen_percent_DryWt;     // Dry wt basis
   float32 carbon_percent_DryWt;       // Dry wt basis
//,  float32 C_N_ratio       can be calculated
   bool cycling_pools;
  float32 fraction_of_biomass             [5/*090821 OM_CYCLING_COUNT*/];  // fraction that is each cycling (values in this array must add to 1.0)
  int16   days_to_50_percent_decomposition[5/*090821 OM_CYCLING_COUNT*/];
};
//_Organic_matter_composition_decomposition___________________________________/
extern Organic_matter_composition_decomposition default_organic_matter_composition_decomposition[];
extern short int manure_decomp_time_coef[12];
extern float32 Xfract_manure_to_deep[5][2];
extern float32 manure_incorporation_fraction[5];                                 //090224
extern float32 N_in_manure_percent_dry_wt[12];

#endif

