#ifndef OM_single_poolH
#define OM_single_poolH

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
//______________________________________________________________________________
class Organic_biomatter_pool_single
: public Organic_biomatter_pool_common                                           //070606
{
   float64 nitrogen_organic_kg;                                                  //110930
 public:
   inline virtual float64 get_nitrogen_organic()                           const //111007
      { return
            ((type == STRAW_RESIDUE) || (type == MANURE_RESIDUE))
            ? Biomass_abstract::get_nitrogen_organic()
            : nitrogen_organic_kg;
      }
   virtual float64 add(const Physical::Mass &addend)              modification_;
   virtual float64 get_carbon_nitrogen_ratio()                       provision_;
 public:  // constructors
   Organic_biomatter_pool_single         // For SOM redistribution
      (Organic_matter_type     _type
      ,nat8                    _layer
      ,float64                &contact_fraction                                  //190327_190215
      ,const Soil::Texture_interface &_soil_texture                              //070707
      // Conceptual, take a single soil texture layer
      ,const Soil::Abiotic_environment_layer & _soil_abiotic_env_layer);
   Organic_biomatter_pool_single(const Organic_biomatter_pool_single &from_copy);//060727
      // Copy constructor is used by Residues_V4_4::respond_to_field_operation
   Organic_biomatter_pool_single         // For stable organic matter and microbial
      (Organic_matter_type     type
      ,nat8                    layer
      ,float64                 biomass
      ,float64                 carbon_fraction
      ,float64                 carbon_nitrogen_ratio
      ,float64                 decomposition_constant
      ,float64                 area_per_mass       // = 0.0  stubble and surface residue_only?
      ,float64                &contact_fraction                                  //190327
      ,const Soil::Texture_interface &soil_texture                               //070707
      ,const Soil::Abiotic_environment_layer & soil_abiotic_env_layer);
   Organic_biomatter_pool_single        // for residue
      (Organic_matter_type     type
      ,Organic_matter_position position
      ,uint8                   layer
      ,Organic_matter_cycling  cycling
      ,RUSLE2_Residue_type     RUSLE2_residue_type                              //060731
      ,float64                 biomass
      ,float64                 carbon_fraction
      ,float64                 carbon_nitrogen_ratio
      ,float64                 decomposition_constant                           //090703
      ,float64                 detrition_constant                               //090413
      ,float64                 area_per_mass      // = 0.0  stubble and surface residue_only?
      ,float64                &contact_fraction                                  //190327
      ,const Soil::Texture_interface &soil_texture                               //070707
      ,const Soil::Abiotic_environment_layer & soil_abiotic_env_layer);
   virtual ~Organic_biomatter_pool_single();
   inline virtual Organic_biomatter_pool_single *clone()                   const //090702
      { return new Organic_biomatter_pool_single(*this); }
 public: // accessors
   void set_standing_stubble();                                                  //060530
   // set_standing_stubble is used by decomposing canopy                         //060530
 public: // decomposition
   virtual Organic_biomatter_pool_common::Decomposition
      *provide_decomposition_residue                                             //111003
      (float64 moisture_function
      ,float64 temperature_function)                                 provision_;
   virtual Organic_biomatter_pool_common::Decomposition *provide_decomposition_SOM //111003
      (float64 tillage_decomposition_rate_adj
      ,float64 minimized_temperature_moisture_function)              provision_;
   virtual float64 transfer_carbon_and_nitrogen();
   virtual float64 post_carbon_transfer()          modification_ { return 0.0; } //190219_111003
   /* This was wrong because we still need to receive
      // post in not applicable to SOM because there are no transfer amount
      // other pools (only simply from residue).
   */
 protected: // decomposition classes
   //_______________________________________________________________
   class Carbon_decomposition_common
   : public Organic_biomatter_pool_common::Decomposition                         //111003
   {
   public:
      Carbon_decomposition_common
        (const Organic_biomatter_pool_common &_pool                              //110930
         ,float64 tillage_decomposition_rate_adj
         ,float64 minimized_temperature_moisture_function
         ,bool    is_a_residue);
      virtual ~Carbon_decomposition_common() {}                                  //141104
      virtual float64 N_mineralization_or_immobilization_demand
         (float64 carbon_nitrogen_ratio);
      // Returns the N immobilization demand
   };
   //_______________________________________________________________
   #ifdef KEMANIAN_HUMIFICATION
   class Carbon_decomposition_residue
   : public Carbon_decomposition_common
   {
      //_______________________________________________________________
      public:
      class Humification
      : public Biomass_abstract
      {
      public:
         float64 factor; // C_fraction_transferred_to[single_SOM]
         Physical::Constituent_of_mass carbon;
         float64 released_CO2_C_informational;
            // this should match what is actually transferred
       public:
         inline Humification(const Humification &from_humification_copy)
         : Biomass_abstract(from_humification_copy)
         , carbon(from_humification_copy.get_carbon_kg(),UC_kg_mass,*this)
         , factor(from_humification_copy.factor)
         , released_CO2_C_informational
            (from_humification_copy.get_carbon_kg()
             * (1.0 - from_humification_copy.factor))
         {}
         //_______________________________________________________________
         inline Humification
            (const Organic_biomatter_pool_common &from_biomass_copy
            ,float64 _humification_factor)
         : Biomass_abstract                                  (from_biomass_copy)
         , carbon                       (from_biomass_copy.get_carbon_kg()
                                         *_humification_factor,UC_kg_mass,*this)
         , factor                                         (_humification_factor)
         , released_CO2_C_informational      (from_biomass_copy.get_carbon_kg()
                                                * (1.0 - _humification_factor) )
         {  set_amount(carbon.kg() / carbon_fraction);                           //151119_150123
         }
         //_______________________________________________________________
         inline virtual float64 multiply_by(float64 multiplier)   modification_
         {  carbon.multiply_by(multiplier);
            released_CO2_C_informational *= multiplier;
            return Biomass_abstract::multiply_by(multiplier);
         }
         //_______________________________________________________________
         inline virtual float64 get_carbon_kg()    const { return carbon.kg(); }
      };
      //_Humification_______________________________________________2011-10-01_/
      float64  temperature_function;
      float64  moisture_function;
   public:
      Humification *humification;
   public:
      Carbon_decomposition_residue
         (const Organic_biomatter_pool_single &pool                              //110930
         ,float64 temperature_function
         ,float64 moisture_function
         ,bool    organic_residue);
      virtual float64 potential();
      virtual ~Carbon_decomposition_residue();                                   //111004
      #ifdef KEMANIAN_HUMIFICATION
      inline virtual float64 get_humified_carbon_nitrogen_ratio()          const
       { return humification? humification->get_carbon_nitrogen_ratio() : 1.0; }
      #endif
      virtual float64 actual(float64 N_deficit_for_immobilization                //111001
                      ,float64 decomposition_reduction_factor);
   };
   //_______________________________________________________________
   #else
   class Carbon_decomposition_residue
   : public Carbon_decomposition_common
   {
      float64  temperature_function;
      float64  moisture_function;
    public:
      Carbon_decomposition_residue
         (const Organic_biomatter_pool_single &pool                              //110930
         ,float64 temperature_function
         ,float64 moisture_function
         ,bool    is_a_residue);
      virtual ~Carbon_decomposition_residue();                                   //111004
      virtual float64 potential();
   };
   //_______________________________________________________________2019-02-14_/
   #endif

 protected:
   provided_ Carbon_decomposition_common *decomposition;  // owned by this //111003
   virtual void receive_carbon_pending(float64 added_carbon_mass) {}
      // Not applicable to single pool
 public:
  virtual void clear_decomposition();
   #ifdef KEMANIAN_HUMIFICATION
   inline virtual float64 get_humified_carbon_nitrogen_ratio(nat8 receiver_index)
      const { return decomposition->get_humified_carbon_nitrogen_ratio(); }
   #endif
   inline virtual Decomposition *mod_decomposition()    { return decomposition;} //111003
   inline virtual const Decomposition *ref_decomposition()                 const
      { return decomposition; }
   virtual std::istream &read(std::istream &streamstream)            stream_IO_; //170228
};
//______________________________________________________________________________
#define Organic_matter_pools_profile_V4_4  Organic_matter_pools_multiple
#endif
//170303 #endif

