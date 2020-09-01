#include "corn/format/binary/binary_file_fstream.h"
#include <fstream>
#if ((__BCPLUSPLUS__ >= 0x0582) || !defined (unix))
// for _access
#include <io.h>
#endif
#ifdef unix
#include <unistd.h>
// unix doesn't have, or is already, in binary mode?
#  define OR_IOS_BINARY  | std:: ios::binary
#else
// I think this may be DOS/Windows specific, so the ifdef would be if not __MSDOS__
#  define OR_IOS_BINARY   |std:: ios::binary
#endif
namespace CORN {
//______________________________________________________________________________
Binary_file_fstream::Binary_file_fstream
(const char *file_name_                                                          //001206
,bool        big_endian_
,std::ios_base::openmode openmode_)                                              //111026
: Binary_file_abstract(file_name_,big_endian_,openmode_)                         //111026
, strm(0)                                                                        //080912
{ }
//_Binary_file_fstream:constructor_____________________________________________/
Binary_file_fstream::~Binary_file_fstream()
{  if (strm)                                                                     //080912
   {
      std::fstream *as_fstream = dynamic_cast<std::fstream *>(strm);             //110827
      if (as_fstream) as_fstream->close();                                       //110820
      //110827, shouldn't need to close for delete so the previous two lines should not be needed
      //       but need to test this.
      delete strm;
      strm = 0;
   }
}  // This appears to be required in order for strm to close correctly and Binary_file_abstract destructor to be called.
//_Binary_file_fstream:destructor___________________________________2007-06-28_/
std::STREAM_TYPE &Binary_file_fstream::provide_stream()             stream_IO_
{  if (strm == NULL)
   {
      const char *strm_name = get_file_name();                                   //150129
      std::ios::openmode                                                         //160812_111114
          open_mode = openmode   OR_IOS_BINARY;                                  //111026
      if ((openmode & std::ios::in) && (openmode & std::ios::out))
      {  // If we are creating a file, we have to disable in mode
         bool include_IOS_in_mode = true;                                        //091222
             include_IOS_in_mode =
               #if (defined(__BCPLUSPLUS__) || defined(__unix))
                  access
               #else
                  _access // MicroSoft renamed these functions
               #endif
                  (get_file_name(), 0) == 0;
         open_mode = (std::ios::out)  OR_IOS_BINARY;
         if (include_IOS_in_mode)
            open_mode |= std::ios::in;
      }
      strm = new std::fstream(strm_name,open_mode);                              //170120
   }
   return *strm;
}
//_provide_stream___________________________________________________2008-09-12_/
bool Binary_file_fstream::goto_EOF()                                  stream_IO_
{ provide_stream().seekg(0, std::istream::end) /*== 0*/; return true; }
//_goto_EOF_________________________________________________________1999-01-07_/
int8  Binary_file_fstream::write_int8(int8 value)                     stream_IO_
{  provide_stream().put(value); return value; }
//_write_int8__________________________________________________________________/
int16 Binary_file_fstream::write_int16(int16 value)                   stream_IO_
{  CORN_endian_correction_16(value);
   provide_stream().write((char *)(&value),sizeof(int16));
   return value;
}
//_write_int16_________________________________________________________________/
int32 Binary_file_fstream::write_int32(int32 value)                   stream_IO_
{  CORN_endian_correction_32(value);                                             //000129
   provide_stream().write((char *)(&value),sizeof(int32));
   return value;
}
//_write_int32_________________________________________________________________/
nat8 Binary_file_fstream::write_nat8(nat8 value)                      stream_IO_
{  provide_stream().write((char *)(&value),sizeof(nat8));
   return value;
}
//_write_nat8_______________________________________________________1999-09-09_/
nat16 Binary_file_fstream::write_nat16(nat16 value)                   stream_IO_
{  CORN_endian_correction_16(value);
   provide_stream().write((char *)(&value),sizeof(nat16));
   return value;
}
//_write_nat16_________________________________________________________________/
nat32 Binary_file_fstream::write_nat32(nat32 value)                   stream_IO_
{  CORN_endian_correction_32(value);                                             //000129
   provide_stream().write((char *)(&value),sizeof(nat32));
   return value;
}
//_write_nat32_________________________________________________________________/
float64 Binary_file_fstream::write_float64(float64 value)             stream_IO_
{  CORN_endian_correction_float64(value);                                        //001214
   provide_stream().write((char *)(&value),sizeof(float64));
   return value;
}
//_write_float64_______________________________________________________________/
float32 Binary_file_fstream::write_float32(float32 value)             stream_IO_
{  CORN_endian_correction_float32(value);                                        //001214
   provide_stream().write((char *)(&value),sizeof(float32));
   return value;
}
//_write_float32____________________________________________________1999-01-07_/
const nat8 *Binary_file_fstream::write_nat8_array
(const nat8 *values,nat32 array_count)                                stream_IO_
{  provide_stream().write((char *)(values),sizeof(nat8)*array_count);
   return values;
}
//_write_nat8_array_________________________________________________2008-12-12_/
const int16   *Binary_file_fstream::write_int16_array
(const int16 *values,nat32 array_count)                               stream_IO_
{  provide_stream().write((char *)(values),sizeof(int16)*array_count);
   return values;
}
//_write_int16_array________________________________________________2014-01-29_/
const int32   *Binary_file_fstream::write_int32_array
(const int32 *values,nat32 array_count)                               stream_IO_
{  provide_stream().write((char *)(values),sizeof(int32)*array_count);
   return values;
}
//_write_int32_array________________________________________________2011-08-28_/
const float32 *Binary_file_fstream::write_float32_array
(const float32 *values,nat32 array_count)                             stream_IO_
{  for (nat32  i = 0; i < array_count; i++)
      CORN_endian_correction_float32(values[i]);
   provide_stream().write((char *)(values),sizeof(float32)*array_count);
   return values;
}
//_write_float32_array______________________________________________1999-01-07_/
void Binary_file_fstream::write
(const char *str, std::streamsize count)                              stream_IO_
{  provide_stream().write(str,count);
}
//_write____________________________________________________________2009-10-19_/
int8  Binary_file_fstream::read_int8()                                stream_IO_
{  int8 value;
   provide_stream().read((char *)(&value),sizeof(int8));
   return value;
}
//_read_int8________________________________________________________2008-11-21_/
int32 Binary_file_fstream::read_int32()                               stream_IO_
{  int32 value;
   provide_stream().read((char *)(&value),sizeof(int32));
   CORN_endian_correction_32(value);                                             //000129
   return value;
}
//_read_int32_______________________________________________________1999-01-03_/
int16  Binary_file_fstream::read_int16()                              stream_IO_
{  int16 value;
   provide_stream().read((char *)(&value),sizeof(int16));
   CORN_endian_correction_16(value);                                             //000129
   return value;
}
//_read_int16__________________________________________________________________/
nat8 Binary_file_fstream::read_nat8()                                 stream_IO_
{  nat8 value;                                                                   //081121
   provide_stream().read((char *)(&value),sizeof(nat8));                         //081121
   return value;                                                                 //081121
}
//_read_nat8________________________________________________________1999-09-09_/
nat16 Binary_file_fstream::read_nat16()                               stream_IO_
{  nat16 value;
   provide_stream().read((char *)(&value),sizeof(nat16));
   CORN_endian_correction_16(value);                                             //000129
   return value;
}
//_read_nat16__________________________________________________________________/
nat32 Binary_file_fstream::read_nat32()                               stream_IO_
{  nat32 value;
   provide_stream().read((char *)(&value),sizeof(nat32));
   CORN_endian_correction_32(value);                                             //000129
   return value;
}
//_read_nat32_______________________________________________________1999-01-03_/
float64 Binary_file_fstream::read_float64()                           stream_IO_
{  float64 value;
   provide_stream().read((char *)(&value),sizeof(float64));
   CORN_endian_correction_float64(value);                                        //001214
   return value;
}
//_read_float64________________________________________________________________/
float32 Binary_file_fstream::read_float32()                           stream_IO_
{  float32 value;
   provide_stream().read((char *)(&value),sizeof(float32));
   CORN_endian_correction_float32(value);                                        //001214
   return value;
}
//_read_float32_____________________________________________________1999-01-07_/
nat8 *Binary_file_fstream::read_nat8_array
(nat8 *values,nat32  array_count)                                     stream_IO_
{  provide_stream().read((char *)(values),sizeof(nat8)*array_count);             //081121
   return values;                                                                //081121
}
//_read_nat8_array__________________________________________________2010-01-14_/
nat16 *Binary_file_fstream::read_nat16_array
(nat16 *values,nat32 array_count)                                     stream_IO_
{  provide_stream().read((char *)(values),sizeof(nat16)*array_count);            //081121
   return values;                                                                //081121
}
//_read_nat16_array_________________________________________________2010-01-14_/
nat32 *Binary_file_fstream::read_nat32_array
(nat32 *values,nat32 array_count)                                     stream_IO_
{  provide_stream().read((char *)(values),sizeof(nat32)*array_count);
   return values;
}
//_read_nat32_array_________________________________________________2011-08-27_/
int32 *Binary_file_fstream::read_int32_array
(int32 *values,nat32 array_count)                                     stream_IO_
{  provide_stream().read((char *)(values),sizeof(int32)*array_count);
   return values;
}
//_read_int32_array_________________________________________________2011-08-27_/
int16 *Binary_file_fstream::read_int16_array
(int16 *values,nat32 array_count)                                     stream_IO_
{  provide_stream().read((char *)(values),sizeof(int16)*array_count);
   return values;
}
//_read_int16_array_________________________________________________2014-01-29_/
float32 *Binary_file_fstream::read_float32_array
(float32 *values,nat32  count)                                        stream_IO_
{  provide_stream().read((char *)(values),sizeof(float32)*count);
   for (nat32  i = 0; i < count; i++)
      CORN_endian_correction_float32(values[i]);
   return values;
}
//_read_float32_array_______________________________________________2008-12-12_/
void Binary_file_fstream::read(char *str, std::streamsize count)      stream_IO_
{  provide_stream().read(str,count);
}
//_read_____________________________________________________________2009-10-19_/
bool Binary_file_fstream::seek
(nat32 pos
#ifdef __GNUC__
, std::_Ios_Seekdir dir                                                          //090305
#else
, std::ios::seekdir dir
#endif
) stream_IO_
{
   bool success = true;                                                          //080911
   #ifdef __unix
   // UNIX usually needs long instead of streamoff
      long seek_pos = pos;                                                       //021212
   #else
      std::streamoff seek_pos = pos;                                             //021212
   #endif
   provide_stream();
   strm->clear(); // For some unknown reason seekg does not appear to be going to the right position hopeing clear will fix this   //100105

   if (pos > 0x7FFFFFFF)                                                         //120110
   {  // seekg only takes an unsigned long 32bit!!!
      // so seekg to the highest addressable position
      // then seek to the target position as an offset of that position.
      // Note: it is unknown if this will work for all implementations of stdlib streams.
      strm->seekg(0x7FFFFFFF);                                                   //120110
      int32 offset = pos - 0x7FFFFFFF;                                           //120110
      strm->seekg(offset,std::ios_base::cur);                                    //120110
   } else                                                                        //120110
      strm->seekg(seek_pos,dir);                                                 //050522

#if (__BCPLUSPLUS__ >=	0x0570)
   std::ios_base::iostate state = strm->rdstate();                               //080911
   if ((state & std::ios_base::failbit) /*|| (state & ios_base::bad)*/)          //080911
      success = false;                                                           //080911
#else
   if (!strm->good())                                                            //081121
      success = false;                                                           //081129
   strm->clear();                                                                //090514
#endif
   return success;
}
//_seek_____________________________________________________________1999-01-07_/
nat32  Binary_file_fstream::get_file_length()                         stream_IO_
{
// Note, that I think I need to use both xxxp() and xxxg()
// depending on the I/O mode, but I haven't figured out how to select this.
//021212 update: tellg seems to be working irregardless of the open mode
// Note: in Borland compilers, a newly open file return 0
// in unix it returns -1 (an error condition?)
// therefore curr_pos and file_length must be long and not unsigned!
   long file_length = 0;
   provide_stream();                                                             //080912
   if (strm->good())                                                             //070628
   {  long curr_pos =strm->tellg();                                              //070628
      strm->seekg(0,std::istream::end);
      file_length = strm->tellg();
      strm->seekg(curr_pos);
   }                                                                            //070628
   return file_length;
}
//_get_file_length__________________________________________________1999-01-15_/
nat32  Binary_file_fstream::tell()                                    stream_IO_
{  if (!strm) return 0xFFFFFFFF;
   long tell_pos_long = provide_stream().tellg();
   nat32  tell_pos =  tell_pos_long < 0 ? 0 : (nat32 )tell_pos_long;             //081018
   return tell_pos;                                                              //021212
}
//_tell_____________________________________________________________1999-01-03_/
bool Binary_file_fstream::at_eof()                                    stream_IO_
{  return provide_stream().eof();
}
//_at_eof___________________________________________________________2005-05-22_/
bool Binary_file_fstream::close()                                    stream_IO_
{  std::fstream *as_fstream = dynamic_cast<std::fstream *>(strm);
      if (as_fstream) as_fstream->close();
   return true;
}
//_close____________________________________________________________2007-06-22_/
bool Binary_file_fstream::reset()                                    stream_IO_
{  std::fstream *as_fstream = dynamic_cast<std::fstream *>(strm);
      if (as_fstream)
      {  as_fstream->clear();
         as_fstream->seekg(0);
      }
   return as_fstream != NULL;
}
//_reset____________________________________________________________2011-11-11_/
}//_namespace CORN_____________________________________________________________/

