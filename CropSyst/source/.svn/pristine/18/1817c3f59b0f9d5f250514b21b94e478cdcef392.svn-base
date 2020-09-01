#ifndef OM_multiple_poolH
#define OM_multiple_poolH
#include "corn/container/unilist.h"
#include "organic_matter/OM_common.h"
#include "common/soil/layering.h"
//______________________________________________________________________________
namespace Soil
{
   class Layers_interface;
   class Texture_interface;
   class Hydraulic_properties_interface;
   class Nitrogen;
   class Abiotic_environment_layer;
   class Abiotic_environment_profile;
   class Disturbance;
}
class Organic_matter_pools_multiple;
//_forward declarations________________________________________________________/
class Organic_biomatter_pool_multiple
: public Organic_biomatter_pool_common                                           //070606
{
 public:  // constructors
   Organic_biomatter_pool_multiple         // For SOM redistribution
      (
      Organic_matter_type      type
      ,nat8                    layer
      ,float64                &contact_fraction                                  //190327_190215
      ,const Soil::Texture_interface   &soil_texture                             //070707
      ,const Soil::Abiotic_environment_layer &soil_abiotic_environment_layer
      );
   Organic_biomatter_pool_multiple(const Organic_biomatter_pool_multiple &from_copy); //060727
      // Copy constructor is used by Residues_V4_4::respond_to_field_operation
   Organic_biomatter_pool_multiple         // For stable organic matter and microbial
      (Organic_matter_type     type
      ,nat8                    layer
      ,float64                 biomass
      ,float64                 carbon_fraction
      ,float64                 carbon_nitrogen_ratio
      ,float64                 decomposition_constant
      ,float64                 area_per_mass       // = 0.0  stubble and surface residue_only?
      ,float64                &contact_fraction                                  //190327_190215
      ,const Soil::Texture_interface   &soil_texture                             //070707
      ,const Soil::Abiotic_environment_layer &soil_abiotic_environment_layer);
   Organic_biomatter_pool_multiple        // for residue
      (
      Organic_matter_type     type
      ,Organic_matter_position position
      ,nat8                    layer
      ,Organic_matter_cycling  cycling
      ,RUSLE2_Residue_type     RUSLE2_residue_type                               //060731
      ,float64                 biomass
      ,float64                 carbon_fraction
      ,float64                 carbon_nitrogen_ratio
      ,float64                 decomposition_constant                            //090703
      ,float64                 detrition_constant                                //090413
      ,float64                 area_per_mass      // = 0.0  stubble and surface residue_only?
      ,float64                &contact_fraction                                  //190327_190215
      ,const Soil::Texture_interface      &soil_texture                          //070707
      ,const Soil::Abiotic_environment_layer   &soil_abiotic_environment_layer
      );
      virtual ~Organic_biomatter_pool_multiple();
      inline virtual Organic_biomatter_pool_multiple *clone()              const //090702
         { return new Organic_biomatter_pool_multiple(*this); }
 public: // decomposition
    virtual Organic_biomatter_pool_common::Decomposition
      *provide_decomposition_residue                                             //111003
      (float64 moisture_function
      ,float64 temperature_function)                                 provision_; //111003
   virtual Organic_biomatter_pool_common::Decomposition
      *provide_decomposition_SOM                                                 //111003
      (float64 tillage_decomposition_rate_adj
      ,float64 minimized_temperature_moisture_function)              provision_; //111003
   virtual float64 transfer_carbon_and_nitrogen();
   virtual float64 post_carbon_transfer()                         modification_; //190219
      // Returns the (actual) net carbon transfer.
      // (the return value is informational, not currently used anywhere
 protected: // decomposition classes
   //_______________________________________________________________
   class Carbon_decomposition_common
   : public Organic_biomatter_pool_common::Decomposition                         //111003
   {
   public:
      Carbon_decomposition_common
         (const Organic_biomatter_pool_multiple &_pool                           //110930
         ,float64 _tillage_decomposition_rate_adj
         ,float64 _minimized_temperature_moisture_function
         ,bool    _is_a_residue);
     virtual float64 actual(float64 N_deficit_for_immobilization
                  ,float64 decomposition_reduction_factor);
      virtual float64 N_mineralization_or_immobilization_demand
         (float64 carbon_nitrogen_ratio);                                        //110930
      // Returns the N immobilization demand
   };
   //_Carbon_decomposition_common______________________________________________/
   class Carbon_decomposition_residue
   : public Carbon_decomposition_common
   {  float64  temperature_function;
      float64  moisture_function;
   public:
      Carbon_decomposition_residue
         (const Organic_biomatter_pool_multiple &pool                            //110930
         ,float64 temperature_function
         ,float64 moisture_function
         ,bool    is_a_residue);
      virtual float64 potential();
   };
   //_Carbon_decomposition_residue_____________________________________________/
 protected:
   provided_ Carbon_decomposition_common *decomposition;  // owned by this       //111003
 public:
   void deduct_carbon_pending(float64 removed_carbon_mass);
   virtual void receive_carbon_pending(float64 added_carbon_mass);
 protected: // Should provide accessors to these
   friend class Residues_organic_matter;
   float64 carbon_to_deduct;
   float64 carbon_to_receive;
 public:
   virtual void clear_decomposition();
 public: //  Organic_biomatter_common
   #ifdef KEMANIAN_HUMIFICATION
   inline virtual float64 get_humified_carbon_nitrogen_ratio
      (nat8 receiver_index)                                                const //110930
      { return receiver[receiver_index]->get_carbon_nitrogen_ratio(); }
   #endif
   inline virtual Decomposition *mod_decomposition()                             //111003
      { return decomposition; }
   inline virtual const Decomposition *ref_decomposition()                 const //111003
      { return decomposition; }
};
//_Organic_biomatter_pool_multiple_____________________________________________/
#define Organic_matter_pools_profile_V4_4  Organic_matter_pools_multiple
#endif


