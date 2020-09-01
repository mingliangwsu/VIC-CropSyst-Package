7#include "VIC_crop_weather_soil.h"
#include "VIC_crop_variable_codes.h"
#include <iomanip>
#include "corn/data_source/vv_file.h"
#include "geodesy/geometry/terrestrial_geometry.h"
#ifdef UNIT_TEST
std::ofstream debug_out_soil("unit_test_soil.dat");
#endif

namespace VIC_crop
{
//______________________________________________________________________________
Crop_with_weather_soil::Crop_with_weather_soil
(const char* database_directory_name
      // NYI,const char *crop_file_name
#ifdef NONCONTINUOUS
      // 110621 obsolete
, nat16 _WSDA_crop_type_code
#endif
, float64 latitude_dec_deg, float64 longitude_dec_deg
, float64 elevation_meter, bool _automatic_harvest
, bool _enable_CO2_response, bool _assume_irrigation)
: Crop_with_weather(database_directory_name
#ifdef NONCONTINUOUS
      // 110621 obsolete
, _WSDA_crop_type_code
#endif
, latitude_dec_deg, longitude_dec_deg
, elevation_meter
, _automatic_harvest
, _enable_CO2_response, _assume_irrigation)
,soil_parameters_filename() /* default to not using soil file */
   // 110202
, soil_parameters() // 110202
, soil_properties(0)
, soil_pot_evaporation_mm(0) // 110527
, soil_act_evaporation_mm(0) // 110527
//, soil_hydrology(0)

   // 110207
, irrigation_threshold_LWP(0), assume_irrigation(_assume_irrigation)
, refill_depth_m(0)
#ifdef UNIT_TEST

//   , headers_output(false)
#endif
{
#ifdef UNIT_TEST
      daily_soil_header(debug_out_soil, 0);
      debug_out_soil << std::endl;
      daily_soil_header(debug_out_soil, 1);
      debug_out_soil << std::endl;
      daily_soil_header(debug_out_soil, 2);
      debug_out_soil << std::endl;
#endif
#ifdef NONCONTINUOUS
      // 110621 obsolete
      start_crop(_WSDA_crop_type_code);
#endif

   }

   // _____________________________________________________Crop_with_weather_soil__/
#ifndef NONCONTINUOUS

   Crop_common /*140808 Crop*/ * Crop_with_weather_soil::start_crop(nat16 WSDA_crop_type_code)
      modification_
   {
      Crop_with_weather::start_crop(WSDA_crop_type_code);
      crop->know(soil_properties->get_layers(), soil_hydrology);
      return crop;
   }
   // _2011-06-14______________________________________________________start_crop__/
#endif
//   #ifdef OLD_SUBLAYERING
   void Crop_with_weather_soil::know_soil_layer_thickness(float64 layer1_m,
      float64 layer2_m, float64 layer3_m)
   {
      for (int l = 0; l < 10; l++)
         VIC_horizon_thickness[l] = 0.0;
      VIC_horizon_thickness[1] = layer1_m;
      VIC_horizon_thickness[2] = layer2_m;
      VIC_horizon_thickness[3] = layer3_m;
      unmodifiable_ Soil_layers_interface &soil_layers =
         dynamic_cast<const Soil_layers_interface&>
         (*(soil_properties->get_layers()));

      for (int layer = 1; layer < soil_layers.get_number_layers(); layer++)
      {
         float64 layer_center_depth = soil_layers.get_layer_center_depth(layer);
         if (layer_center_depth < VIC_horizon_thickness[1])
            VIC_horizon_at_layer[layer] = 1; // first layer is 10cm
         if ((layer_center_depth >= VIC_horizon_thickness[1]) &&
            (layer_center_depth <= VIC_horizon_thickness[2]))
         // second layer is 30cm
            VIC_horizon_at_layer[layer] = 2;
         if ((layer_center_depth > VIC_horizon_thickness[2]))
            VIC_horizon_at_layer[layer] = 3;
      }
   }
//   #endif
   // _2011-03-02_______________________________________know_soil_layer_thickness__/
   float64 Crop_with_weather_soil::know_water_entering_soil_mm
      (float64 water_entering_soil_mm)                             modification_
   {
      float64 flux_in = water_entering_soil_mm; // Flux_In = Water_Flux_In
      // liquid fluxes are in mm/day = kg/m2/day
      float64 flux_out = 0;
      unmodifiable_ Soil_hydraulic_properties_interface &soil_hydraulic_props =
         dynamic_cast<const Soil_hydraulic_properties_interface&>
         (*soil_properties->get_hydraulic_properties_constant());
      unmodifiable_ Soil_layers_interface &soil_layers =
         dynamic_cast<const Soil_layers_interface&>
         (*(soil_properties->get_layers()));
      Soil_hydrology_interface &soil_hydrology = *(this->soil_hydrology);
      for (nat8 layer = 1 // J = 1
         ; layer <= soil_layers.get_number_layers() && flux_in > 0.0
         // While (j <= Number_Of_Layers) And (Flux_In > 0)
         ; layer++)
      {
         float64 WC = soil_hydrology.get_liquid_water_content_volumetric
            (layer); // WC = SoilState.WaterContent(Year, DOY, j)
         float64 layer_thickness = soil_layers.get_thickness_m(layer);
         // m      //    dz = SoilParameters.GetDZ(j)
         float64 FC = soil_hydraulic_props.get_field_capacity_volumetric
            (layer); // FC = SoilProfile.GetFieldCapacity(j)
         if (flux_in > water_density * (FC - WC) * layer_thickness)
         // If Flux_In > WD * (FC - WC) * dz Then
         {
            flux_out = flux_in - water_density * (FC - WC) * layer_thickness;
            // Flux_Out = Flux_In - WD * (FC - WC) * dz
            WC = FC; // WC = FC
         }
         else // Else
         {
            WC = WC + flux_in / (water_density * layer_thickness);
            // WC = WC + Flux_In / (WD * dz)
            flux_out = 0; // Flux_Out = 0
         } // End If
         soil_hydrology.set_water_content_volumetric_layer(layer, WC); // SoilState.WaterContent(Year, DOY, j) = WC                                    //    SoilState.WaterContent(Year, DOY, j) = WC
         flux_in = flux_out; // Flux_In = Flux_Out
         // j = j + 1
      } // Wend
      // Not doing anything with drainage when flux_out > 0;                        // If (j > Number_Of_Layers) And (Flux_Out > 0) Then
      // Drainage = Flux_Out
      // Else
      // Drainage = 0
      // End If
      return flux_out; // drainage
   }

   // _2010-Nov-01____________________________________know_water_entering_soil_mm__/
//   #ifdef OLD_SUBLAYERING
   float64 Crop_with_weather_soil::know_soil_water_content
      (nat8 VIC_horizon_number // index from 1 (VIC layer + 1)
      , float64 new_water_content_volumetric)modification_
   {
      float64 drainage = 0.0;
      unmodifiable_ Soil_layers_interface &soil_layers =
         dynamic_cast<const Soil_layers_interface&>
         (*(soil_properties->get_layers()));
      Soil_hydrology_interface &soil_hydrology = *this->soil_hydrology;
      for (nat8 layer = 1; layer <= soil_layers.get_number_layers(); layer++)
      {
         int corresponding_horizon = VIC_horizon_at_layer[layer];
         if (VIC_horizon_number == corresponding_horizon)
         {
            soil_hydrology.set_water_content_volumetric_layer(layer,
               new_water_content_volumetric);
         }
      }
      return m_to_mm(drainage);
   }
//   #endif
   // ____________________________________________________know_soil_water_content__/
   float64 Crop_with_weather_soil::Soil_hydrology::extract_water
      (float64 amount_to_extract_m[], nat8 start_layer) // 110525 This is a hack because there is mismatch in water potential
   { // Extraction could be water uptake or evaporation
      // it is assumed that the amount to extract already
      // has been limited to permanent wilt point.


      float64 act_total_amount_extracted = 0.0;
      nat8  num_sublayers = soil_layers.get_number_layers();
      for (nat8 layer = start_layer; layer <= num_sublayers; layer++)
      {
         float64 liquid_WC = get_liquid_water_content_volumetric(layer);
         float64 layer_thickness_m = soil_layers.get_thickness_m(layer);
         float64 extract_volumetric = amount_to_extract_m[layer]
            / soil_layers.get_thickness_m(layer);
         float64 act_extract_volumetric = std::min<float64>(liquid_WC,
            extract_volumetric);
         // 110526  negative extraction can be valid      if (act_extract_volumetric > 0)
         {
            float64 act_extract_depth = act_extract_volumetric *
               layer_thickness_m;
            act_total_amount_extracted += act_extract_depth;
            water_content[layer] -= act_extract_volumetric;
         }
      }

      return act_total_amount_extracted;
   }

   // ______________________________________________________________________________
   Crop_with_weather_soil::Soil_hydrology::Soil_hydrology
   // constructor
   (const Soil_layers_interface &_soil_layers,
      const Soil_hydraulic_properties_interface &_hydraulic_properties)
      : hydraulic_properties(_hydraulic_properties), soil_layers(_soil_layers)
   {
      for (nat8 layer = 0; layer < MAX_soil_layers_alloc; layer++)
      {
         water_content[layer] = 0;
      }
   }

   // ________________________________________________Soil_hydrology::constructor__/
   float64 Crop_with_weather_soil::Soil_hydrology::get_water_potential
      (nat8  layer)const
   {
      float64 air_entry_pot = hydraulic_properties.get_air_entry_potential
         (layer, false);
      float64 water_content = get_liquid_water_content_volumetric(layer);
      float64 saturation = hydraulic_properties.
         get_saturation_water_content_volumetric(layer,ICE_WATER_CONTENT_RELEVENCE_UNKNOWN_140822);
      float64 b_value = hydraulic_properties.get_Campbell_b(layer, false);
      float64 water_pot = air_entry_pot * pow((water_content / saturation),
         (-b_value));
      return water_pot;
   }

   // ________________________________________________________get_water_potential__/
   float64 Crop_with_weather_soil::Soil_hydrology::refill_water_depth_m
      (bool assume_irrigation, nat8 observation_layers)modification_
   {
      float64 refill_required_profile_m = 0.0;
      for (nat8 layer = 1; layer <= observation_layers; layer++) // 110828
      {
         float64 layer_thickness_m = soil_layers.get_thickness_m(layer);
         // meter
         float64 WC = get_liquid_water_content_volumetric(layer);
         float64 WC_depth_m = WC * layer_thickness_m;
         float64 FC = hydraulic_properties.get_field_capacity_volumetric(layer);
         float64 FC_depth_m = FC * layer_thickness_m;
         float64 refill_required_layer_m = FC_depth_m - WC_depth_m;
         if (refill_required_layer_m > 0) // 110627
            refill_required_profile_m += refill_required_layer_m;
         if (assume_irrigation)
            set_water_content_volumetric_layer(layer, FC);
      } // for layer
      return refill_required_profile_m;
   }
   // _________________________________________________________refill_water_depth__/
   bool Crop_with_weather_soil::start_day(int32 date)modification_
   {  bool started = Crop_with_weather::start_day(date);
      soil_act_evaporation_mm = 0; // 110527
      return started;
   }
   // _2010-11-22_______________________________________________________start_day__/
   Normal_crop_event_sequence Crop_with_weather_soil::process()modification_
   {
      Normal_crop_event_sequence result_growth_stage = NGS_DORMANT_or_INACTIVE;
      if (crop) // 110614
      {
         if (crop_started && database_valid) // 110617_110218_110614
         {
            #ifdef REFACTOR
            soil_act_evaporation_mm = m_to_mm
               (evaporate(crop->canopy_cover_actual.
                  get_global_solar_rad_intercepted_total_canopy()  ));           //110906

            #else
            soil_act_evaporation_mm = m_to_mm
               (evaporate(*crop->canopy_cover_actual.
                  //110906 ref_solar_rad_intercepted_fraction()
                  ref_global_solar_rad_intercepted_green_canopy()
                  ));
            #endif
            result_growth_stage = crop->process();
            Soil_hydrology &soil_hydrology = *this->soil_hydrology;
            refill_depth_m = 0.0;
            // check for need for irrigation
            nat8 observation_layers = // 110828
            crop // 110828
            ? soil_properties->get_layers()->closest_layer_at
               (crop->get_root_depth()) // 110828
            : soil_properties->get_layers()->get_number_layers();                //110828
            refill_depth_m = soil_hydrology.refill_water_depth_m
               (assume_irrigation, observation_layers);
         }
         else // 110208
         {
            CORN::DOY doy = today.get_DOY();
            soil_act_evaporation_mm = m_to_mm(evaporate(0.0));                   //110614
            CORN::DOY earliest_season_start_DOY =
               crop->parameters.season_correction.earliest_start_DOY;
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
               float64 Tavg = crop->weather.air_temperature_avg.get_Celcius();
               window_avg_temperatures.append(Tavg);
               nat16 window_avg_temperatures_count =
                  window_avg_temperatures.get_count(); // 110620
               if (window_avg_temperatures_count >= // 110620
                  crop->parameters.inactive_period.consider_inactive_days)
               {
                  float64 mean_window_avg_temp =
                     window_avg_temperatures.get_mean();
                  if (mean_window_avg_temp >
                     crop->parameters.inactive_period.inducement_temperature)
                  // was thermal_time.base_temperature
                     found_inducible_period = true;
                  else
                     window_avg_temperatures.remove(0);
               }
               if (found_inducible_period && (doy >= earliest_season_start_DOY))
                  crop_started = true;
            }
            if (crop_started)
               result_growth_stage == NGS_SOWING; // 110209
         }
      }
      else
      /* No crop */
      // 110614
      {
         soil_act_evaporation_mm = m_to_mm(evaporate(0.0));                      //110614
      }
#ifdef UNIT_TEST
      daily_soil_output(debug_out_soil);
      debug_out_soil << std::endl;
#endif
      return result_growth_stage;
   }
   //_______________________________________________________________process()__/
   float64 Crop_with_weather_soil::get
      (nat32 variable_code /* NYI ,nat32 units_code */ )const
   {
      float64 value = 0.0;
      if (crop && (variable_code >= VC_uptake_mm)) // 110614
      {
         nat8 VIC_horizon_number = variable_code % 100;
         unmodifiable_ Soil_layers_interface &soil_layers =
            dynamic_cast<const Soil_layers_interface&>
            (*(soil_properties->get_layers()));
         switch((nat32)(variable_code / 100) * 100)
         {
            case VC_uptake_mm:
            {
               for (nat8 layer = 1; layer <= soil_layers.get_number_layers();
                  layer++)
               {
                  nat8 layers_horizon = VIC_horizon_at_layer[layer];
                  if (layers_horizon == VIC_horizon_number)
                     value += crop->transpiration_actual.get_uptake_mm(layer);
               }
            }break;
         }
      }
      else
      {
         switch(variable_code)
         {
#ifdef CROPSYST_PROVIDED_SOIL_HYDROLOGY
         case VC_refill_water_depth_mm:
            value = m_to_mm(get_refill_water_depth_m());
            break;
#endif
         case VC_evaporation_mm:
            value = soil_act_evaporation_mm;
            break;
         default:
            value = crop ? crop->get(variable_code) : 0.0;
            break;
         }
      }
      return value;
   }

   //_____________________________________________________________________get__/
   float64 Crop_with_weather_soil::get_uptake_mm(nat8 layer)
   {  return get(VC_uptake_mm + layer);
   }
   //___________________________________________________________get_uptake_mm__/
   void Crop_with_weather_soil::daily_soil_output
      (std::ofstream &daily_soil_file)
   {  daily_soil_file << today.get_year() << '\t' // Year
      << today.get_DOY() << '\t'; // doy
      unmodifiable_ Soil_layers_interface &soil_layers =
         dynamic_cast<const Soil_layers_interface&>
         (*(soil_properties->get_layers()));
      Soil_hydrology_interface &soil_hydrology = *this->soil_hydrology;
      for (nat8 layer = 1; layer <= soil_layers.get_number_layers(); layer++)
      {  daily_soil_file << soil_hydrology.get_liquid_water_content_volumetric
            (layer) << '\t';
      }
   }

   //_______________________________________________________daily_soil_output__/
   void Crop_with_weather_soil::daily_soil_header
      (std::ofstream &daily_soil_file, int line)
   {
      switch(line)
      {
      case 0:
         daily_soil_file << "Year\tDOY\tCrop sublayer WC (m3/m3)";
         break;
      case 2:
         daily_soil_file <<
            "1\t2\t3\t4\t5\t6\t7\t8\t9\t10\t11\t12\t13\t14\t15\t16\t17";
         break; // Evaporation\tEvaporation\t\t\tDrainage\tRunoff\tBalance"
      }
   }
   // ___________________________________________________________daily_soil_header_/
   bool Crop_with_weather_soil::set_database_directory
      (const char *directory_name)
   {
      bool crop_database_dir_set = Crop_with_weather::set_database_directory(directory_name);
      bool found_soil = find_soil_file();
      if (found_soil)
      {
         CORN::VV_File soil_file(soil_parameters_filename.c_str());
         soil_file.get(soil_parameters);
         if (soil_properties)
            delete soil_properties;
         soil_properties = new Soil_properties_sublayers(&soil_parameters,
            false);
         soil_properties->reinitialize(true, true, false);
         if (soil_hydrology)
            delete soil_hydrology;
         soil_hydrology = new Crop_with_weather_soil::Soil_hydrology(*soil_properties->get_layers(),
            *soil_properties->get_hydraulic_properties_modifiable());
         // By default water content is initialized to 0, but we need to have
         // non-zero values incase the STATSGO soil profile is deeper than
         // the VIC soil profile.
         unmodifiable_ Soil_layers_interface &soil_layers =
            dynamic_cast<const Soil_layers_interface&>
            (*(soil_properties->get_layers()));
         soil_hydrology->refill_water_depth_m(true,
            soil_layers.get_number_layers()); // 110828

         for (int layer = 1; layer < soil_layers.get_number_layers(); layer++)
         {
            float64 layer_center_depth = soil_layers.get_layer_center_depth
               (layer);
            if (layer_center_depth < VIC_horizon_thickness[1])
               VIC_horizon_at_layer[layer] = 1; // first layer is 10cm
            if ((layer_center_depth >= VIC_horizon_thickness[1]) &&
               (layer_center_depth <= VIC_horizon_thickness[2]))
            // second layer is 30cm
               VIC_horizon_at_layer[layer] = 2;
            if ((layer_center_depth > VIC_horizon_thickness[3]))
               VIC_horizon_at_layer[layer] = 3;
         }
      }
      else
      {
         std::cout << "Unable to find soil file:" <<
            soil_parameters_filename << std::endl;
      }
      return found_soil && crop_database_dir_set;
   }

   //_2011-02-02________________________________________set_database_directory_/
   bool CPU_is_big_endian =
#if (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
   true;
#else
   false;
#endif

   bool Crop_with_weather_soil::adjust_crop_phenology(float64 latitude,
      float64 longitude)
   {
      bool adjusted = false;
      bool adjustment_found = false;
      float32 phenology_adjustment_for_cell = 1.0;
      float32 season_start_adjustment_for_cell = 1.0;
      CORN::Directory_name crop_directory("crop");
      crop_directory.set_path(database_directory_name);
      if (!crop_directory.exists()) // May be it is upper case
         crop_directory.set_name("Crop");
      CORN::Directory_name crop_code_directory("code");
      crop_code_directory.set_path(crop_directory);
      CORN::Smart_file_name crop_calibration_filename(crop_parameters_filename);
      crop_calibration_filename.set_ext("VIC_crop_adjust");
      crop_calibration_filename.set_path(crop_code_directory);
      nat32 low_index = 0;
      nat32 adjustment_record_size = sizeof(float64)+sizeof(float64)+sizeof
         (float32) // 110314
      +sizeof(float32);
      nat32 high_index = (crop_calibration_filename.get_size_bytes()
         - adjustment_record_size) / adjustment_record_size;
      std::ifstream crop_calibration_file(crop_calibration_filename.c_str(),
         std::ios_base::binary);
      nat32 last_mid_index = 0;
      while (!adjusted && crop_calibration_file.good())
      {
         nat32 mid_index = (high_index + low_index) / 2;
         // start search halfway through the file

         nat32 curr_pos = mid_index * adjustment_record_size;
         /*
         crop_calibration_file.seek(curr_pos);
         float64 latd  = crop_calibration_file.read_float64();
         float64 lngtd = crop_calibration_file.read_float64();
         float64 adjst = crop_calibration_file.read_float32();
          */
         crop_calibration_file.seekg(curr_pos);
         float64 latd;
         crop_calibration_file.read((char*)(&latd), sizeof(float64));
         float64 lngtd;
         crop_calibration_file.read((char*)(&lngtd), sizeof(float64));
         float32 phen_adjst;
         crop_calibration_file.read((char*)(&phen_adjst), sizeof(float32));
         float32 ssn_start_adjst;
         crop_calibration_file.read((char*)(&ssn_start_adjst), sizeof(float32));

         if (latd > latitude)
            high_index = mid_index;
         else if (latd < latitude)
            low_index = mid_index;
         else if (lngtd > longitude)
            high_index = mid_index;
         else if (lngtd < longitude)
            low_index = mid_index;

         if (CORN::is_approximately(latd, latitude,
               0.0001) && CORN::is_approximately(lngtd, longitude, 0.0001))
         {
            phenology_adjustment_for_cell = phen_adjst;
            adjusted = true;
            adjustment_found = true;
            season_start_adjustment_for_cell = ssn_start_adjst;
         }
         else if ((low_index == high_index) || (mid_index == last_mid_index))
         {
            adjusted = true;
            adjustment_found = false;
            phenology_adjustment_for_cell = 1.0;
            season_start_adjustment_for_cell = 1.0;
         }
         last_mid_index = mid_index;
      }
      if (!adjustment_found)
      {
         std::cout << "Unable to find adjustment in:" <<
            crop_calibration_filename << " for the cell at :" << latitude <<
            ", " << longitude << std::endl;

      }
      crop_parameters.adjust_phenology(phenology_adjustment_for_cell,
         season_start_adjustment_for_cell);
      return adjustment_found;
   }
   //_2011-03-01________________________________________adjust_crop_phenology__/
   bool Crop_with_weather_soil::find_soil_file()
   {
      bool soil_found = false;
      CORN::Directory_name soil_directory("soil");
      soil_directory.set_path(database_directory_name);
      if (!soil_directory.exists()) // May be it is upper case
         soil_directory.set_name("Soil");
      float64 lat_decdeg;
      float64 long_decdeg;
      Terrestrial_geometry terrestrial_geometry;
      Geolocation *nearest_point = 0;
      float64 nearest_distance = 99999.9; // aribtrary large number to minimize
      float64 elev_m;
      std::string MU_key_or_ID;
      CORN::File_name lat_long_MU_filename("lat_long_MU.txt");
      lat_long_MU_filename.set_path(soil_directory);
      std::ifstream lat_long_MU_file(lat_long_MU_filename.c_str());
      std::string col_headers_line;
      getline(lat_long_MU_file, col_headers_line);
      while (lat_long_MU_file.good() && !soil_found)
      {
         lat_long_MU_file >> lat_decdeg >> long_decdeg >> elev_m >>
            MU_key_or_ID;
         Geolocation this_point;
         this_point.set_latitude_dec_deg_32(lat_decdeg);
         this_point.set_longitude_dec_deg_32(long_decdeg);
         float64 this_distance = terrestrial_geometry.distance_between_km
            (geolocation, this_point);
         // NYN this_point.set_elevation_meter(elev_m);
         if (CORN::is_approximately<float64>(this_distance, 0.0, 0.00001))
         {
            soil_parameters_filename.set_name(MU_key_or_ID);
            soil_parameters_filename.set_ext("CS_soil");
            soil_parameters_filename.set_path(soil_directory);
            soil_found = soil_parameters_filename.exists();
         }
         else
         {
            if (this_distance < nearest_distance)
            {
               nearest_distance = this_distance;
               // Compose the filename for the nearst point
               // so we can check if it exists.
               // (There will be no soil file for the map unit if it is water,
               // outcrop, or covered land).
               soil_parameters_filename.set_name(MU_key_or_ID); // 110224
               soil_parameters_filename.set_ext("CS_soil"); // 110224
               soil_parameters_filename.set_path(soil_directory); // 110224
               if (soil_parameters_filename.exists()) // 110224
               {
                  if (nearest_point)
                     delete nearest_point;
                  nearest_point = new Geolocation(this_point);
                  nearest_point->set_comment(soil_parameters_filename);
                  // (MU_key_or_ID);
               } // else the point is probably water, rock outcrop etc..
            }
         }
      }
      if (!soil_found)
      {
         soil_parameters_filename.set_str(nearest_point->get_comment()); // 110224
         soil_found = true;
         std::cout << "Unable to find soil for point:" << std::setprecision(5)
            << geolocation.get_latitude_dec_deg()
            << ", " << geolocation.get_longitude_dec_deg()
            << ". Using:" << nearest_point->get_latitude_dec_deg()
            << ", " << nearest_point->get_longitude_dec_deg()
            << " mapunit:" << soil_parameters_filename << std::endl;
      }
      delete nearest_point;
      return soil_found;
   }

   //_2011-02-02_______________________________________________find_soil_file__/
   float64 Crop_with_weather_soil::evaporate(float64
      Total_Canopy_Solar_Radiation_Interception_Fraction,
      float64 mulch_cover_fraction // Mulch_Solar_Radiation_Interception
      )modification_
   {
      /*
      Dim Air_Dry_WC As Double, Water_Content_Top_layer As Double, PWP_Top_Layer As Double
      Dim Soil_Water_Evaporation As Double
      Dim Pot_Soil_Water_Evaporation As Double
      Static Total_Canopy_Solar_Radiation_Interception_Fraction As Double
      Dim Maximum_Attainable_Soil_Water_Evaporation As Double
      Dim Mulch_Solar_Radiation_Interception As Double
      Dim Layer_Thickness As Double
      Dim Year As Integer, DOY As Integer, Year_Count As Integer, DACE As Integer
      Const WD = 1000 'Water density = 1000 kg/m3

      'XXXOJO Add mulch shading
       */
      // Year_Count = TimeState.YearCount
      // Year = TimeState.CurrentYear
      // DOY = TimeState.DayOfYear
      float64 Layer_Thickness = soil_properties->get_layers()->get_thickness_m
         (1); // Layer_Thickness = SoilParameters.GetDZ(1)
      float64 PWP_Top_Layer = soil_properties->get_hydraulic_properties_constant
         ()->get_permanent_wilt_point_volumetric(1); // PWP_Top_Layer = SoilProfile.GetPermanentWiltingPoint(1)
      float64 Water_Content_Top_layer =
         soil_hydrology->get_liquid_water_content_volumetric(1); // Water_Content_Top_layer = SoilState.WaterContent(Year_Count, DOY, 1)
      // If Crop_Exist Then DACE = TimeState.DaysAfterCropExist

      /*
      'Roger: modified
      If Crop_Exist Then
      Total_Canopy_Solar_Radiation_Interception_Fraction = CropState.FractionSolarRadInterceptedTotalCanopy(DACE - 1)
      Else
      Total_Canopy_Solar_Radiation_Interception_Fraction = 0
      End If
      'Roger: End modified
       */
      // Mulch_Solar_Radiation_Interception = 0 'XXXOJO
      float64 Air_Dry_WC = PWP_Top_Layer / 3.0; // Air_Dry_WC = PWP_Top_Layer / 3 'This is an approximation to air-dry watercontent
      /* This is an approximation to air-dry watercontent */
      float64 Maximum_Attainable_Soil_Water_Evaporation =
         (Water_Content_Top_layer > Air_Dry_WC) // If Water_Content_Top_layer > Air_Dry_WC Then
      // Maximum_Attainable_Soil_Water_Evaporation
      ? (Water_Content_Top_layer - Air_Dry_WC)
         * Layer_Thickness * water_density // = (Water_Content_Top_layer - Air_Dry_WC) * Layer_Thickness * WD
      : // Else
      0.0; // Maximum_Attainable_Soil_Water_Evaporation = 0
      // End If
      /*121217 ETref unused
      float64 ETref = crop ? crop->transpiration_actual.get_ET_ref_output_mm()
         : weather.ET_reference.get_mm(); // 110614
      */
      float64 transpiration_act_mm = crop ?
         crop->transpiration_actual.get_actual_mm() : 0.0; // 110614
      // soil_pot_evaporation_mm = ETref - transpiration_act_mm;

      float64 ET_max =   crop->transpiration_actual.get_crop_potential_ET();     //121116;
           //121116  crop_potential_ET is also known as ETmax (which is ETref *kc - droplet_evap
      float64 Pot_Soil_Water_Evaporation = //121116 ETref // Pot_Soil_Water_Evaporation = WeatherState.ReferenceCropET(Year_Count, DOY)
         ET_max                                                                  //121116
         * (1.0 - Total_Canopy_Solar_Radiation_Interception_Fraction)
         // * (1 - Total_Canopy_Solar_Radiation_Interception_Fraction)
         * (1.0 - mulch_cover_fraction); // * (1 - Mulch_Solar_Radiation_Interception)
      float64 Soil_Water_Evaporation = // evap mm/day = kg/m2/day
      (Water_Content_Top_layer > PWP_Top_Layer) // If Water_Content_Top_layer > PWP_Top_Layer Then
      ? Pot_Soil_Water_Evaporation // Soil_Water_Evaporation = Pot_Soil_Water_Evaporation 'evap mm/day = kg/m2/day
      : (Water_Content_Top_layer > Air_Dry_WC) // ElseIf Water_Content_Top_layer > Air_Dry_WC Then
      ? Pot_Soil_Water_Evaporation // Soil_Water_Evaporation = Pot_Soil_Water_Evaporation
      * CORN_sqr((Water_Content_Top_layer - Air_Dry_WC)
         // * ((Water_Content_Top_layer - Air_Dry_WC)
         / (PWP_Top_Layer - Air_Dry_WC)) // / (PWP_Top_Layer - Air_Dry_WC)) ^ 2
      : // Else
      0.0 // Soil_Water_Evaporation = 0
      ; // End If
      if (Soil_Water_Evaporation > Maximum_Attainable_Soil_Water_Evaporation) // If Soil_Water_Evaporation > Maximum_Attainable_Soil_Water_Evaporation Then
      {
         Soil_Water_Evaporation = Maximum_Attainable_Soil_Water_Evaporation;
         // Soil_Water_Evaporation = Maximum_Attainable_Soil_Water_Evaporation
         Water_Content_Top_layer = Air_Dry_WC;
         // Water_Content_Top_layer = Air_Dry_WC
      }
      else // Else
         Water_Content_Top_layer = Water_Content_Top_layer
         // Water_Content_Top_layer = Water_Content_Top_layer
         - Soil_Water_Evaporation / (Layer_Thickness * water_density);
      // - Soil_Water_Evaporation / (Layer_Thickness * WD)
      // End If
      soil_hydrology->set_water_content_volumetric_layer(1,
         Water_Content_Top_layer);
      /*
      SoilState.WaterContent(Year_Count, DOY, 1) = Water_Content_Top_layer
      SoilState.SoilWaterPotentialEvaporation(Year_Count, DOY) = Pot_Soil_Water_Evaporation
      SoilState.SoilWaterActualEvaporation(Year_Count, DOY) = Soil_Water_Evaporation

      If Crop_Exist Then
      CropState.PotentialSoilEvaporation(DACE) = Pot_Soil_Water_Evaporation
      CropState.ActualSoilEvaporation(DACE) = Soil_Water_Evaporation
      CropState.CumulativePotentialSoilEvaporation(DACE) = CropState.CumulativePotentialSoilEvaporation(DACE - 1) + Pot_Soil_Water_Evaporation
      CropState.CumulativeActualSoilEvaporation(DACE) = CropState.CumulativeActualSoilEvaporation(DACE - 1) + Soil_Water_Evaporation
      End If
       */
      return mm_to_m(Soil_Water_Evaporation);
   }
   //_______________________________________________________________evaporate__/
} // name space VIC_crop
