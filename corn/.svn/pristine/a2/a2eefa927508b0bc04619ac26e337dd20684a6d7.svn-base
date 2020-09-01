#include "corn/measure/units_enum.h"
#include "corn/metrology/units_symbology.h"
namespace CORN {
//______________________________________________________________________________
Labeled_units_enum::Labeled_units_enum()
: Enum_abstract()
, units_clad(0)
{}
//______________________________________________________________________________
Labeled_units_enum::Labeled_units_enum(Units_code _units)
: Enum_abstract()
, units_clad(_units)
{}
//______________________________________________________________________________
Labeled_units_enum::~Labeled_units_enum() {}                                     //130515
//______________________________________________________________________________
const char *Labeled_units_enum::append_label(std::string &append_to)      const
{  return units_clad.label_string(append_to);
}
//__________________________________________________________________2019-01-09_/
int32 Labeled_units_enum::set_label(const char *_label)
   // This must be overridden to set the enumeration value for the
   // specified label.
{  return units_clad.set_label(_label);
}
//______________________________________________________________________________
int32 Labeled_units_enum::get_int32()                                      const
{  return
   // This will return (casting) the current value to 32bit integer.
   // This must be overridden derived classes
   (int32) units_clad.get();  // Note that unit codes are unsigned, but currently the MSB is not used
}
//______________________________________________________________________________
int32 Labeled_units_enum::set_int32(int32 _value)
{  return units_clad.set((Units_code) _value);
   // This sets the value (casting)
   // This must be overridden derived classes
}
//______________________________________________________________________________
bool Labeled_units_enum::is_label(const char *_label)               affirmation_
{  return  units_identify_code(_label);
   // return true if the label is in the label table
}
//______________________________________________________________________________
/*
const char *Labeled_units_enum::get_description(std::string &return_buffer)const //190105
{
   // Returns the character string description associated with the current
   // enumeration value.
   // If it is a simple label table, the label is simply returned.
   std::string unused_abbrev;                                                    //170423
   units_clad.compose_description_str(return_buffer, unused_abbrev);             //170423
   return return_buffer.c_str();                                                 //170423
}
//______________________________________________________________________________
*/
const char *Labeled_units_enum::append_description(std::string &return_buffer)const
{
   // Returns the character string description associated with the current
   // enumeration value.
   // If it is a simple label table, the label is simply returned.
   std::string unused_abbrev;
   units_clad.compose_description_str(return_buffer, unused_abbrev);
   return return_buffer.c_str();
}
//______________________________________________________________________________2010-01-09

bool Labeled_units_enum::is_descriptive()                           affirmation_ { return true; }
   // Returns true if this is a descriptive enumeration
//______________________________________________________________________________
uint16 Labeled_units_enum::get_max_label_length() const { return 30; }
   // Returns the longest label length.
   // This is used to determine the field size for Data_records enumeration attributes.
//______________________________________________________________________________
}//_namespace_CORN_____________________________________________________________/
