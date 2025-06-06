#include "soil_param_V4.h"
#include <corn/measure/units.ph>
#include "static_phrases.h"

#include "common/weather/units.ph"
#include "corn/validate/validtyp.h"
#include "corn/math/compare.hpp"
//______________________________________________________________________________
Soil_parameters::Soil_parameters()
: version4::Soil_parameters_class()                                              //160915
, old_pH_32                   (0.0) // we use 0.0 as default to indicate that pH is now by layer //011130
, old_cation_exchange_capacity_32(0.0) //we use 0.0 as default to indication that CEC is now by layer.` //011130
, v_override_curve_number     (override_curve_number_16     ,LABEL_override_curve_number  ,0,1,99,0,100,TU_unitless,"SCS curve number override for all other conditions")   //011015
, v_user_fallow_curve_number  (user_fallow_curve_number_16  ,LABEL_fallow_curve_number    ,0,1,99,0,100,TU_unitless,"SCS curve number override for fallow antecedent moisture condition II "/*TL_SCS_Curve_number*/)  //011015
, v_steepness                 (steepness_percent            ,UC_percent                   ,LABEL_steepness     ,2,  1.0, 0.0,100.0,  0.0, 100.0  ,TU_percent,TL_Steepness,"Grade as the tangent of the angle of inclination multiplied by 100 to express as a percentage")
, v_slope_length              (slope_length                 ,UC_meter                     ,LABEL_slope_length  ,2, 20.0, 0.0,200.0,  0.0, 300.0  ,TU_m,TL_Slope_length)
, v_albedo_dry                (albedo_dry                   ,UC_decimal_percent           ,"albedo_dry"        ,3,0.155, 0.05, 0.5,  0.0,   1.0  ,"0-1","Albedo dry") //041203
, v_albedo_wet                (albedo_wet                   ,UC_decimal_percent           ,"albedo_wet"        ,3,0.09 , 0.05, 0.5,  0.0,   1.0  ,"0-1","Albedo wet") //041203
, v_water_vapor_conductance_atmosphere_adj(water_vapor_conductance_atmosphere_adj,UC_adjustment       ,"water_vapor_conductance_atmosphere_adj",5,1.0,0.00001,1.0,0.0,1.0,"0-1","Soil water vapor conductance adjustment")  //041203
#if (CROPSYST_VERSION == 3)
// leaching depth moved to simulation control
, v_leaching_depth            (leaching_depth_32  ,UC_meter   ,LABEL_leaching_depth ,2, 2.0, 0.0,  2.0,  0.0,  10.0  ,TU_m,TL_Depth_where_leaching_is_recorded)
#endif
#if (CROPSYST_VERSION == 3)
// leaching depth moved to simulation control
, leaching_depth_32           (2.0)                                              //981231
, percent_sand_obsolete       (0)                                                //990228
, percent_clay_obsolete       (0)                                                //990228
, percent_silt_obsolete       (0)                                                //990228
, override_leaching_depth     (false)                                            //981231
#endif
, v_surface_storage_mm        (surface_storage_mm_32 ,UC_mm,LABEL_surface_storage,2, 0.0, 0.0, 10.0,  0.0, 100.0  ,TU_mm,TL_Surface_storage )   //990213
, v_SLPF_CropGro              (SLPF_CropGro,       UC_factor,"SLPF",2,1.0,0.0,1.0,0.0,1.0,"Factor","SLPF Soil Limitation? P???? Factor")   //071016
{}
//_Soil_parameters:constructor_________________________________________________/
Soil_parameters::~Soil_parameters()
{}
//_~Soil_parameters_________________________________________________1998-07-22_/
bool  Soil_parameters::set_start()
{  if (description.brief == "")                                                  //990301
       description.brief = "-";                                                  //990301
#ifdef VERSION3
   percent_sand_obsolete = sand_32[1];
   percent_clay_obsolete = clay_32[1];
   percent_silt_obsolete = silt_32[1];
#endif
   //161023 Common_parameters_data_record::set_start();                          //051028
   return version4::Soil_parameters_class::set_start();                          //160123
}
//_set_start________________________________________________________2001-01-09_/
bool Soil_parameters::expect_structure(bool for_write)
{
   bool expected = version4::Soil_parameters_class::expect_structure(for_write); //101010
   structure_defined = false;                                                    //120314
   set_current_section(LABEL_soil);
   expect_valid_float32 (v_steepness);
   expect_valid_float32 (v_slope_length);
   expect_valid_float32 (v_albedo_dry);                                          //041203
   expect_valid_float32 (v_albedo_wet);                                          //041203
   expect_valid_float32 (v_water_vapor_conductance_atmosphere_adj);              //041206
   expect_valid_float32 (v_surface_storage_mm);                                  //990213
   if (!for_write)                                                               //030508
      expect_float32    (LABEL_pH,old_pH_32);                                    //011130
   expect_float32       (LABEL_cation_exchange_capacity,old_cation_exchange_capacity_32); //011130
#ifdef VERSION3
   expect_bool          (LABEL_override_leaching_depth,override_leaching_depth,CORN::VV_bool_entry::form_true ); //020324
   expect_valid_float32 (v_leaching_depth);                                      //981231
   expect_int16         (LABEL_percent_sand,percent_sand_obsolete);
   expect_int16         (LABEL_percent_clay,percent_clay_obsolete);
   expect_int16         (LABEL_percent_silt,percent_silt_obsolete);
#endif
   expect_valid_int16    (v_override_curve_number);                              //150826
      // Not sure why I didn't have this enabled                                 //150826
   expect_valid_int16    (v_user_fallow_curve_number);                           //990213
   set_current_section   ("CropGro");                                            //071016
      expect_valid_float32(v_SLPF_CropGro);                                      //071016
   structure_defined = true;                                                     //120314
   return expected;
}
//_expect_structure_________________________________________________1998-12-12_/
bool Soil_parameters::get_end()
{
//161023 moved to end Common_parameters_data_record::get_end();                  //051028
#ifdef VERSION3
   // These textures are for old version support
   if (!(user[1] & specified_sand) && percent_sand_obsolete)
      sand_32[1] = percent_sand_obsolete;
   if (!(user[1] & specified_clay) && percent_clay_obsolete)
      clay_32[1] = percent_clay_obsolete;
   if (!(user[1] & specified_silt) && percent_silt_obsolete)
      silt_32[1] = percent_silt_obsolete;
#endif
   // We have moved pH and Cation Exchange Capacity to be by layer
   if (!CORN::is_zero<float32>(old_pH_32/*,0.0,0.0001*/))
      for (nat8 layer = 1; layer <= horizon_count; layer++)
          if (CORN::is_approximately<float32>(horizon_pH[layer],DEF_pH,0.00001)
          || (CORN::is_zero<float32>(horizon_pH[layer]/*,0.0,0.00001*/)))
            horizon_pH[layer] = old_pH_32;
   if (!CORN::is_zero<float32>(old_cation_exchange_capacity_32))
      for (nat8 layer = 1; layer <= horizon_count; layer++)
          if (CORN::is_approximately<float32>
               (horizon_cation_exchange_capacity[layer]
               ,DEF_cation_exchange_capacity,0.00001)
           || CORN::is_zero<float32>
               (horizon_cation_exchange_capacity[layer]))
            horizon_cation_exchange_capacity[layer] = old_cation_exchange_capacity_32;
   return Soil_parameters_class_common::get_end();                                      //161023
}
//_get_end______________________________________________2001-11-30__2001-01-09_/
#ifdef VERSION3
This was for old (version2) files that didnt have user specified flags
void Soil_parameters::set_user_specified_flags()
{  for (int layer = 1; layer <= MAX_soil_horizons ; layer++)
       // For an old soil file with out user specified flags
   {  bool any_texture_specified = ((user[layer] & specified_sand) ||
               (user[layer] & specified_clay) ||
               (user[layer] & specified_silt));
      if ((! any_texture_specified && (layer <= number_horizons)))
                 user[layer] |=
                  specified_sand| specified_clay// | specified_silt
                 |specified_FC  | specified_PWP | specified_bulk_density;
   }
}
//_set_user_specified_flags_________________________________________1998-11-04_/
#endif

