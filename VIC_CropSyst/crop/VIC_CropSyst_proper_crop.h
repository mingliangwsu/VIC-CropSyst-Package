#ifndef VIC_CropSyst_proper_cropH
#define VIC_CropSyst_proper_cropH
/*********************************************************************
  2026 NOTICE: this file is NOT part of the compiled build.

  Verified against build/library/*/Makefile_Kamiak and
  build/Xcc/Release/Makefile_Kamiak: neither VIC_CropSyst_proper_crop.cpp
  nor its base class reference (CropSyst::Crop_CropSyst) actually exist
  in the compiled/linked project -- "Crop_CropSyst" is not defined
  anywhere in CropSyst/source (only forward-declared), so this file
  cannot even compile as written. It appears to predate a CropSyst
  engine refactor that renamed the base class to Crop_complete
  (CropSyst/source/crop/crop_cropsyst.h/.cpp, which IS compiled).

  The restore_state()/get() additions below were written against this
  file under the (incorrect) assumption that it was the concrete class
  instantiated for V3 crops at runtime. It is not. The real,
  compiled-and-linked fix lives in:
    - CropSyst/source/crop/crop_interfaced.h   (Crop_model_interface::restore_state(), default no-op)
    - CropSyst/source/crop/crop_cropsyst.h/.cpp (Crop_complete::restore_state(), the real logic)
    - VIC_CropSyst/agronomic/VIC_land_unit_simulation.cpp (calls crop_active_or_intercrop->restore_state() directly, no dynamic_cast)
  See STATE_IO_README.md for the full explanation. The additions below
  are left in place only for historical reference; they have no effect
  on the running program.
*********************************************************************/
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
 public: // 2026 state restore support (scenario-branching / warm restart)
   /**
   \brief Pushes a previously-saved crop state snapshot into this
   (freshly created) crop object so that a new run can resume growth
   from the same point another run reached on a given day, instead of
   re-simulating the crop from planting.
   \param [in] biomass_kg_m2   total above ground biomass, kg/m2
                                (same units as VC_biomass_current_t_ha / 10).
   \param [in] GAI             green area index (unitless).
   \param [in] root_depth_m    rooting depth, meters.
   \param [in] growth_stage    the Normal_crop_event_sequence stage the
                                crop had reached (see growth_stages.h).
   \param [in] accum_thermal_time_deg_day
                                cumulative thermal time (degree-days)
                                the crop had accumulated. NOTE: there is
                                currently no public setter for this
                                quantity inside the CropSyst phenology
                                engine (Phenology_2018 keeps it as a
                                private accumulator). This parameter is
                                accepted and recorded for reporting /
                                validation purposes, and used to pick
                                the closest matching growth stage when
                                \p growth_stage is not supplied (< 0),
                                but it does NOT currently force the
                                internal degree-day accumulator to this
                                exact value. Practically this means a
                                restored crop resumes at the correct
                                growth STAGE, and biomass/canopy/root
                                are restored exactly, but thermal-time
                                accumulation within that stage restarts
                                from the stage's own activation point
                                rather than from the precise saved
                                value. See VIC_crop_state_csv.h and the
                                project README for details and for how
                                to extend this if exact GDD replay is
                                required (it would need a small
                                addition to Phenology_2018 itself to
                                expose a protected/private accumulator
                                such as GDDs via a public setter).
   \return true if at least the growth stage and canopy/root state
   were successfully restored.
   */
   virtual bool restore_state
      (float64                    biomass_kg_m2
      ,float64                    GAI
      ,float64                    root_depth_m
      ,Normal_crop_event_sequence growth_stage
      ,float64                    accum_thermal_time_deg_day
      )                                                            modification_;
};
//_2014-05-09___________________________________________________________________
} // namespace VIC_crop
#endif
