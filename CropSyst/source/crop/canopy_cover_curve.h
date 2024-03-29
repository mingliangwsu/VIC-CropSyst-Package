#ifndef canopy_cover_curveH
#define canopy_cover_curveH
#include "corn/primitive.h"
#include "corn/const.h"
#include "crop/phenology_I.h"

namespace CropSyst
{
//______________________________________________________________________________
interface_ Canopy_cover_curve
{  // This is the standard (green) growth curve
   virtual bool parameterize
      (float64 cover_maximum_CO2_adj
      ,float64 initial_cover)                                 initialization_=0;
   virtual bool know_accrescence  (const Phenology::Period_thermal *accr)modification_=0;
   virtual bool know_senescence   (const Phenology::Period_thermal *sene)modification_=0;
   virtual bool know_culminescence(const Phenology::Period_thermal *culm)modification_=0;
   // virtual bool know_N_leaf_stress_factor(float64 N_leaf_stress_factor_)    = 0; //200528      // The know methods return true if the period is set not null
   virtual float64 calculate()                                   calculation_=0;
   virtual float64 set_cover_at_senescence
      (float64 CC_at_senescence)                                 assignation_=0;

};
//_interface_Canopy_cover_curve_____________________________________2017-12-20_/
class Canopy_cover_curve_common   // rename to Canopy_cover_curve_abstract
: public implements_ Canopy_cover_curve
{protected:
   const Phenology::Period_thermal *accrescence;
   const Phenology::Period_thermal *culminescence;
   const Phenology::Period_thermal *senescence;                                          //181108
 protected: // parameters
   const float32 &canopy_cover_season_end;                                       //171009
   float64 cover_maximum_CO2_adj;                                                //141123
   float64 cover_initial;
 public: // structors
   Canopy_cover_curve_common
      (const float32 &canopy_cover_season_end);
 public:
   virtual bool parameterize
      (float64 cover_maximum_CO2_adj
      ,float64 cover_initial)                                   initialization_;
 public:
   virtual float64 calculate()                                   calculation_;
   virtual float64 calc_during_accrescence()                     calculation_=0;
   virtual float64 calc_during_culminescence()                   calculation_=0;
   virtual float64 calc_during_senescence()                      calculation_=0;
   virtual bool know_accrescence  (const Phenology::Period_thermal *accr) modification_;
   virtual bool know_culminescence(const Phenology::Period_thermal *culm) modification_;
   virtual bool know_senescence   (const Phenology::Period_thermal *sene) modification_;
};
//_class Canopy_cover_curve_common__________________________________2017-12-20_/
class Canopy_cover_curve_2017
: public extends_ Canopy_cover_curve_common
{
 protected:
   float64 B1;
   float64 B2;
   float64 shape_coef_vegetative;   // rename shape_coef_accrescent
   float64 shape_coef_senescent;
   float64 CCmax_asymptotic;
   float64 CCmax2_actual;
   float64 CCend_asymptotic; // at end season

   const float32 &thermal_time_fractional_at_half_cover_max;                     //181015
   const float32 &thermal_time_fractional_at_half_cover_senescence;              //181015
 protected:
   mutable float64 cover_current; // standard curve green
 public: // structors
   Canopy_cover_curve_2017
      (const float32 &canopy_cover_season_end_
      ,float64 shape_coef_accrescent_
      ,float64 shape_coef_senescent_
      ,const float32 &thermal_time_fractional_at_half_cover_max_                 //181015
      ,const float32 &thermal_time_fractional_at_half_cover_senescence_);        //181015
 public: // Canopy_cover_curve implementation
   virtual bool parameterize
      (float64 cover_maximum_CO2_adj
      ,float64 cover_initial)                                   initialization_;
   virtual float64 set_cover_at_senescence
      (float64 CC_at_senescence)  assignation_
      {  return cover_current = CC_at_senescence; }

   /*200306
   virtual float64 set_cover_at_max_canopy
      (float64 CC_at_max_canopy)  assignation_
      {  UNUSED_arg(CC_at_max_canopy); return 0.0;}
      // This class doesn't used cover_at_max_canopy
      // if Canopy_cover_curve_2013 is removed, this function can be removed
   */

   virtual bool know_accrescence  (const Phenology::Period_thermal *accr) modification_; //181230
   virtual bool know_culminescence(const Phenology::Period_thermal *culm) modification_; //181230
   virtual bool know_senescence   (const Phenology::Period_thermal *sene) modification_;
 public: // Canopy_cover_curve_common implementation
   virtual float64 calc_during_accrescence()                       calculation_;
   virtual float64 calc_during_culminescence()                     calculation_;
   virtual float64 calc_during_senescence()                        calculation_;
};
//_Canopy_cover_curve_2017_class____________________________________2017-12-20_/
}//_namespace CropSyst_________________________________________________________/
#endif

