//______________________________________________________________________________
#include <vcl.h>
#include "CropSyst/source/mgmt_param.h"
#include "corn/math/moremath.h"
#include "common/biomatter/decomposition_const.h"
#include "GUI/parameter/form_param_file.h"
#pragma hdrstop
#include "form_organic_N_appl.h"
//______________________________________________________________________________
#pragma package(smart_init)
#pragma link "AutoCheckBox"
#pragma link "AutoFloat32EditBar"
#pragma link "AutoInt16EditBar"
#pragma link "AutoRadioGroup"
#pragma link "AutoFloat32Edit"
#pragma resource "*.dfm"
Torganic_N_application_form *organic_N_application_form;
//______________________________________________________________________________
__fastcall Torganic_N_application_form::Torganic_N_application_form(TComponent* Owner)
   : TForm(Owner)
   , operation(0)
   , est_org_drymatter(0)
{}
//______________________________________________________________________________
void Torganic_N_application_form::bind_to
(CropSyst::Organic_nitrogen_operation  *_operation
,Tparameter_file_form                  *_parameter_file_form)
{  operation = _operation;
   parameter_file_form = _parameter_file_form;
}
//______________________________________________________________________________
void Torganic_N_application_form::show_hide_controls()
{}
//______________________________________________________________________________
void __fastcall Torganic_N_application_form::NH3_volatil_loss_check_onclick(TObject *Sender)
{  show_hide_controls();
}
//______________________________________________________________________________
void __fastcall Torganic_N_application_form::org_N_decomp_time_checkClick(TObject *Sender)
{  show_hide_controls();
}
//______________________________________________________________________________
void __fastcall Torganic_N_application_form::NRCS_org_N_field_op_number_combboxChange(TObject *Sender)
{}
//______________________________________________________________________________
void __fastcall Torganic_N_application_form::est_org_drymatter_outChange(TObject *Sender)
{}
//______________________________________________________________________________
void __fastcall Torganic_N_application_form::org_N_amount_editExit(TObject *Sender)
{}
//______________________________________________________________________________
void __fastcall Torganic_N_application_form::manure_type_radiogroupClick(TObject *Sender)
{}
//______________________________________________________________________________
void __fastcall Torganic_N_application_form::button_NRCS_helpClick(TObject *Sender)
{  parameter_file_form->view_manual("NRCS_field_operation.htm");
}
//---------------------------------------------------------------------------

