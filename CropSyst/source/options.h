#ifndef CS_OPTIONS_H
#define CS_OPTIONS_H

#ifdef CANMS_MODEL
#  include "CANMS/src/options.h"
#else

/*
   This file contains the compile options for CropSyst based modules.
   All CropSyst components include this file to identify the compile options.
*/

#define LABEL_CS_suite "CS_suite"

//    CROPSYST_VERSION is used to deprecate exclude code and
//    parameters that will be obsolete in subsequent versions

#include "CropSyst_version.h"

#if defined LADSS_MODE && unix
#  define LADSS_UNIX
#endif

// Note the distinction between LADSS_MODE and LADSS_UNIX:
// CropSyst is compiled for the Windows version with LADSS_MODE enabled
// This allows the code needed for LADSS to be check by the compiler,
// and also includes the LADSS database (implemented in Oracle under Solaris)
// to be emulated using dBase tables.
// The LADSS_MODE is only enabled for program debugging convenience,
// None of the LADSS_MODE code is actually used in the CropSyst Suite.
// LADSS_UNIX enables codes specifically for the UNIX version of LADSS.
// A number of submodels are excluded in the UNIX version that are
// never used for LADSS because they are not applicable.

//#define OLD_PHENOLOGY_SEQUENCER
#  if defined(CROPSYST)
//130828 #     define WSHEDMOD
// I have disabled Watershed model because we haven't used it recently
// I will need to setup USE_OS_FS in Scenario_directory
      // I want to rename WSHEDMOD to WATERSHED_MODEL
#     define NITROGEN
//NYI#     define PHOSPHORUS
#  endif

#  if (defined(LADSS_UNIX) || defined(linux))
// The watershed submodel is not needed for LADSS
   #undef WSHEDMOD
#  endif

#define DIRECTED_IRRIGATION 2020
// Eventually directed irrigation 2020 may be permenent
// 2014 and 2016 are abandoned

#if defined(CROPSYST) || defined(WSHEDMOD)
#  define RESIDUES
#  define EROSION
#  define SOIL_TEMPERATURE
#  define SOIL_FREEZING
#  define SOIL_INFILTRATION
#  define PESTICIDE_disabled
#  define SALINITY
#  define CROP_ROTATION
#  define CROP_ORCHARD
#  define RUNOFF
#  define MANAGEMENT 4
// At this time I haven't yet upgrade management to version 5
// which will replace tillage and residue with field operations
// and event periods with not have start and stop dates
// but rather simply start with a separate stop event
#  define WATERTABLE
#  ifndef unix
#     define PRINT_PROFILE
#     define DISPLAY_PROFILE
#  endif
#  define CO2_CHANGE
#  define SEED_EMERGENCE_MODEL
#endif

// FRUIT_CROP_MODEL is now obsolete, simply include crop_fruit in the project
// and instanciate it as desired

/*200122
#ifdef SOILR
#  define USE_CORN_TRANSLATOR_CLASS
#  define SOIL_INFILTRATION
#  define SOIL_FREEZING
#  define RESIDUES
#  define EROSION
#  define SOIL_TEMPERATURE
#  define GRASS_ONLY
#  define RUNOFF
#  define CONTINUOUS_CROP
#endif
*/
#endif

#ifdef VIC_CROPSYST_VERSION
#define NITROGEN
#define RESIDUES
#define MANAGEMENT 4
#define CROP_ROTATION
/*190613
#ifndef OLD_ORCHARD
#define OLD_ORCHARD
#endif
*/
#undef RUNOFF
#endif

#ifdef MICROBASIN_VERSION
#  define CROP_ROTATION
#  define RESIDUES
#  define NITROGEN
#  define MANAGEMENT 4
#endif

// Now setup options based on the above selections
#ifdef CROPSYST_VERSION
#include "cmmnopts.h"
#endif

// Some versions dont need some submodels or components that may have been setup by the common options

#ifdef LADSS_UNIX
#  undef CROP_ORCHARD
#endif

#if (CROPSYST_VERSION==5)
//110830 I do not want to use OLD orchard in V5 but the new orchard model is not fully implemented yet
//#  undef CROP_ORCHARD
#endif
//111108 Version 1 STATSGO is now obsolete,  may want to replace this with STATSGO2 database


#ifdef OFOOT_VERSION
#  undef EROSION
#  undef PRINT_PROFILE
#  undef SEED_EMERGENCE_MODEL
#  undef WSHEDMOD
#  undef STATSGO_DATABASE
#  undef DISPLAY_PROFILE
#  undef XLS_OUTPUT
#  undef RUSLE_DETAIL
#endif

#ifdef MICROBASIN_VERSION
//#  define OLD_TDG
// OLD_TDG is old transpiration dependent growth from V4. only temporary.
#  undef EROSION
#  undef SALINITY
#  undef WATERTABLE
#  undef SEED_EMERGENCE_MODEL
//#undef SOIL_INFILTRATION
//#  undef FINITE_DIFF
//190613 #  undef OLD_ORCHARD
#  undef CROP_ORCHARD
#  undef WSHEDMOD
#  undef STATSGO_DATABASE
#  undef PRINT_PROFILE
#  undef DISPLAY_PROFILE
#  undef XLS_OUTPUT
#  undef RUSLE_DETAIL
// Hourly thermal time is temporarily disabled
// until hourly weather is provided.  141105
// (Not sure if needed)
#  undef HOURLY_THERMAL_TIME
#  undef CO2_CHANGE
#endif

#ifdef VIC_CROPSYST_VERSION
/*190613
#  ifdef OLD_ORCHARD
#     define FRUIT_CROP_MODEL
#     define CROP_ORCHARD
#  endif
*/
#define CO2_CHANGE
#  undef EROSION
#  undef SALINITY
#  undef WATERTABLE
#  undef PRINT_PROFILE
#  undef DISPLAY_PROFILE
//#undef HOURLY_THERMAL_TIME
//#  undef FRUIT_CROP_MODEL
//#  undef OLD_ORCHARD
//170220LML #  undef CROP_ORCHARD
#  undef SEED_EMERGENCE_MODEL
//#undef FINITE_DIFF_OR_CASCADE
#  undef WSHEDMOD
#  undef STATSGO_DATABASE
#  undef XLS_OUTPUT
#  undef RUSLE_DETAIL
#  undef HOURLY_THERMAL_TIME
#  undef SOIL_TEMPERATURE
#endif
#if (defined(OFOOT_VERSION))
#  define USE_CS_SIMULATION_ROTATOR
#  define USE_CS_LAND_UNIT
#endif
#ifdef VIC_CROPSYST_VERSION
#  undef HOURLY_THERMAL_TIME
#endif
#endif


