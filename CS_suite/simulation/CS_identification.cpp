#include "CS_identification.h"
#include "corn/string/strconv.hpp"
//______________________________________________________________________________
namespace CS
{
//______________________________________________________________________________
Identification::Identification(int32 ID_)
: number(ID_)
, code()
{  CORN::append_nat32_to_string(number,code);
}
//__________________________________________________________________2015-10-26_/
Identification::Identification(const std::string &ID_)
: number()
, code(ID_)
{  number = CORN::cstr_to_int32(ID_.c_str(),10);
}
//__________________________________________________________________2015-10-26_/
bool Identification::set_code(const std::string &ID_)
{  code = ID_;
   number = CORN::cstr_to_int32(ID_.c_str(),10);
   return true;
}
//_set_code_________________________________________________________2015-10-26_/
bool Identification::set_code(const std::wstring &ID_)
{
   std::string ID_string;
   CORN::wstring_to_string(ID_,ID_string);
   return set_code(ID_string);
}
//_set_code_wstring_________________________________________________2019-03-17_/

bool Identification::set_number(int32 ID_)
{  number =ID_;
   code.clear(); CORN::append_int32_to_string(number,code);
   return true;
}
//_set_number_______________________________________________________2015-10-26_/
} // namespace CS

