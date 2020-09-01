//#include <UED/library/std_codes.h>
#include "VIC_convertor.h"

#include "corn/data_source/command_options_datasrc.h"
#include "corn/OS/file_system_logical.h"

namespace CORN { namespace OS {
File_system &file_system() { return CORN::OS::file_system_logical; }
}}

int main(int argc,const char *argv[])
{
   // Command line: "H:\Climate Change Impact\agroclimatic\weather\UED_import.rsp"
   int result = 0;

   std::cout << "Warning,  Currently the VIC convertor assumes the data are withing the span of years from 1918." << std::endl
    << "This is because our test data was in this period without any date information stored in the file." << std::endl
    << "Hopefully this will be corrected in the future." << std::endl;

   CORN::Command_options_data_source args_DS(argc,argv);
   UED::Convertor_arguments arguments;
   args_DS.get(arguments);
   VIC_convertor convertor(arguments);
   //200329 (argc,argv);

//NYI   if (argv[1] is not the response file name
//NYI   {    then presume it is file to import
//NYI             convertor.parameters.target_filename.set(argv[1]);
//NYI         result = perform_import()
//NYI   } else
      result =  convertor.convert();
//may need to reimplement   convertor.notify_climgen();
   return result;
}

