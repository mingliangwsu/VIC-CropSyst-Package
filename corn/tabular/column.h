#ifndef column_h
#define column_h
#include "corn/format/field.h"
namespace CORN {
//______________________________________________________________________________
struct Delineation_sheet
{
   std::string name;    // of the spreadsheet
   nat16       column;  // number of the column
};
//_Delineation_sheet________________________________________________2020-03-03_/
struct Delineation_column
{
   provided_ Delineation_field *fixed;  // May be 0 if not fixed width
   provided_ Delineation_sheet *sheet;  // May be 0 if not in workbook
};
//_Delineation_column_______________________________________________2020-03-03_/
class Format_column
: public implements_ Item
, public extends_ Format_field
, public extends_ Delineation_column
{
 public:
   Delineation_field &provide_fixed()                          provision_
   {  if (!fixed) fixed = new Delineation_field;
      return *fixed;
   }
   Delineation_sheet &provide_sheet()                          provision_
   {  if (!sheet) sheet = new  Delineation_sheet;
      return *sheet;
   }
};
//_Format_column____________________________________________________2020-03-03_/
}//_namespace_CORN__________________________________________________2020-03-03_/
#endif

