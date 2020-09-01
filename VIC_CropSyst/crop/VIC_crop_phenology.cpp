#if (VIC_CROPSYST_VERSION >= 3)
#  error This class is obsolete in version 3 remove this file from your project
/*
Version 3 now uses VIC_CropSyst_proper_crop
where the Crop class is based on CropSyst proper.
*/
#else
#include "VIC_crop_phenology.h"
//100513  RLN We only need one thermal time without the temperature increase.
#ifndef compareHPP
#  include "corn/math/compare.hpp"
#endif
#  include "corn/math/statistical/stats.h"
#ifndef qualityHPP
#  include "corn/quality.hpp"
#endif
//150319 #include "VIC_crop_canopy_growth.h"
#include "VIC_crop_param.h"
#include "weather/parameter/WP_air_temperature.h"
#include "weather/parameter/WP_vapor_pressure_deficit.h"
//______________________________________________________________________________
namespace VIC_crop
{
   extern float64 unassigned_reference;
//_2011-03-02___________________________________________________________________
static const nat8 period_length = 14;                                            // Period_Length

class Dormancy
{
public:
   const CORN::Date &today;
   bool  first_day;                                                              // FirstDayOfDormancy or FirstDayOfPseudoDormancy when psuedo dormant
   bool  dormant;
   bool  pseudodormant;                                                          //
   bool  season_started;
// ready is not need in C++ we are ready for dormancy when the Dormancy object is created
// ready does not mean started
   bool  quit_search_for_start;                                                  // Quit_Searching_For_Dormancy
   bool  ready_to_break;
   CORN::Statistical_data_set average_temperature_window;
   nat8  searching_days;                                                         // Number_Of_Days_Searching_For_Dormancy
   CORN::DOY DOYToStartSearchingForBreakOfDormancy;                              // DOYToStartSearchingForBreakOfDormancy
   Phenology_common &phenology;
   const Inactive_period_parameters &inactive_period_parameter;
public:
   inline Dormancy
      (const CORN::Date &_today
      ,const Inactive_period_parameters &_inactive_period
      ,Phenology_common &_phenology,bool _ready_obsolete)
      : today(_today)
      , first_day       (false)      // initial parameters may need to be passed
      , pseudodormant   (false)
      , dormant         (false)
      , season_started  (false)
      , quit_search_for_start(false)  // probably false
      , ready_to_break  (false)
      , average_temperature_window()// will have no more than period_length elements
      , searching_days  (0)
      , DOYToStartSearchingForBreakOfDormancy(0)
      , phenology       (_phenology)
      , inactive_period_parameter (_inactive_period)
      {}
   virtual bool prolong()                                          modification_ = 0;
   virtual float64 get_thermal_time_to_start_growing_season()              const = 0;
   void know_average_air_temperature(float64 air_temp_avg)         modification_;
   virtual bool start(bool pseudo)                                 modification_;
   inline bool is_first_day_of_pseudo_dormancy()                           const { return first_day && pseudodormant;}
   bool start_when_ready()                                         modification_;
};
//_2011-03-02___________________________________________________________________
class Dormancy_herbacious
: public Dormancy
{
   nat16 duration;   //days                                                      // DormancyDuration
//   bool  ready_to_break;                                                         // ReadyToBreakDormancy
public:
   inline Dormancy_herbacious
      (const CORN::Date &_today
      ,const Inactive_period_parameters  &_inactive_period
      ,Phenology_common &_phenology
      ,bool _ready)
      : Dormancy(_today,_inactive_period,_phenology,_ready)
      {}
   virtual float64 get_thermal_time_to_start_growing_season()              const;
   virtual bool start(bool pseudo)                                 modification_;
   virtual bool prolong()                                          modification_;
};
//_2011-03-02___________________________________________________________________
class Dormancy_fruit_tree
: public Dormancy
{
private:
   const Dormancy_parameters &dormancy_parameters;
   #ifdef USE_CHILL_REQUIREMENTS
   bool chill_requirement_satisfied;                                             // ChillRequirementSatisfied
   #endif
public:
   inline Dormancy_fruit_tree
      (const CORN::Date &_today
      ,const Inactive_period_parameters  &_inactive_period
      ,const Dormancy_parameters         &_dormancy_parameters
      ,Phenology_common &_phenology
      ,bool _ready)
      : Dormancy(_today,_inactive_period,_phenology,_ready)
      ,dormancy_parameters(_dormancy_parameters)
      #ifdef USE_CHILL_REQUIREMENTS
      , chill_requirment_satisfied(false)
      #endif
      {}
   virtual float64 get_thermal_time_to_start_growing_season()              const;
   virtual bool start(bool pseudo)                                 modification_;
   virtual bool prolong()                                          modification_;
};
//_2011-03-02___________________________________________________________________
float64 Dormancy_herbacious::get_thermal_time_to_start_growing_season()    const
{  return phenology.phenology_parameter.emergence_deg_day;                       // Thermal_Time_To_Start_Growing_Season = CropParameters.GetThermalTimeForEmergence
}
//_2011-03-02___________________________________________________________________
float64 Dormancy_fruit_tree::get_thermal_time_to_start_growing_season()    const
{
   #ifdef USE_CHILL_REQUIREMENTS
   return  chill_requirment_satisfied                                            //               If CropState.ChillRequirementSatisfied Then
                                                                                 //                   Thermal_Time_To_Start_Growing_Season
      ? parameters.deg_day_bud_break_chill_req_sat                               //                     = CropParameters.ThermalTimeForBudbreakIfChillReached
                                                                                 //                     'this is thermal time for budbreak in the case of trees
                                                                                 //                   Else
                                                                                 //                   Thermal_Time_To_Start_Growing_Season
      : deg_day_bud_break_chill_req_not_sat;                                     //                     = CropParameters.ThermalTimeForBudbreakIfChillNotReached
                                                                                 //               End If
   #else
   return phenology.phenology_parameter.emergence_deg_day;
   #endif
}
//_2011-03-02___________________________________________________________________
#ifdef NYN
bool Dormancy::prolong()                                           modification_
{
   // Set thermal time required to start growing season for annual and fruit tree crops.
   float64 Thermal_Time_To_Start_Growing_Season                                  //       If CropParameters.PerennialCrop Then
      = get_thermal_time_to_start_growing_season();                              //           If CropParameters.TreeFruitCrop Then
                                                                                 //               If CropState.ChillRequirementSatisfied Then
                                                                                 //                If CropState.FirstDayChillRequirementSatisfied Then 'XXX ROGER LATEST Jan 11 2010
                                                                                 //                    Thermal_Time_To_Start_Growing_Season = CropParameters.ThermalTimeForBudbreakIfChillReached 'this is thermal time for budbreak in the case of trees
                                                                                 //                    CropState.FirstDayChillRequirementSatisfied = False 'XXX ROGER LATEST Jan 11 2010
                                                                                 //                End If

                                                                                 //110111obs                   Thermal_Time_To_Start_Growing_Season
                                                                                 //110111obs                     = CropParameters.ThermalTimeForBudbreakIfChillReached
                                                                                 //110111obs                     'this is thermal time for budbreak in the case of trees
                                                                                 //                   Else
                                                                                 //                   Thermal_Time_To_Start_Growing_Season
                                                                                 //                     = CropParameters.ThermalTimeForBudbreakIfChillNotReached
                                                                                 //               End If
                                                                                 //           End If
                                                                                 //           Else
                                                                                 //           Thermal_Time_To_Start_Growing_Season = CropParameters.GetThermalTimeForEmergence
                                                                                 //       End If 'End of set thermal time required to start growing season
   // Search for the start of growing season                                     //       'Search for the start of growing season
}
#endif
//_2011-03-02___________________________________________________________________
void Dormancy::know_average_air_temperature(float64 air_temp_avg)  modification_
{  // This kind of implements    CalculateTimeWindowMeanTemperature.
   average_temperature_window.append(air_temp_avg);
   while (average_temperature_window.get_count() > 14) // We only what the last two weeks.
       average_temperature_window.remove_at(0);         //LML 141112 replaced with remove_at()
}
//_2011-03-03___________________________________________________________________
bool Dormancy_herbacious::prolong()                                modification_
{
   bool prolong = true;                                                          //110421
   // BREAKING OUT OF DORMANCY perennial crop, but not tree fruit crop
   if (dormant)
   {
      /* Will always be a perennial at this point since there is dormancy. */    //    If CropParameters.PerennialCrop And
      if ((today.get_DOY() == DOYToStartSearchingForBreakOfDormancy)             //       TimeState.DayOfYear = CropState.DOYToStartSearchingForBreakOfDormancy
           /* && dormant checked above */)                                       //       and CropState.CropDormant Then
         ready_to_break = true;                                                  //          CropState.ReadyToBreakDormancy = True
                                                                                 //    End If
      /* Period_Length is static const */                                        //    Period_Length = 14
      if (ready_to_break && (phenology.days_since_start_of[NGS_SOWING] >= period_length)   //    If CropState.ReadyToBreakDormancy And SimControl.NumberOfSimulationDays >= Period_Length _
          /* && dormant checked above */ )                                       //                   And CropState.CropDormant = True Then
      {  float64 period_mean_temperature
            = average_temperature_window.get_mean();                    //       Call Weather.DetermineTimeWindowMeanTemperature(Period_Length, Period_Mean_Temperature, TimeState, WeatherData)
         if (period_mean_temperature > inactive_period_parameter.inducement_temperature)   //       If Period_Mean_Temperature > CropParameters.TemperatureForDormancy Then
         {  /* moved to phenology::start_season */                               //          SimControl.CropSeasonStarted = True
            /* 110405 now using function to check season ended */                //          SimControl.CropSeasonEnded = False 'XXX ROGER LATEST March 28 2011
            /* use get_days_since_start_of[NGS_RESTART] */                       //          CropState.DaysInGrowingSeason = 0
            dormant = false;                                                     //          CropState.CropDormant = False
            /* 110405 use accessor */                                            //          CropState.CropMature = False 'XXX ROGER LATEST March 28 2011

            /* use get_days_since_start_of[???] */                               //          CropState.DaysAfterCropExistAtStartOfSeason = TimeState.DaysAfterCropExist
            /* VB only used for output */                                        //          CropState.DOYStartGrowingSeason = TimeState.DayOfYear

            /* Warning NYI (not sure how used) 110405*/                          //          If CropState.CurrentYear(DACE) <> CropState.YearPlanting Then CropState.YearPlanting = CropState.YearPlanting + 1 'XXX ROGER LATEST March 25 2011

            /* quit_search_for_start = false;     not needed because deleting */ //          Quit_Searching_For_Dormancy = False
            /* ready_to_break = false;            not needed because deleting */ //          CropState.ReadyToBreakDormancy = False
            /* ready = false;                     not needed because deleting */ //          CropState.ReadyForDormancy = False
            /* searching_days = 0;                not needed because deleting */ //          Number_Of_Days_Searching_For_Dormancy = 0
            prolong = false;                                                     //110421
         }                                                                       //       End If
      }                                                                          //    End If
   }
   /* The following pertaining to annual crops it not applicable because
      the start of season is emergence
              Else                                                               //           Else 'annual crop
              {
   float64 Thermal_Time_To_Start_Growing_Season
      = get_thermal_time_to_start_growing_season();

               If Current_Cumulative_Thermal_Time >= Thermal_Time_To_Start_Growing_Season Then                                      //               If Current_Cumulative_Thermal_Time >= Thermal_Time_To_Start_Growing_Season Then
                   SimControl.CropSeasonStarted = True                                                                              //                   SimControl.CropSeasonStarted = True
                   SimControl.CropSeasonEnded = False 'XXX ROGER LATEST March 25 2011
                   WeatherState.CumulativeChillHours = 0 'XXX ROGER LATEST Jan 11 2010
                   CropState.DaysInGrowingSeason = 0 'XXX ROGER LATEST Jan 11 2010                                                  //                   CropState.DaysInGrowingSeason = 0 'XXX ROGER LATEST Jan 11 2010
                   CropState.CropEmerged = True                                                                                     //                   CropState.CropEmerged = True
                CropState.DOYEmergence = TimeState.DayOfYear                                                                        //                CropState.DOYEmergence = TimeState.DayOfYear
                   CropState.DaysAfterCropExistAtStartOfSeason = TimeState.DaysAfterCropExist                                       //                   CropState.DaysAfterCropExistAtStartOfSeason = TimeState.DaysAfterCropExist
                   CropState.DOYStartGrowingSeason = TimeState.DayOfYear                                                            //                   CropState.DOYStartGrowingSeason = TimeState.DayOfYear
                   If CropState.CurrentYear(DACE) <> CropState.YearPlanting Then CropState.YearPlanting = CropState.YearPlanting + 1 'XXX ROGER LATEST March 25 2011

               End If                                                                                                               //               End If
               }
   */
   return prolong;
}
//_2011-03-02___________________________________________________________________
bool Dormancy_fruit_tree::prolong()                                modification_
{  bool prolonging = true;                                                       // If CropParameters.TreeFruitCrop Then ' Tree fruit crop                                                                    //          If CropParameters.TreeFruitCrop Then ' Tree fruit crop
   float64 Thermal_Time_To_Start_Growing_Season                                  //    If CropParameters.PerennialCrop Then
      = get_thermal_time_to_start_growing_season();
   if ((phenology.get_thermal_time_cumulative() >= phenology.phenology_parameter.emergence_deg_day)//If Current_Cumulative_Thermal_Time >= Thermal_Time_To_Start_Growing_Season Then
       &&(phenology.days_since_start_of[NGS_DORMANT_or_INACTIVE] >  inactive_period_parameter.min_duration))
   {  /* moved to phenology::start_season */                                     //          SimControl.CropSeasonStarted = True
      /* use get_days_since_start_of[NGS_RESTART] */                             //          CropState.DaysInGrowingSeason = 0 'XXX ROGER LATEST Jan 11 2010
      dormant = false;                                                           //          CropState.CropDormant = False
      /* moved to phenology::start_season */                                     //          CropState.CropMature = False 'XXX ROGER LATEST Jan 11 2010
      prolonging = false;                                                        //          CropState.DormancyBroken = True
      /* FirstDayOfDormancyBroken is same as  days_since_start_of[NGS_restart]*/ //          CropState.FirstDayOfDormancyBroken = True
      /* use get_days_since_start_of[???] */                                     //          CropState.DaysAfterCropExistAtStartOfSeason = TimeState.DaysAfterCropExist
      /* VB only used for output */                                              //          CropState.DOYStartGrowingSeason = TimeState.DayOfYear
      /* moved to phenology::start_season */                                     //          CropState.CumulativeThermalTime(DACE) = 0
      /* moved to phenology::start_season */                                     //          Current_Cumulative_Thermal_Time = CropState.CumulativeThermalTime(DACE)
      /* quit_search_for_start = false;           not needed because deleting */ //          Quit_Searching_For_Dormancy = False 'XXX ROGER LATEST Jan 11 2010
      /* searching_days = 0;                      not needed because deleting */ //          Number_Of_Days_Searching_For_Dormancy = 0 'XXX ROGER LATEST Jan 11 2010
   }                                                                             //       End If
   return prolonging;
}
//_2011-03-02___________________________________________________________________
bool Phenology_common::perform_clipping_and_ensure_end_of_pseudodormancy()              modification_
{  bool ensured_pseudodormant_clipping = false;
   bool ensured_pseudodormant_end =
         (dormancy && dormancy->pseudodormant
          && days_since_start_of[NGS_RESTART] == 365);
   int days_since_start_of_dormancy = days_since_start_of[NGS_DORMANT_or_INACTIVE];
      if ((days_since_start_of_dormancy==1)                                      //    If (CropState.FirstDayOfDormancy Or CropState.FirstDayOfPseudoDormancy) _
          || ensured_pseudodormant_end )                                         //        Or (CropState.PseudoDormant    And CropState.DaysInGrowingSeason = 365) Then
      {  // Calculate yield of clipped perennial biomass crops
         if (!is_fruit_tree)                                                      //        If Not CropParameters.TreeFruitCrop Then
             ensured_pseudodormant_clipping = true; /* caller will determine yield */   //            Call DetermineClippedYield(CropParameters, CropState, TimeState)
                                                                                 //        End If
         // Calculate Soil water profile at the end of the growing season
         /* soil water handled by VIC_crop_soil_water */                         //        CropState.FinalSoilProfileWater         = Balance.CalculateSoilWaterProfile(SoilState, TimeState, SoilParameters)
         /* DaysAfterCropExistAtEndOfSeason not used in C++ version
         if (ensured_pseudodormant_end)                                          //        If CropState.PseudoDormant And CropState.DaysInGrowingSeason = 365 Then
            CropState.DaysAfterCropExistAtEndOfSeason = TimeState.DaysAfterCropExist  //            CropState.DaysAfterCropExistAtEndOfSeason = TimeState.DaysAfterCropExist
         */                                                                      //    End If
         /* No output writing here in C++ */                                     //        Call Output.WriteGrowingSeasonOutput(CropParameters, CropState, TimeState, SoilState)
                                                                                 //        Call Output.WriteDailyGrowthOutput(Crop_Name, CropState, TimeState)
         // Reset days after crop exist to 1 due to dormancy.
         /* C++ uses days_since_start_of */                                      //        TimeState.DaysAfterCropExist = 1
         if (!is_fruit_tree)                                                     //        If Not CropParameters.TreeFruitCrop Then
         {  dormancy->first_day = true;                                          //            CropState.FirstDayOfDormancy = False
            dormancy->pseudodormant = false;                                     //            CropState.FirstDayOfPseudoDormancy = False
            reset();                                                             //            CropState.DaysAfterCropExistAtStartOfSeason = 1  110506
            days_since_start_of[NGS_DORMANT_or_INACTIVE] = 1;                    //110506
         }                                                                      //        End If
         /*
         if (is_fruit_tree)                                                      //        If Not CropParameters.TreeFruitCrop Then
         {  delete dormancy;                                                     //            CropState.FirstDayOfDormancy = False
            dormancy = 0;                                                        //            CropState.FirstDayOfPseudoDormancy = False
            start_season();                                                      //            CropState.DaysAfterCropExistAtStartOfSeason = 1
         }                                                                      //        End If
         */
       }                                                                        //    End If
   return ensured_pseudodormant_clipping;
}
//_2011-03-07_______________perform_clipping_and_ensure_end_of_pseudodormancy__/
bool Phenology_common::respond_to_clipping()                       modification_
{                                                                                //If Management.ClippingEvent Then
   thermal_time_cumulative = thermal_time_today;                                 //CropState.CumulativeThermalTime(DACE) = Degree_Days
                                                                                 //Management.ClippingEvent = False
   return true;
}
//_2011-03-07______________________________________________respond_to_clipping_/
void Phenology_common::dormancy_processing()                       modification_
{
   //float64 curr_thermal_time_cumulative = CropState.CumulativeThermalTime(DACE) //Current_Cumulative_Thermal_Time = CropState.CumulativeThermalTime(DACE)
   //Check for start of growing season
   if (dormancy)                                                                 //If Not SimControl.CropSeasonStarted Then
   {  dormancy->know_average_air_temperature(air_Tavg);
      if (!dormancy->prolong())
         start_season();
   }                                                                             //V.B. Version had search for harvest here, I have these in VIC_crop
   else  /* not dormant */                                                       //    Not CropState.CropDormant
   {  // Check for BEGINNING OF DORMANCY of perennial crops
      if /*perennial checked by caller */                                        // If CropParameters.PerennialCrop
         (today.get_DOY() == perennial_inactive_period_parameter->start_DOY)     //    And TimeState.DayOfYear = CropParameters.DOYStartDormancy
      {
         dormancy =
            (fruit_tree_dormancy_parameter!=NULL)                                //              CropState.ReadyForDormancy = True
            ? dynamic_cast<Dormancy *>(new Dormancy_fruit_tree(today,*perennial_inactive_period_parameter,*fruit_tree_dormancy_parameter,*this,false/*ready for dormancy*/))
            : dynamic_cast<Dormancy *>(new Dormancy_herbacious(today,*perennial_inactive_period_parameter,*this,false/*ready for dormancy*/));
         dormancy->know_average_air_temperature(air_Tavg);
         /*  It shouldn't be necessary to know_average_air_temperature
             for the first day since it should be many days more than 14 days
             before we need to start looking for the end of dormancy
         */
      }
   }                                                                             //    End If
   if (dormancy && ! dormancy->dormant)
      if (dormancy->start_when_ready())
      {
         days_since_start_of[NGS_VEGETATIVE_GROWTH] = 0;
         thermal_time_cumulative = 0;
         thermal_time_cumulative_yesterday = 0;
         thermal_time_today = 0;
      }
}
//_2011-03-02__________________________________________________________________/
bool Dormancy::start_when_ready()                                  modification_
{  bool started = false;
   {
      /* Period_Length is here a static const */                                 // Period_Length = 14
      if (         /*perennial checked by caller */                              // If CropParameters.PerennialCrop
            (phenology.days_since_start_of[NGS_SOWING] >= period_length)         //    And SimControl.NumberOfSimulationDays >= Period_Length
             // The start of sowing for perennials is essentially the number of days since the start of the simulation
//110427          && ready                                                               //    And CropState.ReadyForDormancy
          && !quit_search_for_start)                                             //    And Not Quit_Searching_For_Dormancy Then
      {  float64 period_mean_temperature                                         //    Call Weather.DetermineTimeWindowMeanTemperature
            = average_temperature_window.get_mean();                             //       (Period_Length, Period_Mean_Temperature, TimeState, WeatherData)
         if (period_mean_temperature < inactive_period_parameter.inducement_temperature)    //    If Period_Mean_Temperature < CropParameters.TemperatureForDormancy Then
            started = start(false);
         searching_days += 1;                                                    //          Number_Of_Days_Searching_For_Dormancy = Number_Of_Days_Searching_For_Dormancy + 1
         if (searching_days > 120)                                               //          If Number_Of_Days_Searching_For_Dormancy > 120 Then
            started = start(true);
      }  /*End of check for beginning of dormancy of perennial crops */          // End If
   }
   return started;
}
//_2011-03-02__________________________________________________________________/
bool Dormancy::start(bool _pseudo)                                 modification_
{
   dormant = true;                                                               //110406
   phenology.growth_stage = NGS_DORMANT_or_INACTIVE;
   pseudodormant = _pseudo;
   first_day = true;
   if (pseudodormant)                                                            // If Number_Of_Days_Searching_For_Dormancy > 120 Then
   {
      /* pseudodormant set above */                                              //              CropState.PseudoDormant = True
//110427       ready = false;                                                             //              CropState.ReadyForDormancy = False
      /* DaysAfterCropExistAtEndOfSeason used only in V.B. for output */         //              CropState.DaysAfterCropExistAtEndOfSeason = TimeState.DaysAfterCropExist
      /* in C++ checking for first_day and pseudodormant */                      //              CropState.FirstDayOfPseudoDormancy = True
      /* not sure if needed CropState.DaysInGrowingSeason = 0 */                                                                     //              CropState.DaysInGrowingSeason = 0
      searching_days = 0;                                                        //              Number_Of_Days_Searching_For_Dormancy = 0
      quit_search_for_start = true;                                              //              Quit_Searching_For_Dormancy = True
   }                                                                             //    End If
   else
   {
      /* crop season started is indicated by no dormancy */                      //       SimControl.CropSeasonStarted = False
      /* 110405 using accessor functions */                                      //       SimControl.CropSeasonEnded = True 'XXX ROGER LATEST March 25 2011
      /* C++ uses growths_stages */                                              //       CropState.CropHarvested = False
      /* C++ uses growths_stages */                                              //       CropState.CropDormant = True
      /* C++ uses growths_stages */                                              //       CropState.PostHarvest = False
      /* C++ uses growths_stages */                                              //       CropState.CropMature = False
      /* pseudodormant set above */                                              //       CropState.PseudoDormant = False
         //110303 broken = false;                                                            //       CropState.DormancyBroken = False
      /* first_day set above   */                                                //       CropState.FirstDayOfDormancy = True
      /* DaysAfterCropExistAtEndOfSeason unused */                               //       CropState.DaysAfterCropExistAtEndOfSeason = TimeState.DaysAfterCropExist
      phenology.days_since_start_of[NGS_MATURITY] = 0;                           //       TimeState.DaysAfterPhysiologicalMaturity = 0
//      phenology.days_since_start_of[NGS_DORMANT_or_INACTIVE] = 1;                //110406
      quit_search_for_start = true;                                              //       Quit_Searching_For_Dormancy = True
      ready_to_break = false;                                                    //       CropState.ReadyToBreakDormancy = False
//110427      ready = false;                                                             //       CropState.ReadyForDormancy = False

      /* done in biomass::start_dormancy() */                                    //      CropState.CumulativeBiomass(DACE) = 0 'XXX ROGER LATEST March 25 2011
      /* done in biomass::start_dormancy() */                                    //      CropState.PotentialCumulativeBiomass(DACE) = 0 'XXX ROGER LATEST March 25 2011
      /* done in biomass::start_dormancy() */                                    //      CropState.FractionSolarRadInterceptedGreenCanopy(DACE) = 0 'XXX ROGER LATEST March 25 2011
      /* done in biomass::start_dormancy() */                                    //      CropState.PotentialFractionSolarRadiationIntercepted(DACE) = 0 'XXX ROGER LATEST March 25 2011

      // Determine when to start looking for break of dormancy of perennials other than fruit trees
      // is done by  Dormancy_herbacious::start()
   }
   return true;
}
//_2011-03-03__________________________________________________________________/
bool Dormancy_herbacious::start(bool _pseudo)                      modification_
{  bool started = Dormancy::start(_pseudo);
   if ((inactive_period_parameter.end_DOY>0) && (inactive_period_parameter.end_DOY!= 365))
   {  // This is added by RLN to allow user to override the day to start looking for end of dormancy in the Crop parameter file.
       DOYToStartSearchingForBreakOfDormancy = inactive_period_parameter.end_DOY;
   } else
   {  if (today.get_DOY() == 1)                                                  //RLN for the first year, we can start searching right away
      {  DOYToStartSearchingForBreakOfDormancy = 2;
         //WARNING this is only for VIC and only for northern hemisphere
      } else
      {  DOYToStartSearchingForBreakOfDormancy = today.get_DOY()                    //DOY_To_Start_Searching_For_Break_Of_Dormancy = TimeState.DayOfYear + CropParameters.DormancyDuration   //              DOY_To_Start_Searching_For_Break_Of_Dormancy = TimeState.DayOfYear + CropParameters.DormancyDuration
             +inactive_period_parameter.min_duration;
         if (DOYToStartSearchingForBreakOfDormancy > 365)                           //If DOY_To_Start_Searching_For_Break_Of_Dormancy > 365 Then                                             //              If DOY_To_Start_Searching_For_Break_Of_Dormancy > 365 Then 'XXX ROGER LATEST Jan 11 2010
             DOYToStartSearchingForBreakOfDormancy -= 365;                          //DOY_To_Start_Searching_For_Break_Of_Dormancy = DOY_To_Start_Searching_For_Break_Of_Dormancy - 365  //                  DOY_To_Start_Searching_For_Break_Of_Dormancy = DOY_To_Start_Searching_For_Break_Of_Dormancy - 365
      }
   }                                                                             //End If
   // Determine when to start looking for break of dormancy of perennials other than fruit trees
   return started;
}
//_2011-03-03__________________________________________________________________/
bool Dormancy_fruit_tree::start(bool _pseudo)                      modification_
{  bool started = Dormancy::start(_pseudo);
   /*this is TreeFruitCrop condition*/                                           // if (CropParameters.TreeFruitCrop)                                                                   //              If CropParameters.TreeFruitCrop Then
   #ifdef USE_CHILL_REQUIREMENTS
   {   chill_requirement_satisfied = false;                                      //                  CropState.ChillRequirementSatisfied = False
       WeatherState.AccumulateChillHours = true;                                 //                  WeatherState.AccumulateChillHours = True
   }                                                                             //              End If
   #endif
   return started;
}
//_2011-03-03__________________________________________________________________/
Phenology_common::Phenology_common
(const Phenology_parameters      &_phenology_parameter
,const Thermal_time_parameters   &_thermal_time_parameter
,const Season_correction         &_season_correction
,bool                             _is_root_crop
,const CORN::Date                &_today
,const Inactive_period_parameters *_perennial_inactive_period_parameter          //110303
,const Dormancy_parameters        *_fruit_tree_dormancy_parameter                //110303
,const Air_temperature_average    &_air_Tavg
,const Vapor_pressure_deficit_max &_VPD_max)
: CropSyst::Phenology_common                                                     //130902
   (_phenology_parameter
   ,thermal_time_cumulative                                                      //140317
   ,_perennial_inactive_period_parameter != NULL
   ,_fruit_tree_dormancy_parameter       != NULL)
, thermal_time_parameter(_thermal_time_parameter)
, season_correction     (_season_correction)
, is_root_crop          (_is_root_crop)
, air_Tavg  (_air_Tavg)
, VPD_max   (_VPD_max)
, thermal_time_cumulative  (0.00009)
, thermal_time_cumulative_yesterday(0.00009)
, thermal_time_today       (0.00009)
, today(_today)
, perennial_inactive_period_parameter  (_perennial_inactive_period_parameter)
, fruit_tree_dormancy_parameter        (_fruit_tree_dormancy_parameter)
, dormancy                 (0)
{  growth_stage = NGS_SOWING; // preemergence
}
//_____________________________________________________________________________/
bool Phenology_common::start_season()                              modification_
{  if (dormancy) delete dormancy; dormancy = 0;
   return CropSyst::Phenology_common::start_season();
}
//_____________________________________________________________________________/
bool Phenology_common::reset()                                    modification_
{  thermal_time_cumulative  = 0.00009;
   thermal_time_cumulative_yesterday = 0.00009;
   thermal_time_today       =0.00009;
   return CropSyst::Phenology_common::reset();
}
//_2001-05-06__________________________________________________________________/
Phenology_actual::Phenology_actual
(const Phenology_parameters      &_phenology_parameter
,const Thermal_time_parameters   &_thermal_time_parameter
,const Season_correction         &_season_correction                             //110203
,bool _is_root_crop
,const CORN::Date &_today
,const Inactive_period_parameters *_perennial_inactive_period_parameter          //110303
,const Dormancy_parameters        *_fruit_tree_dormancy_parameter                //110303
,const Air_temperature_average    &_air_Tavg
,const Vapor_pressure_deficit_max &_VPD_max)
: Phenology_common(_phenology_parameter,_thermal_time_parameter,_season_correction,_is_root_crop
,_today
,_perennial_inactive_period_parameter                                            //110303
,_fruit_tree_dormancy_parameter                                                  //110303
,_air_Tavg
,_VPD_max)
, water_stress_index_yesterday                  (&unassigned_reference)
{}
//_____________________________________________________________________________/
void Phenology_actual::assign_references
(const float64 * /*150319 Ref_to_const_float*/ _water_stress_index_yesterday)                 modification_
{  water_stress_index_yesterday                       = _water_stress_index_yesterday;
}
//_____________________________________________________________________________/
Normal_crop_event_sequence Phenology_common::start_bulking_or_filling()  modification_
{  growth_stage =
      is_root_crop         // (harvested_part_labeled.get() == ROOT_CROP)
      ? NGS_FILLING        // GRAIN_FILLING
      : NGS_BULK_FILLING;  // BULK_FILLING;
   return growth_stage;
}
//_____________________________________________________________________________/
void Phenology_common::develop()                                   modification_
{  // was CropDevelopment
   float64 plant_temperature_elevation_during_stress = 0.0;
   #ifdef NYN
   //110422 plant_temperature_elevation_during_stress is not currently used in VIC
   plant_temperature_elevation_during_stress = get_plant_temperature_elevation_during_stress();   // For potential mode,  plant_temperature_elevation_during_stress will be 0.0
   #endif
      float64 degree_day = 0.0;
      /* RLN now using daily resolution
      for (int hour = 1; hour <= 24; hour++)
      {
        int hourly_temperature = WeatherState.HourlyTemperature(Hour) + plant_temperature_elevation_during_stress;
        degree_day +=
             calc_thermal_time_hourly
               (thermal_time_parameter.base_temperature
               ,thermal_time_parameter.opt_temperature
               ,thermal_time_parameter.max_temperature
               , hourly_temperature);
      }
      degree_day = degree_day / 24.0 + plant_temperature_elevation_during_stress;
      */
      degree_day = calc_growing_degree_day_daily_resolution(plant_temperature_elevation_during_stress) ;
      thermal_time_today = degree_day;
      thermal_time_cumulative += degree_day;
      if (perennial_inactive_period_parameter) dormancy_processing();               //110302
   /* RLN NYN nothing significant here
   if (thermal_time_cumulative <  CropParameters.GetThermalTimeForEmergence)
      start_preemergence();
   */
   if (dormancy)
   {  if (dormancy->dormant)                                                     //110421
         growth_stage = NGS_DORMANT_or_INACTIVE;                                 //110421
   }
   else
   {  if ((growth_stage == NGS_RESTART)                                          //110421
          ||((growth_stage != NGS_ACTIVE_GROWTH)                                 //110421
          && (thermal_time_cumulative >= phenology_parameter.emergence_deg_day)  // CropParameters.GetThermalTimeForEmergence
          && (thermal_time_cumulative  < phenology_parameter.begin_flowering_deg_day)
          //WARNING Claudio needs to remove check for flowering, this is problematic
          //for crops that don't have flowering (potato)
          //Actually there should be a check for valid apropos growth stages.
          ))       // CropParameters.GetThermalTimeForFlowering
      {  start_vegetative_growth();
         days_since_start_of[NGS_EMERGENCE] = 0;                                 //110506
      }
      if (growth_stage < NGS_MATURITY)                                           //110422
      {
      if ((thermal_time_cumulative >= phenology_parameter.begin_flowering_deg_day) &&  // CropParameters.GetThermalTimeForFlowering
          (thermal_time_cumulative <  phenology_parameter.begin_filling_deg_day) &&    // CropParameters.GetThermalTimeForYieldFormation
          (!is_root_crop))
         start_flowering();
      if ((thermal_time_cumulative >= phenology_parameter.begin_filling_deg_day) &&    // CropParameters.GetThermalTimeForYieldFormation
          (thermal_time_cumulative <  phenology_parameter.begin_maturity_deg_day))     // CropParameters.GetThermalTimeForPhysiologicalMaturity
         start_bulking_or_filling();
      CORN::Days at_season_length_limit = season_correction.duration && (days_since_start_of[NGS_ACTIVE_GROWTH] > season_correction.duration + 10); //110420
      if (((thermal_time_cumulative >= phenology_parameter.begin_maturity_deg_day)
           ||at_season_length_limit //110203 added to ensure season ends if thermal time is not reached
          ))                                                                     // CropParameters.GetThermalTimeForPhysiologicalMaturity
         start_maturity();
      }
      if (growth_stage == NGS_HARVEST)                                           //110421
          growth_stage =  NGS_POSTHARVEST;                                       //110421
   }
}
//_____________________________________________________________________________/
float64 Phenology_common::calc_growing_degree_day_daily_resolution(float64 plant_temperature_elevation_during_stress)       const
{  // value for a single day
   float64 avg_plant_temp = air_Tavg./*LML 150318 get_*/Celcius();
   float64 deg_day_base_temp   = thermal_time_parameter.base_temperature;
   float64 deg_day_cutoff_temp = thermal_time_parameter.cutoff_temperature;
   avg_plant_temp += plant_temperature_elevation_during_stress; // During stress conditions the plant temperature may be a higher than air temperature
   avg_plant_temp = CORN::must_be_between(avg_plant_temp,deg_day_base_temp,deg_day_cutoff_temp);
   float64 result_growing_degree_day = avg_plant_temp - deg_day_base_temp;
   return result_growing_degree_day;
}
//______________________________________________________________________________
/* RLN using daily resolution
float64 Phenology_common::calc_thermal_time_hourly
(float64 air_temperature)                                                  const
{
   float64 opt_temperature = thermal_time_parameter.opt_temperature;
   float64 max_temperature = thermal_time_parameter.max_temperature;
   float64 base_temperature= thermal_time_parameter.base_temperature;
   float64 thermal_time =
      (air_temperature < opt_temperature)
      ? air_temperature - base_temperature
      : (max_temperature - air_temperature)
         * (opt_temperature - base_temperature)
         / (max_temperature - opt_temperature);
  thermal_time = CORN::must_be_greater_or_equal_to<float64>(thermal_time,0.0);
  return  thermal_time;
}
*/
//_____________________________________________________________________________/
#ifdef NYN
float64 Phenology_actual::get_plant_temperature_elevation_during_stress()  const
{  float64 plant_temperature_elevation_during_stress = 0.0;
   nat16 days_after_emergence= days_since_start_of[NGS_EMERGENCE];
   if (days_after_emergence > 1)
   {  nat16 days_after_planting = days_since_start_of[NGS_SOWING];
     // Now a member reference float64 water_stress_index_yesterday = CropState.WaterStressIndex(days_after_planting - 1);
      if (*water_stress_index_yesterday > 0.1)
      {
         float64 ratio = (*solar_rad_intercepted_green_canopy_yesterday)
                       / (*solar_rad_intercepted_fraction_yesterday);           // FractionSolarRadInterceptedTotalCanopy

         float64 VPDmax = VPD_max.get();
         plant_temperature_elevation_during_stress
               = (1.0 + 1.5 * VPDmax)
                * (*water_stress_index_yesterday) * ratio;
      } // if
   } // if
   return plant_temperature_elevation_during_stress;
}
#endif
//______________________________________________________________________________
bool Phenology_common::start_day()                                 modification_
{  bool started = CropSyst::Phenology_common::start_day();                       //130902
   thermal_time_cumulative_yesterday = thermal_time_cumulative;
   if (is_perennial && ! dormancy)
   {
      bool start_dormant = (today.get_DOY() == 1);
         //110406 Assuming perennial crops need to start in Dormancy
         // Warning assuming northern hemisphere
      if (start_dormant)
      {
         dormancy =
            (fruit_tree_dormancy_parameter!=NULL)                                //              CropState.ReadyForDormancy = True
            ? dynamic_cast<Dormancy *>(new Dormancy_fruit_tree(today,*perennial_inactive_period_parameter,*fruit_tree_dormancy_parameter,*this,false/*ready for dormancy*/))
            : dynamic_cast<Dormancy *>(new Dormancy_herbacious(today,*perennial_inactive_period_parameter,*this,false/*ready for dormancy*/));
         dormancy->know_average_air_temperature(air_Tavg);
         dormancy->start(false);
      }
   }
   return true;
}
//______________________________________________________________________________
} // namespace VIC_crop
#endif

