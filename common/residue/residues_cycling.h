#ifndef residues_cyclingH
#define residues_cyclingH

#include <assert.h>                                                              //LML 140909
#include "common/residue/residues_interface.h"
#include "common/residue/residue_V4_common.h"
class Organic_biomatter_V4_4;
class Residue_decomposition_parameters;
class Biomass_decomposition_parameters;
namespace Soil {class Structure; }                                               //181206_110930
class Organic_biomatter_pool_common;                                             //110930
//______________________________________________________________________________
class Residues_cycling
: public Residues_V4_common                                                      //070707
{private: // references to object
   Organic_matter_pools_common &OM_pools; // owned by ????
   const Soil::Layers_interface  &soil_layers;                                   //060122
   const Soil::Structure         &soil_structure;                                //110930
 public:
   Residues_cycling                                                              //020307
      (Organic_matter_pools_common &pools
      ,Soil::Soil_interface                     &soil
      ,Soil::Abiotic_environment_profile   &soil_abiotic_environment_profile
      ,modifiable_ CORN::Dynamic_array<float64> &evaporation_pot_remaining       //160717
      ,const Air_temperature_average      &temperature_air_avg                   //150427
      ,const Wind_speed                   &wind_speed);                          //150427

   virtual void add_surface_plant_pool
      (Organic_matter_position position                                          //040517
      ,Organic_matter_cycling _om_cycling_unused_this_model                      //060219
      ,float64 new_surface
      #ifdef NITROGEN
      ,float64 new_N_surface_conc
      #endif
         // N conc is actually only used when nitrogen is enabled
         // but now N and C/N is a basic element of biomatter class
      ,const Residue_decomposition_parameters &residue_decomposition_params);    //060219
   virtual void add_subsurface_plant_pool
      (soil_sublayer_array_64(new_subsurface)                                    //970421
       #ifdef NITROGEN
       ,float64 new_N_subsurface_conc
       #endif
         // N conc is actually only used when nitrogen is enabled
         // but now N and C/N is a basic element of biomatter class
      ,const Residue_decomposition_parameters &residue_decomposition_params);    //060219
#ifdef NITROGEN
#if ((CROPSYST_VERSION > 0) && (CROPSYST_VERSION <=4))
   virtual void add_manure_pool_original
      (Organic_matter_source OM_source
      ,float64 _shallow_N_content
      ,float64 _deep_N_content
      ,float64 _decomposition_constant_lignified                                 //060122
      ,float64 _default_carbon_nitrogen_ratio   // Used when we don't have a N mass (or for initial residue)   //060225
      ,float64 _carbon_fraction // around 0.5                                    //080225
      ,float64 _area_per_mass);                                                  //070707
#endif
   virtual void add_manure_pool_cycled                                           //080829
      (const Biomass_decomposition_parameters &manure_decomp_params              //020324
      ,nat8    injection_layer                                                   //160120
      ,float64 surface_N_content     // kg N/m2? Currently goes in (sub)layer 1   //090224
      ,float64 subsurface_N_content  // kg N/m2? into specified injection_layer  //160120
      ) modification_;
#endif
 public:
   virtual void exhaust_all();
 public:
   virtual void setup_contribution(Organic_biomatter_common &residue)     const;
 private:
   inline virtual Organic_matter_cycling get_first_OM_cycling()           const  { return OM_SINGLE_CYCLING; }    //070707
   inline virtual Organic_matter_cycling get_last_OM_cycling()            const  { return OM_LIGNIFIED_CYCLING; } //070707
 private:
   float64 fixed_carbon_fraction_from_residue_to_OM_pool
      (Organic_matter_cycling
      ,float64 &C_fraction_from_residue_to_CO2)                           const; //110929
 /* keep this for checking abstract
 private:
     inline Residues_cycling *check_abstract()                                   //020307
     { return new Residues_cycling
         (organic_matter_pools
         ,soil
         ,soil_abiotic_environment_profile
         ,evaporation_potential_remaining                                        //160412
         ,temperature_air_avg                                                    //150427
         ,wind_speed);                                                           //150427
     }
   */
};
//_Residues_cycling____________________________________________________________/
#endif

