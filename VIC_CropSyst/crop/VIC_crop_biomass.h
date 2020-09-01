#ifndef VIC_crop_biomassH
#define VIC_crop_biomassH

#if (VIC_CROPSYST_VERSION >= 3)
#  error This class is obsolete in version 3
/*
Version 3 now uses VIC_CropSyst_proper_crop
where the Crop class is based on CropSyst proper.
*/
#else

#ifndef primitiveH
#  include <corn/primitive.h>
#endif
#ifndef constH
#  include <corn/const.h>
#endif
#include "VIC_crop_param.h"
/*140810 #include "VIC_crop_common.h"
#ifndef VIC_crop_commonH
#  include "VIC_crop_common.h"
#endif
*/
//140815 not used class Weather_provider;
class Air_temperature_average;
class Solar_radiation;
class Vapor_pressure_deficit_daytime;
class Transpiration_interface;
namespace CropSyst
{
class Phenology_interface;
class Crop_CO2_response;                                                         //110903
}
namespace CropSyst                                                               //110903
{
   class Canopy_cover_abstract;                                                  //110903
   class Canopy_cover_reference;                                                 //110903
   class Canopy_cover_actual;                                                    //110903
}
namespace VIC_crop
{
class Transpiration_abstract;
#include "crop/canopy_biomass_continuum.h"

//_____________________________________________________________________________/
} // namespace VIC_crop
#endif
#endif

