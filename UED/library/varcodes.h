#ifndef varcodesH
#define varcodesH

#include "corn/const.h"
#include "UED/library/UED_types.h"

#define UED_qualifier_count 15
#define UED_physical_prop_descr_count 19
#define UED_known_std_var_ranges_count 6

#define UED_variableless             0       // 00000000 00000000 00000000b 0000x

// Physical properties codes:
// The codes usually require composition with a qualifier!

#define UED_temporal                       0x000001
// Note, I need a better code for UED_temporal

//NYN #define UED_station_ID                     0x00000001
// This is a special code for UED convertor import where a column

//  Climate                                                       000

#define UED_climate_air_temperature        0x870001 // 10000111 00000000 00000001
#define UED_climate_dew_point_temperature  0x870002 // 10000111 00000000 00000010

#define UED_climate_precipitation          0x820001 // 10000010 00000000 00000001
#define UED_climate_snow_fall              0x820002 // 10000010 00000000 00000010
// Snow fall in depth of snow (typically 10cm snowfall = 1cm precipitation)
// usually measured in a time step
#define UED_climate_snow_depth             0x820003 // 10000010 00000000 00000011 actually thickness of snow pack

#define UED_climate_relative_humidity      0x830001 // 10000011 00000000 00000001
   //180808 rename to UED_climate_humidity_relative

#define UED_climate_act_evapotranspiration 0x830002 // 10000011 00000000 00000010
#define UED_climate_pot_evapotranspiration 0x830003 // 10000011 00000000 00000011

// Ref ET computed from various methods 830004 0x83000F
#define UED_climate_ref_ET_Penman_Monteith      0x830004
#define UED_climate_ref_ET_Priestly_Taylor      0x830005
#define UED_climate_ref_ET_Hargreaves           0x830006
#define UED_climate_ref_ET_ASCE_EWRI_short_crop 0x830007
#define UED_climate_ref_ET_ASCE_EWRI_tall_crop  0x830008

#define UED_climate_cloud_cover            0x830010 // 10000011 00000000 00000100

#define UED_climate_humidity_specific      0x830011 // 10000011 00000000 00000101 //180808

// Evaporation
#define UED_climate_pan_evaporation        0x830402 // 10000011 00000100 00100001

// Vapor Pressure
#define UED_climate_vapor_pressure         0x830600 // 10000011 00001000 00000000
// Vapor pressure deficit
#define UED_climate_vapor_pressure_actual 0x830601 // 10000011 00001000 00000001

#define UED_climate_vapor_pressure_deficit         0x830604 // 10000011 00001000 00000100

#if (( CS_VERSION_CODE>0x040000D) && ( CS_VERSION_CODE<0x040E0D))
// I was previously erroneously storing fullday and daytime in the same variable
#define UED_climate_vapor_pressure_deficit_fullday UED_climate_vapor_pressure_deficit
#define UED_climate_vapor_pressure_deficit_daytime UED_climate_vapor_pressure_deficit
#else
#define UED_climate_vapor_pressure_deficit_fullday 0x830605 // 10000011 00001000 00000101
   //   unused                                     0x830606
#define UED_climate_vapor_pressure_deficit_daytime 0x830607 // 10000011 00001000 00000111
#endif

#define UED_climate_atmospheric_pressure 0x830700 // 10000011 00000111 00000000

// Snow accumulated on the ground, usually measured at a time step
// I need to find out exactly what kind of evaporation it is we work with
#define UED_climate_solar_radiation        0x860201 // 10000110 00000010 00000001
#define UED_climate_isothermal_longwave_net_radiation        0x860202 // 10000110 00000010 00000100
#define UED_climate_net_radiation        0x860203 // 10000110 00000010 00000011
//WARNING solar radiation (rename to irradiance) may be units per time
#define UED_climate_solar_radiation_hourly 0x860209

// UED_climate_solar_radiation_hourly is a special case where the daily value represented the average of the hourly values
#define UED_climate_sunshine_hours         0x86020A // 10000110 00000010 0000000A

#define UED_climate_wind_speed             0xA20001 // 10100010 00000000 00000001
#define UED_climate_wind_direction         0xA20002 // 10100010 00000000 00000002
#define UED_climate_UAS                    0xA20003 // 10100010 00000000 00000003
#define UED_climate_VAS                    0xA20004 // 10100010 00000000 00000004

// Soil
#define UED_soil_temperature               0x870401 // 10000111 00000100 00000001  980904     100001110000010000000001

#define UED_soil_thickness                 0x810403 // 10000001 00000100 00000011  980904

// Biosphere    2  010
// Residue evaporation
// plant transpiration

// variable code qualifiers should now match CORN statistics types

// The following are obsolete variable code qualifiers  190423
/*
 7  6  5  4  3  2  1  0
31 30 29 28 27 26 25 24
st ly ----qualifier----
      /rsvd\/statistic\
       x  x  0  0  0  0 UED_at_time_step
       x  x  0  0  0  1 UED_average_for_time_step
       x  x  0  0  1  0 UED_maximum_for_time_step
       x  x  0  0  1  1 UED_minimum_for_time_step
       x  x  0  1  0  0 UED_standard_deviation_for_time_step
       x  x  0  1  0  1 UED_total_for_time_step
       x  x  0  1  1  0 UED_accumulation_to_date_time
       x  x  0  1  1  1 UED_accumulation_for_period_deprecated
*/

//NYI #if (CS_VERSION >= 5)
/*NYI
#error Conceptual

150309
compose these codes using CORN Statistic enumeration
4bits for the statistic, 1 bit for timestep/period indicator
(and reserve 1 for future use
#else
*/

#define UED_at_time_step        0
// Exactly at the time step
// Also called instantaneous

#define UED_average_for_time_step 1
#define UED_time_step_avg         1
// Average value for the time step.

#define UED_maximum_for_time_step 2
#define UED_time_step_max         2
// Max average value for the time step.

#define UED_minimum_for_time_step 3
#define UED_time_step_min         3
// Min average value for the time step.

#define UED_standard_deviation_for_time_step 4
#define UED_std_dev_time_step     4
// Standard deviation for the time step.

#define UED_total_for_time_step   5
#define UED_time_step_sum         5

// The following accumulation are accumulation for a period of time matching a record period
// For example the sum of a record's set of values
#define UED_accumulation_to_date_time 6
#define UED_accum_to_date             6
#define UED_accum_to_time             6

// Accumulation to date/time.

#define UED_accumulation_for_period_deprecated    7
#define UED_period_accum_deprecated              7

// This is replaced with UED_period_sum
// Accumulation for a period of time not matching the record time period
// For example accumulation during a growing season.
// The period is application specific.

#define UED_daily_contribution      8
// This is for situations where there is an amount supplied for the time step
// but that may disappear from a total later on:
// I.e. biomass added to a plant during a day, or snow depth added to the snow pack
// There will usually be another variable showing the value "at the time step"
// I.e. the biomass of the plant observed at a given day, or the current snow pack depth

#define UED_average_for_period  9
#define UED_period_avg  9
//This is for running average at a time step  (I.e. CropSyst average nitrogen stress)
#define UED_period_max              10
#define UED_period_min              11
#define UED_period_std_dev          12
#define UED_period_sum              13
#define UED_period_init             14
// Stats and accumulated for a period of time not matching the record time period
// For example accumulation during a growing season.
// The period is application specific.



namespace UED {
//______________________________________________________________________________
// Standard variable physical properties codes
/*200505
struct Physical_property_descr
{  nat32 physical_property;   // Only 24bit used
   const char *description;
};
//______________________________________________________________________________
*/
struct Physical_property_ontology
{  nat32 physical_property;   // Only 24bit used
   const char *ontology;
};
//______________________________________________________________________________

extern const char *Variable_code_qualifier_descriptions[UED_qualifier_count];
//______________________________________________________________________________
class Variable_code_cowl
{
 // This class encapsulates UED_variable_code allowing independent
 // access to the bit field coding of the variable code and can compose
 // descriptions of the standard variables.
 private:
   Variable_code &code_ref;
 public:
   inline Variable_code_cowl(Variable_code &code_)
      : code_ref(code_) {}
/*
   Variable_code_cowl
      (nat32  physical_property_code
      ,nat8   qualifier
      ,bool   non_standard = false);
*/
   inline nat32 get()                                 const { return code_ref; }
   inline nat32 set(Variable_code code_) modification_{ return code_ref=code_; }
   virtual const char *label_string(std::string &buffer)                  const; //170423
      // Composes a text description of the variable
 public:
   inline nat32 get_physical_property()                                    const
      { return
           is_layered_V1()
           ? ((code_ref & 0x00FFFF00) >> 8)
              : is_layered_V2()                                                  //160619
                ? (code_ref & 0x0000FFFF)
                : (code_ref & 0x00FFFFFF); }
   inline nat8 get_qualifier()                                             const //160619
      { return ((nat8)((code_ref >> 24) & 0x3F)) ; }
   inline nat32 set_qualifier(nat8 qualifier)                      modification_ //000518
      { code_ref = (code_ref & 0x00FFFFFF) | (nat32)((nat32)qualifier << (nat32)24); return code_ref;}
   inline nat32 set_standard()                                     modification_ //000518
      { code_ref = (code_ref & 0x7FFFFFFF); return code_ref;}
   inline nat32 set_non_standard()                                 modification_ //000518
      { code_ref = (code_ref & 0xFFFFFFFF) | 0x80000000; return code_ref;}
   inline bool is_standard()                                        affirmation_
      { return  (!(code_ref & 0x80000000)); }
   inline nat8 get_layer()                                          affirmation_ //980820
      { return is_layered_V2()                                                   //160705
         ? get_layer_V2()
         : get_layer_V1();
      }

   inline bool is_layered_V1()                                      affirmation_ //980819
      { return (code_ref & 0x40000000) > 0; }
   inline nat8 get_layer_V1()                                       affirmation_ //980820
      { return (nat8)(code_ref &0xFF); }
   inline nat32 get_without_layer_V1()                              affirmation_ //991220
      { return (nat32) is_layered_V1() ? (code_ref &0xFFFFFF00) : code_ref; }
   inline nat32 set_layer_V1(nat8 layer)                           modification_ //981001
      { code_ref = (0xFFFFFF00 & code_ref) | layer; return code_ref; }

   inline bool is_layered_V2()                                      affirmation_ //160619
      { return (code_ref & 0x00800000) > 0; }
   inline nat8 get_layer_V2()                                       affirmation_ //160619
      { return (nat8)(code_ref &0x00EF0000) >> 16; }
   inline nat32 get_without_layer_V2()                              affirmation_ //160619
      { return (nat32) is_layered_V2() ? (code_ref &0x0000FFFF) : code_ref; }
   inline nat32 set_layer_V2(nat8 layer)                            modification_
      { code_ref=(0xFF00FFFF&code_ref)|(layer << 16)|0x00800000;return code_ref;}//160619

   inline bool is_layered()                                         affirmation_ //160619
      { return is_layered_V1() || is_layered_V2(); }
   inline nat32 get_without_layer()                                 affirmation_ //160619
      { return (nat32) is_layered_V1() ? get_without_layer_V1() : get_without_layer_V2(); }
   inline nat32 set_layer(nat8 layer)                              modification_ //160620
      { return is_layered_V2() ? set_layer_V2(layer) : set_layer_V1(layer); }
};
//_Variable_code_cowl___________________________________2016-06-19__1998-06-24_/
class Variable_code_clad
: public Variable_code_cowl
{
 public:
   Variable_code code;
 public:
   Variable_code_clad(Variable_code code);
   Variable_code_clad
      (nat32  physical_property_code
      ,nat8   qualifier
      ,bool   non_standard = false);
};
//_Variable_code_clad_______________________________________________2016-06-19_/
}//_namespace UED______________________________________________________________/
#endif
//varcodes

