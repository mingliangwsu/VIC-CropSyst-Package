#include "crop/crop_types.h"
#include "crop/crop_fruit.h"
#include "crop/crop_param.h"
#include "crop/canopy_growth.h"
#include "crop/crop_orchard_common.h"
#include "common/weather/hour/weather_hours.h"
#include "common/weather/weather.h"
#include "crop/phenology_I.h"
#include "crop/growth_stages.h"
#include "CS_suite/observation/CS_observation.h"
#include "static_phrases.h"
#include "CS_suite/observation/CS_inspector.h"
#include "csvc.h"

namespace CropSyst {
//______________________________________________________________________________
float32 Fruit_chill_requirement::add_hour_temperature(float64 temperature)
{  // convert temperature to chill units
   float64 chill_units
   =  (temperature < 1.4) ? 0.0
   :  (temperature < 2.4) ? 0.5
   :  (temperature < 9.1) ? 1.0
   :  (temperature <12.4) ? 0.5
   :  (temperature <15.9) ? 0.0
   :  (temperature <18.0) ?-0.5
   :                       -1.0;
   //deduct chill units from chill_hours remaining.
   chill_hours_remaining -= chill_units;
   //190709 should return something, (bool or chill_units, or chill_hours_remaining)
   return (float32)chill_units;                                                  //190709
}
//_add_hour_temperature_____________________________________________2002-11-24_/
bool Fruit_bud_break_requirement::is_satisfied
(bool chill_req_satisfied,float32 accum_deg_days)                   affirmation_ //190708
{  bool sat = chill_req_satisfied
   ?  accum_deg_days >= deg_day_bud_break_chill_req_sat
   :  accum_deg_days >= deg_day_bud_break_chill_req_not_sat;
   return sat;
}
//_Fruit_bud_break_requirement:is_satisfied_________________________2003-05-15_/
Fruit_harvest_requirements::Fruit_harvest_requirements(const CropSyst::Crop_parameters::Fruit &_parameters)
: parameters(_parameters)
, consecutive_days_below_threshold_temperature(0)
{}
//_Fruit_harvest_requirements_______________________________________2008-12-04_/
bool Fruit_harvest_requirements::is_satisfied
(DOY doy,float64 min_temp,float64 avg_temp)                         affirmation_
{  bool result = false;
   if (doy == parameters.harvest_DOY_latest)
      result = true; // we have reached the lastest date for harvest so force harvest.
   else
   switch (parameters.harvest_condition_mode)
   {  case management_scheduled_harvest      :  { result =  false; } break;
         // Harvest should be picked up by a management operation
      case consider_fixed_date_harvest       :
         { if (doy == parameters.harvest_DOY_fixed) result = true; }break;
      case consider_min_temperature_harvest  :
      {  if (   (doy >= parameters.harvest_DOY_earliest)
             && (doy <= parameters.harvest_DOY_latest)
             && (min_temp <= parameters.harvest_temperature))
         {  consecutive_days_below_threshold_temperature += 1;
            if (consecutive_days_below_threshold_temperature
                >= (CORN::Days)parameters.harvest_temperature_days)
               result = true;
         } else  consecutive_days_below_threshold_temperature = 0;
      } break;
      case consider_avg_temperature_harvest  :
      { if (   (doy >= parameters.harvest_DOY_earliest)
            && (doy <= parameters.harvest_DOY_latest)
            && (avg_temp <= parameters.harvest_temperature))
         {  consecutive_days_below_threshold_temperature += 1;
            if (consecutive_days_below_threshold_temperature
                >= (CORN::Days)parameters.harvest_temperature_days)
               result = true;
         } else  consecutive_days_below_threshold_temperature = 0;
      } break;
   }
   return result;
}
//_Fruit_harvest_requirements:is_satisfied______________2008-12-04__2019-07-08_/
Crop_orchard_fruit::Crop_orchard_fruit
(CropSyst::Crop_parameters              *parameters_                             //020409
,float64                                param_dry_soil_root_activity_coef_       //170518
,float64                                param_saturated_soil_root_activity_coef_ //170518
,bool                                   owns_parameters_                         //040121
,const CORN::date32                    &simdate_                                 //170525
,CORN::Unidirectional_list             &performable_operations_                  //151004
,const CS::CO2_atmospheric_concentration *atmospheric_CO2_conc_ppm_optional      //180122
,modifiable_ Crop_CO2_response         *Crop_CO2_response_given_optional_        //151201
,const Air_temperature_maximum         &air_temperature_max_                     //151201
,const Air_temperature_minimum         &air_temperature_min_                     //151201
,const Air_temperature_average         &air_temperature_avg_                     //151201
,const Solar_radiation                 &solar_radiation_                         //151201
,const Vapor_pressure_deficit_daytime  &vapor_pressure_deficitdaytime_           //151201
,const Vapor_pressure_deficit_max      &vapor_pressure_deficit_max_              //151201
,const Psychrometric_constant          &_psychrometric_constant                  //140508
,const float64                         &_aerodynamic_resistance_ref_plants       //140508
,const Sun                             &_sun                                     //151128
,const Weather_spanning_interface      &_weather                                 //151201_130901
//190619 unused ,Weather_provider                      &_weather_provider        //110826
//190619 unused ,CS::Weather_hours                     &_hourly_weather          //080908
,Soil::Soil_interface                  *soil_                                    //181206_050721
,Soil::Nitrogen                        *soil_chem_                               //050721
,modifiable_ Soil::Salt_interface      *soil_salinity_                           //181206_051118
,Crop_nitrogen_model                    nitrogen_model_                          //050722
#ifdef NITROGEN
,const Slope_saturated_vapor_pressure_plus_psychrometric_resistance              //referenced
      *slope_saturated_vapor_pressure_plus_psychrometric_resistance_reference
//190619 unused ,ET_reference_interface       *ET_
#endif
,const CS::Seasons &seasons_ref                                                  //190705
,Orchard_interface *orchard_optional)                                            //071121
:Crop_complete
   (parameters_
   ,param_dry_soil_root_activity_coef_                                           //170518
   ,param_saturated_soil_root_activity_coef_                                     //170518
   /*190613
   #ifdef OLD_ORCHARD
   ,_orchard
   #endif
   */
   ,owns_parameters_
   ,simdate_                                                                     //110824
   ,performable_operations_                                                      //151005
   ,atmospheric_CO2_conc_ppm_optional                                            //180122
   ,Crop_CO2_response_given_optional_                                            //151201
   ,air_temperature_max_                                                         //151201
   ,air_temperature_min_                                                         //151201
   ,air_temperature_avg_                                                         //151201
   ,solar_radiation_                                                             //151201
   ,vapor_pressure_deficitdaytime_                                               //151201
   ,vapor_pressure_deficit_max_                                                  //151201
   ,_psychrometric_constant                                                      //140508
   ,_weather
   ,_sun.ref_sun_rise_time()                                                     //151128_150907
   ,day_length_hours_unused                                                      //140812
   ,0 // _seeding_parameters not applicable to fruit tree
   ,soil_
   ,nitrogen_model_
   #ifdef NITROGEN
   ,slope_saturated_vapor_pressure_plus_psychrometric_resistance_reference       //150602
   #endif
   )
, tolerance_parameters                                  (parameters_->tolerance) //190616
, fruit_parameters_damage_adjusted                                           (0) //200608
, max_fruit_dry_biomass                                                   (9999) //200608
, latest_harvest_fruit_dry_biomass                                         (0.0) //200609
, max_fruit_load_achieved                                                (false) //200709
, fruit_dry_biomass                                                        (0.0)
, reserves_biomass                                                         (0.0) //071117
   //200603obs, fruit_root_turnover_biomass                                (0.0) //071117
, initial_fruit_growth_date                                                  (0) //170525_030521
, veraison_date                                                              (0) //170525_030521
, rapid_fruit_growth_date                                                    (0) //170525_030521
, bud_break_date                                                             (0) //170525_081013
, chill_requirement_satisfied_date                                           (0) //170525_081013
, fruit_chill_requirement                                                    (0) //021125
   // will be set at end of dormancy
, fruit_bud_break_requirement                                                (0) //021125
   // will be set when chill requiment satisfied.
, fruit_harvest_requirements                                                 (0) //200903
, limit_max_LAI                                                          (false) //081103
, harvested                                                              (false) //200603
, seasons                                                          (seasons_ref) //190705
, orchard                                                     (orchard_optional) //071121
{}
//_Crop_orchard_fruit:constructor______________________________________________/
Crop_orchard_fruit::~Crop_orchard_fruit()
{  delete fruit_chill_requirement;
   delete fruit_bud_break_requirement;
   delete fruit_harvest_requirements;                                            //081204
}
//_Crop_orchard_fruit:destructor_______________________________________________/
bool Crop_orchard_fruit::initialize()
{  bool inited = Crop_complete::initialize();                                    //190621
   linger_label.chill_satisfied  = -1;                                           //200420
   linger_label.budbreak         = -1;                                           //200420
   linger_label.veraison         = -1;                                           //200420
   fruit_dry_biomass             = 0;                                            //000616
   reserves_biomass              = 0;                                            //071117
   //200603obs fruit_root_turnover_biomass   = 0;                                //071117
   initial_fruit_growth_date    .clear();
   veraison_date                .clear();
   rapid_fruit_growth_date      .clear();
   bud_break_date               .clear();
   chill_requirement_satisfied_date.clear();
   yield = new Crop_orchard_fruit::Yield_model(fruit_dry_biomass);               //170221
   return inited;                                                                //190621
}
//_initialize_______________________________________________________2007-11-20_/
bool Crop_orchard_fruit::start()
{
   fruit_parameters_damage_adjusted                                              //081113
      = new CropSyst::Crop_parameters::Fruit(parameters->fruit);
      // Must be instanciated before calling Crop_complete::start
   float64 max_fruit_load_kg_m2 = per_ha_to_per_m2
      (fruit_parameters_damage_adjusted->max_fruit_load_kg_ha);
   max_fruit_dry_biomass // This really only needs to be computed once           //200608
      = (max_fruit_load_kg_m2
        * fruit_parameters_damage_adjusted->fract_total_solids);

   //190707 moved to create_roots
   //   roots_vital->root_length       = parameters->root.max_depth;             //061212

   //190701 Dont activate activate_quiescence because this resets counters
   // we start in quiescence, but we dont know how long its been

   //190705    //assuming fruit simulations start jan 1
   if (seasons.is_winter_time())                                                 //190705
   {  // Assume in quiescence/dormancy

      // Note that the fruit crop normally uses chill requirements to break dormancy
      // but in the first year we may be starting the simulation in the middle
      // of winter so fruit chill requirement may not be satisfied in the first
      // year so also providing simple quiescence condition.
      // subsequent years/seasons should used chill requirements.
      quiescence_end_requirement = new CropSyst::Quiescence_end_requirement      //160326
         ((parameters->quiescence));                                             //160326
      // may want to make roots dormant here //190707


      //#ifdef FRUIT_CROP_MODEL
      // fruit bud break requirements check must not start until dormancy ends   //080908
      fruit_chill_requirement = new Fruit_chill_requirement
         (parameters->dormancy.chill_hours);                                     //030529
      //#endif
   }
   //190708 moved to Crop_complete::start                                        //190707
   //190708    plant_height      = parameters->morphology.max_canopy_height;     //110218_990213
      //abandoned phenology.established_crop();                                  //190701
   // 190701 The crop is alreay dormant/inactive we don't want to reset counter
   //190701       trigger_synchronization(NGS_DORMANT_or_INACTIVE);              //030516

   // must call Crop_complete::start() last because
   return Crop_complete::start();
      // Not sure why I didn;t call super start() before.
}
//_start____________________________________________________________2007-11-20_/
/*2005??
bool Crop_orchard_fruit::start_season()
{  // This is common both to restart (after dormancy)
   // the the apparent start of the season when the crop does not have dormancy.
   bool started = Crop_complete::start_season();
   phenology.restart_season();                                                   //200507
   return started;
};
//_start_season_____________________________________________________2020-04_01_/
*/
bool Crop_orchard_fruit::restart_after_inactive_period()
{
   delete fruit_parameters_damage_adjusted;                                      //081113
   fruit_parameters_damage_adjusted                                              //081113
         = new CropSyst::Crop_parameters::Fruit(parameters->fruit);
   delete fruit_bud_break_requirement; fruit_bud_break_requirement = 0;          //200402
   delete fruit_chill_requirement;     fruit_chill_requirement     = 0;          //200402
   bool restarted = Crop_complete::restart_after_inactive_period();
   //moved to start_season harvestable = false;                                  //190710
   initial_fruit_growth_date  .clear();                                          //030521
   veraison_date              .clear();                                          //030521
   rapid_fruit_growth_date    .clear();                                          //030521
   float64 deg_days_begin_canopy_development =
      #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
      thermal_time->get_accum_degree_days();                                     //200211
         //200201 would always be normal because we don't click fruit tree (false);                                //071121
      #endif
      #if ((PHENOLOGY_VERSION==2018))
      phenology.ref_accum_deg_days_normal();                                     //181118
      #endif
   if (orchard) orchard->set_deg_days_begin_canopy_development                   //190702
      (deg_days_begin_canopy_development);                                       //071121
   /*200401 moved to start_season
      fruit_parameters_damage_adjusted                                           //081113
         = new CropSyst::Crop_parameters::Fruit(parameters->fruit);
   */
   limit_max_LAI = false;                                                        //081101
   max_fruit_load_achieved = false;                                              //200709

   {  // For normal crops the root is updated every day.                         //200402
      // For fruit crops roots are considered constant so this update is only done at the start of the season
      float64 canopy_biomass_vital = 2.5;
         //this is just an anticipated biomass to get realistic roots
         // that will be used throughout the season
      roots_vital->know_above_ground_biomass(canopy_biomass_vital);
         // May beable to reference the canopy biomass
         // it is not too important to roots(output only?)
         // so this would have lower overhead
      roots_vital->update(get_water_stress(),terminate_pending);                 //191021_061208
   }
   harvested = false;                                                            //200603
   reserves_biomass = 0;                                                         //200603
   return restarted;                                                             //190621
}
//_restart_____________________________________________________________________/
bool Crop_orchard_fruit::setup_quiescence_end_requirement()
//200402 (CORN::DOY doy)
{  // doesn't actually have quiescence_end_requirement
   // Instead uses fruit_chill_requirement
   if (fruit_chill_requirement)      // Should not happen
      delete fruit_chill_requirement;// but just in case
   fruit_chill_requirement = new Fruit_chill_requirement
      (parameters->dormancy.chill_hours);
   return true;
}
//_setup_inactive_period_end_requirement_________________2007-11-20_2003-05-14_/
bool Crop_orchard_fruit::initiate_fruit_growth()
{
   harvestable = false;                                                          //190710
   fruit_dry_biomass = 0.0;                                                      //000616
   phenology.activate_fructescence();                                            //181109
   log_event("Begin initial fruit growth");
   initial_fruit_growth_date.set(simdate);                                       //020809
   trigger_synchronization(FGS_FRUCTESCENCE);
   return true;                                                                  //140619
}
//_initiate_fruit_growth____________________________________________2003-05-21_/
bool Crop_orchard_fruit::initiate_veraison()
{  linger_label.veraison = 2;
   trigger_synchronization(FGS_VERAISON);
   phenology.activate_veraison();                                                //181109
   log_event(TL_Begin_veraison);
   return true;                                                                  //140619
}
//_initiate_veraison___________________________________________________________/
bool Crop_orchard_fruit::initiate_rapid_fruit_growth()
{
   if (parameters->fruit.grape)                                                  //030521
      initiate_veraison();                                                       //030521
   phenology.activate_rapid_fruit_development();                                 //181109
   log_event("Begin rapid fruit growth");
   rapid_fruit_growth_date.set(simdate);                                         //020809
   trigger_synchronization(FGS_RAPID_FRUIT_DEVELOPMENT);


//assert(!fruit_harvest_requirements);
delete fruit_harvest_requirements;

    fruit_harvest_requirements
                  = new Fruit_harvest_requirements(parameters->fruit);

   return true;                                                                  //140619
}
//_initiate_rapid_fruit_growth______________________________________2003-05-21_/
Crop_orchard_fruit::Yield_model::Yield_model(float64 &fruit_dry_biomass_)
: CropSyst::Yield_model()
, fruit_dry_biomass(fruit_dry_biomass_)
{}
//_Crop_orchard_fruit::Yield::constructor___________________________2019-02-24_/
float64 Crop_orchard_fruit::Yield_model::calculate
   (float64 canopy_biomass_kg_m2)                                   calculation_
{  float64 yield_now   = fruit_dry_biomass;                                      //080912
      // Now we stop partition when max fruit load is reached
   fruit_dry_biomass = 0;  // clear for successive years                         //000616
   return  yield_now;
}
//_Crop_orchard_fruit::Yield_model::calculate_______________________2017-02-20_/
bool Crop_orchard_fruit::thermal_time_event()
{
   if (phenology.is_in_quiescence()) return true;                                //181109
      #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
      if (thermal_time_matchs(parameters->phenology.initiation.filling/*200211fruit trees have no clipping,true*/))             initiate_fruit_growth();        //030521
      if (thermal_time_matchs(parameters->phenology.initiation.rapid_fruit_growth/*200211fruit trees ,false*/)) initiate_rapid_fruit_growth();  //030521
      #endif
      #if ((PHENOLOGY_VERSION==2018))
      if (phenology.was_realized("phenology/fructescence"))       initiate_fruit_growth();        //181118_030521
      if (phenology.was_realized("phenology/rapid_fruit_growth")) initiate_rapid_fruit_growth();  //181118_030521
      #endif
   return Crop_complete::thermal_time_event();
}
//_thermal_time_event_______________________________________________1997-05-14_/
float64 Crop_orchard_fruit::get_solid_canopy_interception()                const
{  return orchard                                                                //190702
      ? orchard->get_solid_canopy_interception()
      : Crop_complete::get_solid_canopy_interception();                          //190702
}
//_get_solid_canopy_interception____________________________________2019-06-13_/
void Crop_orchard_fruit::update_lingers()                          modification_ // actually aguments adjucts
{  Crop_complete::update_lingers();                                              //200420
   linger_label.budbreak--;                                                      //200420_030521
   linger_label.veraison--;                                                      //200420_030521
   linger_label.chill_satisfied--;                                               //200420_030529
}
//_update_lingers___________________________________________________2007-11-20_/
bool Crop_orchard_fruit::consider_inactive_period()                              //200402_020626
//200402 (float64 avg_temperature)                                                        //021202
// originally daily temp with est. night time temps.
{  bool considered = true;
   if (fruit_bud_break_requirement)                                              //021125
   { if (fruit_bud_break_requirement->is_satisfied
         (!fruit_chill_requirement
         #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
         ,thermal_time->get_accum_degree_days()                                 //200201
            //200211 would always be normal because we dont clip fruit trees(false)
         #endif
         #if ((PHENOLOGY_VERSION==2018))
         ,phenology.ref_accum_deg_days_normal()
         #endif
         ))  //040830
       {
/*200504 already handled in restart_after_inactive_period
         phenology.activate_accrescence();                                       //181109
         phenology.activate_root_elongation();                                   //191121
*/
         trigger_synchronization(FGS_BUD_BREAK);                                 //030523
         delete quiescence_end_requirement; quiescence_end_requirement = 0;      //200402
            // Normally fruit tree only used chill requirements
            // but for the first year (which may start in the middle/end of
            // winter, there may be case where there is not sufficient time to
            // break dormancy so quiescence_end_requirement were setup for
            // these first year.
            // Since were we were able to perform normal budbreak,
            // we didnt need quiescence_end_requirement.
         considered &= restart_after_inactive_period();                          //021202
         log_event("Bud break");
         latest_harvest_fruit_dry_biomass = 0.0;                                 //200608
         /*200402 moved to restart_after_inactive_period
         delete fruit_bud_break_requirement;                                     //021125
         fruit_bud_break_requirement = 0;                                        //021125
         */
         bud_break_date.set(simdate);                                            //081013
         linger_label.budbreak = 5;                                              //200420
       }                                                                         //021125
   }
   considered &= Crop_complete::consider_inactive_period();                      //200402_190709_071120
      //200402 (avg_temperature);
      // was temperature_with_est_night

   if (fruit_chill_requirement)                                                  //021125
   {   if (fruit_chill_requirement->is_satisfied())                             //021125
       {
          chill_requirement_satisfied_date.set(simdate);                         //081013
          delete fruit_chill_requirement; fruit_chill_requirement = 0;           //021125
          fruit_bud_break_requirement = new Fruit_bud_break_requirement
               (parameters->dormancy.deg_day_bud_break_chill_req_sat
               ,parameters->dormancy.deg_day_bud_break_chill_req_not_sat);
          // Note from this point we should be accumulating thermal time
          // We have been accumulating thermal which was reset at quiesence.
          // I am not sure if GDDs should be reset now.
          log_event("Chill requirement satisfied");
          linger_label.chill_satisfied = 5;                                      //200420
       }
       else                                                                      //021125
       {  //I still need to move est_temperature object to crop from thermal time class   //021125
          for (Hour hr = 0; hr < 24; hr++)                                       //050816
          {
             float64 air_temperature_hour = ref_hourly_weather->get_air_temperature(hr); //080908
             fruit_chill_requirement->add_hour_temperature(air_temperature_hour);//050816
          }
          //230707LML under extreme condition, the chill is never satisfied, so will be wrongly accumulating.
          //So if out of winter season, set the chill requirement satisfied whatever the temperature is


          if(seasons.is_spring_or_summer_time() && fruit_chill_requirement->get_chill_hours_remaining() > 0) {
                fruit_chill_requirement->set_chill_hours_reminning_negtive();
          }
       }
   }
   return considered;                                                            //190709
}
//_consider_inactive_period____________________________________________________/
bool Crop_orchard_fruit::senesce_at_dormancy()
{  //  currently  do nothing, (we didnt do anything before we moved fruit submodel out of Crop_complete
   harvested =false;                                                             //030529
   return culminate_senescence();                                                //200603
   return true;
}
//_senesce_at_dormancy______________________________________________2007-11-20_/
bool Crop_orchard_fruit::is_at_max_fruit_load()                            const
{
   max_fruit_dry_biomass // This really only needs to be computed once   //080912
      = (per_ha_to_per_m2(fruit_parameters_damage_adjusted->max_fruit_load_kg_ha)
               * fruit_parameters_damage_adjusted->fract_total_solids);
   return fruit_dry_biomass >= max_fruit_dry_biomass;
}
//_is_at_max_fruit_load_____________________________________________2008-11-03_/
float64 Crop_orchard_fruit::distribute_todays_non_leaf_stem_growth
(float64 todays_growth)                                            modification_
{  float64 leaf_and_branch_growth = todays_growth;  // for normal crops everything goes to leaf stem(branch)
   bool at_max_fruit_load = is_at_max_fruit_load();                              //200608
      if (max_fruit_load_achieved || harvested) //200709 (!at_max_fruit_load) //200608 (!is_at_max_fruit_load())  //10282022LML after harvest, no new fruit                                             //081103
      {  // once we reach max fruit load, stop partitioning of biomass to fruit  //080912
         reserves_biomass += leaf_and_branch_growth;                             //200603
         leaf_and_branch_growth = 0;                                             //200603
         // At this point we are probably already in culminescence
      }
      else
      {  // we need to do translation daily if not sufficient biomass (need potential growth rate).//000626
            float64 fract_to_fruit = 0.0;                                        //000626
            #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
            switch (phenology.growth_stage)                                      //000626
            {  case FGS_FRUCTESCENCE : fract_to_fruit =                          //000626
                     fruit_parameters_damage_adjusted->                          //081103
                        initial_growth_fraction_to_fruit ; break;                //000626
               case FGS_VERAISON :                                               //000626
               case FGS_RAPID_FRUIT_DEVELOPMENT :                                //030521
                  fract_to_fruit =                                               //080912
                     fruit_parameters_damage_adjusted->                          //081103
                        rapid_growth_fraction_to_fruit ;                         //000626
               break;
               default : fract_to_fruit = 0.0; break;                            //000626
            }
            #endif
            #if ((PHENOLOGY_VERSION==2018))
            fract_to_fruit =                                                     //000626
               (phenology.is_in_fructescence())                                  //181109
               ? fruit_parameters_damage_adjusted->                              //081103
                        initial_growth_fraction_to_fruit                         //000626
               : (   phenology.is_in_veraison()                                  //181109
                  || phenology.is_in_rapid_fruit_development())                  //181109
                  ? fruit_parameters_damage_adjusted->                           //081103
                        rapid_growth_fraction_to_fruit                           //000626
                  : fract_to_fruit = 0.0;
            #endif
            float64 fruit_dry_biomass_today =  todays_growth * fract_to_fruit;   //011022
            fruit_dry_biomass +=fruit_dry_biomass_today;                         //000626
            /*200608 moved to start
            float64 max_fruit_dry_biomass                                        //080912
            // This really only needs to be computed once
            //(BUT DOES NEED TO BE UPDATED WITH FREEZING DAMAGE)
            = (per_ha_to_per_m2(fruit_parameters_damage_adjusted->max_fruit_load_kg_ha) //081103
                  *  fruit_parameters_damage_adjusted-> fract_total_solids);     //081103_000626
            */
            if (fruit_dry_biomass > max_fruit_dry_biomass)                       //080912
            {
               fruit_dry_biomass_today = 0.0; //200608 max_fruit_dry_biomass-fruit_dry_biomass;//080912
               fruit_dry_biomass = max_fruit_dry_biomass;                        //080912
            }                                                                    //080912
            leaf_and_branch_growth -= fruit_dry_biomass_today;                   //011022

            /*std::clog << "\tfruit_dry_biomass_today:" << fruit_dry_biomass_today
                      << "\tfruit_dry_biomass:" << fruit_dry_biomass
                      << std::endl;*/

      }
      float64 LAI = get_LAI(true);                                               //081111
         // This is just the normal LAI
      if (LAI > parameters->canopy_growth_LAI_based.max_LAI)                     //081103
         limit_max_LAI = true;                                                   //081103
//190625    } // if fruit harvest

/*180622 This was never implemented it could be deleted
#else
// NYI
// 0.08	 Potential Fruit Relative Growth Rate (kg/kg/day)
// 0.5	 Current Fruit Biomass (kg/m2)
// 30	 Current Number of Fruits per square meter
// 0.04	 Available Biomass for Fruit Growth (kg/m2/day)
// 0.95	 Curvature

//071120
   switch (growth_stage)
   {  case INITIAL_FRUIT_GROWTH     : need some thing here
      case VERAISON                 :
      case RAPID_FRUIT_GROWTH       :
               {
                   may need to check max fruit load
                  float64 available_biomass_for_fruit_growth =  todays_growth +  fruit_reserves_biomass + fruit_root_turnover_biomass; // Warning check with Claudio if fruit_root_turnover_biomass included here
                  float64 fruit_biomass_growth = calc_fruit_biomass_growth( available_biomass_for_fruit_growth);
                  fruit_dry_biomass +=fruit_biomass_growth;
                  todays_non_leaf_stem_growth -= fruit_biomass_growth; // actual canopy growth;

               } break;
   }
   switch (growth_stage)
   {  case xxxxx:
      {
      } break;
      case xxxxx:
      {
      } break;
      case xxxxx:
      {
      } break;

   }

 FGS_PLANTING           // when planted from seed or started in orchard
,FGS_reserved_1        // This is a place holder for future use  (matches emergence) May be needed for seeded starts
,FGS_BUD_BREAK
,FGS_ACTIVE_GROWTH
,FGS_FLOWERING
,FGS_FRUIT_DEVELOPMENT
,FGS_VERAISON
,FGS_RAPID_FRUIT_DEVELOPMENT
,FGS_MATURITY
,FGS_DORMANT
,FGS_HARVESTABLE
,FGS_HARVEST
,FGS_TERMINATED      // Due to premature death, removal etc..

               fruit_reserves_biomass
               fruit_root_turnover_biomass

#endif

*/
   return leaf_and_branch_growth;  // for normal crops everything goes to leaf stem(branch)
}
//_distribute_todays_non_leaf_stem_growth___________________________2007-11-20_/
bool Crop_orchard_fruit::is_limit_max_LAI()      const { return limit_max_LAI; }
// only applies to fruit crops (which will override)
//_get_limit_max_LAI________________________________________________2008-11-03_/
bool Crop_orchard_fruit::respond_to_freezing_damage
(float64 fract_reduction_due_to_freezing)                          modification_
{  if (   phenology.is_in_anthesis()
       || phenology.is_in_fructescence())
   {  fruit_parameters_damage_adjusted->max_fruit_load_kg_ha
         *= (1.0 - fract_reduction_due_to_freezing); // 081103 need to check this with Claudio
      //initial_growth_fraction_to_fruit          = 0.0; Currently not adjusting these
      //rapid_growth_fraction_to_fruit            = 0.0; the crop may reach the maximum fruit load sooner and partition will then be stopped.
   }  // else Currently normal crops simply die_back (done by caller) this is overridden by fruit crops
   return true;
}
//_respond_to_freezing_damage_______________________________________2008-11-03_/
#ifdef NYI
// Never implemented, probably abandoned
float64 Crop_orchard_fruit::get_colimitation_curvature()                   const
{  return 0.0; // Not yet implemented
}
//_get_colimitation_curvature__________________________________________________/
float64 Crop_orchard_fruit::calc_fruit_biomass_growth
   (float64 available_biomass_for_fruit_growth)                            const
{  float64 fruit_biomass_growth = 0.0;
   float64 fruit_relative_growth_rate = get_fruit_relative_growth_rate();//
      // (kg/kg/day) This may be a crop parameter
      // or derived from crop parameters with curves for specific crop types
/*
 'Current_Fruit_Biomass (kg/m2)
 'current_fruit_count (Number/m2)
 'available_biomass_for_fruit_growth (kg/m2/day)
 */
   float64 colimitation_curvature = get_colimitation_curvature(); // Not sure what this is
   float64 current_mean_mass_per_fruit // kg
         = fruit_dry_biomass / current_fruit_count;
   float64 pot_fruit_growth_rate = fruit_relative_growth_rate * current_mean_mass_per_fruit;
   float64 fruit_growth_biomass_demand = pot_fruit_growth_rate * current_fruit_count;
   fruit_biomass_growth =
         (  pot_fruit_growth_rate * current_fruit_count + available_biomass_for_fruit_growth
            - sqrt(CORN_sqr(fruit_growth_biomass_demand + available_biomass_for_fruit_growth)
                   - 4.0 * colimitation_curvature * fruit_growth_biomass_demand
                         * available_biomass_for_fruit_growth))
         / (2.0 * colimitation_curvature);
   return fruit_biomass_growth;
}
#endif
//_calc_fruit_biomass_growth________________________________________2005-08-18_/
bool Crop_orchard_fruit::process_day()
{  bool processed = true;
   #if (PHENOLOGY_VERSION==2018)
   if (phenology.is_harvested()
       && (parameters->harvested_part_clad.get() == fruit_part))
       phenology.activate_maturity();
       //200115 Not sure why setting (back) to maturity????
   #else
   if ((phenology.growth_stage == NGS_HARVEST)
       && (parameters->harvested_part_clad.get() == fruit_part))
       phenology.growth_stage = NGS_MATURITY;
       //200115 Not sure why setting (back) to maturity????
   #endif
   if (orchard)                                                                  //190701
   {
      orchard->expand_canopy                                                     //190613
         #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
         (thermal_time->get_accum_degree_days()                                  //200211
            //200211 would always be normal because we dont clip fruit trees (true)
         );
         #endif
         #if ((PHENOLOGY_VERSION==2018))
         (phenology.ref_accum_deg_days_normal());                                //181118
         #endif
      orchard->update_solid_canopy_interception();                               //190621_151129
   }
   if (fruit_harvest_requirements                                                //190710
       && (fruit_harvest_requirements->is_satisfied
            (simdate.get_DOY(),air_temperature_min,air_temperature_avg )
           && fruit_dry_biomass > 1e-6))                                         //170309LML added biomass condition
      harvestable = true;                                                        //190710
   processed &= Crop_complete::process_day();

   if (is_at_max_fruit_load())                                                   //200709
      max_fruit_load_achieved = true;                                            //200709

/*200608 moved to harvest
if (fruit_dry_biomass > 0.000001)                                                //200608
latest_harvest_fruit_dry_biomass= fruit_dry_biomass;                             //200608
// This is just a hack, should have harvest event processing.
// At least this should be moved to end_day()
*/
   return processed;
}
//_Crop_orchard_fruit::process_day_______________________2018-06-22_2008-12-04_/
/*200420 replaced with describe_periods
const char *Crop_orchard_fruit::describe__growth_stage
(std::string &growth_stage_phrase)                                         const
{
   if (fruit_bud_break_requirement)    {growth_stage_phrase += "Bud break evaluation ";} //180701
   if (fruit_chill_requirement)        {growth_stage_phrase += "chill requirements evaluation"; } //180701
   if (linger_label.veraison        >= 0) {growth_stage_phrase += "veraison";  }
   if (linger_label.chill_satisfied >= 0) {growth_stage_phrase += "chill requirements satisfied";   }
   if (linger_label.budbreak        >= 0) {growth_stage_phrase += "bud break"; }
   if (fruit_harvested)       {growth_stage_phrase += "post fruit harvest"; }
      Crop_complete::describe__growth_stage(growth_stage_phrase);
   return growth_stage_phrase.c_str();
}
//_describe__growth_stage___________________________________________2019-06-13_/
*/
nat8 Crop_orchard_fruit::describe_periods
(CORN::Text_list &descriptions)                                            const
{
   if (fruit_bud_break_requirement)    descriptions.add_cstr("budbreak(eval)");
   if (fruit_chill_requirement)        descriptions.add_cstr("chill(eval)");
   if (linger_label.veraison        >= 0) descriptions.add_cstr("veraison");
   if (linger_label.chill_satisfied >= 0) descriptions.add_cstr("chill(satisfied)");
   if (linger_label.budbreak        >= 0) descriptions.add_cstr("budbreak");
   if (harvested)                         descriptions.add_cstr("harvested");
   Crop_complete::describe_periods(descriptions);
   return descriptions.count();
}
//_describe_periods_________________________________________________2019-06-13_/
float64 Crop_orchard_fruit::calc_freezing_damage()                  calculation_
{
   float64 fract_fruit_reduction_due_to_freezing = 0.0;
   float64 fruit_damage_cold_temp = tolerance_parameters.temperature_cold_fruit_damage;//150914
   float64 fruit_lethal_cold_temp = tolerance_parameters.temperature_cold_fruit_lethal;//150914
   static const float64 damage_curve_adj = 2.0; // Could make this a parameter. //081103
   float64 air_temp_min = air_temperature_min .Celcius();
   fract_fruit_reduction_due_to_freezing =
   // For fruit crops we do not kill  the plant
   // (it would have to be really cold (I.e. -30 for pomes))
   // but we do drop the leaves.
   // We also reduce the maximum potential fruit load
   // (accounting for the loss of flowers and early fruit growth).
   std::min<float64>                                                             //081103
      (1.0                                                                       //081103
      ,(air_temp_min < fruit_damage_cold_temp)                                   //081103
       ?pow((fruit_damage_cold_temp - air_temp_min)                              //081103
         /(fruit_damage_cold_temp - fruit_lethal_cold_temp),damage_curve_adj)    //081103
       :0.0);                                                                    //081103
   fract_fruit_reduction_due_to_freezing
      = CORN::must_be_between<float64>(fract_fruit_reduction_due_to_freezing,0.0,1.0);
   // respond_to_freezing_damage(fract_fruit_reduction_due_to_freezing);         //081103
   // Note may want to move respond_to_freezing_damage method to this class
   return fract_fruit_reduction_due_to_freezing;
}
//_calc_freezing_damage_________________________________2012-01-17__2019-06-14_/
void Crop_orchard_fruit::check_hardiness()
{  Crop_complete::check_hardiness();
   float64 fract_fruit_reduction_due_to_freezing=calc_freezing_damage();
   respond_to_freezing_damage(fract_fruit_reduction_due_to_freezing);
}
//_check_hardiness__________________________________________________2019-06-14_/
float64  Crop_orchard_fruit::get_dry_biomass_kg_m2()                       const
{  return
      (fruit_dry_biomass
      + reserves_biomass
      //200603 + fruit_root_turnover_biomass
      + get_canopy_biomass_kg_m2());
}
//_get_dry_biomass_kg_m2________________________________2019-16-18__2007-11-20_/
float32 Crop_orchard_fruit::get_fruit_chill_hours_remaining()              const
{  return fruit_chill_requirement
   ? fruit_chill_requirement->get_chill_hours_remaining()
   : 0;
}
//_get_fruit_chill_hours_remaining__________________________________2019-07-09_/
float64 Crop_orchard_fruit::get_orchard_solid_canopy_interception()        const
{ return Crop_orchard_fruit::get_orchard_solid_canopy_interception();}
//_get_orchard_solid_canopy_interception_______________________________________/
float64 Crop_orchard_fruit::get_orchard_shaded_area_GAI()                  const
{  return orchard ? orchard->get_canopy_shaded_area_GAI() : 0.0;}
//_get_orchard_shaded_area_GAI_________________________________________________/
float64 Crop_orchard_fruit::get_orchard_canopy_porosity()                  const
{  return orchard ? orchard->get_canopy_porosity() : 0.0;}
//_get_orchard_canopy_porosity_________________________________________________/
bool Crop_orchard_fruit::drop_leaves()
{  return
   Crop_complete::drop_leaves()
   && (orchard)
   ? orchard->drop_leaves()
   : true;
}
//_drop_leaves______________________________________________________2019-06-26_/
Canopy_leaf_growth &Crop_orchard_fruit::provide_canopy()              provision_
{
   bool canopy_just_provided = (!canopy_leaf_growth);
   Canopy_leaf_growth &provided_canopy = Crop_complete::provide_canopy();
   if (orchard && canopy_just_provided)                                          //190702
      orchard->know_canopy_leaf(canopy_leaf_growth);
   return provided_canopy;
}
//_provide_canopy___________________________________________________2019-07-02_/
Crop_mass_fate_biomass_and_nitrogen *
Crop_orchard_fruit::harvest
(float64 remove_amount_kg_m2
,const CropSyst::Harvest_or_clipping_operation_struct
*harvest_or_clipping_params
,bool terminate)
{
   harvested = true;
   harvestable = false;
   latest_harvest_date.set(simdate);                                             //200609
   phenology.activate_harvest();                                                 //200903
   latest_harvest_fruit_dry_biomass = fruit_dry_biomass;                         //200608
   //crop.ref_reported()
   reported.season.biomass_production                                            //200609
      = get_canopy_biomass_kg_m2()+reserves_biomass + fruit_dry_biomass;
   fruit_dry_biomass = 0;
   return 0; // No biomass fate
}
//_harvest__________________________________________________________2006-06-03_/
/*NYN
bool Crop_orchard_fruit::end_season()                              modification_
{

}
//_end_season_______________________________________________________2020-06-09_/
*/

RENDER_INSPECTORS_DEFINITION(Crop_orchard_fruit)
{
   #ifdef CS_OBSERVATION
   CS::Emanator *emanator =
       Crop_complete::render_inspectors(inspectors,context,instance);
   inspectors.append(new CS::Inspector_scalar(bud_break_date                     .ref_date32()   ,UT_date   ,*emanator,/*check ontology*/"phenology/budding"              ,final_statistic,CSVP_crop_base_bud_break_date_YD));
   inspectors.append(new CS::Inspector_scalar(initial_fruit_growth_date          .ref_date32()   ,UT_date   ,*emanator,/*need ontology*/"phenology/filling/initial"       ,final_statistic,CSVP_crop_base_initial_fruit_growth_date_YD));
   inspectors.append(new CS::Inspector_scalar(rapid_fruit_growth_date            .ref_date32()   ,UT_date   ,*emanator,/*need ontology*/"phenology/filling/rapid"         ,final_statistic,CSVP_crop_base_rapid_fruit_growth_date_YD));
   inspectors.append(new CS::Inspector_scalar(chill_requirement_satisfied_date   .ref_date32()   ,UT_date   ,*emanator,"phenology/chill/requirement"                      ,final_statistic,CSVP_crop_base_chill_requirement_satisfied_date_YD));
   inspectors.append(new CS::Inspector_scalar(reserves_biomass                                   ,UC_kg_m2  ,*emanator,/*need ontology*/"fruit/biomass/reserves"          ,sum_statistic  ,CSVC_crop_base_fruit_reserves_biomass)); //200603
   return emanator;
   #else
   return 0;
   #endif
}
//_RENDER_INSPECTORS_DEFINITION_____________________________________2018-06-12_/
}//_namespace CropSyst_________________________________________________________/

/*
From: Marsal, Jordi [Jordi.Marsal@irta.cat]
Sent: Thursday, November 29, 2007 1:41 AM
To: Nelson, Roger Leroy; Stockle, Claudio Osvaldo

Attachments: Potential RGR Grapevine_Tempranillo_Formula.xls; Potential
RGR Apple_GoldenSomthee_Formula.xls


Dear Roger and Claudio,

Please find in attachement the potential RGR curves corresponding
to apple and vinegrapes in two different spreadsheets.
The functions can be found within the figures corresponding to the Curve fitting part.

There are two different initial fruit mass to be considered
and that should not be confused. First is the one that permits to initiate
the Potential RGR calculation (obtained under very low fruit load conditions),
second is the one that comes from commercial conditions and permits to
calculate Actual fruit growth.
Both should be determined at the same phenological time,
but only the latter should be provided by the user.

For Apple
GDD after full budbreak to initiate fruit growth routine = 790
Initial frut dry matter for potential RGR calculation = 3.4 g

For Vinegrape
GDD after full budbreak to initiate fruit growth routine = 700
Initial frut dry matter for potential RGR calculation = 0.027 g

Ohter details are provided in the files.

I had trouble with the vinegrape RGR function because it failed to
reproduce the original fruit growth curve it came from.
This is probably due to small accumulated errors in every daily step
for the calculation of the final dry matters.
In order to avoid this underestimation for potential fruit growth,
I have slightly modified the parameters of the original 3-functions.

There's a lot of other information in the files.
I have checked them with commercial field data to evaluate the
information provided by the curves during two different periods.
In apple a curvature of about 0.75 is suggested.
Good news is that potential source calculated by WUE or Radiative method
produces similar results (see second sheet of the file).
However potential source seems to me overestimating the capacity
for allocating biomass to fruit growth.
In consequence we should consider another factor as relevant such as transport limitation.
In vinegrapes, results are not that encouraging, though.
It seems it is going to be a great deal of carbon for shoot growth after veraison.
A good adjutsment of leaf water potential to realistic values observed in the
field should help reduce this putative extra-vegetative growth during summer.

Other stuff
Target number of fruits per tree, grape clusters per vine
and grape berries per cluster should be defined by the user,
since they are a function of the commercial practices carried out in every specific case.
In the second sheet of both attached files you can see the values
I used in my example cases (see cell defined by cropping).

Reserves is a difficult issue, we should also consider the role of stems and branches on the winter reserves though at less extent
 than in roots.
Let's see what claudio says about it. I think a simple approach would come from considering all
growth occured until the start of fruit growth routine calculation as coming from reserves by 80%, for instance.

There are a number of comments that I would like to point out after you have recieved the curves and started to work on it.

My best regards
Jordi
*/

