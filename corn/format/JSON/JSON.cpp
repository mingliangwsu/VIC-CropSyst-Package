#include "JSON.h"
#include <sstream>
#ifdef __BCPLUSPLUS__
#   include <ctype.h>
#endif

namespace CORN
{
namespace JSON
{
//______________________________________________________________________________
std::istream &Object::read_stream(std::istream &data_stream)
{
   // read upto and eat {,
   // anything up to the { should be white space.
   // Currently ignores everything else, but probably should give and error
   char left_curley_bracket;
   do
   {  left_curley_bracket = data_stream.get();
   } while (data_stream.good() && !data_stream.eof() && left_curley_bracket!= '{');//130217
   char next_char;
   bool at_end_of_object = false;
   // At this point we are reading "key" : value pairs
   // separated by , and terminated by }
   // where value is either a scalar in "" or a  { structure
   // Note that we are not reading array items here, that is handled by
   // the derived Array class.
   do
   {
      next_char = data_stream.peek();
      switch (next_char)
      {  case '\"' :
         {  // This member is a pair
            std::string key;
            read_key(data_stream,key);
            read_colon(data_stream);
            read_and_set_value(data_stream,key);
         } break;
         case '}' :
         {
            data_stream.get(next_char);
            at_end_of_object = true;
         } break;
         case ',' :                                                              //121225
         {  data_stream.get(next_char);
            at_end_of_object = false;
         } break;
         default :
         {  // Eat the next character it should be whitespace.
            data_stream.get(next_char);
         } break;;

      } // switch
   } while (data_stream.good() && !data_stream.eof() && !at_end_of_object);      //130217
   return data_stream;
}
//_read_stream_________________________________________________________________/
CORN::Container::Format item_format = //was JSON_object_format =  // inline
{  "{"  //initializer
,  "}"  //finalizer
,  ' '     //indentor
,  {  // Items in an object are key value pairs which are not themselves bracketed
    "" // initializer;
   ,"," //separator
   ,"" // finalizer
   ,' ' // indentor by default JSON objects are not pretty printed
   ,{ //Key
      true  // distinquished;    // If the is specially set assied (I.e. JSON, INI files)
    , true  // case_sensitive;   // used only when reading
    , ":"  // separator;
    , { // Quotation
        "\"" // initializer;   // I.e.  JSON keys are double quoted
      , "\"" // finalizer;     // I.e.  JSON keys are double quoted
      } // quotation
   /*NYI
    , { // Comment
            ""  // initializer JSON format provides no comments
         ,  "" // finalizer
         ,  ""  // endline
      } // comment
*/
    } // key
   } // item
};
//_item_format_________________________________________________________________/
std::ostream &Object::write_stream(std::ostream &data_stream)
{  sections.write_with_format(data_stream,item_format,0);
   return data_stream;
}
//_write_stream________________________________________________________________/
bool Object::read_string(const std::string &data)
{  std::stringstream data_stream(data);
   read_stream(data_stream);
   return true;
}
//_read_string_________________________________________________________________/
bool Object::write_string(std::string &data)
{  std::stringstream data_stream(data);
   write_stream(data_stream);
   return true;
}
//_write_string_____________________________________________________2013-02-17_/
const std::string &Object::read_key(std::istream &data_stream,std::string &buffer) const
{  return read_quoted_string(data_stream,buffer);
}
//_read_key_________________________________________________________2012-08-31_/
bool Object::read_colon(std::istream &data_stream)                         const
{  bool colon_reached = false;
   do
   {  char next_char;
      data_stream.get(next_char);
      if (next_char == ':')
         colon_reached = true;
      /* NYI
         if the next_char is not white space, should signel error.
      */
   } while (!colon_reached && !data_stream.eof());
   return colon_reached;
}
//_read_colon_______________________________________________________2012-08-31_/
const std::string &Object::read_quoted_string
(std::istream &data_stream,std::string &buffer)                            const
{  //  read token "
   char next_char_str[2]; next_char_str[0]= 0; next_char_str[1] = 0;
   char next_char;
   do
   {  data_stream.get(next_char);
      // any other characters at this point should be white space
   } while (!data_stream.eof() && next_char!= '\"');
   // read and append until "
   bool at_closing_quote = false;
   do
   {  data_stream.get(next_char);
      switch (next_char)
      {  case  '\\' : // handle excape character
         {
            data_stream.get(next_char);
            switch (next_char)
            {
               case 'a' : next_char = '\a'; break;
               case 'b' : next_char = '\b'; break;
               case 'f' : next_char = '\f'; break;
               case 'n' : next_char = '\n'; break;
               case 'r' : next_char = '\r'; break;
               case 't' : next_char = '\t'; break;
               case '\\': next_char = '\\'; break;
               case 'u' :
               {  char hex[5]; hex[0] = 0;
                  data_stream.get(hex[0]);
                  data_stream.get(hex[1]);
                  data_stream.get(hex[2]);
                  data_stream.get(hex[3]);
                  next_char = strtoul(hex,0,16);
                  //warning currently assuming the JSON file is ASCII.

               }
               break;
            }
            // backslash and quote character should be handled ok here.
            next_char_str[0]= next_char;
            buffer.append(next_char_str);
         } break;
         case '\"' :
         {
            at_closing_quote = true;
         } break;
         default :
         {
            next_char_str[0]= next_char;
            buffer.append(next_char_str);
         } break;
      } //switch
   } while (!data_stream.eof() && !at_closing_quote);
   return buffer;
}
//_read_quoted_string_______________________________________________2012-08-31_/
bool Object::read_and_set_value(std::istream &data_stream,const std::string &key)
{  bool at_end_of_object;
   char next_character;
   std::string value;
   for (next_character = data_stream.peek()
       ;  (next_character != '}')
       && (next_character != ']')
       ;next_character = data_stream.peek())
   {  switch (next_character)
      {  case '\"' :
         {  read_quoted_string(data_stream,value);
            #ifdef JSON_USES_DATAREC
            if (current_section)                                              //120311
                current_section->set_entry(key.c_str(),value.c_str(),false /* ???unexpected_only*/  );
            #else
            // continue here may want to store the value in  a substructure
            // when derived clas does not consume the value
            #endif

            value.clear();
         } break;
         case '{' :   case '[' :
         {  Structure *substruct = provide_substructure(key);
            if (!substruct)
            {  if (next_character == '{')
               {  substruct = new Object(key);  // not sure if I should pass key to object
            // the value appears to be an object but at this point
            // the derived class hasn't recognized it
            // read to closing }, throw away the value
            // but should store it in a string for preserve unexpected entries
               } else //  (next_character == '[')
               {
                  substruct = new Object::Array(key);
               }
            }
            if (substruct)
            {   substruct->read_stream(data_stream);
//std::cout << key << std::endl;
//std::cout << substruct->get_key() << std::endl;
            substruct->commit();                                                 //130126
            }
         } break;
         case '}' :
         {  // if we've been reading literal number or constants true false null
            #ifdef JSON_USES_DATAREC
            if ((!value.empty()) && (value != "null") && current_section)
               current_section->set_entry(key.c_str(),value.c_str(),true /* ???unexpected_only*/  );
            value.clear();
            #else
            // continue here, may want to store the value in a sub struture
            #endif
            return true;
         } break;                                                                //121225
         case ',' :
         {
           #ifdef JSON_USES_DATAREC
           if ((!value.empty()) && (value != "null") && current_section)
               current_section->set_entry(key.c_str(),value.c_str(),false /* ???unexpected_only*/  );
            value.clear();
           #else
           // continue here provide a virtual function
           // for the derived class to know the current value
           #endif
           return false;
         } break;                                                                //121225
         case ' ' : case '\t' : case '\n' : case '\r' :
            // eat white space
            data_stream.get(next_character);
         break;
         default :
         {  data_stream.get(next_character);                                     //140806
            value += next_character;
         } break;
      } //switch
   }
   return true;
}
//_read_and_set_value_______________________________________________2012-08-31_/
/*NYI
bool Object::write_to_JSON_string(std::string &data);
{

   return true;
}
//_write_to_JSON_string_____________________________________________2012-08-31_/
*/
std::istream &Object::Array::read_stream(std::istream &data_stream)
{  for (char next_char = data_stream.peek()   // peek next non white space char
       ;next_char != '['   // we don't do anything with '[' just eat it.
       ;next_char = data_stream.peek())
       data_stream.get(next_char); // eat white space
   for (char next_char = data_stream.peek()
       ;next_char != ']'
       ;next_char = data_stream.peek())
   {  switch(next_char)
      { case '{' :
         {  Object * item = render_item();
            if (item)
            {
               #ifdef JSON_USES_DATAREC
               item->expect_structure(false);
               #endif
            } else
               item = new Object("");                                            //130217
            {

               item->read_stream(data_stream);
               if (!item->commit())
                  commit_item(item);
                  // Derived classes may immediately consume the item
                  // so we don't need to append to the default structure.
            }
         } break;
         // Don't need to do anything with , it is handled by array item parser
         default :  data_stream.get(next_char); break;
      } // switch
   }
   return data_stream;
}
//_Array::read_stream__________________________________________________________/
CORN::Container::Format JSON_array_format =   // inline
{  "["  //initializer
,  "]"  //finalizer
,  0     //indentor
,  {  // Items in an object are key value pairs which are not themselves bracketed
     0 // initializer;
   ,"," //separator
   ,  0 // finalizer
   ,0 // indentor by default JSON objects are not pretty printed
   ,{ //Key
      false  // distinquished;    // If the is specially set assied (I.e. JSON, INI files)
    , true  // case_sensitive;   // used only when reading
    , 0   // separator;
    , { // Quotation
        "\"" // initializer;   // I.e.  JSON keys are double quoted
      , "\"" // finalizer;     // I.e.  JSON keys are double quoted
      } // quotation
   /*NYI
    , { // Comment
            0  // initializer JSON format provides no comments
         ,  0  // finalizer
         ,  0  // endline
      } // comment
*/
    } // key
   } // item
};
//______________________________________________________________________________
std::ostream &Object::Array::write_stream(std::ostream &data_stream)
{
   items.write_with_format(data_stream,JSON_array_format,0);
   return data_stream;
}
//_Array::write_stream______________________________________________2013-02-17_/
Object *Object::Array::commit_item(Object *item_to_commit)
{
   // Derived classes may immediately consume the item
   // so we don't need to append to the default structure.
//   if (!item_to_commit
//       || !item_to_commit->commit())
      items.append(item_to_commit);
   return item_to_commit;
}
//_Array::commit_item__________________________________________________________.
}//_namespace_CORN_JSON________________________________________________________/
