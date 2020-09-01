#include "organic_matter/single_pool/OM_pools_single.h"
#include "organic_matter/single_pool/OM_single_pool.h"
#include "soil/abiotic_environment.h"
#include "soil/nitrogen_I.h"
#include "soil/disturbance.h"
#include "organic_matter/OM_const.h"
#include "organic_matter/OM_params.h"
#include <math.h>
#include "soil/structure_I.h"

#include "soil/texture_I.h"
#ifdef DETAILED_BALANCES
extern C_Balance *C_balance;                                                     //111005
#endif
//______________________________________________________________________________
Organic_matter_pools_single::Organic_matter_pools_single
(const Organic_matter_decomposition_parameters &OM_decomposition_parameters_     //120912
,Soil::Abiotic_environment_profile     &soil_abiotic_environment_profile_
,const Soil::Layers_interface          &soil_layers_
,unmodifiable_ Soil::Structure         &soil_structure_
,Soil::Nitrogen                        *soil_N_
,const Soil::Texture_interface         &soil_texture_)
: Organic_matter_pools_common
(OM_decomposition_parameters_
,soil_abiotic_environment_profile_
,soil_layers_
,soil_structure_
,soil_N_
,soil_texture_)
{  for (nat8  layer = 0; layer < MAX_soil_layers; layer++)
      SOM_ref[layer] = 0;                                                        //110929
}
//_Organic_matter_pools_single:constructor__________________________2011-09-29_/
bool Organic_matter_pools_single::initialize_layer
(nat8  horizon,nat8  layer
,float64 sand_fraction
,float64 initial_organic_matter_carbon_mass
,float64 microbial_decomposition_const  // Not used by this model
,const Organic_matter_initialization_parameters &OM_initialization_params
,const Soil::Abiotic_environment_layer &ref_soil_environment)
{  float64 SOM_C_mass             = initial_organic_matter_carbon_mass;
   float64 SOM_mass =  SOM_C_mass / SOM_carbon_fraction ;
   Organic_biomatter_pool_single  *SOM = new Organic_biomatter_pool_single
      (SINGLE_SOM
      ,layer
      ,SOM_mass
      ,SOM_carbon_fraction
      ,(float64)OM_decomposition_parameters.single_SOM_CN_ratio
      ,(float64)OM_decomposition_parameters.max_apparent_soil_C_decomposition_const  //110929
      ,0.0  // area_per_mass       // = 0.0  stubble and surface residue_only?
      ,subsurface_contact_fraction                                               //190327
      ,soil_texture                                                              //070707
      ,ref_soil_environment);
   SOM    ->contribute_to(1.0,0,0);
   append(SOM);
   #ifdef DETAILED_BALANCES
   C_balance->OM.initial+=SOM->get_carbon_kg();                                  //111005
   #endif
   organic_matter[SINGLE_SOM][layer] = SOM;                                      //060803
   SOM_ref[layer] = SOM;
   return true;
}
//_initialize_layer_________________________________________________2011-09-29_/
// can be common to single and multiple
Organic_biomatter_common *
Organic_matter_pools_single::provide_organic_matter_pool
(Organic_matter_type type,nat8  layer)                                provision_
{  Organic_biomatter_common *provided_pool = organic_matter[type][layer];        //070707
   if (!provided_pool)
   {  provided_pool = organic_matter[type][layer] = new
         Organic_biomatter_pool_single
            (type,layer,subsurface_contact_fraction,soil_texture                 //190327
            ,soil_abiotic_environment_profile.ref_soil_abiotic_environment_layer(layer));
   }
   return provided_pool;
}
//_provide_organic_matter_pool______________________________________2011-09-29_/
Organic_biomatter_common  *Organic_matter_pools_single::render_OM_pool
(Organic_matter_type     type_
,Organic_matter_position position_
,nat8                    layer_
,Organic_matter_cycling  cycling_
,RUSLE2_Residue_type     RUSLE2_residue_type_                                    //060731
,float64                 biomass_
,float64                 carbon_fraction_
,float64                 carbon_nitrogen_ratio_
,float64                 decomposition_constant_                                 //090703
,float64                 detrition_constant_                                     //090413
,float64                 area_per_mass_      // = 0.0  stubble and surface residue_only?
,float64                &contact_fraction_                                       //190327
,const Soil::Texture_interface &soil_texture_                                    //070707
,const Soil::Abiotic_environment_layer &soil_abiotic_env_layer_)      rendition_
{  return new Organic_biomatter_pool_single    // for residue
      (type_
      ,position_
      ,layer_
      ,cycling_
      ,RUSLE2_residue_type_                                                      //060721
      ,biomass_
      ,carbon_fraction_
      ,carbon_nitrogen_ratio_
      ,decomposition_constant_
      ,detrition_constant_                                                       //090702_090513
      ,area_per_mass_       // = 0.0  stubble and surface residue_only?
      ,contact_fraction_
      ,soil_texture_                                                             //070707
      ,soil_abiotic_env_layer_);
}
//_Organic_matter_pools_single______________________________________2011-10-01_/
Organic_biomatter_pool_common *Organic_matter_pools_single::render_OM_pool_for_read() rendition_
{
   Organic_biomatter_pool_single * OM =
   new Organic_biomatter_pool_single
   ((Organic_matter_type)0,(nat8)0 // type and layer values will be loaded from file
   ,0 // mass will be loaded from file
   ,SOM_carbon_fraction // probably loaded from file
   ,(float64)OM_decomposition_parameters.single_SOM_CN_ratio
   ,(float64)OM_decomposition_parameters.max_apparent_soil_C_decomposition_const //110929
      // For residue the CN_ratio and decomp const should be setup separately
      ,0.0  // area_per_mass       // = 0.0  stubble and surface residue_only?
   ,subsurface_contact_fraction                                                  //190327
   ,soil_texture
   ,soil_abiotic_environment_profile.ref_soil_abiotic_environment_layer(1));
      // the actual layer will be set later by take_OM_pool
   return OM;
}
//_render_OM_pool_for_read__________________________________________2016-11-16_/
modifiable_ Organic_biomatter_pool_common  *Organic_matter_pools_single
::get_metastable_SOM_ref(nat8 layer)
{  return SOM_ref[layer];
}
//_get_metastable_SOM_ref___________________________________________2011-09-29_/
modifiable_ Organic_biomatter_pool_common  *Organic_matter_pools_single
::get_microbial_biomass_ref(nat8 layer)
{  return SOM_ref[layer];
}
//_layer____________________________________________________________2011-09-29_/
/*161116  NYI  Currently assuming the initial organic matter file
   we are reading from is also from a run that used single pool model
   For completeness,should merge various types of organic matter
   (I.e. if the file was generated from
   a multi pool run) into the single pool.
   I am not sure how to do this.
*/
bool Organic_matter_pools_single::specialized_reference
(Organic_biomatter_pool_common *OM_pool_common)
{  Organic_biomatter_pool_single *OM_pool
      = dynamic_cast<Organic_biomatter_pool_single *>(OM_pool_common);
   if (OM_pool->is_straw_or_manure_residue())
   {  // WARNING this is assuming the SOM_ref (SOM pools) have been
      // instantiated and initialized before residues
      float64 C_fraction_from_residue_to_CO2 = 0.55;
         // 0.55 is the default value for a little bit more than single cycling residues.
         // see get_carbon_fraction_from_residue_to_OM_pool()
      float64 C_fraction_from_residue_to_pool = 1.0 - C_fraction_from_residue_to_CO2;
      Organic_biomatter_pool_common *receiving_OM_pool
         = SOM_ref[OM_pool->layer];
      OM_pool->contribute_to
      (C_fraction_from_residue_to_CO2
      ,receiving_OM_pool
      ,C_fraction_from_residue_to_pool);
   } else // is soil OM
   {  SOM_ref[OM_pool->layer] = OM_pool;
      OM_pool->contribute_to(1.0,0); // all SOM decomposed C is to CO2
   }
   return true;
}
//_specialized_reference____________________________________________2017-02-28_/
float64 Organic_matter_pools_single::potential_C_decomposition()
{
   // In the single pool model, the fraction of decomposed carbon sent to the SOM
   // pool is dependent of clay content and current daily soil organic C.
   FOR_EACH(org_BM,Organic_biomatter_pool_common,org_BM_iter)
   {  if (org_BM->is_straw_or_manure_residue())
      {
         org_BM->C_fraction_transferred_to[1] =
            calc_carbon_fraction_from_residue_to_OM_pool
            (*org_BM
            ,org_BM->is_above_ground() ? 1 : org_BM->layer
            ,org_BM->C_fraction_transfered_to_CO2);
      }
   } FOR_EACH_END(org_BM_iter)
   return Organic_matter_pools_common::potential_C_decomposition();
}
//_potential_C_decomposition________________________________________2019-02-15_/
float64 Organic_matter_pools_single::calc_carbon_fraction_from_residue_to_OM_pool
(const Organic_biomatter_pool_common &residue
,nat8 contribute_to_layer
,float64 &C_fraction_from_residue_to_CO2)                                  const
{ // C_conc_g_kg is Carbon concentration  in g/Kg

   // This only applies to single pool (Armens humification)

   float64 clay_percent = soil_texture.get_clay_percent(contribute_to_layer);
   float64 C_mass_g_m2 = kg_to_g(provide_organic_matter_pool
      (MICROBIAL_BM_OR_SINGLE_SOM
      ,contribute_to_layer)->get_carbon_kg());
   float64 soil_mass_kg_m2 = soil_structure.get_mass_kg_m2(contribute_to_layer) ;
   float64 soil_C_conc_g_kg = C_mass_g_m2 / soil_mass_kg_m2;
   float64 clay_fraction = clay_percent / 100.0;                                 //070707
   float64 Saturated_Soil_Carbon_Concentration = 21.1 + 0.375 * clay_percent;
      // 6.0 is soil humification power
   float64 humification_adjustment_by_current_SOC
     = 1.0 - pow((soil_C_conc_g_kg / Saturated_Soil_Carbon_Concentration),6.0);
   // this equation reduces the humification when the C concentration approaches saturation for the layer
   humification_adjustment_by_current_SOC =
      CORN::must_be_between<float64>(humification_adjustment_by_current_SOC,0.0,1.0);

   // humification_adjustment_by_current_SOC need to be calucated every day because SOM carbon is changing


   // Note that Armen was working on surface and subsurface
   // humification factor.  For example, the 0.092 and 0.084 terms would be
   // adjusted for  subsurface but it is not known what these should be.         //070727
   // Armen has since added Rizosphere residue type, in this case
   // the values are  0.0 and 0.084

   //081212 Armen is using the following value        this could be done once
   float64 clay_fraction_factor // need a better name for this                   //081212
         (1.0  -  exp( - 5.5  *  clay_fraction));                                //081212
   float64 maximum_humification_factor =                                         //081212
         (residue.type == STRAW_RESIDUE)                                         //081212
          ?  (0.092 + 0.104  * clay_fraction_factor)   //plant residue           //081212
          :  (0.15  + 0.25   * clay_fraction_factor);  // manure residue (to reach a value of 0.38 of manure) //081212

   // NYI Rizosphere
   //     :  (0.0 + 0.084  * clay_fraction_factor);  // Rizosphere residue

   float64 humification_factor = maximum_humification_factor  *  humification_adjustment_by_current_SOC;
   C_fraction_from_residue_to_CO2 = 1.0 - humification_factor;
   return humification_factor;
}
//_calc_carbon_fraction_from_residue_to_OM_pool_____________________2019-02-15_/

