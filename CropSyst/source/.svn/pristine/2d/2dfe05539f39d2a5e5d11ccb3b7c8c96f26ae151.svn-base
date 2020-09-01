#ifndef crop_N_V5H
#define crop_N_V5H
#include "crop/crop_N_common.h"
namespace CropSyst
{
//______________________________________________________________________________
class Crop_nitrogen_V5
: public extends_ Crop_nitrogen_common
{
   const float64 &transpiration_use_efficiency_CO2_adjusted;
 public:
   Crop_nitrogen_V5
      (Crop_parameters::Nitrogen &_parameters
      ,Crop_complete             &_crop
      ,Soil::Soil_interface      *_soil                                          //181206
      ,Soil::Nitrogen            *_chemicals                                     //181206
      ,const Slope_saturated_vapor_pressure_plus_psychrometric_resistance
         *_slope_saturated_vapor_pressure_plus_psychrometric_resistance_ref
         // although _slope_saturated_vapor_pressure_plus_psychrometric_resistance_reference
         // is optional, if it is not specified, there will be no correction for transpiration
      ,const float64 &transpiration_use_efficiency_CO2_adjusted                  //151104
         // adjusted when CO2 is enabled otherwise unadjusted
      #if ((PHENOLOGY_VERSION==2018))
      ,const float64 &accum_deg_days_adjusted_for_clipping_                      //181118
      #endif
      );
   inline virtual ~Crop_nitrogen_V5() {}                                         //151204
 public:
   virtual float64 update_limited_pot_transpiration()             modification_;
   virtual float64 update_limited_growth
      (float64 attainable_growth_transp_or_RUE
      ,float64 root_activity_factor)                              modification_;
};
//_Crop_nitrogen_V5_________________________________________________2015-10-29_/
}//_namespace CropSyst_________________________________________________________/
#endif

