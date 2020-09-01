#include "UED/library/unitsrec.h"
#include "UED/library/UED.h"
#include "corn/format/binary/binary_file_interface.h"
namespace UED {
//______________________________________________________________________________
Units_definition_format_0_record::Units_definition_format_0_record()
: Record_base                                    (UED_units_definition_format_0)
, Units_definition                                                            ()
{}
//_Units_definition_format_0_record:constructor_____________________1999-01-07_
Units_definition_format_0_record::Units_definition_format_0_record
( UED_units_code  code_
, const char *    description_
, const char *    abbreviation_)
: Record_base                                    (UED_units_definition_format_0)
, Units_definition                            (code_,description_,abbreviation_)
{}   // This constructor is for writing the variable definition to the database
//_Units_definition_format_0_record:constructor_________2004-09-08__1999-01-07_/
Units_definition_format_0_record::Units_definition_format_0_record
(const Units_definition &to_copy)
: Record_base                                    (UED_units_definition_format_0)
, Units_definition
   (to_copy.get_code(),to_copy.description.c_str(),to_copy.abbreviation.c_str())
{}   // This constructor is for writing the variable definition to the database
//_Units_definition_format_0_record:constructor_____________________1999-04-26_/
Units_definition_format_0_record* Units_definition_format_0_record::clone()const
{  return new Units_definition_format_0_record(*this);
}
//_clone____________________________________________________________2014-01-16_/
void Units_definition_format_0_record::write_body(Binary_file_interface &parent_database) // Binary_file_interface_replacing_Binary_fstream
{  parent_database.write_nat32(get());
   parent_database.write_string(description,ASCII_Z_string);
   parent_database.write_string(abbreviation,ASCII_Z_string);
}
//_write_body____________________________________________2010-01-14_2002-01-26_/
bool Units_definition_format_0_record::read_body(Binary_file_interface &parent_database) /// Binary_file_interface_replacing_Binary_fstream
{  nat32 units_code = parent_database.read_nat32();// I think this is the code?  //020126
   set(units_code);
   char read_buffer[255];                                                        //030125
   parent_database.read_c_str(read_buffer,ASCII_Z_string,255); description.assign(read_buffer);
   parent_database.read_c_str(read_buffer,ASCII_Z_string,255); abbreviation.assign(read_buffer);
   return true;                                                                  //100128
}
//_read_body________________________________________________________2002-01-26_/
}//_namespace UED______________________________________________________________/

