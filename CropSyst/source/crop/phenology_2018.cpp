#if ((PHENOLOGY_VERSION==2018))

#include "crop/phenology_2018.h"
#include "CS_suite/observation/CS_inspector.h"
#include "crop/thermal_time_daily.h"
namespace CropSyst
{
//______________________________________________________________________________
Phenology_2018::Phenology_2018
(const Crop_parameters_struct::Phenology &parameters_
,const float64 &stress_adjusted_temperature_C_                                   //190812
,const float64 &air_temperature_min_C_                                           //190812
,Thermal_time_common::Vernalization        *vernalization_given
,Thermal_time_common::Photoperiodization   *photoperiodization_given
,bool  is_perennial_
,bool  is_fruit_tree_
,const CORN::date32 &simdate_raw_
)
: Phenology_abstract // was common
   (parameters_
   ,stress_adjusted_temperature_C_
   ,air_temperature_min_C_
   ,vernalization_given
   ,photoperiodization_given
   ,is_perennial_
   ,is_fruit_tree_
   ,simdate_raw_)
, inspectors                                                                 (0)
, emanator                                                                   (0)
, events_realized                                                         (true) //unique
, germination                                                                (0)
, emergence_event                                                            (0)
, quiescence                                                                 (0)
, anthesis                                                                   (0)
, yield_formation                                                            (0)
, tuberization                                                               (0)
, maturity                                                                   (0)
, fructescence                                                               (0)
, veraison                                                                   (0)
, rapid_fruit_development                                                    (0)
, most_active_period                                                         (0)
{
   //200508GDDs[0] = 0; GDDs[1] = 0;
   GDDs = 0;                                                                     //200508
   FOR_EACH_IN(period_param,Crop_parameters_struct::Phenologic_period,parameters.phenologic_periods,iter_period)
   {
      if (period_param->is_valid())
         periods[pending].take(new /*Phenology_abstract::*/Period_thermal
            (*period_param,simdate_raw
            ,stress_adjusted_temperature_C_ ,air_temperature_min_C_
            ,is_fruit_tree_
            ,period_param->relevant & RELEVANT_VERNALIZATION ? vernalization : 0
            ,period_param->relevant & RELEVANT_PHOTOPERIOD   ? photoperiodization : 0));
   } FOR_EACH_END(iter_period)
}
//_constructor______________________________________________________2018-11-09_/
Phenology_2018::~Phenology_2018()
{}
//__Phenology_2018::destructor______________________________________2018-11-18_/
nat8 Phenology_2018::activate_pending_periods
(CORN::Unidirectional_list &periods_to_activate)
{  nat8 activations = 0;
   FOR_EACH_IN(period_pending,/*Phenology_abstract::*/Period_thermal,periods[pending],pending_iter)
   {
      const Crop_parameters_struct::Phenologic_period &period_pending_params
            = *period_pending->parameters;
      bool basis_matches            = true;
      bool relative_to_matches      = true;
      float32 active_thermal_time   = 999999;
      #if (CROPSYST_VERSION < 6)
      // This is to support legacy parameter files
      // (that don't have separate period phenology)
      // Legacy crop phenologic stages were relative to planting/sowing crop instanciation/start of season
      if (period_pending_params.initiation.relative_to == PHENOLOGY_SEASON_ONTOLOGY)
      {
         active_thermal_time = get_accum_degree_days(/*false*/);
      }
      else
      #endif
      {
         const Phenology::Period_thermal *relative_to_period_active              //200508
            = dynamic_cast<const Period_thermal *>                               //200508
            (periods[active].find_string(period_pending_params.initiation.relative_to));
         if (relative_to_period_active)
         {
            relative_to_matches = true;
            basis_matches = period_pending_params.initiation.basis
               == Crop_parameters_struct::Phenologic_period::initiation_basis;
            if (basis_matches)
               active_thermal_time = relative_to_period_active->get_thermal_time_accum();
         } else relative_to_matches = false;
      }
      bool thermal_time_matches = active_thermal_time
                 >= period_pending_params.initiation.thermal_time_accum;         //180104
      if (basis_matches && relative_to_matches && thermal_time_matches)
      {  period_pending->start_day_in_season = season_duration_days;
         if (inspectors)
         {
            std::string ontology(period_pending_params.ontology);
            ontology+="/initiation";
            inspectors->append
               (new CS::Inspector_scalar
                  (period_pending->initiation_date //200508initiated
                  ,UT_date   ,*emanator
                  ,period_pending_params.ontology.c_str()
                  ,CORN::final_statistic
                  #if ((CS_VERSION>=4) && (CS_VERSION<=5))
                  ,period_pending_params.variable_code_UED
                  #endif
                  ));
         }
         periods_to_activate.take(pending_iter->detach_current());
         activations++;
      }
   } FOR_EACH_END(pending_iter)
   return activations;
}
//_activate_pending_periods_________________________________________2018-11-28_/
bool Phenology_2018::update_GDDs()                                 modification_
{  bool updated = false;
   /*200508
   GDDs[0] = 0;
   GDDs[1] = 0;
   */
   GDDs = 0;                                                                     //200508
   for (nat8 p = active; p <= clipped ; p++)
   {  FOR_EACH_IN(period,Period_thermal,periods[p],periods_iter)
      {  // we should only start day for active periods (done above) started &= period->start_day();
         if (period->parameters->consecutive)
         {  float64 GDDs_period = period->get_thermal_time_accum();
            GDDs += GDDs_period;                                                 //200508
            /*200508 obsolete
            GDDs[0] += GDDs_period;
            if (p != clipped)
               GDDs[1] += GDDs_period;
            */
         }
      } FOR_EACH_END(periods_iter)
   }
   return updated;
}
//_update___________________________________________________________2019-01-04_/
bool Phenology_2018::start_day()                                   modification_
{  bool started = true;
   if (vernalization)      vernalization      ->update();
   if (photoperiodization) photoperiodization ->update();
   #ifdef RELATIVE_TO_PERIODS
   // This is the new implementation
   activate_pending_periods(periods_to_activate);
   if (periods_to_activate.count())
   {
      realize_events(periods_to_activate);
      register_key_periods(periods_to_activate);
      periods[active].transfer_all_from(periods_to_activate);
   }
   FOR_EACH_IN(period_active,Period_thermal,periods[active],active_iter)
   {  started &= period_active->start_day();
      if (period_active->parameters->consecutive)
         most_active_period = period_active;
   } FOR_EACH_END(active_iter)
   #endif
   #ifdef RELATIVE_TO_PERIODS
   // This is the new implementation
   started &= update_GDDs();                                                     //190104
   #endif
   return started;
}
//_start_day________________________________________________________2018-11-07_/
bool Phenology_2018::process_day()                                 modification_
{
   bool processed = true;
   #ifndef RELATIVE_TO_PERIODS
   // originally GDD was updated during the day processing.
   // This is the new implementation

   activate_pending_periods(periods_to_activate);
   if (periods_to_activate.count())
   {  realize_events(periods_to_activate);
      register_key_periods(periods_to_activate);
      periods[active].transfer_all_from(periods_to_activate);
   }
   FOR_EACH_IN(period_active,Period,periods[active],active_iter)
   {  processed &= period_active->start_day();
      if (period_active->parameters->consecutive)
         most_active_period = period_active;
   } FOR_EACH_END(active_iter)
   processed = update_GDDs();
   activate_pending_periods(periods_to_activate);
   if (periods_to_activate.count())
   {  realize_events(periods_to_activate);
      register_key_periods(periods_to_activate);
      periods[active].transfer_all_from(periods_to_activate);
   }
   #endif
   return processed;
}
//_process_day_____________________________________________________2018-12_31_/
bool Phenology_2018::end_day()
{  bool ended = true;
   FOR_EACH_IN(period_active,Period_thermal,periods[active],active_iter)
   {  ended &= period_active->end_day();
      const Crop_parameters_struct::Phenologic_period *period_active_params
            = period_active->parameters;
      if (period_active->has_expired())
      {  FOR_EACH_IN(period_pending,Period_thermal,periods[pending],pending_iter)
         {  // Look for periods whose initiation is relative to this active period.
            const Crop_parameters_struct::Phenologic_period *period_pending_params
               = period_pending->parameters;
            bool basis_matches = period_pending_params->initiation.basis
               == Crop_parameters_struct::Phenologic_period::culmination_basis;
            bool relative_to_matches =
               period_pending_params->initiation.relative_to
               == period_active_params->ontology;
            bool thermal_time_acheived = period_active->get_thermal_time_accum()
                   > period_pending_params->initiation.thermal_time_accum;
            if (basis_matches && relative_to_matches &&thermal_time_acheived)
            {  if (inspectors)
               {  std::string ontology(period_pending_params->ontology);
                  ontology+="/culmination";
                  inspectors->append
                     (new CS::Inspector_scalar
                        (period_pending->initiation_date
                        ,UT_date   ,*emanator
                        ,period_pending_params->ontology.c_str()
                        ,CORN::final_statistic
                        #if ((CS_VERSION>=4) && (CS_VERSION<=5))
                        ,period_pending_params->variable_code_UED
                        #endif
                        ));
               }
               periods_to_activate.take(pending_iter->detach_current());
            }
         } FOR_EACH_END(pending_iter)
         periods[completed].take(active_iter->detach_current());
      }
   } FOR_EACH_END(active_iter)
   return ended && Phenology_abstract::end_day();
}
//_end_day__________________________________________________________2018-11-07_/
bool Phenology_2018::register_key_period(Period_thermal *potential_key_period)
{
      std::string period_ontology(potential_key_period->parameters->ontology);
      if      (period_ontology == "phenology/germination")              germination             = potential_key_period;
      else if (period_ontology == "phenology/emergence")                emergence_event         = potential_key_period;
      else if (period_ontology == "phenology/accrescence")              accrescence             = potential_key_period;
      else if (period_ontology == "phenology/culminescence")            culminescence           = potential_key_period;
      else if (period_ontology == "phenology/senescence")               senescence              = potential_key_period;
      else if (period_ontology == "phenology/quiescence")               quiescence              = potential_key_period;
      else if (period_ontology == "phenology/anthesis")                 anthesis                = potential_key_period;
      else if (period_ontology == "phenology/yield_formation")          yield_formation         = potential_key_period;
      else if (period_ontology == "phenology/tuberization")             tuberization            = potential_key_period;
      else if (period_ontology == "phenology/maturity")                 maturity                = potential_key_period;
      else if (period_ontology == "phenology/root_elongation")          root_elongation         = potential_key_period;
      else if (period_ontology == "phenology/fructescence")             fructescence            = potential_key_period;
      else if (period_ontology == "phenology/veraison")                 veraison                = potential_key_period;
      else if (period_ontology == "phenology/rapid_fruit_development")  rapid_fruit_development = potential_key_period;
    //else if (period_ontology == xxxx)  xxxxx = potential_key_period;

   return true;
}
//_register_key_period______________________________________________2018-12-30_/
nat16 Phenology_2018::register_key_periods(CORN::Container &potential_key_periods)
{  nat16 registered_count = 0;
   FOR_EACH_IN(potential_key_period,Period_thermal,potential_key_periods,period_iter)
      registered_count += register_key_period(potential_key_period);
   FOR_EACH_END(period_iter)
   return registered_count;
}
//_register_key_periods_____________________________________________2018-12-30_/
bool Phenology_2018::start_season()                              modification_
{     return Phenology_abstract::start_season();
}
//_start_season_____________________________________________________2018-11-07_/
bool Phenology_2018::reset
(Normal_crop_event_sequence from
,Normal_crop_event_sequence to)                                    modification_
{
   // Not sure if this method is applicable to 2018 version
   // because active periods get moved to culimnated
   return Phenology_abstract::reset(from,to);
}
//_reset____________________________________________________________2018-11-07_/
bool Phenology_2018::has_emerged()                                  affirmation_
{
   return emergence_event || accrescence || culminescence || senescence || anthesis
      || yield_formation || tuberization || maturity;
}
//_has_emerged______________________________________________________2018-11-13_/
bool Phenology_2018::is_in_germination()                            affirmation_
{  return germination && !germination->has_expired();
}
//_is_in_germination________________________________________________2018-11-07_/
bool Phenology_2018::is_in_accrescence()                            affirmation_
{
   bool in_accrescence = accrescence && !accrescence->has_expired();
    float64 start_growth_deg_day =
      // NYI is_fruit_tree ? 0.0 :
      parameters.initiation.emergence;
   float32 thermal_time_cumulative = ref_accum_deg_days_normal();
   return in_accrescence || // (growth_stage == NGS_ACTIVE_GROWTH) ||                                 //110422
      ( (thermal_time_cumulative  >= start_growth_deg_day)
     && (thermal_time_cumulative  <= parameters.duration.accrescence));
} // was is_in_vegetative_growth
//_is_in_accrescence________________________________________________2018-11-07_/
bool Phenology_2018::has_expired_accrescence()                      affirmation_
{  return accrescence && accrescence->has_expired();
}
//_has_expired_accrescence__________________________________________2019-06-28_/
bool Phenology_2018::is_in_senescence()                             affirmation_
{  return senescence && !senescence->has_expired();
}
//_Phenology_2018:is_in_senescence__________________________________2018-11-07_/
bool Phenology_2018::is_flowering()                                 affirmation_
{  return anthesis && !anthesis->has_expired();
}
//_is_flowering_____________________________________________________2018-11-13_/
bool Phenology_2018::is_in_anthesis()                               affirmation_
{  return anthesis && !anthesis->has_expired();
}
//_is_flowering_____________________________________________________2018-11-13_/
bool Phenology_2018::is_in_fructescence()                           affirmation_
{  return fructescence && !fructescence->has_expired();
}
//_is_in_fructescence_______________________________________________2018-11-13_/
bool Phenology_2018::is_in_veraison()                               affirmation_
{  return veraison && !veraison->has_expired();
}
//_is_in_veraison___________________________________________________2018-11-13_/
bool Phenology_2018::is_in_rapid_fruit_development()                affirmation_
{  return rapid_fruit_development && !rapid_fruit_development->has_expired();
}
//_is_in_rapid_fruit_development____________________________________2018-11-13_/
bool Phenology_2018::is_at_maturity() /*200509 is_maturity_first_day() */ affirmation_
{  return maturity && maturity->get_lapse() <= 1;
}
//_is_in_xxxxx______________________________________________________2018-11-13_/
bool Phenology_2018::is_maturing()                                  affirmation_
      // (the crop has reached maturity initiation but not yet culmination);
{  return maturity && !maturity->has_expired();
}
//_is_in_xxxxx______________________________________________________2018-11-13_/
bool Phenology_2018::has_matured_new()                              affirmation_
{  const Period_thermal *completed_maturity =
      dynamic_cast<const Period_thermal *>
      (periods[completed].find_cstr("phenology/maturity"));
   return completed_maturity || (maturity && maturity->has_expired());
}
//_is_in_xxxxx______________________________________________________2018-11-13_/
bool Phenology_2018::is_in_quiescence()                             affirmation_
{  return quiescence && !quiescence->has_expired();
}
//_is_in_xxxxx______________________________________________________2018-11-13_/
bool Phenology_2018::is_at_quiescence()                                 affirmation_
{  return quiescence && quiescence->get_lapse() <= 1;
}
//_is_at_quiescence_________________________________________________2020-04-04_/
bool Phenology_2018::is_dormant()                                   affirmation_
{  return quiescence && !quiescence->has_expired();
   // Warning may need special dormancy indicator
}
//_is_in_xxxxx______________________________________________________2018-11-13_/
bool Phenology_2018::is_in_growing_season()                         affirmation_
{  return !is_in_quiescence();
}
//_is_in_xxxxx______________________________________________________2018-11-13_/
bool Phenology_2018::is_in_yield_formation()                        affirmation_
{  return yield_formation && !yield_formation->has_expired();
}
//_is_in_yield_formation____________________________________________2018-11-13_/
bool Phenology_2018::is_in_growth_period()                          affirmation_
{  return is_in_growing_season();
}
//_is_in_growth_period______________________________________________2018-11-13_/
bool Phenology_2018::is_in_culminescence()                          affirmation_
{  return culminescence && !culminescence->has_expired();
}
//_is_in_culminescence______________________________________________2018-11-13_/
bool Phenology_2018::is_in_tuberization()                           affirmation_
{  return tuberization && !tuberization->has_expired();
}
//_is_in_tuberization_______________________________________________2018-11-07_/
void Phenology_2018::clear_days_since_start_of_harvest()            assignation_
{
   assert(false);
}
//_clear_days_since_start_of_harvest________________________________2018-11-07_/
bool Phenology_2018::is_early
(const CS::Period *period,nat16 days)                               affirmation_
{  return period && (period->get_lapse() <= days);                               //200508
}
//_is_early_________________________________________________________2018-11-08_/
Phenology_2018::Period_thermal *Phenology_2018
::activate_period(const char *period_ontology_cstr)
{
   Period_thermal *period_active_known = dynamic_cast<Period_thermal *>          //181228
      (periods[active].find_cstr(period_ontology_cstr));                         //181228
   if (period_active_known) // we have already activated such a period.
      return period_active_known;                                                //181228
   Period_thermal *period_activating = dynamic_cast<Period_thermal *>
      (periods[pending].find_cstr(period_ontology_cstr));
   if (period_activating)
   {  periods[pending].detach(period_activating);
   } else
   {  const Crop_parameters_struct::Phenologic_period *period_params =
         dynamic_cast<const Crop_parameters_struct::Phenologic_period *>
         (parameters.phenologic_periods.find_cstr(period_ontology_cstr));
      /* There doesnt need to be period parameters because
         the period may be initiated by a spurious event
         (I.e. planting which initiates the season period)
      */
      if (period_params)
      {
         period_activating = new Period_thermal
            (*period_params
            // Actually should specify if parameters are owned or not,
            // warning current newed period_params are never deleted
            ,simdate_raw
            , stress_adjusted_temperature_C
            , air_temperature_min_C
            , is_fruit_tree
            , vernalization,photoperiodization);
      }
   }
   events_realized.take(new CORN::Item_string(period_ontology_cstr));
   if (period_activating)
   {  period_activating->start_day_in_season = season_duration_days;
      periods[active].take(period_activating);
      register_key_period(period_activating);
   }
   return period_activating;
}
//_activate_period__________________________________________________2018-11-11_/
bool Phenology_2018::activate_sowing()
{  planting_date = simdate_raw;
   return germination = activate_period("phenology/germination");
}
//_activate_sowing__________________________________________________2018-11-08_/
bool Phenology_2018::activate_emergence()
{  germination = 0;
   return emergence_event = activate_period("phenology/emergence");
}
//_activate_emergence_______________________________________________2018-11-11_/
bool Phenology_2018::activate_accrescence()
{  emergence_event = 0;
   accrescence = activate_period("phenology/accrescence");
   return (accrescence
      && Phenology_abstract::activate_accrescence());
}
//_activate_accrescence_____________________________________________2018-11-11_/
bool Phenology_2018::activate_culminescence()
{
   culminescence = activate_period("phenology/culminescence");
   return (culminescence && Phenology_abstract::activate_culminescence());
}
//_activate_culminescence___________________________________________2018-11-13_/
bool Phenology_2018::activate_senescence()
{
   senescence = activate_period("phenology/senescence");
   return (senescence && Phenology_abstract::activate_senescence());
}
//_activate_senescence______________________________________________2018-11-13_/
bool Phenology_2018::activate_quiescence()
{  accrescence = 0;
   culminescence = 0;
   senescence = 0;
   maturity = 0;
   return quiescence = activate_period("phenology/quiescence");
}
//_activate_quiescence______________________________________________2018-11-08_/
bool Phenology_2018::activate_anthesis()
{  return anthesis = activate_period("phenology/anthesis");
}
//_activate_anthesis________________________________________________2018-11-08_/
bool Phenology_2018::activate_yield_formation()
{  return yield_formation = activate_period("phenology/yield_formation");
}
//_activate_filling_________________________________________________2018-11-08_/
bool Phenology_2018::activate_maturity()
{  return maturity = activate_period("phenology/maturity");
}
//_activate_maturity________________________________________________2018-11-08_/
bool Phenology_2018::activate_tuberization()
{  return tuberization = activate_period("phenology/tuberization");
}
//_activate_tuberization____________________________________________2018-11-08_/
bool Phenology_2018::activate_fructescence()
{  return fructescence = activate_period("phenology/fructescence");
}
//_activate_fructescence____________________________________________2018-11-08_/
bool Phenology_2018::activate_veraison()
{  return veraison = activate_period("phenology/veraison");
}
//_activate_veraison________________________________________________2018-11-08_/
bool Phenology_2018::activate_rapid_fruit_development()
{  return rapid_fruit_development = activate_period("phenology/rapid_fruit_development");
}
//_activate_rapid_fruit_development_________________________________2018-11-08_/
Phenology_2018::Period_thermal *Phenology_2018::activate_root_elongation()
{  return root_elongation = activate_period("phenology/root_elongation");
}
//_activate_root_elongation_________________________________________2018-11-18_/
bool Phenology_2018::activate_restart()
{  // restart of growth (accrescence)
   return accrescence = activate_period("phenology/accrescence");
}
//_activate_restart_________________________________________________2018-11-13_/
nat8 Phenology_2018::respond_to_clipping()
{
   nat8 reset_count = 0;
   {
      FOR_EACH_IN(active_period,Period_thermal,periods[active],active_iter)
      {  if (active_period->clipping_resets())
         {  reset_count++;
            periods[completed].take(active_iter->detach_current());
         }
      } FOR_EACH_END(active_iter)
   }
   if (reset_count)
   {  activate_accrescence();
   }
   return reset_count;
}
//_respond_to_clipping______________________________________________2018-11-08_/
nat8 Phenology_2018::realize_events
(const CORN::Container &activating_periods)                        contribution_
{  FOR_EACH_IN(period,Period_thermal,activating_periods,period_iter)
   {  events_realized.take(new CORN::Item_string(period->parameters->ontology));
   } FOR_EACH_END(period_iter)
   return events_realized.count();
}
//_realize_events___________________________________________________2018-11-08_/
bool Phenology_2018::know_observation
(CORN::Container     *inspectors_
,const CS::Emanator  *emanator_)                                      cognition_
{  inspectors = inspectors_;
   emanator   = emanator_;
   return true;
}
//_know_observation_________________________________________________2018-11-09_/
float64 Phenology_2018::get_growing_degree_day()
{
   return most_active_period
        ? most_active_period->thermal_time->get_growing_degree_day(): 0;
}
//_get_growing_degree_day______________________________________________________/
/*200201
float64 Phenology_2018::get_accum_degree_days
(bool adjusted_for_clipping)                                               const
{  return GDDs[adjusted_for_clipping];
}
//_get_accum_degree_days____________________________________________2018-11-14_/
*/
float64 Phenology_2018::get_accum_degree_days
()                                                                         const
{  return GDDs;
}
//_get_accum_degree_days____________________________________________2018-11-14_/

CORN::date32 no_period_date = 0;
// This is just a dummy value when there is no event or period started yet

const CORN::date32 &Phenology_2018::ref_emergence_date()                   const { return emergence_event      ? emergence_event    ->initiation_date : no_period_date;} //181111
const CORN::date32 &Phenology_2018::ref_restart_date()                     const { return accrescence          ? accrescence        ->initiation_date : no_period_date;} //181111
const CORN::date32 &Phenology_2018::ref_anthesis_date()                    const { return anthesis             ? anthesis           ->initiation_date : no_period_date;} //181111
const CORN::date32 &Phenology_2018::ref_tuberization_date()                const { return tuberization         ? tuberization       ->initiation_date : no_period_date;} //181111
const CORN::date32 &Phenology_2018::ref_maturity_date()                    const { return maturity             ? maturity           ->initiation_date : no_period_date;} //181111
const CORN::date32 &Phenology_2018::ref_fructescence_date()                const { return fructescence         ? fructescence       ->initiation_date : no_period_date;} //181111
const CORN::date32 &Phenology_2018::ref_yield_formation_date()             const { return yield_formation      ? yield_formation    ->initiation_date : no_period_date;} //181111
const CORN::date32 &Phenology_2018::ref_culminescence_date()               const { return culminescence        ? culminescence      ->initiation_date : no_period_date;} //181111
const CORN::date32 &Phenology_2018::ref_quiescence_init_date()             const { return quiescence           ? quiescence         ->initiation_date : no_period_date;} //181111
const CORN::date32 &Phenology_2018::ref_quiescence_culm_date()             const { return quiescence           ? quiescence         ->initiation_date: no_period_date;} //181111
const CORN::date32 &Phenology_2018::ref_rapid_fruit_growth_date()          const { return rapid_fruit_development? rapid_fruit_development ->initiation_date : no_period_date;} //181111

//______________________________________________________________________________
bool Phenology_2018::was_realized(const char *phenology_ontology)   affirmation_
{  bool realized = events_realized.remove_all_matching_key_cstr(phenology_ontology);
   return realized;
}
//_was_realized________________________________________________________________/
RENDER_INSPECTORS_DEFINITION(CropSyst::Phenology_abstract)
{
   // Need to add thermal time accumuilation is with Thermal_time_common

/* compiler is complaining, but this is needed!!!

   inspectors.append(new CS::Inspector_scalar(GDDs[0]  ,UC_Celcius_degree_days ,*context, "growing_degree_days/normal" ,sum_statistic,CSVP_crop_base_seasonal_thermal_time));
*/
   return 0; // doesn't have its own emanator emanator;
}
//_RENDER_INSPECTORS_DEFINITION________________________________________________/
}//_namespace_CropSyst_________________________________________________________/
#endif

