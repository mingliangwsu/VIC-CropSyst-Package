#ifndef imgH
#define imgH

#include <corn/format/html/element.h>
namespace HTML
{
//______________________________________________________________________________
class Img
: public Element
, public Attribute_options
, public Data_options
, public Dimension_options
, public Align_option
{
   String_option   src;
   String_option   alt;
   /*NYI
   CORN_string    longdesc;
   uint8          border;
   uint16         hspace;
   uint16         vspace;
   bool           ismap
   CORN_string    usemap;
   bool           controls ?
   bool           dynsrc   ?
   CORN_string    longdesc ?
   ??             loop;
   CORN_string    lowsrc; ???
   CORN_string    name;
   CORN_string    start;???
   CORN_string    VRML; ???
   bool           suppress;
   */
 public:
    #define M_IMG "IMG"
   inline Img(Element            *parent_body
           ,const std::string    &src_
           ,const std::string    &alt_
           ,Align_option::Align   align_ = Align_option::LEFT)
   :Element(M_IMG,parent_body)
   ,Attribute_options()
   ,Data_options()
   ,Dimension_options()
   ,Align_option()
   ,src("SRC")
   ,alt("ALT")
   {  src.set(src_);
      alt.set(alt_);
      Align_option::set_int32(align_);
   }
public: // overrides
   virtual void options();
   inline virtual ~Img() { flush();}
};
//_Img_________________________________________________________________________/
}//_namespace_HTML_____________________________________________________________/
#endif

