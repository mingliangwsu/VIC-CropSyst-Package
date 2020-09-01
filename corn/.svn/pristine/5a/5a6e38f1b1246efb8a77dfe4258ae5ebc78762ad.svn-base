#ifndef data_type_enumH
#define data_type_enumH
//---------------------------------------------------------------------------
#include "corn/enum.h"
namespace CORN
{
//______________________________________________________________________________
// Now bit fields
// bits 0-1 (2bits)primitive data type
// 0 boolean (bitmask)
// 1 natural
// 2 integer
// 3 float
// bit 2-5 (6) number of bytes (0 -

// Abandoned encoding
// bit 2-4 (3bits) size indicator

// 0 unspecified
// 1 1 byte  (8bit)
// 2 2 byte  (16bit)
// 3 4 byte  (32bit)
// 4 8 byte  (64bit)
// 5 16 byte (128bit) reserved
// 6 32 byte (256bit) reserved
// 7 64 byte (512bit) reserved

enum Numeric_type
{NT_bool       //  0    00  // No size indicator  (0 is not a number)
,NT_nat        //  1    01
,NT_int        //  2    10
,NT_float      //  3    11

,NT_bool8      //  4   100  // or 1 byte bitmap
,NT_nat8       //  5   101
,NT_int8       //  6   110
,NT_float8     //  7   111  (not applicable)

,NT_bitmask16  //  8  1000  // 2 byte bitmask
,NT_nat16      //  9  1001
,NT_int16      // 10  1010
,NT_float16    // 11  1011

,NT_bitmask32  // 12  1100 // 4 byte bitmask
,NT_nat32      // 13  1101
,NT_int32      // 14  1110
,NT_float32    // 15  1111

,NT_bitmask64  // 16 10000 // 8 byte bitmask
,NT_nat64      // 17 10001
,NT_int64      // 18 10010
,NT_float64    // 19 10011

// conceptual:
// there could be large bit sizes but these are not specifically enumerated.
,NT_COUNT
};
#define NT_unknown 0

// Deprecated
enum Data_type
{DT_none                                                                         //180821
,DT_nat8       // primitive numeric types
,DT_nat16
,DT_nat32      // Do not reorder DT_nat with out correcting string format
,DT_int8       //  bitmasks in Binary_file_interface
,DT_int16      // and len_data_type_bitmask in binary_file_abstract
,DT_int32
,DT_float32
,DT_float64
,DT_date       // CORN date and time types
,DT_time
,DT_datetime
,DT_bool
,DT_char
,DT_string
,DT_wchar                                                                        //180821
,DT_wstring                                                                      //180821
,DT_COUNT   // count of the enumerations
};

#define DT_not_assigned DT_none
extern const char *data_type_label_table[];
DECLARE_ENUM_CLAD(Data_type_clad,Data_type,data_type_label_table);
//_Data_type___________________________________________________________________/
/*
7 > 0=noBOM,  1=BOM  (include Byte Order Mark)   When BOM is 1 Unicode bit must be set
6 > 0=little endian,  1=big endian  (applies Unicode) When BOM is 1 this endian bit is not applicable (0)
5 \ bits 1&2 Character encoding
4 / 00=7bit , 01=UTF8bit, 10=16bit, 11=32bit  (Note that the binary value of these two bits matches CORN::Data_type enumeration for natural numbers
3 > 0=ASCII,  1=Unicode
2 > 0=no zero/null terminator, 1=zero/null terminated
1 \ bit 4&5 count datatype when string is prepended with character count  (Note that the binary value of these two bits matches CORN::Data_type enumeration for natural numbers
0 / 00=no character count , 01=8bit, 10=16bit, 11=32bit integer (Note that the binary value of these two bits matches CORN::Data_type enumeration for natural numbers
*/

/*
enum Character_type
{CT_none
,CT_char    // single character
,CT_stringZ //zero terminated
,CT_stringC //count (leading)
};
*/


// Codes (for binary storage)
// Cx is character count
// Z is zero/0 terminated
// LE little endian
// BE big endian
enum String_format                              // 76543210
{ASCII_string                           = 0x00  // 00000000
,ASCII_C8_string                        = 0x01  // 00000001
,ASCII_C16_string                       = 0x02  // 00000010
,ASCII_C32_string                       = 0x03  // 00000011
,ASCII_Z_string                         = 0x04  // 00000100
// It is valid that there could be prepended character count
// with null termination, but this this is rare in practice
// LE = Little Endian
,UTF8LE_string                          = 0x18  // 00011000
,UTF8LE_C8_string                       = 0x19  // 00011001
,UTF8LE_C16_string                      = 0x1A  // 00011010
,UTF8LE_C32_string                      = 0x1B  // 00011011
,UTF8LE_Z_string                        = 0x1C  // 00011100
// It is valid that there could be prepended character count
// with null termination, but this this is rare in practice
,UTF16LE_string                         = 0x28  // 00101000
,UTF16LE_C8_string                      = 0x29  // 00101001
,UTF16LE_C16_string                     = 0x2A  // 00101010
,UTF16LE_C32_string                     = 0x2B  // 00101011
,UTF16LE_Z_string                       = 0x2C  // 00101100
// It is valid that there could be prepended character count
// with null termination, but this this is rare in practice
,UTF32LE_string                         = 0x38  // 00111000
,UTF32LE_C8_string                      = 0x39  // 00111001
,UTF32LE_C16_string                     = 0x3A  // 00111010
,UTF32LE_C32_string                     = 0x3B  // 00111011
,UTF32LE_Z_string                       = 0x3C  // 00111100
// It is valid that there could be prepended character count
// with null termination, but this this is rare in practice
// LE = Big Endian
,UTF8BE_string                          = 0x58  // 01011000
,UTF8BE_C8_string                       = 0x59  // 01011001
,UTF8BE_C16_string                      = 0x5A  // 01011010
,UTF8BE_C32_string                      = 0x5B  // 01011011
,UTF8BE_Z_string                        = 0x5C  // 01011100
// It is valid that there could be prepended character count
// with null termination, but this this is rare in practice
,UTF16BE_string                         = 0x68  // 01101000
,UTF16BE_C8_string                      = 0x69  // 01101001
,UTF16BE_C16_string                     = 0x6A  // 01101010
,UTF16BE_C32_string                     = 0x6B  // 01101011
,UTF16BE_Z_string                       = 0x6C  // 01101100
// It is valid that there could be prepended character count
// with null termination, but this this is rare in practice
,UTF32BE_string                         = 0x78  // 01111000
,UTF32BE_C8_string                      = 0x79  // 01111001
,UTF32BE_C16_string                     = 0x7A  // 01111010
,UTF32BE_C32_string                     = 0x7B  // 01111011
,UTF32BE_Z_string                       = 0x7C  // 01111100
// NYN here are UTF-32 encodings, but NYI (will be needed for Unix/Linux!)

// BOMs are usually only used when the entire file is text,
// not in binary record files which have string records.
};

#define  CORN_BOM_bitmask             0x80
#define  CORN_endian_bitmask          0x40
#define  CORN_char_size_bitmask       0x30
#define  CORN_char_code_bitmask       0x08
#define  CORN_null_term_bitmask       0x04
#define  CORN_len_data_type_bitmask   0x03
   // len_data_type field of String_format corresponds to the natural number types in  Data_type
//______________________________________________________________________________
}//_namespace_CORN_____________________________________________________________/
#endif

