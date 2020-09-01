#define VERBOSE
//200817 #include "arguments_CropSyst.h"
#include "CropSyst/source/CropSyst_engine_V5.h"

//______________________________________________________________________________
/*200325
namespace CropSyst {
CORN::Date_clad_32 no_date(0);                                                   //190620
}
//______________________________________________________________________________
*/
bool quick_and_dirty_disable_chem_balance_when_watertable = false;               //150424
namespace CORN { namespace OS {
File_system &file_system() { return CORN::OS::file_system_logical; }
}}
//______________________________________________________________________________
int main(int argc, const char *argv[])
{
   CORN::Command_options_data_source command_line_options(argc,argv);            //160908
   CS::Identification ID_unused(0);                                              //200815
   CS::Arguments arguments(ID_unused,L".CropSyst_scenario");                     //200815
   CORN::global_arguments = &arguments;                                          //200815
   command_line_options.get(arguments);                                          //200815

   CS::Simulation_run_result run_result = CS::RUN_SUCCESS;
   CropSyst::version5::Simulation_engine simulation
      (arguments);                                                               //200817
      //200817 (command_line_options);       //160513
   run_result =  simulation.run();
   return run_result;                                                            //040806
}
//______________________________________________________________________________

