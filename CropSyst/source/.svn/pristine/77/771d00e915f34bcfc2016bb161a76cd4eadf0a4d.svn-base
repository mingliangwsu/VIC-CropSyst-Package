#include "crop/crop_root_V5.h"
#include "crop/thermal_time.h"
#include "soil/soil_I.h"

namespace CropSyst {
//______________________________________________________________________________
Crop_root_vital_V5::Crop_root_vital_V5
(const Crop_parameters_struct::Root      &parameters_
,const Phenology::Period_thermal         &root_growth_period_                    //181107
,const Soil::Layers_interface &soil_layers_
,float32                                  initial_root_length_)
: Crop_root_vital
   (parameters_
   ,root_growth_period_                                                          //181108
   ,soil_layers_,initial_root_length_)
, above_ground_biomass(0)
{  biomass = 0.00001;
   // this is just to prevent a div 0 on the first day
}
//_constructor:Crop_root_vital_V5___________________________________2013-06-24_/
float64 Crop_root_vital_V5::calc_biomass
(float64 *output_root_biomass_by_layer)                             calculation_
{  // if root_biomass_by_layer array is passed, record the root biomass values by layer
   float64 root_shoot_ratio = parameters.root_shoot_full_ratio;
   if (!root_growth_period.has_expired())
   {
      float64 relative_GDDs_remaining = root_growth_period
         .get_thermal_time_relative_remaining();                                 //200204
      root_shoot_ratio =
         parameters.root_shoot_full_ratio +
         (parameters.root_shoot_emergence_ratio - parameters.root_shoot_full_ratio)
            *  pow(relative_GDDs_remaining , 3.0);
   }
   float64 calced_root_biomass = above_ground_biomass * root_shoot_ratio;
   calced_root_biomass = std::max<float64>(calced_root_biomass,biomass);
   if (output_root_biomass_by_layer)
      for (nat8 lyr = rooting_layer; (lyr <= soil_layers.count()); lyr++)
      {  float64 root_biomass_lyr = total_fract_root_length[lyr] * calced_root_biomass;
         output_root_biomass_by_layer[lyr] = root_biomass_lyr;
      }
   return calced_root_biomass;
}
//_calc_biomass_____________________________________________________2006-06-12_/
}//_namespace CropSyst_________________________________________________________/

