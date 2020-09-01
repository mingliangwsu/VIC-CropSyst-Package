#ifndef surface_infiltration_green_amptH
#define surface_infiltration_green_amptH
#include "corn/primitive.h"                                                      //150929RLN_150219LML
#include "soil/layering.h"                                                       //150219LML
#include "soil/runoff.h"                                                         //150219LML
#include "pond.h"                                                                //150219LML
//_____________________________________________________________________________/
namespace Soil
{
class Hydraulic_properties_interface;                                            //150219LML
class Hydrology_interface;                                                       //150219LML
//_____________________________________________________________________________/
class Surface_infiltration_Green_Ampt
{
 public:
   bool    first_hour_of_watering;
 public:
   float64 wetting_front_depth;
   nat8    wetting_front_layer;                                                  //170426
 protected:
   float64 Saturated_Hydraulic_Conductivity_For_Infiltration;
   float64 WP_m_wetting_front_avg;                                               //130325
   float64 water_potential_top_m ;                                               //130325
   soil_layer_array64(initial_water_potential_m);                                //140819
      // Not sure if initial_water_potential_m is reset every day
 protected:
   const Layers_interface               &layers;                                 //170428
   const Hydraulic_properties_interface &soil_hydraulic_properties;              //071012
   const Hydrology_interface            &hydrology;                              //140820
 public:
   Surface_infiltration_Green_Ampt
      (const Soil::Layers_interface                 &layers_                     //170428
      ,const Soil::Hydraulic_properties_interface   &soil_hydraulic_properties_  //140424
      ,const Soil::Hydrology_interface              &hydrology_                  //140820_060512
      //200316unused ,CropSyst::Soil_runoff                       *runoff_                      //131101
      ,CropSyst::Pond                              &pond_);
   float64 update
      (Hour hour                                                                 //140814
      ,float64  water_input_hourly
      ,float64  water_content_layer_1
      ,float64 &runoff_amount
      ,nat8     number_layers  );                                                //140820
   bool update_wetting_front
      (nat8 layer
      ,bool is_bottom_layer
      ,float64 _wetting_front_depth
      ,float64 water_content_below);
   bool clear_wetting_front();
};
//_Surface_infiltration_Green_Ampt__________________________________2013-05-03_/
}//_namespace_Soil_____________________________________________________________/
#endif

