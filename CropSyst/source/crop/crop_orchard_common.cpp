#include "common/geodesy/geolocation.h"
#include "crop_orchard_common.h"
#include "crop_orchard_Kemanian_Stockle.h"
#include "crop_orchard_Oyarzun_Stockle.h"
#include "crop_param.h"
#include <math.h>
#include "corn/math/compare.h"
//---------------------------------------------------------------------------
#define percent_slope_to_rad(x) asin(x/100)
namespace CropSyst {
//______________________________________________________________________________
Orchard_common::Orchard_common
(float64 kc_
, Interception_architectural *interception_
,CropSyst::Crop_parameters_struct::Canopy_architecture &architecture_params_     //110218
, float64 deg_days_flowering
, float64 deg_days_begin_rapid_fruit_growth)
: Orchard_interface()
, interception(interception_)
, tree_canopy                                                                (0) //200403
, canopy_architectural(0)                                                        //081103
{  if (interception)
   {  tree_canopy = new Tree_canopy_extent
         (architecture_params_,deg_days_flowering
         ,deg_days_begin_rapid_fruit_growth);
      interception->know_tree_canopy(tree_canopy);
   }
}
//_Orchard_common:constructor__________________________________________________/
float64 Orchard_common::get_solid_canopy_interception()                    const
{  // If there is not interception model then interceptions is not a factor
   return interception ? interception->get_solid_canopy_interception() : 1.0 ;
}
//_get_solid_canopy_interception_______________________________________________/
float64 Orchard_common::get_canopy_porosity()                              const
{  return canopy_architectural->get_porosity();
}
//_get_canopy_porosity______________________________________________2008-11-03_/
float64 Orchard_common::get_canopy_shaded_area_GAI()                       const
{  return canopy_architectural->get_shaded_area_GAI();
}
//_get_canopy_shaded_area_GAI_______________________________________2008-11-03_/
float64 Orchard_common::update_solid_canopy_interception()        rectification_
{  float64 solid_canopy_interception
   = (interception)
   ?  interception->update_solid_canopy_interception()                           //151027
   : 1.0;
   canopy_architectural->know_solid_canopy_interception(solid_canopy_interception); //081103
   return solid_canopy_interception;
}
//_update_solid_canopy_interception____________________________________________/
Orchard_common::Tree_canopy_extent::Tree_canopy_extent
(CropSyst::Crop_parameters_struct::Canopy_architecture &architecture             //110218
, float64 _deg_days_begin_canopy_development
, float64 _deg_days_end_vegetative_growth)
: deg_days_begin_canopy_development (_deg_days_begin_canopy_development)
, deg_days_end_vegetative_growth    (_deg_days_end_vegetative_growth)
, width  (architecture.initial_width  ,architecture.final_width  ,deg_days_begin_canopy_development,deg_days_end_vegetative_growth)
, breadth(architecture.initial_breadth,architecture.final_breadth,deg_days_begin_canopy_development,deg_days_end_vegetative_growth)
, height (architecture.initial_height ,architecture.final_height ,deg_days_begin_canopy_development,deg_days_end_vegetative_growth)
{
}
//_Tree_canopy_extent__________________________________________________________/
Orchard_common::Tree_canopy_extent::Dimension::Dimension
( float64 _initial_size
, float64 _final_size
, float64 &_deg_days_begin_canopy_development
, float64 &_deg_days_end_vegetative_growth)
: current            (_initial_size)
, initial            (_initial_size)
, final              (_final_size)
, deg_days_begin_canopy_development (_deg_days_begin_canopy_development)
, deg_days_end_vegetative_growth    (_deg_days_end_vegetative_growth)
{ }
//_Tree_canopy_extent::Dimension::Dimension____________________________________/
float64 Orchard_common::Tree_canopy_extent::Dimension::expand(float64 deg_days_today) modification_
{
   if (   (deg_days_today > deg_days_begin_canopy_development)
       && (deg_days_today < deg_days_end_vegetative_growth))                     //071117
   {  float64 change =                                                           //000717
        ((final - initial)                                                       //000717
         *(deg_days_today - deg_days_begin_canopy_development)    // When leaf starts //000717
         /(deg_days_end_vegetative_growth - deg_days_begin_canopy_development)); //000717
      current = initial + change;                                                //071117
   } // else keep current                                                        //071117
   return current;                                                               //000717
}
//_Tree_canopy_extent::Dimension::expand_______________________________________/
void Orchard_common::expand_canopy
(float64 deg_days_today)
{  if (tree_canopy) tree_canopy->expand(deg_days_today);                         //200403
}
//_expand_canopy_______________________________________________________________/
void Orchard_common::Tree_canopy_extent::expand(float64 deg_days_today) modification_
{  width  .expand(deg_days_today);
   breadth.expand(deg_days_today);  // was col_width
   height .expand(deg_days_today);
}
//_Tree_canopy_extent::expand__________________________________________________/
void Orchard_common::Tree_canopy_extent::set_deg_days_begin_canopy_development
(float64 deg_days_begin_canopy_development_)                        assignation_
{  deg_days_begin_canopy_development = deg_days_begin_canopy_development_;
}
//_set_deg_days_begin_canopy_development____________________________2007-11-21_/
void Orchard_common::set_deg_days_begin_canopy_development
(float64 deg_days_begin_canopy_development_)                        assignation_
{  if (tree_canopy)
      tree_canopy->set_deg_days_begin_canopy_development
         (deg_days_begin_canopy_development_);
}
//_set_deg_days_begin_canopy_development____________________________2007-11-21_/
Orchard_common::Interception_architectural::Interception_architectural
(CropSyst::Crop_parameters_struct::Layout &layout_                               //110218
,CropSyst::Crop_parameters_struct::Canopy_architecture &architecture_            //110218
,float64          slope_percent_
,float64          slope_aspect_N0_E90_degrees_
,const Sun_days   &sun_days_         // Provided by weather/meteorology
,const float64    &fract_cloud_cover_
,const CORN::Date_const &today_)
: layout                                                               (layout_)
, architecture                                                   (architecture_)
, sun_days                                                           (sun_days_)
, fract_cloud_cover                                         (fract_cloud_cover_) //151027
, today                                                                 (today_) //151027
{
   slope_radians = percent_slope_to_rad(slope_percent_);
   row_azimuth_radians =deg_to_rad(convert_aspect_from_0_north_with_east_90_to_0_south_with_east_90(layout_.row_azimuth_true_north_deg));  //071117
   // We need to convert Aspect_N0_E90_degrees
   // to the orientation use by this model.
   // Armen confirmed that this works for southern hemisphere                    //020614
   // When negative is used for southern hemisphere.
   float32 slope_aspect_S0_E90_degrees = convert_aspect_from_0_north_with_east_90_to_0_south_with_east_90(slope_aspect_N0_E90_degrees_);   //020612
   slope_aspect_from_south_radians = deg_to_rad(slope_aspect_S0_E90_degrees);
}
//_Interception:constructor____________________________________________________/
float64 Orchard_common::Interception_architectural
::convert_aspect_from_0_north_with_east_90_to_0_south_with_east_90(float64 aspect_N0_E90_degrees) const
{
   float32 aspect_S0_E90_degrees =                                               //020612
      (aspect_N0_E90_degrees < 180.0)   ? (180.0 - aspect_N0_E90_degrees)
   /* (aspect_N0_E90_degrees >=180.0)*/ : (540.0 - aspect_N0_E90_degrees);
   return aspect_S0_E90_degrees;
}
//_convert_aspect_from_0_north_with_east_90_to_0_south_with_east_90____________/
#ifdef NYN
bool Orchard_common::process_day()                               modification_
{
   expand_dimensions()
   if (orchard_interception) orchard_interception->update_day();
}
#endif
//_process_day_________________________________________________________________/
Orchard_common::Canopy_architectural::Canopy_architectural
(float64 light_extinction_coef_)
: canopy_leaf_growth                                                         (0)
, light_extinction_coef                                 (light_extinction_coef_) //191106_180618
, shaded_area_GAI                                                          (0.0) //081103
, porosity                                                                 (0.0) //081103
{}
//______________________________________________________________________________
float64 Orchard_common::Canopy_architectural::calc_cover_fract
(float64 xAI    // Either LAI or  GAI
,bool photosynthetic_active_rad_based
,bool use_clumping_factor_unused
,bool record_shading_porosity_interception                                       //081104
) const
{
   float64 kc_local =
      /*200115
    #ifdef OLD_PROVIDE_CANOPY
      #if (CROPSYST_VERSION == 4)
         crop_parameters.param_kc();
      #else
         crop_parameters.morphology.kc;                                          //130406
      #endif
      crop_parameters.morphology.kc;                                             //130406
    #else
      light_extinction_coef; //191106 kc                                         //180618
    #endif
      */

      light_extinction_coef;                                                     //180618

    //  Correct kc parameter which is total radiation based.                     //011022
    //  Kc based on photosynthetic active radiation is needed
   if (photosynthetic_active_rad_based)                                          //011108
      kc_local = std::min<float64>(kc_local * 1.4,0.9);                          //000716
   // kc will be total radiation based entered in the parameter editors.
   float64 local_shaded_area_GAI = (xAI / solid_canopy_interception);
      // Eventually move this to orchard submodel //081020_
   float64 local_porosity =   exp(-kc_local * local_shaded_area_GAI);            //071121
   if (record_shading_porosity_interception)                                     //081104
   {  shaded_area_GAI  = local_shaded_area_GAI;                                  //081104
      porosity         = local_porosity;                                         //081104
   }
   float64 canopy_interception  =
      (xAI > 0.00001)                                                            //071121
      ? (1.0 - local_porosity)                                                   //071121
      : 0.0;
   float64 result_fract_canopy_cover =  solid_canopy_interception *canopy_interception;
      // canopy_porosity really only applies to fruit canopy since solid_canopy_interception in 1.0 for field crops //071121_
   return  result_fract_canopy_cover;                                            //071121
}
//_calc_cover_fract_________________________________________________2008-10-22_/
bool Orchard_common::know_canopy_leaf
(const Canopy_leaf_growth *canopy_leaf_growth)
{
   if (!canopy_architectural)
        canopy_architectural = new Canopy_architectural(light_extinction_coef);
   canopy_architectural->know_canopy_leaf(canopy_leaf_growth);
    return canopy_architectural;
}
//_know_canopy_leaf_________________________________________________2019-06-25_/
bool Orchard_common::drop_leaves()
{  return canopy_architectural
   ? canopy_architectural->drop_leaves()
   : true;
}
//_drop_leaves______________________________________________________2019-06-25_/
}//_namespace_CropSyst_________________________________________________________/

