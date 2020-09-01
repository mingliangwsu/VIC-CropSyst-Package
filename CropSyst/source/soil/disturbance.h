#ifndef disturbanceH
#define disturbanceH

#include "common/soil/layering.h"
#include "management/management_param_struct.h"
namespace Soil
{
class Hydrology_interface;
class Texture_interface;
//______________________________________________________________________________
class Disturbance
{
 private:
   nat8                       deepest_layer_affected;
   const Soil::Hydrology_interface   &hydrology;
   const Soil::Texture_interface     &texture;
   soil_layer_array64         (rating); // soil_disturbance_rating (from NRCS SDR table,  this is additive for each operation performed).
   soil_layer_array64         (decomposition_adjustment_factor);   // A value of one has no effect
   CropSyst::Tillage_operation_struct::Oxidation_effect oxidation_effect;               //100121
 public:
   Disturbance
      (const Soil::Hydrology_interface  &hydrology
      ,const Soil::Texture_interface    &texture);                                //131001
   bool process();
   // Returns true if soil disturbance is still significant
   // (if not significant, the soil disturbance can be deleted).
   float64  add_rating(float64 _rating, nat8 affected_layers_now); // applied to all affected layers
   inline float64 get_decomposition_adjustment_factor(uint8 layer)         const { return decomposition_adjustment_factor[layer];}
   void set_oxidation_effect(const CropSyst::Tillage_operation_struct::Oxidation_effect &_oxidation_effect);  //100121
 private:
   float64  calc_tillage_intensity(float64 clay, float64 SDR)              const;
   float64  decay(float64 available_water)                                 const;//070801
};
//_Disturbance______________________________________________________2006-08-07_/
}//_namespace_Soil_____________________________________________________________/

#endif

