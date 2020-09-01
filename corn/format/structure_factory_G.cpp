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
   std::string specific_map_type(type_tag ? type_tag : "map");


   std::string compact_key_clad(compact_key);
   size_t colon_pos = compact_key_clad.find(":");
   if (colon_pos == std::string::npos)
   {  // using imposed structure model


      Scalar_string_clad *key = new Scalar_string_clad(compact_key);
      Pair_key_value *pair = current_map->provide_key_value_pair(key,"map");
         // Usually this will be a new pair, which would be
         // instanciated by current_map, but it is acceptable
         // to continue/superceed a previously composed map.

      Construct *value = pair->get_value();
      Map *value_as_map = dynamic_cast<Map *>(value);
      // If there is already a value (previously constructed),
      // we are expecting it to be a map
// We will always have a provided value, it will be a map unless there has already been defined a pair with this key
//      if (value)
//      {
         if (value_as_map)
         {
            map_stack[++map_index] = current_map = value_as_map;
         } else
         {
            std::cerr << "error: " << "expecting map " << compact_key
               << " but previously encountered pair is not a map" << std::endl;
            return false;
         }
//      }
/*
      {
         Map_clad *value_map = new Map_clad;
         map_stack[++map_index] = current_map = value_map;
         pair->take_value(value_map);
      }
U/
      /*
      Pair_key_map *pair_as_key_map = dynamic_cast<Pair_key_map *>(pair);
      if (pair_as_key_map)
      {
               Map *value_as_map = dynamic_cast<Map *>(pair_as_key_map->map);
               map_stack[map_index++] = value_as_map;
               map_stack[map_index] = 0;
               current_map = value_as_map;
      } else
      {

      }
      */
   }
   else
   {
      assert(false);

      // check implementation
      // I intend that one could start with imposed structure model
      // and mix map qualification

      // compact map qualification (i.e. xxx.yyy.zzz)
      // Currently assuming fully qualified
      // Generally all espectations would be fully qualified,
      // but it is perfectly acceptable to have following qualifications
      // using indentation idiom, so we maintain the stack.
      CORN::Text_list qualification(compact_key,COMPACT_SEPARATOR);
      map_index = 0;
      map_stack[0] = current_map = document_map;
      for (CORN::Item_string *key_name
            = dynamic_cast<CORN::Item_string *>(qualification.pop_at_head())
          ;key_name
          ;key_name
             = dynamic_cast<CORN::Item_string *>(qualification.pop_at_head()))
      {
         Scalar_string_clad *key = new Scalar_string_clad(key_name);
         Pair_key_value *pair = current_map->provide_key_value_pair(key,specific_map_type);
            // key relinquished
            // item provided
         Pair_key_map *pair_as_key_map = dynamic_cast<Pair_key_map *>(pair);
         if (pair_as_key_map)
         {
            Map *value_as_map = dynamic_cast<Map *>((pair_as_key_map->map));
            map_stack[map_index++] = value_as_map;
            map_stack[map_index] = 0;
            current_map = value_as_map;
         } else return false;
            // Should not occur but would indicate the programmer had defined
            // an item (probably scalar) with the same name as the map at the same level.
      }
   }
   return true;
}
//_____________________________________________________________________________/
bool Factory_generic::expect_map_end()
{  bool ended = map_index >= 1; // I think
   if (ended)
   {  map_stack[map_index--] = 0;
      //map_index--;
      current_map = map_stack[map_index];
   }
   else       std::cerr << "error: map end mismatch" << std::endl;
   return true;
}
//_expect_map_end______________________________________________________________/
Pair_key_value *Factory_generic::provide_map_scalar
(const char *compact_cstr) provision_
{
   Pair_key_value *key_value_pair_scalar=0;
   Key_string *scalars_key = 0;
   std::string compact_str(compact_cstr);
   size_t rightmost_colon_pos = compact_str.rfind(":");
   if (rightmost_colon_pos == std::string::npos)
   {  // imposed structure mode
      scalars_key = new Key_string(compact_cstr); // unqualified key
      key_value_pair_scalar = current_map->provide_key_value_pair
         (scalars_key,"map");
   } else
   {  // qualified mode

      // 200623 Warning this needs to be reimplemented


      nat8 map_count = map_index+1;
      nat8 colon_count
      = std::count(compact_str.begin(), compact_str.end(), '.');
      if (colon_count > map_count)
      {
         std::cerr << "error: there are more maps in the qualification than have currently been encountered in map expectation" << std::endl;
         return 0;
      } else
      {  // pop the qua maps
         while (map_count > colon_count)
         {
            map_stack[colon_count--] = 0;
            current_map = map_stack[colon_count];
         }
         std::string scalars_key_raw(compact_str,rightmost_colon_pos);
         // continue here scalars_key = new Key_string(indented);
      }
      key_value_pair_scalar = new Pair_key_value(scalars_key,0);
         // the value for the pair will be empty, to be provided by caller.
      current_map->take(key_value_pair_scalar);
   }
   return key_value_pair_scalar;
}
//_provide_map_scalar_______________________________________________2020-03-20_/
bool Factory_generic::expect(const char *compact,std::string &as_str) expectation_
{  return expect_T<Scalar_string_cowl,std::string>(compact,as_str);
/*obsolete now use expect_T
   Pair_key_value *key_value_pair = provide_map_scalar(compact);
   Scalar_string_cowl *scalar = new Scalar_string_cowl(as_str);
   return key_value_pair->take_value(scalar) != 0;
*/
}
//_expect______________________________________________________________________/
bool Factory_generic::expect(const char *compact,bool &as_bool)    expectation_
{
   return expect_T<Scalar_bool_cowl,bool>(compact,as_bool);
/*obsolete now use expect_N
   Pair_key_value *key_value_pair = provide_map_scalar(compact);
   Scalar_bool *scalar = new Scalar_bool(as_bool);
   return key_value_pair->take_value(scalar) != 0;
*/
}
//_____________________________________________________________________________/
bool Factory_generic::expect(const char *compact,char &as_char)    expectation_
{
   return expect_N<Scalar_number_clad,char>(compact,as_char);
/*obsolete now use expect_N
   Pair_key_value *key_value_pair = provide_map_scalar(compact);
   Scalar_number *scalar = new Scalar_number();
   scalar->associate_value<char>(as_char);
   return key_value_pair->take_value(scalar) != 0;
*/
}
//_____________________________________________________________________________/
bool Factory_generic::expect(const char *compact,nat8 &as_nat8)    expectation_
{
   return expect_N<Scalar_number_clad,nat8>(compact,as_nat8);
/*obsolete now use expect_N
   Pair_key_value *key_value_pair = provide_map_scalar(compact);
   Scalar_number *scalar = new Scalar_number();
   scalar->associate_value<nat8>(as_nat8);
   return key_value_pair->take_value(scalar) != 0;
*/
}
//_____________________________________________________________________________/
bool Factory_generic::expect(const char *compact,nat16 &as_nat16)  expectation_
{
   return expect_N<Scalar_number_clad,nat16>(compact,as_nat16);
/*obsolete now use expect_N
   Pair_key_value *key_value_pair = provide_map_scalar(compact);
   Scalar_number *scalar = new Scalar_number();
   scalar->associate_value<nat16>(as_nat16);
   return key_value_pair->take_value(scalar) != 0;
*/
}
//_____________________________________________________________________________/
bool Factory_generic::expect(const char *compact,nat32 &as_nat32)  expectation_
{
   return expect_N<Scalar_number_clad,nat32>(compact,as_nat32);
/*obsolete now use expect_N
   Pair_key_value *key_value_pair = provide_map_scalar(compact);
   Scalar_number *scalar = new Scalar_number();
   scalar->associate_value<XXXX>(as_XXXX);
   return key_value_pair->take_value(scalar) != 0;
*/
}
//_____________________________________________________________________________/
bool Factory_generic::expect(const char *compact,int8 &as_int8)    expectation_
{
   return expect_N<Scalar_number_clad,int8>(compact,as_int8);
/*obsolete now use expect_N
   Pair_key_value *key_value_pair = provide_map_scalar(compact);
   Scalar_number *scalar = new Scalar_number();
   scalar->associate_value<XXXX>(as_XXXX);
   return key_value_pair->take_value(scalar) != 0;
*/
}
//_____________________________________________________________________________/
bool Factory_generic::expect(const char *compact,int16 &as_int16)  expectation_
{
   return expect_N<Scalar_number_clad,int16>(compact,as_int16);
/*obsolete now use expect_N
   Pair_key_value *key_value_pair = provide_map_scalar(compact);
   Scalar_number *scalar = new Scalar_number();
   scalar->associate_value<XXXX>(as_XXXX);
   return key_value_pair->take_value(scalar) != 0;
*/
}
//_____________________________________________________________________________/
bool Factory_generic::expect(const char *compact,int32 &as_int32)  expectation_
{
   return expect_N<Scalar_number_clad,int32>(compact,as_int32);
/*obsolete now use expect_N
   Pair_key_value *key_value_pair = provide_map_scalar(compact);
   Scalar_number *scalar = new Scalar_number();
   scalar->associate_value<XXXX>(as_XXXX);
   return key_value_pair->take_value(scalar) != 0;
*/
}
//_____________________________________________________________________________/
bool Factory_generic::expect(const char *compact,float32 &as_float32) expectation_
{
   return expect_N<Scalar_number_clad,float32>(compact,as_float32);
/*obsolete now use expect_N
   Pair_key_value *key_value_pair = provide_map_scalar(compact);
   Scalar_number *scalar = new Scalar_number();
   scalar->associate_value<XXXX>(as_XXXX);
   return key_value_pair->take_value(scalar) != 0;
*/
}
//_____________________________________________________________________________/
bool Factory_generic::expect(const char *compact,float64 &as_float64) expectation_
{
   return expect_N<Scalar_number_clad,float64>(compact,as_float64);
/*obsolete now use expect_N
   Pair_key_value *key_value_pair = provide_map_scalar(compact);
   Scalar_number *scalar = new Scalar_number();
   scalar->associate_value<XXXX>(as_XXXX);
   return key_value_pair->take_value(scalar) != 0;
*/
}
//_____________________________________________________________________________/
bool Factory_generic::expect(const char *compact,Enum_abstract &as_enum) expectation_
{
   return expect_T<Scalar_enum_cowl,Enum_abstract>(compact,as_enum);
/*obsolete now use expect_N
   Pair_key_value *key_value_pair = provide_map_scalar(compact);
   Scalar_number *scalar = new Scalar_number();
   scalar->associate_value<XXXX>(as_XXXX);
   return key_value_pair->take_value(scalar) != 0;
*/
}
//_____________________________________________________________________________/
}//_namespace_structural_______________________________________________________/

