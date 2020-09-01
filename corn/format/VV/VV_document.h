#ifndef VV_documentH
#define VV_documentH
#include "corn/format/structural.h"
namespace CORN
{
//______________________________________________________________________________
// KV is Key-Value
class KV_Document
: public extends_ structural::Map_clad
{
   // NYI
};
//_KV_Document______________________________________________________2017-01-28_/
namespace VV
{
//_____________________________________________________________________________/
/*
A VV file Document is always a Map (of sections) with one or pair_key_map

Note, it could be considered that the filename/URL
of the document is a key for the document.
Furthermore, the file system structure itself could be considered a map
(for example an ontology directory structure).
Although from the document's perspective, the context within
a file system is irrelevent/unknown.

For this reason the Document, does not know its filename

*/

class Document
: public extends_ structural::Map_clad // list of Sections
{
 public:
   typedef structural::Map_clad Section;

   //___________________________________________________________________________
 protected:
   //Section *current_section;
   Section *current_section;
 public:
   inline Document()
      : structural::Map_clad()
      // Currently documents don't have specifier but essentially
      {}
 public:
   virtual bool expect_structure()                               expectation_=0;
      // Derived classes must implement this method
      // to associate the structure of the
   /* Conceptual
      Provide a mechanism to define a structure using
      a template file with ontology definitions.
      A where onology definitions are bound to data values.


   */
 public:
//   structural::Mapping /*Section*/
   Section &expect_section(const char          *section_name)      expectation_;
   Section &expect_section(const wchar_t       *section_name)      expectation_;
   Section &expect_section(const std::string   &section_name)      expectation_;
   Section &expect_section(const std::wstring  &section_name)      expectation_;
};
//_Document_________________________________________________________2017-01-28_/
}}//_namespace_CORN:VV__________________________________________________________/

#endif
