#  include "UED/library/varcodes.h"
#  include <string.h>
namespace UED {
//______________________________________________________________________________
const char *Variable_code_qualifier_descriptions[UED_qualifier_count] =
{"at_time_step"         //UED_at_time_step
,"time_step_avg"        //UED_average_for_time_step
,"time_step_max"        //  UED_maximum_for_time_step
,"time_step_min "       //  UED_minimum_for_time_step
,"std_dev_time_step"    //  UED_standard_deviation_for_time_step
,"time_step_sum"        //    UED_total_for_time_step
,"accum_to_date_time"   //  UED_accumulation_to_date_time
,"period_accum"         // deprecated, use period_sum
,"daily_contrib"
,"period_avg"
,"period_max"
,"period_min"
,"period_std_dev"
,"period_sum"
,"period_init"                                                                   //150309
// Up to 127 qualifiers can be defined
};
//_Variable_code_qualifier_descriptions________________________________________/
Physical_property_ontology
physical_property_ontology[UED_physical_prop_descr_count] =
{  { UED_variableless                              ,"No variable" }
,  { UED_climate_air_temperature                   ,"meteorology/temperature/air"}
,  { UED_climate_precipitation                     ,"meteorology/precipitation/liquid"}
,  { UED_climate_snow_fall                         ,"meteorology/precipitation/solid"} // snow fall
,  { UED_climate_snow_depth                        ,"meteorology/precipitation/snow/depth"} // Climate snow depth"}
,  { UED_climate_solar_radiation                   ,"meteorology/solar/radiation/shortwave"}
,  { UED_climate_isothermal_longwave_net_radiation ,"meteorology/solar/radiation/longwave/net"} //"Climate isothermal longwave net radiation"}
,  { UED_climate_net_radiation                     ,"meteorology/solar/radiation/net"}
,  { UED_climate_pan_evaporation                   ,"biometeorology/evaporation/pan"}
,  { UED_climate_dew_point_temperature             ,"meteorology/temperature/dewpoint"}
,  { UED_climate_relative_humidity                 ,"meteorology/humidity/relative"}
,  { UED_climate_act_evapotranspiration            ,"biometeorology/evapotranspiration/actual"}
,  { UED_climate_pot_evapotranspiration            ,"biometeorology/evapotranspiration/potential"}
,  { UED_climate_cloud_cover                       ,"meteorology/cloud/cover"}
,  { UED_climate_vapor_pressure                    ,"meteorology/vapor/pressure"}
,  { UED_climate_vapor_pressure_deficit            ,"meteorology/vapor/pressure/deficit"}
,  { UED_climate_wind_speed                        ,"meteorology/wind/velocity"}
,  { UED_soil_temperature                          ,"soil/temperature"}
,  { UED_soil_thickness                            ,"soil/thickness"}

// NYI meteorology/humidity/specific

// Many more to be defined
};
//_variable_ontology________________________________________________2020-05-05_/
/*200505 obsolete, now use ontology
Physical_property_descriptions[UED_physical_prop_descr_count] =
{  { UED_variableless               ,"No variable" }
,  { UED_climate_air_temperature    ,"Climate air temperature"}
,  { UED_climate_precipitation      ,"Climate Precipitation"}
,  { UED_climate_snow_fall          ,"Climate snow fall"}
,  { UED_climate_snow_depth         ,"Climate snow depth"}
,  { UED_climate_solar_radiation    ,"Climate solar radiation"}
,  { UED_climate_isothermal_longwave_net_radiation   ,"Climate isothermal longwave net radiation"}
,  { UED_climate_net_radiation    ,"Climate net radiation"}
,  { UED_climate_pan_evaporation    ,"Climate pan evaporation"}
,  { UED_climate_dew_point_temperature    ,"Climate dew point temperature"}
,  { UED_climate_relative_humidity    ,"Climate relative humidity"}
,  { UED_climate_act_evapotranspiration    ,"Climate act. evapotranspiration"}
,  { UED_climate_pot_evapotranspiration    ,"Climate pot. evapotranspiration"}
,  { UED_climate_cloud_cover    ,"Climate cloud cover"}
,  { UED_climate_vapor_pressure    ,"Climate vapor pressure"}
,  { UED_climate_vapor_pressure_deficit    ,"Climate vapor pressure deficit"}
,  { UED_climate_wind_speed    ,"Climate wind speed"}
,  { UED_soil_temperature    ,"Soil temperature"}
,  { UED_soil_thickness    ,"Soil thickness"}

// NYI ,  { UED_climate_relative_humidity    ,"Climate humidity specific"}

//,  { UED_climate_pot_evapotranspiration    ,"Climate pot. transpiration"}

// Many more to be defined
};
//_Physical_property_descriptions______________________________________________/
*/


Variable_code_clad::Variable_code_clad(Variable_code code_)
: Variable_code_cowl(code)
, code(code_)
{}
//_Variable_code_clad:constructor______________________________________________
Variable_code_clad::Variable_code_clad
(nat32  physical_property_code
,nat8   qualifier
,bool    non_standard
)
: Variable_code_cowl(code)
, code(  ((nat32)non_standard << 31)
       | ((nat32)qualifier    << 24)
       | (nat32)physical_property_code)
{}
//_Variable_code_clad:constructor______________________________________________/
const char *Variable_code_cowl::label_string(std::string &result)          const
{
   result = is_standard()
      ? "Unidentified standard UED variable"
      : "Non-standard UED_variable";
   nat32 varcode_phys_prop = get_physical_property();
   for (int i = 0 ; i < UED_physical_prop_descr_count; i++)
   {     nat32 physprop_i =
            physical_property_ontology[i].physical_property;                     //200505
            //200505 Physical_property_descriptions[i].physical_property;
         if (physprop_i == varcode_phys_prop)
         {
           result +=
            physical_property_ontology[i].ontology;                              //200505
            //200505 Physical_property_descriptions[i].description;
           result +=":";
           nat8 qualifier_index = get_qualifier();
           result += (qualifier_index < UED_qualifier_count)
              ? Variable_code_qualifier_descriptions[qualifier_index]
              : "Unknown qualification code";
           return result.c_str();
       }
   }
   // not yet implemented  Give a general description based on bit coding if no description
   return result.c_str();
}
//_label_string_____________________________________________________2002-01-26_/
}//_namespace UED______________________________________________________________/

