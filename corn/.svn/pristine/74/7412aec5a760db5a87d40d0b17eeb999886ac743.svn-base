#include "YAML_document.h"
#include <iostream>
//#include <stdlib.h>
#include <ctype.h>
#include "corn/const.h"
#include <assert.h>


namespace YAML
{
   Anchor *anchors[1000]; nat16 anchor_count;// Need to use dynamic structure
//______________________________________________________________________________
void clear_anchors()
{  anchor_count = 0;
   for (int a = 0; a < 1000 ; a++)
      anchors[a] = 0;
}
//______________________________________________________________________________

//______________________________________________________________________________
/*NYN checking if need to reimplement
nat8 Node::recognize_structure_indicator
(wchar_t chr0, wchar_t chr1)
{
   nat8 chars = 0;
   switch (chr0)
   {

      break;

      case L':' : // Value indicator.(key terminator)
                  if (iswspace(chr1))
                  {  chars = chr1 == '\r' ? 1 : 2; //eat colon and space
                     appears_to_be_key = true;
                     error = appears_to_be_key ? ERROR_none : ERROR_colon_encountered_but_not_key;
                  }
                  else
                  {
                     error = ERROR_value_colon_space_expected;
                  }
                  parsed = error == ERROR_none;
      break;




   } // switch
   return chars;
}
*/
//______________________________________________________________________________
/*NYN checking if need to reimplement
nat8 Sequence::recognize_structure_indicator(wchar_t chr0, wchar_t chr1)
{
   nat8 chars = 0;
   switch (chr0)
   {

      case L']'  :
      {
         chars = 1;
         parsed = true;
               //NYI
               //   Sequence *curr_sequence = dynamic_cast<Sequence *>(curr_node);
                //  if (curr_sequence) curr_node = 0;
                //  remaining_text = text + 1;

      } break;

   } // switch
   return chars;
}
*/
//______________________________________________________________________________
/*NYN checking if need to reimplement
Node *Node::resolve_node(given_ YAML::Node *child) // child is given
{
   // Called when the parser has found the termination
   // of the node text
   // (either the stream indentation) or the current text line is at key value separator or end of line with no expected continuation.
   // Derived classes will override this method and render
   // a more specialized node.

   YAML::Node *resolved_node = promote(child);
// promote always returns a promoted node (or the child)
//   if (resolved_node)
//   {
//      // promote deletes childeif (resolved_node != child) delete child;
//      // at this point resolved_node parent_node should  still point to this
//   } else resolved_node = child;

   //150119 resolved_node->resolved = true;
//   resolved_node->parent_node = this;  WARNING caller should set in_collection
   return resolved_node;
}
*/
//______________________________________________________________________________
/*NYN checking if need to reimplement
nat16 Block_scalar::parse_header(const wchar_t *text)
{
   nat16 consumed = 0;
   nat8 indent = 0;
   nat16 c = 0;
   wchar_t chr = text[c];
   switch (chr)
   {  case '-' : chomp = strip_chomp;  consumed++; break;
      case '+' : chomp = keep_chomp ;  consumed++; break;
      default :
         if (isdigit(chr))
         {
             nat16 digit = chr - 48;
             indent *= 10 + digit;
             consumed += 1;
         }
      break;
   }
   return consumed;
}
*/
//______________________________________________________________________________

/*
wchar_t *Mapping::parse_text(wchar_t *text)
{

   wchar_t *remaining_text = text;

   //if (!curr_key_value_pair)
   //   curr_key_value_pair = new Pair_key_value;
   if (curr_key_value_pair) // Then the key node will have been previously parsed
   {  // we must be parsing in the value node
      if (!curr_key_value_pair->get_value()->is_parsed())
         remaining_text = curr_key_value_pair->get_value()->parse_text(remaining_text);
      if (curr_key_value_pair->get_value()->is_parsed())
      {
         if (curr_key_value_pair->resolve())
            append(dynamic_cast<Pair_key_value *>(curr_key_value_pair));
         curr_key_value_pair = 0;
      }
   }
   else
   {
      if (specifier)                                                             //150219
      {
         // If this node has been identified as a generic mapping,
         // then the specifier will have been set and there will be no need for curr_key_node_pending
         generic_map_item_value = new YAML::Node(this,0);                        //150219
         remaining_text = generic_map_item_value->parse_text(remaining_text);    //150219
         parsed = generic_map_item_value->is_parsed();                           //150219
      }
      else
      {
         if (!curr_key_node_pending
            )
         {
            curr_key_node_pending = new YAML::Node(0,this); // Provide the key
            //curr_key_node_pending->key_of = this;
            //curr_key_node_pending->appears_to_be_key = true;
         }
         if (!curr_key_node_pending->is_parsed())
               remaining_text = curr_key_node_pending->parse_text(remaining_text);
            // else continue to the next line to resolved
         if (curr_key_node_pending->is_parsed())
         {
            resolve_node(curr_key_node_pending);

            curr_key_node_pending = 0;
         }
      }
   }
   return remaining_text;
}
*/
//______________________________________________________________________________
/*NYN checking if need to reimplement
Node *Mapping::resolve_node(given_  YAML::Node *key_child_given)
{  // Derived classes may override either this method
   // and/or  instanciate_key_value_pair to provide more specialized
   // resolved or partially resolved nodes
   YAML::Node *promoted_child = promote(key_child_given); // key

   promoted_child->resolve();                                                              //150205
   if (!curr_key_value_pair)
   {  // The element of map is key
      curr_key_value_pair = instanciate_key_value_pair
         (promoted_child,0);
      promoted_child = 0; // was consumed
   }
   return curr_key_value_pair->get_map_key();
}
*/
//______________________________________________________________________________
/*
YAML::Node *Mapping::get_current_value()
{
   return curr_key_value_pair->get_value();
}
//______________________________________________________________________________
YAML::Mapping *Mapping::get_current_mapping()
{
   return dynamic_cast<YAML::Mapping *>(curr_key_value_pair);
}
//______________________________________________________________________________
*/
/*NYN checking if need to reimplement
YAML::Node *Mapping::take_value(YAML::Node *value_relinquished)     modification_
{
   if (curr_key_value_pair->get_value() && !curr_key_value_pair->get_value()->is_resolved())
   {
      delete curr_key_value_pair->get_value();
      curr_key_value_pair->take_value(value_relinquished);
   } else delete value_relinquished;
   return curr_key_value_pair->get_value();
}
*/
//______________________________________________________________________________
/*NYN checking if need to reimplement
wchar_t *String::parse_text(wchar_t *text)
{
   // NYI
   return Node::parse_text(text);
}
*/
//______________________________________________________________________________
/*NYN checking if need to reimplement
bool Pair_key_value_interface::is_resolved()                        affirmation_
{  // This is a valid interface method because is only uses interface methods
   const YAML::Node *key = get_map_key();
   const YAML::Node *value = get_value();
   bool resolved = key && key->is_resolved();
   resolved &=
      (!value)
      ? true // mappings don't necessarily have a scalar value
      : value->is_resolved();
   return resolved;
}
*/
//______________________________________________________________________________
/*NYN checking if need to reimplement
bool Mapping::is_resolved()                                         affirmation_
{
   bool resolved = // get_map_key() && get_map_key()->is_resolved();
      Pair_key_value_interface::is_resolved();
   resolved &= is_parsed();
      // We don't care about value which will be null because
      // the value is the associated collection
      // (Probably should check that the collection is not null)
   resolved &= count();
      // according to http://www.yaml.org/spec/1.2/spec.html'
      // there must be at least one key value pair.
      // Having now pairs is probably an error but not an indicator of
      // not resolved.

   return resolved;
}
*/
//______________________________________________________________________________
/*NYN checking if need to reimplement
bool Scalar::resolve()                                             modification_
{  return unresolved_text
      ? resolve_unresolved_text()
      : true;
}
*/
//_2015-02-05___________________________________________________________________
/* 200620 obsolete
bool Node::promote_from(modifiable_ YAML::Parsing_element *element) modification_
{
   comment = element->comment; element->comment = 0; // takes the element comment
   bool was_set = set_text(*element->text);
   if (!was_set) // the text wasn't resolved so this is just an arbitrary node.
      { text = element->text;  element->text = 0; } // takes the element text
   return was_set;
}
*/
/*
, unresolved_text(0)                                                             //150119
, in_collection      (promote_from->in_collection)
, key_of             (promote_from->key_of)
, anchors            (promote_from->anchors)
, anchor_name        (promote_from->anchor_name)
, user_defined_type  (promote_from->user_defined_type)
, kind_or_type       (promote_from->kind_or_type)
, tag                (promote_from->tag)
, error              (promote_from->error)
, parsed             (true)
//150119 , resolved           (promote_from->resolved)
, appears_to_be_key  (promote_from->appears_to_be_key)
, inline_style(false)
, comment(0)
*/

/*
   promote_from->anchors = 0;
   promote_from->tag     = 0;
*/


//_Node::promote_from_______________________________________________2015-03-01_/
/*200620
bool String::promote_from(modifiable_ YAML::Parsing_element *element) modification_
{
   string_value = element->text; element->text =0; // takes the element text
   return Node::promote_from(element);
}
//_String::promote_from_____________________________________________2015-03-01_/
*/
/*200620 obsolete Now attempt to instanciate and Scalar_bool_cowl set_text
bool Bool::set_text_wstring(const std::wstring &_text)            modification_
{
   if      (_text == L"true")  bool_value = true;
   else if (_text == L"True")  bool_value = true;
   else if (_text == L"TRUE")  bool_value = true;
   else if (_text == L"yes")   bool_value = true;
   else if (_text == L"Yes")   bool_value = true;
   else if (_text == L"YES")   bool_value = true;
   else if (_text == L"false") bool_value = false;
   else if (_text == L"False") bool_value = false;
   else if (_text == L"FALSE") bool_value = false;
   else if (_text == L"no")    bool_value = false;
   else if (_text == L"No")    bool_value = false;
   else if (_text == L"NO")    bool_value = false;
   return true;
}
//_Bool::set_text_wstring___________________________________________2015-02-05_/
*/
/*200620
bool Int::set_text_wstr(const wchar_t *_text)                      modification_
{  int_value = CORN::wstr_to_int32(_text,10);
   return true;
}
//_Int::set_text_wstr_______________________________________________2015-02-05_/
bool Float::set_text_wstr(const wchar_t *_text)                    modification_
{  float_value = CORN::wstr_to_float64(_text);
   return true;
}
//_Float::set_text_wstr_____________________________________________2015-02-05_/
*/
/*
Document indicators:
    '%'  : Directive indicator.
    '---': Document header.
    '...': Document terminator.
Misc indicators:
    ' #' : Throwaway comment indicator.
    '`@' : Both reserved for future use.
Special keys:
    '='  : Default "value" mapping key.
    '<<' : Merge keys from another mapping.
Core types: # Default automatic tags.
    '!!map' : { Hash table, dictionary, mapping }
    '!!seq' : { List, array, tuple, vector, sequence }
    '!!str' : Unicode string
More types:
    '!!set' : { cherries, plums, apples }
    '!!omap': [ one: 1, two: 2 ]
Language Independent Scalar types:
    { ~, null }              : Null (no value).
    [ 1234, 0x4D2, 02333 ]   : [ Decimal int, Hexadecimal int, Octal int ]
    [ 1_230.15, 12.3015e+02 ]: [ Fixed float, Exponential float ]
    [ .inf, -.Inf, .NAN ]    : [ Infinity (float), Negative, Not a number ]
    { Y, true, Yes, ON  }    : Boolean true
    { n, FALSE, No, off }    : Boolean false
    ? !!binary >
        R0lG...BADS=
    : >-
        Base 64 binary value.
Escape codes:
 Numeric   : { "\x12": 8-bit, "\u1234": 16-bit, "\U00102030": 32-bit }
 Protective: { "\\": '\', "\"": '"', "\ ": ' ', "\<TAB>": TAB }
 C         : { "\0": NUL, "\a": BEL, "\b": BS, "\f": FF, "\n": LF, "\r": CR,
               "\t": TAB, "\v": VTAB }
 Additional: { "\e": ESC, "\_": NBSP, "\N": NEL, "\L": LS, "\P": PS }
*/
}//_namespace YAML_____________________________________________________________/
