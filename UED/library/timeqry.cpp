#include "corn/chronometry/date_32.h"
#include "UED/library/timeqry.h"
namespace UED {
///////////////////////////////////////////////////////////////////////////////
bool Time_query::match(datetime64 match_date) // cannot be const
{  bool result = false;
   datetime64 target_date_time = get_target_datetime64();                        //050718
   int match_target_comparison =
        (match_date > target_date_time)
      ? 1
      : (match_date < target_date_time)
         ? -1 : 0;
   switch (query_mode)
   {  case at        : result = match_target_comparison == 0; break;
      case after     : result = match_target_comparison > 0;  break;
      case before    : result = match_target_comparison < 0;  break;
      case between   : result = (match_date >= low_date_time) && (match_date <= high_date_time); break;
      // between is inclusive
      case all       : result = true; break;
   }
   return result;
}
//_Time_query:match____________________________________________________________/
bool  Time_query::match_range
(datetime64 match_low_date
,datetime64 match_high_date)
{  return (match(match_low_date) && match(match_high_date));
}
//_Time_query:match_range______________________________________________________/
bool Time_search::match(datetime64 match_date)
{  bool query_matches = Time_query::match(match_date);
   bool search_matches = false;
   switch (search_mode)
   {case earliest :
         if (match_date <= low_date_time)
         {  low_date_time = match_date;                                          //030714
            search_matches = true;
         }
    break;
    case latest :
         if (match_date >= high_date_time)
         {  high_date_time = match_date;                                         //030714
            search_matches = true;
         }
    break;
    case closest:
    { if (match_date >= low_date_time)                                           //030714
         {  low_date_time = match_date;
            search_matches = true;
         }
      if (match_date <= high_date_time)
         {  high_date_time = match_date;
            search_matches = true;
         }
    } break;
   }
   return query_matches && search_matches;
}
//_Time_search:match___________________________________________________________/
#if (CROPSYST_VERSION==3)
// 030714 This is used only in CropSyst Version 3
void Time_query::set_year(int16 new_year)
{
   target_date_time.set_year(new_year);
   low_date_time.set_year(new_year);
   high_date_time.set_year(new_year);
}
//_Time_query::set_year_____________________________________________2003-07-14_/
#endif
//______________________________________________________________________________
//             Constructor for at, after, before
Time_query_single_date::Time_query_single_date
(datetime64       target_date_time_
,UED_units_code   time_step_
,query_modes      query_mode_)
:Time_query(target_date_time_,0,0,time_step_,query_mode_)
{  if (query_mode_ == Time_query::at)
   {  low_date_time  = target_date_time_;
      high_date_time = target_date_time_;
   }
}
//_Time_query_single_date:constructor_______________________________2004-02-02_/
//             Constructor for month
#ifdef REIMPLEMENTED
//170525 This was never used but reimplement for completion
Time_query_month::Time_query_month
(Year    target_year
,Month   target_month
,query_modes      query_mode_)
:Time_query(CORN::Date_clad_32((const CORN::Year)target_year,(const CORN::Month)target_month,(const CORN::DOM)1).get_datetime64(),0,0,UT_month,query_mode_)
{  if (i_query_mode == Time_query::at)
   {  low_date_time  = get_datetime64();
      high_date_time = get_datetime64();
   }
}
#endif
//_Time_query_month:constructor_____________________________________2009-03-03_/
bool Time_query::offset_year(int16 by_years)
{  float64 years_by = by_years * 1000.0;
   low_date_time += years_by;
   high_date_time += years_by;
   inc_years(by_years);
   return true;
}
//_Time_query:offset_year___________________________________________2014-10-31_/
}//_namespace UED______________________________________________________________/


