#ifndef form_VIC_crop_test_compare_VBH
#define form_VIC_crop_test_compare_VBH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "AdvGrid.hpp"
#include "AdvObj.hpp"
#include "BaseGrid.hpp"
#include "tmsAdvGridExcel.hpp"
#include <ExtCtrls.hpp>
#include <Grids.hpp>
#include "AdvGridWorkbook.hpp"
#include "AdvEdBtn.hpp"
#include "AdvEdit.hpp"
#include "AdvFileNameEdit.hpp"
#include <ComCtrls.hpp>
#include "Chart.hpp"
#include "Series.hpp"
#include "TeEngine.hpp"
#include "TeeProcs.hpp"
//---------------------------------------------------------------------------
class TForm2 : public TForm
{
__published: // IDE-managed Components
   TAdvGridExcelIO *AdvGridExcelIO1;
   TPanel *Panel1;
   TPageControl *PageControl1;
   TTabSheet *tabsheet_VB_growth;
   TTabSheet *tabsheet_VB_soil;
   TAdvStringGrid *grid_daily_soil_output;
   TAdvStringGrid *grid_daily_growth_output;
   TPageControl *PageControl2;
   TTabSheet *tabsheet_cpp_growth;
   TTabSheet *tabsheet_cpp_WC_start_day;
   TAdvStringGrid *stringgrid_cpp_growth;
   TAdvStringGrid *stringgrid_cpp_WC_start_day;
   TTabSheet *tabsheet_cpp_WC_end_day;
   TTabSheet *tabsheet_VIC_WC;
   TAdvStringGrid *stringgrid_cpp_WC_end_day;
   TAdvStringGrid *string_grid_VIC_WC_start_day;
   TTabSheet *TabSheet1;
   TChart *Chart1;
   TTabSheet *TabSheet2;
   TChart *Chart2;
   TRadioGroup *radio_applied_water;
   TRadioGroup *radiogroup_crop;
   TPanel *Panel2;
   TAdvFileNameEdit *AdvFileNameEdit1;
   TAdvFileNameEdit *filename_edit_weather_UED;
   TCheckBox *checkbox_weather_estimate;
   TPanel *Panel3;
   TButton *Button1;
   TButton *button_run_simulation;
   TListBox *listbox_yields;
   TCheckBox *checkbox_CO2;
   void __fastcall Button1Click(TObject *Sender);
   void __fastcall button_run_simulationClick(TObject *Sender);
private: // User declarations
   bool sheets_loaded;
public: // User declarations
   __fastcall TForm2(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm2 *Form2;
//---------------------------------------------------------------------------
#endif

