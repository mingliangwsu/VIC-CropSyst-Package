#ifndef VIC_CropSyst_proper_cropH
#define VIC_CropSyst_proper_cropH
#include "VIC_crop_common.h"
#include "crop/crop_cropsyst.h"
#include "VIC_weather_provider.h"
namespace VIC_crop
{
//______________________________________________________________________________
class CropSyst_proper_crop
: public extends_ Crop_common // Crop_interface
, public extends_
   //141206 #if (CROPSYST_VERSION==5)
   CropSyst::
   //141206 #endif
   Crop_CropSyst // Crop_interfaced_CropSyst
{
   float64 daylight_hours_unused;                                                 //140812
      // daylight_hours are used by thermal time accum photoperiod.
      // photoperiod is not currently used in VIC
      // If it is needed, we will need a function for VIC to
      // tell the crop the current daylight hours

 public: // structors
    CropSyst_proper_crop
      (Crop_parameters &         _parameters
      ,CORN::Date &              _today
      ,bool                      _clipped_automatically
      ,bool                      _mint_clipping
      ,const float64            &_aerodynamic_resistance_ref_plants
      ,const float64            &_snow_pack_thickness                            //141205
      ,const VIC::Weather_provider   &_weather_provider                               //130818_110826
      ,Soil_interface           *_soil = 0                                       //050721
      );
 public: // methods interfacing with VIC
   virtual bool know_transpiration_max_mm(double transpiration_max_mm) modification_;
   inline virtual bool is_perennial()                                      const
      { return CropSyst::Crop_CropSyst::is_perennial(); }
   virtual float64 update_ET_coef_full_shading()                   modification_;
   virtual bool reinitialize_CO2_response()                        modification_;
   virtual bool start_day()                                        modification_;
   virtual float64 harvest_annual(bool terminate)                  modification_;
   virtual float64 harvest_fruit_tree()                            modification_;
   virtual bool process();
   virtual Normal_crop_event_sequence get_crop_event_sequence()    modification_;
   virtual float64 get(nat32 variable_code)                                const;
   virtual const CropSyst::Phenology_interface &ref_phenology()            const;
   virtual float64 clipped_yield()                                 modification_;
   virtual bool is_ready_for_clipping()                                    const;
   virtual bool respond_to_clipping()                              modification_;
   virtual bool perform_clipping_and_ensure_end_of_pseudodormancy()modification_;
};
//_2014-05-09___________________________________________________________________
} // namespace VIC_crop
#endif
