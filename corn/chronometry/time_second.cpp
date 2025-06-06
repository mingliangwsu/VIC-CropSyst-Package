#include "corn/chronometry/time_second.h"
namespace CORN
{
//______________________________________________________________________________
datetime64 Time_second_cowl::get_datetime64()                              const
{  return (float64)ref_seconds / (double)86400.0;
}
//_get_datetime64_______________________________________2017-04-27__2003-01-08_/
float64 Time_second_cowl::get_time64()                                     const
{  return (float64)ref_seconds / (double)86400.0;
}
//_2017-04-27_____________________________________________________________________
datetime64 Time_second_cowl::set_S(Seconds seconds_)
{  ref_seconds = seconds_;
   return get_datetime64();
}
//_2017-04-27____________________________________________________________set_S_/
datetime64 Time_second_cowl::set_time(const Time &time_)
{  ref_seconds = (time_.get_seconds());
   return get_datetime64();
}
//_2017-04-27_________________________________________________________set_time_/
datetime64 Time_second_cowl::set_datetime64(datetime64 time64)      assignation_
{  int32 ignore_date = (int32)time64;  // Ignore any date part
   float64 time_only = (float64)time64 - (float64)ignore_date;
   ref_seconds = (Seconds)(time_only * 86400.0);
   return get_datetime64();
}
//_2003-07-14___________________________________________________set_datetime64_/
datetime64 Time_second_cowl::set_HMSc
(Hour hour, Minute minute, Second second
,Centisecond centisecond)                                           assignation_
{  ref_seconds = second + minute*60 + hour*3600;
   // We don't have centisecode resolution, so can only set seconds
   return get_datetime64();
}
//_2017-04-27_________________________________________________________set_HMSc_/
datetime64 Time_second_cowl::set_HMSm
(Hour hour, Minute minute, Second second
,Millisecond millisecond)                                           assignation_
{  ref_seconds = second + minute*60 + hour*3600;
   // We don't have millisecond resolution, so can only set seconds
   return get_datetime64();
}
//_2017-04-27_________________________________________________________set_HMSm_/
datetime64 Time_second_cowl::inc_hours(Hours hours)
{  Seconds seconds_augend = hours * 3600;
   return inc_seconds(seconds_augend);
}
//_2017-04-27________________________________________________________inc_hours_/
datetime64 Time_second_cowl::inc_minutes(Minutes minutes)
{  Seconds seconds_augend = minutes * 60;
   return inc_seconds(seconds_augend);
}
//_2003-07-14______________________________________________________inc_minutes_/
datetime64 Time_second_cowl::inc_seconds(Seconds more_seconds)
{  nat32 seconds = ref_seconds;
   seconds += more_seconds;
   CORN::Hours more_days = seconds / 86400;
   ref_seconds =           seconds % 86400;
   inc_days(more_days);
   // Time classes don't know about days so they just through away the days.
   // Date and time derived classes will take the days and increment date.
   return get_datetime64();
}
//_2017-04-27______________________________________________________inc_seconds_/

//_2017-04-27___________________________________________________________________
int8 Time_second_cowl::compare(const Temporal /*180626 Time*/ &other_temporal) const
{
   const Time &other_time = dynamic_cast<const Time &>(other_temporal);                //180626
   return (int8)((ref_seconds < (other_time).get_seconds())
   ? -1 :        (ref_seconds > (other_time).get_seconds()) ? 1 : 0);
}
//_2003-01-08__________________________________________________________compare_/
}//_namespace CORN_____________________________________________________________/
