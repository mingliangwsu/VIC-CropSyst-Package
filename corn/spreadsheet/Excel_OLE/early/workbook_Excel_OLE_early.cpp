#include "corn/spreadsheet/Excel_OLE/early/workbook_Excel_OLE_early.h"

#include "corn/string/strconv.hpp"
#  include "corn/OS/file_system_engine.h"
#if (__BCPLUSPLUS__ < 0x0570)
// They are different in BDS and so far (at least RadStudio2010)
// they have not been modified.
#define get_Range Get_Range
#define get_Sheets Get_Sheets
#define get_Name   Get_Name

#define SETUP_CELLS(RANGE_OR_COORDINATE) \
   Excel_NS::RangePtr cells;                           \
   cells=worksheet->Get_Range(RANGE_OR_COORDINATE,TNoParam());
#else

#define SETUP_CELLS(RANGE_OR_COORDINATE) \
   Excel_NS::ExcelRangePtr cells;                       \
   cells=worksheet->get_Range(TVariant(RANGE_OR_COORDINATE),TNoParam());
#endif
namespace CORN {
using namespace OS;
namespace Excel_OLE {
namespace earlybinding {
//______________________________________________________________________________
TExcelApplication *global_application = 0;
bool application_start(bool minimize)
{
   global_application = new TExcelApplication(0);
   global_application->AutoQuit = true;                                          //200122
   int lcid = LOCALE_USER_DEFAULT;
   global_application->ConnectKind = ckNewInstance;
   global_application->Connect();
   global_application->set_Visible(lcid, true);
   global_application->Connect();
   global_application->Visible[0] = true;
   if (minimize) global_application->set_WindowState
      (lcid,0xFFFFEFD4); //xlMinimized
   return true;
}
//_application_start________________________________________________2019-10-18_/
bool application_stop()
{
   bool stopped = false;                                                         //200122
   if (global_application)                                                       //200122
   {
      int lcid = LOCALE_USER_DEFAULT;
      global_application->set_ScreenUpdating(lcid,true);
      global_application->Disconnect();
      global_application->Quit();
      //no need to delete global_application because it is a VCL component
      stopped = true;
   }
   return stopped;                                                               //200122
}
//_application_stop_________________________________________________2019-10-18_/
Workbook::Workbook
(const File_name&  workbook_filename_XLS_
// usually fully qualified but may be relative to current directory
,TComponent*            owner_)
: filename_XLS                                          (workbook_filename_XLS_)
{  int lcid = LOCALE_USER_DEFAULT;
   owner = owner_;
   if (!global_application)                                                      //191018
   {
      application_start(false);                                                  //191018
      // Ideally the application would be started external to the workbook,
      // but this is a convenience if one forgets to start the application.
      // Note that the application is not stopped by the workbook object
      // because we may want to open/close multiple workbooks and we dont
      // want to quit the application when we close one of the workbooks
   }
   workbook = new TExcelWorkbook(NULL);
   workbook->ConnectKind = ckAttachToInterface;                                  //110704
   if (file_system_engine.exists(filename_XLS))                                  //130905
   {
      UnicodeString workbook_name_unicode(filename_XLS.w_str());
      workbook->ConnectTo
         (global_application->Workbooks->Open
            (StringToOleStr(workbook_name_unicode)
         ,TNoParam()
         ,Variant(false), TNoParam(), TNoParam(), TNoParam()
         ,TNoParam(), TNoParam(), TNoParam(), Variant(false)
         ,TNoParam(), TNoParam(), TNoParam(), lcid));
   } else
   workbook->ConnectTo(global_application->Workbooks->Add());
}
//_Workbook:constructor________________________________________________________/
bool Workbook::set_updating(bool enabled)                          contribution_
{  int lcid = LOCALE_USER_DEFAULT;
   global_application->set_ScreenUpdating(lcid,enabled);
   return enabled;
}
//_set_updating_____________________________________________________2019-10-17_/
Workbook::~Workbook()
{
   close();                                                                      //191018
   workbook->Disconnect();
   delete workbook;
}
//______________________________________________________________________________
const std::string &Workbook::get_name(std::string &name) const
{  // I would actually like to return the workbook name property (I don't know what it is)
//   static AnsiString name;
   #if (__BCPLUSPLUS__ >= 0x0620)
      CORN::wstring_to_string(workbook->Name,name);
   #else
      name = workbook->Name;
   #endif
   return name;
}
//______________________________________________________________________________
bool Workbook::save(bool display_alerts)     performs_IO_
{  int lcid = LOCALE_USER_DEFAULT;
   if (!global_application) return false;                                        //191018
   TVariant workbook_filename(filename_XLS.c_str());
   global_application->DisplayAlerts[lcid] = display_alerts;                     //110918
   if (file_system_engine.exists(filename_XLS))                                  //130905_110712
        workbook->Save(LOCALE_SYSTEM_DEFAULT);                                   //110712
   else workbook->SaveAs
         (workbook_filename
         ,TNoParam(),TNoParam(),TNoParam(),TNoParam(),TNoParam()
         ,1/*xlNoChange*/,TVariant(2)/*xlLocalSessionChanges*/
         ,TNoParam(),TNoParam(),TNoParam(),LOCALE_SYSTEM_DEFAULT);
   workbook->Saved[lcid] = true;                                                 //110918
   global_application->DisplayAlerts[lcid] = true;                               //110920
   return true;
}
//_save_____________________________________________________________2011-01-05_/
// NYI save_as
// NYI flush
//______________________________________________________________________________
Workbook::Sheet *Workbook::provide_sheet
(const std::string &sheet_name)                                       provision_ //110711
{
   // Sets the specified sheet as active (Create if necessary).
   // For spreadsheet formats that only support single sheets,
   // this may be the name of the file for the sheet.
   // Returns a pointer to the sheet.
   // The sheet is owned by the workbook.
   std::string valid_sheet_name(sheet_name);                                     //190107
   if (valid_sheet_name.empty()) valid_sheet_name.assign("Sheet 1");             //190107
   bool sheet_just_added_to_workbook = true;
   Workbook::Sheet * sheet = (Workbook::Sheet *)sheets.find_string(valid_sheet_name);
   if (!sheet)
   {  TExcelWorksheet *new_worksheet = new TExcelWorksheet(owner);
      // I actually want to connect to a named sheet
      long num_sheets =  workbook->get_Sheets()->get_Count();
      bool sheet_exists = false;
      for (int i_based = 1; (i_based <= num_sheets) && !sheet_exists; i_based++)
      {
         #if (__BCPLUSPLUS__ < 0x0570)
         new_worksheet->ConnectTo(workbook->get_Sheets()->get_Item(i_based));
         #else
         //101229, not sure if this is still needed
         Variant item_index(i_based);
         new_worksheet->ConnectTo(workbook->Worksheets->get_Item(item_index));
         #endif
         AnsiString existing_sheet_name;
         existing_sheet_name = new_worksheet->get_Name();
         if (existing_sheet_name == valid_sheet_name.c_str())
            sheet_exists = true;
      }
      if (!sheet_exists)
      {
         //160823 long new_sheet_index_1based = 1; // It appears excel adds new sheets at index 1 (STUPID IDEA!)
         //  workbook->Get_Sheets()->get_Count() +1;
         Excel_NS::_WorksheetPtr added_worksheet=workbook->get_Sheets()->Add();  //160823
         // Note: I think Add returns the sheet index, it could return the sheet, I DONT KNOW!
         #if (__BCPLUSPLUS__ < 0x0570)
         VARIANT vsheet_name;
         // Now  set the sheet tab name to the sheet name
         vsheet_name = valid_sheet_name.c_str();
         new_worksheet->Name = vsheet_name;
         #else
         new_worksheet->ConnectTo(added_worksheet);                              //160823
         VARIANT vsheet_name (valid_sheet_name.c_str());
         // Now  set the sheet tab name to the sheet name
         new_worksheet->Name = vsheet_name;
         #endif
         sheet_just_added_to_workbook = true;
      }
      // This works for a given sheet number: new_worksheet->ConnectTo(workbook->Get_Sheets()->get_Item(1));
      sheet = new Sheet(new_worksheet, sheet_just_added_to_workbook);
      sheets.append(sheet);
   }
   return sheet;
}
//_provide_sheet_______________________________________________________________/
Workbook::Sheet *Workbook::provide_sheet(const std::wstring &sheet_name) provision_
{  std::string sheet_name_str;
   CORN::wstring_to_string(sheet_name,sheet_name_str);
   return provide_sheet(sheet_name_str);
}
//_provide_sheet(wide)______________________________________________2020-07-13_/

Workbook::Sheet *Workbook::copy_sheet
(const std::string &from_sheet_name
,const std::string &to_sheet_name)                                 modification_
{
   Workbook::Sheet *from_sheet = (Workbook::Sheet *)sheets.find_string(from_sheet_name);
   Workbook::Sheet *to_sheet = (Workbook::Sheet *)sheets.find_string(to_sheet_name);
   bool copied =
         (from_sheet != NULL) && (from_sheet->worksheet != NULL)
      && (to_sheet == NULL);
   if (copied)
   {  TExcelWorksheet *from_excel_worksheet = from_sheet->worksheet;
      TExcelWorksheet *to_excel_worksheet = to_sheet->worksheet;
      int lcid = LOCALE_USER_DEFAULT;
      /* NYI
         need to select range to cut and paste
      */
   }
   return to_sheet;                                                              //190803
}
//_copy_sheet_______________________________________________________2019-08-13_/
//NYI close sheet
//_____________________________________________________________________________/
bool Workbook::save_as_format
(const std::wstring &filename,XlFileFormat format,bool display_alerts) performs_IO_
{
   if (!global_application) return false;                                        //191018
   int lcid = LOCALE_USER_DEFAULT;
   std::string workbook_filename_as_string;                                      //140101
   CORN::wstring_to_string(filename,workbook_filename_as_string);                //140101
   TVariant workbook_filename(workbook_filename_as_string.c_str());              //140101
   global_application->DisplayAlerts[lcid] = display_alerts;                     //110918
   TVariant V_format = format;
   workbook->SaveAs
         (workbook_filename
         ,V_format,TNoParam(),TNoParam(),TNoParam(),TNoParam()
         ,1/*xlNoChange*/,TVariant(2)/*xlLocalSessionChanges*/
         ,TNoParam(),TNoParam(),TNoParam(),LOCALE_SYSTEM_DEFAULT);
   workbook->Saved[lcid] = true;                                                 //110918
   global_application->DisplayAlerts[lcid] = true;                               //110920
   return true;
}
//_save_as_format___________________________________________________2011-11-15_/
Workbook::Sheet::Sheet
(TExcelWorksheet *excelworksheet_
,bool sheet_just_added_to_workbook_)
: CORN::Excel_OLE::Workbook::Sheet                                            () //190803
, worksheet                                                    (excelworksheet_)
, just_added_to_workbook                         (sheet_just_added_to_workbook_)
{}
//_Worksheet:constructor_______________________________________________________/
Workbook::Sheet::~Sheet()
{  if (worksheet)
   {  worksheet->Disconnect();
      delete worksheet;
   }
   worksheet = 0;
}
//_Worksheet:destructor________________________________________________________/
bool Workbook::Sheet::clear_range(const char *range)
{  SETUP_CELLS(range);
   cells->Clear();
   return true;
}
//_clear_range______________________________________________________2011-08-31_/
const VARIANT &Workbook::Sheet::set_range_Variant
(const char *cell_coordinate
,VARIANT &value)                                                  const
{  SETUP_CELLS(cell_coordinate);
   worksheet->Activate();
   cells->Activate();
   //Put an int in the activated cell
   Variant intValue;
   intValue=value;
   cells->set_Value(intValue);
   return value;
}
//_set_range_Variant________________________________________________2019-10-16_/
int32 Workbook::Sheet::set_range_integer30
(const char *cell_coordinate, int32 value)
{  VARIANT VarValue = value;
   set_range_Variant(cell_coordinate,VarValue);
   return value;
}
//_set_range_integer30______________________________________________2019-10-16_/
bool Workbook::Sheet::set_range_bool
(const char *cell_coordinate, bool value)
{  VARIANT VarValue = value;
   set_range_Variant(cell_coordinate,VarValue);
   return value;
}
//_set_range_bool______________________________________________________________/
float64 Workbook::Sheet::set_range_number
(const char *cell_coordinate, float64 value,uint8 precision)
{  VARIANT VarValue = value;
   set_range_Variant(cell_coordinate,VarValue);
   return value;
}
//_set_range_number____________________________________________________________/
const std::string &Workbook::Sheet::set_range_label
(const char *cell_coordinate, const std::string &value)
{  VARIANT VarValue;
   #if (__BCPLUSPLUS__ < 0x0570)
   VarValue=value;
   #else
   VarValue = value.c_str();
   #endif
   set_range_Variant(cell_coordinate,VarValue);
   return value;
}
//_set_range_label_____________________________________________________________/
const std::wstring &Workbook::Sheet::set_range_label_wstring
(const char *cell_coordinate, const std::wstring &value)
{  VARIANT VarValue;
   #if (__BCPLUSPLUS__ < 0x0570)
   VarValue=value;
   #else
    VarValue = value.c_str();
   #endif
   set_range_Variant(cell_coordinate,VarValue);
   return value;
}
//_set_range_label_wstring__________________________________________2013-09-05_/
int32 Workbook::Sheet::set_range_date
(const char *range, const CORN::Date_const &date_value  )
{  Year year; Month month; DOM dom;
   date_value.gregorian(year,month,dom);
   TDateTime builder_datevalue((unsigned short )year
      ,(unsigned short )month,(unsigned short)dom);
   #if (__BCPLUSPLUS__ < 0x0570)
   VARIANT dateValue;
   dateValue = builder_datevalue;
   #else
   TVariant dateValue(builder_datevalue);
   #endif
   SETUP_CELLS(range);
   worksheet->Activate();
   cells->Activate();
   cells->set_Value(dateValue);
   return date_value.get_date32();
}
//_set_range_date______________________________________________________________/
bool Workbook::Sheet::set_range_color
(const char *range,nat8 red,nat8 green, nat8 blue)
{  SETUP_CELLS(range);
   //NYI   cells->Interior->Color = RGB(red,green,blue);
   cells->get_Interior()->set_Color(TVariant(RGB(red,green,blue)));
   return true;
}
//_2011-08-15___________________________________________________________________
const std::string &Workbook::Sheet::set_range_formula
(const char *range, const std::string &formula)
{  SETUP_CELLS(range);
   VARIANT strFormula;
   #if (__BCPLUSPLUS__ < 0x0570)
   strFormula=formula;
   #else
   strFormula = formula.c_str();
   #endif
   cells->set_Formula(strFormula);
   return formula;
}
//_set_range_formula________________________________________________2011-08-15_/
const VARIANT &Workbook::Sheet::get_range_Variant
(const char *cell_coordinate,VARIANT &value)                               const
{  SETUP_CELLS(cell_coordinate);
   worksheet->Activate();
   cells->Activate();
   //Put a number in the activated cell
   value = cells->get_Value();
   return value;
}
//_get_range_VariantInParam_________________________________________2019-10-16_/
float64 Workbook::Sheet::get_range_number
(const char *cell_coordinate)                                              const
{  VARIANT VarientValue;
   float64 value = get_range_Variant(cell_coordinate,VarientValue);
   return value;
}
//_get_range_number_____________________________________2019-10-16__2011-09-14_/
int32 Workbook::Sheet::get_range_integer30
(const char *cell_coordinate)                                              const
{  VARIANT VarientValue;
   int32 value = get_range_Variant(cell_coordinate,VarientValue);
   return value;
}
//_get_range_integer30__________________________________2019-10-16__2011-11-01_/
bool Workbook::Sheet::get_range_bool
(const char *cell_coordinate)                                              const
{  VARIANT VarientValue;
   bool value = get_range_Variant(cell_coordinate,VarientValue);
   return value;
}
//_get_range_bool_______________________________________2019-10-16__2013-03-28_/

const std::wstring &Workbook::Sheet::get_range_label
(const char *cell_coordinate,std::wstring &label)                          const
{  VARIANT VarientValue;
   const wchar_t *value = get_range_Variant(cell_coordinate,VarientValue);
   label.assign(value);
   return label;
}
//_get_range_label______________________________________2019-10-16__2011-10-01_/
// Sheet attributes:
void Workbook::Sheet::freeze_pane
(nat16 left_most_column,nat32 top_most_row )
{
//  Not yet implemented
}
//_freeze_pane__________________________________________________________________
const std::string &Workbook::Sheet::get_name
(std::string &name)                                                        const
{  // I would actually like to return the workbook name property
   // (I don't know what it is)
   #if (__BCPLUSPLUS__ >= 0x0620)
   CORN::wstring_to_string(worksheet->Name,name);
   #else
   name = worksheet->Name;
   #endif
   return name.c_str();
}
//______________________________________________________________________________
bool Workbook::Sheet::is_key_string(const std::string &key)         affirmation_
{
   #if (__BCPLUSPLUS__ >= 0x0620)
   std::string sheet_name;
   CORN::wstring_to_string(worksheet->Name,sheet_name);
   return sheet_name == key;
   #else
   return worksheet->Name == key;
   #endif
}
//_is_key_string____________________________________________________2019-07-30_/
bool Workbook::close()                                                stream_IO_
{  int lcid = LOCALE_USER_DEFAULT;
   VARIANT unused_filename;
   VARIANT unused_routeworkbook;
   //VARIANT save_changes = false;
   workbook->Close(Variant(false),TNoParam(),TNoParam(),lcid);
   return true;
}
//______________________________________________________________________________
bool Workbook::run_macro(const std::string &macro_name)
{
assert(false); // NYI
/* Will be something like this
   excelapplication->Run(macro_name.c_str())
*/
   return true;
}
//_run_macro________________________________________________________2019-08-03_/
}}}//_namespace CORN Excel_OLE earlybinding____________________________________/

