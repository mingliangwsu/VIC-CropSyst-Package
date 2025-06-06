#include <string>
#include "corn/quality.hpp"
#ifndef compareHPP
#  include "corn/math/compare.hpp"
#endif

namespace CORN
{
#include "corn/quality.c_cpp"
//______________________________________________________________________________
Quality_cowl::Quality_cowl(Quality_code  &_code)                                 //150120
: Enum_abstract(quality_labels)                                                  //190109
, ref_code(_code)
{}
//_Quality_cowl_____________________________________________________2015-01-30_/
const char *Quality_cowl::get_description() const
{  static std::string  quality_label("?");  // need to make this a return buffer
   switch (ref_code)                                                             //000830
   {  case  not_valid_quality                   : quality_label = "invalid"                     ; break;
      case  out_of_range_deprecated_quality     : quality_label = "out of range"                ; break; //050415
      case  out_of_range_warning_quality        : quality_label = "warning: out of range"       ; break; //050415
      case  out_of_range_low_warning_quality    : quality_label = "warning: out of range (low)" ; break; //050415
      case  out_of_range_high_warning_quality   : quality_label = "warning: out of range (high)"; break; //050415
      case  out_of_range_error_quality          : quality_label = "error: out of range"         ; break; //050415
      case  out_of_range_low_error_quality      : quality_label = "error: out of range (low)"   ; break; //050415
      case  out_of_range_high_error_quality     : quality_label = "error: out of range (high)"  ; break; //050415
      case  missing_quality                     : quality_label = "missing (may be optional)"   ; break;
      case  unknown_quality                     : quality_label = "unknown"                     ; break;
      default :
      {  if (ref_code < derived_quality)
                        quality_label = quality_labels[ref_code];
         else if (is_valid())
         {
             Quality_code significant_quality = (Quality_code)(ref_code >> 4) ;
             Quality_code degraded_quality = (Quality_code)(ref_code & 0xF);

             std::string significant_quality_label = quality_abbr_labels[significant_quality];
             quality_label.assign(
                degraded_quality < derived_quality
                ? quality_abbr_labels[degraded_quality]
                : "derived");
             quality_label.append(" from ");
             quality_label.append(significant_quality_label);
      }  } break;
   }
   return quality_label.c_str();                                                 //050609
}
//_get_description_____________________________________________________________/
int32 Quality_cowl::set_label(const char *_label)
{  // This must be overridden to set the enumeration value for the
   // specified label.
   // NYI  Currently only doing the simple ones,
   // but need to handle composed and invalid codes.
   return Enum_abstract::set_label(_label);                                      //190109
}
//_set_label___________________________________________________________________/
int32 Quality_cowl::set_int32(int32 _value)
{ ref_code = (Quality_code)_value; return _value;}
//_set_int32___________________________________________________________________/
uint16 Quality_cowl::get_max_label_length()                                const
{  // Returns the longest label length.
   // This is used to determine the field size for Data_records enumeration attributes.
   // NYI should determing this, but 50 should be sufficient.
   return 50;
}
//_get_max_label_length_____________________________________________2005-09-29_/
Quality_code Quality_cowl::invalidate(bool absolutely)            rectification_
{  if (absolutely || !is_optimal())                                              //091215
      ref_code = not_valid_quality;
   return ref_code;
}
//_invalidate__________________________________________________________________/
bool Quality::is_optimal()                                          affirmation_
{  return is_optimal_quality(get_quality_code());
}
//_is_optimal_______________________________________________________2005-06-09_/
bool Quality::is_worse_than(const Quality &that)                    affirmation_
{ return quality_is_worse_than(get_quality_code(),that.get_quality_code()); }
//_is_worse_than____________________________________________________2008-10-24_/
bool Quality::is_worse_than(Quality_code that)                      affirmation_
{ return quality_is_worse_than(get_quality_code(),that); }
//_is_worse_than____________________________________________________2015-01-24_/
bool Quality::is_better_than(const Quality &that)                   affirmation_
{   return quality_is_better_than(get_quality_code(),that.get_quality_code()); }
//_is_better_than___________________________________________________2008-10-24_/
bool Quality::is_better_than(Quality_code that_code)                affirmation_
{    return quality_is_better_than(get_quality_code(),that_code);}
      // returns true if this has better quality than that.
//_is_better_than___________________________________________________2008-10-24_/
bool Quality::is_same_or_better_than( Quality_code that_code)       affirmation_
{  return quality_is_same_or_better_than(get_quality_code(),that_code); }
//_is_same_or_better_than___________________________________________2008-10-24_/
bool Quality::is_same_or_better_than(const Quality &that_code)      affirmation_
{  return is_same_or_better_than(that_code.get_quality_code()); }
//_2008-10-24___________________________________________________________________
const Quality &Quality::worst(const Quality &that)                         const
{  return this->is_worse_than(that) ? *this : that; }
      // returns this or that which ever is the worst quality.
//_worst____________________________________________________________2008-10-24_/
const Quality &Quality::best(const Quality &that)                          const
{  return this->is_better_than(that) ? *this : that; }
      // returns this or that which ever is the best quality.
//_best_____________________________________________________________2008-10-24_/
Quality_code quality_calculated_from_quality   (const Quality &qual) {  return degrade_quality(qual.get_quality_code(),calculated_quality);  }
Quality_code quality_other_real_from_quality   (const Quality &qual) {  return degrade_quality(qual.get_quality_code(),other_real_quality);  }
Quality_code quality_interpolated_from_quality (const Quality &qual) {  return degrade_quality(qual.get_quality_code(),interpolated_quality);}
Quality_code quality_estimated_from_quality    (const Quality &qual) {  return degrade_quality(qual.get_quality_code(),estimated_quality);   }
Quality_code quality_forecasted_from_quality   (const Quality &qual) {  return degrade_quality(qual.get_quality_code(),forecasted_quality);  }
Quality_code quality_generated_from_quality    (const Quality &qual) {  return degrade_quality(qual.get_quality_code(),generated_quality);   }
//_quality_from_quality_____________________________________________2014-05-08_/
const Quality &Quality_cowl::assume_code(CORN::Quality_code _quality) rectification_
{  ref_code = _quality;                                          return *this; }
//_assume_code______________________________________________________2015-01-23_/
const Quality &Quality_cowl::assume(const CORN::Quality &new_quality) rectification_
{  ref_code = new_quality.get_quality_code();                    return *this; }
//_assume___________________________________________________________2015-01-23_/
}//_namespace_CORN_____________________________________________________________/
