#include "population_insect.h"
#include "CS_suite/observation/CS_inspector.h"
#include "corn/math/moremath.h"                                                    #define diapause_end_window_size 10
namespace CropSyst
{
//______________________________________________________________________________
Population_insect::Population_insect
(const CropSyst::Parameters_insect  &parameters_
,bool                                northern_hemisphere_                        //160211
,const CORN::date32                 &simdate_raw
,const Air_temperature_average      &air_temperature_mean_daily_
,const Air_temperature_minimum      &air_temperature_min_daily_
,const CORN::Dynamic_array<float32> &air_temperature_by_hour_
,const CORN::Dynamic_array<float32> &soil_temperature_by_hour_
)
: Pest                                                             (simdate_raw)
, parameters                                                       (parameters_)
, northern_hemisphere                                     (northern_hemisphere_) //160211
, air_temperature_mean_daily                       (air_temperature_mean_daily_)
, air_temperature_min_daily                         (air_temperature_min_daily_)
, air_temperature_by_hour                             (air_temperature_by_hour_)
, soil_temperature_by_hour                           (soil_temperature_by_hour_)
, first_population                                                        (true) //160118
, degree_days_cum                                                          (0.0)
, larvae_feeding_daily                                                     (0.0)
, stages                                                          (sub_elements)
{}
//_Population_insect:constructor____________________________________2015-03-16_/
bool Population_insect::restart_thermal_time_accumulation()       rectification_ //151210
{  degree_days_cum = 0;
   return true;
}
//_restart_thermal_time_accumulation________________________________2015-12-10_/
bool Population_insect::has_stage(Stage_holometabolic_enum stage,nat8 instar)     affirmation_
{  nat16 stage_index = (instar << 8) | (nat8)stage;
   return stages.find_nat32(stage_index) != 0;
}
//______________________________________________________________________________
//Specifically leaf area fed is " area of leaf fed by different larval instars
// in square meters in a unit field area of 1 square meter.
// Thus it corresponds to loss in leaf area index in 1 square meter field.
float64 Population_insect::get_damaged_GAI()                               const
{  return larvae_feeding_daily;
}
//______________________________________________________________________________
Population_insect::Stage::Stage
(const Parameters_insect::Development  &development_
,const CORN::date32 &simdate_raw
,const Population_insect               &population_
,const Air_temperature_average         &air_temperature_mean_daily_
,const CORN::Dynamic_array<float32>    &air_or_soil_temperature_by_hour_)
: CS::Simulation_element_abstract(simdate_raw)
, param_development        (development_)
, population               (population_)
, air_temperature_mean_daily   (air_temperature_mean_daily_)
, temperature_by_hour      (air_or_soil_temperature_by_hour_)
, development_rate_daily   (0.0)
, mortality_rate_daily     (0.0)
, appearance_date          (simdate_raw)
, days_since_appearance    (0)
, cohort_live              (30,30)
, development_cumulative   (30,30)
{}
//_Stage:constructor________________________________________________2015-03-16_/
bool Population_insect::Stage::matriculate(float32 initial_live)   modification_
{  cohort_live.set(days_since_appearance,initial_live);
   return true;
}
//_matriculate______________________________________________________2015-06-15_/
float64 Population_insect::Stage::reduce_to_fraction(float64 fraction) modification_
{  float64 pop_now = (float64)cohort_live.sum();
   float64 pop_after = (float64)cohort_live.multiply_by(fraction);
   float64 count_removed = pop_now - pop_after;
   return pop_after;
}
//_reduce_to_fraction_______________________________________________2016-01-20_/
bool Population_insect::Stage::start_day()                         modification_
{
   // Calculate Degree-days accumulation using the single-sine method. CLB base temperature = 9C
   development_rate_daily = 0.0;
   mortality_rate_daily   = 0.0;
   for (CORN::Hour hour = 0; hour < 24; hour++)
   {
      float64 air_temperature_hour = temperature_by_hour.get(hour);
      development_rate_daily += calc_development_rate_hourly(air_temperature_hour) / 24.0;
      mortality_rate_daily   += calc_mortality_rate_hourly  (air_temperature_hour) / 24.0;

      //160510 NYI
      // Warning, for diapause we use mortality_fraction dont use morality rate

   }
   return CS::Simulation_element_abstract::start_day();
}
//_Stage::start_day_________________________________________________2015-04-12_/
bool Population_insect::Stage::end_day()                           modification_
{  days_since_appearance  += 1;
   return CS::Simulation_element_abstract::end_day();
}
//_Stage::end_day___________________________________________________2015-05-19_/
Population_insect::Stage *Population_insect::matriculate_to_stage
(Stage_holometabolic_enum stage, nat8 instar, float64 initial_live)   provision_
{  Stage *provided_stage =provide_stage(stage,instar);
   provided_stage->matriculate(initial_live);
   return provided_stage;
}
//_matriculate_to_stage_____________________________________________2015-04-12_/
Population_insect::Stage *Population_insect::provide_stage
(Stage_holometabolic_enum stage,nat8 instar)                       provision_
{  nat32 stage_index = ((nat32)instar << 8) | stage;
   Stage *provided_stage = dynamic_cast<Stage *>(stages.find_nat32(stage_index));
   if (!provided_stage)
   {
      switch (stage)
      {
         case adult_preseason_stage   : provided_stage = new Adult_pre_season    (parameters.stage_development[adult_preseason_stage] ,parameters.adult_pre_season_mortality_rate_coef,parameters.oviposition_rate_coef,simdate_raw,*this,air_temperature_mean_daily,air_temperature_min_daily, air_temperature_by_hour,degree_days_cum); break;
         case ovum_stage              : provided_stage = new Ovum                (parameters.stage_development[ovum_stage]            ,parameters.ova_morality                                          ,simdate_raw,*this,air_temperature_mean_daily,air_temperature_by_hour/*,degree_days_cum*/); break;
         case larva_stage             : provided_stage = new Larva               (instar,parameters.larva_instar_development[instar]  ,parameters.larva_instar_mortality[instar]
                                                                                       ,parameters.larva_feeding_rate[instar] //160222
                                                                                       ,larvae_feeding_daily,simdate_raw,*this,air_temperature_mean_daily,air_temperature_by_hour/*,degree_days_cum*/); break;
         case pupa_stage              : provided_stage = new Pupa                (parameters.stage_development[pupa_stage]            ,parameters.pupa_mortality                                        ,simdate_raw,*this,air_temperature_mean_daily,air_temperature_by_hour/*,degree_days_cum*/); break;
         case adult_postseason_stage  : provided_stage = new Adult_post_season   (parameters.stage_development[adult_postseason_stage],parameters.adult_post_season_mortality_rate_coef                 ,simdate_raw,*this,air_temperature_mean_daily,air_temperature_min_daily,air_temperature_by_hour/*,degree_days_cum*/); break;
         #ifdef USE_DIAPAUSE_STAGE
         case adult_diapause_stage    : provided_stage = new Adult_diapause      (parameters.stage_development[adult_diapause_stage]    /*uses specialized mortality rate calculation*/                 ,simdate_raw,*this,air_temperature_mean_daily,air_temperature_min_daily,soil_temperature_by_hour/*,degree_days_cum*/); break;
         #endif
      }
      provided_stage->start();
      provided_stage->start_day();
      stages.append(provided_stage);
   }
   return provided_stage;
}
//_Stage::provide_stage_____________________________________________2015-04-12_/
const Population_insect::Stage *Population_insect::get_stage
(Stage_holometabolic_enum stage,nat8 instar)                               const
{  nat32 stage_index = (nat32)(instar << 8) | (nat32)stage;
   Stage *got_stage = dynamic_cast<Stage *>(stages.find_nat32(stage_index));
   return got_stage;
}
//_get_stage________________________________________________________2015-06-19_/
bool Population_insect::Stage::process_day()                       modification_
{  nat16 date_index = 0;
   for (CORN::Date_clad_32 date=appearance_date; date <= today; date.inc_day())  //180417
   {  float32 dev = development_cumulative.get(date_index);
      float32 cohort_remaining = cohort_live.get(date_index);
      if (cohort_remaining > 0)
      {  dev += development_rate_daily;
         development_cumulative.set(date_index,dev);
         cohort_remaining *= (1.0 - mortality_rate_daily);
         cohort_remaining = CORN::must_be_0_or_greater<float64>(cohort_remaining); //160222
         cohort_live.set(date_index,cohort_remaining);
      }
      if (dev >= 1.0)
      {  nat8 next_instar = 0;
         Stage_holometabolic_enum next_stage =get_next_stage(next_instar);
          population.matriculate_to_stage(next_stage,next_instar,cohort_remaining);
          cohort_live.set(date_index,0);
      }
      date_index ++;
   }
   return true;
}
//_Stage::process_day_______________________________________________2015-04-12_/
bool Population_insect::Larva::set_metabolism(Metabolism metabolism_)modification_
{  metabolism = metabolism_;
   return true;
}
//_Larva::set_metabolism____________________________________________2016-01-15_/
bool Population_insect::Larva::end_day()                           modification_
{  // this is done at the end of the day, to allow starving larvae to
   // to survive at least one day.
   if (metabolism == catabolic)
   {  // the insects are starving dying out up to about 80% for instar 4
      // later instars are less impacted because they will have built up reserves so should survive longer
      float64 survival_rate = (float64)instar / (float64)(Oulema_melanopus_instars + 1);
          // 1 is 100% survival
      float64 remaining_live = cohort_live.multiply_by(survival_rate);
   }
   return Stage::end_day();
}
//_end_day__________________________________________________________2016-01-15_/
bool Population_insect::Larva::process_day()
{     bool processed = Stage::process_day();
   float64 live = total_live();
   feeding = mm2_to_m2(param_feeding_rate) * live;
   // convert to m2 (divide by 1e6), and multiplied by # larva/m2 ground yields LAI consumed per day
   ref_larvae_feeding_daily += feeding;
   return processed;
}
//_process_day______________________________________________________2015-04-12_/
bool Population_insect::Adult_pre_season::start_day()              modification_
{
   oviposition_rate_daily = 0.0;
   for (CORN::Hour hour = 0; hour < 24; hour++)
   {  float64 temperature_hour = temperature_by_hour.get(hour);
      oviposition_rate_daily += calc_oviposition_rate_hourly(temperature_hour) / 24.0; //'Calculate oviposition rate: Coefficient range 0.18 - 0.7
   }
   static const float64 decay_begin_DD = 220;                                    //160222
   static const float64 decay_end_DD   = 550;                                    //160222
   // K was just to square                                                       //160222
   float64 decay_factor =                                                        //160222
      (degree_days_cum < decay_begin_DD)                                         //160222
      ? 1.0                                                                      //160222
      : (degree_days_cum < decay_end_DD )                                        //160222
         ? 1.0 -                                                                 //160222
            CORN_sqr((degree_days_cum - decay_begin_DD)                          //160222
                      / (decay_end_DD - decay_begin_DD))                         //160222
         : 0.0;                                                                  //160222
   oviposition_rate_daily *= decay_factor;                                       //160222
   eggs_laid_cum_output = CORN::must_be_0_or_greater<float64>(eggs_laid_cum_output); //160216COS
   return Population_insect::Adult::start_day();
}
//_Adult_pre_season::start_day()____________________________________2015-04-12_/
#define before_heavy_winter() (today.get_DOY() < 365)

bool Population_insect::Adult_pre_season::process_day()            modification_
{  bool processed = true;
   // In the Excel version eggs produced during the day applied morality the same day
   // I think mortatilty should not be applied the same day RLN 150621
   if (laying_eggs)                                                              //151210
   {
   float64 total_live_today = total_live();
   eggs_laid_today = oviposition_rate_daily * total_live_today;
   eggs_laid_cum_output += eggs_laid_today;
   if (eggs_laid_today > 0.0)
      population.matriculate_to_stage(CropSyst::ovum_stage,0,eggs_laid_today);
   }
   // Preseason adults do not develop and
   // mortality rate is applied for the entire population
   // not cohort

  // Update preseason adults due to mortality
   if (before_heavy_winter())
   {
      bool has_larvae_instar_1 = population.has_stage(larva_stage,1);
      if (has_larvae_instar_1)
        mortality_rate_daily *= 2.0; //Mortality rate doubles (Switch coefficient from 0.002 to 0.004)
      live_count *= (1.0 - mortality_rate_daily);
   }
   return Adult::process_day();
}
//_Adult_pre_season::process_day____________________________________2015-04-12_/
#ifdef USE_DIAPAUSE_STAGE
bool Population_insect::Adult_diapause::start_day()                modification_
{  bool started = Adult::start_day();
   diapause_end_window.dataset.append(air_temperature_min_daily.Celcius());
   // WARNING not sure if min max or avg temperature
   if (diapause_end_window.dataset.get_count() > diapause_end_window_size) // limit window to 10 days
       diapause_end_window.dataset.remove_at(0);
   return started;
}
//_Adult_diapause::start_day________________________________________2015-12-06_/
bool Population_insect::Adult_diapause::process_day()              modification_
{
// bool processed = true;
   // trigger to bring post season adults out of diapause
   float64 live_count = total_live();                                            //151210
   if ((live_count > 0)
//Don't delete this should be here     && (diapause_end_window.dataset.get_count() == diapause_end_window_size)  //160119
//160120        && (diapause_end_window.get_mean() > 2.0))                                //151210
       && (diapause_end_window.get_count() == 10)
       && (diapause_end_window.get_mean() > 5.0))                                //151210
   {
      // half will be females to start next  preseason adult cohort.
      float64 emerged_preseason_count = live_count / 2.0;
      // half will be females to start next  preseason adult cohort.

      Population_insect::Adult_pre_season *new_preseason =
         dynamic_cast<Population_insect::Adult_pre_season *>
            (population.matriculate_to_stage
               (adult_preseason_stage, 0, emerged_preseason_count));
      // Actually population could probably delete this stage
      new_preseason->begin_laying_eggs(false);
      development_cumulative.clear();
      appearance_date.set(today);
      days_since_appearance = 0;
      cohort_live.clear();   // count / m2 field area

      population.restart_thermal_time_accumulation();
      /*   NYI
         These will emerge but not started laying eggs until the degree_days_cum > parameters.first_egg_deg_days
   they are subject to normal mortality rate
   */
   }
   return Adult::process_day();
}
#endif
//_Adult_diapause::process_day______________________________________2015-12-06_/
bool Population_insect::Adult_post_season::start_day()             modification_
{
#ifdef USE_DIAPAUSE_STAGE
   //diapause_start_window.dataset.append(air_temperature_mean_daily.Celcius());
   diapause_start_window.dataset.append(air_temperature_min_daily.Celcius());
   // WARNING not sure if min max or avg temperature
   if (diapause_start_window.dataset.get_count() > diapause_end_window_size) // limit window to 10 days
       diapause_start_window.dataset.remove_at(0);
#endif
   return Adult::start_day();
}
//_Adult_post_season::start_day_____________________________________2015-12-10_/
bool Population_insect::Adult_post_season::process_day()           modification_
{  nat16 date_index = 0;
   for (CORN::Date_clad_32 date=appearance_date; date <= today; date.inc_day())
   {  float32 cohort_remaining_live = cohort_live.get(date_index);
      cohort_remaining_live *= 1.0 - mortality_rate_daily;
      cohort_live.set(date_index,cohort_remaining_live);
      date_index ++;
   }
   float64 post_season_count = total_live();
   #ifdef USE_DIAPAUSE_STAGE
   if ((post_season_count > 0) && (diapause_start_window.get_mean() < 1.0))
      // param_development.Tbase  // WARNING Not sure what trigger is
      //WARNING dont know what is the threshold temperature
   {

      // all go into diapause
      population.matriculate_to_stage(adult_diapause_stage,0,post_season_count);
      // Actually population could probably delete this stage
      development_cumulative.clear();
      appearance_date.set(today);
      days_since_appearance = 0;
      cohort_live.clear();
      diapause_start_window.clear();
   }
   #else
   if ((post_season_count > 0)
      && (today.get_DOY() == 1)  // for northern hemisphere southren
       )
      // param_development.Tbase  // WARNING Not sure what trigger is
      //WARNING dont know what is the threshold temperature
   {

      // all go into diapause
      population.matriculate_to_stage(adult_preseason_stage,0
         ,post_season_count/2.0);
      // Actually population could probably delete this stage
      development_cumulative.clear();
      appearance_date.set(today);
      days_since_appearance = 0;
      cohort_live.clear();
   }

   #endif
   return true;
}
// 2015-03-16________________________________Adult_post_season::process_day()__/
bool Population_insect::start_year()                               modification_
{
   //151210 reset when diapause ends degree_days_cum = 0;
   // Warning the will only work in Northern hemisphere
   // also I dont know if Jan 1 is necessarily the appropriate reference date.
   return CropSyst::Pest::start_year();;
}
//_start_year_______________________________________________________2015-06-18_/
bool Population_insect::start_day()                                modification_
{
   degree_days_cum += calc_degree_day();

   Population_insect::Adult_pre_season *preseason_adults = 0;
   preseason_adults                                                              //151201
      = dynamic_cast<Population_insect::Adult_pre_season  *>(stages.find_nat32(adult_preseason_stage));
   if ((degree_days_cum >= parameters.first_egg_deg_days)
       && first_population)                                                      //160118
   {
      if (preseason_adults) preseason_adults->reduce_to_fraction(0);             //160218
      preseason_adults                                                           //151201
      = dynamic_cast<Population_insect::Adult_pre_season  *>
      (matriculate_to_stage(adult_preseason_stage,0,parameters.adult_preseason_count_initial));
      first_population = false;                                                  //160118
   }
   if (preseason_adults
          && (preseason_adults->total_live() > 0)
          && !preseason_adults->is_laying_eggs()
          && (degree_days_cum > parameters.first_egg_deg_days ))
         preseason_adults->begin_laying_eggs(true);
//std::clog << "verify stages:" << std::endl;

   FOR_EACH_IN(stage,Stage,stages,stage_iterator)
   {
      if (CORN::is_zero<float64>((double)stage->total_live(),0.0,0.0000001))
      {
//         stage_iterator->detach_current(); //180418

         //  Something is crashing after this point if detaching works,
         //   then I need to figure out what is still being referenced.
         stage_iterator->delete_current();

      }
   } FOR_EACH_END(stage_iterator)
   if (today.get_DOY() ==  // mid winter DOY
      (northern_hemisphere ? 1 : 182) )
      restart();
   bool started = CS::Simulation_element_composite::start_day();
   larvae_feeding_daily = 0;
   return started;
}
//_start_day________________________________________________________2015-04-12_/
bool Population_insect::restart()
{
   first_population = true;
   stages.delete_all();
   matriculate_to_stage(adult_preseason_stage,0,parameters.adult_preseason_count_initial);
   restart_thermal_time_accumulation();
   return true;
}
//_restart__________________________________________________________2016_02-11_/
bool Population_insect::end_day()                                  modification_
{
   #ifdef USE_DIAPAUSE_STAGE
   const Adult_diapause *adjult_diapause = dynamic_cast<const Adult_diapause *>(get_stage(adult_diapause_stage));
   if (adjult_diapause && adjult_diapause->get_days_since_appearance()==1 )
   {  // By the time we reach diapause any other extant stages should should
      // surely have expired.
      terminate_non_adults_diapause();
   }
   #endif
   return CropSyst::Pest::end_day();
}
//_end_day__________________________________________________________2016-01-19_/
float64 Population_insect::Adult_pre_season::calc_oviposition_rate_hourly
(float64 Tmean_hour_Celcius)                                               const
{ // replaced with param_oviposition_rate_coef static const float64 C2 = 0.4; // C2 may be variable?
   float64 oviposition_rate = (Tmean_hour_Celcius - population.parameters.phenology_temperature_base ) * param_oviposition_rate_coef;
   return CORN::must_be_0_or_greater<float64>(oviposition_rate);
}
//_Adult_pre_season::calc_oviposition_rate_hourly2015-03-16____________________/
float64 Population_insect::calc_degree_day()                        calculation_
{  float64 degree_day_cum = 0.0;
   for (CORN::Hour hour = 0; hour < 24; hour++)
   {
      float64 temperature_hour = air_temperature_by_hour.get(hour);
      float64 degree_day =(temperature_hour <= parameters.phenology_temperature_base /*9.0*/)     //RLN I think this should be base temperature not hardcoded 9.0
         ? 0
         : temperature_hour - parameters.phenology_temperature_base;
      degree_day_cum += degree_day / 24.0;
   } // nour
   return degree_day_cum;
}
//_calc_degree_day__________________________________________________2015-03-16_/
float64 Population_insect::Stage::calc_development_rate_hourly(float64 Tmean_hourly_Celcius) const
{
   float64 Tbase     = get_temperature_base();
   float64 Toptimum  = get_temperature_optimum();
   float64 Tmaximum  = get_temperature_maximum();
   float64 development_rate_max = get_development_rate_maximum();
   float64 development_rate = 0.0;
   if (Tmean_hourly_Celcius >= Tbase)
   {
      if (Tmean_hourly_Celcius <= Toptimum)
      development_rate = development_rate_max * ( 1.0 - (Toptimum - Tmean_hourly_Celcius) / ( Toptimum - Tbase));
      else
      {  development_rate = development_rate_max * (Tmaximum - Tmean_hourly_Celcius) / (Tmaximum - Toptimum);
         if (development_rate < 0.0) development_rate = 0.0;
      }
   }
   return development_rate;
}
// 2015-06-04___________________________________________________________________
#ifdef USE_DIAPAUSE_STAGE
float64 Population_insect::Adult_diapause::calc_mortality_fraction_hourly(float64 Tmean_hourly_Celcius) const
{
   // May want to use soil temperature (layer 1?)
   // this temperature will be soil temperature (provided by CropSyst ) not air temperature

   float64 mortality_rate =
      (Tmean_hourly_Celcius <= -15)
      ?0.0000007*exp(-0.602*Tmean_hourly_Celcius)
      :0.0001   *exp(-0.272*Tmean_hourly_Celcius);
   return CORN::must_be_between<float64>(mortality_rate,0.0,1.0);
}
// 2015-08-16___________________________________________________________________
#endif
float64 Population_insect::Adult::calc_mortality_rate_hourly(float64 Tmean_hourly_Celcius) const
{
   float64 mortality_rate =  (Tmean_hourly_Celcius - population.parameters.phenology_temperature_base) * get_mortality_rate_coef(); //160227RLN
   return CORN::must_be_0_or_greater<float64>(mortality_rate);
   // Warning check with Claudio I think should be between 0 and 1
   //return CORN::must_be_between<float64>(mortality_rate,0.0,1.0);
}
// 2015-03-16___________________________________________________________________
float64 Population_insect::Ovum::calc_mortality_rate_hourly(float64 Tmean_hourly_Celcius) const
{  static const float64 B0 = param_mortality.rate_B0; // -5.38779;
   static const float64 B1 = param_mortality.rate_B1; //  0.144906;
   static const float64 B2 = param_mortality.rate_B2; // -0.00244;
   float64 mortality_rate = exp(B0 + B1 * Tmean_hourly_Celcius + B2 * CORN_sqr(Tmean_hourly_Celcius));
   return CORN::must_be_0_or_greater<float64>(mortality_rate);
   // Warning check with Claudio I think should be between 0 and 1
   //return CORN::must_be_between<float64>(mortality_rate,0.0,1.0);
}
//_Ovum::calc_mortality_rate_hourly_________________________________2015-03-16_/
float64 Population_insect::Larva::calc_mortality_rate_hourly(float64 Tmean_hourly_Celcius) const
{  float64 mortality_rate = 0.0;
   if (instar == 1)
   {
      static const float64 B0 = param_mortality.rate_B0; // -5.235233;
      static const float64 B1 = param_mortality.rate_B1; // 0.082856;
      static const float64 B2 = param_mortality.rate_B2; //-0.001767;
      mortality_rate = exp(B0 + B1 * Tmean_hourly_Celcius + B2 * CORN_sqr(Tmean_hourly_Celcius ));
   }
   else
   {
      float64 B0 = param_mortality.rate_B0; // 1;
      float64 B1 =  param_mortality.rate_B1; // 1;
/*
      switch (instar)
      {
         case 2 :
            B0 = -0.050247;
            B1 = 0.00449;
         break;
         case 3 :
            B0 = -0.042993;
            B1 = 0.003922;
         break;
         case 4 :
            B0 = -0.0444958;
            B1 = 0.0044547;
         break;
      } // switch
*/
      mortality_rate = B0 + B1 * Tmean_hourly_Celcius;
   }
   return CORN::must_be_0_or_greater<float64>(mortality_rate);
   // Warning check with Claudio I think should be between 0 and 1
   //return CORN::must_be_between<float64>(mortality_rate,0.0,1.0);
}
//_Larvae::calc_mortality_rate_hourly_______________________________2015-03-16_/
float64 Population_insect::Pupa::calc_mortality_rate_hourly(float64 Tmean_hourly_Celcius)    const
{
   static const float64 f0 = param_mortality.f0;// 0.0169621;
   static const float64 f1 = param_mortality.f1;// -0.0002145;
   static const float64 s0 = param_mortality.s0;// -0.043594;
   static const float64 s1 = param_mortality.s1;// 0.003657;
   static const float64 threshold_temperature = param_mortality.theshold_temperature; // 15.64; //  'C
   float64 mortality_rate =
      (Tmean_hourly_Celcius <= threshold_temperature)
      ? f0 + f1 * Tmean_hourly_Celcius
      : s0 + s1 * Tmean_hourly_Celcius;
   return CORN::must_be_0_or_greater<float64>(mortality_rate);
   // Warning check with Claudio I think should be between 0 and 1
   //return CORN::must_be_between<float64>(mortality_rate,0.0,1.0);

}
//_Pupa::calc_mortality_rate_hourly_________________________________2015-03-16_/
Population_insect::Ovum::Ovum
(const Parameters_insect::Development  &development_
,const Parameters_insect::Mortality_ova&param_mortality_
,const CORN::date32 &simdate_raw
,const Population_insect               &population_
,const Air_temperature_average         &air_temperature_mean_daily_
,const CORN::Dynamic_array<float32>    &air_temperature_by_hour_)
: Stage(development_,simdate_raw,population_,air_temperature_mean_daily_,air_temperature_by_hour_)
, param_mortality (param_mortality_)
{}
//_Egg::constructor_________________________________________________2015-06-10_/
Population_insect::Larva::Larva
(nat8                                instar_
,const Parameters_insect::Development     &param_development_
,const Parameters_insect::Mortality_larva &param_mortality_
,float32 param_feeding_rate_                                                     //160222
,modifiable_ float64                &ref_larvae_feeding_daily_
,const CORN::date32 &simdate_raw
,const Population_insect            &population_
,const Air_temperature_average      &air_temperature_mean_daily_
,const CORN::Dynamic_array<float32> &air_temperature_by_hour_)
: Stage(param_development_,simdate_raw,population_,air_temperature_mean_daily_,air_temperature_by_hour_)
, instar                      (instar_)
, param_mortality             (param_mortality_)
, param_feeding_rate          (param_feeding_rate_)                              //160222
, metabolism                  (anabolic)                                         //160115
, feeding                     (0)
, ref_larvae_feeding_daily    (ref_larvae_feeding_daily_)
{}
//_Larvae::constructor______________________________________________2015-06-10_/
Population_insect::Pupa::Pupa
(const Parameters_insect::Development    &development_
,const Parameters_insect::Mortality_pupa &param_mortality_
,const CORN::date32                 &simdate_raw //180417 ,const CORN::Date &_today
,const Population_insect            &population_
,const Air_temperature_average      &air_temperature_mean_daily_
,const CORN::Dynamic_array<float32> &air_temperature_by_hour_)
: Stage
   (development_,simdate_raw,population_
   ,air_temperature_mean_daily_,air_temperature_by_hour_)
,param_mortality     (param_mortality_)
{}
//_Pupa::constructor________________________________________________2015-06-10_/
Population_insect::Adult::Adult
(const Parameters_insect::Development  &development_
,float32                                param_mortality_rate_coef_
,const CORN::date32                    &simdate_raw
,const Population_insect            &population_
,const Air_temperature_average      &air_temperature_mean_daily_
,const Air_temperature_minimum      &air_temperature_min_daily_
,const CORN::Dynamic_array<float32> &temperature_by_hour_)
: Stage(development_,simdate_raw,population_,air_temperature_mean_daily_,temperature_by_hour_)
,param_mortality_rate_coef(param_mortality_rate_coef_)
,air_temperature_min_daily(air_temperature_min_daily_)
{}
//_Adult_pre_season::constructor____________________________________2015-06-10_/
Population_insect::Adult_pre_season::Adult_pre_season
(const Parameters_insect::Development  &development_
,float32                                param_mortality_rate_coef_
,float32                                param_oviposition_rate_coef_
,const CORN::date32                 &simdate_raw
,const Population_insect            &population_
,const Air_temperature_average      &air_temperature_mean_daily_
,const Air_temperature_minimum      &air_temperature_min_daily_
,const CORN::Dynamic_array<float32> &air_temperature_by_hour_
,const float64                      &degree_days_cum_                             //160222
)
: Adult(development_,param_mortality_rate_coef_,simdate_raw,population_
   ,air_temperature_mean_daily_
   ,air_temperature_min_daily_
   ,air_temperature_by_hour_)
,param_oviposition_rate_coef(param_oviposition_rate_coef_)
,oviposition_rate_daily(0)
,laying_eggs            (false)                                                  //151210
,eggs_laid_today        (0)
,eggs_laid_cum_output   (0)
,degree_days_cum        (degree_days_cum_)                                       //160222
,live_count             (0)
{}
//_Adult_pre_season::constructor____________________________________2015-06-10_/
#ifdef USE_DIAPAUSE_STAGE
Population_insect::Adult_diapause::Adult_diapause
(const Parameters_insect::Development  &development_
,const CORN::date32 &simdate_raw
,const Population_insect            &population_
,const Air_temperature_average      &air_temperature_mean_daily_
,const Air_temperature_minimum      &air_temperature_min_daily_
,const CORN::Dynamic_array<float32> &soil_temperature_by_hour_)
: Adult(development_,999.99,simdate_raw,population_,air_temperature_mean_daily_
,_air_temperature_min_daily
   ,soil_temperature_by_hour_)
{}
#endif
//_Adult_diapause::constructor______________________________________2015-12-10_/
Population_insect::Adult_post_season::Adult_post_season
(const Parameters_insect::Development  &development_
,float32                                param_mortality_rate_coef_
,const CORN::date32                    &simdate_raw
,const Population_insect               &population_
,const Air_temperature_average         &air_temperature_mean_daily_
,const Air_temperature_minimum         &air_temperature_min_daily_
,const CORN::Dynamic_array<float32>    &temperature_by_hour_)
: Adult(development_,param_mortality_rate_coef_,simdate_raw,population_
   ,air_temperature_mean_daily_
   ,air_temperature_min_daily_,temperature_by_hour_)
{}
//_Adult_post_season::constructor___________________________________2015-06-10_/
Stage_holometabolic_enum Population_insect::Ovum::get_next_stage(nat8 &next_instar) const
{
   Stage_holometabolic_enum next_stage = larva_stage;
   next_instar = 1;
   next_stage = larva_stage;
   return next_stage;
}
//_Egg::get_next_stage______________________________________________2015-06-19_/
Stage_holometabolic_enum Population_insect::Larva::get_next_stage(nat8 &next_instar) const
{
   Stage_holometabolic_enum next_stage = larva_stage;
   if (instar == population.parameters.instars)
   {  next_instar = 0;
      next_stage = pupa_stage;
   } else
   {  next_instar = instar + 1;
   }
   return next_stage;
}
//_Larvae::get_next_stage___________________________________________2015-06-19_/
bool Population_insect::set_metabolism_larval(Metabolism metabolism) modification_
{  bool sat = true;
   FOR_EACH_IN(stage,Stage,stages,each_stage)
   {  Larva *larva = dynamic_cast<Larva *>(stage);
      if (larva)
         sat &= larva->set_metabolism(metabolism);
   }  FOR_EACH_END(each_stage)
   return sat;
}
//_set_metabolism_larval____________________________________________2016-01-15_/
bool Population_insect::respond_to_harvest()             modification_
{
      // This is used to respond to harvest event
   FOR_EACH_IN(stage,Stage,stages,each_stage)
   {
      Adult_post_season *as_adult_post_season_stage = dynamic_cast<Adult_post_season *>(stage);
      Pupa  *as_pupa_stage  = dynamic_cast<Pupa *>(stage);
      if (!as_adult_post_season_stage && !as_pupa_stage)
         stage->reduce_to_fraction(0.0);

   } FOR_EACH_END(each_stage)
   degree_days_cum = 0;
      // need to clear otherwise preseason adults will be generated
   return true;
}
//_respond_to_harvest_______________________________________________2016-01-16_/
#ifdef USE_DIAPAUSE_STAGE
bool Population_insect::terminate_non_adults_diapause()            modification_
{
   FOR_EACH_IN(stage,Stage,stages,each_stage)
   {
      Adult_diapause *as_adult_diapause_stage = dynamic_cast<Adult_diapause *>(stage);
      if (!as_adult_diapause_stage)
         stage->reduce_to_fraction(0.0);
         //160220 stage->terminate_all();
   } FOR_EACH_END(each_stage)
   degree_days_cum = 0;
      // need to clear otherwise preseason adults will be generated
//   population_at_diapause_cleared = true;
   return true;
}
//_terminate_non_adults_diapause____________________________________2016-01-17_/
#endif
struct CropSyst::Parameters_insect parameters_Oulema_melanopus =
{
    10    // adult_preseason_count_initial
  , 7.0 // 160222 9.0  // phenology_temperature_base
  ,115.0 /* 160222 80.0*/  // first_egg_deg_days
  , 0.35 /*160227 0.25 */ /*160222 0.4*/  // oviposition_rate_coef  (ranges 0.1830 to 0.7 in literature)
  , 0.0028 /*160227COS 0.002*/  // adult_pre_season_mortality_rate_coef
  , 0.0007 /*160227COS 0.0005*/ // adult_post_season_mortality_rate_coef
  , 4    //instars
  ,{ //stage_development
     //Tbase Topt Tmax DRmax
      { NA  , NA  , NA  , NA }  // not stage
     ,{  7.0 /*160222 9.0*/, 29.0, 35.0 /*34.0*/, 0.27} // egg       base was 11
     ,{  7.0 /*160222 9.0*/, 32.0, 35.0 /*36.0*/, 0.65} // Larva     (obsolete)
     ,{  7.0 /*160222 9.0*/, 30.0, 35.0 /*34.0*/, 0.1}  // Pupa       base was 10.5
     ,{ NA  , NA  , NA  , NA }  // Adults have no further development
     ,{ NA  , NA  , NA  , NA }  // Adults have no further development
   }
  ,{ // larva_instar_development
     //Tbase  Topt  Tmax DRmax
      {  NA ,   NA,   NA,  NA } // no instar 0
     ,{  7.0 /*160222 9.0*/, 32.0, 35.0 /*36.0*/, 0.65} // instar1
     ,{  7.0 /*160222 9.0*/, 30.0, 35.0 /*36.0*/, 0.75} // instar2
     ,{  7.0 /*160222 9.0*/, 29.0, 35.0 /*36.0*/, 0.6}  // instar3
     ,{  7.0 /*160222 9.0*/, 29.0, 35.0 /*34.0*/, 0.5}  // instar4
   }
  ,{ NA , 10.0, 22.0, 66.0, 152.0 }                                              //160222
    // B0 B1 B2
  ,{  -5.38779,0.144906, -0.00244 }  //ova_morality
  ,{ // larva_instar_mortality
     // B0 B1 B2
      {        NA,      NA , NA}       // no instar 0
     ,{-5.235233 ,0.082856 ,-0.001767} //instar1
     ,{-0.050247 ,0.00449  ,NA }       //instar2
     ,{-0.042993 ,0.003922 ,NA }       //instar3
     ,{-0.0444958,0.0044547,NA }       //instar4
   }
  ,{ // pupa_mortality
     // threshold, f0 , f1, s0, s1
     15.64,0.0169621,-0.0002145, -0.043594,0.003657
   }
};
//_parameters_Oulema_melanopus_________________________________________________/
bool Parameters_insect_data_record::expect_structure(bool for_write)
{
   set_current_section("adult_preseason");
      expect_float32("count_initial",parameters.adult_preseason_count_initial);
   set_current_section("egg");
      expect_float32("first_deg_days",parameters.first_egg_deg_days);
   return CORN::Data_record::expect_structure(for_write);                        //180417
}
//_expect_structure_________________________________________________2016-01-27_/
/*151229 NYI
//______________________________________________________________________________
CORN::Parameter_properties_literal PP_live_count={"live_count","count","live","live count","hint_NYN","URL_NYN",UC_count,"count"};
RENDER_INSPECTORS_DEFINITION(Population_insect::Stage)
{
   KNOW_EMANATOR;
   inspectors.append(new CS::Inspector
      (cohort_live.ref_summation()
      ,emanator
      ,PP_live_count,UC_count
      ,inspectors.desired_descriptive_summary_elements&(STAT_sum_bit|STAT_value_bit),sum_statistic));
   return emanator;
}
//______________________________________________________________________________
CORN::Parameter_properties_literal PP_feed_rate={"feeding_rate","rate","feeding","feeding rate","hint_NYN","URL_NYN",UC_mm2,"mm�/larva/day"};
RENDER_INSPECTORS_DEFINITION(Population_insect::Larva)
{
   KNOW_EMANATOR;
   inspectors.append(new CS::Inspector
      (feeding_rate_output
      ,emanator
      ,PP_feed_rate,UC_mm2
      ,inspectors.desired_descriptive_summary_elements&(STAT_sum_bit|STAT_value_bit),sum_statistic));
   return emanator;
}
//______________________________________________________________________________
CORN::Parameter_properties_literal PP_eggs_laid={"eggs_laid","laid","eggs","eggs laid" ,"hint_NYN","URL_NYN",UC_count,"eggs"};
RENDER_INSPECTORS_DEFINITION(Population_insect::Adult_pre_season)
{
   const CS::Emanator &emanator = Adult::render_inspectors(inspectors,context,"adult_pre_season",statistic);
   inspectors.append(new CS::Inspector
      (eggs_laid_today
      ,emanator
      ,PP_eggs_laid,UC_count
      ,inspectors.desired_descriptive_summary_elements&(STAT_sum_bit|STAT_value_bit),sum_statistic));
   return emanator;
}
//______________________________________________________________________________
CORN::Parameter_properties_literal PP_feeding_LAI={"feeding_LAI","LAIfeed","LAIfeed" ,"feeding leaf area index" ,"hint_NYN","URL_NYN",UC_index,"m�/m�"};
RENDER_INSPECTORS_DEFINITION(Population_insect)
{
   KNOW_EMANATOR;
   inspectors.append(new CS::Inspector
      (larvae_feeding_daily
      ,emanator
      ,PP_feeding_LAI,UC_index
      ,inspectors.desired_descriptive_summary_elements&(STAT_sum_bit|STAT_value_bit),sum_statistic));
   FOR_EACH_IN(stages,Stage,stages,each_stage)
   {
      stage->render_inspectors(inspectors,&emanator,stage->label(),statistic);
   } FOR_EACH_END(each_stage)
   return emanator;
}
//_RENDER_INSPECTORS_DEFINITION(Population_insect)__________________2015-12-29_/
*/

bool Population_insect::dump
(std::ostream &population_file
,std::ostream &feeding_file)                                               const
{

         CORN::DOY doy = today.get_DOY();
         CORN::Year year = today.get_year();

         const Population_insect::Ovum             *ovum             = dynamic_cast<const Population_insect::Ovum             *>(get_stage(ovum_stage));
         const Population_insect::Larva            *larva_instar_1   = dynamic_cast<const Population_insect::Larva            *>(get_stage(larva_stage,1));
         const Population_insect::Larva            *larva_instar_2   = dynamic_cast<const Population_insect::Larva            *>(get_stage(larva_stage,2));
         const Population_insect::Larva            *larva_instar_3   = dynamic_cast<const Population_insect::Larva            *>(get_stage(larva_stage,3));
         const Population_insect::Larva            *larva_instar_4   = dynamic_cast<const Population_insect::Larva            *>(get_stage(larva_stage,4));
         const Population_insect::Pupa             *pupa             = dynamic_cast<const Population_insect::Pupa             *>(get_stage(pupa_stage));
         const Population_insect::Adult_pre_season *adult_pre_season = dynamic_cast<const Population_insect::Adult_pre_season *>(get_stage(adult_preseason_stage));
         const Population_insect::Adult_post_season*adult_post_season= dynamic_cast<const Population_insect::Adult_post_season*>(get_stage(adult_postseason_stage));
         #ifdef USE_DIAPAUSE_STAGE
         const Population_insect::Adult_diapause   *adult_diapause   = dynamic_cast<const Population_insect::Adult_diapause*>(get_stage(adult_diapause_stage));
         #endif

         // NYI diapause

         float64 laid_ova_cum             = (adult_pre_season) ? adult_pre_season->eggs_laid_cum_output : 0.0;
         float64 live_ova                 = (ovum)             ? ovum           ->total_live()  : 0.0;
         float64 live_larvae_instar_1     = (larva_instar_1)   ? larva_instar_1->total_live()   : 0.0;
         float64 live_larvae_instar_2     = (larva_instar_2)   ? larva_instar_2->total_live()   : 0.0;
         float64 live_larvae_instar_3     = (larva_instar_3)   ? larva_instar_3->total_live()   : 0.0;
         float64 live_larvae_instar_4     = (larva_instar_4)   ? larva_instar_4->total_live()   : 0.0;
         float64 live_pupae               = (pupa)             ? pupa->total_live()             : 0.0;
         float64 live_adults_pre_season   = (adult_pre_season) ? adult_pre_season->total_live() : 0.0;
         float64 live_adults_post_season  = (adult_post_season)? adult_post_season->total_live(): 0.0;
         #ifdef USE_DIAPAUSE_STAGE
         float64 live_adults_diapause     = (adult_diapause)   ? adult_diapause->total_live(): 0.0;
         #endif

         float64 live_larvae_total = (live_larvae_instar_1 +live_larvae_instar_2 + live_larvae_instar_3 + live_larvae_instar_4 );

         population_file
            << year                                << '\t'
            << doy                                 << '\t'
            << air_temperature_mean_daily.Celcius()<< '\t'
            << get_degree_days_cum()   << '\t'
            << live_adults_pre_season              << '\t'
            << laid_ova_cum                        << '\t'
            << live_ova                            << '\t'
            << live_larvae_instar_1                << '\t'
            << live_larvae_instar_2                << '\t'
            << live_larvae_instar_3                << '\t'
            << live_larvae_instar_4                << '\t'
            << live_pupae                          << '\t'
            << live_adults_post_season             << '\t'
            #ifdef USE_DIAPAUSE_STAGE
            << live_adults_diapause                << '\t'
            #else
            << 0.0                << '\t'
            #endif
            << get_larvae_feeding_daily()    << '\t'
            << live_larvae_total
            << std::endl;

         float64 feedrate_instar_1 = (larva_instar_1) ? larva_instar_1->get_feeding_rate() : 0.0;
         float64 feedrate_instar_2 = (larva_instar_2) ? larva_instar_2->get_feeding_rate() : 0.0;
         float64 feedrate_instar_3 = (larva_instar_3) ? larva_instar_3->get_feeding_rate() : 0.0;
         float64 feedrate_instar_4 = (larva_instar_4) ? larva_instar_4->get_feeding_rate() : 0.0;
         feeding_file
            << year                                << '\t'
            << doy                                 << '\t'
            << air_temperature_mean_daily.Celcius()<< '\t'
            << live_larvae_instar_1                << '\t'
            << live_larvae_instar_2                << '\t'
            << live_larvae_instar_3                << '\t'
            << live_larvae_instar_4                << '\t'
            << feedrate_instar_1                   << '\t'
            << feedrate_instar_2                   << '\t'
            << feedrate_instar_3                   << '\t'
            << feedrate_instar_4                   << '\t'
            << get_larvae_feeding_daily()
            << std::endl;

   return true;
}
//_dump_____________________________________________________________2015-12-29_/
}//_namespace CropSyst_________________________________________________________/

