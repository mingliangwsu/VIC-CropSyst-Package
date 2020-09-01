181215 This editor is obsolete now CS explorer handle scenario generation and colation of output
is done with the collator.
Delete this unit after 2021

#ifndef CS_project_editor_parametersH
#define CS_project_editor_parametersH
//______________________________________________________________________________
#include "corn/OS/filtered_filename.h"
#  include "project/optimization/carbon_sequestration/carbon_sequestration_param.h"
#  include "project/GIS/GIS_params.h"

#include "rptoptns.h"
namespace CropSyst
{
//______________________________________________________________________________
class Project_editor_parameters
: public extends_ CropSyst_project
{
 public: // GIS mode info  (I think for watershed only)
   float32  cell_width;      // Watershed model
   bool     subsurface_flow;                                                     //990604
   bool     watershed_runoff_enabled;                                            //020329
   bool           CPF_water;                                                     //030517
   bool           minimize_runtime_window;                                       //040609
   FXiltered_file_name   report_generation_filename_rel;  // check where used, may be obsolete 070425
 public:
   // The statistics and probablity selections are unpacked for used in the program
   // These arrays are not stored in the file; instead,
   // they are packed into GS_statistic_selection ANNL_statistic_selection which are stored.
   nat32 GS_statistics       [GS_MAX_IDS];
   nat32 GS_probability_10   [GS_MAX_IDS];
   nat32 GS_probability_5    [GS_MAX_IDS];
   nat32 ANNL_statistics     [MAX_YLY_entries];
   nat32 ANNL_probability_10 [MAX_YLY_entries];
   nat32 ANNL_probability_5  [MAX_YLY_entries];
 public:
   GIS_Project_parameters  GIS;                                                  //140306_050202
   Carbon_sequestration_optimization_parameters carbon_sequestration_optimization_parameters; //040810
 public: // 'structors
   Project_editor_parameters();
 public: // data record overrides
   virtual bool set_start();                                                     //161023_010109
   virtual bool get_end() ;                                                      //161023_010109
   virtual bool expect_structure(bool for_write);
};
//_Project_editor_parameters________________________________________2007-08-21_/
}//_namespace CropSyst_________________________________________________________/
#endif

