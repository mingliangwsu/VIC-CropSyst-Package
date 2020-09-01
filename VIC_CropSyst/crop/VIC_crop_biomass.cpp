#if (VIC_CROPSYST_VERSION >= 3)
#  error This class is obsolete in version 3 remove this file from the project
/*
Version 3 now uses VIC_CropSyst_proper_crop
where the Crop class is based on CropSyst proper.
*/
#else
#ifndef compareHPP
#  include <corn/math/compare.hpp>
#endif
#include <stdlib.h>
#include <algorithm>
#include "corn/measure/measures.h"
#include "VIC_crop_biomass.h"
//150319obs #include "VIC_crop_canopy_growth.h"
#include "crop/canopy_cover_continuum.h"
#if (!VB_VERSION_DATE || VB_VERSION_DATE>=20110110)
#  include "VIC_crop_transpiration.h"
#else
#  include "VIC_crop_transpiration_old.h"
#endif
#include "VIC_crop_phenology.h"
#include "crop/CO2_response.h"
#ifndef constH
#  include <corn/const.h>
#endif
//140815  not used #  include "common/weather/weather_provider.h"
#  include "weather/parameter/WP_air_temperature.h"
#  include "weather/parameter/WP_solar_radiation.h"
#  include "weather/parameter/WP_vapor_pressure_deficit.h"
#  include "UED/library/std_codes.h"
#include <math.h>
class Air_temperature_average;                 //140815
class Solar_radiation;                         //140815
class Vapor_pressure_deficit_daytime;                             //140815

//_____________________________________________________________________________/
namespace VIC_crop
{  // weather_crop_interface must be included within the VIC_crop namespace
#include "crop/canopy_biomass_continuum.cpp"
} // namespace VIC_crop
#endif

