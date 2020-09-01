#error This class is obsolete, this file can be removed from your project/Makefile 160414
#ifndef unstressed_growthH
#define unstressed_growthH

#ifndef primitiveH
#  include "corn/primitive.h"
#endif
#include "corn/math/compare.hpp"

#include "options.h"
// These class is for growing a crop as if it were completely unstressed
// these are used to compute N_concentrations
// Also used by the new Crop parameter estimator utility.
#ifdef YAML_PARAM_VERSION
#include "crop/transpiration_dependent_growth_V5.h"
#else
#include "crop/transpiration_dependent_growth_V4.h"
#endif
namespace CropSyst {                                                             //131106
//______________________________________________________________________________
class Unstressed_growth
{
   /*
   Note the O prefix in the O_xxx member variables
   Refers to previous variable names optimal_biomass
   Claudio didn't want to use this term.
   I have retained the prefix O_ to avoid
   any ambiguity with the respective actual crop variables.
   */
 protected:
   float64 O_biomass;// this is the biomass of the crop if it were never stressed (except temperature effect) used for N biomass concentration curves.//011016
   float64 O_GAI;    // Potential GAI for determination of critical N content    //011016
 public:
   //151029 Transpiration_dependent_growth_common
   #ifdef YAML_PARAM_VERSION
   Biomass_production_transpiration_dependent_V5
   #else
   Biomass_production_transpiration_dependent_V4
   #endif
      transpiration_dependent_growth_model;                                      //130627
 public:
   float64 O_attainable_top_growth;  // this is biomass produced today if the crop were never stressed. //011016
   float64 O_light_depend_growth;  // For determination of Pot. biomass          //011016
 public: // these two need to be public for now                                  //011016
   float64 O_cover_green_fract;                                                  //011016
   float64 O_pot_transpiration_CO2_adjusted;                                     //151031_011016
      // Pot transpiration for determination of critical N content
   const float64 &CO2_transpiration_adjustment_factor;                           //151031
 public:
   Unstressed_growth                                                             //130627
      (
      #ifdef YAML_PARAM_VERSION
      Crop_parameters_struct::Biomass_production   &parameters_                  //150907
      #else
      const Crop_parameters &parameters_
      #endif
      ,const Vapor_pressure_deficit_daytime &daytime_vapor_pressure_deficit_
      ,const float64 &pot_ref_evapotranspiration_
      ,const float64 &CO2_TUE_adjustment_
      ,const float64 &CO2_transpiration_adjustment_factor_);
   void initialize()                                            initialization_;
   void restart
      (float64 init_biomass
      ,float64 init_GAI
      ,float64 init_cover_green_fract);
   inline void remove_biomass(float64 act_removed_biomass)
      { O_biomass -= act_removed_biomass; O_biomass
         = CORN::must_be_0_or_greater<float64>(O_biomass);}                      //040622
      //040622 I don't think this is always set to 0 { O_biomass = 0; }
   void adjust_for_removal(float64 new_biomass,float64 new_GAI);                 //041206
   void accumulate_GAI_and_biomass
      (Normal_crop_event_sequence growth_stage
      ,float64        param_specific_leaf_area
      ,float64        param_stem_leaf_partition);
   void daily_setup    // probably should return bool 140922
      (Normal_crop_event_sequence growth_stage
      ,float64 unstressed_fract_canopy_cover // total radiation based with unstressed GAI
      ,float64 act_GAI);
   float64 update_evapotranspiration_max
      (float64 ET_ref_m // This is ET with crop coefficient applied
      ,float64 act_GAI)                                          rectification_; //200127
   void update_attainable_top_growth
      (float64 est_solar_rad     // Estimated solar radiation.
      ,float64 biomass_water_ratio
      #if ((CROPSYST_VERSION > 0) && (CROPSYST_VERSION < 5))
      // This mode is for compatibility with previous versions
      // These parameters are deprecated and will be removed in version 5
      ,bool    RUE_PAR_based                                                     //080801
      ,float64 RUE_PAR_kg_MJ
      #endif
      ,float64 RUE_global_kg_MJ                                                  //080730
      ,float64 unstressed_fract_canopy_cover_PAR // photosynthetically active radiation based!!!
      ,float64 unstressed_GAI_fract_canopy_cover                                 //080730
      ,float64 temperature_limitation);
   inline float64   get_GAI()                            const { return O_GAI; }
   inline float64   get_pot_transpiration_CO2_adjusted()                   const
                                    { return O_pot_transpiration_CO2_adjusted; } //151031
   inline float64   get_biomass()                                          const
   {  return
               //  This is a hack, can't figure out
               //   why O_biomass is being initialized to
               //   0.000 instead of 0.00001}

//020624 Note I think this problem has been fixed and the conditional (hack)
//       is obsolete so we can just return O_biomass.
         (O_biomass == 0.0) ?  0.00001:
            O_biomass;
   }
};
//______________________________________________________________________________
}//_namespace CropSyst_________________________________________________________/                                                           //131106
#endif
//unstressed_growth.h

