#include "residue_biomatter_profile_simple.h"
#include "common/residue/residue_pools_simple.h"
#include "common/biomatter/decomposition_const.h"
#include "soil/layers.h"
#include <math.h>
#ifndef compareHPP
#  include "corn/math/compare.hpp"
#endif
#include <assert.h>
//______________________________________________________________________________
Residue_biomatter_profile_simple::Residue_biomatter_profile_simple
(const Soil::Layers_interface &soil_layers_)                                     //150424
: standing_stubble                                                        (true) //090416
, surface                                                                    (0) //090416
, initial                                            (new Biomatter_residue_V41)
, soil_layers                                                     (soil_layers_)
{  for (nat8 layer = 0; layer <= MAX_soil_layers; layer++)
      subsurface[layer] = 0;
}
//_Residue_biomatter_profile_simple:constructor________________________________/
Residue_biomatter_profile_simple
::Residue_biomatter_profile_simple
(const Biomatter_residue_V41 subsurface_[]
,nat8 horizons[]
,const Soil::Layers_interface &soil_layers_)                                     //150424
: soil_layers(soil_layers_)                                                      //150424
{  for (nat8 layer = 0; layer <= MAX_soil_layers; layer++)                       //011011
   {  modifiable_ Biomatter_residue_V41 *subsurface_layer=mod_subsurface(layer); //060214
      if (subsurface_[layer].kg() && !subsurface_layer )                         //040820
         subsurface_layer  = create_biomatter_residue(subsurface_[layer]);       //040820
       // WARNING need to check if this needs to set the general properties of the organic matter.
       // May need to copy from initial and also make sure initial gets the general properties;
      else                                                                       //040820
      {  subsurface_layer ->add(subsurface_[layer]);
         //Note may want to move the set_xxxx() out of the if/else block because create_migh not set it
         subsurface_layer ->set_contact_fraction(subsurface_[layer].get_contact_fraction()); //040818
   }  }
   float64 total_N_unused;                                                       //050118
   totalize(get_initial() ,ALL_HORIZON,horizons,total_N_unused);                 //011021
}
//_Residue_biomatter_profile_simple____________________________________________/
Residue_biomatter_profile_simple
::Residue_biomatter_profile_simple
(bool    standing_stubble_                                                       //040517
//200220 ,float64  decomposition_time_63_obsolete                                         //040802
,float64  halflife_                                                  //040826
,float64 surface_dry_weight_
,soil_sublayer_array_64(subsurface_dry_weight_)
#ifdef NITROGEN
,float64 surface_N_content_
,soil_sublayer_array_64(subsurface_N_content_)
#endif
,nat8 horizons[]
,const Soil::Layers_interface &soil_layers_)                                     //150424
: standing_stubble                                           (standing_stubble_) //060214
, soil_layers                                                     (soil_layers_) //150424
{  subsurface_dry_weight_[0] = surface_dry_weight_;
   #ifdef NITROGEN
   subsurface_N_content_[0] = surface_N_content_;
   #endif
   float64 residue_decomposition_constant = 0.0;
       residue_decomposition_constant
         = convert_halflife_to_constant(halflife_);
   initialize
      (standing_stubble_                                                         //040517
      ,residue_decomposition_constant                                            //060202
      ,50.0 // NYN default_carbon_nitrogen_ratio_   // Used when we dont have a N mass (or for initial residue)  //060225
      ,default_straw_residue_fraction_carbon //  straw_fraction_carbon_residue   //061101
      ,subsurface_dry_weight_
      #ifdef NITROGEN
      ,subsurface_N_content_
      #endif
      ,horizons);
}
//_Residue_biomatter_profile_simple:constructor_____________________2001-10-11_/
Residue_biomatter_profile_simple::~Residue_biomatter_profile_simple()
{  delete initial;
}
//_Residue_biomatter_profile_simple_________________________________2009-04-16_/
bool Residue_biomatter_profile_simple::delete_subsurface(nat8 layer)
{  bool deleted = true;
   if (subsurface[layer])
   {  delete subsurface[layer];
      subsurface[layer] = 0;
   } else deleted = false;
   if (layer == 0) surface = 0;
   return deleted;
}
//_delete_subsurface________________________________________________2006-02-14_/
Biomatter_residue_V41 *Residue_biomatter_profile_simple
::create_biomatter_residue(const Biomatter_residue_V41 &from_copy)         const
{  return new Biomatter_residue_V41(from_copy);
}
//_Residue_biomatter_profile_simple_________________________________2006-02-14_/
Biomatter_residue_V41 *Residue_biomatter_profile_simple
::provide_biomatter_residue(nat8 layer)                               provision_
{  if (!subsurface[layer])
      subsurface[layer] = new Biomatter_residue_V41;
   if (layer == 0) surface = subsurface[0];
   return  subsurface[layer];
}
//_provide_biomatter_residue________________________________________2006-02-14_/
Biomatter_residue_V41 *Residue_biomatter_profile_simple::create_biomatter_residue
( float64 _carbon_fraction ) const
{ return new Biomatter_residue_V41(); };
//_create_biomatter_residue_________________________________________2006-02-02_/
float64 Residue_biomatter_profile_simple::totalize
(Biomatter_residue_V41 &total,Residue_horizons horizon, const nat8 horizons[], float64 &N_this_pool) const
{  float64 mass_this_pool = 0;                                                   //050116
   N_this_pool = 0;                                                              //050118
   nat8 total_from_layer = ((horizon == ALL_HORIZON) || (horizon == SURFACE_HORIZON) ) ? 0 : 1;
   for (nat8 sublayer = total_from_layer; sublayer <= MAX_soil_sublayers; sublayer++) //011021
       if ( ((horizon == horizons[sublayer]) || (horizon == ALL_HORIZON)
           ||(horizon == ALL_SUBSURFACE_HORIZON))
           && (horizons[sublayer] > NULL_HORIZON))
   {  const Biomatter_residue_V41 *subsurface_layer = get_subsurface(sublayer);  //060214
      if (subsurface_layer )                                                     //040820
      {  mass_this_pool +=subsurface_layer ->kg();                               //050116
         N_this_pool += subsurface_layer ->get_N_content();                      //050118
         total.add(*subsurface_layer );
      }
   }
   return mass_this_pool;                                                        //050116
}
//_totalize_________________________________________________________2001-10-05_/
Biomatter_residue_V41 &Residue_biomatter_profile_simple::sum_subsurface_to_layer
(Biomatter_residue_V41 &total, nat8 downto_sublayer)                       const
{  if (downto_sublayer == 0) // default option is sum whole profile
      downto_sublayer = MAX_soil_sublayers;
   for (nat8 sublayer = 1; sublayer <= downto_sublayer ; sublayer++)
   {
      const Biomatter_residue_V41 *subsurface_layer = get_subsurface(sublayer);  //060214
      if (subsurface_layer )                                                     //040820
         total.add(*subsurface_layer );
   }
   return total;
}
//_sum_subsurface_to_layer__________________________________________2001-10-05_/
void Residue_biomatter_profile_simple::exhaust()
{  for (int sl = 0; sl <= MAX_soil_sublayers; sl++)
   {  modifiable_ Biomatter_residue_V41 *subsurface_layer = mod_subsurface(sl);  //180110_040823
      if (subsurface_layer )
      {  subsurface_layer ->exhaust();
         delete_subsurface(sl);                                                  //060214
      }
   }
}
//_exhaust__________________________________________________________2004-05-23_/
bool Residue_biomatter_profile_simple::no_such_horizon
(Residue_horizons search_for_horizon,const nat8 horizons[])                const
{  bool result = true;
   for (int8 sublayer = 1; sublayer <= MAX_soil_sublayers; sublayer++)
      if (horizons[sublayer] == search_for_horizon )
             result = false;
   return result;
}
//_no_such_horizon__________________________________________________1998-10-18_/
void Residue_biomatter_profile_simple::redistribute
(float64 fract_shallow_to_deeps                                                  //980924
,float64 fract_surface_to_deeps                                                  //980924
,float64 fract_surface_to_shallow
,float64 shallow_horizon_thickness                                               //990217
,float64 deep_horizon_thickness                                                  //990217
,const nat8 horizons[])                                                          //9702xx
{  standing_stubble = false;                                                     //040517
   Biomatter_residue_V41 current_shallow_amount;                                 //011011
   float64 total_N_unused;                                                       //050118
   totalize(current_shallow_amount,SHALLOW_HORIZON,horizons,total_N_unused);     //011021
   bool no_deep_horizon = no_such_horizon(DEEP_HORIZON,horizons);                //981018
   fract_shallow_to_deeps = no_deep_horizon                                      //981018
      ? 0.0                                                                      //981018
      : fract_shallow_to_deeps;                                                  //990217
   Biomatter_residue_V41 from_shallow_to_deeps(current_shallow_amount);          //011011
   from_shallow_to_deeps.multiply_by(fract_shallow_to_deeps);                    //011021
   fract_surface_to_deeps = no_deep_horizon                                      //011011
      ? 0.0                                                                      //981018
      : fract_surface_to_deeps;                                                  //990217
   Biomatter_residue_V41 *from_surface_to_deeps = 0;                             //040820
   Biomatter_residue_V41 *from_surface_to_shallow = 0;                           //011011
   Biomatter_residue_V41 *surface = mod_surface();                               //180110_060214
   if (surface)                                                                  //040820
   {  float64 mass_from_surface_to_deeps = surface->kg() * fract_surface_to_deeps;     //101012
      float64 mass_from_surface_to_shallow =  surface->kg() * fract_surface_to_shallow;//101012
      from_surface_to_deeps   = dynamic_cast<Biomatter_residue_V41 *>(surface->extract_mass_kg(mass_from_surface_to_deeps));  //101012
      from_surface_to_shallow = dynamic_cast<Biomatter_residue_V41 *>(surface->extract_mass_kg(mass_from_surface_to_shallow));//101012

assert(from_surface_to_deeps != 0);
assert(from_surface_to_shallow != 0);
// 101012 this is just to make sure the new extract_mass_kg() is working
// eventually delete the 'if assert' statement

#ifdef POOL_MASS_BALANCE
   pool_mass_balance.removed += from_surface_to_shallow->kg();                   //040523
   pool_mass_balance.removed += from_surface_to_deeps->kg();                     //040523
#endif
   }
   for (int8 sublayer_apport = 1 ; sublayer_apport <= MAX_soil_sublayers; sublayer_apport++)//9702xx
   {  if ((horizons[sublayer_apport] == SHALLOW_HORIZON) )                       //9702xx
      {  float64 current_shallow_biomass = current_shallow_amount.kg();          //980921
         float64 apportion_to =
            soil_layers.get_thickness_m(sublayer_apport)                           //150424
            //150424 soil_thickness[sublayer_apport]
            / shallow_horizon_thickness;                                         //980217
         modifiable_ Biomatter_residue_V41 *subsurface_sublayer_apport = mod_subsurface(sublayer_apport); //180110_060214
         float64 subsurface_biomass = subsurface_sublayer_apport ? subsurface_sublayer_apport->kg()  : 0.0;          //040820
         float64 apportion_from = CORN::is_zero<float64>(current_shallow_biomass,0.000001) //011101
            ? 0.0                                                                //980921
            : subsurface_biomass / current_shallow_biomass;                      //980921
         if (from_surface_to_shallow)                                            //040820
         {  Biomatter_residue_V41 apportion_surface_to_shallow(*from_surface_to_shallow); //011011
            apportion_surface_to_shallow.multiply_by(apportion_to);              //011021
            subsurface_sublayer_apport = provide_biomatter_residue(sublayer_apport);      //060214
            subsurface_sublayer_apport->add(apportion_surface_to_shallow);       //011011
#ifdef POOL_MASS_BALANCE
    subsurface_sublayer_apport->mass_balance.added += apportion_surface_to_shallow.kg();  //040523
#endif
         }
         subsurface_sublayer_apport = provide_biomatter_residue(sublayer_apport);
         Biomatter_residue_V41 *apportion_from_shallow_to_deeps =
            dynamic_cast<Biomatter_residue_V41 *>
            (subsurface_sublayer_apport->extract_fraction(apportion_from));

assert(apportion_from_shallow_to_deeps != 0);
// 101012 this is just to make sure the new extract_mass_kg() is working
// eventually delete the 'if assert' statement
#ifdef POOL_MASS_BALANCE
   subsurface_sublayer_apport->mass_balance.removed += apportion_from_shallow_to_deeps.kg(); //040523
#endif
      }                                                                          //980921
      else if (horizons[sublayer_apport] == DEEP_HORIZON)                        //981019
      {  float64 apportion_to =
            soil_layers.get_thickness_m(sublayer_apport)                         //150424
            / deep_horizon_thickness;                                            //980921
         if (from_surface_to_deeps)                                              //040820
         {  Biomatter_residue_V41 apportion_from_surface_to_deeps(*from_surface_to_deeps);  //040820
            apportion_from_surface_to_deeps.multiply_by(apportion_to);           //011021
            Biomatter_residue_V41 *subsurface_sublayer_apport
               = provide_biomatter_residue(sublayer_apport);                     //060214
            subsurface_sublayer_apport->add(apportion_from_surface_to_deeps);    //011011
#ifdef POOL_MASS_BALANCE
    subsurface_sublayer_apport->mass_balance.added += apportion_from_surface_to_deeps.kg(); //040523
#endif
         }
         Biomatter_residue_V41 apportion_from_shallow_to_deeps(from_shallow_to_deeps);  //011011
            apportion_from_shallow_to_deeps.multiply_by(apportion_to);           //011021
         Biomatter_residue_V41 *subsurface_sublayer_apport = provide_biomatter_residue(sublayer_apport); //060214
         subsurface_sublayer_apport ->add(apportion_from_shallow_to_deeps);      //031130
#ifdef POOL_MASS_BALANCE
   subsurface_sublayer_apport->mass_balance.added += apportion_from_shallow_to_deeps.kg();   //040523
#endif
      }
   }
//000812_      In the case that material is deposited into particular shallow layers,
//             then distributed, since the material from the shallow layer
//             is simply removed proportionally above to deeper layers,
//             there will be bands of unmoved material.
//             This will evenly mix the shallow and deep material
   Biomatter_residue_V41 shallow_total/*(1.0)*/;   nat8 shallow_layer_count = 0; //011011
   Biomatter_residue_V41 deep_total/*(1.0)*/;      nat8 deep_layer_count    = 0; //011011
   for (int8 sublayer = 1 ; sublayer <= MAX_soil_sublayers; sublayer++)          //000812
   {  const Biomatter_residue_V41 *subsurface_sublayer=get_subsurface(sublayer); //000812
      if (subsurface_sublayer)                                                   //040820
      {  if ((horizons[sublayer] == SHALLOW_HORIZON) )                           //000812
         {  shallow_total.add(*subsurface_sublayer );                            //000812
            shallow_layer_count++;                                               //000812
         } else if (horizons[sublayer] == DEEP_HORIZON)                          //000812
         {  deep_total.add(*subsurface_sublayer);                                //000812
            deep_layer_count++;                                                  //000812
   }  }  }
   Biomatter_residue_V41 even_mix_shallow/*(1.0)*/; // Initially cleared         //011011
   if (shallow_layer_count)                                                      //011011
   {  even_mix_shallow.add(shallow_total);                                       //011021
      even_mix_shallow.divide_by(shallow_layer_count);                           //011021
   }
   Biomatter_residue_V41 even_mix_deep/*(1.0)*/; // Initially cleared            //011011
   if (deep_layer_count)                                                         //011011
   {  even_mix_deep.add(deep_total);                                             //011021
      even_mix_deep.divide_by(deep_layer_count);                                 //011011
   }
   for (int8 sublayer_mix=1; sublayer_mix <= MAX_soil_sublayers; sublayer_mix++) //000812
   { if ((horizons[sublayer_mix] == SHALLOW_HORIZON) )                           //000812
     {   delete_subsurface(sublayer_mix);                                        //011011
            Biomatter_residue_V41 *shallow_replacement = provide_biomatter_residue(sublayer_mix);  //060214
            shallow_replacement->add(even_mix_shallow);                          //060309
     } else if (horizons[sublayer_mix] == DEEP_HORIZON)                          //011011
      {  delete_subsurface(sublayer_mix);                                        //011011
            Biomatter_residue_V41 *deep_replacement = provide_biomatter_residue(sublayer_mix);     //060214
            deep_replacement->add(even_mix_deep);                                //060309
}  }  }
//_redistribute_____________________________________________________1999-02-17_/
float64 Residue_biomatter_profile_simple::get_surface_N_content()          const
{  const Biomatter_residue_V41 *surface = get_surface_constant();                //060214
   return (surface ? surface->get_N_content() : 0.0);
}
//_get_surface_N_content____________________________________________2001-10-04_/
float64 Residue_biomatter_profile_simple::get_subsurface_N_content(nat8 layer) const
{  const Biomatter_residue_V41 *subsurface_layer = get_subsurface(layer);        //060214
   return (subsurface_layer ? subsurface_layer->get_N_content(): 0.0);
}
//_get_subsurface_N_content_________________________________________2001-10-04_/
void Residue_biomatter_profile_simple::initialize
(bool    standing_stubble_                                                       //040517
,float64 decomposition_constant_                                                 //060122
,float64 default_carbon_nitrogen_ratio_                                          //060225
   // Used when we dont have a N mass (or for initial residue)
,float64 residue_carbon_fraction_                                                //060227
,soil_sublayer_array_64(subsurface_dry_weight_)
#ifdef NITROGEN
,soil_sublayer_array_64(subsurface_N_content_)
#endif
,nat8 horizons[])
{  standing_stubble = standing_stubble_;                                         //040517
   for (nat8 layer = 0; layer <= MAX_soil_layers; layer++)                       //011011
   {  Biomatter_residue_V41 *subsurface_layer                                    //060214
      = (!CORN::is_zero<float64>(subsurface_dry_weight_[layer],0.000001))        //200214_060214
      ? provide_biomatter_residue(layer) : 0;                                    //060214
      if (subsurface_layer )                                                     //040820
      {                                                                          //040820
         subsurface_layer ->update_amount_in_preset_units
            (subsurface_dry_weight_[layer]);                                     //150123_101012
         subsurface_layer->set_carbon_fraction(residue_carbon_fraction_);        //060227
         #ifdef NITROGEN
         subsurface_layer ->N_content = subsurface_N_content_[layer];
         #endif
         subsurface_layer ->set_contact_fraction                                 //040818
            ( layer? 1.0 :standing_stubble_
               ? DEFAULT_STANDING_STUBBLE_CONTACT_FRACTION
               : DEFAULT_SURFACE_ORGANIC_RESIDUE_CONTACT_FRACTION);
   }  }                                                                                                                                                                          //040820
   float64 total_N_unused;                                                                                                                                                       //050118
   totalize(get_initial(),ALL_HORIZON,horizons,total_N_unused);                                                                                                                  //060215
}
//_initialize_______________________________________________________1999-02-17_/
float64 Residue_biomatter_profile_simple::get_surface_mass()               const
{  const Biomatter_residue_V41 *surface = get_surface_constant();
   return (surface ? surface->kg()  : 0.0);
}
//_get_surface_mass_________________________________________________2001-10-04_/
float64 Residue_biomatter_profile_simple::get_subsurface_mass(nat8 layer)  const
{  const Biomatter_residue_V41 *subsurface_layer = get_subsurface(layer);
   return (subsurface_layer ? subsurface_layer->kg()  : 0.0);
}
//_get_subsurface_mass______________________________________________2001-10-04_/
float64 Residue_biomatter_profile_simple::set_surface_mass
(float64 new_surface_mass_kg_m2)                                   modification_
{  Biomatter_residue_V41 *surface = provide_biomatter_residue(0);
   return surface->set_biomass(new_surface_mass_kg_m2);
}
//_set_surface_mass_________________________________________________2001-10-04_/
float64 Residue_biomatter_profile_simple::set_subsurface_mass
(nat8 layer, float64 new_subsurface_mass_kg_m2)                    modification_
{  Biomatter_residue_V41 *subsurface_layer = provide_biomatter_residue(layer);
   return  subsurface_layer->set_biomass(new_subsurface_mass_kg_m2);
}
//_set_subsurface_mass______________________________________________2001-10-04_/
float64 Residue_biomatter_profile_simple::set_surface_N_content
(float64 new_surface_N_content)                                    modification_
{  Biomatter_residue_V41 *surface = provide_biomatter_residue(0);
   return surface->set_N_content(new_surface_N_content);
}
//_set_surface_N_content____________________________________________2001-10-04_/
float64 Residue_biomatter_profile_simple::set_subsurface_N_content
(nat8 layer, float64 new_subsurface_N_content)                     modification_
{  Biomatter_residue_V41 *subsurface_layer = provide_biomatter_residue(layer);
   return subsurface_layer->set_N_content(new_subsurface_N_content);
}
//_set_subsurface_N_content_________________________________________2001-10-04_/

