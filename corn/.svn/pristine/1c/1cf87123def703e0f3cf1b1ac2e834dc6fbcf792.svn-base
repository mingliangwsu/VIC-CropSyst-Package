#ifndef CORN_HTML_ANCHOR_H
#define CORN_HTML_ANCHOR_H

#include <corn/format/html/html.h>

#define M_A "A"

namespace HTML
{

extern char *special_target_label_table[];
//______________________________________________________________________________
class Anchor
: public Element
, public Attribute_options
, public Data_options
, public Focus_event_options
{
protected: // options
   URI_option     href;
   String_option  name;
/*NYI
   String_option hreflang;
   String_option type;  // content type
   String_option  charset; // link encoding
*/
   String_option  target;  // target frame
/* NYI
   String_option methods;
   String_option rel;
   String_option rev;

*/
   URI_option  urn; // permanet destination
/* NYI
   accesskey; // hotkey
   shape; // Map shape
   coords; // Shape coordinates
   tabindex; // Tab order
*/

public:
   enum Special_targets {blank,self,parent,top};
public:
   inline Anchor(Body *i_parent_element)
      :Element(M_A,i_parent_element)
      ,Attribute_options()
      ,Data_options()
      ,Focus_event_options()
      ,href("HREF")
      ,name("NAME")
      ,target("TARGET")
      ,urn("URN")
      {}
   inline void set_href(const std::string &href_)             {href.set(href_);}
   inline void set_name(const std::string &name_)             {name.set(name_);}
   inline void set_target(const std::string &target_)     {target.set(target_);}
   inline void set_target_special(Special_targets target_)
                             { target.set(special_target_label_table[target_]);}
   inline void set_urn(const std::string &urn_)                 {urn.set(urn_);}
   inline virtual void options()
   {  Element::options();
      Attribute_options::write(document);
      Attribute_options::write(document);
      Attribute_options::write(document);
      href.write(document);
      name.write(document);
      target.write(document);
      urn.write(document);
   }
};
//_Anchor______________________________________________________________________/
}//_namespace_HTML_____________________________________________________________/
#endif

