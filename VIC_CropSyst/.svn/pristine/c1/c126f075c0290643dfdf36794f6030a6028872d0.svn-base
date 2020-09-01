//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "form_VIC_crop_test_compare_VB.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "AdvGrid"
#pragma link "AdvObj"
#pragma link "BaseGrid"
#pragma link "tmsAdvGridExcel"
#pragma link "AdvGridWorkbook"
#pragma link "AdvEdBtn"
#pragma link "AdvEdit"
#pragma link "AdvFileNameEdit"
#pragma link "Chart"
#pragma link "Series"
#pragma link "TeEngine"
#pragma link "TeeProcs"
#pragma resource "*.dfm"
TForm2 *Form2;
//---------------------------------------------------------------------------
#include "CropSyst_VB_spreadsheet_data.h"
//#include <VIC_crop_test_run_common.h>
#include "VIC_crop_test_compare_VB_common.h"
#include "VIC_crop_param.h"
#include "corn/string/strconv.hpp"
//---------------------------------------------------------------------------
__fastcall TForm2::TForm2(TComponent* Owner)
   : TForm(Owner)
   , sheets_loaded(false)
{}
//---------------------------------------------------------------------------
void __fastcall TForm2::Button1Click(TObject *Sender)
{
   //"C:\\dev\\VIC\\crop\\to_be_converted\\CropSystVB_Water July 20 2010_potato.xls"
//   AdvGridExcelIO1->XLSImport(AdvFileNameEdit1->FileName);
   std::wstring CropSystVB_filename_unicode(AdvFileNameEdit1->FileName.c_str());
   std::string  CropSystVB_filename_ANSI;
   CORN::wstring_to_string(CropSystVB_filename_unicode,CropSystVB_filename_ANSI);
   grid_daily_growth_output->LoadFromXLSSheet
      (CropSystVB_filename_ANSI.c_str()/*"C:\\dev\\VIC\\crop\\unit_tests\\CropSystVB_Water November 8 2010_B.xls"*/
      ,"Daily Growth Output");
   grid_daily_soil_output->LoadFromXLSSheet
      (CropSystVB_filename_ANSI.c_str()/*"C:\\dev\\VIC\\crop\\unit_tests\\CropSystVB_Water November 8 2010_B.xls"*/
      ,"Daily Soil Output");
   sheets_loaded = true;
}
//---------------------------------------------------------------------------
class CropSyst_VB_Spreadsheet_data_TMS_gridio
: public CropSyst_VB_Spreadsheet_data
{
public:
   void read_soil_WC(TAdvStringGrid *grid_daily_soil_output,TAdvStringGrid *string_grid_VIC_WC_start_day);
   void read_ET_VPD(TAdvStringGrid *grid_daily_soil_output);
};
//---------------------------------------------------------------------------
void CropSyst_VB_Spreadsheet_data_TMS_gridio::read_soil_WC
(TAdvStringGrid *grid_daily_soil_output
,TAdvStringGrid *string_grid_VIC_WC_start_day)
{
   bool at_end_of_sheet = false;
   for (int row = 3; !at_end_of_sheet; row++)
   {  bool at_end_of_col = false;
      if (grid_daily_soil_output->GridCells[0][row] == "")
         at_end_of_sheet = true;
      else
      {
         nat8 layer = 1;
         int32 year = grid_daily_soil_output->Ints[0][row];
         int32 doy  = grid_daily_soil_output->Ints[1][row];

         string_grid_VIC_WC_start_day->RowCount = row;

         string_grid_VIC_WC_start_day->Ints[0][row] = year;
         string_grid_VIC_WC_start_day->Ints[1][row]  = doy;
         Daily *daily = provide_daily(year, doy);
         if (daily)
         {
            for (int col = 2; !at_end_of_col; col++)
            {
               if (grid_daily_soil_output->GridCells[col][row] == "")
                  at_end_of_col = true;
               else
               {
                  float64 WC_layer = grid_daily_soil_output->Floats[col][row-1];
                  daily->WC[layer] = WC_layer;
                  string_grid_VIC_WC_start_day->RowCount = row+1;
                  string_grid_VIC_WC_start_day->ColCount = col+1;
                  string_grid_VIC_WC_start_day->Floats[col][row] = WC_layer;
               }
               layer ++;
            }
         }
      }
   }
}
//---------------------------------------------------------------------------
void CropSyst_VB_Spreadsheet_data_TMS_gridio::read_ET_VPD(TAdvStringGrid *grid_daily_growth_output)
{
   bool at_end_of_sheet = false;
   for (int row = 4; !at_end_of_sheet; row++)
   {  bool at_end_of_col = false;
      if (grid_daily_growth_output->GridCells[0][row] == "")
         at_end_of_sheet = true;
      else
      {
         nat8 layer = 1;
         int32 year = grid_daily_growth_output->Ints[0][row];
         int32 doy  = grid_daily_growth_output->Ints[1][row];
         Daily *daily = provide_daily(year, doy);
         if (daily)
         {
            float64 ETr       = grid_daily_growth_output->Floats[15][row];
            float64 VPDdaytime= grid_daily_growth_output->Floats[26][row];
            daily->VPD_daytime = VPDdaytime;
            daily->ETr = ETr;
         }
      }
   }
}
//---------------------------------------------------------------------------
#ifdef CROP_PARAMETERS_HARDCODED
extern VIC_crop::Crop_parameters_struct crop_parameters_potato;
#endif

void __fastcall TForm2::button_run_simulationClick(TObject *Sender)
{

   if (!sheets_loaded)
   {
      //   Application->MessageBox(L"First click [Load CropSyst VB]",L"not ready", MB_OK);
      //      return;
      Button1Click(Sender);
   }
   CropSyst_VB_Spreadsheet_data_TMS_gridio CropSyst_VB_data;

   CropSyst_VB_data.read_soil_WC(grid_daily_soil_output,string_grid_VIC_WC_start_day);
   CropSyst_VB_data.read_ET_VPD(grid_daily_growth_output);

   //CropSyst_VB_data
   // NYI set up the parameters from the spreadsheet crop_parameters_potato
   // Currently these are hardcoded as constants.

   float64 yield_t_ha = 0;
   std::wstring Wweather_UED_filename(filename_edit_weather_UED->FileName.c_str());
   std::string weather_UED_filename;
   CORN::wstring_to_string(Wweather_UED_filename,weather_UED_filename);
   nat16 harvest_years[100];                                                   //110512
   float64 season_yields[100];                                                   //110512
   for (int i = 0; i < 100; i++) {harvest_years[i]=0;season_yields[i]=0;}         //110512
   crop_test_run
      (weather_UED_filename.c_str()
      ,CropSyst_VB_data
      ,radiogroup_crop->ItemIndex
      ,checkbox_weather_estimate->Checked
      ,checkbox_CO2->Checked
      ,radio_applied_water->ItemIndex
      ,harvest_years,season_yields                                               //110512
      //110512 ,yield_t_ha
     );

   stringgrid_cpp_growth->Delimiter = '\t';
   stringgrid_cpp_growth->LoadFromCSV
      ("C:\\temp\\daily_growth_output.dat");
   stringgrid_cpp_WC_start_day->Delimiter = '\t';
   stringgrid_cpp_WC_start_day->LoadFromCSV
      ("C:\\temp\\daily_soil_WC_start_day_output.dat");
   stringgrid_cpp_WC_end_day->Delimiter = '\t';
   stringgrid_cpp_WC_end_day->LoadFromCSV
      ("C:\\temp\\daily_soil_WC_end_day_output.dat");
   int season_days = stringgrid_cpp_growth->RowCount;


   //Chart1->Clear();
   TLineSeries *series_VB_biomass_pot = new TLineSeries(this); Chart1->AddSeries(series_VB_biomass_pot);
      series_VB_biomass_pot->Title = "pot (VB)";
      series_VB_biomass_pot->Color = clWebGreen;
      series_VB_biomass_pot->GetVertAxis->SetMinMax(0,30);
      series_VB_biomass_pot->GetHorizAxis->SetMinMax(1,season_days);//(90,250);
   TLineSeries *series_VB_biomass_act = new TLineSeries(this); Chart1->AddSeries(series_VB_biomass_act);
      series_VB_biomass_act->Title = "act (VB)";
      series_VB_biomass_act->Color = clWebLightGreen;
      series_VB_biomass_act->GetVertAxis->SetMinMax(0,30);
      series_VB_biomass_act->GetHorizAxis->SetMinMax(1,season_days);//(90,250);
   TLineSeries *series_Cpp_biomass_pot = new TLineSeries(this);Chart1->AddSeries(series_Cpp_biomass_pot);
      series_Cpp_biomass_pot->Title = "pot (C++)";
      series_Cpp_biomass_pot->Color = clWebBlue;
      series_Cpp_biomass_pot->GetVertAxis->SetMinMax(0,30);
      series_Cpp_biomass_pot->GetHorizAxis->SetMinMax(1,season_days);//(90,250);

   TLineSeries *series_Cpp_biomass_act = new TLineSeries(this);Chart1->AddSeries(series_Cpp_biomass_act);
      series_Cpp_biomass_act->Title = "act (C++)";
      series_Cpp_biomass_act->Color = clWebLightBlue;
      series_Cpp_biomass_act->GetVertAxis->SetMinMax(0,30);
      series_Cpp_biomass_act->GetHorizAxis->SetMinMax(1,season_days);//(90,250);


   TLineSeries *series_VB_LWP =  new TLineSeries(this); Chart2->AddSeries(series_VB_LWP);
      series_VB_LWP->Title = "(VB)";
      series_VB_LWP->Color = clWebBlue;
      series_VB_LWP->GetVertAxis->SetMinMax(-1400,0);
      series_VB_LWP->GetHorizAxis->SetMinMax(1,season_days);//(90,250);


   TLineSeries *series_Cpp_LWP =  new TLineSeries(this); Chart2->AddSeries(series_Cpp_LWP);
      series_Cpp_LWP->Title = "(C++)";
      series_Cpp_LWP->Color = clWebMagenta;
      series_Cpp_LWP->GetVertAxis->SetMinMax(-1400,0);
      series_Cpp_LWP->GetHorizAxis->SetMinMax(1,season_days);//(90,250);


//   int doy = 0;
   int dos = 0;

   for (int row = 4; row < season_days+4 /*110404 365*/; row++)
   {
      //doy = grid_daily_growth_output->Ints[1][row];
      dos = grid_daily_growth_output->Ints[3][row];
//      if (doy >= 93 && doy <= 240)
      if (dos >= 1 && dos <= season_days)
      {
      float VB_biomass_pot = grid_daily_growth_output->Floats[6][row];
      series_VB_biomass_pot->AddXY(dos/*doy*/,VB_biomass_pot);
      float VB_biomass_act = grid_daily_growth_output->Floats[7][row];
      series_VB_biomass_act->AddXY(dos/*doy*/,VB_biomass_act);

      float VB_LWP = grid_daily_growth_output->Floats[13][row];
      series_VB_LWP->AddXY(dos/*doy*/,VB_LWP);
      }
   }

   int doy = 0;
   int rowcount =  stringgrid_cpp_growth->RowCount;
   for (int row = 4; row < rowcount-4 ; row++)
   {
//      doy = stringgrid_cpp_growth->Ints[1][row];
      dos = grid_daily_growth_output->Ints[3][row];

//      if (doy >= 93 && doy <= 240)
      if (dos >= 1 && dos <= season_days)
      {
      String floats_6_row = stringgrid_cpp_growth->Cells[6][row];
      float Cpp_biomass_pot = stringgrid_cpp_growth->Floats[6][row];
      series_Cpp_biomass_pot->AddXY(dos,Cpp_biomass_pot);
      float Cpp_biomass_act = stringgrid_cpp_growth->Floats[7][row];
      series_Cpp_biomass_act->AddXY(dos,Cpp_biomass_act);

      float Cpp_LWP =   stringgrid_cpp_growth->Floats[13][row];
      series_Cpp_LWP->AddXY(dos,Cpp_LWP);
      }
   }

//   char yield_cstr[10];
//   ftoa(yield_t_ha,yield_cstr);
//   edit_yield->Text =CORN_float64_to_str(yield_t_ha,  2);
   listbox_yields->Clear();
   for (int i = 0; (i < 100) && harvest_years[i]; i++)
   {
      char num_buff[20];
      CORN::int16_to_cstr(harvest_years[i],num_buff,10);
      std::string year_yield(num_buff);
      year_yield.append(" ");
      year_yield.append( CORN::float64_to_cstr(season_yields[i], 2));
      listbox_yields->AddItem(year_yield.c_str(),0);
   }
}
//---------------------------------------------------------------------------



