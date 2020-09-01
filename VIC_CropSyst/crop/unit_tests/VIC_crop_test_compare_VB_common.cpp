//---------------------------------------------------------------------------
#define MATCH_VIC
#pragma hdrstop
#include "VIC_crop_test_compare_VB_common.h"
#include <weather/parameter/WP_solar_radiation.h>
#include <weather/parameter/WP_relative_humidity.h>
#include "CropSyst_VB_spreadsheet_data.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#include <fstream>
#include <iostream>
#include "VIC_crop_variable_codes.h"
#include "../VIC_crop_weather_soil.h"
//#include "VIC_crop_test_soil_WC.h"

//#undef USE_WEATHER_PROVIDER
#include <common/weather/database/weather_UED_database.h>



float test_soil_layer_thickness[13] =
{0,0.05,0.05,0.1,0.1,0.1,0.1,0.1,0.3,0.3,0.3,0.3,0.3};

float sand[13] = {0,20,20,20,20,20,20,20,20,20,20,20,20};
float clay[13] = {0,15,15,15,15,15,15,15,15,15,15,15,15};
//______________________________________________________________________________

///#define OTHELLO
//#define PULLMAN
#define POINT_322112


//#define VIC_WEATHER


int debug_DOY = 0;

using namespace VIC_crop;

bool VIC_estimate_missing_values = false;

int crop_test_run
(const char *weather_UED
,CropSyst_VB_Spreadsheet_data &CropSyst_VB_data
,int radiogroup_crop_index
,bool weather_estimate
,bool CO2_submodel
,nat8 applied_water_mode
,nat16 harvest_years[]                                                   //110512
,float64 season_yields[]                                                   //110512
//,float64 &yield_t_ha
)
{
   nat16 season_index =0;
   float64 refill_water_depth_mm = 0;
   VIC_estimate_missing_values = weather_estimate;
   bool active_growth_started=false;

   CORN::OS::File_name_logical daily_growth_filename("C:\\temp\\daily_growth_output.dat");
   CORN::OS::File_name_logical daily_soil_WC_start_file_filename("C:\\temp\\daily_soil_WC_start_day_output.dat");
   CORN::OS::File_name_logical daily_soil_WC_end_file_filename("C:\\temp\\daily_soil_WC_end_day_output.dat");
   CORN::OS::File_name_logical daily_ET_filename("C:\\temp\\daily_ET.dat");

   std::ofstream daily_growth_file(daily_growth_filename.c_str());
   std::ofstream daily_soil_WC_start_file(daily_soil_WC_start_file_filename.c_str());
   std::ofstream daily_soil_WC_end_file(daily_soil_WC_end_file_filename.c_str());
   std::ofstream daily_ET(daily_ET_filename.c_str());

   std::ofstream yields("c:\\temp\\yields.dat");

   std::ofstream weather_dat("TmaxTminSradVPDdaytime.dat");

   Normal_crop_event_sequence curr_growth_stage = NGS_SOWING;

   nat16 crop_code = 0;
   nat16 crop_start_DOY = 1;
/*
   const char *weather_file_name =
      #ifdef VIC_WEATHER
      "C:\\Projects\\VIC\\weather validation\\pullman_bad_measured_SradHumid_VIC.UED";
//      "C:\\Projects\\VIC\\weather validation\\pullman_bad_VIC.UED"
      #else
      "C:\\dev\\VIC\\crop\\unit_tests\\pullman.ued";
      #endif
*/
   CORN::Year start_year;
   CORN::Year end_year;

   bool maturity_reached = false;
   bool sowing_reached = false;
   float64 uptake_mm = 0.0;
   bool perennial = false;
   switch (radiogroup_crop_index)
   {  case 0: { crop_code = 701; crop_start_DOY = 1;  start_year = 1998; end_year = 2006; perennial = true;}  break; // alfalfa
      case 1: { crop_code = 218; crop_start_DOY = 280;start_year = 1996; end_year = 2006;} break; // winterwheat
      case 2: { crop_code = 1827;crop_start_DOY = 120; start_year = 1996; end_year = 2006; } break; // potato
      case 3: { crop_code = 1401;crop_start_DOY = 1;  start_year = 1996; end_year = 2000; perennial = true;}  break; //
      case 4: { crop_code = 1101;crop_start_DOY = 1;  start_year = 1996; end_year = 2000; perennial = true;}  break; //
      case 5: { crop_code = 1824;crop_start_DOY = 90; start_year = 1996; end_year = 2006;  } break; //
      case 6: { crop_code =  901;crop_start_DOY = 1; start_year = 1996; end_year = 2006; perennial = true; } break; //
      case 7: { crop_code = 1205;crop_start_DOY = 1; start_year = 1996; end_year = 2006;  perennial = true;} break; //
      case 8: { crop_code = 2002;crop_start_DOY = 1; start_year = 1996; end_year = 2006;  perennial = true;} break; //
   }
      //"G:\\Simulation\\Database\\weather\\completed\\300033 - WSU HQ.UED"
      //"C:\\dev\\VIC\\crop\\unit_tests\\othello.ued"
   Weather_UED_database weather_database
      (weather_UED,std::ios_base::in ,CORN::measured_quality);
   const Geolocation *geoloc = weather_database.get_geolocation_constant();
   Crop_with_weather_soil crop_soil_weather                                      //110614
               ("C:\\Projects\\VIC\\database"                                    //110203
               ,geoloc->get_latitude_dec_deg(),geoloc->get_longitude_dec_deg(),geoloc->get_elevation()
               ,true // automatic_harvest
               ,CO2_submodel // enable_CO2_response
               ,false ); // don't assume irrigation

//   crop_soil_weather.know_atmospheric_CO2_concentration(360);
   crop_soil_weather.know_atmospheric_CO2_concentration(360);
   crop_soil_weather.daily_soil_header(daily_soil_WC_start_file,0); daily_soil_WC_start_file << std::endl;
   crop_soil_weather.daily_soil_header(daily_soil_WC_start_file,1); daily_soil_WC_start_file << std::endl;
   crop_soil_weather.daily_soil_header(daily_soil_WC_start_file,2); daily_soil_WC_start_file << std::endl;
   crop_soil_weather.daily_soil_header(daily_soil_WC_end_file,0); daily_soil_WC_end_file << std::endl;
   crop_soil_weather.daily_soil_header(daily_soil_WC_end_file,1); daily_soil_WC_end_file << std::endl;
   crop_soil_weather.daily_soil_header(daily_soil_WC_end_file,2); daily_soil_WC_end_file << std::endl;

   for (CORN::Date date(start_year,(CORN::DOY)1);date.get() <= CORN::Date(end_year,365).get() /*1997365*/;date.inc())
   {
      std::cout
         << ((int)(date.get_date32()) / 1000)
         << ' ' << ((int)(date.get_date32()) % 1000)
         << std::endl;

debug_DOY = date.get_DOY();
if (date.get() == 1996301)
std::cout << "reached";
      //      nat32 date = weather_elements[weather_date_index].year * 1000
      //           + weather_elements[weather_date_index].doy;




      //110614if (!crop_soil_weather)
      {  // No crop yet
         if (date.get_DOY() == crop_start_DOY) // creation  date (start looking for sowing)
         {
            if (perennial)
            {
               if (date.get_year() == start_year)
                  crop_soil_weather.start_crop(crop_code);                             //110614

            }else
               crop_soil_weather.start_crop(crop_code);                             //110614
            /*110614obsolete
            crop_soil_weather = new Crop_with_soil_weather
               ("C:\\Projects\\VIC\\database"                                    //110203
               ,crop_code
               ,geoloc->get_latitude_dec_deg(),geoloc->get_longitude_dec_deg(),geoloc->get_elevation()
               ,true // automatic_harvest
               ,false // enable_CO2_response
               ,false //true /*assume irrigation
               );
            #ifdef OLD_SUBLAYERING
            for (nat8 layer = 1; layer <= 12; layer ++)
               crop_soil_weather->know_soil_layer
                  (layer
                  ,test_soil_layer_thickness[layer]
                  ,20 // sand[layer]
                  ,15 // clay[layer]
                  ,0,0,false);
            #endif
            */

//100827 WARNING the subdivide was added to check subdivision

         // starting at sowing
            crop_soil_weather.crop->daily_growth_header(daily_growth_file,0); daily_growth_file << std::endl;
            crop_soil_weather.crop->daily_growth_header(daily_growth_file,1); daily_growth_file << std::endl;
            crop_soil_weather.crop->daily_growth_header(daily_growth_file,2); daily_growth_file << std::endl;


//110614            daily_ET << "date\t";
//110614obs            crop_soil_weather.ET_reference.dump_header(daily_ET);
//110614            daily_ET << std::endl;
         }
      }
      //110614 if (crop_soil_weather)
      {
         CropSyst_VB_Spreadsheet_data::Daily *daily = CropSyst_VB_data.provide_daily(date.get_year(),date.get_DOY());
         float64 water_entering_soil = 0.0;

         if (date.get_DOY() == crop_start_DOY)
         {
#ifdef MATCH_VIC
/*
            crop_soil_weather.know_soil_water_content(1,0.230835);
            crop_soil_weather.know_soil_water_content(2,0.2259838);
            crop_soil_weather.know_soil_water_content(3,0.2259838);
            crop_soil_weather.know_soil_water_content(4,0.2259838);
            crop_soil_weather.know_soil_water_content(5,0.2259838);
            crop_soil_weather.know_soil_water_content(6,0.1847668);
            crop_soil_weather.know_soil_water_content(7,0.1847668);
            crop_soil_weather.know_soil_water_content(8,0.1847668);
            crop_soil_weather.know_soil_water_content(9,0.1847668);
            crop_soil_weather.know_soil_water_content(10,0.1847668);
            crop_soil_weather.know_soil_water_content(11,0.1847668);
            crop_soil_weather.know_soil_water_content(12,0.1847668);
            // High initial water water_entering_soil = 200.0;
*/
#else
#endif

         if (season_index == 0)
         for (nat8 layer = 1; layer <= 12; layer++)
               crop_soil_weather.know_soil_water_content
//#ifdef PULLMAN
                  (layer,0.28);  // WC is 0 indexed
/*
#endif
#ifdef OTHELLO
                  (layer,0.11);  // WC is 0 indexed
#endif
#ifdef POINT_322112
                  (layer,0.11);  // WC is 0 indexed
#endif
*/
         }

         if (daily)
         {
            CORN::Quality tmax_quality;
            CORN::Quality tmin_quality;
            CORN::Quality windspeed_quality;

            //NYI weather_database.get_to(crop_soil_weather.weather);         //110911

#ifdef MATCH_VIC
            CORN::Quality precip_quality;
            float64 infiltration_mm = 0.0; // m_to_mm(weather_database.get_precipitation_m(precip_quality, date));
            infiltration_mm = m_to_mm(weather_database.get_precipitation_m(precip_quality, date));
            m_to_mm(weather_database.get_precipitation_m(precip_quality, date));
            //NYI infiltration_mm = crop_soil_weather.weather.precipitation.get_mm();
            switch (applied_water_mode)
            {
               case 0 : /* only precipitation */ break;
               case 1 : infiltration_mm = (refill_water_depth_mm > 20.0) ? 20.0 : infiltration_mm; break;
               case 2 :  infiltration_mm = 10.0; break;
            }

//infiltration_mm = 0.0;
           //
            if (infiltration_mm > 0.0)
               water_entering_soil= infiltration_mm;
            /* high water entering debug
            else
               water_entering_soil = 5.0;
            */
#endif

//water_entering_soil = 0;

            float64 drainage = crop_soil_weather.know_water_entering_soil_mm(water_entering_soil);


            crop_soil_weather.know_air_temperature
               (weather_database.get_max_temperature(tmax_quality, date)
               ,weather_database.get_min_temperature(tmin_quality, date));

            tmax_quality.set(measured_quality);
            tmin_quality.set(measured_quality);
            windspeed_quality.set(measured_quality);

            float64 WS_m_d = weather_database.get_wind_speed_m_d(windspeed_quality, date);
            float64 WS_m_s =  m_per_day_to_m_per_sec(WS_m_d);
            crop_soil_weather.know_wind_speed_m_s(WS_m_s);


            if (!weather_estimate)
            {

               CORN::Quality Srad_quality;
               CORN::Quality RH_max_quality;
               CORN::Quality RH_min_quality;
               float64 Srad   = weather_database.get_solar_radiation(Srad_quality,date);
               float64 RHmax  = weather_database.get_max_relative_humidity(RH_max_quality,date);
               float64 RHmin  = weather_database.get_min_relative_humidity(RH_min_quality,date);

               RH_max_quality.set(measured_quality);
               RH_min_quality.set(measured_quality);

               crop_soil_weather.weather.solar_radiation.set_MJ_m2(Srad,measured_quality);
               crop_soil_weather.weather.relative_humidity_max.set(RHmax,RH_max_quality);
               crop_soil_weather.weather.relative_humidity_min.set(RHmin,RH_min_quality);

/*
               crop_soil_weather.weather.provide_solar_radiation(UT_day,false).set_MJ_m2(Srad,Srad_quality);
               crop_soil_weather.weather.provide_relative_humidity_maximum(UT_day,false).set(RHmax,RH_max_quality);
               crop_soil_weather.weather.provide_relative_humidity_minimum(UT_day,false).set(RHmin,RH_min_quality);
*/
            }

#ifndef  CROPSYST_PROVIDED_ET
            CORN::Quality srad_quality;
            crop_soil_weather.know_weather
               (weather_database.get_max_temperature(tmax_quality, date)
               ,weather_database.get_min_temperature(tmin_quality, date)
               ,weather_database.get_solar_radiation(srad_quality, date)
               ,daily->VPD_daytime);
#endif
            crop_soil_weather.start_day(date.get_date32());
            crop_soil_weather.daily_soil_output(daily_soil_WC_start_file); daily_soil_WC_start_file << std::endl;


weather_dat
<< (int)date.get_year() <<"\t"
<< (int)debug_DOY << "\t"
<<  crop_soil_weather.weather.precipitation.get() << "\t"
<<  crop_soil_weather.weather.air_temperature_max.get() << "\t"
<<  crop_soil_weather.weather.air_temperature_min.get() << "\t"
<<  crop_soil_weather.weather.solar_radiation.get() << "\t"
<<  crop_soil_weather.weather.relative_humidity_max<< "\t"
<<  crop_soil_weather.weather.relative_humidity_min<< "\t"
<<  crop_soil_weather.weather.wind_speed.get()<< "\t"
//<<  crop_soil_weather.weather.vapor_pressure_deficit_daytime.get() << "\t"
<< std::endl;


#ifndef  CROPSYST_PROVIDED_ET
            //101209 unused float VPD_daytime =  daily->VPD_daytime;
            float64 ETr = daily->ETr;
            crop_soil_weather.know_potential_reference_evapotranspiration_mm(ETr);
#endif
//obs            ETref_date_index ++;
//            if (curr_growth_stage != NGS_HARVEST)
            {
            float64 biomass = crop_soil_weather.get(VC_biomass_current_t_ha);
               curr_growth_stage = crop_soil_weather.process();

            for (int layer = 1; layer <= 3; layer++)
                     uptake_mm += crop_soil_weather.get_uptake_mm(layer);


            switch (curr_growth_stage)
            {
               case NGS_HARVESTABLE          :
               {

               } break;
               case NGS_SOWING               :
                  if (!sowing_reached)
                  {
                  yields << "Sowing=" << date.get() << '\t';
                  maturity_reached = false;
                  sowing_reached = true;
                  uptake_mm = 0.0;
                  }
                  break;
               case NGS_EMERGENCE            : std::cout << "Emergence" << std::endl; break;
               case NGS_RESTART              : std::cout << "Restart" << std::endl; break;
               case NGS_ACTIVE_GROWTH        :
               {
                   if (!active_growth_started)
                   {
               crop_soil_weather.know_soil_water_content(1,0.141867); // WC is 0 indexed
               crop_soil_weather.know_soil_water_content(2,0.141867);  // WC is 0 indexed
               crop_soil_weather.know_soil_water_content(3,0.092598);  // WC is 0 indexed
               crop_soil_weather.know_soil_water_content(4,0.092598);  // WC is 0 indexed
               crop_soil_weather.know_soil_water_content(5,0.092598);  // WC is 0 indexed
               crop_soil_weather.know_soil_water_content(6,0.092598);  // WC is 0 indexed
               crop_soil_weather.know_soil_water_content(7,0.0894912);  // WC is 0 indexed
               crop_soil_weather.know_soil_water_content(8,0.0894912);  // WC is 0 indexed
               crop_soil_weather.know_soil_water_content(9,0.0894912);  // WC is 0 indexed
               crop_soil_weather.know_soil_water_content(10,0.0894912);  // WC is 0 indexed
               crop_soil_weather.know_soil_water_content(11,0.0894912);  // WC is 0 indexed
               crop_soil_weather.know_soil_water_content(12,0.0894912);  // WC is 0 indexed
               crop_soil_weather.know_soil_water_content(13,0.0894912);  // WC is 0 indexed
               crop_soil_weather.know_soil_water_content(14,0.0894912);  // WC is 0 indexed
               crop_soil_weather.know_soil_water_content(15,0.0894912);  // WC is 0 indexed
               crop_soil_weather.know_soil_water_content(16,0.0894912);  // WC is 0 indexed
               crop_soil_weather.know_soil_water_content(17,0.0894912);  // WC is 0 indexed
               crop_soil_weather.know_soil_water_content(18,0.0894912);  // WC is 0 indexed
               crop_soil_weather.know_soil_water_content(19,0.0894912);  // WC is 0 indexed
               crop_soil_weather.know_soil_water_content(20,0.0894912);  // WC is 0 indexed
                     active_growth_started = true;
                   }

                  std::cout << "Acive growth" << std::endl;
               }   break;

               case NGS_ANTHESIS            : std::cout << "Flowering" << std::endl; break;
               case NGS_FILLING              : std::cout << "filling" << std::endl; break;
               case NGS_MATURITY             :
                     if (!maturity_reached)
                     {
                        yields << "Maturity=" << '\t';
                        yields << date.get() << '\t';
                        maturity_reached = true;
                     }
                     break;
               case NGS_DORMANT_or_INACTIVE  :  break;
               case NGS_HARVEST              :
               {
                  float64 biomass_after_harvest = crop_soil_weather.get(VC_biomass_after_harvest_t_ha);
                  std::cout << "Final biomass:" << biomass_after_harvest << std::endl;
                  float64 yield_t_ha = crop_soil_weather.get(VC_biomass_yield_t_ha);
                  harvest_years[season_index] = date.get_year();
                  season_yields[season_index] = yield_t_ha;




                  yields << "after_biomass=" << (biomass_after_harvest + yield_t_ha) << '\t'
                     << "yield="<< yield_t_ha <<'\t'
                     << "uptake=" << uptake_mm <<'\t'
                     << std::endl;
                  season_index ++;

                  sowing_reached = false;
               } break;
               case  NGS_POSTHARVEST :
               {
                  //110614 delete crop_soil_weather; crop_soil_weather = 0;
               }

               case NGS_TERMINATED           : // Due to premature death, ploughing under etc.
               default :   break; // do nothing in these other cases
            } // switch

//110614            if (crop_soil_weather)
//110614            {
#ifdef CROPSYST_PROVIDED_SOIL_HYDROLOGY
               refill_water_depth_mm = crop_soil_weather.get(VC_refill_water_depth_mm);
#endif
               float64 LAI = crop_soil_weather.get(VC_leaf_area_index);
               float64 evap = crop_soil_weather.get(VC_evaporation_mm);
               if (crop_soil_weather.crop)
               {
               crop_soil_weather.crop->daily_growth_output(daily_growth_file);
               daily_growth_file
                  <<daily_growth_file << "\td\t" << drainage
                  <<daily_growth_file << "\tLAI\t" << LAI
                  <<daily_growth_file << "\tevap\t" << evap
                  << std::endl;
               }
               crop_soil_weather.daily_soil_output(daily_soil_WC_end_file); daily_soil_WC_end_file << std::endl;
               daily_ET <<  "\"" << date.get_year() << " " << date.get_DOY() << '\"' << '\t';
//110614obs               crop_soil_weather.ET_reference.dump_detail(daily_ET);

               std::cout << "Biomass:" << crop_soil_weather.get(VC_biomass_current_t_ha) << std::endl;
               crop_soil_weather.end_day();

               daily_ET << std::endl;
            }
//110614            }
         } // if daily
//         soil_date_index++;
      } // if crop_soil_weather
   } // for
   /*110614
   delete crop_soil_weather;
   crop_soil_weather = 0;
   */
   return EXIT_SUCCESS;
} // crop_test
//__________________________________________________________________crop_test__/

