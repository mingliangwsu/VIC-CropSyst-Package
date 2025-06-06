#include "project/GIS/GIS_table.h"
#include "corn/OS/file_system_engine.h"
#include "cs_scenario.h"
namespace CropSyst {                                                             //130621
//______________________________________________________________________________
 Simulation_override_record::Simulation_override_record
(int32  _scenario_ID                                                             //050919
,const Polygon_simulation_parameters &parameter_overrides_from_pat
,nat8   actual_seqnum)                                                           //050729
: Data_record("simulation")
, Item()
, scenario_ID(_scenario_ID)                                                      //050919
, same_as_ID(equiv_polygon_not_present)
, same_as_record_num(0)                                                          //050631
, run_it(true)                                                                   //050729
, notes("")                                                                      //050922
, parameter_overrides(parameter_overrides_from_pat)                              //050630
, cell_width(0)                                                                  //050630
{  // When the polygon attribute table does not have a specified sequence
   // number (I.e. when running the regional soil weather database mode it will be 0,
   // we will be running the simulation for all sequences
   // (we generate a simulation override record for each sequence) 050729
   parameter_overrides.SEQNUM = actual_seqnum;                                   //050729
}
//_Simulation_override_record:constructor______________________________________/
bool Simulation_override_record::check_coded_file_exists
(const CORN::OS::File_name&   template_filename
,const std::string&           filename_code
,std::ostream&                errors)
{  bool result = true;
   if (filename_code.length())
      {  CORN::OS::File_name_concrete actual_filename(template_filename.get_parent_directory_name_qualified(),filename_code);
         if (!CORN::OS::file_system_engine.exists(actual_filename))
         {
            // errors << "ID:" << ID << common_translate(L_Unable_to_open_file) << ' ' << actual_filename << ". " << common_translate(L_file_may_not_exist) << endl;
            errors
               /*050915 << "ID:" << parameter_overrides.polygon_ID  The filename should identify the scenario, it is no*/
               << "Unable to open file" << ' ' << actual_filename.c_str() << ". " << "file may not exist" << std::endl;
            result = false;                                                      //980721
         }
      }
   return result;
}
//_check_coded_file_exists__________________________________________2001-01-29_/
bool Simulation_override_record::setup_record
(const Scenario*      template_sim_params                                        //010609
,bool                _soils_mapped                                               //010520
,bool                _uniform_soil_profile_mapped                                //050531
,float32             _cell_width                                                 //990528
,bool                subsurface_flow                                             //990604
,bool                _steepness_mapped                                           //010129
,float32            *horizon_thickness_override                                  //991004
,bool                _init_PAW_mapped                                            //010129
,bool                _init_NO3_mapped                                            //010129
,bool                _weather_mapped                                             //010129
,bool                _rotations_mapped                                           //010129
,bool                _recalibration_mapped                                       //060411
,bool                _water_table_mapped                                         //060706
,std::ostream      &errors)
{
   parameter_overrides.soils_mapped      = _soils_mapped;                        //010520
   parameter_overrides.uniform_soil_profile_mapped =_uniform_soil_profile_mapped;//050531
   parameter_overrides.weather_mapped    = _weather_mapped;                      //010520
   parameter_overrides.rotations_mapped  = _rotations_mapped;                    //010520
   parameter_overrides.recalibration_mapped  = _recalibration_mapped;            //060411
   parameter_overrides.water_table_mapped  = _water_table_mapped;                //060706
   parameter_overrides.steepness_mapped  = _steepness_mapped;                    //010520
   parameter_overrides.init_PAW_mapped   = _init_PAW_mapped;                     //010520
   parameter_overrides.init_NO3_mapped   = _init_NO3_mapped;                     //010520

   CORN::strip_string (parameter_overrides.soil_code, CORN::Both, ' ');          //101101
   CORN::strip_string (parameter_overrides.weather_code, CORN::Both, ' ');       //101101
   CORN::strip_string (parameter_overrides.rotation_code, CORN::Both, ' ');      //101101
   //             Don't attempt to setup the simulation override if we dont have a template_sim file
   bool polygon_simulation_valid = true;                                         //980721
   {
      cell_width = _cell_width;                                                  //990525
      if (horizon_thickness_override)                                            //991004
            for (int8 horizon = 0; horizon <= MAX_soil_horizons; horizon++)      //991004
               horizon_thickness[horizon] = horizon_thickness_override[horizon]; //991004
      if (template_sim_params)                                                   //140216
      {
      if (parameter_overrides.soils_mapped                                       //010520
            && !check_coded_file_exists(template_sim_params->soil_filename,parameter_overrides.soil_code,errors))
      {  polygon_simulation_valid = false;                                       //050922
         notes.append("Soil file not found:"); notes.append(parameter_overrides.soil_code); //050922
      }                                                                          //050922
      // Rotation ID is now provided through ArcCS  table
      // Weather ID and Location_ID
      // is also provided through  ArcCS table
      // But we still check that the locatton file exists

      if (parameter_overrides.weather_mapped                                     //050630
          &&  !check_coded_file_exists(template_sim_params->weather_filename,parameter_overrides.weather_code,errors))
      {  polygon_simulation_valid = false;                                       //050922
         notes.append("weather file not found:"); notes.append(parameter_overrides.weather_code); //050922
      }                                                                          //050922
      if (parameter_overrides.rotations_mapped                                   //050630
          &&  !check_coded_file_exists(template_sim_params->rotation_filename,parameter_overrides.rotation_code,errors))
      {  polygon_simulation_valid = false;                                       //050922
         notes.append("Rotation file not found:"); notes.append(parameter_overrides.rotation_code); //050922
      }                                                                          //050922
      }
    }
   return polygon_simulation_valid;                                              //990417
}
//_setup_record_____________________________________________________2001-01-29_/
int Simulation_override_record::compare(const Item &other)                 const
{  int result = 0;
   Simulation_override_record &other_comb = (Simulation_override_record &)other;
   result = parameter_overrides.compare(other_comb.parameter_overrides);         //050630
   return result;
}
//_compare_____________________________________________________________________/
bool Simulation_override_record::expect_structure(bool for_write)
{  bool expected = Data_record::expect_structure(for_write);
   set_current_section("tracking");
   expect_int32("scenario_ID",scenario_ID);                                      //050919
   expect_int32("same_as_ID" ,same_as_ID);
   expect_int32("same_as_rec",same_as_record_num);
   expect_bool("run",run_it);                                                    //050922
   set_current_section("GIS");
   parameter_overrides.setup_structure(*this,for_write);                         //050630
   // Notes should be the last column because it is so wide
   set_current_section("other");                                                 //050922
   expect_string("notes",notes,255);                                             //050922
   structure_defined = true;                                                     //120314
   return expected;
}
//_expect_structure_________________________________________________1998-12-08_/
const std::string& Simulation_override_record::compose_as_command_line_arguments
(std::string& command_line_args)                                           const
{  std::string buffer;                                                           //140616
   FOR_EACH_IN(section,VV_Section,sections,each_section)
   {
      //for each entry in current section:
      std::string entries;
      FOR_EACH_IN(entry,VV_abstract_entry,section->entries,each_entry)
      {  // lookup field in actual fields:
         //181118 const char *entry_label = entry->get_variable().c_str()/*180821 get_key()*/;  //180821 may need need to replace get_key
         // Currently entries dont know the default values if (!entry->has_default_value())
         entry->append_to_string(buffer);
         if (buffer.length() && (buffer.c_str()[0] != '<'))                      //140616
         {  entry->append_variable(entries);                                     //181118
            entries.append("=");
            entry->append_to_string(buffer);                                     //140616
         }
      } FOR_EACH_END(each_entry)
      if (entries.length())
      {  command_line_args.append("[");
         section->key_string(command_line_args);                                 //180820
         command_line_args.append("] ");
         command_line_args.append(entries);
      }
   } FOR_EACH_END(each_section)
   return command_line_args;
}
//_compose_as_command_line_arguments________________________________2014-02-16_/
}//_namespace CropSyst_________________________________________________________/                                                          //130621

