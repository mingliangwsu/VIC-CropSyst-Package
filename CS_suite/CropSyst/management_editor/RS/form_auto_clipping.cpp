//______________________________________________________________________________
#include <vcl.h>
#include "CropSyst/source/management/management_param_V5.h"
#pragma hdrstop
#include "CropSyst/GUI/help/management.h"
#include "GUI/parameter/form_param_file.h"
#include "form_auto_clipping.h"
#include "form_clipping2.h"
//______________________________________________________________________________
#pragma package(smart_init)
#pragma link "RNAutoRadioGroup"
#pragma link "RNAutoParameterEditBar"
#pragma link "RNAutoParameterEditBar"
#pragma link "RNAutoCheckBox"
#pragma resource "*.dfm"
Tauto_clipping_form *auto_clipping_form;
//______________________________________________________________________________
__fastcall Tauto_clipping_form::Tauto_clipping_form(TComponent* Owner)
   : TForm(Owner)
{}
//______________________________________________________________________________
void __fastcall Tauto_clipping_form::bind_to
(CropSyst::Auto_clip_biomass_mode   *auto_clipping_mode_
,Tparameter_file_form               *parameter_file_form_)
{
   parameter_file_form = parameter_file_form_;
   auto_clipping_mode = auto_clipping_mode_;
   auto_clipping_mode->update_considerations();

   Tclipping_form *common_clipping_form = new Tclipping_form(this);
   common_clipping_form->bind_to(auto_clipping_mode_,parameter_file_form);
   common_clipping_form->Parent = operation_tabsheet;
   common_clipping_form->Show();
   common_clipping_form->Top = 0;
   common_clipping_form->Left = 0;
   common_clipping_form->BorderStyle = bsNone;

   adjust_rate_clip_edit      ->bind_to(&(auto_clipping_mode->p_adjust_relative_growth_rate_for_clipping));
   LAI_forces_clip_edit       ->bind_to(&(auto_clipping_mode->p_LAI_forces_clipping)                     );
   biomass_forces_clip_edit   ->bind_to(&(auto_clipping_mode->p_biomass_forces_clipping)                 );
   flowering_forces_clip_edit ->bind_to(&(auto_clipping_mode->p_flowering_forces_clipping)               );

   adjust_rate_clip_checkbox     ->bind_to(&(auto_clipping_mode->consider_adjust_rate) );
   LAI_forces_clip_checkbox      ->bind_to(&(auto_clipping_mode->consider_LAI)         );
   biomass_forces_clip_checkbox  ->bind_to(&(auto_clipping_mode->consider_biomass)     );
   flowering_forces_clip_checkbox->bind_to(&(auto_clipping_mode->consider_flowering)   );
   show_hide_controls(0);
}
//______________________________________________________________________________
void __fastcall Tauto_clipping_form::show_hide_controls(TObject *Sender)
{  auto_clipping_mode->set_considerations();
   adjust_rate_clip_edit->Visible      = auto_clipping_mode->consider_adjust_rate;
   LAI_forces_clip_edit->Visible       = auto_clipping_mode->consider_LAI;
   biomass_forces_clip_edit->Visible   = auto_clipping_mode->consider_biomass;
   flowering_forces_clip_edit->Visible = auto_clipping_mode->consider_flowering;
   Update();
}
//______________________________________________________________________________
void __fastcall Tauto_clipping_form::Update(void)
{
   adjust_rate_clip_edit      ->Update();
   LAI_forces_clip_edit       ->Update();
   biomass_forces_clip_edit   ->Update();
   flowering_forces_clip_edit ->Update();
}
//______________________________________________________________________________

