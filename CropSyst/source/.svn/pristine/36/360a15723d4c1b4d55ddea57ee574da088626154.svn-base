#include "soil/hydrology_cascade.h"
#include "soil/soil_I.h"
#include "corn/math/compare.h"
#include "CS_suite/observation/CS_optation.h"
#include "csvc.h"
#include "CS_suite/observation/CS_inspector.h"
namespace Soil {                                                                 //181206
//______________________________________________________________________________
Hydrology_cascade::Hydrology_cascade
(const Layers_interface                &soil_layers_ref_
,const Hydraulic_properties_interface  &hydraulic_properties_ref_
,const Structure                       &soil_structure_)
: Hydrology_abstract
   (soil_layers_ref_,hydraulic_properties_ref_,soil_structure_)
{  clear_layer_array64(liquid_water_content);
}
//_Hydrology_cascade::constructor______________________________________________/
Hydrology_cascade::Hydrology_cascade
(const Hydrology_cascade &copy_from
,float32 fraction)
: Hydrology_abstract(copy_from,fraction)
{  copy_layer_array64(liquid_water_content,copy_from.liquid_water_content);
}
//_Hydrology_cascade::constructor___________________________________2014-06-10_/
float64 Hydrology_cascade
::get_liquid_water_content_volumetric(nat8 layer)                         const
{  float64 WC = liquid_water_content[layer];                                     //070824
   return WC;
}
//_get_liquid_water_content_volumetric_________________________________________/
float64 Hydrology_cascade
::set_water_content_volumetric_layer
(nat8 layer, float64 new_water_content)                           modification_
{  liquid_water_content[layer] = new_water_content;
   ice_water_content[layer] = 0;
   invalidate();                                                                 //130930
   return get_water_plus_ice_content_volumetric(layer);
}
//_set_water_content_volumetric_layer___________________2007-08-26__1999-03-15_/
float64 Hydrology_cascade
::calc_water_depth_profile(bool liquid_and_ice)                            const
{  float64 water_depth_profile = 0.0;
   for (nat8 i = 1 ; i <= soil_layers_ref.count(); i++)                          //980724
   {  float64 wc_sl = liquid_and_ice
            ? get_water_plus_ice_content_volumetric(i)                           //990316
            : get_liquid_water_content_volumetric(i);                            //990316
      water_depth_profile += wc_sl * soil_layers_ref.get_thickness_m(i);
   }
   return water_depth_profile;
}
//_calc_water_depth_profile_________________________________________1999-03-15_/
float64 Hydrology_cascade::extract_water
(float64 amounts_m[] ,nat8 start_layer)                  modification_
{  float64 act_total_extraction_m = 0.0;
   for (nat8 layer=start_layer;layer<=soil_layers_ref.count();layer++)
   {  float64 volumetric_amount
         = amounts_m[layer] / soil_layers_ref.get_thickness_m(layer);
      float64 liquid_WC = get_liquid_water_content_volumetric(layer);            //070813
      float64 limited_volumetric_amount                                          //070813
         = std::min<float64>(liquid_WC,volumetric_amount);
      liquid_water_content[layer] -= limited_volumetric_amount;                  //990316
      float64 act_extraction_layer_m = limited_volumetric_amount
         * soil_layers_ref.get_thickness_m(layer);                               //070813
      act_total_extraction_m +=  act_extraction_layer_m;
      /*std::clog<<"L:"<<(int)layer
               <<"\tact_extract_mm:"<<m_to_mm(act_extraction_layer_m)
               <<"\tTotal_act_extract_mm:"<<m_to_mm(act_total_extraction_m)
               <<std::endl;*/
   }
   invalidate();                                                                 //130930
   return act_total_extraction_m;
}
//_extract_water____________________________________________________2006-05-05_/
float64 Hydrology_cascade
::change_liquid_water_content_by_volumetric
(nat8 layer, float64 change_WC)                                   modification_
{  liquid_water_content[layer] += change_WC;                                     //070824
   invalidate();                                                                 //130930
   return liquid_water_content[layer] ;                                          //070824
}
//_change_liquid_water_content_by_volumetric________________________2007-08-13_/
RENDER_INSPECTORS_DEFINITION(Hydrology_cascade )
{
   #ifdef CS_OBSERVATION
   CS::Emanator *emanator
      = Hydrology_abstract::render_inspectors(inspectors,context,"hydrology");
   if (
      #if ((CS_VERSION > 0) &&(CS_VERSION < 6))
        (CS::optation_global.is_desired(CSVP_soil_base_water_content)) ||
      #endif
        (CS::optation_global.is_desired("hydrology/water/content/volumetric")))
      inspectors.append(new CS::Inspector_vector
      (liquid_water_content,soil_layers_ref.count()
      ,UC_m3_per_m3,*context,"hydrology/water/content/volumetric",value_statistic
      ,CSVP_soil_base_water_content));
   #endif
   return 0;
}
// _RENDER_INSPECTORS_DEFINITION____________________________________2019-01-19_/
} //namespace Soil                                                               //181206

