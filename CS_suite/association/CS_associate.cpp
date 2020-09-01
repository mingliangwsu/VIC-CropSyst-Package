#include "CS_suite/association/CS_associate.h"

namespace CS
{
nat16 associate_count = 0;
//____________________________________________________________________________
Associate::Associate
(CORN::Units_code                          value_units_code_
,const Emanator                           &context_
,const char                               *ontology_
,CORN::Statistic                           already_statistic_
,UED::Variable_code                        preferred_variable_code)
: CORN::Item()
, variable_code_clad(preferred_variable_code ? preferred_variable_code : ++associate_count,0,true)
, value_units_code   (value_units_code_)
, context            (context_)
, ontology           (ontology_)
, already_statistic  (already_statistic_)
, instance_name_qualified()
, instance_qualification_list(0)
{ set_variable_code(preferred_variable_code); }
//_Associate:constructor____________________________________________2017-03-14_/
void Associate::set_variable_code(UED::Variable_code preferred_variable_code)
{
   if (preferred_variable_code)
   {
      if ((preferred_variable_code & 0xFF000000) == 0)// This appears to be a CropSyst property (CSVP)
      {
         UED::Variable_code_cowl var_prop_cowled(preferred_variable_code);
         if (var_prop_cowled.is_layered_V2())
         {  // variable property is setup already we only need to indicate non-standard
            variable_code_clad.set(preferred_variable_code);
         } else
         {  // Convert variable code to V2
            variable_code_clad.set(var_prop_cowled.get_without_layer_V1());
            variable_code_clad.set_layer_V2(var_prop_cowled.get_layer_V1());
         }
         variable_code_clad.set_non_standard();
      } else
      {  // This is a fully composed variable code UED standard or CropSyst
         variable_code_clad.set(preferred_variable_code);
      }
   } else
   {  variable_code_clad.set(++associate_count);
      variable_code_clad.set_non_standard();
   }
}
//_Associate::set_variable_code_____________________________________2016-06-27_/
bool Associate::write(std::ostream &strm)                             stream_IO_
{  strm << provide_instance_name_qualified();
   return true;
}
//_Associate::write_________________________________________________2015-01-02_/
const char *Associate::key_string (std::string &buffer)          const
{  buffer = provide_instance_name_qualified();
   return buffer.c_str();
}
//_key_string_______________________________________________________2018-08-23_/
const std::string &Associate::provide_instance_name_qualified
(char qualification_separator)                                        provision_
{  if (!instance_name_qualified.length())
   {
      CORN::Text_list instance_path;
      list_instance_qualification(instance_path);
      instance_path.string_items(instance_name_qualified,qualification_separator);
      // Part of the instance name may be ontological so replace / with .
      std::replace(instance_name_qualified.begin(),instance_name_qualified.end(),'/','.');
   }
   return instance_name_qualified;
}
//_provide_instance_name_qualified__________________________________2015-08-27_/
nat16 Associate::list_instance_qualification
(CORN::Text_list &append_to_qualification_list)                            const
{  context.list_instance_path(append_to_qualification_list);
   append_to_qualification_list.add_string(ontology);
      // At this point the variable instance is the ontology
      // Eventually this will be final name in the string
      // returns a list of the qualified descriptive labels for the listener.
      // List the recorder contexts and append this instance's variable name
   if (already_statistic)                                                        //160626
      append_to_qualification_list.add_cstr
         (CORN::statistic_label_table[already_statistic]);
   return true;
}
//_list_instance_qualification______________________________________2015-08-28_/
const CORN::Text_list &Associate::provide_instance_qualification_list() provision_
{  if (!instance_qualification_list)
   {  instance_qualification_list = new CORN::Text_list;
      list_instance_qualification(*instance_qualification_list);
   }
   return *instance_qualification_list;
}
//_provide_instance_qualification_list______________________________2015-08-28_/
const std::string &Associate::provide_ontology_qualified
(char qualification_separator)                                        provision_
{  if (!ontology_qualified.length())
   {
      CORN::Text_list ontology_path;
      context.append_ontology_domain_name_to(ontology_qualified,':');            //170322
      nat32 appended_count = ontology_path.string_items(ontology_qualified,qualification_separator);
      if (appended_count)
         ontology_qualified += qualification_separator;
      ontology_qualified.append(ontology);
          // Eventually this will be final name in the string
   }
   return ontology_qualified;
}
//_provide_ontology_qualified_______________________________________2016-06-14_/
nat16 Associate::list_ontology_qualification
(CORN::Text_list &append_to_qualification_list)                            const
{
      // returns a list of the qualified descriptive labels for the inspector.
      // List the recorder contexts and append this instance's variable name
  append_to_qualification_list.add_string(ontology);
   if (already_statistic)                                                        //160626
      append_to_qualification_list.add_cstr
         (CORN::statistic_label_table[already_statistic]);
   return true;
}
//_list_ontology_qualification______________________________________2016-06-14_/
const CORN::Text_list &Associate
::provide_ontology_qualification_list()                               provision_
{  if (!ontology_qualification_list)
   {  ontology_qualification_list = new CORN::Text_list;
      list_ontology_qualification(*ontology_qualification_list);
   }
   return *ontology_qualification_list;
}
//_provide_ontology_qualification_list______________________________2016-06-14_/
nat32 Associate::get_variable_code()                                       const
{
   //NYI assert(false);
   // I am not sure yet how to derive the variable code
   // I could continue to use the current coding mechansim
   // But I in V5 would like a more automated method to compose these codes.
   return variable_code_clad.get();
}
//_get_variable_code________________________________________________2016-06-09_/
const char *Associate::get_units_label(std::string &units_description)     const
{  char component_description [256];
   char component_abbr        [100];
   CORN::Units_clad smart_units(value_units_code);
   smart_units.compose_description(component_description, component_abbr);
   units_description.assign(component_abbr);
   return  units_description.c_str();
}
//_get_units_label_____________________________________________________________/
Associate_scalar::Associate_scalar
(float64            &value_
,CORN::Units_code    value_units_code_
,const Emanator     &context_
,const char         *ontology_
,CORN::Statistic     already_statistic_
,UED::Variable_code  preferred_variable_code_)
: Associate
      (value_units_code_
      ,context_
      ,ontology_
      ,already_statistic_
      ,preferred_variable_code_)
, CORN::Number_keyed(value_)
{}
//_Associate_scalar:constructor________________________________________________/
Associate_scalar::Associate_scalar
(float32    &value_
,CORN::Units_code value_units_code_
,const Emanator   &context_
,const char       *ontology_
,CORN::Statistic  already_statistic_
,UED::Variable_code preferred_variable_code_)
: Associate
      (value_units_code_
      ,context_
      ,ontology_
      ,already_statistic_
      ,preferred_variable_code_)
, CORN::Number_keyed(value_)
{}
//_Associate_scalar:constructor________________________________________________/
Associate_scalar::Associate_scalar
(int32      &value_
,CORN::Units_code value_units_code_
,const Emanator   &context_
,const char       *ontology_
,CORN::Statistic  already_statistic_
,UED::Variable_code preferred_variable_code_)
: Associate
      (value_units_code_
      ,context_
      ,ontology_
      ,already_statistic_
      ,preferred_variable_code_)
, CORN::Number_keyed(value_)
{}
//_Associate_scalar:construtor_________________________________________________/
Associate_scalar::Associate_scalar
(nat16            &value_
,CORN::Units_code value_units_code_
,const Emanator   &context_
,const char       *ontology_
,CORN::Statistic  already_statistic_
,UED::Variable_code preferred_variable_code_)
: Associate
      (value_units_code_
      ,context_
      ,ontology_
      ,already_statistic_
      ,preferred_variable_code_)
, CORN::Number_keyed(value_)
{}
//_Associate_scalar:constructor________________________________________________/
Associate_scalar::Associate_scalar
(Physical::Measure_datum &datum_
,const Emanator   &context_
,const char       *ontology_
,CORN::Statistic  already_statistic_
,UED::Variable_code preferred_variable_code_)
: Associate
      (datum_.units
      ,context_
      ,ontology_
      ,already_statistic_
      ,preferred_variable_code_)
, CORN::Number_keyed(datum_.amount)
{}
//_Associate_scalar:constructor________________________________________________/
Associate_scalar::Associate_scalar
(const float64    &value_
,CORN::Units_code value_units_code_
,const Emanator   &context_
,const char       *ontology_
,CORN::Statistic  already_statistic_
,UED::Variable_code preferred_variable_code_)
: Associate
      (value_units_code_
      ,context_
      ,ontology_
      ,already_statistic_
      ,preferred_variable_code_)
, CORN::Number_keyed(value_)
{}
//_Associate_scalar:constructor________________________________________________/
Associate_scalar::Associate_scalar
(const float32    &value_
,CORN::Units_code value_units_code_
,const Emanator   &context_
,const char       *ontology_
,CORN::Statistic  already_statistic_
,UED::Variable_code preferred_variable_code_)
: Associate
      (value_units_code_
      ,context_
      ,ontology_
      ,already_statistic_
      ,preferred_variable_code_)
, CORN::Number_keyed(value_)
{}
//_Associate_scalar:constructor________________________________________________/
Associate_scalar::Associate_scalar
(const int32      &value_
,CORN::Units_code value_units_code_
,const Emanator   &context_
,const char       *ontology_
,CORN::Statistic  already_statistic_
,UED::Variable_code preferred_variable_code_)
: Associate
      (value_units_code_
      ,context_
      ,ontology_
      ,already_statistic_
      ,preferred_variable_code_)
, CORN::Number_keyed(value_)
{}
//_Associate_scalar:constructor________________________________________________/
Associate_scalar::Associate_scalar
(const nat16      &value_
,CORN::Units_code value_units_code_
,const Emanator   &context_
,const char       *ontology_
,CORN::Statistic  already_statistic_
,UED::Variable_code preferred_variable_code_)
: Associate
      (value_units_code_
      ,context_
      ,ontology_
      ,already_statistic_
      ,preferred_variable_code_)
, CORN::Number_keyed(value_)
{}
//_Associate_scalar:constructor________________________________________________/
Associate_scalar::Associate_scalar
(const Physical::Measure_datum &datum_
,const Emanator   &context_
,const char       *ontology_
,CORN::Statistic  already_statistic_
,UED::Variable_code preferred_variable_code_)
: Associate
      (datum_.units
      ,context_
      ,ontology_
      ,already_statistic_
      ,preferred_variable_code_)
, CORN::Number_keyed(datum_.amount)
{}
//_Associate_scalar:constructor________________________________________________/
int  Associate_scalar::compare(const CORN::Item &other_item)               const
{
   const Associate_scalar *other_as_scalar
      = dynamic_cast<const Associate_scalar *>(&other_item);                     //170303
   if (!other_as_scalar) return 1;                                               //170303
   const Associate_scalar &other = dynamic_cast<const Associate_scalar &>(other_item);
   const CORN::Number_keyed::Value *other_value_ref = &other.value;
   const CORN::Number_keyed::Value *this_value_ref = &this->value;
   int comparison = 0;
   if (other_value_ref == this_value_ref)
        comparison = 0;
   else comparison = -1;
   return comparison;
}
//_Associate_scalar::compare________________________________________2016-06-18_/

//____________________________________________________________________________
} // namespace CS

