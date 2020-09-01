#include "soil/infiltration_cascade_common.h"
#include "corn/math/moremath.h"
#include "corn/math/compare.h"
#include "common/physics/water_depth.h"
namespace Soil {                                                                 //181206
//______________________________________________________________________________
Infiltration_cascade_common::Infiltration_cascade_common
(const Layers_interface                &_soil_layers                             //140424
,const Hydraulic_properties_interface  &_soil_hydraulic_properties               //140423
,      Hydrology_interface             &_hydrology
,CropSyst::Soil_runoff     *_runoff                                              //130308
,CropSyst::Pond            &_pond)                                               //130308
: Infiltration_abstract                                                          //130308
   (_soil_layers                                                                 //140424
   ,_soil_hydraulic_properties                                                   //140423
   ,_hydrology
   ,_runoff
   ,_pond)
, hydrology(_hydrology)                                                          //060504
{  clear_layer_array64(water_flow);                                              //080208
}
//_____________________________________________________________________________/
}//_namespace_Soil______________________________________________________________

