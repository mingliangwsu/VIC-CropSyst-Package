#include "crop/crop_root_V4.h"
#include "soil/soil_I.h"
namespace CropSyst {
//______________________________________________________________________________
Crop_root_vital_V4::Crop_root_vital_V4
(const Crop_parameters_struct::Root       &parameters_
,const Phenology::Period_thermal          &root_growth_period_                   //191121
,const Soil::Layers_interface             &soil_layers_                          //181206
,float32                                   initial_root_length_
   // This is used by  calc_max_N_update_per_unit_root_length()
)
: Crop_root_vital
   (parameters_
   , root_growth_period_                                                         //191121
   ,soil_layers_,initial_root_length_)
{}
//_Crop_root_vital_V4::constructor__________________________________2013-06-24_/
float64 Crop_root_vital_V4::calc_root_biomass(float64 *output_root_biomass_by_layer) const
{  // if root_biomass_by_layer array is passed, record the root biomass values by layer
   float64 calced_root_biomass = 0.0;
   uint8 number_soil_layers = soil_layers.count();
   for (uint8 lyr = rooting_layer; (lyr < number_soil_layers); lyr++)
   {  float64 root_lengths_lyr = root_lengths[lyr];                              //061129
      float64 root_length_per_unit_mass_m_kg
         = km_to_m(parameters.length_per_unit_mass_km_kg_V4);
      float64 root_biomass_lyr=root_lengths_lyr/root_length_per_unit_mass_m_kg;
      if (output_root_biomass_by_layer)
         output_root_biomass_by_layer[lyr] = root_biomass_lyr;
      calced_root_biomass += root_biomass_lyr;
   }
   return calced_root_biomass;
}
//_calc_root_biomass________________________________________________2006-06-12_/
}//_namespace CropSyst_________________________________________________________/

