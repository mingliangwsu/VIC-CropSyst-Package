#ifndef model_optionsH
#define model_optionsH

#include "options.h"
#include "common/weather/weather_types.h"
#include "common/weather/parameter/WC_vapor_pressure_deficit.h"
#include "corn/chronometry/date_types.hpp"
#include "corn/parameters/parameter.h"
#include "corn/OS/directory_entry_name_concrete.h"
#ifndef CROP_TYPES_H
#  include "crop/crop_types.hpp"
#endif
#include "common/biometeorology/ET_model.h"
#include "common/biometeorology/ET_reference_Priestley_Taylor.h"

namespace CORN {class Data_record;}

using namespace CORN;

#define  LABEL_simulation           "simulation"
#define     LABEL_starting_date        "starting_date"
#define     LABEL_ending_date          "ending_date"

#define     LABEL_infiltration_model   "infiltration_model"
#define        LABEL_cascade              "cascade"
#define        LABEL_finite_diff          "finite_diff"

#define     LABEL_curve_number         "curve_number"
#define     LABEL_numerical            "numerical"

#define LABEL_output                "output"
#define    LABEL_runtime_graph        "run_time_graph"
#define    LABEL_display_profile      "display_profile"
#define    LABEL_report_format        "report_format"

#define LABEL_optimization_output   "optimization_output"
#define     LABEL_total_grazed_biomass "total_grazed_biomass"
#define     LABEL_carbon_sequestered   "carbon_sequestered"

#define LABEL_submodels             "submodels"
#define    LABEL_run_nitrogen         "run_nitrogen"
#define    LABEL_run_chemical         "run_chemical"
#define    LABEL_run_erosion          "run_erosion"
#define    LABEL_run_freezing         "run_freezing"
#define    LABEL_erode_soil           "erode_soil"
#define    LABEL_hourly               "hourly"
#define    LABEL_automatic_irrigation_disable "automatic_irrigation_disable"

#define    LABEL_runoff_model         "runoff_model"
#define    LABEL_salinity             "salinity"
#define    LABEL_snow_pack_model      "snow_pack_model"

#define LABEL_CO2                   "CO2"
#define    LABEL_simulate_CO2_OLD         "simulate_CO2"
#define    LABEL_enable                  "enable"
#define    LABEL_annual_CO2_change    "annual_CO2_change"
#define    LABEL_initial_CO2_conc     "initial_CO2_conc"
#define     LABEL_leaching_observation_depth_mode "leaching_observation_depth_mode"
//______________________________________________________________________________
enum Runoff_model {NO_RUNOFF_MODEL,CURVE_NUMBER_RUNOFF_MODEL,NUMERICAL_RUNOFF_MODEL};
extern const char *runoff_model_label_table[];
DECLARE_ENUM_CLAD(Runoff_model_clad,Runoff_model,runoff_model_label_table)
//______________________________________________________2002-03-24_1999-02-20_/
enum Rain_event_distribution {REALISIC_RAIN_EVENT,EVEN_RAIN_EVENT};
extern const char *rain_event_distribution_label_table[];
DECLARE_ENUM_CLAD(Rain_event_distribution_clad,Rain_event_distribution,rain_event_distribution_label_table)
//_______________________________________________________2008-02-01_2002-03-24_/
enum Freezing_model{JUMAKIS_FREEZING,NUMERICAL_FREEZING};
extern const char *freezing_model_label_table[];
DECLARE_ENUM_CLAD(Freezing_model_clad,Freezing_model,freezing_model_label_table)
//_______________________________________________________1999-03-15_2002-03-24_/
enum Infiltration_model{NO_INFILTRATION,CASCADE_DAILY_INFILTRATION,FINITE_DIFFERENCE_INFILTRATION,CASCADE_HOURLY_INFILTRATION,NO_LIMITATION};
extern const char *infiltration_model_label_table[];
DECLARE_ENUM_CLAD(Infiltration_model_clad,Infiltration_model,infiltration_model_label_table)
//_______________________________________________________2002-03-24_1999-02-11_/
enum Leaching_observation_depth_mode{LEACH_BOTTOM_OF_PROFILE,LEACH_AT_SPECIFIED_DEPTH};
extern const char *leach_observation_depth_mode_label_table[];
DECLARE_ENUM_CLAD(Leaching_observation_depth_mode_clad,Leaching_observation_depth_mode,leach_observation_depth_mode_label_table);
//__________________________________________________________________2002-05-19_/
enum Organic_matter_and_residue_model
   {NO_ORGANIC_MATTER_AND_RESIDUE
   ,single_SOM_multiple_residues
   ,multiple_SOM_multiple_residues
   #if ((((CROPSYST_VERSION >0) && (CROPSYST_VERSION < 5)) || ((CS_VERSION >0) && (CS_VERSION < 5)) ))
   ,V4_1_ORGANIC_MATTER_AND_RESIDUE
   ,V4_4_ORGANIC_MATTER_AND_RESIDUE
   ,V4_9_ORGANIC_MATTER_AND_RESIDUE
   #endif
};
extern const char *organic_matter_and_residue_model_label_table[];
DECLARE_ENUM_CLAD(Organic_matter_and_residue_model_clad,Organic_matter_and_residue_model,organic_matter_and_residue_model_label_table);
//__________________________________________________________________2005-07-26_/
enum Orchard_solid_interception_model {none_orchard_interception,Kemanian_Stockle_orchard_interception,Oyarzun_Stockle_orchard_interception};
extern const char *orchard_solid_interception_model_label_table[];
DECLARE_ENUM_CLAD(Orchard_solid_interception_model_clad,Orchard_solid_interception_model,orchard_solid_interception_model_label_table);
//_______________________________________________________2007-11-16 2005-07-26_/
enum Moisture_characterization {specific,dry,average,wet,saturated};
extern const char *moisture_characterization_label_table[];
DECLARE_ENUM_CLAD(Moisture_characterization_clad,Moisture_characterization,moisture_characterization_label_table);
//_______________________________________________________2007-11-16 2005-07-26_/
class CropSyst_Model_options
{
 public:
   struct Observation
   {
      float32 H2O_depth;
      float32 N_depth;
      float32 leaching_depth; // this is used for selecting leaching layer   was leaching_observation_depth //040714
   } observation;
 public:  // output options
   bool fast_graph;// Output data for fast graph display
   bool display_profile;    // Display runtime profile                           //000406
   bool minimize_runtime_window;                                                 //040609
   #if ((CS_VERSION >= 1) && (CS_VERSION < 5))
   CORN::OS::File_name_concrete
      report_format_filename_rel;                                                //030406
   #endif
   // This may actually be cropsyst specific
 public:  // submodel options
   bool run_nitrogen; // Do nitrogen computations?
   bool run_chemical; // Do chemical computations?
   bool run_salinity; // Do salinity computations?
   bool run_erosion ; // Do erosion computations?
   bool run_freezing; // Do freezing simulation?                                 //990618
   bool erode_soil;   // Remove eroded soil at the end of the year?              //971117

   bool subdivide_horizons;  // split the soil horizons into sublayers           //060229
   nat32 hourly_not_in_UI;  // Some models can be run with hourly resolution     //041104
   #define HOURLY_crop_thermal_time         0x00000001
   #define HOURLY_soil_temperature          0x00000002
   #define HOURLY_nitrogen_transformations  0x00000004
   #define HOURLY_evaporation               0x00000070
   #define HOURLY_evaporation_soil          0x00000010
   #define HOURLY_evaporation_pond          0x00000020
   #define HOURLY_evaporation_residue       0x00000040
   Organic_matter_and_residue_model_clad  organic_matter_and_residue_model_clad; //050726
   Infiltration_model_clad       infiltration_model_clad;                        //990213
   Runoff_model_clad             runoff_model_clad;                              //990220
   Rain_event_distribution_clad  rain_event_distribution_clad;                   //080201
   Snow_pack_model_clad          snow_pack_model_clad;                           //991004
   bool                          extend_available_weather;                       //150218
   std::string equilibration;                                                    //170414
      // The unqualified name of equilibration directory,
      // if specified, attempts to compose the equilibrated
      // filename from a directory found in a parallel scenario path.
      // The program will fail if unable to find the equilibrated file.
      // Currently only equilibrated file is equilibrated.organic_matter file.

   int16                      substitute_year_base;                              //170323
   int8                       substitute_year_offset;                            //161207
      // substitute_year_offset added for FlexCropping but could be generally useful //161207
   CORN::date32               weather_preclude_date;                             //170310
      // Date afterw hich assume there is no more available weather data
      // (even though there is such provided date)
      // 0 indicates disable this feature.
      // Added for FlexCropping to be used with substitute year offset
      // to force when weather substitution occurs.
      // but probably not generally use for (needed for Harsimran's project
   int16 default_event_duration_minutes;
      Parameter_number p_default_event_duration_minutes;                         //170604
                           // Eventually we may want to have a default event start time
                           // (current defaults to midnight)
   bool CPF_irrigation_disable; // used for crop production function simulation  (used to be only automatic irrigation, now all irrigation //081118

   Leaching_observation_depth_mode_clad leaching_observation_depth_mode_clad;    //020519

   float32               leaching_observation_depth;  // m This is used for selecting leaching layer  //040714
      // Move leaching_observation_depth to observation

   Parameter_number      *p_leaching_observation_depth;
      // Needs to be a pointer because this is only used for parameter editor,
      // During the run we make a copy of these options
      // but there is no copy constructor for Valid_float32

   float32 soil_profile_depth_limit;                                             //170609
      Parameter_number p_soil_profile_depth_limit;                               //170612
      // meter,  0.0 indicates no limit.

   Orchard_solid_interception_model_clad  orchard_solid_interception_model_clad; //071116
   #if ((CROPSYST_VERSION >= 1) && (CROPSYST_VERSION < 5))
   Vapor_pressure_deficit_max_calculator::Parameters VPDmax_parameters;          //170804
   LOC_ET_Model_clad                      evapotranspiration_model_clad;         //051230
   CS::Priestley_Taylor_Parameters           Priestley_Taylor_parameters;        //170808
   #else
   // In V5 the user weather is expected to have ETref
   #endif
   CORN::OS::File_name_concrete               output_options_filename_rel;       //041007
 public: // CO2
   bool      simulate_CO2;
   float32  annual_CO2_change;
   float32  initial_CO2_conc;    Parameter_number p_initial_CO2_conc;
 public: // optimization_output
   bool  total_grazed_biomass;                                                   //040812
   bool  carbon_sequestered;                                                     //040812
 public:
   nat16 FLEXCROPPING_AW_DOY;
      // Day to print available water
      // This is temporary for special output for FlexCropping.
      // Eventually I will provide a general mechanism for recording
      // output on distinguished date
 public: // structors
   CropSyst_Model_options();
   virtual ~CropSyst_Model_options();
 public:
   virtual bool setup_structure
      (CORN::Data_record &data_rec,bool for_write)                modification_;
   #if ((CROPSYST_VERSION >= 1) && (CROPSYST_VERSION < 5))
   virtual bool get_end()                                         modification_; //170811
   #endif
 public:
   virtual void copy(const CropSyst_Model_options &from)          modification_;
   bool requires_hourly_soil_profile_temperature()                        const; //060405
};
//_CropSyst_Model_options______________________________________________________/
#endif

