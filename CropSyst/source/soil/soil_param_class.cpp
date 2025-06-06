#include "soil_param_class.h"
#  include "options.h"
#include "corn/math/compare.hpp"
#include <assert.h>
//---------------------------------------------------------------------------
Soil_parameters_class_common::Soil_parameters_class_common()
: Common_parameters_data_record(NO_ASSOCIATED_DIRECTORY,LABEL_soil,CS_VERSION)   //180626 CS_VERSION_NUMBERS)   //050323
, steepness_percent           (0.01)
, hydrologic_group_clad    (USDA_NRCS::A)
, hydrologic_condition_clad(GOOD)                                                //971010
, slope_length                (10.0)
, albedo_dry                  (0.155)                                            //041202
, albedo_wet                  (0.09)                                             //041202
, water_vapor_conductance_atmosphere_adj(1.0)                                    //041206
, user_fallow_curve_number_16 (0)                                                //051019
, override_curve_number_16    (0)                                                //051019
, surface_storage_mm_32       (0.0)                                              //990212
, SLPF_CropGro                (1.0)                                              //071016
, compute_surface_storage     (true)
, horizon_count               (0)                                                //190402_060620
  // 060620  number_horizons needs to be 0 otherwise the Soil Editor crashes
  // when trying to calculated hydraulic properties on a new soil
, save_only_specified_layers  (false)                                            //060129
, evaporative_layer_thickness_cascade(0.05)                                      //120724
, evaporative_layer_thickness_finite_difference(0.02)                            //120724
, water_holding_capacity_to_1m(0.0)                                              //080728
, aggregated_water_holding_capacity_to_1m(0.0)                                   //080728
, capability_class_irrigated  (0)                                                //080728
, capability_class_dryland    (0)                                                //080728
, agricultural_dryland        (true)                                             //080728
, agricultural_irrigated      (true)                                             //080728
, bound_by_bedrock            (false)
, NH4_Langmuir                (2000.0,0.003)                                     //190404
{
   for (int8 layer = 1; layer <= MAX_soil_horizons ; layer++)
   {  //  Need some default values or the soil editor will crash when it tries to update soil properties 060620_
      user[layer] = all_computer_generated;                                      //981104
   }
   #ifndef MICROBASIN_VERSION
   STATSGO.path.clear();                                                         //181029_120207
   #endif
   STATSGO.COMPPCT = 0;                                                          //080728
}
//_Soil_parameters_class_common:constructor____________________________________/
bool Soil_parameters_class_common::expect_structure(bool for_write)
{
   set_current_section(LABEL_soil);
   Common_parameters_data_record::expect_structure(for_write);
   structure_defined = false;                                                    //120314
   set_current_section  (LABEL_soil);
      expect_enum           (LABEL_hydrologic_group,hydrologic_group_clad);
      expect_enum           (LABEL_hydrologic_condition,hydrologic_condition_clad);
      expect_int16          (LABEL_override_curve_number,override_curve_number_16);
      expect_bool           (LABEL_compute_surface_storage,compute_surface_storage,VV_bool_entry::form_true);
   set_current_section  (LABEL_profile);
      expect_nat8           (LABEL_number_layers,horizon_count,10);
   set_current_section  ("evaporative_layer_thickness");
      expect_float32       ("cascade"     , evaporative_layer_thickness_cascade);     //120724
      expect_float32       ("finite_diff" , evaporative_layer_thickness_finite_difference); //120724

   //set_current_section   ("CropGro");
     // expect_valid_float32(v_SLPF_CropGro);
      // Normally all possible layers are saved to the file
      // (so that the user can temporarily change the soil layers)
      // Soil generator programs dont need to save all the layers.
   nat16 save_horizons = (for_write && save_only_specified_layers) ? (nat16)horizon_count : (nat16)MAX_soil_horizons;
   expect_int16_array(LABEL_user_value,user,1,save_horizons,false);

//NYN     expect_float32_array_with_units_code_fixed("water_holding_capacity",UC_meter           ,water_holding_capacity_m        ,1,save_horizons,false);

               // These two parameters were special for the Agroclimate simulations RLN
   set_current_section("STATSGO");
   expect_float32 ("water_holding_capacity_to_1m"           ,water_holding_capacity_to_1m);
   expect_float32 ("aggregated_water_holding_capacity_to_1m", aggregated_water_holding_capacity_to_1m);
   expect_float32 ("capability_class_irrigated"             ,capability_class_irrigated);
   expect_float32 ("capability_class_dryland"               ,capability_class_dryland);
   expect_bool    ("agricultural_irrigated"                 ,agricultural_irrigated);
   expect_bool    ("agricultural_dryland"                   ,agricultural_dryland);
   expect_bool    ("bound_by_bedrock"                       ,bound_by_bedrock);
   structure_defined = true;                                                     //120314
   return true;
}
//_expect_structure____________________________________________________________/
bool Soil_parameters_class_common::delete_horizon( nat8 horizon)
{  // The soil must have at least 1 layer
   if (horizon_count == 1) return false;
   if (horizon < MAX_soil_horizons)
      for ( nat8 l = horizon; l < MAX_soil_horizons_alloc-1 ; l++)
         user[l] = user[l+1];
   user[MAX_soil_horizons] = 0;
   if (horizon <= horizon_count) horizon_count --; //  -= 1;                     //050311
  return true;
}
//_delete_horizon___________________________________________________2004-12-14_/
bool Soil_parameters_class_common::insert_horizon( nat8 layer)
{  if (horizon_count == MAX_soil_horizons) return false;
   for (nat8 l = MAX_soil_horizons; l > layer; l--)
      user[l] = user[l-1];
   user[layer] = 0;
   if (layer <= horizon_count+1) horizon_count++ ; // only increment the number of layers if the insert layer point is above the last layer or would append a layer //041214
   return true;                                                                  //050311
}
//_insert_horizon___________________________________________________2004-12-14_/
nat8  Soil_parameters_class_common::get_horizon_at(float64 a_given_depth_m) const
{  float64 horizon_depth = 0.0;
   nat8 horizon = 0;
   if (CORN::is_zero<float64>(a_given_depth_m))                                  //040524
      a_given_depth_m = 9999.9;
   do
   { horizon++;
     horizon_depth += get_horizon_thickness(horizon);                            //160915
   } while ((horizon_depth < a_given_depth_m) && (horizon < horizon_count));
   return horizon;                                                               //970509
}
//_insert_horizon___________________________________________________2006-01-27_/
float64 Soil_parameters_class_common::get_horizon_center_depth(nat8 horizon)      const
{  float64 depth = 0.0;
   for (nat8 h = 1; h <= horizon; h++)
   {
      float64 thickness_horizon = get_horizon_thickness(horizon);                //160915
      depth += thickness_horizon;
      return depth - (thickness_horizon / 2.0);
   }
   return 0.0;
}
//_get_horizon_center_depth_________________________________________2013-02-07_/
float64 Soil_parameters_class_common::get_horizon_depth(nat8 horizon)             const
{  float64 depth = 0.0;
   for (nat8 h = 1; h <= horizon; h++)
      depth += get_horizon_thickness(h);                                         //160915
   return depth;
}
//_get_horizon_depth________________________________________________2013-02-07_/
nat8 Soil_parameters_class_common::limit_profile_depth(float32 soil_profile_depth_limit)
{
   if (CORN::is_zero<float32>(soil_profile_depth_limit))
      return horizon_count;
   nat8 limited_horizon = get_horizon_at(soil_profile_depth_limit);
   float32 limited_horizon_depth_curr = get_horizon_depth(limited_horizon);
   float32 horizon_reduction = limited_horizon_depth_curr - soil_profile_depth_limit;
   if (horizon_reduction > 0)
   {  float32 limited_horizon_thickness_curr = (float32)get_horizon_thickness(limited_horizon);
      float32 horizon_thickness_new
         = limited_horizon_thickness_curr - horizon_reduction;
      set_horizon_thickness(limited_horizon,horizon_thickness_new);
      // Could delete the truncated horizons, but shouldnt be necessary.
      horizon_count = limited_horizon;
   }
   return horizon_count;
}
//_limit_profile_depth______________________________________________2017-06-09_/
namespace version4
{
//______________________________________________________________________________
Soil_parameters_class::Soil_parameters_class()
: ::Soil_parameters_class_common()
{
   clear();                                                                      //070516
/*abandoned
   horizon_count = MAX_soil_horizons;                                            //190416
      // We need the soil horizon count
      // because in V4 the where was no (dynanic) array items to bind            //190416
*/
   for (int8 layer = 1; layer <= MAX_soil_horizons ; layer++)
   {  //  Need some default values or the soil editor will crash when it tries to update soil properties 060620
      horizon_sand[layer] = 60;                                                  //060620
      horizon_clay[layer] = 20;                                                  //060620
      horizon_silt[layer] = 20;                                                  //060620
   }
}
//_Soil_parameters_class:constructor___________________________________________/
void Soil_parameters_class::clear()
{
   clear_horizon_array32(horizon_thickness);
   clear_horizon_array32(horizon_bypass_coef);
   clear_horizon_array32(horizon_permanent_wilt_point);
   clear_horizon_array32(horizon_field_capacity);
   clear_horizon_array32(horizon_water_pot_at_FC);                               //130127
   clear_horizon_array32(horizon_bulk_density_g_cm3);
   clear_horizon_array32(horizon_sand);
   clear_horizon_array32(horizon_clay);
   clear_horizon_array32(horizon_silt);
   clear_horizon_array32(horizon_sat_hydraul_cond_m_d);
   clear_horizon_array32(horizon_saturation_WC);
   clear_horizon_array32(horizon_air_entry_pot);
   clear_horizon_array32(horizon_Campbell_b);
   clear_horizon_array32(horizon_pH,7.0);
   clear_horizon_array32(horizon_cation_exchange_capacity,20.0);
   clear_horizon_array32(horizon_organic_matter);
   clear_horizon_array32(horizon_organic_matter_low);
   clear_horizon_array32(horizon_organic_matter_high);
   /*don't clear NH4_Langmuir because
      when reading from the file this clear the array
      so there are no items to bind do
   NH4_Langmuir.clear();                                                         //190404
   */
   /*190404
   clear_horizon_array32(horizon_NH4_Langmuir_K);                                //190402
   clear_horizon_array32(horizon_NH4_Langmuir_Q);                                //190402
   */
//NYN    clear_horizon_array32(water_holding_capacity_m);
}
//_clear____________________________________________________________2007-05-16_/
bool Soil_parameters_class::expect_structure(bool for_write)
{
   bool expected = ::Soil_parameters_class_common::expect_structure(for_write);         //161025
   structure_defined = false;
   set_current_section(LABEL_soil);
   nat16 save_horizons = (for_write && save_only_specified_layers) ? (nat16)horizon_count : (nat16)MAX_soil_horizons;

   expect_float32_array_with_units_code_fixed(LABEL_thickness       ,UC_meter             ,horizon_thickness               ,1,save_horizons,false);
   expect_float32_array_with_units_code_fixed(LABEL_bypass_coef     ,UC_coefficient       ,horizon_bypass_coef             ,1,save_horizons,false);
   expect_float32_array_with_units_code_fixed(LABEL_bulk_density    ,UC_g_per_cm3         ,horizon_bulk_density_g_cm3      ,1,save_horizons,false);
   expect_float32_array_with_units_code_fixed(LABEL_perm_wilt_point ,UC_m3_per_m3         ,horizon_permanent_wilt_point    ,1,save_horizons,false);
   expect_float32_array_with_units_code_fixed(LABEL_field_capacity  ,UC_m3_per_m3         ,horizon_field_capacity          ,1,save_horizons,false);
   expect_float32_array_with_units_code_fixed("water_pot_at_FC"     ,UC_kPa               ,horizon_water_pot_at_FC         ,1,save_horizons,false); //130228

  // Sand Clay Silt by layer added
   expect_float32_array_with_units_code_fixed(LABEL_sand            ,UC_percent           ,horizon_sand                    ,1,save_horizons,false);
   expect_float32_array_with_units_code_fixed(LABEL_clay            ,UC_percent           ,horizon_clay                    ,1,save_horizons,false);
   expect_float32_array_with_units_code_fixed(LABEL_silt            ,UC_percent           ,horizon_silt                    ,1,save_horizons,false);
   expect_float32_array_with_units_code_fixed(LABEL_sat_hydraul_cond,UC_m_per_day         ,horizon_sat_hydraul_cond_m_d    ,1,save_horizons,false);
   expect_float32_array_with_units_code_fixed("saturation"          ,UC_m3_per_m3         ,horizon_saturation_WC           ,1,save_horizons,false);

   expect_float32_array_with_units_code_fixed(LABEL_air_entry_pot   ,UC_J_per_kg          ,horizon_air_entry_pot           ,1,save_horizons,false);
   expect_float32_array_with_units_code_fixed(LABEL_b               ,UC_factor            ,horizon_Campbell_b              ,1,save_horizons,false);
   expect_float32_array_with_units_code_fixed(LABEL_pH              ,UC_pH                ,horizon_pH                        ,1,save_horizons,false);
   expect_float32_array_with_units_code_fixed(LABEL_cation_exchange_capacity,UC_mEq_per_100g,horizon_cation_exchange_capacity,1,save_horizons,false);
   expect_float32_array_with_units_code_fixed("organic_matter"      ,UC_percent           ,horizon_organic_matter            ,1,save_horizons,false);
   expect_float32_array_with_units_code_fixed("organic_matter_low"  ,UC_percent           ,horizon_organic_matter_low        ,1,save_horizons,false);
   expect_float32_array_with_units_code_fixed("organic_matter_high" ,UC_percent           ,horizon_organic_matter_high       ,1,save_horizons,false);

   expect_dynamic_float32_array("NH4/Langmuir/K"      ,/*properties.*/NH4_Langmuir.K          ,horizon_count,1,UC_kg_kg      ,false);
   expect_dynamic_float32_array("NH4/Langmuir/Q"      ,/*properties.*/NH4_Langmuir.Q          ,horizon_count,1,UC_kg_kg      ,false);
   return expected;                                                              //161025
}
//_expect_structure_________________________________________________2016-09-15_/
void Soil_parameters_class::set_horizons_thickness(soil_horizon_array32(_horizon_thickness_32))
{  // This is only used for GIS projects to override soil thickness for polygons/cells
   for ( nat8 layer = 0; layer <= horizon_count; layer++)
      if (!CORN::is_zero<float32>(_horizon_thickness_32[layer]))
         horizon_thickness[layer] = _horizon_thickness_32[layer];
}
//_set_horizon_thickness____________________________________________1999-09-29_/
bool Soil_parameters_class::delete_horizon( nat8 horizon)
{  // The soil must have at least 1 layer
   if (horizon_count == 1) return false;
   delete_horizon_at(horizon_thickness                ,horizon);
   delete_horizon_at(horizon_bypass_coef              ,horizon);
   delete_horizon_at(horizon_bulk_density_g_cm3       ,horizon);
   delete_horizon_at(horizon_permanent_wilt_point     ,horizon);
   delete_horizon_at(horizon_field_capacity           ,horizon);
   delete_horizon_at(horizon_water_pot_at_FC          ,horizon);                 //130128
   delete_horizon_at(horizon_sand                     ,horizon);
   delete_horizon_at(horizon_clay                     ,horizon);
   delete_horizon_at(horizon_silt                     ,horizon);
   delete_horizon_at(horizon_sat_hydraul_cond_m_d     ,horizon);
   delete_horizon_at(horizon_saturation_WC            ,horizon);                 //080113
   delete_horizon_at(horizon_air_entry_pot            ,horizon);
   delete_horizon_at(horizon_Campbell_b               ,horizon);
   delete_horizon_at(horizon_pH                       ,horizon);
   delete_horizon_at(horizon_cation_exchange_capacity ,horizon);
   delete_horizon_at(horizon_organic_matter           ,horizon);                 //060130
   delete_horizon_at(horizon_organic_matter_low       ,horizon);                 //070607
   delete_horizon_at(horizon_organic_matter_high      ,horizon);                 //070607
   NH4_Langmuir.remove_at(horizon);                                              //190404
   /*190404
   delete_horizon_at(horizon_NH4_Langmuir_K           ,horizon);                 //190402
   delete_horizon_at(horizon_NH4_Langmuir_Q           ,horizon);                 //190402
   */
//NYN /*080516_*/    delete_horizon_at(water_holding_capacity_m         ,horizon);
   if (horizon <= horizon_count) horizon_count --; //  -= 1;                 //050311
  return ::Soil_parameters_class_common::delete_horizon(horizon);
}
//_delete_horizon___________________________________________________2004-12-14_/
bool Soil_parameters_class::insert_horizon(nat8 layer)
{  if (horizon_count == MAX_soil_horizons) return false;
   insert_horizon_at(horizon_thickness                ,layer);
   insert_horizon_at(horizon_bypass_coef              ,layer);
   insert_horizon_at(horizon_bulk_density_g_cm3       ,layer);
   insert_horizon_at(horizon_permanent_wilt_point     ,layer);
   insert_horizon_at(horizon_field_capacity           ,layer);
   insert_horizon_at(horizon_water_pot_at_FC          ,layer);
   insert_horizon_at(horizon_sand                     ,layer);
   insert_horizon_at(horizon_clay                     ,layer);
   insert_horizon_at(horizon_silt                     ,layer);
   insert_horizon_at(horizon_sat_hydraul_cond_m_d     ,layer);
   insert_horizon_at(horizon_saturation_WC            ,layer);                   //080313
   insert_horizon_at(horizon_air_entry_pot            ,layer);
   insert_horizon_at(horizon_Campbell_b               ,layer);
   insert_horizon_at(horizon_pH                       ,layer);
   insert_horizon_at(horizon_cation_exchange_capacity ,layer);
   insert_horizon_at(horizon_organic_matter           ,layer);                   //060130
   insert_horizon_at(horizon_organic_matter_low       ,layer);                   //070607
   insert_horizon_at(horizon_organic_matter_high      ,layer);                   //070607
   NH4_Langmuir.insert_at(layer);                                                //190404
   /*190404
   insert_horizon_at(horizon_NH4_Langmuir_K           ,layer);                   //190402
   insert_horizon_at(horizon_NH4_Langmuir_Q           ,layer);                   //190402
   */
   //NYN /*080516_*/    insert_horizon_at(water_holding_capacity_m,layer);
   return ::Soil_parameters_class_common::insert_horizon( layer);                                                                  //050311
}
//_insert_horizon___________________________________________________2004-12-14_/
void Soil_parameters_class::copy_horizon( nat8 from_horizon, nat8 to_horizon)
{  // Don't copy thickness (this should be assigned when copied
   this->horizon_sand               [to_horizon] = this->horizon_sand               [from_horizon];   //000327
   this->horizon_clay               [to_horizon] = this->horizon_clay               [from_horizon];   //000327
   this->horizon_silt               [to_horizon] = this->horizon_silt               [from_horizon];   //000327
   this->horizon_pH                         [to_horizon] = this->horizon_pH         [from_horizon];   //051201
   this->horizon_organic_matter             [to_horizon] = this->horizon_organic_matter             [from_horizon];   //060130
   this->horizon_organic_matter_low         [to_horizon] = this->horizon_organic_matter_low         [from_horizon];   //070607
   this->horizon_organic_matter_high        [to_horizon] = this->horizon_organic_matter_high        [from_horizon];   //070607
   this->horizon_cation_exchange_capacity   [to_horizon] = this->horizon_cation_exchange_capacity   [from_horizon];   //051201
   this->horizon_bulk_density_g_cm3 [to_horizon] = this->horizon_bulk_density_g_cm3 [from_horizon];   //060131
   this->horizon_permanent_wilt_point[to_horizon]= this->horizon_permanent_wilt_point[from_horizon];  //060131
   this->horizon_field_capacity     [to_horizon] = this->horizon_field_capacity     [from_horizon];   //060131
   this->horizon_water_pot_at_FC    [to_horizon] = this->horizon_water_pot_at_FC            [from_horizon];   //130128
   this->horizon_sat_hydraul_cond_m_d       [to_horizon] = this->horizon_sat_hydraul_cond_m_d       [from_horizon];      //060131
   this->horizon_saturation_WC      [to_horizon] = this->horizon_saturation_WC      [from_horizon];      //080313
   this->horizon_air_entry_pot      [to_horizon] = this->horizon_air_entry_pot      [from_horizon];      //060131
   this->horizon_Campbell_b         [to_horizon] = this->horizon_Campbell_b         [from_horizon];      //060131
   this->NH4_Langmuir.copy(from_horizon,to_horizon);                             //190404

//NYN /*080516_*/    this->water_holding_capacity_m   [to_horizon] = this->water_holding_capacity_m   [from_horizon];
}
//_copy_horizon________________________________________________________________/
float32 Soil_parameters_class::set_horizon_thickness
(nat8 horizon,float32 horizon_thickness_new)
{  horizon_thickness[horizon] = horizon_thickness_new;
   return horizon_thickness_new;
}
//_set_horizon_thickness____________________________________________2017-06-09_/
}//_namespace version4_________________________________________________________/

namespace version5
{
//______________________________________________________________________________
Soil_parameters_class::Soil_parameters_class()
: ::Soil_parameters_class_common()
{
   properties.sand.set_default_value(60);
   properties.clay.set_default_value(20);
   properties.silt.set_default_value(20);

   /* shouldn't be needed with default value

   for (int8 layer = 1; layer <= MAX_soil_horizons ; layer++)
   {  //  Need some default values or the soil editor will crash when it tries to update soil properties 060620_
      properties.sand.set(layer,60);                                                  //060620
      properties.clay.set(layer,20);                                                  //060620
      properties.silt.set(layer,20);                                                  //060620
   }
   */
}
//_Soil_parameters_class:constructor___________________________________________/
Soil_parameters_class::Horizon_properties::Horizon_properties()
: thickness                (5,5)
, bulk_density_g_cm3       (5,5)
, permanent_wilt_point     (5,5)
, field_capacity           (5,5)
, water_pot_at_FC          (5,5)
, sand                     (5,5)
, clay                     (5,5)
, silt                     (5,5)
, sat_hydraul_cond_m_d     (5,5)
, saturation_WC            (5,5)
, air_entry_pot            (5,5)
, Campbell_b               (5,5)
, pH                       (5,5)
, cation_exchange_capacity (5,5)
, organic_matter           (5,5)
, organic_matter_low       (5,5)
, organic_matter_high      (5,5)
, bypass_coef              (5,5)
/*190404 abandoned
, NH4_Langmuir_K           (5,5)                                                 //190402
, NH4_Langmuir_Q           (5,5)                                                 //190402
*/
{}
//_Horizon_properties:constructor______________________________________________/
bool Soil_parameters_class::expect_structure(bool for_write)
{
   bool expected = ::Soil_parameters_class_common::expect_structure(for_write);         //161025
   structure_defined = false;

   set_current_section(LABEL_soil);
   expect_dynamic_float32_array(LABEL_thickness       ,properties.thickness               ,horizon_count,1,UC_meter,             false);
   expect_dynamic_float32_array(LABEL_bypass_coef     ,properties.bypass_coef             ,horizon_count,1,UC_coefficient       ,false);
   expect_dynamic_float32_array(LABEL_bulk_density    ,properties.bulk_density_g_cm3      ,horizon_count,1,UC_g_per_cm3         ,false);
   expect_dynamic_float32_array(LABEL_perm_wilt_point ,properties.permanent_wilt_point    ,horizon_count,1,UC_m3_per_m3         ,false);
   expect_dynamic_float32_array(LABEL_field_capacity  ,properties.field_capacity          ,horizon_count,1,UC_m3_per_m3         ,false);
   expect_dynamic_float32_array("water_pot_at_FC"     ,properties.water_pot_at_FC         ,horizon_count,1,UC_kPa               ,false); //130228

  // Sand Clay Silt by layer added
   expect_dynamic_float32_array(LABEL_sand            ,properties.sand                    ,horizon_count,1,UC_percent           ,false);
   expect_dynamic_float32_array(LABEL_clay            ,properties.clay                    ,horizon_count,1,UC_percent           ,false);
   expect_dynamic_float32_array(LABEL_silt            ,properties.silt                    ,horizon_count,1,UC_percent           ,false);
   expect_dynamic_float32_array(LABEL_sat_hydraul_cond,properties.sat_hydraul_cond_m_d    ,horizon_count,1,UC_m_per_day         ,false);
   expect_dynamic_float32_array("saturation"          ,properties.saturation_WC           ,horizon_count,1,UC_m3_per_m3         ,false);

   expect_dynamic_float32_array(LABEL_air_entry_pot   ,properties.air_entry_pot           ,horizon_count,1,UC_J_per_kg          ,false);
   expect_dynamic_float32_array(LABEL_b               ,properties.Campbell_b              ,horizon_count,1,UC_factor            ,false);
   expect_dynamic_float32_array(LABEL_pH              ,properties.pH                      ,horizon_count,1,UC_pH                ,false);
   expect_dynamic_float32_array(LABEL_cation_exchange_capacity,properties.cation_exchange_capacity ,horizon_count,1,UC_mEq_per_100g,false);
   expect_dynamic_float32_array("organic_matter"      ,properties.organic_matter          ,horizon_count,1,UC_percent           ,false);
   expect_dynamic_float32_array("organic_matter_low"  ,properties.organic_matter_low      ,horizon_count,1,UC_percent           ,false);
   expect_dynamic_float32_array("organic_matter_high" ,properties.organic_matter_high     ,horizon_count,1,UC_percent           ,false);
   expect_dynamic_float32_array("NH4/Langmuir/K"      ,/*properties.*/NH4_Langmuir.K          ,horizon_count,1,UC_kg_kg             ,false);
   expect_dynamic_float32_array("NH4/Langmuir/Q"      ,/*properties.*/NH4_Langmuir.Q          ,horizon_count,1,UC_kg_kg             ,false);
   return expected;                                                              //161025
}
//_expect_structure_________________________________________________2016-09-15_/
void Soil_parameters_class::set_horizons_thickness(soil_horizon_array32(_horizon_thickness_32))
{  // This is only used for GIS projects to override soil thickness for polygons/cells
   for ( nat8 layer = 0; layer <= horizon_count; layer++)
      if (!CORN::is_zero<float32>(_horizon_thickness_32[layer]))
         properties.thickness.set(layer,_horizon_thickness_32[layer]);
}
//_set_horizons_thickness___________________________________________1999-09-29_/
bool Soil_parameters_class::delete_horizon( nat8 horizon)
{  // The soil must have at least 1 layer
   if (horizon_count == 1) return false;
   properties.thickness                .remove_at(horizon);
   properties.bypass_coef              .remove_at(horizon);
   properties.bulk_density_g_cm3       .remove_at(horizon);
   properties.permanent_wilt_point     .remove_at(horizon);
   properties.field_capacity           .remove_at(horizon);
   properties.water_pot_at_FC          .remove_at(horizon);
   properties.sand                     .remove_at(horizon);
   properties.clay                     .remove_at(horizon);
   properties.silt                     .remove_at(horizon);
   properties.sat_hydraul_cond_m_d     .remove_at(horizon);
   properties.saturation_WC            .remove_at(horizon);
   properties.air_entry_pot            .remove_at(horizon);
   properties.Campbell_b               .remove_at(horizon);
   properties.pH                       .remove_at(horizon);
   properties.cation_exchange_capacity .remove_at(horizon);
   properties.organic_matter           .remove_at(horizon);
   properties.organic_matter_low       .remove_at(horizon);
   properties.organic_matter_high      .remove_at(horizon);
   NH4_Langmuir             .remove_at(horizon);                      //190404
//NYN /*080516_*/    delete_horizon_at(water_holding_capacity_m         ,horizon);
  return ::Soil_parameters_class_common::delete_horizon(horizon);
}
//_delete_horizon___________________________________________________2004-12-14_/
bool Soil_parameters_class::insert_horizon( nat8 layer)
{  if (horizon_count == MAX_soil_horizons) return false;
   assert(false); // NYI
/*160915 NYI I simply need to add insert_at to Dynamic_array
  // This isn't needed until I reimplement soil editor
   properties.thickness                .insert_at(layer);
   properties.bypass_coef              .insert_at(layer);
   properties.bulk_density_g_cm3       .insert_at(layer);
   properties.permanent_wilt_point     .insert_at(layer);
   properties.field_capacity           .insert_at(layer);
   properties.water_pot_at_FC          .insert_at(layer);
   properties.sand                     .insert_at(layer);
   properties.clay                     .insert_at(layer);
   properties.silt                     .insert_at(layer);
   properties.sat_hydraul_cond_m_d     .insert_at(layer);
   properties.saturation_WC            .insert_at(layer);
   properties.air_entry_pot            .insert_at(layer);
   properties.Campbell_b               .insert_at(layer);
   properties.pH                       .insert_at(layer);
   properties.cation_exchange_capacity .insert_at(layer);
   properties.organic_matter           .insert_at(layer);
   properties.organic_matter_low       .insert_at(layer);
   properties.organic_matter_high      .insert_at(layer);
*/
//NYN //080516   properties.water_holding_capacity_m.insert_at(layer);
   return ::Soil_parameters_class_common::insert_horizon(layer);                                                                  //050311
}
//_insert_horizon___________________________________________________2004-12-14_/
float32 Soil_parameters_class::set_horizon_thickness
(nat8 horizon,float32 horizon_thickness_new)
{  properties.thickness.set(horizon,horizon_thickness_new);
   return horizon_thickness_new;
}
//_set_horizon_thickness____________________________________________2017-06-09_/
}//_namespace version5_________________________________________________________/
