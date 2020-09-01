#ifndef YAML_documentH
#define YAML_documentH

#include "corn/format/structural.h"
#include "corn/container/item.h"
//#include "corn/container/container.h"
#include "corn/container/unilist.h"
#include "corn/string/strconv.hpp"
#include <assert.h>

/*
   Although typically YAML document contains maps ,
   this isnt necessarily the case,
   it may have as the outtermost structure
   a sequence/list

   Thus YAML documents may have any structural morphology

   The YAML document will have optional anchors each of which
   reference an assocated encountered structural element
   (typically a map ?but not necessarily?)
   Currently achors are a feature of YAML so only implemented
   here, but this could be more generally applicable
   so may want to move anchor implementation to structural
*/

/*200620 obsolete
namespace YAML
{
   //interface_ Pair_key_value_interface;
   class Node;
}
*/

namespace YAML
{
   class Mapping;
//______________________________________________________________________________
enum Parse_error_code
{ ERROR_none
, ERROR_unknown
, ERROR_indentation_unexpected_character
, ERROR_value_question_space_expected
, ERROR_value_colon_space_expected
, ERROR_colon_encountered_but_not_key
, ERROR_document_start_not_found
};
//______________________________________________________________________________
enum Chomp
{  no_chomp
,  strip_chomp
,  keep_chomp
};
//______________________________________________________________________________
/*
enum Node_type
{  unknown_node_type
,  scalar_node_type
,  sequence_node_type
,  mapping_node_type
, NODE_TYPE_COUNT
};
*/
//_Forward declarations_________________________________________________________
class Sequence;
class Mapping;
//class Sequence;
class Block_literal_style;
class Block_folded_style;
//151019 class Pair_key_value;
//200620 class Node;
//______________________________________________________________________________
class Tag
{
};
//______________________________________________________________________________
/*200620 obsolete
class Node
: public implements_ structural::Construct
{

 public:
   std::wstring *text;
   std::wstring *comment;


 public:

   Node();
   virtual bool promote_from(modifiable_ YAML::Parsing_element *element) modification_;
      // Cannibalizes element
   inline virtual bool is_key_wstring(const std::wstring &key)      affirmation_  //180820
      { return (*text) == key; }



#ifdef TEMP_DISABLED
 public:
   virtual wchar_t *parse_text_indented(wchar_t *text);
      /// \return 0 if the character is consumed
      /// \return error code if the character is unexpected/recognized.

   virtual wchar_t *parse_text_unindented(wchar_t *text);




   inline void know_anchors(CORN::Container *_anchors) cognition_
      {  anchors = _anchors;
      }

   nat16 get_indentation_total()                                           const;
   virtual bool expect_structure() modification_ {}
      // Derived classes may define an expected structure
      // of recognized constructs.
      // This is an optional
#endif
};
//______________________________________________________________________________
*/
/*200620 obsolete  use structural::
class Scalar
: public extends_ YAML::Node
{
   //The content of a scalar node is an opaque datum that can be presented as a series of zero or more Unicode characters.

 public:
   Scalar()
      : YAML::Node()
      {}
};
//______________________________________________________________________________
*/
/*200620 obsolete  use structural::Scalar_string
class String
: public extends_ Scalar
{
   std::wstring *string_value;
 public:
   inline String()
      : Scalar()
      , string_value(0)
      {}
   virtual bool promote_from(modifiable_ YAML::Parsing_element *element) modification_;

};
//______________________________________________________________________________
*/
// Blocks are kind of YAML specific

class Block_scalar
: public extends_ structural::Scalar_string_clad //200620 Scalar  // May be string
{
   nat16 indent;
   Chomp chomp;
 public:
};
//______________________________________________________________________________
class Block_literal_style
: public extends_ Block_scalar
{
 public:
   inline virtual bool newline_ends_node()                          affirmation_ { return false; }

};
//______________________________________________________________________________
class Block_folded_style
: public extends_ Block_scalar
{
 public:
   inline virtual bool newline_ends_node()                          affirmation_ { return false; }

};
//______________________________________________________________________________
class Null
: public extends_ structural::Construct_arbitrary
{
 public:
   inline Null()
      : structural::Construct_arbitrary()
      {}
};
//______________________________________________________________________________
/*200620 obsolete used structural::Scalar_bool_clad
class Bool
: public extends_ Scalar
{
   bool bool_value;
 public:
   inline Bool()
      : Scalar()
      , bool_value(false)
      {}
   virtual bool set_text_wstring(const std::wstring &_text)        modification_;
 };
//______________________________________________________________________________
*/
/*200620 obsolete used structural::Scalar_nmber_clad
class Int
: public extends_ Scalar
{
   int int_value;
 public:
   inline Int()
      : Scalar()
      , int_value(0)
      {}
   virtual bool set_text_wstr(const wchar_t *_text)                modification_;

};
//______________________________________________________________________________
*/
/*200620 obsolete used structural::Scalar_number_clad
class Float
: public extends_ Scalar
{
   float float_value;
 public:
   inline Float()
      : Scalar()
      , float_value(0.0) // Might want to use NAN
      {}
   virtual bool set_text_wstr(const wchar_t *_text)                modification_;
};
*/
//______________________________________________________________________________
/*200619 obsolete
class Sequence
: public extends_ structural::Sequence_clad
, public extends_ YAML::Node
{
   //The content of a sequence node is an ordered series of zero or more nodes.
   //In particular, a sequence may contain the same node more than once.
   //It could even contain itself (directly or indirectly).

 public:
   inline Sequence
      (
      // check if need to reimplement const YAML::Node  *_in_collection
      //,YAML::Node  *_key_of
      )
      {}
   inline Sequence(YAML::Node &promote_from_node)
      {}
   inline virtual bool is_case_sensitive()                const { return true; }
   inline virtual nat32 count()                              const { return 0; } // place holder
};
//______________________________________________________________________________
*/

   /* The content of a mapping node is an unordered set of key: value node pairs,
    with the restriction that each of the keys is unique.
    YAML places no further restrictions on the nodes.
    In particular, keys may be arbitrary nodes, the same node may be used as
    the value of several key: value pairs, and a mapping could even contain itself
    as a key or a value (directly or indirectly).
   */
      /* May be 0 for the upper most document level
        For example:

        ---
        one: 1
        two: 2
        three: 3
        ...
        The document itself is a mapping but it has no specific sections

        ---
        sectionA:
          one: 1
          two: 2
          three: 3
        ...
        Here the document itself is a mapping,
        sectionA is a mapping specified by simple string scalar "sectionA"
        This is the most common usage.


        ---
        [ A 1 ]:
          one: 1
          two: 2
          three: 3
        ...
        In this case the first mapping is specfied by a Sequence with values "A" 1

        The specifier may itself serve as a key when the mapping is
        itself a member of a mapping.

        The spacifier can be any kind of node, even a complex collection.
       */


//______________________________________________________________________________
class Anchor
: public implements_ CORN::Item
{
   std::wstring ID;
   const structural::Construct *associated_construct;
      // probably should be a reference, at the point
      // the achor is read we should have the associated constructruct
 public:
   Anchor
      (const std::wstring &_ID
      ,const structural::Construct *_associated_construct)
      : ID(_ID)
      , associated_construct(_associated_construct)
      {}
   // Need to override get key
};
//_Anchor______________________________________________________________________/
void clear_anchors();
//______________________________________________________________________________
}//_namespace_YAML_____________________________________________________________/
#endif

