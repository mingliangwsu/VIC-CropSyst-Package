#ifndef VIC_crop_weather_soilH
#define VIC_crop_weather_soilH
#include "VIC_crop_weather.h"
#include "soil/soil_param_V5.h"
#include <assert.h>
#ifndef OLD_SUBLAYERING
#  include "soil/soil_properties_sublayers.h"
#endif
//______________________________________________________________________________
/** \class This version of the Crop class maintains the soil and weather
*/
using namespace CropSyst;
namespace VIC_crop
{

class Crop_with_weather_soil
: public extends_ Crop_with_weather
{
   //___________________________________________________________________________
//120619 not sure why I had marked this as old   
//   #ifdef OLD_SUBLAYERING
   //nat8 number_VIC_horizons;
   float64 VIC_horizon_thickness[10]; // Indexed from 1 Actually VIC currently only has 3 layers
   nat8 VIC_horizon_at_layer[42];
//   #endif
   //___________________________________________________________________________

   class Soil_hydrology
   : public implements_ Soil_hydrology_interface
   {  // Refer to the file soil/hydrology_interface.h for documentations on these methods
   public: //friend class Crop_with_weather_soil;
      const Soil_hydraulic_properties_interface &hydraulic_properties;
   // WARNING these are currently implemented as stubs for the testing.
      soil_layer_array64(water_content);  // volumetric
   protected:
      const Soil_layers_interface &soil_layers;
   public: // Soil profile state accessors
      virtual float64 get_water_plus_ice_content_gravimetric (nat8 layer) const  { return water_content[layer]; /* this model does not use this method */ }
      virtual float64 get_liquid_water_content_volumetric    (nat8 layer) const  { return water_content[layer]; /* this model does not use this method */ }
      virtual float64 get_water_plus_ice_content_volumetric_at
         (nat8  layer,CORN::Hour hour)                                    const  { return water_content[layer]; /* this model does not use this method */ }
      virtual float64 get_water_plus_ice_content_volumetric  (nat8  layer)const  { return water_content[layer]; /* this model does not use this method */ }
      virtual float64 get_water_filled_porosity
         (nat8  layer, CORN::Hour for_hour)                               const  { return 0.0; /* this model does not use this method */ }
      virtual float64 get_water_potential                    (nat8 layer) const;
      virtual float64 get_water_depth_profile(bool liquid_and_ice)        const  { return 0.0; /* this model does not use this method */ }
      virtual float64 get_plant_avail_water(nat8 layer)                   const  { return 0.0; /* this model does not use this method */ }
      virtual float64 calc_available_water
         (nat8 layer , bool real_wilt_based)                              const  { return 0.0; /* this model does not use this method */ }
   public: // soil freezing accessors
      // Derived classes may return 0 in the case of no soil freezing simulation.
      virtual float64 get_ice_content_volumetric(nat8 layer)              const  { return 0.0; /* this model does not use this method */ }
      virtual float64 get_ice_content_volumetric)yesterday(nat8 layer)    const  { return 0.0; /* this model does not use this method */ }
      virtual bool is_frozen(nat8 layer)                                  const  { return false; /* this model does not use this method */ }
   public: // set state accessors

      virtual float64 set_water_content_volumetric_layer
         (nat8  layer, float64 new_water_content)                  modification_
         {  return water_content[layer] = new_water_content; /* this model does not use this method */
         }
      virtual float64 change_liquid_water_content_by_volumetric
         (nat8  layer, float64 change_WC)                          modification_ { return 0.0; /* this model does not use this method */ }
         // Pass a positive value to increase the WC and negative to decrease.
         // Returns the new water content.
         // Note that change_WC is probably always liquid so this will work
         // for either liquid or total water basis.
   public: // processes
      virtual float64 extract_water(float64 water_uptake_m[],nat8 start_layer);
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
      float64 refill_water_depth_m(bool assume_irrigation,nat8 observation_layers) modification_;
   public:  // process daily
      Soil_hydrology
         (const Soil_layers_interface                 &_soil_layers
         ,const Soil_hydraulic_properties_interface   &_hydraulic_properties);
      virtual bool start_day()                                    modification_ {  /* this model does not use this method */ }
      inline virtual void update_liquid_and_ice_water_content()   modification_ {  /* this model does not use this method */ }
      float64  refill_water_depth_m(bool and_refill,nat8 observation_layers) const;
         ///\returns the depth of water that would refill the soil to field capacity.
         /// If and_refill if enabled, the WC will also be set to FC
         /// (as an autoirrigation).
      #ifdef UNIT_TEST_SOIL
      void dump_water_content(std::ofstream &dump_stream)                  const;
      #endif
   };
   //___________________________________________________________________________
private:
   CORN::OS::File_name_concrete soil_parameters_filename;                         //110202
   Smart_soil_parameters      soil_parameters;                                   //110202
   Soil_properties_sublayers  *soil_properties;                                  //110202
   float64                    soil_pot_evaporation_mm;                           //110527
   float64                    soil_act_evaporation_mm;                           //110527
 private:
   Crop_with_weather_soil::Soil_hydrology             *soil_hydrology;
public: // structors
   Crop_with_weather_soil
      (const char *database_directory_name
//NYI      ,const char *crop_file_name
      #ifdef NONCONTINUOUS
      ,nat16 WSDA_crop_type_code
      #endif
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
public:   // Soil accessors
   virtual bool set_database_directory(const char *directory_name);              //110128
   void know_soil_layer_thickness(float64 layer1_m,float64 layer2_m,float64 layer3_m);
      // VIC layers are indexed from 0, we pick them up here from 1

   #ifndef NONCONTINUOUS
   Crop_common *start_crop(nat16 WSDA_crop_type_code)             modification_; //110614
   #endif

   float64 know_soil_water_content
         (nat8 horizon_number
         ,float64 water_content_volumetric)                       modification_;
      // Returns any drain (depth in mm)
   float64 know_water_entering_soil_mm
         (float64 water_entering_soil_mm)                         modification_; //101101
public:
   virtual float64 get(nat32 variable_code/*NYI,nat32 units_code*/)       const;
   virtual float64 get_uptake_mm(nat8 layer);
   virtual bool start_day(int32 date)                             modification_;
   virtual Normal_crop_event_sequence process()                   modification_;
private:
   bool assume_irrigation;
   float64 irrigation_threshold_LWP;
   nat16 at_irrigation_threshold_day_count;
   float64  refill_depth_m; // m
         /**< Computed by refill_water_depth when irrigation_conditions are met.
              This is the depth of water that would refill soil to FC
         */
   float64 evaporate
      (float64 Total_Canopy_Solar_Radiation_Interception_Fraction
      ,float64 mulch_cover_fraction=0.0)                          modification_;

      //(bool fallow=false,bool summer_time=false,float64 mulch_cover_fraction=0.0);
protected: // methods specific to this model.
   inline float64 get_refill_water_depth_m()                               const { return refill_depth_m; }
   bool find_soil_file()                                          modification_;
   bool adjust_crop_phenology(float64 latitude,float64 longitude);modification_;
public:
   void daily_soil_header(std::ofstream &daily_soil_file,int line);
   void daily_soil_output(std::ofstream &daily_soil_file);
};
//______________________________________________________________________________
} // namespace VIC_crop
#endif

