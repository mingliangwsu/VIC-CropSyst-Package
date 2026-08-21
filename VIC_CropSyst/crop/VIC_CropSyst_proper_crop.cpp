
#include "VIC_CropSyst_proper_crop.h"
#include "crop/CO2_response.h"
#include "VIC_crop_variable_codes.h"
namespace VIC_crop
{
//______________________________________________________________________________
CropSyst_proper_crop::CropSyst_proper_crop
(Crop_parameters &         _parameters
,CORN::Date &              _today
,bool                      _clipped_automatically
,bool                      _mint_clipping
,const float64 &           _aerodynamic_resistance_ref_plants
,const float64 &           _snow_pack_thickness               //141205
,const VIC::Weather_provider &  _weather_provider
,Soil_interface *          _soil
)
: VIC_crop::Crop_common
(_parameters
,_today
,_clipped_automatically
,_mint_clipping
)
, CropSyst::Crop_CropSyst
(&_parameters
#ifdef OLD_ORCHARD
      ,Orchard_interface *_orchard
#endif
,false // doesn't own params
#if (((CS_VERSION >= 1) && (CS_VERSION < 5)) &&!defined(REACCH_VERSION))
      ,Transpiration_model           transpiration_model
#endif
,_today
,_weather_provider.air_temperature_max
,_weather_provider.air_temperature_min
,_weather_provider.air_temperature_avg
,_weather_provider.solar_radiation
,_weather_provider.vapor_pressure_deficit_daytime // daytime_vapor_pressure_deficit
,_weather_provider.vapor_pressure_deficit_max
,_weather_provider.vapor_pressure_saturation_slope //vapor_pressure_saturation_slope
,_weather_provider.psychrometric_constant
,                 _aerodynamic_resistance_ref_plants
,                 _snow_pack_thickness                                           //141206
/*141206RLN
//LML 140916 #ifdef CROPSYST
#ifdef USE_WEATHER_PROVIDER
#else
,0 //LML 140916
#endif
*/
,daylight_hours_unused                                                           //140812
,0 //Seeding_parameters
,_soil
,CROP_NITROGEN_DISABLED    // 140815 Eventually we will need crop nitrogen
#ifdef NITROGEN
,0 //LML 140916
#endif
)
{}
//______________________________________________________________________________
const CropSyst::Phenology_interface &CropSyst_proper_crop::ref_phenology()                       const //140811
{ return
         //141206 #if (CROPSYST_VERSION==5)
         CropSyst::
         //141206 #endif
            Crop_CropSyst::ref_phenology();
}
//______________________________________________________________________________
bool CropSyst_proper_crop::know_transpiration_max_mm(double transpiration_max_mm) modification_
{
   assert(false);  // This needs to be implemented
   //NYI
   return false;
}
//______________________________________________________________________________
float64 CropSyst_proper_crop::update_ET_coef_full_shading()        modification_
{
   assert(false);  // This needs to be implemente
   return 0.0; // not sure how to implement this with CropSyst proper
      //might not be needed
}
//_2014-08-10______________________________________update_ET_coef_full_shading_/
bool CropSyst_proper_crop::reinitialize_CO2_response()             modification_
{  return CO2_response && CO2_response->initialize();
}
//_2014-08-10________________________________________reinitialize_CO2_response_/
bool CropSyst_proper_crop::start_day()                             modification_
{  return
   //141206    #if (CROPSYST_VERSION==5)
   CropSyst::
   //141206 #endif
   Crop_CropSyst::start_day();
}
//_2014-08-10________________________________________________________start_day_/
float64 CropSyst_proper_crop::harvest_annual(bool terminate)       modification_
{
   assert(false);     // This needs to be implemente
   return 0.0; //NYI
}
//_2014-08-10___________________________________________________harvest_annual_/
float64 CropSyst_proper_crop::harvest_fruit_tree()                   modification_
{
   assert(false);   // This needs to be implemente
   return 0.0; //NYI
}
//_2014-08-10_____________________________________________harvest_fruit_tree()_/
bool CropSyst_proper_crop::process()
{  return
   //141206       #if (CROPSYST_VERSION==5)
      CropSyst::
   //141206       #endif
      Crop_CropSyst::process()
      && process_harvest();
}
//_2014-08-10__________________________________________________________process_/
/*140811 NYN
bool CropSyst_proper_crop::process_harvest()                       modification_;
{  return Crop_common::process_harvest();
}
*/
//_2014-08-10__________________________________________________________process_/
Normal_crop_event_sequence CropSyst_proper_crop::get_crop_event_sequence() modification_
{  return
   //141206       #if (CROPSYST_VERSION==5)
      CropSyst::
   //141206       #endif
      Crop_CropSyst::get_growth_stage_sequence();
   // Warning may need to especially handle harvest event today, but probably not.
}
//_2014-08-10__________________________________________get_crop_event_sequence_/
float64 CropSyst_proper_crop::get
(nat32 variable_code)                                                      const
{
float64 value = 0.0;
   // 2026: replaced the previously unimplemented (fully commented-out)
   // version of this function with a working implementation based on
   // the public accessors that are confirmed present on Crop_CropSyst
   // (crop/crop_cropsyst.h) and already exercised elsewhere in this
   // codebase (see agronomic/VIC_land_unit_C_interface.cpp, which
   // calls get_accum_degree_days(), get_GAI(), get_recorded_root_depth_m(),
   // get_act_transpiration_m() etc. on the equivalent Crop_model_interface
   // pointer). The member names used by the previous (commented out)
   // version -- canopy_cover_actual, biomass_actual, transpiration_actual,
   // root -- do not exist on this class and were left over from an
   // earlier, incompatible draft; they have been replaced below.
   switch (variable_code)
   {
      case VC_leaf_area_index            : value = get_GAI(true);                     break;
      case VC_biomass_current_t_ha       : value = get_canopy_biomass_kg_m2() * 10.0;  break;
         // 1 kg/m2 == 10 t/ha
      case VC_biomass_yield_t_ha         : value = get_latest_yield_kg_m2() * 10.0;    break;
      case VC_water_stress_index         : value = get_water_stress_index_mean();      break;
      case VC_transpiration_actual_mm    : value = get_act_transpiration_m() * 1000.0; break;
      case VC_root_depth_mm              : value = get_recorded_root_depth_m() * 1000.0; break;
      // 2026 additions in support of state save/restore -- see
      // VIC_crop_state_csv.h. These reuse the existing get() dispatch
      // mechanism rather than adding new C interface entry points.
      case VC_accum_degree_days          : value = get_accum_degree_days();            break;
      case VC_growth_stage_code          : value = (float64)get_growth_stage_sequence();break;
      case VC_root_biomass_kg_m2         : value = get_act_root_biomass_kg_m2();       break;
      /* Not yet implemented: VC_biomass_after_harvest_t_ha, VC_evaporation_mm,
         VC_ETmax_mm, VC_canopy_cover_fraction_green/total -- no confirmed
         public accessor for these was found on Crop_CropSyst at the time
         this was written; they still return 0.0 (previous behavior). */
   }
   return value;
}
//_2014-08-10______________________________________________________________get_/

float64 CropSyst_proper_crop::clipped_yield()                             modification_
{
   assert(false);   // This needs to be implemente
   return 0.0; //NYI
}
//_2014-08-10______________________________________________________________
//_2014-08-10______________________________________________________________
bool CropSyst_proper_crop::is_ready_for_clipping()                                const
{
   assert(false); // This needs to be implemente
   return true;
}
//_2014-08-10______________________________________________________________
bool CropSyst_proper_crop::respond_to_clipping()                          modification_
{
   assert(false); // This needs to be implemente
   return true;
}
//_2014-08-10______________________________________________________________respond_to_clipping_/
bool CropSyst_proper_crop::perform_clipping_and_ensure_end_of_pseudodormancy()modification_
{
   assert(false); // This needs to be implemente
   return true;
}
//_2014-08-10______________________________________________________________respond_to_clipping_/
bool CropSyst_proper_crop::restore_state
(float64                    biomass_kg_m2
,float64                    GAI
,float64                    root_depth_m
,Normal_crop_event_sequence growth_stage
,float64                    accum_thermal_time_deg_day
)                                                                   modification_
{
   bool ok = true;

   // --- Canopy / above-ground biomass ------------------------------------
   // Canopy_leaf_growth::restart_with() is the CropSyst engine's own,
   // already-existing hook for exactly this purpose (see
   // CropSyst/source/crop/canopy_growth.h). canopy_leaf_growth is a
   // protected member of Crop_CropSyst, accessible here because
   // CropSyst_proper_crop derives from it.
   if (canopy_leaf_growth)
      ok = canopy_leaf_growth->restart_with(biomass_kg_m2, GAI, true) && ok;
   else
      ok = false;

   // --- Roots ---------------------------------------------------------
   // Crop_root::initialize() is documented in CropSyst/source/crop/crop_root.h
   // as: "Call once when crop is planted and at restart".
   if (roots_current)
      ok = roots_current->initialize(root_depth_m) && ok;
   else
      ok = false;

   // --- Phenology / growth stage ---------------------------------------
   // Force the phenology state machine to the saved growth stage using
   // the public activate_*() methods CropSyst_2018 already exposes for
   // this purpose (CropSyst/source/crop/phenology_2018.h). This restores
   // the correct STAGE. Exact within-stage degree-day accumulation
   // (accum_thermal_time_deg_day) cannot currently be forced because
   // Phenology_2018 keeps its GDD accumulator private with no public
   // setter; the value is intentionally accepted here (and can be
   // logged/compared by the caller) so that a future extension only
   // needs to add such a setter to phenology_2018.h/.cpp, without
   // having to change this call site or the state file format.
   switch (growth_stage)
   {
      case NGS_GERMINATION:
      case NGS_PLANTING:        ok = phenology.activate_sowing()         && ok; break;
      case NGS_EMERGENCE:       ok = phenology.activate_emergence()      && ok; break;
      case NGS_ACCRESCENCE:     ok = phenology.activate_accrescence()    && ok; break;
      case NGS_ANTHESIS:        ok = phenology.activate_anthesis()       && ok; break;
      case NGS_FILLING:         ok = phenology.activate_yield_formation()&& ok; break;
      case NGS_MATURITY:        ok = phenology.activate_maturity()       && ok; break;
      case NGS_QUIESCENCE:      ok = phenology.activate_quiescence()     && ok; break;
      default:
         // NGS_NONE, NGS_RESTART, NGS_HARVESTABLE, NGS_HARVEST,
         // NGS_POSTHARVEST, NGS_TERMINATED and any stage not handled
         // above: fall back to the engine's own generic restart entry
         // point rather than guessing at an activate_*() call that may
         // not apply.
         ok = phenology.activate_restart() && ok;
         break;
   }

   (void)accum_thermal_time_deg_day; // see note above; currently informational only

   return ok;
}
//_2026____________________________________________________________restore_state_/
}; // namespace VIC_crop

#if (CROPSYST_VERSION==4)
void CropSyst::Crop_CropSyst::setup_structure(CORN::Data_record &data_rec,bool for_write,const CropSyst::Desired_variables &desired_vars)
{
   // This output mechanism is not used in VIC.
   // (This is also becoming obsolete).
}
#endif

