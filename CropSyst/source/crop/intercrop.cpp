#include "intercrop.h"
#include "CS_suite/simulation/CS_inspector.h"
#include <assert.h>

namespace CropSyst
{
//______________________________________________________________________________
Intercrop::Intercrop(const Air_temperature_average *air_temperature_avg)
: Crop_interfaced(air_temperature_avg)                                           //150209
, predominant_crop(0)
, secondary_crop(0)
{}
//______________________________________________________________________________
Crop_interfaced *Intercrop::take(Crop_interfaced *crop_to_add)    appropriation_
{  if       (!predominant_crop)  predominant_crop  = crop_to_add;
   else if  (!secondary_crop)    secondary_crop    = crop_to_add;
   else assert(false); // we currently only provide for two crops.
   return crop_to_add;
}
//______________________________________________________________________________
//NYN void *get_association_for_events()                              const { assert(false); return 0; /* This will never get called by intercrop*/}
bool         Intercrop::is_terminate_pending()                             const { return predominant_crop->is_terminate_pending(); }
   #if (CROPSYST_VERSION == 4)
bool         Intercrop::is_fallow_deprecated()                             const { return predominant_crop->is_fallow_deprecated(); }
   #endif
CropSyst::Phenology_interface &Intercrop::mod_phenology()                        { return predominant_crop->mod_phenology();}
const        CropSyst::Phenology_interface &Intercrop::ref_phenology()     const { return predominant_crop->ref_phenology();}
//200228 float64      Intercrop::get__fract_canopy_cover()                           const {  return std::max<float64>(predominant_crop->get__fract_canopy_cover(),secondary_crop? secondary_crop->get__fract_canopy_cover(): 0.0); }
float64      Intercrop::get_canopy_interception_global_total()             const {  return std::max<float64>(predominant_crop->get_canopy_interception_global_total(),secondary_crop? secondary_crop->get_canopy_interception_global_total(): 0.0); }
      // WARNING probably need to aggregate with secondary crop
float64      Intercrop::get_leaf_water_pot()                               const { return predominant_crop->get_leaf_water_pot();}
//NYN void        *Intercrop::get_unique_identifier()                            const { assert(false); return 0; /* This should never be called at intercrop level*/}
Crop_model   Intercrop::get_crop_model()                                   const { return predominant_crop->get_crop_model(); }
float64      Intercrop::get_canopy_resistance()                            const {  return predominant_crop->get_canopy_resistance(); /* checking if needed */}
      // WARNING probably need to aggregate with secondary crop
//public: // setters
bool         Intercrop::set_fallow_deprecated(bool fallow)                       { return predominant_crop->set_fallow_deprecated(fallow); /* checking if needed */}
bool         Intercrop::set_no_more_clippings_this_season()         modification_{ return predominant_crop->set_no_more_clippings_this_season();}
bool         Intercrop::get_no_more_clippings_this_season()                const { return predominant_crop->get_no_more_clippings_this_season();}
//NYN void         Intercrop::end_season_if_no_dormancy()                              { assert(false);           /* should never be called for intercrop */}
//131204 no longer used//NYN void         set_harvest_date(CORN::Date32 harvest_date)              { assert(false);           /* checking if needed */}
      // This is provided for compatibility with CropGro
//NYN void         Intercrop::set_terminate_pending()                                  { assert(false);           /* should never be called for intercrop */}
//181108 obsolete //NYN int16        Intercrop::get_days_after_flowering()                         const { assert(false); return 0; /* should never be called for intercrop */}

//______________________________________________________________________________
float64      Intercrop::get_pot_transpiration_m()                          const
{  return predominant_crop->get_pot_transpiration_m() +
      (secondary_crop ? secondary_crop->get_pot_transpiration_m() : 0);
}
//______________________________________________________________________________
float64      Intercrop::get_intercepted_precipitation_m()                  const
{  return predominant_crop->get_intercepted_precipitation_m() +
      (secondary_crop? secondary_crop->get_intercepted_precipitation_m() : 0);
} // used only for output
//______________________________________________________________________________
//delete this obsolete: float64      Intercrop::set_intercepted_precipitation_m(float64 _interception_m) { assert(false); return 0; /* should never be called for intercrop */}
//______________________________________________________________________________
float64 Intercrop::intercept_precipitation(float64 water_reaching_plant) modification_
{  float32 water_leaving_crops = predominant_crop->intercept_precipitation(water_reaching_plant);
   if (secondary_crop) water_leaving_crops = secondary_crop->intercept_precipitation(water_leaving_crops);
   return water_leaving_crops;
}
//_intercept_precipitation__________________________________________2014-03-20_/
/*200420
const char  *Intercrop::describe_growth_stage()                            const
{ return predominant_crop->describe_growth_stage(); }
*/
nat8 Intercrop::describe_periods(CORN::Text_list &descriptions)           const
{ return predominant_crop->describe_periods(descriptions); }
      // May want to compose description to include predominate and secondary descriptions
//__________________________________________________________________2020-04-20_/
float64      Intercrop::get_recorded_root_depth_m()                        const
{ return std::max<float64>(predominant_crop->get_recorded_root_depth_m()
         ,secondary_crop ? secondary_crop->get_recorded_root_depth_m() : 0.0); }
//______________________________________________________________________________
float64   Intercrop::get_N_mass_canopy_kg_m2(bool recorded_before_harvest) const
{  return predominant_crop->get_N_mass_canopy_kg_m2(recorded_before_harvest) +
      (secondary_crop ? secondary_crop->get_N_mass_canopy_kg_m2(recorded_before_harvest) : 0);
}
//______________________________________________________________________________
float64      Intercrop::get_root_length_m()                                const
{ return std::max<float64>(predominant_crop->get_root_length_m()
          ,(secondary_crop ? secondary_crop->get_root_length_m() : 0.0));
}
//______________________________________________________________________________
 // used only for output
//NYN float64      Intercrop::get_plant_height_m()                               const { assert(false); return 0; /* checking if needed */}
//NYN float64      Intercrop::get_active_fract_root_length_m(uint8 sublayer)     const { assert(false); return 0; /* checking if needed */}
//______________________________________________________________________________
float64 const *Intercrop::get_total_fract_root_length_m()                  const
   {  Crop_interfaced *deepest_rooted_crop =
         (secondary_crop && secondary_crop->get_root_length_m() > predominant_crop->get_root_length_m())
         ? secondary_crop
         : predominant_crop;
      return deepest_rooted_crop->get_total_fract_root_length_m(); }
//______________________________________________________________________________
//130716  I think some how aggregation of predominant and secondary crop but not sure:
float64      Intercrop::get_latest_yield_kg_m2()                           const { return predominant_crop->get_latest_yield_kg_m2();}
   // This is the yield from the latest harvest event.
const CORN::Date &Intercrop::get_planting_date()                           const { return predominant_crop->get_planting_date();}
const CORN::Date &Intercrop::get_restart_date()                            const { return predominant_crop->get_restart_date();}
const CORN::Date &Intercrop::get_emergence_date()                          const { return predominant_crop->get_emergence_date();}
const CORN::Date &Intercrop::get_flowering_date()                          const { return predominant_crop->get_flowering_date();}
const CORN::Date &Intercrop::get_tuber_init_date()                         const { return predominant_crop->get_tuber_init_date();}
const CORN::Date &Intercrop::get_grain_filling_date()                      const { return predominant_crop->get_grain_filling_date();}
const CORN::Date &Intercrop::get_maturity_date()                           const { return predominant_crop->get_maturity_date();}
const CORN::Date &Intercrop::get_harvest_date()                            const { return predominant_crop->get_harvest_date();}
//180413 const CORN::Date &Intercrop::get_largest_GAI_date()                        const { return predominant_crop->get_largest_GAI_date();}
//190705 const CORN::Date &Intercrop::ref_GAI_max_date()                           const { return predominant_crop->ret_GAI_max_date();} //180413
   #ifdef CROP_ORCHARD
const CORN::date32 &Intercrop::get_bud_break_date()                        const { return predominant_crop->get_bud_break_date();}
const CORN::date32 &Intercrop::get_initial_fruit_growth_date()             const { return predominant_crop->get_initial_fruit_growth_date();}
const CORN::date32 &Intercrop::get_rapid_fruit_growth_date()               const { return predominant_crop->get_rapid_fruit_growth_date();}
const CORN::date32 &Intercrop::get_chill_requirement_satisfied_date()      const { return predominant_crop->get_chill_requirement_satisfied_date();}
   #endif
const CORN::date32 &Intercrop::get_inactive_begin_date()                   const { return predominant_crop->get_inactive_begin_date();}
//190807 unused const CORN::Date &Intercrop::get_inactive_end_date()                       const { return predominant_crop->get_inactive_end_date();}
// The following are for report details only,
         // Eventually they may be replaced by data_record I/O
//NYN float64 Intercrop::get_accum_degree_days(bool adjusted_for_clipping)       const { assert(false); return 0; /* checking if needed */}
   #ifdef CROP_ORCHARD
//NYN int16 Intercrop::get_fruit_chill_hours_remaining()                         const { assert(false); return 0; /* checking if needed */}

//NYN float64 Intercrop::get_fruit_dry_biomass_kg_ha()                           const { assert(false); return 0; /* checking if needed */}
//NYN float64 Intercrop::get_orchard_solid_canopy_interception()                 const { assert(false); return 0; /* checking if needed */}
//NYN float64 Intercrop::get_orchard_shaded_area_GAI()                           const { assert(false); return 0; /* checking if needed */}
//NYN float64 Intercrop::get_orchard_canopy_porosity()                           const { assert(false); return 0; /* checking if needed */}
   #endif
//NYN float64 Intercrop::get_dry_biomass_kg_ha()                                 const { assert(false); return 0; /* checking if needed */}
//NYN float64 Intercrop::get_live_green_canopy_biomass_kg_ha()                   const { assert(false); return 0; /* checking if needed */}
//NYN float64 Intercrop::get_act_root_biomass_kg_ha()                            const { assert(false); return 0; /* checking if needed */}
//NYN float64 Intercrop::get_canopy_biomass_kg_ha()                              const { assert(false); return 0; /* checking if needed */}
//NYN float64 Intercrop::get_VPD_daytime()                                       const { assert(false); return 0; /* checking if needed */}
//NYN float64 Intercrop::get_VPD_daytime_mean()                                  const { assert(false); return 0; /* checking if needed */}
//NYN float64 Intercrop::get_intercepted_PAR_MJ_m2()                             const { assert(false); return 0; /* checking if needed */}
//NYN float64 Intercrop::get_intercepted_PAR_accum_MJ_m2()                       const { assert(false); return 0; /* checking if needed */}
//NYN float64 Intercrop::get_intercepted_PAR_season_accum_MJ_m2()                const { assert(false); return 0; /* checking if needed */}
//NYN float64 Intercrop::get_peak_LAI(bool reported)                             const { assert(false); return 0; /* checking if needed */}
//NYN float64 Intercrop::get_total_season_collected_biomass_kg_m2()              const { assert(false); return 0; /* checking if needed */}
//NYN float64 Intercrop::get_total_season_biomass_production_kg_m2()             const { assert(false); return 0; /* checking if needed */}
//NYN const Crop_nitrogen_interface *Intercrop::get_nitrogen()                   const { assert(false); return 0; /* checking if needed */}
//NYN float64 Intercrop::update_root_depth(float64 soil_rooting_depth,float64 total_soil_depth)         modification_{ assert(false); return 0; /* checking if needed */}
// data record output
/*NYN bool Intercrop::setup_structure
      (CORN::Data_record &data_rec
      ,bool for_write
      ,const Desired_variables &desired_vars)                       modification_{ assert(false);           // checking if needed }
*/
// The following are used for accumulation and/or output
//______________________________________________________________________________
float64 Intercrop::get_act_transpiration_m()                               const
{  return  predominant_crop->get_act_transpiration_m()
      + ((secondary_crop) ? secondary_crop->get_act_transpiration_m() : 0.0);
   // RLN need to verify this return value (may want to simply have the predominate crop.
}
/*190705 moved to recorded (see end_day)
//______________________________________________________________________________
float64 Intercrop::get_recorded_act_transpiration_m()                      const
{  return  predominant_crop->get_recorded_act_transpiration_m()
      + ((secondary_crop) ? secondary_crop->get_recorded_act_transpiration_m() : 0.0);
   // RLN need to verify this return value (may want to simply have the predominate crop.
}
//______________________________________________________________________________
*/
//NYN float64 Intercrop::get_reported_harvest_index()                            const { assert(false); return 0; /* checking if needed */}
//NYN float64 Intercrop::get_canopy_biomass_kg_m2()                              const { assert(false); return 0; /* checking if needed */}
//NYN float64 Intercrop::get_act_root_biomass_kg_m2()                            const { assert(false); return 0; /* checking if needed */}
//NYN float64 Intercrop::calc_root_biomass(float64 *output_root_biomass_by_layer=0) const { assert(false); return 0; /* checking if needed */}
float64 Intercrop::get_LAI(bool from_canopy)                               const
{  // The LAI when interfacing with the other CropSyst submodels is used for
   // Soil temperature, so returning the maximum of the predominate and secondary crop
   return std::max<float64>
      (predominant_crop->get_LAI(from_canopy)
      ,secondary_crop? secondary_crop->get_LAI(from_canopy) : 0.0);
}
//NYN float64 Intercrop::get_GAI(uint8 include_GAI)                              const { assert(false); return 0; /* checking if needed */}
//NYN float64 Intercrop::get_GAI_today()                                         const { assert(false); return 0; /* checking if needed */}
//NYN float64 Intercrop::get_temperature_stress()                                const { assert(false); return 0; /* checking if needed */}
//NYN float64 Intercrop::get_water_stress()                                      const { assert(false); return 0; /* checking if needed */}
//NYN float64 Intercrop::get_water_stress_index()                                const { assert(false); return 0; /* checking if needed */}
//NYN float64 Intercrop::get_water_stress_index_mean()                           const { assert(false); return 0; /* checking if needed */}
//NYN float64 Intercrop::get_temperature_stress_index()                          const { assert(false); return 0; /* checking if needed */}
//NYN float64 Intercrop::get_temperature_stress_index_mean()                     const { assert(false); return 0; /* checking if needed */}

   #if (CROPSYST_VERSION==4)
//NYN float64 Intercrop::get_flower_temperature_stress_index_mean()              const { assert(false); return 0; /* checking if needed */}
//NYN float64 Intercrop::get_flowering_N_stress_index_mean()                     const { assert(false); return 0; /* checking if needed */}
//NYN float64 Intercrop::get_flower_water_stress_index_mean_check_used()         const { assert(false); return 0; /* checking if needed */}
//NYN float64 Intercrop::get_filling_duration_index_mean()                       const { assert(false); return 0; /* checking if needed */}
   #endif
//NYN float64 Intercrop::get_tuber_init_temperature_stress_index()               const { assert(false); return 0; /* checking if needed */}
//NYN float64 Intercrop::get_tuber_fill_temperature_stress_index()               const { assert(false); return 0; /* checking if needed */}
//NYN float64 Intercrop::get_tuber_init_temperature_stress_index_mean()          const { assert(false); return 0; /* checking if needed */}
//NYN float64 Intercrop::get_tuber_fill_temperature_stress_index_mean()          const { assert(false); return 0; /* checking if needed */}

//NYN float64 Intercrop::get_overall_growth_stress()                             const { assert(false); return 0; /* checking if needed */}
//______________________________________________________________________________
float64 Intercrop::get_C_mass_canopy()                                     const
      {  return predominant_crop->get_C_mass_canopy()
         + (secondary_crop?secondary_crop->get_C_mass_canopy():0.0);
      }
//______________________________________________________________________________
float64 Intercrop::get_C_mass_roots()                                      const
      {  return predominant_crop->get_C_mass_roots()
         + (secondary_crop?secondary_crop->get_C_mass_roots():0.0);
      }
//______________________________________________________________________________
// The following are parameter accessors
//NYN int32    Intercrop::param_LADSS_land_use_ID()                       const { assert(false); return 0; /* checking if needed */}
Land_use Intercrop::param_land_use()                                       const { return predominant_crop->param_land_use(); /* actually only called for predominant crop */}
//NYN float64  Intercrop::param_emergence_deg_day()                       const { assert(false); return 0; /* checking if needed */}
// float64  Intercrop::param_wilt_leaf_water_pot()                     const { assert(false); return 0; /* checking if needed */}

Residue_decomposition_parameters&Intercrop::get_residue_decomposition_parameters() const { return predominant_crop->get_residue_decomposition_parameters(); } //definitely predominate crop because this is to output harvested removed Carbon
// The following parameters are used for scaling runtime graph
       //NYN float64  Intercrop::param_max_LAI()                                 const { assert(false); return 0; /* checking if needed */}
//NYN float64  Intercrop::param_max_root_depth_m()                        const { assert(false); return 0; /* checking if needed */}
      // Derived classes may override param_max_root_depth_m if available
      // it is used primarily for fastgraph scaling.
// The following are for LADSS only
//NYN int32 Intercrop::get_LADSS_land_use_ID()                                   const { assert(false); return 0; /* checking if needed */}
//NYN const char *Intercrop::get_sowing_event_ID()                               const { assert(false); return 0; /* checking if needed */}
// The following are used to implement operations and events
   // that remove biomatter from the plant
/*NYN
      Crop_mass_fate_biomass_and_nitrogen *
      Intercrop::process_biomass_fate
         (Biomass_fated_cause fated_cause
         ,Harvest_amount_mode harvest_amount_mode
         ,float64 remove_amount_kg_m2
         ,float64 retain_GAI
         ,float64 retain_biomass_kg_m2
         ,const Biomass_fate_parameters  &biomass_fate_parameters
         ,bool terminate
         ,float64 yield_now)                                                      { assert(false); return 0; //checking if needed }
*/
      // This moves plant biomass to various pools, harvest yield, residue, loss etc..
      // Returns the fate of the material if processed otherwise 0
      // (call must delete the returned fate object)
void Intercrop::commit_biomass_fate()                                            { predominant_crop->commit_biomass_fate(); if (secondary_crop) secondary_crop->commit_biomass_fate() ;  }
      // This should be called daily, it checks if there is
      // any biomatter fate and generates a log.
const Crop_mass_fate *Intercrop::get_biomass_fate_today_record()           const
   { return predominant_crop->get_biomass_fate_today_record(); }
//NYN Crop_mass_fate *Intercrop::get_biomass_fate_season()                             { assert(false); return 0; /* checking if needed */}                            //040624
// The following are processes used by the calling program
//NYN   float64 Intercrop::rain_interception(float64 water_reaching_plant)                       { assert(false); return 0; /* checking if needed */}
/*NYN
      Crop_mass_fate_biomass_and_nitrogen *
      Intercrop::clip_biomass
      (Harvest_amount_mode harvest_amount_mode
      ,float64 remove_amount_kg_m2
      ,float64 min_retain_GAI
      ,float64 min_retain_biomass_kg_m2
      ,const Biomass_fate_parameters &biomass_fate_parameters
      ,Biomass_fated_cause fated_cause)                                          { assert(false); return 0; }
      // Returns the fate of the biomass (must be deleted by the caller)
*/
//NYN bool Intercrop::respond_to_clipping()                              modification_ { assert(false); return 0; /* checking if needed */}
/*NYN
   bool Intercrop::check_for_automatic_clipping
      (const CropSyst::Auto_clip_biomass_mode &clip_biomass_op
      ,float64 remove_amount_kg_m2
      ,bool   clip_now_or_never
      ,float64 avg_temperature
      ,Common_simulation_log *log)                                               { assert(false); return 0; }                                              //050929
      // Returns true if clipping actually performed.
*/
//NYN bool Intercrop::sow()                                                            { assert(false); return 0; /* checking if needed */}
bool Intercrop::start_day()
{
   // recorded.daily.clear();
   return predominant_crop->start_day()&& (secondary_crop) ? secondary_crop->start_day() : true;
   }
//______________________________________________________________________________
bool Intercrop::end_day()
{
   // Mostly we are recording the predominant crop
   // conceptual may want to amagamate the existing crops recorded values
   recorded.copy(predominent_crop->ref_recorded());                              //190705
   // The following are cumulative:
   recorded.daily.act_transpiration_m                                            //190705
   = predominant_crop->get_recorded().act_transpiration_m()
   +((secondary_crop)?secondary_crop->get_recorded().act_transpiration_m():0.0);
   return predominant_crop->end_day()  && (secondary_crop) ? secondary_crop->end_day()   : true;
}
//______________________________________________________________________________
bool Intercrop::process_day()
{  return predominant_crop->process_day()  && (secondary_crop) ? secondary_crop->process_day()   : true; }
//______________________________________________________________________________
   #ifdef NITROGEN
//NYN void Intercrop::set_automatic_nitrogen_mode(bool enable)                         { assert(false);           /* checking if needed */}                        //040525
   #endif
/*NYN Crop_mass_fate_biomass_and_nitrogen *Intercrop::harvest
      (float64 remove_amount_kg_m2
      ,const CropSyst::Harvest_or_clipping_operation *harvest_or_clipping_params){ assert(false); return 0; // checking if needed }
*/
      // Returns the fate of the biomass if it could harvest                   
      // otherwise 0 (must be deleted by the caller)
/*NYN Crop_mass_fate_biomass_and_nitrogen *Intercrop::harvest_crop
      (Harvest_amount_mode harvest_amount_mode                                 
      ,float64 remove_amount_kg_m2                                             
      ,float64 retain_GAI
      ,const Biomass_fate_parameters &harvest_biomass_fate_parameters
      ,bool terminate)                                                           { assert(false); return 0;}
      // Returns the fate of the biomass if it could harvest
      // otherwise 0 (must be deleted by the caller)
*/
//______________________________________________________________________________
float64 Intercrop::update_evapotranspiration_max// 190812 update_pot_evapotranspiration
      (float64 ET_ref_m)
{ float64 predominant_adjust_ET=predominant_crop->update_evapotranspiration_max
      //190812 update_pot_evapotranspiration
      (ET_ref_m);
   if (secondary_crop) secondary_crop->update_evapotranspiration_max
      //190812 update_pot_evapotranspiration
      (ET_ref_m);
   return predominant_adjust_ET;
}
//140320 Claudio confirmed this.
//______________________________________________________________________________
//NYN float64 Intercrop::est_degree_days_X_days_from_now(const CORN::Date & today, CORN::Days days_from_now)                 const { assert(false); return 0; /* checking if needed */}
      // This returns an estimate (in this case the actual)
      // of the thermal time the crop will have in the specified
      // number of days from the current date.
/*130903 now using phenology::days_since_start_of
nat32  Intercrop::relinquish_triggered_synchronizations()
{ return predominant_crop->relinquish_triggered_synchronizations();              //140513
}
      // returns the current triggered syncs and also clears resets them.
*/      
//NYN void Intercrop::initialize()                                                     { assert(false);           /* checking if needed */}
//NYN bool Intercrop::track_nitrogen_mass_balance(Crop_nitrogen_mass_balancer *_mass_balance)                         const { assert(false); return 0; /* checking if needed */}
// optional setup
//NYN   inline virtual void Intercrop::know_event_log(Common_simulation_log *_event_log)         { assert(false);           /* checking if needed */}
      // Use set_event_log if you want to record crop event and change of status.
      // This function is usually called once immediately after instanciating the crop object,
      // but it can be called at any time when you want to start logging events.
      // Pass 0 to disable logging.
      // Derived classes do not have to override this method if they don't want to support logging.
//NYN   inline virtual void Intercrop::know_residues(Residues_interface *residues)               { assert(false);           /* checking if needed */}
      // This method allows the crop to interact and exchange material with residue pools.
      // This function is usually called once immediately after instanciating the crop object.
      // This is optional, it allows the decomposing canopy submodel to be enabled.
      // In CropSyst, if the crop does not know about residues,
      // scenesced biomass in the plant canopy will not decompose while the plant is alive (I.e. for perennials)
      // (although biomass will be available for decomposition on crop termination.)

   // The following methods provided daily parameters required by optional submodels
   // When using any of these models,
   // the respective methods must be called prior to calling process().
   #if (CO2_RESPONSE_VERSION==4)
//NYN void Intercrop::set_daily_ratio_elevated_to_baseline_ppm_CO2(float64 ratio_from_ET = 1.0)                                           { assert(false);           /* checking if needed */}
      // In CropSyst, this value is calculated by the
      // cropSyst ET model which include the CO2 response.
   #endif
//NYN bool Intercrop::know_directed_irrigation_soil_wetting(const Directed_irrigation_soil_wetting *_soil_wetting)         cognition_ { assert(false); return 0; /* checking if needed */}
//______________________________________________________________________________
RENDER_INSPECTORS_DEFINITION(Intercrop)
{
   // NYN need to determine which crop to record
   // May need to have each recorder have an ID for the crop so that
   // we know which crop the variable reference is associated with;
   // however the reference is for a given variable recorder will
   // be different for each crop so it might not be necessariy to
   // uniquely identify these.
   // Furthermore this would not be an issure if the maintainer of the
   // variable recorders keeps separate lists of recorders for the
   // respective associated crops.
   const CS::Emanator &emanator =
      predominant_crop
      ? predominant_crop->render_inspectors(inspectors,context,instance_description)
      : CS::provide_global_emanators().know((nat32)this,instance_description,context);
   return emanator;

}
//_RENDER_INSPECTORS_DEFINITION_____________________________________2015-09-16_/
}//_namespace_CropSyst_________________________________________________________/

