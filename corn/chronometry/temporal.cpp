#include "corn/chronometry/temporal.h"
#include "corn/string/strconv.hpp"
namespace CORN
{
//______________________________________________________________________________
int8 Temporal::temporal_compare(const Temporal & second_time)              const
{  float64 time64 = get_datetime64();
   float64 second_time64 = second_time.get_datetime64();
   return (int8)(time64 < second_time64)
      ? (int8)-1
      : (int8)((time64 > second_time64)
         ? 1
         : 0);
}
//_2003-01-08_________________________________________________temporal_compare_/
std::string Temporal::as_string()                                          const
{  std::string buffer;
   append_to_string(buffer _ISO_FORMAT_DATE_TIME);
   return buffer;
}
//_2014-06-14________________________________________________________as_string_/
const std::wstring &Temporal::append_to_wstring(std::wstring &target)      const
{  CORN::string_to_wstring(as_string(),target);
   return target;
}
//_2014-09-19________________________________________________append_to_wstring_/
}//_namespace_CORN__________________________________________________2016-06-06_/
