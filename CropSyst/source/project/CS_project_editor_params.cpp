#include "project/CS_project_editor_params.h"
#include "CS_suite/application/CS_suite_directory.h"
namespace CropSyst
{
//______________________________________________________________________________
Project_editor_parameters::Project_editor_parameters()
: CropSyst_project()
,cell_width(1.0)                                                                 //990525
,subsurface_flow(false)                                                          //990604
,CPF_water(false)                                                                //051229
,minimize_runtime_window(false)                                                  //051229
{
   for (int i = 0; i < GS_MAX_IDS; i++)
   {  GS_probability_5[i] = 0;
      GS_probability_10[i]= 0;
      GS_statistics[i]    = 0;
   }
   for (int i = 0; i < MAX_YLY_entries; i++)
   {  ANNL_probability_5[i] = 0;
      ANNL_probability_10[i]= 0;
      ANNL_statistics[i]    = 0;
   }
}
//_Project_editor_parameters________________________________________2005-02-04_/
bool Project_editor_parameters::set_start()
{
//    The statistics and probablity selection are packed in a single nat32 for storage in the file
//    0000 pppp pppp ppPP PPPP PPPP SSSS SSSS
//    S statistic, P probility 10, p probability 5
   for (int i = 0; i < GS_MAX_IDS; i++)
      GS_statistic_selection[i] = (((nat32)GS_probability_5[i]) << 22) | (((nat32)GS_probability_10[i]) << 12) | ((nat32)GS_statistics[i]);
   for (int i = 0; i < MAX_YLY_entries; i++)
      ANNL_statistic_selection[i] = (((nat32)ANNL_probability_5[i]) << 22) | (((nat32)ANNL_probability_10[i]) << 12) | ((nat32)ANNL_statistics[i]);
   return CropSyst_project::set_start();                                         //161023
}
//_set_start________________________________________________________2005-02-04_/
bool Project_editor_parameters::get_end()
{
//    The statistics and probablity selection are packed in a single nat32 for storage in the file
//     ppppppppppPPPPPPPPPP00SSSSSSSSSS
#define        _11111111110000000000000000000000   0xFFC00000
#define        _00000000001111111111000000000000   0x003FF000
#define        _00000000000000000000001111111111   0x000003FF
//    S statistic, P probility 10, p probability 5

#     if (__BCPLUSPLUS__ >= 0x0550)
// 060720 For some  unknown reason this is trashing the report format filename (in the main CropSyst model, but not the project editor).
   for (int i = 0; i < GS_MAX_IDS; i++)
   {
      GS_probability_5[i] = (GS_statistic_selection[i] & _11111111110000000000000000000000) >> 22;
      GS_probability_10[i]= (GS_statistic_selection[i] & _00000000001111111111000000000000) >> 12;
      GS_statistics[i]    = (GS_statistic_selection[i] & _00000000000000000000001111111111);
   }
   for (int i = 0; i < MAX_YLY_entries; i++)
   {
      ANNL_probability_5[i] = (ANNL_statistic_selection[i] & _11111111110000000000000000000000) >> 22;
      ANNL_probability_10[i]= (ANNL_statistic_selection[i] & _00000000001111111111000000000000) >> 12;
      ANNL_statistics[i]    = (ANNL_statistic_selection[i] & _00000000000000000000001111111111);
   }
#endif
   return CropSyst_project::get_end();                                           //161023
}
//_get_end__________________________________________________________2005-02-04_/
bool Project_editor_parameters::expect_structure(bool for_write)
{
   bool expected = CropSyst_project::expect_structure(for_write);                                //020328
   structure_defined = false;                                                    //120314
   set_current_section(LABEL_project);                                           //021012
   set_current_section("crop_production_function");                              //030517
      expect_bool("water",CPF_water,CORN::VV_bool_entry::form_true );            //030517
   set_current_section(LABEL_GIS);                                               //990525
      expect_bool(LABEL_subsurface_flow,subsurface_flow,CORN::VV_bool_entry::form_true );  //990604
   // not needed for LADSS
   carbon_sequestration_optimization_parameters.setup_structure(*this,for_write);//040810
   expected &= GIS.setup_structure(*this,for_write);                             //170808_050202
   structure_defined = true;                                                     //120314
   return expected;
}
//_expect_structure____________________________________________________________/
}//_namespace CropSyst_________________________________________________________/

