#ifndef residue_constH
#define residue_constH

#include "corn/enum.h"
#include "corn/primitive.h"
#define  MAX_RESIDUE_HOLD_m3_kg  0.003  /* m3/kg */
#define  MAX_RESIDUE_HOLD_m3_kg  0.003  /* m3/kg */
#define  manure_water_hold    0.003   //120523 0.004   // WARNING check this }
#define  manure_area_per_mass 5.0

#define  default_straw_residue_fraction_carbon        0.46
#define  default_manure_fraction_carbon               0.5

#define  straw_carbon_nitrogen_ratio_V4_4          50.0
#define  straw_carbon_nitrogen_ratio_V4_9          50.0

// WARNING must rename this to   default_straw_carbon_nitrogen_ratio
#define DEFAULT_STANDING_STUBBLE_CONTACT_FRACTION           0.3
#define DEFAULT_SURFACE_ORGANIC_RESIDUE_CONTACT_FRACTION    0.7
#define DEFAULT_SURFACE_MANURE_CONTACT_FRACTION             1.0
//______________________________________________________________________________
enum Residue_horizons
{NULL_HORIZON
, SURFACE_HORIZON
, SHALLOW_HORIZON
, DEEP_HORIZON/*,DEEPER_HORIZON*/
, UNREACHABLE_HORIZON                                                            //970418
,ALL_HORIZON
,ALL_SUBSURFACE_HORIZON};
//_Residue_horizons_________________________________________________1999-02-17_/
enum RUSLE2_Residue_type
{UNKNOWN_RESIDUE_TYPE
, FRAGILE_RESIDUE_TYPE
, MODERATE_RESIDUE_TYPE
, NONFRAGILE_RESIDUE_TYPE
, TOUGH_RESIDUE_TYPE
, STONE_RESIDUE_TYPE
,RUSLE2_RESIDUE_TYPE_COUNT};
extern const char *RUSLE2_residue_type_label_table[];
DECLARE_ENUM_CLAD(RUSLE2_Residue_type_clad,RUSLE2_Residue_type,RUSLE2_residue_type_label_table);
// RLN I a presuming manure is fragile (need to verify)
#define     RUSLE2_MANURE_RESIDUE_TYPE FRAGILE_RESIDUE_TYPE
//_RUSLE2_Residue_type______________________________________________2006-07-31_/
enum  Organic_matter_source
{plant_residue  // Replace this with plant_residue
 //  The rest are manure organic matter 
,poultry_layers_manure
,poultry_fresh_broilers_turkeys_manure
,poultry_aged_broilers_turkeys_manure
,swine_manure
,dairy_fresh_manure
,dairy_tank_storage_manure
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
,default_source                                                                  //100216
,organic_matter_source_COUNT
};
extern const Label_description organic_matter_source_label_table[];              //060731
//___Organic_matter_source_____________________________________________________/
class Organic_matter_source_cowl
: public Enum_abstract
{
   Organic_matter_source &value;
public:
   inline Organic_matter_source_cowl
      (Organic_matter_source &value_,Organic_matter_source default_value = (Organic_matter_source)0)
      : Enum_abstract((Label_description *)organic_matter_source_label_table)
      , value(value_)
      { value = default_value; }
   inline Organic_matter_source_cowl(Organic_matter_source &value_,const char *default_value_label)
      : Enum_abstract((Label_description *)organic_matter_source_label_table)
      , value(value_)
      { value = (Organic_matter_source)0;
        set_label(default_value_label);}
   inline virtual ~Organic_matter_source_cowl()                                    {}
   inline virtual int32 get_int32()       const {return (int32)value; }
   inline Organic_matter_source get()               const {return value; }
   inline virtual int32 set_int32(int32 value_)
      {value = (Organic_matter_source)value_; return value_;}
   inline Organic_matter_source set(Organic_matter_source value_) {value = value_; return value_;}
   inline bool is(Organic_matter_source value_)     const { return value_ == value; }
};
/*191010 DECLARE_DESCRIPTIVE_ENUM_COWL is not compiling.
but this is the only occurance of  DECLARE_DESCRIPTIVE_ENUM_COWL
so I am not using the macro
DECLARE_DESCRIPTIVE_ENUM_COWL(Organic_matter_source_cowl,Organic_matter_source,organic_matter_source_label_table)
*/
//_Organic_matter_source____________________________________________2008-09-08_/
struct Organic_matter_composition_decomposition
{//   Residue_source source
   float32 nitrogen_percent_DryWt;     // Dry wt basis
   float32 carbon_percent_DryWt;       // Dry wt basis
   bool cycling_pools;
   float32 fraction_of_biomass             [5];  // fraction that is each cycling (values in this array must add to 1.0)
   float32 days_to_50_percent_decomposition[5];
public:
};
//_Organic_matter_composition_decomposition____________________________________/
extern Organic_matter_composition_decomposition default_organic_matter_composition_decomposition[];
extern nat16 manure_decomp_time_coef         [12];
extern float32 N_in_manure_percent_dry_wt    [12];
extern float32 fract_manure_to_deep          [5][2];
extern float32 manure_incorporation_fraction [5];                                 //090224
//_externs_____________________________________________________________________/
#endif

