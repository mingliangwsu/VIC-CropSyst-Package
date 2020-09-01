#include "corn/spreadsheet/textual/textual_workbook.h"
namespace CORN
{
namespace textual
{
// These dummy values are only for not yet implemented methods
std::string  dummy_string;
std::wstring dummy_wstring;
//______________________________________________________________________________
Workbook::Sheet::Line &Workbook::Sheet::provide_line
(nat16 row)                                                           provision_
{  Workbook::Sheet::Line *provided_line =
      dynamic_cast<Workbook::Sheet::Line *>
      (lines.find_nat32(row));
   if (!provided_line)
   {  provided_line = new Workbook::Sheet::Line
         (row,format.columns.count(),format.detail);                             //200616
      lines.take(provided_line); // lines is Binary_tree so sorted automatically
      line_ultimate_ndx = std::max<int16>((int16)row,line_ultimate_ndx);
   }
   return *provided_line;
}
//_provide_line_________________________________________________________________
Workbook::Sheet::Field &Workbook::Sheet::Line::provide_field
(nat16 col
,nat16 start_ndx_)                                                    provision_
{  Field *provided_field = dynamic_cast<Field *>(fields_tree.find_nat32(start_ndx_));
   if (!provided_field)
   {  provided_field = new Field(start_ndx_);
      fields_tree.take(provided_field);
   }
   sat.set(col,col);
   return *provided_field;
}
//_provide_field_______________________________________________________________/
bool Workbook::Sheet::Line::write(std::ostream &strm)                 stream_IO_
{  bool first = true;
   std::string delimiter; delineation.append_delimiter(delimiter);
   FOR_EACH_IN(field,Field,fields_tree,field_iter);
   {
      if (!first)
         strm << delimiter;
      field->write(strm); // field is already quoted and width
      first = false;
   } FOR_EACH_END(field_iter)
   return true;
}
//______________________________________________________________________________
bool Workbook::Sheet::Line::is_ready(nat16 last_column_index)       affirmation_
{
   for (nat16 col = 0; col <= last_column_index/* sat.get_count()-1*/; col++)
   {
      //nat16 sat_col = satX[col];
      nat16 sat_col = sat.get(col);
      if (sat_col != col)
         return false;
   }
   return true;
}
//______________________________________________________________________________
Workbook::Sheet::Sheet
(const std::string                  &sheet_name_
,const Tabular_format               &format_                                     //200616
,std::ostream                       *stream_)
: CORN::Workbook::Sheet                                                       ()
, name                                                             (sheet_name_)
, format                                                               (format_)
, stream                                                               (stream_)
, file_stream                                                                (0)
, line_flushed_ndx                                                          (-1)
, line_ultimate_ndx                                                         (-1)
{  if (!stream_)                                                                 //190422
   {  std::string filename(sheet_name_);
      filename.append(".dat"); // may want the file extension selectable
      file_stream = new std::ofstream(filename.c_str());
      stream = file_stream;
   }
}
//______________________________________________________________________________
Workbook::Sheet::Field &Workbook::Sheet::provide_line_field
(nat32 row
,nat16 col
,nat16 start_ndx)                                                     provision_
{  Line &line = provide_line(row);
   Field &field = line.provide_field(col,start_ndx);
   return field;
}
//______________________________________________________________________________
Tabular_column *Workbook::Sheet::get_column(nat16 col_index)             const
{
   return dynamic_cast<Tabular_column *>
      (format.columns.get_at(col_index));
   // may be null
}
//______________________________________________________________________________
float64   Workbook::Sheet::set_cell_number
(nat16 col, nat32 row, float64 value,nat8 precision)
{
   /*NYI
   if row is in meta data get the meta data position
   else
   */
   {
   Tabular_column *found_column = get_column(col);
   if (found_column)
   {
      nat16 start_ndx = col;
         /*200616 columns are not simply indexed
         found_column-> number - 1;
         */
      //200616 nat16 start_ndx = found_column->number /*190325 start_number*/ - 1;
      Field &field = provide_line_field(row,col,start_ndx);
      // *found_column);
      CORN::append_float64_to_string(value,precision,field,found_column->get_field_width());
   }
   }
   return value;
}
//_set_cell_number_____________________________________________________________/
const std::string &Workbook::Sheet::set_cell_label
(nat16 col, nat32 row, const std::string &value)
{
   /*NYI
   if row is in meta data get the meta data position
   else
   */

   {
   Tabular_column *found_column = get_column(col);
   if (found_column)
   {
      nat16 start_ndx = col;
         /*200616 columns are not simply indexed
          nat16 start_ndx = found_column->number - 1;
         */
         // At this point the start_ndx is just a unique identifier 0 or greater
      Field &field = provide_line_field(row,col,start_ndx);
      nat16 field_width = found_column->get_field_width();
         // field width may be 0 for formats not using fixed width
      field.clear();                                                             //200707
      compose_formatted_string_from_cstr
         (field // field is an Item_string
         ,value.c_str()
         ,format.detail.separation.quotation                                     //200616_200220
         ,field_width);
   }
   }
   return value;
}
//_set_cell_label______________________________________________________________/
const std::wstring &Workbook::Sheet::set_cell_label_wstring
(nat16 col, nat32 row, const std::wstring &value_wstring)
{  // Currently the textual workbook is ASCII
   std::string value;
   CORN::wstring_to_string(value_wstring,value);
   set_cell_label(col,row,value);
   return value_wstring;
}
//_set_cell_label_wstring___________________________________________2019-01-13_/
sint32    Workbook::Sheet::set_cell_integer30(nat16 col, nat32 row, sint32 value)
{  std::string value_str;
   CORN::append_int32_to_string(value,value_str);
   set_cell_label(col,row,value_str);
   return value;
}
//_set_cell_integer30__________________________________________________________/
nat32 Workbook::Sheet::flush
(nat32 to_row_ndx
,nat16 expected_column_count)                                         stream_IO_
// 0 indicates all cells
{
   to_row_ndx = std::min<nat32>(to_row_ndx,line_ultimate_ndx);
   if (line_flushed_ndx < 0)                                                    //200707
       line_flushed_ndx = 0;                                                    //200707
   lines.sort();                                                                 //200707
   for (Line *pending = dynamic_cast<Line *>(lines.pop_first())
       ;pending
       ;pending = dynamic_cast<Line *>(lines.pop_first()))
   {
      // We may have blank lines to the first line needing to flush
      nat32 line_row_index = pending->row; // is index -1;                                     //200707
      while (line_flushed_ndx < line_row_index)                                  //200707
      {  line_flushed_ndx++;                                                     //200707
         (*stream) << std::endl;                                                 //200707
      }
      nat32 expect_column_index = expected_column_count - 1;
      bool ready = !expected_column_count || pending->is_ready(expect_column_index);
      if (!ready || (pending->row > (nat16)to_row_ndx))
      {  lines.prepend(pending); // relist it
         return line_flushed_ndx;
      }
      // we are either flushing everything or the line is ready
      assert(pending->row <= (nat16)to_row_ndx);
      {
         nat32 written_count = 0;
         nat16 field_ndx = 0;
         std::string delimiter;
         pending->delineation.append_delimiter(delimiter);
         //200616 Delineation::Tabulation tabulation = pending->delineation.tabulation_clad.get();
         CORN::Tabulation tabulation = pending->delineation.tabulation_clad.get();
         nat16 col_count = format.columns.count();                               //200709
         for (nat16 col_ndx = 0; col_ndx < col_count; col_ndx++)                 //200709
         /* This was a problem because the fields were not added in the
            same order as the format.columns
         for (Field *field = dynamic_cast<Field *>(pending->fields.pop_first())
             ;field
             ;field = dynamic_cast<Field *>(pending->fields.pop_first()))
         */
         {
            Field *field = dynamic_cast<Field *>(&pending->provide_field(col_ndx,col_ndx)); //200709
            if (field)  // should always have field
            {

            if (field_ndx) // don't delimit the first item
            {  (*stream) << delimiter;
               written_count += delimiter.length();
            }
            // In the case of fixed width formats, we may need to pad with spaces
            if (tabulation == CORN::fixed_TABULATION)
            {  while((nat16)written_count < field->start_ndx)
               {  (* stream) << " ";
                  written_count ++;
               }
            } else
            {  while(field_ndx < field->start_ndx) // consider blank columns
               {  std::string *blank = format.render_blank(field_ndx,pending->row);
                  if (blank)
                  {  (*stream) << blank->c_str();
                     written_count += blank->length();
                  }
                  // empty will occur when not fixed width mode
                  delete blank;
                  field_ndx ++;
               }
            }
            field->write(*stream);
            written_count += field->length();
            //200709 delete field;
            }
            field_ndx ++;
         }
         pending->fields_tree.clear();                                                //200709
         (*stream) << std::endl; // actually the format specifies the line terminator
         line_ultimate_ndx = std::max<int32>((int32)pending->row,line_ultimate_ndx);
         line_flushed_ndx ++;                                                    //200707
      }
      //200709 line_flushed_ndx = pending->row;
      delete pending;
   }
   // We may have blank lines to the first line needing to flush
/*
      while (line_ultimate_ndx < to_row_ndx)
      {  line_ultimate_ndx++;
         (*stream) << std::endl;
      }
*/
   return line_flushed_ndx;
}
//_flush____________________________________________________________2019-01-08_/
Workbook::Sheet::~Sheet()
{
   if (file_stream)
      delete file_stream; // should close the stream
   // else the stream is std in/out
}
//_sheet:destructor_________________________________________________2019-04-22_/
Workbook::~Workbook()
{  flush();
   FOR_EACH_IN(closing_sheet,Sheet,sheets,each_sheet)
         sheets.remove(closing_sheet);
   FOR_EACH_END(each_sheet)
   //sheets.clear();
}
//_Workbook::destructor_____________________________________________2019-04-26_/
Workbook::Sheet *Workbook::provide_sheet(const std::string &sheet_name) provision_
{  Sheet *provided_sheet = dynamic_cast<Sheet *>(sheets.find_string(sheet_name));
   if (!provided_sheet)
   {  provided_sheet = new Sheet
         (sheet_name,format,(sheet_name.empty() ? stream : 0));
      sheets.append(provided_sheet);
   }
   return provided_sheet;
}
//_provide_sheet_______________________________________________________________/
bool Workbook::flush()                                                stream_IO_
{  bool flushed = true;
   FOR_EACH_IN(sheet,Sheet,sheets,sheet_iter)
   {   flushed &= sheet->flush();
   } FOR_EACH_END(sheet_iter)
   return flushed;
}
//_flush____________________________________________________________2019-01-07_/
bool Workbook::close_sheet(const std::string &sheet_name)             stream_IO_
{  bool closed = true;                                                           //200616
   Sheet *closing_sheet = dynamic_cast<Sheet *>(sheets.find_string(sheet_name));
   if (closing_sheet)
   {
      closing_sheet->flush();
      closed &= sheets.remove(closing_sheet);                                    //200616
   } // else it is not an error if the sheet is already closed/nonexistant.
   return closed;  // may want to return result of flush remove
}
//_close_sheet_________________________________________________________________/
/* NYI
   save create a file for each sheet
   the filename will be composed of the workbook name
   (as a directory)
*/
}}//_namespace_CORN_textual___________________________________________________________/


