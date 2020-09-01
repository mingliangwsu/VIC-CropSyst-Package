#include "corn/format/structural.h"
#include "corn/string/strconv.hpp"
#include <string.h>
#include "corn/data_type_enum.h"
using namespace CORN;
namespace structural
{
nat16 write_indentation = 0;
//______________________________________________________________________________
Scalar_bool_cowl::Scalar_bool_cowl
(bool &value_ref_)
: Construct()
, value_ref(value_ref_)
{}
//Scalar_bool:constructor___________________________________________2015-08-30_/
bool Scalar_bool_cowl::set_text
(const wchar_t *value_as_wstring)                                  modification_
{
   bool recognized = true;
   // Note that the following boolean values are recognized YAML
   // but this need not necessarily be only YAML
   if      (wcscmp( value_as_wstring, L"true") ==0) value_ref = true;
   else if (wcscmp( value_as_wstring, L"True") ==0) value_ref = true;
   else if (wcscmp( value_as_wstring, L"TRUE") ==0) value_ref = true;
   else if (wcscmp( value_as_wstring, L"yes")  ==0) value_ref = true;
   else if (wcscmp( value_as_wstring, L"Yes")  ==0) value_ref = true;
   else if (wcscmp( value_as_wstring, L"YES")  ==0) value_ref = true;
   else if (wcscmp( value_as_wstring, L"false")==0) value_ref = false;
   else if (wcscmp( value_as_wstring, L"False")==0) value_ref = false;
   else if (wcscmp( value_as_wstring, L"FALSE")==0) value_ref = false;
   else if (wcscmp( value_as_wstring, L"no")   ==0) value_ref = false;
   else if (wcscmp( value_as_wstring, L"No")   ==0) value_ref = false;
   else if (wcscmp( value_as_wstring, L"NO")   ==0) value_ref = false;
   // Other recognizable boolean values could be added here
   else recognized = false;
   return recognized;
}
//_Scalar_bool::set_value_wchr_________________________________________________/
Pair_key_value *Map_clad::find_key_value_pair
(const structural::Construct &key_construct)                               const
{
   Pair_key_value *matching_KV_pair = dynamic_cast<Pair_key_value *>
      (get_key_value_pairs().find_matching(key_construct));
   return matching_KV_pair;
}
//_find_key_value_pair______________________________________________2015-10-18_/
Pair_key_value *Map_clad::provide_key_value_pair
( given_ Construct  *key_given
, const std::string &type_tag) // may be empty
//200623 , const std::string *type_tag)
{
   Pair_key_value *existing_KV_pair = find_key_value_pair(*key_given);
   if (!existing_KV_pair)
   {
      Construct *value = render_construct(type_tag); //200620(type_tag?type_tag->c_str():0);
      existing_KV_pair = new Pair_key_value(key_given,value);
   }
   return existing_KV_pair;
}
//_provide_key_value_pair______________________________________________________/
int Map_clad::compare(const CORN::Item &other)                             const
{  int comparison = -1;
   const Map *mapping_to_compare = dynamic_cast<const Map *>(&other);
   if (mapping_to_compare)
   {
      comparison = key_value_pairs.count() - mapping_to_compare->count();
      if (comparison == 0)
      {  // So far the mapping is superficially comparable because
         // they have the same number of items.
         // Now check each item
         // (we don't care about the order of the items).
         FOR_EACH_IN(KV_pair, Pair_key_value,key_value_pairs,each_KV_pair)
         {  const Pair_key_value *other_matching_KV_pair
               = find_key_value_pair(*(KV_pair->get_specifier()));
            if (!other_matching_KV_pair) return -1;
         } FOR_EACH_END(each_KV_pair)
      }
   }
   return comparison;
}
//_compare__________________________________________________________2015-10-18_/
bool Map_clad::write(std::ostream &strm)                              stream_IO_
{
   //strm << std::endl;0
   for (nat16 indent = 0; indent < write_indentation; indent++)
      strm << " ";
   write_indentation += 3;
   key_value_pairs.write(strm);
   write_indentation -= 3;
   return true;
}
//_Map_clad::write__________________________________________________2016-07-03_/
bool Pair_key_map::write(std::ostream &strm)                          stream_IO_
{
   for (nat16 indent = 0; indent < write_indentation; indent++)
      strm << " ";
   if (specifier)
   {  specifier->write(strm);
      strm << ": " << std::endl;
   }
   write_indentation += 3;
   map->write(strm);
   write_indentation -= 3;
   return true;
}
//_Pair_key_map::write______________________________________________2020-03-03_/
bool Scalar_number_clad::set_text
(const wchar_t *value_as_wstr)                                     modification_
{  bool recognized = true;
   if (value_as_wstr && mode != NT_unknown)   // Not sure if atof already checks for nul
   {  set_value_as_float64(CORN::wstr_to_float64(value_as_wstr));

      //should only return true if a recognized number.
      // could try converting back to a string and see if
      // if matches  value_as_wstr  but this is a little expensive

   } else recognized = false;
   return recognized;
}
//_set_value_wchr___________________________________________________2017-01-29_/
float64 Scalar_number_clad::set_value_as_float64(float64 new_value) modification_
{
   switch (mode)
   {
      case NT_int8:   *(value.as_int8)   = (int8)   new_value; break;
      case NT_int16:  *(value.as_int16)  = (int16)  new_value; break;
      case NT_int32:  *(value.as_int32)  = (int32)  new_value; break;
      // conceptual NT_int64
      case NT_nat8:   *(value.as_nat8)   = (nat8)   new_value; break;
      case NT_nat16:  *(value.as_nat16)  = (nat16)  new_value; break;
      case NT_nat32:  *(value.as_nat32)  = (nat32)  new_value; break;
      // conceptual NT_nat64
      // conceptual NT_float16 NT_float8
      case NT_float32:*(value.as_float32)= (float32)new_value; break;
      case NT_float64:*(value.as_float64)= (float64)new_value; break;
      /*200620
      case DT_int8:   *(value.as_int8)   = (int8)   new_value; break;
      case DT_int16:  *(value.as_int16)  = (int16)  new_value; break;
      case DT_int32:  *(value.as_int32)  = (int32)  new_value; break;
      case DT_nat8:   *(value.as_nat8)   = (nat8)   new_value; break;
      case DT_nat16:  *(value.as_nat16)  = (nat16)  new_value; break;
      case DT_nat32:  *(value.as_nat32)  = (nat32)  new_value; break;
      case DT_float32:*(value.as_float32)= (float32)new_value; break;
      case DT_float64:*(value.as_float64)= (float64)new_value; break;
      */
   }
   return new_value;
   // may want to echo back the converted value but should be the same get_value_as_float64();
}
//_set_value_as_float64_____________________________________________2017-01-29_/
bool Pair_key_value::is_key_string(const std::string &key)          affirmation_ //180820
{  return specifier && specifier->is_key_string(key); }
//_Pair_key_value::is_key_string_______________________________________________/
bool Pair_key_value::set_value_wstring
(const std::wstring &value_as_wstring)                             modification_
{
 return false;
}
//_Pair_key_value::set_value_wstring___________________________________________/
bool Pair_key_value::set_value_wchr
(const wchar_t *value_as_wstring)                                  modification_
{
 return false;
}
//_Pair_key_value::set_value_wchr______________________________________________/
int Pair_key_value::compare(const CORN::Item &key_to_compare)              const
{  // interface method uses interface methods.
   const Construct *specifier = get_specifier();
   int compare_result = specifier ? specifier->compare(key_to_compare) : -1;
   return compare_result;
}
//_Pair_key_value::compare_____________________________________________________/
//const structural::Construct *
bool
Pair_key_value::take_value
(structural::Construct *value_given)                              appropriation_
{
   if (value_given)
   {
      if (value != value_given)
      {  delete value; value = value_given; }
   }
   return value_given;
}
//_Pair_key_value::take_value__________________________________________________/
bool Scalar_number_clad::write(std::ostream &strm)                    stream_IO_
{  std::string value_as_string;
   strm << append_value_in_radix_to_string(value_as_string,3,10);
   return true;
}
//______________________________________________________________________________
const char *Scalar_number_clad::append_to(std::string &buffer)             const
{  append_value_in_radix_to_string(buffer,3,10);
   return buffer.c_str();
}
//_Scalar_number_clad::append_to____________________________________2020-06-21_/
const char *Scalar_number_clad::append_value_in_radix_to_string
(std::string &buffer,nat8 precision,nat8 radix_preferred)                  const
{  nat8 radix = (radix_preferred == 0) ? 10.0 : radix_preferred;
   switch (mode)
   {  case DT_int8:   append_int8_to_string  (*(value.as_int8) ,buffer,radix);break;
      case DT_int16:  append_int16_to_string (*(value.as_int16),buffer,radix);break;
      case DT_int32:  append_int32_to_string (*(value.as_int32),buffer,radix);break;
      case DT_nat8:   append_nat8_to_string  (*(value.as_nat8) ,buffer,radix);break;
      case DT_nat16:  append_nat16_to_string (*(value.as_nat16),buffer,radix);break;
      case DT_nat32:  append_nat32_to_string (*(value.as_nat32),buffer,radix);break;
      case DT_float32:append_float32_to_string(*(value.as_float32),precision,buffer);break;
      case DT_float64:append_float64_to_string(*(value.as_float64),precision,buffer);break;
   }
   return  buffer.c_str();
}
//_append_value_in_radix_to_string__________________________________2017-01-29_/
bool Scalar_bool_cowl::write(std::ostream &strm)                      stream_IO_
{  strm << value_ref ? "true" : "false";
   return true;
}
//_Scalar_bool::write__________________________________________________________/
const char *Scalar_bool_cowl::append_to(std::string &buffer)               const
{  buffer += value_ref ? "true" : "false";
   return buffer.c_str();
}
//_Scalar_bool_cowl::append_to______________________________________2020-06-21_/
bool Scalar_enum_cowl::set_text(const wchar_t *value_as_wstr)      modification_
{  std::string value_as_string;
   CORN::UnicodeZ_to_string(value_as_wstr,value_as_string);
   labeled_enum.set_label(value_as_string.c_str());
   return true;


   // should only return true if value_as_wstr is a recognized enum item

}
//_Scalar_enum::set_text_______________________________________________________/
bool Scalar_enum_cowl::write(std::ostream &strm)                      stream_IO_
{  labeled_enum.stream_label(strm);
   return true;
}
//_Pair_key_enum::write________________________________________________________/
Pair_key_value::Pair_key_value
(structural::Construct *specifier_given
,structural::Construct *value_given)
: Construct                                                                   ()
, specifier                                                    (specifier_given)
, value                                                            (value_given)
{}
//______________________________________________________________________________
bool  Pair_key_value::write(std::ostream &strm)                  stream_IO_
{  for (nat16 indent = 0; indent < write_indentation; indent++) strm << " ";
   if (specifier) // there should always be a specifier at this point
   {  specifier->write(strm);
      strm << ": " << std::endl;
   }
   write_indentation += 3;
   if (value) value->write(strm);                                                //200315
   write_indentation -= 3;
   return true;
}
//_Pair_key_value::write_______________________________________________________/
nat32 Sequence_clad::append_text_list(CORN::Text_list &values)    appropriation_
{  nat32 items_taken = 0;
   FOR_EACH_IN(value,CORN::Item,values,each_value)
   {  items_taken += 1;
      // Continue here

   } FOR_EACH_END(each_value)
   return items_taken;
}
//_Sequence_clad::append_text_list__________________________________2017-04-08_/
//200623 Construct *Sequence_clad::render_item(const char *type_tag)           rendition_
Construct *Sequence_clad::render_item(const std::string &type_tag)    rendition_
{
   return render_construct_raw(type_tag.c_str());
}
//_Sequence_clad::render_item_______________________________________2020-06-20_/
Construct *render_construct_raw(const char *type_tag)
{
   // While this function takes YAML type tags, it isn't necessarily YAML specific
   Construct *rendered = 0;
   if (type_tag)
   {
      if      (strstr(type_tag,"str"))      rendered = new Scalar_string_clad;
      else if (strstr(type_tag,"bool"))     rendered = new Scalar_bool_clad;
      else if (strstr(type_tag,"int"))      rendered = new Scalar_number_clad(CORN::NT_int32);
      else if (strstr(type_tag,"float"))    rendered = new Scalar_number_clad(CORN::NT_float32);
     //NYI else if (type_tag == "enum")     rendered = new Scalar_enum_clad;
      else if (strstr(type_tag,"seq"))      rendered = new Sequence_clad;
      else if (strstr(type_tag,"map"))      rendered = new Map_clad;
   }
   if (!rendered)
   {  // Not sure what to return in this case
      // May need to be an arbitrary construct
      // (to be promoted by parser as information from document is further encountered)
      rendered = new Construct_arbitrary;
   }
   return rendered;
}
}//_namespace_structural_______________________________________________________/

