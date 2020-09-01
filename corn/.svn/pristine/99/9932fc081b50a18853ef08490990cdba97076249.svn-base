#include "corn/tabular/tabular_text.h"
#include "corn/math/compare.h"
#include "corn/tabular/delineated_datarec.h"
#include "corn/chronometry/date_time_64.h"
#include "corn/tabular/tabular.h"
#include "corn/OS/directory_entry_name.h"
#include <fstream>
namespace CORN {
//______________________________________________________________________________
int Tabular_text_file::get_column_count()
{  // returns the number of columns identified in the table.
   return format.columns.count();                                                //170327
}
//_get_column_count_________________________________________________2005-04-11_/
int Tabular_text_file::get_column_width(int column)
{  // returns the width (in characters of the specified column
   return format.get_width_column(column);                                       //170327
}
//_get_column_width_________________________________________________2005-04-11_/
Tabular_text_file::Tabular_text_file
(const std::string &filename_                                                    //190506
, std::ios_base::openmode   openmode_                                            //141111
, bool                     keep_read_records_in_memory_
, const Tabular_format/*200220 Common_tabular_file__format*/ &format_                                       //170327
, bool                     record_data_positions_                                //111110
,Indexer                  *optional_indexer_)                                    //111024
:CORN::Tabular_file(&filename_,keep_read_records_in_memory_,optional_indexer_)   //111024
,format  (format_)                                                                //170327
,data_line_count(0)
,at_end_data_marker(false)                                                       //050426
,record_data_positions(record_data_positions_)                                   //111110
,openmode(openmode_)
,data_stream()                                                                   //141018
,raw_data_c_str(0)                                                               //050322
,write_buffer("")                                                                //090305
{     //Currently we are reading all records into memory
      // but latter we may read the lines as they are needed
      // first reading to build an index of the file pointers
      // to the begining of each line.
   if (openmode & std::ios::in)                                                  //150405
      redelineate_if_necessary();                                                //000908
}
//_Tabular_text_file:constructor_______________________________________________/
Tabular_text_file::Tabular_text_file
(const char                *raw_data_c_str_
,bool                       keep_read_records_in_memory_
,const Tabular_format/*200220 Common_tabular_file__format*/ &format_                                       //170327
, bool record_data_positions_                                                    //111110
,Indexer *optional_indexer_                                                      //111024
)
:CORN::Tabular_file
   (0  // in memory mode (cout or no file)
   ,keep_read_records_in_memory_,optional_indexer_)                              //111024
,format  (format_)                                                               //170327
,data_line_count(0)
,at_end_data_marker(false)                                                       //050426
,record_data_positions(record_data_positions_)                                   //111110
,data_stream(0)                                                                  //050407
,openmode(std::ios::out)                                                         //160306
,raw_data_c_str(raw_data_c_str_)                                                 //050322
,write_buffer("")                                                                //090305
{  //   constructor for c_str mode
      redelineate_if_necessary();                                                //000908
}
//_Tabular_text_file:constructor____________________________________2005-03-22_/
Tabular_text_file::Tabular_text_file
(std::ostream                       *output_stream_
,const Tabular_format/*200220 Common_tabular_file__format*/   &format_                                     //170327
, bool                               record_data_positions_
) // Constructor when writing to a stream
:CORN::Tabular_file
   (0 // in memory mode (cout or no file)
   ,false,0)
,format  (format_)                                                               //170327
,data_line_count           (0)
,at_end_data_marker        (false)
,record_data_positions     (record_data_positions_)
,data_stream               (0)
,openmode                  (std::ios::out)
,raw_data_c_str            ()
,write_buffer              ("")
{}
//_Tabular_text_file:constructor____________________________________2014-10-08_/
File_data_stream::File_data_stream
(bool record_data_positions_)                                                    //111110
: Data_stream()
, filename()
,record_data_positions  (record_data_positions_)
,data_record_position   (10,10,0)                                                //100322
{}
//_File_data_stream:constructor________________________________________________/
File_data_istream::File_data_istream
(const char *filename_
,bool        record_data_positions_)
: File_data_stream   (record_data_positions_)
, raw_data_xistream  (new std::ifstream(filename_))                              //141210
{  filename = filename_;
}
//_File_data_istream:constructor____________________________________2014-10-09_/
File_data_ostream::File_data_ostream
(const char *filename_
,bool        record_data_positions_)
: File_data_stream(record_data_positions_)
, raw_data_ostream(new std::ofstream(filename_))
{  filename = filename_;
}
//_File_data_ostream:constructor____________________________________2014-10-09_/
File_data_ostream::~File_data_ostream()
{
   if (owns_raw_data_stream) delete raw_data_ostream;                            //160306
}
//_File_data_ostream:destructor_____________________________________2015-05-04_/
const char *File_data_istream::read_raw_line()
{
   getline((*raw_data_xistream),raw_data_line);
   #ifndef _Windows                                                              //120921
   // If reading a text file created for Windows/DOS where the line ends in \r\n
   // under an operating systems (Unix/Linux) where the text files don't have \r
   // we automatically strip the \r.
   size_t raw_data_line_length = raw_data_line.length();
   if (raw_data_line[raw_data_line_length-1] == '\r')
       raw_data_line[raw_data_line_length-1] = 0;
   #endif
   return raw_data_line.c_str();
}
//_read_raw_line_______________________________________________________________/
bool File_data_ostream::write_raw_line(const char *raw_line)
{
   if (raw_data_ostream)
      (*raw_data_ostream) << raw_line << std::endl;
   else
      std::cout << raw_line << std::endl;
   return true;
}
//_write_raw_line___________________________________________________2005-06-12_/
Container::Iterator *C_str_data_stream::iterator() const
{  CORN::Text_list &this_container = (CORN::Text_list      &)*this;
   C_str_data_stream::Iterator *itor = new C_str_data_stream::Iterator(this_container);
   return itor;
}
//_iterator_________________________________________________________2005-04-10_/
C_str_data_stream::C_str_data_stream(const char *raw_data_c_str)
: Data_stream()
, CORN::Text_list(raw_data_c_str)
, line_iterator((C_str_data_stream::Iterator *)iterator())
{}
//_C_str_data_stream___________________________________________________________/
C_str_data_stream::~C_str_data_stream()
{  if (line_iterator) delete line_iterator; line_iterator = 0;
}
//______________________________________________________________________________
static char empty_string[2] = "";
const char *C_str_data_stream::read_raw_line()
{  const char *result = empty_string;
   const Item_string *curr_text_line
      = dynamic_cast<const Item_string *>(line_iterator->current());             //120204
   if (curr_text_line)
   {  result = curr_text_line->c_str();
      line_iterator->next();
   }
   return result;
}
//_read_raw_line_______________________________________________________________/
bool C_str_data_stream::write_raw_line(const char *raw_line)
{  return append(new Item_string (raw_line)) != 0;
}
//_write_raw_line___________________________________________________2005-06-12_/
Data_stream* Tabular_text_file::provide_data_stream()
{
   if (!data_stream)
   {
      if (openmode == std::ios::out)                                             //141109
         data_stream =
            filename                                                             //160306
            ? new File_data_ostream(filename->c_str(),true)                      //160306
            : new File_data_ostream(0 /*std::cout*/,false,false);                //160306
      else
         data_stream                                                             //050322
            = (raw_data_c_str)                                                   //050322
            ? (Data_stream *)(new C_str_data_stream(raw_data_c_str))             //050322
            : (Data_stream *)(new File_data_istream(filename->c_str(),record_data_positions));//160306_050322
//std::clog << "status:" << (data_stream->good() ? "good" : "bad") << std::endl;
   }
   return data_stream;
}
//_provide_data_stream______________________________________________2014-11-11_/
bool Tabular_text_file::redelineate_if_necessary()
{
   if (data_stream)                                                              //150909
       data_stream->deallocate();                                                //150505
   delete data_stream; data_stream = 0;                                          //050407
   header_records.delete_all();
   provide_data_stream();                                                        //141111
   // skip to begin data marker (if specified in the format)                     //050322
   //skipping any comments at the beginning of the file
   //(I.e. email header text or other similar comments)
   if (format.document.extent.begin_marker.length())                             //200220_170327050322
   {  bool at_begin_marker = false;                                              //050322
      while (!at_begin_marker && (!data_stream->at_end()) && data_stream->good())//110822_050322
      {  std::string
            pot_data_marker_line(data_stream->read_raw_line());                  //050322
         if (pot_data_marker_line.
               find(format.document.extent.begin_marker) != std::string::npos)   //200220

         at_begin_marker = true;                                                 //050322
   } }                                                                           //050322
   // Currently presuming that at this point we are ready to start reading
   // header lines (if any)
   nat32  data_start_row = get_start_data_row_1_based();                         //050524
   for (nat32  header_line = 1; header_line < data_start_row; header_line++)     //070305
   {  if (!data_stream->at_end() && data_stream->good())                         //110822_050322
      {  Item_string *new_record = new Item_string(data_stream->read_raw_line()); //050407
         header_records.append(new_record);
   } }
   // At this point we should be at the beginning of the data records
   // Dont' delete the data_stream  delete data_stream;
   // it must remain open for get
   return data_stream->remember_data_line_position();                            //050410
}
//______________________________________________________________________________
bool File_data_stream::goto_start_of_data()
{
/*171110 This doesn't look right
   std::streampos  start_of_data = data_record_position.get(0);
   seekX(start_of_data);                                                         //141009
*/
   bool is_good = good();                                                        //130805
   return is_good;
}
//_goto_start_of_data_______________________________________________2004-05-10_/
bool File_data_stream::remember_start_of_data()
{  data_record_position.clear();
   std::streampos  start_of_data = tellX();
   data_record_position.append(start_of_data);
   return good();
}
//_remember_start_of_data___________________________________________2004-05-10_/
bool File_data_stream::remember_data_line_position()
{  if (record_data_positions)                                                    //111110
   {  std::streampos  start_of_data = tellX();
      data_record_position.append(start_of_data);
   }
   return good();
}
//_remember_data_line_position______________________________________2004-05-10_/
bool File_data_stream::goto_data_line(nat32  zero_based_index)
{  if (!(record_data_positions || (zero_based_index==0))) return false;          //131210
   bool index_valid = (zero_based_index < data_record_position.get_count());
   if (index_valid)
   {  std::streampos line_position = data_record_position.get(zero_based_index);
      index_valid = (bool)seekX(line_position);                                  //141009
   }
   return index_valid;
}
//_goto_data_line___________________________________________________2004-05-10_/
bool File_data_stream::goto_file_position(std::streampos file_pos)    stream_IO_
{  bool succeeded = good();
   if (succeeded)
      seekX(file_pos);
   else
      std::cerr << filename << " table not good" << std::endl;
   return succeeded ;
}
//_goto_file_position_______________________________________________2011-11-11_/
C_str_data_stream::Iterator::Iterator(CORN::Text_list &container_)
:Text_list::Iterator(container_)
{}
//_C_str_data_stream::Iterator:constructor__________________________2004-05-10_/
CORN::Item *C_str_data_stream::Iterator::Ith_data_line(nat32 i)
{  nat32  count = 0;
   for (Node *local_curr = (Node *)start_of_data_node; local_curr
       ;local_curr = (Node*)local_curr->next)
   {  if (count == i)
      {  curr_node = local_curr;
         return local_curr->get(); // set curr and return
      }
      count ++;
   }
   return 0;
}
//_Ith_data_line____________________________________________________2004-05-10_/
bool C_str_data_stream::goto_start_of_data()
{  return  line_iterator->Ith_data_line(0);
}
//_goto_start_of_data_______________________________________________2004-05-10_/
bool C_str_data_stream::remember_start_of_data()
{  line_iterator->remember_start_of_data_node();
   return true;
}
//_remember_start_of_data___________________________________________2004-05-10_/
bool C_str_data_stream::remember_data_line_position()
{  return true; // we don't need to remember the data_line positions, the are already intrinsically indexed
}
//_remember_data_line_position______________________________________2004-05-10_/
bool C_str_data_stream::at_end()                                      stream_IO_
{return line_iterator->current() == 0;
}
//_at_end_______________________________________________________________________
bool C_str_data_stream::good()                                             const
{ return true;
}
//_good_____________________________________________________________2011-08-22_/
void C_str_data_stream::Iterator::remember_start_of_data_node()
{  start_of_data_node = curr_node;
   if (start_of_data_node == 0)
      start_of_data_node = container.get_head();
}
//_remember_start_of_data_node______________________________________2004-05-10_/
bool C_str_data_stream::goto_data_line(nat32  zero_based_index)
{  CORN::Item *data_line_item = line_iterator->Ith_data_line(zero_based_index);  //070305
   return data_line_item!= 0;                                                    //070305
}
//_goto_data_line___________________________________________________2004-05-10_/
bool CORN::Tabular_text_file::goto_index(nat32  zero_based_index)
{  return data_stream->goto_data_line(zero_based_index);
}
//_goto_index_______________________________________________________2005-04-10_/
bool CORN::Tabular_text_file::goto_first()
{
   provide_data_stream();                                                        //181001
   at_end_data_marker = false;                                                   //191001
   return data_stream->goto_start_of_data();
}
//_goto_first__________________________________________________________________/
bool CORN::Tabular_text_file::get(Data_record &data_rec)
{
   provide_data_stream();                                                        //181001
   if (data_stream->at_end()) return false;                                      //180312
   bool got = false;                                                             //180312
   got = CORN::Tabular_file::get(data_rec);                                      //180312
   // got will correctly be false at this point!!
   //180312 bool got = false;

   // At this point we should be at a data_record
   Text_list raw_field_values("");
   std::string record_line_string(data_stream->read_raw_line());
   if (format.document.extent.end_marker.length()                                //200220
     && record_line_string.find(format.document.extent.end_marker) != std::string::npos) // 070424 these may need to be find_substring //050426
         at_end_data_marker  = true;                                             //050426
   if ((record_line_string.length() == 0))                                       //050606
   {  if ((format.document.extent.end_marker == "{blankline}")
          || format.document.extent.end_marker.empty())                                          //190926
          at_end_data_marker = true;                                             //050606
   }
   if (at_end_data_marker)                                                       //050426
       return false;                                                             //050426
   if (!record_line_string.empty())                                              //180312
   {
      const char *record_line = record_line_string.c_str();
      size_t record_line_length =  record_line_string.length();                  //070305
      bool at_EOL = false;
      size_t line_index = 0;  // this is the line string index which is 0 based
      std::string field_value;
      // Determine columns and widths
      size_t col_pos = 1;  // 0 is not used                                      //100204
      int curr_column_number = 1;  // 0 is not used                              //100204
      size_t field_pos = 0; // 0 indexed                                         //100204
      bool in_field = false;                                                     //100204
      bool at_delimiter = false;                                                 //050412
      bool at_delimiter_or_column_width = false;
      //200220 Delineation::Text_quote_mode in_quote=Delineation::NO_QUOTE; //180119
      std::string in_quote;                                                      //200220
      do
      {  char inchr[2]; inchr[1] = 0;
         inchr[0] = record_line[line_index];
         if (inchr[0] == 0) at_EOL = true;                                       //180312
         at_delimiter_or_column_width = false;
         nat16 curr_column_index = curr_column_number - 1;
         size_t position = format.Table_definition::get_position(curr_column_index); //200306_100205
         std::string quotation = format.detail.separation.quotation;             //200220
         if (quotation == "\"")
         {
            if (inchr[0] == '"')                                                 //200220_180119
            {
                  in_quote = (in_quote == "\"")                                  //200220_180119
                     // are we at the end of started double quote                //180119
                  ?  "" : "\"";                                                  //200220_180119
                  inchr[0] = 0;                                                  //1803012
            }
         } else if (quotation == "'")                                            //200220
         {
            if (inchr[0] == '\'')                                                //200220_180119
               {
                  in_quote = (in_quote == "'")                                   //200220_180119
                     // are we at the end of started double quote                //180119
                  ?  "" : "'";                                                   //200220_180119
                  inchr[0] = 0;                                                  //1803012
               }
         } else if (quotation == "\"'")                                          //200220
         {
           // alternation is NYI
         } else if (quotation == "'\"")                                          //200220
         {
           // alternation is NYI
         }

/*200220 replaced
         switch (format.detail_lines_delineation.text_quote_mode_clad.get())  //180119
         {
            case Delineation::DOUBLE_QUOTE :                              //180119
            {  if (inchr[0] == '"')                                              //180119
               {
                  in_quote = (in_quote == Delineation::DOUBLE_QUOTE)      //180119
                     // are we at the end of started double quote                //180119
                  ?  Delineation::NO_QUOTE : Delineation::DOUBLE_QUOTE; //180119
                  inchr[0] = 0;                                                  //1803012
               }
            } break;
            case Delineation::SINGLE_QUOTE :                              //180119
            {  if (inchr[0] == '\'')                                             //180119
               {
                  in_quote = (in_quote == Delineation::SINGLE_QUOTE)      //180119
                     // are we at the end of started double quote                //180119
                  ?  Delineation::NO_QUOTE : Delineation::SINGLE_QUOTE; //180119
                  inchr[0] = 0;                                                  //1803012
               }
            } break;
            case Delineation::ALTERNATING_QUOTE :                         //180119
            {
               //assert(false); // NYI                                           //180119
            } break;
         } // switch

*/
         switch (format.detail.tabulation_clad.get())     //200220_170327
         {  case fixed_TABULATION:
            {  if (col_pos == position) in_field = true;
               at_delimiter_or_column_width
                  =
                  in_field && (field_pos ==
                     format.get_width_column(curr_column_index));                //170327_100204
            } break;
            /*200220
            #if ((CS_VERSION > 0) && (CS_VERSION < 6))
            case Delineation::DELIMIT_character_renamed :
            #endif
            */
            case delimit_TABULATION :                    //200131

               at_delimiter_or_column_width = at_delimiter =
                  format.detail                                                  //200220_170327
                  .is_delimiter(inchr[0])
                  && in_quote.empty(); //200220 !in_quote;                       //180120
                  // current_column delineators == inchr[0];
               if (at_delimiter_or_column_width)                                 //100229
                  inchr[0] = 0;
               in_field = true;  // in delimit character mode, we are always in a field
            break;
         }
         if (at_EOL)                                                             //180312
            at_delimiter_or_column_width = true;  // End of line delimits everything (currently presuming one raw record per line) //050410_
         if (at_delimiter_or_column_width)
         {
            raw_field_values.append(new Item_string(field_value.c_str()));
            field_value.clear();
            curr_column_number ++;
            position = format.Table_definition::get_position                     //200306
               (curr_column_number-1);                                           //170327
            field_pos = 0;
            in_field =
               (format.detail.tabulation_clad.get() == fixed_TABULATION)         //200220_170327
               //200220(detail_lines_delineation.delimit_mode_clad.get() == Delineation::fixed_width_TABULATION)
               ? (col_pos == position)  // in fixed width mode there may be padding space or unrecognized columns of data that are ignored/omitted
               : true;  // in delimit character mode, we are always in a field
         }
         if (in_field)
            {  if (inchr[0] == format.detail.separation.decimal)                 //200220
                  inchr[0] = '.';   // convert decimal point marker (from europoean
               field_value.append(inchr);
               field_pos+= 1;                                                    //100204
            } // else we are at a character position in the line that is not defined by a column delineation,
            // possibly a blank or an omitted field.

         // Eat continuous delimiter characters
         line_index++;
         col_pos++;
         //200220 if (at_delimiter&&format.detail_lines_delineation.continuous_delimiter) //170327
         if (at_delimiter&&format.detail.separation.continuous)                  //200220_170327
            while (
               format.detail                                                    //170327
               .is_delimiter(record_line[line_index]) && (line_index < record_line_length))
               line_index++;
         if ((line_index== record_line_length) )
            raw_field_values.append(new Item_string(field_value.c_str()));
      } while (line_index < record_line_length);
      FOR_EACH_IN(section,VV_Section,data_rec.sections,each_section)
      { // This format doesn't really have sections, but the expected data record layout may
         FOR_EACH_IN(entry,VV_abstract_entry,section->entries,each_entry)
         {  // I could check the field name but they should be in sequence
            Item_string *datum
               = dynamic_cast<Item_string *>(raw_field_values.pop_first());
            if (datum)
            {  //110217 Currently parameter entries
               // are don't have tabular column description
               // eventually I will probably consolidate these
               // and the column data type will be available
               // so this check for entry type is not be needed.
               //180120 Actually, numbers could be quoted also (ala JSON)
               {  // For string fields, we strip any quote marks if used in the format
                  //unused unsigned char vv_type = entry->get_type();
                  std::string quotation =                                        //200220
                        format.detail.separation.quotation;                      //200220
                  if (quotation.length() == 1)                                   //200220
                  {  // single or double quote
                     CORN::strip_string(*datum,CORN::Both,quotation[0]);            //200220
                  }
                  else if (quotation.length() == 2)
                     {  // ALTERNATING_QUOTE :
                        if (datum->c_str()[0] == '\'') CORN::strip_string(*datum,CORN::Both,'\''); //130919
                        if (datum->c_str()[0] == '\"') CORN::strip_string(*datum,CORN::Both,'\"'); //130919
                     }
                  // else  no quotation do nothing

                  /*200220
                  Delineation::Text_quote_mode quote_mode =
                        format.detail_lines_delineation                          //170327
                        .text_quote_mode_clad.get();                          //110217
                  switch (quote_mode)                                            //110217
                  {  case Delineation::SINGLE_QUOTE :
                        CORN::strip_string(*datum,CORN::Both,'\'');              //130919
                        break;
                     case Delineation::DOUBLE_QUOTE :
                        CORN::strip_string(*datum,CORN::Both,'\"');              //130919
                        break;
                     case Delineation::ALTERNATING_QUOTE :
                     {
                        if (datum->c_str()[0] == '\'') CORN::strip_string(*datum,CORN::Both,'\''); //130919
                        if (datum->c_str()[0] == '\"') CORN::strip_string(*datum,CORN::Both,'\"'); //130919
                     } break;
                     case Delineation::NO_QUOTE : default : break;
                  }

                  */
               }
               entry->set_str(datum->c_str());
               VV_date_entry *entry_as_date = dynamic_cast<VV_date_entry *>(entry); //181001
               if (entry_as_date)
                  entry_as_date->apply_offset_years(format.year_offset_for_dates_with_only_two_digits);       //181001
               VV_datetime64_entry *entry_as_datetime= dynamic_cast<VV_datetime64_entry *>(entry); //181001
               if (entry_as_datetime)
                  entry_as_datetime->apply_offset_years(format.year_offset_for_dates_with_only_two_digits);      //181001
      //NYI  Here we have the raw data as a string
      //NYI   we should check for special markers
      //      we would provide CORN::Tabular_file with marker/code pairs (I.e. validation status)
      //NYI              entry->set_validation_status(validate(/*column_index,*/datum->c_str())); // Here we can check for any attribute markers 050518_
            // Derived classes can override check_datum_code for missing markers etc...
               delete datum;
            }
            got = true;
         } FOR_EACH_END(each_entry)
      } FOR_EACH_END(each_section)
      data_stream->remember_data_line_position();                                //050410
   }
   return got;
}
//_get______________________________________________________________2005-04-06_/
bool CORN::Tabular_text_file::get_next(Data_record &data_rec)
{  bool got = get(data_rec);
   return got;
}
//_get_next_________________________________________________________2005-04-06_/
bool CORN::Tabular_text_file::set_data(Data_record &data_rec,bool append)        //150505
{
/*
//             This commits (writes) data from data_rec to this data source.
//             Classes derived from Data_record may provide information such
//             as the position of the record in the data file an the Data_source
//             may then need to position the file pointer before writing the record.
//             In classes such as VV_File, a Data_record constitutes all the data in
//             a file, consequently the data can be written out to the file.
//             Returns true if the record could be written.
//             Returns true if there was a problem.
//             This abstract method must be provided by a derived class.

               The new function "set(Data_record &)" is used to commit a Data_record
               data structure to the file.

      If the Data_record's record number is unassigned the record is assumed
      to be appended so a record number will be assigned, the file pointer
      positioned and the buffer written to the file.
      If the record number is assigned (The Data_record must have been
      previously loaded from the file), the file pointer will be computed,
      positioned and the buffer written to the file.
      Once the buffer has been read or written to the file is marked as not modified.

      When a database is being created, the first Data_record provided in
      a set() method call will be used to create the new database format in the
      dBase header.
      The width of character and float fields, and float precision
      will depend on the suggested width and precision specified for the Data_record entry.
*/
   provide_data_stream();                                                        //151027
   bool sat = false;                                                             //050705
   modified |= 1;                                                                //130821 was modified += 1
   // In write mode, there may be header and preable line pending to dump.
   if (write_buffer.length())                                                    //070203
   {  data_stream->write_raw_line(write_buffer.c_str());                         //070203
      write_buffer.assign("");                                                   //090709
   }                                                                             //090709
   {  bool first_item = true;
      nat16 column_index = 0;                                                    //070523
      //200220 if (format.FORTRAN_carriage_control)
      if (format.lineation == "ASA") // FORTRAN CC                                //200220
      {  write_buffer.append(" ");
      }


      // VV is to be replaced with YAML  (use structural)

      FOR_EACH_IN(section,VV_Section,data_rec.sections,each_section)             //030709
      {
            //for each entry in current section
            FOR_EACH_IN(entry,VV_abstract_entry,section->entries,each_entry)     //030709
            {  // lookup field in actual fields:
               unsigned char vv_type = entry->get_type();
               const Tabular_column *tabular_column =entry->get_tabular_column();//070524
               std::string value_string;                                         //140615
               std::string quotation ( (vv_type == VV_string)                    //200220
                  ?  format.detail.separation.quotation.c_str()                  //200220
                  : "");                                                         //200220
               /*200220
               Delineation::Text_quote_mode quote_mode =  (vv_type == VV_string)
                  ?  format.detail_lines_delineation                             //170327
                     .text_quote_mode_clad.get()
                  : Delineation::NO_QUOTE;                                //090712
               */
               if ((format.detail.tabulation_clad.get() == delimit_TABULATION) //200220
                  /*200220
                  #if ((CS_VERSION > 0) && (CS_VERSION < 6))
               if ((format.detail_lines_delineation                               //170327
                    .delimit_mode_clad.get() == Delineation::delimit_TABULATION) //200131
                  || (format.detail_lines_delineation                               //170327
                  .delimit_mode_clad.get() == Delineation::DELIMIT_character_renamed) //070522
                  #endif
                  */
               )
               {  if (!first_item)                                               //050121
                  {                                                              //070522
                     //should not be needed if (format.detail_lines_delineation.has_delimiter())       1//190106
                     format.detail.append_delimiter(write_buffer);               //200220_190106
                  }
                  VV_datetime64_entry *as_datetime64_entry = dynamic_cast<VV_datetime64_entry *>(entry);
                  if (as_datetime64_entry)
                  {
                      as_datetime64_entry->set_cstr
                              (  tabular_column->date_time_format.length()
                               ? tabular_column->date_time_format.c_str()
                               : "YYYY-MM-DD(DOY)" ,"");
                      as_datetime64_entry->get_value().
                        append_to_string(value_string,0,0);                      //141120
                  } else                                                         //110822
                  entry->append_to_string(value_string);                         //140516
                   write_field_string(value_string.c_str(),quotation);           //200220_140516_090712
                   // NYI end quote for string
               } else // fixed with
               {
                  // Warning:     070525
                  // currently, I am presuming that all data columns defined in the format will
                  // cover all character positions in the detail line.
                  // So, I am simply appending the text of the field to the record buffer
                  // HOWEVER, the is nothing to prevent a fixed width format
                  // from omitting data columns or skipping character columns
                  // or even defining columns out of sequence
                  // So, I should use a c_str record buffer (cleared to spaces)
                  // and copy the field value value_str into the column start position in the record buffer.
                  uint16 column_width = tabular_column ->get_field_width();
                  { // Here we get the value as a string
                     switch (vv_type)
                     {
                     case VV_date :
                     {
                        VV_date_entry *date_entry = (VV_date_entry *)entry;
                        // NYI should have date format string in the tabular format file //050612
                        CORN::Date_clad_32 dbase_format_date(date_entry->get_value().get_date32());
                        dbase_format_date.append_to_string(value_string _ISO_FORMAT_DATE_TIME);        //140615
                     } break;
                     case VV_string:                                             //090714
                     {
                        // NYI
                     } break;
                     default :
                     {  char decimal = '.';                                      //021214
                        if (!format.detail                                       //200220_170327
                           .separation.decimal == 0 /*200220 .empty()*/)         //200220
                           decimal = format.detail.separation.decimal;           //170327

                        // NYN int8 precision = tabular_column ? tabular_column->precision_16 : 3; //070524
                         entry->append_to_string(value_string);                  //140615
                      }
                      /*200220
                     {  char decimal_mark = '.';                                 //021214
                        if (format.detail_lines_delineation                      //170327
                           .decimal_mark.length())
                           decimal_mark = format.detail_lines_delineation        //170327
                              .decimal_mark.c_str()[0];
                        // NYN int8 precision = tabular_column ? tabular_column->precision_16 : 3; //070524
                         entry->append_to_string(value_string);                  //140615
                      }
                      */
                      break;
                      }
                   }
                   {  // Add some extra spaces in case the columns start positions skip character columns
                      // (I.e. the TDF columns don't account for all the possible columns, and/or there is white space)
                      sint16 sync_column_ndx = (tabular_column->delineation.position - 1) - write_buffer.length(); //200302_130821_100104
                      if (sync_column_ndx < 0) sync_column_ndx = 0; // This should never happen unless the TDF column start positions are wrong) //100104
                      if (sync_column_ndx) write_buffer.append                   //100104
                              (std::string(sync_column_ndx,' '));                //130821
                   }
                  int copy_length = std::min<int>(column_width
                     ,(int) value_string.length());                              //140615
                  int tab_pos = 0;
                  { // Here we setup tabpos
                     switch (vv_type)
                     {
                     case VV_int8      :
                     case VV_int16     :
                     case VV_int32     :
                     case VV_nat16    :
                     case VV_nat8     :
                     case VV_nat32    :
                     case VV_float32  :
                     case VV_float64  :
                        tab_pos = column_width - copy_length;
                     break;
                     default :   tab_pos = 0; break;
                     }
                  } // end setup tabpos
                  if (tab_pos) write_buffer.append(std::string(tab_pos,' '));
                  write_buffer.append(value_string);
                  sat = true; // If any field is encountered then we accept this data
               }
               // else the field was simply not encountered
               entry->set_encountered(true /*field*/);  // for text proably always mark as encountered
               first_item = false;
               column_index++;                                                   //070523
            } FOR_EACH_END(each_entry)
      } FOR_EACH_END(each_section)
//debug std::cout << write_buffer.c_str() << std::endl;
      data_stream->write_raw_line(write_buffer.c_str());
      write_buffer.assign("");                                                   //100126
   }
   return sat;
}
//_set_data_________________________________________________________2001-01-03_/
const char *CORN::Tabular_text_file::label_string(std::string &buffer) const
{
   // NYI NYN
   // Normally (for text output (I.e. Windows title bar)
   // we would want to show the name of the file and the line (actual line or data line?)being processed
   // This currently isn't needed for the UED import utilities
   return buffer.c_str();
}
//_label_string________________________________________________________________/
CORN::Tabular_text_file::~Tabular_text_file()
{  if (write_buffer.length())
      data_stream->write_raw_line(write_buffer.c_str());
   write_buffer.assign("");
   if (data_stream) data_stream->deallocate();                                   //150505
   delete data_stream;                                                           //150505
}
//_Tabular_text_file:destructor_____________________________________2007-02-03_/
bool CORN::Tabular_text_file::goto_raw_line(nat32 _line_number_1_based)
{  if (_line_number_1_based < current_raw_line) return false; // we can't back up in text files
   provide_data_stream();                                                        //150405
   for (;current_raw_line < _line_number_1_based;current_raw_line++ )
   {  data_stream->write_raw_line(write_buffer.c_str());
      write_buffer.assign("");
      //200220 if (format.FORTRAN_carriage_control)
      if (format.lineation == "ASA") // FORTRAN CC                                //200220
      {  write_buffer.append(" ");
      }
      // May also need to handle New line and CR currently assuming this is the same as the current operating system.
   }
   // fix_width_mode_curr_col_pos = 0;  // don't include carridge control        //100104
   return true;
}
//_goto_raw_line____________________________________________________2007-02-03_/
bool CORN::Tabular_text_file::goto_field_column(nat32 column_number_)
{  if (column_number_ < current_field_column) return false; // we can't back up in text files
   for (;current_field_column <= column_number_;current_field_column++)
   {
      // WARNING currently assuming the delineation is the same as the data records, this is not necessarily the case!
      // Should pass column header delineation
         switch (format.detail                               //170327
            .tabulation_clad.get())
         {  case fixed_TABULATION :
            {  // Pad empty fixed width fields with spaces
               nat32 position = format.Table_definition::get_position(current_field_column); //200306_190325
               size_t write_buffer_length = write_buffer.length();               //130821
               sint16 padding = (position-1) - write_buffer_length;              //130821_100104
               if (padding < 0) padding = 0;                                     //100104
               for (size_t i = 0; i < (size_t)padding; i++)                      //130821_100104
               // TDF descriptions do not have to cover every character column, a column can start at any arbitrary position skipping white space.
                  write_buffer.append(" ");
            } break;
            /*200220
            #if ((CS_VERSION > 0) && (CS_VERSION < 6))
            case CORN::Delineation::DELIMIT_character_renamed   :
            #endif
            */
            case /*200220 CORN::Delineation::*/delimit_TABULATION :
            {  // A format may allow any delimiter option when reading
               // but when writing we have to choose one as the preferred.
               // currently tab is preferred
               if (current_field_column > 1) // The first column does not need delimiter
               {
                 format.detail.append_delimiter(write_buffer);                   //200220_190106
               } // if current field column
            } break;
            default : case intrinsic_TABULATION : break; // should not occur does not apply to text files
         } // switch delimit_mode
   } // for current field_column
   current_field_column = column_number_;                                        //100104
   return true;
}
//_goto_field_column________________________________________________2007-02-03_/
nat32 CORN::Tabular_text_file::write_raw_string
(const char *c_str)
{  write_buffer.append(c_str);
   return (nat32)strlen(c_str);
}
//_write_raw_string_________________________________________________2007-02-03_/
nat32 CORN::Tabular_text_file::write_delimiter_string
(const char *c_str)
{  write_buffer.append(c_str);
   return (nat32)strlen(c_str);
}
//_write_delimiter_string___________________________________________2007-02-03_/
nat32 CORN::Tabular_text_file::write_field_string
(const char *c_str
,const std::string &quotation                                                    //200220
//200220 ,CORN::Delineation::Text_quote_mode quote_mode                          //090712
)
{  std::string formatted_string;
   nat16 field_width =                                                           //190106
      format.detail.tabulation_clad.get() == fixed_TABULATION                    //200220
      /*200220
      format.detail_lines_delineation.delimit_mode_clad.get()
         == CORN::Delineation::fixed_width_TABULATION
      */
      ? format.get_width_column(current_field_column)
      : 0;
   compose_formatted_string_from_cstr
      (formatted_string,c_str
      ,quotation                                                                 //200220
      //200220 ,quote_mode                                                       //140922
      ,field_width);                                                             //190106
   write_buffer.append(formatted_string);
   return (nat32)formatted_string.length();
}
//_write_field_string_______________________________________________2007-02-03_/
void CORN::Tabular_text_file::dump_column_header_labels(Data_record &data_rec)
{  // This dumps the column headers (this should be an option based on delineation)
   std::string record_buffer;
   bool first_item = true;
   FOR_EACH_IN(section,VV_Section,data_rec.sections,each_section)                //030709
   {  //for each entry in current section
         FOR_EACH_IN(entry,VV_abstract_entry,section->entries,each_entry)        //030709
         {  // lookup field in actual fields:
            if (!first_item) record_buffer.append("\t");                         //050621
               // NYI output here the delimiter
               first_item = false;
            entry->append_variable(record_buffer);                               //181115
         } FOR_EACH_END(each_entry)
   } FOR_EACH_END(each_section)
   data_stream->write_raw_line(record_buffer.c_str());
}
//_dump_column_header_labels________________________________________2007-03-07_/
const std::string *CORN::Tabular_text_file::get_Nth_header(int header_row_1based)
{  // 1 base index
   // returns a pointer to the specified header row string.
   // return 0 if no header at the specified row.
   return  dynamic_cast<const std::string *>(header_records.get_Nth(header_row_1based));
}
//_get_Nth_header___________________________________________________2010-02-08_/
#ifdef NYI
void CORN::Tabular_text_file::flush_any_preamble_headers_and_column_headers()
{
   // NYI This is called when we first start writing to the file.
   // This should write the respective text lines.
}
//_flush_any_preamble_headers_and_column_headers____________________2007-03-07_/
#endif
}//_namespace CORN_____________________________________________________________/

