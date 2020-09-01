#if (VIC_CROPSYST_VERSION >= 3)
#  error This class is obsolete in version 3, remove this file from the project
/* Now we use VIC_CropSyst_proper_crop
where the Crop class based on CropSyst proper.
*/
#else
#ifndef compareHPP
#  include "corn/math/compare.hpp"
#endif
#include "VIC_crop.h"
#include "VIC_crop_param.h"
#include "crop/CO2_response.h"
#include <math.h>
#include "VIC_crop_variable_codes.h"
#include <iostream>
#include "corn/chronometry/date_types.hpp"
#include "UED/library/std_codes.h"
//170628LML #include "corn/datetime/date.cpp"
//included by header #include "crop/canopy_cover_continuum.h"
#include "crop/harvest_grain_asymptotic.c"

namespace VIC_crop
{
#include "soil/layers_interface.h"
//_____________________________________________________________________________/
Crop::Crop
(/*const*/Crop_parameters &_parameters
,CORN::Date &              _today
,const VIC::Weather_provider   &_weather
,bool                      _harvest_automatically
,bool                      _clipped_automatically                                //110426
,bool                      _mint_clipping                                        //110427
,bool                      enable_CO2_response)
: Crop_common                                                                    //140811
   (_parameters
   ,_today
   ,_clipped_automatically
   ,_mint_clipping
   )
/*140811 moved to Crop_common
: parameters               (_parameters)
, today(_today)
*/
, harvest_parameters
      (parameters.harvest)

,weather                   (_weather)
,soil_layers               (0)//(_soil_layers)
,soil_hydrology            (0)//(_soil_hydrology)
//140811 appears to be obsolete , harvest_automatically    (_harvest_automatically)
/*140811 moved to Crop_common
, clipped_automatically    (_clipped_automatically)
, mint_clipping            (_mint_clipping)
*/
, clippings_completed      (0)                                                   //110421
, yield_reference_kg_m2          (0.0)
, yield_actual_kg_m2             (0.0)
, biomass_after_harvest    (0.0)
, cumulative_thermal_time_to_trigger_automatic_clipping_events(800) // Currently this is the default hard coded parameter WARNING it should be a management parameter
, biomass_at_filling       (0.0)                                                 //140317
, biomass_at_maturity      (0.0)                                                 //140317

, CO2_response             (enable_CO2_response ? new CropSyst::Crop_CO2_response
                              (_parameters.CO2_response
                              #if (VIC_CROPSYST_VERSION < 2)
                              ,_weather
                              #else
                              ,vapor_pressure_saturation_slope                   //140507
                              ,psychrometric_constant                            //140508
                              ,aerodynamic_resistance_ref_plants                 //140508
                              ,weather.atmospheric_CO2_conc_ppm                  //140508
                              #endif
                              ) : 0) //110903
                              //140317 (_parameters.CO2_response,_parameters.canopy_growth_cover_based,_weather) : 0) //110903
, phenology_reference      (parameters.phenology,parameters.thermal_time,parameters.season_correction,false /* Currently no root crops *//*,weather*/
                           ,_today
                           ,parameters.is_perennial() ? &parameters.inactive_period : 0                           //110303
                           ,(parameters.harvested_part_labeled.get() == fruit_part) ? &parameters.dormancy : 0    //110303
                           ,weather.air_temperature_avg
                           ,weather.vapor_pressure_deficit_max)
, phenology_actual         (parameters.phenology,parameters.thermal_time,parameters.season_correction,false /* Currently no root crops *//*,weather*/
                           ,_today
                           ,parameters.is_perennial() ? &parameters.inactive_period : 0                           //110303
                           ,(parameters.harvested_part_labeled.get() == fruit_part) ? &parameters.dormancy : 0    //110303
                           ,weather.air_temperature_avg
                           ,weather.vapor_pressure_deficit_max)
, root                     (parameters.root,parameters.phenology
                           #ifdef CROP_PARAMETERS_HARDCODED
                           ,parameters.phenology.perennial
                           #else
                           ,parameters.is_perennial()
                           #endif
                           )
, transpiration_unstressed  (parameters.transpiration
                           ,parameters.canopy_growth_cover_based                 //110208 was canopy_growth_cover_based
                           #ifdef CROP_PARAMETERS_HARDCODED
                           ,parameters.canopy_growth_cover_based
                           #else
                           ,parameters.morphology
                           #endif
                           ,parameters.phenology.max_canopy_deg_day)
, biomass_reference        (_parameters.biomass_production
                           , _parameters.canopy_growth_cover_based
                           ,_parameters.thermal_time
                           ,weather.air_temperature_avg                          //140815
                           ,weather.solar_radiation                              //140815
                           ,weather.vapor_pressure_deficit_daytime               //140815
                           //140815 ,weather
                           )
, biomass_actual           (_parameters.biomass_production, _parameters.canopy_growth_cover_based,_parameters.thermal_time
                           ,parameters.is_fruit_tree()                           //110427
                           ,weather.air_temperature_avg                          //140815
                           ,weather.solar_radiation                              //140815
                           ,weather.vapor_pressure_deficit_daytime               //140815
                           //140815 ,weather
                           )

, canopy_cover_reference   (parameters.canopy_growth_cover_based
                           ,parameters.morphology
                           ,parameters.phenology,phenology_reference
                           ,parameters.is_fruit_tree())                           //120619_110422
, canopy_cover_actual      (parameters.canopy_growth_cover_based
                           ,parameters.morphology
                           ,parameters.phenology
                           ,phenology_actual
                           ,parameters.is_fruit_tree()                           //110422
                           ,transpiration_actual.ref_water_stress_index_yesterday()    //110902
                           ,canopy_cover_reference)

, transpiration_actual     (parameters.transpiration
                           ,parameters.canopy_growth_cover_based
                           #ifdef CROP_PARAMETERS_HARDCODED
                           ,parameters.canopy_growth_cover_based
                           #else
                           ,parameters.morphology
                           #endif
                           ,parameters.phenology.max_canopy_deg_day)

{
   if (CO2_response)
   {
      //CO2_response->know_canopy_cover_reference(&canopy_cover_reference); LML 141114 obs.
      CO2_response->initialize();                                                //110628
   }
   phenology_actual.assign_references
      (transpiration_actual.point_water_stress_index_yesterday());

      // RLN warning I am not sure in the original VB code if this is intended
      // to be the actual or reference cumulative
//   canopy_actual.know_reference_thermal_time_cumulative
//      (phenology_actual.thermal_time_cumulative) ;

//      answer only one thermal time accum. (I'll wait until Claudio provides and updated version
   transpiration_unstressed.assign_references
      (canopy_cover_reference.ref_global_solar_rad_intercepted_green_canopy()
      ,phenology_reference .ref_thermal_time_cumulative()   // Warning, not sure if this needs to be from phenology actual
      ,CO2_response
      #if (!VB_VERSION_DATE || (VB_VERSION_DATE >= 20110110))
      ,&weather.wind_speed
      ,&weather.relative_humidity_min
      #ifdef CROPSYST_PROVIDED_ET
      ,&weather.ET_reference
      #endif
      #endif
      );                                                   //110113
   transpiration_actual .assign_references
      (canopy_cover_actual    .ref_global_solar_rad_intercepted_green_canopy()
      ,phenology_actual .ref_thermal_time_cumulative()
      ,CO2_response
      ,&weather.wind_speed
      ,&weather.relative_humidity_min
      #ifdef CROPSYST_PROVIDED_ET
      ,&weather.ET_reference
      #endif
      );                                                   //110113
   root.assign_references
      (phenology_actual.ref_thermal_time_cumulative()
         // Warning: check with Claudio, do we really want reference biomass cumulative not actual?
      ,transpiration_actual.ref_water_stress_factor_yesterday());
}
//________________________________________________________________constructor__/
bool Crop::reinitialize_CO2_response()
{  return CO2_response && CO2_response->initialize();
}
//_110628______________________________________________________________________/
void Crop::know
(const Soil_layers_interface                 *_soil_layers
,modifiable_ Soil_hydrology_interface        *_soil_hydrology)
{
   soil_layers              =_soil_layers;
   soil_hydrology           =_soil_hydrology;
   biomass_reference    .know
      (&canopy_cover_reference
      ,&transpiration_unstressed
      ,&phenology_reference
      ,CO2_response);
   biomass_actual          .know(&canopy_cover_actual,&transpiration_actual,&phenology_actual,CO2_response);
   transpiration_actual    .know(_soil_layers,_soil_hydrology,&phenology_actual,&root);
   biomass_reference       .know(&canopy_cover_reference,&transpiration_unstressed,&phenology_reference,CO2_response);
   root.know(_soil_layers);
}
//________________________________________________________________constructor__/
bool Crop::start_day()                                             modification_
{  phenology_reference     .start_day();                                         //110406
   phenology_actual        .start_day();                                         //110406
   canopy_cover_reference  .start_day();                                         //110601
   canopy_cover_actual     .start_day();                                         //110601
   transpiration_unstressed.start_day();                                         //110601
   transpiration_actual    .start_day();                                         //110601
   // Need to make sure yields and biomass after harvest for output is cleared
   // otherwise this will be reported every day during dormancy.                 //110504
   yield_reference_kg_m2 = 0.0;                                                  //110504
   yield_actual_kg_m2    = 0.0;                                                  //110504
   biomass_after_harvest = 0.0;                                                  //110504
   return true;
}
//______________________________________________________________________________
// RLN Initialize not needed because all initialization is handled by construtors
// RLN SetEmergence function removed.
//     Instead C++ initializes in the constructor, Crop is instanciated on sowing
// RLN SetHarvest function from VB version removed.
//     Instead the crop is reinstanciated and all variables will be reinitialized
//_____________________________________________________________________________/
void Crop::develop()                                               modification_//DetermineDevelopment was Development()
{  phenology_reference.develop();                                                // Call CropDevelopment.CropDevelopment(True, CropParameters, CropState, WeatherState, TimeState, SoilState)
   phenology_actual.develop();                                                   // Call CropDevelopment.CropDevelopment(False, CropParameters, CropState, WeatherState, TimeState, SoilState)
}
//_____________________________________________________________________________/
void Crop::process_reference_crop()                                modification_//PerformPotentialProcesses was CalculatePotentialCropProcesses
{  // Calculate fraction of intercepted irradiance
   canopy_cover_reference.update_cover();                                        // Call CanopyCover.UnstressedCanopyCover(CropParameters, CropState, TimeState)
   // Calculate potential transpiration for potential crop growth
   transpiration_unstressed.update_potential();                                  // Call PotTransp.CalculatePotentialTranspiration(True, WeatherState, WeatherData, CropState, CropParameters, TimeState)
   // Calculate potential biomass growth
   biomass_reference.grow_potential();                                           //was CalculatePotentialGrowth(True, WeatherState, CropParameters, CropState, TimeState, WeatherData)
}
//_____________________________________________________________________________/
bool /*140810 Normal_crop_event_sequence*/ /*140314 Normal_crop_growth_stage*/ Crop::process()                           modification_// DailyCropProcesses
{  // RLN initialization no needed because C++ has constructors */               // If DAP = 0 Then Call Initialize(Crop_Name, CropParameters, CropState, TimeState, SimControl)
//140811obs   Normal_crop_event_sequence /*140314 Normal_crop_growth_stage*/ returned_growth_stage;                               //110620
   develop();                                                                    // Call Development(CropParameters, CropState, WeatherState, TimeState, SoilState)
   /* RLN  SetEmergence (which simply cleared variables) is not needed for C++*/ //   IF (CropState.CropEmerged = True And Waiting_For_Emergence = True ) THEN
   /* because the crop is reinstantiated on on planting event. */                //      Call SetEmergence(CropState, CropParameters, TimeState, SoilParameters)
                                                                                 //      SimControl.CropEmerged = True
                                                                                 //      Waiting_For_Emergence = False
                                                                                 //   END_IF
   nat32 days_since_start_of_emergence = phenology_actual.get_days_since_start_of(NGS_EMERGENCE);
   if (days_since_start_of_emergence == 0)
      {  biomass_reference.start();
         biomass_actual.start();
         if (!parameters.is_perennial())                                         //110419
            root.start();
      }
   if (phenology_actual.has_emerged()                                            // (CropState.CropEmerged)
       && (phenology_actual.days_since_start_of[NGS_VEGETATIVE_GROWTH] >= 1))                                                                           // AND  (DAP > CropState.DaysAfterPlantingAtEmergence) //110404
   {
      if (!phenology_actual.is_mature())                                         //121203  Not sure why I had is_manure() and not is_mature()
      {
         transpiration_unstressed.update_potential();                             // Call PotTransp.CalculatePotentialTranspiration(True, WeatherState, WeatherData, CropState, CropParameters, TimeState)
         transpiration_actual.update_potential();                                //   PotTransp.CalculatePotentialTranspiration(False, WeatherState, WeatherData, CropState, CropParameters, TimeState)
      }

   if (!phenology_actual.is_dormant()&&
      (!phenology_actual.is_mature() ||
       (parameters.is_fruit_tree() && phenology_actual.has_matured())))
   {  if (CO2_response)  CO2_response->update(); //131208renamed calc_responses();
      process_reference_crop();                                                  // CalculatePotentialCropProcesses(TimeState, CropParameters, CropState, CanopyCover, WeatherState, WeatherData, SoilState)
      // Calculate fraction of intercepted irradiance
      canopy_cover_actual.update_cover(transpiration_actual.get_leaf_water_potential_yesterday());  // CanopyCover.ActualCanopyCover(CropParameters, CropState, TimeState)
            // RLN warning, I think I may  the current leaf_water_potential
            // which would be yesterdays values because transpiration_actual has not be updated yest
      // Calculate potential transpiration of actual crop

      transpiration_actual.update_potential();                                   //   PotTransp.CalculatePotentialTranspiration(False, WeatherState, WeatherData, CropState, CropParameters, TimeState)
      // Calculate potential biomass growth
      bool has_above_ground_BM = biomass_actual.grow_potential() > 0.0;          //  biomass_actual.CalculatePotentialGrowth();
      root.update(days_since_start_of_emergence,has_above_ground_BM);                                                             // Root.CalculateRootGrowth(CropParameters, CropState, SoilParameters, TimeState)
      //'Calculate water uptake = actual transpiration
      if (canopy_cover_actual.
            get_global_solar_rad_intercepted_green_canopy_yesterday()
             > 0.0) // (CropState.FractionSolarRadInterceptedGreenCanopy(DAP - 1) > 0 )
         transpiration_actual.update_water_uptake();                             //  Call WatUpt.CalculateWaterUptake(CropParameters, CropState, SoilParameters, SoilState, WeatherState, TimeState, SimControl)
      /* RLN not needed because C++ version does not maintain daily arrays
      else
      {  CropState.ActualCropTranspiration(DAP) = 0.0;
         CropState.CumulativeActualCropTranspiration(DAP) = CropState.CumulativeActualCropTranspiration(DAP - 1)
      }
      */

      if (phenology_actual.get_days_since_start_of(NGS_FILLING) == 1)            //140317 RLN
         biomass_at_filling = biomass_actual.get_production_cum();               //140317 RLN
      biomass_actual.process();
      /* RLN moved to  biomass_actual.process()
       //'Calculate water limited growth (apply water stress effect if any)
       float64 water_limited_growth = biomass_actual.calc_water_limited_growth(); // Call ActBiom.CalculateWaterLimitedBiomassGrowth(CropState, TimeState)
       //'Calculate actual biomass growth
       float64 act_biomass_growth = biomass_actual.actual_growth(water_limited_growth); // Call ActBiom.CalculateActualBiomassGrowth(CropState, TimeState)
      */
   }  // End If
   }
   /* RLN is is not necessary to check both if mature and days after maturity */ // if (CropState.CropMature)
//140811    returned_growth_stage=                                                        // TimeState.DaysAfterPhysiologicalMaturity = TimeState.DaysAfterPhysiologicalMaturity + 1
//140811       (process_harvest())                                                        //110620
//140811       ? NGS_HARVEST                                                              //110620
//140811       : (Normal_crop_event_sequence)phenology_actual.get_growth_stage_sequence();//140317_110620
   if (phenology_actual.days_since_start_of[NGS_RESTART] == 1)                   //110406
   {  biomass_reference.start_season();
      biomass_actual.start_season();
      transpiration_actual.start_season();                                       //110624
      transpiration_unstressed.start_season();                                   //110624
      /*
      if (parameters.is_perennial())
      {
      canopy_cover_reference.start_season();                                           //110830
      canopy_cover_actual.start_season();                                              //110830
      }
      */
      root.start();                                                              //110624
      clippings_completed = 0;                                                   //110623
   }
   if (phenology_actual       .days_since_start_of[NGS_DORMANT_or_INACTIVE]==1)  //110406
   {   biomass_reference      .start_dormancy();                                 //110406
       biomass_actual         .start_dormancy();                                 //110406
       canopy_cover_reference .start_dormancy();                                 //110406
       canopy_cover_actual    .start_dormancy();                                 //110406
       clippings_completed = 0;                                                  //110623
   }
   return process_harvest();                                                                  //140810
}
//_____________________________________________________________________________/
Normal_crop_event_sequence Crop::get_crop_event_sequence()         modification_
{  return (Normal_crop_event_sequence)phenology_actual.get_growth_stage_sequence();
}
//_2014-08-10__________________________________________________________________/
bool Crop::process_harvest()                                       modification_
{
#if (VIC_CROPSYST_VERSION < 3)
//140811 moved to Crop_common
   bool harvested = false;                                                       //110620
   // Search for harvest of annual crops
   if (!parameters.is_perennial())                                               //   If Not CropParameters.PerennialCrop And CropState.CropMature Then
   {
      if (phenology_actual.is_mature())
      {
         if (phenology_actual.get_days_since_start_of(NGS_MATURITY) == 1)        //140317
            biomass_at_maturity = biomass_actual.get_production_cum();           //140317

         if (phenology_actual.get_days_since_start_of(NGS_MATURITY) == 10)       //       If TimeState.DaysAfterPhysiologicalMaturity = 10 Then
         {  // Harvest is assumed to occur 10 days after maturity.
            harvest_annual(true);
            harvested = true;                                                    //110620
            /* CropHarvested set by harvest_annual */                            //           CropState.CropHarvested = True
            /* DOYHarvestset by harvest_annual */                                //           CropState.DOYHarvest = TimeState.DayOfYear
            /* DaysAfterCropExistAtEndOfSeason either set by harvest_annual or not used in c++*/ //           CropState.DaysAfterCropExistAtEndOfSeason = TimeState.DaysAfterCropExist
            /* CropTerminated set by harvest_annual */                           //           SimControl.CropTerminated = True
            /* 110405 use function to check if season ended */                   //           SimControl.CropSeasonEnded = True 'XXX ROGER LATEST March 25 2011
            /* CropExist not appl to C++ */                                      //           SimControl.CropExist = False
            /* NewCropFound not appl to C++ */                                   //           SimControl.NewCropFound = False
            /* CropSeasonStartednot appl to C++ */                               //           SimControl.CropSeasonStarted = False
          }                                                                      //       End If
      }  /*End of search for harvest of annual crops*/                           //   End If 'End of search for harvest of annual crops

   }
   // Calculate yield, write outputs, and dispose crop at harvest for annual crops
   /* handled above and C++ has no output
   If Not CropParameters.PerennialCrop And CropState.CropHarvested Then          //If Not CropParameters.PerennialCrop And CropState.CropHarvested Then
          'Calculate yield
           Call DetermineAnnualCropYield(CropState, CropParameters, TimeState)   //        Call DetermineAnnualCropYield(CropState, CropParameters, TimeState)
           'Write outputs
           Call Output.WriteGrowingSeasonOutput(CropParameters, CropState, TimeState, SoilState) //        Call Output.WriteGrowingSeasonOutput(CropParameters, CropState, TimeState, SoilState)
           Call Output.WriteDailyGrowthOutput(Crop_Name, CropState, TimeState)   //        Call Output.WriteDailyGrowthOutput(Crop_Name, CropState, TimeState)
          'Clear TimeState and WeatherState crop-related properties
           Call SetHarvest(CropState, CropParameters, TimeState, WeatherState)   //        Call SetHarvest(CropState, CropParameters, TimeState, WeatherState)
   End If 'End of calculate yield, write outputs, and dispose crop at harvest for annual crops   //End If 'End of calculate yield, write outputs, and dispose crop at harvest for annual crops
   */
   // Calculate yield for tree fruit crops
   /* 110303 Moved to harvest_fruit_tree performed above
   If CropParameters.TreeFruitCrop And CropState.CropHarvested Then                 //If CropParameters.TreeFruitCrop And CropState.CropHarvested Then
        Call DetermineTreeFruitYield(CropParameters, CropState)                     //     Call DetermineTreeFruitYield(CropParameters, CropState)
        CropState.CropHarvested = False                                             //     CropState.CropHarvested = False
        CropState.PostHarvest = True                                                //     CropState.PostHarvest = True
        CropState.CumulativeFruitBiomassGrowth = 0                                  //     CropState.CumulativeFruitBiomassGrowth = 0
   End If 'end of calculate yield for tree fruit crops                              //End If 'end of calculate yield for tree fruit crops
   */

   // calculate yield for clipped perennials (not fruit trees)
   else //                                                                       //If CropParameters.PerennialCrop Then
   {  // special case of perennial where it has been a year in pseudodormancy
      // we break the pseudodormancy and get a yield.
      // Search for harvest of tree fruit crops
      if (parameters.is_fruit_tree() /*110422harvested_part_labeled.get() == fruit_part*/ )                 //   If CropParameters.TreeFruitCrop And CropState.CropMature Then
      {
         if (phenology_actual.is_mature() &&
             (phenology_actual.get_days_since_start_of(NGS_MATURITY) == 5))      //       If TimeState.DaysAfterPhysiologicalMaturity = 5 Then 'Harvest is assumed to occur 5 days after maturity
         {  // Harvest is assumed to occur 5 days after maturity
            harvest_fruit_tree();                                                //     Call DetermineTreeFruitYield(CropParameters, CropState)
            harvested = true;                                                    //110620
            /* moved to harvest_fruit_tree*/                                     //           CropState.CropHarvested = True
            /* moved to harvest_fruit_tree*/                                     //           CropState.DOYHarvest = TimeState.DayOfYear
         }                                                                       //       End If
      } /* End of search for harvest of tree fruit crops */                      //   End If 'End of search for harvest of tree fruit crops
      else
      if (clipped_automatically)                                                 //110426
      {  if (!mint_clipping)                                                     //110620
         {
            if (phenology_actual.perform_clipping_and_ensure_end_of_pseudodormancy())
            {  if (today.get_DOY() != 1)
               {  respond_to_clipping();
                  clipped_yield();                                                  //            Call DetermineClippedYield(CropParameters, CropState, TimeState)
                  harvested = true;                                              //110620
         }   }  }
         if (is_ready_for_clipping())
         {
            respond_to_clipping();
            clipped_yield();                                                  //            Call DetermineClippedYield(CropParameters, CropState, TimeState)
            harvested = true;                                                    //110620
         }
       }
   }  /* End of write output for perennial crops at the onset of dormancy */     //End If
#else
   bool harvested = Crop_common::process_harvest();                                      //140811_110620
#endif


   #ifdef USE_CHILL_REQUIREMENTS
      // Move this to dormancy
   // Calculate accumulation of chill hours for fruit trees
   If CropParameters.TreeFruitCrop And WeatherState.AccumulateChillHours Then    //If CropParameters.TreeFruitCrop And WeatherState.AccumulateChillHours Then
       Call Weather.DetermineChillHours(WeatherData, TimeState, WeatherState)    //    Call Weather.DetermineChillHours(WeatherData, TimeState, WeatherState)
       If WeatherState.CumulativeChillHours >= CropParameters.RequiredChillHours Then //    If WeatherState.CumulativeChillHours >= CropParameters.RequiredChillHours Then
           CropState.ChillRequirementSatisfied = True                            //        CropState.ChillRequirementSatisfied = True
           CropState.FirstDayChillRequirementSatisfied = True                    //        CropState.FirstDayChillRequirementSatisfied = True
           WeatherState.AccumulateChillHours = False                             //        WeatherState.AccumulateChillHours = False
       End If                                                                    //    End If
   End If 'End of calculate accumulation of chill hours for fruit trees          //End If 'End of calculate accumulation of chill hours for fruit trees
   #endif
   return harvested;
}
//_____________________________________________________________________________/
#include "crop/harvest_grain_asymptotic.c"
float64 Crop::calculate_yield
(float64 &return_yield_actual
,float64 &return_yield_reference)                                          const
{
   float64 harvest_index_unstressed    = harvest_parameters.index_unstressed;    //Unstressed_Harvest_Index = CropParameters.GetUnstressedHarvestIndex
   float64 harvest_index = harvest_index_unstressed;
   #ifdef DISABLED
   // Disabled in V.B. code probably obsolete
   float64 flowering_stress_factor     = 1.0 - transpiration_actual.get_water_stress_flowering_index();         // Flowering_Stress_Factor = 1 - CropState.FloweringWaterStressIndex
   #endif
   //'Determine maximum possible yield
   //RLN unused float64 cum_biomass_pot = CropState.PotentialCumulativeBiomass(CropState.DaysAfterPlantingAtMaturity)
   float64 cum_biomass_act    = biomass_actual.get_production_cum();             // Actual_Crop_Cumulative_Biomass = CropState.CumulativeBiomass(CropState.DaysAfterCropExistAtMaturity)
   //'Determine maximum yield as affected by sink damage  XXX OJO: Consider sources of damage other than water stress
                                                                                 // If Not CropParameters.GrainCrop Then  'Vegetative_Crop Then 110630
                                                                                 //    Sink_Damaged_Maximum_Harvest_Index = Unstressed_Harvest_Index
                                                                                 //    else
   float64 sink_damaged_maximum_harvest_index = harvest_index_unstressed;
                                              //    If Flowering_Stress_Factor < 0.3 Then
                      //    Sink_Damaged_Maximum_Harvest_Index = Unstressed_Harvest_Index * Flowering_Stress_Factor / 0.3
                                                 //    Else Sink_Damaged_Maximum_Harvest_Index = Unstressed_Harvest_Index
                                                                                 //endif
   if (parameters.harvested_part_labeled.get() == grain_part)                    //If Grain_Crop Then
   {
      /*140317  calculate_biomass_gain_based_harvest_index was replaced with calc_source_limited_harvest_index
      harvest_index = calculate_biomass_gain_based_harvest_index
         ( biomass_actual.get_produced_during_yield_formation()
         , biomass_actual.get_produced_before_yield_formation()
         , harvest_index_unstressed * 1.1
         , harvest_parameters.translocation_fraction_max);
      */

      /* 140317  NYI check with Claudio if needed
                  float64 stress_effect =
                     harvest_temperature_stress_factor
                     ? harvest_temperature_stress_factor->get_factor() : 1.0;

                  float64 sink_strength_based_harvest_index
                     =  harvest_ndx_unstressed * stress_effect;                  //130424
      */

      float64 biomass_gain_based_harvest_index =                     //110903
                     //130510 this is source limited harvest index
                  calc_source_limited_harvest_index                              //130513
                  (biomass_at_maturity//130510 biomass_produced_during_yield_formation                     //110903     //130510
                  ,biomass_at_filling                                            //131022
                  ,harvest_index_unstressed                                       //130510
                        // under certain conditions the harvest index can be slightly above (upto 10%) the parameter
                  ,(float64)harvest_parameters.translocation_fraction_max);     //120117


harvest_index = biomass_gain_based_harvest_index;  //RLN I think at this point this is similar to what we had before.


      /*140317  NYI check with Claudio if needed
      float64 source_limited_yield = biomass_gain_based_harvest_index* biomass_at_maturity;//130510
      float64 biomass_if_no_stress_during_yield_formation =
                  biomass_at_filling                                             //131022
                  //131022 biomass_at_flowering
                  / (1.0 - harvest_ndx_unstressed);//130510

               float64 sink_limited_yield = sink_strength_based_harvest_index * biomass_if_no_stress_during_yield_formation;//130510

               actual_yield = std::min<float64>(sink_limited_yield,source_limited_yield);  //130510
      */
  } // else  non-grain crop defaults to harvest_index_unstressed                 //       Else 'No grain crops
                                                                                 //   Harvest_Index = Unstressed_Harvest_Index
                                                                                 //End If
   float64 limited_harvest_index                                                 //Crop_Yield = Actual_Crop_Cumulative_Biomass * CalculateMinimum(Harvest_Index, Sink_Damaged_Maximum_Harvest_Index, 99.99)
      =  CORN::must_be_less_or_equal_to<float64>
            (CORN::must_be_less_or_equal_to<float64>
               (harvest_index, sink_damaged_maximum_harvest_index), 99.99);
   float64 yield = cum_biomass_act * limited_harvest_index;
   return_yield_actual  = yield;                                                 //CropState.CropYield = Crop_Yield 'This crop yield could be reduced if not sufficient N is available to meet the minimum grain N concentration (see CalculateCropNitrogenPartitioning)
   return_yield_reference                                                        // CropState.PotentialCropYield =  CropState.PotentialCumulativeBiomass(CropState.DaysAfterPlantingAtMaturity) * CropParameters. GetUnstressedHarvestIndex
      = biomass_reference.get_production_cum()                                   //CropState.PotentialCropYield = CropState.PotentialCumulativeBiomass(CropState.DaysAfterCropExistAtMaturity) * CropParameters.GetUnstressedHarvestIndex
       *harvest_index_unstressed;
   return return_yield_actual;
}                                                                                //End Sub
//_____________________________________________________________________________/
float64 Crop::harvest_fruit_tree()                                 modification_ //Sub DetermineTreeFruitYield(CropParameters As CropInputClass, CropState As CropStateClass)
{  nat16 harvest_doy = today.get_DOY();
   /* Fress_Mass_Fruit_Load dereferenced directly below */                       // Fress_Mass_Fruit_Load = CropParameters.FreshMassFruitLoad
   /* Fraction_Of_Solids dereferenced directly below */                          // Fraction_Of_Solids = CropParameters.FractionSolidsInFruits
   float64 yield_max_kg_m2 =       // Maximum_Yield = Fress_Mass_Fruit_Load * Fraction_Of_Solids / 10000 'converted from kg/ha to kg/m2
      per_ha_to_per_m2(parameters.fruit.max_fruit_load_kg_ha * parameters.fruit.fract_total_solids );
   yield_reference_kg_m2 = yield_max_kg_m2;                                      // CropState.PotentialCropYield = Maximum_Yield
   yield_actual_kg_m2 = std::min<float64>(yield_max_kg_m2, biomass_actual.get_production_fruit());   //CropState.CropYield = CalculateMinimum(Maximum_Yield, CropState.CumulativeFruitBiomassGrowth, 9999)
   biomass_after_harvest = biomass_actual.remove_yield( yield_actual_kg_m2);     //100827
   biomass_reference.remove_yield(yield_reference_kg_m2);                        //100827
   phenology_reference.know_harvest(harvest_doy);                                //110422
   phenology_actual.know_harvest(harvest_doy);                                   // CropState.CropHarvested = False
                                                                                 // CropState.PostHarvest = True
                                                                                 // CropState.DOYHarvest = TimeState.DayOfYear
   return yield_actual_kg_m2;
}
//_2011-03-07__________________________________________________________________/
float64 Crop::clipped_yield()                                      modification_ //DetermineClippedYield
{  yield_actual_kg_m2 = 0.0;
   if (clippings_completed)                                                      //110421
   {
   nat16 harvest_doy = today.get_DOY();                                          //DACE = TimeState.DaysAfterCropExist
                                                                                 //Actual_Crop_Cumulative_Biomass = CropState.CumulativeBiomass(DACE - 1)
                                                                                 //Potential_Crop_Cumulative_Biomass = CropState.PotentialCumulativeBiomass(DACE - 1)
/* WARNING NYI  110405
Actual_Crop_Cumulative_Biomass = CropState.CumulativeHarvestableBiomass 'XXX ROGER LATEST March 25 2011

Potential_Crop_Cumulative_Biomass = CropState.CumulativePotentialHarvestableBiomass 'XXX ROGER LATEST March 25 2011
*/
   float64 harvest_index = parameters.harvest.index_unstressed;                 //Harvest_Index = CropParameters.GetUnstressedHarvestIndex
   yield_reference_kg_m2 = biomass_reference.get_harvestable_biomass_cum() * harvest_index; //CropState.PotentialCropYield = Potential_Crop_Cumulative_Biomass * harvest_index
   yield_actual_kg_m2    = biomass_actual.get_harvestable_biomass_cum() * harvest_index;  //CropState.CropYield = Actual_Crop_Cumulative_Biomass * harvest_index
   //biomass_after_harvest = biomass_actual.remove_yield( yield_actual_kg_m2);     //100827
   //biomass_reference.remove_yield(yield_reference_kg_m2);                        //100827
   phenology_actual.know_harvest(harvest_doy);                                   // CropState.CropHarvested = False
      /* in VB code clippings_completed is not used but it is used here */
   }
   return yield_actual_kg_m2;
}                                                                                //End Sub
//_2011-03-07__________________________________________________________________/
float64 prescribed_clipping_mint_thermal_time[] =
{800
,1200
,1200
,1200
,1200
,0};
bool Crop::is_ready_for_clipping()                                         const //Sub FindClippingEvent(CropParameters As CropInputClass, CropState As CropStateClass, TimeState As TimeStateClass)
{  bool ready = false;
   /*  VIC has no facilities for specifying mangement operations    */           //    DACE = TimeState.DaysAfterCropExist
                                                                                 //    Current_Thermal_Time = CropState.CumulativeThermalTime(DACE)
   if (mint_clipping) // mint                                                    //If Schedule.PrescribedClipping Then 'XXX ROGER LATEST March 25 2011
   {                                                                             //    DACE = TimeState.DaysAfterCropExist
      float64 next_clipping_thermal_time
      =  prescribed_clipping_mint_thermal_time[clippings_completed];             //    For i = Clipping_Events_Completed To Schedule.NumberOfClippings
      if ((phenology_actual.get_thermal_time_cumulative() >= next_clipping_thermal_time) // If CropState.CumulativeThermalTime(DACE) >= Schedule.ClippingEvent(i, 1) Then
           && (clippings_completed < 2))
      {  ready = true;                                                           //          ClippingEvent = True
                                                                                 //             i = Schedule.NumberOfClippings + 1
                                                                                 //           Clipping_Events_Completed = Clipping_Events_Completed + 1
                                                                                 //           Else
                                                                                 //           ClippingEvent = False
      }                                                                          //        End If
                                                                                 //    Next i
   }
   else // other clipped crops
   {
      float64 thermal_time_at_the_end_of_canopy_growth                              //    Thermal_Time_At_The_End_Of_Canopy_Growth = CropParameters.ThermalTimeAtTheEndOfCanopyGrowth
         = parameters.phenology.max_canopy_deg_day/*renamed end_vegetative_growth_deg_day*/;
      float64 relative_thermal_time                                                 //    Relative_Thermal_Time
         = phenology_actual.get_thermal_time_cumulative()                           //       = Current_Thermal_Time
         / thermal_time_at_the_end_of_canopy_growth;                                //        / Thermal_Time_At_The_End_Of_Canopy_Growth
      //110421 WARNING currently this parameter is hard coded, it should be a management parameter
      float64 fractional_thermal_time_for_clipping
         = cumulative_thermal_time_to_trigger_automatic_clipping_events
          / thermal_time_at_the_end_of_canopy_growth;
//    Fractional_Thermal_Time_For_Clipping = Schedule.CumulativeThermalTimeToTriggerAutomaticClippingEvents / Thermal_Time_At_The_End_Of_Canopy_Growth //110325
       ready = relative_thermal_time >= fractional_thermal_time_for_clipping;   //    If Relative_Thermal_Time >= Fractional_Thermal_Time_For_Clipping Then //110325
                                                                                 //        ClippingEvent = True
                                                                                 //        Else
                                                                                 //        ClippingEvent = False
                                                                                 //    End If
   }
//End If 'XXX ROGER LATEST March 25 2011
/* NYI
If Schedule.PrescribedClipping Then 'XXX ROGER LATEST March 25 2011
    DACE = TimeState.DaysAfterCropExist
    For i = Clipping_Events_Completed To Schedule.NumberOfClippings
        If CropState.CumulativeThermalTime(DACE) >= Schedule.ClippingEvent(i, 1) Then
           ClippingEvent = True
           i = Schedule.NumberOfClippings + 1
           Clipping_Events_Completed = Clipping_Events_Completed + 1
           Else
           ClippingEvent = False
        End If
    Next i
End If
*/
   return ready;
}                                                                                //End Sub
//_2011-04-21__________________________________________________________________/
bool Crop::respond_to_clipping()                                   modification_
{ /* Apply clipping event, condition checked by caller */                        //If ClippingEvent Then
   clippings_completed += 1;
   return
      canopy_cover_reference.respond_to_clipping()
   && canopy_cover_actual   .respond_to_clipping()
   /* in V.B. the harvested indicated is only for output */                      //    CropState.DevelopmentStage(TimeState.DaysAfterCropExist) = "Harvested"
   && biomass_reference     .respond_to_clipping()
   && biomass_actual        .respond_to_clipping()
   && phenology_reference   .respond_to_clipping()
   && phenology_actual      .respond_to_clipping()
   && transpiration_actual  .respond_to_clipping();
}
//_2011-04-21__________________________________________________________________/
float64 Crop::harvest_annual(bool terminate)                       modification_
{  nat16 harvest_doy = today.get_DOY();                                          // CropState.DOYHarvest = TimeState.DayOfYear
   phenology_actual.know_harvest(harvest_doy);                                   // SimControl.CropHarvested = True
   calculate_yield(yield_actual_kg_m2,yield_reference_kg_m2);
   biomass_after_harvest = biomass_actual.remove_yield( yield_actual_kg_m2);     //100827
   biomass_reference.remove_yield(yield_reference_kg_m2);                        //100827
   return yield_actual_kg_m2;                                                    //   Call CalculateYield(CropState, CropParameters, TimeState)
      // RLN output recording will be done by unit test
      /*RLN no need to clear state because crop is reinstanciated.  */           //   Call SetHarvest(CropState, CropParameters, TimeState, WeatherState)
}
//_____________________________________________________________________________/
float64 Crop::get
      (nat32 variable_code
      /*NYI ,nat32 units_code*/ )                                          const
{  float64 value = 0.0;
   switch (variable_code)
   {
      case VC_leaf_area_index         : value = canopy_cover_actual.calc_leaf_area_index();  break;
      case VC_biomass_current_t_ha    : value = kg_m2_to_tonne_ha(biomass_actual.get_production_cum());  break;
      case VC_biomass_yield_t_ha      : value = kg_m2_to_tonne_ha(yield_actual_kg_m2);  break;
      case VC_biomass_after_harvest_t_ha : value = kg_m2_to_tonne_ha(biomass_after_harvest); break;
      case VC_water_stress_index : value = transpiration_actual.get_water_stress_index(); break;
      case VC_transpiration_actual_mm  : value = transpiration_actual.get_actual_mm(); break;                                 //121116
      case VC_ETmax_mm                 : value = transpiration_actual.get_crop_potential_ET() ; break;                        //121116
      case VC_canopy_cover_fraction_green : value = canopy_cover_actual.get_global_solar_rad_intercepted_green_canopy(); break;  //121116
      case VC_canopy_cover_fraction_total : value = canopy_cover_actual.get_global_solar_rad_intercepted_total_canopy(); break;  //121116
      case VC_root_depth_mm               : value = m_to_mm(root.get_depth()); break; //130725
     // case VC_root_depth_mm            : value = m_to_mm(root.get_depth()); break;//130725
   }
   return value;
}
/*140810 obsolete
      // VIC is now handling water uptake so if it still needs
      // to have uptake, then it needs to keep track.
      // CropSyst proper does not keep the uptake by layer.
//_____________________________________________________________________________/
float64 Crop::get_uptake_mm(nat8 layer)                                    const
{
   return transpiration_actual.get_uptake_mm(layer);
}
//_2014-05-08__________________________________________________________________/
*/
#ifdef CROPSYST_PROVIDED_SOIL_HYDROLOGY
// CropSyst in this case also handles computation of potential transpiration
#else
float64 Crop::update_ET_coef_full_shading()                       modification_
{  transpiration_unstressed.update_crop_ET_coef_full_shading();
   float64 actual_crop_ET_coef_full_shading =
   transpiration_actual   .update_crop_ET_coef_full_shading();
   return actual_crop_ET_coef_full_shading;
}
//_2012-11-29______________________________________update_ET_coef_full_shading_/
bool Crop::know_transpiration_max_mm(double transpiration_max_mm) modification_
{  transpiration_actual   .potential_mm = transpiration_max_mm;
   transpiration_unstressed.potential_mm
      = (transpiration_actual.potential_mm/canopy_cover_actual.get_global_solar_rad_intercepted_green_canopy())
                                      * canopy_cover_reference.get_global_solar_rad_intercepted_green_canopy();
   return true;
}
//_2012-11-29________________________________________know_transpiration_max_mm_/
#endif
#ifdef UNIT_TEST
void Crop::daily_growth_output(std::ofstream &daily_growth_file)
{
   //float32 precip = m_to_mm(weather_database.get_precipitation_m(precip_quality, today));
   CORN::Quality tmax_quality;         float32 Tmax = weather.air_temperature_max.get_Celcius();
   CORN::Quality tmin_quality;         float32 Tmin = weather.air_temperature_min.get_Celcius();
   CORN::Quality Srad_quality;         float32 Srad = weather.solar_radiation.get_MJ_m2();
   CORN::Quality RHmax_quality;        float32 RHmax= weather.relative_humidity_max.get();
   CORN::Quality RHmin_quality;        float32 RHmin= weather.relative_humidity_min.get();
   CORN::Quality windspeed_quality;    float32 windsp = weather.wind_speed.get_m_s();

   daily_growth_file
   <<       today.get_year()  // Year
   <<'\t'<< today.get_DOY()   // doy
   <<'\t'<< "crop"
   <<'\t'<< phenology_actual.get_days_since_start_of(NGS_EMERGENCE/*NGS_SOWING*/) //-1
   <<'\t'<< normal_crop_growth_stage_label_table[phenology_actual.get_growth_stage_index()]
   <<'\t'<< phenology_actual.thermal_time_cumulative
   <<'\t'<< (10 * biomass_reference.get_production_cum())
   <<'\t'<< (10 * biomass_actual.get_production_cum())
   #ifdef REFACTOR
   <<'\t'<< (*canopy_cover_reference.ref_global_solar_rad_intercepted_green_canopy())     // potential green canopy cover
   <<'\t'<< canopy_cover_actual.get_global_solar_rad_intercepted_green_canopy()     // actual green canopy cover
   <<'\t'<< (canopy_cover_actual.get_global_solar_rad_intercepted_total_canopy())    // actual total canopy cover
   #else
   <<'\t'<< (*canopy_cover_reference.ref_solar_rad_intercepted_fraction())     // potential green canopy cover
   <<'\t'<< canopy_cover_actual.get_solar_rad_intercepted_green_canopy()     // actual green canopy cover
   <<'\t'<< (*canopy_cover_actual.ref_solar_rad_intercepted_fraction())    // actual total canopy cover
   #endif
   <<'\t'<< root.get_depth()      // root depth
   <<'\t'<< transpiration_actual.get_water_stress_index()
   <<'\t'<< transpiration_actual.get_leaf_water_potential()
   <<'\t'<< 0.0 // 121217 obsoletetranspiration_actual.get_ET_ref_output_mm()
   <<'\t'<< transpiration_unstressed .get_ET_ref_CO2_output_mm()
   <<'\t'<< transpiration_unstressed .get_potential_mm()
   <<'\t'<< transpiration_actual    .get_potential_mm()
   <<'\t'<< transpiration_actual    .get_actual_mm()

   <<'\t'<< Tmax
   <<'\t'<< Tmin
   <<'\t'<< Srad
   <<'\t'<< RHmax
   <<'\t'<< RHmin
   <<'\t'<< windsp
   <<'\t';
      canopy_cover_actual.dump(daily_growth_file);

/*
   << "pot soil evap" <<'\t'
   << "act soil evap" <<'\t'
   << precipt<<'\t'
   <<Irrigation<<'\t'
   <<Water<<'\t'
   <<Water<<'\t'
   <<Water
*/
}
//_____________________________________________________________________________/
void Crop::daily_growth_header(std::ofstream &daily_growth_file,int line)
{
   switch (line)
   {  case 0 : daily_growth_file << "Year\tDOY\tCrop\tDays\tGrowth\tThermal\tPotential\tActual\tPotential\tActual\tActual\tRoot\tWater\tLeaf Water\tBaseline CO2\tElevated CO2\tTransp of\tActual Crop\tActual Crop";  break;
      case 1 : daily_growth_file << "\t\tName\tAfter\tStage\tTime\tBiomass\tBiomass\tGreen Canopy\tGreen Canopy\tTotal Canopy\tDepth\tStress\tPotential\tETo\tETo\tPot Crop\tPot Transp\tTranspiration\tTmax\tTmin\tSrad\tRHmax\tRHmin\tWS"   ; break;// Evaporation\tEvaporation\t\t\tDrainage\tRunoff\tBalance"
      case 2 : daily_growth_file << "\t\t\tEmergence\t\t(C-days)\t(t/ha)\t(t/ha)\tCover\tCover\tCover\t(m)\tIndex\t(J/kg)\t(mm/day)\t(mm/day)\t(mm/day)\t(mm/day)\t(mm/day)\t°C\t°C\tMJ/m2\t%\t%\tm/s" ; break;// (mm)\t(mm)\t(mm)\t(mm)\t(mm)\t(mm)" << std::endl
   }
   canopy_cover_actual.dump_headers(daily_growth_file);

}
#endif
//_____________________________________________________________________________/
float64 Crop::get_root_depth() const
{  return root.get_depth();
}
//_2001-08-28__________________________________________________________________/
bool Crop::perform_clipping_and_ensure_end_of_pseudodormancy()     modification_
{  return phenology_actual.perform_clipping_and_ensure_end_of_pseudodormancy();
}
//_2014-08-11__________________________________________________________________/
// unassigned_reference is a temporary place holder for member references that are assigned
// after an object is instanciated.
float64 unassigned_reference=999999.9;
} // namespace VIC_crop
//_____________________________________________________________________________/
/*

No yield for vegetables, berries and crops harvested for seed
(Do not know what yields would be, so simulating for water use only)

mint only clipped twice

report yield as 0 for surrogate crops

*/

#endif
