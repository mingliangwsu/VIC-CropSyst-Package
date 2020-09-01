//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "form_planting_event.h"
#include "CropSyst/source/rot_param.h"
#include "CropSyst/source/cs_event.h"
#include "corn/OS/file_system_engine.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "AdvEdBtn"
#pragma link "AdvEdit"
#pragma link "AdvFileNameEdit"
#pragma link "RNAutoDateButton"
#pragma link "RNAutoParameterEditBar"
#pragma link "RNAutoRadioGroup"
#pragma resource "*.dfm"
extern bool edit_parameter_file
(const wchar_t *filename // qualified_with_extension
,const_UnicodeZ editor_name  // without .exe extension
,const_UnicodeZ option);

Tplanting_event_form *planting_event_form;
//______________________________________________________________________________
__fastcall Tplanting_event_form::Tplanting_event_form(TComponent* Owner)
: TForm(Owner)
,sowing_event  (0)
,sowing_op     (0)
{}
//_constructor______________________________________________________2018-03-26_/
void Tplanting_event_form::bind_to
(CropSyst::Sowing_event *sowing_event_
,const CORN::OS::Directory_name *rot_file_path_                                  //190114
,nat8                    CropSyst_version)                                       //180525
{
/* this was here got get help
   parameter_form =  _parameter_form;
*/
   sowing_event   = sowing_event_;
   rot_file_path  = rot_file_path_;                                              //190114
   if (sowing_event)
   {
      sowing_op = &sowing_event->provide_sowing_operation();                     //130416
      if (sowing_op)
      {
         planting_date_button      ->bind_to(&(sowing_event->begin_sync),false /* V4_HELP_URL(HELP_P_rotation_planting_date)*/);
         radiogroup_mode  ->bind_to(&(sowing_op->sowing_date_mode_clad)                 /* V4_HELP_URL(HELP_P_rotation_sowing_date_mode)*/);

         fileedit_crop->Text = sowing_op->crop_filename.c_str();                 //180326
         DragAcceptFiles(fileedit_crop->Handle,true);                            //180326

         fileedit_management->Text = sowing_op->management_filename.c_str();    //180326
         DragAcceptFiles(fileedit_management->Handle,true);                      //180326
         edit_delay_after_precip ->bind_to(&(sowing_op->p_delay_after_precip)       /* V4_HELP_URL(HELP_P_rotation_sow_delay_after_precip)*/);
         edit_appreciable_precip ->bind_to(&(sowing_op->p_appreciable_precip_mm)    /* V4_HELP_URL(HELP_P_rotation_sow_appreciable_precip)*/);
         edit_min_required_PAW   ->bind_to(&(sowing_op->p_sowing_min_PAW)           /* V4_HELP_URL(HELP_P_rotation_sow_min_required_PAW)*/);
         edit_max_required_PAW   ->bind_to(&(sowing_op->p_sowing_max_PAW)           /* V4_HELP_URL(HELP_P_rotation_sow_max_required_PAW)*/);
         edit_typical_days_emerge->bind_to(&(sowing_op->p_expected_days_to_emerge)  /* V4_HELP_URL(HELP_P_rotation_sow_expected_days_to_emerge)*/);
         edit_temperature_days   ->bind_to(&(sowing_op->p_sowing_avg_temp)          /* V4_HELP_URL(HELP_P_rotation_sow_temperature_5_days)*/);

         edit_temperature_days->Caption =
            (CropSyst_version < 5)
               ?  L"5 day mean temperature required for sowing"
               : L"15 day mean temperature required for sowing";

         radiogroup_temperature_evaluation_mode->bind_to(&(sowing_op->sowing_date_mode_clad) /*V4_HELP_URL(HELP_P_rotation_sow_temperature_evaluation)*/);
         { // Seeding option
            edit_seeding_rate    ->bind_to(&(sowing_op->seeding_parameters.p_rate_seeds     )/*V4_HELP_URL(HELP_P_rotation_seed_application_rate)*/);
            edit_emergence_rate  ->bind_to(&(sowing_op->seeding_parameters.p_emergence_rate )/*V4_HELP_URL(HELP_P_rotation_seed_emergence_rate)*/);
            edit_sowing_depth    ->bind_to(&(sowing_op->seeding_parameters.p_sowing_depth   )/*V4_HELP_URL(HELP_P_rotation_sowing_depth)*/);
         }

//NYI dont forget to add plant_by_date 171116


      }
   }
   /* Not sure if still needed
   radiogroup_mode_OnClick(0); //180326 sowing_date_radiogroup_onclick(0);
   */
   groupbox_delay_conditions->Visible = radiogroup_mode->ItemIndex > 0;
}
//_bind_to__________________________________________________________2018-03-26_/
void __fastcall Tplanting_event_form::button_OKClick(TObject *Sender)
{  sowing_op->description.brief.assign("");
   sowing_op->description.brief.append("crop:");
   std::wstring name_buffer;                                                     //140124
   sowing_op->crop_filename.append_name_with_extension_to(name_buffer);          //140124_120625
   CORN::append_wstring_to_string(name_buffer,sowing_op->description.brief);     //140124
   sowing_op->description.brief.append(" | management:");
   sowing_op->management_filename.append_name_with_extension_to(name_buffer);    //140124_120625
   CORN::append_wstring_to_string(name_buffer,sowing_op->description.brief);
}
//_button_OKClick___________________________________________________2018-03-26_/
void __fastcall Tplanting_event_form::button_helpClick(TObject *Sender)
{
/*NYI
   CORN::OS::File_name* sowing_event_docs =
      parameter_file_form->provide_program_directory().Documentation()
      .find_document_filename_cstr("sowing_event.htm");
   if (sowing_event_docs)
   {
      CORN::OS::file_system_engine.view_file(*sowing_event_docs);
      delete sowing_event_docs;
   }
*/
}
//---------------------------------------------------------------------------
extern bool edit_parameter_file
(const wchar_t *filename // qualified_with_extension
,const_UnicodeZ editor_name  // without .exe extension
,const_UnicodeZ option);
//______________________________________________________________________________
void __fastcall Tplanting_event_form::fileedit_cropChange(TObject *Sender)
{  sowing_op->crop_filename.set_wstr(fileedit_crop->Text.c_str());
   // fileedit always selects absolete path so we only need to assign reletive if enabled
   if (checkbox_relative->Checked) reassign_paths_relative();                    //190114
}
//---------------------------------------------------------------------------
void __fastcall Tplanting_event_form::fileedit_managementChange(TObject *Sender)
{  sowing_op->management_filename.set_wstr(fileedit_management->Text.c_str());
      // fileedit always selects absolete path so we only need to assign reletive if enabled
   if (checkbox_relative->Checked) reassign_paths_relative();                    //190114
}
//---------------------------------------------------------------------------
void __fastcall Tplanting_event_form::fileedit_cropDblClick(TObject *Sender)
{  edit_parameter_file(fileedit_crop->Text.c_str(),L"crop_editor",L""); }
//---------------------------------------------------------------------------
void __fastcall Tplanting_event_form::fileedit_cropLabelClick(TObject *Sender)
{  edit_parameter_file(fileedit_crop->Text.c_str(),L"crop_editor",L""); }
//---------------------------------------------------------------------------
void __fastcall Tplanting_event_form::fileedit_managementDblClick(TObject *Sender)
{  edit_parameter_file(fileedit_management->Text.c_str(),L"management_editor",L""); }
//---------------------------------------------------------------------------
void __fastcall Tplanting_event_form::fileedit_managementLabelClick(TObject *Sender)
{  edit_parameter_file(fileedit_management->Text.c_str(),L"management_editor",L""); }
//---------------------------------------------------------------------------
void __fastcall Tplanting_event_form::radiogroup_modeClick(TObject *Sender)
{  groupbox_delay_conditions->Visible = radiogroup_mode->ItemIndex > 0; }
//---------------------------------------------------------------------------
void __fastcall Tplanting_event_form::checkbox_relativeClick(TObject *Sender)
{
   if (!rot_file_path) return;   // we should always have the path
   if (checkbox_relative->Checked)
         reassign_paths_relative();
   else  reassign_paths_absolute();
}
//---------------------------------------------------------------------------
void Tplanting_event_form::reassign_paths_relative()
{
   bool has_absolute_path;
   if (has_absolute_path = !sowing_op->crop_filename.is_qualified_relative ())
   {
      std::wstring as_relative;
         sowing_op->crop_filename.compose_relative_to
            (*rot_file_path
            ,as_relative,CORN::OS::Directory_entry_name::relative_partial_qualification);
      // delete sowing_op->crop_filename;
      sowing_op->crop_filename.set_wstr(as_relative.c_str());
      TNotifyEvent hold_onchange_event = fileedit_crop->OnChange;
      fileedit_crop->OnChange = NULL;
      fileedit_crop->Text = sowing_op->crop_filename.c_str();
      fileedit_crop->OnChange = hold_onchange_event;
      //delete as_relative;
   } // else the file is already relative do nothing
   if (has_absolute_path = !sowing_op->management_filename.is_qualified_relative())
   {
      std::wstring as_relative;
         sowing_op->management_filename.compose_relative_to
         (*rot_file_path
            ,as_relative,CORN::OS::Directory_entry_name::relative_partial_qualification);
      sowing_op->management_filename.set_wstr(as_relative.c_str());
      TNotifyEvent hold_onchange_event = fileedit_management->OnChange;
      fileedit_management->OnChange = NULL;
      fileedit_management->Text = sowing_op->management_filename.c_str();
      fileedit_management->OnChange = hold_onchange_event;
      //delete as_relative;
   } // else the file is already relative do nothing
}
//---------------------------------------------------------------------------
void Tplanting_event_form::reassign_paths_absolute()
{
   if (sowing_op->crop_filename.is_qualified_relative())
   {
      CORN::OS::Directory_entry_name *as_absolute =
         CORN::OS::file_system_engine.render_resolved_absolute_DEN
            (sowing_op->crop_filename
            ,*rot_file_path);

      sowing_op->crop_filename.set_DEN(*as_absolute);
      TNotifyEvent hold_onchange_event = fileedit_crop->OnChange;
      fileedit_crop->OnChange = NULL;
      fileedit_crop->Text = sowing_op->crop_filename.c_str();
      fileedit_crop->OnChange = hold_onchange_event;

      delete as_absolute;
   } // else it is already absolute do nothing
   if (sowing_op->management_filename.is_qualified_relative())
   {
      CORN::OS::Directory_entry_name *as_absolute =
         CORN::OS::file_system_engine.render_resolved_absolute_DEN
            (sowing_op->management_filename
            ,*rot_file_path);
      sowing_op->management_filename.set_DEN(*as_absolute);
      TNotifyEvent hold_onchange_event = fileedit_management->OnChange;
      fileedit_management->OnChange = NULL;
      fileedit_management->Text = sowing_op->management_filename.c_str();
      fileedit_management->OnChange = hold_onchange_event;
      delete as_absolute;
   } // else it is already absolute do nothing
}
//---------------------------------------------------------------------------
void __fastcall Tplanting_event_form::button_edit_cropClick(TObject *Sender)
{  edit_parameter_file(fileedit_crop->Text.c_str(),L"crop_editor",L"");
}
//---------------------------------------------------------------------------
void __fastcall Tplanting_event_form::button_edit_managementClick(TObject *Sender)
{  edit_parameter_file(fileedit_management->Text.c_str(),L"management_editor",L"");
}
//---------------------------------------------------------------------------

