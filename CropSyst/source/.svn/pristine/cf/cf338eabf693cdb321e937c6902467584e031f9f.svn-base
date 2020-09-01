#include "chemical_mass_profile.h"
#include "corn/math/compare.hpp"
#include "soil/soil_I.h"
//______________________________________________________________________________
float64 Chemical_mass_profile_interface
::get_layers(soil_layer_array64(mass_by_layer))                            const
{  float64 mass_sum = 0.0;
   for (nat8 layer = 0; layer < MAX_soil_layers; layer++)
   {  float64 mass_layer = get_layer(layer);
      mass_by_layer[layer] = mass_layer;
      mass_sum += mass_layer;
   }
   return mass_sum;
}
//_get_layers_______________________________________________________2008-01-17_/
float64 Chemical_mass_profile_interface
::set_layers(soil_layer_array64(mass_by_layer))                    modification_
{  float64 mass_sum = 0.0;
   for (nat8 layer = 0; layer < MAX_soil_layers; layer++)
   {  float64 mass_layer = mass_by_layer[layer] ;
      set_layer(layer,mass_layer);
      mass_sum += mass_layer;
   }
   commit_diffs();                                                               //120731
   return mass_sum;
}
//_set_layers_______________________________________________________2008-01-17_/
Chemical_mass_profile_layer_based
::Chemical_mass_profile_layer_based(const Soil::Layers_interface &_layers)
: Chemical_mass_profile_interface()
, layers(_layers)
{  clear_layer_array64(mass);
}
//_Chemical_mass_profile_layer_based:constructor_______________________________/
bool Chemical_mass_profile_layer_based
:: initialize(soil_layer_array64(initial_mass_by_layer))           modification_
{
   for (nat8 layer = 1; layer <= MAX_soil_layers; layer ++)
      mass[layer] = initial_mass_by_layer[layer];
   return true;
}
//______________________________________________________________________________/
float64 Chemical_mass_profile_layer_based
::sum_to_depth(float64 a_given_depth)                                      const
{  float64 sum = 0.0;
   nat8 layer_at_depth = layers.
      //170630 get_layer_at_depth
      get_layer_at_depth_or_last_layer                                           //170630
      (a_given_depth);
   for (nat8 layer = 1; layer <= layer_at_depth; layer++)
      sum += mass[layer];
   return sum;
}
//_sum_to_depth________________________________________________________________/
float64 Chemical_mass_profile_layer_based
::sum_profile()                                                            const
{  float64 sum = 0.0;
   for (nat8 layer = 1; layer <= MAX_soil_sublayers; layer++)
      sum += mass[layer];
   return sum;
}
//_sum_profile_________________________________________________________________/
Chemical_mass_profile_node_based
::Chemical_mass_profile_node_based(const Soil::Layers_interface &_layers)
: Chemical_mass_profile_interface()
, layers(_layers)
, layer_diffs(0)
{
   clear_layer_array64(mass_upper); // Upper part of the respective layer        //080116
   clear_layer_array64(mass_lower); // lower part of the respective layer        //080116
#ifdef VB_compare
   clear_layer_array64(mass_upper_E); // Upper part of the respective layer      //080116
   clear_layer_array64(mass_lower_E); // lower part of the respective layer      //080116
#endif
}
//_Chemical_mass_profile_node_based:constructor________________________________/
float64 Chemical_mass_profile_node_based
::get_layer(nat8 layer)                                                    const
{
   float64 mass_layer =
   mass_upper[layer] + mass_lower[layer];                                        //080128
   return mass_layer;
}
//_get_layer___________________________________________________________________/
float64 Chemical_mass_profile_node_based
::set_layer(nat8 layer,float64 set_mass)                           modification_
{
   float64 curr_mass_layer = get_layer(layer);
   dec_layer(layer,curr_mass_layer);                                             //120730
   inc_layer(layer,set_mass);                                                    //120730
   return set_mass;
}
//_set_layer___________________________________________________________________/
bool Chemical_mass_profile_node_based
::initialize(soil_layer_array64(initial_mass_by_layer))            modification_
{  // Note that mass is split this way ONLY for initialization
   for (nat8 layer = 1; layer <= MAX_soil_layers; layer++)                       //080128
   {  float64 half_layer_mass = initial_mass_by_layer[layer]/2.0;
      mass_upper[layer] = half_layer_mass; // Upper part of the respective layer //080128
      mass_lower[layer] = half_layer_mass; // lower part of the respective layer //080128
   }
   return true;
}
//_initialize__________________________________________________________________/
float64 Chemical_mass_profile_layer_based
::dec_layer(nat8 layer,float64 decrement_mass)                     modification_
{
   float64 act_decrement_mass = decrement_mass;
   act_decrement_mass = std::min<float64>(decrement_mass,mass[layer]);
   mass[layer] -= act_decrement_mass;
   return act_decrement_mass;
/*170503 limit deduction to what is available
   mass[layer] -= decrement_mass;
   if (mass[layer] < 0.0)
       mass[layer] = 0.0;                                                         //060715
   return mass[layer];
*/
}
//_dec_layer________________________________________________________2017-05-03_/
float64 Chemical_mass_profile_node_based
::dec_layer(nat8 layer,float64 decrement_mass)                     modification_
{
  float64 act_decrement_mass = CORN::must_be_between<float64>
      (decrement_mass,0.0,get_layer(layer));
   if (act_decrement_mass > 0.0000001)                                           //170503_120808
   {
      if (!layer_diffs) layer_diffs = new CORN::Dynamic_array<float64>(MAX_soil_layers);
      float64 cum_diff = layer_diffs->get(layer);
      cum_diff -= act_decrement_mass;
      layer_diffs->set(layer,cum_diff);
   }
   return act_decrement_mass;
/*170503 limit deduction to what is available
  float64 new_mass_assumed = get_layer(layer) - decrement_mass;
   if (!CORN::is_zero<float64>(decrement_mass))           //120808
   {
   if (!layer_diffs) layer_diffs = new CORN::Dynamic_array<float64>(MAX_soil_layers);
   float64 cum_diff = layer_diffs->get(layer);
   cum_diff -= decrement_mass;
   layer_diffs->set(layer,cum_diff);
   }
   return new_mass_assumed;
*/
}
//_dec_layer________________________________________________________2008-01-28_/
float64 Chemical_mass_profile_node_based
::inc_layer(nat8 layer,float64 increment_mass)                     modification_
{  if (!CORN::is_zero<float64>(increment_mass))                                  //120808
   {
   if (!layer_diffs) layer_diffs = new CORN::Dynamic_array<float64>(MAX_soil_layers);
   float64 cum_diff = layer_diffs->get(layer);
   cum_diff += increment_mass;
   layer_diffs->set(layer,cum_diff);
   }
   return increment_mass;
}
//_inc_layer____________________________________________2012-07-30__2008-01-28_/
bool Chemical_mass_profile_node_based::commit_diffs()              modification_
{
   float64 balance = 0;

   if (!layer_diffs) return true;;
   float64 mass_profile_new = 0.0;
   float64 mass_profile_old = 0.0;
   float64 diffs_profile = 0;
   soil_layer_array64(diff_nodal); clear_layer_array64(diff_nodal);
   nat8 layer_count = layers.count();
   nat8 node_count = layer_count+1;
   for (nat8 node = 1; node <= node_count - 1; node++)
   {  nat8 prev_node = node-1;
      nat8 next_node = node+1;
      nat8 layer = node;
      nat8 prev_layer = layer-1;
      mass_profile_old += mass_upper[layer] +  mass_lower[layer];
      float64 mass_node      = get_node(node     ,number_real_plus_extended_nodes);
      float64 mass_prev_node = get_node(prev_node,number_real_plus_extended_nodes);
      float64 mass_next_node = get_node(next_node,number_real_plus_extended_nodes);
      float64 diff_layer = layer_diffs->get(layer);
      float64 diff_prev_layer = layer_diffs->get(prev_layer);
      float64 this_plus_next_node =  mass_node + mass_next_node;
      float64 this_plus_prev_node =  mass_node + mass_prev_node;
      float64 next_node_weighting =  (this_plus_next_node == 0) ? 1.0 : mass_node / this_plus_next_node;
      float64 prev_node_weighting =  (this_plus_prev_node == 0) ? 1.0 : mass_node / this_plus_prev_node;
      float64 diff_node =
         (node == 1)
            ?      diff_layer     * next_node_weighting
            : (node == node_count)
               ?  diff_prev_layer * prev_node_weighting
               : (diff_prev_layer * prev_node_weighting
                 +diff_layer      * next_node_weighting);
      diff_nodal[node] = diff_node;
      diffs_profile += diff_node;
   }
   if (diffs_profile != 0.0)
   {
      for (nat8 layer = 1; layer <= layer_count; layer++)
      {  nat8 node = layer;
         if (layer == 1)
         {  mass_upper[layer] += diff_nodal[node];
            float64 half_diff_node_next = diff_nodal[node+1]/2.0;                //140724
            mass_lower[layer] += half_diff_node_next; //diff_nodal[node+1]/2.0;
         } else if (layer == layer_count)
         {  mass_upper[layer] += diff_nodal[node] / 2.0;
            mass_lower[layer] += diff_nodal[node+1];
         } else
         {  mass_upper[layer] += diff_nodal[node] / 2.0;
            mass_lower[layer] += diff_nodal[node+1] /2.0;
         }
         // Rolf found a case where very small immobilization was giving
         // negative mass resulting in huge error.
         // So we can't allow neg mass.                                          //140724
         mass_upper[layer] = CORN::must_be_0_or_greater<float64>(mass_upper[layer]);  //140724
         mass_lower[layer] = CORN::must_be_0_or_greater<float64>(mass_lower[layer]);  //140724
         mass_profile_new += mass_upper[layer] + mass_lower[layer] ;
      }
      balance = mass_profile_old + diffs_profile - mass_profile_new;
   }
   delete layer_diffs; layer_diffs = 0;
   return CORN::is_zero<float64>(balance);                                       //080220
}
//_commit_diffs_____________________________________________________2012-07-31_/
float64 Chemical_mass_profile_node_based
::sum_to_depth(float64 a_given_depth)                                      const
{  float64 sum = 0.0;
   nat8 layer_at_depth = layers.get_layer_at_depth(a_given_depth);
   for (nat8 layer = 1; layer <= layer_at_depth; layer++)
      sum += get_layer(layer);
   return sum;
}
//_sum_to_depth________________________________________________________________/
float64 Chemical_mass_profile_node_based
::sum_profile()                                                            const
{  float64 sum = 0.0;
   for (nat8 layer = 1; layer <= MAX_soil_sublayers; layer++)
      sum += mass_upper[layer] + mass_lower[layer];
   return sum;
}
//_sum_profile_________________________________________________________________/
float64 Chemical_mass_profile_node_based
::get_node(nat8 node,nat8 number_of_real_plus_extended_nodes)              const
{
   nat8 layer = node;                                                            //080128
   float64 mass_node =  0.0;                                                     //080128
   mass_node = mass_upper[layer] + mass_lower[layer-1];
   return            mass_node;
}
//_get_node____________________________________________________________________/
#ifdef NOT_CURRENTLY_NEEDED
#error check obsolete
float64 Chemical_mass_profile_node_based::set_node
(nat8 node,float64 new_mass,nat8 number_of_real_plus_extended_nodes
,float64 molecular_to_elemental) modification_
{
   nat8 layer = node;         // Node is at the top of the layer                 //080128
   if (node == 1)                                                                //080128
   {
         mass_upper[layer] = new_mass;                                           //080128
#ifdef VB_compare
mass_upper_E[layer] = new_mass* molecular_to_elemental;
#endif
   }
   else if (node == number_of_real_plus_extended_nodes)     // Number of effective node = number of eff layers + 1 //080128
   {
      mass_lower[number_of_real_plus_extended_nodes] = new_mass;                 //080128
#ifdef VB_compare
mass_upper_E[number_of_real_plus_extended_nodes] = new_mass * molecular_to_elemental;
#endif
   }
   else                                                                          //080128
   {                                                                             //080128
      float64 thickness_layer       = layers.get_thickness_m(layer);             //080128
      float64 thickness_prev_layer  = layers.get_thickness_m(layer-1);           //080128
      mass_lower[layer-1] = new_mass / 2.0 ;                                     //080128
      mass_upper[layer]   = new_mass / 2.0;                                      //080128
#ifdef VB_compare
      mass_lower_E[layer-1] = mass_lower[layer-1] * molecular_to_elemental;      //080128
      mass_upper_E[layer]   = mass_upper[layer]* molecular_to_elemental;         //080128
#endif
   }
   return  new_mass;
}
//_set_node__________________________________________________________set_node__/
#endif
float64 Chemical_mass_profile_node_based::set_nodes
(soil_node_array64(new_mass_by_node)
,float64 number_real_plus_extended_nodes
,float64 molecular_to_elemental)                                   modification_
{  float64 mass_profile = 0.0;
   for (nat8 node = 1; node <= number_real_plus_extended_nodes; node++)
   {  nat8 layer = node;
      if (node == 1)
            mass_upper[layer] = new_mass_by_node[node];
      else  mass_upper[layer] = new_mass_by_node[node] / 2.0;
      mass_lower[layer] = new_mass_by_node[node+1] /2.0;
      #ifdef VB_compare
      mass_lower_E[layer] = mass_lower[layer] * molecular_to_elemental;          //080128
      mass_upper_E[layer] = mass_upper[layer]* molecular_to_elemental;           //080128
      #endif
      mass_profile += new_mass_by_node[node];
   } // for node
   return mass_profile;                                                          //080220
}
//_set_nodes________________________________________________________2008-02-20_/
#ifdef VB_compare
// This is used for debugging and comparison with VB version.
soil_layer_array64(mass_upper_E); // Upper part of the respective layer          //080116
soil_layer_array64(mass_lower_E); // lower part of the respective layer          //080116
#endif
//______________________________________________________________________________/
