#ifndef hydrology_cascadeH
#define hydrology_cascadeH
#include "soil/hydrology_A.h"
namespace Soil {                                                                 //181206
//______________________________________________________________________________
class Hydrology_cascade
: public Hydrology_abstract
{
   soil_layer_array64(liquid_water_content);   // m3/m3 volumetric               //070824
 public: // constructor
   Hydrology_cascade
      (const Layers_interface                &soil_layers_ref_
      ,const Hydraulic_properties_interface  &hydraulic_properties_ref_
      ,const Structure            &soil_structure_);
   Hydrology_cascade                                                             //140610
      (const Hydrology_cascade &copy_from
      ,float32 fraction = 1.0);
   inline virtual Hydrology_cascade *copy(float32 fraction = 1.0)         const  //140610
      { return new Hydrology_cascade (*this,fraction); }                         //140610
 public: // processes
   virtual float64 extract_water(float64 amounts_m[],nat8 start_layer);
 public:  // Calculators
   virtual float64 calc_water_depth_profile
      (bool liquid_and_ice)                                               const;
   virtual float64 get_liquid_water_content_volumetric
      (nat8 layer)                                                        const;
   virtual float64 set_water_content_volumetric_layer
      (nat8 layer, float64 new_water_content)                     modification_;
   virtual float64 change_liquid_water_content_by_volumetric                     //070813
      (nat8 layer, float64 change_WC)                             modification_;
   RENDER_INSPECTORS_DECLARATION;                                                //150916
};
//_class_Hydrology_cascade_____________________________________2006-05-05_/
} // namespace Soil
#endif
