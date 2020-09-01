#include "surface_infiltration_green_ampt.h"
#include "common/physics/standard_constants.h"

// Warning I think this is m/hour
namespace Soil {                                                                 //181206
//_____________________________________________________________________________/
Surface_infiltration_Green_Ampt::Surface_infiltration_Green_Ampt
(const Soil::Layers_interface                 &layers_                           //170428
,const Soil::Hydraulic_properties_interface   &soil_hydraulic_properties_        //140424
,const Soil::Hydrology_interface              &hydrology_                        //140820
      //200316unused ,CropSyst::Soil_runoff                       *runoff_                            //131101
,CropSyst::Pond                              &pond_)
: first_hour_of_watering                                                  (true)
, wetting_front_depth                                                      (0.0)
, wetting_front_layer                                                        (0) //170426
, WP_m_wetting_front_avg                                                   (0.0) //130328
, water_potential_top_m                                                    (0.0)
, layers                                                               (layers_) //170428
, soil_hydraulic_properties                         (soil_hydraulic_properties_) //140424
, hydrology                                                         (hydrology_) //140820
, Saturated_Hydraulic_Conductivity_For_Infiltration                        (0.0) //160430
{  clear_layer_array64(initial_water_potential_m);                               //140814
}
//_Surface_infiltration_Green_Ampt:constructor_________________________________/
float64 Surface_infiltration_Green_Ampt::update
(Hour hour
,float64 water_input_hourly /* pass by value only local copy updated */ //    (ByVal Hourly_Irrigation As Single
,float64  water_content_layer_1
,float64 &runoff_amount
,nat8     number_layers)                                                         //140820
{
   /* NEW 03/16/13
   Determine darcian infiltration rate given current water front depth.
   Water potential of the weting front is the average of water potentials (in m)
   immediately above and below the water front (1 J/kg = 0.102 m).
   The saturated hydraulic conductivity is the minimum above the wetting front.
   */
   // Green and Ampt infiltration calculation
   float64 water_depth_to_infiltrate = 0.0;
   float64 surface_infiltration_capacity = 0.0;

   if (water_input_hourly > 0)
   {  // Note in C++ version standard hours are indexed from 0 not 1

      if (first_hour_of_watering)
      {
         for (nat8 layer = 1; (layer <= number_layers) ; layer++)
            initial_water_potential_m[layer] = J_per_kg_to_m
               (hydrology.calc_water_potential(layer,false,false));
               //was  (Air_Entry_Potential(Layer) * (Water_Content(Layer) / WC_Sat(Layer)) ^ (-b_Value(Layer)));
         Saturated_Hydraulic_Conductivity_For_Infiltration                       //170426COS
            = soil_hydraulic_properties.get_sat_hydraul_cond_m_hr(1);
         float64 air_entry_pot_1 =// soil_hydraulic_properties.get_air_entry_potential(1,false);
               soil_hydraulic_properties.get_air_entry_potential(1,false);
                 //warning, I dont know if to specifically calculate or use normals
         float64 water_potential_above_wetting_front_m = J_per_kg_to_m(air_entry_pot_1);
                        //'Convert J/kg to m
         float64 Water_Potential_Below_Wetting_Front_m = initial_water_potential_m[1]; //140814COS
         water_potential_top_m = water_potential_above_wetting_front_m;
         WP_m_wetting_front_avg =
            (water_potential_above_wetting_front_m + Water_Potential_Below_Wetting_Front_m) / 2.0;
         first_hour_of_watering = false;
      } // if first_hour_of_watering
      if (CORN::is_zero<float64>(wetting_front_depth,0.000000001))
      {
         wetting_front_depth = layers.get_depth_m(1) / 2.0;  //was 0.001         //170427COS
         wetting_front_layer = 1;                                                //170426COS
      }
      surface_infiltration_capacity =  // Infiltration_Rate in VB code
         Saturated_Hydraulic_Conductivity_For_Infiltration                       //170426COS
          * (water_potential_top_m - (WP_m_wetting_front_avg - wetting_front_depth))
          / wetting_front_depth ;
      surface_infiltration_capacity = CORN::must_be_0_or_greater<float64>
         (surface_infiltration_capacity);                                        //141204LML
   } else
   {  surface_infiltration_capacity = 0.0; //  'in m, actually m/h
      first_hour_of_watering = true;
      // Reset the boolean variable to be ready for the next watering event
   }
   // Determine the actual amount of water to infiltrate, used for saturation cascading
   water_depth_to_infiltrate   // in m, actually m/h
      = std::min<float64>(surface_infiltration_capacity, water_input_hourly);
   // Determine if there is any runoff
   if (water_depth_to_infiltrate < water_input_hourly)
      runoff_amount = water_input_hourly - water_depth_to_infiltrate;
   return water_depth_to_infiltrate;
}
//_update___________________________________________________________2013-05-03_/
bool Surface_infiltration_Green_Ampt
::update_wetting_front
(nat8 layer
,bool is_bottom_layer
,float64 wetting_front_depth_
,float64 water_content_below)
{
   wetting_front_depth = wetting_front_depth_;
   wetting_front_layer = layer;                                                  //170428
   float64 air_entry_pot_layer
      = soil_hydraulic_properties.get_air_entry_potential(layer,false);
   float64 water_potential_above_wetting_front_m = J_per_kg_to_m(air_entry_pot_layer);//130709
   float64 WPl_below_wetting_front_m = 0;
   if (is_bottom_layer)
      WPl_below_wetting_front_m = water_potential_above_wetting_front_m;
   else
   {
      nat8 layer_below = layer + 1;                                              //170427
      float64 air_entry_pot_m_layer_below = J_per_kg_to_m                        //170427
         (soil_hydraulic_properties.get_air_entry_potential(layer_below,false)); //170427
      float64 WC_sat_ice_adjusted_below                                          //170427
         = soil_hydraulic_properties.get_saturation_water_content_volumetric     //170427
            (layer_below,hydrology.get_ice_content_volumetric(layer_below));     //170427
      float64 b_value_below                                                      //170427
         = soil_hydraulic_properties.get_Campbell_b(layer_below,false);          //170427
      WPl_below_wetting_front_m                                                  //170427
         = air_entry_pot_m_layer_below;                                          //170428
   }
   WP_m_wetting_front_avg
      = (water_potential_above_wetting_front_m
       + WPl_below_wetting_front_m) / 2.0;
   return true;
}
//_Surface_infiltration_Green_Ampt__________________________________2014-08-20_/
bool Surface_infiltration_Green_Ampt::clear_wetting_front()
{  wetting_front_depth = 0;
   wetting_front_layer = 0;                                                      //170426
   return true;
}
//_clear_wetting_front______________________________________________2014-08-20_/
}//_namespace_Soil_____________________________________________________________/
