#ifndef biomass_growth_RUE_TUEH
#define biomass_growth_RUE_TUEH
#include "crop/biomass_growth.h"
#include "common/weather/parameter/WP_solar_radiation.h"
#ifdef YAML_PARAM_VERSION
#  include "crop/transpiration_dependent_growth.h"
#else
#  include "crop/transpiration_dependent_growth_V4.h"
#endif
#include "corn/math/statistical/stats.h"
#include "common/weather/parameter/WP_air_temperature.h"
namespace CropSyst
{
class Thermal_time_common;
class Crop_CO2_response;
//191025 class Canopy_growth_interface;
class Canopy_leaf_growth;                                                        //191025
class Crop_parameters;
class Biomass_production_transpiration_dependent_V4;
class Biomass_production_transpiration_dependent_V5 ;
//______________________________________________________________________________
class Biomass_growth_RUE_TUE
: public implements_ Biomass_growth
{
   bool                                 is_fruit_tree;                           //191025
   //191025 bool                                 orchard_model;
   const Crop_parameters               *parameters;
   #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
   const Normal_crop_event_sequence    &growth_stage;
   #endif
   const nat8                          &emergence_count;                         //191025
   //191025 const nat8                          &season;

   //101025 const float64                       *pot_transpiration_m; // [2];
   const float64                       &pot_transpiration_m_CO2_adj;             //191025
   //200608 const bool                          &at_max_fruit_load;                       //180308
   #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
   const Thermal_time_common           *thermal_time; // from crop common
   #endif
   const Crop_CO2_response             *CO2_response;
   //191025 const Canopy_growth_interface       *canopy;
   const Canopy_leaf_growth            *canopy;                                  //191025
   const Air_temperature_average       &air_temperature_avg;                     //140225
   const Solar_radiation               &solar_radiation;
   const float64                       &intercepted_radiation;

   #ifdef YAML_PARAM_VERSION
   // Using adjustable parameter idiom
   const CropSyst::Biomass_production_transpiration_dependent_V5 &transpiration_dependent_growth;
   #else
   const CropSyst::Biomass_production_transpiration_dependent_V4 &transpiration_dependent_growth;
   #endif
   mutable CORN::statistical::Sample_cumulative<float64,float64>
      &report_temperature_stress_stats;
 private:
   #if ((PHENOLOGY_VERSION==2018))
   const float64 &accum_deg_days_adjusted_for_clipping;
   #endif
   // These parameters are accessed frequently, so they are dereferenced         //191030
   mutable Efficiency_model efficiency_model;                                                    //191030
   float64 RUE_kg_MJ[2];
   //float64 RUE_efficiency_decrease_rate;                                         //191030
   float64 RUE_efficiency_decrease_rate_kg_m2_d_MJ2;
 public:
   Biomass_growth_RUE_TUE
      (
         bool                              is_fruit_tree_  //  orchard_model_
      ,const Crop_parameters              *parameters_

      #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
      ,const Normal_crop_event_sequence   &growth_stage_
      #endif
      ,const nat8                         &emergence_count_ //191025 was season_
      //191025 ,const float64                      *pot_transpiration_m_ //[2]
      ,const float64                      &pot_transpiration_m_CO2_adj_        //191025
      #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
      ,const Thermal_time_common          *thermal_time_
      #endif
      #if ((PHENOLOGY_VERSION==2018))
      ,const float64                      &accum_deg_days_adjusted_for_clipping_
      #endif

      ,const Crop_CO2_response            *CO2_response_
      ,const Canopy_leaf_growth           *canopy_leaf_growth_
      ,const Air_temperature_average      &air_temperature_avg_
      ,const Solar_radiation              &solar_radiation_
      ,const float64                      &intercepted_radiation_
      #ifdef YAML_PARAM_VERSION
      // Using adjustable parameter idiom
      ,const CropSyst::Biomass_production_transpiration_dependent_V5 &transpiration_dependent_growth_
      #else
      ,const CropSyst::Biomass_production_transpiration_dependent_V4 &transpiration_dependent_growth_
      #endif
      ,CORN::statistical::Sample_cumulative<float64,float64>
           &report_temperature_stress_stats
      //200608  ,const bool                         &is_at_max_fruit_load
      );
   inline virtual bool start()                       modification_{return true;}
   inline virtual bool process_day()                 modification_{return true;}
   virtual float64 calc_attainable_top_growth()                    calculation_; //180308

   // Note that calc_RUE_temperature_limitation is not applicable to Orchard model where it is always 1.0
   // so orchard model should have its own Biomass_growth
   // which overrides calc_RUE_temperature_limitation returning 1.0
 private:
   virtual float64 calc_RUE_temperature_limitation()               calculation_;
   float64 get_RUE_global_kg_MJ()                    const;
};
//_class Biomass_growth_RUE_TUE_____________________________________2018-01-11_/
}//_namespace CropSyst_________________________________________________________/
#endif
