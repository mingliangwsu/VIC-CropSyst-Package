#include "organic_matter/OM_common.h"
#include "corn/math/compare.hpp"
#include "soil/texture_I.h"
#include <math.h>
#ifdef DETAILED_BALANCES
#include "common/residue/residue_balancer.h"
C_Balance *C_balance=0;                                                          //111005
#endif

//______________________________________________________________________________
Organic_biomatter_common::Organic_biomatter_common
(const Organic_biomatter_common &from_copy)
: Biomass_abstract(from_copy)
, type                                            (from_copy.type)
, position                                        (from_copy.position)
, layer                                           (from_copy.layer)
, detrition_constant                              (from_copy.detrition_constant) //090702
, soil_texture                                    (from_copy.soil_texture)
, soil_abiotic_env                                (from_copy.soil_abiotic_env)
{}
//_Organic_biomatter_common:constructor________________________________________/
Organic_biomatter_common::Organic_biomatter_common    // For stable provide
(Organic_matter_type     type_
,nat8                    layer_
,Organic_matter_position position_
,float64                 biomass_
,float64                 carbon_fraction_
,float64                 carbon_nitrogen_ratio_
,float64                 decomposition_constant_
,float64                 detrition_constant_
   // only applies to residues (otherwise should be ????)                        //090702
,const Soil::Texture_interface &soil_texture_                                    //070707
,const Soil::Abiotic_environment_layer &soil_abiotic_env_)                       //190215
:Biomass_abstract
   (biomass_
   ,carbon_fraction_                                                             //070726
   ,carbon_nitrogen_ratio_                                                       //070726
   ,decomposition_constant_)                                                     //100210
, type                           (type_)
, position                       (position_)
, layer                          (layer_)
, detrition_constant             (detrition_constant_)                           //090702
, soil_texture                   (soil_texture_)
, soil_abiotic_env (const_cast<Soil::Abiotic_environment_layer *>(&soil_abiotic_env_))
{}
//_Organic_biomatter_common:constructor_________________________________________
bool Organic_biomatter_common::is_straw_or_manure_residue()                const
{  return  (type == STRAW_RESIDUE) || (type == MANURE_RESIDUE); }
//______________________________________________________________________________
bool Organic_biomatter_common::is_manure()                                 const
{  return  (type == MANURE_RESIDUE); }
//______________________________________________________________________________
bool Organic_biomatter_common::is_standing_stubble()                       const
{  return  (position == OM_stubble_surface_position); }
//______________________________________________________________________________
bool Organic_biomatter_common::is_flat_surface()                           const
{  return  (position == OM_flat_surface_position); }
//_is_flat_surface__________________________________________________2006-07-28_/
bool Organic_biomatter_common::is_above_ground()                           const
{  // NYI may want to specify flags for what to include as above ground.
   return position > OM_subsurface_position;
}
//_is_above_ground__________________________________________________2006-07-17_/
bool  Organic_biomatter_common::is_comparable(const Organic_biomatter_common &to_other) const
{  Organic_matter_cycling  this_cycling = get_carbon_cycling();                  //090827
   Organic_matter_cycling  other_cycling = to_other.get_carbon_cycling();        //090827
   bool comparable
   =  (type       == to_other.type)
   && (position   == to_other.position)
   && (layer      == to_other.layer)
   && (this_cycling == other_cycling)                                            //090827
   && (CORN::is_approximately<float64>(carbon_fraction        ,to_other.carbon_fraction       ,0.000001))
   && (CORN::is_approximately<float64>(carbon_nitrogen_ratio  ,to_other.carbon_nitrogen_ratio ,0.000001))
   && (CORN::is_approximately<float64>(decomposition_constant ,to_other.decomposition_constant,0.000001))
   && (CORN::is_approximately<float64>(detrition_constant     ,to_other.detrition_constant    ,0.000001)); //090702
   return comparable;
}
//_is_comparable____________________________________________________2006-06-05_/
float64 Organic_biomatter_common::add_different_mass
(const Organic_biomatter_common &added_biomatter)                                //060728
{  float64 mass_to_add           = added_biomatter.kg();                         //060728
   float64 current_mass = kg();                                                  //060316
   add_similar_mass(added_biomatter);                                            //070726
   if (mass_to_add > current_mass)                                               //060721
      set_RUSLE2_residue_type(added_biomatter.get_RUSLE2_residue_type());
   // This needs to go in mass balance too somehow
   // Determine new area to mass ratio weighted by mass
   if (is_above_ground())    // area_per_mass only applies to above ground       //060728
   {  float64 current_area_per_mass = get_area_per_mass();                       //060728
      float64 new_area_per_mass                                                  //060728
        = (current_mass * current_area_per_mass                                  //060728
         + mass_to_add * added_biomatter.get_area_per_mass())                    //070707
         / (current_mass + mass_to_add);                                         //060728
      set_area_per_mass(new_area_per_mass);                                      //070707
   }
   return kg();                                                                  //060728
}
//_add_different_mass___________________________________________________________
modifiable_ Organic_biomatter_common *Organic_biomatter_common
::detrition()                                                      modification_
{  Organic_biomatter_common *detritis = 0;
   if (detrition_constant >0.000000001)
   {  detritis  = clone();
      detritis->set_position(OM_subsurface_position); // The detritis is comingled with the first soil layer
      detritis->layer = 2;                            // The detrition_constant is set to 0 by set_position.
      float64 non_decayed_fraction        = exp(-detrition_constant);
      float64 decayed_fraction            = (1.0 - non_decayed_fraction);
      detritis->multiply_by(decayed_fraction);
      this    ->multiply_by(non_decayed_fraction);
   }
   return detritis;
}
//_detrition________________________________________________________2009-07-02_/
#ifdef DETAILED_BALANCES
#include <common/residue/residue_balancer.h>
extern Residue_balancer *residue_mass_balance[OM_POSITION_COUNT];                //090407
#endif
#include "soil/abiotic_environment.h"
//______________________________________________________________________________
Organic_biomatter_pool_common::Organic_biomatter_pool_common
      (Organic_matter_type     type_
      ,nat8                    layer_
      ,Organic_matter_position position_
      ,Organic_matter_cycling  cycling_
      ,RUSLE2_Residue_type     RUSLE2_residue_type_                              //060721
      ,float64                 biomass_
      ,float64                 carbon_fraction_
      ,float64                 carbon_nitrogen_ratio_
      ,float64                 decomposition_constant_
      ,float64                 detrition_constant_                               //090702
      ,float64                 area_per_mass_       // = 0.0  stubble and surface residue_only?
      ,float64                &contact_fraction_                                 //190327
      ,const Soil::Texture_interface    &soil_texture_                           //070707
      ,const Soil::Abiotic_environment_layer  &soil_abiotic_environment_layer_)
: Organic_biomatter_common
      (type_
      ,layer_
      ,position_
      ,biomass_
      ,carbon_fraction_
      ,carbon_nitrogen_ratio_
      ,decomposition_constant_
      ,detrition_constant_                                                       //090702
      ,soil_texture_                                                             //070707
      ,soil_abiotic_environment_layer_)
, contact_fraction                                          (&contact_fraction_)
, carbon_cycling                                                      (cycling_)
, RUSLE2_residue_type                                     (UNKNOWN_RESIDUE_TYPE) //060731
, area_per_mass                                                 (area_per_mass_)
, C_fraction_transfered_to_CO2                                             (0.0) //120120
, C_decomposed_for_output_only                                             (0.0) //090520
{  for (int i = 0; i <= 3; i++) C_fraction_transferred_to[i] = 0.0;              //120120
}
// Constructor used for For SOM redistribution
//__Organic_biomatter_pool_common:constructor___________________________________
Organic_biomatter_pool_common::Organic_biomatter_pool_common
(const Organic_biomatter_pool_common &from_copy)
: Organic_biomatter_common(from_copy)
,  contact_fraction                 (from_copy.contact_fraction)
,  carbon_cycling                   (from_copy.carbon_cycling)
,  RUSLE2_residue_type              (from_copy.RUSLE2_residue_type)              //060731
,  area_per_mass                    (from_copy.area_per_mass)
,  C_fraction_transfered_to_CO2     (from_copy.C_fraction_transfered_to_CO2)
,  C_decomposed_for_output_only     (0.0)                                        //090520
{  for (int i = 0; i <= 3; i++)
      C_fraction_transferred_to[i] = from_copy.C_fraction_transferred_to[i];
}
//_Organic_biomatter_pool_common:constructor___________________________________/
void Organic_biomatter_pool_common::contribute_to
(float64       C_fraction_transfered_to_CO2_
,Organic_biomatter_pool_common *receiver_1 ,float64 C_fraction_transferred_to_1
,Organic_biomatter_pool_common *receiver_2 ,float64 C_fraction_transferred_to_2
,Organic_biomatter_pool_common *receiver_3 ,float64 C_fraction_transferred_to_3)
{  C_fraction_transfered_to_CO2 = C_fraction_transfered_to_CO2_;
   receiver[1] = receiver_1;
   receiver[2] = receiver_2;
   receiver[3] = receiver_3;
   C_fraction_transferred_to[1] = C_fraction_transferred_to_1;
   C_fraction_transferred_to[2] = C_fraction_transferred_to_2;
   C_fraction_transferred_to[3] = C_fraction_transferred_to_3;
}
//_contribute_to_______________________________________________________________/
float64 Organic_biomatter_pool_common::actual_C_decomposition
(float64 &N_deficit_for_immobilization
,float64 &decomposition_reduction_factor)
{  float64 act_carbon_decomp = 0.0;
   Decomposition *decomposition = mod_decomposition();                           //111003                                                 0
   if (decomposition)
   {  act_carbon_decomp = decomposition->actual
         (N_deficit_for_immobilization,decomposition_reduction_factor);
   }
   C_decomposed_for_output_only = act_carbon_decomp;                             //090520
   return act_carbon_decomp;
}
//_actual_C_decomposition______________________________________________________/
float64 Organic_biomatter_pool_common::Decomposition::actual
(float64 N_deficit_for_immobilization
,float64 decomposition_reduction_factor)
{  // Begin reduction to potential decomposition amount

   act_carbon = pot_carbon *
      (((net_N_mineralization < 0.0) // negative net_N_mineralization implies immobilization
         || is_a_residue)  // decomposition reduction only applies to residues   //060424
      ? decomposition_reduction_factor
      : 1.0);
   act_carbon = std::min<float64>(act_carbon,pool.get_carbon_kg());              //120312
   return act_carbon;
}
//_Decomposition::actual____________________________________________2011-10-04_/
float64 Organic_biomatter_pool_common::deduct_carbon(float64 removed_carbon_mass)
{
   if (removed_carbon_mass > 0.0)
   {  float64 removed_biomass = removed_carbon_mass / get_carbon_fraction();
      subtract_mass(removed_biomass);
      #ifdef DETAILED_BALANCES
      // Actually in general we should have an option to not do these balances
      // residue balance
      if (is_straw_or_manure_residue())                                          //080229
      {
         if (residue_mass_balance[position])                                     //090408
             residue_mass_balance[position]->decomposed += removed_biomass;      //090407
      }
      #endif
      get_carbon_nitrogen_ratio();
      // This should update carbon and nitrogen.
   }
   return get_carbon_kg();                                                       //111007
}
//_deduct_carbon_______________________________________________________________/
float64 Organic_biomatter_pool_common::receive_carbon(float64 added_carbon_mass)
{  float64 added_N = 0.0; // Not currently used;
   if (added_carbon_mass > 0.0000001)
   {  float64 added_biomass = added_carbon_mass /  get_carbon_fraction();
      add_mass(added_biomass);
   // This should update carbon and nitrogen.
   // (Carbon is not currently kept as a separate value).
   // Nitrogen should be adjusted relatively
   // but I should check with the following.
   // May need to explicitly set it, I dont think it will be the same.
      if (carbon_nitrogen_ratio > 0.00001)                                       //190404
      added_N =  added_carbon_mass / carbon_nitrogen_ratio;
      // else should never be 0.0 not sure how/why getting C/N 0
   }
   return get_carbon_kg();                                                       //190219
}
//_receive_carbon___________________________________________________2019-02-19_/
Organic_matter_position Organic_biomatter_pool_common
::set_position(Organic_matter_position new_position)               modification_
{  if (new_position == OM_subsurface_position)                                   //090702
      detrition_constant  = 0.0;                                                 //090702
   return Organic_biomatter_common::set_position(new_position);
}
//_set_position_____________________________________________________2009-04-13_/
void Organic_biomatter_pool_common::set_standing_stubble()
{  // This is used by decomposing canopy.
   // It is called when the canopy portion is deleted
   // (usually when the plant is completely dead).
   // It is only valid to call this when this matter is currently attached to the plant.
   if (position == OM_attached_to_plant_position)                                //061025
       position =  OM_stubble_surface_position;                                  //060125
   // Once the matter is no longer attached to the plant,
   // it is fully relinquished to the organic matter pools
   // and can be deleted when exhausted.
}
//_set_standing_stubble_____________________________________________2006-05-30_/
/*190327 obsolete pools now take a reference to contact fraction is relevent
float64 Organic_biomatter_pool_common::set_contact_fraction
(float64 &_contact_fraction) modification_
{   Xcontact_fraction = _contact_fraction;
    return Xcontact_fraction;
}
//_set_contact_fraction________________________________________________________/
*/
float64 &Organic_biomatter_pool_common::know_contact_fraction
(float64 &contact_fraction_)                                          cognition_
{  contact_fraction = &contact_fraction_;
   return contact_fraction_;
}
//_know_contact_fraction____________________________________________2019-03-27_/
float64 Organic_biomatter_pool_common::reset_mass(float64 new_mass) modification_
{
   // Check if this method is applicable only to multi pool
   float64 return_mass = Organic_biomatter_common::reset_mass(new_mass);
   if (type == MICROBIAL_BM_OR_SINGLE_SOM)                                       //060807
   {  // correct for change in sand (Not yet in VB)
      float64 sand_fraction = soil_texture.get_sand_percent(layer) / 100.0;      //060807
      float64 alpha = 0.25  / (1.0 + (pow((sand_fraction / 0.4), 3)));           //070801_050714 Added by Armen
         //070801 0.25 was 0.5
         // transfer coefficient
      float64 C_Fraction_From_MB_To_MetastableSOM = alpha;                       //060307
      float64 C_Fraction_From_MB_To_LabileSOM = (1.0 - alpha);                   //060307
      C_fraction_transferred_to[1] = C_Fraction_From_MB_To_LabileSOM;            //060307
      C_fraction_transferred_to[2] = C_Fraction_From_MB_To_MetastableSOM;        //060307
   }
   return return_mass;
}
//_reset_mass_______________________________________________________2007-07-07_/
Organic_biomatter_pool_common::Decomposition::Decomposition
(const Organic_biomatter_pool_common &pool_                                      //110930
,float64 tillage_decomposition_rate_adj_
,float64 minimized_temperature_moisture_function_
,bool    is_a_residue_)
:pool(pool_)
,minimized_temperature_moisture_function(minimized_temperature_moisture_function_)
,is_a_residue(is_a_residue_)
, pot_carbon             (0.0)
, act_carbon             (0.0)
, tillage_decomposition_rate_adj(tillage_decomposition_rate_adj_)
, N_immobilization_demand(0.0)
, net_N_mineralization   (0.0)
, N_mineralization       (0.0)
{}
//_Decomposition:constructor___________________________________________________/
float64 Organic_biomatter_pool_common::Decomposition::potential()    // original: CarbonDecomosed()
{  // This applies to:
   // All residue and organic matter in V4.2
   // Only residue in V4.3   (V4.3

// Not yet implemented hourly temperature
//For Hour = 1 To 24
//    Hourly_Temperature = SoilState.SoilTemperature(Hour, Layer)
//    Temperature_Function = AbioticFunctions.TemperatureFunction(Hourly_Temperature)
//    Term = Term + (1 - Exp(-(Carbon_Decomposition_Constant / 24) * Temperature_Function))
//Next Hour

/*
continue here need to make this like VB version
note that this is still sort of valid for Residue organic matter
so will need decomposition potential for that mode
*/


// This disables for debugging only
//minimized_temperature_moisture_function = 1.0;

#ifdef DEBUG_CARBON_TEST
minimized_temperature_moisture_function = 0.5;
tillage_decomposition_rate_adj = 1.0;
#endif

   float64 carbon_mass = pool.get_carbon_kg() ;
   pot_carbon =  carbon_mass * minimized_temperature_moisture_function
      * tillage_decomposition_rate_adj
            * pool.get_decomposition_constant();                                 //111001
   return pot_carbon;
}
//_potential___________________________________________________________________/
float64 Organic_biomatter_pool_common::potential_C_decomposition
(float64 &N_mineralization_sum
,float64 &N_immobilization_demand_sum
,float64 tillage_decomposition_rate_adj)
{
   float64 pot_C_decomp = 0.0;
   float64 carbon_mass = get_carbon_kg();
   Decomposition *decomposition = 0;
   if (carbon_mass > 0.0)
   {  // Only perform decomposition if there is mass (any carbon)
      const Soil::Abiotic_environment_layer &abiotic_env=ref_soil_abiotic_env(); //190118
      switch (type)
      {  case STRAW_RESIDUE : case  MANURE_RESIDUE :
         {  float64 moisture_funct    = abiotic_env.get_moisture_function();
            float64 temperature_funct = abiotic_env.get_temperature_function_daily_mean
               (Soil::decomposition_temperature_function);

            //printf("moisture_function(%f)\ttemperature_function(%f)\n",moisture_function,temperature_function);
            decomposition = provide_decomposition_residue
               (moisture_funct,temperature_funct);                               //111003
         } break;
         default :  // Soil organic matter, or microbial
         {  float64 minimized_temperature_moisture_function
               = abiotic_env.get_minimized_temperature_moisture_function();
            decomposition = provide_decomposition_SOM
               (tillage_decomposition_rate_adj,minimized_temperature_moisture_function);  // Not is a residue   is_residue()
         } break;
      }
      pot_C_decomp = decomposition->potential();
      update_net_N_mineralization();
      float64 CN_ratio = get_carbon_nitrogen_ratio();                            //170301
      //if (CORN::is_approximately<float64>(CN_ratio,0.0,0.000001))              //170301
      if (CN_ratio < 1)                                                          //170301
      {
      /*
         std::cerr << "This should not occur: "
         << organic_matter_type_label_table[type]
         << " C/N < 1 (layer=" << int(layer) << ")="
         << CN_ratio
         << std::endl;
      */
      }else
      decomposition->N_mineralization_or_immobilization_demand(CN_ratio);        //110930

      N_mineralization_sum       +=decomposition->N_mineralization;
      N_immobilization_demand_sum+=decomposition->N_immobilization_demand;
   }
//was debug only else
//std::clog << "no carbon" << std::endl;
return pot_C_decomp;
}
//_potential_C_decomposition________________________________________2011-10-03_/
bool Organic_biomatter_pool_common::update_net_N_mineralization()
{  // In VB this function is NetNMineralization()
   // Note that in the single OM model, this function is essentially
   // applicable only to residues because the SOM pool doesn't release C to other pools.
   float64 CN_ratio_decomposer = carbon_nitrogen_ratio;

   float64 NC_ratio_decomposer = (CN_ratio_decomposer == 0.0)                    //190212RLN
   ? 0.0                                                                         //190212RLN
   : 1.0 / CN_ratio_decomposer;                                                  //190212RLN
   Decomposition *decomposition = mod_decomposition();                           //190212RLN
   float64 decomposing_N = decomposition->pot_carbon * NC_ratio_decomposer;      //190212RLN
   float64 assimilate_N = 0.0;                                                   //190212RLN
   for (int r = 1; r <=3; r++)                                                   //190212RLN
   {  if (receiver[r])                                                           //190212RLN
      {                                                                          //190212RLN
         float64 CN_ratio_receiver=99999999;
         #ifdef KEMANIAN_HUMIFICATION
         CN_ratio_receiver = get_humified_carbon_nitrogen_ratio(i);              //190212RLN
         // For single pool, should be C/N of humified residue                   //190212RLN
         // In multiple pool, it is the carbon_nitrogen_ratio of the receiver    //190212RLN
         #else
         CN_ratio_receiver = receiver[r]->get_carbon_nitrogen_ratio();           //190214RLN
         #endif
         float64 NC_ratio_receiver = (CN_ratio_receiver == 0.0)                  //190212RLN
         ? 0.0                                                                   //190212RLN
         : 1.0 / CN_ratio_receiver;                                              //190212RLN
         float64 carbon_decomposed_available_to_receiver                         //190212RLN
            =
               C_fraction_transferred_to[r]
               //190215 abandoned get_C_fraction_transferred_to[r]
               * decomposition->pot_carbon;          //190212RLN
         float64 N_to_receiver =                                                 //190212RLN
           carbon_decomposed_available_to_receiver * NC_ratio_receiver;          //190212RLN
         assimilate_N += N_to_receiver;                                          //190212RLN
      }                                                                          //190212RLN
   }                                                                             //190212RLN
   float64 net_N_mineralization = decomposing_N - assimilate_N;                  //190212RLN
//std::clog << net_N_mineralization << std::endl;
   decomposition->net_N_mineralization = net_N_mineralization;
   decomposition->net_N_mineralization  = decomposing_N - assimilate_N;

   /*
   At this point free_N would be N associated with C released to CO2.
      I presume the freed N is released to the soil with no
      further limitation
      (it is always a positive number)

   assimilate_N_pot is the total potential N contributed to other pools
   (it is always a positive number)

   Under normal circumstances
   think the above would be equivelent to
      float64 free_N = C_fraction_transferred_to_CO2 * C_fraction_transferred_to;
      float64 contributing_N_pot = decomposing_N - free_N;

   However I don't think the C_fraction_transferred_to_CO2 been determined yet
   because it is not necessarily always constant for some types of organic
   matter or conditions.
   I.e. for multiple pool, the distribution to the receiver is dependent on
   on the current soil clay content.
   For single pool all SOM decomposed C is to CO2
   And in any case the intensive properties (i.e. fraction to CO2)
   would be merged when residue pools are combined in tillage operations.
   */

/*190311 now completely obsolete
   #else
   float64 net_transfer =  (CN_ratio_decomposer == 0.0)
   ? 0.0
   : 1.0 / carbon_nitrogen_ratio;

   for (int i = 1; i <=3; i++)
      if (receiver[i])
         net_transfer -= C_fraction_transferred_to[i] /
            get_humified_carbon_nitrogen_ratio(i);
            // For single pool, should be C/N of humified residue
            // for multiple pool, it is the carbon_nitrogen_ratio

      // RLN something is wrong with this because net_transfer is used as a ratio
      // but the subtractor would be N mass.

   Decomposition *decomposition = mod_decomposition();                           //111003
   decomposition->net_N_mineralization =  net_transfer * decomposition->pot_carbon;

   // decomposition->net_N_mineralization would appear to be always positive
   // at this point

   #endif
*/
   return true;
   //190212 return decomposition_pot->net_N_mineralization;
}
//_update_net_N_mineralization______________________________________2011-10-03_/
float64 Organic_biomatter_pool_common::get_N_mineralization_mass()         const
{  // Returns the actual N mass mineralized today
   const Decomposition *decomposition = ref_decomposition();
   return decomposition ? decomposition->N_mineralization : 0.0;
}
//_get_N_mineralization_mass____________________________2001-10-04__2006-11-16_/
float64 Organic_biomatter_pool_common::get_N_immobilization_demand_mass() const
{  // Returns the actual N mass immobilized  today
   const Decomposition *decomposition = ref_decomposition();
   return decomposition ? decomposition->N_immobilization_demand : 0.0;
}
//_get_N_immobilization_demand_mass_____________________2001-10-04__2006-11-16_/
void  Organic_biomatter_common::write_header(std::ostream &stream)  performs_IO_
{
   std::string units_buffer;                                                     //170423
   CORN::Units_clad units(units_ref);                                            //150227
   stream
   << "type\t"
   << "position\t"
   << "layer\t"
   << "detrition\t"
   // The following are Biomass_abstract and superclasses
   // I dont currently have read/write for these classes
   << "C_fraction\t"
   << "C/N\t"
   << "decomposition\t"
   << "amount(" <<units.label_string(units_buffer)<<")\t";                       //170423
   //170423 << "amount(" <<units.label_cstr(units_buffer)<<")\t";
}
//_write_header_____________________________________________________2012-09-12_/
bool Organic_biomatter_common::write(std::ostream &stream)            stream_IO_
{  stream
   << ((nat16)type) << "\t"
   << ((nat16)position) << "\t"
   << ((nat16)layer) << "\t"
   << detrition_constant << "\t"
   // The following are Biomass_abstract and superclasses
   // I dont currently have read/write for these classes
   << carbon_fraction << "\t"
   << carbon_nitrogen_ratio << "\t"
   << decomposition_constant << "\t"
   << get_amount() << "\t";                                                      //150227
   return true;                                                                  //150728
}
//_write____________________________________________________________2012-09-12_/
std::istream &Organic_biomatter_common::read(std::istream &stream)    stream_IO_
{
   nat16 type_buff;
   nat16 position_buff;
   nat16 layer_buff;
   stream
   >> type_buff
   >> position_buff
   >> layer_buff
   >> detrition_constant
   >> carbon_fraction
   >> carbon_nitrogen_ratio
   >> decomposition_constant
   >> (*amount_ptr);
   type     = (Organic_matter_type)type_buff;
   position = (Organic_matter_position)position_buff;
   layer    = (nat8)layer_buff;
   return stream;
}
//_read_____________________________________________________________2012-09-12_/
void  Organic_biomatter_pool_common::write_header(std::ostream &stream) stream_IO_
{  Organic_biomatter_common::write_header(stream);
   stream
   << "C_cycling\t"
   << "residue_type\t"
   << "area/mass\t";
}
//_write_header_____________________________________________________2012-09-12_/
bool Organic_biomatter_pool_common::write(std::ostream &stream)       stream_IO_
{  Organic_biomatter_common::write(stream);
   stream
   << ((nat16)carbon_cycling) << "\t"
   << ((nat16)RUSLE2_residue_type) << "\t"
   << area_per_mass << "\t";
   // dont write transfers and receivers
   return true;
}
//_write________________________________________________2015-07-28__2012-09-12_/
std::istream &Organic_biomatter_pool_common::read(std::istream &stream) stream_IO_
{
   Organic_biomatter_common::read(stream);
   nat16 carbon_cycling_buff;
   nat16 RUSLE2_residue_type_buff;
   stream
   >>carbon_cycling_buff
   >>RUSLE2_residue_type_buff
   >>area_per_mass;
   // dont read transfers and receivers, these need to be reconnected after reading
   carbon_cycling = (Organic_matter_cycling)carbon_cycling_buff;
   RUSLE2_residue_type = (RUSLE2_Residue_type)RUSLE2_residue_type_buff;
   return stream;
}
//_read_____________________________________________________________2012-09-12_/

