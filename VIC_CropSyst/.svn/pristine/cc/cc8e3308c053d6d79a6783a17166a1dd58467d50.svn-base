//---------------------------------------------------------------------------

#ifndef CropSyst_VB_spreadsheet_dataH
#define CropSyst_VB_spreadsheet_dataH
//---------------------------------------------------------------------------
#include <corn/container/enumlist.h>
class CropSyst_VB_Spreadsheet_data
{
public:
   class Daily
   : public CORN::Enumeration
   {
   public:
      nat32 date;
      // From Daily Soil Output
      float32 WC[31];   // m3/m3
      float32 drainage;  //mm

      // From daily Growth Output
      float32 ETr;         //mm
      float32 VPD_daytime; // kPa?
   public :
      Daily(nat32 _date);
      virtual nat32 get_key_nat32()                                        const { return date;} ;
   };
public:
   CORN::Enumeration_list daily_output;

   Daily *daily;

public:
   Daily *provide_daily(int32 for_year, int32 for_doy);                    // provides

   CropSyst_VB_Spreadsheet_data()
   : daily_output()
   , daily              (0)
   {};


};

#endif
