#include "corn/format/spreadsheet/biff.h"

void main()
{
   BIFF234_File test_xls("test.xls",0,0,10,10);

   test_xls.write_label_cell(1,1,Spreadsheet_abstract::CSS_justify_left,"A");
   test_xls.write_label_cell(1,2,Spreadsheet_abstract::CSS_justify_center,"B");
   test_xls.write_label_cell(1,3,Spreadsheet_abstract::CSS_justify_right,"C");

   test_xls.write_integer_cell(2,1,1);
   test_xls.write_integer_cell(2,2,2);
   test_xls.write_integer_cell(2,3,3);

   test_xls.write_number_cell(3,1,1.1,1);
   test_xls.write_number_cell(3,2,2.2,2);
   test_xls.write_number_cell(3,3,3.3,3);

   // Next sheet
   test_xls.write_BOF_record();
   test_xls.write_dimensions(0,0,10,10);

   test_xls.write_integer_cell(2,1,1);
   test_xls.write_integer_cell(2,2,2);
   test_xls.write_integer_cell(2,3,3);
}

