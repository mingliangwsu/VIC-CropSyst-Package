#error check obsolete
#ifndef parameter_interpolationH
#define parameter_interpolationH
#include "corn/math/numerical/interpolator.h"
#include "corn/parameters/number_keyed.h"
namespace CORN
{
//______________________________________________________________________________
class Parameter_interpolation_dated  // rename to Parameter_interpolator_temporal
: public implements_ CORN::Item
{
   const       CORN::Date   &today;
   modifiable_ Number_keyed &parameter;
   Interpolation_temporal curve;   // owned always instanciated
 public:
   Parameter_interpolation_dated
      (Interpolation_curve_type curve_type
      ,const CORN::Date     &_today
      ,Number_keyed  &_parameter);
   bool update();
   inline virtual bool is_key_string(const std::string &key)        affirmation_ //180820
      { return parameter == key; }
   inline virtual bool is_key_wstring(const std::wstring &key)      affirmation_ //180820
      { return parameter == key; } NYI
};
//_Parameter_interpolation_dated____________________________________2015-01-11_/
}//_namespace_CORN_____________________________________________________________/
#endif // PARAMETER_INTERPOLATION_H

