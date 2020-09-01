#ifdef USE_PCH
#  include "simulation/CropSyst_sim_pch.h"
#else
#  include <math.h>
#  include "corn/math/moremath.h"
#  ifndef compareHPP
#     include "corn/math/compare.hpp"
#  endif
#  include "cs_glob.h"
#  include "soil/runoff.h"
#  include "crop/growth_stages.hpp"
#endif

// Eventually change changes this namespace to USDA NRCS

namespace CropSyst {
//______________________________________________________________________________
Soil_runoff_common::Soil_runoff_common
(Soil_base &soil_)                                                               //200316
: soil                                                                   (soil_) //200316
, crop_land_use                                                         (FALLOW) //190718
, act_crop_storage                                                         (0.0) //990528
, act_residue_storage                                                      (0.0) //990528
, random_roughness_m                                              (cm_to_m(0.6)) //990422
, estimated_runoff                                                           (0)
{}
//_Soil_runoff_common:constructor___________________________________2014-12-01_/
float64 pot_crop_storage[5][4] =// mm
{//notreat, straight, contour, terrace
 { 1.0,      1.0    ,  1.0   , 1.0}       // fallow
,{ 1.0,      2.1    ,  3.2   , 4.4}       // row
,{ 1.0,      2.7    ,  3.8   , 4.4}       // small grain
,{ 1.0,      3.2    ,  3.8   , 5.0}       // close seed
,{ 1.0,     60.0    , 60.0   ,60.0}       // pasture
                  // Others ignored at this time
};
//_pot_crop_storage_________________________________________________1999-05-28_/
float64 Soil_runoff_common::calculate_crop_storage
(Land_treatment  land_treatment
, float64 fract_canopy_cover)
{  act_crop_storage =
      (crop_land_use == PASTURE) ? mm_to_m(60.0) :  // <- currently redundent
      mm_to_m(pot_crop_storage[crop_land_use][land_treatment])* fract_canopy_cover;
   return  act_crop_storage;
}
//_calculate_crop_storage___________________________________________1999-05-28_/
float64 Soil_runoff_common::calculate_residue_storage(float64 fract_residue_cover)
{  act_residue_storage = mm_to_m(60.0) * fract_residue_cover;
   return act_residue_storage ;
}
//_calculate_residue_storage________________________________________1990-05-28_/
float64 Soil_runoff_common::maximum_depression_storage()
{
   // Used in numerical runoff model.
   // called every day  when storage is needed
   float64 random_roughness_cm = m_to_cm(random_roughness_m);
   float64 slope = soil.parameters_ownable->get_steepness_percent();
   float64 term1 = 0.112 * random_roughness_cm;
   float64 term2 = 0.031 * CORN_sqr(random_roughness_cm);
   float64 term3 = 0.012 * random_roughness_cm * slope;
   float64 max_depression_storage_m = cm_to_m((term1  + term2- term3));
   return CORN::must_be_greater_or_equal_to<float64>
      (max_depression_storage_m,0.001);                                          //990512
}
//_maximum_depression_storage_______________________________________1999-04-22_/
void Soil_runoff_common::adjust_random_roughness
(float64 effective_precipitation_m
,float64 fract_canopy_cover
,float64 fract_residue_cover)
{  if (effective_precipitation_m > 0.0)
   {  float64 random_roughness_ratio = 0.89
      * exp(-0.026 * (m_to_cm(effective_precipitation_m)
            * (1.0-fract_canopy_cover) * (1.0 -fract_residue_cover)));
      random_roughness_m *= random_roughness_ratio;
   }
}
//_adjust_random_roughness__________________________________________1999-04-22_/
}//_namespace CropSyst_________________________________________________________/

