#ifndef VIC_cropH
#define VIC_cropH
#if (VIC_CROPSYST_VERSION >= 3)
#  error This class is obsolete, now using Crop class based on CropSyst proper.
#else

#ifndef date_typesHPP
#  include "corn/chronometry/date_types.hpp"
#endif
#include "VIC_crop_common.h"
#include "VIC_crop_root.h"

#if (!VB_VERSION_DATE || (VB_VERSION_DATE >= 20110110))
#include "VIC_crop_transpiration.h"
#else
#include "VIC_crop_transpiration_old.h"
#endif
#include "VIC_crop_phenology.h"
#include "VIC_crop_biomass.h"
#include "crop/canopy_cover_continuum.h"
#if (VIC_CROPSYST_VERSION >= 2)
#  include "weather/parameter/WP_vapor_pressure_saturation_slope.h"
#  include "weather/parameter/WP_psychrometric_const.h"
#endif

#include <fstream>
#include "geodesy/geolocation.h"
#include "weather/solar_rad.h"
#ifdef CROPSYST_PROVIDED_ET
#  include "weather/weather_provider.h"
#else
#  include "VIC_weather_provider.h"
#endif
//_forward_declarations_________________________________________________________
class Air_temperature_maximum_parameter;
class Air_temperature_minimum_parameter;
class ET_reference_interface;
class Soil_layers_interface;
//_______________________________________________________forward_declarations__/
namespace VIC_crop
{
class Crop_with_weather;      // for friendship
class Crop_with_weather_soil; // for friendship
//_______________________________________________________Forward declarations__/
class Crop
: public extends_ Crop_common
{
friend class Crop_with_weather;                                                  //120618
friend class Crop_with_weather_soil;
public: // 'structors
   Crop
      (/*const*/ Crop_parameters    &_parameters
      ,CORN::Date &today
      ,const VIC::Weather_provider       &_weather
      ,bool _harvest_automatically
      ,bool _clipped_automatically                                               //110426
      ,bool _mint_clipping
      ,bool  enable_CO2_response);
 public: // methods interfacing with VIC
   virtual void know
      (const Soil_layers_interface                 *_soil_layers
      ,modifiable_ Soil_hydrology_interface        *_soil_hydrology);
   virtual bool know_transpiration_max_mm(double transpiration_max_mm) modification_;

   /*virtual void know_pychrometrics                                               //140508
      (float64 vapor_pressure_saturation_slope_kPa_C
      ,float64 psychrometric_constant_kPa_C
      ,float64 aerodynamic_resistance_ref_plants)                  modification_;*/
   virtual bool start_day(/*140808 int32 date*/)                   modification_;
   virtual bool /*140810 Normal_crop_event_sequence*/ process()    modification_;
   virtual Normal_crop_event_sequence get_crop_event_sequence()    modification_;//140810
   inline bool is_valid()                                                  const { return true; } // Currently assuming always valid
   virtual float64 harvest_annual(bool terminate)                  modification_;
   virtual float64 get
      (nat32 variable_code
      /*NYI ,nat32 units_code*/)                                          const;
   /*140810 obsolete
      // VIC is now handling water uptake so if it still needs
      // to have uptake, then it needs to keep track.
      // CropSyst proper does not keep the uptake by layer.
   virtual float64 get_uptake_mm(nat8 layer)                              const;
   */
   float64 update_ET_coef_full_shading()                         modification_; //121129
protected: // external references
   #ifdef  Moved
   //140811 to Crop_common
   /*const*/ Crop_parameters            &parameters;
   #endif
   const     Harvest_parameters         &harvest_parameters;
      // const disabled so I could easily reset the season parameters
public:
   inline virtual bool is_perennial()                                      const { return parameters.is_perennial(); }
/*140811 moved to Crop_common
protected:
   CORN::Date &today;
*/
protected: // weather
   const VIC::Weather_provider                  &weather;
   #if (VIC_CROPSYST_VERSION >= 2)
      //140508 Some of these values may be known from VIC
      // and we will need to set, otherwise replace these with calculators
      #ifndef CROPSYST_PROVIDED_ET
      Vapor_pressure_saturation_slope vapor_pressure_saturation_slope;    //140507
      Psychrometric_constant          psychrometric_constant;             //140508
      float64                         aerodynamic_resistance_ref_plants;  //140508
      #endif
   #endif
private: // external references
   const       Soil_layers_interface      *soil_layers;
   modifiable_ Soil_hydrology_interface   *soil_hydrology;
protected:
   //140811 appears to be obsolete bool harvest_automatically;
   /*140811 moved to Crop_common
   bool clipped_automatically;                                                   //110426
   bool mint_clipping;
   */
   nat8 clippings_completed;                                                     //110405
   float64 yield_reference_kg_m2;
   float64 yield_actual_kg_m2;
   float64 biomass_after_harvest; // only for output
   float64 cumulative_thermal_time_to_trigger_automatic_clipping_events;
   float64 biomass_at_filling;                                                   //140317
   float64 biomass_at_maturity;                                                  //140317
private:
   CropSyst::Crop_CO2_response *CO2_response; // optional                        //110903
   Phenology_reference        phenology_reference;
   Phenology_actual           phenology_actual;
   Root                       root;
   Transpiration_unstressed    transpiration_unstressed;   // Must be declared before biomass
   Canopy_biomass_continuum_reference  biomass_reference;
   Canopy_biomass_continuum_actual     biomass_actual;
   CropSyst::Canopy_cover_reference    canopy_cover_reference;
   CropSyst::Canopy_cover_actual       canopy_cover_actual;
protected:
   Transpiration_actual                transpiration_actual;
private:
   float64 calculate_yield
      (float64 &return_yield_actual
      ,float64 &return_yield_reference)                                   const;
   void process_reference_crop()                                   modification_;
   void develop()                                                  modification_;
   virtual bool process_harvest()                                  modification_;
   virtual float64 harvest_fruit_tree()                            modification_;
   virtual float64 clipped_yield()                                 modification_;
   virtual bool is_ready_for_clipping()                                    const;
   virtual bool respond_to_clipping()                              modification_;
   float64 get_root_depth()                                                const;
 public:
   bool reinitialize_CO2_response()                                modification_; //110628
#ifdef UNIT_TEST
public:
   void daily_growth_header(std::ofstream &daily_growth_file,int line);
   void daily_growth_output(std::ofstream &daily_growth_file);
#endif
 protected:
   virtual const CropSyst::Phenology_interface &ref_phenology()            const { return phenology_actual; } //140911
   virtual bool perform_clipping_and_ensure_end_of_pseudodormancy()modification_;
};
//______________________________________________________________________________
}  // namespace VIC_crop
#endif
#endif

