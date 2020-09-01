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

