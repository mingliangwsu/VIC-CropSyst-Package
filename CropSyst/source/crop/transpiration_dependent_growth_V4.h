#ifndef transpiration_dependent_growth_V4H
#define transpiration_dependent_growth_V4H
#include "crop/transpiration_dependent_growth.h"

namespace CropSyst {
//______________________________________________________________________________
class Biomass_production_transpiration_dependent_V4
: public extends_ Biomass_production_transpiration_dependent_common
{
   const Crop_parameters  &parameters;      // Owned by crop object
   // Currently this includes all the CropSyst crop parameters
   // But I should narrow down to just the parameters relevent to attainable growth.
   // TUE = transpiration use efficiency

   const Vapor_pressure_deficit_daytime &daytime_vapor_pressure_deficit;
      // used by TUE curve and Tanner-Sinclair models (not AquaCrop)

   const bool &before_WUE_change;                                                //180111
 public:
   Biomass_production_transpiration_dependent_V4
      (const Crop_parameters                 &parameters_
      ,const Vapor_pressure_deficit_daytime  &daytime_vapor_pressure_deficit_
      ,const float64                         &CO2_TUE_adjustment_
      ,const bool                            &before_WUE_change_);               //180111
 public:
 protected:
   float64 get_TUE_at_1kPa_VPD                   (bool before_WUE_change) const; //080428
   float64 get_TUE_scaling_coef                  (bool before_WUE_change) const; //091201
   virtual float64 get_transpiration_use_efficiency_unadj()               const; //151029
};
//_Biomass_production_transpiration_dependent_V4____________________2008-07-21_/
}
#endif

