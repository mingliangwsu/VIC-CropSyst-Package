#if (VIC_CROPSYST_VERSION >= 3)
#  error This class is obsolete in version 3 remove this file from the project
/*
Version 3 now uses VIC_CropSyst_proper_crop
where the Crop class is based on CropSyst proper.
*/
#else
#include "VIC_crop_root.h"
//---------------------------------------------------------------------------
#include <math.h>
namespace VIC_crop
{
 extern float64 unassigned_reference;
//_____________________________________________________________________________/
Root:: Root
(const Root_parameters       &_parameters
,const Phenology_parameters  &_phenology_parameter
,bool                         _perennial)
: parameters(_parameters)
,phenology_parameter          (_phenology_parameter)
,perennial(_perennial)
,soil_layers                  (0) 
,thermal_time_cumulative      (&unassigned_reference)
//now passing has_above_ground_biomass,PotentialCumulativeBiomass   (&unassigned_reference)
,WaterStressFactor_yesterday  (&unassigned_reference)
, root_depth_current          (_parameters.get_depth_emergence_m())
, max_seasonal_root_depth_gain(_parameters.max_depth - _parameters.get_depth_emergence_m())
, normalized_cum_thermal_time_today(0)
, normalized_cum_thermal_time_yesterday(0.01504746)
{  for (uint8 layer = 0; layer < MAX_soil_layers_alloc; layer++)
     root_fraction[layer] = 0;
} 
//_____________________________________________________________________________/
void Root::start()
{  //Claudio In the VB code this was computed every day
   //Here I calcute this once for the first day and
   // simply copy the current value to yesterday's value
   normalized_cum_thermal_time_yesterday=
      (* thermal_time_cumulative

         //RLN This should actually be  thermal_time_cumulative_yesterday
         // But it is not critical.
         // I would need to make it available


        - phenology_parameter.emergence_deg_day)       // (CropState.CumulativeThermalTime(DAP) - CropParameters.GetThermalTimeForEmergence)
      / (phenology_parameter.max_canopy_deg_day - phenology_parameter.emergence_deg_day);         // was   / (CropParameters.ThermalTimeAtTheEndOfCanopyGrowth - CropParameters.GetThermalTimeForEmergence);

       max_seasonal_root_depth_gain = (parameters.max_depth - parameters.get_depth_emergence_m());
   #ifdef MATCH_VB
      normalized_cum_thermal_time_yesterday =  0.00214295;
   #endif

} 
//_2010-01-7____________________________________________________________________
void Root::update(nat16 days_after_season_start, bool has_above_ground_biomass)           // was CalculateRootGrowth
{  float64 planting_depth = parameters.sow_depth;                                      // CropParameters.GetPlantingDepth
   float64 curvature = parameters.density_distribution_curvature;                      // CropParameters.GetCurvatureRootDensityFunction
   if (perennial)                                                                      // If CropParameters.PerennialCrop = True Then
   {  root_depth_current = parameters.max_depth;                                       //   Current_Root_Depth = Max_Root_Depth
   } else
   {                                                                                  // ELSE
   //'Total_Root_Biomass_Yesterday = CropState.CumulativeRootBiomass(DAP - 1)
   //RLN moved to constructor float64 Root_Depth_At_Emergence = parameters.depth_emergence;                       // was CropParameters.GetRootDepthAtEmergence
   //RLN only need to set this once Max_Root_Depth = parameters.depth_maximum;                                        // CropParameters.GetMaxRootDepth
   //RLN only need to set this once float64 max_seasonal_root_depth_gain = parameters.depth_maximum - Root_Depth_At_Emergence;
   //'Calculate root depth

   if (*thermal_time_cumulative <= phenology_parameter.max_canopy_deg_day)             // (CropState.CumulativeThermalTime(DAP) <= CropParameters.ThermalTimeAtTheEndOfCanopyGrowth)
   {  float64 stress_factor =
         has_above_ground_biomass                                                      // (PotentialCumulativeBiomass > 0)                                              // if (CropState.PotentialCumulativeBiomass(DAP) > 0)
         ? pow(*WaterStressFactor_yesterday                                            // CropState.WaterStressFactor(DAP - 1)
              , ((float64)parameters.sensitivity_to_water_stress))
         : 1.0;
      //'Calculate root depth gain today
      normalized_cum_thermal_time_today =
              (*thermal_time_cumulative - phenology_parameter.emergence_deg_day)       // (CropState.CumulativeThermalTime(DAP) - CropParameters.GetThermalTimeForEmergence)
              / (phenology_parameter.max_canopy_deg_day - phenology_parameter.emergence_deg_day);         // was   / (CropParameters.ThermalTimeAtTheEndOfCanopyGrowth - CropParameters.GetThermalTimeForEmergence);

      if (days_after_season_start > 1)                                                 //    If TimeState.DaysAfterGrowingSeasonStarted > 1 Then
      {
         /*RLN moved to the end of this method (yesterdays is simply todays's for tomorrow.
         normalized_cum_thermal_time_yesterday =
               = (CropState.CumulativeThermalTime(DAP - 1) - CropParameters.GetThermalTimeForEmergence) / _
           (CropParameters.ThermalTimeAtTheEndOfCanopyGrowth - CropParameters.GetThermalTimeForEmergence)
         */
         float64 unstressed_root_depth_today     = parameters.get_depth_emergence_m() + max_seasonal_root_depth_gain * normalized_cum_thermal_time_today;
         float64 unstressed_root_depth_yesterday = parameters.get_depth_emergence_m() + max_seasonal_root_depth_gain * normalized_cum_thermal_time_yesterday;
         float64 root_depth_growth_today = (unstressed_root_depth_today - unstressed_root_depth_yesterday) * stress_factor;
         root_depth_current += root_depth_growth_today;                                // root_depth_current = CropState.RootDepth(DAP - 1) + root_depth_growth_today;
      }
      // RLN already set in the constructor else root_depth_current = Root_Depth_At_Emergence;
      // End If
      // RLN not kept in an array CropState.RootDepth(DAP) = root_depth_current
   }
   } 
   /*RLN for C++ not needed, just keep the current root depth
   else
   {
       root_depth_current = CropState.RootDepth(DAP - 1)
       CropState.RootDepth(DAP) = root_depth_current
   }  // End If
   */
   nat8 number_of_soil_layers = soil_layers->get_number_layers();                      // Number_Of_Soil_Layers = SoilParameters.GetNumberOfLayers
   float64 Sum_Weighting_Factor = 0.0; // RLN not actually used, but could be used to verify they add to 1.
   float64 sum_root_fraction_weighting_factor = 0;
   soil_layer_array64(root_fraction_weighting_factor);
   soil_layer_array64(soil_thickness_having_roots);  // was  Thickness_With_Roots

   for (nat8 layer = 0 ; layer < MAX_soil_layers_alloc; layer++)
   {  root_fraction_weighting_factor[layer] = 0.0;
      soil_thickness_having_roots   [layer] = 0.0;
   } 
   for (nat8 layer = 2 ; layer < number_of_soil_layers; layer++)
   {  // RLN doesn't need to be an array                                            // Weigthing_Factor[layer] = 0
      soil_thickness_having_roots[layer] = soil_layers->get_thickness_m(layer);     // SoilParameters.GetDZ[layer]
   // WARNING need to confirm units of soil_thickness_having_roots
   }
   nat8 seading_depth_layer = 2 ; // was Layer_Number_Of_Seeding_Depth
   //    Dim Current_Layer_Bottom_Depth As Single
   // Current_Layer_Bottom_Depth = 0
   for (nat8 layer = 2 ; layer <= number_of_soil_layers; layer ++)
   {  float64 Current_Layer_Bottom_Depth = soil_layers->get_depth_m(layer);      // was Current_Layer_Bottom_Depth = Current_Layer_Bottom_Depth + soil_thickness_having_roots[layer]
      if (Current_Layer_Bottom_Depth > planting_depth)
      {  soil_thickness_having_roots[layer] = Current_Layer_Bottom_Depth - planting_depth;
         seading_depth_layer = layer;
         layer = number_of_soil_layers;
      } else
      {  root_fraction[layer] = 0.0;                                             // was CropState.RootFraction(DAP, layer) = 0;
         soil_thickness_having_roots[layer] = 0;
      }  // End If
   }  // for
   float64 mid_soil_layer_depth = 0.0;
   for (nat8 layer = seading_depth_layer; layer <= number_of_soil_layers; layer++)
   {   //'Determine mid layer depth
       mid_soil_layer_depth +=
         (layer == seading_depth_layer )
         ? soil_thickness_having_roots[layer] / 2.0
         : soil_thickness_having_roots[layer-1] / 2.0 + soil_thickness_having_roots[layer] / 2.0;
       //'Calculate exponential distribution function per soil layer
      float64 weighting_factor_layer = 0.0;
      if ((root_depth_current - planting_depth) >= mid_soil_layer_depth )
      {  weighting_factor_layer                                                  //was Weigthing_Factor[layer]
         = (-(1.0 - exp(curvature * ((root_depth_current - planting_depth) - mid_soil_layer_depth))));
         root_fraction_weighting_factor[layer] = weighting_factor_layer * soil_thickness_having_roots[layer];
      } else
      {  // RLN already cleared above Weigthing_Factor[layer] = 0.0;
         root_fraction_weighting_factor[layer] = 0.0;
      } 
       Sum_Weighting_Factor += weighting_factor_layer;                           // Weigthing_Factor[layer];
       sum_root_fraction_weighting_factor += root_fraction_weighting_factor[layer];
   }  // for
   //'Calculate root fraction and root density
   float64 Sum_Root_Fraction = 0; //'this is just a check
   // RLN unused Total_Root_Biomass_Today = 0;
   for (int layer = seading_depth_layer
      ; (layer <= number_of_soil_layers)
        && (sum_root_fraction_weighting_factor != 0)
      ; layer++)
   {   //'Root fraction
       root_fraction[layer] = root_fraction_weighting_factor[layer] / sum_root_fraction_weighting_factor;
       Sum_Root_Fraction += root_fraction[layer];                                // was = Sum_Root_Fraction + CropState.RootFraction(DAP, layer) //'this is just a check
   } 
   normalized_cum_thermal_time_yesterday = normalized_cum_thermal_time_today;
} 
//_____________________________________________________________________________/
void Root::assign_references
( const float64 */*140810  Ref_to_const_float*/  _thermal_time_cumulative
, const float64 */*140810  Ref_to_const_float*/  _WaterStressFactor_yesterday)
// this could actually be simply WaterStressFactor that hasn't been updated yet
{  thermal_time_cumulative    = _thermal_time_cumulative;
   WaterStressFactor_yesterday= _WaterStressFactor_yesterday;
} 
//_____________________________________________________________________________/
void Root::know(const Soil_layers_interface         *_soil_layers)
{   soil_layers                = _soil_layers;
}
//_____________________________________________________________________________/
}  // namespace VIC_crop
#endif

