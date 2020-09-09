#include "format/structure_factory_G.h"
#include "container/text_list.h"
#include <algorithm>
#include <iostream>
namespace structural
{
//______________________________________________________________________________
Factory_generic::Factory_generic(Map *document_map_)
: Factory                                                                     ()
, map_stack                                                                   ()
//, map_count                                                                  (0)
, map_index                                                                  (0)
, document_map                                                   (document_map_)
, current_map                                                                (0)
{
   for (int i = 0; i < 127; i++)
   {  map_stack[i] = 0;
   }
   map_stack[map_index] = current_map = document_map_;
}
//_Factory_generic:constructor_____________________________________2020-03-20_/

Construct *Factory_generic::render
(const char *type_tag)
{
   return render_construct_raw(type_tag);
   // The tag is the type of structure to be rendered
   // tag does not have bangbang introducer (I.e. str not !!str)
   // Normally this will be implemented in derived classes, but may want
   // to default to a generic Map

   /*200620
   Construct *rendered = 0;

   if      (type_tag == "str")      rendered = new Scalar_string_clad;
   else if (type_tag == "bool")     rendered = new Scalar_bool_clad;
   else if (type_tag == "int")      rendered = new Scalar_number_clad(CORN::NT_int32);
   else if (type_tag == "float")    rendered = new Scalar_number_clad(CORN::NT_float32);
  //NYI else if (type_tag == "enum")     rendered = new Scalar_enum_clad;
   else if (type_tag == "seq")      rendered = new Sequence_clad;
   else if (type_tag == "map")      rendered = new Map_clad;
   else rendered = new Map_clad;

   return rendered;
   */
}
//_render___________________________________________________________2020-03-15_/

Pair_key_value *Factory_generic::render_key_value_compact
(const char *key_compact
,const char *type_tag)
{  Pair_key_value *rendered = 0;
   // The key may be simple xxx  or compact xxx.yyy
   // The tag is the type of structure to be rendered

   /*
   if (type_tag)
   {
      if (type_tag == "xxx")
      {
         rendered = new Pair_key_xxxx
      } else

   } else
   */
      rendered = new
         Pair_key_value
            (new Scalar_string_clad(key_compact)
            ,new Scalar_string_clad(""));
   return rendered;
}
//_render_key_value_compact_________________________________________2020-03-15_/
Pair_key_value &Factory_generic::provide_key_value_compact
(const char *key_compact
,const char *type_tag)
{
   Pair_key_value *key_value_ultimate = 0;
   // The key may be simple xxx  or compact xxx.yyy
   // The tag is the type of structure to be rendered

   // This will be reimplemented in derived classes that
   // know specific keys and/or type_tags

   std::string curr_key;
   nat16 key_compact_length = strlen(key_compact);
   current_map = document_map;
   const char *ultimate_separator_loc = strrchr(key_compact,'.');
   for (nat16 key_i = 0; key_i < key_compact_length; key_i++)
   {
      char key_ch = key_compact[key_i];
      const char *key_compact_remaining = key_compact + key_i+1;
      if (key_ch == COMPACT_SEPARATOR)
      {
            const char *separator_loc = key_compact + key_i;
            if (separator_loc == ultimate_separator_loc)
            {  // The remaining text is ultimate key
               key_value_ultimate = &provide_key_value_scalar
                  (key_compact_remaining,type_tag);
               current_map->take(key_value_ultimate);
            } else
            {
               Pair_key_map &next_key_map = dynamic_cast<Pair_key_map &>
                  (provide_key_value_compact(key_compact_remaining,type_tag));
               current_map->take(&next_key_map);
               current_map = next_key_map.map;
            }
      } else curr_key += key_ch;
   } // for

   return *key_value_ultimate;
}
//_provide_key_value_compact________________________________________2020-03-15_/
Pair_key_value &Factory_generic::provide_key_value_scalar
(const char *key_unqual
,const char *type_tag)
{
   Scalar_string_clad *key = new Scalar_string_clad(key_unqual);
   Construct * value = render(type_tag);
   Pair_key_value *pair = new Pair_key_value(key,value);
   return *pair;
}
//_provide_key_value_scalar_________________________________________2020-03-15_/
bool Factory_generic::expect_map
(const char *compact_key // with colon full qualification without is imposed structure
,const char *type_tag)                                              expectation_
{




















































































































































{  return expect_T<Scalar_string_cowl,std::string>(compact,as_str);
/*obsolete now use expect_T
   Pair_key_value *key_value_pair = provide_map_scalar(compact);







   return expect_T<Scalar_bool_cowl,bool>(compact,as_bool);
/*obsolete now use expect_N
   Pair_key_value *key_value_pair = provide_map_scalar(compact);
   Scalar_bool *scalar = new Scalar_bool(as_bool);



//_____________________________________________________________________________/
bool Factory_generic::expect(const char *compact,char &as_char)    expectation_

   return expect_N<Scalar_number_clad,char>(compact,as_char);
/*obsolete now use expect_N
   Pair_key_value *key_value_pair = provide_map_scalar(compact);
   Scalar_number *scalar = new Scalar_number();





bool Factory_generic::expect(const char *compact,nat8 &as_nat8)    expectation_
{
   return expect_N<Scalar_number_clad,nat8>(compact,as_nat8);
/*obsolete now use expect_N
   Pair_key_value *key_value_pair = provide_map_scalar(compact);
   Scalar_number *scalar = new Scalar_number();






{


   Pair_key_value *key_value_pair = provide_map_scalar(compact);








   return expect_N<Scalar_number_clad,nat32>(compact,as_nat32);

   Pair_key_value *key_value_pair = provide_map_scalar(compact);






bool Factory_generic::expect(const char *compact,int8 &as_int8)    expectation_
{
   return expect_N<Scalar_number_clad,int8>(compact,as_int8);

   Pair_key_value *key_value_pair = provide_map_scalar(compact);






bool Factory_generic::expect(const char *compact,int16 &as_int16)  expectation_
{
   return expect_N<Scalar_number_clad,int16>(compact,as_int16);

   Pair_key_value *key_value_pair = provide_map_scalar(compact);






bool Factory_generic::expect(const char *compact,int32 &as_int32)  expectation_
{
   return expect_N<Scalar_number_clad,int32>(compact,as_int32);
/*obsolete now use expect_N
   Pair_key_value *key_value_pair = provide_map_scalar(compact);
   Scalar_number *scalar = new Scalar_number();





bool Factory_generic::expect(const char *compact,float32 &as_float32) expectation_
{
   return expect_N<Scalar_number_clad,float32>(compact,as_float32);

   Pair_key_value *key_value_pair = provide_map_scalar(compact);






bool Factory_generic::expect(const char *compact,float64 &as_float64) expectation_
{
   return expect_N<Scalar_number_clad,float64>(compact,as_float64);

   Pair_key_value *key_value_pair = provide_map_scalar(compact);






bool Factory_generic::expect(const char *compact,Enum_abstract &as_enum) expectation_
{


   Pair_key_value *key_value_pair = provide_map_scalar(compact);







