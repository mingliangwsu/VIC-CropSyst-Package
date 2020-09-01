#include "corn/chronometry/date_time_A.h"
#include <string.h>
#include "corn/string/strconv.hpp"
#include <assert.h>
namespace CORN {
//______________________________________________________________________________
Date_time_abstract::Date_time_abstract()
{}
//______________________________________________________________________________
const std::string &Date_time_abstract::append_to_string
(std::string &target
,const Date_format *from_date_format                                             //180624
,const Time_format *from_time_format)                                      const //180624
{
   ref_date().append_to_string
         (target,from_date_format,0);
   if (from_time_format)                                                         //200707
   {
      std::string time_str;                                                      //171017
      ref_time().append_to_string(time_str,0,from_time_format);                  //171017
      if (time_str.length())                                                     //171017
      {  if (from_date_format) // we did append a date so we need delimiter      //190110
            target += "T"; // ISO format separates with T                        //180208_171017
         target += time_str;                                                     //171017
      }
   }
/*200502
   if (from_date_format)                                                         //190110
      ref_date().append_to_string(target,from_date_format,from_time_format);
   std::string time_str;                                                         //171017
   if (from_time_format)
   {  ref_time().append_to_string(time_str,0,from_time_format);                  //171017
      if (time_str.length())                                                     //171017
      {  if (from_date_format) // we did append a date so we need delimiter      //190110
            target += "T"; // ISO format separates with T                        //180208_171017
         target += time_str;                                                     //171017
      }
   }
*/
/*200502 abandoned
   if (!from_date_format && !from_time_format)
      CORN::append_float64_to_string(get_datetime64(),6,target);                 //200502
*/
   return target;
}
//_append_to_string______________________________________2014-11-20_2014-06-15_/
DOY Date_time_abstract::get_DOY()                                          const
   // extracts the day of year from the Date date
{  return ref_date().get_DOY();
}
//_get_DOY__________________________________________________________2017-08-15_/
// assignations
datetime64 Date_time_abstract::set_now()
{  mod_date().set_now();
   mod_time().set_now();
   return get_datetime64();
}
//_set_now__________________________________________________________2017-08-15_/
datetime64 Date_time_abstract::set_c_str
(const char *date_time_str
,const Date_format *from_date_format                                             //180625
,const Time_format *from_time_format)                               assignation_ //180625
{  mod_date().set_c_str(date_time_str,from_date_format);                        //180625
/*now implemented
    //continue here need to implement
    // need to get the time pos and mod_time() for the time substring!!!
    // Search for T // ISO
*/
   //Currently only supporting ISO date time separator
    const char *time_str = strstr(date_time_str,"T");                            //190110
    if (time_str) mod_time().set_c_str(time_str,from_time_format);              //190110
   return get_datetime64();
}
//_set_c_str___________________________________________________________________/
}//_namespace_CORN__________________________________________________2016-06-06_/
