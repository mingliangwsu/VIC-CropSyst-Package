#ifndef structural_streamH
#define structural_streamH
#include <fstream>
#include <string>
#include "corn/const.h"
#include "corn/format/structural.h"
#include "corn/OS/directory_entry_name_concrete.h"
namespace CORN
{
//______________________________________________________________________________
enum Parse_error_code
{ ERROR_none
, ERROR_unknown
};
//______________________________________________________________________________
struct Error
: public extends_ Item
{
   nat8                     code;
   structural::Construct   *in_construct;
   nat32                    line;
};
//______________________________________________________________________________
interface_ Structural_istream
{
   inline virtual nat8 get_document(structural::Collection /*200621 Construct*/ &document) modification_
      { return parse_document(document); }
   virtual nat8 parse_document
      (structural::Collection /*200620 Construct*/     &document)                   modification_ = 0;
      /// \returns error code or 0 if no error
};
//_Structural_istream_______________________________________________2017-01-28_/
template <typename  ifstream_T,typename  String_T >
class Structural_ifstream_generic
: public extends_ ifstream_T
, public implements_ Structural_istream
{
 public:
   Structural_ifstream_generic
      (const char *filename_)
      : ifstream_T(filename_)
      {
        // this->open(filename_.c_str());
      }
};
//_Structural_ifstream_generic______________________________________2017-01-28_/
class Structural_ifstream_ASCII
: public extends_ Structural_ifstream_generic<std::ifstream,std::string>
{
 public:
   inline Structural_ifstream_ASCII(const CORN::OS::File_name &filename)
      : Structural_ifstream_generic<std::ifstream,std::string >
      (filename.c_str())                                                         //190606
      {}
};
//_Structural_ifstream_ASCII________________________________________2017-01-28_/
class Structural_ifstream_Unicode
: public extends_ Structural_ifstream_generic<std::wifstream ,std::wstring>
{
 public:
   inline Structural_ifstream_Unicode(const CORN::OS::File_name &filename)
      : Structural_ifstream_generic<std::wifstream,std::wstring>
      (filename.c_str())
      {}
};
//_Structural_ifstream_Unicode______________________________________2017-01-28_/
}//_namespace_CORN_____________________________________________________________/
#endif

