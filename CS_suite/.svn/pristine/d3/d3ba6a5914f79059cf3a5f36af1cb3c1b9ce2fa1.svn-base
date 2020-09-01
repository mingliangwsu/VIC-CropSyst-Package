#ifndef CS_inspectorH
#define CS_inspectorH

#include "CS_suite/association/CS_associate.h"
#include "corn/container/enumlist.h"

namespace CS {
#define GENERATE_VARIABLE_CODE 0
//______________________________________________________________________________
typedef Associate Inspector_abstract ;
//______________________________________________________________________________
class Inspector_scalar
: public extends_ Associate_scalar                                               //181012
{
   // Inspection of a single value
 public:
   Inspector_scalar
      (const float64    &value
      ,CORN::Units_code value_units_code
      ,const Emanator   &context
      ,const char       *ontology
      ,CORN::Statistic  already_statistic
      ,UED::Variable_code preferred_variable_code = 0);
   Inspector_scalar
      (const float32    &value
      ,CORN::Units_code value_units_code
      ,const Emanator   &context
      ,const char       *ontology
      ,CORN::Statistic  already_statistic
      ,UED::Variable_code preferred_variable_code = 0);
   Inspector_scalar
      (const int32      &value
      ,CORN::Units_code value_units_code
      ,const Emanator   &context
      ,const char       *ontology
      ,CORN::Statistic   already_statistic_                                      //171031
      ,UED::Variable_code preferred_variable_code = 0);
   Inspector_scalar
      (const nat16      &value
      ,CORN::Units_code value_units_code
      ,const Emanator   &context
      ,const char       *ontology
      ,CORN::Statistic  already_statistic                                        //171031
      ,UED::Variable_code preferred_variable_code = 0);
   Inspector_scalar
      (const Physical::Measure_datum &datum
      ,const Emanator   &context
      ,const char       *ontology
      ,CORN::Statistic  already_statistic = CORN::value_statistic
      ,UED::Variable_code preferred_variable_code = 0);
};
//_Inspector_scalar_____________________________________2017-03-14__2016-06-04_/
class Inspector_vector
: public extends_ Associate
{
   const float64   *values;
   nat8             count;
 public:
   Inspector_vector
      (const float64   *values_
      ,nat8             count_
      ,CORN::Units_code value_units_code
      ,const Emanator   &context
      ,const char       *ontology
      ,CORN::Statistic  already_statistic
      ,UED::Variable_code preferred_variable_code = 0);
   inline virtual CORN::Data_type get_mode()                               const
      { return CORN::DT_float64; }
   inline virtual bool is_vector()                 affirmation_ { return true; }
   inline virtual int compare(const CORN::Item &)                          const
      { return 1; }
      // Not currently needed assuming all Inspector_vector intances
      // are unique. Could return 00 if pointing to the same values.

};
//_Inspector_vector_________________________________________________2017-03-14_/
} // namespace CS
#endif

