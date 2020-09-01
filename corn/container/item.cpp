#include "corn/container/item.h"
#include "corn/string/strconv.hpp"
#include <cstring>
#include <stdlib.h>
#include <string.h>
namespace CORN {
//______________________________________________________________________________
Item::Item()
: Item_interface()
{}
//______________________________________________________________________________
Item::~Item(){}
         // All derived classes must define compare and label for the class.
         // This destructor is required!
         // (if it is not included, items don't get deleted correctly.
//______________________________________________________________________________
bool Item::matches_filter(Filter filter)                           confirmation_
{
   if (filter == filter_all) return true;                                        //151204
   bool selected = is_selected();
   bool checked  = is_checked();
   bool not_selected = !selected;
   bool not_checked  = !checked;
   bool matches =
         ((selected && (filter == filter_selected)) || (filter == filter_select_or_checked))
      || ((checked  && (filter == filter_checked))  || (filter == filter_select_or_checked))
      || (not_selected && (filter == filter_not_selected))
      || (not_checked && (filter == filter_not_checked))
      || ((not_selected && not_checked) && (filter == filter_not_selected_and_not_checked))
      || ((not_selected || not_checked) && (filter == filter_not_checked_or_not_checked));
   return matches;
}
//_matches_filter___________________________________________________2004-03-18_/
bool  Item::write(std::ostream &strm)                               performs_IO_ //150728
{  std::string buffer;
   append_to_string(buffer);
   strm << buffer;
   return true;
}
//_write____________________________________________________________2013-01-03_/
bool Item::write_with_format                                                     //150728
(std::ostream &strm,const Format &format, nat16 indent_level)       performs_IO_
{  std::string buffer;
   append_to_string(buffer);
   if (format.indentor)
   {  for (nat16 i = 0; i < indent_level; i++)
      {  strm << format.indentor;
      }
   }
   if (format.initializer)  strm << format.initializer;
   if (format.key.distinquished)
   {
      if (format.key.quotation.initializer) strm << format.key.quotation.initializer;

      //180820 might be better to have method write key

      write_key(strm);                                                           //180820

/*abandoned now using write_key
      std::wstring key;                                                          //180820
      key_wstringX(key);                                                         //180820
      strm << key.c_str();                                                       //180820
*/
      /*180820  now using write_key
      const wchar_t *key_wstr = get_key_wide();
      if (key_wstr)     strm << key_wstr;
      else
      {  const char *key_cstr = get_key();
         if (key_cstr)  strm << key_cstr;
         else           strm << get_key_nat32();
      }
      */
      if (format.key.quotation.finalizer) strm << format.key.quotation.finalizer;
      if (format.key.separator)           strm << format.key.separator;
   }
   strm << buffer;
   if (format.indentor)
   {  for (nat16 i = 0; i < indent_level; i++)
      {  strm << format.indentor;
      }
   }
   if (format.finalizer) strm << format.finalizer;
   return true;
}
//_write_with_format________________________________________________2013-01-03_/
const wchar_t *Item::append_to(std::wstring &buffer)                       const
{
   std::string buffer_str;
   append_to(buffer_str);
   CORN::append_string_to_wstring(buffer_str,buffer);
   return buffer.c_str();
}
//_append_to(wstring)________________________________________________2020-06-21_/
const char *Item::append_to(std::string  &buffer)                          const
{ return buffer.c_str(); }
//_append_to(string)________________________________________________2020-06-21_/
const std::wstring &Item::append_to_wstring(std::wstring &buffer)          const
{  std::string buffer_str;
   append_to_string(buffer_str);
   return CORN::append_string_to_wstring(buffer_str,buffer);
}
//_append_to_wstring________________________________________________2013-01-03_/
int Item::compare(const Item &other)                                       const
{  int compare_result = 0;                                                       //190122
   const Item &other_item = (const Item &)other;                                 //150211

   //190122 NYI/Conceptual items probably should identify their key type

   std::wstring this_key;  key_wstring(this_key);                                //180220
   std::wstring other_key; other.key_wstring(other_key);                         //180220
   if (this_key.length() || other_key.length())                                  //190122
      return this_key.compare(other_key);                                        //180220

   // We didnt find key strings it may be numeric
   nat32 this_key_32 = get_key_nat32();                                          //190122
   nat32 other_key_32 = other.get_key_nat32();                                   //190122
   if (this_key_32 && other_key_32)                                              //190122
      return this_key_32 - other_key_32;                                         //190122

/*180220 Keep this for a while because it handles case sensitivity,
but I am not sure if still needed
   const char *key_buf = get_key();                                              //030117
   const char *other_key_buf = other_item.get_key();                             //150211_030117
   int comparison = 1;                                                           //150211
   bool either_is_case_sensitive = is_case_sensitive() ||other_item.is_case_sensitive();
   if (key_buf)                                                                  //151019_150211
   {  if (other_key_buf)
         comparison =                                                            //051101
         either_is_case_sensitive                                                //051101
         ? strcmp(key_buf,other_key_buf)
         : stricmp(key_buf,other_key_buf);                                       //051101
      else
      {  const wchar_t *other_key_wide_buf = other_item.get_key_wide();          //151019
         std::wstring key_wstring;                                               //151019
         CORN::ASCIIZ_to_wstring(key_buf,key_wstring);                           //151019
         comparison = key_wstring.compare(other_key_wide_buf);                   //151019
      }
   } else // try wide key                                                        //150211
   {
      const wchar_t *key_wide_buf = get_key_wide();
      const wchar_t *other_key_wide_buf = other_item.get_key_wide();
      if (key_wide_buf && other_key_wide_buf)
      {  comparison =
            either_is_case_sensitive
            ? wcscmp  (key_wide_buf,other_key_wide_buf)
            #ifdef _MSC_VER
            : _wcsicmp (key_wide_buf,other_key_wide_buf);
            #else
            :wcscmp (key_wide_buf,other_key_wide_buf);
            #endif
      } else // No keys to match to not comparable
        return -1;
   }
   return comparison;                                                            //020125
*/
   return compare_result;
}
//_compare__________________________________________________________2015-02-11_/
std::ostream &Item::write_key(std::ostream &to_stream)                stream_IO_
{ std::string buffer; key_string(buffer); to_stream << buffer.c_str();
   return to_stream;
}
//______________________________________________________________________________
const wchar_t *Item::key_wstring(std::wstring &buffer)                     const
{  // This is fall through if not reimplemented
   std::string buffer_as_string;
   key_string(buffer_as_string);
   return CORN::append_string_to_wstring(buffer_as_string,buffer).c_str();
}
//_key_wstring______________________________________________________2019-01-08_/
}//_namespace CORN_____________________________________________________________/
