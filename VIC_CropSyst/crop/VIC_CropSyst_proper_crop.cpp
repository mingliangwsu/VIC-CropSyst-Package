
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
   switch (variable_code)
   {

/*140810 Need to implement


      case VC_leaf_area_index         : value = canopy_cover_actual.calc_leaf_area_index();  break;
      case VC_biomass_current_t_ha    : value = kg_m2_to_tonne_ha(biomass_actual.get_production_cum());  break;
      case VC_biomass_yield_t_ha      : value = kg_m2_to_tonne_ha(yield_actual_kg_m2);  break;
      case VC_biomass_after_harvest_t_ha : value = kg_m2_to_tonne_ha(biomass_after_harvest); break;
      case VC_water_stress_index : value = transpiration_actual.get_water_stress_index(); break;
      case VC_transpiration_actual_mm  : value = transpiration_actual.get_actual_mm(); break;                                 //121116
      case VC_ETmax_mm                 : value = transpiration_actual.get_crop_potential_ET() ; break;                        //121116
      case VC_canopy_cover_fraction_green : value = canopy_cover_actual.get_global_solar_rad_intercepted_green_canopy(); break;  //121116
      case VC_canopy_cover_fraction_total : value = canopy_cover_actual.get_global_solar_rad_intercepted_total_canopy(); break;  //121116
      case VC_root_depth_mm               : value = m_to_mm(root.get_depth()); break; //130725
*/
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
}; // namespace VIC_crop

#if (CROPSYST_VERSION==4)
void CropSyst::Crop_CropSyst::setup_structure(CORN::Data_record &data_rec,bool for_write,const CropSyst::Desired_variables &desired_vars)
{
   // This output mechanism is not used in VIC.
   // (This is also becoming obsolete).
}
#endif

