#include "corn/primitive.h"
#include "common/residue/residues_cycling.h"
#include "common/residue/residue_decomposition_param.h"
#include "common/biomatter/biomass_decomposition_param.h"
#include "common/biomatter/decomposition_const.h"
#include "organic_matter/OM_pools_common.h"
#include "organic_matter/OM_common.h"
#include "organic_matter/multiple_pool/OM_multiple_pool.h"
#include "soil/soil_I.h"
#include "soil/texture_I.h"
#include "soil/structure_I.h"
#include "soil/abiotic_environment.h"
#include "corn/math/compare.hpp"
#include "common/residue/residue_balancer.h"
#include <math.h>

using namespace std;

#define TL_Residue "Residue"
#define TL_Delete  "Delete"
#ifdef DETAILED_BALANCES
extern C_Balance *C_balance;                                                     //111005
#endif
//______________________________________________________________________________
Residues_cycling::Residues_cycling
(Organic_matter_pools_common        &pools_
,Soil::Soil_interface                     &soil_
,Soil::Abiotic_environment_profile   &soil_abiotic_environment_profile_
,modifiable_ CORN::Dynamic_array<float64> &evaporation_potential_remaining_      //160719
,const Air_temperature_average      &temperature_air_avg_                        //150427
,const Wind_speed                   &wind_speed_)                                //150427
:Residues_V4_common
   (pools_,soil_
   ,soil_abiotic_environment_profile_
   ,evaporation_potential_remaining_                                             //160412
   ,temperature_air_avg_,wind_speed_)                                            //140507_070707
,OM_pools                                                               (pools_)
,soil_layers                                               (*soil_.ref_layers())
,soil_structure                                         (*soil_.ref_structure()) //160412_110930
{}
//_Residues_cycling:constructor________________________________________________/
void Residues_cycling::add_surface_plant_pool
( Organic_matter_position position                                               //040517
,Organic_matter_cycling _om_cycling_obsolete_in_V4_4                             //060219
,float64 new_surface_biomass
#ifdef NITROGEN
//LML 140909 added macro
,float64 new_N_surface_conc
#endif
,const Residue_decomposition_parameters &decomp_params)                          //060219
{
   // In these arrays only  OM_FAST_CYCLING, OM_SLOW_CYCLING, and OM_LIGNIFIED_CYCLING are relevent
   // Currently there is no single cycling mode for plant material (only manure)
   float64 top_component_fract [OM_CYCLING_COUNT];
   top_component_fract[OM_FAST_CYCLING] = decomp_params.fast_cycling_top_fraction;
   top_component_fract[OM_SLOW_CYCLING] = decomp_params.slow_cycling_top_fraction;
   top_component_fract[OM_LIGNIFIED_CYCLING]=decomp_params.lignified_top_fraction;
   top_component_fract[0] = 0;
   top_component_fract[OM_INTRINSIC_CYCLING] = 0;
   // transfer coeficients for each pool:
   const Soil::Abiotic_environment_layer &soil_environment_point_1 = soil_abiotic_environment_profile.ref_soil_abiotic_environment_layer(1);
   for (nat8 /*Organic_matter_cycling*/ cycling = OM_FAST_CYCLING; cycling <= OM_LIGNIFIED_CYCLING; cycling++)
   {
      float64 component_surface_biomass
         = new_surface_biomass * top_component_fract[cycling];
      float64 new_decomposition_constant
         = decomp_params.get_decomposition_constant((Organic_matter_cycling)cycling); //061107
      // In each of these cases I append because this is the order in CropSystVB
      // and V4.1 prepended, not sure why
      float64 carbon_fraction = decomp_params.carbon_fraction;                   //060328
      // Note: I want to pass each: standing_stubble_biomass,
      // flat_surface_biomass,  and subsurface array
      if (component_surface_biomass > 0.0)                                       //060426
      {                                                                          //060426
         float64 CN_ratio               =
         #ifdef NITROGEN
          calc_carbon_nitrogen_ratio(component_surface_biomass
          ,new_N_surface_conc,carbon_fraction,straw_carbon_nitrogen_ratio_V4_4); //060530
         #else
            straw_carbon_nitrogen_ratio_V4_9;                                    //140923
         #endif
         float64 area_per_mass          = 0.0;                                   //060219
             // must be calculated for each different type of surface residue
         float64 &contact_fraction      = flat_surface_contact_fraction;         //060530
            // will be set below, note that this is a reference to residues object
         switch (position)                                                       //060530
         {  case OM_stubble_surface_position :                                   //060530
            {   area_per_mass          = decomp_params
                  .get_area_covered_to_mass_ratio(true);                         //060530
                contact_fraction       = stubble_contact_fraction;               //060530
            } break;
            case OM_attached_to_plant_position :                                 //060530
            {   area_per_mass          = decomp_params
                  .get_area_covered_to_mass_ratio(true); // Same as stubble      //060530
                contact_fraction       = stubble_contact_fraction;               //060530
            } break;
            default : // flat surface residue                                    //060530
            {   area_per_mass          = decomp_params
                  .get_area_covered_to_mass_ratio(false);                        //060530
                contact_fraction       = flat_surface_contact_fraction;          //060530
            } break;
         }
         Organic_biomatter_common *residue= organic_matter_pools.render_OM_pool
            (STRAW_RESIDUE,position,0,(Organic_matter_cycling)cycling
            ,decomp_params.RUSLE2_type_clad.get()
            ,component_surface_biomass,carbon_fraction,CN_ratio
            ,new_decomposition_constant
            ,decomp_params.get_detrition_constant()                              //090413
            ,area_per_mass,contact_fraction
            ,*soil.ref_texture()                                                                                                                                              //070707
            ,soil_environment_point_1); // all surface interacts with top soil layer
         setup_contribution(*residue);                                           //060728
         organic_matter_pools.append(residue);
         #ifdef DETAILED_BALANCES
         C_balance->add_residue(residue->get_carbon_kg());                       //111005
         #endif
         #ifdef DETAILED_BALANCES
         if (residue_mass_balance[position])                                                                                                                                  //090407
             residue_mass_balance[position]->added += residue->kg();                                                                                                          //090407
         #endif
      }                                                                                                                                                                      //060426
   }
   // CropSyst V3 and V4.1  updated totals here
   // In V4.4 adding pools marks the organic_matter_pools as modified which
   // I am going to then update totals when get_total_();
   // Delete this comment once I verify that I have done that
}
//_add_surface_plant_pool______________________________________________________/
void Residues_cycling::add_subsurface_plant_pool
(soil_sublayer_array_64(new_subsurface_biomass)                                  //970421
#ifdef NITROGEN
//LML 140909 added macro
,float64 new_N_subsurface_conc
 #endif
,const Residue_decomposition_parameters &decomp_params                           //060219
)
{  // In these arrays only  OM_FAST_CYCLING, OM_SLOW_CYCLING, and OM_LIGNIFIED_CYCLING are relevent
   // Currently there is no single cycling mode for plant material (only manure)
   float64 top_component_fract[OM_CYCLING_COUNT];
      top_component_fract[OM_FAST_CYCLING]      =decomp_params.fast_cycling_top_fraction;
      top_component_fract[OM_SLOW_CYCLING]      =decomp_params.slow_cycling_top_fraction;
      top_component_fract[OM_LIGNIFIED_CYCLING] =decomp_params.lignified_top_fraction;
      top_component_fract[0] = 0;
      top_component_fract[OM_INTRINSIC_CYCLING] = 0;
   float64 root_component_fract[OM_CYCLING_COUNT];
      root_component_fract[OM_FAST_CYCLING]     =decomp_params.fast_cycling_root_fraction;
      root_component_fract[OM_SLOW_CYCLING]     =decomp_params.slow_cycling_root_fraction;
      root_component_fract[OM_LIGNIFIED_CYCLING]=decomp_params.lignified_root_fraction;
      root_component_fract[0] = 0;
      root_component_fract[OM_INTRINSIC_CYCLING] = 0;
      // transfer coeficients for each pool:
   for (nat8 /*Organic_matter_cycling*/ cycling = OM_FAST_CYCLING
       ; cycling <= OM_LIGNIFIED_CYCLING; cycling++)                             //061107
   {
      float64 new_decomposition_constant  = decomp_params.get_decomposition_constant
         ((Organic_matter_cycling)cycling);                                      //061107
      // In each of these cases I append because this is the order in CropSystVB  and V4.1 prepended, not sure why
         float64 carbon_fraction                = decomp_params.carbon_fraction; //060328
      // Note: I want to pass each: standing_stubble_biomass, flat_surface_biomass,  and subsurface array
      for (Layer layer = 1; layer <= MAX_soil_sublayers; layer++)
      if ((new_subsurface_biomass[layer] > 0.00001) && (horizons[layer] > NULL_HORIZON))
      {  // we have straw in this layer
         const Soil::Abiotic_environment_layer &soil_environment_point_layer     //190215
            = soil_abiotic_environment_profile.ref_soil_abiotic_environment_layer(layer);
         float64 component_subsuface_biomass    = new_subsurface_biomass[layer] * root_component_fract[cycling];
         float64 subsurface_CN_ratio            =
         #ifdef NITROGEN
            calc_carbon_nitrogen_ratio
               (component_subsuface_biomass,new_N_subsurface_conc
               ,carbon_fraction,straw_carbon_nitrogen_ratio_V4_4);               //060225
         #else
            straw_carbon_nitrogen_ratio_V4_9;                                    //140923
         #endif
         Organic_biomatter_common *subsurface_residue= organic_matter_pools.render_OM_pool
         (STRAW_RESIDUE,OM_subsurface_position
            ,layer,(Organic_matter_cycling)cycling
            ,decomp_params.RUSLE2_type_clad.get()
            ,component_subsuface_biomass,carbon_fraction,subsurface_CN_ratio
            ,new_decomposition_constant
            ,0.0   // does not apply to subsurface decomp_params.get_decomposition_constant_nonmicrobial()  //090413
            ,0.0,subsurface_contact_fraction
            ,*soil.ref_texture()                                                 //070707
            ,soil_environment_point_layer
         );
         setup_contribution(*subsurface_residue);                                //060728
         Organic_biomatter_common *comparable_residue = organic_matter_pools
            .find_similar_organic_biomatter(*subsurface_residue);                //060605
         #ifdef DETAILED_BALANCES
         C_balance->add_residue (subsurface_residue->get_carbon_kg());           //111005
         #endif
         #ifdef DETAILED_BALANCES
         if (residue_mass_balance[OM_subsurface_position])                       //090408
             residue_mass_balance[OM_subsurface_position]->added += subsurface_residue->kg(); //090407
         #endif
         if (comparable_residue)                                                 //060605
         {  comparable_residue->add_mass(subsurface_residue->kg());
            delete subsurface_residue; subsurface_residue = 0; }                 //060605
         else organic_matter_pools.append(subsurface_residue);
               // Find similar pool and add to it                                //060605
      }
   }
   // CropSyst V3 and V4.1  updated totals here
   // In V4.4 adding pools marks the organic_matter_pools as modified which
   // I am going to then update totals when get_total_();
   // Delete this comment once I verify that I have done that
}
//_add_subsurface_plant_pool___________________________________________________/
#ifdef NITROGEN
#if ((CROPSYST_VERSION > 0) && (CROPSYST_VERSION <=4))
// This is being replaced by add_manure_pool_cycled
void Residues_cycling::add_manure_pool_original
(Organic_matter_source _OM_source                                                //120316
// needed ?,nat8                injection_layer                                  //160120
,float64             _shallow_N_content      // kg N/m2? Currently goes in (sub)layer 1
,float64             _deep_N_content        // kg N/m2? currently goes in (sub)layer 2
,float64             _decomposition_constant_lignified                           //060122
,float64             _default_carbon_nitrogen_ratio_UNUSED   // Used when we dont have a N mass (or for initial residue)  //060225
,float64             _carbon_fraction                                            //080225
,float64             _area_per_mass_UNUSED)
{
   // This is the original add_manure_pool method
   // we never really tested it.
   // It assumed that manure is primarily ligninous material.
   // It has been replaced with one that accomodates slow fast and ligninous material. //080829
   // Warning need to have these for different types of manure
   float64 N_percent_dry_wt = N_in_manure_percent_dry_wt[_OM_source];            //080828

   /*170104 I think this was wrong, anyway now water is stored as depth (effectively mass)
   computed below
   curr_water_hold += manure_water_hold;
   */

   float64 N_fraction = CORN::is_approximately<float64>(N_percent_dry_wt, 0.0,0.0000001) ? 0 : N_percent_dry_wt/100.0;     //080828_120312 // was N_percent_dry_wt/100.0
   float64 carbon_nitrogen_ratio =  CORN::is_approximately<float64>(N_fraction, 0.0,0.0000001) ? 0 :                       //080829
      _carbon_fraction / N_fraction;
   float64 manure_carbon_nitrogen_ratio[OM_CYCLING_COUNT];
   manure_carbon_nitrogen_ratio [OM_SINGLE_CYCLING]   = carbon_nitrogen_ratio;   // 120318 50.0;
   manure_carbon_nitrogen_ratio [OM_FAST_CYCLING]     = carbon_nitrogen_ratio;   // 120318 50.0;
   manure_carbon_nitrogen_ratio [OM_SLOW_CYCLING]     = carbon_nitrogen_ratio;   // 120318 50.0;
   manure_carbon_nitrogen_ratio [OM_LIGNIFIED_CYCLING]= carbon_nitrogen_ratio;   // 120318 50.0;
   nat8  cycling = OM_LIGNIFIED_CYCLING;                                         //070122
   // Claudio decided that manure is probably predominately
   // ligninous material. (This is also the only material we have data for.)
   {
      float64 decomposition_constant = _decomposition_constant_lignified;        //070122
      //080225 now user assignable   float64 carbon_fraction= 0.5;
         // Warning need to get carbon_fraction from table or parameters
      // Layer 1 (shallow) pool
      float64 biomass_to_layer_1 = _shallow_N_content * N_fraction ; //shallow
         Organic_biomatter_common *subsurface_manure_1= organic_matter_pools.render_OM_pool
         (MANURE_RESIDUE,OM_subsurface_position
         ,1,(Organic_matter_cycling)cycling,RUSLE2_MANURE_RESIDUE_TYPE
         ,biomass_to_layer_1,_carbon_fraction,manure_carbon_nitrogen_ratio[cycling]
         ,decomposition_constant
         ,0.0 // NYI not yet applicable to manure                                //090413
         ,0.0,subsurface_contact_fraction
         ,*soil.ref_texture()                                                    //070707
         ,soil_abiotic_environment_profile.ref_soil_abiotic_environment_layer(1));

      setup_contribution(*subsurface_manure_1);                                  //060721
      #ifdef DETAILED_BALANCES
      C_balance->add_residue(subsurface_manure_1->get_carbon_kg());              //111005
      #endif
      organic_matter_pools.prepend(subsurface_manure_1);
      // Layer 2 (deep)    pool
      float64 biomass_to_layer_2 = _deep_N_content * N_fraction;    //deep
      #ifdef OLD_SINGLE_RESIDUES_POOL
      Organic_biomatter_multiple_pool *subsurface_manure_2=
         new Organic_biomatter_multiple_pool
      #else
      Organic_biomatter_common *subsurface_manure_2=
         organic_matter_pools.render_OM_pool
      #endif
         (MANURE_RESIDUE,OM_subsurface_position
         // needed ? ,injection_layer //160120
         ,2
         ,(Organic_matter_cycling)cycling,RUSLE2_MANURE_RESIDUE_TYPE
         ,biomass_to_layer_2,_carbon_fraction,manure_carbon_nitrogen_ratio[cycling]
         ,decomposition_constant
         ,0.0  // nonmicrobial decomposition constant is not yet applicable to manure  //090413
         ,0.0,subsurface_contact_fraction
         ,*soil.ref_texture()                                                    //070707
         ,soil_abiotic_environment_profile.ref_soil_abiotic_environment_layer(2));

      setup_contribution(*subsurface_manure_2);                                  //060721
      #ifdef DETAILED_BALANCES
      C_balance->add_residue(subsurface_manure_2->get_carbon_kg());              //111005
      #endif
      organic_matter_pools.prepend(subsurface_manure_2);
   }
}
#endif
//_add_manure_pool_original____________________________________________________/
void Residues_cycling::add_manure_pool_cycled
(const Biomass_decomposition_parameters &manure_decomp_params                    //020324
,nat8       injection_layer                                                      //160120
,float64             _surface_N_content       // kg N/m2? Currently goes in (sub)layer 1
,float64             _subsurface_N_content    // kg N/m2? goes into specified layer //160120 currently goes in (sub)layer 2
) modification_
{
   /*170104 I think this was wrong water hold is m3/kg, anyway now using water storage
   curr_water_hold += manure_water_hold;
   */
   // Warning need to have these for different types of manure

   bool cycling_pools  = manure_decomp_params.composition_decomposition.cycling_pools;
   nat8 cycling_start = cycling_pools ? OM_FAST_CYCLING :OM_SINGLE_CYCLING;      //120312
   nat8 cycling_end   = cycling_pools ? OM_LIGNIFIED_CYCLING :OM_SINGLE_CYCLING; //120312
   float64 biomass_to_surface = 0.0;                                             //170104
   float64 biomass_to_layer_2 = 0.0;                                             //170104
   for (nat8 cycling_i = (nat8) cycling_start; cycling_i <= cycling_end ; cycling_i++)
   {  // looping with nat8 because some C++ compilers dont allow looping enum
      Organic_matter_cycling cycling  = (Organic_matter_cycling)cycling_i;       //090309

      // Note, currently we assume application goes into the soil
      // with no surface manure residue.
      // If that is added then we need to use the
      // area_to_mass_ratio_after_application.
      // Area to mass is 0.0 for shallow and deep pools.

      // Layer 1 (shallow) pool
      float64 nitrogen_fraction = manure_decomp_params.composition_decomposition.nitrogen_percent_DryWt / 100.0;  //080908
      float64 carbon_fraction   = manure_decomp_params.composition_decomposition.carbon_percent_DryWt   / 100.0;  //080908
      biomass_to_surface = manure_decomp_params.get_fraction_of_biomass
         ((Organic_matter_cycling)cycling)*_surface_N_content/nitrogen_fraction; //shallow //090224
      float64 &surface_contact_fraction      = manure_surface_contact_fraction;
         // will be set below, now that this is a reference to residues object   //090225
      Organic_biomatter_common *surface_manure= organic_matter_pools.render_OM_pool
         (MANURE_RESIDUE,OM_flat_surface_position
         ,1,(Organic_matter_cycling)cycling,RUSLE2_MANURE_RESIDUE_TYPE
         ,biomass_to_surface                                                     //090244 biomass_to_layer_1
         ,carbon_fraction
         ,manure_decomp_params.get_carbon_nitrogen_ratio(cycling)
         ,manure_decomp_params.get_decomposition_constant(cycling)
         ,0.0  // nonmicrobial decomposition constant is not yet appl. to manure //090413
         ,0.0
         ,surface_contact_fraction
         ,*soil.ref_texture()                                                    //070707
         ,soil_abiotic_environment_profile.ref_soil_abiotic_environment_layer(1));

      setup_contribution(*surface_manure);                                       //060721_090224
      #ifdef DETAILED_BALANCES
      C_balance->add_residue(surface_manure->get_carbon_kg());                   //111005
      #endif
      organic_matter_pools.prepend(surface_manure);                              //090224
      // Layer 2 (deep)    pool
      biomass_to_layer_2 = manure_decomp_params.get_fraction_of_biomass(cycling)
         * _subsurface_N_content /  nitrogen_fraction; //deep                    //090317
      Organic_biomatter_common *subsurface_manure_2= organic_matter_pools.render_OM_pool
         (MANURE_RESIDUE,OM_subsurface_position
         ,injection_layer
         ,(Organic_matter_cycling)cycling,RUSLE2_MANURE_RESIDUE_TYPE
         ,biomass_to_layer_2
         ,carbon_fraction
         ,manure_decomp_params.get_carbon_nitrogen_ratio(cycling)
         ,manure_decomp_params.get_decomposition_constant(cycling)
         ,0.0  // nonmicrobial decomp constant is not yet appl.  to manure       //090413
         ,0.0,subsurface_contact_fraction
         ,*soil.ref_texture()                                                    //070707
         ,soil_abiotic_environment_profile.ref_soil_abiotic_environment_layer(2));
      setup_contribution(*subsurface_manure_2);                                  //060721
      #ifdef DETAILED_BALANCES
      C_balance->add_residue(subsurface_manure_2->get_carbon_kg());              //111005
      #endif
      organic_matter_pools.prepend(subsurface_manure_2);
   }
   float64 water_to_surface
      = (biomass_to_surface + biomass_to_layer_2) * manure_water_hold;           //170104
         // m = kg/m2 * m3 / kg
   // Currently added all the manure water to the surface residues
   // but the water for layer 2 should go in layer 2.
   take_pool_water(water_to_surface);                                            //170104
   std::clog << "check manure water to surface" << std::endl;
}
#endif
//_add_manure_pool_cycled___________________________________________2008-08-29_/
void Residues_cycling::exhaust_all()
{
   float32 exhausted_today = 0.0;                                                //040523
   FOR_EACH_IN(popped_pool,Organic_biomatter_common,organic_matter_pools,each_pool) //070418
   {
      if ( popped_pool->is_straw_or_manure_residue())
      {  float64 pool_biomass = popped_pool->kg();
         if (CORN::is_approximately<float64>(pool_biomass,0.0,0.0000001)
             && (popped_pool->position != OM_attached_to_plant_position)
               // We can delete until the plant is done with it
               // (change it to stubble)                                         //060530
            )
         {  // pool is exhausted
            //NYI mass_balance.exhausted += pool_biomass;
            exhausted_today += pool_biomass;
            #ifdef DETAILED_BALANCES
            if (residue_mass_balance[popped_pool->position])                     //090408
                residue_mass_balance[popped_pool->position]->exhausted += pool_biomass;   //090407
            #endif
            each_pool->detach_current();                                         //070418
            delete popped_pool;
         } // pool is not exhausted
      }
      // else dont exhaust stable organic matter pools (only residues are exhausted)
      // Because the OM pools are linked to each other (for carbon contribution).
   } FOR_EACH_END(each_pool);                                                    //070418

/*170525 need to reimplement using Explaination

  if (detail_log && (exhausted_today > 0.0000001))                               //020307
             (*detail_log) << today.as_string() << '(' << today.get_DOY() <<')' << ':' << TL_Residue <<'-' << TL_Delete <<':' << exhausted_today << endl; //020618
*/
}
//_exhaust_all_________________________________________________________________/
float64 Residues_cycling::fixed_carbon_fraction_from_residue_to_OM_pool
(Organic_matter_cycling residue_carbon_cycling
,float64 &C_fraction_from_residue_to_CO2)                                  const
{
   C_fraction_from_residue_to_CO2 = 0.55;  // 0.55 is not used, the value will be set with repect to the cycling mode below
   switch (residue_carbon_cycling)
   {  case OM_FAST_CYCLING      : C_fraction_from_residue_to_CO2 = 0.6; break;
      case OM_SLOW_CYCLING      : C_fraction_from_residue_to_CO2 = 0.7; break;
      case OM_LIGNIFIED_CYCLING : C_fraction_from_residue_to_CO2 = 0.0; break;   //090518 was 0.3
      case OM_SINGLE_CYCLING    : C_fraction_from_residue_to_CO2 = 0.5; break;
         // This doesn't really apply to any case so this shouldn't occur
      default: /* other cylcings not applicable */ break;                        //090416
   }
   float64 C_fraction_from_residue_to_XXX = 1.0 - C_fraction_from_residue_to_CO2;   // To either microbial for fast and slow, or metastable for lignified   //060524
         // C_fraction_from_residue_to_XXX  is humification factor
   return C_fraction_from_residue_to_XXX;
}
//_get_carbon_fraction_from_residue_to_OM_pool______________________2011-09-29_/
void Residues_cycling::setup_contribution(Organic_biomatter_common &residue_) const
{  // This only applies to residue
   Organic_biomatter_pool_common &residue =
      (Organic_biomatter_pool_common &)residue_;                                 //110930_070707
   bool is_multiple_pool_mode =
      #ifdef CROPSYST_PROPER
      dynamic_cast<Organic_biomatter_pool_multiple *>(&residue_);                //110930
      #else
      false;
      #endif
   float64 C_fraction_from_residue_to_CO2 = 0.55;  // 0.55 is not used, the value will be set with repect to the cycling mode below
   nat8 contribute_to_layer = residue.is_above_ground()
      ? 1
      : residue.layer;
   float64 C_fraction_from_residue_to_pool =    // <- Humification
      is_multiple_pool_mode                                                      //110930
      ? fixed_carbon_fraction_from_residue_to_OM_pool
            (residue.carbon_cycling,C_fraction_from_residue_to_CO2)              //110929
      : std::numeric_limits<float32>::quiet_NaN();
      /*190215 this was erroneously only setting up the C fraction to transfer
      once at instanciation, however for the single pool it is
      based on the current soil OM carbon every day, so this value will
      be calculated later
      : calc_carbon_fraction_from_residue_to_OM_pool
            (residue, contribute_to_layer,C_fraction_from_residue_to_CO2);       //110930
      */
   // Note, in single OM pool mode, the receiving pool
   // returned by get_metastable_SOM_ref and get_microbial_biomass_ref is
   // simply the single pool.
   Organic_biomatter_pool_common *receiving_OM_pool =
      (residue.carbon_cycling == OM_LIGNIFIED_CYCLING)                           //060524
         ?  // flat surface contributes to layer 1                               //060524
            organic_matter_pools.get_metastable_SOM_ref(contribute_to_layer)     //110929
         :  // stubble  contributes to layer 1                                   //060524
            organic_matter_pools.get_microbial_biomass_ref(contribute_to_layer); //110929
   residue.contribute_to
      (C_fraction_from_residue_to_CO2
      ,receiving_OM_pool
      ,C_fraction_from_residue_to_pool);
}
//_setup_contribution_______________________________________________2006-07-28_/

