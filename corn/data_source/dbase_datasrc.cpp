#ifdef __unix
 // This will define _BIG_ENDIAN if appropriate
#   include <sys/param.h>
#endif
#ifdef debug
#   include "corn/std/std_iostream.h"
#endif

#ifndef compareHPP
#include "corn/math/compare.hpp"
#endif
#include "corn/string/strconv.hpp"

#include <stdlib.h>
// For toupper
#include <ctype.h>
#include <string.h>
// for memset

#include "corn/data_source/datarec.h"
#include "corn/data_source/generic_datarec.h"
#include "corn/data_source/dbase_datasrc.h"
#include "corn/container/indexer.h"
#include "corn/container/text_list.h"
#include "corn/format/binary/binary_file_fstream.h"
#include "corn/chronometry/date_32.h"

//#include "corn/container/search_tree.h"
#define HEADER_BASE_LENGTH  32
#define FIELD_HEADER_LENGTH 32
#define skip_delete_marker 1

using namespace std;

namespace CORN {
//______________________________________________________________________________
// Copy constructor
Data_source_dBase::Field::Field(const Field &from_copy)
: Item()
,type					(from_copy.type)
,data_address		(from_copy.data_address)
,length				(from_copy.length)
,decimals			(from_copy.decimals)
,reserved_18_19	(from_copy.reserved_18_19)
,work_area_ID		(from_copy.work_area_ID) // Perhaps should initialize to 0 in all other version?
,reserved_21_22	(from_copy.reserved_21_22)
,set_fields_flag	(from_copy.set_fields_flag)
// set below,reserved_24_30	(from_copy.reserved_24_30)
,MDX_field_flag	(from_copy.MDX_field_flag)
{  strncpy(name,(char *)from_copy.name,11);
   strncpy((char *)reserved_24_30,(char *)from_copy.reserved_24_30,12);
}
//_Field:constructor____________________________________2008-08-04__1997-08-02_/
Data_source_dBase::Header::Header()
: version							(0x03) // Currently no support for .DBT files
, update_year 						   (1)
, update_month 					   (8)
, update_dom 							(4)
, num_records 							(0) // We are creating a new file
, header_length						(0) // The header length is set when writing the first record because we won't know what this is until the fields are provided
, record_length                  (0)
, reserved_bytes_12_13				(0) // Byte12-13Reserved and zero filled in all version of dBase III,IV, 5
, incomplete_transaction_flag		(0) // 00h Transaction ended (or rolled back)
, encryption_flag						(0) // not encrypted
//, reserved_16_27 set below
, production_MDX_flag				(0) // 00h No MDX file (Currently this class always writes 0 since we don't work with MDX file
, lanuage_driver_ID					(0) // 0 is default because not used in dBaseIII presume that all other versions will accept 0
, reserved_30_31						(0)
{  memset(reserved_16_27,0,12);
   CORN::Date_clad_32  today;
   Year year; CORN::Month month; CORN::DOM dom;
   today.gregorian(year,month,dom);
   update_year = year; // We need to get this from the system
   update_month = month;
   update_dom = dom;
}
//_Header:constructor___________________________________1997-08-02__2008-08-04_/
Data_source_dBase::Header::Header(const Header &to_copy)
: version                        (to_copy.version)
, update_year                    (to_copy.update_year)
, update_month                   (to_copy.update_month)
, update_dom                     (to_copy.update_dom)
, num_records                    (0)  // we are only coping the structure so the copied database initially has 0 records.
, header_length                  (to_copy.header_length)
, record_length                  (to_copy.record_length)
, reserved_bytes_12_13           (to_copy.reserved_bytes_12_13) // Byte12-13Reserved and zero filled in all version of dBase III,IV, 5
, incomplete_transaction_flag    (to_copy.incomplete_transaction_flag) // 00h Transaction ended (or rolled back)
, encryption_flag                (to_copy.encryption_flag) // not encrypted
//   , reserved_16_27 set below
, production_MDX_flag            (to_copy.production_MDX_flag) // 00h No MDX file (Currently this class always writes 0 since we don't work with MDX file
, lanuage_driver_ID              (to_copy.lanuage_driver_ID) // 0 is default because not used in dBaseIII presume that all other versions will accept 0
, reserved_30_31                 (to_copy.reserved_30_31)
{  strncpy((char *)reserved_16_27,(char *)to_copy.reserved_16_27,12);
}
//_Header:constructor___________________________________1997-08-02__2008-08-04_/
void Data_source_dBase::Header::write(Binary_file_interface &f)
{  f.write_nat8(version);
   f.write_nat8(update_year);
   f.write_nat8(update_month);
   f.write_nat8(update_dom);
   f.write_nat32(num_records);
   f.write_nat16(header_length);
   f.write_nat16(record_length);
   f.write_nat16(reserved_bytes_12_13);
   f.write_nat8(incomplete_transaction_flag);
   f.write_nat8(encryption_flag);
   f.write_c_str(reserved_16_27,ASCII_string,12);
   //100114    f.write_ASCIx(reserved_16_27,Binary_file_interface::ASCII_string,12); //080804_
   f.write_nat8(production_MDX_flag);
   f.write_nat8(lanuage_driver_ID);
   f.write_nat16(reserved_30_31);
}
//_Header::write________________________________________1997-08-02__2008-08-04_/
bool Data_source_dBase::Header::read(Binary_file_interface &f)
{
  version         = f.read_nat8();
  update_year     = f.read_nat8();
  update_month    = f.read_nat8();
  update_dom      = f.read_nat8();
  num_records     = f.read_nat32();    //f.read_sint32();
  header_length   = f.read_nat16();
  record_length   = f.read_nat16();
  reserved_bytes_12_13   = f.read_nat16();
  incomplete_transaction_flag      = f.read_nat8();
  encryption_flag      = f.read_nat8();
  f.read_c_str(reserved_16_27,ASCII_string,12);
  production_MDX_flag= f.read_nat8();
  lanuage_driver_ID  = f.read_nat8();
  reserved_30_31     = f.read_nat16();
   return version > 0; // assume we were successful if the version is valid
}
//_Header::read_________________________________________1997-08-02__2008-08-04_/
bool Data_source_dBase::is_creating()                               affirmation_
{  // If the actual_fields have not been defined yet, then we must be creating this database
      return (!actual_fields.count());
}
//_creating______________________________________________2009-10-31_2001-01-04_/
bool Data_source_dBase::initialize()                                  stream_IO_
{
   if (is_initialized()) return true;
   binary_file->seek(0,std::istream::end);
   nat32 filelen = binary_file->tell();   // Note this can be local if I keep this in initialize
   bool file_existed = filelen;
   binary_file->seek(0);
   // We always call define fields so derived class can create local instances.
   // We must call this last because dbase has priority.
   // Eventually fields list will be passed to dbase constructor.
   if (file_existed)
   {  if (binary_file->good())
      {  if (header.read(*binary_file ))
         {  nat16 pos = 0;
            int16 field_num;
            int num_fields = (header.header_length - HEADER_BASE_LENGTH - 1) / FIELD_HEADER_LENGTH;
            for (field_num = 1; field_num <= num_fields; field_num++)
            { // Now read the fields info
               int32 field_record_offset = (int32)HEADER_BASE_LENGTH + ((int32)(field_num - 1) * (int32)FIELD_HEADER_LENGTH);
               binary_file->seek(field_record_offset);
                Field_item *actual_field = new Field_item(*this);
                actual_field->pos = pos;
                actual_fields.append(actual_field);
                pos += actual_field->length;
            }
         }
         initialized = true;
      }
   }
   if (indexer)
   {  // Goto the beginning of the file
      char field_name[256];
      char field_value[256];  // dBase limits field lengths to 255
       int32 record_1_offset = (int32)header.header_length;
       binary_file->seek(record_1_offset);
      char *record_buffer = new char[header.record_length + 1]; // Give one extra character for null termination
      for (nat32 record_num = 1; record_num <= (nat32)(header.num_records); record_num++)
      {  // read each record into a buffer
         binary_file->read_c_str(record_buffer,ASCII_string,header.record_length);
         int key_count = 0;
         int field_start_pos = 1; // Position 0 is the delete marker
         FOR_EACH_IN(field,Field_item,actual_fields,each_field)
         {    strncpy(field_name,(char *)field->name,12);
              field_name[12] = 0;  // strncpy does not append 0
            nat8 key_field_sequence = indexer->get_key_sequence(field_name,0);
            if (key_field_sequence)
            {
               strncpy(field_value,(char *)(record_buffer+field_start_pos),field->length);
               field_value[field->length] = 0; // strncpy does not append 0
               indexer->know_value(field_value , field_name ,0 );
               key_count++;
            }
            field_start_pos += field->length;
         } FOR_EACH_END(each_field)
         indexer->know_position(record_num);
         indexer->commit();
      }
      // goto back to the beginning of the file
      binary_file-> seek(record_1_offset);
      delete [] record_buffer;
   }
   return initialized;
}
//_initialize___________________________________________2005-11-08__2000-01-19_/
Data_source_dBase::Data_source_dBase
(const char *filename_
,std::ios_base::openmode openmode_                                               //111026
,Indexer *optional_indexer)
: Data_table(optional_indexer)                                                   //011113
, binary_file (new Binary_file_fstream(filename_,false,openmode_))               //111026_070622
, current_record(0)
, deleted_shown(true)
, current_record_deleted(false)
, modified(0)
, deleted_modified(false)
, initialized(false)
, header()
, next_field_for_write_pos(0)                                                    //091030
{}
//_Data_source_dBase:constructor_______________________________________________/
const char *Data_source_dBase::label_string(std::string &buffer)           const
{
   // This returns a text label that could be used to identify
   // The data source being edited.
   // It is left to the derived data source class how this text is formatted.
   // For VV_File we simply use the fully qualified filename.
   // For dBase we could use the filename and the record number  C:\path\file.dbf#123
   buffer = binary_file->get_file_name();
   return buffer.c_str();
}
//_label_string_____________________________________________________2001-04-23_/
bool Data_source_dBase::position_record_pointer(long record_num)
{
   if (!is_initialized()) if (initialize()) return false;                        //041210
   int32 record_offset = (int32)header.header_length +
    (record_num ?                                                                //010511
     (int32)(record_num - 1) * (int32)header.record_length
    : 0);                                                                        //010511
   bool result = binary_file-> seek(record_offset);
   return result;
}
//_position_record_pointer__________________________________________1997-08-03_/
Data_source_dBase::Data_source_dBase
( const char *filename_                                                          //020125
, const Data_source_dBase &from_copy)  // For an existing file
: Data_table()
, binary_file (new Binary_file_fstream(filename_,false,std::ios_base::in|std::ios_base::out)) //111026_070622
, current_record(0)
, deleted_shown(from_copy.deleted_shown)
, current_record_deleted(from_copy.current_record_deleted)
, modified(from_copy.modified)
, deleted_modified(from_copy.deleted_modified)
, initialized(true)                                                              //000113
, header(from_copy.header)                                                       //010120
, next_field_for_write_pos(0)                                                    //091030
{  FOR_EACH_IN(field_from_copy,Field_item,from_copy.actual_fields,each_field)    //030709
   {   actual_fields.append(new Field_item(*field_from_copy));
      next_field_for_write_pos += field_from_copy->get_length();                 //091030
   } FOR_EACH_END(each_field)
}
//          This copy constructor is to create a new file having the same structure as the one to copy.
//_Data_source_dBase:constructor____________________________________1997-08-02_/
bool Data_source_dBase::create_header()
{  // Each record also gets a delete indicator field
   nat32 actual_fields_count = actual_fields.count();                            //050618
   if (!actual_fields_count) return false;                                       //080718
   // when writing/creating a new file, we need to know the fields inorder to    //080718
   // setup the data. Data may become available later when we attempt            //080718
   // to write the first record when we should have the record structure.        //080718
   header.header_length = (nat16)(HEADER_BASE_LENGTH +  // for fieldfield terminator //980403
         actual_fields_count * FIELD_HEADER_LENGTH +1);  // +1 is the deleted indicator field
   header.record_length = 1;  // 1 for the delete marker
   FOR_EACH_IN(field,Field_item,actual_fields,each_field)                        //030709
   {   header.record_length += field->get_length();
   } FOR_EACH_END(each_field)                                                    //030709
   binary_file->seek(0);
   header.write(*binary_file);
   modified = true;                                                              //050215
   write_fields();
   binary_file->write_nat8(13);  // field terminator
   initialized = true;                                                           //050618
      // This is needed now because position_record_pointer
      // will check if not initialized and will try to read the header
   current_record = 0;
   position_record_pointer(current_record);
   return true;
}
//_create_header____________________________________________________1997-07-25_/
bool Data_source_dBase::create(Data_record &data_rec)
// When we use get() and we have not yet created the database
// We need to create the list of actual fields based on the from_record
{  nat16 field_pos = 0;
   if (expected_fields.count() && !actual_fields.count())                        //091031
      actual_fields.transfer_all_from(expected_fields);                          //091031
   else
   FOR_EACH_IN(section,VV_Section,data_rec.sections,each_section)                //030709
   {  //for each entry in current section:
      FOR_EACH_IN(entry,VV_abstract_entry,section->entries,each_entry)           //030709
      {  // lookup field in actual fields:
         std::string entry_label; entry->append_variable(entry_label);           //180820
         unsigned char entry_type = entry->get_type();
         char field_type = dB_character;
         nat16 field_length = entry->get_max_strlen();
         nat8 field_decimals = 0;
         switch (entry_type)
         {  case VV_unknown  : field_type = dB_character; break;
            case VV_string   : field_type = dB_character; break;
            case VV_int8     : field_type = dB_number   ; break;
            case VV_int16    : field_type = dB_number   ; /*length =   6;*/   break;
            case VV_int32    : field_type = dB_number   ; /*length =  11;*/   break;
            case VV_nat16    : field_type = dB_number   ; /*length =   5;*/   break;
//NYI       case VV_nat8     : field_type = dB_number   ; /*length =   3;*/   break;
            case VV_nat32    : field_type = dB_number   ; /*length =  10;*/   break;
            case VV_float32  : field_type = dB_number   ; /*length =  10;*/   field_decimals = 5; break;
            case VV_float64  : field_type = dB_number   ; /*length =  16;*/   field_decimals = 8; break;
            // dBase IV introduced a floating point binary numeric 'F' dB_float
            // I don't know what format this is (The documentation I've found seems inconsisten.
            // I currently am always creating files with 'N' numeric
            // since this is compatible will all versions of dBase
            case VV_bool      : field_type = dB_logical  ; break;
            case VV_date      : field_type = dB_date     ; field_length = 8; break;
            case VV_byte      : field_type = dB_character; /*length = 1;   */ break;
            case VV_char_array: field_type = dB_character; /*length = ???; */ break;
            case VV_filename  : field_type = dB_character; /*length = ???; */ break;
            case VV_enum      : field_type = dB_character; /*length = ???; */ break;
               // The max length of enum  we would know from the label table
         }
         //010108 Warning we will want to contract the entry key to maximim field name size (12 bytes)
         Field_item *field = new Field_item
         (entry_label.c_str(),field_type,field_length,field_decimals,field_pos); //180820_030117
         actual_fields.append(field);
         field_pos += field_length;
      } FOR_EACH_END(each_entry)
   } FOR_EACH_END(each_section)
   bool header_created = create_header();                                        //030117
   return header_created;
}
//_create___________________________________________________________2001-01-03_/
bool Data_source_dBase::expect_field_for_write
( const char *new_name                // field name in ASCII zero-filled
, char new_type                 // (C N L D or M)
, nat8 new_length    // field length in binary
, nat8 new_decimals) // field decimal count in binary
{  if (is_initialized()) return false; // can't add fields if we've already initialized
   // Also should check if the file is open for read
   expected_fields.append(new Field_item(new_name,new_type ,new_length,new_decimals,next_field_for_write_pos));
   next_field_for_write_pos += new_length;
   return true;
}
//_expect_field_for_write___________________________________________2009-10-30_/
bool Data_source_dBase::is_record_modified()                        affirmation_
{  bool a_field_modified = false;
   FOR_EACH_IN(field,Field_item,actual_fields,each_field)                        //030709
       if (field->modified)
          a_field_modified = true;
   FOR_EACH_END(each_field)
   return a_field_modified || modified;                                          //980216
}
//_is_record_modified_______________________________________________1997-07-20_/
Data_source_dBase::~Data_source_dBase()
{  if (modified)
   {  binary_file->seek(4);
      binary_file->write_nat32(header.num_records);
   }
   delete binary_file; binary_file = 0;
}
//_Data_source_dBase:destructor_________________________2008-07-18__2007-05-25_/
bool Data_source_dBase::finalize()    stream_IO_
{  if (modified)
   {  binary_file->seek(4);
      binary_file->write_nat32(header.num_records);
   }
   return true;
}
//_finalize_____________________________________________2008-07-18__1997-07-20_/
void Data_source_dBase::write_fields()
{  FOR_EACH_IN(field,Field_item,actual_fields,each_field)
     field->Field::write_to(*this);
   FOR_EACH_END(each_field)
}
//_write_fields_____________________________________________________1997-07-20_/
bool Data_source_dBase::goto_record(int32 record_number)
{
   if (!is_initialized()) if (!initialize()) return false;                       //010514
   bool in_bounds = (record_number >= 1) && (record_number <= header.num_records);
   // If the requested position is beyond the bounds of the database
   // set set the current_record to 0 this will cause any following get
   // to create a new record. 010104
   current_record = in_bounds ? record_number : 0;
   position_record_pointer(current_record);
   // Determine if the current record is deleted, but only if the record is within the database range
   // This is used for goto_next etc..
    if (in_bounds)                                                               //010511
   {  char deleted_marker;
      deleted_marker= binary_file->read_nat8();
      current_record_deleted = deleted_marker == '*';
      in_bounds = position_record_pointer(current_record);
   }
   return(in_bounds);
}
//_goto_record_________________________________________________________________/
bool Data_source_dBase::set_data(Data_record &data_rec,bool append)              //150503
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
   bool sat = false;
   if (is_creating())
      create(data_rec);
   modified += 1;
   if (!data_rec.get_position())
   {  header.num_records += 1;
      current_record = data_rec.set_position(header.num_records); // set_number(num_records);
   }
   if (position_record_pointer(current_record))
   {  // Fill record buffer
      char *record_buffer = new char[header.record_length];
      memset(record_buffer,' ',(size_t)header.record_length);
      FOR_EACH_IN(section,VV_Section,data_rec.sections,each_section)             //030709
      {     //for each entry in current section
            FOR_EACH_IN(entry,VV_abstract_entry,section->entries,each_entry)     //030709
            {  // lookup field in actual fields:
               std::string entry_label; entry->append_variable(entry_label);        //181115
               // Note that lookup_field will truncate as needed
               Field_item *field = lookup_field(entry_label.c_str());   // Note I want to do the same for oracle //030117
               if (field)
               {
                  std::string value_string;                                      //140516
                  entry->append_to_string(value_string);                         //140516
                  int copy_length = std::min<int>
                     ((int)field->get_length(),(int)value_string.length());      //140615
                  int tab_pos = 0;
                  unsigned char vv_type = entry->get_type();
                  switch (vv_type)
                  {
                     case VV_int8      :
                     case VV_int16     :
                     case VV_int32     :
                     case VV_nat16    :
#ifdef NOT_YET_IMPLEMENTED
                     case VV_nat8     :
#endif
                     case VV_nat32    :
                     case VV_float32  :
                     case VV_float64  :
                        tab_pos = field->get_length() - copy_length;
                     break;
                     default : tab_pos = 0; break;
                  }
                  strncpy(record_buffer+field->pos+tab_pos+skip_delete_marker
                         ,(char *)value_string.c_str(),copy_length);              //140516
                  sat = true; // If any field is encountered then we accept this data
               }
               // else the field was simply not encountered
               entry->set_encountered(field);
            }
            FOR_EACH_END(each_entry)
      } FOR_EACH_END(each_section)
      binary_file->write_c_str(record_buffer,ASCII_string,(int)header.record_length);  //100114
      delete [] record_buffer;
   }
   return sat;
}
//_set_data_________________________________________________________2001-01-03_/
nat32 Data_source_dBase::new_record(int16 num_new_records)
{
   if (!initialized) return 0;  // should return false                           //080718
   // We used to be able to simply position the record point                     //080718
   // beyond the end of the file to extend the file.
   // Because later versions of the Borland compiler
   // (maybe std C/C++ did not allow me to open the a non existent file r+b
   // it may be the opening the the file rb does not allow this
   // anywhy I now specifically write the new records at the end of the file:

   binary_file-> seek(0, std::ios::end);
   nat32 file_pos = binary_file->tell();
   for (int r = 1; r <= num_new_records; r++)
   {  for (int rec_index=0
            ; rec_index <= header.record_length+1  // +1 is for delete marker
            ; rec_index++)
         binary_file->write_nat8(' ');
   }
   nat32 first_of_the_new_records = header.num_records + 1;                      //080729
   header.num_records += num_new_records;
   modified = 1;  // we always force the current record to be modified when newing because we might want empty records etc.
   binary_file-> seek(file_pos, std::ios::beg);                                  //080718
   current_record = header.num_records;                                          //080718
   return  first_of_the_new_records;                                             //080729
}
//_new_record__________________________________________________________________/
bool Data_source_dBase::goto_BOF()
{  return(goto_record(1));
}
//_goto_BOF____________________________________________________________________/
bool Data_source_dBase::goto_EOF()
{  return(goto_record(header.num_records));
}
//_goto_EOF____________________________________________________________________/
bool Data_source_dBase::goto_next(int32 skip)
{  if (!goto_record(current_record + skip))  return false;
   if (deleted_shown)                        return true;
   while (is_record_deleted())
      if (!goto_record(current_record+1))    return false;
   return true;
}
//_goto_next___________________________________________________________________/
bool Data_source_dBase::goto_prev(int32 skip)
{
   if (!goto_record(current_record - skip))  return false;
   if (deleted_shown)                        return true;
   while (is_record_deleted())
      if (!goto_record(current_record-1))    return false;
   return true;
}
//_goto_prev___________________________________________________________________/
char *variable_to_dBase_field_name(const char *variable,char *field_name)
{  // Note I want to do the same for oracle
  strncpy(field_name,(char *)variable,11);
   field_name[11] = 0;                                                          //050920
   // dBase field names cannot have spaces ("Excel converts spaces to underscore) so will we.
   for (int i = 0; i < 12; i++)
   {  if (field_name[i] == ' ')
           field_name[i] = '_';
      else field_name[i] = field_name[i]; //190217 toupper(field_name[i]);
      // although dBase itself only specifies uppercase field names
      // Other application writing dBase files allow lowercase
   }
  return field_name;
}
//_variable_to_dBase_field_name_____________________________________2002-07-24_/
bool Data_source_dBase::reset()
{  return binary_file->reset(); }
//_reset____________________________________________________________2011-11-11_/
Data_source_dBase::Field_item *Data_source_dBase::lookup_field(const char *variable_name)
{   // Note I want to do the same for oracle
   char field_name[255];                                                         //020724
   variable_to_dBase_field_name(variable_name,field_name);                       //020724
   Field_item * found_field
      = dynamic_cast<Field_item *>(actual_fields.find_cstr(field_name,true/*171008 false*/));  //must not be case sensative  050620
   //170928      = (Field_item *)actual_fields.find_cstr(field_name,false);  //must not be case sensative  050620
   // Excel truncates variables to 10 characters instead of eleven
   // we will try looking for a 10 character field name just for Excel (MICROSOFT!!!!!)
   if (! found_field)                                                            //020724
   {  field_name[10] = 0;                                                        //030125
      found_field
      = dynamic_cast<Field_item *>(actual_fields.find_cstr(field_name,false));  //must not be case sensative  050620
      //170928 = (Field_item *)actual_fields.find_cstr(field_name,false);      //must not be case sensative   //050620
   }
   return found_field;
}
//_lookup_field_____________________________________________________2002-07-24_/
nat8 Data_source_dBase::get_field_type(const char *field_name)
{   nat8 result = 0;
   // returns the filed type corresponding to VV_entry types
   Data_source_dBase::Field_item *field = lookup_field(field_name);
   if (field)
   {
      switch(field->type)
      {  case 'C' : result = VV_string; break;
         case 'N' : result = field->decimals ? VV_float32 : VV_int32; break;
         case 'L' : result = VV_bool; break;
         case 'D' : result = VV_date; break;
         case 'M' : result = VV_string; break;
// NYI
//         case 'B' : result = VV_xxxxxx; break; Added dBase5 (Windows only?)
           // I am not sure how 'B' binary worked it seems to be related to
           // DBT files which are not currently supported by this class
//         case 'F' : result = VV_xxxxxx; break; Added dBaseIV
                    // dBase IV introduced a floating point binary numeric 'F' dB_float
                  // I don't know what format this is (The documentation I've found seems inconsisten.

//         case 'G' : result = VV_xxxxxx; break; Added dBase5 (Windows only?)
           // I am not sure how 'G' general worked it seems to be related to
           // DBT files which are not currently supported by this class
      }
   }
   return result;
}
//_get_field_type___________________________________________________2002-11-11_/
nat32 Data_source_dBase::delete_record()
{  current_record_deleted = true;
   deleted_modified = 1;
   return get_record_number();                                                   //031106
}
//_delete_record_______________________________________________________________/
void Data_source_dBase::undelete_record()
{  current_record_deleted = false;
   deleted_modified = 1;
}
//_undelete_record_____________________________________________________________/
Data_source_dBase::Field::Field
(const char *new_name
,char        new_type
,nat8       new_length
,nat8       new_decimals)
:Item()                                                                          //170928
,type(new_type)
,data_address      (0)
,length            (new_length)
,decimals         (new_decimals)
,reserved_18_19   (0)                                                            //080804
,work_area_ID      (1) // Perhaps should initialize to 0 in all other version?    //080804
,reserved_21_22   (0)                                                            //080804
,set_fields_flag   (0)                                                            //080804
// set below,reserved_24_30   (0)                                                //080804
,MDX_field_flag   (0)                                                            //080804
{  memset(name,0,12);                                                            //010415
   memset(reserved_24_30,0,7);                                                   //080804
   variable_to_dBase_field_name(new_name,name);
   switch (new_type)
   {  // The following types are always this length no matter what
      case dB_logical   : length =  1; break;
      case dB_memo      : length = 10; break;
      case dB_date      : length =  8; break;
      case rB_CORN_date : length =  4; break;
      case rB_int16     : length =  2; break;
      case rB_float32  : length =  4; break;
      case rB_float64  : length =  8; break;
      case rB_nat8     : length =  1; break;
      case rB_nat16    : length =  2; break;
      case rB_nat32    : length =  4; break;
   }
}
//_Field:constructor___________________________________________________________/
Data_source_dBase::Field::Field(Data_source_dBase &f)
: CORN::Item()                                                                   //070425
{  memset(name,0,12);                                                            //010516
   read_from(f);
}
//_Field:consturctor___________________________________________________________/
void Data_source_dBase::Field::read_from(Data_source_dBase &f)
{  f.binary_file->read_c_str(name,ASCII_string,11);                              //070622
   type           = f.binary_file->read_nat8();
   data_address   = f.binary_file->read_int32();
   length         = f.binary_file->read_nat8();
   decimals       = f.binary_file->read_nat8();
   reserved_18_19 = f.binary_file->read_nat16();                                 //080804
   work_area_ID   = f.binary_file->read_nat8();                                  //080804
   reserved_21_22 = f.binary_file->read_nat16();                                 //080804
   set_fields_flag= f.binary_file->read_nat8();                                  //080804
   f.binary_file->read_c_str(reserved_24_30,ASCII_string,7);                     //100114
   MDX_field_flag         = f.binary_file->read_nat8();                          //080804
}
//_Field::read_from____________________________________________________________/
void Data_source_dBase::Field::write_to(Data_source_dBase &f)
{  f.binary_file->write_c_str(name,ASCII_string,11);                             //100114
   f.binary_file->write_nat8(type);
   f.binary_file->write_nat32(data_address);
   f.binary_file->write_nat8(length);
   f.binary_file->write_nat8(decimals);
   f.binary_file->write_nat16(reserved_18_19);                                   //080804
   f.binary_file->write_nat8(work_area_ID);                                      //080804
   f.binary_file->write_nat16(reserved_21_22);                                   //080804
   f.binary_file->write_nat8(set_fields_flag);                                   //080804
   f.binary_file->write_c_str(reserved_24_30,ASCII_string,7);                    //100114
   f.binary_file->write_nat8(MDX_field_flag);                                    //080804
}
//_Field::write_to_____________________________________________________________/
// This constructor may be obsolete, I see nor reason for it
Data_source_dBase::Field_item::Field_item(Data_source_dBase &f)
:Field(f)
,modified(0)
{}
//_Field_item:constructor___________________________________________1997-08-20_/
Data_source_dBase::Field_item::Field_item
(const char *_name
,char        _type
,nat8        _length
,nat8        _decimals
,nat16       _pos)                                                               //010104
:Field(_name,_type,_length,_decimals)
,modified(0)
,pos(_pos)                                                                       //010104
{}
//_Field_item:constructor______________________________________________________/
Data_source_dBase::Field_item::Field_item(Field_item &from_copy)
:Field(from_copy)
,modified(false)
{}
//_Field_item:constructor(copy)_____________________________________1997-08-02_/
//050705 this constructor may be obsolete since buffer has been removed
Data_source_dBase::Field_item::~Field_item()
{}
//______________________________________________________________________________
bool Data_source_dBase::Field::is_key_cstr(const char *key)         affirmation_
{   return name == key;
}
//_Field::is_key_cstr_______________________________________________2018-11-15_/
bool Data_source_dBase::Field::is_key_string(const std::string &key)affirmation_  //180820
{  return key == name /*181115 Field_item_name_return*/;
}
//______________________________________________________________________________
const char *Data_source_dBase::Field::get_name()                            const
{    strncpy((char *)Field_item_name_return,(char *)name,12);
      Field_item_name_return[12] = 0;
      return Field_item_name_return;
}
//_Field::get_name__________________________________________________2003-10-17_/
bool Dynamic_string_indexed_dBase_data_source::Index_item::is_key_string
(const std::string &key)                                            affirmation_ //180820
{  return rec_key_value == key;
}
//_is_key_string____________________________________________________2018-08-20_/
nat16 Data_source_dBase::get_pos(const std::string  &field_name)           const
{  nat32 pos = (nat32)std::string::npos;
   FOR_EACH_IN(field,Field_item,actual_fields,each_field)                        //030709
   {  std::string trimed_name(field->get_name());                                //040909
      CORN::strip_string(trimed_name);
      if (field_name == trimed_name)
         return pos;
      pos += field->length;
   } FOR_EACH_END(each_field)
   return pos;
}
//_get_pos__________________________________________________________2004-09-09_/
nat32 Data_source_dBase::get_record_count()
{  if (!is_initialized()) initialize();                                          //031107
   return header.get_record_count();
}
//_get_record_count_________________________________________________1997-10-19_/
bool Data_source_dBase::get(Data_record &data_rec)
{  bool got = data_rec.get_start();                                              //190924_021220
   got &= Data_source::get(data_rec);                                            //190924
   if (!current_record)                                                          //021111
      goto_record(1); // Do not use goto BOF We might not have positioned yet so try to go to BOF  //021111
   if (current_record)
      if (position_record_pointer(current_record))
      {
         data_rec.set_position(current_record);                                  //160927
         char *record_buffer = new char[header.record_length + 1]; // Give one extra character for null termination  //010516
         record_buffer[0] = 0;                                                   //051108
         binary_file->read_c_str(record_buffer,ASCII_string,header.record_length);//100114
         record_buffer[header.record_length] = 0;                                //010516
         current_record_deleted = record_buffer[0] == '*';
         FOR_EACH_IN(section,VV_Section,data_rec.sections,each_section)          //030709
         {
            FOR_EACH_IN(entry,VV_abstract_entry,section->entries,each_entry)     //030709
            {  // lookup field in actual fields:
               std::string entry_label; entry->append_variable(entry_label);     //181115
               Field_item *field = lookup_field(entry_label.c_str());            //181115_030117
               if (field)
               {
                  size_t start = field->pos+skip_delete_marker;                  //010516
                  size_t len = field->length;                                    //010516
                  std::string value_str(std::string (record_buffer),start,len);  //040909
                  // dBase space fills fields (especially string fields), C++ strings tend to be null terminated.
                  // I currently presume that all strings should be stripped, especially filenames.
                  CORN::strip_string(value_str,Both);                                 //130919_010516
                  if (field->type == 'D')                                        //021212
                  {  // dBase date are in the format YYYYMMDD                    //021212
                     // set_str needs date string YYYYDDD                        //021212

                     #ifdef REIMPLEMENT

                     //170525 This needs to be reimplemented
                     // to use the new date classes

                     Date field_date(value_str.c_str(),D_YMD,D_YYYY|D_lead_zero|D_M,0,false); //021212
                     field_date.set_format(D_YD,D_YYYY|D_lead_zero,0,false);     //141120_021212
                     value_str.clear();                                          //140616
                     field_date.append_to_string(value_str);                     //140616

                     #endif

                  }
                  entry->set_str(value_str.c_str());
                  got = true; // If any field is encountered then we accept this data
               }
               // else the field was simply not encountered
               entry->set_encountered(field);
            } FOR_EACH_END(each_entry)
         } FOR_EACH_END(each_section)
         delete [] record_buffer;
      }
   data_rec.get_end();                                                           //021220
   return got;
}
//_get______________________________________________________________2001-01-03_/
bool Data_source_dBase::get_next(Data_record &data_rec)
{  // A where clause index more than one record.
   // In this case, use get() to fetch the first record and
   // repeat calling get_next() to fetch remaining records.
   // get_next will return false if there were no additional
   // records returned by the SELECT WHERE statement to fetch.
   // Note that this abstract class ALWAYS returns false because this abstract class does not
   // actually fetch the data, it only set up the the structure.
   // Derived classes must return true unless the data could not be fetched.
   bool got = goto_next();
   if (got) got = get(data_rec);
   return got;
}
//_get_next_________________________________________________________2002-09-14_/
Generic_data_record *Data_source_dBase::render_generic_data_record()  stream_IO_ //160831
{  initialize(); // In most cases we have only just openned the database need to initialize to get fields.
   Generic_data_record *created_DR = new Generic_data_record("none");  // dBase tables don't have sections
   FOR_EACH_IN(field,Field_item,actual_fields,each_field)
   {  switch(field->type)
      {  case dB_character :  created_DR->expect_string(field->name,created_DR->strings[(created_DR->string_count++)],field->length);  break;
         case dB_number    :
         {
            //170511 if (field->decimals == 0)  created_DR->expect_int16(field->name,created_DR->ints[(created_DR->int_count++)]);
            if (field->decimals == 0)  created_DR->expect_int32(field->name,created_DR->int32s[(created_DR->int_count++)]);
            else                       created_DR->expect_float32(field->name,created_DR->floats[(created_DR->float_count)++]);
         } break;
         case dB_float     :           created_DR->expect_float32(field->name,created_DR->floats[(created_DR->float_count)++]);        break;
         case dB_logical   :           created_DR->expect_bool(field->name,created_DR->bools[(created_DR->bool_count)++]);             break;
         case dB_memo      : created_DR->expect_string(field->name,created_DR->strings[(created_DR->string_count++)],field->length);  break;
         case dB_date      :
            created_DR->expect_int32(field->name,created_DR->int32s[(created_DR->int_count++)]);
            //170525 created_DR->expect_date(field->name,created_DR->dates[(created_DR->date_count++)]);
            break;
         // Default : currently simply an unexpected field but probably could be string
      }
   } FOR_EACH_END(each_field)
   created_DR->structure_defined = true;
   return created_DR;
}
//_render_generic_data_record_______________________________________2005-09-28_/
nat32 Data_source_dBase::append(Data_record &data_rec)
{  // This creates a new record at the end of the table and sets data_rec.
   // Returns the record index of the new record that was appended.
   nat32 record_number = new_record();
   data_rec.set_position(record_number);                                         //080729
   set(data_rec);
   return get_record_count();
}
//_append___________________________________________________________2003-11-06_/
nat32 Dynamic_string_indexed_dBase_data_source::find_record
(const char *search_key_cstr,bool from_first_record)                  stream_IO_
{  nat32 found_position = 0;                                                     //060130
   if (!index_built)
   {  from_first_record = true;                                                  //021111
      // We have to goto to the first record if we build the index
      // otherwise we will be at the end if the index and would never
      // read the record.
      build_index();
   }
   Index_item *item = (Index_item *)indicies.find_cstr(search_key_cstr,true);    //051101
   if (item)                                                                     //051101
      {  goto_record(item->rec_num);
         found_position = item->rec_num;                                         //060130
      }
   return found_position;                                                        //060130
}
//_find_record______________________________________________________2002-09-14_/
Dynamic_indexed_dBase_data_source::Dynamic_indexed_dBase_data_source
(const char *filename_
,const char *index_field_name_
,std::ios_base::openmode openmode_)                                              //111026
:Data_source_dBase(filename_,openmode_)                                         //111026_021208
,index_built(false)                                                              //000118
,where_mode(false)                                                               //021111
,where_found(false)                                                              //021111
,index_field_name(index_field_name_)                                            //000119
{}
//_Dynamic_indexed_dBase_data_source:constructor____________________2001-05-09_/
Dynamic_string_indexed_dBase_data_source::Dynamic_string_indexed_dBase_data_source
(const char *filename_                                                           //000113
,const char *index_field_name_
,std::ios_base::openmode openmode_)                                              //111026
:Dynamic_indexed_dBase_data_source(filename_,index_field_name_,openmode_)      //111026_970720
,indicies()                                                                      //970720
{}
//_Dynamic_string_indexed_dBase_data_source:constructor_____________2001-05-09_/
Dynamic_int32_indexed_dBase_data_source::Dynamic_int32_indexed_dBase_data_source
(const char *filename_                                                           //000113
,const char *index_field_name_
,std::ios_base::openmode openmode_)                                              //111026
:Dynamic_indexed_dBase_data_source(filename_,index_field_name_,openmode_)      //111026_970720
,indicies()                                                                      //970720
{}
//_Dynamic_int32_indexed_dBase_data_sourceconstructor_______________2001-05-09_/
bool Dynamic_indexed_dBase_data_source::goto_BOF()
{  where_mode = false;
   return Data_source_dBase::goto_BOF();
}
//_goto_BOF_________________________________________________________2002-11-11_/
void Dynamic_string_indexed_dBase_data_source::build_index()     stream_IO_
{  for (bool at_end = !goto_BOF(); !at_end; at_end = !goto_next())
     indicies.append(new Index_item(*this,index_field_name.c_str()));            //970720
   index_built = true;
}
//_build_index______________________________________________________2001-05-09_/
void Dynamic_int32_indexed_dBase_data_source::build_index()    stream_IO_
{  for (bool at_end = !goto_BOF(); !at_end; at_end = !goto_next())
   {  Index_item *index_item = new Index_item(*this,index_field_name.c_str());
      indicies.append(index_item);
   }
   index_built = true;
}
//_build_index______________________________________________________2001-05-09_/
bool Dynamic_int32_indexed_dBase_data_source::find_record
(int32 search_key,bool from_first_record)                             stream_IO_
{  bool found = false;
   if (!index_built) build_index();
   Container::Iterator *index_iterator = indicies.iterator();
   for (Index_item *item =
         from_first_record ? (Index_item *)index_iterator->first()
                           : (Index_item *)index_iterator->next()
       ;item && !found
       ;item = (Index_item *)index_iterator->next()
       )
      if ((item->rec_key_value == search_key))
      {  goto_record(item->rec_num);
         found = true ;
      }
   delete index_iterator;
   return found;
}
//_find_record__________________________________________2003-07-09__2001-05-09_/
bool Dynamic_int32_indexed_dBase_data_source::get_next(Data_record &data_rec)
{  bool got = where_mode                                                         //021111
   ? find_record(last_where_value,false)
   : goto_next();                                                                //021111
   if (got)
      got = get(data_rec);
   return got;
}
//_get_next_________________________________________________________2002-09-14_/
bool Dynamic_string_indexed_dBase_data_source::get_next(Data_record &data_rec)
{  bool got = where_mode                                                         //021111
   ? (bool)find_record(last_where_value.c_str(),false)
   : (bool)goto_next();                                                          //021111
   if (got) got = get(data_rec);
   return got;
}
//_get_next_________________________________________________________2002-09-14_/
void Dynamic_int32_indexed_dBase_data_source::where_long
(const char *attribute, const long value) modification_
{  last_where_value = value;                                                     //020914
   where_mode = true;                                                            //021111
   if (attribute == index_field_name)
         find_record(value);
   else where2_long(attribute, value,attribute, value);
      // using where_long2 here is just a hack so I don't have to write another search.
}
//_where_long__________________________________________________________________/
class Hack_datarec
: public Data_record
{
public:
   Hack_datarec()
   :Data_record("Hack")
   {}
};
//_Hack_datarec________________________________________________________________/
void Dynamic_int32_indexed_dBase_data_source::where2_long
(const char *attribute1, const long value1
,const char *attribute2, const long value2)                        modification_
{  bool at_eof = !goto_BOF();
   bool found = false;
   do
   {  int32 record_value1;
      int32 record_value2;
      Hack_datarec hack_rec;
      hack_rec.expect_int32(attribute1,record_value1);
      hack_rec.expect_int32(attribute2,record_value2);
      get(hack_rec);
      found = (record_value1 == value1) && (record_value2 == value2);
      if (!found)
         at_eof = !goto_next();
   }  while (!found && !at_eof);
   // WARNING, this is just a hack to test LADSS under Windows with dbase tables
   // it works but is slow.
   // Not yet implemented, currently I have this as a stub
   // for compiling LADSS cropsyst.
   // Eventually I do what to implement this or something better.
}
//_where2_long_________________________________________________________________/
void Dynamic_int32_indexed_dBase_data_source::where_cstr
(const char *attribute, const char *value)                         modification_
{  if (attribute == index_field_name)
   {  // we presume value is a numeric since this is implied
      long value_int = atol(value);
      where_long(attribute,value_int);
   } else // the where statement is not for the indexed field we will have to do an exhaustive search
      Dynamic_indexed_dBase_data_source::where_cstr(attribute, value);
}
//_where_cstr__________________________________________________________________/
class Index_int32_data_record : public Data_record
{public:
  std::string key_field_name;                                                    //040909
  int32 key_field_value;
 public:
   inline Index_int32_data_record(const std::string  &_key_field_name)  // 040909 I think this should be const char   //040909
      : Data_record("-") // dBase does not use sections.
      , key_field_name(_key_field_name)
      {}
   inline virtual bool expect_structure(bool for_write)
   {  bool expected = Data_record::expect_structure(for_write);                  //161023_030123
      structure_defined = false;                                                 //120314
      expect_int32(key_field_name.c_str(),key_field_value);                      //030117
      structure_defined = true;                                                  //120314
      return expected;                                                           //161023
   }
};
//_Index_int32_data_record__________________________________________2001-05-09_/
Dynamic_int32_indexed_dBase_data_source::Index_item::Index_item
(modifiable_ Dynamic_indexed_dBase_data_source &file // file cannot be const because we perform IO //090813
,const char *field_name)
: Item()
, rec_num(file.get_record_number())
{  Index_int32_data_record rec_to_index(field_name);
   rec_num = file.get_record_number();
   file.get(rec_to_index);
   rec_key_value = rec_to_index.key_field_value;
}
//_Index_item:constructor______________________________________________________/
Dynamic_string_int32_indexed_dBase_data_source::Index_item::Index_item
(const std::string &_rec_key1_string_value
,int32 _rec_key2_int_value)
: rec_key1_string_value(_rec_key1_string_value)
, rec_key2_int_value(_rec_key2_int_value)
, rec_num()
{}
//_Index_item:constructor______________________________________________________/
class Index_string_data_record : public Data_record
{public:
  std::string key_field_name;                                                    //040909
  std::string key_field_value;                                                   //040909
 public:
   inline Index_string_data_record(const std::string &_key_field_name)   // I think this should be const char 040909
      : Data_record("-") // dBase does not use sections.
      , key_field_name(_key_field_name)
      {}
   inline virtual bool expect_structure(bool for_write)
   {  bool expected = Data_record::expect_structure(for_write);                  //161023
      structure_defined = false;                                                 //120314
      expect_string(key_field_name.c_str(),key_field_value,255);                 //030117
         // Don't care about field length, it will be read from the file
      structure_defined = true;                                                  //120314
      return expected;
   }
};
//_Index_string_data_record_________________________________________2001-05-09_/
Dynamic_string_indexed_dBase_data_source::Index_item::Index_item
(modifiable_ Dynamic_indexed_dBase_data_source &file                             //090813
   // file cannot be const because we perform IO
,const char *field_name)
: Item()                                                                         //170928
, rec_num(file.get_record_number())
{   Index_string_data_record rec_to_index(field_name);
   rec_num = file.get_record_number();
   file.get(rec_to_index);
   rec_key_value = rec_to_index.key_field_value;                                 //020125
}
//_Index_item:constructor______________________________________________________/
void Dynamic_string_indexed_dBase_data_source::where_long
(const char *attribute, const long value) modification_
{  if (attribute == index_field_name)
   {  // since value is numeric, we need to convert it to a string.
      char value_str[10];
      CORN::int32_to_cstr(value,value_str,10);
      where_cstr(attribute,value_str);                                           //021015
   } else Dynamic_indexed_dBase_data_source::where_long(attribute, value);
}
//_where_long_______________________________________________________2002-06-12_/
void Dynamic_string_indexed_dBase_data_source::where_cstr
(const char *attribute, const char *value) modification_
{  where_mode = true;
   where_found = false;                                                          //021111
   if (attribute == index_field_name)
      where_found = (bool)find_record(value);                                    //021111
   else Dynamic_indexed_dBase_data_source::where_cstr(attribute, value);
      // the where statement is not for the indexed field we will have to do an exhaustive search
}
//_where_cstr_______________________________________________________2002-06-12_/
bool Dynamic_string_int32_indexed_dBase_data_source::find_record
(const char *search_value1
,int32 search_value2
,bool from_first_record)                                              stream_IO_
{
   if (!index_built)
   {  from_first_record = true;                                                  //021111
      // We have to goto to the first record if we build the index
      // otherwise we will be at the end if the index and would never
      // read the record.
      build_index();
   }
   bool found = false;                                                           //030709
   Dynamic_string_int32_indexed_dBase_data_source::Index_item comparable_index(search_value1,search_value2);
   Index_item *item = (Index_item *)indicies.find_comparable(comparable_index,true); //051101
   if (item)                                                                     //051101
   {  goto_record(item->rec_num);
      found = true;
   }
   return found;
}
//_find_record______________________________________________________2002-09-14_/
int  Dynamic_string_int32_indexed_dBase_data_source::Index_item::compare(const Item &other) const
{
   const Dynamic_string_int32_indexed_dBase_data_source::Index_item &other_index //141111
      = dynamic_cast<const Dynamic_string_int32_indexed_dBase_data_source::Index_item &>
         (other);
   // compare the string
   int comparison = rec_key1_string_value.compare(other_index.rec_key1_string_value);
   if (comparison == 0)
   {  // The string matches, now compare int
      if       (rec_key2_int_value <  other_index.rec_key2_int_value)   comparison = -1;
      else if  (rec_key2_int_value >  other_index.rec_key2_int_value)   comparison = 1;
      else comparison = 0;
   }
   return comparison;
}
//_Index_item::compare_________________________________________________________/
Dynamic_string_int32_indexed_dBase_data_source::Dynamic_string_int32_indexed_dBase_data_source
(const char *filename_                                                           //000113
,const char *_primary_field_name
,const char *_secondary_field_name
,std::ios_base::openmode openmode_)                                              //111026
:Dynamic_indexed_dBase_data_source(filename_,_primary_field_name,openmode_)      //111026_970720
,secondary_field_name(_secondary_field_name)                                     //051101
,indicies()                                                                      //970720
{}
//_Dynamic_string_int32_indexed_dBase_data_source:constructor_______2001-05-09_/
void Dynamic_string_int32_indexed_dBase_data_source::build_index()     stream_IO_
{  for (bool at_end = !goto_BOF(); !at_end; at_end = !goto_next())
     indicies.append(new Index_item(*this,index_field_name.c_str(),secondary_field_name.c_str())); //970720
   index_built = true;
}
//_build_index______________________________________________________2001-05-09_/
class Index_string_int32_data_record : public Data_record
{public:
  std::string primary_key_field_name;
  std::string secondary_key_field_name;
  std::string primary_field_value;
  int32       secondary_field_value;
 public:
  inline Index_string_int32_data_record(const std::string &_primary_key_field_name,const std::string &_secondary_key_field_name)   // I think this should be const char
     : Data_record("-") // dBase does not use sections.
     , primary_key_field_name(_primary_key_field_name)
     , secondary_key_field_name(_secondary_key_field_name)
     {}
  inline virtual bool expect_structure(bool for_write)
  {  bool expected = Data_record::expect_structure(for_write);                   //161023_120314
     expect_string(primary_key_field_name.c_str(),primary_field_value,255); // Don't care about field length, it will be read from the file
     expect_int32(secondary_key_field_name.c_str(),secondary_field_value); // Don't care about field length, it will be read from the file
     structure_defined = false;                                                  //120314
     return expected;
  }
};
//_Index_string_int32_data_record___________________________________2005-11-01_/
Dynamic_string_int32_indexed_dBase_data_source::Index_item::Index_item
(modifiable_ Dynamic_indexed_dBase_data_source &file // file cannot be const because we perform IO //090813
,const char *primary_field_name,const char *secondary_field_name)
: Item()
, rec_num(file.get_record_number())
{   Index_string_int32_data_record rec_to_index(primary_field_name,secondary_field_name);
   rec_num = file.get_record_number();
   file.get(rec_to_index);
   rec_key1_string_value = rec_to_index.primary_field_value;                     //020125
   rec_key2_int_value = rec_to_index.secondary_field_value;                      //020125
}
//_Index_item:constructor______________________________________________________/
//______________________________________________________________________________
Dynamic_int32_int32_indexed_dBase_data_source::Index_item::Index_item
(int32 _rec_key1_int_value
,int32 _rec_key2_int_value)
: rec_key1_int_value(_rec_key1_int_value)
, rec_key2_int_value(_rec_key2_int_value)
{}
//_Index_item:constructor______________________________________________________/
bool Dynamic_int32_int32_indexed_dBase_data_source::find_record
(int32 search_value1
,int32 search_value2
,bool from_first_record)                                              stream_IO_
{
   if (!index_built)
   {  from_first_record = true;                                                  //021111
      // We have to goto to the first record if we build the index
      // otherwise we will be at the end if the index and would never
      // read the record.
      build_index();
   }
   bool found = false;                                                           //030709
   Dynamic_int32_int32_indexed_dBase_data_source::Index_item comparable_index(search_value1,search_value2);
   Index_item *item = (Index_item *)indicies.find_comparable(comparable_index,true); //051101
   if (item)                                                                     //051101
   {  goto_record(item->rec_num);
      found = true;
   }
   return found;
}
//_Dynamic_int32_int32_indexed_dBase_data_source::find_record_______2002-09-14_/
int  Dynamic_int32_int32_indexed_dBase_data_source::Index_item::compare(const Item &other) const
{
   const Dynamic_int32_int32_indexed_dBase_data_source::Index_item &other_index //141111
      = dynamic_cast<const Dynamic_int32_int32_indexed_dBase_data_source::Index_item &>
         (other);
   int comparison = 0;
      if       (rec_key1_int_value <  other_index.rec_key1_int_value)   comparison = -1;
      else if  (rec_key1_int_value >  other_index.rec_key1_int_value)   comparison = 1;
      else comparison = 0;
   if (comparison == 0)
   {  // The string matches, now compare int
      if       (rec_key2_int_value <  other_index.rec_key2_int_value)   comparison = -1;
      else if  (rec_key2_int_value >  other_index.rec_key2_int_value)   comparison = 1;
      else comparison = 0;
   }
   return comparison;
}
//_Index_item::compare_________________________________________________________/
Dynamic_int32_int32_indexed_dBase_data_source::Dynamic_int32_int32_indexed_dBase_data_source
(const char *filename_                                                           //000113
,const char *_primary_field_name
,const char *_secondary_field_name
,std::ios_base::openmode openmode_)                                              //111026
:Dynamic_int32_indexed_dBase_data_source(filename_,_primary_field_name,openmode_)//111026_970720
,secondary_field_name(_secondary_field_name)                                     //051101
,indicies()                                                                      //970720
{}
//_Dynamic_int32_int32_indexed_dBase_data_source:constructor________2001-05-09_/
void Dynamic_int32_int32_indexed_dBase_data_source::build_index()     stream_IO_
{  for (bool at_end = !goto_BOF(); !at_end; at_end = !goto_next())
     indicies.append(new Index_item(*this,index_field_name.c_str(),secondary_field_name.c_str())); //970720
   index_built = true;
}
//_Dynamic_int32_int32_indexed_dBase_data_source::build_index_______2001-05-09_/
class Index_int32_int32_data_record : public Data_record
{public:
  std::string primary_key_field_name;
  std::string secondary_key_field_name;
  int32 primary_field_value;
  int32 secondary_field_value;
 public:
  inline Index_int32_int32_data_record(const std::string &_primary_key_field_name,const std::string &_secondary_key_field_name)   // I think this should be const char
     : Data_record("-") // dBase does not use sections.
     , primary_key_field_name(_primary_key_field_name)
     , secondary_key_field_name(_secondary_key_field_name)
     , primary_field_value(0)
     , secondary_field_value(0)
     {}
  inline virtual bool expect_structure(bool for_write)
  {  bool expected = Data_record::expect_structure(for_write);                   //161023_120314
     expect_int32(primary_key_field_name  .c_str(),primary_field_value);
      // Don't care about field length, it will be read from the file
     expect_int32(secondary_key_field_name.c_str(),secondary_field_value);
      // Don't care about field length, it will be read from the file
     structure_defined = false;                                                  //120314
     return expected;                                                            //161023
  }
};
//_Index_int32_int32_data_record____________________________________2005-11-01_/
Dynamic_int32_int32_indexed_dBase_data_source::Index_item::Index_item
(modifiable_ Dynamic_indexed_dBase_data_source &file // file cannot be const because we perform IO //090813
,const char *primary_field_name,const char *secondary_field_name)
: Item()
, rec_num(file.get_record_number())
{  Index_int32_int32_data_record rec_to_index(primary_field_name,secondary_field_name);
   rec_num = file.get_record_number();
   file.get(rec_to_index);
   rec_key1_int_value = rec_to_index.primary_field_value;
   rec_key2_int_value = rec_to_index.secondary_field_value;
}
//_Dynamic_int32_int32_indexed_dBase_data_source::Index_item:constructor_______/
}//_namespace CORN_____________________________________________________________/

