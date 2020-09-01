#include "UED/convert/convert_response.h"
#include "import_export_engine.h"
#include "corn/OS/file_system_logical.h"
#include "corn/data_source/command_options_datasrc.h"
#include "UED/collate/collate_engine.h"
namespace CORN { namespace OS {
File_system &file_system() { return CORN::OS::file_system_logical; }
}}
//______________________________________________________________________________
int main (int argc, const char *argv[])
{
   CORN::Command_options_data_source command_line_options(argc,argv);            //170616
   UED::Arguments_collator  arguments;                                           //190418
   // Collate_engine::Arguments arguments;                                          //170616
   UED::Convertor_arguments parameters;
   command_line_options.get(parameters);                                         //170616
   bool geolocation_encountered_not_used=false;
   return UED::Quality_import_export_engine(parameters).process();
}
//_2014-06-14___________________________________________________________________