#  include "soil/temperature_hourly.h"
#  include "soil/temperature_functions.h"
#  include "soil/soil_I.h"
#  include "soil/soil_evaporator.h"
#include <math.h>
#ifndef moremathH
#  include "corn/math/moremath.h"
   using namespace std;
#endif
#  include "CS_suite/simulation/CS_land_unit_meteorological.h"
#  include "common/weather/hour/weather_hours.h"
#  ifndef compareHPP
#     include "corn/math/compare.hpp"
#  endif
#include <math.h>
#ifndef moremathH
#  include "corn/math/moremath.h"
   using namespace std;
#endif

// The following includes are for dump only
#  include "soil/temperature_profile.h"
#  include "soil/hydrology_I.h"
#  include "soil/texture_I.h"
#  include "soil/structure_I.h"
#  include "corn/math/statistical/stats.h"
#  include "weather/parameter/WP_solar_radiation.h"
#  include "weather/sky.h"

#define clear_SL_array(target) {for (nat8 sublayer = 0; sublayer <= MAX_soil_sublayers; sublayer++) target[sublayer] = 0;};

namespace Soil {                                                                 //181206
//______________________________________________________________________________/
Temperature_profile_hourly::Temperature_profile_hourly
(const Soil::Soil_interface                        &soil_
,const CS::Land_unit_meteorological          &meteorological_                    //151022
,const CS::Solar_radiation_extraterrestrial &ET_solar_radiation_                 //150627
,const CS::Annual_temperature                &annual_temperature_                //150424
,const CORN::Date_const                      &today_)                            //140724
// The annual values come from // from scenario parameters
: Soil_temperature_profile_hourly_interface()
, soil                                 (soil_)
, soil_layers                          (*(soil.ref_layers()))
, meteorological                       (meteorological_)                         //151022
, today                                (today_)                                  //140724
, ET_solar_radiation                  (ET_solar_radiation_)                      //150627
, solar_radiation                      (meteorological_.ref_solar_radiation())   //151022
, has_snow_cover                       (false)                                   //160721
, annual_temperature                   (annual_temperature_)                     //150424
{
   // Warning, need to make sure that weather has been started otherwise
   // we need to call initialize when the soil and weather have both already been
}
//_Soil_temperature_profile_hourly:constructor_________________________________/
bool Temperature_profile_hourly::initialize()
{  // Need to initialize_nodes first because we need the node depths
   bool result = initialize_nodes();
   avg_temperature[0]=0;
   CORN::DOY doy = today.get_DOY();                                              //170220
   for (Hour hr = 0; hr < 24; hr++) temperature[hr][0] = 0;
   Layer number_layers = soil_layers.count();
   for (Layer layer = 1; layer <= MAX_soil_layers; layer++)
   {  float64 layer_temperature
      = (layer > number_layers)
      ? 0.0
      : estimate_soil_temperature(node_depth[layer],doy);
      for (Hour hr = 0; hr < 24; hr++)
         temperature[hr][layer] = layer_temperature;
      avg_temperature[layer] = layer_temperature;
   }
   return result;
}
//_initialize__________________________________________________________________/
bool Temperature_profile_hourly::initialize_nodes()
{  // if the soil profile changes (I.e. erosion)
   clear_SL_array(node_depth);
   // meters
   nat8 number_layers = soil_layers.count();
   node_depth[0] = -0.5 * soil_layers.get_thickness_m(1);
   float64 top_of_layer_depth = 0;
   for (nat8 layer = 1; layer <= number_layers; layer++)
   {  float64 thickness =  soil_layers.get_thickness_m(layer);
      node_depth[layer] = top_of_layer_depth + thickness /2.0;
      top_of_layer_depth = soil_layers.get_depth_m(layer);
   }
   node_depth[number_layers+1] = soil_layers.get_depth_m(number_layers) + soil_layers.get_thickness_m(number_layers)/2.0;
   return true;
}
//_______________________________________________________________reinitialize__/
float64 local_calc_saturation_vapor_pressure_kPa(float64 T)
{  // Note this is only a temporary hack, I already have this in Vapor_pressure::calc_saturation_vapor_pressure_kPa
   // but it isn't easily accessible.
   return 0.6108 * exp(17.27 * T / (T + 237.3));
}
//_local_calc_saturation_vapor_pressure_kPa____________________________________/
void Temperature_profile_hourly::update
(float64 crop_leaf_area_index
,float64 stubble_area_index
,float64 residue_area_index)
{
   const Soil::Evaporator *evaporator = soil.ref_evaporator();                   //200225

   // RLN     surface_light_absorptivity
   float64  surface_absorptivity = 1.0 - soil.get_albedo();
   const float64 long_wave_absorptivity = 0.95;
   const float64 surface_emissivity = 0.95;
   const float64 F = 0.6;  // constant for setting forward, backward or centered difference method
   const float64 G = 0.4;  // G = 1 - F
   nat8 number_layers = soil_layers.count();
   const CS::Weather_hours &hourly_weather = *                                   //130626
      meteorological.provide_hourly();                                           //151022

      // At this point we should always have hourly
   // We already have calculated cloud_cover_fract in weather,
   // but it is not exactly the same as the VB version because that uses the
   // daily ET solar rad calucated from the daily
   // where has here we calculated the daily from the sum of the hourly.

   float64 extraterrestrial_solar_irrad =
      ET_solar_radiation.get_MJ_per_m2_day();                                    //150627
   float64 solar_radiation_MJ_m2 = solar_radiation.get_MJ_m2();                  //151022_140715
   float64 cloud_cover_fract = meteorological.ref_sky().
   calc_cloud_cover_fraction(extraterrestrial_solar_irrad,solar_radiation_MJ_m2);
   soil_sublayer_array_64(heat_capacity_factor); clear_SL_array(heat_capacity_factor);
   // RLN: index 0 surface air heat capacity or just boundry?
   heat_capacity_factor[0] = 0; // heat capacity of boundary layer  = 0

   soil_sublayer_array_64(k); clear_SL_array(k);
   k[0] = 20.0; // boundary layer conductance (W/m2 K)

   statistical::Descriptive_progressive daily_temp_stats[MAX_soil_sublayers_alloc]; //200701

   const Soil::Hydrology_interface *hydrology = soil.ref_hydrology();             //160412
   const Soil::Structure           *structure = soil.ref_structure();             //160412
   const Soil::Texture_interface *texture= soil.ref_texture();
   //070117 k[] used to be calculated once (daily value) here,
   // but now water content is available hourly so this has been moved into the hourly loop.
   // (Need to check with current VB code).

   soil_layer_array64(T);  clear_SL_array(T);// current running temperature this hour
   soil_layer_array64(TN); clear_SL_array(TN);

    // Recalculate bottom boundary condition
    T[number_layers + 1] = estimate_soil_temperature
      (node_depth[number_layers + 1]
      , today.get_DOY());
   TN[number_layers + 1] = T[number_layers + 1];

   float64 net_short_wave_radiation_absorbed = 0;
   for (Hour hour = 0; hour < 24; hour++) // CropSyst hours are 0 - 23 (24 hour time)
   {
      for (nat8 layer = 1; layer <= number_layers; layer++)
      {
         daily_temp_stats[layer].reset();
         float64 clay_fraction = texture->get_clay_percent(layer)/100.0;
         {  // this should be inside the heat flux calculation loop if the water content is to change hourly
            // calculates heat capacity weighted by solid and water phase only
            float64 bulk_density = structure->get_bulk_density_g_cm3(layer);

            // Warning this could be make like the soil freezing

            float64 water_content = hydrology->get_water_plus_ice_content_volumetric_at(layer,hour);  // 070813 Warning I dont know if this is should be total or liquid (in VB it is total because there is no freezing)
            // This will automatically default to daily water content if infiltation model is not hourly

            heat_capacity_factor[layer] = calc_local_heat_capacity_J_m3_C(bulk_density, water_content) *soil_layers.get_thickness_m(layer);
            //041026  RLN: asked Claudio if we should consider ice content in heat capacity (the daily temperature does)

            // calculates conductance per layer, equation 4.20 for thermal conductivity
            float64 node_thickness = node_depth[layer+1] - node_depth[layer];
            k[layer] = calc_heat_conductivity   // 070827  This needs to explicitly account for ICE content which it isn't currently doing.
               (bulk_density
               ,water_content
               ,clay_fraction) / node_thickness;
               // If layer = number_layers Then k(layer) = 0 ' set K(last layer) = 0
         }
      } //
      float64 air_temperature_hour = hourly_weather.get_air_temperature(hour);
      float64 fract_solar_radiation_hour = hourly_weather.get_insolation_fraction(hour); //151223
      float64 hourly_solar_radiation_MJ_m2  = hourly_weather.get_solar_radiation_MJ_m2(hour);

//std::clog << "irrad:" << hourly_solar_radiation_MJ_m2 << std::endl;

//std::clog << "hour:" << (int)hour << std::endl;


      if (fract_solar_radiation_hour > 0.000000001)
      {
         float64 solar_zenith_angle_rad        = hourly_weather.get_solar_zenith_angle_rad(hour);
         float64 solar_direct_radiation_MJ_m2 = hourly_weather.get_solar_direct_radiation_MJ_m2(hour);
         float64 solar_diffuse_radiation_MJ_m2= hourly_weather.get_solar_diffuse_radiation_MJ_m2(hour);
         float64 canopy_intercepted_solar_rad_fract  = calc_solar_irradiance_interception_fract
         (0.5
         ,crop_leaf_area_index // crop->get_leaf_area_index()
         ,0.5  // indicates photosynthetic and near infrared portion  of the spectrum (geometic mean)
         ,solar_zenith_angle_rad
         ,solar_direct_radiation_MJ_m2    // any units of radiation (I.e J/m2/sec or MJ/m2/day)  comes from HourlyFractionOfSolarIrradiance * solar_radiation * fraction_direct_radiation   watch units
         ,solar_diffuse_radiation_MJ_m2   // any units of radiation (I.e J/m2/sec or MJ/m2/day)  comes from HourlyFractionOfSolarIrradiance * solar_radiation * fraction_diffuse_radiation
         );
         float64 stubble_intercepted_solar_rad_fract = calc_solar_irradiance_interception_fract
         (0.00000001    // RLN this was 0 but could have div0
         ,stubble_area_index // residue_pools->get_area_index(true)
         ,1.0 // 0.4   claudio changed 041202
         ,solar_zenith_angle_rad     // radians
         ,solar_direct_radiation_MJ_m2    // any units of radiation (I.e J/m2/sec or MJ/m2/day)  comes from HourlyFractionOfSolarIrradiance * solar_radiation * fraction_direct_radiation   watch units
         ,solar_diffuse_radiation_MJ_m2   // any units of radiation (I.e J/m2/sec or MJ/m2/day)  comes from HourlyFractionOfSolarIrradiance * solar_radiation * fraction_diffuse_radiation
         );
         float64 residue_intercepted_solar_rad_fract = calc_solar_irradiance_interception_fract
         (10.0
         ,residue_area_index // residue_pools->get_area_index(false)
         ,1.0   // 0.4    claudio changed 041202
         ,solar_zenith_angle_rad
         ,solar_direct_radiation_MJ_m2
         ,solar_diffuse_radiation_MJ_m2    // any units of radiation (I.e J/m2/sec or MJ/m2/day)  comes from HourlyFractionOfSolarIrradiance * solar_radiation * fraction_diffuse_radiation
         );
         float64 hourly_solar_irradiance_J_m2_s  = MJ_m2_hour_to_J_m2_sec(hourly_solar_radiation_MJ_m2);
         float64 cover_adj_hourly_solar_irradiance_J_m2_s =
             hourly_solar_irradiance_J_m2_s
            * (1.0 - canopy_intercepted_solar_rad_fract)
            * (1.0 - stubble_intercepted_solar_rad_fract)
            * (1.0 - residue_intercepted_solar_rad_fract);
         net_short_wave_radiation_absorbed = surface_absorptivity * cover_adj_hourly_solar_irradiance_J_m2_s;
      } else net_short_wave_radiation_absorbed = 0.0;
      if (CORN::is_zero<float64>(net_short_wave_radiation_absorbed/*200127 ,0.0,0.00001*/))
         net_short_wave_radiation_absorbed = 0.0;
      float64 relative_humidity_fraction_hour = hourly_weather.get_relative_humidity(hour)/ 100.0;
      float64 vapor_pressure_hour_kPa = relative_humidity_fraction_hour
         * local_calc_saturation_vapor_pressure_kPa(air_temperature_hour);
      const float64 lambda_J_kg = 2450000.0;
      // Latent heat of vaporization in J/kg

//160719 WARNING need to check that we have already calculated hourly evaporation!!!


      // RLN wondering if  Daily_Soil_Water_Evaporation needs to be adjusted for hour
      // (but may be already adjusted by HourlyFractionOfSolarIrradiance)
      // We may not need yesterday's value , the current value is probably
      // yesterday, but need to check  200225

      float64 evap_act_hour = 0;
         evap_act_hour =
            evaporator->get_evaporation_actual_hour(hour);                       //200225
            /*200225 The current is actually yesterday value
            soil.ref_evaporator()->get_evaporation_actual_hour_yesterday(hour); //160719
            */
      float64 hourly_latent_heat_J_m2_s =
               evap_act_hour                                                     //160719
             * hourly_solar_radiation_MJ_m2
             * lambda_J_kg / seconds_per_hour; // Convert from kg/m2/h to J/m2/s
      // Pass previous time step temperatures for all soil layers
      for (nat8 layer = 1; layer <= number_layers; layer++)
      {  T[layer]
         = (hour == 0) // CropSyst uses hours 0 to 23
         ? temperature[23][layer]
         : temperature[hour - 1][layer];
      }
      TN[0] = air_temperature_hour;
      T[0] = air_temperature_hour;
      float64 T0_kelvin = C_to_kelvin(air_temperature_hour);
      // This can be moved out of J loop
      float64 clear_sky_emissivity = meteorological.ref_sky().calc_clear_sky_emissivity_2(T0_kelvin,vapor_pressure_hour_kPa);
      float64 clouded_sky_emissivity = meteorological.ref_sky().calc_cloud_emissivity(T[0],cloud_cover_fract,clear_sky_emissivity);
      float64 sky_long_wave_radiation_absorbed = long_wave_absorptivity * clouded_sky_emissivity * 0.0000000567 * pow(T0_kelvin, 4.0);

      soil_layer_array64(A);    clear_SL_array(A);
      soil_layer_array64(B);    clear_SL_array(B);
      soil_layer_array64(d);    clear_SL_array(d);
      soil_sublayer_array_64(C);
      float64 Rnet = 0;
      float64 T1_kelvin = 0;
      float64 exit_test_value = 9999.0;
      float64 surface_long_wave_radiation_outgoing = 0.0;


      for (int j = 1
          ;    (j < 100) // maximum number of iterations
            && (exit_test_value >= 0.01)
          ; j++)
      {  // this loop updates temperatures after the first Thomas loop and is needed
         // due to the inclusion of Tsurface [T[1]] to calculate net radiation at "exchange" surface
         if (j > 1)
            T[1] = TN[1];
         clear_SL_array(C);
         for (nat8 layer = 1; layer <=  number_layers; layer++)
         {  // calculates elements of the matrix
            float64 k_previous_node =  k[layer - 1];
            C[layer] = -k[layer] * F;
            A[layer + 1] = C[layer];
            B[layer] = F * (k[layer] + k_previous_node) + heat_capacity_factor[layer] / seconds_per_hour;
            d[layer] = G * k_previous_node * T[layer - 1]
                       + (heat_capacity_factor[layer] / seconds_per_hour
                           - G * (k[layer] + k_previous_node)) * T[layer]
                       + G * k[layer] * T[layer + 1];
         }
         // as an approximation calculate net radiation here with given surface temperature
         // recalculate in new loop and repeat the procedure until Tsurface [T(1)] does not oscillate

         // COS  Rnet = absorptivity * ST + emissivity_surface * 0.0000000567 * (sky_emissivity(0.7, T(0) + 273.15, 0) * (T(0) + 273.15) ^ 4 - (T(1) + 273.15) ^ 4)
          T1_kelvin = C_to_kelvin(T[1]);
          surface_long_wave_radiation_outgoing // units  MJ/m2?
            = surface_emissivity * 0.0000000567 * pow(T1_kelvin ,4.0);
         // RLN Note this  can not be moved out of J loop or put in hourly solar rad class because
         // we need the surface_long_wave_radiation_outgoing updated for J.
         Rnet = net_short_wave_radiation_absorbed + sky_long_wave_radiation_absorbed - surface_long_wave_radiation_outgoing;
         d[1] += k[0] * TN[0] * F + Rnet - hourly_latent_heat_J_m2_s; // COS: add net radiation and latent heat transfer here
         d[number_layers] += k[number_layers] * F * TN[number_layers + 1]; // the last term is redundant if Tn(number_layers+1) = T[number_layers+1] because K(number_layers) = 0
         { // Thomas algorithm:
           for (nat8 layer = 1; layer < number_layers; layer++)    // loop to sublayers - 1
           {   C[layer] /= B[layer];
               d[layer] /= B[layer];
               B[layer + 1] -= A[layer + 1] * C[layer];
               d[layer + 1] -= A[layer + 1] * d[layer];
           }
           TN[number_layers] = d[number_layers] / B[number_layers];
           for (int layer = number_layers - 1 ; layer >= 1; layer--)
               TN[layer] = d[layer] - C[layer] * TN[layer + 1];
         }
         exit_test_value = fabs(T[1] - TN[1]) / fabs(T[1] + 0.01 + 0.00000001);
      } // end for j
      // updates T[layer] before the next loop
      for (int layer = 0; layer <= number_layers + 1; layer++)
      {  // COS SoilTemperatureState.HeatFlux(DOY, Hour, layer) = k[layer] * (G * (T[layer] - T[layer + 1]) + F * (TN[layer] - TN(layer + 1)))
         T[layer] = TN[layer];
         temperature[hour][layer] = TN[layer];
         float32 TN_sl = (float32)TN[layer];
         daily_temp_stats[layer].append(TN_sl);

//if ((today.get() >= 2013255) && (today.get() <= 2013256) )
//debug_soil_temp_hourly << "D:\t" << today.as_string() <<  "\tH:\t" << (int)hour << "\tL:\t" << (int)layer << "\tT:\t" << temperature[hour][layer] << std::endl;

      }
   } // Next hour
   for (int layer = 0; layer <= number_layers + 1; layer++)
      avg_temperature[layer] = daily_temp_stats[layer].get_mean();
}
//_update______________________________________________________________________/
// This function will work for any units of irradiance
float64 Temperature_profile_hourly::calc_solar_irradiance_interception_fract // was FractionSunlitShadedRadiationInterception
(float64 X
// for crop canopy      use 0.5
// for standing stubble use 0.0
// for residue          use 10.0
,float64 area_index                 // (I.e. LAI)
,float64 absorbtivity               // 0-1
// for crops use 0.5 which indicates photosynthetic and near infrared portion  of the spectrum (geometic mean)
// for residue use 0.4
,float64 solar_zenith_angle_rad     // radians
,float64 solar_direct_irradiance    // any units of irradiance (I.e J/m2/sec or MJ/m2/day)  comes from HourlyFractionOfSolarIrradiance * solar_irradiance * fraction_direct_irradiance   watch units
,float64 solar_diffuse_irradiance   // any units of irradiance (I.e J/m2/sec or MJ/m2/day)  comes from HourlyFractionOfSolarIrradiance * solar_irradiance * fraction_diffuse_irradiance
)
{
   //unused K_Beam_Solar_Black_Leaves = BeamExtinctionCoeff(X, Solar_Zenith_Angle, 1)
   //(as if the leave absorbed 100% of intercepted irradiance  )
   float64 direct_solar_irradiance_extinction_coef
      = calc_direct_extinction_coef(X, solar_zenith_angle_rad, absorbtivity);
   //unused Beam_Black_Leaves_Transmissivity = Exp(-K_Beam_Solar_Black_Leaves * area_index)
   float64 direct_solar_irradiance_transmissivity_fract
   = exp(-direct_solar_irradiance_extinction_coef * area_index);
   // hitting the soil going between the leaves
   // (includes a portion of irradiance going through the leaf that is not absorbed).

   // (light extinction going through material)
   float64 diffuse_solar_irradiance_extinction_coef
      = calc_diffuse_extinction_coef(X, area_index, absorbtivity);
   float64 diffuse_solar_irradiance_transmissivity_fract
      = exp(-diffuse_solar_irradiance_extinction_coef * area_index);
   float64 direct_solar_irradiance_interception //  same units as solar_diffuse_irradiance (I.e J/m2/sec or MJ/m2/day)
      = (1.0 - direct_solar_irradiance_transmissivity_fract) * solar_direct_irradiance;
   float64 diffuse_solar_irradiance_interception //  same units as solar_diffuse_irradiance (I.e J/m2/sec or MJ/m2/day)
      = (1.0 - diffuse_solar_irradiance_transmissivity_fract) * solar_diffuse_irradiance;
   float64 solar_irradiance_interception_fract
      = (direct_solar_irradiance_interception + diffuse_solar_irradiance_interception )
      / (solar_direct_irradiance               + solar_diffuse_irradiance);
   return solar_irradiance_interception_fract;
}
//_calc_solar_irradiance_interception_fract____________________________________/
float64 Temperature_profile_hourly::calc_direct_extinction_coef  // direct beam
(float64 X
,float64 solar_zenith_angle_rad
,float64 absorptivity)                                                     const
{
   //This function calculates extinction coefficient for beam radiation of
   //a canopy with Leaves angles distribution approximated using an ellipsoidal model.
   //The extinction coefficient is for specified absorptivity value

   // eventually this function could be moved some where else
   // (It is common to both crop and residue)

   //  In C++ version now passing radians
   //#  define  dtr 0.0174533 'Convert degrees to radians
   // float64 z = dtr * solar_zenith_angle_deg;

   float64 numerator = sqrt(CORN_sqr(X) + CORN_sqr(tan(solar_zenith_angle_rad)));
   float64 denominator = X + 1.774 * pow((X + 1.182),-0.733);
   float64 direct_extinction_coef = (numerator / denominator) * sqrt(absorptivity);
   return  direct_extinction_coef;
}
//_calc_direct_extinction_coef_________________________________________________/
float64 Temperature_profile_hourly::calc_diffuse_extinction_coef
(float64 X
,float64 area_index // I.e. LAI
,float64 absorptivity)                                                     const
{
  //This function calculates extinction coefficient for diffuse radiation of
  //a canopy with Leaves angles distribution approximated using an ellipsoidal model.
  //The extinction coefficient is for specified Leaves area index and absorptivity values

   // eventually this function could be moved some where else
   // (It is common to both crop and residue)

   float64 tau_diffuse = 0.0;
   // RLN: warning, presuming zenith_solar_angle  is radians
   for (float64 zenith_solar_angle = 0.01; zenith_solar_angle <= 1.57;  zenith_solar_angle += 0.01)
   {  // Integrate beam transmissivity for all zenith solar angles (radians)
      float64 tau_direct = exp( - calc_direct_extinction_coef(X, zenith_solar_angle, absorptivity) * area_index);
      tau_diffuse += tau_direct * sin(zenith_solar_angle) * cos(zenith_solar_angle) * 0.01;
   }
  tau_diffuse *= 2.0; // RLN Not sure why multiplying by 2.0
  float64 diffuse_extinction_coef =
   (area_index == 0.0) ? 0.0 : // <- avoid div0
   -log(tau_diffuse) / area_index;
  return diffuse_extinction_coef;
}
//_calc_diffuse_extinction_coef________________________________________________/
float64 Temperature_profile_hourly::calc_heat_conductivity
(float64 bulk_density_g_cm3
,float64 volumetric_WC
,float64 fraction_clay) const
{  // coefs to calculate thermal conductivity (page 32+)
   float64 C1 = 0.65 - 0.78 * bulk_density_g_cm3 + 0.6 * CORN_sqr(bulk_density_g_cm3); // equation 4.27; coeff of 4.20
   float64 C2 = 1.06 * bulk_density_g_cm3;                                             // equation 4.25; coeff of 4.20
   float64 C3 = 1 + 2.6 / sqrt(fraction_clay);                                         // equation 4.28; coeff of 4.20
   float64 C4 = 0.03 + 0.1 * CORN_sqr(bulk_density_g_cm3);                             // equation 4.22; coeff of 4.20
   float64 heat_conductivity = C1 + C2 * volumetric_WC - (C1 - C4) * exp(-  pow((C3 * volumetric_WC), 4.0));
   return heat_conductivity;
}
//_calc_heat_conductivity______________________________________________________/
float64 Temperature_profile_hourly::estimate_soil_temperature
(float64 node_depth_layer
,DOY     doy)                                                              const
{  const float64 DD = 2.0; // damping depth                                      //060501
      // could be done more mechanisticaly?
   float64 est_soil_temperature
   =  annual_temperature.avg
      + annual_temperature.avg_half_amplitude
         * exp(-node_depth_layer / DD)
         * sin(2.0 * 3.1416 / 365.0 * (doy - annual_temperature.phase)
             - node_depth_layer / DD);
   return est_soil_temperature;
}
//_estimate_soil_temperature___________________________________________________/
#ifdef DEBUG_DUMP
//______________________________________________________________________________/
void Temperature_profile_hourly::dump_temporary
(ostream &strm,Soil_temperature_profile &soil_temp_profile)
{
   int32 date_num = today.get_date32();
   nat8 number_layers = soil_layers.count();
   strm << date_num << ',' << "old_daily_temperature";
   strm << ',' << weather.get_curr_avg_air_temperature();
   for (int sl = 1; sl <= number_layers ; sl++)
      strm <<  ',' << soil_temp_profile.get_temperature(sl);
   strm << endl;
   strm << date_num << ',' << "avg_daily_temperature"  ;
   strm << ',' << weather.get_curr_avg_air_temperature();
   for (int sl = 1; sl <= number_layers ; sl++)
      strm <<  ',' << avg_temperature[sl];
   strm << endl;
   for (int hr = 0; hr<24; hr++)
   {  strm << date_num << ',' << hr ;
      strm << ',' << weather.hourly().get_air_temperature(hr);
      for (int sl = 1; sl <= number_layers ; sl++)
         strm <<  ',' <<  temperature[hr][sl];
      strm << endl;
   }
}
//_dump_temporary______________________________________________________________/
#endif
}//_namespace_Soil_____________________________________________________________/

