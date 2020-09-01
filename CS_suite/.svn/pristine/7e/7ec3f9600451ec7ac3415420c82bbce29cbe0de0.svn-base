//______________________________________________________________________________
#include <vcl.h>
#include "CropSyst/source/management/management_param_V5.h"
#pragma hdrstop
#include "form_irrig_appl.h"
#include "CropSyst/GUI/help/management.h"
#include "GUI/parameter/form_param_file.h"
//______________________________________________________________________________
#pragma package(smart_init)
#pragma link "RNAutoParameterEditBar"
#pragma link "RNAutoRadioGroup"
#pragma link "RNAutoCheckBox"
#pragma resource "*.dfm"
TIrrigation_application_form *Irrigation_application_form;
//______________________________________________________________________________
__fastcall TIrrigation_application_form::TIrrigation_application_form(TComponent* Owner)
   : TForm(Owner)
{}
//______________________________________________________________________________
void __fastcall TIrrigation_application_form::bind_to
(CropSyst::Irrigation_operation_class *irrig_op_                                 //170710
,CropSyst::Irrigation_operation_parameters_numeric *irrig_op_num_                //170710
,bool                   _auto_irrig_mode
, Contingency_clad     *_contigency_clad
,Tparameter_file_form  *parameter_file_form)
{
   irrig_op = irrig_op_;                                                         //170710
   irrig_op_num = irrig_op_num_;                                                 //170710

   contigency_clad = _contigency_clad;
   auto_irrig_mode = _auto_irrig_mode;
#define NO_HELP_YET 0
   { // auto irrig parameters now provided here as well
      radiogroup_application_mode      ->bind_to(&(irrig_op->application_mode_cowl_or_labeled)); //cs_suite_paths->get_cropsyst_parameter_help(HELP_P_irrigation_xxx).c_str());
      edit_max_allowable_depletion     ->bind_to(&(irrig_op_num->p_max_allowable_depletion)/*,parameter_file_form->compose_manual_subdirectory_filename_cstr(HELP_P_mgmt_irrig_max_allow_depl)*/);

      radiobutton_depletion_observation_depth_mode->bind_to(&(irrig_op->depletion_observation_depth_mode_cowl_or_labeled));
      edit_depletion_observe_depth  ->bind_to(&(irrig_op_num->p_depletion_observe_depth)           /* ,parameter_file_form->compose_manual_subdirectory_filename_cstr(HELP_P_mgmt_irrig_obs_depth_depl)*/);
      edit_depletion_obs_fract_root_depth->bind_to(&(irrig_op_num->p_depletion_observe_root_zone_fract) /* ,parameter_file_form->compose_manual_subdirectory_filename_cstr(HELP_P_mgmt_irrig_obs_depth_depl)*/);

      edit_net_irrigation_mult   ->bind_to(&(irrig_op_num->p_net_irrigation_mult)    /*,parameter_file_form->compose_manual_subdirectory_filename_cstr(HELP_P_mgmt_irrig_net_multiplier)*/);
      edit_refill_point          ->bind_to(&(irrig_op_num->p_refill_point)           /*,parameter_file_form->compose_manual_subdirectory_filename_cstr(HELP_P_irrigation_xxx).c_str()*/);
      edit_max_application       ->bind_to(&(irrig_op_num->p_max_application)        /*,parameter_file_form->compose_manual_subdirectory_filename_cstr(HELP_P_mgmt_irrig_max_appl)*/);
      edit_min_application       ->bind_to(&(irrig_op_num->p_min_application)        /*,parameter_file_form->compose_manual_subdirectory_filename_cstr(HELP_P_mgmt_irrig_min_appl)*/);
      #if ((DIRECTED_IRRIGATION>=2014) && (DIRECTED_IRRIGATION<=2016))
      xxxx
      #endif
      #if (DIRECTED_IRRIGATION==2020)
            edit_wetting_fraction            ->bind_to(&(irrig_op_num->p_wetting_area_surface));
      #endif
   };
   edit_amount    ->bind_to(&(irrig_op_num->p_amount)                                   /*,parameter_file_form->compose_manual_subdirectory_filename(HELP_P_mgmt_irrig_specific).c_str()*/);
   edit_salinity  ->bind_to(&(irrig_op_num->p_salinity)                                 /*,parameter_file_form->compose_manual_subdirectory_filename_cstr(HELP_P_mgmt_irrig_salinity)*/);
   edit_NO3_N_conc   ->bind_to(&(irrig_op_num->p_NO3_N_concentration)                      /*,parameter_file_form->compose_manual_subdirectory_filename_cstr(HELP_P_mgmt_irrig_salinity)*/);
   edit_NH4_N_conc   ->bind_to(&(irrig_op_num->p_NH4_N_concentration)                      /*,parameter_file_form->compose_manual_subdirectory_filename_cstr(HELP_P_mgmt_irrig_salinity)*/);
   radiogroup_consideration    ->bind_to(&(irrig_op->consideration_mode_cowl_or_labeled)       /*,parameter_file_form->compose_manual_subdirectory_filename_cstr(HELP_P_mgmt_irrig_consideration_mode)*/);
   edit_leaf_water_potential->bind_to(&(irrig_op_num->p_leaf_water_potential)           /*,parameter_file_form->compose_manual_subdirectory_filename_cstr(HELP_P_mgmt_irrig_leaf_water_potential)*/);
//NYI   fixed_periodic = &_fixed_periodic;
   show_hide_controls();
}
//______________________________________________________________________________
void __fastcall TIrrigation_application_form::application_mode_radiogroup_onclick(TObject *Sender)
{
   show_hide_controls();
}
//______________________________________________________________________________
void  TIrrigation_application_form::show_hide_controls()
{
   if (!irrig_op) return;
   tabsheet_application_conditions->TabVisible = auto_irrig_mode; // NYI  | fixed_periodic;

   bool consideration_leaf_water_potential_mode = (irrig_op->consideration_mode_cowl_or_labeled.get() == consider_leaf_water_potential);
   groupbox_depletion_constraints->Visible = !consideration_leaf_water_potential_mode;
   edit_leaf_water_potential->Visible = consideration_leaf_water_potential_mode;
   if (consideration_leaf_water_potential_mode)
   {
   } else
   {  // Soil water depletion consideration mode
      bool soil_cond_mode =  (irrig_op->application_mode_cowl_or_labeled.get() == irrigate_soil_condition);

      groupbox_depletion_constraints->Visible = soil_cond_mode || auto_irrig_mode;
      groupbox_depletion_constraints->Visible
         = auto_irrig_mode
            || (contigency_clad && (contigency_clad->get() == POSTPONE_CONTINGENCY));
      switch (radiobutton_depletion_observation_depth_mode->ItemIndex) // irrig_op->get_depletion_observation_depth_mode())
      {  case constant_fixed_depth_mode:
         {
            edit_depletion_observe_depth->Caption = "Depletion observation depth";
            edit_depletion_observe_depth->Visible = true;
            edit_depletion_obs_fract_root_depth->Visible = false;
         }break;
         case fraction_of_root_depth_mode :
         {
            edit_depletion_observe_depth->Caption = "Depletion observation depth when no crop";
            edit_depletion_observe_depth->Visible = true;
            edit_depletion_obs_fract_root_depth->Visible = true;
         }break;
         case soil_profile_depth_mode :
         {  edit_depletion_observe_depth->Visible = false;
            edit_depletion_obs_fract_root_depth->Visible = false;
         }break;
      }; // switch depletion depth mode
   }; // end else soil water depletion consideraton model
   bool fixed_amount_mode =  irrig_op->application_mode_cowl_or_labeled.get() == irrigate_fixed_amount;
   panel_refill->Visible = !fixed_amount_mode;
   edit_amount->Visible = fixed_amount_mode;
//   if (auto_irrig_mode)
      panel_application_mode->Visible = true;
}
//______________________________________________________________________________
void __fastcall TIrrigation_application_form::radiobutton_depletion_observation_depth_modeClick(TObject *Sender)
{
   radiobutton_depletion_observation_depth_mode->Update();
   show_hide_controls();
}
//______________________________________________________________________________
void __fastcall TIrrigation_application_form::radiobutton_depletion_observation_depth_modeExit(TObject *Sender)
{
   show_hide_controls();
}
//______________________________________________________________________________
void __fastcall TIrrigation_application_form::radiogroup_considerationClick(TObject *Sender)
{
//   radiogroup_consideration->Update();
   show_hide_controls();   
}
//---------------------------------------------------------------------------


