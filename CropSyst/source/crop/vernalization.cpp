#error abandoned, delete this

#include "crop/vernalization.h"
namespace CropSyst
{
//____________________________________________________________________________
float64  Vernalization::update_factor
(float64 temperature)                                             rectification_
{  Vfactor =1.0;
   float64 temp_low   = parameters.low_temp;                                     //020628
   float64 temp_high  = parameters.high_temp;                                    //020628
   float64 vern_start = parameters.start;                                        //020628
   float64 vern_end   = parameters.end;                                          //020628
   float64 min_factor = parameters.min_factor;                                   //160712
   float64 vern_day = (temperature > temp_high)
   ? (1.0 - (temperature - temp_high) / 7.0)
   : (temperature < temp_low)
      ? (1.0 - (temp_low - temperature) / 7.0)
      : 1.0;
   if (vern_day < 0.0)  vern_day = 0.0;
   vern_days += vern_day;
   if (vern_days >= vern_start)                                                  //160713
   {  Vfactor =
         min_factor + ( (1.0 - min_factor) *                                     //160712
           (vern_days - vern_start) / (vern_end - vern_start));
      if (status != VERNALIZATION_ACTIVE)
      {   // output vernalization starts to log was here
          status  = VERNALIZATION_START;                                         //020628
      }
   }
   if ((vern_days >= vern_end) && (status == VERNALIZATION_ACTIVE))
   {  Vfactor = 1.0;
      vern_days = 0.0;                                                           //021204
      status = VERNALIZATION_END;                                                //020628
   }
   return Vfactor;
}
//_update_factor_________________________________________________________1997?_/
Vernalization::Vernalization
(const CropSyst::Crop_parameters_struct::Vernalization &_parameters)
:parameters(_parameters)
,status(VERNALIZATION_INACTIVE)
,vern_days(0)
,Vfactor(1.0)
{}
//_Vernalization::constructor______________________________________2011-11-09_/


//____________________________________________________________________________
}//_namespace CropSyst_________________________________________________________/
