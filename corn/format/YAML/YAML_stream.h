#ifndef YAML_streamH
#define YAML_streamH

#include "corn/container/unilist.h"
#include "corn/container/text_list.h"
#include "corn/format/YAML/YAML_document.h"
#include "corn/format/structural_stream.h"

namespace YAML
{

//   class Document;
/*
   enum Construct_type                                                           //151025
      {
         construct_unknown
        ,construct_mapping
        ,construct_sequence
        ,construct_scalar
      };
*/
//______________________________________________________________________________
class Indentation
{public:
   nat16 level;
      // level 0 indicates no indentation (starting case)
   nat16 level_space_count[255];
      // the number of spaces adding to this level
      // not the total number of spaces to this level.
   bool  next_level_expected;

   //nat16 leading_spaces_encountered; // encountered since last new line
   //bool  reading_leading_spaces;
 public:
   Indentation();
   bool indent(nat16 next_level_spaces);
   bool unindent();



/*
   inline bool unindented()                                         affirmation_
      { return leading_spaces_encountered < level_spaces[level]; }
   inline bool at_expected_indentation()                            affirmation_
      { return leading_spaces_encountered == level_spaces[level]; }
//   bool stop_reading_indentation();
   bool new_line();
*/
   nat16 current_level_spacing() const;
};
//_Indentation_____________________________________________________________________________
class Stream
: public extends_ CORN::Structural_ifstream_Unicode
{
 protected:
   CORN::Text_list directives;
 protected:
   // only for debugging std::streampos start_line_strm_pos ;
   nat32 file_line;
   Indentation indentation;
 public:
   Stream(const CORN::OS::File_name &filename);
   virtual ~Stream();
   virtual nat8 get(structural::Collection &document)             modification_;
 protected:
   virtual nat8 parse_document
      (structural::Collection &document);
      /// \returns error code or 0 if no error
   Parse_error_code parse_document_known
      (structural::Collection &document);
      // This is used for documents that may have preamble text
      // in addition to actual YAML syntax.
 protected:
   structural::Construct *
   parse_construct
      (structural::Construct  *expected_construct
      ,structural::Map        *context_map

//      ,structural::Sequence   *context_sequence = 0
//       structural::Collection      *collection
//      ,structural::Construct       *expected_context
      // ,nat8                         indentation_level_expected
/*200630
      ,nat16                        indentation// = 0
      ,nat16                        leading_space_count
*/
      ); //= 0
/*
   Parse_error_code :parse_associative_array_inline
      (wchar_t                *text_remainder
      ,structural::Construct  &context_construct
      );
*/
   bool read_sequence
      (structural::Sequence &sequence);
   bool read_map
      (structural::Map &map);

 protected:
   nat16 process_directive(const std::wstring &directive);
   bool parse_to_start_of_document();
   /*obsolete
   nat32 read_value
      (std::wstring &value, std::wstring &comment)    ;
   */
   //200623 std::string *
   bool read_type_tag
      (
      //200624 std::wistream &strm,
      std::string &type_tag);                                                     //200623
//      ,wchar_t &chr0 ,wchar_t &chr1);
   bool read_end_document_marker();
   bool read_remainder_of_line_to_comment
      (std::wistream &strm
      ,modifiable_ structural::Construct &current_parsing_element);              //200216
      // Actually comment is common to construct for all document types
      // so this could be method on Construct

   structural::Map      *promote_to_map(structural::Construct &construct) rendition_;
   structural::Sequence *promote_to_sequence(structural::Construct &construct) rendition_;

   structural::Pair_key_value *expect_pair_key_value // inline_mode
      (
       //200624 std::wistream               &strm,
       structural::Map             &map
      //,nat16                        indentation// = 0
      //,nat16                        leading_space_count
      ); //= 0
   structural::Construct      *expect_item          // inline_mode
      (//200624 std::wistream               &strm,
      structural::Sequence        &sequence);
 protected:
   nat16 read_leading_spaces
      (nat16 &hyphen_encountered_position
      ,nat16  hyphen_expected_position);
 private:
   wchar_t safe_peek();

};
//______________________________________________________________________________
extern CORN::Container::Format item_format_unquoted_keys;
}//_namespace_YAML_____________________________________________________________/
#endif
