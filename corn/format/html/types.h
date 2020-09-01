#ifndef CORN_HTML_TYPES_H
#define CORN_HTML_TYPES_H

#include "corn/std/std_iostream.h"

#include "corn/enum.h"
#include "corn/string/strconv.h"
#ifndef primitiveH
#  include "corn/primitive.h"
#endif

namespace HTML
{
enum Units  { pixels,percent };
extern char *units_label_table[];
//______________________________________________________________________________
class Option_base
{
protected:
   bool specified;
public:
   inline Option_base()
   : specified(false)
   {}
   inline void set_specified() { specified = true;}
};
//_Option_base_________________________________________________________________/
class Option
: public Option_base
{
   std::string label;
public:
   inline Option(const std::string &label_)
   : label(label_)
   {}
   inline virtual void write(std::ostream &doc)
   {
      doc << ' ' << label.c_str() << '=';
   }
};
//_Option______________________________________________________________________
class Bool_option
: public Option_base
{
   bool        enabled;
   std::string label;
public:
   inline Bool_option(const std::string &label_)
   : label(label_)
   , enabled(false)
   {}
   inline virtual void write(std::ostream &doc)
   {
      if (enabled)
         doc << ' ' << label.c_str();
   }
   inline void enable_() { enabled = true; }
   inline void disable_(){ enabled = false; }
};
//_Bool_option_________________________________________________________________/
class Uint_option
: public Option
{
   nat32   value;
public:
   inline Uint_option(const std::string &label_)
   : Option(label_)
   , value(0)
   {}
   inline virtual void write(std::ostream &doc)
   {
      if (value || specified)
      {  // Only write option if setup
         Option::write(doc);
         doc << value;
      }
   }
   inline void set(uint32 value_)
   { value = value_;
      set_specified();
   }
};
//_Uint_option_________________________________________________________________/
class Int_option
: public Option
{
   int32   value;
public:
   inline Int_option(const std::string &label_)
   : Option(label_)
   , value(0)
   {}
   inline virtual void write(std::ostream &doc)
   {
      if (value || specified)
      {  // Only write option if setup
         Option::write(doc);
         doc << value;
      }
   }
   inline void set(int32 value_)
   { value = value_;
      set_specified();
   }
};
//_Int_option__________________________________________________________________/
class String_option
: public Option
{
protected:
   std::string value;
public:
   inline String_option(const std::string &label_)
   : Option(label_)
   , value("")
   {}

   inline virtual void write(std::ostream &doc)
   {
      if (value.length() || specified)
      {
         Option::write(doc);
         doc << '\"' << value.c_str() << '\"';
      }
   }
   inline void set(const std::string &value_)
   { value.assign(value_);
      set_specified();
   }
};
//_String_option_______________________________________________________________/
class URI_option
: public String_option
{
public:
   inline URI_option(const std::string &label_)
   :String_option(label_)
   {}
};
//_URI_option__________________________________________________________________/
class Color
{
public:
   std::string name; // color name labeol
   nat32      RGB_code; // will be 0 if color name used
public:
   inline Color
      (const std::string &_name)
      :name(_name)
      ,RGB_code(0)
      {}
   inline Color
      (uint32      _RGB_code)
      :name("")
      ,RGB_code(_RGB_code)
   {}
};
//_Color_______________________________________________________________________/
class Color_option
: public String_option
{
   nat32      RGB_code; // will be 0 if color name used
public:
   inline Color_option()
      : String_option("COLOR")
      , RGB_code(0)
      {}
   inline Color_option(const std::string &label_)
      : String_option(label_)
      , RGB_code(0)
      {}
   inline virtual void write(std::ostream &doc)
   {
      if (value.length())
         String_option::write(doc);
      else
      if (RGB_code || specified)
      {  std::string RGB_hex("#");
         char RGB_str[10];
         CORN::nat32_to_cstr(RGB_code,RGB_str,16);
         RGB_hex.append(RGB_str);
         value.assign(RGB_str);
      }
   }
   inline void set(const Color &color_)
   {
      RGB_code = color_.RGB_code;
      value = color_.name;
      set_specified();
   }
/* only used color_
   void set(const char* color_)
   {
     RGB_code = 0;
     value = color_)
     set_specified();
   }
*/
};
//_Color_option_______________________________________________________________/
class Enum_option
: public Option
, public Enum_abstract //200323 Labeled_enum   // 020321
{
   int32 value;
public:
   inline Enum_option(const std::string &label_,const char **enum_value_labels_)
      : Option(label_)
      , Enum_abstract(enum_value_labels_)
      //, Labeled_enum(enum_value_labels_)
      ,value(0)
      {}
   inline virtual void write(std::ostream &doc)
   {  if (value || specified)
      {  Option::write(doc);
         //200323 doc << get_label();
         stream_label(doc);                                                      //200323
      }
   }
   inline virtual int32 get_int32()                        const {return value;}
   inline virtual int32 set_int32(int32 value_)
   {  value = value_;
      set_specified();
      return value;
   }
};
//_Enum_option_________________________________________________________________/
class Arbitrary
{
   // This class defines anything that may occur in an HTML document
   // Either HTML elements or text.
protected:
   std::ostream &document;
   virtual void flush() = 0;
public:
   inline Arbitrary(std::ostream &i_doc)
      :document(i_doc)
      {}
   void text_out(const std::string &out_text);
      // Any arbitrary text
   void float32_out(float32 value);
      // output a float
   inline std::ostream &get_document_stream()
      {  return document; }
protected:
   void simple(const char *element_marker);
      // Used for simple Simple elements I.e.  <marker> as in <BR>
   void color(char *color_appl,uint32 color_code);
public:
   // Quick/simple elements implemented as functions
   // That can occur in just about any part of an HTML document
   // Text layout
   inline void BR()      { simple("BR"); }
   inline void P()       { simple("P"); }
   inline void XP()      { simple("/P"); }
   // Special text markup
   void A_HREF(const std::string &url, const std::string &target);
   void XA();
   // not yet classified
   void FONT_COLOR(uint32 color_code);
   void XFONT();
};
//_Arbitrary___________________________________________________________________/
}//_namespace_HTML______________________________________________________________
#endif

