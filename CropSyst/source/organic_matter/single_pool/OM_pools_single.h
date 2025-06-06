#ifndef OM_pools_singleH
#define OM_pools_singleH
#include "organic_matter/OM_pools_common.h"
//_forward declarations_________________________________________________________
class Organic_matter_initialization_parameters;
class Organic_matter_decomposition_parameters;
class Organic_biomatter_pool_single;
//______________________________________________________forward declarations___/
class Organic_matter_pools_single
: public Organic_matter_pools_common
{
public:
   Organic_biomatter_pool_single           *SOM_ref[MAX_soil_layers];   // This is so lignified residue has access for contribute_to //060524
public: // structors
   Organic_matter_pools_single
      (const Organic_matter_decomposition_parameters &_organic_matter_decomposition_parameters //120912
      ,Soil::Abiotic_environment_profile  &_soil_abiotic_environment_profile     //060726
      ,const Soil::Layers_interface       &_soil_layers                          //060726
      ,unmodifiable_ Soil::Structure      &_soil_structure                       //070725
      ,Soil::Nitrogen                     *_soil_N
      ,const Soil::Texture_interface      &_soil_texture);                       //070707
private:
   virtual Organic_biomatter_common *provide_organic_matter_pool                 //070707
      (Organic_matter_type type,nat8  layer)                         provision_; //190215
      // Instantiates as needed. Always returns a valid pointer
   inline virtual bool is_applicable(Organic_matter_type OM_type)   affirmation_ //170301
      { return OM_type == SINGLE_SOM; }
   inline virtual nat8 SOM_index_last()                                   const  //170301
      { return SINGLE_SOM; }
   virtual Organic_biomatter_common  *render_OM_pool                             //111001
      (Organic_matter_type     type
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
      ,float64                &contact_fraction                                  //190327
      ,const Soil::Texture_interface &soil_texture                               //070707
      ,const Soil::Abiotic_environment_layer &soil_abiotic_environment_layer) rendition_;
      //190215, Soil::Abiotic_environment_layer * _soil_abiotic_environment_layer) rendition_;
   virtual Organic_biomatter_pool_common *render_OM_pool_for_read() rendition_;  //161116
   virtual modifiable_ Organic_biomatter_pool_common  *get_metastable_SOM_ref(nat8 layer);    //110929
   virtual modifiable_ Organic_biomatter_pool_common  *get_microbial_biomass_ref(nat8 layer); //110929
      // In the case of single SOM pool there is just the one contribution pool
public:
   virtual bool initialize_layer
      (nat8  horizon,nat8  layer
      ,float64 sand_fraction                                                     //060418
      ,float64 initial_organic_matter_carbon_mass
      ,float64 microbial_decomposition_const
      ,const Organic_matter_initialization_parameters &organic_matter_historical_parameters  //060215
      ,const Soil::Abiotic_environment_layer &soil_abiotic_environment_layer_ref);     //190215
      // this was added so we can store historical equilibrated OM for futher future scenario runs 120912
   inline virtual nat32 write(std::ostream &)                       performs_IO_ //150728
      { return 0; }                                                              //120912
   virtual bool specialized_reference                                            //170228
      (Organic_biomatter_pool_common *OM_pool_common);
   virtual float64 potential_C_decomposition();                                  //111004
   float64 calc_carbon_fraction_from_residue_to_OM_pool                          //190215
      (const Organic_biomatter_pool_common &residue
      ,nat8 contribute_to_layer
      ,float64 &C_fraction_from_residue_to_CO2)                                  const;
};
//_Organic_matter_pools_single______________________________________2011-09-29_/
#endif

