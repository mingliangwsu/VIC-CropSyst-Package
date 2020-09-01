#include "corn/format/html/structure.h"
//#include <corn/format/html/html.h>

#define M_HTML "HTML"

namespace HTML
{
//_____________________________________________________________________________
Document::Document
(std::ostream &doc_
,bool auto_HTML_XHTML_
)
:Element(M_HTML,doc_)
,auto_HTML_XHTML(auto_HTML_XHTML_)
{  if (auto_HTML_XHTML) start();
}
//_____________________________________________________________________________/
Document::~Document()
{  if (auto_HTML_XHTML) end();
      //document << HTML_XHTML << endl;
}
//_____________________________________________________________________________/
}//_namespace_HTML_____________________________________________________________/