
#include "crop/canopy_growth_cover_based.h"
#include "crop/thermal_time.h"
#ifndef moremathH
#  include "corn/math/moremath.h"
#endif
#ifndef compareHPP
#  include "corn/math/compare.hpp"
#endif
#ifdef CO2_CHANGE
#  include "crop/CO2_response.h"
#endif
#include <math.h>
using namespace std;
#include <assert.h>
// unassigned_reference is a temporary place holder for member references that are assigned
// after an object is instanciated.
float64 unassigned_reference=999999.9;
namespace CropSyst {
//______________________________________________________________________________
Canopy_growth_cover_based::Canopy_growth_cover_based
(const Crop_parameters       &crop_parameters_
,const Crop_parameters_struct::Canopy_growth_cover_based &parameters_
,Residues_interface          *residues_                                          //060816
,Crop_nitrogen_interface     *nitrogen_                                          //151006_060816
,const Phenology             &phenology_
,const float64               &water_stress_index_yesterday_
,const float64               &water_canopy_expansion_factor_
#ifdef CO2_CHANGE
,Crop_CO2_response           *CO2_response_
#endif
)
: Canopy_growth_portioned
   (crop_parameters_
   ,residues_
   ,nitrogen_)
, phenology(phenology_)
, cover_reference
   (crop_parameters_.canopy_growth_cover_based
   ,crop_parameters_.morphology
   ,phenology_
   ,crop_parameters_.is_fruit_tree())                                            //120510
, cover_actual
   (crop_parameters_.canopy_growth_cover_based
   ,crop_parameters_.morphology
   ,phenology_
   ,crop_parameters_.is_fruit_tree()
   ,water_stress_index_yesterday_
   ,cover_reference)
, canopy_vital
   #ifdef OLD_SENESCED_BIOMASS_FATE
   (crop_parameters_.senesced_biomass_fate_labeled_obsolete.get()
   ,false,crop_parameters_.linger_days)                                          //060911
   #else
   (crop_parameters_.abscission_clad.get(),false,crop_parameters_.linger_days)   //130406_060911
   #endif
, canopy_effete
   #ifdef OLD_SENESCED_BIOMASS_FATE
   (crop_parameters_.senesced_biomass_fate_labeled_obsolete.get()
   ,true,crop_parameters_.linger_days)                                           //060911
   #else
   (crop_parameters_.abscission_clad.get(),true,crop_parameters_.linger_days)    //130406
   #endif
, untouchable_portion(0)                                                         //080818
, parameters         (parameters_)
, water_canopy_expansion_factor(water_canopy_expansion_factor_)                  //080806
#ifdef CO2_CHANGE
, CO2_response       (CO2_response_)
#endif
{
      // RLN warning I am not sure in the original VB code if this is intended
      // to be the actual or reference cumulative
//   canopy_actual.know_reference_thermal_time_cumulative
//      (phenology_actual.thermal_time_cumulative) ;

//      answer only one thermal time accum. (I'll wait until Claudio provides and updated version
}
//_Canopy_growth_cover_based:constructor_______________________________________/
bool Canopy_growth_cover_based::initialize()                     initialization_
{
   Crop_parameters_struct::Canopy_growth_cover_based &parameters_                //180614
   = const_cast<Crop_parameters_struct::Canopy_growth_cover_based&>(parameters); //180614
   #ifdef CO2_CHANGE
      parameters_.initialize                                                     //140205
         (CO2_response?                                                          //140226
          CO2_response->actual_biomass_gain_ratio_in_response_to_CO2_RUE         //171208
         : 1.0);                                                                 //140226
   #endif
   bool initialized = cover_reference.initialize();                              //110901
   return initialized;
}
//_initialize_______________________________________________________2011-09-01_/
bool Canopy_growth_cover_based::restart_with
(float64 restart_biomass
,float64 restart_GAI_not_used_in_this_model
,bool clumping_unused)                                             modification_
{  canopy_vital.append(new Canopy_accumulation::Portion
      (restart_biomass,parameters.cover_initial,0.0/*thermal_time*/));           //130510
         // probably should have today's thermal_time
   return true;
}
//_restart_with_____________________________________________________2011-09-01_/
bool Canopy_growth_cover_based::start()                            modification_

// I am not sure if I want to call these start or start_season

{
   return   cover_reference.start_season()                                       //110902
         && cover_actual   .start_season();                                      //110902
}
//_start____________________________________________________________2011-09-01_/
float64 Canopy_growth_cover_based::calc_plant_development
(float64 length_max
,float64 last_length
,float64 overall_growth_stress)                                            const //080805
{  float64 development = 0;
#ifdef NYI
Something similar to the following is needed
   float64 LAI_fract =
       canopy->get_LAI_to_LAImax_fract();
   float64 development =
     std::max<float64>(last_length ,
         std::min<float64>(pow(LAI_fract,overall_growth_stress ) * length_max,
             1.15 * LAI_fract * length_max ));   // was 1.1
#endif
  return development;
}
//_calc_plant_development___________________________________________2011-09-01_/
bool Canopy_growth_cover_based::Canopy_accumulation::update()     rectification_
{  bool common_updated = Canopy_growth_portioned::Canopy_accumulation::update(); //080808
   Portion *accum_today = provide_accum_calculated_today();                      //130520
   return common_updated && accum_today != NULL;
}
//_Canopy_accumulation::update______________________________________2011-09-01_/
bool Canopy_growth_cover_based::update()                          rectification_
{
   float64 fract_canopy_cover_green_previous
      = cover_actual.get_interception_global_green(); // was calc                //200228
   bool updated_cover = true;
   cover_reference.update_cover(LWP_yesterday);
   cover_actual   .update_cover(LWP_yesterday);                                  //110901
   float64 global_fract_canopy_cover_green
      = cover_actual.get_interception_global_green(); //was calc                 //200228
   float64 change_canopy_cover_green_fract
      = global_fract_canopy_cover_green - fract_canopy_cover_green_previous;
   if (change_canopy_cover_green_fract >= 0.0)
      fract_canopy_cover_green_expansion = change_canopy_cover_green_fract;
         // This is setup in case we add biomass
      // This is later used in develop leaf
   else
   {  fract_canopy_cover_green_expansion = 0;                                    //130818
      float64 fract_canopy_cover_to_reduce = -change_canopy_cover_green_fract;
      Canopy_growth_cover_based & updatable_self
         = const_cast<Canopy_growth_cover_based &>(*this);                       //090316
      updatable_self.reduce_green_fraction_canopy_cover(fract_canopy_cover_to_reduce);
         // We reduce any effete canopy accumulation portions first
   }
   return updated_cover;
}
//_update___________________________________________________________2011-09-01_/
bool Canopy_growth_cover_based::process_day()                      modification_
{  return update(); }
//_process_day______________________________________________________2011-09-01_/
float64 Canopy_growth_cover_based::reduce_green_fraction_canopy_cover
   (float64 canopy_cover_to_reduce_fraction)
{  if ((canopy_cover_to_reduce_fraction>0.000001)&&(!canopy_effete.is_empty()))  //130818
   canopy_cover_to_reduce_fraction = canopy_effete
      .reduce_green_fraction_canopy_cover(canopy_cover_to_reduce_fraction);
         // Then with any remainder of that reduce the vital portions
   if (canopy_cover_to_reduce_fraction > 0.000001)                               //130818
   canopy_cover_to_reduce_fraction = canopy_vital
      .reduce_green_fraction_canopy_cover(canopy_cover_to_reduce_fraction);
         // If there is any remainder of vital then
         // There is a discrepency.
   return canopy_cover_to_reduce_fraction;
}
//_reduce_green_fraction_canopy_cover_______________________________2011-09-01_/
float64 Canopy_growth_cover_based::Canopy_accumulation
::reduce_green_fraction_canopy_cover(float64 canopy_cover_to_reduce_fraction)
{  FOR_EACH(portion,Portion,each_portion)
   {  canopy_cover_to_reduce_fraction
         = portion->reduce_green_fraction_canopy_cover(canopy_cover_to_reduce_fraction);
      if (CORN::is_zero<float64>                                                 //200127
          (canopy_cover_to_reduce_fraction,0.000000001)) return 0.0;             //130818
   } FOR_EACH_END(each_portion)
   return canopy_cover_to_reduce_fraction;
}
//_reduce_green_fraction_canopy_cover_______________________________2011-09-01_/
float64 Canopy_growth_cover_based::Canopy_accumulation::Portion
::reduce_green_fraction_canopy_cover(float64 canopy_cover_to_reduce_fraction)
{  if ((canopy_fraction_green > 0.0) && (canopy_cover_to_reduce_fraction > 0.0))
   {  if (canopy_cover_to_reduce_fraction > canopy_fraction_green)
      {  canopy_cover_to_reduce_fraction -= canopy_fraction_green;
         canopy_fraction_dead += canopy_fraction_green; //Transfer green to dead
         canopy_fraction_green = 0.0;                                            //130818
            // canopy_fraction_green wasn't being set to 0 not sure why not      //130818
      } else
      {  canopy_fraction_green -= canopy_cover_to_reduce_fraction;
         canopy_cover_to_reduce_fraction = 0.0;
      }
   }  // else there is a discrepency
      // but normally canopy_cover_to_reduce_fraction should never be negative
   return canopy_cover_to_reduce_fraction;
}
//_reduce_green_fraction_canopy_cover_______________________________2011-09-01_/
modifiable_ float64 &Canopy_growth_cover_based
::reference_fract_canopy_cover()                                   modification_
{  return cover_actual.mod_interception_insolation_global_green();
}
//_reference_fract_canopy_cover_____________________________________2011-09-01_/
bool Canopy_growth_cover_based::end_day()                          modification_
{  return cover_reference.end_day()
      &&  cover_actual   .end_day();
}
//_end_day__________________________________________________________2019-08-21_/
bool Canopy_growth_cover_based::know_accrescence
   (const Phenology::Period_thermal *accrescence_)                 modification_
{  return cover_reference.know_accrescence(accrescence_)
      &&  cover_actual   .know_accrescence(accrescence_);
}
//_Canopy_growth_cover_based::know_accrescence______________________2018-12-30_/
bool Canopy_growth_cover_based::know_culminescence
   (const Phenology::Period_thermal *culminescence_)               modification_
{  return cover_reference.know_culminescence(culminescence_)
      &&  cover_actual   .know_culminescence(culminescence_);
}
//_Canopy_growth_cover_based::know_culminescence____________________2018-13-30_/
bool Canopy_growth_cover_based::know_senescence
   (const Phenology::Period_thermal *senescence_)                  modification_
{
   cover_reference.set_interception_insolation_global_green_at_senescence        //200306
      (cover_actual.get_interception_global_green());                            //200306
   return cover_reference.know_senescence(senescence_)
      &&  cover_actual   .know_senescence(senescence_);
}
//_Canopy_growth_cover_based::know_senescence_______________________2018-11-09_/
bool Canopy_growth_cover_based::know_maturity_initiation()         modification_
{  return cover_reference.know_maturity_initiation()
      &&  cover_actual.know_maturity_initiation();
}
//_know_maturity_initiation_________________________________________2020-04-09_/
bool Canopy_growth_cover_based::know_N_leaf_stress_factor
(float64 N_leaf_stress_factor_)
{  return cover_actual.know_N_leaf_stress_factor(N_leaf_stress_factor_);
}
//_know_N_leaf_stress_factor________________________________________2020-05-28_/
bool Canopy_growth_cover_based::Canopy_accumulation::invalidate()  modification_
{  delete accum_today; accum_today = 0;
   return true;
}
//_Canopy_accumulation::invalidate__________________________________2011-09-01_/
bool Canopy_growth_cover_based::emerge_cotyledon
(float64 new_cotalydon_GAI
,float64 new_cotalydon_BM)                                         modification_
{  assert(false);
   return false; /* NYI */
}
//_emerge_cotyledon_________________________________________________2007-07-28_/
void Canopy_growth_cover_based::terminate()
{  // Makes the untouchable portion available for removal with biomass fate.
   if (untouchable_portion)
   {  canopy_vital.append(untouchable_portion);
      untouchable_portion = 0;
   }
}
//_terminate________________________________________________________2008-02-22_/
float64 Canopy_growth_cover_based::get_senescence_reduction_factor()       const
{  float64 fract_CC_vital_plus_effete
      = cover_actual.get_interception_global_green(); // was calc
   float64 peak_fract_canopy_cover = cover_actual.cover_attained_max;            //110901
/*
if (peak_fract_canopy_cover > 0.0)
std::clog << "reached  peak_fract_canopy_cover with peak_fract_canopy_cover" << std::endl;
*/
   return (CORN::is_zero<float64>(peak_fract_canopy_cover) //200127 ,0.0,0.00001)
      ? 0.0  // avoid div0 error
      : fract_CC_vital_plus_effete  /* was get_GAI_vital_plus_effete()*/
      / peak_fract_canopy_cover);
}
//_get_senescence_reduction_factor__________________________________2008-08-07_/
float64 Canopy_growth_cover_based::calc_cover_fract
(float64 xAI
,bool photosynthetic_active_rad_based
,bool use_clumping_factor  // This model has no concept of clumping              //080801
,bool record_shading_porosity_interception)                                const //081104
{  return photosynthetic_active_rad_based
   ? cover_actual.get_interception_PAR_green()
   : cover_actual.get_interception_global_green(); // was calc                   //200228
}
//_calc_cover_fract_________________________________________________2011-09-10_/
bool Canopy_growth_cover_based::develop_leaf
(bool continue_leaf_growth
,float64 canopy_growth      // mass
,float64 unused_LAI_related_growth  // Today's LAI related growth    In this model we use fract_canopy_cover_green_expansion
,float64 thermal_time_growing_degree_day)                                        //130510_080728
{  canopy_vital.append(new Canopy_accumulation::Portion
      (canopy_growth,fract_canopy_cover_green_expansion
      ,thermal_time_growing_degree_day));                                        //130510
   return true;                                                                  //081102
}
//_develop_leaf_________________________________________________________________
float64 Canopy_growth_cover_based
::get_influence_on_root_activity_factor
(bool after_end_of_vegetative_growth)                                      const
{  float64 peak_fract_canopy_cover = cover_actual.cover_attained_max;            //110901
   float64 fract_canopy_cover_green = cover_actual
            .get_interception_global_green();   //200228 was calc
   float64 root_activity_factor =                                                //080728
         (!crop_parameters.is_perennial()                                        //000808
         && after_end_of_vegetative_growth // (accum_degree_days > crop_parameters.phenology.end_vegetative_growth_deg_day)
         && peak_fract_canopy_cover > 0.0)
         ?  CORN::must_be_less_or_equal_to<float64>
               (fract_canopy_cover_green / peak_fract_canopy_cover,1.0)
         : 1.0;
   // Reduces root activity similar to the way GAI is scenescing.  GAI vital is GAI for the new season

   return root_activity_factor;
}
//_get_influence_on_root_activity_factor____________________________2008-08-05_/
int Canopy_growth_cover_based::Canopy_accumulation::Portion::compare(const CORN::Item &other) const
{  return Canopy_growth_portioned::Canopy_accumulation::Portion::compare(other);
#ifdef NYN
080809 This class dos not need sorting of Portions
  float64 other_age = ((const Canopy_growth_cover_based::Canopy_accumulation::Portion &)(other)).age;
       return is_essentially(age,other_age,0.0000001) ? 0 : (age  < other_age) ? -1 : 1;
#endif
}
//_Canopy_growth_cover_based::Canopy_accumulation::Portion::compare____________/
bool Canopy_growth_cover_based::Canopy_accumulation::Portion::is_valid() const
{  bool members_valid = (canopy_fraction_green > 0.0) || (canopy_fraction_dead > 0.0);
   return  members_valid || Canopy_growth_portioned::Canopy_accumulation::Portion::is_valid();
}
//_is_valid____________________________________________________________________/
#ifdef NA
bool Canopy_growth_cover_based::Canopy_accumulation::Portion::age_by(float64 daily_age_deg_day,float64 leaf_duration_deg_day_f32)
{  age += daily_age_deg_day;
   bool senesce_now = (age > leaf_duration_deg_day_f32) && !is_dead();           //060316
   if (senesce_now) die();
   return senesce_now;                                                           //060316
}
//_Canopy_accumulation::Portion::age_by_____________________________2005-08-23_/
#endif
void Canopy_growth_cover_based::Canopy_accumulation::Portion::die()
{  canopy_fraction_dead += canopy_fraction_green;                                //130510_080806
   canopy_fraction_green = 0.0;                                                  //080806
   thermal_time = 0;                                                             //130510
}
//_Canopy_accumulation::Portion::die________________________________2005-08-23_/
void Canopy_growth_cover_based::Canopy_accumulation::Portion::die_back_to_GAI(float64 target_GAI)
{
assert(false); // This is never called in this model
/*
float64 GAI_reduced = GAI - target_GAI;
   dead_AI = GAI_reduced;
   GAI= target_GAI;
*/
/* need to do something here
#ifdef CO2_CHANGE
   dead_AI_at_baseline_conc = GAI_at_baseline_conc;
   GAI_at_baseline_conc = 0.0;
#endif
*/
}
//_Portion::die_____________________________________________________2005-08-23_/
void Canopy_growth_cover_based::Canopy_accumulation::Portion::increment_by
(const Canopy_growth_cover_based::Canopy_accumulation::Portion &addend) modification_
{  //Don't add age this method is used to get the current totals, the age has no meaning in this context.
   Canopy_growth_portioned::Canopy_accumulation::Portion::increment_by(addend);
   canopy_fraction_green += addend.canopy_fraction_green;
   canopy_fraction_dead  += addend.canopy_fraction_dead;
   thermal_time          += addend.thermal_time;                                 //130510
}
//_Canopy_accumulation::Portion::increment_by__________________________________/
float64 Canopy_growth_cover_based::Canopy_accumulation::Portion::dec_living
(float64 subtrahend_area_index)
{  canopy_fraction_green -= subtrahend_area_index;
   if (canopy_fraction_green < 0) thermal_time = 0;                              //130510
   return canopy_fraction_green;
}
//_Canopy_accumulation::Portion::dec_living_________________________2006-05-30_/
#ifdef NA
float64 Canopy_growth_cover_based::Canopy_accumulation::Portion::inc_green_area_index
(float64 addend_area_index)
{  GAI += addend_area_index;
   return GAI;
}
//_Canopy_accumulation::Portion::inc_green_area_index_______________2006-05-30_/
void Canopy_growth_cover_based::Canopy_accumulation::Portion
::clear_GAI_related_biomass()                                      modification_
{  GAI_related_biomass = 0.0;                 // kg/m2 GAI related canopy biomass.  It is used for GAI/biomass development calculation.  // was  GAI_related_canopy_biomass
   #ifdef CO2_CHANGE
   GAI_at_baseline_concX = 0.0;                 // distr renamed to daily_GAI  (remove _X after we fixup the new biomass.}
   GAI_related_biomass_at_baseline_concX = 0.0; // was GAI_related_canopy_biomass_at_baseline_conc
   #endif
}
//_clear_GAI_related_biomass________________________________________2008-05-15_/
#endif
//______________________________________________________________________________
float64 Canopy_growth_cover_based::Canopy_accumulation::Portion::slough() renunciation_
{  // This invalidates the canopy portion the biomass amount is relinquished to the caller.
   // Returns the biomass sloughed
   canopy_fraction_green = 0;
   canopy_fraction_dead  = 0;
   thermal_time = 0;                                                             //130510
   return Canopy_growth_portioned::Canopy_accumulation::Portion::slough();
}
//_Canopy_accumulation::Portion::slough_____________________________2006-08-24_/
bool Canopy_growth_cover_based::Canopy_accumulation::Portion::dormancy()
{
// NA GAI_related_biomass = 0;
// NA CO2            GAI_related_biomass_at_baseline_conc = 0;
   // RLN not sure if canopy_fraction_green should be transfered to canopy_fraction_dead //080806
   // Probably not because it will go to effet.
   return Canopy_growth_portioned::Canopy_accumulation::Portion::dormancy();
}
//_dormancy_________________________________________________________2006-08-25_/
Canopy_growth_cover_based::Canopy_accumulation::Portion *
Canopy_growth_cover_based::Canopy_accumulation
::provide_accum_today()                                               provision_
{  if (!accum_today)                                                             //190404
      accum_today = new Canopy_growth_cover_based::Canopy_accumulation::Portion; //080811
   return accum_today;
}
//_provide_accum_today_________________________________________________________/
Canopy_growth_cover_based::Canopy_accumulation::Portion *Canopy_growth_cover_based
::Canopy_accumulation::provide_accum_calculated_today()               provision_
{
   if (!accum_today)                                                             //190404
   {
   provide_accum_today();
   FOR_EACH(portion,Portion,each_portion)
   {  accum_today->increment_by(*portion);
   } FOR_EACH_END(each_portion)
   }
   return accum_today;
}
//_provide_accum_calculated_today___________________________________2013-05-20_/
float64 Canopy_growth_cover_based::calc_adjusted_ET_crop_coefficient()     const
{  float64 limit = crop_parameters.ref_canopy_growth_cover_based().cover_maximum_no_CO2_adj;
   float64 param_ET_coef = crop_parameters.transpiration.ET_coef;
   float64 canopy_interception_fract
      = get_interception_global_total();                                         //200228
      //200228 = get_interception_global_green();                                //120405
   float64 adjusted_ET_coef =
         ((param_ET_coef > 1.0) && (canopy_interception_fract < limit))
         ? (1.0 + (param_ET_coef -1.0) * canopy_interception_fract / limit)      //980311
         : param_ET_coef;
   return adjusted_ET_coef;
}
//_calc_adjusted_ET_crop_coefficient________________________________2008-08-11_/
float64 Canopy_growth_cover_based
::increment_untouchable_portion(Canopy_growth_portioned::Canopy_accumulation::Portion &by_portion) modification_
{  Canopy_growth_cover_based::Canopy_accumulation::Portion & local_by_portion
      = (Canopy_growth_cover_based::Canopy_accumulation::Portion &)by_portion;
   untouchable_portion->increment_by(local_by_portion);
   return untouchable_portion->get_biomass_current();
}
//__increment_untouchable_portion___________________________________2008-08-18_/
bool Canopy_growth_cover_based::take_untouchable_portion(Canopy_growth_portioned::Canopy_accumulation::Portion *_untouchable_portion)
{  untouchable_portion = (Canopy_growth_cover_based::Canopy_accumulation::Portion  *)_untouchable_portion;
   return true;
}
//_take_untouchable_________________________________________portion_2008-08-18_/
float64 Canopy_growth_cover_based::get_GAI(uint8 include_GAI)              const
{ return cover_actual.calc_green_area_index();  }
//_get_GAI__________________________________________________________2011-09-01_/
float64 Canopy_growth_cover_based::get_LAI()                               const
{ return cover_actual.calc_leaf_area_index();  }
//_get_LAI__________________________________________________________2011-09-01_/
float64 Canopy_growth_cover_based::get_LAI(bool from_canopy)               const
{ return cover_actual.calc_leaf_area_index();  }
//_get_LAI__________________________________________________________2011-09-01_/
float64 Canopy_growth_cover_based::get_peak_LAI()                          const
{ return cover_actual.calc_peak_leaf_area_index(); }
//_get_peak_LAI_____________________________________________________2011-09-01_/
bool Canopy_growth_cover_based::respond_to_clipping()              modification_
{  Canopy_accumulation::Portion *accum =
      canopy_vital.provide_accum_calculated_today();
   float64 fraction_green = accum->canopy_fraction_green;
   cover_reference.set_interception_insolation_global_green(fraction_green);
   cover_actual   .set_interception_insolation_global_green(fraction_green);
   return true;
}
//_respond_to_clipping__________________________________2013-05-19__2011-09-01_/
bool Canopy_growth_cover_based::end_season(bool apply_dormancy)    modification_
{  return (apply_dormancy)
      ?   cover_reference .start_dormancy()
        &&cover_actual    .start_dormancy()

        // WARNING need to check cover or biomass needs to be called first.

      : true;
}
//_end_season_______________________________________________________2011-09-01_/
modifiable_ Canopy_growth_portioned::Canopy_accumulation::Portion
*Canopy_growth_cover_based::mod_untouchable_portion()                      const
{return untouchable_portion; }
//_mod_untouchable_portion__________________________________________2019-08-16_/
unmodifiable_ Canopy_growth_portioned::Canopy_accumulation::Portion
*Canopy_growth_cover_based::ref_untouchable_portion()                      const
{return untouchable_portion; }
//_ref_untouchable_portion__________________________________________2019-08-16_/
float64 Canopy_growth_cover_based::get_interception_global_total()         const //200228
{ return cover_actual.get_interception_global_total();
} // eventually rename to get_fract_canopy_cover_total
//_get_fract_canopy_cover___________________________________________2011-09-01_/
float64 Canopy_growth_cover_based::get_interception_global_green()         const
{
   return cover_actual.get_interception_global_green(); //200228 was calc  but was actually just get
}
//_interception_insolation_global_green_canopy______________________2011-09-01_/
float64 Canopy_growth_cover_based::get_interception_PAR_green()            const //130902
{ return cover_actual.get_interception_PAR_green();
}
//_get_interception_PAR_green_canopy________________________________2011-09-01_/
/*200228 this was calc but actually it is just get
float64 Canopy_growth_cover_based::calc_interception_global_green()
const
{  return cover_actual.calc_interception_global_green();
   //110904 WARNING not sure if solar_rad_intercepted_green_canopy or solar_rad_intercepted_fraction
}
//_calc_interception_global_green_canopy__________________2001-09-?4_/
*/

float64 Canopy_growth_cover_based::update_LAI_for_self_shading_response
(bool after_accescence
,bool before_maturity)
modification_
{  return get_LAI();
   //110903 WARNING I am not sure if this is correct
}
//_update_LAI_for_self_shading_response_____________________________2011-09-02_/
float64 Canopy_growth_cover_based::Canopy_accumulation::Portion::get_GAI() const
{  assert(false); return 0.0; }
//_Canopy_accumulation::Portion::get_GAI____________________________2005-08-23_/
float64 Canopy_growth_cover_based::Canopy_accumulation::get_thermal_time_for_reset() const
{  Portion *accum = provide_accum_calculated_today();                            //130818
   return accum->get_thermal_time();
}
//Canopy_accumulation::get_thermal_time_for_reset___________________2013-05-12_/
float64 Canopy_growth_cover_based::get_thermal_time_for_reset()            const
{  // This is in response to clipping
   float64 thermal_time  =  canopy_vital.get_thermal_time_for_reset();
   return  thermal_time;
}
//_Canopy_growth_cover_based::get_thermal_time_for_reset____________2013-05-12_/
float64 Canopy_growth_cover_based::remove_biomass
(float64 biomass_to_remove
,float64 retain_vital_living
         // biomass won't be removed if it takes the GAI or fCCg below this level. Use 0.0 for no limitation
,bool remove_newest_first
,bool reserve_todays_portion
,Crop_biomass &accumulated_removed_biomass_returned) modification_
{
   float64 biomass_before = get_biomass_current(include_vital|include_effete);    //190611
   float64 removed_biomass =
      (biomass_to_remove > 0.0000001)
      ? Canopy_growth_portioned::remove_biomass
         (biomass_to_remove
         ,retain_vital_living
         ,remove_newest_first
         ,reserve_todays_portion
         ,accumulated_removed_biomass_returned)
      : 0.0;
   if (biomass_before > 0.0)                                                     //190611
   {
      float64 biomass_after = get_biomass_current(include_vital|include_effete); //190611
      float64 reduction_adj = (biomass_after/biomass_before);                    //190611
         // adjustment

      //190611 big warning this model doesnt really distingish the
      // evolution/aging of canopy portions for green or total GAI,
      // (so clipping hasn't be throughly tested for the canopy curve based model).
      // At this time one might prefer to use the LAI duration model
      // for crops that a clipped.

      // adjust reference and actual canopy curve  to account for GAI reduction
      // (otherwise this canopy curve model sort of treats the reduction as senescence).
      cover_reference.adjust_interception_insolation_global_green(reduction_adj);
      cover_actual   .adjust_interception_insolation_global_green(reduction_adj);

      cover_reference.adjust_interception_insolation_global_total(reduction_adj);  //190611
      cover_actual   .adjust_interception_insolation_global_total(reduction_adj);  //190611
   }
   return removed_biomass;
}
//_remove_biomass___________________________________________________2019-04-05_/
}//_namespace CropSyst_________________________________________________________/

