#include "corn/spreadsheet/Excel_OLE/workbook_Excel_OLE.h"
#include "corn/string/strconv.hpp"
#  include "corn/OS/file_system_engine.h"
#if (__BCPLUSPLUS__ < 0x0570)
//101229 identifiers were renamed at some time after BC++5
// They are different in BDS and so far (at least RadStudio2010) they have not been modified.
#define get_Range Get_Range
#define get_Sheets Get_Sheets
#define get_Name   Get_Name
#endif

namespace CORN {
namespace Excel_OLE {
//______________________________________________________________________________
bool Workbook::save_as(const std::wstring &filename,bool display_alerts ) performs_IO_
{  return save_as_format
   (filename
   ,0xFFFFEFD1 // Excel_2k::xlWorkbookNormal
   ,display_alerts);
}
//_save_as__________________________________________________________2011-12-15_/
Workbook::Sheet::Sheet()
: CORN::Workbook::Sheet()
{}
//_Sheet:constructor________________________________________________2019-10-16_/
char *Workbook::Sheet::cell_name(nat16 col_0, nat32 row_0,char *name) const
{  int col_1 = col_0 + 1;
   int row_1 = row_0 + 1;
   int k = 0;
   if (col_1 > 26)
   {  name[k++] = 64 + (col_0/26); name[k] = 0;
      name[k++] = 64 + (col_0%26)+1; name[k] = 0;
   } else
      name[k++] = 64 + col_1; name[k] = 0;

//std::clog << col_1 << " cell:" << name << std::endl;

   std::sprintf(name+k,"%d",row_1);
   return name;
}
//_Sheet::cell_name____________________________________________________________/
bool Workbook::Sheet::clear_cell(nat16 col_0, nat32 row_0)
{ char cell_coordinate[20];
   cell_name(col_0,row_0,cell_coordinate);
   clear_range(cell_coordinate);
   return true;
}
//_clear_cell_______________________________________________________2011-08-31_/
bool Workbook::Sheet::set_cell_bool
   (nat16 col_0, nat32 row_0, bool value)
{
   char cell_coordinate[20];
   cell_name(col_0,row_0,cell_coordinate);
   return set_range_bool(cell_coordinate, value);                                //110713
}
//_set_cell_bool____________________________________________________2011-09-22_/
int32 Workbook::Sheet::set_cell_integer30
(nat16 col_0, nat32 row_0, int32 value)
{  char cell_coordinate[20];
   cell_name(col_0,row_0,cell_coordinate);
   return set_range_integer30(cell_coordinate, value);                           //110713
}
//_set_cell_integer30__________________________________________________________/
float64     Workbook::Sheet::set_cell_number
(nat16 col_0, nat32 row_0, float64 value,nat8 precision)
{  char cell_coordinate[20];
   cell_name(col_0,row_0,cell_coordinate);
   return set_range_number(cell_coordinate,value, precision);                    //110713
}
//_set_cell_number_____________________________________________________________/
const std::string &Workbook::Sheet::set_cell_label
(nat16 col_0, nat32 row_0, const std::string &value)
{  char cell_coordinate[20];
   cell_name(col_0,row_0,cell_coordinate);
   return set_range_label(cell_coordinate, value);                               //110713
}
//_set_cell_label______________________________________________________________/
const std::wstring &Workbook::Sheet::set_cell_label_wstring
(nat16 col_0, nat32 row_0, const std::wstring &value)
{  char cell_coordinate[20];
   cell_name(col_0,row_0,cell_coordinate);
   return set_range_label_wstring(cell_coordinate, value);
}
//_set_cell_label_wstring___________________________________________2014-01-02_/
sint32 Workbook::Sheet::set_cell_date
(nat16 col_0, nat32 row_0, const CORN::Date_const &date_value)

{  char cell_coordinate[20];
   cell_name(col_0,row_0,cell_coordinate);
   return set_range_date(cell_coordinate,date_value);                            //110713
}
//_set_cell_date_______________________________________________________________/
bool Workbook::Sheet::set_cell_color(nat16 col_0, nat32 row_0,nat8 red,nat8 green, nat8 blue)
{  char cell_coordinate[20];
   cell_name(col_0,row_0,cell_coordinate);
   return set_range_color(cell_coordinate, red, green,  blue);
}
//_set_cell_color___________________________________________________2011-08-15_/
const std::string &Workbook::Sheet::set_cell_formula
(nat16 col_0, nat32 row_0, const std::string &formula)                               //110815
{  char cell_coordinate[20];
   cell_name(col_0,row_0,cell_coordinate);
   return set_range_formula(cell_coordinate,formula);
}
//_set_cell_formula_________________________________________________2011-08-15_/
int32 Workbook::Sheet::get_cell_integer30
(nat16 col_0, nat32 row_0)                                                 const
{  char cell_coordinate[20];
   cell_name(col_0,row_0,cell_coordinate);
   return get_range_integer30(cell_coordinate);                                  //111001
}
//_get_cell_integer30__________________________________________________________/
bool Workbook::Sheet::get_cell_bool
(nat16 col_0, nat32 row_0)                                                 const //130506
{  char cell_coordinate[20];
   cell_name(col_0,row_0,cell_coordinate);
   return get_range_bool(cell_coordinate);
}
//_get_cell_bool____________________________________________________2013-03-28_/
float64 Workbook::Sheet::get_cell_number
(nat16 col_0, nat32 row_0)                                                 const
{  char cell_coordinate[20];
   cell_name(col_0,row_0,cell_coordinate);
   return get_range_number(cell_coordinate);
}
//_get_cell_number_____________________________________________________________/
const std::wstring &Workbook::Sheet::get_cell_label
(nat16 col_0, nat32 row_0,std::wstring &label)                             const
{ char cell_coordinate[20];
  cell_name(col_0,row_0,cell_coordinate);
  return get_range_label(cell_coordinate,label);
}
//_get_cell_label______________________________________________________________/
// Sheet attributes:
void Workbook::Sheet::freeze_pane
(nat16 left_most_column_0,nat32 top_most_row_0 )
{
//  Not yet implemented
}
//_freeze_pane__________________________________________________________________
}}//_namespace_CORN_Excel_OLE__________________________________________________/

