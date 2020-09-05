#include "crop/phenology_2013.h"
namespace CropSyst
{
// std::ofstream debug_canopy_thermaltime("canopy_TT.dat");

//______________________________________________________________________________
Phenology_2013::Phenology_2013
(const Crop_parameters_struct::Phenology &phenology_parameter_
,const float64 &thermal_time_cumulative_ref
,const float64 &stress_adjusted_temperature_C_                                   //190812
,const float64 &air_temperature_min_C_                                           //190812
,Thermal_time_common::Vernalization         *vernalization_given                 //190701
,Thermal_time_common::Photoperiodization    *photoperiodization_given            //190701
,bool  is_perennial_
,bool  is_fruit_tree_
,const CORN::date32 &simdate_raw_
)
: Phenology_abstract
   (phenology_parameter_
   ,stress_adjusted_temperature_C_
   ,air_temperature_min_C_
   ,vernalization_given
   ,photoperiodization_given
   ,is_perennial_,is_fruit_tree_
   ,simdate_raw_)
, phenology_parameter      (phenology_parameter_)
, thermal_time_cumulative  (thermal_time_cumulative_ref)
{
   for (int i =0 ; i < NGS_COUNT; i++)
   {  days_since_start_of_0 [i]    = -29999; //200404   0;
      duration_of[i]             = 0; //200404   0;
      initialization_date32[i]   = 0;                                            //190710
      finalization_date32[i]     = 0;                                            //190710
      start_day_in_season[i]     =65000;                                         //140625
   }
   start_day_in_season_senescence=65000;                                         //160211
   start_day_in_season_senescence_full=65000;                                    //160211
   start_date_tuber_initiation.set_datetime64(0);                                //141217
}
//_constructor______________________________________________________2018-11-07_/
Phenology_2013::~Phenology_2013()
{
   delete accrescence;
   delete culminescence;
   delete senescence;
   delete root_elongation;                                                       //191121
}
//_Phenology_2013:destructor________________________________________2019-10-22_/
bool Phenology_2013::start_day()                                   modification_
{
   /*200121 moved to end_day
   for (int gs = 0; gs < NGS_COUNT; gs++)
   {  duration_of          [gs] += (growth_stage == gs) ? 1 : 0;
      days_since_start_of  [gs] +=
         (growth_stage == gs)
         || (gs == NGS_SOWING)
         || (growth_stage == NGS_QUIESCENCE)
         || (//is_accrescence_initiated()                                        //190101
            has_emerged()
             && (growth_stage >= gs)) ? 1 : 0;
   }
   */

   //debug_canopy_thermaltime << std::endl<< "CTT:" << simdate_raw << "\t";

   // Not in 2018 version the periods are put in a list which is processed

   if (accrescence)     accrescence    ->start_day();
   if (culminescence)   culminescence  ->start_day();
   if (senescence)      senescence     ->start_day();
   if (root_elongation) root_elongation->start_day();                            //200125
   //200503 season_duration_days++;  // note that PHENOLOGY_2018 increments at the end of the day
   return true; // Phenology_abstract::start_day();
}
//_start_day________________________________________________________2013-09-02_/
bool Phenology_2013::end_day()                                     modification_
{
   // Moved from start_day                                                       //200121
   nat8 ngs_count = NGS_COUNT;
   for (int gs = 0; gs < ngs_count; gs++)
   {  duration_of          [gs] += (growth_stage == gs) ? 1 : 0;
       // days_since_start_of_0 will be set to 0 when the event is initiated
      nat8 increment = (days_since_start_of_0[gs] >= 0) ? 1 : 0;                 //200904
      days_since_start_of_0  [gs] +=
         increment;                                                              //200904
      /*200904
         (growth_stage == gs)
         || (gs == NGS_SOWING)
         || (growth_stage == NGS_QUIESCENCE)
         || (//is_accrescence_initiated()                                         //190101
            has_emerged() && (growth_stage >= gs))
         ? 1 : 0;
       */
   }
   if (accrescence)     accrescence    ->end_day();
   if (culminescence)   culminescence  ->end_day();
   if (senescence)      senescence     ->end_day();
   if (root_elongation) root_elongation->end_day();                              //200125
   // we override because 2013 increments season duration at the start of the day
   return Phenology_abstract::end_day();
}
//_end_day__________________________________________________________2013-09-02_/
bool Phenology_2013::restart_season()                              modification_
{  // Warning this is only working for fruit trees so this may be restart season
   growth_stage             = NGS_RESTART;
   harvest_DOY              = 0;
   days_since_start_of_0[NGS_EMERGENCE] = 0;  //1                                //110531 110506
   days_since_start_of_0[NGS_QUIESCENCE] = 0; //1                                //200404
   return Phenology_abstract::start_season();                                    //110506
}
//_start_season_____________________________________________________2013-09-02_/
bool Phenology_2013::is_in_germination()                            affirmation_
{  return growth_stage == NGS_GERMINATION;
}
//_is_in_germination________________________________________________2018-11-07_/
bool Phenology_2013::has_emerged()                                  affirmation_
{  Normal_crop_event_sequence growth_stage = get_growth_stage_sequence();        //120820
   return growth_stage >= NGS_ACTIVE_GROWTH
       && growth_stage < NGS_QUIESCENCE;
}
//______________________________________________________________________________
bool Phenology_2013::is_in_accrescence()                            affirmation_
{  return accrescence != 0;                                                      //190630
}
//_is_in_accrescence________________________________________________2013-09-02_/
bool Phenology_2013::has_expired_accrescence()                      affirmation_
{  return accrescence && accrescence->has_expired();                              //190630
}
//_has_expired_accrescence__________________________________________2019-06-28_/
bool Phenology_2013::is_flowering()                                 affirmation_
{  Normal_crop_event_sequence growth_stage = get_growth_stage_sequence();        //130903
   return growth_stage == NGS_ANTHESIS;
}
//_is_flowering________________________________________________________________/
bool Phenology_2013::is_in_anthesis()                                affirmation_
{  Normal_crop_event_sequence growth_stage = get_growth_stage_sequence();        //130903
   return growth_stage == NGS_ANTHESIS;
}
//_is_in_anthesis______________________________________________________________/
bool Phenology_2013::is_in_tuberization()                           affirmation_
{  return start_date_tuber_initiation.get_date32() > 0                           //170525
      &&  !days_since_start_of_0[NGS_FILLING] < 0;                               //200404
}
//_is_in_tuber_initiation___________________________________________2015-08-25_/
bool Phenology_2013::is_in_yield_formation()                        affirmation_
{  Normal_crop_event_sequence growth_stage = get_growth_stage_sequence();        //120820
   return growth_stage == NGS_BULK_FILLING;
}
//______________________________________________________________________________
bool Phenology_2013::is_in_culminescence()                          affirmation_
{  return  culminescence && !culminescence->has_expired();                       //190630
}
//_is_in_culminescence______________________________________________2018-11-09_/
bool Phenology_2013::is_in_senescence()                             affirmation_
{  return  senescence && ! senescence->has_expired();                            //190630
}
//______________________________________________________________________________
bool Phenology_2013::is_at_maturity()/*is__maturity_first_day()*/   affirmation_
{  return get_days_since_start_of(NGS_MATURITY) == 0; //200404 1;
}
//_is_at_maturity()_________________________________________________2018-11-09_/
bool Phenology_2013::is_maturing()                                  affirmation_
{  Normal_crop_event_sequence growth_stage = get_growth_stage_sequence();        //120820
   return growth_stage == NGS_MATURITY;
}
//_is_maturing()_______________________________________________________________/
bool Phenology_2013::has_matured_new()                              affirmation_
{  Normal_crop_event_sequence growth_stage = get_growth_stage_sequence();        //130903
   return (growth_stage >= NGS_MATURITY);
}
//______________________________________________________________________________
bool Phenology_2013::is_harvested()                                 affirmation_
{  return get_days_since_start_of(NGS_HARVEST) > 0;
}
//_is_harvested_____________________________________________________2017-08-20_/
bool Phenology_2013::is_in_quiescence()                             affirmation_
{  return growth_stage == NGS_QUIESCENCE;
}
//_is_in_quiescence_________________________________________________2018-11-09_/
bool Phenology_2013::is_at_quiescence()                             affirmation_
{  return days_since_start_of_0[NGS_QUIESCENCE] == 0;
}
//_is_at_quiescence_________________________________________________2020-04-04_/
bool Phenology_2013::is_dormant()                                   affirmation_
{  Normal_crop_event_sequence growth_stage = get_growth_stage_sequence();        //130903
   return growth_stage == NGS_QUIESCENCE;
}
//_is_dormant__________________________________________________________________/
bool Phenology_2013::is_in_growth_period()                          affirmation_
{  Normal_crop_event_sequence growth_stage = get_growth_stage_sequence();        //130903
   return ((growth_stage >= NGS_GERMINATION)
         &&(growth_stage < NGS_HARVESTABLE));                                    //130902_970313
}
//______________________________________________________________________________
bool Phenology_2013::is_in_growing_season()                         affirmation_
{  Normal_crop_event_sequence growth_stage = get_growth_stage_sequence();        //130903
   bool in_season = (growth_stage >= NGS_GERMINATION)                            //040608
                 && (growth_stage < NGS_QUIESCENCE);                             //130902
   if ((growth_stage == NGS_HARVEST)                                             //130902_040608
             //         && (!biomass_fate_today)
             // there is a condition that we dont wan't to return true
             // if we still have biomatter fate to dispose
           )    // Indicates in season harvest today.
       in_season = true; // special case for clipping harvest which may occur during growing season //040608
   return in_season;
}
//______________________________________________________________________________
bool Phenology_2013::is_early
(Normal_crop_event_sequence growth_stage
,nat16 days)                                                        affirmation_
{  nat16 days_since_start_of_stage = get_days_since_start_of(growth_stage);
   return days_since_start_of_stage
         > 0                                                                     //200404
      && ( days_since_start_of_stage <= days);
}
//_is_early_________________________________________________________2018-11-08_/
nat8 Phenology_2013::respond_to_clipping()
{
   nat8 reset_count = 0;
   if (accrescence)                                                              //190630
       reset_count += accrescence->respond_to_clipping();
   if (culminescence)
   {
      reset_count += culminescence->respond_to_clipping();
      std::cerr << "clipping occurred during culminescence (not expected, not handled)" << std::endl;
   }
   if (senescence)
   {
      reset_count += senescence->respond_to_clipping();
      std::cerr << "clipping occurred during senescence (not expected, not handled)" << std::endl;
   }
   return reset_count;
}
//_respond_to_clipping______________________________________________2020-02-10_/
bool Phenology_2013::reset
(Normal_crop_event_sequence from
,Normal_crop_event_sequence to)                                    modification_
{
   for (int i = from ; i <= to; i++)                                             //141202
   {
      days_since_start_of_0  [i] = -29999; //200404 0;
      duration_of          [i] = 0;
      start_day_in_season  [i]=65000;                                            //140625
      initialization_date32[i]=0;                                                //190710
      finalization_date32  [i]=0;                                                //190710
   }
   // NYI senescence
   // and senescence_full
   return Phenology_abstract::reset(from,to);
}
//_reset____________________________________________________________2018-11-07_/
bool Phenology_2013::activate_stage(Normal_crop_event_sequence sequence)
{  initialization_date32[sequence] = simdate_raw;                                //190710
   start_day_in_season[sequence] = season_duration_days;
   days_since_start_of_0[sequence] = 0; //200404 1;                              //200404
   return true;
}
//_activate_stage___________________________________________________2019-07-01_/
bool Phenology_2013::activate_sowing()
{  growth_stage = NGS_GERMINATION;                                               //190703
   return activate_stage(NGS_PLANTING);                                          //190701
}
//_activate_sowing__________________________________________________2018-11-08_/
bool Phenology_2013::activate_emergence()
{  growth_stage = NGS_ACTIVE_GROWTH;
   return activate_stage(NGS_EMERGENCE);                                         //190701
}
//_activate_emergence_______________________________________________2018-11-08_/
bool Phenology_2013::activate_accrescence()
{  activate_stage(NGS_ACCRESCENCE);
   accrescence = new Period_thermal
            (*(parameters.accrescence_period)
            // Actually should specify if parameters are owned or not,
            // warning current newed period_params are never deleted
            ,simdate_raw
            , stress_adjusted_temperature_C
            , air_temperature_min_C
            , is_fruit_tree
            , vernalization,photoperiodization);
   return Phenology_abstract::activate_accrescence();
}
//_activate_accrescence_____________________________________________2018-11-08_/
bool Phenology_2013::activate_culminescence()
{  activate_stage(NGS_END_CANOPY_GROWTH);
   culminescence = new Period_thermal
            (*(parameters.culminescence_period)
            // Actually should specify if parameters are owned or not,
            // warning current newed period_params are never deleted
            ,simdate_raw
            , stress_adjusted_temperature_C
            , air_temperature_min_C
            , is_fruit_tree
            , vernalization,photoperiodization);
   return Phenology_abstract::activate_culminescence();
}
//_activate_culminescence___________________________________________2019-07-01_/
bool Phenology_2013::activate_senescence()
{  // stage = NO STAGE
   //NYI activate_stage(NGS_XXXX);

//std::clog << "culm DD:" << culminescence->get_thermal_time_accum() << std::endl;

   senescence = new Period_thermal
            (*(parameters.senescence_period)
            // Actually should specify if parameters are owned or not,
            // warning current newed period_params are never deleted
            ,simdate_raw
            , stress_adjusted_temperature_C
            , air_temperature_min_C
            , is_fruit_tree
            , vernalization,photoperiodization);
   return Phenology_abstract::activate_senescence();
}
//_activate_senescence______________________________________________2019-07-01_/
Phenology::Period_thermal *Phenology_2013::activate_root_elongation()
{
   root_elongation = new Period_thermal
            (*(parameters.root_elongation_period)
            // Actually should specify if parameters are owned or not,
            // warning current newed period_params are never deleted
            ,simdate_raw
            , stress_adjusted_temperature_C
            , air_temperature_min_C
            , is_fruit_tree
            , vernalization,photoperiodization);
   return root_elongation;
}
//_activate_root_elongation_________________________________________2019-11-21_/
bool Phenology_2013::activate_tuberization()
{  start_date_tuber_initiation.set_date32(simdate_raw);

   // warning tuberization is not necessarily filling

   return activate_stage(NGS_FILLING);                                           //190701
}
//_activate_tuber_initiation________________________________________2018-11-08_/
bool Phenology_2013::activate_anthesis()
{  growth_stage = NGS_ANTHESIS;                                                  //200409
   return activate_stage(NGS_ANTHESIS);
}
//_activate_anthesis________________________________________________2018-11-08_/
bool Phenology_2013::activate_yield_formation()
{  growth_stage = NGS_FILLING;                                                   //200409
   return activate_stage(NGS_FILLING);
}
//_activate_filling_________________________________________________2018-11-08_/
bool Phenology_2013::activate_maturity()
{  growth_stage = NGS_MATURITY;                                                  //200409
   return activate_stage(NGS_MATURITY);
}
//_activate_maturity________________________________________________2018-11-08_/
bool Phenology_2013::activate_quiescence()
{  growth_stage = NGS_QUIESCENCE;                                                //200404
   bool activated = activate_stage(NGS_QUIESCENCE);                              //190710
   return activated;                                                             //200404
}
//_activate_quiescence______________________________________________2018-11-08_/
bool Phenology_2013::activate_restart()
{  return activate_stage(NGS_RESTART)
      && restart_season();                                                       //200507
}
//_activate_restart_________________________________________________2018-11-08_/
bool Phenology_2013::activate_harvest()
{  return activate_stage(NGS_HARVEST);
}
//_activate_harvest_________________________________________________2018-11-08_/
/*200503
nat16 Phenology_2013::get_season_duration_days()                           const
{  nat16 season_duration = get_days_since_start_of                               //190701
      ((is_perennial || is_fruit_tree) ? NGS_ACCRESCENCE : NGS_PLANTING);
   return season_duration;
}
*/
//_get_season_duration_days_________________________________________2018-11-11_/
}//_namespace CropSyst_________________________________________________________/
