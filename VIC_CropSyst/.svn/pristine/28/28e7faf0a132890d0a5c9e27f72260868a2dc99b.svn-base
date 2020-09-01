#ifndef VIC_crop_weatherH
#define VIC_crop_weatherH
#if (VIC_CROPSYST_VERSION<3)
#include "VIC_crop.h"
#else
#include "VIC_CropSyst_proper_crop.h"
#endif
#ifndef measureH
# include "corn/measure/measures.h"
#endif
#ifndef date_typesHPP
#  include "corn/chronometry/date_types.hpp"
#endif
#include <assert.h>
#ifndef OLD_SUBLAYERING
#  include "soil/soil_properties_sublayers.h"
#else
#  include "CropSyst/soil/soil_interface.h"
#endif
//______________________________________________________________________________
/** \class This version of the Crop class maintains the soil and weather
*/
#ifdef CROPSYST_PROVIDED_ET
#  include "biometeorology/ET_reference.h"
#else
#  include "biometeorology/ET_reference_interface.h"
// loc_param and solar rad may not actually be needed but
// weather provider currently expects them.
//#  include "weather/loc_param.h"
#  include "weather/solar_rad.h"

#endif
#include "soil/hydrology_interface.h"
#include "common/geodesy/geolocation.h"
#ifdef CROPSYST_PROVIDED_ET
//140815 Trying now a simpler weather provider for now
// but likely will need full weather provider at some point
#include "common/weather/weather_provider.h"
#else
#include "VIC_weather_provider.h"
#endif
//______________________________________________________________________________
using namespace CropSyst;
namespace VIC_crop
{
//______________________________________________________________________________
class Crop_with_weather
{
 protected:
   CORN::Date today;   /*int32 */
 public:
   //___________________________________________________________________________
      Season_S_W_SF season;
      Geolocation                   geolocation;
      #ifdef CROPSYST_PROVIDED_ET
      Location_parameters           location;
      Extraterrestrial_solar_irradiance_day  ET_solar_irradiance; // MJ/m2       //110113
      #endif
      VIC::Weather_provider              weather;
      float64                       aerodynamic_resistance_ref_plants;           //140811
      float64                       snow_pack_thickness;                         //141206
#ifdef UNIT_TEST
      bool headers_output;
#endif
   //___________________________________________________________________________
   #ifdef OLD_SUBLAYERING
   //nat8 number_VIC_horizons;
   float64 VIC_horizon_thickness[10]; // Indexed from 1 Actually VIC currently only has 3 layers
   nat8 VIC_horizon_at_layer[42];
   #endif
   //___________________________________________________________________________
   #ifndef CROPSYST_PROVIDED_SOIL_HYDROLOGY
   class Soil_layers
   : public implements_ Soil_layers_interface
   {
      provided_ float64 depths[MAX_soil_layers_alloc]; provided_ bool depths_provided;
   public:
    virtual nat8  get_max_number_layers()                                  const;
      // This is the absolute maximum number of layers
      // (array dimension) that are reserved to store soil profile data by layer.
    virtual nat8  get_number_layers()                                      const;
      // This is the number of layers identified in the soil profile.
    virtual nat8  get_number_uninundated_layers()                          const;
      // This is usually the same value returned by get_number_layers()
      // however, in the CropSyst finite difference model when there is a water
      // table extending up into the vadose zone, this is the number of layers
      // (from the top of the profile) that are not inundated
      // [used to be called get_number_effective_layers]
      // Generally this represents the vadose zone, but there could
      // indeed by soil layers below the simulated soil profile and the water table.
   inline virtual nat8  get_layer_at_depth
      (float64 a_given_depth_m
      /*, bool limit_to_MAX_trans_layers=false*/)                          const { assert(false); return 0; /* not currently needed in this model */ }
      // Returns the index of the layer at the specified depth.

      // Note  need to get ride of   bool limit_to_MAX_trans_layers=false
      // Instead, caller should take the min of the layer at depth
      // or layer at max_transformation_depth

   inline virtual nat8  get_layer_at_depth_or_last_layer
      (float64 a_given_depth_m
      /*, bool limit_to_MAX_trans_layers=false*/)                          const { assert(false); return 0; /* not currently needed in this model */}
      // Returns the index of the layer at the specified depth.
      // if  a_given_depth_m is 0.0 (unspecified) the last layer is returned


//NYN  virtual nat8  get_bottom_layer()                                       const = 0;
//      // Returns the index of the layer at the specified depth.

   virtual nat8 get_horizon_at_layer(nat8 layer)                         const { assert(false); return 0; /* not currently needed in this model */ }
      // This returns the layer (horizon) in which the specified layer/sublayer occurs.
 public: // Soil profile parameter accessors (layer number is 1 based)
   inline virtual float64 get_thickness_mm                      (nat8 layer)     const { return get_thickness_m(layer) * 1000.0; }
   inline virtual float64 get_thickness_cm                      (nat8 layer)     const { return get_thickness_m(layer) *  100.0; }
   virtual float64 get_thickness_m                       (nat8 layer)     const;
      // These return the thickness of the specified layer
   inline virtual float64 get_depth_cm                          (nat8 layer)     const { return get_depth_m(layer) * 100.0; }
   virtual float64 get_depth_m                           (nat8 layer)     const;
      // These return the depths to the bottom of the specified layer.
   virtual float64 get_depth_profile_m                  ()                 const { assert(false); return 0; /* not currently needed in this model */ }
      // This returns the total depth of the soil profile in centimeters.
   virtual float64 get_depth_profile_cm                  ()                const { assert(false); return 0; /* not currently needed in this model */ }
      // This returns the total depth of the soil profile in centimeters.
   virtual nat8   closest_layer_at(float64 a_given_depth)                 const { assert(false); return 0; /* not currently needed in this model */ }
      // Returns the closest layer at the specified depth.
   virtual float64 get_layer_center_depth(nat8 sublayer)                  const { assert(false); return 0; /* not currently needed in this model */ }
   public: // set accessors
    virtual nat8 set_number_uninundated_layers(nat8 new_uninundated_layers)modification_ { assert(false); return 0; /* not currently needed in this model */ }
//080211 virtual nat8 set_number_effective_layers(nat8 new_effective_layers)modification_ = 0;
   public: // Utilities for working with layer data
    virtual void copy_array64(soil_layer_array64(target),const soil_layer_array64(source)) const { assert(false);  /* not currently needed in this model */ }
    virtual void copy_array32(soil_layer_array32(target),const soil_layer_array32(source)) const { assert(false); /* not currently needed in this model */ }
    inline Soil_layers()
       : depths_provided(false)
       {}
   };
   #endif
   //___________________________________________________________________________


   class Soil_hydrology
   : public implements_ Soil_hydrology_interface
   {  // Refer to the file soil/hydrology_interface.h for documentations on these methods
   public: //friend class Crop_with_soil_weather;
   //      const Soil_hydraulic_properties_interface &hydraulic_properties;
   // WARNING these are currently implemented as stubs for the testing.
      //soil_layer_array64(water_content);  // volumetric
   protected:
      const Soil_layers_interface &soil_layers;
   public: // Soil profile state accessors

      virtual float64 get_water_plus_ice_content_gravimetric (nat8 layer) const;
      virtual float64 get_liquid_water_content_volumetric    (nat8 layer) const;
      virtual float64 get_water_plus_ice_content_volumetric_at
         (nat8 layer,CORN::Hour hour)                                     const;
      virtual float64 get_water_plus_ice_content_volumetric  (nat8 layer) const;
      inline virtual float64 get_water_filled_porosity
         (nat8 layer, CORN::Hour for_hour)                                const  { return 0.0; /* this model does not use this method */ }
      inline virtual float64 get_water_potential             (nat8 layer) const;
//130930       inline virtual float64 get_water_depth_profile(bool liquid_and_ice) const  { return 0.0; /* this model does not use this method */ }
      inline virtual float64 provide_water_depth_profile(bool all_water)  provision_ {return 0.0; } //130930
      inline virtual float64 get_plant_avail_water(nat8 layer)            const  { return 0.0; /* this model does not use this method */ }
      inline virtual float64 calc_available_water
         (nat8 layer , bool real_wilt_based)                              const  { return 0.0; /* this model does not use this method */ }
      inline virtual float64 get_K_Theta
         (nat8 layer
         ,float64 Theta, float64 Theta_s)                                 const  { return 0.0; /* this model does not use this method */ }
   virtual float64 *get_liquid_water_content_layers_volumetric                   //140527
      (soil_layer_array64(water_content_layers))                          const  { return 0; /* this model does not use this method */ } //140610_990321
   virtual float64 get_reference_water_content(nat8  sublayer)            const  { return 0; /* this model does not use this method */ } //140610_990321
   virtual float64 get_air_entry_pot
      (nat8 layer,bool consider_ice_content,bool FC_PWP_based)            const  { return 0.0; /* this model does not use this method */ } //140610_990321
   virtual float64 get_real_wilt_point(nat8 layer)                        const  { return 0.0; /* this model does not use this method */ } //140610
   virtual float64* ref_plant_avail_water()                                      { return 0; /* this model does not use this method */ } //140610
   virtual float64 calc_water_potential                                          //140610
      ( nat8 layer,bool consider_ice,bool FC_PWP_based)                    const { return 0.0; /* this model does not use this method */ } //140610

   public: // soil freezing accessors
      // Derived classes may return 0 in the case of no soil freezing simulation.
      inline virtual float64 get_ice_content_volumetric(nat8 layer)        const { return 0.0; /* this model does not use this method */ }
      inline virtual float64 get_ice_content_volumetric_yesterday(nat8 layer) const  { return 0.0; /* this model does not use this method */ }
      inline virtual bool is_frozen(nat8 layer)                            const { return false; /* this model does not use this method */ }
   public: // set state accessors
      inline virtual float64 set_water_content_volumetric_layer
         (nat8 layer, float64 new_water_content)                  modification_
         {  return 0; }  // This model does not maintain the water content.
      virtual float64 change_liquid_water_content_by_volumetric
         (nat8 layer, float64 change_WC)                          modification_ { return 0.0; /* this model does not use this method */ }
         // Pass a positive value to increase the WC and negative to decrease.
         // Returns the new water content.
         // Note that change_WC is probably always liquid so this will work
         // for either liquid or total water basis.
   inline virtual float64 set_plant_avail_water       (nat8 layer, float64 plant_avail_water)      modification_ { return 0.0; /* this model does not use this method */ } //140610
   inline virtual float64 set_real_wilt_point         (nat8 layer, float64 real_wilt_point)        modification_ { return 0.0; /* this model does not use this method */ } //140610
   inline virtual float64 set_reference_water_content (nat8 layer, float64 reference_water_content)modification_ { return 0.0; /* this model does not use this method */ } //140610

   public: // processes
      virtual float64 extract_water_mm(float64 water_uptake_mm[] , nat8 start_layer) { return 0.0; /* this model does not use this method */ } //140624
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
      float64 refill_water_depth_m(bool assume_irrigation,nat8 observation_layers) modification_ { return 0.0; /* this model does not use this method */ }
   public:  // process daily
      Soil_hydrology
         (const Soil_layers_interface &_soil_layers)
         : soil_layers(_soil_layers)
         {}
      virtual bool start_day()                                    modification_ {  /* this model does not use this method */ return true; }
      inline virtual void update_liquid_and_ice_water_content()   modification_ {  /* this model does not use this method */ }
      float64  refill_water_depth_m(bool and_refill,nat8 observation_layers)                       const;
         ///\returns the depth of water that would refill the soil to field capacity.
         /// If and_refill if enabled, the WC will also be set to FC
         /// (as an autoirrigation).
      #ifdef UNIT_TEST_SOIL
      void dump_water_content(std::ofstream &dump_stream)                  const;
      #endif
    #ifdef USE_VARIABLE_RECORDERS
    public:
      virtual bool render_variable_recorders
         (CS::Variable_recorders &recorders
         ,nat32 desired_descriptive_summary_elements)                /*rendition_*/{ return false; /* this model does not use this method */ } //130924
    #endif
    private:
      Soil_hydrology *debug_find_abstract_error() const
         {  Soil_layers_interface *soil_layers;
            return new Soil_hydrology(*soil_layers);}
         // This method is not used other to find if Soil_hydrology class is abstract.

   virtual void record_water_content_for_hour(CORN::Hour hour)                   {  /* this model does not use this method */ } //140527
//      { /* this is just for output do nothing by defaut */ }
   virtual Soil_hydrology_interface *copy(float32 fraction)                const {  return 0; /* this model does not use this method */ } //140610
   virtual void expand_last_layer_for_boundary_conditions
      ( nat8 number_layers, nat8 number_expanded_layers)           modification_ {  /* this model does not use this method */ } //140610

   };
   //___________________________________________________________________________
   class Soil
      #if (VIC_CROPSYST_VERSION >= 3)
      : public Soil_interface
      #endif
   {
    public:
      #ifndef CROPSYST_PROVIDED_SOIL_HYDROLOGY
      //120612 currently VIC is providing soil hydrology
      const Soil_layers                layers;                                 //120619
      Soil_hydrology                  *hydrology;
      #endif
      Soil();
   virtual unmodifiable_ Soil_structure_interface *get_structure_unmodifiable()  const { assert(false); return 0; } // Not currently needed
   virtual   modifiable_ Soil_structure_interface *get_structure_modifiable()    const { assert(false); return 0; } // Not currently needed
      // Returns the soil structure (bulk density)
   virtual unmodifiable_ Soil_hydrology_interface *get_hydrology_constant()      const { return hydrology; };
      // Returns the current soil hydrology state and process object.
      // Generally this is required in derived classes.
      // Generally derived classes will return the object.
   virtual   modifiable_ Soil_hydrology_interface *get_hydrology_modifiable() modification_ { return hydrology; }
   virtual float64 get_temperature_C                     (nat8 layer)      const { assert(false); return 0; } // Not currently needed
   virtual float64 get_act_water_entering_soil()                           const { assert(false); return 0; } // Not currently needed
   virtual float64 get_act_evaporation()                                   const { assert(false); return 0; } // Not currently needed
   virtual float64 get_pot_evaporation()                                   const { assert(false); return 0; } // Not currently needed
   virtual bool start_day()                                         modification_{ assert(false); return true; } // Not currently needed
   virtual bool process()                                           modification_{ assert(false); return 0; } // Not currently needed
   virtual Seconds   get_hydrology_timestep_resolution_today()             const { assert(false); return 0; } // Not currently needed
   virtual unmodifiable_ Soil_layers_interface *get_layers()               const { /*LML 141104 assert(false); return 0*/return &layers; } // Not currently needed
   virtual   modifiable_ Soil_layers_interface *get_layers_modifiable() modification_ { assert(false); return 0; } // Not currently needed
      // Returns the soil layering system.
      // This is required in derived classes.
      // It should never return 0.
   virtual unmodifiable_ Soil_texture_interface  *get_texture()            const { assert(false); return 0; } // Not currently needed
   virtual   modifiable_ Soil_texture_interface *get_texture_modifiable() modification_ { assert(false); return 0; } // Not currently needed
      // Returns the soil texture parameters object.
   virtual unmodifiable_ Soil_hydraulic_properties_interface *get_hydraulic_properties_constant() const { assert(false); return 0; } // Not currently needed
      // Returns the soil properties that are commonly derived from soil survey data
      // and/or pedo transfer functions.
   virtual   modifiable_ Soil_hydraulic_properties_interface *get_hydraulic_properties_modifiable() modification_ { assert(false); return 0; } // Not currently needed
      // Returns the soil properties that are commonly derived from soil survey data
      // and/or pedo transfer functions.

   };
   //___________________________________________________________________________
   #ifndef CROPSYST_PROVIDED_ET
   // now providing our own calculation
   class ET_Reference_from_VIC
   : public ET_reference_interface
   {
   protected: friend class Crop_with_soil_weather;
      friend class Crop_with_weather;
      float64 potential_reference_ET_mm;  // millimeter
      virtual float64 get_m/*LML 150318 reference_ET_m*/(ET_Model   preferred_ET_model)    const { return mm_to_m(potential_reference_ET_mm); }
      virtual float64 get_mm/*LML 150318 reference_ET_mm*/(ET_Model  preferred_ET_model)    const { return potential_reference_ET_mm; }
      virtual float64 get_vapor_pressure_plus_resistance
         (float64 bulk_resistance)                                         const { return 0.0; } // Not currently used in this model
   public:
      inline ET_Reference_from_VIC()
         : potential_reference_ET_mm(0)
         {}
   };
   private:
      ET_Reference_from_VIC   ET_reference;
   #endif
   //___________________________________________________________________________
 protected:
   CORN::OS::Directory_name_concrete database_directory_name;                     //110202
   CORN::OS::File_name_concrete     *crop_parameters_filename;                    //110202

   Soil                             soil;                                        //140812
      // Number of consecutive days where the leaf water potential
      // is more negative than the LWP that begins to reduce canopy expansion.
      // This is use to trigger need for irrigation.
 public: // currently public because crop used to be a superclass
   //140810 #ifdef CROP_PARAMETERS_HARDCODED
   //140810    #else
   Crop_parameters crop_parameters;
   //140810    #endif
   Crop_interface *crop;                                                         //140509
   bool database_valid;                                                          //110617 crop_and_soil_valid;
   bool automatic_harvest;
   bool enable_CO2_response;
 protected:
   bool crop_started;                                                            //110207
 protected:
   bool found_inducible_period;                                                  //110207
   CORN::Statistical_data_set window_avg_temperatures;                           //110207
 public: // structors
   Crop_with_weather
      (const char *database_directory_name
      ,float64 latitude_dec_deg,float64 longitude_dec_deg,float64 elevation_meter
      ,bool automatic_harvest
      ,bool enable_CO2_response
      ,bool assume_irrigation);
      // This constructor is used to load hard coded parameters.
      // Becoming obsolete but currently used for testing and debugging.         //110202

      // The database_directory can be nil or ""
      // In this case the soil must be setup with know_soil_layer()
      // and the constructor must specify either the WSDA_crop_type_code
      // or a fully qualified crop_file_name.
      // When database_directory is specified the crop_file_name does not need
      // to be fully qualified the path and filename extension will be affixed.
   ~Crop_with_weather();
 public:   // Soil accessors
   virtual bool set_database_directory(const char *directory_name);              //110128
   // void know_soil_layer_thickness(float64 layer1_m,float64 layer2_m,float64 layer3_m);
      // VIC layers are indexed from 0, we pick them up here from 1

   //140810 now perm #ifndef NONCONTINUOUS
   Crop_interface *start_crop(nat16 WSDA_crop_type_code)          modification_; //140509_110614
   bool  stop_crop()                                              modification_; //110615
   //140810 now perm    #endif
/*
   float64 know_soil_water_content
         (nat8 horizon_number
         ,float64 water_content_volumetric)                       modification_;
      // Returns any drain (depth in mm)
   float64 know_water_entering_soil_mm
         (float64 water_entering_soil_mm)                         modification_; //101101
*/
   void know_air_temperature                                                     //101122
         (float64 air_temperature_max
         ,float64 air_temperature_min)                            modification_;
   nat16 adjust_phenology
         (nat16 air_temperature_data_count
         ,float64 air_temperature_max[]
         ,float64 air_temperature_min[])                          modification_;

   void know_solar_radiation
         (double daily_irradiance_MJ_m2)                          modification_; //110413
   /*140815 RLN doesn't appear to be used anymore in any version
   void know_relative_humdity
         (double RelHumid_max, double RelHumid_min)               modification_; //110413
   */

   void know_wind_speed_m_s(float64 wind_speed_m_s)               modification_; //101122
   #ifdef CROPSYST_PROVIDED_ET
   #else
   void know_weather
         (float64 air_temperature_max
         ,float64 air_temperature_min
         ,float64 solar_radiation
         ,float64 vapor_pressure_deficit_max)                     modification_;
   virtual void know_pychrometrics                                               //140508
      (float64 vapor_pressure_saturation_slope_kPa_C
      ,float64 psychrometric_constant_kPa_C
      ,float64 aerodynamic_resistance_ref_plants)                  modification_;

   inline void know_potential_reference_evapotranspiration_mm
      (float64 ET_reference_mm)                                    modification_
      {         ET_reference.potential_reference_ET_mm = ET_reference_mm;}
   inline bool know_transpiration_max_mm(double transpiration_max_mm)     modification_
      { return crop && crop->know_transpiration_max_mm(transpiration_max_mm); }
   #endif
   void know_atmospheric_CO2_concentration
         (float64 new_atmospheric_CO2_concentration_ppm)          modification_;
/*
   void know_geolocation
         (latitude_dec_deg,longitude_dec_deg,elevation_meter)   modification_;
*/
   void know_snow_pack_thickness(float64 snow_pack_thickness_m)   modification_; //141207
      // Snow pack thickness (meter)
      // It is used to apply stem snow insulation factor, is not critical for the crop model.
 public: // processes
   float64 harvest(bool terminate_crop)                           modification_;
 public:
   virtual float64 get(nat32 variable_code/*NYI,nat32 units_code*/)       const;
   /*140810 obsolete
      // VIC is now handling water uptake so if it still needs
      // to have uptake, then it needs to keep track.
      // CropSyst proper does not keep the uptake by layer.
   virtual float64 get_uptake_mm(nat8 layer);
   */
   virtual bool start_day(int32 date)                             modification_;
   virtual bool end_day()                                         modification_;
   inline virtual bool is_perennial()                                     const { return crop && crop->is_perennial(); }
   inline virtual float64 update_ET_coef_full_shading()           modification_ { return crop ?  crop->update_ET_coef_full_shading() : 0 ; } //121129
#ifdef TEST_C_CPP
   virtual Normal_crop_growth_sequence process()                  modification_ { return false;} // Not implemented for test
#else
   virtual Normal_crop_event_sequence process()                   modification_; //140314
#endif
 private:
   bool assume_irrigation;
   float64 irrigation_threshold_LWP;
   nat16 at_irrigation_threshold_day_count;
         /**< Computed by refill_water_depth when irrigation_conditions are met.
              This is the depth of water that would refill soil to FC
         */
 protected: // methods specific to this model.
   bool find_crop_file
      (const std::string &crop_name
      ,const std::wstring &crop_ext)                               modification_;
      //140508 ()                                                 modification_;
   bool adjust_crop_phenology(float64 latitude,float64 longitude);modification_;
   bool adjust_for_county(nat16 WSDA_crop_type_code,Crop_parameters &crop_to_adjust) modification_; //110702
};
//______________________________________________________________________________
} // namespace VIC_crop
#endif

