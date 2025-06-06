#include "soil_properties_sublayers.h"
#  ifndef compareHPP
#     include "corn/math/compare.hpp"
#  endif
#  include <math.h>
#include "common/physics/standard_constants.h"
#include "soil/txtrhydr.h"
namespace Soil {                                                                 //181206
//______________________________________________________________________________
Properties_sublayers::Properties_sublayers
(Soil_parameters *_parameters,bool _parameters_owned)
:parameters(_parameters)
,parameters_owned(_parameters_owned)
,layers(new Soil_sublayers(*parameters))
,hydraulic_properties                  (new Hydraulic_properties(*ref_layers())) //181206_051204
,NH4_Langmuir                                                     (2000.0,0.003) //190404
{}
//_Properties_sublayers:constructor____________________________________________/
bool Properties_sublayers::reinitialize
(bool beginning_of_simulation
,bool subdivide_horizons                                                         //060229
,bool finite_diff)                                                               //990311
{  float64 soil_depth = 0.0;
   for (nat8  horizon = 1; horizon <= parameters->horizon_count ; horizon++)
      soil_depth += parameters->get_horizon_thickness(horizon);
   float64 sublayer_depth = 0.0; //Start subdivisions from the second soil layer
   nat8  sublayer = 1;
   for (nat8 horizon = 1; horizon <= (nat8)parameters->horizon_count; horizon++)
   {  if (subdivide_horizons)                                                    //060229
      {  // Sublayering scheme:
         //         - Sublayer 1 is always what the user specifies.
         //         - until 50cm subdivide the layers into approx. 5cm
         //         - until 1.2cm subdivide the layers into approx. 10cm
         //         - remaining will be 20cm.
         //        {16 bit version doesn't split well unless we give a little tolerance}
      layers->divisions[horizon ] = 0;
      float64 thickness_to_divide = parameters->get_horizon_thickness(horizon );
      // thickness_to_divide is the remaining amount of
      // the sublayer thickness to divide.
      while (! CORN::is_zero<float64>(thickness_to_divide/*200127,0.0*/,0.0001)
         && (sublayer < MAX_soil_sublayers))                                     //000323
      {  float64 sublayer_thickness = thickness_to_divide;
         if (sublayer == 1)
         {  float64 evaporative_layer_thickness =
                  (finite_diff
                  ? parameters->evaporative_layer_thickness_finite_difference
                  : parameters->evaporative_layer_thickness_cascade);
            sublayer_thickness = std::min<float64>(thickness_to_divide,evaporative_layer_thickness);
         } else
         {
            float64 MAX_sublayer_thickness  =
             ((sublayer_depth > 1.2001)
              ? 0.2001
                : (sublayer_depth > 0.5001)
                  ? 0.1001
                  : 0.0501);
#ifdef OBSOLETE
May need to do this if GIS
/*051207_*/             if (finite_diff)
/*051207_*/             {  switch (sublayer)
/*051207_*/                {  case 0 : case 1 :  MAX_sublayer_thickness = 0.01; break;
/*051207_*/                   case 2 :  MAX_sublayer_thickness = 0.02; break;
/*051207_*/                   case 3 : case 4 : case 5 :  MAX_sublayer_thickness = 0.03; break;
/*051207_*/                   case 6 :  MAX_sublayer_thickness = 0.04; break;
/*051207_*/                   case 7 :  MAX_sublayer_thickness = 0.05; break;
/*051207_*/                }
/*051207_*/             }
#endif
            {
               int divs = thickness_to_divide / MAX_sublayer_thickness;
               int divs_1 =(thickness_to_divide / MAX_sublayer_thickness) +1;
               if (divs <= 0) divs = 1;  // must be at least one division
               sublayer_thickness = thickness_to_divide / divs;
               float64 sublayer_thickness_1 = thickness_to_divide / divs_1;
               if (fabs(sublayer_thickness_1 -MAX_sublayer_thickness) <
                   fabs(sublayer_thickness   -MAX_sublayer_thickness))
                  sublayer_thickness = sublayer_thickness_1;
            }                                                                    //051121
        }
        sublayer_depth += sublayer_thickness;
        thickness_to_divide -= sublayer_thickness;
        subdivide_horizon(horizon ,sublayer,sublayer_thickness,sublayer_depth);
        layers->divisions[horizon ]++;
        sublayer++;
        layers->num_uninundated_sublayers =                                      //080211
        layers->num_sublayers = (nat8 )(sublayer - 1);
     }
     } else // dont subdivide horizons
     {   float64 layer_thickness = parameters->get_horizon_thickness(horizon );  //060229
         sublayer_depth += layer_thickness;                                      //060229
         subdivide_horizon(horizon ,horizon,layer_thickness,sublayer_depth);     //060229
         layers->divisions[horizon ]=1;                                          //060229
         layers->num_uninundated_sublayers =                                     //080211
         layers->num_sublayers = (nat8 )parameters->horizon_count;
     }
   }
   return true;
}
//_reinitialize_____________________________________________________2010-11-04_/
void Properties_sublayers::subdivide_horizon
(nat8  horizon
,nat8  sublayer
,float64 sublayer_thickness
,float64 sublayer_depth)
{  layers->in_horizon_of[sublayer] = horizon;
   //The following 3 are constant after initialization
   layers->depth[sublayer] = sublayer_depth;
   layers->thickness[sublayer] = sublayer_thickness;
   float64 sat_water_content =                                                   //080313
            parameters->get_horizon_saturation_WC(horizon);                      //160916_080313
   hydraulic_properties->set_saturation_water_content_volumetric(sublayer,sat_water_content); //080313
   float64 FC_sublayer = hydraulic_properties->set_field_capacity_volumetric(sublayer,parameters->get_horizon_field_capacity_volumetric(horizon));  //160915_051204
   float64 PWP_sublayer = hydraulic_properties->set_permanent_wilt_point_volumetric(sublayer,parameters->get_horizon_permanent_wilt_point_volumetric(horizon)); //160915_051204
   hydraulic_properties->set_bypass_coef(sublayer,parameters->get_horizon_bypass_coef(horizon)); //070117
   float64 horizon_sand = parameters->get_horizon_sand(horizon);                 //160916
   float64 horizon_clay = parameters->get_horizon_clay(horizon);                 //160916
   float64 horizon_silt = parameters->get_horizon_silt(horizon);                 //160916
   texture.set_sand_clay_silt(sublayer,horizon_sand,horizon_clay,horizon_silt);  //060724
   float64 Campbell_b_parameter_sublayer = parameters->get_horizon_Campbell_b(horizon);
   //    air_entry_pot[] is constant after initialization}
   hydraulic_properties->set_Campbell_b_parameter     (sublayer,Campbell_b_parameter_sublayer);  //051205


//150411RLN I am not sure why this was disabled for MicroBasin version
// I am reinstating this, If it causes problems, See roger

   modifiable_ Soil_texture_hydraulics  *layer_texture_hydraulics = parameters->mod_texture_hydraulics(layers->in_horizon_of[sublayer]);

   float64 Campbell_b_FC_PWP_based_sublayer = layer_texture_hydraulics->calc_Campbell_b(FC_sublayer ,PWP_sublayer);
   hydraulic_properties->set_Campbell_b_FC_PWP_based  (sublayer,Campbell_b_FC_PWP_based_sublayer);     //051205
   float64 air_entry_pot_FC_PWP_based_sublayer = layer_texture_hydraulics->calc_air_entry_pot    //051205
      (FC_sublayer ,sat_water_content,Campbell_b_FC_PWP_based_sublayer);
   hydraulic_properties->set_air_entry_potential_FC_PWP_based(sublayer,air_entry_pot_FC_PWP_based_sublayer); //051205
   float64 air_entry_pot_parameter_sublayer = parameters->get_horizon_air_entry_pot(horizon);    //051205
   hydraulic_properties->set_air_entry_potential_parameter(sublayer,air_entry_pot_parameter_sublayer); //051205
   {  float64 sat_hydraul_cond_m_d_horizon = parameters->get_horizon_sat_hydraul_cond_m_d(horizon); //160915
      float64 sat_hydraul_cond_kg_s_m3 =  //saturated hydraulic conductivity, Kgsec/m3  //051205
         CORN::is_zero<float64>(sat_hydraul_cond_m_d_horizon,0.000001)           //980526
         ? 1.0e-3 / CORN_sqr(air_entry_pot_parameter_sublayer)                   //990331
         : sat_hydraul_cond_m_d_horizon / (standard_gravity *86.4); // convert to flux unit?   //980526
      hydraulic_properties->set_saturated_hydraulic_conductivity_kg_s_m3(sublayer,sat_hydraul_cond_kg_s_m3);
   }
   NH4_Langmuir.K.set(sublayer,parameters->NH4_Langmuir.K.get(horizon));         //190404
   NH4_Langmuir.Q.set(sublayer,parameters->NH4_Langmuir.Q.get(horizon));         //190404
}
//_subdivide_horizon________________________________________________2010-11-04_/
}//_namespace_Soil_____________________________________________________________/

