#include "crop/transpiration_dependent_growth_V4.h"
#include <math.h>
#ifndef moremathH
#  include "corn/math/moremath.h"
using namespace std;
#endif
#include "corn/math/compare.hpp"
#include "corn/measure/measures.h"

namespace CropSyst {
//______________________________________________________________________________
Biomass_production_transpiration_dependent_V4
::Biomass_production_transpiration_dependent_V4
(const Crop_parameters                 &parameters_
,const Vapor_pressure_deficit_daytime  &daytime_vapor_pressure_deficit_
,const float64                         &CO2_TUE_adjustment_
,const bool                            &before_WUE_change_                       //180111
)
: Biomass_production_transpiration_dependent_common        (CO2_TUE_adjustment_)
, parameters                                                       (parameters_)
, daytime_vapor_pressure_deficit               (daytime_vapor_pressure_deficit_)
, before_WUE_change                                         (before_WUE_change_) //180111
{}
//_Biomass_production_transpiration_dependent_V4:constructor___________________/
float64 Biomass_production_transpiration_dependent_V4::get_transpiration_use_efficiency_unadj() const
{  float64 sane_daytime_vapor_pressure_deficit =
      CORN::must_be_greater_or_equal_to<float64>
         (daytime_vapor_pressure_deficit.get_kPa(),0.00001);
   float64 transpiration_use_efficiency = 0.0;                                         //080505

      float64 scaling_coef =  get_TUE_scaling_coef(before_WUE_change);                 //080428
         /// unitless   b parameter is a scaling coef.
      float64 TUE_at_1kPa_VPA = get_TUE_at_1kPa_VPD(before_WUE_change);                //080428
            // g biomass / kg water
            // Although TUE_at_1kPa_VPA is in g BM/kg water                            //080429
            // transpiration is in 1000's kg                                           //080429
            // so mathmatically the units of the equation works out the same           //080429
            // No need to convert units of TUE_at_1kPa_VPA                             //080429
      float64 scaling_factor =  pow(sane_daytime_vapor_pressure_deficit,-scaling_coef);//080505
      transpiration_use_efficiency // kg biomass / kg H2O                              //080428
         = g_to_kg(TUE_at_1kPa_VPA) * scaling_factor;                                  //080428
   return transpiration_use_efficiency; // unadjusted (no CO2 effect);
}
//_get_transpiration_use_efficiency_unadj___________________________2015-10-29_/
// 151101 caller may be able to just get the parameter directly
float64 Biomass_production_transpiration_dependent_V4::get_TUE_at_1kPa_VPD(bool before_WUE_change) const
{  float64 TUE_at_1kPa = parameters.param_TUE_at_1kPa_VPD(before_WUE_change );
   return TUE_at_1kPa;
}
//_get_TUE_at_1kPa_VPD______________________________________________2008-04-28_/
float64 Biomass_production_transpiration_dependent_V4::get_TUE_scaling_coef(bool before_WUE_change) const
{  float64 TUE_scaling_coef = parameters.param_TUE_scaling_coef (before_WUE_change);
   return TUE_scaling_coef ;
}
//_get_TUE_scaling_coef_____________________________________________2009-12-01_/
}//_namespace CropSyst_________________________________________________________/

