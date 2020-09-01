#ifndef crop_root_V4H
#define crop_root_V4H
#include "crop/crop_root.h"

namespace CropSyst {
//______________________________________________________________________________
class Crop_root_vital_V4
: public Crop_root_vital                                                         //070131
{
 public:
   Crop_root_vital_V4
      (const Crop_parameters_struct::Root       &parameter_                      //061212
      ,const Phenology::Period_thermal          &root_growth_period_             //191121
      ,const Soil::Layers_interface             &soil_layers_                    //181206
      ,float32  initial_root_length_ );
      // This is used by  calc_max_N_update_per_unit_root_length()
      // This requires a soil layering system.  If the crop model does not have soil available, it could provide a dummy soil
   virtual float64 calc_root_biomass
      (float64 *output_root_biomass_by_layer=0)                           const; //060612
};
//_Crop_root_vital_V4_______________________________________________2013-06-24_/
}//_namespace CropSyst_________________________________________________________/
#endif

