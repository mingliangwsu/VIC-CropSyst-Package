#include "YAML_stream.h"
#include "YAML_document.h"
#include "corn/string/strconv.hpp"
#include <assert.h>
#include <ctype.h>
#include <iostream>
#include <sstream>
using namespace structural;
namespace YAML
{
//______________________________________________________________________________
Indentation::Indentation()
: level                       (0)
, next_level_expected         (false)
//, leading_spaces_encountered  (0)
//, reading_leading_spaces      (true)
{
   for (int lev = 0; lev < 127; lev ++)
      level_space_count[lev] = 0;
}
//______________________________________________________________________________
/* newline says nothing about indentation the may be consecutive blank lines
bool Indentation::new_line()
{
//   reading_leading_spaces = true;
   leading_spaces_encountered = 0;
   return true;
}
//______________________________________________________________________________
*/
bool Indentation::indent(nat16 next_level_spaces)
{  level++;
   level_space_count[level] = next_level_spaces;
   return true;
}
//______________________________________________________________________________
bool Indentation::unindent()
{
   level_space_count[level] = 0;
   level --;
   next_level_expected = false;
   return true;
}
//______________________________________________________________________________
nat16 Indentation::current_level_spacing() const
{
   nat16 expected = 0;
   for (nat16 lvl = 1; lvl <= level; lvl++)
      expected += level_space_count[lvl];
   return expected;
}
//______________________________________________________________________________

/*
bool Indentation::stop_reading_indentation()
{
//   reading_leading_spaces = false;
   return true;
}
//______________________________________________________________________________
*/
Stream::Stream(const CORN::OS::File_name &filename)
: CORN::Structural_ifstream_Unicode(filename)
, file_line(1)
, indentation()
{
}
//______________________________________________________________________________
Stream::~Stream()
{}
//______________________________________________________________________________
nat8 Stream::parse_document
(Collection &document)
{
   structural::Map *doc_map = dynamic_cast<structural::Map *>(&document);
   // usually a document is a map
   // Since the document map it the construct to parse
   // there would not be a promoted context
   parse_construct(
      0
      ,doc_map
      //200624 *this,
      // &document
      //,&document
      //,0
      );
   return ERROR_none;
}
//______________________________________________________________________________
nat16 Stream::read_leading_spaces
(nat16 &hyphen_encountered_position
,nat16  hyphen_expected_position
)
{
   nat16 leading_space_count = 0;
   wchar_t pot_space = 0;
   wchar_t consumed = 0;
   bool spacing_encountered = true;

   do
   {
      pot_space = safe_peek();
      switch (pot_space)
      {
         case L' ' :
         {
           consumed = std::wistream::get();
           leading_space_count ++;
         } break;
         case L'-' :
         {
            consumed = std::wistream::get();
            hyphen_encountered_position = leading_space_count;
            // If the hyphen is encountered in expected position.
            // it is considered a space.
            if (hyphen_encountered_position == hyphen_expected_position)
               leading_space_count++;
         } break;
         case L'\r':
         {  // do nothing with CR  (may occur in Windows files)
            consumed = std::wistream::get();

         } break;
         case L'\n' :
         {
            // If we have been reading indentation spacing and get new line
            // this it is essentially a blank line.

            // dont consume newline let caller deal with ith.
            if (hyphen_encountered_position != 0xFFFF)
               leading_space_count = 0;
            spacing_encountered = false;
         } break;
         default :
            // dont consume char let caller deal with ith.
            spacing_encountered = false;
         break;
      } // switch
      if (!good() || eof())
      {
         spacing_encountered = false;
         leading_space_count = 0;
      }
   } while (spacing_encountered);
   return leading_space_count;
}
//______________________________________________________________________________
structural::Construct * // returns promoted context
Stream::parse_construct
(
 structural::Construct     *expected_construct
,structural::Map           *context_map
//,structural::Collection      *collection
//, structural::Sequence     *context_sequence




// ,nat8 indentation_level_expected
/*200630
,nat16                        indentationX
,nat16                        leading_space_countX
*/
)
{
/*
   if (indentation.unindented())
   {
      indentation.unindent();
      return 0;
   }
*/
   // bool possibly_in_indentation = true;
   //200705 read_leading_spaces now handles empty lines bool line_is_empty = true;                                                    //200620
   structural::Construct *current_context  = expected_construct;
   structural::Construct *return_context = 0;

//   wchar_t chr0;
//   wchar_t chr1;


   std::string type_tag; read_type_tag(type_tag);



   nat16 hyphen_encountered_position = 0xFFFF;


   nat16 leading_space_count =
         read_leading_spaces(hyphen_encountered_position,0xFFFF);

   nat16 current_level_spaces = indentation.current_level_spacing();
   if (indentation.next_level_expected)
   {

      if (leading_space_count < current_level_spaces)
      {
         indentation.unindent();
         return 0; // end of construct
      }

      if (leading_space_count > current_level_spaces)
      {
         nat16 next_level_spaces = leading_space_count - current_level_spaces;
         indentation.indent(next_level_spaces);
      }
      else std::cerr << "error: unexpected indentation encountered line " << file_line << std::endl;
   }

   // if there is a hypen, then we are reading a sequence

   structural::Sequence     *context_sequence =
      dynamic_cast<structural::Sequence     *>(expected_construct);
   if ((hyphen_encountered_position != 0xFFFF))
   {

      if (!context_sequence)                                            //160222
      {
                  // The context
                  context_sequence                              //160222
                     = new structural::Sequence_clad();                          //200619
                  // This sequence is just to store arbitrary constructs.
                  // since the containing document does define
      }
      structural::Construct *item_identified = 0;
      do
      {
         structural::Construct *item
                  = context_sequence->render_item(type_tag);
               // The sequency may be derived and may know/have a specific
               // expected type/structure which it may instanciate.
               // And/Or the file prepared by the user may have type tag,
               // Or the type tag may be basic type indicator (I.e. YAML)
               // In these case item will be specialized.
               // Otherwise the item is a simple place holder (construct)
         //      structural::Map_clad *item_as_map = dynamic_cast<structural::Map_clad *>(item);
         // continue here parse construct

         structural::Construct *item_identified                            //151025
            = parse_construct
               (item
               ,0
               );
      } while (item_identified);
      return context_sequence;
   }

   // At this point we have identified indentation and will
   // be at a special character or reading data.

   do
   {


   //wchar_t next_character = safe_peek();

   //switch (next_character)


   //obs structural::Construct *promoted_context = 0;
   if (!current_context)
        current_context = new structural::Construct_arbitrary;
   // The Node structure contents are necessarily owned by the stream
   // once nodes are resolved they are generally relinquished
   // to the document.

   std::wstring unresolved;                                        //200216
   //structural::Map      *expected_map      = dynamic_cast<structural::Map      *>(collection);   //200619
   //structural::Sequence *context_sequence = dynamic_cast<structural::Sequence *>(collection);
         // rename to context sequence
   //debug start_line_strm_pos = tellg();
      // currently start_line_strm_pos is not used (was used for logging)
   //std::clog << "{pc_slsp: " << start_line_strm_pos << "}";
   wchar_t chr0 = 0;
   wchar_t chr1 = safe_peek();
      //200624 bool line_essentially_empty = false;
//      wchar_t chr0 = std::wistream::get();
   if ((chr1 == 0) || (chr1 == 0xFFFF)|| eof())
      return 0;

//200705      if (!std::isspace(chr0)) indentation.stop_reading_indentation();            //200703

      /*200703
      if (possibly_in_indentation && !std::isspace(chr0))                        //200624
          possibly_in_indentation = false;                                       //200624
      */
   bool end_construct = false;
//   do
//   {
      chr1 = safe_peek();


      switch (chr1)
      {  case L'\r' :
         {  // Files from Windows have \r at end of line.
            chr0 = std::wistream::get();
         } break;
         case L'\n' :                                                          //150830
         {
            chr0 = std::wistream::get();
            file_line++;
            //indentation.new_line();                                                 //200703
            end_construct = true;
         } break;
         case L'.' :
         {
            file_line++;
            if (read_end_document_marker())
               end_construct = true; // we are at the end of the document
        } break;
      }

      if (end_construct)
      {
         if (!unresolved.empty())
         {
               if (!return_context)
                    return_context=new structural::Scalar_string_clad(unresolved);
               else return_context->set_text(unresolved);                           //200216
               return return_context;                                               //151025
         } else
         return 0;
      }

      chr0 = std::wistream::get();
      chr1 = safe_peek();






      bool chr1_is_white_space = isspace(chr1);

      switch(chr0)
      {
         case L'#' :                                                          //160222
            // Actually In the YAML spec # should be followed by a space
            // but it is too eason to forget this
         {  // The rest of the line is comment
               std::wstring remainder_comment;                                   //200619
               std::getline((*this)/*strm*/,remainder_comment);                  //200619
               current_context->provide_comment().append                         //200619
                  (new CORN::Item_wstring(remainder_comment));                   //200619
               /*conceptual
               // The comment could be stored so we can rewrite the file preserving comment.
               // Would need to store row and leading space count
               start_line_strm_pos = strm.tellg();                               //151020
               // std::clog << "{end#_slsp:" << start_line_strm_pos << "}";
               leading_space_count = 0;
                  // promoted_context may be 0 if there was just a comment
               */
         } break;
         // Not sure if space after --- is considered leading spacing indentation
         // I don't think so

         case '!':
         {
            std::string type_tag; read_type_tag(type_tag);
            return_context = //collection/*context_construct*/
               context_map
                  // I think collection because collection will know the kinds of things therein
                  ->render_construct(type_tag);
            // NYI  the return_context should get the current_context's comments etc...
            current_context = return_context;

            /* Warning may need to set, but trying to move local
            context_map      = dynamic_cast<structural::Map      *>(collection);   //200619
            context_sequence = dynamic_cast<structural::Sequence *>(collection);
            */


         } break;

         case L':' :
         {
            bool chr1_is_white_space = isspace(chr1);
            // Must be followed by a space otherwise it is simply data
            if (chr1_is_white_space)
            {
               while ((chr1 == L' ') || (chr1 == '\r')) //eat any spaces
                  {  chr0 = std::wistream::get();
                     chr1 = safe_peek();
                  }

               if ((chr1 == '\n'))
               {
                  chr0 = std::wistream::get();
                  file_line += 1;
                  indentation.next_level_expected = true;
               } else
                  indentation.next_level_expected = false;



               //200623 std::string *type_tag = read_type_tag(strm,chr0,chr1);
               // Any type_tag could actually be read with following parse,
               // but reading the tag to instanciate the construct
               // might give more clarity.
               // Encountered so far is a key
               structural::Construct *key = current_context;
               key->set_text(unresolved); unresolved.clear();

std::string key_debug;
key->append_to(key_debug);
std::clog << "#:" << file_line << ' ' << key_debug << std::endl;

               if (!context_map)
                  return_context = context_map                                 //200621
                     = promote_to_map(*current_context);                         //200621
               structural::Pair_key_value *key_value_pair =
                  context_map->provide_key_value_pair(key,type_tag.c_str());             //190606
               // The value might be preestablished or yet to be determined
               // inwhich case if may be arbitrary
               key = 0;




               // At this point KV_pair either existed or was instanciated
               {
                  // Do nothing with popped element because it was the key
                  // and it has been given to instanciated key_value_pair.
                  {


                     structural::Construct *value_predetermined
                        = dynamic_cast<structural::Construct *>
                        (key_value_pair->get_value());
                     structural::Construct *value_to_parse = value_predetermined;;
                     //structural::Collection *value_is_collection = dynamic_cast<structural::Collection *>
                     //   (value_predetermined);
                     structural::Map *value_to_parse_as_map = dynamic_cast<structural::Map *>(value_to_parse);
                     structural::Construct *value_identified = parse_construct
                        (value_to_parse_as_map ? 0 : value_to_parse
                           // I am not sure if the context map is relevent
                           // at this point, we have already established
                           // the key value pair.
                        ,dynamic_cast<structural::Map *>(value_to_parse)  // 0 indicates not predefined
                        // ,indentation_level_expected+1
                        );

                     if (value_identified && !value_predetermined)               //200620
                        key_value_pair->take_value(value_identified);            //200620
                     //std::clog << "{return_slsp: " << start_line_strm_pos << "}";
                     //200630  leading_space_count = 0;
                     //200630 should only be cleared on new line leading_spaces_encountered = 0;
                  }
               }
               // indentation_level_spaces[indentation_level_expected--];           //200630
//               indentation.unindent();                                             //200630
               //200703  possibly_in_indentation = true;                                   //200630
               chr0 = 0;
         } // is spaced
      } break;


      case L'{' :
      {   // When we are in a resolved key value pair we would already know
               // that the value is a mapping
               // an arbitrary key value pair will not have known that
               // the value is a mapping
               if (!context_map)
               {  return_context = context_map = promote_to_map(*current_context); //200621
               }
               structural::Pair_key_value *KV_encountered = 0;
               do
               {  KV_encountered = expect_pair_key_value(*context_map);
                  // the returned item will already be added to the map
               } while (KV_encountered);
               //std::clog << "{mapping_return_slsp: " << start_line_strm_pos << "}";
               return return_context; //return promoted_context ? promoted_context: current_context;
      } break;
      case L'[' :
      {
               // When we are in a resolved key value pair we would already know
               // that the value is a sequence
               // an arbitrary key value pair will not have known that
               // the value is a sequence
               if (!context_sequence)
                  return_context = /*promoted_context = */context_sequence
                     = promote_to_sequence(*current_context);
               structural::Construct *item_encountered = 0 ;
               do
               {  item_encountered = expect_item(*context_sequence);
               } while (item_encountered);
                  //std::clog << "{seq_return_slsp: " << start_line_strm_pos << "}";
      } break;
      case L',' :
      {
               // At this point we are at the end of an item in a collection.
               while ((chr1 == L' ' || chr1 == L'\t') ) //eat any spaces or tabs
               {  chr0 = std::wistream::get();
                  chr1 = safe_peek();                                                     //200624
               }
               return return_context;
               ///* optional type tag will be processed by parse_construct
                  // The type_tag is optional .
      } break;
      case L']' :
      {  // end of sequence
               return 0 ; //ERROR_none;                                                //150904
      } break;
      case '}' :
      {
               // end of map
               return 0;
      } break;
            /*NYI  also note that YAML directives occur at the top of the document
            std::wstring potential_tag;
            if (char_at_pos == L'%')
            {  char_at_pos++;
               std::wstring directive;
               for (char_at_pos = text[pos]
                  ; char_at_pos && !isspace(char_at_pos)
                  ; pos++) // Eat white space (space or tab)
                  directive += char_at_pos;
               directives.add_wstring(directive);
            }
            */
/*
            case '\r' :
               //200630 leading_space_count = 0;
               indentation.leading_spaces_encountered = 0;
            break; //eat carridge return
            case '\n':
               //200630 leading_space_count = 0;
               //file_line ++;
               indentation.leading_spaces_encountered = 0;
               //debug start_line_strm_pos = tellg();                                    //151026
            break; //eat newline
*/
         default:
            if (chr0)
            {
                  unresolved += chr0;
                  //200705 read_leading_spaces now handles empty lines  line_is_empty = false;                                            //200624
            }
         break;
      } // switch chr0

   } while (good());
   return return_context;
}
//______________________________________________________________________________
bool Stream::read_remainder_of_line_to_comment
(std::wistream &strm
,modifiable_ Construct &current_parsing_element)
{
   CORN::Container &comments = current_parsing_element.provide_comment();
   CORN::Item_wstring *comment_line = new CORN::Item_wstring(L"");
   // multiline comment
   std::getline(strm,*comment_line);
   CORN::strip_wstring(*comment_line,CORN::Both,L' ');
   comments.take(comment_line);
   //debug start_line_strm_pos = strm.tellg();
   //std::clog << "{end#_slsp: " << start_line_strm_pos << "}";
   return true;
}
//_read_remainder_of_line_to_comment________________________________2020-02-18_/
nat8 Stream::get(Collection &document)                             modification_
{  return parse_document_known(document);
}
//______________________________________________________________________________
Parse_error_code Stream::parse_document_known
(Collection &document)
{
   return parse_to_start_of_document()
      ? parse_document(document)
      : ERROR_document_start_not_found;
}
//______________________________________________________________________________
bool Stream::parse_to_start_of_document()
{
   //std::wstreampos start_file_pos= 0;
   bool at_start = false;
   // My need to read BOM
   std::wstring text;
   const wchar_t *text_remainder = 0;
   while (good() && !at_start)
   {

      std::wstreampos currpos  = tellg();
      std::getline(*this,text);
      text_remainder = text.c_str();

      if (text.find(L"---") == 0)
         // Presuming that document separator is always at the beginning of the line.
         // It is unclear from the YAML documentation
      {
         currpos += 3;
         //curr_document->know_anchors(&anchors);
         // The document starts immediately after ---,
         // This is because there may be a block literal

         text_remainder = text.c_str() + 3;
         for (wchar_t next_char =(*text_remainder)
             ;next_char && (next_char == L'.')
             ;next_char = (*text_remainder++))
         {
             currpos += 1;
         }
         at_start = true;
         seekg(currpos);
      } else
      {
         seekg(currpos);
         at_start=true;
      }
   }
   return at_start;
}
//______________________________________________________________________________
#ifdef obsolete
nat32 Stream::read_value
(
//200624 std::wistream &strm,
std::wstring &value, std::wstring &comment)
{  nat32 consumed = 0;
   wchar_t //200624 chr1 = peek();
   chr1 = safe_peek();                                                     //200624
   while (good() && (chr1 != '\n'))
   {
      wchar_t chr0 = std::wistream::get();
      //200624chr1 = peek();
      chr1 = safe_peek();                                                     //200624
      switch (chr0)
      {
         case L',' : std::wistream::get(); consumed+=2; return consumed; // break; // eat the , and there will always be a space after ,
         case L'#' : std::getline((*this)/*strm*/,comment); consumed += comment.length(); return consumed; //break;
         case '}' : return consumed;  //break; // dont eat the } it is needed for further parsing by the stream
         default :
         {  value += chr0;
            consumed += 1;
         } break;
      }
   }
   return consumed;
}
//______________________________________________________________________________
#endif
bool /*std::string **/
Stream::read_type_tag
(std::string &type_tag                                                           //200623
//,wchar_t &chr0
//,wchar_t &chr1
)
{
   wchar_t chr0 = 0;
   wchar_t chr1 = safe_peek();
   bool tag_encountered = (chr1 == '!');
   //200630    std::string *type_tag = 0;
   //200623 if (chr1 == '!')
   if (tag_encountered)
   {
      // type_tag = new std::string;
      //(*type_tag) += chr0;
      while (!std::isspace(chr1))
      {  chr0 = std::wistream::get();
         chr1 = safe_peek();                                                     //200624
         (type_tag) += chr0;
      }
      // Tags will be delimited by
      while ((chr1 == L' ' || chr1 == L'\n') )  // eat any spaces
      {  chr0 = std::wistream::get();
         chr1 = safe_peek();                                                     //200624
      }
   }
   return tag_encountered;
}
//_get_type_tag_____________________________________________________2020-06-20_/
bool Stream::read_end_document_marker()
{
   bool marker_encountered = true;
   std::streampos strm_pos = tellg();
   wchar_t dot = '.';
   //wchar_t next = L'.';
   for (int d = 1; (d <= 3) && (dot ==  L'.'); d++)
   {
      dot = std::wistream::get();
      //wchar_t next = safe_peek();

   }
   if (dot !=  L'.')
   {  marker_encountered = false;
      seekg(strm_pos);
   }
   return marker_encountered;
}


structural::Construct *promote_scalar(const structural::Construct *unresolved)
{

   /*
      could split this into YAML specific and structural basic types
      YAML specific would date time radix

   */
   structural::Construct *promoted = 0;
   // check bool first because it is the most discrimatable
   structural::Scalar_bool_clad *potential_bool = new structural::Scalar_bool_clad;
   std::wstring value_buffer;
   const wchar_t *value_wstr = unresolved->append_to(value_buffer);
   if (potential_bool->set_text(value_wstr))
       promoted = potential_bool;
   else
   {
      /* NYI
         should check if the format of the arbitary text
         is reognizable number
      else
      */
      /* NYI radix integers
      */
      /* NYI dates and times

      */
         // At the very least it can be a string
         promoted = new structural::Scalar_string_clad(value_buffer);
   }
   if (promoted) delete unresolved;
   return promoted;
}
//_promote_scalar___________________________________________________2020-06-20_/
structural::Map *Stream::promote_to_map
(structural::Construct &construct)                                    rendition_
{
   structural::Map_clad *promoted_map = new structural::Map_clad;
   // NYI  copy comments anchors etc to promoted_map
   return promoted_map;
}
//_promote_to_map___________________________________________________2020-06-21_/
structural::Sequence *Stream::promote_to_sequence
(structural::Construct &construct)                                    rendition_
{
   structural::Sequence_clad *promoted_seq = new structural::Sequence_clad;
   return promoted_seq;
}
//_promote_to_sequence______________________________________________2020-06-21_/
Pair_key_value *Stream::expect_pair_key_value
(
//200624 std::wistream               &strm,
structural::Map             &map
//,nat16                        indentation// = 0
//,nat16                        leading_space_count
)
{
   Pair_key_value *key_value = 0;
   Construct *key_identified //200620 context_promoted                     //151025
      // actually a key value pair
         = parse_construct
         (&map
         ,0
         //,0
         //,0,0
         );
   // Key may be 0 if end of list encountered }
   if (key_identified)
   {
      Pair_key_value *key_value = map.provide_key_value_pair                     //200619
         (key_identified
         ,"!!str"  // if the value is not redefined, presume it is a string
         // type will have been identified in parse is explicit
         );
      // Since provide idiom will always add the KV pair
      // if there isnt already a matching pair, we are
      // building the listed items as needed.
      structural::Construct *value_identified                                    //200620_151025
         // actually a key value pair
            = parse_construct
            (key_value->get_value()
            ,&map
               //may be 0 indicating the value hasn't been resolved yet
            //,0
            //200630 ,0
            );
      key_value->take_value(value_identified);
      // note that value identified may already be
      // the preestablished value in this case the
      // value identified be ignored.
      // In accordance with the take idiom,
      // If the value_indentified isnt what the
      // KV pair may already have, the value will be
      // deleted and replaced.
   }
   return  key_value;
}
//_expect_pair_key_value____________________________________________2020-06-22_/
Construct     *Stream::expect_item          // inline_mode
(structural::Sequence        &sequence)
{
   return parse_construct(&sequence,0);
}
//_expect_item______________________________________________________2020-06-22_/

/*unused (conceptual)
CORN::Container::Format item_format_unquoted_keys =
{  "---"  //initializer
,  0
,  0     //indentor
,  {  // Items in an object are key value pairs which are not themselves bracketed
     0 // initializer;
   ,"," //separator
   ,  0 // finalizer
   ,' ' // use if objects are not pretty printed
   ,{ //Key
      true  // distinquished;    // If the is specially set assied (I.e. JSON, INI files)
    , true  // case_sensitive;   // used only when reading
    , ": "  // separator;
    , { // Quotation
        0  // "\"" // initializer;   // I.e.  keys are not currently double quoted
      , 0  // "\"" // finalizer;     // I.e.  keys are not double quoted
      } // quotation

    , { // Comment
            0  // initializer JSON format provides no comments
         ,  0  // finalizer
         ,  "#"  // endline
      } // comment
    } // key
   } // item
};
*/
//______________________________________________________________________________
wchar_t Stream::safe_peek()
{
   wchar_t peaked = 0;
   nat32 last_pos = 0;
   last_pos = tellg();
   if (good()) peaked = peek();
   seekg(last_pos);
   return peaked;
}
//______________________________________________________________________________
}//_namespace YAML_____________________________________________________________/

