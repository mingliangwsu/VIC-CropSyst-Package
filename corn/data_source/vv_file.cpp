
/* ini_file.cpp
   by Roger Nelson   rnelson@mail.wsu.edu
   see the file ini_file.h for documentation
*/
#  pragma once

#include "corn/data_source/vv_file.h"

#if (defined(_WIN32)  || defined(_WIN64) || defined(__MSDOS__))
#     include <windows.h>
#  endif
#ifndef _INC_STDIO
#  include <stdio.h>
#endif
#ifndef _INC_STDLIB
#  include <stdlib.h>
#endif
#  ifndef strconvHPP
#     include "corn/string/strconv.hpp"
#  endif
#ifndef datarecH
#  include "corn/data_source/datarec.h"
#endif
#ifndef vv_entryH
#  include "corn/data_source/vv_entry.h"
#endif
#include "corn/OS/file_system_engine.h"
#include "corn/OS/directory_entry_name_concrete.h"

#if (defined(_WIN32)  || defined(_WIN64) || defined(__MSDOS__))
#     ifndef _INC_IO
#       include <io.h>
#     endif
#     ifdef _MSC_VER
#        define getcwd(a,b) _getcwd(a,b)
#        define chdir(x) _chdir(x)
#        define access(a,b) _access(a,b)
#     else
#        include <dir.h>
#     endif
#     include <direct.h>
#     include <dos.h>
#  endif

#ifdef __unix
#include <unistd.h>
#endif
namespace CORN {
//______________________________________________________________________________
bool VV_File::read(std::istream &strm,Data_record &data_rec, bool unexpected_only,bool single_section_mode)
{  nat32 data_read = false;                                                      //130401_050215
   bool started_single_section = false;                                          //050215
   int16 contiguous_blank_line_count = 0;  //    Bug fix for BC5                 //970614
   while (!strm.eof() && (contiguous_blank_line_count < 20))                     //050215
   {  nat32 this_line_pos = strm.tellg(); // remember current position in case this line is the start of a next section for single_section_mode mode
      std::string buffer;                                                        //131116
      getline(strm,buffer);                                                      //110725
      size_t comment_pos = buffer.find("#");
      if (comment_pos != std::string::npos)                                      //200213
      {  std::string buffer_comment_stripped(buffer,0,comment_pos);              //200213
         strip_string(buffer_comment_stripped,Trailing,' ');                     //200213
         buffer = buffer_comment_stripped;                                       //200213
      }
      strip_string(buffer,Trailing,'\r');                                        //131116
      if (buffer.length())
      {
         Submission_status status = submit(data_rec,buffer,unexpected_only);
         data_read += (status != unrecognized);
         bool is_section = (status == section_known) || (status == section_unknown);
         if (is_section)
            if (single_section_mode)                                             //050215
            {  // This is a special mode for reading a single section at a time  //050215
               if (started_single_section)                                       //050215
               {  // this line is the start of the next section so we can return now   //050215
                  strm.seekg(this_line_pos);                                     //050215
                  return true;                                                   //050215
               }                                                                 //050215
               started_single_section = true;                                    //050215
            }                                                                    //050215
          contiguous_blank_line_count = 0;
      } else contiguous_blank_line_count++;
   }
  return data_read > 0;                                                          //130401_050215
}
//_read________________________________________________________________________/
const char * VV_File::label_string(std::string &buffer)                    const
{  if (file_name) file_name->append_to_string(buffer);
   return buffer.c_str();
}
//_label_string_____________________________________________________2017-04-23_/
bool VV_File::exists() const
{  return file_name && OS::file_system_engine.exists((*file_name));              //160128
}
//_exists_______________________________________________________________________
bool VV_File::get(Data_record &data_rec)
{  bool result = false;
   if (exists())  //  Don't try to open empty filename (this shouldn't be needed, presumably f.good() should detect file the file doesn't exist
   {  std::ifstream f(file_name->c_str());                                       //160128
      if (f.good())
      { result = data_rec.get_start()                                            //010109
        && read(f,data_rec,false)                                                //010109
        && data_rec.get_end();                                                   //010109
        if (!remember_structure)                                                 //030516
           data_rec.delete_structure();                                          //010109
     }                                                                           //980405
   }                                                                             //980922
   return result;                                                                //020208
}
//_get______________________________________________________________2001-01-09_/
bool VV_File::set_data
(Data_record &data_rec
,bool         append)                                                            //150502
{  if (!file_name)                                                               //020125
      return false;  //  Don't try to open empty filename
   data_rec.set_current_section(data_rec.get_primary_section_name());
   bool result = false;                                                          //980405
    // Make sure we have structures for enuermated sections.
   FOR_EACH_IN(sect,VV_enumerated_section,data_rec.enumerated_sections_definitions,each_sect)//030709
     sect->setup_structure(data_rec,true);                                       //970929
   FOR_EACH_END(each_sect)                                                       //030709
   if (data_rec.get_preserve_unexpected_entries() && !append)                    //150502_080820
   {  std::ifstream strm(file_name->c_str());                                    //160218
      read(strm,data_rec,true);
   }
   std::ofstream strm(file_name->c_str()                                         //160128
      ,append?std::ios::app:std::ios::out);                                      //150502_980415
   {
      if (data_rec.inherits.count())                                             //170721
      {
         strm << "[inherit]" << std::endl;
         nat32 inherit_index = 0;
         FOR_EACH_IN(inherit,Item_string,data_rec.inherits,each_inherit)
         {
           strm << inherit_index++ << "=";
           inherit->write(strm);                                                 //180115
           //180115 << (*inherit)
           strm << std::endl;                                                    //180115
         } FOR_EACH_END(each_inherit)
      }
      data_rec.sections.write(strm);
      // Continue here I should now be able to use the new container Format
      result = true;                                                             //010109
   }
   data_rec.delete_structure();                                                  //010112
   //050705  probably want to move delete structure to Data_source::set()
   return result;                                                                //010109
}
//_set_data_________________________________________________________2005-07-05_/
VV_File::VV_File(const char *file_name_, bool _remember_structure)
: VV_Data_source()                                                               //130401_010108
,file_name(0)                                                                    //020125
,remember_structure(_remember_structure)                                         //030519
{  set_file_name_cstr(file_name_);                                               //160128_020125
}
//_constructor______________________________________________________1998-04-07_/
const OS::File_name &VV_File::set_file_name_cstr(const char *filename_)
{  int file_name_len = filename_ ? strlen(filename_)+1 : 0;
   if (file_name_len)
      file_name = new OS::File_name_concrete(filename_);                         //160128
   return *file_name;
}
//_set_file_name_cstr_______________________________________________2016-01-28_/
const OS::File_name &VV_File::set_file_name(const OS::File_name &file_name_)
{  file_name = new OS::File_name_concrete(file_name_);
   return (*file_name);
}
//_set_file_name____________________________________________________2016-01-28_/
VV_File::~VV_File()
{  delete file_name;
}
//_destructor__________________________________________________________________/
bool VV_File::inherit
(Data_record &data_rec
,const std::string &key, const std::string &inherit_URL)
{
   bool inherited = file_name != 0;
   if (!inherited) return 0;
   // Just a sanity check, because at this point we should actually always have
   // filename that is currently being loaded.

   // Currently assuming URL is a file on this machine,
   // either relative or fully qualified.
   // but it would be nice to be able to load files anywhere off the Internet.
   OS::File_name_concrete inherit_filename_unresolved(inherit_URL);
/*
std::wstring fnamew; file_name->get_Windows_representation(fnamew);
std::string fname;  CORN::wstring_to_string(fnamew,fname);
std::clog << "filename:" << fname.c_str() << std::endl;
*/

   OS::File_name *inherit_filename_resolved =
      OS::file_system_engine.render_resolved_absolute_DEN
      (inherit_filename_unresolved
      ,file_name->get_parent_directory_name_qualified());
   inherited &= (inherit_filename_resolved != 0);
 std::clog << "resolved:" << inherit_filename_resolved->c_str() << std::endl;
   inherited &= OS::file_system_engine.exists((*inherit_filename_resolved));
   if (inherited)
   {  // Now read at this point from the inherited file

   //std::clog << inherit_filename_resolved->c_str() << std::endl;
      std::ifstream VV_stream(inherit_filename_resolved->c_str());
      inherited = read(VV_stream,data_rec,false);
   } else
   {
      std::cerr << "error: " << "name=" << inherit_filename_unresolved.c_str() <<std::endl;
      if (inherit_filename_resolved)
         std::cerr << " resolved to=" << inherit_filename_resolved->c_str() << std::endl;
      else
         std::cerr << " unresolved" << std::endl;
      std::cerr << " not found." << std::endl;
   }
   delete inherit_filename_resolved;
   return inherited;
}
//_inherit__________________________________________________________2016-01-28_/
}//_namespace_CORN_____________________________________________________________/

