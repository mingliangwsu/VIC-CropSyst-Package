#ifndef yieldH
#define yieldH
#include "corn/const.h"
#include "corn/math/statistical/stats.h"
#include "crop/yield_temperature_extreme_response.h"
#include "crop/yield_response.h"
#include "crop/phenology_I.h"
#include "weather/parameter/WP_temperature.h"
namespace CropSyst {
//______________________________________________________________________________
interface_ Yield_model
   // Cannot use the name Yield because there is a WinAPI macro with name
{public:
   virtual float64 calculate(float64 canopy_biomass_kg_m2)       calculation_=0;
   virtual bool know_stress(float64 water_and_or_nitrogen_stress)   assignation_ { UNUSED_arg(water_and_or_nitrogen_stress);return true; }
   virtual bool end_day()                            modification_{return true;} //200424
   virtual bool know_biomass_at_maturity(float64 maturity_canopy_biomass_kg_m2_) assignation_ { UNUSED_arg(maturity_canopy_biomass_kg_m2_); return true; }
   virtual bool know_biomass_at_filling (float64 maturity_canopy_biomass_kg_m2_) assignation_ { UNUSED_arg(maturity_canopy_biomass_kg_m2_); return true; }
   virtual float64 get_reported_harvest_index()                         const=0;
   virtual bool know_flowering()                                  {return true;}
   virtual bool know_filling()                                    {return true;}
   virtual bool reset()                            initialization_{return true;}
};
//_Yield_model_________________________________________________________________/
class Yield_complete
: public implements_ Yield_model
{
 public:
   inline Yield_complete()
      {}
   inline virtual float64 calculate(float64 canopy_biomass_kg_m2)   calculation_
      {

         // Currently this is doing the same thing as Yield_canopy
         // I need to provide root biomass
         float64 actual_yield = canopy_biomass_kg_m2
            // NYI + root_biomass
            ;
         return actual_yield;
      }
   inline virtual float64 get_reported_harvest_index()     const { return 1.0; }
      // everything is harvested
};
//_Yield_complete___________________________________________________2015-09-29_/
class Yield_unadjusted
: public implements_ Yield_model
{
   float_P param_harvest_ndx_unstressed;
 public:
   inline Yield_unadjusted(float_P _param_harvest_ndx_unstressed)
   : param_harvest_ndx_unstressed(_param_harvest_ndx_unstressed)
   {}
   inline virtual float64 calculate(float64 canopy_biomass_kg_m2)   calculation_
   {
         // reported_harvest_index = param_harvest_ndx_unstressed;
      float64 actual_yield = canopy_biomass_kg_m2 * param_harvest_ndx_unstressed;
      return actual_yield;
   }
   inline virtual float64 get_reported_harvest_index()                     const
      { return param_harvest_ndx_unstressed; }
};
//_Yield_unadjusted____________________________________________________________/
class Yield_abstract
: public implements_ Yield_model
{
 public:
   float_P param_harvest_ndx_unstressed;
   bool  param_apply_adjustment_to_unstressed_harvest_index;
   const Phenology &phenology;  // reference to crop
   contribute_ float64 reported_harvest_index;
 public:
   inline Yield_abstract
      (float_P _param_harvest_ndx_unstressed
      ,bool    _param_apply_adjustment_to_unstressed_harvest_index
      ,const Phenology &_phenology)
      :
      param_harvest_ndx_unstressed   (_param_harvest_ndx_unstressed)
      , param_apply_adjustment_to_unstressed_harvest_index
         (_param_apply_adjustment_to_unstressed_harvest_index)
      , phenology                      (_phenology)
      , reported_harvest_index         (0.0)
      {}
   inline virtual float64 get_reported_harvest_index()                     const
      { return reported_harvest_index; }

 protected:
   float64 sensitivity_effect
      (float64 mean_period_stress
      ,float64 harvest_ndx_period_sensitivity_index)                calculation_;
   inline virtual bool know_biomass_at_filling(float64 filling_canopy_biomass_kg_m2_) assignation_
      { UNUSED_arg(filling_canopy_biomass_kg_m2_); return false; }; // do nothing, used only by Yield_grain
   inline virtual bool know_biomass_at_maturity(float64 maturity_canopy_biomass_kg_m2_) assignation_
      { UNUSED_arg(maturity_canopy_biomass_kg_m2_);return false; }; // do nothing, used only by Yield_grain
};
//_Yield_abstract______________________________________________________________/
class Yield_simple_harvest_index
: public extends_ Yield_abstract
{
   float_P param_growth_sensitivity;
   CORN::statistical::Descriptive_progressive stress_since_emergence_stats;      //200701_010726
 public:
   inline Yield_simple_harvest_index
      (float_P _param_harvest_ndx_unstressed
      ,bool    _param_apply_adjustment_to_unstressed_harvest_index
      ,float_P _param_growth_sensitivity
      ,const Phenology &_phenology)
      : Yield_abstract
         (_param_harvest_ndx_unstressed
         ,_param_apply_adjustment_to_unstressed_harvest_index
         ,_phenology)
      , param_growth_sensitivity(_param_growth_sensitivity)
      {}
   virtual float64 calculate(float64 canopy_biomass_kg_m2)         calculation_;
   inline virtual bool reset()                                  initialization_
      { return stress_since_emergence_stats.reset(); }
   inline virtual bool know_stress(float64 water_and_or_nitrogen_stress) assignation_
      {
         //190707 if (phenology.is_accrescence_initiated()/*190101 has_emerged()*/)
         //190707 check for emergence shouldn't be needed, yield class wouldnot
         // exist unless crop emerged atleast there would be no stress
           stress_since_emergence_stats.append((float_P)water_and_or_nitrogen_stress);
        return true;
      }
 private:
   float64 harvest_index_1_stress_period
      (float64 harvest_ndx_unstressed
      ,float64 mean_period_stress
      ,float64 harvest_ndx_period_sensitivity)                     calculation_;
};
//_Yield_abstract______________________________________________________________/
class Yield_tuber
: public extends_ Yield_abstract
{
   float_P param_tuber_init_sensitivity;
   float_P param_tuber_growth_sensitivity;
   float_P param_tuber_critical_temperature;
   CORN::statistical::Sample_cumulative<float32,float32> tuber_init_temperature_stress_stats;         //010726
      // was float32 could be float64
   CORN::statistical::Sample_cumulative<float32,float32> tuber_growth_temperature_stress_stats;       //010726
      // was float32 could be float64
   const CS::Temperature            &soil_temperature;
 public:
   inline Yield_tuber
      (float_P param_harvest_ndx_unstressed_
      ,bool    param_apply_adjustment_to_unstressed_harvest_index_
      ,float_P param_tuber_init_sensitivity_
      ,float_P param_tuber_growth_sensitivity_
      ,float_P param_tuber_critical_temperature_
      ,const Phenology        &phenology_
      ,const CS::Temperature  &soil_temperature_
      )
      : Yield_abstract
         (param_harvest_ndx_unstressed_
         ,param_apply_adjustment_to_unstressed_harvest_index_
         ,phenology_)
      , param_tuber_init_sensitivity      (param_tuber_init_sensitivity_)
      , param_tuber_growth_sensitivity    (param_tuber_growth_sensitivity_)
      , param_tuber_critical_temperature  (param_tuber_critical_temperature_)
      , tuber_init_temperature_stress_stats  (STAT_arithmetic_mean_bit)          //160803
      , tuber_growth_temperature_stress_stats(STAT_arithmetic_mean_bit)          //160803
      , soil_temperature                  (soil_temperature_)
      {}
   virtual float64 calculate(float64 canopy_biomass_kg_m2)         calculation_;
   inline virtual bool reset()                                   initialization_
      { return tuber_init_temperature_stress_stats.reset()
            && tuber_growth_temperature_stress_stats.reset(); }
    virtual bool know_stress(float64 water_and_or_nitrogen_stress) assignation_;
 protected:
   float64 sensitivity_effect_B
      (float64 mean_period_stress
      ,float64 harvest_ndx_period_sensitivity)                     calculation_;
      // Currently only used in tuber but was common to hold harvest index calculation
   float64 initiation_temperature_stress_factor()                  calculation_;
   float64 bulking_temperature_stress_factor()                     calculation_;
};
//_Yield_tuber_________________________________________________________________/
class Yield_canopy
: public extends_ Yield_abstract
{
 public:
   inline Yield_canopy
      (float_P _param_harvest_ndx_unstressed
      ,bool    _param_apply_adjustment_to_unstressed_harvest_index
      ,const Phenology &_phenology)
      : Yield_abstract
         (_param_harvest_ndx_unstressed
         ,_param_apply_adjustment_to_unstressed_harvest_index
         ,_phenology)
      {}
   virtual float64 calculate(float64 canopy_biomass_kg_m2)         calculation_;
};
//_Yield_canopy________________________________________________________________/
class Yield_grain
: public extends_ Yield_abstract
{
   float_P param_translocation_fraction_max;
   float64 biomass_at_maturity; //kg/m2
   float64 biomass_at_filling;  //kg/m2
 public: // So parameter scheduling can reset
   Yield_grain_temperature_extreme_response *temperature_extreme_reponse;        //200423
   Yield_grain_water_response          *flowering_water_response; //owned        //200423
   Yield_grain_temperature_response    *flowering_heat_response;  //owned        //200423
      // temperature extremes are optional (crop parameeter
 public:
   Yield_grain
      (float_P  param_harvest_ndx_unstressed
      ,bool     param_apply_adjustment_to_unstressed_harvest_index
      ,bool     apply_heat_extreme_adjustment_to_unstressed_harvest_index
      ,float_P  translocation_fraction_max
      ,Yield_grain_water_response          *flowering_water_response_submitted   //200423
      ,Yield_grain_temperature_response    *flowering_heat_response_submitted    //200423
      ,const Phenology &phenology
      ,const CORN::Local_time &ref_sun_rise_time
      ,const CORN::Dynamic_array<float32> &stress_adj_hourly_temperature);
   virtual ~Yield_grain();
   virtual float64 calculate(float64 canopy_biomass_kg_m2)         calculation_;
   inline virtual bool know_biomass_at_filling
      (float64 _filling_canopy_biomass_kg_m2)                      assignation_
      { biomass_at_filling = _filling_canopy_biomass_kg_m2; return true; }
   inline virtual bool know_biomass_at_maturity
      (float64 _maturity_canopy_biomass_kg_m2)                     assignation_
      { biomass_at_maturity = _maturity_canopy_biomass_kg_m2; return true; }
   virtual bool know_stress(float64 water_and_or_nitrogen_stress)  assignation_;
   virtual bool know_flowering()                                              ;
   virtual bool know_filling()                                                ;
   virtual bool end_day()                                         modification_; //200423
};
//_Yield_grain_________________________________________________________________/
}//_namespace CropSyst_________________________________________________________/
#endif

