#ifndef crop_root_V5H
#define crop_root_V5H
#include "crop/crop_root.h"
namespace CropSyst {
//______________________________________________________________________________
class Crop_root_vital_V5
: public Crop_root_vital
{
   contribute_ float64 above_ground_biomass; // provided by caller
public:
   Crop_root_vital_V5
      (const Crop_parameters_struct::Root      &parameter_                       //061212
      ,const Phenology::Period_thermal         &root_growth_period_              //181107
      ,const ::Soil::Layers_interface          &soil_layers_
      ,float32                                  initial_root_length_);
         // initial_root_length_ is used by  calc_max_N_update_per_unit_root_length()
      // This requires a soil layering system.  If the crop model does not have soil available, it could provide a dummy soil
   virtual float64 calc_biomass
      (float64 *output_root_biomass_by_layer=0)                    calculation_; //060612
   inline virtual float64 know_above_ground_biomass
      (float64 above_ground_biomass_)                                cognition_ //130625
      { return above_ground_biomass = above_ground_biomass_; }
};
//_Crop_root_vital_V5_______________________________________________2013-06-24_/
}//_namespace CropSyst_________________________________________________________/
#endif

