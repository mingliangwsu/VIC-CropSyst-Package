#ifndef VIC_crop_rootH
#define VIC_crop_rootH
//---------------------------------------------------------------------------
#if (VIC_CROPSYST_VERSION >= 3)
#  error This class is obsolete in version 3 
/*
Version 3 now uses VIC_CropSyst_proper_crop
where the Crop class is based on CropSyst proper.
*/
#else

#include "corn/primitive.h"
#include "corn/const.h"
#ifndef  VIC_crop_paramH
#  include "VIC_crop_param.h"
#endif
#include "soil/layers_interface.h"
namespace VIC_crop
{
//______________________________________________________________________________
class Root
{
protected:  // external references
   const Root_parameters       &parameters;
   const Phenology_parameters  &phenology_parameter;
   const Soil_layers_interface *soil_layers;
   bool                         perennial;                                       //110219
public:
   Root  (const Root_parameters       &_parameters
         ,const Phenology_parameters  &_phenology_parameter
         ,bool                         _perennial);
   void start();
   void update(nat16 DaysAfterEmergence,bool has_above_ground_biomass)  modification_;
   inline float64 get_root_fraction(nat8 layer)                            const { return root_fraction[layer]; } 

   void assign_references
      (const float64 */*140810  Ref_to_const_float*/ _thermal_time_cumulative
//now passing has_above_ground_biomass      , Ref_to_const_float_param _PotentialCumulativeBiomass
      , const float64 */*140810  Ref_to_const_float*/  _WaterStressFactor_yesterday  // this could actually be simply WaterStressFactor that hasn't been updated yet
      );
   void know(const Soil_layers_interface         *_soil_layers);
private: // external references
    const float64 */*140810  Ref_to_const_float*/  thermal_time_cumulative;
    //obs was only needed to check if has_above_ground_biomass Ref_to_const_float PotentialCumulativeBiomass;
    const float64 */*140810  Ref_to_const_float*/  WaterStressFactor_yesterday;
private:
//   nat8 number_of_soil_layers;
   float64 root_depth_current;
   soil_layer_array64(root_fraction);
   float64 max_seasonal_root_depth_gain;
   float64 normalized_cum_thermal_time_today;
   float64 normalized_cum_thermal_time_yesterday;
public: // accessor for output
   float64 get_depth()  const { return root_depth_current; } 
}; 
//______________________________________________________________________________
}  // namespace VIC_crop
#endif
#endif
