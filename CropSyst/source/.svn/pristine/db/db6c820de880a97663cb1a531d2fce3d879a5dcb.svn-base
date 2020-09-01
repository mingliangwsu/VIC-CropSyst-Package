#ifndef quiescenceH
#define quiescenceH
#include "crop/crop_param_struct.h"
namespace CropSyst {

// Note: this used to be crop dormancy period and Inactive_period
//______________________________________________________________________________
class Quiescence_requirement
{
 protected:
   const Crop_parameters_struct::Quiescence &parameters;
   CORN::Days                 consecutive_days;
 public:
   Quiescence_requirement
      (const Crop_parameters_struct::Quiescence  &parameters_);
   inline virtual ~Quiescence_requirement() {}                                   //170217
   virtual inline bool is_satisfied()                               affirmation_ //170817
   {  return consecutive_days >= (CORN::Days)parameters.consider_inactive_days;  //080401
   }
};
//_class Quiescence_requirement_________________________2016-03-26__2002-12-02_/
class Quiescence_start_requirement
: public Quiescence_requirement
{
   bool fruit_crop;         // fruit crops do not consider the end of dormancy
   mutable bool within_valid_dormancy_period_now ;
   // This is used to check the start of dormancy
 public:
   Quiescence_start_requirement
      (const Crop_parameters_struct::Quiescence &parameters, bool fruit_crop);   //140318
    CORN::Days add_daily_temperature
      (CORN::DOY today_DOY,float32 temperature_with_est_night);
/*180601
 private:
   bool in_valid_inactive_period(CORN::DOY doy) const;                           //011205
*/
};
//_class Quiescence_start_requirement________2016-03-26__2016-03-26_2002-12-02_/
class Quiescence_end_requirement
: public Quiescence_requirement
{  // This is used to check the end of dormancy
/*180601 Claudio says obsolete but dont delete yet
   int                  inactive_days_remaining;
*/
 public:
   Quiescence_end_requirement
      (const Crop_parameters_struct::Quiescence &parameters_
      //190424 ,CORN::DOY dormancy_start_DOY_obsolete
      );
    CORN::Days add_daily_temperature(float32 average_air_temperature);
 };
//_class Quiescence_end_requirement_____________________2016-03-26__2002-12-02_;
}//_namespace CropSyst_________________________________________________________/
#endif

