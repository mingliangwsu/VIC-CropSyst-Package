#include "corn/string/strconv.hpp"
#include "corn/math/statistical/statistics_types.hpp"
#include "UED/library/variable_format.h"
#include "UED/library/std_codes.h"

//#ifndef MINIMAL_UED
//#ifdef UED_DUMP
#include <iomanip>
//#endif
namespace UED {
//______________________________________________________________________________
Variable_format_abstract::Variable_format_abstract()
: CORN::Item() //191007 Enumeration()
, code_clad(0)
, preferred_units_clad(0xFFFFFFFF)
{
   // 0xFFFFFFFF indicates that we have a reference to the preferred units definitions
}
//_Variable_format_abstract:constructor_____________________________2016-06-19_/
Variable_format_abstract::Variable_format_abstract
(UED::Variable_code  code
,CORN::Units_code    preferred_units_code)
: CORN::Item() //191007 Enumeration   ()
, code_clad           (code)
, preferred_units_clad(preferred_units_code)
{
}
//_Variable_format_abstract:constructor_____________________________2016-06-19_/
Variable_format_abstract::Variable_format_abstract(const Variable_format_abstract &original)
: CORN::Item() //191007 Enumeration()
, code_clad(original.code_clad.get())
, preferred_units_clad(original.preferred_units_clad)
{}
//_2016-06-19___________________________________________________________________
Variable_format_ontological::Variable_format_ontological()
: Variable_format_abstract()
, ontology()
{
   // 0xFFFFFFFF indicates that we have a reference to the preferred units definitions
}
//_Variable_format_ontological:construtor___________________________2016-06-19_/
Variable_format_ontological::Variable_format_ontological
(UED::Variable_code code_
,CORN::Units_code  preferred_units_code_
,const char *ontology_)
: Variable_format_abstract(code_,preferred_units_code_)
,ontology(ontology_)
{}
//_Variable_format_ontological:constructor__________________________2016-06-19_/
nat32 Variable_format_ontological::compute_variable_format_size()          const
{  nat32 ontology_length = ontology.length()+1;
   return sizeof(nat32) //(variable code)
      + sizeof(nat32)   //(units code )
      + ontology_length;
}
//_compute_variable_format_size_____________________________________2016-06-19_/
int Variable_format_ontological::compare(const CORN::Item &other_item)     const
{  const Variable_format_ontological &other
      = dynamic_cast<const Variable_format_ontological &>(other_item);
   return ontology.compare(other.ontology);
}
//_compare__________________________________________________________2016-06-19_/
const char  *Variable_format_ontological::label_string(std::string &buffer)const
{
   if (ontology.length())
      buffer = ontology.c_str();
   else  code_clad.label_string(buffer);
   buffer += " [";
   buffer += CORN::statistic_label_table[code_clad.get_qualifier()];
   buffer += "]";
   buffer += " (";
   std::string units_description;
   std::string units_abbreviation;
      preferred_units_clad.compose_description_str(units_description,units_abbreviation);
   buffer += units_description;
   buffer += ")";
   return buffer.c_str();
}
//_label_string_____________________________________________________2016-06-19_/
Variable_format_0::Variable_format_0()
: Variable_format_abstract()
, owns_description_and_abbreviation(false)                                       //070223
{
   // 0xFFFFFFFF indicates that we have a reference to the preferred units defintions
}
//_Variable_format_0:constructor____________________________________1997-11-06_/
Variable_format_0::Variable_format_0
(  Variable_code           code_
,  CORN::Units_code        preferred_units_code_                                 //980818
,  float32 min_warning_
,  float32 max_warning_
,  float32 min_error_
,  float32 max_error_
,  const char * description_
,  const char * abbreviation_)
: Variable_format_abstract                                                       //160619
   (code_,preferred_units_code_)
, owns_description_and_abbreviation(false)                                       //070227
, description(description_ ? description_ : "")                                  //070619
, abbreviation(abbreviation_ ? abbreviation_ : "")                               //070619
{
   min_warning=min_warning_;
   max_warning=max_warning_;
   min_error=min_error_;
   max_error=max_error_;
}  // This constructor for standard units codes which can be dynamically defined by UED.
//_Variable_format_0:constructor____________________________________1997-11-06_/
Variable_format_0::Variable_format_0(const Variable_format_0 &original)
: Variable_format_abstract(original)
, owns_description_and_abbreviation(false)                                       //070227
, description(original.description)
, abbreviation(original.abbreviation)                                            //020908
{
   min_warning=original.min_warning;
   max_warning=original.max_warning;
   min_error=original.min_error;
   max_error=original.max_error;
}
//_Variable_format_0:constructor_______________________________________________/
int Variable_format_0::compare(const CORN::Item  &other_item)              const
{  const Variable_format_0 &other = (const Variable_format_0 &)other_item;
   return description.compare(other.description);
}
//_compare__________________________________________________________1998-02-10_/
const char *Variable_format_0::label_string(std::string &buffer)           const
{
   if (description.length())
         buffer = description;
   else  code_clad.label_string(buffer);
   buffer += " [";
   buffer += Variable_code_qualifier_descriptions[code_clad.get_qualifier()];
   buffer += "]";
   buffer += " (";
   CORN::append_float32_to_string(min_warning,2,buffer);
   buffer += "-";
   CORN::append_float32_to_string(max_warning,2,buffer);
   buffer += " ";
   std::string units_desc;
   std::string units_abbr;
   preferred_units_clad.compose_description_str(units_desc,units_abbr);
   buffer += units_desc;
   buffer += ")";
   return buffer.c_str();
}
//_label_string__________________________________________2017-04-23_1998-06-24_/
nat32 Variable_format_0::compute_variable_format_size() const
{  nat32 description_length = description.length()+1;
   return sizeof(nat32) //(UED_variable_code)                                    //980624
      + sizeof(nat32) //(units code )
      + sizeof(float32) //(min_warning)
      + sizeof(float32) //(max_warning)
      + sizeof(float32) //(min_error)
      + sizeof(float32) //(max_error)
   //070216 NYI   in future versions of UED, add preferred precision
      + description_length;
}
//_compute_variable_format_size________________________________________________/
using namespace std;
#ifdef UED_DUMP
void Variable_format_0::dump(std::ostream &txt_strm)
{
   //170826 char var_code_label[512] ;code_clad.label_cstr(var_code_label);
   std::string var_code_label;code_clad.label_string(var_code_label);           //170826
   txt_strm << var_code_label << ' '
            << setbase(16) << code_clad.get() << 'h' << ' '
            << setbase(10) << code_clad.get() << 'd'
            << " | Variable code " << endl;
   std::string unit_code_label;
      preferred_units_clad.label_string(unit_code_label);
   UED_units_code out_code = preferred_units_clad.get();
   txt_strm << unit_code_label << ' '
            << setbase(16) << out_code << 'h' << ' '
            << setbase(10) << out_code << 'd'
            << " | Preferred units code" << endl;
   txt_strm << min_warning << " | Min warning" << endl;
   txt_strm << max_warning << " | Max warning" << endl;
   txt_strm << min_error   << " | Min error" << endl;
   txt_strm << max_error   << " | Max error" << endl;
   txt_strm << description << " | Description" << endl;
}
//_dump________________________________________________________________________/
void Variable_format_ontological::dumpX(std::ostream &txt_strm)
{
   //170826 char var_code_label[512] ;code_clad.label_cstr(var_code_label);
   std::string var_code_label ;code_clad.label_string(var_code_label);
   txt_strm << var_code_label << ' '
            << setbase(16) << code_clad.get() << 'h' << ' '
            << setbase(10) << code_clad.get() << 'd'
            << " | Variable code " << endl;
   std::string unit_code_label;
      preferred_units_clad.label_string(unit_code_label);
   /*170826
   char unit_code_label[512];
      preferred_units_clad.label_cstr(unit_code_label);
   */
   UED_units_code out_code = preferred_units_clad.get();
   txt_strm << unit_code_label << ' '
            << setbase(16) << out_code << 'h' << ' '
            << setbase(10) << out_code << 'd'
            << " | Preferred units code" << endl;
   txt_strm << ontology << " | ontology" << endl;
}
//_dumpX_______________________________________________________________________/
#endif
//______________________________________________________________________________
void Variable_format_0::write_YAML(std::ostream &YAML_strm,nat16 indent_level)
{
   std::string indent(indent_level,' ');
   std::string var_code_label ;code_clad.label_string(var_code_label);
   YAML_strm<<indent << var_code_label << " # "
            << "0x" <<setbase(16) << code_clad.get()  << ' '
            << setbase(10) << code_clad.get()  << endl;
   std::string unit_code_label;
      preferred_units_clad.label_string(unit_code_label);

   UED_units_code out_code = preferred_units_clad.get();
   YAML_strm<<indent << unit_code_label << " # "
            << "0x" << setbase(16) << out_code << ' '
            << setbase(10) << out_code
            << " # Preferred units code" << endl;
   YAML_strm<<indent << "warning: " << endl;
   YAML_strm<<indent << " min: " << min_warning << endl;
   YAML_strm<<indent << " max: " << max_warning << endl;
   YAML_strm<<indent << "error: " << endl;
   YAML_strm<<indent << " min: " << min_error << endl;
   YAML_strm<<indent << " max: " << max_error << endl;

   YAML_strm<<indent << "description: " << description <<endl;
}
//_write_YAML_______________________________________________________2018-01-08_/
void Variable_format_ontological::write_YAML(std::ostream &YAML_strm,nat16 indent_level)
{
   std::string indent(indent_level,' ');

   std::string var_code_label ;code_clad.label_string(var_code_label);
   YAML_strm<<indent << "variable: "
            << "0x" << setbase(16) << code_clad.get()
            << " # " << var_code_label
            // << setbase(10) << code_clad.get()
            << endl;
   std::string unit_code_label;
      preferred_units_clad.label_string(unit_code_label);
   /*170826
   char unit_code_label[512];
      preferred_units_clad.label_cstr(unit_code_label);
   */
   UED_units_code out_code = preferred_units_clad.get();
   YAML_strm<<indent << "units: " << unit_code_label << " # "
            << "0x" << setbase(16) << out_code << ' '
            << setbase(10) << out_code << endl;
   YAML_strm<<indent << "ontology: " << ontology << endl;
}
//_write_YAML_______________________________________________________2018-01-08_/
std::string unrecognized_variable_abbreviation("unrecognized");
const std::string &standard_variable_abbreviation(UED::Variable_code var_code)
{
   std::string &var_abbrv = unrecognized_variable_abbreviation;
   if (STD_VD_date.code_clad.get()                              == var_code) var_abbrv = STD_VD_date                               .abbreviation;
   if (STD_VD_precipitation.code_clad.get()                     == var_code) var_abbrv = STD_VD_precipitation                      .abbreviation;
   if (STD_VD_max_temperature.code_clad.get()                   == var_code) var_abbrv = STD_VD_max_temperature                    .abbreviation;
   if (STD_VD_min_temperature.code_clad.get()                   == var_code) var_abbrv = STD_VD_min_temperature                    .abbreviation;
   if (STD_VD_avg_temperature.code_clad.get()                   == var_code) var_abbrv = STD_VD_avg_temperature                    .abbreviation;
   if (STD_VD_obs_temperature.code_clad.get()                   == var_code) var_abbrv = STD_VD_obs_temperature                    .abbreviation;
   if (STD_VD_max_dew_point_temperature.code_clad.get()         == var_code) var_abbrv = STD_VD_max_dew_point_temperature          .abbreviation;
   if (STD_VD_min_dew_point_temperature.code_clad.get()         == var_code) var_abbrv = STD_VD_min_dew_point_temperature          .abbreviation;
   if (STD_VD_avg_dew_point_temperature.code_clad.get()         == var_code) var_abbrv = STD_VD_avg_dew_point_temperature          .abbreviation; //050601
   if (STD_VD_max_relative_humidity.code_clad.get()             == var_code) var_abbrv = STD_VD_max_relative_humidity              .abbreviation;
   if (STD_VD_min_relative_humidity.code_clad.get()             == var_code) var_abbrv = STD_VD_min_relative_humidity              .abbreviation;
   if (STD_VD_avg_relative_humidity.code_clad.get()             == var_code) var_abbrv = STD_VD_avg_relative_humidity              .abbreviation; //050419
   if (STD_VD_solar_radiation_global.code_clad.get()            == var_code) var_abbrv = STD_VD_solar_radiation_global             .abbreviation; //131013
   if (STD_VD_net_radiation.code_clad.get()                     == var_code) var_abbrv = STD_VD_net_radiation                      .abbreviation; //070204
/*170926  obsolete
#if ((CS_VERSION >0) && (CS_VERSION < 6))
   if (STD_VD_solar_radiation_deprecated.code_clad.get()                   == var_code) var_abbrv = STD_VD_solar_radiation_deprecated                    .abbreviation;
   if (STD_VD_isothermal_longwave_net_radiation.code_clad.get() == var_code) var_abbrv = STD_VD_isothermal_longwave_net_radiation  .abbreviation; //070204
//   if (STD_VD_net_radiation.code_clad.get()                     == var_code) var_abbrv = STD_VD_net_radiation                      .abbreviation; //070204
#else
   //if (STD_VD_solar_radiation.code_clad.get()                   == var_code) var_abbrv = STD_VD_solar_radiation_total                    .abbreviation;
   //if (STD_VD_isothermal_longwave_net_radiation_total.code_clad.get() == var_code) var_abbrv = STD_VD_isothermal_longwave_net_radiation_total  .abbreviation; //070204
//   if (STD_VD_net_radiation.code_clad.get()                     == var_code) var_abbrv = STD_VD_net_radiation                      .abbreviation; //070204
*/
#if (CS_VERSION<6)
   if (STD_VD_solar_radiation_ERRONEOUS.code_clad.get()                   == var_code) var_abbrv = STD_VD_solar_radiation_ERRONEOUS                    .abbreviation;
   if (STD_VD_isothermal_longwave_net_radiation_ERRONEOUS.code_clad.get() == var_code) var_abbrv = STD_VD_isothermal_longwave_net_radiation_ERRONEOUS  .abbreviation;  //070204
   if (STD_VD_net_radiation_ERRONEOUS.code_clad.get()                     == var_code) var_abbrv = STD_VD_net_radiation_ERRONEOUS                      .abbreviation;  //070204
#endif
//170926 obsolete #endif
   if (STD_VD_solar_radiation_avg_hourly        .code_clad.get()== var_code) var_abbrv = STD_VD_solar_radiation_avg_hourly         .abbreviation;//131013
   if (STD_VD_sunshine_hours                    .code_clad.get()== var_code) var_abbrv = STD_VD_sunshine_hours                     .abbreviation;//131013
   if (STD_VD_wind_speed                        .code_clad.get()== var_code) var_abbrv = STD_VD_wind_speed                         .abbreviation;
   if (STD_VD_wind_direction                    .code_clad.get()== var_code) var_abbrv = STD_VD_wind_direction                     .abbreviation;
   if (STD_VD_pot_evapotranspiration            .code_clad.get()== var_code) var_abbrv = STD_VD_pot_evapotranspiration             .abbreviation;
   if (STD_VD_act_evapotranspiration            .code_clad.get()== var_code) var_abbrv = STD_VD_act_evapotranspiration             .abbreviation;
   if (STD_VD_pan_evaporation                   .code_clad.get()== var_code) var_abbrv = STD_VD_pan_evaporation                    .abbreviation;
   if (STD_VD_ref_ET_Penman_Monteith            .code_clad.get()== var_code) var_abbrv = STD_VD_ref_ET_Penman_Monteith             .abbreviation;
   if (STD_VD_ref_ET_Priestly_Taylor            .code_clad.get()== var_code) var_abbrv = STD_VD_ref_ET_Priestly_Taylor             .abbreviation;
   if (STD_VD_ref_ET_Hargreaves                 .code_clad.get()== var_code) var_abbrv = STD_VD_ref_ET_Hargreaves                  .abbreviation;
   if (STD_VD_ref_ET_ASCE_EWRI_short_crop       .code_clad.get()== var_code) var_abbrv = STD_VD_ref_ET_ASCE_EWRI_short_crop        .abbreviation; //091005
   if (STD_VD_ref_ET_ASCE_EWRI_tall_crop        .code_clad.get()== var_code) var_abbrv = STD_VD_ref_ET_ASCE_EWRI_tall_crop         .abbreviation; //091005
   if (STD_VD_avg_daytime_vapor_pressure_deficit.code_clad.get()== var_code) var_abbrv = STD_VD_avg_daytime_vapor_pressure_deficit .abbreviation;
   if (STD_VD_avg_fullday_vapor_pressure_deficit.code_clad.get()== var_code) var_abbrv = STD_VD_avg_daytime_vapor_pressure_deficit .abbreviation;
   if (STD_VD_max_vapor_pressure_deficit        .code_clad.get()== var_code) var_abbrv = STD_VD_max_vapor_pressure_deficit         .abbreviation;
   if (STD_VD_cloud_cover                       .code_clad.get()== var_code) var_abbrv = STD_VD_cloud_cover                        .abbreviation;
   if (STD_VD_snow_fall                         .code_clad.get()== var_code) var_abbrv = STD_VD_snow_fall                          .abbreviation;
   if (STD_VD_snow_depth                        .code_clad.get()== var_code) var_abbrv = STD_VD_snow_depth                         .abbreviation;
   return var_abbrv;
}
//_standard_variable_abbreviation___________________________________2002-09-07_/
}//_namespace UED______________________________________________________________/


