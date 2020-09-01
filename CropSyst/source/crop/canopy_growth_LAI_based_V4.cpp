#include "crop/canopy_growth_LAI_based_V4.h"

namespace CropSyst {
//______________________________________________________________________________
Canopy_growth_leaf_area_index_based_V4::Canopy_growth_leaf_area_index_based_V4
(const Crop_parameters                                   &crop_parameters_
,const Crop_parameters_struct::Canopy_growth_LAI_based   &parameters_
/*190628
,const Thermal_time_immutable       &_thermal_time                               //151106
*/
,Residues_interface        *residues_
,Crop_nitrogen_interface   *nitrogen_
,float64                   &reported_peak_LAI_
,Crop_CO2_response         *CO2_response_
//190630 ,bool                       is_continuous_grass_
)
:Canopy_growth_leaf_area_index_based
(crop_parameters_
,parameters_
/*190620
,_thermal_time
*/
,residues_
,nitrogen_
,reported_peak_LAI_
,CO2_response_
//190630 ,is_continuous_grass_
)
{}
//_Canopy_growth_leaf_area_index_based_V4:constructor_______________2013-06-10_/
}//_namespace CropSyst_________________________________________________________/


