#ifndef chemical_mass_profileH
#define chemical_mass_profileH
#include "corn/const.h"
#include "common/soil/layering.h"
#include "corn/dynamic_array/dynamic_array_T.h"
// The units of the mass is defined by the instance Chemical mass profile
// For CropSyst chemical pools, it is kg chem / m^2 soil molecular
// but these classes should be equally usable in kg
// and molecular or elemental forms.

namespace Soil
{  interface_ Layers_interface;
}
//______________________________________________________________________________
interface_ Chemical_mass_profile_interface
{
   virtual float64 get_layer(nat8  layer)                               const=0;
   virtual float64 set_layer(nat8  layer,float64 new_mass)      modification_=0;
   virtual float64 dec_layer(nat8  layer,float64 dec_mass)      modification_=0;
      //< \return the amount actually decremented (if limited)
   virtual float64 inc_layer(nat8  layer,float64 inc_mass)      modification_=0;
   virtual float64 sum_to_depth(float64 depth)                          const=0;
   virtual float64 sum_profile()                                        const=0;
   virtual bool initialize(soil_layer_array64(init_mass))initialization_     =0;
   virtual float64 get_layers(soil_layer_array64(mass_layers))            const; //080117
   virtual float64 set_layers(soil_layer_array64(mass_layers))    modification_; //080117
   virtual bool commit_diffs()                                  modification_=0; //120730
   inline virtual ~Chemical_mass_profile_interface() {}                          //170217
};
//_Chemical_mass_profile_interface__________________________________2008-01-16_/
class Chemical_mass_profile_layer_based
: public Chemical_mass_profile_interface
{
 private:
   const Soil::Layers_interface &layers;                                          //060503
   soil_layer_array64(mass);  // layer based
 public:
   Chemical_mass_profile_layer_based(const Soil::Layers_interface &_layers);
   bool initialize(soil_layer_array64(initial_mass_by_layer))     modification_;
   inline virtual float64 get_layer(nat8 layer)    const { return mass[layer]; }
          virtual float64 dec_layer(nat8 layer,float64 dec_mass)  modification_;
   inline virtual float64 inc_layer(nat8 layer,float64 inc_mass)  modification_
      { mass[layer] += inc_mass; return mass[layer]; }
   inline virtual float64 set_layer(nat8 layer,float64 new_mass)  modification_
      { mass[layer] = new_mass; return mass[layer]; }
   virtual float64 sum_to_depth(float64 depth)                            const;
   virtual float64 sum_profile()                                          const;
   inline virtual bool commit_diffs()             modification_ { return true; } //120730
      // commit_diffs not needed in this model
};
//_Chemical_mass_profile_layer______________________________________2008-01-16_/
class Chemical_mass_profile_node_based
: public Chemical_mass_profile_interface
{
   const Soil::Layers_interface &layers;                                         //060503
   nat8  number_real_plus_extended_nodes;                                        //080219
   soil_layer_array64(mass_upper); // Upper part of the respective layer         //080116
   soil_layer_array64(mass_lower); // lower part of the respective layer         //080116
   contribute_ CORN::Dynamic_array<float64> *layer_diffs;                        //120730
 public:  // layer accessors
   Chemical_mass_profile_node_based(const Soil::Layers_interface &_layers);
   virtual bool initialize(soil_layer_array64(initial_mass))modification_;
      // the initial initial_mass_by_layer will be split into nodes
   virtual float64 get_layer(nat8  layer)                                 const;
   virtual float64 dec_layer(nat8  layer,float64 decrement_mass)  modification_;
   virtual float64 inc_layer(nat8  layer,float64 increment_mass)  modification_;
   virtual float64 set_layer(nat8  layer,float64 new_mass)        modification_;
   virtual float64 sum_to_depth(float64 depth)                            const;
   virtual float64 sum_profile()                                          const;
   virtual bool commit_diffs()                                    modification_; //120730
 public:  // node accessors
   virtual float64 get_node(nat8 node,nat8 real_plus_extended_nodes)      const;
   virtual float64 set_nodes(soil_node_array64(new_mass_by_node)
      ,float64 number_real_plus_extended_nodes
      ,float64 molecular_to_elemental)                            modification_; //080208
};
//_Chemical_mass_profile_node_based_________________________________2008-01-16_/
#endif

