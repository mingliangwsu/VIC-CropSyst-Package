#include <math.h>
#include "corn/quality.hpp"
#include "corn/math/moremath.h"
#include "weather/loc_param.h"
#include "weather/database/weather_database.h"
#include "weather/weather_interface.h"
#include "biometeorology/ET_reference_FAO.h"
#include "common/solar/irradiance_extraterrestrial.h"
#include "weather/weather_provider.h"
//______________________________________________________________________________
ET_Reference_FAO::ET_Reference_FAO
(
 const Weather_provider &provider                                                //150804
      // eventually rename this to weather_provider_
,const Geocoordinate &geocoordinate_                                             //151118
,const float64 &_day_length_hours_today                                          //150627
//NYN, Aerodynamics &_aerodynamics                                                    //150601
#ifdef LOC_ONLY_FOR_GEN
, float64 _ET_slope                                                              //070314
, float64 _ET_intercept                                                          //070314
#endif
, ET_Model preferred_ET_model)
:ET_Reference_common
   (geocoordinate_.get_elevation()                                               //151128_150804
   ,_day_length_hours_today                                                      //150627
   ,provider.aerodynamics                                                        //150617
   ,provider.ET_solar_radiation                                                  //150625
   ,provider.air_temperature_max
   ,provider.air_temperature_min
   ,provider.atmospheric_pressure
   ,provider.vapor_pressure_actual                                               //100421
   ,provider.solar_radiation
   ,provider.vapor_pressure_saturation_slope                                     //151126
   ,provider.latent_heat_vaporization                                            //151126
   ,provider.psychrometric_constant                                              //151126
   )
, weather_provider(provider)                                                     //150804
,enabled_model(preferred_ET_model)
#ifdef LOC_ONLY_FOR_GEN
,ET_slope(_ET_slope)                                                             //070314
,ET_intercept(_ET_intercept)                                                     //070314
#endif
{  for (int i = 0; i < ET_Model_COUNT; i++)
      ET_ref[i].invalidate(true);                                                //180404
}
//_ET_Reference_FAO:constructor________________________________________________/
const CORN::Quality &ET_Reference_FAO::update()                   rectification_ //180822
{  for (int i = 0; i < ET_Model_COUNT ; i++)                                     //011018
      ET_ref[i].invalidate(true);                                                //180404
   CORN::Quality_clad PM_elements_quality
      (air_temperature_max.worst
      (air_temperature_min.worst
      (solar_radiation.worst
      //(weather_provider.wind_speed.worst
      //200328(weather_provider.relative_humidity_max.worst
      (weather_provider.vapor_pressure_deficit_fullday_avg.worst                 //200328
      (weather_provider.wind_speed)))));
   CORN::Quality_clad PM_quality
      (PM_elements_quality.is_valid()                                            //150805_040511
         ? CORN::quality_estimated_from(PM_elements_quality.get_quality_code())  //040511
         : CORN::not_valid_quality);
      // If we have not selected all models or not penman monteith model         //011018
   if (PM_quality.is_valid()
       && (
      //200228          (enabled_model == ET_MODEL_AUTOMATIC) ||
         #if (CS_VERSION==4)
         (enabled_model ==PENMAN_MONTEITH_MODEL )
         #else
         (enabled_model ==ET_FAO_56_Penman_Monteith_CropSyst)
         #endif
         ))
   {
   curr_avg_air_temperature = weather_provider.air_temperature_avg.Celcius();    //131105

   curr_soil_heat_flux_density   = 0.0; // 0.0 when full day  (if daylight, a value of 1.0) would be used.
   curr_net_radiation            = calc_net_radiation(false);// Rn

   float64 curr_avg_fullday_vapor_pressure_deficit_kPa
      = weather_provider.vapor_pressure_deficit_fullday_avg.get_kPa();           //150805
   float64 wind_speed_m_d =weather_provider.wind_speed.get_m_d();                //131105
   float64 wind_speed_at_standard_height_m_d
      = aerodynamics.calc_normalized_wind_speed(wind_speed_m_d);                 //150805
   float64 curr_latent_heat_vaporization = latent_heat_vaporization.get_MJ_per_kg(); //140512

   //   Although aero resistance is only used by P.M. it is also used by limited growth calculations 011117
   #define reference_crop_plant_height_meters  0.12
   curr_aerodynamic_resistance_d_m =         // ra
      aerodynamics.calc_resistance_of_plants_d_m
      (wind_speed_at_standard_height_m_d, reference_crop_plant_height_meters);   //011008
   slope_saturated_vapor_pressure_plus_psychrometric_resistance_reference.update(); //150601
      // Actual vapor pressure computed by common_et
      // (it is used also for isothermal longwave net radiation).
      // Warning psychrometric_constant is not the same as FAO (data not available) using CropSyst
      float64 slope_saturated_vapor_pressure_plus_psychrometric_resistance_ref_kPa_per_C = 0.0;
      slope_saturated_vapor_pressure_plus_psychrometric_resistance_ref_kPa_per_C
         = slope_saturated_vapor_pressure_plus_psychrometric_resistance_reference.get_kPa_per_C(); //150601
      float64 atmospheric_density = calc_atmospheric_density(C_to_kelvin(curr_avg_air_temperature)); // kg/m3    //011008
      static const float64 spec_heat_cap_moist_air = 0.001013; // MJ/(kg C)                                    //011008
      float64 vol_heat_cap = atmospheric_density * spec_heat_cap_moist_air;                                   //011008
      float64 vapor_pressure_component = (curr_avg_fullday_vapor_pressure_deficit_kPa * vol_heat_cap) / curr_aerodynamic_resistance_d_m; //011008
#ifdef ANOTHER_WAY
011008 This is another way of computing the P.M. terms
These should be functionally equivelent to the ones currently use,
but are factored a different way.

      float64 radiation_component =
         vapor_pressure_saturation_slope.get_kPa_C()                             //140507
         * (curr_net_radiation - curr_soil_heat_flux_density);                   //011008

      float64 wind_speed_at_standard_height_m_s
      = weather.normalize_wind_speed(wind_speed_m_d/seconds_per_day);
      float64 curr_psychrometric_const = psychrometric_constant.get_kPa_per_C(); //140507
      float64 denominator
      =  vapor_pressure_saturation_slope.get_kPa_C()                             //140507
         + curr_psychrometric_const * (1.0 + 0.34 * wind_speed_at_standard_height_m_s);
      float64  radiation_component
         = inverse_latent_heat_flux_vaporization/*131105 0.408*/
         * slope_vapor_pressure_curve
         * (curr_net_radiation - curr_soil_heat_flux_density);
      float64 mean_saturation_vapor_pressure // kPa  (es)
      = (weather.get_sat_VP_at_max_temp() + weather.get_sat_VP_at_min_temp()) / 2.0;
      saturation_vapor_pressure_deficit // kPa
      = mean_saturation_vapor_pressure -
#ifdef REF_WEATHER
//100420 now references to parameters are provided directly rather than having and entire weather object
                     abstract_weather.get_curr_act_vapor_pressure();
                              weather.get_curr_act_vapor_pressure();
#else
                     curr_vapor_pressure_actual.value;
#endif
      float64  vapor_pressure_component
      = psychrometric_const
        * (900.0 /C_to_Kelvin(avg_air_temperature))
        * wind_speed_at_standard_height_m_s
        * curr_saturation_vapor_pressure_deficit;
#endif
      float64 Rad_term_num = vapor_pressure_saturation_slope.get_kPa_per_C()     //140507
         * curr_net_radiation / slope_saturated_vapor_pressure_plus_psychrometric_resistance_ref_kPa_per_C; //011009
      float64 Rad_term = Rad_term_num / curr_latent_heat_vaporization;           //011009
      float64 Aero_term_num =  vapor_pressure_component
         / slope_saturated_vapor_pressure_plus_psychrometric_resistance_ref_kPa_per_C; //011009
      float64 Aero_term = Aero_term_num / curr_latent_heat_vaporization;         //011009
      float64 ET_ref_penman_monteith = Rad_term + Aero_term;                     //011009

      ET_ref[ET_FAO_56_Penman_Monteith_CropSyst]                                 //180404
         .set_mm(ET_ref_penman_monteith,PM_quality);
   }
   else                                                                          //011018
   {
      PM_quality.assume_code(CORN::not_valid_quality);                           //150805_011018
      ET_ref[ET_FAO_56_Penman_Monteith_CropSyst].assume_code(CORN::not_valid_quality); //180404
   }
   float64 ET_hargreaves_unadjusted = 0.0;
   CORN::Quality_code ET_hargreaves_unadjusted_quality = CORN::not_valid_quality;//150123_011018
   if ((
       //200228  (enabled_model == ET_all_models) ||
        (enabled_model == ET_Hargreaves_unadjusted))                             //011018
         && PM_quality.is_valid())                                               //150805_020401
   {
      static const float64 linear_reg_coef_uncalibrated = 0.0023;
      float64 air_temp_max_C =  air_temperature_max.Celcius();
      float64 air_temp_min_C =  air_temperature_min.Celcius();
      float64 extra_terrestrial_radiation_MJ_m2   // This should be daily derived from hourly when available
          = solar_radiation_extraterrestrial.get_MJ_per_m2_day();                //150625
      float64 radiation_evaporation_equivalent                                   //131105
         = inverse_latent_heat_flux_vaporization
          *extra_terrestrial_radiation_MJ_m2;
      ET_hargreaves_unadjusted
         = linear_reg_coef_uncalibrated * (curr_avg_air_temperature + 17.8)
         * sqrt(air_temp_max_C - air_temp_min_C)
         * radiation_evaporation_equivalent;                                     //011009
      ET_hargreaves_unadjusted_quality
      = CORN::quality_estimated_from
         (air_temperature_max.worst(air_temperature_min).get_quality_code());    //150805
   }
   ET_ref[ET_Hargreaves_unadjusted].set_mm
      (ET_hargreaves_unadjusted,ET_hargreaves_unadjusted_quality);               //180404
   float64 ET_hargreaves_adjusted = 0.0;
   CORN::Quality_code ET_hargreaves_adjusted_quality = CORN::not_valid_quality;  //150123_011018
   if ((enabled_model==ET_Hargreaves_adjusted)
      //200228          ||(enabled_model==ET_all_models)
         )  //011018
   {
#ifdef LOC_ONLY_FOR_GEN
      ET_hargreaves_adjusted =ET_hargreaves_unadjusted * ET_slope + ET_intercept;//070314
      ET_hargreaves_adjusted_quality = ET_hargreaves_unadjusted_quality;         //070314
#else
     const Location_parameters *location =
           weather_provider.location;                                            //150805
      if (location)                                                              //050912
      {  ET_hargreaves_adjusted = ET_hargreaves_unadjusted * location->ET_slope  //050525
            + location->ET_intercept;
         ET_hargreaves_adjusted_quality = ET_hargreaves_unadjusted_quality;      //150123
      } // else location information not available to comput adjustment          //050912
#endif
      ET_ref[ET_Hargreaves_adjusted].set_mm
         (ET_hargreaves_adjusted,ET_hargreaves_adjusted_quality);                //180404
   }
   set_m(ET_ref[enabled_model].get_m(),ET_ref[enabled_model]);                   //180404
   return ET_ref[enabled_model];                                                 //180822
}
//_update______________________________________________________________update__/

