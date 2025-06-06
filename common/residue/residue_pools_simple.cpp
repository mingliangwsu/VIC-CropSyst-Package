#include "common/residue/residue_pools_simple.h"
#  include <math.h>
#ifndef compareHPP
#  include "corn/math/compare.hpp"
#endif

#if (CROPSYST_VERSION < 5)
// 070627 In version 5 we will only have NRCS soil SCS tillage will be obsolete
#  include "cs_till.h"
#endif
#  include "soil/soil_I.h"
#  ifdef NITROGEN
#     include "organicn.h"
#  endif
#  include "corn/math/moremath.h"

#include "soil/chemical_profile.h"
#include "soil/chemicals_profile.h"

CORN::statistical::Descriptive_progressive  residue_running_balance;             //200701
   // 090407 was in residues_common but is only used in V4.1
                                           // eventually rename this to residue_running_balance_41
#include "common/residue/residue_decomposition_param.h"
#include "common/biomatter/decomposition_const.h"

//130711 we dont bother with the detailed balance check
#include "common/residue/residue_balancer.h"

#include "common/residue/residue_pool_simple.h"

class Residue_pool;                                                              //181207

using namespace std;

#define TL_Residue "Residue"
#define TL_Delete  "Delete"

Residue_pool *debug_track_straw = 0;
// this points to the first allocated straw pool
// used to track decomposition when debugging

// I not keep a pointer in the Residue_pools to the detail log which can be optional.
//______________________________________________________________________________
Residue_pools_simple::Residue_pools_simple
(Soil::Soil_interface                     &soil_
,modifiable_ CORN::Dynamic_array<float64> &evaporation_potential_remaining_      //160719
,const Air_temperature_average      &temperature_air_avg_                        //150427
,const Wind_speed                   &wind_speed_                                 //150427
,Organic_matter_simple              *organic_matter_pool_
,const CORN::date32                 &today_                                      //170523
,std::ostream                       *detail_log_)
:Residues_common
   (soil_
   ,evaporation_potential_remaining_                                             //160412
   ,temperature_air_avg_,wind_speed_/*181217 ,today_,detail_log_*/)
// mineralization and immobilization is now done only in organic_matter class   (this entire function is not needed)  050118

#ifdef NITROGEN
,total_N_immobilization                                                    (0.0)
#endif
,organic_matter_pool                                      (organic_matter_pool_)
,pool_list                                                                    ()
{
   mineralization_total[0] = mineralization_total[1] = 0;                        //190124
}
//_Residue_pools_simple:constructor______________________________________1997?_/
bool Residue_pools_simple::clear_totals()
{  for (nat8 i = 0; i < OM_POSITION_COUNT ; i++)
      total_fract_ground_cover[i] = 0.0;
   for (nat8 /*Organic_matter_position*/ p = OM_subsurface_position; p < OM_POSITION_COUNT; p++)  //050115
   {  total_surface_mass[p] = 0.0;
      for (nat8 /*Organic_matter_type*/ t = STRAW_RESIDUE; t < RESIDUE_TYPE_COUNT; t++)
      {  biomass_output[p][t] = 0.0;
         N_output[p][t] = 0.0;
      }
   }
   return true;                                                                  //160126
}
//_Residue_pools_simple::clear_totals_______________________________2006-02-28_/
bool Residue_pools_simple::clear_all()                             modification_
{
//190124   total_straw_mineralization = 0.0;
//190124   total_manure_mineralization=0.0;
   mineralization_total[0] = mineralization_total[1] = 0;                        //190124
   total_N_immobilization = 0.0;
   pool_list.delete_all();
   bool cleared
      =  total_surface_biomatter.clear()
      && total_subsurface_biomatter.clear()
      && total_all_biomatter.clear()
      && residue_balancer.reset();                                               //030628
   return cleared && clear_totals();
}
//_Residue_pools_simple::clear_all__________________________________2016-01-26_/
void Residue_pools_simple::update_totals_all()
{  Residues_common::update_totals_all();
   total_surface_biomatter.clear();
   total_subsurface_biomatter.clear();
   total_all_biomatter.clear();

/*190327 pools now take a reference to contact fraction
   FOR_EACH_IN(curr_pool2,Residue_pool_simple,pool_list,each_pool2)
   {  Biomatter_residue_V41 *surface= curr_pool2->get_biomatter_profile().mod_surface(); //180110
      if (surface)
         surface->know_contact_fraction(flat_surface_contact_fraction);  // Apparently all pools get the same contact fraction
   } FOR_EACH_END(each_pool2)
*/
}
//_Residue_pools_simple::update_totals_all______________2006-02-28__2001-10-05_/
void Residue_pools_simple::update_totals_sum_pools()
{  FOR_EACH_IN(curr_pool,Residue_pool_simple ,pool_list,each_pool)               //030707
   {
      Organic_matter_type  residue_type = curr_pool->type;                       //190124
      /*190124
         = curr_pool->is_manure ? MANURE_RESIDUE : STRAW_RESIDUE;                //060228
      */
      Organic_matter_position     surface_position =  (int)(curr_pool->get_biomatter_profile().is_standing_stubble()) ? OM_stubble_surface_position: OM_flat_surface_position;
      float64 surface_N    = 0;                                                  //050118
      float64 incorp_N     = 0;                                                  //050118
      float64 all_N        = 0;                                                  //050118
      float64 surface_mass = curr_pool->get_biomatter_profile().totalize(total_surface_biomatter     ,SURFACE_HORIZON,horizons,surface_N); //011005
      float64 incorp_mass  = curr_pool->get_biomatter_profile().totalize(total_subsurface_biomatter  ,ALL_SUBSURFACE_HORIZON,horizons,incorp_N); //011005_970418
      /* float64 all_mass=*/ curr_pool->get_biomatter_profile().totalize(total_all_biomatter         ,ALL_HORIZON,horizons,all_N);  //020403_970418P
      // I dont currently need all_mass but we must totalize
      biomass_output[surface_position]       [residue_type] += surface_mass;     //050116
      biomass_output[OM_subsurface_position ][residue_type] += incorp_mass;      //050116
      N_output[surface_position]             [residue_type] += surface_N;        //050116
      N_output[OM_subsurface_position ]      [residue_type] += incorp_N;         //050116
      // DOES not account for overlapping straws.
      float64 pool_surface_mass = curr_pool->get_biomatter_profile().get_surface_mass(); //060302
      total_surface_mass[surface_position] += pool_surface_mass;                 //060302
      float64 pool_area_per_mass =curr_pool->area_per_mass;                      //060302
      float64 pool_residue_area_index = pool_surface_mass * pool_area_per_mass;  //060302
      total_residue_area_index[surface_position] += pool_residue_area_index;     //060228
      total_residue_area_index_both_flat_and_standing += pool_residue_area_index;//000609
   } FOR_EACH_END(each_pool)
}
//__Residue_pools_simple::update_totals_sum_pools______________________________/
void Residue_pools_simple::balance(bool run_nitrogen)
{  update_totals_all();
   residue_balancer.balance(total_all_biomatter,false); //note currently disabled show balance errors eventually enable when I hook up OM balance 011005_
}
//_Residue_pools_simple::balance____________________________________1998-07-18_/
void Biomatter_residue_V41::exhaust()
{
#ifdef NEW_RESIDUE_BALANCE
#else
// didn't have this in balance?
#endif
   clear();
}
//_Biomatter_residue_V41::exhaust___________________________________2004-05-23_/
void Residue_pools_simple::exhaust_all()
{
   #ifdef NEW_RESIDUE_BALANCE
   float32 pools_balance = 0.0;                                                  //040523
   #endif
   Unidirectional_list  continue_pool_list;                                      //040823
   for (Residue_pool_simple *curr_pool = dynamic_cast<Residue_pool_simple *>
         (pool_list.pop_at_head())                                               //040823
       ; curr_pool
       ; curr_pool = dynamic_cast<Residue_pool_simple *>(pool_list.pop_at_head()))
   {  Biomatter_residue_V41 total;
      float64 total_N_unused;                                                    //040823
      float64 total_biomass = curr_pool->get_biomatter_profile().totalize
         (total,ALL_HORIZON,horizons,total_N_unused);                            //050115
      //040523 We need to balance here because after this point, the
      // pools are deleted and we won't be able to get the balance
      //#ifdef NEW_RESIDUE_BALANCE
      ///*040523_*/           pools_balance = curr_pool->balance();
      //#endif
      if (CORN::is_zero<float64>(total_biomass, 0.00001))                        //200214_040128
      {
         curr_pool->exhaust();                                                   //040523

         /*170525 reimplement using explaination


         if (detail_log)                                                         //020307
            (*detail_log) << today.as_string() << '(' << today.get_DOY() <<')' << ':' << TL_Residue <<'-' << TL_Delete << endl;


         */

         delete curr_pool;                                                       //040822
       } else
         continue_pool_list.append(curr_pool);                                   //040823
   }
   pool_list.transfer_all_from(continue_pool_list);                              //040823
   update_totals_all();
#ifdef NEW_RESIDUE_BALANCE
   residue_running_balance.append(pools_balance);

   float64 clean_balance = pools_balance;
   float64 allowable_error = 0.001; //  * applied;
   if (CORN::is_zero<float64>(clean_balance,allowable_error))                    //200214
      clean_balance = 0.0;
#ifdef APPTYPE_CONSOLE
   if ((clean_balance != 0.0))
          cerr << "exhaust residue balance error:" << clean_balance << endl;
#endif
#endif
}
//_Residue_pools_simple:exhaust_all____________________________________________/
void Residue_pool_simple::redistribute
(float64 fract_shallow_to_deeps                                                  //011004
,float64 fract_surface_to_deeps                                                  //980924
,float64 fract_surface_to_shallow
,float64 shallow_horizon_thickness                                               //990217
,float64 deep_horizon_thickness                                                  //990217
,const nat8 horizons[])                                                          //9702xx
{  Residue_biomatter_profile_simple &biomatter_profile=get_biomatter_profile();  //060214
   biomatter_profile.redistribute                                                //990217
      (fract_shallow_to_deeps                                                    //011004
      ,fract_surface_to_deeps
      ,fract_surface_to_shallow
      ,shallow_horizon_thickness
      ,deep_horizon_thickness
      ,horizons);
   fract_ground_cover
      = 1.0 - exp(-area_per_mass * biomatter_profile.get_surface_mass());        //990217
}
//_redistribute________________________________________________________________/
void Residue_pools_simple::redistribute_all
(float64 fract_surface_to_surface
,float64 fract_surface_to_shallow)
{  if (!CORN::is_zero<float64>(fract_surface_to_shallow,0.0000001))              //200212_010516
   {  float64 fract_surface_to_deep
         = 1.0 - (fract_surface_to_surface + fract_surface_to_shallow);
      float64 fract_shallow_to_deep = (fract_surface_to_deep != 0.0)
      ? fract_surface_to_shallow / 2.0
      : 0.0;
      if (! CORN::is_approximately<float64>(fract_surface_to_surface,1.0,0.000001)) //980504
         FOR_EACH_IN(curr_pool,Residue_pool_simple,pool_list,each_pool)          //030707
         curr_pool->redistribute
         (fract_shallow_to_deep
         ,fract_surface_to_deep
         ,fract_surface_to_shallow
         ,shallow_horizon_thickness                                              //980217
         ,deep_horizon_thickness                                                 //980217
         ,horizons);
         FOR_EACH_END(each_pool)
   }
   update_totals_all();
}
//_redistribute_all____________________________________________________________/
#define N_conc_active_biomass(biomass) ( biomass * fraction_carbon_OM * (1/C_N_ratio) * active_OM_fraction)
//______________________________________________________________________________
void Residue_pools_simple::add_surface_plant_pool
(Organic_matter_position position                                                //040517
,Organic_matter_cycling _om_cycling                                              //060219
,float64 new_surface
#ifdef NITROGEN
,float64 new_N_surface_conc
#endif
,const Residue_decomposition_parameters &residue_decomp_params                   //060219
)
{
   bool standing_stubble                                                         //060530
      = (position == OM_stubble_surface_position)                                //171107
      ||(position == OM_attached_to_plant_position);                             //171107
   float64 new_decomposition_constant = residue_decomp_params.get_decomposition_constant(_om_cycling);         //060219
   float64 new_area_per_mass          = residue_decomp_params.get_area_covered_to_mass_ratio(standing_stubble);//060219
   float64 default_residue_CN_ratio   = residue_decomp_params.get_carbon_nitrogen_ratio(_om_cycling);  //060225
   bool has_straw = new_surface > 0.0;                                           //981124
   if (has_straw)                                                                //981124
   {
      soil_sublayer_array_64(new_subsurface_NA);  clear_sublayer_array(new_subsurface_NA);
      Residue_pool_simple *new_pool =                                            //980303
      create_straw_pool
      (standing_stubble                                                          //040517
      ,new_surface
      ,new_subsurface_NA // new_subsurface
#if (CROPSYST_VERSION < 5)
// 070118 becoming obsolete Claudio decided to alway be initialy 0.0 (as in V.B. code)
      ,0 // 090311 ,new_water_hold
#endif
#ifdef NITROGEN
      ,new_N_surface_conc
      ,new_N_surface_conc  // Not applicable for surface new_N_subsurface_conc
#endif
      ,new_decomposition_constant
      ,default_residue_CN_ratio                                                  //060225
      ,residue_decomp_params.carbon_fraction                                     //060227
      ,new_area_per_mass
      ,horizons
      ,soil                                                                      //060122
      );                                                                         //971231
      pool_list.prepend((Item *)new_pool);                                       //980718
      #ifdef DETAILED_BALANCES
      float64 total_N_unused;                                                    //050118
      Biomatter_residue_V41 total_surface/*(1.0)*/;                              //011021
      //130711 we dont bother with the detailed balance check (actually we should have a flag if we want the residue balance at all
      if (residue_mass_balance[position])                                        //090408
          residue_mass_balance[position]->added += new_pool->get_biomatter_profile()
            .totalize(total_surface,ALL_HORIZON,horizons,total_N_unused); //
      #endif
      // Note that although ALL_HORIZON is selected the pool is actually only surface (so it is sort of a sanity check).
  }
}
//_residue_decomp_params_______________________________________________________/
void Residue_pools_simple::add_subsurface_plant_pool
(soil_sublayer_array_64(new_subsurface)                                          //970421
#ifdef NITROGEN
,float64 new_N_subsurface_conc
#endif
,const Residue_decomposition_parameters &residue_decomp_params)                  //060219
{
   Organic_matter_cycling _om_cycling = OM_ORIGINAL_CYCLING;  // Was a parameter
   float64 new_decomposition_constant = residue_decomp_params.get_decomposition_constant(_om_cycling);   //060219
   float64 new_area_per_mass          = residue_decomp_params.get_area_covered_to_mass_ratio(false);     //060219
   float64 default_residue_CN_ratio   = residue_decomp_params.get_carbon_nitrogen_ratio(_om_cycling);    //060225
   bool has_straw = false;                                                       //981124
   for (int8 sublayer =  1; sublayer <= MAX_soil_sublayers; sublayer++)          //981124
         if ((new_subsurface[sublayer] > 0.000001)                               //981124
             && (horizons[sublayer] > NULL_HORIZON))                             //991204
            has_straw = true;                                                    //981124
  if (has_straw)                                                                 //981124
  {   Residue_pool_simple *new_pool =                                            //980303
      create_straw_pool
      (false                                                                     //040517
      ,0.0 // no surface
      ,new_subsurface
#if (CROPSYST_VERSION < 5)
      ,0.0 // 060530 now not applicable (for subsurface) new_water_hold
#endif
#ifdef NITROGEN
      ,new_N_subsurface_conc  // 060530 now  Not applicable new_N_surface_conc
      ,new_N_subsurface_conc
#endif
      ,new_decomposition_constant
      ,default_residue_CN_ratio                                                  //060225
      ,residue_decomp_params.carbon_fraction                                     //060227
      ,new_area_per_mass
      ,horizons
      ,soil                                                                      //060122
      );                                                                         //971231
      pool_list.prepend((Item *)new_pool);                                       //980718
      #ifdef DETAILED_BALANCES
      Biomatter_residue_V41 total_all_horizons/*(1.0)*/;                         //011021
      float64 total_N_unused;                                                    //050118
      float64 total_mass_all_horizons = new_pool->get_biomatter_profile()
         .totalize(total_all_horizons,ALL_HORIZON,horizons,total_N_unused);      //011021
      // although I total ALL_HORIZON, the new_pool will actually have only subsurface (sort of a sanity) check.
      //130711 we dont bother with the detailed balance check (actually we should have a flag if we want the residue balance at all
      if (residue_mass_balance[OM_subsurface_position])                          //090408
          residue_mass_balance[OM_subsurface_position]->added += total_mass_all_horizons; //090408
      #endif
  }
}
//_add_subsurface_plant_pool___________________________________________________/
#ifdef NITROGEN
#if ((CROPSYST_VERSION > 0) && (CROPSYST_VERSION < 5))
void Residue_pools_simple::add_manure_pool_original
(Organic_matter_source _org_N_source                                             //120316
,float64 _shallow_N_content
,float64 _deep_N_content
,float64 _decomposition_constant                                                 //060122
,float64 _default_carbon_nitrogen_ratio                                          //060225
   // Used when we dont have a N mass (or for initial residue)
,float64 _carbon_fraction                                                        //080225
,float64 _area_per_mass)
{
//NYI I will need to add manure cycling parameters
/* resinue/manure water is now stored as depth (mass)
    curr_water_hold += manure_water_hold;
*/
    Residue_pool_simple *new_pool =
      create_manure_pool                                                         //060219
      (_org_N_source
      ,_shallow_N_content
      ,_deep_N_content
      ,_decomposition_constant                                                   //060122
      ,_carbon_fraction                                                          //080225
      ,_default_carbon_nitrogen_ratio   // Used when we dont have a N mass (or for initial residue)
      ,_area_per_mass
      ,horizons);

   float64 manure_water = new_pool->get_surface_mass() * manure_water_hold;      //170104
std::clog << "check manure water to surface" << std::endl;
   take_pool_water(manure_water);                                                //170104

   pool_list.prepend((Item *)new_pool);                                          //971231
   Biomatter_residue_V41 total_all_horizons;                                     //011021
   float64 total_N_unused;                                                       //050118
   new_pool->get_biomatter_profile().totalize
      (total_all_horizons,ALL_HORIZON,horizons,total_N_unused);                  //011021
      // I dont currently need mass_all_horizons but we must totalize
}
#endif
#endif
//_add_manure_pool_original____________________________________________________/
bool Biomatter_residue_V41::clear()                                modification_
{  multiply_by(0);
   return true;
}
//_clear____________________________________________________________2003-09-18_/
void Residue_pools_simple::recalibrate_surface_residue
(float64 surface_residue_param
,float64 incorp_residue_param                                                    //010202
,const Residue_decomposition_parameters &residue_decomposition_params            //060219
,float64 residue_N_conc_param
)
{  total_surface_biomatter.clear();                                              //011011
   total_subsurface_biomatter.clear();                                           //011011
   for (nat8 i = OM_subsurface_position; i <OM_POSITION_COUNT; i++)
      total_fract_ground_cover[i]  = 0.0;
   evaporation_actual.clear();                                                   //160719
   evaporation_potential.clear();                                                //160719
   curr_water_hold        = 0.0;// m3/kg
   water_storage          = 0.0;                                                 //170104
   interception           = 0.0;// m
   pool_list.delete_all();
   { soil_sublayer_array_64(subsurface_residue);
     clear_sublayer_array(subsurface_residue);
     for (int sl = 0; sl <= MAX_soil_sublayers; sl++)
         subsurface_residue[sl] = 0;
     subsurface_residue[2] = incorp_residue_param;                               //010202
     add_surface_plant_pool
        (OM_flat_surface_position // false   // Currently assuming initial surface residue is laying flat (compatibility with previous versions) //040517
        ,OM_ORIGINAL_CYCLING
        ,surface_residue_param                                                   //010202
        ,residue_N_conc_param
      ,residue_decomposition_params);                                            //060219
     add_subsurface_plant_pool
        (subsurface_residue                                                      //040517
        ,residue_N_conc_param
        ,residue_decomposition_params);                                          //060219
  }
}
//_recalibrate_surface_residue______________________________________2000-03-12_/
Biomatter_residue_V41 *Residue_pool_simple::remove_from_surface(float64 fract_to_remove)
{  Biomatter_residue_V41 *surface_removed = 0;
   Residue_biomatter_profile_simple &biomatter_profile = get_biomatter_profile();//060214
   Biomatter_residue_V41 *surface= biomatter_profile.mod_surface();              //180110_060214
   if (surface)                                                                  //040820
   {
      #ifdef POOL_MASS_BALANCE
      surface_removed = (Biomatter_residue *) surface->extract_fraction(fract_to_remove);
      surface->mass_balance.removed += surface_removed.get_weight(basis);        //040523
      #else
      surface_removed =                                                          //050214
         dynamic_cast<Biomatter_residue_V41 *>(surface->extract_fraction(fract_to_remove));
      #endif
   }
   return surface_removed;                                                       //050303
}
//_remove_from_surface______________________________________________2000-05-01_/
float64 Residue_pools_simple::remove_from_surface
(float64    fract_to_remove // 0-1
#if ((CROPSYST_VERSION > 0) && (CROPSYST_VERSION < 5))
,int16      SCS_code   //070627 will be obsolete in version 5
#endif
)
{  // This method may return null if not usable biomass is available for disposal
   // or disposal method
   if (CORN::is_zero<float64>(fract_to_remove,0.00001))                          //200214
      return 0;
   Biomatter_residue_V41 *removed_biomass = 0;
   FOR_EACH_IN(curr_pool,Residue_pool_simple,pool_list,each_pool)                //030707
   { //  for each pool remove and accumulate to removed biomass pool.
      Biomatter_residue_V41 *biomass_removed_from_pool
            = curr_pool->remove_from_surface(fract_to_remove);                   //040523
      if (biomass_removed_from_pool)                                             //050303
      {  if (!removed_biomass) removed_biomass = biomass_removed_from_pool;      //050303
         else  removed_biomass->add(*biomass_removed_from_pool);                 //050303
      }                                                                          //050303
   } FOR_EACH_END(each_pool)
   //To clean up balance:
   /*190327 shouldn't be needed because deleting
   if (removed_biomass)                                                          //060616
       removed_biomass->know_contact_fraction(subsurface_contact_fraction);      //190327040818
       //190327 removed_biomass->set_contact_fraction(DEFAULT_SURFACE_ORGANIC_RESIDUE_CONTACT_FRACTION);  //040818
   */
   update_totals_all();
   float64 removed_biomass_amount = removed_biomass ? removed_biomass->kg() : 0.0; //060122
   // Some residues are eliminated or nothing collected to be disposed of.
#if ((CROPSYST_VERSION > 0) && (CROPSYST_VERSION < 5))
// 070627 In version 5 this will be obsolete
   if (  (SCS_code == SCS_residue_flail_chop_bust)  // <- 060122 RLN not sure why flail was deleting when it could be going to residue?
                                                    // Now that we have stubble residue type, this should send stubble to surface, reather than remove
       ||(SCS_code == SCS_residue_burning))
   {  if (removed_biomass)  delete removed_biomass; removed_biomass = 0;}
   // In the future we may want to do something with this biomass
   // (when it is not burned
   // but currently it is just disposed.
#endif
   if (removed_biomass) delete removed_biomass;                                  //060122
   return removed_biomass_amount;
}
//_remove_from_surface______________________________________________2000-05-01_/
float64 Residue_pools_simple::get_subsurface_plant_biomass
(soil_sublayer_array_64(plant_residues))
{  float64 total = 0;
   FOR_EACH_IN(residue_pool,Residue_pool_simple,pool_list,each_pool)             //030707
   {  for (nat8 sublayer = 1; sublayer <= MAX_soil_sublayers; sublayer++)
         if (residue_pool->type != MANURE_RESIDUE)
         {  Residue_biomatter_profile_simple &biomatter_profile
               = residue_pool->get_biomatter_profile();
            if (plant_residues)
               plant_residues[sublayer] += biomatter_profile.get_subsurface_mass(sublayer);
            total += biomatter_profile.get_subsurface_mass(sublayer);
         }
   } FOR_EACH_END(each_pool)
   return total;
}
//_get_subsurface_plant_biomass_____________________________________2000-08-10_/
float64 Residue_pools_simple::get_subsurface_manure_biomass
(soil_sublayer_array_64(manure_residues))
{  float64 total = 0;
   FOR_EACH_IN(residue_pool,Residue_pool_simple,pool_list,each_pool)             //030707
   {  for (nat8 sublayer = 1; sublayer <= MAX_soil_sublayers; sublayer++)
         if (residue_pool->type == MANURE_RESIDUE)
         {  Residue_biomatter_profile_simple &biomatter_profile
               = residue_pool->get_biomatter_profile();                          //060214
            if (manure_residues)
               manure_residues[sublayer] += biomatter_profile.get_subsurface_mass(sublayer);
            total += biomatter_profile.get_subsurface_mass(sublayer);
         }
   } FOR_EACH_END(each_pool)
   return total;
}
//_get_subsurface_manure_biomass____________________________________2000-08-10_/
Biomatter_residue_V41::Biomatter_residue_V41()
: Biomass_abstract()                                                             //101012
, N_content(0)                                                                   //030301
{} ;
//_Biomatter_residue_V41:constructor___________________________________________/
Biomatter_residue_V41::Biomatter_residue_V41(const Biomatter_residue_V41 &from_copy)
: Biomass_abstract(from_copy)                                                    //101012
, N_content(from_copy.N_content)
{}
//_Biomatter_residue_V41:copy_constructor________________________________2004-08-20_
float64 Biomatter_residue_V41::add(const Physical::Mass &addend)   modification_
{  const Biomass *addend_as_same_type = dynamic_cast<const Biomass *>(&addend);
   if (addend_as_same_type)
       N_content += addend_as_same_type->get_nitrogen_organic();
   return Biomass_abstract::add(addend);
}
//_add______________________________________________________________2010-10-12_/
float64 Biomatter_residue_V41::multiply_by(float64 multiplier)     modification_
{  N_content *= multiplier;
   return Biomass_abstract::multiply_by(multiplier);
}
//_multiply_by______________________________________________________2010-10-12_/
float64 Residue_pools_simple::sum_carbon_to_sublayer_kg_m2(nat8 to_sublayer) const
{  Biomatter_residue_V41 sum_biomatter_residue;
   FOR_EACH_IN(residue_pool,Residue_pool_simple,pool_list,each_pool)
   {  residue_pool->get_biomatter_profile().sum_subsurface_to_layer
         (sum_biomatter_residue,to_sublayer);
   } FOR_EACH_END(each_pool)
   float64 sum = sum_biomatter_residue.kg() * default_straw_residue_fraction_carbon; //061101
   return sum;
}
//_sum_carbon_to_sublayer_kg_m2_____________________________________2004-02-06_/
float64 Residue_pools_simple::sum_carbon_surface()                         const
{  float64 sum = total_surface_biomatter.kg()
                * default_straw_residue_fraction_carbon;                         //061101
   return sum;
}
//_sum_carbon_surface_______________________________________________2006-02-24_/
float64 Residue_pools_simple::get_biomass_output
(nat32 include_positions ,nat32  include_residue_types)                    const
{  float32 total = 0;                                                            //060328
   for (nat8 p = 0; p < OM_POSITION_COUNT; p++)                                  //060328
      for (nat8 t = STRAW_RESIDUE; t <= MANURE_RESIDUE; t++)                     //060328
         if (((1 << p) & include_positions) && ((1<<t) & include_residue_types)) //060601
            total += biomass_output[p][t];                                       //060328
   return total;                                                                 //060328
}
//_get_biomass_output_______________________________________________2005-01-16_/
float64 Residue_pools_simple::get_N_output
(nat32 include_positions, nat32 include_residue_types)                     const //060328
{  float32 total = 0;                                                            //060328
   for (nat8 p = 0; p < OM_POSITION_COUNT; p++)                                  //060328
      for (nat8 t = STRAW_RESIDUE; t <= MANURE_RESIDUE; t++)                     //060328
         if (((1 << p) & include_positions) && ((1<<t) & include_residue_types)) //060601
            total += N_output[p][t];                                             //060328
   return total;                                                                 //060328
}
//_get_N_output_____________________________________________________2005-01-16_/
Residue_pool_simple *Residue_pools_simple::create_straw_pool
( bool    standing_stubble                                                       //040517
,float64 new_surface
,soil_sublayer_array_64(new_subsurface)                                          //970421
#if (CROPSYST_VERSION < 5)
,float64 new_water_hold     // m3/kg
#endif
,float64 new_N_surface_conc
,float64 new_N_subsurface_conc
,float64 new_decomposition_constant                                              //060122
,float64 _default_CN_ratio                                                       //060225
,float64 _residue_carbon_fraction                                                //060227
,float64 new_area_per_mass
,nat8 horizons[]
,Soil::Soil_interface &_soil)  // Note, I think soil reference is already a member so this may be superfluous
{  Residue_pool_simple *straw_pool =                                             //060227
   new   Residue_pool_simple  (standing_stubble                                  //040517
      ,new_surface
      ,new_subsurface
      #if (CROPSYST_VERSION < 5)
      ,new_water_hold
      #endif
      #ifdef NITROGEN
      ,new_N_surface_conc
      ,new_N_subsurface_conc
      #endif
      ,new_decomposition_constant                                                //060122
      ,new_area_per_mass
      ,horizons
      ,_soil
      ,temperature_air_avg                                                       //150427
      );
    return straw_pool;
}
//_create_straw_pool___________________________________________________________/
Residue_pool_simple *Residue_pools_simple::create_manure_pool
(Organic_matter_source _OM_source                                                //120316
,float64 _shallow_N_content
,float64 _deep_N_content
,float64 _decomposition_constant                                                 //060122
,float64 _default_carbon_nitrogen_ratio   // Used when we dont have a N mass (or for initial residue) //060225
,float64 _carbon_fraction_UNUSED // about 0.5 Not used in this model             //080225
,float64 _area_per_mass
,nat8 horizons[])
{  return new Residue_pool_simple
     (_OM_source
     ,_shallow_N_content
     ,_deep_N_content
     ,_decomposition_constant                                                    //060122
     ,_area_per_mass
     ,horizons
     ,soil
     ,temperature_air_avg);                                                      //150427
}
//_create_manure_pool__________________________________________________________/
void Residue_pools_simple::process_all()
{  decompose_all();
   exhaust_all();
}
//_process_all_________________________________________________________________/
void Residue_pools_simple::decompose_all()
{
   float64 lignin_halflife = 72.433;                                //060123
      // days for 50% decomposition
   float64 lignin_decomposition_constant
      = convert_halflife_to_constant(lignin_halflife);
   #ifdef NITROGEN
   mineralization_total[0] = mineralization_total[1] = 0;                        //190124
   //190124total_manure_mineralization = 0.0;
   //190124total_straw_mineralization = 0.0;
   total_N_immobilization = 0.0;
   #endif
   FOR_EACH_IN(curr_pool,Residue_pool_simple,pool_list,each_pool)                //030707
   {
      float64 decomposed_biomass_recalculated_from_C = 0.0;                      //040207
      float64 residue_decomposed = curr_pool->decompose                          //980718
        (curr_water_hold                                                         //060116
        ,decomposed_biomass_recalculated_from_C                                  //040207
         #ifdef NITROGEN
         ,organic_matter_pool
         #endif
         );
      residue_balancer.add_DM_decomposed_carbon(residue_decomposed);             //040207
#ifdef NITROGEN
      mineralization_total[curr_pool->type] += curr_pool->mineralization;        //190124
      /*190124
      if (curr_pool->is_manure)
           total_manure_mineralization += curr_pool->mineralization;
      else total_straw_mineralization  += curr_pool->mineralization;
      */
#endif
      Biomatter_residue_V41 temp_all_totals;                                     //011005
      float64 N_this_pool = 0.0;                                                 //050209
      Residue_biomatter_profile_simple &biomatter_profile = curr_pool->get_biomatter_profile(); //060214
      biomatter_profile.totalize(temp_all_totals,ALL_HORIZON,horizons,N_this_pool);             //050209
      float64 biomass_in_all_horizons_kg_m2 = temp_all_totals.kg()  ;            //020719
      float64 pool_init_biomass_kg_m2 = biomatter_profile.get_initial().kg()  ;  //020719
      if (pool_init_biomass_kg_m2 > 0.0)                                         //041212
      {  // avoid div0 error, could be 0 if very low residue biomass fate        //041212
         float64 fraction_of_initial_biomass
            = (biomass_in_all_horizons_kg_m2 / pool_init_biomass_kg_m2);         //020719
         if ((fraction_of_initial_biomass < 0.10)                                //990217
#ifdef NITROGEN
                  && (curr_pool->type != MANURE_RESIDUE) //190124 ! curr_pool->is_manure
#endif
            )
            curr_pool->decomposition_constant = std::max<float64>
              (curr_pool->decomposition_constant,lignin_decomposition_constant); //060123
      }
#ifdef NITROGEN
      total_N_immobilization += curr_pool->N_immobilization;
#endif
   } FOR_EACH_END(each_pool)
#ifdef NITROGEN
   residue_balancer.add_N_mineralized
     (mineralization_total[MANURE_RESIDUE]+mineralization_total[STRAW_RESIDUE]);
      //190124 (total_manure_mineralization + total_straw_mineralization);
   residue_balancer.add_N_immobilized(total_N_immobilization);
#endif
   update_totals_all();
} //decompose residues
//_decompose_all_______________________________________________________________/
void Residue_balancer_V41::balance
(const Biomatter_residue_V41 &current_BM                                         //011005
, bool show_balance_errors)                                                      //050725
{  float64 mass_applied    = BM_applied.kg()  ;                                  //031130
   float64 mass_exhausted  = BM_exhausted.kg()  ;                                //031130
   float64 mass_removed    = BM_removed.kg()  ;                                  //031130
   float64 mass_current    = current_BM.kg()  ;                                  //031130
   float64 residue_biomass_balance =  // 0 initial biomass
      + mass_applied
      - DM_decomposed_carbon  - mass_exhausted - mass_removed - mass_current;    //040207
   float64 allowable_BM_error = 0.001 * BM_applied.kg()  ;                       //021216
   if (CORN::is_zero<float64>(residue_biomass_balance,allowable_BM_error))       //021216
      residue_biomass_balance = 0.0;
   if (show_balance_errors)
   {
#ifdef APPTYPE_CONSOLE
       if ((residue_biomass_balance != 0.0) && !CORN::is_approximately<float64>
           (residue_biomass_balance,residue_biomass_balance_yesterday,0.00001))
           cerr << "residue balance error:" << residue_biomass_balance << endl;
#endif
      residue_biomass_balance_yesterday = residue_biomass_balance;               //040128
#ifdef NITROGEN
   {
      float64 N_applied   = BM_applied.get_N_content();                          //031130
      float64 N_exhausted = BM_exhausted.get_N_content();                        //031130
      float64 N_removed   = BM_removed.get_N_content();                          //031130
      float64 N_current   = current_BM.get_N_content(); // current N of all pools//031130
      float64 residue_N_balance =
      //  0 initial N
         + N_applied // BM_applied.get_N_content() //obs N_applied
         + N_immobilized                                                         //040207
         - N_mineralized
         - N_OM
         - N_to_atmosphere                                                       //980908
         - N_exhausted
         - N_removed                                                             //000501
         - N_current;   // current N of all pools
      float64 allowable_N_error = 0.001 * BM_applied.get_N_content();            //021212
      if (CORN::is_zero<float64>(residue_N_balance,allowable_N_error))           //200212_021212
         residue_N_balance = 0.0;
#ifdef APPTYPE_CONSOLE
      if ((residue_N_balance != 0.0)&& !CORN::is_approximately<float64>
          (residue_N_balance,residue_N_balance_yesterday,0.0001))
         cerr << endl << "residue N balance error:" << residue_N_balance << endl ;
#endif
      residue_N_balance_yesterday = residue_N_balance;                           //040128
   }
#endif
   } // endif show_balance_errors
}
//_balance_____________________________________________________________________/
//050722 This is still being used, except with compiling for new organic matter model.
bool Residue_balancer_V41::reset()                                               //160126
{  BM_applied.clear();                                                           //011005
   BM_exhausted.clear();                                                         //011005
   BM_removed.clear();                                                           //011005
   DM_decomposed_carbon = 0.0;                                                   //040207
#ifdef NITROGEN
   N_mineralized =0.0;
   N_OM =0.0;
   N_to_atmosphere =0.0;
   N_immobilized = 0.0;
#endif
   return true;
}
//_reset_______________________________________________________________________/

