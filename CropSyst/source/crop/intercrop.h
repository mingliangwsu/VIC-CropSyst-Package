#ifndef intercropH
#define intercropH
#ifndef INTERCROPPING
#  error This module is only needed if using the intercropping
#endif

#include "crop_interfaced.h"
#include <assert.h>
//______________________________________________________________________________
namespace CropSyst
{
//______________________________________________________________________________
class Intercrop
: public implements_ Crop_interfaced
{
   // This class, although not currently derived from CORN::Container will
   // behave like a container so implements such methods with simular functions.
   Crop_interfaced *predominant_crop;
      // predominant_crop will always be non 0 during normal processing
   Crop_interfaced *secondary_crop;
      // secondary crop may be 0.
   Recorded recorded;                                                            //190705
 public:
   Intercrop(const Air_temperature_average *air_temperature_avg);                //150209
 public: // Meta information (not critical, used for outputs
   inline virtual const char  *get_description()   const { return "intercrop"; } //checking if needed
 public: // The following are container manipulation
   Crop_interfaced *take(Crop_interfaced *crop_to_add)           appropriation_;
 public: // accessors
   inline Crop_interfaced *get_predominant_crop()                  modification_ { return predominant_crop; }
   inline Crop_interfaced *get_secondary_crop()                    modification_ { return secondary_crop; }
   inline virtual bool get_name(modifiable_ std::wstring &active_crop_name) const { active_crop_name.assign(L"Intercrop"); return true;} //150720
 public: // Crop_interfaced implementations
   virtual bool         is_terminate_pending()                            const;
   #if (CROPSYST_VERSION == 4)
   virtual bool         is_fallow_deprecated()                            const;
   #endif
   virtual              CropSyst::Phenology_interface &mod_phenology();
   virtual const        CropSyst::Phenology_interface &ref_phenology()    const;

   virtual float64      get_canopy_interception_global_total()            const;
   //202228 virtual float64      get_fract_canopy_cover()                          const;
      // WARNING probably need to aggregate with secondary crop
   virtual float64      get_leaf_water_pot()                              const;
   virtual void        *get_unique_identifier()                           const
   { assert(false); return 0; /* This should never be called at intercrop level*/}
   virtual Crop_model   get_crop_model()                                  const;
   virtual float64      get_canopy_resistance()                           const;
      // WARNING probably need to aggregate with secondary crop
public: // setters
   virtual bool         set_fallow_deprecated(bool fallow)        modification_;
   virtual bool         set_no_more_clippings_this_season()       modification_;
   virtual bool         get_no_more_clippings_this_season()               const;
   virtual void         end_season_if_no_dormancy()           { assert(false); } // should never be called for intercrop
   virtual void         set_terminate_pending()               { assert(false); } // should never be called for intercrop
   virtual int16        get_days_after_flowering() const{assert(false);return0;}// should never be called for intercrop
public:  // The following accessors are used only for output
         // Some are report details, some are runtime etc.
   virtual float64      get_pot_transpiration_m()                         const;
   virtual float64      get_intercepted_precipitation_m()                 const;
   virtual float64 intercept_precipitation(float64 water_reaching_plant) modification_; //140320

   /*200420 replaced by  describe_periods
   virtual const char  *describe__growth_stage
      (std::string &growth_stage_phrase)                                  const; //190613
   */
   virtual nat8 describe_periods(CORN::Text_list &descriptions)           const;
      // May want to compose description to include predominate and secondary descriptions
   virtual float64      get_recorded_root_depth_m()                       const;
   virtual float64   get_N_mass_canopy_kg_m2(bool recorded_before_harvest)const;
      // used only for output
   virtual float64      get_root_length_m()                               const;
   virtual float64      get_plant_height_m()                              const  {  assert(false);return predominant_crop->get_plant_height_m(); /* checking if needed */}
   virtual float64      get_active_fract_root_length_m(nat8 sublayer)     const  {  assert(false);return predominant_crop->get_active_fract_root_length_m(sublayer); /* checking if needed */}
   virtual float64 const *get_total_fract_root_length_m()                 const;
/* implemented
   {  Crop_interfaced *deepest_rooted_crop =
         (secondary_crop && secondary_crop->get_root_length_m() > predominate_crop->get_root_length_m())
         ? secondary_crop
         : predominate_crop;
      return deepest_rooted_crop->get_total_fract_root_length_m(); }
*/
      //130716  I think some how aggregation of predominant and secondary crop but not sure
   virtual float64      get_latest_yield_kg_m2()                          const;
   // This is the yield from the latest harvest event.
   virtual const CORN::Date &get_planting_date()                          const;
   virtual const CORN::Date &get_restart_date()                           const;
   virtual const CORN::Date &get_emergence_date()                         const;
   virtual const CORN::Date &get_flowering_date()                         const;
   virtual const CORN::Date &get_tuber_init_date()                        const;
   virtual const CORN::Date &get_grain_filling_date()                     const;
   virtual const CORN::Date &get_maturity_date()                          const;
   virtual const CORN::Date &get_harvest_date()                           const;
   #ifdef CROP_ORCHARD
   virtual CORN::date32 get_bud_break_date()                              const;
   virtual CORN::date32 get_initial_fruit_growth_date()                   const;
   virtual CORN::date32 get_rapid_fruit_growth_date()                     const;
   virtual CORN::date32 get_chill_requirement_satisfied_date()            const;
   #endif
   virtual CORN::date32 get_inactive_begin_date()                         const;
public:  // The following are for report details only,
         // Eventually they may be replaced by data_record I/O
   virtual float64 get_accum_degree_days
      (bool adjusted_for_clipping)                                         const { return predominant_crop->get_accum_degree_days(adjusted_for_clipping); /* daily output */}
   #ifdef CROP_ORCHARD
   virtual int16 get_fruit_chill_hours_remaining()                         const { return predominant_crop->get_fruit_chill_hours_remaining();  /* daily output */}
   virtual float64 get_fruit_dry_biomass_kg_ha()                           const { return predominant_crop->get_fruit_dry_biomass_kg_ha();      /* daily output */}
   virtual float64 get_orchard_solid_canopy_interception()                 const { return predominant_crop->get_orchard_solid_canopy_interception(); /* daily output */}
   virtual float64 get_orchard_shaded_area_GAI()                           const { return predominant_crop->get_orchard_shaded_area_GAI();      /* daily output */}
   virtual float64 get_orchard_canopy_porosity()                           const { return predominant_crop->get_orchard_canopy_porosity();      /* daily output */}
   #endif
   virtual float64 get_dry_biomass_kg_ha()                                 const { return predominant_crop->get_dry_biomass_kg_ha(); /* daily output */}
   virtual float64 get_live_green_canopy_biomass_kg_ha()                   const { return predominant_crop->get_live_green_canopy_biomass_kg_ha(); /* daily output */}
   virtual float64 get_act_root_biomass_kg_ha()                            const { assert(false);return predominant_crop->get_act_root_biomass_kg_ha(); /* checking if needed */}
   virtual float64 get_canopy_biomass_kg_ha()                              const { assert(false);return predominant_crop->get_canopy_biomass_kg_ha(); /* checking if needed */}
   virtual float64 get_VPD_daytime()                                       const { return predominant_crop->get_VPD_daytime(); /* daily output */}
   virtual float64 get_VPD_daytime_mean()                                  const { assert(false);return predominant_crop->get_VPD_daytime_mean(); /* checking if needed */}
   virtual float64 get_intercepted_PAR_MJ_m2()                             const { return predominant_crop->get_intercepted_PAR_MJ_m2(); /* daily output */}
   virtual float64 get_intercepted_PAR_accum_MJ_m2()                       const { return predominant_crop->get_intercepted_PAR_accum_MJ_m2(); /* daily output */}
   virtual float64 get_intercepted_PAR_season_accum_MJ_m2()                const { assert(false); return predominant_crop->get_intercepted_PAR_season_accum_MJ_m2(); /* checking if needed */}
   virtual float64 get_peak_LAI(bool reported)                             const { assert(false); return predominant_crop->get_peak_LAI(reported); /* checking if needed */}
   virtual float64 get_total_season_collected_biomass_kg_m2()              const { assert(false); return predominant_crop->get_total_season_collected_biomass_kg_m2(); /* checking if needed */}
   virtual const Crop_nitrogen_interface *get_nitrogen()                   const { return predominant_crop->get_nitrogen(); /* checking if needed */}
   virtual float64 update_root_depth
      (float64 soil_rooting_depth,float64 total_soil_depth)        modification_ { return 0; /* checking if needed */}
 public: // data record output
   virtual bool setup_structure
      (CORN::Data_record &data_rec
      ,bool for_write
      ,const Desired_variables &desired_vars)                      modification_ { assert(false);           /* checking if needed */}
 public:  // The following are used for accumulation and/or output
   virtual float64 get_act_transpiration_m()                              const;
   virtual float64 get_reported_harvest_index()                           const { return predominant_crop->get_reported_harvest_index(); /* daily output */}
   virtual float64 get_canopy_biomass_kg_m2()                             const { return predominant_crop->get_canopy_biomass_kg_m2();   /* daily output */}
   virtual float64 get_act_root_biomass_kg_m2()                           const { return predominant_crop->get_act_root_biomass_kg_m2(); /* daily output */}
   virtual float64 calc_root_biomass(float64 *output_root_BM_by_layer=0)  const {  assert(false);return predominant_crop->calc_root_biomass(output_root_BM_by_layer); /* checking if needed */}
   virtual float64 get_LAI(bool from_canopy)                              const;
   virtual float64 get_GAI(uint8 include_GAI)                             const {  return predominant_crop->get_GAI(include_GAI); /* daily output */}
   virtual float64 get_GAI_today()                                        const {  assert(false);return predominant_crop->get_GAI_today(); /* checking if needed */}
   virtual float64 get_temperature_stress()                               const {  return predominant_crop->get_temperature_stress(); /* daily output */}
   virtual float64 get_water_stress()                                     const {  return predominant_crop->get_water_stress(); /* daily output */}
   virtual float64 get_water_stress_index()                               const {  return predominant_crop->get_water_stress_index(); /* daily output  */}
   virtual float64 get_water_stress_index_mean()                          const {  assert(false);return predominant_crop->get_water_stress_index_mean(); /* checking if needed */}
   virtual float64 get_temperature_stress_index()                         const {  return predominant_crop->get_temperature_stress_index(); /* daily output */}
   virtual float64 get_temperature_stress_index_mean()                    const {  assert(false);return predominant_crop->get_temperature_stress_index_mean(); /* checking if needed */}
   //200127 not needed virtual float64 update_adjusted_ET_crop_coefficient() const {  assert(false);return predominant_crop->update_adjusted_ET_crop_coefficient(); /* checking if needed */}
   #ifdef OLD_YIELD
   #if (CROPSYST_VERSION==4)
   virtual float64 get_flower_temperature_stress_index_mean()             const {  assert(false);return predominant_crop->get_flower_temperature_stress_index_mean(); /* checking if needed */}
   virtual float64 get_flowering_N_stress_index_mean()                    const {  assert(false);return predominant_crop->get_flowering_N_stress_index_mean(); /* checking if needed */}
   virtual float64 get_flower_water_stress_index_mean_check_used()        const {  assert(false);return predominant_crop->get_flower_water_stress_index_mean_check_used(); /* checking if needed */}
   virtual float64 get_filling_duration_index_mean()                      const { assert(false);return predominant_crop->get_filling_duration_index_mean(); /* checking if needed */}
   #endif
   virtual float64 get_tuber_init_temperature_stress_index()              const {  assert(false);return predominant_crop->get_tuber_init_temperature_stress_index(); /* checking if needed */}
   virtual float64 get_tuber_fill_temperature_stress_index()              const {  assert(false);return predominant_crop->get_tuber_fill_temperature_stress_index(); /* checking if needed */}
   virtual float64 get_tuber_init_temperature_stress_index_mean()         const {  assert(false);return predominant_crop->get_tuber_init_temperature_stress_index_mean(); /* checking if needed */}
   virtual float64 get_tuber_fill_temperature_stress_index_mean()         const {  assert(false);return predominant_crop->get_tuber_fill_temperature_stress_index_mean(); /* checking if needed */}
   #endif

   virtual float64 get_overall_growth_stress()                            const { assert(false); return predominant_crop->get_overall_growth_stress(); /* checking if needed */}
   virtual float64 get_C_mass_canopy()                                    const;
   virtual float64 get_C_mass_roots()                                     const;
public:  // The following are parameter accessors
          virtual int32    param_LADSS_land_use_ID()                      const {  assert(false);return predominant_crop->param_LADSS_land_use_ID(); /* checking if needed */}
          virtual Land_use param_land_use()                               const;
          virtual float64  param_emergence_deg_day()                      const {  assert(false);return predominant_crop->param_emergence_deg_day(); /* checking if needed */}
   /*181111 appears to be unused
          virtual float64  param_max_canopy_deg_day()                     const { return predominant_crop->param_max_canopy_deg_day(); } //140512
   */
   inline virtual float64  param_wilt_leaf_water_pot()                    const {  assert(false);return predominant_crop->param_wilt_leaf_water_pot(); /* checking if needed */}
          virtual float64  param_base_temp()                              const {  assert(false);return predominant_crop->param_base_temp(); /* checking if needed */}
   #ifdef RESIDUES
          virtual Residue_decomposition_parameters
                  &get_residue_decomposition_parameters()                 const;
   #endif
public: // The following parameters are used for scaling runtime graph
          virtual float64  param_max_LAI()                                const {  assert(false);return predominant_crop->param_max_LAI(); /* checking if needed */}
/*160624 eliminating param_XXX() methods because now passing the parameter data structures to the classes where used
   inline virtual float64  param_max_root_depth_m()                       const {  assert(false);return predominant_crop->param_max_root_depth_m(); /* checking if needed */}
      // Derived classes may override param_max_root_depth_m if available
      // it is used primarily for fastgraph scaling.
*/
public: // The following are for LADSS only
   virtual int32 get_LADSS_land_use_ID()                                  const {  assert(false);return predominant_crop->get_LADSS_land_use_ID(); /* checking if needed */}
   virtual const char *get_sowing_event_ID()                              const { assert(false); return predominant_crop->get_sowing_event_ID(); /* checking if needed */}
public:
   // The following are used to implement operations and events
   // that remove biomatter from the plant
   virtual                                                                     
      Crop_mass_fate_biomass_and_nitrogen *                                    
      process_biomass_fate
         (Biomass_fated_cause fated_cause
         ,Harvest_amount_mode harvest_amount_mode                              
         ,float64 remove_amount_kg_m2                                          
         ,float64 retain_GAI                                                   
         ,float64 retain_biomass_kg_m2
         ,const Biomass_fate_parameters  &biomass_fate_parameters
         ,bool terminate
         ,float64 yield_now)                                                     { assert(false); return 0; /* checking if needed */}
      // This moves plant biomass to various pools, harvest yield, residue, loss etc..
      // Returns the fate of the material if processed otherwise 0
      // (call must delete the returned fate object)
   virtual void commit_biomass_fate()                                          ;
      // This should be called daily, it checks if there is
      // any biomatter fate and generates a log.
      // 140314 This is primarily for output only.
   virtual const Crop_mass_fate *get_biomass_fate_today_record()          const;
   virtual Crop_mass_fate *get_biomass_fate_season()                      const  {  assert(false);return predominant_crop->get_biomass_fate_season(); /* checking if needed */}                            //040624
public:  // The following are processes used by the calling program
   float64 rain_interception(float64 water_reaching_plant)                       { assert(false); return 0; /* checking if needed */}
   virtual                                                                      
      Crop_mass_fate_biomass_and_nitrogen *
      clip_biomass
      (Harvest_amount_mode harvest_amount_mode
      ,float64 remove_amount_kg_m2                                              
      ,float64 min_retain_GAI                                                   
      ,float64 min_retain_biomass_kg_m2
      ,const Biomass_fate_parameters &biomass_fate_parameters
      ,Biomass_fated_cause fated_cause)                                          { assert(false); return 0; /* checking if needed */}
      // Returns the fate of the biomass (must be deleted by the caller)
   virtual bool respond_to_clipping()                              modification_ { assert(false); return 0; /* checking if needed */}
   bool check_for_automatic_clipping                                             
      (const CropSyst::Auto_clip_biomass_mode &clip_biomass_op
      ,float64 remove_amount_kg_m2                                               
      ,bool   clip_now_or_never
      ,float64 avg_temperature
      ,Common_simulation_log *log)                                               { assert(false); return 0; /* checking if needed */}                                              //050929
      // Returns true if clipping actually performed.
   virtual bool sow(bool _fall_sowing)                                           { assert(false); return 0; /* checking if needed */}
   virtual bool start_day();
   virtual bool end_day();
   virtual bool process_day();
   #ifdef NITROGEN
   virtual void set_automatic_nitrogen_mode(bool enable)                         { assert(false);           /* checking if needed */}                        //040525
   #endif
   virtual Crop_mass_fate_biomass_and_nitrogen *harvest                         
      (float64 remove_amount_kg_m2
      ,const CropSyst::Harvest_or_clipping_operation_struct *harvest_or_clipping_params
      ,bool terminate){ assert(false); return 0; /* checking if needed */}
      // Returns the fate of the biomass if it could harvest                   
      // otherwise 0 (must be deleted by the caller)
   virtual Crop_mass_fate_biomass_and_nitrogen *harvest_crop
      (Harvest_amount_mode harvest_amount_mode                                 
      ,float64 remove_amount_kg_m2                                             
      ,float64 retain_GAI
      ,const Biomass_fate_parameters &harvest_biomass_fate_parameters
      ,bool terminate)                                                           { assert(false); return 0; /* checking if needed */}
      // Returns the fate of the biomass if it could harvest
      // otherwise 0 (must be deleted by the caller)
   virtual float64  update_evapotranspiration_max(float64 ET_ref_m) rectification_;
      //140320 RLN need to check with Claudio.
   virtual float64 est_degree_days_X_days_from_now
      (const CORN::Date & Date_const, CORN::Days days_from_now)            const { assert(false); return 0; /* checking if needed */}
      // This returns an estimate (in this case the actual)
      // of the thermal time the crop will have in the specified
      // number of days from the current date.
   virtual bool initialize()                                                     { assert(false);  return true; /* checking if needed */}
   virtual bool track_nitrogen_mass_balance
      (CropSyst::Crop_nitrogen_mass_balancer *_mass_balance)               const { assert(false); return 0; /* checking if needed */}
public: // optional setup
   inline virtual void know_event_log(Common_simulation_log *_event_log)         { assert(false);           /* checking if needed */}
      // Use set_event_log if you want to record crop event and change of status.
      // This function is usually called once immediately after instanciating the crop object,
      // but it can be called at any time when you want to start logging events.
      // Pass 0 to disable logging.
      // Derived classes do not have to override this method if they don't want to support logging.
   inline virtual void know_residues(Residues_interface *residues)               { assert(false);           /* checking if needed */}
      // This method allows the crop to interact and exchange material with residue pools.
      // This function is usually called once immediately after instanciating the crop object.
      // This is optional, it allows the decomposing canopy submodel to be enabled.
      // In CropSyst, if the crop does not know about residues,
      // scenesced biomass in the plant canopy will not decompose while the plant is alive (I.e. for perennials)
      // (although biomass will be available for decomposition on crop termination.)
public:
   // The following methods provided daily parameters required by optional submodels
   // When using any of these models,
   // the respective methods must be called prior to calling process().
   #if (CO2_RESPONSE_VERSION==4)
   #error reached
   virtual void set_daily_ratio_elevated_to_baseline_ppm_CO2
         (float64 ratio_from_ET = 1.0)                                           { assert(false);           /* checking if needed */}
      // In CropSyst, this value is calculated by the
      // cropSyst ET model which include the CO2 response.
   #endif
   virtual bool know_directed_irrigation_soil_wetting
      (const Directed_irrigation_soil_wetting *_soil_wetting)         cognition_ { assert(false); return 0; /* checking if needed */}
 public: // The following are added for REACCH and OFOOT and will probably be in V5
   RENDER_INSPECTORS_DECLARATION;
};
//_Intercrop________________________________________________________2013-07-11_/
}//_namespace_CropSyst_________________________________________________________/
#endif
