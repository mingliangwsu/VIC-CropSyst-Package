//---------------------------------------------------------------------------

#pragma hdrstop

#include "CropSyst_VB_spreadsheet_data.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

CropSyst_VB_Spreadsheet_data::Daily::Daily(nat32 _date)
: date(_date)
, drainage(0)
, ETr(0)
, VPD_daytime(0)
{
   for (int i = 0; i < 31; i++)
       WC[i] = 0;

};
//______________________________________________________________________________
CropSyst_VB_Spreadsheet_data::Daily *CropSyst_VB_Spreadsheet_data::provide_daily(int32 for_year, int32 for_doy)
{
   nat32 curr_date = for_year * 1000 + for_doy;
   CropSyst_VB_Spreadsheet_data::Daily *provided_daily =
      dynamic_cast<CropSyst_VB_Spreadsheet_data::Daily *>(daily_output.find(curr_date));
   if (!provided_daily)
   {  provided_daily = new CropSyst_VB_Spreadsheet_data::Daily(curr_date);
      daily_output.append(provided_daily);
   };
   return provided_daily;
};
//______________________________________________________________________________

