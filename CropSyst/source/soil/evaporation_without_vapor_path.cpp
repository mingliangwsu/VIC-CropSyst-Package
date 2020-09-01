#include "evaporation_without_vapor_path.h"
#include "soil/soil_base.h"
#include <math.h>
#ifndef moremathH
#  include "corn/math/moremath.h"
   using namespace std;
#endif
#ifndef compareHPP
#  include "corn/math/compare.hpp"
#endif
#include "common/physics/standard_constants.h"
#include "soil/hydrology_finite_diff.h"
#include "common/weather/hour/weather_hours.h"
namespace Soil {
//______________________________________________________________________________
Evaporation_without_vapor_path::Evaporation_without_vapor_path
(modifiable_ CORN::Dynamic_array<float64> &evaporation_potential_remaining_,     //160717
 const Layers_interface                 &layers_
,const Hydraulic_properties_interface   *hydraulic_props_
,Hydrology_finite_difference            &hydrology_)
: Evaporator
   (evaporation_potential_remaining_
   ,layers_
   ,*hydraulic_props_  //160606 Warning this actually should be point Need to modify Evaporator_soil
   ,hydrology_)
, hydrology_FD                      (hydrology_)
, first_day                         (true)                                       //090417
, period_evaporation_done           (false)                                      //090417
, evaporation_depth                 (0.1)                                        //090504
, sum_evaporation_mm_ts             (0.0)                                        //090417
, max_current_period_evaporation    (0.0)                                        //090417
, absolute_max_evaporation          (0.0) // setup in initialize()               //090417
, old_WC                            (0.0)                                        //090417
, mulch_cover_fraction              (0.0)
{}
//_Soil_evaporation_without_vapor_path:constructor_____________________________/
void Evaporation_without_vapor_path::initialize
(nat8 number_real_plus_extended_layer)                             modification_
{
   // currently doing nothing, but should initialize the vapor_path_length_lower
   // Currently the VB codes does not do this.
   evaporation_depth = layers.get_thickness_m(1);                                //090504
   float64 PWP_1  =
      hydraulic_properties.
      get_permanent_wilt_point_volumetric(1);
   float64 WC_1   = hydrology_FD.get_liquid_water_content_volumetric(1);
   float64 WC_sat_1 =
      hydraulic_properties.
      get_saturation_water_content_volumetric(1,ICE_WATER_CONTENT_RELEVENCE_UNKNOWN_140822);  //090504
   absolute_max_evaporation
      = (WC_sat_1 - PWP_1 / 3.0) * evaporation_depth  * water_density;
   max_current_period_evaporation
      = (WC_1 - PWP_1 / 3.0)  * water_density * evaporation_depth;
     /* Max_Current_Period_Evaporation range validation is done in evaporate() */

    /* Sum_Evaporation set in constructor */
    /* Period_Evaporation_Done set in constructor */
}
//_initialize_______________________________________________________2009-04-17_/
 float64 Evaporation_without_vapor_path::evaporate
 ( float64 &water_depth_in_mm_per_ts
    /* I return the actual evaporation now pass by reference */
    /* member reference */
 , Seconds time_step
 , Hour    hour
 , float64 pot_evap_mm_day  /* mm/day */ )                  modification_
   /* Soil is a member  */
   /* weather is a member */
 {
    float64 evaporation_to_subtract_mm_ts = 0.0;


    float64 new_WC = hydrology_FD.get_liquid_water_content_volumetric(1);        //090417
    if (first_day)                                                               //090417
    {  /* Absolute_Max_Evaporation setup in initialize() */                      //090417
       /* Max_Current_Period_Evaporation setup in initialize() */                //090417
       /* Max_Current_Period_Evaporation setup in initialize() */                //090417
                                                                                 //090417
       /* Sum_Evaporation setup in constructor */                                //090417
       /* Period_Evaporation_Done setup in constructor */                        //090417
       first_day = false;                                                        //090417
    } else if ((new_WC > old_WC)                                                 //090417
             && (water_depth_in_mm_per_ts > 0))                                  //090504
    {  //170217unused float64 soil_thickness_1_m = soil_layers.get_thickness_m(1); //090417
       /* max_current_period_evaporation range validation is done below */       //090417
                                                                                 //090417
       float64 PWP_1 = hydraulic_properties.
         get_permanent_wilt_point_volumetric(1);                                 //090504
       float64 WC_1   = hydrology_FD.get_liquid_water_content_volumetric(1);     //090504
       max_current_period_evaporation                                            //090504
       = (WC_1 - PWP_1 / 3.0)  * water_density   * evaporation_depth;            //090504
       sum_evaporation_mm_ts = 0;                                                //090417
       period_evaporation_done = false;                                          //090417
    }                                                                            //090417
    max_current_period_evaporation = CORN_must_be_between                        //090417
          (max_current_period_evaporation                                        //090417
          ,0.0,absolute_max_evaporation);                                        //090417
    float64 actual_evaporation_mm_ts = 0.0;
    /* actual_evaporation always defaults to 0 and is calculated as needed */    //090417
                                                                                 //090417
    if (! period_evaporation_done)                                               //090417
    {  float64 available_period_evaporation_mm_ts                                //090417
          = max_current_period_evaporation - sum_evaporation_mm_ts;              //090417
       actual_evaporation_mm_ts = calc_actual(time_step, hour,pot_evap_mm_day);  //090417
       if (actual_evaporation_mm_ts >= available_period_evaporation_mm_ts)       //090417
       {  actual_evaporation_mm_ts = available_period_evaporation_mm_ts;         //090417
          period_evaporation_done = true;                                        //090417
       }                                                                         //090417
    }                                                                            //090417
    actual_evaporation_mm_ts *= (1.0 - mulch_cover_fraction);                    //100610
    sum_evaporation_mm_ts += actual_evaporation_mm_ts;                           //090417
    evaporation_to_subtract_mm_ts = actual_evaporation_mm_ts;                    //090417
    // Reconstitute Flux in kg/m2/s with evaporation flux substracted
    // In the C++ code we are providing  water_depth_in_mm
    // flux_mm = water_depth_in_mm_per_ts / time_step;
    /* condition already evaluated in for loop */
       // RLN not sure if I is node or layer
       for (int i = 1; evaporation_to_subtract_mm_ts > 0.000000001; i++)
          subtract_water_from_layer(evaporation_to_subtract_mm_ts, i   );



    // water content after this time step evaporation and before next time step infiltration
    old_WC = hydrology_FD.get_liquid_water_content_volumetric(1);                //090417
    return actual_evaporation_mm_ts;   /* flux is also a return */               //090417
 }
//_evaporate___________________________________________________________________/
float64  Evaporation_without_vapor_path
::calc_actual
( /* member reference */
  Seconds time_step   /* May need to be integer */
                                             //, Time_Step As Double
, Hour    hour_0based
, float64 pot_evap_mm_day /* mm/day */
//, Soil As SoilClass
//, Weather As WeatherClass
)                                                                          const
{  // unused




   // unused

   // unused
   static const nat8 evaporating_layer = 1;
   float64 actual_evaporation_kg_m2_s = 0.0;
   // This data pertains to current evaporating layer
      float64 pot_evap_kg_m2_s
         = m_to_mm(evaporation_potential.get(hour_0based)) / time_step;          //160719

//hourly_fraction_of_demand = 1.0;

//cout << "WARNING hourly fraction of demand set to 1.0 for debugging" << endl;


//   if (hourly_fraction_of_demand > 0.0)   // The V.B. code does not have this optimization
//   {  // We only have actual evaporation if there is hourly demand.


//pot_evap_kg_m2_s =  0.15/ 3600.0;   this like was for comparing against V.B version
// cout << "pot_evap_kg_m2_s set to fixed value for debugging" << endl;


      // Potential evaporation is converted from m/hour to kg/m2/s
      float64 surface_vapor_conductance     /* kg.s / m^4 */
         = -pot_evap_kg_m2_s / atmospheric_water_potential;
      float64 wu_1 = hydrology_FD.get_upper_water_content_liquid(1);
      float64 wl_1 = hydrology_FD.get_lower_water_content_liquid(1);
      float64 PWP_1=hydraulic_properties.get_permanent_wilt_point_volumetric(1);
      float64 evap_layer_WC =
      // 090417 changed back to only one fully 'dryable' layer
      (wu_1 > PWP_1)
      ? wu_1

      : 0.5 * wu_1 + wl_1 * 0.5;
//090504      : 0.8 * wu_1 + wl_1 * 0.2; // Could be 0.5 and 0.5

#ifdef OBSOLETE
090417 now only one dryable layer done above
      float64 wu_2   = hydrology_FD.get_upper_water_content_liquid(2);
      float64 wl_2   = hydrology_FD.get_lower_water_content_liquid(2);
      float64 evap_layer_WC =
         (wu_1 > PWP_1)
         ?  wu_1
         :
            only_one_dryable_layer
            ?  0.5 * (wl_1 + wu_2) :
               (wl_1 > PWP_1)
               ? wl_1
               :
                  0.5 * (wu_2 + wl_2);


      // The one dryable layer option has been incorporated into the
      // expression above and selected with only_one_dryable_layer




#endif
      float64 evap_layer_water_potential
         = hydrology_FD.calc_water_potential_with_respect_to_layer(evap_layer_WC,evaporating_layer);
      actual_evaporation_kg_m2_s =  surface_vapor_conductance
               * (evap_layer_water_potential - atmospheric_water_potential);
      actual_evaporation_kg_m2_s =
         CORN::must_be_0_or_greater<float64>(actual_evaporation_kg_m2_s);
// }  // else no potential evaporation so actual_evaporation_mm_ts remains 0.0;
   return actual_evaporation_kg_m2_s * time_step; /*convert kg/m2/s to mm/time step*/
}
//_calc_actual_________________________________________________________________/
void Evaporation_without_vapor_path
::subtract_water_from_layer
( /* member reference */
  float64 &evaporation_to_subtract
, nat8 i   /* appears to be layer but check that it's not node */
)                                                                  modification_
{
   float64 layer_evaporation = 0.0;  /* mm? */
   float64 Campbell_b_i    = hydraulic_properties.get_Campbell_b(i,false);
   float64 air_entry_pot_i = hydraulic_properties.get_air_entry_potential(i,false);

//cout << (int)i << endl;
//cout << "AWP:" << atmospheric_water_potential << endl;
//cout << "b:" << Campbell_b_i << endl;
//cout << "AEP:" << air_entry_pot_i << endl;
   float64 air_dry_water_content =
      hydraulic_properties.get_saturation_water_content_volumetric               //106060
            (i,ICE_WATER_CONTENT_RELEVENCE_UNKNOWN_140822)
        * pow((air_entry_pot_i
               / atmospheric_water_potential)
             , (1.0 /Campbell_b_i ));
   float64 half_layer_thickness_m = layers.get_thickness_m(i) / 2.0;
   // Substract evaporation from upper portion of the layer
   float64 wu_i   = hydrology_FD.get_upper_water_content_liquid(i);
   if (wu_i > air_dry_water_content )
   {  float64 water_depth_available_to_evaporate   /*in mm?*/
         = (wu_i - air_dry_water_content) * half_layer_thickness_m * water_density;
      if (water_depth_available_to_evaporate > evaporation_to_subtract)
      {    layer_evaporation = evaporation_to_subtract;
           evaporation_to_subtract = 0.0;
      } else
      {    layer_evaporation = water_depth_available_to_evaporate;
           evaporation_to_subtract = evaporation_to_subtract - layer_evaporation;
      }

      float64 water_content_change_due_to_soil_water_evaporation
         = layer_evaporation / (half_layer_thickness_m * water_density);
       wu_i -= water_content_change_due_to_soil_water_evaporation;
       hydrology_FD.set_upper_water_content_liquid(i,wu_i);
   }
   // Substract evaporation from lower portion of the layer
   float64 wl_i   = hydrology_FD.get_lower_water_content_liquid(i);
   if (wl_i > air_dry_water_content )

      // Even when  wu_i is infintesimally greater than  air_dry_water_content
      // this will cause all evaporation_to_subtract to be subtracted
      // This correction should also needed in V.B. code although
      // in these case were this condition occurs

   {  float64 water_depth_available_to_evaporate =  /* in mm */
         (wl_i-air_dry_water_content) * half_layer_thickness_m * water_density;
      if (water_depth_available_to_evaporate > evaporation_to_subtract)
      {    layer_evaporation = evaporation_to_subtract;
           evaporation_to_subtract = 0.0;
      } else
      {    layer_evaporation = water_depth_available_to_evaporate;
           evaporation_to_subtract -= layer_evaporation;
      }

       float64 water_content_change_due_to_soil_water_evaporation
         = layer_evaporation / (half_layer_thickness_m * water_density);
       wl_i -= water_content_change_due_to_soil_water_evaporation;
       hydrology_FD.set_lower_water_content_liquid(i,wl_i);
   }
}
//_subtract_water_from_layer___________________________________________________/
}//_namespace_Soil_____________________________________________________________/
