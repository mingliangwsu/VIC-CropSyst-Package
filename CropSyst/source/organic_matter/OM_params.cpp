#include "OM_params.h"
#include "organic_matter/OM_const.h"
#include "corn/data_source/datarec.h"
//______________________________________________________________________________
CORN::Parameter_range   decomposition_const_PR    ={0.0,0.5,0.0,1.0};
//CORN::Parameter_formatX  decomposition_const_PF    ={{13},{10,10}};

CORN::Parameter_properties_literal  unadj_microbial_decomposition_const_PP     =  {LABEL_microbial_biomass_decomposition_constant,""/*"https://en.wikipedia.org/wiki/XXXX"*//*,SUBSECTION_UNUSED*/,"Microbial biomass (unadjusted)"          ,"Decomposition constant","",UC_1_per_day,0};
CORN::Parameter_properties_literal  labile_active_SOM_decomposition_const_PP   =  {LABEL_labile_SOM_decomposition_constant     , ""/*"https://en.wikipedia.org/wiki/XXXX"*//*,SUBSECTION_UNUSED*/,"Labile active stable organic matter"      ,"Decomposition constant","",UC_1_per_day,0};
CORN::Parameter_properties_literal  metastable_active_SOM_decomposition_const_PP ={LABEL_metastable_SOM_decomposition_constant , ""/*"https://en.wikipedia.org/wiki/XXXX"*//*,SUBSECTION_UNUSED*/,"Metastable active stable organic matter"  ,"Decomposition constant","",UC_1_per_day,0};
CORN::Parameter_properties_literal  passive_SOM_decomposition_const_PP           ={LABEL_passive_SOM_decomposition_constant    , ""/*"https://en.wikipedia.org/wiki/XXXX"*//*,SUBSECTION_UNUSED*/,"Passive stable organic matter"            ,"Decomposition constant","",UC_1_per_day,0};
CORN::             Parameter_range  CN_ratio_PR ={1.0,100.0,0.0,100.0};
//CORN::            Parameter_formatX  CN_ratio_PF ={{10},{2,10}};

CORN::Parameter_properties_literal  microbial_CN_ratio_PP                        ={"microbial_CN_ratio"                         , ""/*"https://en.wikipedia.org/wiki/XXXX"*//*,SUBSECTION_UNUSED*/,"Microbial carbon/nitrogen ratio"         ,HINT_UNUSED,"",UC_ratio,0};
CORN::Parameter_properties_literal  labile_active_SOM_CN_ratio_PP                ={"labile_active_SOM_CN_ratio"                , ""/*"https://en.wikipedia.org/wiki/XXXX"*//*,SUBSECTION_UNUSED*/,"Labile active SOM carbon/nitrogen ratio",HINT_UNUSED,"",UC_ratio,0};
CORN::Parameter_properties_literal  metastable_active_SOM_CN_ratio_PP            ={"metastable_active_SOM_CN_ratio"            , ""/*"https://en.wikipedia.org/wiki/XXXX"*//*,SUBSECTION_UNUSED*/,"Metastable active SOM carbon/nitrogen ratio",HINT_UNUSED,"",UC_ratio,0};
CORN::Parameter_properties_literal  passive_SOM_CN_ratio_PP                      ={"passive_SOM_CN_ratio"                      , ""/*"https://en.wikipedia.org/wiki/XXXX"*//*,SUBSECTION_UNUSED*/,"Passive active SOM carbon/nitrogen ratio",HINT_UNUSED,"",UC_ratio,0};

CORN::Parameter_properties_literal  max_apparent_soil_C_decomposition_const_PP   ={"max_apparent_soil_C_decomposition_const"   , ""/*"https://en.wikipedia.org/wiki/XXXX"*//*,SUBSECTION_UNUSED*/,"Maximum apparent soil carbon decomposition for undisturbed soil","rate constant",""       ,UC_1_per_day,0};
CORN:: Parameter_range  max_apparent_soil_C_decomposition_const_PR ={0,1,0,1};
//CORN::Parameter_formatX  max_apparent_soil_C_decomposition_const_PF ={{12},{9,10}};

CORN::             Parameter_range  C_fraction_PR ={0.0,1.0,0.0,1.0};
//CORN::            Parameter_formatX  C_fraction_PF ={{8},{4,10}};
CORN::Parameter_properties_literal  C_fraction_from_labile_to_passive_SOM_PP     ={"C_fraction_from_labile_to_passive_SOM"     , ""/*"https://en.wikipedia.org/wiki/XXXX"*//*,SUBSECTION_UNUSED*/,"Fraction of carbon transferred from labile to passive SOM",HINT_UNUSED,"https://en.wikipedia.org/wiki/XXXX"       ,UC_fraction,0};
CORN::Parameter_properties_literal  C_fraction_from_metastable_to_passive_SOM_PP ={"C_fraction_from_metastable_to_passive_SOM" , ""/*"https://en.wikipedia.org/wiki/XXXX"*//*,SUBSECTION_UNUSED*/,"Fraction of carbon transferred from metastable to passive SOM",HINT_UNUSED,"https://en.wikipedia.org/wiki/XXXX"       ,UC_fraction,0};

CORN::Parameter_properties_literal  single_SOM_CN_ratio_PP                       ={"single_CN_ratio"                           , ""/*"https://en.wikipedia.org/wiki/XXXX"*//*,SUBSECTION_UNUSED*/,"SOM carbon/nitrogen ratio",HINT_UNUSED,"",UC_ratio,0};

//______________________________________________________________________________
Organic_matter_initialization_parameters::Organic_matter_initialization_parameters()
:organic_matter_partitioning_mode_clad(OMPM_previously_crop_land)
{
   for (nat8 h = 0; h <=3 ; h++)
   {  microbial_biomass_percent[h]=  3.0;
      labile_SOM_percent       [h]=  2.0;
      metastable_SOM_percent   [h]= 45.0;
      passive_SOM_percent      [h]= 50.0;
   }
   for (nat8 h = 4; h <= MAX_soil_horizons; h++)
   {  microbial_biomass_percent[h]=  0.5;
      labile_SOM_percent       [h]=  0.0;
      metastable_SOM_percent   [h]=  1.0;
      passive_SOM_percent      [h]= 98.5;
   }
}
//_Organic_matter_initialization_parameters:constructor_____________2006-02-15_/
bool Organic_matter_initialization_parameters::setup_structure(Data_record &data_rec,bool for_write/* = false*/ ) modification_
{  data_rec.set_current_section("organic_matter_initialization");                                                                                                                            //040812
      data_rec.expect_float32_array_with_units_code_fixed(LABEL_microbial_biomass,UC_percent,microbial_biomass_percent,1,MAX_soil_horizons,false);  //070404// "Microbial biomass"           ,"portion of the total organic matter")
      data_rec.expect_float32_array_with_units_code_fixed(LABEL_labile_SOM       ,UC_percent,labile_SOM_percent       ,1,MAX_soil_horizons,false);  //070404// "Labile stable organic matter","portion of the total organic matter")
      data_rec.expect_float32_array_with_units_code_fixed(LABEL_metastable_SOM   ,UC_percent,metastable_SOM_percent   ,1,MAX_soil_horizons,false);  //070404// "Metastable organic matter"   ,"portion of the total organic matter")
      data_rec.expect_float32_array_with_units_code_fixed(LABEL_passive_SOM      ,UC_percent,passive_SOM_percent      ,1,MAX_soil_horizons,false);  //070404// "Passive organic matter"      ,"portion of the total organic matter")
   data_rec.set_current_section("organic_matter_history");
      data_rec.expect_enum(LABEL_partitioning_mode,organic_matter_partitioning_mode_clad);
   return true;
}
//_Organic_matter_initialization_parameters:setup_structure_________2012-09-09_/
Organic_matter_decomposition_parameters::Organic_matter_decomposition_parameters()
:unadj_microbial_decomposition_const      (default_microbial_decomposition_const)
,labile_active_SOM_decomposition_const    (default_labile_active_SOM_decomposition_const)
,metastable_active_SOM_decomposition_const(default_metastable_active_SOM_decomposition_const)
,passive_SOM_decomposition_const          (default_passive_SOM_decomposition_const)
,microbial_CN_ratio                       (default_microbial_CN_ratio)
,labile_active_SOM_CN_ratio               (default_labile_active_SOM_CN_ratio)
,metastable_active_SOM_CN_ratio           (default_metastable_active_SOM_CN_ratio)
,passive_SOM_CN_ratio                     (default_passive_SOM_CN_ratio)
,single_SOM_CN_ratio                      (default_single_SOM_CN_ratio)
,C_fraction_from_labile_to_passive_SOM     (0.001) //090520 (0.004)//(0.000001/default_labile_active_SOM_decomposition_const)
,C_fraction_from_metastable_to_passive_SOM (0.01)  //090520 (0.03) // (0.000001/default_metastable_active_SOM_decomposition_const)
,max_apparent_soil_C_decomposition_const   (0.000164384)

,p_unadj_microbial_decomposition_const       (unadj_microbial_decomposition_const      ,unadj_microbial_decomposition_const_PP      ,decomposition_const_PR ,PPF_F) // decomposition_const_PF)
,p_labile_active_SOM_decomposition_const     (labile_active_SOM_decomposition_const    ,labile_active_SOM_decomposition_const_PP    ,decomposition_const_PR ,PPF_F) // decomposition_const_PF)
,p_metastable_active_SOM_decomposition_const (metastable_active_SOM_decomposition_const,metastable_active_SOM_decomposition_const_PP,decomposition_const_PR ,PPF_F) // decomposition_const_PF)
,p_passive_SOM_decomposition_const           (passive_SOM_decomposition_const          ,passive_SOM_decomposition_const_PP          ,decomposition_const_PR ,PPF_F) // decomposition_const_PF)
,p_microbial_CN_ratio                        (microbial_CN_ratio                       ,microbial_CN_ratio_PP                       ,CN_ratio_PR            ,PPF_F) // CN_ratio_PF)
,p_labile_active_SOM_CN_ratio                (labile_active_SOM_CN_ratio               ,labile_active_SOM_CN_ratio_PP               ,CN_ratio_PR,PPF_F) // CN_ratio_PF)
,p_metastable_active_SOM_CN_ratio            (metastable_active_SOM_CN_ratio           ,metastable_active_SOM_CN_ratio_PP           ,CN_ratio_PR,PPF_F) // CN_ratio_PF)
,p_passive_SOM_CN_ratio                      (passive_SOM_CN_ratio                     ,passive_SOM_CN_ratio_PP                     ,CN_ratio_PR,PPF_F) // CN_ratio_PF)
,p_max_apparent_soil_C_decomposition_const   (max_apparent_soil_C_decomposition_const  ,max_apparent_soil_C_decomposition_const_PP  ,max_apparent_soil_C_decomposition_const_PR,PPF_F) // max_apparent_soil_C_decomposition_const_PF)
,p_C_fraction_from_labile_to_passive_SOM     (C_fraction_from_labile_to_passive_SOM    ,C_fraction_from_labile_to_passive_SOM_PP    ,C_fraction_PR,PPF_F) // C_fraction_PF)
,p_C_fraction_from_metastable_to_passive_SOM (C_fraction_from_metastable_to_passive_SOM,C_fraction_from_metastable_to_passive_SOM_PP,C_fraction_PR,PPF_F) // C_fraction_PF)
,p_single_SOM_CN_ratio                       (single_SOM_CN_ratio                      ,single_SOM_CN_ratio_PP                      ,CN_ratio_PR,PPF_F) // CN_ratio_PF)
{}
//_Organic_matter_decomposition_parameters:constructor_________________________/
bool Organic_matter_decomposition_parameters::setup_structure(Data_record &data_rec,bool for_write/* = false*/ ) modification_
{
   data_rec.set_current_section("organic_matter_decomposition");
      // Note, eventually I need to use Parameters
      // but I dont have a V5 scenario editor yet, so I stick with these:

      data_rec.expect_float32(LABEL_microbial_biomass_decomposition_constant  ,unadj_microbial_decomposition_const);       //120823
      data_rec.expect_float32(LABEL_labile_SOM_decomposition_constant         ,labile_active_SOM_decomposition_const);     //120823
      data_rec.expect_float32(LABEL_metastable_SOM_decomposition_constant     ,metastable_active_SOM_decomposition_const); //120823
      data_rec.expect_float32(LABEL_passive_SOM_decomposition_constant        ,passive_SOM_decomposition_const);           //120823
      data_rec.expect_float32("microbial_CN_ratio"                      ,microbial_CN_ratio);                              //120823
      data_rec.expect_float32("labile_active_SOM_CN_ratio"              ,labile_active_SOM_CN_ratio);                      //120823
      data_rec.expect_float32("metastable_active_SOM_CN_ratio"          ,metastable_active_SOM_CN_ratio);                  //120823
      data_rec.expect_float32("passive_SOM_CN_ratio"                    ,passive_SOM_CN_ratio);                            //120823
      data_rec.expect_float32("max_apparent_soil_C_decomposition_const",max_apparent_soil_C_decomposition_const);          //120823
      data_rec.expect_float32("single_CN_ratio"                            ,single_SOM_CN_ratio);                          //120823
      data_rec.expect_float32("C_fraction_from_labile_to_passive_SOM"      ,C_fraction_from_labile_to_passive_SOM );       //120823
      data_rec.expect_float32("C_fraction_from_metastable_to_passive_SOM"  ,C_fraction_from_metastable_to_passive_SOM );   //120823
   return true;
}
//_Organic_matter_decomposition_parameters::setup_structure_________2012-09-09_/

