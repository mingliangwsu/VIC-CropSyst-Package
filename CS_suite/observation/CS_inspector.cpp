#include "CS_inspector.h"
#include <algorithm>
#include "corn/measure/unitconv.h"
#include "corn/math/statistical/statistics_types.hpp"
#ifndef Data_type_enumH
#  include "corn/data_type_enum.h"
#endif
namespace CS {
//181012 now associate_count nat16 inspector_count = 0;
// Used to produce unique variable code.
// It is only 16bit because will need to reserve high word for stat descriptor
// Eventually variable the code may be composed from property ontology

CORN::Units_convertor units_convertor;

//______________________________________________________________________________
Inspector_scalar::Inspector_scalar
(const float64                            &value_
,CORN::Units_code                          value_units_code_
,const Emanator                           &context_
,const char                               *ontology_
,CORN::Statistic                           already_statistic_
,UED::Variable_code                        preferred_variable_code)
: Associate_scalar(value_,
   value_units_code_,context_,ontology_,already_statistic_,preferred_variable_code)
{}
//_2013-07-30__________________________________________________________________/
Inspector_scalar::Inspector_scalar
(const float32                            &value_
,CORN::Units_code                          value_units_code_
,const Emanator                           &context_
,const char                               *ontology_
,CORN::Statistic                           already_statistic_
,UED::Variable_code                        preferred_variable_code)
: Associate_scalar(value_,
   value_units_code_,context_,ontology_,already_statistic_,preferred_variable_code)
{}
//_2013-07-30__________________________________________________________________/
Inspector_scalar::Inspector_scalar
(const int32                              &value_
,CORN::Units_code                          value_units_code_
,const Emanator                           &context_
,const char                               *ontology_
,CORN::Statistic                           already_statistic_                    //171031
,UED::Variable_code                        preferred_variable_code)
: Associate_scalar(value_,
   value_units_code_,context_,ontology_,already_statistic_,preferred_variable_code)
{}
//_2013-09-18__________________________________________________________________/
Inspector_scalar::Inspector_scalar
(const nat16                              &value_
,CORN::Units_code                          value_units_code_
,const Emanator                           &context_
,const char                               *ontology_
,CORN::Statistic                           already_statistic_                    //171031
,UED::Variable_code                        preferred_variable_code)
: Associate_scalar(value_,
   value_units_code_,context_,ontology_,already_statistic_,preferred_variable_code) //171031
{}
//_2013-09-18__________________________________________________________________/
Inspector_scalar::Inspector_scalar
(const Physical::Measure_datum &datum
,const Emanator                &context_
,const char                    *ontology_
,CORN::Statistic                already_statistic_
,UED::Variable_code             preferred_variable_code)
: Associate_scalar(datum,
   context_,ontology_,already_statistic_,preferred_variable_code)
{}
//_2016-06-26__________________________________________________________________/
/*NYI
Inspector::Inspector
(const Inspector &copy_from)
: CORN::Item()
, CORN::Parameter_number               (copy_from.current,copy_from.properties,copy_from.range,copy_from.format)
//151111, CORN::statistical::Sample_cumulative<float64,float64>(copy_from.applicable) // Note do not copy the tally values or data elements
, record(0)
, context                (copy_from.context)                                     //150818
, display_units_code                   (copy_from.display_units_code)
, already_statistic                    (copy_from.already_statistic)
, qualification_list(0)
{
   //NYI    record = copy_from.clone();
   assert(false);
}
*/
//_2013-09-13_________________________________________________copy_constructor_/
//______________________________________________________________________________
Inspector_vector::Inspector_vector
(const float64   *values_
,nat8             count_
,CORN::Units_code value_units_code_
,const Emanator   &context_
,const char       *ontology_
,CORN::Statistic  already_statistic_
,UED::Variable_code preferred_variable_code_)
: Inspector_abstract
   (value_units_code_,context_,ontology_
   ,already_statistic_,preferred_variable_code_)
, values    (values_)
, count     (count_)
{}
//_2017-03-26___________________________________________________________________
} // namespace CS

