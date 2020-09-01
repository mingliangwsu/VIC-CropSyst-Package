#ifndef infiltration_cascade_hourlyH
#define infiltration_cascade_hourlyH
#include "corn/chronometry/time_types.hpp"                                       //LML150219

#include "soil/infiltration_cascade_common.h"
#include "soil/layers_I.h"                                                       //LML150219
#include "soil/texture_I.h"                                                      //LML150219
#include "corn/dynamic_array/dynamic_array_T.h"                                  //LML150219

namespace Soil {                                                                 //181206
class Surface_infiltration_Green_Ampt;
//______________________________________________________________________________
class Infiltration_cascade_hourly
: public  Infiltration_cascade_common
{
      // hydrology recorded hourly (index 0 is for hour 0 (midnight)).
      // the values are for the end of the timestep (hour).
      // These are dynamically allocated because constructor has arguments
   const Texture_interface &soil_texture;                                        //070316
 public:
   Surface_infiltration_Green_Ampt *surface_infilt_Green_Ampt;                   //140501 was Green_Ampt_infiltration_for_numerical_runoff;   //130503
   float64 microirrigation_excess;                                               //200309
   bool using_numerical_runoff_model;                                            //200316
 public: // Constructor
   Infiltration_cascade_hourly
      (const Layers_interface     &soil_layers_                                  //140424
      ,const Texture_interface    &soil_texture_                                 //140424
      ,const Hydraulic_properties_interface &soil_hydraulic_properties_          //140423
      ,Hydrology_interface        &hydrology_
      ,CropSyst::Soil_runoff      *runoff_   // Optional                             //130308
      ,bool                        using_numerical_runoff_model_                 //200316
      ,CropSyst::Pond             &pond_);                                           //130308
   #if ((DIRECTED_IRRIGATION>=2014) && (DIRECTED_IRRIGATION<=2016))
   #error obsolete
   Infiltration_cascade_hourly                                                   //140609
      (const Infiltration_cascade_hourly &from_copy);
   virtual Infiltration_nominal *copy(float32 fraction)                   const; //140609
   #endif
   virtual bool initialize()                                    initialization_;
 public:
   virtual bool infiltrate
      (float64                       water_entering_soil_not_irrigation
      ,float64                       management_irrigation
      ,float64                       management_auto_irrigation_concentration
      ,Dynamic_water_entering_soil *non_runoff_water_entering_in_intervals_mm    //990204
      ,Dynamic_water_entering_soil *irrigation_water_entering_in_intervals_mm    //070119 optional (may be 0)
      ,const CORN::Dynamic_array<float32>   &runon_in_intervals
      ,CORN::Dynamic_array<float32>         &runoff_in_intervals) modification_;
      // Returns the amount of infiltration (m of water depth?)
   inline virtual float64 get_microirrigation_excess()                     const
                                                {return microirrigation_excess;} //200309

 private: // These variables can carry over multiple days.
   soil_layer_array64(hours_to_field_capacity);                                  //080609
 private: // working variables conveniently calculated once so we don't have to pass around.
    Layer number_layers;          // Simply the number of soil layers, moved here so not have to pass to process_infiltration
 public:
   void process_hour                                                             //140423
      (CORN::Hour hour                                                           //130325
      ,float64 natural_water_input_m                                             //200226
      ,float64 irrigation_m                                                      //200226
      //200226 ,float64    water_input_hourly
      ,float64   &surface_out_flow )                              modification_; //140319
         // surface_out_flow returns meter  (in the hour)                        //130127
         // surface_out_flow is essentially contribution to runoff and/or ponding
 private:
   float64 calc_hours_to_field_capacity                                          //120716
      (float64 potential_at_field_capacity)                               const;
   float64 calc_max_water_content_during_infiltration
      ( float64 flow_in
      , float64 KSat
      , float64 air_entry_potential
      , float64 bValue
      , float64 WCSat
      , CORN::Seconds time_step)                                          const;
   float64 CalculateInfiltrationCapacity
      (float64 WaterDepthToInfiltrate
      ,float64 Saturated_Hydraulic_Conductivity
      ,float64 Air_Entry_Pot
      ,float64 WC
      ,float64 WCsat
      ,float64 Bvalue
      ,float64 LayerThickness);
   float64 distribute_excess_flow_upwards
      ( nat8 from_layer
      , float64 &excess_flow_in
      , soil_layer_array64(water_content))                        modification_;
};
//_Infiltration_cascade_hourly______________________________________2007-01-05_/
}//_namespace_Soil_____________________________________________________________/
#endif
