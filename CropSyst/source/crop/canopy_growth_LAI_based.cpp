#include "crop/canopy_growth_LAI_based.h"
#include "corn/math/compare.hpp"
#include "crop/crop_param.h"
#include "crop/crop_biomass.h"
#include "crop/thermal_time.h"
#include "crop/growth_stages.h"
#include <math.h>
#include "corn/math/compare.hpp"
#ifndef moremathH
#  include "corn/math/moremath.h"
   using namespace std;
#endif

#ifdef CO2_CHANGE
#  include "crop/CO2_response.h"
#endif
#include <fstream>
#ifdef DEBUG_CANOPY_PORTIONS
ofstream canopy_portions_stream("canopy_portions.dat");
#endif
//______________________________________________________________________________
namespace CropSyst {
//______________________________________________________________________________
Canopy_growth_leaf_area_index_based::Canopy_accumulation::Portion::Portion
(float64 age_
,float64 GAI_                         // Initially area_index is always green area index
,float64 biomass_
,float64 GAI_related_biomass_         // This is the GAI related canopy biomass. It is used for development calculation 980226P
)
: Canopy_growth_portioned::Canopy_accumulation::Portion(biomass_)
,age                    (age_)
,GAI                    (GAI_)                                                   //060911
,dead_AI                (0.0)                                                    //050823
,GAI_related_biomass    (GAI_related_biomass_)
{}
//_Canopy_accumulation::Portion:constructor____________________________________/
bool  Canopy_growth_leaf_area_index_based::Canopy_accumulation::Portion::write   //150729
(std::ostream &strm)                                              performs_IO_
{ strm<< '\t'<< (int)
	#if (__LP64__ || _M_IA64)
	(uint64_t)
	#endif
	this  // the ID
      << '\t'<< age
      << '\t'<< days_dead
      << '\t'<< GAI
      << '\t'<< dead_AI
      << '\t'<< biomass_produced
      << '\t'<<  biomass_current
      << '\t'<< GAI_related_biomass
      << std::endl;
   return true;                                                                  //150728
}
//_Portion::write___________________________________________________1998-05-11_/
Canopy_growth_leaf_area_index_based::~Canopy_growth_leaf_area_index_based()
{  delete untouchable_portion;
}
//_~Canopy_growth_leaf_area_index_based:destructor_____________________________/
Canopy_growth_leaf_area_index_based::Canopy_accumulation::~Canopy_accumulation()
{  delete accum_today;
}
//_Canopy_accumulation:deconstructor___________________________________________/
Canopy_growth_leaf_area_index_based::Canopy_accumulation::Portion::Portion()
: Canopy_growth_portioned::Canopy_accumulation::Portion()
,age(0)
,GAI(0)
,dead_AI(0.0)                                                                    //050823
,GAI_related_biomass(0)
{}
//__________This constructor is used for accumulators__________________________/
int Canopy_growth_leaf_area_index_based::Canopy_accumulation::Portion::compare(const CORN::Item &other) const
{  float64 other_age = ((const Canopy_growth_leaf_area_index_based::Canopy_accumulation::Portion &)(other)).age;
      return CORN::is_approximately<float64>(age,other_age,0.0000001) ? 0 : (age  < other_age) ? -1 : 1;
}
//_Portion::compare____________________________________________________________/
bool Canopy_growth_leaf_area_index_based::Canopy_accumulation::Portion::is_valid() const
{  bool common_is_valid = Canopy_growth_portioned::Canopy_accumulation::Portion::is_valid();
   // GAI and GAI related canopy biomass can be 0.0 when Biomass is 0.0.
   return common_is_valid;
}
//_Portion::is_valid________________________________________________2008-08-06_/
void Canopy_growth_leaf_area_index_based::Canopy_accumulation::Portion::die()
{
   dead_AI = GAI;
   GAI= 0.0;
}
//_Portion::die_____________________________________________________2005-08-23_/
void Canopy_growth_leaf_area_index_based::Canopy_accumulation::Portion
::die_back_to_GAI(float64 target_GAI)
{  float64 GAI_reduced = GAI - target_GAI;
   dead_AI = GAI_reduced;
   GAI= target_GAI;
/* need to do something here
#ifdef CO2_CHANGE
   dead_AI_at_baseline_conc = GAI_at_baseline_conc;
   GAI_at_baseline_conc = 0.0;
#endif
*/
}
//_Canopy_accumulation::Portion::die_back_to_GAI____________________2005-08-23_/
void Canopy_growth_leaf_area_index_based::Canopy_accumulation::Portion
::increment_by
   (const Portion &addend)
{  //Don't add age this method is used to get the current totals, the age has no meaning in this context.
   Canopy_growth_portioned::Canopy_accumulation::Portion::increment_by(addend);
   GAI += addend.GAI;                                                            //970305
   dead_AI  += addend.dead_AI;                                                   //970305
   GAI_related_biomass  += addend.GAI_related_biomass;    // kg/m2 GAI related canopy biomass.  It is used for GAI/biomass development calculation.      //980226
}
//_Canopy_accumulation::Portion::increment_by__________________________________/
float64 Canopy_growth_leaf_area_index_based::Canopy_accumulation::Portion::dec_living(float64 subtrahend_area_index) modification_
{  GAI -= subtrahend_area_index;
   return GAI;
}
//_dec_living_______________________________________________________2006-05-30_/
#ifdef NYN
080806 handled by superclas
void Canopy_growth_leaf_area_index_based::Canopy_accumulation::Portion::dec_biomass_current(float64 subtrahend_biomass)
{  biomass_current -= subtrahend_biomass;
}
//_Canopy_accumulation::Portion::dec_biomass_current________________2006-09-11_/
#endif
//_____________________________________________________________________________/
void Canopy_growth_leaf_area_index_based::Canopy_accumulation::Portion
::clear_GAI_related_biomass()                                      modification_
{  GAI_related_biomass = 0.0;                 // kg/m2 GAI related canopy biomass.  It is used for GAI/biomass development calculation.  // was  GAI_related_canopy_biomass
}
//_Canopy_accumulation::Portion::clear_GAI_related_biomass__________2008-05-15_/
float64  Canopy_growth_leaf_area_index_based::Canopy_accumulation::Portion::slough() renunciation_
{  // This invalidates the canopy portion the biomass amount is relinquished to the caller.
   // Returns the biomass sloughed
   GAI_related_biomass = 0;
   dead_AI = 0;
   GAI= 0;
   age = 0;
   return Canopy_growth_portioned::Canopy_accumulation::Portion::slough();
}
//_Canopy_accumulation::Portion::slough_____________________________2006-08-24_/
bool  Canopy_growth_leaf_area_index_based::Canopy_accumulation::Portion::dormancy()
{  GAI_related_biomass = 0;
   return Canopy_growth_portioned::Canopy_accumulation::Portion::dormancy();
}
//_Canopy_accumulation::Portion::dormancy___________________________2006-08-25_/
Canopy_growth_leaf_area_index_based::Canopy_growth_leaf_area_index_based
(const Crop_parameters                                   &crop_parameters_
,const Crop_parameters_struct::Canopy_growth_LAI_based   &parameters_
,Residues_interface          *residues_                                          //060816
,Crop_nitrogen_interface     *nitrogen_                                          //060816
,float64                     &reported_peak_LAI_                                 //080725
,CropSyst::Crop_CO2_response *CO2_response_                                      //080728
)
:Canopy_growth_portioned                  (crop_parameters_,residues_,nitrogen_)
, canopy_vital
   (crop_parameters_.abscission_clad.get()                                       //141202_130406
   ,false,crop_parameters_.linger_days)                                          //060911
, canopy_effete
                                         (crop_parameters_.abscission_clad.get() //141202_130406
                                             ,true,crop_parameters_.linger_days) //060911
,senescence                                                                  (0) //190630
,parameters                                                        (parameters_)
, GAI_has_died_off                                          (false) ,  resets(0) //080415
,peak_LAI                                                                  (0.0)
,reported_peak_LAI                                          (reported_peak_LAI_) //080725
,LAI_for_self_shading_response                                         (0.00001) // this was the value used on sowing when this var was in Crop_CropSyst class
,CO2_response                                                    (CO2_response_) //080726
#if (CO2_RESPONSE_VERSION==4)
// actually this did not appear to be used anymore
,leaf_area_growth_related_biomass_at_baseline_conc                         (0.0) //131208
#endif
,untouchable_portion                                                         (0) //080818
,cover_total_fract                                                         (0.0) //080101
,cover_green_fract                                                         (0.0) //080101
,orchard_shaded_area_GAI                                                   (0.0) //081020
    // eventually move to orchard submode
,canopy_porosity                                                             (0) //081020
// some of these may be made common
{}
//_Canopy_growth_leaf_area_index_based:constructor_____________________________/
bool Canopy_growth_leaf_area_index_based::Canopy_accumulation
::update()                                                        rectification_
{  bool common_updated = Canopy_growth_portioned::Canopy_accumulation::update(); //080808
   //190404 member Portion *accum_today =
   provide_accum_today();
   FOR_EACH(portion,Portion ,each_portion)
   {  accum_today->increment_by(*portion);
   } FOR_EACH_END(each_portion)
   return common_updated && accum_today != NULL;
}
//_Canopy_accumulation::update_________________________________________________/
bool Canopy_growth_leaf_area_index_based::update()                rectification_
{  Canopy_growth_portioned::update();                                            //080806
   update_fraction_of_canopy_cover();                                            //080801
   return  true;
}
//_update______________________________________________________________________/
float64 Canopy_growth_leaf_area_index_based::remove_biomass
      (float64 biomass_to_remove
      ,float64 retain_GAI        // biomass won't be removed if it takes the GAI below this level. Use 0.0 for no limitation
      ,bool remove_newest_first
      ,bool reserve_todays_portion
      ,Crop_biomass &removed_biomass // Return about of biomass removed          //070412
      )                                                                          //070412
{  float64 removed_biomass_mass = Canopy_growth_portioned::remove_biomass
      ( biomass_to_remove
      , retain_GAI        // biomass won't be removed if it takes the GAI below this level. Use 0.0 for no limitation
      , remove_newest_first
      , reserve_todays_portion
      ,removed_biomass);  // Return about of biomass removed                     //070412
   LAI_for_self_shading_response = get_GAI(include_vital|include_effete);        //080725
   return removed_biomass_mass;
}
//_remove_biomass______________________________________________________________/
bool Canopy_growth_leaf_area_index_based::start()
{  GAI_has_died_off = false;
   // This allow continued accumulation of GAI related biomass
   return Canopy_growth_portioned::start();                                      //080807
}
//_start____________________________________________________________2008-05-15_/
float64  Canopy_growth_leaf_area_index_based::update_LAI_for_self_shading_response
(bool culminesence_initiated,
 bool before_maturity)                                             modification_
{
   if (!crop_parameters.has_leaf_senescence()                                    //030521
         || crop_parameters.is_fruit_tree())                                     //190625
   {  LAI_for_self_shading_response =
               get_GAI(include_vital|include_effete);                            //080725
      if (LAI_for_self_shading_response >reported_peak_LAI)
         reported_peak_LAI = LAI_for_self_shading_response;                      //081117
   } else  // The following does not apply to the fruit model.
   {
      if (culminesence_initiated)                                                //181108
      {  // For shading the soil in the case of annual crops
         if (before_maturity && senescence)                                      //200312_181108
         {  // reduce LAI
            float64 senescence_factor = 1.0;
            senescence_factor
               = senescence->get_thermal_time_relative_remaining();              //200403_190628
               //200304 = senescence->calc_relative_thermal_time_remaining();    //190628
            float64 fract_LAI_mat =  parameters.fract_LAI_mature;                //981209
            LAI_for_self_shading_response = peak_LAI -
               (peak_LAI - peak_LAI * fract_LAI_mat) * senescence_factor;
               // On the first date this condition is true we can print the largest GAI event in schedule
         } else
            LAI_for_self_shading_response = peak_LAI * parameters.fract_LAI_mature;
      } else
      {  LAI_for_self_shading_response = std::max<float64>
            (get_GAI(include_vital|include_effete)                               //080725
               // Now we have untouchable portions so GAI is never 0.0;            080725
            ,LAI_for_self_shading_response);
         peak_LAI = std::max<float64>(LAI_for_self_shading_response,peak_LAI);   //170820
         reported_peak_LAI = peak_LAI;                                           //170820_040929
   }  }
   if (LAI_for_self_shading_response < 0.0)  LAI_for_self_shading_response = 0.0;
   LAI_for_self_shading_response =
      CORN::must_be_greater_or_equal_to<float64>
      (LAI_for_self_shading_response
      ,get_GAI(include_vital|include_effete) );                                  //080725
      // Now we have untouchable portions so we can never have GAI as 0.0; 080725
   return LAI_for_self_shading_response;                                         //080728
}
//_update_LAI_for_self_shading_response__________________________________1998?_/
float64 Canopy_growth_leaf_area_index_based::Canopy_accumulation
::get_GAI()                                                       rectification_
{  if (!is_valid()) update();
   return accum_today->get_GAI();
}
//_Canopy_accumulation::get_GAI________________________________________________/
float64 Canopy_growth_leaf_area_index_based
::get_GAI(uint8 include_GAI)                                               const
{  float64 GAI_vital  = (include_GAI & include_vital)  ? canopy_vital.get_GAI() : 0.0; //060921
   float64 GAI_effete = (include_GAI & include_effete) ? canopy_effete.get_GAI(): 0.0; //060921
   const Canopy_accumulation::Portion *this_untouchable_portion = (const Canopy_accumulation::Portion *)untouchable_portion; //080807
   float64 GAI_untouchable = (include_GAI & include_vital)  && this_untouchable_portion   //071126
         ? this_untouchable_portion->get_LAI() // Even though this method is to get GAI for we need to presume the untouchable_portion is always green
         : 0.0;
   return GAI_vital + GAI_effete + GAI_untouchable;                              //071126
}
//_Canopy_growth_leaf_area_index_based::get_GAI________________________________/
float64 Canopy_growth_leaf_area_index_based::Canopy_accumulation
::get_LAI()                                                             mutates_
{  if (!is_valid()) update();
   return (accum_today)->get_LAI();
}
//_Canopy_accumulation::get_LAI_____________________________________2006-09-11_/
float64 Canopy_growth_leaf_area_index_based::get_LAI()                     const
{  // Canopy portion gets updated if necessary  080808
   const Canopy_accumulation::Portion *this_untouchable_portion = (const Canopy_accumulation::Portion *)untouchable_portion; //080807
   float64 sum_LAI
      = canopy_vital.get_LAI()
      + canopy_effete.get_LAI()
      + (this_untouchable_portion ? this_untouchable_portion->get_LAI() : 0.0);  //071126
   return sum_LAI;
}
//_get_LAI__________________________________________________________2006-08-24_/
bool Canopy_growth_leaf_area_index_based::respond_to_clipping()    modification_
{  GAI_has_died_off = false;
   peak_LAI = 0;
   invalidate();                                                                 //110501
   return update();                                                              //110824_110501
}
//_respond_to_clipping______________________________________________2008-05-15_/
bool Canopy_growth_leaf_area_index_based::restart_with
(float64 restart_biomass,float64 restart_GAI
,bool use_clump_factor)                                            modification_
{  LAI_for_self_shading_response  = restart_GAI;
   float64 canopy_biomass_curr = get_biomass_current(include_vital|include_effete); //070321
   float64 canopy_biomass_to_add =
      CORN::must_be_0_or_greater<float64>
      (restart_biomass - canopy_biomass_curr);                                   //070321
   float64 GAI_curr =                                                            //070321
         get_GAI(include_vital|include_effete);                                  //080725
         // Now we have untouchable portions so  we can never have GAI as 0.0;   //080725
   float64 GAI_to_add =
      CORN::must_be_0_or_greater<float64>
      (restart_GAI -  GAI_curr);                                                 //070321
   Canopy_growth_leaf_area_index_based::Canopy_accumulation::Portion *portion =  //060531
      new Canopy_growth_leaf_area_index_based::Canopy_accumulation::Portion
         (0,GAI_to_add,canopy_biomass_to_add,canopy_biomass_to_add);             //070321
   take_portion(portion,true,true);                                              //180612_050823
   float64 _clumping = use_clump_factor ? clumping(restart_GAI ,0.35): 1.0;      //000630
   float64 kc = crop_parameters.morphology.light_extinction_coef;                //200115
   float64 fract_cover = 1.0 - exp(-kc * restart_GAI *_clumping );
   cover_total_fract    = fract_cover;
   cover_green_fract     = fract_cover;
   return update();                                                              //061006
}
//_restart_with___________________________________________________________2000_/
bool Canopy_growth_leaf_area_index_based::develop_leaf
(bool     continue_leaf_growth
,float64  act_biomass_growth                                                     //011022
,float64  GAI_related_biomass_growth  // Today's GAI related growth              //980206
,float64  thermal_time_growing_degree_day)                                       //080728
{  float64 daily_GAI = 0.0;                                                      //980226
   #if (CO2_RESPONSE_VERSION==4)
   // actually this did not appear to be used anymore
   float64 daily_GAI_at_baseline_concX = 0.0;                                     //980226
   #endif
   if (continue_leaf_growth)
   {  float64 specific_leaf_area = parameters.specific_leaf_area;                //980213
      float64 stem_leaf_partition = parameters.stem_leaf_partition;              //981204
      float64 GAI_canopy_biomass =  get_GAI_related_biomass();                   //080725
      float64 GAI_canopy_biomass_new = GAI_canopy_biomass + GAI_related_biomass_growth; //080515
      daily_GAI = GAI_related_biomass_growth * specific_leaf_area                //980226
           / CORN_sqr(stem_leaf_partition * GAI_canopy_biomass_new + 1.0);       //980226
   }
   return take_portion                                                           //050823
      (new Canopy_growth_leaf_area_index_based::Canopy_accumulation::Portion(
         thermal_time_growing_degree_day                                         //080728
         ,daily_GAI                                                              //050823
      ,act_biomass_growth,GAI_related_biomass_growth),false,false);              //180612_050823_050823
}
//_develop_leaf_____________________________________________________2008_______/
bool Canopy_growth_leaf_area_index_based::Canopy_accumulation::Portion::age_by
(float64 daily_age_deg_day,float64 leaf_duration_deg_day_f32)
{  age += daily_age_deg_day;
   bool senesce_now = (age > leaf_duration_deg_day_f32) && !is_dead();           //060316
   if (senesce_now) die();
   return senesce_now;;                                                          //060316
}
//_Canopy_accumulation::Portion::age_by_____________________________2005-08-23_/
void Canopy_growth_leaf_area_index_based::age_by
(float64 daily_age_deg_day
,float64 leaf_duration_deg_day_f32)                        modification_
{  FOR_EACH_IN(vital_portion,Canopy_accumulation::Portion  ,canopy_vital,each_vital_portion)
   {  bool just_died = vital_portion->age_by(daily_age_deg_day,leaf_duration_deg_day_f32);
      if (just_died)
         GAI_has_died_off = true;
   } FOR_EACH_END(each_vital_portion)
   FOR_EACH_IN(effet_portion,Canopy_accumulation::Portion ,canopy_effete,each_effete_portion)
   {  //170217unused bool just_died =
      effet_portion->age_by(daily_age_deg_day,leaf_duration_deg_day_f32);
   } FOR_EACH_END(each_effete_portion)
   // Do not age untouchable portion.
   invalidate();
   if ((GAI_has_died_off && resets == 0))                                        //081110
   {  //go through and clear the GAI_related canopy biomasses for all vital portions
      FOR_EACH_IN(vital_portion,Canopy_accumulation::Portion ,canopy_vital,each_vital_portion)
         {  vital_portion->clear_GAI_related_biomass();
      } FOR_EACH_END(each_vital_portion)
      resets ++;
      GAI_has_died_off = false;
   }
}
//_develop_leaf_____________________________________________________2008-05-15_/
bool Canopy_growth_leaf_area_index_based::age_leaf
(float64 thermal_time_growing_degree_day                                         //080728
,float64 water_stress_index)
{  float64 daily_age =                                                           //020628
      thermal_time_growing_degree_day                                            //080726
      * (1 + parameters.leaf_duration_sensitivity_stress * water_stress_index);  //010726
   if (crop_parameters.has_leaf_senescence())                                    //970521
      age_by(daily_age,parameters.leaf_duration_deg_day);                        //050823
   return true;
}
//_age_leaf_________________________________________________________2007-03-28_/
/*190708 obsolete
#if (CO2_RESPONSE_VERSION==4)
// actually this did not appear to be used anymore
#ifdef CO2_CHANGE
float64 Canopy_growth_leaf_area_index_based::get_GAI_canopy_biomass_at_baseline_conc_kg_m2() const
{  float64 sum_daily_GAI_canopy_biomass_at_baseline_conc = get_GAI_related_biomass_at_baseline_conc();
   return sum_daily_GAI_canopy_biomass_at_baseline_conc;
}
#endif
//_get_GAI_canopy_biomass_at_baseline_conc_kg_m2_______________________________/
#endif
*/
bool Canopy_growth_leaf_area_index_based::emerge_cotyledon
(float64 new_cotalydon_GAI
,float64 new_cotalydon_BM)                                         modification_
{
   return take_portion(new Canopy_growth_leaf_area_index_based::Canopy_accumulation::Portion(0,new_cotalydon_GAI,new_cotalydon_BM,new_cotalydon_BM
   /*190708 obsolete
   #if (CO2_RESPONSE_VERSION==4)
   // actually this did not appear to be used anymore

   #ifdef CO2_CHANGE
      , new_cotalydon_GAI,new_cotalydon_BM
   #endif
   #endif
   */
      ),true,true);
}
//_emerge_cotyledon_________________________________________________2008-07-28_/
void Canopy_growth_leaf_area_index_based::terminate()              modification_
{  // Makes the untouchable portion available for removal with biomass fate.
   LAI_for_self_shading_response = 0.0;
   if (untouchable_portion)
   {  canopy_vital.append(untouchable_portion);
      untouchable_portion = 0;
   }
}
//_terminate________________________________________________________2008-07-28_/
float64 Canopy_growth_leaf_area_index_based::get_LAI_to_LAImax_fract()     const
{  return  std::min<float64>((LAI_for_self_shading_response/parameters.max_LAI),1.0);
}
//_get_LAI_to_LAImax_fract__________________________________________2008-07-28_/
float64 Canopy_growth_leaf_area_index_based
::get_senescence_reduction_factor()                                        const
{
   float64 GAI_vital_plus_effete = get_GAI(include_vital|include_effete);
   return (CORN::is_zero<float64>(peak_LAI) //200127 ,0.0,0.00001)
      ? 0.0  // avoid div0 error
      : GAI_vital_plus_effete
      / peak_LAI);
}
//_get_senescence_reduction_factor__________________________________2008-08-07_/
float64 Canopy_growth_leaf_area_index_based
::calc_adjusted_ET_crop_coefficient()                                      const
//200127 was get....
{

//conceptual
//  This should actually be consistent as in canopy growth cover based (applying the defined limit)
// using fract_canopy_cover_total
//    float64 fract_canopy_cover_total = get_canopy_interception_global_green();//120405
// instead of LAI

   float64 LAI = get_LAI(false);
   float64 param_ET_coef = crop_parameters.transpiration.ET_coef;                //110218
   float64 adjusted_ET_coef =
      ((param_ET_coef > 1.0) && (LAI < 3.0))
      ? (1.0 + (param_ET_coef -1.0) * LAI / 3.0)  // adjusting for LAI up to 3.0 //980311
      : param_ET_coef;
   return adjusted_ET_coef;
}
//_calc_adjusted_ET_crop_coefficient________________________________2008-08-11_/
float64 Canopy_growth_leaf_area_index_based::get_LAI(bool from_canopy)     const
{ return from_canopy ? get_LAI() : LAI_for_self_shading_response; }
//_get_LAI__________________________________________________________2006-08-24_/
void Canopy_growth_leaf_area_index_based::update_fraction_of_canopy_cover() rectification_
{  cover_total_fract    = calc_cover_fract(get_LAI(false),false,true,false);
   cover_green_fract    = calc_cover_fract
        (get_GAI(include_vital|include_effete),false,true,true); // indeed both  080801
}
//_update_fraction_of_canopy_cover__________________________________2003-11-30_/
float64 Canopy_growth_leaf_area_index_based::clumping(float64 xAI,float64 area_factor) const
{  // This is analagous to row_intercepting for row crops
   static const float64 clump_factor = 0.75;                                     //070810
   return (clump_factor + (1.0-clump_factor) * (1.0-exp(-area_factor * xAI)));
}
//_clumping_________________________________________________________2000-06-23_/
float64 Canopy_growth_leaf_area_index_based::calc_cover_fract
(float64 xAI    // Either LAI or  GAI
,bool photosynthetic_active_rad_based
,bool use_clumping_factor                                                        //080801
,bool unused // applicable only to fruit trees                                   //081104
) const
{   // Note that this method is overridden by fruit tree.
   float64 light_extinction_coef
      = crop_parameters.morphology.light_extinction_coef;                        //200115_130406
   // Correct kc parameter which is total radiation based.                       //011022
   // Kc based on photosynthetic active radiation is needed
   if (photosynthetic_active_rad_based)                                          //011108
      light_extinction_coef = std::min<float64>(light_extinction_coef *1.4,0.9); //111017_000716
      // kc will be total radiation based entered in the parameter edtiros.
   float64 clumping_factor = use_clumping_factor
      ?  clumping(xAI,(photosynthetic_active_rad_based? 0.25: 0.35)) : 1.0;      //080801
   // eventually calc_cover_fract will be specialized for a yet to be define crop_orchard canopy growth submodel
   float64 canopy_interception  =
    (xAI > 0.00001)                                                              //071121
    ? 1.0 - exp(-light_extinction_coef * xAI * clumping_factor)                  //011116
    : 0.0;
   float64 result_fract_canopy_cover = canopy_interception;                      //071121
   return  result_fract_canopy_cover;                                            //071121
}
//_calc_cover_fract_________________________________________________2000-06-23_/
bool Canopy_growth_leaf_area_index_based::end_day()                modification_
{  // This could actually be done at the end of the day, but I
   // simply didn't want to bother creating end_day() methods for
   // all the classes.
   // If I ever do, make sure to move this to the end_day() method
//081105 yesterday_fract_green_canopy_solrad_interception = cover_green_fract;   //080327
                  // Transpiration model uses the fact_canopy_cover from the day before
                  // This is because update_pot_evaptranspiration which uses
                  // fact_canopy_cover is called before today's fact_canopy_cover is calculated.
                  // This is also how it is done in the V.B. code.
#ifdef DEBUG_CANOPY_PORTIONS
dump_ages(canopy_portions_stream);
#endif
   return true;
}
//_start_day________________________________________________________2008-08-01_/
float64 Canopy_growth_leaf_area_index_based::calc_plant_development
(float64 length_max
,float64 last_length
,float64 overall_growth_stress                                                   //080805
) const
{  float64 LAI_fract = get_LAI_to_LAImax_fract();                                //080728
   float64 development =
    std::max<float64>(last_length ,                                              //111017
        std::min<float64>(pow(LAI_fract,overall_growth_stress ) * length_max,
            1.15 * LAI_fract * length_max ));   // was 1.1
  return development;
}
//_calc_plant_development___________________________________________1999-03-26_/
float64 Canopy_growth_leaf_area_index_based::get_influence_on_root_activity_factor
(bool after_end_of_canopy_growth)                                      const
{  float64 peak_LAI = get_peak_LAI();
   float64 root_activity_factor =                                                //080728
      (!crop_parameters.is_perennial()                                           //000808
         && after_end_of_canopy_growth
         && peak_LAI > 0.0)
         ?  CORN::must_be_less_or_equal_to<float64>
               (get_GAI(include_vital) / peak_LAI,1.0)  // indeed vital
         : 1.0;
   // Reduces root activity similar to the way GAI is senescing.
   // GAI vital is GAI for the new season
   return root_activity_factor;
}
//_get_influence_on_root_activity_factor____________________________2008-08-05_/
Canopy_growth_leaf_area_index_based::Canopy_accumulation::Portion*
Canopy_growth_leaf_area_index_based::Canopy_accumulation
::provide_accum_today()                                               provision_
{
   if (!accum_today)                                                             //190404
      accum_today =  new
      Canopy_growth_leaf_area_index_based::Canopy_accumulation::Portion;
   return accum_today;
}
//_provide_accum_today_________________________________________________________/
bool Canopy_growth_leaf_area_index_based::Canopy_accumulation
::invalidate()                                                     modification_
{  delete accum_today;  accum_today= 0;
   return true;
}
//_invalidate__________________________________________________________________/
#ifdef FOR_DEBUG_OUTPUT
float64 Canopy_growth_leaf_area_index_based::get_live_and_or_dead_biomass
(bool include_vital,bool include_effete,bool include_dead)                 const
{  // Note, could make the live_biomass a property of portion
 // an simply add this with the reset or update()
   if (!is_valid()) update();
   float64 biomass = 0.0;  // kg_m2
   FOR_EACH_IN(portion,Portion  ,canopy_vital,each_vital_portion)
   {  if (include_dead && portion->is_dead())
         biomass += portion->get_biomass_current();  // produced and current should be the same for live
      if (include_vital && (portion->get_GAI() > 0.0000001))
         biomass += portion->get_biomass_current();  // produced and current should be the same for live
   } FOR_EACH_END(each_vital_portion)
   FOR_EACH_IN(portion,Portion  ,canopy_effete,each_effete_portion)
   {  if (include_dead && portion->is_dead())
         biomass += portion->get_biomass_current();  // produced and current should be the same for live
      if (include_effete && (portion->get_GAI() > 0.0000001))
         biomass += portion->get_biomass_current();  // produced and current should be the same for live
   } FOR_EACH_END(each_effete_portion)
   biomass +=untouchable_portion?untouchable_portion->get_biomass_current():0.0; //071126
  return biomass;
}
//_invalidate_______________________________________________________2006-09-07_/
#endif
void Canopy_growth_leaf_area_index_based::dump_ages(std::ofstream &tostream)
{  static nat32 day=0;
   day++;
   if (canopy_effete.count())
   {  tostream << "day " <<(int)day << " effete:"
          << '\t'<< "ID"
          << '\t'<< "age"
          << '\t'<< "dead"
          << '\t'<< "GAI"
          << '\t'<< "AI_dead"
          << '\t'<< "BM_prod"
          << '\t'<< "BM_curr"
          << '\t'<< "BM_GAI"
          << '\t' << std::endl ;
      FOR_EACH_IN(effete_portion,Canopy_accumulation::Portion ,canopy_effete,each_effete_portion)
      {  tostream << std::endl;
          effete_portion->write(tostream);
      }FOR_EACH_END(each_effete_portion)
   }
   if (canopy_vital.count())
   {
       tostream << "day " << (int)day << " vital:"
             << '\t'<< "ID"
             << '\t'<< "age"
             << '\t'<< "dead"
             << '\t'<< "GAI"
             << '\t'<< "AI_dead"
             << '\t'<< "BM_prod"
             << '\t'<< "BM_curr"
             << '\t'<< "BM_GAI"
             << '\t' << std::endl ;
       FOR_EACH_IN(vital_portion,Canopy_accumulation::Portion ,canopy_vital,each_vital_portion)
          vital_portion->write(tostream);
       FOR_EACH_END(each_vital_portion)
    }
    if (untouchable_portion)                                                     //071126
       tostream
          << "day " << (int)day <<" untouchable portion:"
          << '\t'<< "ID"
          << '\t'<< "age"
          << '\t'<< "dead"
          << '\t'<< "GAI"
          << '\t'<< "AI_dead"
          << '\t'<< "BM_prod"
          << '\t'<< "BM_curr"
          << '\t'<< "BM_GAI"
          << '\t' << std::endl
          << '\t' << (int)
		#if (__LP64__ || _M_X64)
		(uint64_t)
		#endif
		untouchable_portion
          << "\t"<< untouchable_portion->get_age()
          << "\t0" // never dead
          << "\t" << untouchable_portion->get_GAI()
          << "\t0" // never dead
          << "\t" << untouchable_portion->get_biomass_produced()
          << "\t" << untouchable_portion->get_biomass_current()
          << "\t" << untouchable_portion->get_GAI_related_biomass();
   tostream << std::endl;
}
//_dump_ages________________________________________________________2007-07-05_/
float64 Canopy_growth_leaf_area_index_based::get_GAI_related_biomass()     const
{  if (!is_valid()) update();
   const Canopy_accumulation::Portion *canopy_vital_accum_today
      = (const Canopy_accumulation::Portion *)(canopy_vital.accum_today);        //080807
   const Canopy_accumulation::Portion *canopy_effete_accum_today
      = (const Canopy_accumulation::Portion *)(canopy_effete.accum_today);       //080807
   const Canopy_accumulation::Portion *this_untouchable_portion
      = (const Canopy_accumulation::Portion *)untouchable_portion;               //080807
   float64 GAI_related_biomass_vitale     = canopy_vital_accum_today
      ? canopy_vital_accum_today ->get_GAI_related_biomass() : 0.0;
   float64 GAI_related_biomass_effete     = canopy_effete_accum_today
      ? canopy_effete_accum_today->get_GAI_related_biomass():0.0;
   float64 GAI_related_biomass_untouchable= this_untouchable_portion
      ? this_untouchable_portion ->get_GAI_related_biomass() : 0.0;
   return
      GAI_related_biomass_vitale + GAI_related_biomass_effete
      + GAI_related_biomass_untouchable; // Note the effete may already be 0
}
//_get_GAI_related_biomass_____________________________________________________/
float64 Canopy_growth_leaf_area_index_based::increment_untouchable_portion
(Canopy_growth_portioned::Canopy_accumulation::Portion &by_portion)modification_
{  Canopy_growth_leaf_area_index_based::Canopy_accumulation::Portion & local_by_portion
       = (Canopy_growth_leaf_area_index_based::Canopy_accumulation::Portion &)by_portion;
   untouchable_portion->increment_by(local_by_portion);
   return untouchable_portion->get_biomass_current();
}
//_increment_untouchable_portion____________________________________2008-08-18_/
bool Canopy_growth_leaf_area_index_based::take_untouchable_portion
(Canopy_growth_portioned::Canopy_accumulation::Portion *_untouchable_portion)
{  untouchable_portion
      = (Canopy_growth_leaf_area_index_based::Canopy_accumulation::Portion *)_untouchable_portion;
   return true;
}
//_take_untouchable_portion_________________________________________2008-08-18_/
float64 Canopy_growth_leaf_area_index_based
::get_interception_PAR_green()                                             const
{  float64 sum_daily_GAI = get_GAI(include_vital|include_effete);                //050824
   float64 GAI_vital_plus_effete =
    CORN::must_be_greater_or_equal_to<float64>(sum_daily_GAI, 0.00001);
    // Now have untouchable canopy portion so the check for is probably obsolete //080726
   return calc_cover_fract(GAI_vital_plus_effete,true,true,false);
}
//_get_interception_PAR_green_canopy________________________________2011-09-01_/
float64 Canopy_growth_leaf_area_index_based
::calc_interception_global_green()                                         const
{  float64 sum_daily_GAI = get_GAI(include_vital|include_effete);                //050824
   float64 GAI_vital_plus_effete = CORN::must_be_greater_or_equal_to<float64>(sum_daily_GAI, 0.00001);
   // Now have untouchable canopy portion so the check is probably obsolete      //080726
   return calc_cover_fract(GAI_vital_plus_effete,false,true,true);
}
//_calc_interception_global_green___________________________________2011-09-04_/
float64 Canopy_growth_leaf_area_index_based::get_interception_global_green() const
{
   // This is was always calculating, but probably only need to calculated once
   return calc_interception_global_green();
}
//_get_interception_global_green____________________________________2020-02-08_/
}//_namespace CropSyst_________________________________________________________/

