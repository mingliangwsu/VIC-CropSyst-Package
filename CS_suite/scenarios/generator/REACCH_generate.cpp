#include "CS_suite/scenarios/generator/scenarios_generator_engine.h"
#include "corn/OS/file_system_engine.h"
#include "common/geodesy/geocoordinates.h"
#include <assert.h>
namespace CS
{
//______________________________________________________________________________
nat32 Scenarios_generator_engine::generate_REACCH_scenarios()
{
   nat32 scenarios_ultimate_generated = 0;
   CORN::Text_list CSs;
   {  // Get list of all accessable rotation files
      CORN::Unidirectional_list *rotation_filenames = databases
         ->render_specialization_files_qualified_in_context_string("Rotation");
      FOR_EACH_IN(rot_filename,CORN::OS::File_name,(*rotation_filenames),each_rot)
      {  std::wstring CS_name(rot_filename->get_name());
         CSs.add_wstring(CS_name);
      } FOR_EACH_END(each_rot)
   }
   CORN::Text_list RCPs;
   /*
   if (   arguments.target_name_template_composed.has_in_path_cstr
            ("equilibration",include_qualification,false)
       || arguments.target_name_template_composed.has_in_path_cstr
            ("historical"   ,include_qualification,false))
   */

   std::wstring first_level;

   if (arguments.template_scenario.find(L"equilibration")!=std::wstring::npos)
      first_level = L"equilibration";
   else if (arguments.template_scenario.find(L"historical")   !=std::wstring::npos)
      first_level = L"historical";
   else
   {  first_level = L"future";
      RCPs.add_cstr("RCP4.5");                                                   //151019
      RCPs.add_cstr("RCP8.5");                                                   //151019
   }

//170512    CORN::OS::Directory_name_concrete first_scenario(arguments.generate_directory,first_level);

// std::clog << "first scenario:" << first_scenario.c_str() << std::endl;

   CORN::OS::Directory_name *Database_Weather_dir                                //170612
               = databases->render_closest_context(L"Weather");                  //170612
   if (!Database_Weather_dir) return 0; // Should always find weather dir        //170612
   FOR_EACH_IN(cropping_syst,CORN::Item_wstring/*Text_list::Item*/,CSs,each_CS)
   {  // cropping_syst is unqualified and no ext
      CORN::OS::Directory_name_concrete CS_dir
         (arguments.generate_directory,(*cropping_syst));

      // WARNING check this the CS directory will be under
      // the copied template generated directory (I.e. historical, equilibration, future name)

      if (RCPs.count())
      {
         FOR_EACH_IN(RCP,CORN::Item_wstring/*Text_list::Item*/,RCPs,each_RCP)
         {  // RCP is unqualified and no ext
            CORN::OS::Directory_name_concrete CS_RCP_dir(CS_dir,(*RCP));
            CORN::OS::Directory_name_concrete  Weather_future_dir
               (*Database_Weather_dir,L"future");
            CORN::OS::Directory_entry_name_concrete Weather_future_RCP_dir
               (Weather_future_dir,*RCP);
            CORN::Text_list climates;  //GCMS or historical
            CORN::OS::file_system_engine.list_names_in
               (Weather_future_RCP_dir,0,&climates);
            FOR_EACH_IN(climate,CORN::Item_wstring,climates,each_climate)
            {
               CORN::OS::Directory_name_concrete CS_RCP_climate(CS_RCP_dir,*climate);
               scenarios_ultimate_generated += generate_REACCH_cell_scenarios
                  (CS_RCP_climate
                  ,Weather_future_RCP_dir);                                      //170612
            } FOR_EACH_END(each_climate)
            //170612 delete Database_Weather_dir;
         } FOR_EACH_END(each_RCP)
      }
      else
      {
         CORN::OS::Directory_name_concrete Database_Weather_historical_dir       //170612
            (*Database_Weather_dir,L"historical");                               //170612
            // (will be the same list of files for equilibration)
         scenarios_ultimate_generated += generate_REACCH_cell_scenarios
            (CS_dir,Database_Weather_historical_dir);                            //170612
      }
   } FOR_EACH_END(each_CS)
   delete Database_Weather_dir;                                                  //170612
   return scenarios_ultimate_generated;
}
//______________________________________________________________________________
Geocoordinate_format cell_name_format
= {{0,1,2},decimal_degrees,true,2,2,0,0,0};
// 2 digit precision, 0 fill, no separator,no elevation

nat32 Scenarios_generator_engine::generate_REACCH_cell_scenarios
(const CORN::OS::Directory_name_concrete &penulimate_scenario_directory
,const CORN::OS::Directory_name_concrete &Database_Weather_scenario_directory)
// Directory names dont need to be concrete
{
   geodesy::Geocoordinates available_weather_locations(&arguments);                   //170612
   REACCH_get_available_weather_locations                                        //170612
            (available_weather_locations,Database_Weather_scenario_directory);   //170612
   nat32 scenarios = 0;
   CORN::Unidirectional_list REACCH_predominant_soils;
   FOR_EACH_IN(predefined_scenario,Predefined_scenario
      ,predefined_combinations,each_predefined)
   {  //  predefined_combinations correspond to cells
      std::string predefined_scenario_name; // cell ID
      assert (predefined_scenario->Geocoordinate::is_valid());
      // scenario_name is simply the predefined_scenario geocoordinate
      if (predefined_scenario->is_valid()) //(predefined_combinations.fields_dBase->geocoordinate_field)
         // use user specified geocoordinate (code)
      {
         predefined_scenario->know_format(cell_name_format);
         predefined_scenario->label_string(predefined_scenario_name);
      }
      std::string MUKEY(predefined_scenario->soil_MUKEY);
      CORN::OS::Directory_entry_name *database_soil_filename
      = databases->render_find_in_context_string(L"Soil",MUKEY,"CS_soil");
      CORN::OS::Directory_name_concrete cell_scenario_directory                  //170612
         (penulimate_scenario_directory,predefined_scenario_name);
      if (database_soil_filename)
      {
         float64 soil_area = predefined_scenario->soil_area;
         REACCH_Predominant_soil *existing_predominant_soil
            = dynamic_cast<REACCH_Predominant_soil *>
               (REACCH_predominant_soils.find_string(predefined_scenario_name));
         REACCH_Predominant_soil *most_predominant_soil = 0;
         if (existing_predominant_soil)
         {
            if (existing_predominant_soil->soil_area < soil_area )
            {  // we have a better soil so delete the previously created soil file
               CORN::OS::File_name_concrete existing_soil_filename
               (cell_scenario_directory,existing_predominant_soil->MUKEY,L"CS_soil");
//std::clog << "recycling:" << existing_soil_filename.c_str() << std::endl;
               CORN::OS::file_system_engine.recycle(existing_soil_filename,true);
//if (CORN::OS::file_system_engine.exists(existing_soil_filename))
//std::clog << "recycle failed" << std::endl;
               REACCH_predominant_soils.remove(existing_predominant_soil);
               most_predominant_soil = new REACCH_Predominant_soil;
            }
         } else
         {
            most_predominant_soil = new REACCH_Predominant_soil;
            scenarios ++;
         }
         if (most_predominant_soil)
         {
            most_predominant_soil->ID = predefined_scenario_name;
            REACCH_predominant_soils.take(most_predominant_soil);
            most_predominant_soil->MUKEY = MUKEY;
            most_predominant_soil->soil_area = soil_area;
            if (CORN::OS::file_system_engine.provide_directory/*180321 create_directory*/(cell_scenario_directory))
               {}
               /*171215 disabled/not needed
               if (ultimates_file)
                 (*ultimates_file) << cell_scenario_directory.c_str() << std::endl;
               */
            CORN::OS::File_name_concrete cell_soil_filename
               (cell_scenario_directory,MUKEY,L"CS_soil");
            create_best_kind_of_link(*database_soil_filename,cell_soil_filename);
         }
         // geocoordinates might not exactly match available weather files
         // so create an inheriting link to the UED file.
         REACCH_inherit_link_weather_if_needed                                   //170612
            (*predefined_scenario
            ,cell_scenario_directory
            ,available_weather_locations
            ,Database_Weather_scenario_directory);
      } // else not a valid cell (don't generate)
      delete database_soil_filename;
   } FOR_EACH_END(each_predefined)
   return scenarios;
}
//______________________________________________________________________________
nat32  Scenarios_generator_engine::REACCH_get_available_weather_locations
(geodesy::Geocoordinates &available_weather_locations // returned
,const CORN::OS::Directory_name &Database_Weather_scenario_dir)
{
   CORN::Text_list weather_names;
   CORN::OS::file_system_engine.list_names_in
      (Database_Weather_scenario_dir,&weather_names,0);
   FOR_EACH_IN(weather_name_ext,CORN::Item_wstring
              ,weather_names,each_weather)
   {  nat32 UED_pos =  weather_name_ext->find(L".UED");
      std::wstring weather_nameW(*weather_name_ext,0,UED_pos);                   //180511
      std::string  weather_name;
      CORN::wstring_to_string(weather_nameW,weather_name);                       //180511
      Geocoordinate_item *geocoord = new Geocoordinate_item();
      Geocoordinate_format surmised_format;
      geocoord->set_from_ISO_6709(weather_name,surmised_format);
      available_weather_locations.append(geocoord);
   } FOR_EACH_END(each_weather)
   return available_weather_locations.count();
}
//_2017-06-12___________________________________________________________________
bool Scenarios_generator_engine::REACCH_inherit_link_weather_if_needed
(const Geocoordinate &predefined_scenario_geocoord
,const CORN::OS::Directory_name_concrete &cell_scenario_directory // doesnt need to be concrete
,const geodesy::Geocoordinates &available_weather_locations
,const CORN::OS::Directory_name_concrete &Database_Weather_scenario_directory) // doesnt need to be concrete
{
   const Geocoordinate *nearest_geocoord = available_weather_locations.find_nearest
      (predefined_scenario_geocoord);
   if (predefined_scenario_geocoord.compare(*nearest_geocoord) == 0)
   {  // nearest geocoord is the scenarios geocoord so no need to link, the
      // scenario will simply used the directory's geocoord for weather filename composition.
      nearest_geocoord = 0;
      return false;
   }
   if (nearest_geocoord)
   {
      std::string nearest_geocoord_string;
      nearest_geocoord->label_decimal_degrees_with_format
         (nearest_geocoord_string,true,2,2,0,0,0) ;
      CORN::OS::Directory_entry_name_concrete nearest_geocoord_database_weather_filename
         (Database_Weather_scenario_directory
         ,nearest_geocoord_string,L"UED");
      CORN::OS::File_name_concrete cell_weather_filename
         (cell_scenario_directory,nearest_geocoord_string,L"UED");
      create_best_kind_of_link
         (nearest_geocoord_database_weather_filename
         ,cell_weather_filename);
   }
   return true;
}
//_2017-06-12___________________________________________________________________
} // namespace CS
