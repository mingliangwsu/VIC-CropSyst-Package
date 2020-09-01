#ifndef CORN_HTML_DOC_H
#define CORN_HTML_DOC_H

#include "corn/format/html/element.h"
//#include <corn/std/std_iostream.h>

namespace HTML
{
//_____________________________________________________________________________/
class Document
: public Element
{
   bool     auto_HTML_XHTML;
public:
   Document
   (std::ostream &i_doc
   , bool auto_HTML_XHTML = true
   );
   // This constructor is used for creating streaming HTML output
   // as in CGI script responses
   // True will output <HTML> and </HTML>
   ~Document();
private: // Common attributes
public: // HTML markup
};
//_Document____________________________________________________________________/
class Head
: public Element
{
   // Header elements
   /* NYI
   class HTML_title
   {
   };
   class HTML_isindex
   {
   };
   class HTML_meta
   {
   };
   class HTML_base
   {
   };
   class HTML_link
   {
   };
   class HTML_style
   {
   };
   class HTML_nextid
   {
   };
   */
public: // constructors
   Head(Document &doc);
   // default header
   ~Head();
};
//_Head________________________________________________________________________/
// The element BODY is document structure element
// It is found in  body.h
class Comment
: public Element
{
public:
   Comment(const std::string &text);
};
//_Comment_____________________________________________________________________/
}//_namespace_HTML_____________________________________________________________/
#endif

