#ifndef VIC_crop_variablesH
#define VIC_crop_variablesH

//______________________________________________________________________________
enum Variable_code
{
   VC_biomass_current_t_ha
,  VC_biomass_yield_t_ha
,  VC_biomass_after_harvest_t_ha
,  VC_leaf_area_index
,  VC_water_stress_index
,  VC_evaporation_mm
#ifdef CROPSYST_PROVIDED_SOIL_HYDROLOGY
//120803 VIC now provides all soil hydrology
,  VC_refill_water_depth_mm
#endif
,  VC_root_depth_mm
,  VC_ETmax_mm                                                                   //121116
,  VC_transpiration_actual_mm
,  VC_canopy_cover_fraction_green                                                //121218
,  VC_canopy_cover_fraction_total                                                //121218
/* 2026: additional codes added to support saving/restoring crop state
   for scenario-branching "initial state" runs -- see
   crop/VIC_crop_state_csv.h and VIC_CropSyst_proper_crop::restore_state().
   These are prognostic/state variables (as opposed to the diagnostic
   flux-like variables above) and are meaningful to capture and later
   feed back into restore_state().

   IMPORTANT: this Variable_code enum is consumed by
   CropSyst_proper_crop::get() (see VIC_CropSyst_proper_crop.cpp) when
   called directly on a CropSyst_proper_crop/Crop_CropSyst pointer.
   It is a *different*, independently-numbered enum from
   VIC::CropSyst_Variables (agronomic/VIC_land_unit_simulation.h),
   which is what VIC_land_unit_get()/Land_unit_simulation::get() (the
   actual V3 driver dispatch path -- see active_land_unit in
   agronomic/VIC_land_unit_C_interface.cpp) uses instead. Passing a
   Variable_code value into VIC_land_unit_get()/VIC_CropSyst_get() in a
   V3 build will not reach the switch below; it will be interpreted as
   a VIC::CropSyst_Variables value, which is almost certainly not what
   was intended. For V3 crop-state CSV I/O, use the dedicated
   VIC_land_unit_get_*_for_state() accessors in
   agronomic/VIC_land_unit_C_interface.h instead (see
   crop/VIC_crop_state_csv.c), which bypass this ambiguity entirely. */
,  VC_accum_degree_days       // cumulative thermal time, degree-days
,  VC_growth_stage_code       // current Normal_crop_event_sequence value
,  VC_root_biomass_kg_m2      // actual root biomass, kg/m2
/*
// The following are by layer Add the soil layer number to specify
// the variable by layer.
*/
,  VC_uptake_mm = 100
};

/* The default units for these variables
  VC_biomass_current       t/ha
  VC_biomass_yield         t/ha
  VC_biomass_after_harvest t/ha
  VC_leaf_area_index       unitless
  VC_water_stress_index    0-1 1=full stress
  VC_refill_water_depth    mm
  VC_uptake                mm
*/

//______________________________________________________________________________
#endif

