#include "corn/data_source/datarec.h"
#include "corn/string/strconv.hpp"
#ifndef compareHPP
#  include "corn/math/compare.hpp"
#endif
#ifndef indexerH
#  include "corn/container/indexer.h"
#endif
#include "corn/OS/file_system_engine.h"
#include "corn/OS/directory_entry_name_concrete.h"
namespace CORN {
typedef Bidirectional_list Association_list;                                     //180101
//______________________________________________________________________________
Data_record::Data_record
(const char *_primary_section                                                    //030109
,bool _one_based_enumerated_sections_obsolete)
: primary_section_name(_primary_section)
, sections()
, structure_defined(false)                                                       //010516
, enumerated_sections_definitions()
, current_section(0)
, position(0)
, modified(false)
, preserve_unexpected_entries(false)                                             //080820
{  set_current_section(_primary_section);  // must come before and after initialize!
}
//_Data_record:constructor______________________________________________________
VV_string_entry *Data_record::expect_string
(const char *key_ ,modifiable_ std::string &value_,uint16 max_strlen_
,Phrase_label annotation1_,Phrase_label annotation2_)
{
   Association_list &section_entries = get_current_section()->entries;
   VV_string_entry *entry = (VV_string_entry *)section_entries.find_cstr(key_);  //120921_050518
   if (!entry) // avoid duplicate entries                                        //010404
   {  entry = new VV_string_entry(key_,value_,max_strlen_,annotation1_,annotation2_);//050518
      section_entries.append((CORN::Item *)entry) ;                              //010404
   }                                                                             //050518
   return entry;                                                                 //050518
}
//_expect_string_______________________________________________________________/
VV_directory_entry_entry * Data_record::expect_directory_entry_name
(const char *key_
,modifiable_ CORN::OS::Directory_entry_name & value_
,nat16 max_strlen_,Phrase_label annotation1_,Phrase_label annotation2_)
{   Association_list &section_entries = get_current_section()->entries;
   VV_directory_entry_entry *entry=(VV_directory_entry_entry *)section_entries.find_cstr(key_);//120921_050518
   if (!entry) // avoid duplicate entries                                        //010404
   {  entry = new VV_directory_entry_entry(key_,value_,max_strlen_,annotation1_,annotation2_); //050518
      section_entries.append((CORN::Item *)entry) ;                              //010404
   }                                                                             //050518
   return entry;                                                                 //050518
}
//_expect_directory_entry_name______________________________________2012-02-07_/
#ifndef _MSC_VER
//100929 This is sort of deprecated because it is unsafe, it is currently only used in ClimGen.
VV_char_array_entry *Data_record::expect_char_array
(const char *key_ ,modifiable_  char *value_
,nat16 max_strlen_,Phrase_label annotation1_,Phrase_label annotation2_)
{  Association_list &section_entries = get_current_section()->entries;
   VV_char_array_entry *entry = (VV_char_array_entry *)section_entries.find_cstr(key_); //120921_050518
   if (!entry) // avoid duplicate entries                                        //010404
   {  entry = new VV_char_array_entry(key_,value_,max_strlen_,annotation1_,annotation2_);  //050518
      section_entries.append((CORN::Item *)entry) ;                              //010404
   }
   return entry;                                                                 //050518
}
//_expect_char_array___________________________________________________________/
#endif
VV_int8_entry *Data_record::expect_int8
(const char *key_ ,modifiable_ int8 & value_, Phrase_label annotation1_,Phrase_label annotation2_)
{  Association_list &section_entries = get_current_section()->entries;
   VV_int8_entry *entry = (VV_int8_entry *)section_entries.find_cstr(key_);      //120921_050518
   if (!entry) // avoid duplicate entries                                        //010404
   {  entry = new VV_int8_entry(key_,value_,annotation1_,annotation2_);          //050518
      section_entries.append((CORN::Item *)entry) ;                              //010404
   }                                                                             //050518
   return entry;                                                                 //050518
}
//_expect_int8______________________________________________________1998-04-05_/
VV_int16_entry *Data_record::expect_int16
(const char *key_ ,modifiable_ int16 & value_, Phrase_label annotation1_,Phrase_label annotation2_)
{  Association_list &section_entries = get_current_section()->entries;
   VV_int16_entry *entry = (VV_int16_entry *)section_entries.find_cstr(key_);    //120921
   if (!entry) // avoid duplicate entries
   {  entry =  new VV_int16_entry(key_,value_,annotation1_,annotation2_);
      section_entries.append((CORN::Item *)entry) ;
   }
   return entry;
}
//_expect_int16_________________________________________2005-05-18__2001-04-04_/
VV_int32_entry * Data_record::expect_int32
(const char *key_ ,modifiable_ int32 & value_, Phrase_label annotation1_,Phrase_label annotation2_)
{  Association_list &section_entries = get_current_section()->entries;
   VV_int32_entry *entry = (VV_int32_entry *)section_entries.find_string(key_);  //120921
   if (!entry) // avoid duplicate entries
   {  entry =  new VV_int32_entry(key_,value_,annotation1_,annotation2_);
      section_entries.append((CORN::Item *)entry) ;
   }
   return entry;
}
//_expect_int32_________________________________________2005-05-18__2001-04-04_/
VV_nat16_entry *Data_record::expect_nat16
(const char *key_ ,modifiable_ nat16 & value_, nat8 radix_, Phrase_label annotation1_,Phrase_label annotation2_)
{  Association_list &section_entries = get_current_section()->entries;
   VV_nat16_entry *entry = (VV_nat16_entry *)section_entries.find_cstr(key_);    //120921
   if (!entry) // avoid duplicate entries
   {  entry =  new VV_nat16_entry(key_,value_,radix_,annotation1_,annotation2_);
      section_entries.append((CORN::Item *)entry) ;
   }
   return entry;
}
//_expect_nat16_____________________________2005-05-18__2001-04-04__2000-09-26_/
VV_nat8_entry * Data_record::expect_nat8
(const char *key_ ,modifiable_ nat8 & value_, nat8 radix_,Phrase_label annotation1_,Phrase_label annotation2_)
{  Association_list &section_entries = get_current_section()->entries ;
   VV_nat8_entry *entry = (VV_nat8_entry *)section_entries.find_cstr(key_);      //120921
   if (!entry) // avoid duplicate entries
   {  entry =  new VV_nat8_entry(key_,value_,radix_,annotation1_,annotation2_);
      section_entries.append((CORN::Item *)entry) ;
   }
   return entry;
}
//_expect_nat8______________________________2005-05-18__2001-04-04__1998-04-05_/
VV_nat32_entry *Data_record::expect_nat32
(const char *key_ ,modifiable_ nat32 & value_, nat8 radix_, Phrase_label annotation1_,Phrase_label annotation2_)
{  Association_list &section_entries =  get_current_section()->entries ;
   VV_nat32_entry *entry = (VV_nat32_entry *)section_entries.find_cstr(key_);    //120921
   if (!entry) // avoid duplicate entries
   {  entry =  new VV_nat32_entry(key_,value_,radix_, annotation1_,annotation2_);
      section_entries.append((CORN::Item *)entry) ;
   }
   return entry;
}
//_expect_nat32_________________________________________2005-05-18__2000-09-26_/
VV_float32_entry * Data_record::expect_float32
(const char *key_ ,modifiable_  float32   & value_
, Phrase_label annotation1_,Phrase_label annotation2_)
{  Association_list &section_entries = get_current_section()->entries;
   VV_float32_entry *entry=(VV_float32_entry *)section_entries.find_cstr(key_);  //120921
   if (entry)
   {  // relink the entry value
      //      entry->
   }
   else // avoid duplicate entries
   {  entry =  new VV_float32_entry(key_,value_,annotation1_,annotation2_);
      section_entries.append((CORN::Item *)entry) ;
   }
   return entry;
}
//_expect_float32____________________________2005-05-18__2001-04-04_2009-09-26_/
VV_float64_entry *Data_record::expect_float64
(const char *key_ ,modifiable_  float64 & value_, Phrase_label annotation1_,Phrase_label annotation2_)
{  Association_list &section_entries = get_current_section()->entries;
   VV_float64_entry *entry = (VV_float64_entry *)section_entries.find_cstr(key_);
   if (!entry) // avoid duplicate entries
   {  entry =  new VV_float64_entry(key_,value_,annotation1_,annotation2_);
      section_entries.append((CORN::Item *)entry) ;
   }
   return entry;
}
//_expect_float64____________________________2005-05-18__2001-04-04_2009-09-26_/
VV_bool_entry * Data_record::expect_bool
(const char *key_ ,modifiable_  bool    & value_,VV_bool_entry::Form    _text_form, Phrase_label annotation1_,Phrase_label annotation2_)
{  Association_list &section_entries = get_current_section()->entries;
   VV_bool_entry *entry = (VV_bool_entry *)section_entries.find_cstr(key_);
   if (!entry) // avoid duplicate entries
   {  entry =  new VV_bool_entry(key_,value_,_text_form,annotation1_,annotation2_);
      section_entries.append((CORN::Item *)entry) ;
   }
   return entry;
}
//_expect_bool_______________________________2005-05-18__2001-04-04_2009-09-26_/
bool Data_record::expect_switch
(bool &flag_value
,const char *long_form
,const char *short_form
,const char *caption1
,const char *caption2)
{  bool expected
   = expect_bool(long_form   ,flag_value,VV_bool_entry::form_unary,"Verbose log",0);
   if (short_form && short_form[0])
      expected &= (expect_bool(short_form,flag_value,VV_bool_entry::form_unary,caption1,caption2)
               != 0);
   return expected;
}
//_expect_switch____________________________________________________2019-11-01_/


// These following dates can probably take interfaces
VV_date_entry *Data_record::expect_date
(const char *key_ ,modifiable_  CORN::Date_clad_32 &value_
, bool formatted, Phrase_label annotation1_,Phrase_label annotation2_)
{  Association_list &section_entries = get_current_section()->entries;
   VV_date_entry *entry = (VV_date_entry *)section_entries.find_cstr(key_);
   if (!entry) // avoid duplicate entries
   {  entry =  new VV_date_entry(key_,value_,formatted
         ,annotation1_,annotation2_);
      section_entries.append((CORN::Item *)entry) ;
   }
   return entry;
}
//_expect_date__________________________________________2005-05-18__2005-04-12_/
VV_datetime64_entry * Data_record::expect_datetime64
(const char *key_ ,modifiable_ CORN::Date_time_clad_64 & value_, bool formatted
, Phrase_label annotation1_,Phrase_label annotation2_)
{  Association_list &section_entries = get_current_section()->entries;
   VV_datetime64_entry *entry = (VV_datetime64_entry *)section_entries.find_cstr(key_);
   if (!entry) // avoid duplicate entries
   {  entry =  new VV_datetime64_entry(key_,value_,formatted
         ,annotation1_,annotation2_);
       section_entries.append((CORN::Item *)entry) ;
   }
   return entry;
}
//_expect_datetime64____________________________________2005-05-18__2005-04-12_/
VV_datetime64_entry *Data_record::expect_datetime64_formatted
(const char *key_, modifiable_ Date_time_clad_64 &value_
,const char *format_,Phrase_label annotation1_,Phrase_label annotation2_)
{  Association_list &section_entries = get_current_section()->entries;
   VV_datetime64_entry *entry = (VV_datetime64_entry *)section_entries.find_cstr(key_);
   if (!entry) // avoid duplicate entries
   {  entry =  new VV_datetime64_entry(key_,value_,format_
         ,annotation1_,annotation2_);
       section_entries.append((CORN::Item *)entry) ;
   }
   return entry;
}
//_expect_datetime64_formatted______________________________________2019-05-14_/
VV_byte_entry * Data_record::expect_byte
(const char *key_ ,modifiable_  unsigned char    & value_
, Phrase_label annotation1_,Phrase_label annotation2_)
{  Association_list &section_entries = get_current_section()->entries;
   VV_byte_entry *entry = (VV_byte_entry *)section_entries.find_cstr(key_);
   if (!entry) // avoid duplicate entries
   {  entry =  new VV_byte_entry(key_,value_,annotation1_,annotation2_);
      section_entries.append((CORN::Item *)entry) ;
   }
   return entry;
}
//_expect_byte______________________________________________________2005-05-18_/
VV_enum_entry *  Data_record::expect_enum
(const char *key_
,modifiable_  Enum_abstract                & value_
, Phrase_label annotation1_,Phrase_label annotation2_)
{  Association_list &section_entries = get_current_section()->entries;
   VV_enum_entry *entry = (VV_enum_entry *)section_entries.find_cstr(key_);
   if (!entry) // avoid duplicate entries
   {  entry =  new VV_enum_entry(key_,value_, annotation1_,annotation2_);
      section_entries.append((CORN::Item *)entry) ;
   }
   return entry;
}
//_expect_enum_______________________________2005-05-18__2001-04-04_2002-03-22_/
VV_units_entry * Data_record::expect_units(const char *key_, modifiable_ Units_clad &units)
{  Association_list &section_entries = get_current_section()->entries;
   VV_units_entry *entry = (VV_units_entry *)section_entries.find_cstr(key_);
    if (!entry) // avoid duplicate entries
   {  entry =  new VV_units_entry(key_,units);
      section_entries.append((CORN::Item *)entry) ;
   }
   return entry;
}
//_expect_units_____________________________________________________2005-04-13_/
VV_int16_entry *Data_record::expect_valid_int16 (const Valid_int16  &v_integer)
{  Association_list &section_entries = get_current_section()->entries;
  VV_int16_entry *entry = (VV_int16_entry *)section_entries.find_cstr(v_integer.key);
   if (!entry) // avoid duplicate entries
   {  entry = new VV_int16_entry(v_integer.key,v_integer.value,v_integer.description1,v_integer.description2);
       section_entries.append((CORN::Item *)entry) ;
      expect_valid_number(v_integer,section_entries);
   }
   return entry;
}
//_expect_valid_int16__________________________________________________________/
VV_int32_entry * Data_record::expect_valid_int32 (const Valid_int32 &v_long)
{  Association_list &section_entries = get_current_section()->entries;
   VV_int32_entry *entry = (VV_int32_entry *) section_entries.find_cstr(v_long.key);
   if (!entry) // avoid duplicate entries
   {  entry = new VV_int32_entry(v_long.key,v_long.value,v_long.description1,v_long.description2);
      section_entries.append((CORN::Item *)entry) ;
      expect_valid_number(v_long,section_entries);
   }
   return entry;
}
//_expect_valid_int32________________________2005-05-18__2001-04-04_2002-03-22_/
VV_float32_entry * Data_record::expect_valid_float32(const Valid_float32 &v_float)
{  Association_list &section_entries = get_current_section()->entries;
   VV_float32_entry *entry  = (VV_float32_entry *)section_entries.find_cstr(v_float.key);
   if (!entry) // avoid duplicate entries
   {  entry = new VV_float32_entry(v_float.key,v_float.value,v_float.description1,v_float.description2);
      section_entries.append((CORN::Item *)entry) ;
      expect_valid_number(v_float,section_entries);
   }
   return entry;
}
//_expect_valid_float32______________________2005-05-18__2001-04-04_2002-03-22_/
VV_float64_entry * Data_record::expect_valid_float64(const Valid_float64 &v_double)
{  Association_list &section_entries = get_current_section()->entries;
   VV_float64_entry *entry = (VV_float64_entry *)section_entries.find_cstr(v_double.key);
   if (!entry) // avoid duplicate entries
   {  entry = new VV_float64_entry(v_double.key,v_double.value,v_double.description1,v_double.description2);
       section_entries.append((CORN::Item *)entry) ;
      expect_valid_number(v_double,section_entries);
   }
   return entry;
}
//_expect_valid_float64______________________2005-05-18__2001-04-04_2002-03-22_/
VV_parameter_entry *Data_record::expect_parameter(Parameter_abstract &_parameter)
{  Association_list &section_entries = get_current_section()->entries;
   VV_parameter_entry *entry = (VV_parameter_entry *)section_entries.find_cstr(_parameter.properties.name_long);
   if (!entry) // avoid duplicate entries
   {  entry = new VV_parameter_entry(_parameter);
      section_entries.append((CORN::Item *)entry) ;
   }
   return entry;
}
//_expect_parameter_________________________________________________2009-04-14_/
void Data_record::expect_valid_number(const Valid_number &v_number,Association_list &section_entries)
{  if (v_number.alt_units_codes)
   {  std::string key_units(v_number.key);                                       //131008_040909
                  key_units.append("_units");
      std::string key_units_shown(key_units);                                    //131008_040909
                  key_units_shown.append("_shown");
      // I think now I can use expect units
      section_entries.append((CORN::Item *)(new VV_units_entry(key_units.c_str(),(Units_clad &)v_number.units_code)));
      section_entries.append((CORN::Item *)(new VV_units_entry(key_units_shown.c_str(),(Units_clad &)v_number.shown_units_code)));
   }
}
//_expect_valid_number______________________________________________2004-04-14_/
char *Data_record::enum_variable(nat16 index,const char *variable,bool enumerate_variable) const
{  static char buffer[255];
   buffer[0] = 0;
   if (enumerate_variable)
   {  strcat(buffer,variable);
      strcat(buffer,"_");
   }
   char _str[10];
   CORN::int16_to_cstr((int16)index,_str,10);
   strcat(buffer,_str);
   return buffer;
}
//_enum_variable____________________________________________________2003-08-01_/
void Data_record::expect_float32_array
(const char *variable_or_section_label_, float32 *array, bool one_based ,nat16 max_index,bool enumerate_variable)
{  if (!enumerate_variable)                                                      //030801
       set_current_section(variable_or_section_label_);
    for (nat16 i = one_based; i <= max_index; i++)
      expect_float32(enum_variable(i,variable_or_section_label_,enumerate_variable),array[i]);//030801
}
//_expect_float32_array_____________________________________________1998-12-12_/
void Data_record::expect_float64_array
(const char *variable_or_section_label_, float64 *array, bool one_based ,nat16 max_index_,bool enumerate_variable)
{  if (!enumerate_variable)                                                      //030801
    set_current_section(variable_or_section_label_);
    for (nat16 i = one_based; i <= max_index_; i++)
      expect_float64(enum_variable(i,variable_or_section_label_,enumerate_variable),array[i]); //030801
}
//_expect_float64_array_____________________________________________1998-12-12_/
void Data_record::expect_float32_array_with_units_code_adjustable
(const char *variable_or_section_label_
,Units_clad &units_code
,float32 *array
,bool one_based
,nat16 max_index
,bool enumerate_variable)
{  if (!enumerate_variable)
       set_current_section(variable_or_section_label_);
   char units_variable[256]; units_variable[0] = 0;
   if (enumerate_variable)
   {  strcpy(units_variable,variable_or_section_label_);
      strcat(units_variable,"_");
   }
   strcat(units_variable,"units");
   expect_units(units_variable,units_code);
   expect_float32_array(variable_or_section_label_, array,  one_based , max_index,enumerate_variable);
}
//_expect_float32_array_with_units_code_adjustable__________________2007-04-03_/
void Data_record::expect_float32_array_with_units_code_fixed
(  const char *variable_or_section_label_
,  Units_code units_code_fixed,  float32 *array, bool one_based ,nat16 max_index,bool enumerate_variable)
{  if (!enumerate_variable)
       set_current_section(variable_or_section_label_);
    Units_code_item *units_code_item = new Units_code_item(units_code_fixed);
//070418 there are problems in some cases with ths units_codes is deallocated
//       I am not sure why,  Not adding units_code_item to the units_codes list is a memory leak
//       but this doesn't add much so it shouldn't hurt anything for now.
   char units_variable[256]; units_variable[0] = 0;
   if (enumerate_variable)
   {  strcpy(units_variable,variable_or_section_label_);
      strcat(units_variable,"_");
   }
   strcat(units_variable,"units");
   expect_units(units_variable,units_code_item->units_code);
   expect_float32_array(variable_or_section_label_, array,  one_based , max_index,enumerate_variable);
}
//_expect_float32_array_with_units_code_fixed_______________________2007-04-03_/
void Data_record::expect_float32_array_with_units_label
(const char *variable_or_section_label_
,const char *units_label, float32 *array, bool one_based ,nat16 max_index,bool enumerate_variable)
{  if (!enumerate_variable)
       set_current_section(variable_or_section_label_);
    Units_label_item *units_label_item = new Units_label_item(units_label);
    units_labels.append(units_label_item);
   char units_variable[256]; units_variable[0] = 0;
   if (enumerate_variable)
   {  strcpy(units_variable,variable_or_section_label_);
      strcat(units_variable,"_");
   }
   strcat(units_variable,"units");
    expect_string(units_variable,units_label_item->units_label,255);
   expect_float32_array(variable_or_section_label_, array,  one_based , max_index,enumerate_variable);
}
//_expect_float32_array_with_units_label________________2003-08-01__2001-11-28_/
void Data_record::expect_dynamic_float32_array
(const char *variable_or_section_label_
,CORN::Dynamic_array<float32> &array
,const nat8 &array_size
,bool one_based
,Units_code units_code_fixed
,bool enumerate_variable)
{  if (!enumerate_variable)
       set_current_section(variable_or_section_label_);
   char units_variable[256]; units_variable[0] = 0;
   if (enumerate_variable)
   {  strcpy(units_variable,variable_or_section_label_);
      strcat(units_variable,"_");
   }
   if (units_code_fixed)
   {  Units_code_item *units_code_item = new Units_code_item(units_code_fixed);
      strcat(units_variable,"units");
      expect_units(units_variable,units_code_item->units_code);
   }
  if (!enumerate_variable)
       set_current_section(variable_or_section_label_);

    //190416 WARNING this is problematic because at any time
    // the array could be reallocated
    // Instead, the dynamic array VV_item set value should index into the array

    for (nat16 i = one_based; i < (nat16)array.get_count(); i++)
      expect_float32(enum_variable(i,variable_or_section_label_,enumerate_variable),array.ref(i));
}
//_expect_dynamic_float32_array_____________________________________2016-09-18_/
void Data_record::expect_bool_array
(const char *variable_or_section_label_
,bool *array, VV_bool_entry::Form _text_form, bool one_based ,nat16 max_index,bool enumerate_variable)
{  if (!enumerate_variable)
       set_current_section(variable_or_section_label_);
    for (nat16 i = one_based; i <= max_index; i++)
      expect_bool(enum_variable(i,variable_or_section_label_,enumerate_variable),array[i],_text_form);
}
//_expect_bool_array____________________________________2003-08-01__1999-05-19_/
void Data_record::expect_int16_array
( const char *variable_or_section_label_
, int16 *array, bool one_based ,nat16 max_index,bool enumerate_variable)
{  if (!enumerate_variable)
       set_current_section(variable_or_section_label_);
   for (nat16 i = one_based; i <= max_index; i++)
      expect_int16(enum_variable(i,variable_or_section_label_,enumerate_variable),array[i]);
}
//_expect_int16_array___________________________________2003-08-01__1998-12-12_/
void Data_record::expect_nat32_array
(const char *variable_or_section_label_
,nat32 *array, bool one_based ,nat16 max_index, nat8 radix_,bool enumerate_variable)
{  if (!enumerate_variable)
       set_current_section(variable_or_section_label_);
   for (nat16 i = one_based; i <= max_index; i++)
      expect_nat32(enum_variable(i,variable_or_section_label_,enumerate_variable),array[i],radix_);
}
//_expect_nat32_array___________________________________2003-08-01__1998-12-12_/
void Data_record::expect_int32_array(
   const char *variable_or_section_label_,
    int32 *array, bool one_based ,nat16 max_index,bool enumerate_variable)
{  if (!enumerate_variable)
       set_current_section(variable_or_section_label_);
   for (nat16 i = one_based; i <= max_index; i++)
      expect_int32(enum_variable(i,variable_or_section_label_,enumerate_variable),array[i]);
}
//_expect_int32_array___________________________________2003-08-01__1998-12-12_/
void Data_record::expect_string_array
( const char *variable_or_section_label_                                         //030109
, std::string *array
, nat16 max_strlen_
, bool one_based
, nat16 max_index
, bool enumerate_variable)
{  if (!enumerate_variable)                                                      //030801
       set_current_section(variable_or_section_label_);
    for (nat16 i = one_based; i <= max_index; i++)
      expect_string
         (enum_variable(i,variable_or_section_label_,enumerate_variable)
         ,array[i],max_strlen_);                                                 //030801
}
//_expect_string_array______________________________________________1998-12-12_/
VV_Section *Data_record::set_current_section(const char *new_current_section)
{  if (strlen(new_current_section))                                              //030109
   {  {  current_section = (VV_Section *)(sections.find_cstr(new_current_section));   //030117
         if (!current_section)
         {  if (!structure_defined || (structure_defined && preserve_unexpected_entries)
                /*170307
                 * always preserve unexpected entries otherwise
                 * values default to the primary section.
                 * This was problem with crop [emergence] model= clobbering
                 * the [crop] model=
                 * */
                     ) //120311
            {  current_section = new VV_Section(new_current_section);
               sections.append(current_section);
            }
         }
      }
   }
   // else no name was specified, don't try to change the current section.       //030109
   return current_section;
}
//_set_current_section_________________________________________________________/
void Data_record::expect_enumerated_section
(VV_enumerated_section *enumerated_section_def
,bool for_write)
{  //Add the section prefix to a association list of section prefixes
   enumerated_sections_definitions.append(enumerated_section_def);
   std::string section_count(enumerated_section_def->section_label_prefix);      //131008_040909
   section_count.append(":count");                                               //040208
   expect_nat16 (section_count.c_str(),enumerated_section_def->count,10);        //120314
}
//_expect_enumerated_section___________________________________________________/
bool VV_enumerated_section::setup_structure
(Data_record &data_rec,bool for_write)                             modification_
{  data_rec.structure_defined =false;                                            //120314
   std::string current_section_key(data_rec.get_current_section()->get_label()); //040909
   nat32 actual_list_count = get_container_element_count();                      //401230
   set_count(for_write? actual_list_count :(nat32)count,false);                  //110719
   for (nat16 ndx = (int16)(one_based ? 1 : 0)                                   //981008
        ; ndx < (count + one_based) ; ndx++)                                     //981008
   {  // Now create a section for all the counted elements
     std::string section_ndx(section_label_prefix);                              //040909
     section_ndx.append(":");                                                    //040208
      char ndx_str[10];
      nat16_to_cstr(ndx,ndx_str,10);                                             //120314
      section_ndx.append(ndx_str);
     data_rec.set_current_section(section_ndx.c_str());                          //030109
      //        Expect entries for the section
     setup_element_structure(data_rec,for_write,ndx);                            //010103
   }
   data_rec.set_current_section(current_section_key.c_str());                    //010103
   // make sure we return to the original section
   return true;
}
//_setup_structure_____________________________________________________________/
void VV_enumerated_section::set_count(nat16 new_count,  bool unexpected_only)
{  if (!(unexpected_only && count))                                              //040323
       count= std::max<nat16>(new_count,count);
}
//_set_count________________________________________________________2012-03-14_/
bool VV_container_enumerated_section::setup_element_structure
(Data_record &data_rec,bool for_write, int16 index0)               modification_
{  Item *element =                                                               //990129
         one_based ? container.get_Nth((nat16) index0) :
          container.get_at((nat16) index0);
   if (element) element->setup_structure(data_rec,for_write);
   return element != 0;
}
//_setup_element_structure_____________________________________________________/
bool Data_record::has_structure()
{  return structure_defined; }
//_has_structure____________________________________________________2001-01-12_/
bool Data_record::delete_entry(const char *_section,const char *_entry)
{   bool at_least_one_entry_deleted = false;
    VV_Section *from_section = (VV_Section *)sections.find_cstr(_section);
    if (from_section)
      at_least_one_entry_deleted = from_section->entries.remove_all_matching_key_cstr(_entry);
    return at_least_one_entry_deleted;
}
//_delete_entry_____________________________________________________2003-01-17_/
modifiable_ VV_abstract_entry *Data_record::get_entry(const char *_section,const char *_entry)
{  modifiable_ VV_abstract_entry *found_entry = 0;
    VV_Section *from_section =
      dynamic_cast<VV_Section *>(_section ? sections.find_cstr(_section): sections.get_at(0));
    if (from_section)
      found_entry = dynamic_cast<modifiable_ VV_abstract_entry *>(from_section->entries.find_cstr(_entry));
   return found_entry;
}
//_get_entry________________________________________________________2011-11-07_/
bool Data_record::was_section_encountered(const char *section_name)        const
{  VV_Section *section = (VV_Section *)sections.find_cstr(section_name);
   bool encountered = section
   ?  section->was_encountered()                                                 //050927
   :  false;
   return encountered;
}
//_was_section_encountered__________________________________________2003-01-17_/
bool Data_record::was_entry_encountered(const char *section_name,const char *entry_name) const
{  VV_Section *section = (VV_Section *)sections.find_cstr(section_name);
   bool encountered = section
   ?  section->was_encountered()                                                 //050927
   :  false;
   if (encountered) // if the section was encountered, see if the entry was encountered
      encountered = section->was_entry_encountered(entry_name);
   return encountered;
}
//_was_entry_encountered____________________________________________2005-09-27_/
bool Data_record::expect_structure(bool for_write_unused)
{  UNUSED_arg(for_write_unused);
   structure_defined = false;                                                    //120314
   // If we don't have sections derived methods will crash. 021113
   if (sections.count() == 0)                                                    //021113
      set_current_section(primary_section_name.c_str());
   return true;
}
//_expect_structure_________________________________________________1998-04-05_/
bool Data_record::Units_label_item::is_key_string
(const std::string &key)                                            affirmation_ //180820
{ return units_label == key; }
//_is_key_string____________________________________________________2003-01-09_/
VV_Section *Data_record::get_current_section()
{  if (!current_section) current_section = set_current_section(get_primary_section_name());  //030709
   return current_section;
}
//_get_current_section______________________________________________2001-01-09_/
void Data_record::delete_section(const char * _section)
{  sections.remove((Item *)(sections.find_cstr(_section)));
}
//_delete_section___________________________________________________2003-01-17_/
bool Data_record::notify_indexer(CORN::Indexer &indexer)
{
   std::string buffer;                                                           //150730
   indexer.know_position(get_position());
   FOR_EACH_IN(section,VV_Section,sections,each_section)
   {  const char *section_label=section->get_label().c_str();                    //180820
      //for each entry in current section:
      FOR_EACH_IN(entry,VV_abstract_entry,section->entries,each_entry)
      {  // lookup field in actual fields:
         std::string entry_label; entry->append_variable(entry_label);           //181115
         buffer.clear(); entry->append_to_string(buffer);                        //150731
         indexer.know_value(buffer,entry_label.c_str(),section_label);           //181115
      } FOR_EACH_END(each_entry)
   } FOR_EACH_END(each_section)
   return indexer.commit();
}
//_notify_indexer___________________________________________________2005-11-08_/
nat16 Data_record::resolve_directory_entry_names
(const CORN::OS::Directory_name &with_respect_to_dirname)                  const //160114
{  nat16 directory_entry_names_encountered = 0;
   FOR_EACH_IN(section,VV_Section,sections,each_section)
   {
      FOR_EACH_IN(entry,VV_abstract_entry,section->entries,each_entry)
      {
         const CORN::VV_directory_entry_entry *entry_as_DEN =
            dynamic_cast<const CORN::VV_directory_entry_entry *>(entry);
         if (entry_as_DEN)
         {
            CORN::OS::Directory_entry_name &entry_DEN = entry_as_DEN->ref_value();
            if (entry_DEN.has_name(true))                                        //160121
            {
            if (entry_DEN.is_relative()
               || CORN::OS::file_system_engine.is_shortcut(entry_DEN)
               ||entry_DEN.has_designator_special()                              //171204
               )
            {  CORN::OS::Directory_entry_name *resolved_DEN =
                   CORN::OS::file_system_engine.
                      render_resolved_absolute_DEN
                      (entry_DEN,with_respect_to_dirname);                       //160114
                if (resolved_DEN)
                   entry_DEN.set_DEN(*resolved_DEN);
            } // if needs resolution
            if (!entry_DEN.is_qualified())
            {  std::wstring  name_with_extension;
               entry_DEN.get_name_with_extension(name_with_extension);
               CORN::OS::Directory_entry_name_concrete
                  DEN_qualified(with_respect_to_dirname,name_with_extension);
               entry_DEN.set_DEN(DEN_qualified);
            }
            directory_entry_names_encountered ++;
            }
         } // if (entry_as_DEN)
      } FOR_EACH_END(each_entry)
   } FOR_EACH_END(each_section)
   return directory_entry_names_encountered > 0;
   // returns the number of DENs encountered, whether actually needed resolving or not.
}
//_resolve_directory_entry_names____________________________________2015-12-07_/
bool Data_record::get_start()
{  set_current_section(get_primary_section_name());
   if (!has_structure()) expect_structure(false);
   return true;
}
//_get_start________________________________________________________2016-10-23_/
}//_namespace_CORN_____________________________________________________________/

