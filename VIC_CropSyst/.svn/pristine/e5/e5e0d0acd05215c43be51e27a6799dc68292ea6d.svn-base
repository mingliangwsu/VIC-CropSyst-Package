#include <math.h>
#include <iostream>
#include <assert.h>

#include "VIC_crop_weather.h"
#include "VIC_crop_variable_codes.h"
#include "WSDA_croptype.h"
#include "corn/math/moremath.h"
#include "corn/math/compare.hpp"
#include "UED/library/std_codes.h"
#include "weather/parameter/WP_air_temperature.h"
#include "weather/parameter/WP_wind_speed.h"
#include "corn/data_source/vv_file.h"
#include "corn/format/binary/binary_file_fstream.h"
#include "corn/string/strconv.h"
#include "corn/OS/file_system_engine.h"







#ifdef UNIT_TEST
std::ofstream debug_out_crop("unit_test_crop.dat");
#endif

//120803  These functions are implemented in VIC_crop_C_interface.c
#ifdef __cplusplus
extern "C" {
#endif
#include "VIC_soil_hydrology.h"
#ifdef __cplusplus
}
#endif
#ifdef VIC_CROPSYST_VERSION

namespace VIC_crop
{


   //___________________________________________________________________________
   // when pasture/grasses grown as seed don't clip (There will be different crop codes for seed crops).
   nat16 clipped_crops[] =
   {
      701 // Alfalfa, Hay
      , 1208 // Pasture_Grass
      , 713 // Grass, Hay Pasture
      , 722 // Timothy Pasture
      , 1206 // NLCDPasture_Hay
      , 703 // Alfalfa/Grass, Hay
      , 1207 // Pasture	OtherHays	OtherHays
      , 1205 // Pasture
      , 1517 // Ryegrass, Seed   WARNING Currently clipped but probably should not be clipped 110426
      , 708 // Clover, Hay
      // No yield calculated ,508  //CloverWildFlower
      , 1101 // Mint   // only has two clippings
      , 901 // Hop
      , 0
   };

//_____________________________________________________________________________/
Crop_with_weather::Crop_with_weather
   (const char*database_directory_name
   ,float64 latitude_dec_deg, float64 longitude_dec_deg
   ,float64 elevation_meter
   ,bool _automatic_harvest
   ,bool _enable_CO2_response
   ,bool _assume_irrigation
   )
: geolocation()
   #ifdef CROPSYST_PROVIDED_ET
   , location(geolocation, 0, 0, 0) // version numbers
   ,ET_solar_irradiance(UC_MJ_m2, geolocation)
   ,weather(today, ET_solar_irradiance, &location)
   #else
   ,weather()
   #endif
   ,aerodynamic_resistance_ref_plants(0)                                         //141206
      // aerodynamic_resistance_ref_plants is setup by know_pychrometrics
   ,snow_pack_thickness(0)                                                       //141206
   ,database_directory_name() // default to CWD                                  //110202
   ,crop_parameters_filename(0)                                                  //110202
   ,soil()                                                                       //140812
   , crop(0)                                                                     //110404
   , database_valid(true)                                                        //110617
   , automatic_harvest(_automatic_harvest)
   , crop_started(false)                                                         //110207
   , enable_CO2_response(_enable_CO2_response), found_inducible_period(false)    //110207
   , irrigation_threshold_LWP(0), assume_irrigation(_assume_irrigation)
   #ifdef UNIT_TEST
   , headers_output(false)
   #endif
   {
      geolocation.set_latitude_dec_deg_32((float32)latitude_dec_deg);
      geolocation.set_longitude_dec_deg_32((float32)longitude_dec_deg);
      geolocation.set_elevation_meter((float32)elevation_meter);
      database_valid = set_database_directory(database_directory_name);
      #ifdef CROPSYST_PROVIDED_ET
      location.VPD_parameters.aridity_factor = 0.02;
      location.VPD_parameters.slope = 1.0001;
      location.VPD_parameters.intercept = 0.0001;
      location.solar_radiation_parameters.fitted_B = 0.19;
      location.solar_radiation_parameters.Tnc = 14.0;
      #endif
   }

//_____________________________________________________Crop_with_soil_weather__/
   Crop_with_weather::~Crop_with_weather()
   {  delete crop;
      delete crop_parameters_filename;                                           //140509
   }
   //_2011-06-15____________________________Crop_with_soil_weather:destructor__/
//140810 now perm #ifndef NONCONTINUOUS

   Crop_interface * Crop_with_weather::start_crop(nat16 WSDA_crop_type_code)
      modification_
   {  delete crop;
      crop = 0;                                                                  // 110614
      char crop_code_cstr[20];
      #ifdef _Windows
      itoa(WSDA_crop_type_code, crop_code_cstr, 10);
      #else
      CORN::nat32_to_cstr(WSDA_crop_type_code, crop_code_cstr, 10);
      #endif
      bool found_crop = find_crop_file(crop_code_cstr,L"CS_crop");                //140508
      if (found_crop)
      {  VV_File crop_file(crop_parameters_filename->c_str());
         crop_file.get(crop_parameters);
         crop_parameters.setup_season_correction                                 //130708
            (geolocation.get_latitude_dec_deg_32(),
            geolocation.get_longitude_dec_deg_32());
         adjust_crop_phenology(geolocation.get_latitude_dec_deg(),
            geolocation.get_longitude_dec_deg());
         adjust_for_county(WSDA_crop_type_code, crop_parameters);                //110702
      }
      bool automatic_clipping = false;                                           //110404
      for (int cci = 0; clipped_crops[cci] && !automatic_clipping; cci++)        //110404
         if (clipped_crops[cci] == WSDA_crop_type_code)                          //110404
            automatic_clipping = true;                                           //110404
      #if (VIC_CROPSYST_VERSION < 3)
      Crop *old_VIC_crop;
      crop = old_VIC_crop = new Crop(crop_parameters                             //140810_110404
         , today, weather, automatic_harvest, automatic_clipping                 //110426
         , (WSDA_crop_type_code == 1101), enable_CO2_response);                  //101208
      #else

      crop = new CropSyst_proper_crop
         (crop_parameters
         ,today
         ,automatic_clipping
         ,(WSDA_crop_type_code == 1101) // mint_clipping
         ,aerodynamic_resistance_ref_plants
         ,snow_pack_thickness
         ,weather //weather_provider
         ,&soil
        // ,soil_chem
         );
      #endif
      irrigation_threshold_LWP // at 70% LWP that reduces canopy expansion
         =  crop_parameters.ref_morphology().reduce_canopy_expansion_LWP * 0.70;
      #if (VIC_CROPSYST_VERSION < 3)
      old_VIC_crop->know(                                                        //140810
         #ifdef CROPSYST_PROVIDED_SOIL_HYDROLOGY
         soil_properties->get_layers()
         #else
         soil.get_layers()
         #endif
         ,soil.get_hydrology_modifiable());
      #else
      // Crop_CropSyst take soil in constructor
      #endif
      crop_parameters.setup_season_correction
         (geolocation.get_latitude_dec_deg_32(),
         geolocation.get_longitude_dec_deg_32());

      if (crop_parameters.is_perennial())                                        //120618_110406
         crop_started = true; // Assuming perennials as established crops        //110406
      #ifdef UNIT_TEST
      crop->daily_growth_header(debug_out_crop, 0);
      debug_out_crop << std::endl;
      crop->daily_growth_header(debug_out_crop, 1);
      debug_out_crop << std::endl;
      crop->daily_growth_header(debug_out_crop, 2);
      debug_out_crop << std::endl;
      #endif
      return crop;
   }

//_2011-06-14______________________________________________________start_crop__/
   bool Crop_with_weather::stop_crop()modification_
   {  delete crop;
      crop = 0;
      crop_started = false;                                                      //110829
      return true;
   }
//_2011-06-15_______________________________________________________stop_crop__/
   void Crop_with_weather::know_air_temperature
      (float64 air_temperature_maximum, float64 air_temperature_minimum)
      modification_
   {  weather.air_temperature_max.set_Celcius
         (air_temperature_maximum,measured_quality);
      weather.air_temperature_min.set_Celcius
         (air_temperature_minimum,measured_quality);
   }
//_2010-11-22____________________________________________know_air_temperature__/

   #ifndef CROPSYST_PROVIDED_ET
   void Crop_with_weather::know_weather
         (float64 air_temperature_max
         ,float64 air_temperature_min
         ,float64 solar_radiation
         ,float64 vapor_pressure_deficit_max)                      modification_
   {
      weather.air_temperature_max.set_Celcius
         (air_temperature_max,measured_quality);
      weather.air_temperature_min.set_Celcius
         (air_temperature_min,measured_quality);
      weather.solar_radiation.set_MJ_m2(solar_radiation,measured_quality);
      weather.vapor_pressure_deficit_max.set_kPa(vapor_pressure_deficit_max/*,measured_quality*/);
   }
//_2014-08-11____________________________________________________know_weather__/
   void Crop_with_weather::know_pychrometrics
      (float64 vapor_pressure_saturation_slope_kPa_C
      ,float64 psychrometric_constant_kPa_C
      ,float64 _aerodynamic_resistance_ref_plants)                 modification_
    {
       weather.vapor_pressure_saturation_slope.set_kPa_C
         (vapor_pressure_saturation_slope_kPa_C,measured_quality);
       weather.psychrometric_constant.set_kPa_C
         (psychrometric_constant_kPa_C,measured_quality);
       aerodynamic_resistance_ref_plants = _aerodynamic_resistance_ref_plants;
    }
//_2014-08-11______________________________________________know_pychrometrics__/
   #endif

   void Crop_with_weather::know_solar_radiation
      (double daily_irradiance_MJ_m2)modification_
   {  weather.solar_radiation.set_MJ_m2(daily_irradiance_MJ_m2,
         CORN::calculated_quality);
      // In VIC, solar radiation is estimated from temperature
      // We should indicate estimated quality but we want the model to
      // assume it is close to measured quality.
   }
   //_2001-04-13_________________________________________know_solar_radiation__/
   // _2001-04-13__________________________________________________________________/
   // In think crop always needs to know windspeed for calc_weather_adjusted_crop_ET_coef
   void Crop_with_weather::know_wind_speed_m_s(float64 windspeed_m_s)
   {
      #ifdef CROPSYST_PROVIDED_ET
      weather.wind_speed.set_m_s(windspeed_m_s, measured_quality);
      #endif
   }
   //_2010-11-22_____________________________________________________
   void Crop_with_weather::know_atmospheric_CO2_concentration
      (float64 new_atmospheric_CO2_concentration_ppm)modification_
   {  weather.atmospheric_CO2_conc_ppm = new_atmospheric_CO2_concentration_ppm;
      if (crop) crop->reinitialize_CO2_response();
   }
   //______________________________________know_atmospheric_CO2_concentration__/
   void Crop_with_weather::know_snow_pack_thickness(float64 snow_pack_thickness_m)   modification_
   {  snow_pack_thickness = snow_pack_thickness_m;
   }
   //_2014-12-06________________________________________________________________

   float64 Crop_with_weather::harvest(bool terminate_crop)modification_
   {
#ifdef TEST_C_CPP
      return 0.0;
#else
      return crop->harvest_annual(terminate_crop);
#endif
   }
   //____________________________________________________________________harvest__/
   bool Crop_with_weather::start_day(int32 date)modification_
   {  bool started = true;
      today.set_date32(date); // 101210
      #ifdef CROPSYST_PROVIDED_ET
      ET_solar_irradiance.set_date_noon(today);
      ET_solar_irradiance.update();
      #endif
      weather.update();
      // don't clear soil_pot_evaporation because it is calculated from the day before   //110527
      return started |= crop ? crop->start_day() : started;
   }
   //_2010-11-22_______________________________________________________start_day__/
   bool Crop_with_weather::end_day()modification_
   {
      // float64 ETref = crop->transpiration_actual.get_ET_ref_output_mm();
      // float64 transpiration_act_mm = crop->transpiration_actual.get_actual_mm();
      // soil_pot_evaporation_mm = ETref - transpiration_act_mm;
      #ifdef CROPSYST_PROVIDED_ET
      ET_solar_irradiance.invalidate(true);
      #endif
      weather.invalidate();
      return true;
   }
   //_2010-12-10___________________________________________________________________
   Normal_crop_event_sequence Crop_with_weather::process()         modification_
   {
      //140314  Normal_crop_growth_stage result_growth_stage = NGS_DORMANT_or_INACTIVE;
      // 110421 NGS_DORMANT_or_INACTIVE;
      Normal_crop_event_sequence result_growth_stage;                            //140509
      if (crop) // 110614
      {
         if (crop_started && database_valid)                                     // 110617_110218_110614
         {
            #ifdef CROPSYST_PROVIDED_ET
            CORN::Quality ET_quality;
            #endif
            //140810 result_growth_stage = crop->process();
            crop->process();                                                     //140810
            result_growth_stage = crop->get_crop_event_sequence();               //140810
         }
         else // 110208
         {
            CORN::DOY doy = today.get_DOY();
            #ifdef CROPSYST_PROVIDED_SOIL_HYDROLOGY
            soil_act_evaporation_mm = m_to_mm(evaporate(0.0));                   //110614
            #endif
            CORN::DOY earliest_season_start_DOY =
               crop_parameters.season_correction.earliest_start_DOY;
            bool winter_crop = (earliest_season_start_DOY > 182);
            if (winter_crop)
            { // Winter crop
               if (doy >= earliest_season_start_DOY)
               {
                  crop_started = true;
               }
            }
            else
            { // Spring/summer crop
               float64 Tavg = weather.air_temperature_avg./*LML 150318 get_*/Celcius();
               window_avg_temperatures.append(Tavg);
               nat16 window_avg_temperatures_count =
                  window_avg_temperatures.get_count();                           // 110620
               if (window_avg_temperatures_count >=                              // 110620
                  crop_parameters.inactive_period.consider_inactive_days)
               {
                  float64 mean_window_avg_temp =
                     window_avg_temperatures.get_mean();
                  if (mean_window_avg_temp >
                     crop_parameters.inactive_period.inducement_temperature)
                  // was thermal_time.base_temperature
                     found_inducible_period = true;
                  else
                     window_avg_temperatures.remove_at(0);  //LML 141112 replaced from remove()
               }
               if (found_inducible_period && (doy >= earliest_season_start_DOY))
                  crop_started = true;
            }
            if (crop_started)
               result_growth_stage == NGS_SOWING; // 110209
         }
         #ifdef UNIT_TEST
         if (!headers_output)
         {
            crop->daily_growth_header(debug_out_crop, 0);
            debug_out_crop << std::endl;
            crop->daily_growth_header(debug_out_crop, 1);
            debug_out_crop << std::endl;
            crop->daily_growth_header(debug_out_crop, 2);
            debug_out_crop << std::endl;
            headers_output = true;
         }
         crop->daily_growth_output(debug_out_crop);
         debug_out_crop << std::endl;
         #endif
      }
      return result_growth_stage;
   }
   //_______________________________________________________________process()__/
   float64 Crop_with_weather::get
      (nat32 variable_code  )const
   {  return crop ? crop->get(variable_code) : 0.0;
   }
   //_____________________________________________________________________get__/
   bool Crop_with_weather::set_database_directory(const char *directory_name)
   {  database_directory_name.set_cstr(directory_name);
      return true;
   }
   //_2011-02-02________________________________________set_database_directory_/
   bool Crop_with_weather::find_crop_file
      (const std::string &crop_name
      ,const std::wstring &crop_ext)
   {
      CORN::OS::Directory_name_concrete crop_directory(database_directory_name,"crop");
      if (!CORN::OS::file_system_engine.exists(crop_directory)) // May be it is upper case
      {  CORN::OS::Directory_name_concrete Crop_directory(database_directory_name,"Crop");
         crop_directory.set_DEN(Crop_directory);
      }
      CORN::OS::Directory_name_concrete crop_code_directory(crop_directory,"code");
      delete crop_parameters_filename;
      crop_parameters_filename = new CORN::OS::File_name_concrete(crop_code_directory,crop_name,crop_ext);
      return CORN::OS::file_system_engine.exists(*crop_parameters_filename);
   }
   //_2011-02-02_______________________________________________find_crop_file__/

   bool CPU_is_big_endian =
#if (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
   true;
#else
   false;
#endif

   bool Crop_with_weather::adjust_crop_phenology(float64 latitude,float64 longitude)
   {  bool adjusted = false;
      bool adjustment_found = false;
      std::wstring crop_parameter_name;                             //LML 141020
      float32 phenology_adjustment_for_cell = 1.0;
      float32 season_start_adjustment_for_cell = 1.0;
      CORN::OS::Directory_name_concrete crop_directory(database_directory_name,"crop");
      if (!CORN::OS::file_system_engine.exists(crop_directory))     // May be it is upper case
      {  CORN::OS::Directory_name_concrete Crop_directory(database_directory_name,"Crop");
         crop_directory.set_DEN(Crop_directory);
      }
      CORN::OS::Directory_name_concrete crop_code_directory(crop_directory,"code");
      crop_parameter_name = crop_parameters_filename->get_name();//LML 141104 get_name_with_extension(crop_parameter_name);
      CORN::OS::File_name_concrete crop_calibration_filename(crop_code_directory,crop_parameter_name,L"VIC_crop_adjust"); //141020LML
      nat32 low_index = 0;
      nat32 adjustment_record_size = sizeof(float64)+sizeof(float64)+sizeof
         (float32) // 110314
      +sizeof(float32);
      nat32 high_index =
         (CORN::OS::file_system_engine.get_file_size_bytes(crop_calibration_filename) //140509
         - adjustment_record_size) / adjustment_record_size;
      std::ifstream crop_calibration_file(crop_calibration_filename.c_str(),
         std::ios_base::binary);
      nat32 last_mid_index = 0;
      while (!adjusted && crop_calibration_file.good())
      {
         nat32 mid_index = (high_index + low_index) / 2;
         // start search halfway through the file
         nat32 curr_pos = mid_index * adjustment_record_size;
         crop_calibration_file.seekg(curr_pos);
         float64 latd;
         crop_calibration_file.read((char*)(&latd), sizeof(float64));
         float64 lngtd;
         crop_calibration_file.read((char*)(&lngtd), sizeof(float64));
         float32 phen_adjst;
         crop_calibration_file.read((char*)(&phen_adjst), sizeof(float32));
         float32 ssn_start_adjst;
         crop_calibration_file.read((char*)(&ssn_start_adjst), sizeof(float32));
         if (latd > latitude)       high_index = mid_index;
         else if (latd < latitude)  low_index  = mid_index;
         else if (lngtd > longitude)high_index = mid_index;
         else if (lngtd < longitude)low_index  = mid_index;
         if (CORN::is_approximately(latd, latitude,
               0.0001) && CORN::is_approximately(lngtd, longitude, 0.0001))
         {  phenology_adjustment_for_cell = phen_adjst;
            adjusted = true;
            adjustment_found = true;
            season_start_adjustment_for_cell = ssn_start_adjst;
         }
         else if ((low_index == high_index) || (mid_index == last_mid_index))
         {  adjusted = true;
            adjustment_found = false;
            phenology_adjustment_for_cell = 1.0;
            season_start_adjustment_for_cell = 1.0;
         }
         last_mid_index = mid_index;
      }
      if (!adjustment_found)
      {  std::cout << "Unable to find adjustment in:" <<
            crop_calibration_filename << " for the cell at :" << latitude <<
            ", " << longitude << std::endl;
      }
      crop_parameters.adjust_phenology(phenology_adjustment_for_cell,
         season_start_adjustment_for_cell);
      return adjustment_found;
   }
//_2011-03-01___________________________________________adjust_crop_phenology__/

nat8  Crop_with_weather::Soil_layers::get_max_number_layers()              const { return (nat8)VIC_get_max_number_soil_layers(); }
      // This is the absolute maximum number of layers
      // (array dimension) that are reserved to store soil profile data by layer.
nat8  Crop_with_weather::Soil_layers::get_number_layers()                  const { return VIC_get_number_soil_layers(); }
      // This is the number of layers identified in the soil profile.
nat8  Crop_with_weather::Soil_layers::get_number_uninundated_layers()      const { return VIC_get_number_soil_layers(); }
//______________________________________________________________________________
float64 Crop_with_weather::Soil_layers::get_thickness_m(nat8 layer)        const
{  return  VIC_get_soil_layer_thickness_m(layer);
}
//_2012-08-20_________________________________________________get_thickness_m__/
float64 Crop_with_weather::Soil_layers::get_depth_m(nat8 CropSyst_layer)                                            const
{  float64 depth = 0;
   if (!depths_provided)
   {
      for (nat8 l = 1; l <= get_number_layers(); l++)
      {
         depth += get_thickness_m(l);
         depths[l] = depth;
      }
      depths_provided = true;
   }
   return depths[CropSyst_layer];
}
//_2012-08-22___________________________________________________________________
float64 Crop_with_weather::Soil_hydrology::get_water_plus_ice_content_gravimetric (nat8 layer)const { assert(false); return 0;  } //this model does not use this method
float64 Crop_with_weather::Soil_hydrology::get_liquid_water_content_volumetric    (nat8 layer)const { return VIC_soil_hydrology_get_water_content(layer);  } // this model does not use this method
float64 Crop_with_weather::Soil_hydrology::get_water_plus_ice_content_volumetric_at
(nat8 layer,CORN::Hour hour)                                                                  const { return VIC_soil_hydrology_get_water_content(layer);  } // this model does not use this method
float64 Crop_with_weather::Soil_hydrology::get_water_plus_ice_content_volumetric(nat8 layer)  const { return VIC_soil_hydrology_get_water_content(layer); }

float64 Crop_with_weather::Soil_hydrology::get_water_potential                   (nat8 layer) const { return VIC_get_water_potential(layer); }
float64 Crop_with_weather::Soil_hydrology::extract_water(float64 water_uptake_m[] ,nat8 start_layer)
      { return VIC_soil_hydrology_extract_water(water_uptake_m,start_layer); }
//______________________________________________________________________________
Crop_with_weather::Soil::Soil()
#ifndef CROPSYST_PROVIDED_SOIL_HYDROLOGY
   //120612 currently VIC is providing soil hydrology
: layers()
, hydrology(new Soil_hydrology(layers))
#endif
{
}
//_2014-08-12___________________________________________________________________

} // VIC_crop

#endif

