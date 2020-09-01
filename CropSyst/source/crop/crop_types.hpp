#ifndef crop_typeHPP
#define crop_typeHPP
#ifndef crop_typesH
#   include "crop/crop_types.h"
#endif
#include "corn/enum.h"

static const nat8 include_vital = 1;
static const nat8 include_effete = 2;
static const nat8 include_canopy_dead = 4;

DECLARE_ENUM_CLAD(Stem_type_clad                ,Stem_type                 ,stem_type_label_table);
DECLARE_ENUM_CLAD(Crop_model_clad               ,Crop_model                ,crop_model_label_table);
DECLARE_ENUM_CLAD(Harvested_part_clad           ,Harvested_part            ,harvested_part_label_table)
DECLARE_ENUM_CLAD(Life_cycle_clad               ,Life_cycle                ,life_cycle_label_table);
DECLARE_ENUM_CLAD(Photosynthetic_pathway_clad   ,Photosynthetic_pathway    ,photosynthetic_pathway_label_table)
DECLARE_ENUM_CLAD(Emergence_model_clad          ,Emergence_model           ,emergence_model_label_table);
DECLARE_ENUM_CLAD(Canopy_growth_clad            ,Canopy_growth_model       ,canopy_growth_label_table);
DECLARE_ENUM_CLAD(Nitrogen_leaf_stress_mode_clad,Nitrogen_leaf_stress_mode ,nitrogen_leaf_stress_model_label_table);
DECLARE_ENUM_CLAD(Continuation_clad             ,Continuation_mode         ,continuation_mode_label_table);
//200212 DECLARE_ENUM_CLAD(Season_dormancy_mode_clad     ,Season_dormancy_mode      ,season_dormancy_mode_label_table);
DECLARE_ENUM_CLAD(Harvest_condition_mode_clad   ,Harvest_condition_mode    ,harvest_condition_mode_label_table);
DECLARE_ENUM_CLAD(TUE_Equation_clad_obsolete    ,TUE_Equation_obsolete     ,TUE_equation_label_table);
DECLARE_ENUM_CLAD(RUE_Bais_clad_obsolete        ,RUE_Basis_obsolete        ,RUE_basis_label_table);
DECLARE_ENUM_CLAD(Thermal_response_clad         ,Thermal_response          ,Thermal_response_label_table)  //1510021124
DECLARE_ENUM_CLAD(Photoperiodism_clad,Photoperiodism ,photoperiodism_label_table)   //151122_100512

//16-05-31 At the moment V5 is still using V4 parameter files
DECLARE_ENUM_CLAD(Senesced_biomass_fate_clad    ,Senesced_biomass_fate_obsoleteX    ,senesced_biomass_fate_label_table);
DECLARE_ENUM_CLAD(Abscission_clad,Abscission,abscission_label_table);
#endif
