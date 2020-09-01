#include "corn/spreadsheet/Excel_OLE/late/workbook_Excel_OLE_late.h"

// The late binding may not be fully implemented
// (I usually use early binding)

namespace CORN {
namespace Excel_OLE {
namespace latebinding {
//______________________________________________________________________________//______________________________________________________________________________
Workbook::Workbook()
{
   Variant vMSExcel;
   vMSExcel = Variant::CreateObject("Excel.Application");
   vMSExcel.OlePropertySet("Visible", true);

   Variant vXLWorkbooks;
   vXLWorkbooks = vMSExcel.OlePropertyGet("Workbooks");
   vXLWorkbook = vXLWorkbooks.OleFunction("Add");
}
//______________________________________________________________________________
Workbook::~Workbook()
{
   Variant vSaveChanges, vFileName;
   vSaveChanges = true;
   char filename[255];
   strcpy(filename,name.c_str());
   strcat(filename,".xls");
   vFileName = filename;
   vXLWorkbook.OleFunction("Close", vSaveChanges, vFileName);
}
//______________________________________________________________________________
const std::string &Workbook::get_name(std::string &return_name) const
{  // I would actually like to return the workbook name property (I don't know what it is)
   return_name.assign(name.c_str());
   return return_name;
}
//______________________________________________________________________________
// NYI save
// NYI save_as
// NYI flush
//______________________________________________________________________________
Workbook::Sheet *Workbook::provide_sheet(const std::string &sheet_name) provision_
{
   std::string valid_sheet_name(sheet_name);                                     //190107
   if (valid_sheet_name.empty())                                                 //190107
      valid_sheet_name = "Sheet 1";                                              //190107

   // Sets the specified sheet as active (Create if necessary).
   // For spreadsheet formats that only support single sheets,
   // this may be the name of the file for the sheet.
   // Returns a pointer to the sheet.
   // The sheet is owned by the workbook.

   Workbook::Sheet *sheet=(Workbook::Sheet *)sheets.find_string(valid_sheet_name/*.c_str()*/); //190803
   if (!sheet)
   {
      Variant vWorksheet;
      Variant vSheetName;
      vSheetName = valid_sheet_name.c_str();
      try
      {
         vWorksheet = vXLWorkbook.
            OlePropertyGet("Worksheets").OlePropertyGet("Item",vSheetName);         //190803
      }
      catch (Exception &exception)
      {
         vWorksheet = vXLWorkbook.OleFunction("Add");
         vWorksheet.OlePropertySet("Name",valid_sheet_name);
      }
      sheet = new Worksheet(vWorksheet,valid_sheet_name);
      sheets.append(sheet);
   }
   return sheet;
}
//______________________________________________________________________________
//NYI copy sheet
//NYI close_sheet
//______________________________________________________________________________
Workbook::Sheet::Sheet(Variant &ivWorksheet,const char *i_name)
: vWorksheet(ivWorksheet)
, name(i_name)
{}
//______________________________________________________________________________
#define CELL_SET                                                               \
   Variant vRange;                                                             \
   vRange = cell_name(col,row);                                                \
   Variant vCell;                                                              \
   vCell = vWorksheet.OlePropertyGet("Range",vRange);                          \
   Variant vValue;                                                             \
   vValue = value;                                                             \
   vCell.OlePropertySet("Value",vValue);
//______________________________________________________________________________
sint16 Workbook::Sheet::set_cell_integer
(nat16 col, nat32 row, sint16 value)
{
   CELL_SET
   return value;
}
//______________________________________________________________________________
float64     Workbook::Sheet::set_cell_number
(nat16 col, nat32 row, float64 value)
{
   CELL_SET
   return value;
}
//______________________________________________________________________________
const std::string &Workbook::Sheet::set_cell_label
(nat16 col, nat32 row, const std::string &value)
{
   CELL_SET
   return value;
}
//______________________________________________________________________________
      // The following are for getting data in the sheet
#define CELL_GET                                                               \
   Variant vRange;                                                             \
   vRange = cell_name(col,row);                                                \
   Variant vCell;                                                              \
   vCell = vWorksheet.OlePropertyGet("Range",vRange);                          \
   Variant vValue;                                                             \
   vCell.OlePropertyGet("Value",vValue);

sint16 Workbook::Sheet::get_cell_integer
(nat16 col, nat32 row)
{
   CELL_GET
   return vValue;
}
//______________________________________________________________________________
float64     Workbook::Sheet::get_cell_number
(nat16 col, nat32 row)
{
   CELL_GET
   return vValue;

}
//______________________________________________________________________________
const std::wstring &Workbook::Sheet::get_cell_label
(nat16 col, nat32 row,std::wstring &label)                                 const
{
   CELL_GET
   static AnsiString result;
   result = vValue;
   CORN::append_ASCIIZ_to_wstring(result.c_str(),label);                         //190803
   return label;                                                                 //190803
   //190803 return result.c_str();
}
//______________________________________________________________________________
// Sheet attributes:
void Workbook::Sheet::freeze_pane(uint16 left_most_column,uint32 top_most_row )
{
//NYI
}
//______________________________________________________________________________
const std::string &Workbook::Sheet::get_name
(std::string &sheet_name)                                                  const
{  sheet_name = name;
   return sheet_name;
}
//______________________________________________________________________________
/*not yet needed
uint8       Workbook::Sheet::get_index() // the sheet tab index (0 based).
{

}
*/

}}}//_namespace CORN Excel_OLE latebinding_____________________________________/
