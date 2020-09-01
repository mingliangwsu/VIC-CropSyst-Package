#include "crop/quiescence.h"

namespace CropSyst {
//______________________________________________________________________________
Quiescence_requirement::Quiescence_requirement
(const Crop_parameters_struct::Quiescence &_parameters)
:parameters(_parameters)
,consecutive_days(0)
{}
//_Quiescence_start_requirement:constructor_________________________2008-04-01_/
Quiescence_start_requirement::Quiescence_start_requirement
(const Crop_parameters_struct::Quiescence &parameters_
,bool _fruit_crop)
:Quiescence_requirement(parameters_)
,fruit_crop(_fruit_crop)
,within_valid_dormancy_period_now(false)                                         //030701
{}
//_Quiescence_start_requirement:constructor_________________________2008-04-01_/
/*180601 dont delete yet

bool Quiescence_start_requirement::in_valid_inactive_period(CORN::DOY doy) const
{

#if (CS_VERSION==4)
   CORN::DOY start_inactive_period_valid_DOY =  parameters.start_valid_date.get_DOY(); //020409
   CORN::DOY leave_inactive_period_valid_DOY =  parameters.leave_valid_date.get_DOY(); //020409
#else
   CORN::DOY start_inactive_period_valid_DOY =  parameters.start_DOY;            //110911
   CORN::DOY leave_inactive_period_valid_DOY =  parameters.end_DOY;              //110911
#endif
   if (doy > start_inactive_period_valid_DOY) within_valid_dormancy_period_now = true; //030616
   bool in_dorm_period
   //  030631   note, this is not working some time for fruit when the crop doesn't start dorrmancy by the beginning of the year
   // , we should set a bool for start of dormancy check and clear it when it goes.
      = fruit_crop
      ?  // Fruit crops dont have leave_dormancy date
         within_valid_dormancy_period_now
      :  ( (leave_inactive_period_valid_DOY < start_inactive_period_valid_DOY)
         ? (doy < leave_inactive_period_valid_DOY) || (doy > start_inactive_period_valid_DOY)
         : (doy > start_inactive_period_valid_DOY) && (doy < leave_inactive_period_valid_DOY))
      ;
   return in_dorm_period;
}
//_in_valid_inactive_period_________________________________________2001-12-05_/
*/
CORN::Days Quiescence_start_requirement::add_daily_temperature
(CORN::DOY today_DOY ,float32 temperature_with_est_night)
{  if (temperature_with_est_night < parameters.inducement_temperature)           //011205
        consecutive_days ++;
   else consecutive_days = 0;
   return consecutive_days;                                                      //190709
}
//_add_daily_temperature_______________________________________________________/
Quiescence_end_requirement::Quiescence_end_requirement
(const Crop_parameters_struct::Quiescence &parameters_)
:Quiescence_requirement(parameters_)                                             //160326
/*180601 Claudio says obsolete but dont delete yet
,inactive_days_remaining(60)
*/
{
/*180601 Claudio says obsolete but dont delete yet

#if (CS_VERSION==4)
   CORN::DOY quick_leave_inactive_period_valid_DOY = parameters.leave_valid_date.get_DOY();
#else
   CORN::DOY quick_leave_inactive_period_valid_DOY = parameters.end_DOY;
#endif
   inactive_days_remaining  =
      (quick_leave_inactive_period_valid_DOY < inactive_period_start_DOY)
      ? 365 - inactive_period_start_DOY + quick_leave_inactive_period_valid_DOY
      : quick_leave_inactive_period_valid_DOY -  inactive_period_start_DOY;
*/
}
//_Quiescence_end_requirement_constructor_______________2008-04-01__1998-07-28_/
CORN::Days Quiescence_end_requirement::add_daily_temperature(float32 average_air_temperature)
{
/*180601 Claudio says obsolete but dont delete yet
   inactive_days_remaining -= 1;
*/
   if (
/*180601 Claudio says obsolete but dont delete yet
      (inactive_days_remaining < 0)                                              //970520
         &&
*/
         (average_air_temperature >= parameters.inducement_temperature))         //080911
        consecutive_days ++;                                                     //970219
   else consecutive_days = 0;                                                    //970219
   return consecutive_days;                                                      //190709
}
//_add_daily_temperature____________________________________________2002-12-02_/
}//_namespace CropSyst_________________________________________________________/


