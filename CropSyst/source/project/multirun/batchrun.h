#ifndef batchrunH
#define batchrunH

#include "project/multirun/multirun.h"
#ifdef OLD_GEN_SCENARIOS_DIR
//140210 now generated scenarios are created in a subdirectory of template
#include "project/scenario_generator.h"
#endif
#include "CS_suite/CropSyst/PAT_run/PAT_tally_engine.h"

// Batch run adds to multirun the tally function
// Batch run function is a standalone function
// GIS_simulation is derived from batch run.
//_Forward_declarations_________________________________________________________
class CropSyst::Project_editor_parameters;
namespace CropSyst
{
   #ifdef OLD_GEN_SCENARIOS_DIR
   class Project_directory;                                                      //140203
   #else
   class Scenario_directory;                                                     //120112
   #endif
}
class Avg_prob_map_tallier;
//______________________________________________________________________________
class CropSyst_batch_run
: public CropSyst_multirun
#ifdef OLD_GEN_SCENARIOS_DIR
, public Scenario_generator
#endif
, public PAT_tally_engine                                                        //140220
{
protected:
   const CropSyst::Project_editor_parameters              &project;
      // Will have  Carbon_sequestration_optimization_parameters
   // checking if can be const
protected:
   std::fstream error_log;
public: // Constructor
   CropSyst_batch_run
      (const CropSyst::Project_editor_parameters& _project
      #ifdef OLD_GEN_SCENARIOS_DIR
      ,const CropSyst::Project_directory        & _project_directory
      #else
      ,const CropSyst::Scenario_directory       & _scenario_directory
      #endif
      );                   //140203
public:
   virtual Simulation_run_result run();
   virtual const CORN::OS::Directory_name &get_generated_scenarios_directory_qual();
protected: // The following variables are part of the simulation run
   nat32                   run_count; // for progress display                    //060710
protected:
   virtual void clean_up_scenario(CropSyst::Scenario_directory &polygon_scenario_directory);   //050920
   virtual nat32 tally_scenarios();                                              //050629
};
//______________________________________________________________________________
#endif

