#ifndef soil_properties_sublayersH
#define soil_properties_sublayersH

#include "soil/hydraulic_properties.h"
#include "soil/profile_texture.h"
#include "soil/sublayers.h"
#ifdef STATSGO_DATABASE
#include "soil/soil_param_with_STATSGO.h"
#else
// LADSS doesn't need STATSGO
#include "soil/soil_param.h"
#endif
//______________________________________________________________________________
class Soil_sublayers;
class Smart_soil_parameters;
class Soil_with_sublayering;
namespace Soil {                                                                 //181206
//______________________________________________________________________________
class Properties_sublayers
: public implements_ Properties_sublayers_interface
{protected: // friend class Soil_with_sublayering
   Soil_parameters *parameters;
   bool parameters_owned;                                                        //101104
   Profile_texture        texture;                                               //060724
   Soil_sublayers        *layers; // Must come before hydraulic_properties       //061003
   Hydraulic_properties  *hydraulic_properties;
      // The current soil hydraulic properties
      // (these may be redone when there is erosion, or dust mulching ).  //051204_
 public:
   Langmuir               NH4_Langmuir;                                          //190404
 public:
   Properties_sublayers
      (Soil_parameters *parameters
      ,bool parameters_owned);
   inline virtual float64 get_rooting_depth()                              const { return layers->get_depth_m(1);}
   bool reinitialize
      (bool beginning_of_simulation
      ,bool subdivide_horizons
      ,bool finite_diff);
   virtual void subdivide_horizon
      (nat8  horizon
      ,nat8  sublayer
      ,float64 sublayer_thickness
      ,float64 sublayer_depth);
   inline virtual unmodifiable_ Layers_interface *ref_layers()             const { return layers;} //160412
   inline virtual   modifiable_ Layers_interface *mod_layers()     modification_ { return layers;} //160412
      // Returns the soil layering system.
      // This is required in derived classes.
      // It should never return 0.
   inline virtual unmodifiable_ Texture_interface *ref_texture()           const { return &texture;} //160412
   inline virtual   modifiable_ Texture_interface *mod_texture()   modification_ { return &texture;} //160412
      // Returns the soil texture parameters object.
   inline virtual unmodifiable_ Hydraulic_properties_interface *ref_hydraulic_properties() const { return hydraulic_properties;} //160412
      // Returns the soil properties that are commonly derived from soil survey data
      // and/or pedo transfer functions.
   inline virtual   modifiable_ Hydraulic_properties_interface *mod_hydraulic_properties() modification_  { return hydraulic_properties;} //160412
      // Returns the soil properties that are commonly derived from soil survey data
      // and/or pedo transfer functions.
};
//_class Properties_sublayers_______________________________________2010-11-04_/
} // namespace Soil                                                              //181206
#endif
