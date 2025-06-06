#include "organic_matter/simple/OM_residues_profile_simple_V4.h"
#if ((CS_VERSION > 0) && (CS_VERSION <= 4))
//200815#include "cs_chem_pot_mineral_N.h"
#include "organic_matter/simple/cs_chem_pot_mineral_N_V4.h"
#ifndef compareHPP
#  include "corn/math/compare.hpp"
#endif

#include "soil/soil_I.h"
#include "soil/nitrogen_I.h"

#include "soil/structure_I.h"
#include "corn/measure/measures.h"
#include "organic_matter/simple/OM_simple.h"
#include "common/residue/residue_pool_simple.h"
#include "common/residue/residue_const.h"
#include "common/residue/residue_pools_simple.h"
#include "weather/parameter/WP_air_temperature.h"
//______________________________________________________________________________
Organic_matter_residues_profile_simple::Organic_matter_residues_profile_simple
(Soil::Soil_interface                    *soil_
#ifndef OLD_N_XFER
,Soil::Chemical_pool                     &receiver_NO3_                          //170502
#endif
,modifiable_ CORN::Dynamic_array<float64> &evaporation_potential_remaining_      //160720
,Infiltration_model                 infiltration_model // default CASCADE_MODEL  //080117
,soil_sublayer_array_64            (sublayer_percent_organic_matter_)
,bool                               constant_organic_matter_
,nat8                               trans_sublayers                              //060116
,float64                            V3_adjust_mineralize                         //060116
,bool                               enable_nitrogen
,const Air_temperature_average     &air_temperature_avg_                         //140507
,const Wind_speed                  &wind_speed_                                  //150427
,const CORN::date32                &simdate_raw_)                                //170524_140507
: Organic_matter_residues_profile_abstract(soil_,simdate_raw_)                   //140507_070707
, organic_matter        (0)                                                      //060116
, residue_pools_kg_m2   (0)                                                      //060116
, pot_mineralizable_N   (0)                                                      //060116
, air_temperature_avg   (air_temperature_avg_)                                   //140507
, wind_speed            (wind_speed_)
{  soil_sublayer_array_64  (initial_org_matter_biomass);
   clear_sublayer_array    (initial_org_matter_biomass);
   for (nat8 sl = 1 ; sl <= MAX_soil_sublayers; sl++)                            //040209
   {  float64 bulk_density_tonne_m3 =  // tonne/m3 is same as g/cm3              //051204
         soil_structure.get_bulk_density_g_cm3(sl);                              //070725
      initial_org_matter_biomass[sl] =                                           //040209
         (sublayer_percent_organic_matter_[sl]/100.0) *                          //051120
          soil_layers.get_thickness_m(sl) *                                      //040209
          tonne_to_kg(bulk_density_tonne_m3);                                    //040209
   }                                                                             //051204
   organic_matter       = new Organic_matter_simple                              //050726
      (initial_org_matter_biomass, constant_organic_matter_);
   residue_pools_kg_m2  = new Residue_pools_simple
      (*soil_
      ,evaporation_potential_remaining_                                          //160412
      ,air_temperature_avg,wind_speed, organic_matter,simdate_raw_);             //150427_981120
   if (enable_nitrogen)                                                          //060115
      pot_mineralizable_N = new Pot_mineralizable_N_profile
         (simdate_raw_
         ,trans_sublayers
         ,V3_adjust_mineralize // WARNING check if this is indeed mineralize
         #ifndef OLD_N_XFER
         ,receiver_NO3_                                                          //170502
         #endif
         ,*soil_                                                                 //060504
         ,infiltration_model                                                     //080117
         ,*organic_matter
         );
}
//_Organic_matter_residues_profile_simple:_____________________________________/
Organic_matter_residues_profile_simple::~Organic_matter_residues_profile_simple()
{  if (organic_matter)      delete organic_matter;        organic_matter = 0;
   if (residue_pools_kg_m2) delete residue_pools_kg_m2;   residue_pools_kg_m2=0; //060122
   if (pot_mineralizable_N) delete pot_mineralizable_N;   pot_mineralizable_N=0; //060515
}
//______________________________________________________________________________
const Residues_interface *Organic_matter_residues_profile_simple::ref_residues()
{  return residue_pools_kg_m2; }
//_ref_residues_____________________________________________________2016-07-20_/
float64 Organic_matter_residues_profile_simple::get_organic_matter_mass(nat8 layer)    const
{return organic_matter->get_biomass(layer) ; }
//_get_organic_matter_mass__________________________________________2006-04-01_/
bool Organic_matter_residues_profile_simple::process_day()         modification_ //150611RLN
{  bool result = true;
   residue_pools_kg_m2->process_all();
   return result;
}
//_process_day______________________________________________________2006-02-14_/
float64 Organic_matter_residues_profile_simple::get_organic_carbon(nat8 sublayer, nat8 include_OM )  const
{  // 060122 warning not sure how to use include_OM in this case (I think it may apply only to V4_2
   return organic_matter->get_organic_carbon(sublayer,soil_layers.get_thickness_m(sublayer));
}
//______________________________________________________________________________
float64 Organic_matter_residues_profile_simple::get_total_straw_mineralization() const
{  return residue_pools_kg_m2->
      get_mineralization_total(STRAW_RESIDUE);
}
//______________________________________________________________________________
float64 Organic_matter_residues_profile_simple::get_total_manure_mineralization() const
{  return residue_pools_kg_m2->
      get_mineralization_total(MANURE_RESIDUE);
}
//______________________________________________________________________________
float64 Organic_matter_residues_profile_simple::get_N_immobilization_profile() const
{  return residue_pools_kg_m2->get_total_N_immobilization();
}
//______________________________________________________________________________
float64 Organic_matter_residues_profile_simple::get_soil_organic_matter_mineralization_profile_N() const
{  // this is repeat of get_NH4_mineralization because get_soil_organic_matter_mi//neralization_profile is indented for V4.2 provided here for compatibility/completeness
   return soil_nitrogen                                                          //060616
      ? soil_nitrogen->get_NH4_N_mass_mineralized_from_PMN_kg_m2_profile()       //060123
      : 0; // Nitrogen simulation not enabled                                    //060616
}
//______________________________________________________________________________
float64 Organic_matter_residues_profile_simple::get_NH4_mineralization()   const
{  return soil_nitrogen
      ? soil_nitrogen->get_NH4_N_mass_mineralized_from_PMN_kg_m2_profile()
      : 0;  // Nitrogen simulation not enabled
}
//_get_NH4_mineralization___________________________________________2006-01-23_/
float64 Organic_matter_residues_profile_simple::get_soil_organic_matter_mineralization_N(nat8 layer) const
{  return soil_nitrogen
      ? soil_nitrogen->get_NH4_N_mass_mineralized_from_PMN_kg_m2_profile()
      : 0; // Nitrogen simulation not enabled
}
//_get_soil_organic_matter_mineralization_N_________________________2006-01-23_/
float64 Organic_matter_residues_profile_simple::sum_carbon_to_depth_kg_m2
(nat8   include_OM
,float64 to_depth_m     // To depth of soil
,bool    include_surface ) const
{  nat8 layer_at_depth = CORN::is_approximately<float64>(to_depth_m,ENTIRE_PROFILE_DEPTH_as_9999,0.0001)
      ? soil_layers.count()
      : soil_layers.get_layer_at_depth(to_depth_m);
   float64 residue_subsurface_carbon= residue_pools_kg_m2->sum_carbon_to_sublayer_kg_m2(layer_at_depth);
   float64 residue_surface_carbon   = 0.0;
   if (include_surface)
           residue_surface_carbon   = residue_pools_kg_m2->sum_carbon_surface();
   float64 organic_matter_carbon    = 0.0;
   for (nat8 layer = 1; layer <= layer_at_depth; layer++)
   {  float64 thickness_sl = soil_layers.get_thickness_m(layer);
      organic_matter_carbon += organic_matter->get_biomass(layer) * fraction_carbon_OM;
   }
   return residue_subsurface_carbon + residue_surface_carbon + organic_matter_carbon;
}
//_sum_carbon_to_depth_kg_m2________________________________________2006-01-23_/
#else
#error This is the original organic matter residues profile introduced in V3/V4. It is Obsolete in V5
#endif

