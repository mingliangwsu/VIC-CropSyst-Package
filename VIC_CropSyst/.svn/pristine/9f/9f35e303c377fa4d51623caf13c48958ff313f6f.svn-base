#if (VIC_CROPSYST_VERSION >= 3)
#  error This class is obsolete in version 3
/*
Version 3 now uses VIC_CropSyst_proper_crop
where the Crop class is based on CropSyst proper.
*/
#else

#if (!VB_VERSION_DATE || (VB_VERSION_DATE >= 20110110))

#include "VIC_crop_transpiration.h"
#include "VIC_crop_phenology.h"
#include "VIC_crop_root.h"
#include "crop/CO2_response.h"
#include "soil/layering.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "VIC_soil_hydrology.h"
#ifdef __cplusplus
}
#endif
#include "corn/measure/measures.h"
#include <math.h>
#ifndef compareHPP
#  include "corn/math/compare.hpp"
#endif

#include <iostream>
#include "biometeorology/ET_reference_interface.h"

#include "soil/layers_interface.h"
#include "soil/hydrology_interface.h"
#include "corn/quality.hpp"
#include "weather/parameter/WP_relative_humidity.h"
#include "weather/parameter/WP_wind_speed.h"
#include "weather/parameter/WP_ET_reference.h"

//_____________________________________________________________________________/
namespace VIC_crop
{
 extern float64 unassigned_reference;
//_____________________________________________________________________________/
Transpiration_abstract::Transpiration_abstract
(const Transpiration_parameters  &_transpiration_parameters
,const Canopy_parameters         &_canopy_parameters
,const Canopy_growth_common_parameters &_canopy_growth_common_parameter
,float32 _phenology_parameter_end_vegetative_growth_deg_day
)
: parameter                  (_transpiration_parameters)
, canopy_parameter           (_canopy_parameters)
, canopy_growth_common_parameter(_canopy_growth_common_parameter)
, phenology_parameter_end_vegetative_growth_deg_day
   (_phenology_parameter_end_vegetative_growth_deg_day)
, canopy_solar_rad_intercepted_fraction(&unassigned_reference)
, thermal_time_cumulative     (&unassigned_reference)
#ifdef CROPSYST_PROVIDED_ET
, ET_ref             (0)
#endif
, CO2_response       (0) // will be set with assign_references
, crop_height        (0)
,crop_height_max_attained(0)
, wind_speed_element (0)
, RH_min_element     (0)
, ET_crop_coefficient(1.0) // at emergence
, ET_crop_coef_full_shading(1.0)
, potential_mm       (0)
, potential_cum_mm   (0)
, crop_potential_ET  (0)
, crop_potential_ET_cum(0)
, ET_reference_crop_CO2_adjusted(0)                                              //110902
, ET_crop_coef_full_shading_yesterday(0.0)                                       //110406 (1.0)
, ET_crop_coef_unadj_max(1.0)
{}
//_____________________________________________________________________________/
void Transpiration_abstract::assign_references
(
const float64 */*140810  Ref_to_const_float*/             _canopy_solar_rad_intercepted_fraction
,const float64 */*140810  Ref_to_const_float*/             _thermal_time_cumulative
,CropSyst::Crop_CO2_response  *_CO2_response                                     //110903
#if (!VB_VERSION_DATE || (VB_VERSION_DATE >= 20110110))
,const Wind_speed             *_wind_speed_element                               //110111
,const Relative_humidity      *_RH_min_element
#ifdef CROPSYST_PROVIDED_ET
,const ET_reference /*140810  Ref_to_const_float*/     *_ET_ref                //110113
#endif
#endif
)                     //110111
{  canopy_solar_rad_intercepted_fraction = _canopy_solar_rad_intercepted_fraction;
   thermal_time_cumulative = _thermal_time_cumulative;
   CO2_response = _CO2_response;
   #if (!VB_VERSION_DATE || (VB_VERSION_DATE >= 20110110))
   wind_speed_element = _wind_speed_element;
   RH_min_element = _RH_min_element;
   #ifdef CROPSYST_PROVIDED_ET
   ET_ref = _ET_ref;                                                             //110113
   #endif
   #endif
}
//_____________________________________________________________________________/
bool Transpiration_abstract::start_day()                           modification_
{  ET_crop_coef_full_shading_yesterday = ET_crop_coef_full_shading;
   return true;
}
//_110601____________________________________________________________________________/
bool Transpiration_actual::start_day()                             modification_
{  water_stress_index_yesterday = water_stress_index;
   leaf_water_potential_yesterday = leaf_water_potential;
   water_stress_factor_yesterday = water_stress_factor;
   return Transpiration_abstract::start_day();
}
//_110601____________________________________________________________________________/
bool Transpiration_abstract::start_season()                        modification_
{  potential_cum_mm = 0.0;
   crop_potential_ET_cum = 0.0;
   return true;
}
//_2011-06-24___________________________________________________________________
bool Transpiration_actual::start_season()                          modification_
{  water_stress_flowering_index.reset();
   actual_cum_mm = 0.0;
   return Transpiration_abstract::start_season();
}
//_2011-06-24___________________________________________________________________
Transpiration_actual::Transpiration_actual
(const Transpiration_parameters  &_transpiration_parameters
,const Canopy_parameters         &_canopy_parameters
,const Canopy_growth_common_parameters &_canopy_growth_common_parameter
,float32 _phenology_parameter_end_vegetative_growth_deg_day)
: Transpiration_abstract
   (_transpiration_parameters
   ,_canopy_parameters
   ,_canopy_growth_common_parameter
   ,_phenology_parameter_end_vegetative_growth_deg_day
   )
, phenology       (0)//(_phenology)
, soil_layers     (0)//(_soil_layers)
, soil_hydrology  (0)//(_soil_hydrology)
, root            (0)//(_root)
, water_stress_factor            (1.0) , water_stress_factor_yesterday(1.0)
, water_stress_index             (0.0) , water_stress_index_yesterday (0.0)
, water_stress_flowering_index   () //110524(0.0)
, actual_mm                      (0.0)
, actual_cum_mm                  (0.0)
, leaf_water_potential           (0.0)
, leaf_water_potential_yesterday (0.0)
{  for (uint8 layer = 0; layer < MAX_soil_layers_alloc; layer++)
     water_uptake_m[layer] = 0;
}
//______________________________________________________________________________
float64 Transpiration_abstract::update_crop_ET_coef_full_shading()    modification_
{
   // Crop ET coefficient: it grows from Kc=1 until reaching the maximum Kc at full canopy, assuming that any canopy,
   // no matter how sparse, will have Kc>1 if actually covering (shading) the ground completely (a reflection of roughness due to heigth)
   float64 maximum_cover_CO2_adj = canopy_parameter.maximum_cover_CO2_adj;
   if (CORN::is_approximately<float32>(parameter.ET_coef,0.0,0.000001)) // 'The model will determine ET crop coefficients
   {
      if (   ((*canopy_solar_rad_intercepted_fraction) < maximum_cover_CO2_adj) //140317 get_maximum_cover(true)
           && ((*thermal_time_cumulative) <= phenology_parameter_end_vegetative_growth_deg_day /*phenology_parameter.end_vegetative_growth_deg_day*/))
        {
            float64 height_maximum =
               canopy_growth_common_parameter.max_canopy_height;
            float64 crop_height = height_maximum * (*canopy_solar_rad_intercepted_fraction) / maximum_cover_CO2_adj; //140317canopy_parameter.get_maximum_cover (true);
            crop_height_max_attained = crop_height; //store value for use after maximum canopy growth is reached
            float64 ET_crop_coef_unadj = CalculateFullShadingCropETCoeff(crop_height); // '* Crop_ET_Coeff_Ratio 'scale to Kc user input
            // Current crop coefficient is now adjusted for current weather conditions
            ET_crop_coef_unadj_max = ET_crop_coef_unadj; // 'store value for use after maximum canopy growth is reached
            ET_crop_coef_full_shading = calc_weather_adjusted_crop_ET_coef(crop_height, ET_crop_coef_unadj);
        } else
            ET_crop_coef_full_shading = calc_weather_adjusted_crop_ET_coef(crop_height_max_attained, ET_crop_coef_unadj_max);
   } else
   {  ET_crop_coef_full_shading
         = 1.0 + (parameter.ET_coef - 1.0)
           * ((*canopy_solar_rad_intercepted_fraction) / maximum_cover_CO2_adj); //140317 canopy_parameter.get_maximum_cover(true));
      ET_crop_coef_full_shading = std::max<float64>(ET_crop_coef_full_shading_yesterday, ET_crop_coef_full_shading);
   }
//110601 moved to start_day   ET_crop_coef_full_shading_yesterday = ET_crop_coef_full_shading; // remember for tomorrow
   return ET_crop_coef_full_shading;
}
//_2010-01-11___________________________________________________________________
float64 Transpiration_abstract::update_potential()                               //was DeterminePotentialTranspiration
{
   float64 CO2_adjustment_to_transpiration =                                     // was CropState.CO2AdjustmentToTranspiration(DAP)
      CO2_response ?
         CO2_response->get_TUE_adjustment()                                      //140317
         //CO2_response->transpiration_adjustment
         : 1.0;
   #ifdef CROPSYST_PROVIDED_SOIL_HYDROLOGY
   update_crop_ET_coef_full_shading(); // sets ET_crop_coef_full_shading
   float64 ET_reference_crop =                                                   // was CropState.ReferenceCropET(DAP)  = WeatherState.ReferenceCropET(Year_Count, DOY)
      ET_ref->get_mm();                                                          //110113
                                                                                 //    CropState.CurrentETCropCoefficientFullShading(DACE) = ET_crop_coef_full_shading
   crop_potential_ET =ET_reference_crop * ET_crop_coef_full_shading;
   crop_potential_ET -= VIC_get_irrigation_droplet_evaporation_mm();             //121116
      // droplet_evaporation is added to accomodate energy loss
      // from irrigation water droplet evaporation
   crop_potential_ET_cum +=  crop_potential_ET;
    //'Calculate potential transpiration for actual crop
   ET_crop_coefficient = ET_crop_coef_full_shading * (*canopy_solar_rad_intercepted_fraction);
   //CropState.CurrentETCropCoefficient(DACE) = ET_crop_coefficient
   ET_reference_crop_CO2_adjusted  = CO2_adjustment_to_transpiration * ET_reference_crop;
   potential_mm = ET_reference_crop_CO2_adjusted * ET_crop_coefficient /* fraction the goes to transpiration */
      - VIC_get_canopy_interception_mm();                                        //121116
   #else
   //potential_mm = potential_mm; // VIC_get_crop_transpiration_potential();
   potential_mm *= CO2_adjustment_to_transpiration; /* this is actually just transpiraton adjustment */
   #endif
    //WeatherState.ActualCropPotentialTranspiration(Year_Count, DOY) = Potential_Transpiration
    //CropState.CropPotentialTranspiration(DACE) = Potential_Transpiration
    //CropState.CumulativeCropPotentialTranspiration(DACE) = CropState.CumulativeCropPotentialTranspiration(DACE - 1) + Potential_Transpiration
   potential_cum_mm += potential_mm;
   return potential_mm;
}
//_2010-01-11___________________________________________________________________
float64  Transpiration_actual::update_water_uptake()                   modification_ // was CalculateWaterUptake  and PerformWaterUptake
{
   //110601 moved to start daywater_stress_factor_yesterday = water_stress_factor;
//RLN unused   static const float64 water_density = 1000.0; //'kgH2O/m3H2O
   //110601 moved to start day leaf_water_potential_yesterday = leaf_water_potential;                        // RLN added to obviate array
   float64 fract_green_canopy_interception_solar_rad                             // was Fract_Green_Canopy_Interception_SolRad
      = (*canopy_solar_rad_intercepted_fraction);                                // was = CropState.FractionSolarRadInterceptedGreenCanopy(DAP);
        // canopy_solar_rad_intercepted_fraction is canopy_actual.solar_rad_intercepted_green_canopy_fraction
   float64 water_uptake_mm = 0.0;  //'Initialize crop water uptake = 0              // was Crop_Water_Uptake
   float64 expected_water_uptake_mm = 0.0;

   if (fract_green_canopy_interception_solar_rad > 0)
   {
      //'Define soil water potential at field capacity
      static const float64 soil_water_potential_at_field_capacity = -33.0;
      // was       float64 LWP_at_onset_of_stress = CropParameters.GetLeafWaterPotentialForStomatalClosure
      // RLN now member                                                          // was Full_Cover_Crop_Water_Uptake_Max  = CropParameters.GetMaxWaterUptake;
      // RLN moved to member float64 wilt_LWP = CropParameters.GetWiltingLeafWaterPotential
      int soil_layers_count = soil_layers->get_number_layers();
      /*RLN
      for (layer = 1; layer < soil_layers_count; layer++)
      {
         Soil_Water_Potential[layer] = SoilState.WaterPotential(Year_Count, DOY, layer);
         Root_Fraction[layer] =
      }
      */
      //Calculate potential transpiration rate (kg/m2/d = mm/d)
                                                                                 // was CropState.ActualCropPotentialTranspiration(DAP);
      float64 full_cover_water_uptake_max = parameter.
         #ifdef CROP_PARAMETERS_HARDCODED
         full_cover_water_uptake_max;
         #else
         normal_max_water_uptake_mm;
         #endif
         //110218 Warning I need to confirm with Claudio that these are the
         //same, I am pretty sure they are.

      //Calculate crop maximun water uptake rate (kg/m2/d = mm/d)
      float64 water_uptake_max_mm      // was Max_Crop_Water_Uptake
         = full_cover_water_uptake_max * fract_green_canopy_interception_solar_rad;
      //Calculate maximum crop transpiration rate (kg/m2/d = mm/d). This includes nitrogen stress reduction of transpiration
      float64 max_transpiration = CORN::must_be_between<float64>(potential_mm,0.0001, water_uptake_max_mm);
                                         // was Max_Crop_Transpiration
      //Calculate plant hydraulic conductivity ((kg^2)/(m2-J-d)
      float64 plant_hydraulic_conductance = (water_uptake_max_mm / (soil_water_potential_at_field_capacity - parameter.stomatal_closure_LWP));
      //Adjust plant hydraulic conductivity based on soil dryness
      float64 root_hydraulic_conductance = plant_hydraulic_conductance / 0.65;
      float64 top_hydraulic_conductance  = plant_hydraulic_conductance / 0.35;
      float64 root_conductance_adjustment = 0.0;
      //This constant should be calculated for each layer to account for salinity reduction of layer root permeability, currently not implemented
      float64 salinity_reduction_factor = 1.0; //'value between 0 and 1    // was Salinity_Reduction_Factor
              // RLN salinity_reduction_factor appears to be disabled for this
      soil_layer_array64(layer_root_conductance_adjustment);
      soil_layer_array64(layer_plant_hydraulic_conductance);
      soil_layer_array64(layer_root_hydraulic_conductance);
      for (nat8 layer = 0; layer < MAX_soil_layers; layer++)                     //110316
      {
         layer_root_conductance_adjustment[layer] = 0.0;
         layer_plant_hydraulic_conductance[layer] = 0.0;
         layer_root_hydraulic_conductance[layer] = 0.0;
         water_uptake_m[layer] = 0.0;                                            //110525
      }
      for (nat8 layer = 2 /*110316 1*/; layer <= soil_layers_count; layer++)
      {  float64 soil_WP_layer = soil_hydrology->get_water_potential(layer);
         float64 root_activity_factor_layer =
            1.0 - pow(((soil_WP_layer - soil_water_potential_at_field_capacity)
                     / (parameter.wilt_LWP - soil_water_potential_at_field_capacity)) , 8.0);
         root_activity_factor_layer =
            CORN::must_be_between<float64>(root_activity_factor_layer,0.0,1.0);
         float64 root_fraction = root->get_root_fraction(layer);                  // was CropState.RootFraction(DAP, layer);

//root_activity_factor_layer = 1.0; // RLN debugging test

         //RLN  Layer_Salinity_Reduction_Factor[layer] = salinity_reduction_factor; // RLN doesn't need to be an array
         layer_root_conductance_adjustment[layer]
            = root_activity_factor_layer
            * root_fraction
            * salinity_reduction_factor; // RLN doesn't need to be an array Layer_Salinity_Reduction_Factor[layer]
         root_conductance_adjustment = root_conductance_adjustment + layer_root_conductance_adjustment[layer];
         layer_root_hydraulic_conductance[layer] = root_hydraulic_conductance * layer_root_conductance_adjustment[layer];
      }
      for (nat8 layer = 2 /*110316 1*/; layer < soil_layers_count; layer++)
      {
         if (layer_root_conductance_adjustment[layer] > 0.0)
         {
            float64 layer_top_hydraulic_conductance/* RLN doesn't need to be an array [layer] */
               = top_hydraulic_conductance * layer_root_conductance_adjustment[layer] / root_conductance_adjustment;
            layer_plant_hydraulic_conductance[layer]
               =   layer_root_hydraulic_conductance[layer] * layer_top_hydraulic_conductance/*RLN [layer]*/
                / (layer_root_hydraulic_conductance[layer] + layer_top_hydraulic_conductance/*RLN [layer]*/);
         } else
            layer_plant_hydraulic_conductance[layer] = 0.0;
      } // for layer
      root_hydraulic_conductance *= root_conductance_adjustment;
      plant_hydraulic_conductance
         = (root_hydraulic_conductance * top_hydraulic_conductance) /
           (root_hydraulic_conductance + top_hydraulic_conductance);
      /* now member float64*/ water_stress_index = 1.0;
      if (plant_hydraulic_conductance > 0.0)
      {  //Calculate average soil water potential (J/kg)
         float64 soil_WP_avg = 0.0;  // was Average_Soil_WP
         for (nat8 layer = 2 ; layer < soil_layers_count; layer++)
         {  float64 soil_WP_layer = soil_hydrology->get_water_potential(layer);
            soil_WP_avg +=  soil_WP_layer * layer_root_conductance_adjustment[layer] / root_conductance_adjustment;
         }
         //Calculate leaf water potential
         leaf_water_potential = soil_WP_avg - potential_mm / plant_hydraulic_conductance;
         if (leaf_water_potential < parameter.stomatal_closure_LWP)
         {
            leaf_water_potential =
               (plant_hydraulic_conductance * soil_WP_avg *
                   (parameter.stomatal_closure_LWP - parameter.wilt_LWP)
                   + parameter.wilt_LWP * potential_mm)
               / (plant_hydraulic_conductance *
                  (parameter.stomatal_closure_LWP - parameter.wilt_LWP)
                  + potential_mm);
         }
         leaf_water_potential =
            CORN::must_be_greater_or_equal_to<float64>
            (leaf_water_potential,(float64)parameter.wilt_LWP);
        // Reduce transpiration when leaf water potential is less than the
        // critical leaf water potential at the onset of stomatal closure.
         float64 transpiration_ratio = 1.0;
         float64 attainable_transp = max_transpiration;  // RLN used only for verification
         if (leaf_water_potential < parameter.stomatal_closure_LWP)
         {
            // Attainable transpiration is just a check. It should be equal to actual transpiration in all cases
            /* Used only for verification
            attainable_transp = max_transpiration *
                (leaf_water_potential - parameter.wilt_LWP) /
                (parameter.stomatal_closure_LWP - parameter.wilt_LWP);
            */
            // Transpiration ratio is less than 1 when the atmospheric evaporative demand (potential transpiration),
            // corrected by a nitrogen stress factor, is greater than the maximum ability of the crop to uptake water
            // from a well-watered soil
            transpiration_ratio = max_transpiration / potential_mm;
         } // else defaults set above
         // Calculate crop water uptake (kg/m2/d = mm/d)
         for (int layer = 1; layer <= soil_layers->get_number_layers(); layer++)
         {  float64 soil_WP_layer = soil_hydrology->get_water_potential(layer);

           float64 demand_mm  // soil water to extract
               =  (soil_WP_layer > leaf_water_potential)                         //130718
               ? layer_plant_hydraulic_conductance[layer]
                   * (soil_WP_layer - leaf_water_potential)
                   * transpiration_ratio
               : 0.0 ;                                                           //130718
            //demand_mm = CORN::must_be_greater_or_equal_to<float64>(demand_mm,0.0);
            water_uptake_m[layer] = mm_to_m(demand_mm);

            /* RLN
            //'Record layer water uptake
            SoilState.LayerSoilWaterUptake(Year_Count, DOY, i) = soil_water_extracted
            //'Update soil water content
            SoilState.WaterContent(Year_Count, DOY, layer) = SoilState.WaterContent(Year_Count, DOY, layer) - soil_water_extracted / (SoilParameters.GetDZ(layer) * water_density)

            RLN warning I am not sure why water density is here
            soil_water_extracted / (SoilParameters.GetDZ(layer) * water_density)
            I looks to me like this should be simply a units conversion

            water_uptake += soil_water_extracted;
            */
            expected_water_uptake_mm += m_to_mm(water_uptake_m[layer]);
         }   // for layer

         water_uptake_mm = m_to_mm(soil_hydrology->extract_water(water_uptake_m ,1));
/*for debug
         if (!CORN::is_approximately(expected_water_uptake_mm,water_uptake_mm,0.00001))
            std::cout << "expected water uptake not matching actual uptake:" << expected_water_uptake_mm << "\t"<< water_uptake_mm<< std::endl;
*/
#ifdef UNIT_TEST
         if (expected_water_uptake_mm < water_uptake_mm)
            std::cout << "negative water uptake :" << expected_water_uptake_mm << "\t"<< water_uptake_mm<< std::endl;
#endif
      }
      water_stress_index =
         CORN::must_be_between<float64>
#ifdef MATCH_VB
         (1.0 - expected_water_uptake_mm / max_transpiration,0.0,1.0);
#else
         (1.0 - water_uptake_mm / max_transpiration,0.0,1.0);
#endif
   }
   else // Fraction of green area is zero
   {
      water_stress_index = 1.0;
      water_uptake_mm = 0.0;
   }
   water_stress_factor = 1.0 - water_stress_index;                               //   CropState.WaterStressFactor(DAP) = 1.0 - water_stress_index;
   if (phenology->is_flowering() && !phenology->is_in_yield_formation())
      water_stress_flowering_index.append(water_stress_index);
   actual_mm = water_uptake_mm;
   actual_cum_mm += water_uptake_mm;
   return water_uptake_mm;
}
//___________________________________________________________________________/
/*121217 obsolete
float64 Transpiration_abstract::get_ET_ref_output_mm()                        const
{  return ET_ref->get_mm();                                                      //110113
}
*/
//___________________________________________________________________________/
float64 Transpiration_abstract::CalculateFullShadingCropETCoeff(float64 crop_height) const          // Private Function CalculateFullShadingCropETCoeff(Crop_Height As Double) As Double
{
   return
      (crop_height > 0.12)                                                       //If Crop_Height > 0.12 Then
      ? 0.118 * log(crop_height) + 1.2428                                        //     CalculateFullShadingCropETCoeff = 0.118 * Log(Crop_Height) + 1.2428
      : 1.0;                                                                     //Else CalculateFullShadingCropETCoeff = 1 EndIf
}                                                                               //End Function
//___________________________________________________________________________/
float64 Transpiration_abstract::calc_weather_adjusted_crop_ET_coef               //CalculateWeatherAdjustedCropETCoeff
(float64 crop_height                                                             //,Crop_Height As Double
,float64 Unajusted_Crop_ET_Full_Shading)                                   const //,Unajusted_Crop_ET_Full_Shading As Double
                                                                                 //, WeatherData As WeatherReadFilesClass
                                                                                 //, TimeState As TimeStateClass)
{
   float64 RHmin = RH_min_element->get_in_units(UC_percent);                                        // RHmin = WeatherData.GetHumRelMin(Current_year, DOY)
   float64 wind_ms  = wind_speed_element->get_m_s();                             // WeatherData.GetWind(Current_year, DOY)
   RHmin =  CORN::must_be_between<float64>(RHmin,20.0,80.0);                     // If RHmin < 20 Then RHmin = 20 Else If RHmin > 80 Then RHmin = 80 End If
   wind_ms = CORN::must_be_between<float64>(wind_ms,1.0,6.0);                    // If Wind < 1 Then Wind = 1 Else If Wind > 6 Then Wind = 6 End If
   float64 Kc                                                                    //Kc = Unajusted_Crop_ET_Full_Shading + (0.04 * (Wind - 2) - 0.004 * (RHmin - 45)) * (Crop_Height / 3) ^ 0.3
      = Unajusted_Crop_ET_Full_Shading
      + (0.04 * (wind_ms - 2.0)
         - 0.004 * (RHmin - 45.0)) * pow((crop_height / 3.0) , 0.3);
   Kc = CORN::must_be_greater_or_equal_to<float64>(Kc,1.0);                      // If Kc < 1 Then Kc = 1
   return Kc;                                                                    // CalculateWeatherAdjustedCropETCoeff = Kc
}                                                                               // End Function

//_2011-01-10__________________________________________________________________/

} // namespace VIC_crop
#endif
#endif
