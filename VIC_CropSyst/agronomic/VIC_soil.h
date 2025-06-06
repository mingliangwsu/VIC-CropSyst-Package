#ifndef VIC_soilH
#define VIC_soilH
#ifdef VIC_CROPSYST_VERSION
#include "soil.h"

#ifdef VCS_V5
#include "vic_def.h"
#else
#include "vicNl_def.h"
#endif

#include "soil/temperature_I.h"
#include "soil/temperature_hourly_I.h"
#include "soil/hydrology_A.h"

#include "soil/layers.h"
#include <assert.h>                                                              //150922LML

//extern bool need_irrigation(double MAD_crop_specific
//                               , double depletion_obs_depth_mm
//                               , int rec
//                               , double applied_irrigation
//                               , layer_data_struct *layer                        //150701LML
//                               , const soil_con_struct &soil_con                 //150701LML
//                               , double &irrigation_demand
//#if (FULL_IRRIGATION==FALSE)
//                               ,int rotation_code
//#endif
//                               , bool fill_water_to_fc
//                               , double max_capacity                             //190724LML
//                               , double *real_added_water                        //190724LML
//                               , double top_layer_to_fc_fraction                 //190805LML control drip irrigations over fruit trees (vine in this case)
//                               );
namespace VIC
{
//190128RLN class Soil;
//______________________________________________________________________________
class Soil_layers
: public extends_ CS::Soil_layers                                                //150424
//150424 : public implements_ Soil_layers_interface
{
protected:   //190128 friend class Soil;
   provided_ float64 depths[MAX_soil_layers_alloc];    provided_ bool depths_provided;
   provided_ float64 thickness[MAX_soil_layers_alloc]; provided_ bool thickness_provided; //LML 150403
public:
   inline virtual nat8  count_max()            const { return MAX_soil_layers; }
      // This is the absolute maximum number of layers
      // (array dimension) that are reserved to store soil profile data by layer.
   virtual nat8  count()                const; //190102RLN { return options.Nlayer; }
      // This is the number of layers identified in the soil profile.
   inline virtual nat8    get_number_sublayers()                           const { return count(); }  //200715LML
   virtual nat8  count_uninundated()    const; //190102RLN { return options.Nlayer; }
      // This is usually the same value returned by count()
      // however, in the CropSyst finite difference model when there is a water
      // table extending up into the vadose zone, this is the number of layers
      // (from the top of the profile) that are not inundated
      // [used to be called get_number_effective_layers]
      // Generally this represents the vadose zone, but there could
      // indeed by soil layers below the simulated soil profile and the water table.

 public: // Soil profile parameter accessors (layer number is 1 based)
   virtual float64 get_thickness_m                       (nat8 layer)      const
      { return thickness[layer]; /* probably needs to be implemented */ }
      // These return the thickness of the specified layer
   virtual float64 get_depth_m                           (nat8 layer = 0)  const
      { return (layer == 0) ? (count() - 1) : depths[layer];}
      // These return the depths to the bottom of the specified layer.
   virtual float64 get_layer_center_depth(nat8 sublayer)                   const
      { UNUSED_arg(sublayer); assert(false); return 0; /* not currently needed in this model */ }
   public: // set accessors
    virtual nat8 set_number_uninundated_layers(nat8 new_uninundated_layers)modification_
       { UNUSED_arg(new_uninundated_layers); assert(false); return 0; /* not currently needed in this model */ }

   public:  //LML 150403 For this class only.
   inline virtual void set_depth_m(nat8 layer, float64 depth_m)                  {depths[layer] = depth_m;}         //LML 150403
   inline virtual void set_thickness_m(nat8 layer, float64 thickness_m)          {thickness[layer] = thickness_m;}  //LML 150403
   virtual bool set_thickness_by_depth(nat8 tolayer);                           //LML 150403
   virtual bool set_depth_by_thickness(nat8 tolayer);                           //LML 150403
   inline virtual void set_depths_provided(bool bset)                            {depths_provided = bset;}          //LML 150403
   inline virtual void set_thickness_provided(bool bset)                         {thickness_provided = bset;}       //LML 150403
   inline virtual bool get_depths_provided()                                     {return depths_provided;}          //LML 150403
   inline virtual bool get_thickness_provided()                                  {return thickness_provided;}       //LML 150403
   inline virtual bool is_bound_by_impermeable_layer()                     const {return false;}                    //161031LML

 public: //structors
    Soil_layers();
    virtual ~Soil_layers();                                                      //150318
 private:
    Soil_layers *check_abstract() { return new Soil_layers();}
};
//_2014-12-07________________________________________________class:Soil_layers_/
class Soil_hydrology
: public implements_ Soil::Hydrology_abstract //Hydrology_abstract                                    //190128
//190128 Soil::Hydrology_interface //190128 RLN Soil_hydrology_interface
{  // Refer to the file soil/hydrology_interface.h for documentations on these methods

 public: //friend class Crop_with_soil_weather;
//      const Soil_hydraulic_properties_interface &hydraulic_properties;
// WARNING these are currently implemented as stubs for the testing.
   //soil_layer_array64(water_content);  // volumetric
 protected:
   const Soil::Layers_interface &layers;                                    //190128RLN
   const Soil::Hydraulic_properties_interface &hydraulic_properties_ref;         //190128RLN_150529LML
 private: // VIC data structures
   /*LML 150415 const*/layer_data_struct *VIC_layer;       //Array                                 //141207
   const soil_con_struct   &VIC_soil_con;
 private: // The following are copied CropSyst::Soil_hydrology_abstract we probably could have a more generalize base class RLN
   soil_layer_array64(reference_water_content);
   soil_layer_array64(real_wilt_point);
   soil_layer_array64(plant_avail_water);                                        //150609LML 0-1    current
 public: // Soil profile state accessors
   virtual float64 get_water_plus_ice_content_gravimetric (nat8 layer)     const
      { UNUSED_arg(layer); assert(false); return 0.0; /* this model does not use this method */ }
   virtual float64 get_liquid_water_content_volumetric    (nat8 layer)     const;
   virtual float64 calc_water_depth_profile         (bool liquid_and_ice)  const; //170628LML
   virtual float64 get_water_plus_ice_content_volumetric_at
      (nat8 layer,CORN::Hour hour)                                         const
      { UNUSED_arg(layer); UNUSED_arg(hour); return get_water_plus_ice_content_volumetric(layer);} //MAY NEED HOURLY!!! 151022LML
   virtual float64 get_water_plus_ice_content_volumetric  (nat8 layer)     const; //170525LML
   inline virtual float64 get_water_filled_porosity
      (nat8 layer, CORN::Hour for_hour)                                    const;

   virtual float64 get_water_potential             (nat8 layer)     const;
   inline virtual float64 provide_water_depth_profile(bool all_water) provision_ { UNUSED_arg(all_water); return layers.get_depth_profile_m(); }
   inline virtual float64 get_plant_avail_water(nat8 layer)                const
      { return calc_available_water_fraction/*190131RLN I think I renamed this calc_available_water*/
      (layer,true)
      /*150609LML talk with Roger plant_avail_water[layer]*/; /*150609LML*/
      }
   virtual float64 calc_available_water_fraction(nat8 layer, bool real_wilt_based) const; //160629LML
   virtual float64 calc_available_water_depth_to_depth(float64 depth) calculation_; //190128RLN_160629LML
   //190128 virtual float64 calc_available_water_depth_to(float64 depth)     calculation_; //160629LML
   //190131RLN abandoned virtual float64 calc_available_water_depth_to_layer(nat8 layer) calculation_; //190128RLN
   virtual float64 calc_ice_depth_to(float64 depth)                 calculation_; //160812LML
   /*190131RLN abandonded, deriving from Hydrology_abstract
   virtual float64 calc_available_water
      (nat8 layer , bool real_wilt_based)                                  const;

   virtual float64 calc_water_depth_to_layer
      (bool all_water, nat8 layer=PROFILE_LAYER_INDEX)                    const; //190128RLN
   virtual float64 calc_water_depth_to_depth
      (bool all_water, float32 depth=9999)                                const; //190128RLN
   */
/*190129
   virtual float64 initialize_moisture(nat8 layer                                //190128RLN
      ,Moisture_characterization moisture ,float64 WC_initial)    modification_;
*/
   inline virtual float64 get_K_Theta
      (nat8 layer
      ,float64 Theta, float64 Theta_s)                                     const
      { UNUSED_arg(layer); UNUSED_arg(Theta); UNUSED_arg(Theta_s);assert(false); return 0.0;  } // this model does not use this method
   virtual float64 *get_liquid_water_content_layers_volumetric                   //141207
      (soil_layer_array64(water_content_layers))                           const;
   inline virtual float64 get_reference_water_content(nat8  sublayer)      const //150424RLN
      { return reference_water_content[sublayer]; }
   virtual float64 get_air_entry_pot
      (nat8 layer,bool consider_ice_content,bool FC_PWP_based)             const; //150529LML { assert(false); return 0.0; /* this model does not use this method */ }
   inline virtual float64 get_real_wilt_point  (nat8 layer)                const //140424RLN
      { return real_wilt_point[layer]; }
   virtual float64* ref_plant_avail_water()                                      { return plant_avail_water; /*150609LML*/ }
   virtual float64 calc_water_potential
      ( nat8 layer,bool consider_ice,bool FC_PWP_based)                    const; //150608LML used for transoiration { assert(false); return 0.0; /* this model does not use this method */ }
 public: // soil freezing accessors
   // Derived classes may return 0 in the case of no soil freezing simulation.
          virtual float64 get_ice_content_volumetric(nat8 layer)           const; //141207RLN
   inline virtual float64 get_ice_content_volumetric_yesterday(nat8 layer) const { UNUSED_arg(layer); assert(false); return 0.0; /* this model does not use this method */ }
   inline virtual bool is_frozen(nat8 layer)                               const { UNUSED_arg(layer); assert(false); return false; /* this model does not use this method */ }
 public: // set state accessors
   inline virtual float64 initialize_moisture                                           //170323
      (nat8 layer
      ,Moisture_characterization moisture
      ,float64 water_content_initial)                              modification_ //170323LML
      { UNUSED_arg(layer); UNUSED_arg(moisture); assert(water_content_initial); return 0;};
   inline virtual float64 set_water_content_volumetric_layer
      (nat8 layer, float64 new_water_content)                      modification_
      { UNUSED_arg(layer); UNUSED_arg(new_water_content); return 0; }  // This model does not maintain the water content.
   virtual float64 change_liquid_water_content_by_volumetric
      (nat8 layer, float64 change_WC)                              modification_;

      //190201 moved to cpp { return VIC_layer[layer-1].moist += change_WC * soil_layers.get_thickness_mm(layer); } //150529LML
      // used by tillage_effect
      // Pass a positive value to increase the WC and negative to decrease.
      // Returns the new water content.
      // Note that change_WC is probably always liquid so this will work
      // for either liquid or total water basis.
   inline virtual float64 set_plant_avail_water(nat8 layer, float64 _plant_avail_water_sl)      modification_
      {  return plant_avail_water[layer] = _plant_avail_water_sl; }              //150529LML
   inline virtual float64 set_real_wilt_point                                    //150424RLN
      (nat8 layer, float64 real_wilt_point_)
      { return real_wilt_point[layer] = real_wilt_point_; }
   inline virtual float64 set_reference_water_content                            //150424RLN
      (nat8  layer, float64 reference_water_content_)              modification_
      { return reference_water_content[layer] = reference_water_content_; }      //140610
 public: // processes
   virtual float64 extract_water_mm(float64 water_uptake_mm[] , nat8 start_layer)
      {  UNUSED_arg(water_uptake_mm); UNUSED_arg(start_layer); assert(false);
         return 0.0; /* this model does not use this method */ }
   virtual float64 extract_water(float64 water_uptake_m[] ,nat8 start_layer);
   // This function should remove the specified soil water (in depth) from each of the layers.
   // The specified water uptake is usually extracted do to crop transpiration,
   // but this function may also be used for other functions such as evaporation
   // or other water fates.
   // The function should return the actual water uptake.
   // The caller of this function will normally only specify the amount to exctract
   // based on the current water content for the respective layers.
   // So the returned value should usually match the sum of the passed water_uptake_m
   // array.
   // If for some reason, there was not enough water in the soil, then
   // the returned value may be less than the sum of the passed array indicating that there
   // was not enough water.
   // It is suggested to change the array values to the actual
   // values that could be uptaken from each layer.
   float64 refill_water_depth_m(bool assume_irrigation,nat8 observation_layers) modification_
   {  UNUSED_arg(assume_irrigation); UNUSED_arg(observation_layers); assert(false); // this model does not use this method
      return 0.0; }
public:  // process daily
   Soil_hydrology
      (
       const Soil::Layers_interface                &soil_layers_                 //190128RLN
      ,const Soil::Hydraulic_properties_interface  &hydraulic_properties_ref_    //190128RLN_150529LML
      ,const Soil::Structure                       &soil_structure_              //190131RLN
      ,const soil_con_struct                       &VIC_soil_con_
      ,layer_data_struct                           *VIC_layer_
      );

 //     : Soil::Hydrology_abstract(soil_layers_,hydraulic_properties_ref_)         //190128

/*
   inline Soil_hydrology()
      : Soil::Hydrology_interface()                                              //190128
      , soil_layers(soil_layers_)
      , hydraulic_properties_ref(hydraulic_properties_ref_)                      //150529LML
      , VIC_soil_con(_VIC_soil_con)
      , VIC_layer(_VIC_layer)                                                    //LML 150422
      {}
*/

   inline virtual bool start_day()                                 modification_ { assert(false); /* this model does not use this method */ return true; }
   inline virtual void update_liquid_and_ice_water_content()       modification_ { assert(false); /* this model does not use this method */ }
   float64  refill_water_depth_m(bool and_refill,nat8 observation_layers)  const
      {  UNUSED_arg(and_refill); UNUSED_arg(observation_layers); assert(false); /* this model does not use this method */
         return 0.0; }
      ///\returns the depth of water that would refill the soil to field capacity.
      /// If and_refill if enabled, the WC will also be set to FC
      /// (as an autoirrigation).
   #ifdef UNIT_TEST_SOIL
   void dump_water_content(std::ofstream &dump_stream)                     const
   {  UNUSED_arg(dump_stream); assert(false); /* this model does not use this method */ }
   #endif
   //LML 150422
   inline virtual void set_VIC_layer_array(layer_data_struct *orig_VIC_layer) modification_ {VIC_layer = orig_VIC_layer;}
 #ifdef USE_VARIABLE_RECORDERS
 public:
   virtual bool render_variable_recorders
      (CS::Variable_recorders &recorders
      ,nat32 desired_descriptive_summary_elements)                /*rendition_*/
      {  UNUSED_arg(recorders); UNUSED_arg(desired_descriptive_summary_elements);
         return false; /* this model does not use this method */ }
 #endif
 private:
/*190128 temp disabled
   Soil_hydrology *find_abstract_class_error()                             const
      {  //Soil_layers_interface soil_layers;
         return new Soil_hydrology
            (soil_layers,hydraulic_properties_ref,VIC_soil_con,VIC_layer);}
      // This method is not used other to find if Soil_hydrology class is abstract.
*/
   virtual void record_water_content_for_hour(CORN::Hour hour)
      {  UNUSED_arg(hour); /* this model does not use this method */ }
      //      { /* this is just for output do nothing by defaut */ }
   virtual Soil::Hydrology_interface *copy(float32 fraction)               const //190128RLN
      {  UNUSED_arg(fraction); return 0;}
   // this model does not use this method
   virtual void expand_last_layer_for_boundary_conditions
      ( nat8 number_layers, nat8 number_expanded_layers)           modification_
      { /* this model does not use this method */
         UNUSED_arg(number_layers); UNUSED_arg(number_expanded_layers); }
   inline const char *get_ontology()                                       const { return "VIC_soil"; } //160629LML
   RENDER_INSPECTORS_DECLARATION;                                                //150922LML
};
//_2014-12-07_____________________________________________class:Soil_hydrology_/
class Soil_temperature
: public implements_ Soil::Temperature_interface                                 //190128RLN
{
   const layer_data_struct *VIC_layer;  // not owned
 public: // strutors
   inline Soil_temperature(const layer_data_struct * _VIC_layer)
      { VIC_layer = _VIC_layer; }
   inline ~Soil_temperature() {}
 public:  // Accessors
   inline virtual float64 get_temperature(nat8 layer)      const;
   /*190201 moved to cpp
      {  nat8 VIC_layer_index = layer -1;
         return VIC_layer[VIC_layer_index].T;
      }
   */
};
//_2014-12-07_____________________________________________class:Soil_temperature_/
class Soil_hydraulic_properties
: public implements_ Soil::Hydraulic_properties_interface                        //190128RLN
{
   const soil_con_struct &VIC_soil_con;
 public:
   Soil_hydraulic_properties (const soil_con_struct &VIC_soil_con_)
     :VIC_soil_con (VIC_soil_con_)
     {}
   virtual float64 get_field_capacity_volumetric             (nat8 layer) const; //190201RLN { return mm_to_m(VIC_soil_con.Wcr[layer-1]) / VIC_soil_con.depth[layer - 1]; }
   virtual float64 get_permanent_wilt_point_volumetric       (nat8 layer) const; //190201RLN { return mm_to_m(VIC_soil_con.Wpwp[layer-1]) / VIC_soil_con.depth[layer - 1]; }
   virtual float64 get_drained_upper_limit_volumetric        (nat8 layer) const  { UNUSED_arg(layer); assert(false);return 0; /* need to check if needed in this model */ }
   // Note: I think this may be field capacity
   virtual float64 get_lower_limit_volumetric                (nat8 layer) const  { UNUSED_arg(layer); assert(false);return 0; /* need to check if needed in this model */ }
   // Note: I think this may be PWP
   virtual float64 get_saturation_water_content_volumetric   ( nat8 layer,float64 current_ice_water_content) const;
   //190201 moved to cpp   { UNUSED_arg(current_ice_water_content); return mm_to_m(VIC_soil_con.max_moist[layer-1]) / VIC_soil_con.depth[layer - 1]; }
   //    Usually {  return 1.0 - get_bulk_density_g_cm3(layer) / 2.65;
   inline virtual float64 get_unsaturated_conductivity_function_slope( nat8 layer)const { UNUSED_arg(layer); assert(false); return 0; /* need to check if needed in this model */ }
   inline virtual float64 get_bypass_coef                           ( nat8 layer) const { UNUSED_arg(layer); return 0; /* 150729LML */ }
   virtual float64 get_sat_hydraul_cond_kg_s_m3              (nat8 layer) const;
   //190201 moved to cpp { return VIC_soil_con.Ksat[layer-1] * 1.18023e-6; } //Need to check
   //inline virtual float64 get_sat_hydraul_cond_m_hr          ( nat8 layer) const
   //   { return  get_sat_hydraul_cond_kg_s_m3(layer) * 0.00981 * 3600.0; }
      //< \return sat_hydraul_cond converted to m/hr
   virtual float64 get_Campbell_b          (nat8 layer,bool FC_PWP_based) const; //190201 moved to cpp { UNUSED_arg(FC_PWP_based); return VIC_soil_con.VCS.b_campbell[layer-1]; }
   virtual float64 get_air_entry_potential (nat8 layer,bool FC_PWP_based) const; //190201 moved to cpp { UNUSED_arg(FC_PWP_based); return VIC_soil_con.VCS.AE[layer-1]; }
   virtual float64 get_water_pot_at_FC     (nat8 layer)                   const; //190201 moved to cpp { return VIC_soil_con.VCS.water_pot_at_FC[layer-1]; }  //170504LML -33.0; /* need to check the value  */ }
   virtual const float64 *ref_soil_water_pot_at_FC()                      const; //190201 moved to cpp { return VIC_soil_con.VCS.water_pot_at_FC; }           //170504LML
};
//_2015-03-31___________________________________________________________________

//150422  Warning this Soil_temperature_profile_hourly class is only
// a temporary place holder until Roger can hookup the CropSyst proper version.

class Soil_temperature_profile_hourly
: public implements_ Soil_temperature_profile_hourly_interface
{
   const Soil_temperature &temperature_profile;
private:
   mutable bool has_snow_cover;                                                  //160921LML
public:
   inline virtual ~Soil_temperature_profile_hourly() {}
   inline Soil_temperature_profile_hourly
      (const Soil_temperature &_temperature_profile)
      : temperature_profile(_temperature_profile)
      {}
   inline virtual float64 get_temperature(CORN::Hour hour, nat8 layer)     const
      { UNUSED_arg(hour); return  temperature_profile.get_temperature(layer); }
   inline virtual bool initialize()                              initialization_
      { return true; }
   inline virtual bool know_has_snow_cover(bool has_snow_)            cognition_ //160812LML
      { UNUSED_arg(has_snow_); return false; }
};
//______________________________________________________________________________
class Soil_components // Soil 190128RLN renamed be because now Soil is namespace
: public extends_ CropSyst::Soil_components                                      //190128RLN
{
 private:
   double daily_infiltrated_water_m;                                             //151001LML
 public:
   Soil_layers                      layers;
   Soil_hydrology                   hydrology;
   Soil_temperature                 temperature;
   Soil_hydraulic_properties        hydraulic_properties;                        //150331

   const soil_con_struct            &VIC_soil_con;
   VIC::Soil_temperature_profile_hourly  temperature_profile_hourly;             //150424
   Physical::Water_depth            evaporation_potential_remaining;             //160419LML
 public:
      Soil_components
         (const CORN::date32 &simdate
         ,const CS::Annual_temperature &_annual_temperature                      //150424
         , /*190128 const */ layer_data_struct *VIC_layer
         , const soil_con_struct   &_VIC_soil_con);
   inline virtual const Soil::Hydrology_interface *ref_hydrology()         const { return &hydrology; }


      // Returns the current soil hydrology state and process object.
      // Generally this is required in derived classes.
      // Generally derived classes will return the object.
   inline virtual modifiable_ Soil::Hydrology_interface *mod_hydrology() modification_ { return &hydrology; } //190128RLN
   inline virtual provision_ Soil::Temperature_interface *provide_temperature_profile() provision_ { return &temperature; }//141212
   inline virtual provision_ Soil_temperature_profile_hourly_interface                             //150424 161103LML added provision after virtual
      *provide_temperature_profile_hourly()                           provision_
      { return &temperature_profile_hourly; }

   inline virtual float64 get_temperature_C                     (nat8 layer)      const { return temperature.get_temperature(layer); }
   inline virtual float64 get_act_water_entering_soil()                           const { return daily_infiltrated_water_m;} //151001LML
   inline virtual modifiable_ float64 &mod_act_water_entering_soil()      modification_ { return daily_infiltrated_water_m;} //160502LML

   inline virtual float64 get_act_evaporation()                                   const { assert(false); return 0; } // Not currently needed
   inline virtual float64 get_pot_evaporation()                                   const { assert(false); return 0; } // Not currently needed
   inline virtual bool start_day()                                         modification_{ assert(false); return true; } // Not currently needed
   inline virtual bool process_day()                               modification_ { return CropSyst::Soil_components::process_day(); } //190128RLN_150406LML Not currently needed
   inline virtual Seconds   get_hydrology_timestep_resolution_today()             const { assert(false); return 0; } // Not currently needed
   inline virtual unmodifiable_ Soil::Layers_interface *ref_layers()       const { return &layers; }
   inline virtual modifiable_ Soil::Layers_interface *mod_layers() modification_ { return &layers; }
      // Returns the soil layering system.
      // This is required in derived classes.
      // It should never return 0.
   inline virtual unmodifiable_ Soil::Texture_interface  *ref_texture()    const   { return Soil_base::ref_texture(); } //150422LML  used by organic_matter_pools
   inline virtual modifiable_ Soil::Texture_interface *mod_texture() modification_ { return Soil_base::mod_texture(); } //150422LML  used by organic_matter_pools
      // Returns the soil texture parameters object.
   inline virtual unmodifiable_ Soil::Hydraulic_properties_interface *ref_hydraulic_properties() const
      { return &hydraulic_properties; } // Not currently needed
      // Returns the soil properties that are commonly derived from soil survey data
      // and/or pedo transfer functions.
   inline virtual   modifiable_ Soil::Hydraulic_properties_interface *mod_hydraulic_properties() modification_
      { return &hydraulic_properties; } // Not currently needed
      // Returns the soil properties that are commonly derived from soil survey data
      // and/or pedo transfer functions.
   virtual float64 &ref_act_water_entering_soil() {return daily_infiltrated_water_m;} //151001LML&RLN
   inline virtual float64 distribute_amount                                      //150424RLN
      (const soil_horizon_array32(from_horizon_amount)
      ,soil_sublayer_array_64(to_layer_amount))                            const
      { //200715LML return /*190128RLN Soil_interface*/CropSyst::Soil_components::distribute_amount(from_horizon_amount,to_layer_amount);


       return 0;   //200715LML not implemented yet!

      }
      // Bypass Soil_base which uses sublayers and default to Soil_interface implementation

   inline virtual float64 get_rooting_depth()                              const { return 0.05; } //150424RLN
   //150421 Roger need to fix these
   inline virtual float64 get_pH(nat8  sublayer)                           const { UNUSED_arg(sublayer); return 7.0;}  //011130
   inline float64 set_daily_infiltrated_water_m (float64 infiltration_m)         { return daily_infiltrated_water_m = infiltration_m;} //151001LML
   inline virtual float64 distribute_property
      (const soil_horizon_array32(from_horizon_property)
      ,soil_sublayer_array_64(to_sublayer_property))                       const
      { UNUSED_arg(from_horizon_property); UNUSED_arg(to_sublayer_property); return 0;}; //51120
};
//______________________________________________________________________________
} // namespace VIC

#endif //VIC_CROPSYST_VERSION
#endif //VIC_soilH
