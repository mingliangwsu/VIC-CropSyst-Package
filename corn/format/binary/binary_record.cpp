#include "corn/format/binary/binary_record.h"
#include "corn/format/binary/binary_record_file.h"
#include "corn/string/strconv.hpp"
#ifndef compareHPP
#  include "corn/math/compare.hpp"
#endif
namespace CORN {
//______________________________________________________________________________
void Binary_record::write(Binary_file_interface &out_file,bool big_body_sizes)
{  identify_IO();                                                                 //001126
   rec_length = compute_record_body_size();                                       //990111
   write_header(out_file,big_body_sizes);
   write_body(out_file);
   modified = false;
   // When writing, don't reposition pointer to beginning or record
}
//_Binary_record::write_____________________________________________2000-07-05_/
Record_code Binary_record::read(Binary_file_interface &in_file,bool big_body_sizes)
{  read_header(in_file,big_body_sizes);
   in_memory = read_body(in_file);
   modified = false;
   return get_code();
}
//_Binary_record::read______________________________________________2000-07-05_/
bool Binary_record_header::read_header
(Binary_file_interface &in_file                                                  //000129
,bool big_body_sizes)
{  rec_length = 0;
   bool successful = true;
   if (in_file.at_eof())
      successful = false;
   else
   {  rec_code = in_file.read_nat16();                                           //000129
      if (in_file.at_eof())                                                      //070622
         successful = false;
      else
      {  rec_length = (big_body_sizes)                                           //990107
         ? in_file.read_nat32()                                                  //000129
         : in_file.read_nat16();                                                 //000129
         if (in_file.at_eof())                                                   //070622
            successful = false;
      }
   }
   return successful;
}
//_Binary_record_header::read_header________________________________1999-01-07_/
bool Binary_record_header::write_header
(Binary_file_interface &out_file,bool big_body_sizes)
{  // file pointer should be positioned prior to calling this.
   out_file.write_nat16(get_code_for_write());
   if (big_body_sizes) out_file.write_nat32(rec_length);
   else                out_file.write_nat16(rec_length);
   return true; // WARNING probably should check for file still good
}
//_Binary_record_header::write_header_______________________________1997-11-03_/
void Binary_record_header::dumpX(std::ostream &txt_strm)
{  //             This is used to debug a database.
   char radix[10];
   int16_to_cstr (get_code(),radix,16);                                          //001126
   txt_strm << radix << 'h' << ' ';
   int16_to_cstr(get_code(),radix,2);                                            //001126
   txt_strm << radix << 'b' << " | code" << std::endl;
   txt_strm << rec_length << " | length" << std::endl;
}
//_Binary_record_header::dumpX_________________________________________________/
void Binary_record_header::write_YAML(std::ostream &YAML_strm,nat16 indent_level)
{
   std::string indent(indent_level,' ');
   char radix[10];
   int16_to_cstr (get_code(),radix,16);
   YAML_strm << indent<< "record: " << std::endl;
   YAML_strm << indent<< " code: " << "0x" << radix <<  std::endl;
   //int16_to_cstr(get_code(),radix,2);
   //txt_strm << radix << 'b' << " | code" << std::endl;
   YAML_strm << indent << " length: " <<  std::dec << rec_length << std::endl;
}
//_Binary_record_header::write_YAML_________________________________2018-01-08_/
int Binary_record_header::compare(const Binary_record_header &other)       const
{  int result = CORN::compare<Record_code>(rec_code, other.get_code());
   if (result == 0)
       result = CORN::compare<Record_code>(get_length(),other.get_length());
   return result;
}
//_Binary_record_header::compare____________________________________2009-10-21_/
////////////////////////////////////////////////////////////////////////////////
Binary_record::Binary_record
(Binary_file_interface &in_file
,bool big_body_sizes )
:Binary_record_header(in_file,big_body_sizes)
,in_memory(false)                                                               //100128
{}
//_Binary_record:constructor___________________________________________________/
bool Binary_record::set_modified(bool _modified)                   contribution_
{ modified = _modified; return modified; }
//_Binary_record::set_modified______________________________________1997-09-23_/
Binary_record_index_node_dynamic::Binary_record_index_node_dynamic
(Binary_file_interface &in_file
,bool big_body_sizes)
: Item()
, record(0)
, position(0)
, positioned(false)
{  record = new Binary_record_without_body(in_file ,big_body_sizes);             //040822
   // This constructor is used when creating an index from
   // an existing binary record stream.
   // Position will be set to the current position in the file.
   // The binary record (header) will be read from this position in the file.
   // (The body is not read)
   // The file position will be advanced past what would be the body of the record
}
//_Binary_record_index_node_dynamic:constructor________________________________/
bool Binary_record_index_node_dynamic::know_position(nat32 position_)
{  if (!positioned)
   {  position = position_;
      positioned = true;
   }
   return positioned; // actually may want return true if the position changes
}
//_Binary_record_index_node_dynamic::know_position_____________________________
nat32  Binary_record_index_node_dynamic::get_node_record_length
(bool big_body_sizes)                                                      const
{  nat32  node_record_length =
         + sizeof(Record_code)
         + (big_body_sizes ? sizeof(nat32 ) :sizeof(uint16))
         + record->get_length();
   return node_record_length;
}
//_Binary_record_index_node_dynamic::get_node_record_length_________2008-09-11_/
Binary_record_index_node_dynamic::Binary_record_index_node_dynamic
(Binary_file_interface &binary_record_file                                       //050522
,Binary_record *_record
,bool big_body_sizes
,bool write_it_now
,bool file_is_read_only)                                                         //050522
: Item()
, position(0)                                                                    //000711
, record(_record)                                                                //000707
, positioned(false)                                                              //000711
{  // This constructor is used when adding a record to the file.
   // The position will be set to the current file length.
   // The record will be immediately written.
   if (write_it_now && !file_is_read_only)
   {  position = binary_record_file.get_file_length();                           //000710
      binary_record_file.seek(position);
      record->write(binary_record_file,big_body_sizes);
      positioned = true;                                                         //000826
   }
}
//_Binary_record_index_node_dynamic:constructor________________________________/
Binary_record_index_node_dynamic::~Binary_record_index_node_dynamic()
{  if (record) delete record; record = 0;
   // It should not be necessary to finish, B.R.F.I will finish
}
//_Binary_record_index_node_dynamic:destructor_________________________________/
Binary_record * Binary_record_index_node_dynamic::finish
(Binary_record_file_dynamic &binary_record_file)
{  // The process used here of replacing the pointer to the in memory object to the file position is called 'unswizzling'
   Binary_record *record_doesnot_fit = 0;
   if (is_in_memory())
   {  Record_code rec_code_for_header  = record->get_code();
      nat32  record_length_in_file = record->get_length_in_file() ;  // We need to know the length before identifying IO and new length 091221_
      record->identify_IO(); // Makesure we account for optional record fields.  //030208
      nat32       length_for_header    = record->get_length();
      bool record_modified = record->is_modified();                              //100202
      bool record_valid    = record->is_valid();                                 //100202
      if (record_modified && record_valid)                                       //000706
      {  Binary_file_interface *binary_stream = binary_record_file.provide_binary_file();
         bool appending_record = !positioned;
         if (!positioned && binary_stream)                                       //000711
         {  position = binary_stream->get_file_length();                         //000711
            positioned = true;                                                   //000711
         }                                                                       //000711
         nat32 record_length_curr = record->compute_record_body_size();
         if (!appending_record && (record_length_in_file != record_length_curr))
         {  // Ack! the record has been resized, we can't save it where it used to be
            //                      Remember the record
            record_doesnot_fit = record;
            rec_code_for_header = binary_record_file.get_free_code(record->get_code());
            length_for_header = record_doesnot_fit->get_length();
            //                      Change the record at this position in the file to free space
            //                      Note that Binary_record will just write the header part this will
            //                      preserve the original body data in the file (only the record code will change to free code).
            if (!binary_record_file.is_read_only() && binary_stream)             //050522
            {  binary_stream->seek(position);                                    //001027
               Binary_record_without_body(rec_code_for_header,record_length_in_file,true)
                  .write(*binary_stream,binary_record_file.is_big_body_sizes());
            }
            record_doesnot_fit->set_length(length_for_header);                   //091221
            record = 0;
            positioned = false;  // This will force the record to be added as new to the end of the file  091221_
         } else
         {  // commit the modified record to the file (it will fit)
            if (!binary_record_file.is_read_only() && binary_stream)
            {  binary_stream->seek(position);
               record->write(*binary_stream,binary_record_file.is_big_body_sizes());
            }
            // delete the record with body and allow it to be replaced with the header only
         }
      }
      // Now replace the record (which was a with body) with just the header info
      if (record)
         delete record;
      record = new Binary_record_without_body
         (rec_code_for_header,length_for_header,false);                          //000706
   }
   // Otherwise the record points to a header only so we just leave it at that.
   // If the record did not fit where it used to be, return the misfit record caller should append it as a new record.
   return record_doesnot_fit;
}
//_Binary_record_index_node_dynamic::finish____________________________________/
Binary_record *Binary_record_index_node_dynamic::relinquish()
{  Binary_record *relinquished_record = record;
   record = new Binary_record_without_body
      (relinquished_record->get_code(),relinquished_record->get_length(),false);
   return relinquished_record;
}
//_Binary_record_index_node_dynamic::relinquish________________________________/
Binary_record *Binary_record_index_node_dynamic::get
(Binary_record_file_dynamic &binary_record_file)
{  // If record is in memory it is returned
   // Othersize record (which will be a header is deleted)
   // and a new full record will be read from the respective position;
   // The process here of replacing file position with pointer to the
   // loaded object into memory is called 'swizzling' http://en.wikipedia.org/wiki/Pointer_swizzling
   // When the record is not in memory this is called an object fault where we need to load it into memory 070806
   Binary_record *result = 0;
   bool in_mem = is_in_memory();
   if (!in_mem)
   {  Binary_file_interface *binary_stream = binary_record_file.provide_binary_file();
      if (binary_stream ->seek(position))                                        //020619
      {  Record_code rec_code = get_record_code();
      // Replace record(which is just a record header in this case)
      // with a complete record read from the file.
         result = binary_record_file.instanciate_record(rec_code);
         if (result)
         {   // If the record could be loaded OK
            result->read(*binary_stream,binary_record_file.is_big_body_sizes());
            delete record;
            record = result;
         }  // Otherwise we just leave the record header as is.
      } // else seek failed there must be a problem with the file.               //081121
   } else // The record is already in memory
      result = record;
   return result;
}
//_Binary_record_index_node_dynamic::get_______________________________________/
void Binary_record_index_node_dynamic::dump(std::ostream &txt_strm)
{  char radix[10];
   nat32_to_cstr(position,radix,10);                                             //001126
   txt_strm << radix << " | position" << std::endl;
   record->dumpX(txt_strm);
}
//_Binary_record_index_node_dynamic::dump______________________________________/
void Binary_record_index_node_dynamic::write_YAML
(std::ostream &YAML_strm,nat16 indent_level)
{  std::string indent(indent_level,' ');
   char radix[10];
   nat32_to_cstr(position,radix,10);                                             //001126
   // YAML_strm<<indent << "position: " << radix << std::endl;
   record->write_YAML(YAML_strm,indent_level);
}
//_Binary_record_index_node_dynamic::write_YAML_____________________2018-01-08_/
}//_namespace_CORN_____________________________________________________________/
