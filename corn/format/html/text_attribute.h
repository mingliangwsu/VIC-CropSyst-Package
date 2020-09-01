#ifndef text_attributeH
#define text_attributeH
#include "corn/format/html/element.h"
#define M_FONT "FONT"
namespace HTML
{
//______________________________________________________________________________
class Font
: public Element
, public Attribute_options
{
   Int_option      size; // + or -
   Uint_option     point_size; // ???
   Color_option    color;
   String_option   face;
   //NYI weight ???
public: // structors
   inline Font
   (Color color_,Element *parent_element_)
   :Element(M_FONT,parent_element)
   ,size("SIZE")          // default
   ,point_size("POINT-SIZE")    // default
   ,color()
   ,face("FACE")         // default
   // ,weight(???)
   {  color.set(color_); }
   inline Font
   (const std::string &face_, Element *parent_element_)
   :Element(M_FONT,parent_element_)
   ,size("SIZE")
   ,point_size("POINT-SIZE")
   ,color()
   ,face("FACE")
   // ,weight(???)
   {  face.set(face_); }
   inline virtual ~Font() { flush();}
public: // overrides
   virtual void options();
};
//_____________________________________________________________________________/
}//_namespace_HTML_____________________________________________________________/
#endif

