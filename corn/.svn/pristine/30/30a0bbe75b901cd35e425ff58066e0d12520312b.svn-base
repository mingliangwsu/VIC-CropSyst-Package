
#include "corn/format/spreadsheet/WKS_file.h"

#define VAR_LEN = -1;
//___________________________________________________________________________
Common_spreadsheet_file::Record_type WKS_file::read_next(Common_spreadsheet_file::Record_type filter_type)
{
      Record_type result = filter_type;
         // NYI  This needs to be implemented as in biff.cpp
      return result;
};
//___________________________________________________________________________
int16 WKS_file::read_record()
{  switch (read_rec_header())                                                    //981016
   { case WKS_CODE_INTEGER :  read_integer_cell();                       break;
     case WKS_CODE_NUMBER :   read_number_cell();                        break;
     case WKS_CODE_LABEL :    read_label_cell(/*length-6*/);             break;  //981018
     case WKS_CODE_COLW1 :    data_type = not_data; read_column_width(); break;
     case WKS_CODE_MARGINS :  data_type = not_data; read_margins();      break;
     case WKS_CODE_HEADER:    data_type = not_data; read_header(length); break;
     case WKS_CODE_PRANGE:    data_type = not_data; read_print_range();  break;
     default :  // Don't know what this record is ignore it
     { // We also don't do anything with:
       // WKS_CODE_BOF
       // WKS_CODE_EOF
       if (rec_code == WKS_CODE_EOF)
          at_eof = 1;
       data_type = not_data;                                                     //981016
       read_unknown_record_data();                                               //981016
     }
     break;
   }
  return rec_code;                                                               //981016
}
//_read_record_________________________________________________________________/
void WKS_file::write_BOF_record()
{  write_rec_header(WKS_CODE_BOF,WKS_LEN_BOF);
   write_uint16(0x0404); // { Lotus file format}
}
//_write_BOF_record_________________________________________________1998-10-16_/
void WKS_file::write_EOF_record()
{  write_rec_header(WKS_CODE_EOF,WKS_LEN_EOF);
}
//_write_EOF_record_________________________________________________1998-10-16_/
bool WKS_file::read_BOF_record()
{  read_rec_header();
   WKS_version =read_uint16();                                                    //001206
   return (rec_code == WKS_CODE_BOF);
}
//___________________________________________________________________________
/* not implemented yet
void WKS_output_file::write_window1
( int16 cursor_col,
  int16 cursor_row,
  byte format,
  int16 col_width,
  int16 cols_screen,
  int16 rows_screen,
  int16 col_left,
  int16 row_top,
  int16 cols_title,
  int16 rows_title,
  int16 col_title,
  int16 row_title,
  int16 col_width_border,
  int16 row_width_border,
  int16 win_width,
)
{
   write_rec_header(outfile,WKS_CODE_WINDOW1,WKS_LEN_WINDOW1);

   put_int16(cursor_col,outfile);
   put_int16(cursor_row,outfile);
   write_byte(format,outfile);
   write_byte(0,outfile);
   put_int16(col_width,outfile);
   put_int16(cols_screen,outfile);
   put_int16(rows_screen,outfile);
   put_int16(col_left,outfile);
   put_int16(row_top,outfile);
   put_int16(cols_title,outfile);
   put_int16(rows_title,outfile);
   put_int16(col_title,outfile);
   put_int16(row_title,outfile);
   put_int16(col_width_border,outfile);
   put_int16(row_width_border,outfile);
   put_int16(win_width,outfile);
   write_byte(0,outfile);
}
//_write_window1_______________________________________________________________/
void WKS_output_file::write_col_width_win1(int16 column,int16 width)
{
   byte width_byte;

   write_rec_header(WKS_CODE_COLW1,WKS_LEN_COLW1);
   put_int16(column,outfile);
   width_byte := width;
   write_byte(width_byte,outfile);
}
//_write_col_width_win1________________________________________________________/
*/
void  WKS_file::read_header(int16 length)
{  read((char *)&header,length);                                                 //001205
}
//_read_header_________________________________________________________________/
void  WKS_file::read_margins()
{  read((char *)&margins,sizeof(Margins_struct));                                //001205
}
//_read_margins________________________________________________________________/
void  WKS_file::read_print_range()
{  print_range_enabled = 1;
   read((char *)&print_range,sizeof(Print_range_struct));                        //001205
}
//_read_print_range____________________________________________________________/
void WKS_file::write_column_width(int16 col, int16 width)
{  write_rec_header(WKS_CODE_COLW1,WKS_LEN_COLW1);
   write_sint16(col);                                                            //981016
   write_uint8(width);                                                           //981016
}
//_write_column_width__________________________________________________________/
void  WKS_file::read_column_width()
{  int16 col = read_sint16();                                                    //981016
   column_widths[col] = read_uint8();                                            //981016
}
//_read_column_width___________________________________________________________/
void  WKS_file::read_cell()
{  byte format = read_uint8();
   extract_format(format);
   col = read_sint16();
   row = read_sint16();
};
//_read_cell________________________________________________________2000-12-06_/
void  WKS_file::read_integer_cell(bool expect_header)
{  read_cell();                                                                  //001206
   int_value = read_sint16();                                                    //001206
   data_type = type_integer;
   Common_spreadsheet_file::read_integer_cell(expect_header);
}
//_read_integer_cell___________________________________________________________/
void WKS_file::write_cell(int16 col, int16 row,uint8 format)
{  write_uint8(format);
   write_sint16(col);
   write_sint16(row);
}
//_write_cell_______________________________________________________2000-12-06_/
void WKS_file::write_label_cell ( int16 col, int16 row, CSS_justifications alignment_,const CORN_string &value_)
{  Common_spreadsheet_file::write_label_cell(col,row,alignment_,value_);
   char value[255];
   strcpy(value,value_.c_str());
   int16 len = strlen(value);
   write_rec_header(WKS_CODE_LABEL,5+len+2);
   write_cell(col,row,gen_format
      (0/*not protected*/,WKS_format_special,WKS_special_text));                 //001206
   write_uint8(alignment);
   write(value,strlen(value));
   write_uint8(0);
}
//_write_label_cell____________________________________________________________/
void WKS_file::write_integer_cell( int16 col, int16 row, int16 value)
{  Common_spreadsheet_file::write_integer_cell(col,row,value);
   write_rec_header(WKS_CODE_INTEGER,WKS_LEN_INTEGER);
   write_cell(col,row,WKS_gen_format_integer);                                   //001206
   write_sint16(value);                                                          //001206
}
//_write_integer_cell__________________________________________________________/
void WKS_file::write_number_cell(int16 col, int16 row, double value,int8 precision)
{  Common_spreadsheet_file::write_number_cell( col,  row,  value,precision);
   write_rec_header(WKS_CODE_NUMBER,WKS_LEN_NUMBER);
   write_cell(col,row,gen_format(0/*not protected*/,WKS_format_fixed,precision));//001206
   write_float64(value);                                                         //001206
}
//_write_number_cell___________________________________________________________/
void  WKS_file::read_number_cell(bool expect_header)
{  read_cell();                                                                  //001206
   float_value = read_float64();
   data_type = type_float;
   Common_spreadsheet_file::read_number_cell(expect_header);
}
//_read_number_cell____________________________________________________________/
void  WKS_file::read_label_cell(bool expect_header)
{  int lngth = length-6; // length includes terminating 0
   read_cell();                                                                  //001206
   alignment = read_uint8();
   read((char *)str_value,lngth);
   data_type = type_str;
   Common_spreadsheet_file::read_label_cell(expect_header);
}
//_read_label_cell_____________________________________________________________/
byte WKS_file::gen_format(byte protection, byte format_type, byte decimals_special)
{  return  (protection << 7) | (format_type << 4) | decimals_special;
}
//_gen_format__________________________________________________________________/
void WKS_file::extract_format(byte format)
{                                 // 76543210
   byte protection_mask = 0x80;    // 10000000;
   byte format_mask     = 0x7f;    // 01110000;
   byte precision_mask  = 0x0f;    // 00001111;

   protection  = ((format & protection_mask) > 0);
   format_type = ((format & format_mask) >> 4);
   if (format == WKS_format_special)
   {
      special_format = precision_mask & format;
      precision = 0;
      switch (special_format)
      {
       case WKS_special_plusminus      : data_type = type_integer; break;
       case WKS_special_general        : data_type = type_integer; break;
       case WKS_special_day_month_year : data_type = type_date; break;
       case WKS_special_day_month      : data_type = type_date; break;
       case WKS_special_month_year     : data_type = type_date; break;
       case WKS_special_text           : data_type = type_str; break;
       default : data_type = type_integer;  break;
      }
   }
   else
   {  precision = format & precision_mask;
     data_type = type_float;
   }
}
//_extract_format______________________________________________________________/
WKS_file::WKS_file(const char *filename_)                                        //001206
: Common_spreadsheet_file(filename_)
, protection(false)
, format_type(WKS_format_fixed)
, special_format(0)
, WKS_version(1)                                                                 //001206
{
   margins.left = 0;
   margins.right = 0;
   margins.top = 0;
   margins.bottom = 0;
   margins.page_length = 0;
   print_range.start_col = 0;
   print_range.start_row = 0;
   print_range.end_col   = 0;
   print_range.end_row  = 0;
   str_value[0] = 0;                                                                 //981016
   header[0] = 0;
    // If the file has length, then it must exists an the first record should be BOF record
   if (get_file_length())                                                           //001206
      at_eof = !read_BOF_record();                                                  //981018
}
//_constructor for output___________________________________________1998-10-16_/
WKS_file::WKS_file
(const char *filename_                                                           //001206
,int16 first_row_   ,int16 rows_
,int16 first_col_   ,int16 cols_)
: Common_spreadsheet_file
(filename_
,first_row_
,rows_
,first_col_
,cols_)
, protection(false)
, format_type(WKS_format_fixed)
, special_format(0)
, WKS_version(1)                                                                 //001206
{
   margins.left = 0;
   margins.right = 0;
   margins.top = 0;
   margins.bottom = 0;
   margins.page_length = 0;
   print_range.start_col = 0;
   print_range.start_row = 0;
   print_range.end_col   = 0;
   print_range.end_row  = 0;
   str_value[0] = 0;                                                             //981016
   header[0] = 0;                                                                //981016
   write_BOF_record();                                                           //981018
   write_dimensions(first_row_,rows_,first_col_,cols_);                          //981018
}
//__________________________________________________________________1998-10-16_/
int16 WKS_file::get_BOF_code() {return WKS_CODE_BOF;}
int16 WKS_file::get_EOF_code() {return WKS_CODE_EOF;}
//_____________________________________________________________________________/
