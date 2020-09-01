#ifndef canopy_growth_LAI_based_V4H
#define canopy_growth_LAI_based_V4H
#include "crop/canopy_growth_LAI_based.h"

namespace CropSyst {

//______________________________________________________________________________
class Canopy_growth_leaf_area_index_based_V4
: public extends_ Canopy_growth_leaf_area_index_based
{  //190612 This should be renamed Canopy_leaf_growth_leaf_area_index_based_V4
 public: // structors
   Canopy_growth_leaf_area_index_based_V4
      (const Crop_parameters                                 &crop_parameters_       // Will be owned by this crop object
      ,const Crop_parameters_struct::Canopy_growth_LAI_based &parameters_
      //190630 ,const Thermal_time_immutable          &_thermal_time                      //151106
      ,Residues_interface        *residues_                                      //060816
      ,Crop_nitrogen_interface   *nitrogen_                                      //060816
      ,float64                   &reported_peak_LAI_                             //080725
      ,Crop_CO2_response         *CO2_response_                                  //080728
      //190630 ,bool                      is_continuous_grass_
      );
   inline virtual float64 know_above_ground_biomass(float64 _above_ground_biomass_kg_m2) cognition_
      { return _above_ground_biomass_kg_m2; /*not used by this model */}         //130625
};
//_class_Canopy_growth_leaf_area_index_based_V4________________________________/
}//_namespace CropSyst_________________________________________________________/
#endif



