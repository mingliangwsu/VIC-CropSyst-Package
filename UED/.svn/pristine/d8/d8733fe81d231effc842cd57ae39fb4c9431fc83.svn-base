#include "collate_engine.h"
#include "corn/OS/file_system_engine.h"
#include "corn/data_source/vv_file.h"
#if (COLLATOR_VERSION==2014)
#include "output_collator_2014.h"
#else
#include "output_collator_2019.h"
#endif
using namespace CORN::OS;                                                        //200806
using namespace CORN::OS::FS;                                                    //200806
namespace UED
{
//______________________________________________________________________________
Collate_engine::Collate_engine
(Arguments_collator &arguments_)
:  arguments(arguments_)
{}
//_Collate_engine:constructor__________________________________________________/
bool Collate_engine::process()
{
   CORN::OS::FS::Path_discovery *source                                          //200802
      = dynamic_cast<CORN::OS::FS::Path_discovery *>                             //200802
         (arguments.path_discoveries.find_string("source"));                     //200807
   //200807 (arguments.provide_discovery_source());                              //200802
   if (arguments.is_verbose(2)) //discovery
   {  std::string UED_names_buffer;
      arguments.UED_names.string_items(UED_names_buffer,' ');
      // These names are the UED file names (pattern) to be found and processed
      std::clog <<  "collate output:"<< UED_names_buffer<< std::endl;
   }
   if (arguments.is_verbose(2)) //inclusion/selection
   {  std::string level_captions_buffer;arguments.level_captions.string_items
         (level_captions_buffer,' ') ;
      std::clog << "levels=" << level_captions_buffer << std::endl;
   }
   nat32 total_detail_lines = 0;
   FOR_EACH_IN(UED_name, CORN::Item_string,arguments.UED_names, each_UED )
   {  if (arguments.is_verbose(1)) //progression
      {  std::clog << "collating:" <<  (*UED_name) << std::endl;
      }
      std::wstring UED_nameW; CORN::string_to_wstring((*UED_name),UED_nameW);
      total_detail_lines += process_UED(UED_nameW);
   } FOR_EACH_END(each_UED)
   return total_detail_lines > 0;
}
//_process__________________________________________________________2014-09-03_/
nat32 Collate_engine::process_UED(const std::wstring &UED_name)                  //180119
{
   nat32 UED_detail_lines = 0;
   #if (COLLATOR_VERSION==2014)
   Output_collator_2014
   #else
   Output_collator_2019
   #endif
    collator
      (arguments,UED_name                                                        //160526_170328
      ,arguments.level_captions,arguments.exclude_directories);
   // The variable definition array is instanciated here because it needs
   // to be the same for each set of UED files processed.
   collator.initialize();
   CORN::Text_list parent_level_names;
   UED_detail_lines = collator.collate_level
      (arguments.start_directory                                                 //170328
      ,0,parent_level_names,true);
   /*lines_written += */collator.commit_postamble();                             //190107

   File_name *format_filename
         #if (COLLATOR_VERSION==2014)
         (collator.parameters.format_filename_discovery.provide_primary());      //200806_160606
         #else
         (arguments.target_format_discovery->provide_primary());                 //200806_191007_190105
         // probably works for all version, I just haven't checked yet
         #endif

   if (format_filename && CORN::OS::file_system_engine.exists(*format_filename)) //200806
   {
      std::clog << "info: "
      << "Format already exists it will not be overwritten" << std::endl
      << format_filename->c_str()                                                //200806_190105
      << std::endl;
   }

/*200806
   CORN::OS::File_name_concrete format_filename
         #if (COLLATOR_VERSION==2014)
         (collator.parameters.format_filename);                                  //160606
         #else
         (arguments.target.format_filename);                                     //191007_190105
         // probably works for all version, I just haven't checked yet
         #endif
   if (CORN::OS::file_system_engine.exists(format_filename))
   {
      std::clog << "info: "
      << "Format already exists it will not be overwritten" << std::endl
      << format_filename.c_str()                                                 //190105
      << std::endl;
   }
*/
   else
   {  // No TDF file was specified on the command line
      // so by default the default format (R) is used
      // and columns are derived from variables available in the
      // UED file.
      // We write out the format created and used to the CWD
      // so the user can identify the columns and units.
      CORN::OS::Directory_name_concrete CWD;
      if ((CS_VERSION > 0) && (CS_VERSION < 6))
      {
         CORN::OS::File_name_concrete TDF_filename_generated(CWD,UED_name,L"TDF");
         CORN::VV_File TDF_file(TDF_filename_generated.c_str());
         #if (COLLATOR_VERSION==2014)
         TDF_file.set(collator.format);
         #else
         TDF_file.set(collator.target.format);                                   //200804
         #endif
      }
      else
      {
         //200803 Using CS_format file (YAML)
         assert(false); // NYI
      }
   }
   return UED_detail_lines;
}
//_process_UED______________________________________________________2014-10-17_/
}//_namespace_UED______________________________________________________________/

