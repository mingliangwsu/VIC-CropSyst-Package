#ifndef CS_parameter_float64H
#define CS_parameter_float64H

#include "UED/library/codes.h"
#include "corn/const.h"
#include "corn/quality.hpp"
#include "common/CS_parameter_cowl_T.h"
#include "common/physics/property_complete.h"

namespace CS {
//______________________________________________________________________________
class Parameter_float64
: public extends_ Physical::Measure_datum
, public extends_ Parameter_cowl_T<float64>
{
 public:
   CORN::Quality_code      quality_code;
 public:
   inline Parameter_float64
      (bool                extensive_
      ,float64             default_value   // Probably should be NAN
      ,CORN::Units_code    default_units  // = U_units_not_specified
      ,CORN::Quality_code  default_quality)// = CORN::unknown_quality
      : Physical::Measure_datum     (default_value,default_units)
      , Parameter_cowl_T<float64>     (extensive_,amount,units,quality_code)
      , quality_code                (default_quality)
      {}
   inline virtual ~Parameter_float64() {}                                        //151204
   inline virtual float64 force(float64 new_amount)               rectification_ //150125
      { return amount = new_amount; }
 protected:
   inline virtual float64 get_amount()                                     const
      { return amount; }
};
//_class Parameter_float64__________________________________________2015-01-22_/
/*NYI
class Parameter_float32
: public extends_ Physical::Measure_datum
, public extends_ Parameter_cowl_T<float32>
{
 public:
   CORN::Quality_code      quality_code;
 public:
   inline Parameter_float32
      (bool                extensive_
      ,float32             default_value   // Probably should be NAN
      ,CORN::Units_code    default_units  // = U_units_not_specified
      ,CORN::Quality_code  default_quality)// = CORN::unknown_quality
      : Physical::Measure_datum     (default_value,default_units)
      , Parameter_cowl_T<float32>     (extensive_,amount,units,quality_code)
      , quality_code                (default_quality)
      {}
   inline virtual ~Parameter_float32() {}
   inline virtual float64 force(float64 new_amount)               rectification_
      { return amount = new_amount; }
 protected:
   inline virtual float64 get_amount()                                     const
      { return amount; }
};
//_class Parameter_float32__________________________________________2018-11-01_/
*/
}//_namespace CS_______________________________________________________________/
#endif

