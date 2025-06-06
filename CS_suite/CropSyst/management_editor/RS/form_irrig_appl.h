//---------------------------------------------------------------------------

#ifndef form_irrig_applH
#define form_irrig_applH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include "RNAutoParameterEditBar.h"
#include "RNAutoRadioGroup.h"
#include "RNAutoCheckBox.h"
#include <ComCtrls.hpp>
#include "event_types.h"
//______________________________________________________________________________
namespace CropSyst
{
//170710 class Irrigation_operation;
class Irrigation_operation_class;                                                //170710
class Irrigation_operation_parameters_numeric;                                   //170710
}
class Tparameter_file_form;
//______________________________________________________________________________
class TIrrigation_application_form : public TForm
{
__published:	// IDE-managed Components
   TPageControl *PageControl1;
   TTabSheet *tabsheet_application_conditions;
   TTabSheet *TabSheet2;
   TPanel *panel_consideration_mode;
   TLabel *Label2;
   TRNAutoRadioGroup *radiogroup_consideration;
   TGroupBox *groupbox_depletion_constraints;
   TRNAutoParameterEditBar *edit_max_allowable_depletion;
   TRNAutoParameterEditBar *edit_depletion_obs_fract_root_depth;
   TRNAutoParameterEditBar *edit_depletion_observe_depth;
   TPanel *panel_observation_depth_model;
   TLabel *depletion_contigency_note;
   TRNAutoRadioGroup *radiobutton_depletion_observation_depth_mode;
   TRNAutoParameterEditBar *edit_leaf_water_potential;
   TPanel *panel_application_mode;
   TLabel *note_depletion_constraints;
   TRNAutoRadioGroup *radiogroup_application_mode;
   TRNAutoParameterEditBar *edit_amount;
   TPanel *panel_refill;
   TRNAutoParameterEditBar *edit_net_irrigation_mult;
   TRNAutoParameterEditBar *edit_refill_point;
   TTabSheet *TabSheet3;
   TRNAutoParameterEditBar *edit_salinity;
   TRNAutoParameterEditBar *edit_NO3_N_conc;
   TRNAutoParameterEditBar *edit_NH4_N_conc;
   TLabel *label_not_appl_0;
   TLabel *label_app_cond_auto;
   TRNAutoParameterEditBar *edit_wetting_fraction;
   TGroupBox *groupbox_system_capacity;
   TRNAutoParameterEditBar *edit_max_application;
   TRNAutoParameterEditBar *edit_min_application;
   void __fastcall application_mode_radiogroup_onclick(TObject *Sender);
   void __fastcall radiobutton_depletion_observation_depth_modeClick(TObject *Sender);
   void __fastcall radiobutton_depletion_observation_depth_modeExit(TObject *Sender);
   void __fastcall radiogroup_considerationClick(TObject *Sender);
private: // User declarations
   CropSyst::Irrigation_operation_class *irrig_op;                               //170710
   CropSyst::Irrigation_operation_parameters_numeric *irrig_op_num;        //170710
   bool                  auto_irrig_mode;
//NYI   bool                  *fixed_periodic;
   Contingency_clad *contigency_clad;   // may be 0 (I.e. for auto irrig mode)
public: // User declarations
   __fastcall TIrrigation_application_form(TComponent* Owner);
      void __fastcall bind_to
         (CropSyst::Irrigation_operation_class *irrig_op                        //170710
         ,CropSyst::Irrigation_operation_parameters_numeric *irrig_op_num        //170710
         ,bool                    auto_irrig_mode_
         ,Contingency_clad       *contigency_clad_
         ,Tparameter_file_form   *parameter_file_form_);
      void  show_hide_controls();
};
//---------------------------------------------------------------------------
extern PACKAGE TIrrigation_application_form *Irrigation_application_form;
//---------------------------------------------------------------------------
#endif
