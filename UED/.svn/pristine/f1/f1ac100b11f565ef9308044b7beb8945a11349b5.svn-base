#include "UED/convert/convert_response.h"
#include "corn/OS/file_system_engine.h"
#include "corn/OS/FS/directory_entry.h"
//#include "corn/OS/FS/directory_entry.h"
#include "corn/seclusion.h"
using namespace CORN::OS::FS;
namespace UED {                                                                  //171120
//______________________________________________________________________________
//______________________________________________________________________________
//______________________________________________________________________________
// type,{direction{depth max/min},required,pattern}
Path_discovery_struct UED_discovery_raw    = { file_bit,{(descendent|breadth_first),{1,0}},{1,1},"*.UED"};
Path_discovery_struct foreign_discovery_raw= { file_bit,{(descendent|breadth_first),{1,0}},{0,1},0};         // pattern is arbitrary
Path_discovery_struct format_discovery_raw = { file_bit,{(descendent|breadth_first),{1,0}},{0,1},"*.TDF"};
   // TDF is not necessarily required,  specific convertors may already
   // and a defined format

//______________________________________________________________________________
Convertor_arguments::Convertor_arguments()
: CORN::Arguments                   ()
//200806 ,UED_filenameX                       ("")
//200806 ,foreign_filenameX                   ("")
//200806,format_filenameX                    ("")
,UED_filename_discovery             (0) //(UED_discovery_raw      ,"UED")
,foreign_filename_discovery         (0) //(foreign_discovery_raw  ,"target")
,format_filename_discovery          (0) //(format_discovery_raw   ,"format")
,UED_filenames                      ()
,calculate_daily                    (true)
,location_info_imported             (false)
,use_station_name                   (false)
,format                             ("")
,subformat                          ("")
,units_system                       ("intrinsic")
,year_earliest                      (0)
,year_latest                        (9999)
,date_time_earliest                 (0.0)
,date_time_latest                   (9999365.999999)
,seasonal                           (false)                                      //181106
,offset_years                       (0)
,prefix                             ("")
,vapor_pressure_calculation         (0)
,net_radiation_option               (0)
,geolocation_is_provided_by_caller  (false)
,geolocation                        ()
,geolocation_record                 ("coordinate")                               //191007
,allow_16bit_floats_in_UED          (false)                                      //150307
{
   UED_filename_discovery     = new CORN::OS::FS::Path_discovery_cowl(UED_discovery_raw      ,"UED");
   foreign_filename_discovery = new CORN::OS::FS::Path_discovery_cowl(foreign_discovery_raw  ,"target");
   format_filename_discovery  = new CORN::OS::FS::Path_discovery_cowl(format_discovery_raw   ,"format");
   path_discoveries.take(UED_filename_discovery);
   path_discoveries.take(foreign_filename_discovery);
   path_discoveries.take(format_filename_discovery);
   set_current_section("convert");                                               //170328

   /*191007 temp disabled until I implement Geolocation_record

   geolocation_record.know(&geolocation);                                        //191007
   */
}
//_Convertor_arguments:constructor_____________________________________________/
bool Convertor_arguments::expect_structure(bool for_write)
{
   bool expected = CORN::Arguments::expect_structure(for_write);                 //200804
   //200804 bool expected = CORN::Data_record::expect_structure(for_write);
   expect_string  ("command"           ,operation,255);   // deprecated
      // This was used by CS_explorer V4
   expect_string  ("operation"         ,operation,255);
   /*200804  now using CORN::Arguments path_discovery.
   expect_file_name("UED_filename"     ,UED_filenameX);        // Used mainly (only?) for export (on import there could be multiple stations (each generating their own UED file)
   */
   /*200804
   expect_string  ("UED_filenames"     ,UED_filenames,2048);
   expect_file_name("target_filename"  ,target_filename);        // Target filename (text file)
   */
   expect_string  ("station_ID_filter" ,station_IDs_filter,1024);
   /*200804
   expect_file_name("format_filename"  ,format_filename);     // I.e. TDF file
   */
   expect_string  ("format"            ,format,255);
   expect_string  ("subformat"         ,subformat,255);
   expect_string  ("units_system"      ,units_system,20);
   set_current_section("years");
      expect_int16   ("earliest"          ,year_earliest);
      expect_int16   ("latest"            ,year_latest);
      expect_int16   ("offset"            ,offset_years);
   set_current_section("date_times");
      expect_float64 ("earliest"          ,date_time_earliest);
      expect_float64 ("latest"            ,date_time_latest);
      expect_bool    ("seasonal"          ,seasonal);                            //181106
   set_current_section("import");
      expect_bool    ("calculate_daily"   ,calculate_daily);
      expect_bool    ("use_station_name"  ,use_station_name);
   set_current_section("export");
      expect_string("prefix",prefix,255);
      expect_bool    ("allow_16bit_floats",allow_16bit_floats_in_UED);           //150307
   set_current_section("status");
      expect_bool    ("location_info_imported",location_info_imported);
   set_current_section("DYRESM");
      expect_int16("vapor_pressure_calculation",vapor_pressure_calculation);
      expect_int16("net_radiation_option",net_radiation_option);
   if (!for_write || (for_write && geolocation_is_provided_by_caller))
   {
      set_current_section("geolocation");
      /*NYI structure_defined &= */geolocation.setup_structure(*this,for_write);
   }

   UED_filename_discovery->setup_structure(*this,for_write);                      //200804
   UED_filename_discovery->setup_structure(*this,for_write);                      //200804

   set_current_section("convert");                                               //171120
   structure_defined = true;                                                     //120314
   return expected;                                                              //161103
}
//_expect_structure_________________________________________________1998-04-05_/
bool Convertor_arguments::get_end()
{  bool ended = true;
   //200804 ended &= CORN::Arguments::get_end();
   if (date_time_earliest == 0)
       date_time_earliest = 1;
   if (operation.length() == 0) operation = "export";                            //191103
   else operation = "import";
   // Original covertors and command keyword, this is deprecated
   if (unrecognized.find_cstr("import")) operation = "import";                   //200202
   if (unrecognized.find_cstr("export")) operation = "export";                   //200202
   // We dont actually know what is source or target until this point
   // CORN::Arguments may have tried to instanciate
   // source and target but these need to be instanciated here.

   // Setting up discovery parameters so CORN::Arguments::get_end
   // with discover filename options automatically.
   if (operation == "import")                                                    //200804
   {  // Data is loaded from abritrary format files (source)
      // into target UED

      // convertor alreadys requires UED filename UED_filename_discovery.options_ref.required = 1;
      foreign_filename_discovery->options_ref.require.specifying = 1;
      foreign_filename_discovery->options_ref.require.existing   = 1;
         // The original UED convertors only processed a single input file
      /*200807
      discovery_source = &foreign_filename_discovery;
      discovery_target = &UED_filename_discovery;
      */
   } else // export
   {
      //convertor alreays requires UED filename UED_filename_discovery.options_ref.required = 1;

      // When exporting it is not necessary to specify the foreign target
      // filename, in this case output is written to stdout
      foreign_filename_discovery->options_ref.require.specifying = 0;
      foreign_filename_discovery->options_ref.require.existing   = 0;
      /*200807
      discovery_source = &UED_filename_discovery;
      discovery_target = &foreign_filename_discovery;
      */
   }
   //discovery_source->options_ref.required = true;                                //200804
   // discovery_target->options_ref.required = false;                               //200804
/*200804  obsolete now using Path_discovery
   CORN::Inclusion UED_pattern("*.UED",true);                                    //191103
   if (operation == "import")                                                    //191103
   {

      UED_filename_discovery->must_exist = false;                                //200804
         // Import my be creating the UED file

      CORN::OS::Directory_entry_name *found_UED_fname
         = dynamic_cast<CORN::OS::Directory_entry_name *>
            (discover_one
               (UED_pattern
               ,CORN::OS::file_entry
               ,CORN::OS::File_system::superdirectory_recursion_inclusive)
            );
      if (found_UED_fname)
         UED_filenameX.set_DEN(*found_UED_fname);
    else // export the UED file would be created
    {
      UED_filename_discovery->must_exist = true;                                //200804
         // Explort requires the UED file to exist


      CORN::Item *ued_fname_raw = discover_one(UED_pattern);
      if (ued_fname_raw)
      {  std::wstring buffer;
         ued_fname_raw->key_wstring(buffer);
         UED_filenameX.set_wstring(buffer);
      }
    }
   }
*/
/*200805 obsolete now using Path_discovery
   CORN::Inclusion TDF_pattern("*.TDF",true);                                    //191103
   CORN::OS::Directory_entry_name *TDF_filename
       = dynamic_cast<CORN::OS::Directory_entry_name *>
            (discover_one
               (TDF_pattern
               ,CORN::OS::file_entry
               ,CORN::OS::File_system::superdirectory_recursion_inclusive)
            );
   if (TDF_filename)
   {
      format_filename.set_DEN(*TDF_filename);
      ended &= true;
   }
*/
/*200807 added to path discoveries so now ended automatically
   ended &= format_filename_discovery.get_end();                                //200805
*/
   // CORN::Argument::get_end() must be called last
   // because it handles search for source and target
   // but we need to know operation selection so we can setup requirements
   return ended
      &= CORN::Arguments::get_end();
}
//_get_end__________________________________________________________2017-06-16_/

/*200804
bool Convertor_arguments::get_end()
{
   bool ended = CORN::Arguments::get_end();
   if (date_time_earliest == 0)
       date_time_earliest = 1;

   if (operation.length() == 0) operation = "export";                            //191103
   else operation = "import";

   // Original covertors and command keyword, this is deprecated
   if (unrecognized.find_cstr("import")) operation = "import";                   //200202
   if (unrecognized.find_cstr("export")) operation = "export";                   //200202

   CORN::Inclusion UED_pattern("*.UED",true);                                    //191103
   if (operation == "import")                                                    //191103
   {
      CORN::OS::Directory_entry_name *found_UED_fname
         = dynamic_cast<CORN::OS::Directory_entry_name *>
            (discover_one
               (UED_pattern
               ,CORN::OS::file_entry
               ,CORN::OS::File_system::superdirectory_recursion_inclusive)
            );
      if (found_UED_fname)
         UED_filenameX.set_DEN(*found_UED_fname);
    else // export the UED file would be created
    {
      CORN::Item *ued_fname_raw = discover_one(UED_pattern);
      if (ued_fname_raw)
      {  std::wstring buffer;
         ued_fname_raw->key_wstring(buffer);
         UED_filenameX.set_wstring(buffer);
      }
    }
   }
   CORN::Inclusion TDF_pattern("*.TDF",true);                                    //191103
   CORN::OS::Directory_entry_name *TDF_filename
       = dynamic_cast<CORN::OS::Directory_entry_name *>
            (discover_one
               (TDF_pattern
               ,CORN::OS::file_entry
               ,CORN::OS::File_system::superdirectory_recursion_inclusive)
            );
   if (TDF_filename)
   {
      format_filename.set_DEN(*TDF_filename);
      ended &= true;
   }
   return ended;
}
//_get_end__________________________________________________________2017-06-16_/

*/
//______________________________________________________________________________
}//_namespace_UED___________________________________________________2017-11-20_/

