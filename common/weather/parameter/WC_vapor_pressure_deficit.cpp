#include "weather/weather_types.h"
#include "weather/parameter/WC_vapor_pressure_deficit.h"
#include "weather/parameter/WP_vapor_pressure_act.h"
#include "weather/parameter/WP_relative_humidity.h"
#include "weather/parameter/WP_dew_point_temperature.h"
#include "weather/parameter/WP_air_temperature.h"
#include "weather/loc_param.h"
using namespace CORN;
//______________________________________________________________________________
float64 Vapor_pressure_deficit_max_calculator::saturation_at_max_dew_point
   // probably rename this to calc_saturation?
(bool over_water_only)                                                     const
{  float64 air_temp_max = air_temperature_max->get_value_update_if_necessary();
   float64 svp_max_temp = calc_saturation_kPa
      (air_temp_max,air_temp_max,over_water_only);                               //040114
   float64 VPD_max = get_value_update_if_necessary();
   float64 svp_max_dew_pt =  svp_max_temp - VPD_max;
   return svp_max_dew_pt;                                                        //970723
}
//_saturation_at_max_dew_point______________________________________1997-07-23_/
const Quality &Vapor_pressure_deficit_max_calculator::update()    rectification_
{  if (!updating)
   {  updating = true;
      {  // First try calculating from temperature, this should have the lowest quality
         Quality_clad  max_VPD_quality;
         float64 VPD_max = est_from_temperature(max_VPD_quality);
         update_value_qualified(VPD_max,max_VPD_quality);                        //150123
            // set only updates if this is the base quality so far
      }
      {  // Next try calculating from relative humidity , this should have the mid quality
         Quality_clad  max_VPD_quality;
         float64 VPD_max = calc_from_relative_humidity_min(max_VPD_quality);
         if (relative_humidity_min->is_valid())                                  //200227
            // make sure to override quality from VPD_max estimated from temperature
            force_value_qualified(VPD_max,max_VPD_quality);                      //200227_150123
         //200227    update_value_qualified(VPD_max,max_VPD_quality);            //150123
            // set only updates if this is the base quality so far
      }
      {  // Finally try calculating from dew point temperature , this should have the best quality
         Quality_clad  max_VPD_quality;
         float64 VPD_max = calc_from_dew_point_temperature_max(max_VPD_quality);
         update_value_qualified(VPD_max,max_VPD_quality);                        //150123
            // set only updates if this is the base quality so far
      }
      updating = false;
   }
   return get_quality();
}
//_update___________________________________________________________2008-10-27_/
float64 Vapor_pressure_deficit_max_calculator::calc_rel_humid
(float64 max_or_min_sat_vapor_press
,float64 min_or_max_relative_humidity)                                     const
{  float64 vpd_rh =(max_or_min_sat_vapor_press * (1.0 - min_or_max_relative_humidity/100.0));
   return vpd_rh;
}
//_calc_rel_humid___________________________________________________1997-07-10_/
float64 Vapor_pressure_deficit_max_calculator::calc_from_relative_humidity_min   // rename this to calc_from_relative_humidity_min
(Quality &VPDmax_quality)                                                  const
{  VPDmax_quality.assume_code(unknown_quality);                                  //150123
   float64 VPDmax_from_RH = 0.0;
   if (air_temperature_max && relative_humidity_min)                             //070212
   {  // Air temperature parameter is required.
      float64 RH_min = relative_humidity_min->get_value_update_if_necessary();   //150126_081104
      if (relative_humidity_min->is_valid())                                     //091211
      {
         float64 air_temp_max = air_temperature_max->get_value_update_if_necessary();//150126_050831
         const Quality &contrib_quality
            = (air_temperature_max->worst(*relative_humidity_min));              //991014
         if (contrib_quality.is_valid())                                         //081027
         {  VPDmax_quality.assume_code(quality_calculated_from_quality(contrib_quality));  //081104
            VPDmax_from_RH  = calc_rel_humid
               (calc_saturation_kPa(air_temp_max,air_temp_max,OVER_WATER_ONLY)  // 081031 claudio confirms OVER_WATER_ONLY!!!!
               ,RH_min);
         }
      } // else RHmin in not available
   } else  VPDmax_quality.assume_code(missing_quality);
   return  VPDmax_from_RH;
}
//_calc_from_relative_humidity_min__________________________________1998-03-11_/
float64 Vapor_pressure_deficit_max_calculator::calc_from_dew_point_temperature_max
(Quality &identified_quality) const
{  identified_quality.assume_code(unknown_quality);
   float64 VPD_max = 0.0;
   if (air_temperature_max && dew_point_temperature_max)
   {
      float64 air_temp_max = air_temperature_max      ->get_value_update_if_necessary(); //150126_050831
      float64 dewpt_max    = dew_point_temperature_max->get_value_update_if_necessary(); //150126_081028
      const Quality &contrib_quality = air_temperature_max->worst(*dew_point_temperature_max);    //050602
      if (contrib_quality.is_valid())
      {  identified_quality.assume_code(quality_calculated_from_quality(contrib_quality)); //150126_991014
         VPD_max = calc_saturation_kPa(air_temp_max,air_temp_max,OVER_WATER_ONLY)//081031_040114 claudio confirms OVER_WATER_ONLY!!!!
                 - calc_saturation_kPa(dewpt_max  ,air_temp_max,OVER_WATER_ONLY);//050901
      } else identified_quality.assume_code(not_valid_quality);
   } else identified_quality.assume_code(missing_quality);
   return VPD_max;
}
//_calc_from_dew_point_temperature_max______________________________1998-03-11_/
float64 Vapor_pressure_deficit_max_calculator::est_from_temperature
(Quality &identified_quality) const// kPa
{  identified_quality.assume_code(unknown_quality);
   float64 VPD_max = 0.0;
   if (air_temperature_max && air_temperature_min && generation_parameters)
   {
      float64 air_temp_max =air_temperature_max->get_value_update_if_necessary();//150126
      float64 air_temp_min =air_temperature_min->get_value_update_if_necessary();//150126_050831
      const Quality &worst_temp_qual = air_temperature_max->worst(*air_temperature_min);       //050609
      Quality_clad contrib_qual(worst_temp_qual);                                //170804
      Quality_clad aridity_factor_qual(generation_parameters->get_aridity_factor_quality());   //050609
      Quality_clad VPD_curve_qual     (generation_parameters->get_curve_quality());   //050609
      if (worst_temp_qual.is_valid())
      {
         float64 VP_sat_max = calc_saturation_kPa                                //970711
            (air_temp_max,air_temp_max,OVER_WATER_ONLY);                         //040114
         float64 VP_sat_min
            = calc_saturation_kPa(air_temp_min,air_temp_min,OVER_WATER_ONLY);    //970711
         float64 VPD_max_unadjusted = VP_sat_max - VP_sat_min;                   //970711
            // I think VPD_max_unadjusted is actually VPD_max_unadjusted         //170804
         VPD_max = VPD_max_unadjusted;                                           //171201
         if (aridity_factor_qual.is_valid())                                     //170804
         {  float64 aridity_factor = generation_parameters->aridity_factor;      //091214
            VPD_max = VPD_max_unadjusted
               / ( 1.0 - aridity_factor *  VPD_max_unadjusted);                  //980304
            contrib_qual.assume(aridity_factor_qual);                            //170804
         } else                                                                  //170804
         {  // This is an estimate when aridity_factor is unknown.
            float64 VPDmax_adj = generation_parameters->VPDmax_adj.in_prescribed_units(); //170811
            if (generation_parameters->VPDmax_adj.is_valid())                    //170808
            {  VPD_max = VPD_max_unadjusted / VPDmax_adj;                        //170804
               identified_quality.assume_code(CORN::estimated_quality);          //170811
            } else identified_quality.assume_code(CORN::missing_quality);        //1700929
         }
         // The following corrects for upper end curve sagging in ClimGen
         // aridity factor computation, otherwise take VPD max as is. //970711
         if (VPD_curve_qual.is_valid())                                          //170804
         {  VPD_max = (VPD_max * generation_parameters->slope)
               + generation_parameters->intercept;                               //970711
            contrib_qual.assume(VPD_curve_qual);                                 //170804
         }
         identified_quality.assume_code(quality_generated_from_quality(contrib_qual)); //170804
      }
   } else identified_quality.assume_code(missing_quality);
   return CORN::must_be_between<float64>(VPD_max,0.0,7.0);                       //990423
}
//_est_from_temperature________________________________________________________/
const Quality &Vapor_pressure_deficit_fullday_calculator::update() rectification_
{  if (!updating)                                                                //081027
   {  updating = true;                                                           //081027
      float64 fullday_VPD = 0.0;
      Quality_clad fullday_VPD_quality(CORN::unknown_quality);                   //080627
      Quality_clad missing(missing_quality);                                     //101209
      Quality_clad quality_if_calc_from_temperature_saturated_vapor_pressure     //080627
         ( vapor_pressure_act
            ? worst_quality_of_3
               (vapor_pressure_act ->get_quality_code()
               ,air_temperature_max->get_quality_code()
               ,air_temperature_min->get_quality_code())
            : missing_quality);                                                  //101209
           Quality_clad quality_if_calc_from_vapor_pressure_deficit_max          //080627
               (vapor_pressure_deficit_max
                  ? vapor_pressure_deficit_max->get_quality_code()
                  : missing_quality);                                            //150126 101208
// 080617   Note I could clean up the quality checking a bit here.
// NYI      I should not be attempting to calculate the VPD
//          if the currently fullday_VPD quality is already better than
//          what the calculated qualities would be.
//          I should probably create functions prequalify_calculate_from_saturated_vapor_pressure
//          that would take the VPD quality (I.e. It can now be a value read from the UED file)
//          and return true if the calculated value quality would be better than the current VPD quality.
//          Currently is does check the quality, but only after we do the calculations
//          and are attempting to set the value.

      fullday_VPD =
         (quality_if_calc_from_temperature_saturated_vapor_pressure              //081109
            .is_better_than(quality_if_calc_from_vapor_pressure_deficit_max))    //080627
         ? calculate_from_temperature_saturated_vapor_pressure(fullday_VPD_quality)
         : calculate_from_max_vapor_pressure_deficit          (fullday_VPD_quality);//050801
      update_value_qualified(fullday_VPD,fullday_VPD_quality);                   //150121
      updating = false;                                                          //081027
   }
   return get_quality();                                                         //150121_080908
}
//_Vapor_pressure_deficit_fullday_parameter::update_________________2008-09-08_/
float64 Vapor_pressure_deficit_fullday_calculator
::calculate_from_temperature_saturated_vapor_pressure
(Quality &identified_quality)                                              const
{
   float64 Tmax = air_temperature_max->get_value_update_if_necessary();          //150126_080627
   float64 Tmin = air_temperature_min->get_value_update_if_necessary();          //150126_080627
   float64 SVP_at_Tmax =  vapor_pressure_act->calc_saturation_kPa(Tmax,Tmax,OVER_WATER_ONLY); // es(Tmax)
   float64 SVP_at_Tmin =  vapor_pressure_act->calc_saturation_kPa(Tmin,Tmin,OVER_WATER_ONLY); // es(Tmin)

   float64 mean_saturation_vapor_pressure = (SVP_at_Tmax + SVP_at_Tmin) / 2.0;
   float64 actual_vapor_pressure = vapor_pressure_act->get_value_update_if_necessary();//150126 ea
   float64 VPD = mean_saturation_vapor_pressure - actual_vapor_pressure;
   identified_quality.assume_code
      (quality_calculated_from_quality
         (vapor_pressure_act->worst(air_temperature_max->worst(*air_temperature_min)))); //081109

   return VPD;
}
//_calculate_from_temperature_saturated_vapor_pressure______________2001-10-08_/
float64 Vapor_pressure_deficit_fullday_calculator::calculate_from_max_vapor_pressure_deficit
(Quality &identified_quality) const
{  static const float64 daily_max_to_fullday_mean_VPD_const = 0.55;              //170804
      // daily_max_to_fullday_mean_VPD_const is derived from 10 locations
      // around the world from very wet to very dry (Claudio).
      // was 0.45 but this was based on smaller set of locations.
   // If rel humid or dew point is not available, derive from VPD_max (which will have been computed from temperature).
   // This is important now for Priestly Taylor                                  //020212
   float64 daily_max_to_mean_VPD_const = daily_max_to_fullday_mean_VPD_const;    //010125
   float64 fullday_VPD = vapor_pressure_deficit_max->get_value_update_if_necessary()
      * daily_max_to_mean_VPD_const;                                             //150126_991109
   identified_quality.assume_code(quality_calculated_from_quality(*vapor_pressure_deficit_max)); //150126_081109
   return fullday_VPD;
   // since computed from max_VPD, avg VPD quality will be same as max VPD quality
}
//_calculate_from_max_vapor_pressure_deficit________________________2001-10-08_/
const Quality &Vapor_pressure_deficit_daytime_calculator::update() rectification_
{  if (!updating)                                                                //081027
   {  updating = true;                                                           //081027
      Quality_clad daytime_VPD_quality;
      if (vapor_pressure_deficit_max && vapor_pressure_deficit_max->is_valid())  //150125
      {  float64 daytime_VPD = estimate_from_VPD_max(daytime_VPD_quality);
         update_value_qualified(daytime_VPD,daytime_VPD_quality);                //150121
      }
      updating = false;                                                          //081027
   }
   return get_quality();
}
//_update___________________________________________________________2008-09-08_/
static const float64 daily_max_to_daytime_mean_VPD_constant = 0.666666; // cited as 2/3
float64 Vapor_pressure_deficit_daytime_calculator::estimate_from_VPD_max(Quality &quality) const
{  float64 VPD_max = vapor_pressure_deficit_max->get_value_update_if_necessary();//150126
   float64 VPD_daytime = VPD_max * daily_max_to_daytime_mean_VPD_constant;       //991109
   if (VPD_daytime <= 0.0)  VPD_daytime = 0.00001;                               //140813 // This used to be in Crop_cropsyst
   quality.assume_code(quality_estimated_from_quality(*vapor_pressure_deficit_max));//081111
   return  VPD_daytime;
}
//_estimate_from_VPD_max____________________________________________2004-08-01_/
void Vapor_pressure_deficit_max_calculator::know_generation_parameters
(const Parameters
 &_generation_parameters) contribution_
{  generation_parameters = &_generation_parameters;
}
//_know_generation_parameters_______________________________________2009-12-14_/
Vapor_pressure_deficit_max_calculator::Parameters::Parameters()
: aridity_factor              (DEF_aridity_factor)
, slope                       (DEF_VPD_slope)
, intercept                   (DEF_VPD_intercept)
, VPDmax_adj                       (false,0.0,UC_unitless,CORN::missing_quality) //170808
, VPDavg_seasonal_fullday_observed (false,0.0,UC_kPa,CORN::missing_quality)      //170808
, RHavg_seasonal_observed          (false,0.0,UC_percent,CORN::missing_quality)  //170808
{}
//_Vapor_pressure_deficit_max_calculator::Parameters:constructor_______________/
void Vapor_pressure_deficit_max_calculator::Parameters::invalidate()
{  aridity_factor             = 0.0;
   slope                      = DEF_VPD_slope;   ///1.0;
   intercept                  = DEF_VPD_intercept;  //0.0;
   VPDmax_adj.clear();                                                           //170808
   VPDavg_seasonal_fullday_observed.clear();                                     //170808
   RHavg_seasonal_observed.clear();                                              //170808
}
//_invalidate__________________________________________________________________/
CORN::Quality_code Vapor_pressure_deficit_max_calculator::Parameters
::get_curve_quality()                                                      const
{  // we only check the slope because the intercept can be 0.0
   return (CORN::is_approximately<float32>(slope ,DEF_VPD_slope ,0.00001))
       ? CORN::missing_quality                                                   //081016
       : CORN::calculated_quality;
}
//_get_curve_quality________________________________________________2005-06-10_/
CORN::Quality_code Vapor_pressure_deficit_max_calculator::Parameters
::get_aridity_factor_quality()                                             const
{   return (CORN::is_approximately<float32>(aridity_factor,DEF_aridity_factor,0.00001)
         ||CORN::is_approximately<float32>(aridity_factor,0.0,0.00001)
         ||CORN::is_approximately<float32>(aridity_factor,1.0,0.00001))
      // 1.0 would cause a division by 0 error
   ? CORN::missing_quality                                                       //081016
   : CORN::calculated_quality;
}
//_get_aridity_factor_quality_______________________________________2005-06-09_/
void Vapor_pressure_deficit_max_calculator::Parameters::copy(const Parameters &copy_from)
{  aridity_factor              = copy_from.aridity_factor;
   slope                       = copy_from.slope;
   intercept                   = copy_from.intercept;
   VPDmax_adj.copy(copy_from.VPDmax_adj);                                        //170808
   VPDavg_seasonal_fullday_observed.copy(copy_from.VPDavg_seasonal_fullday_observed); //170811_170808
   RHavg_seasonal_observed.copy(copy_from.RHavg_seasonal_observed);               //170811_170808
}
//_copy________________________________________________________________________/
bool Vapor_pressure_deficit_max_calculator::Parameters::setup_structure
(Data_record &data_rec,bool for_write)                             modification_
{  // caller should set the section (I.e. VPDmax_estimation)
   data_rec.expect_float32("aridity_factor"     ,aridity_factor);
   data_rec.expect_float32("VPDmax_slope"       ,slope);
   data_rec.expect_float32("VPDmax_intercept"   ,intercept);
   data_rec.expect_float64("VPDmax_adj"                        ,VPDmax_adj                      .mod_amount()); //170808
   data_rec.expect_float64("VPDavg_seasonal_fullday_observed"  ,VPDavg_seasonal_fullday_observed.mod_amount()); //170808
   data_rec.expect_float64("RHavg_seasonal_observed"           ,RHavg_seasonal_observed         .mod_amount()); //170808
   return true;
}
//_setup_structure__________________________________________________2017-08-08_/
bool Vapor_pressure_deficit_max_calculator::Parameters::get_end() modification_
{
   VPDavg_seasonal_fullday_observed.modify_quality_code
      (CORN::is_zero(VPDavg_seasonal_fullday_observed.in_prescribed_units())
       ? CORN::missing_quality
       : CORN::observed_quality);
   RHavg_seasonal_observed.modify_quality_code
      (CORN::is_zero<float64>(RHavg_seasonal_observed.in_prescribed_units())
       ? CORN::missing_quality
       : CORN::observed_quality);
   if (!CORN::is_zero<float64>(VPDmax_adj.in_prescribed_units()))
      VPDmax_adj.modify_quality_code(CORN::measured_quality);
   if (CORN::is_zero<float64>(VPDmax_adj.in_prescribed_units())
      ||!VPDmax_adj.is_valid())
   {  // The user has not provided VPDmax_adj
      if (RHavg_seasonal_observed.is_valid())
      {  // The user has provided the observed seasonal average relative humidity
         float64 VPDmax_adj_ = 0.0134 * RHavg_seasonal_observed + 0.1587;
         //VPDmax_adj.assume_quality(calculated_from(observed_quality));
         VPDmax_adj.force_value_qualified_code
            (VPDmax_adj_,CORN::quality_calculated_from(observed_quality));
      }
      else
          VPDmax_adj.force_value_qualified_code
            (1.0,CORN::quality_calculated_from(missing_quality));
   } else // The user supplied a value which is assumed to be valid(measured)
      VPDmax_adj.modify_quality_code(CORN::measured_quality);
   return true;
}
//_get_end__________________________________________________________2017-08-11_/

