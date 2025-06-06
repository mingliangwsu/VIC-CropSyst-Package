#include <math.h>
#ifndef moremathH
#  include "corn/math/moremath.h"
   using namespace std;
#endif
#include "common/biomatter/decomposition_const.h"
#include "common/residue/residue_decomposition_param.h"
#include "common/residue/residue_const.h"
#include "corn/data_source/datarec.h"

#define DEFAULT_DECOMPOSITION_TIME_50   40
using namespace CORN;
//______________________________________________________________________________
#if (CS_VERSION!=4)
Parameter_properties_literal PP_decomposition_time_50              = {LABEL_decomposition_time_50         ,"ShortName"/*LABEL_decomposition_time_50*/         ,"Time to reach 50% decomposition","Hint","HTTP:\\descriptionURL",UT_day};
Parameter_properties_literal PP_fast_cycling_top_fraction          = {"fast_cycling_top_fraction"         ,"ShortName"/*"fast_cycling_top_fraction"*/         ,"Fast cycling top fraction","Hint","HTTP:\\descriptionURL",UC_fraction};
Parameter_properties_literal PP_slow_cycling_top_fraction          = {"slow_cycling_top_fraction"         ,"ShortName"/*"slow_cycling_top_fraction"*/         ,"Slow cycling top fraction","Hint","HTTP:\\descriptionURL",UC_fraction};
Parameter_properties_literal PP_lignified_top_fraction             = {"lignified_top_fraction"            ,"ShortName"/*"lignified_top_fraction"*/            ,"Lignified top fraction","Hint","HTTP:\\descriptionURL",UC_fraction};
Parameter_properties_literal PP_fast_cycling_root_fraction         = {"fast_cycling_root_fraction"        ,"ShortName"/*"fast_cycling_root_fraction"*/        ,"Fast cycling root_fraction","Hint","HTTP:\\descriptionURL",UC_fraction};
Parameter_properties_literal PP_slow_cycling_root_fraction         = {"slow_cycling_root_fraction"        ,"ShortName"/*"slow_cycling_root_fraction"*/        ,"Slow cycling root_fraction","Hint","HTTP:\\descriptionURL",UC_fraction};
Parameter_properties_literal PP_lignified_root_fraction            = {"lignified_root_fraction"           ,"ShortName"/*"lignified_root_fraction"*/           ,"Lignified root_fraction","Hint","HTTP:\\descriptionURL",UC_fraction};
Parameter_properties_literal PP_fast_cycling_halflife              = {"fast_cycling_halflife"             ,"ShortName","Fast cycling halflife","Hint","HTTP:\\descriptionURL",UM_day};
Parameter_properties_literal PP_slow_cycling_halflife              = {"slow_cycling_halflife"             ,"ShortName","Slow cycling halflife","Hint","HTTP:\\descriptionURL",UM_day};
Parameter_properties_literal PP_lignified_halflife                 = {"lignified_halflife"                ,"ShortName","Lignified biomass halflife","Hint","HTTP:\\descriptionURL",UM_day};
Parameter_properties_literal PP_detrition_halflife                 = {"detrition_halflife"                ,"ShortName"/*,"detrition_halflife" */                ,"Time to 50% detrition","Accounts for decomposition due to exposure to the elements.","HTTP:\\descriptionURL",UM_day};
Parameter_properties_literal PP_carbon_fraction                    = {"carbon_fraction"                   ,"ShortName"/*,"carbon_fraction"*/                   ,"Top and root carbon fraction","Hint","HTTP:\\descriptionURL",UC_fraction};
Parameter_properties_literal PP_stubble_area_covered_to_mass_ratio = {"stubble_area_covered_to_mass_ratio","ShortName"/*,"stubble_area_covered_to_mass_ratio"*/,"Stubble area covered to mass ratio","Hint","HTTP:\\descriptionURL",UC_m2_kg};
Parameter_properties_literal PP_surface_area_covered_to_mass_ratio = {LABEL_area_covered_to_mass_ratio    ,"ShortName"/*,LABEL_area_covered_to_mass_ratio    */,"Flattened surface residue area covered to mass ratio","Hint","HTTP:\\descriptionURL",UC_m2_kg};
Parameter_properties_literal PP_max_decomposition_rate_single_pool = {"max_decomposition_rate"            ,"ShortName"/*,"max_decomposition_rate"            */,"Maximum decomposition rate","1/day","HTTP:\\descriptionURL",/*1/day*/};

Parameter_range PPR_halflife                           ={1,100,1,1000};       //Parameter_format PPF_halflife              ={{8,0},{ 1,10} };
Parameter_range PPR_fast_cycling_top_fraction          ={0,1,0,1};            //Parameter_format PPF_fast_cycling_top_fraction          ={{8},{ 2,10} };
Parameter_range PPR_slow_cycling_top_fraction          ={0,1,0,1};            //Parameter_format PPF_slow_cycling_top_fraction          ={{8},{ 2,10} };
Parameter_range PPR_lignified_top_fraction             ={0,1,0,1};            //Parameter_format PPF_lignified_top_fraction             ={{8},{ 2,10} };
Parameter_range PPR_fast_cycling_root_fraction         ={0,1,0,1};            //Parameter_format PPF_fast_cycling_root_fraction         ={{8},{ 2,10} };
Parameter_range PPR_slow_cycling_root_fraction         ={0,1,0,1};            //Parameter_format PPF_slow_cycling_root_fraction         ={{8},{ 2,10} };
Parameter_range PPR_lignified_root_fraction            ={0,1,0,1};            //Parameter_format PPF_lignified_root_fraction            ={{8},{ 2,10} };
Parameter_range PPR_fast_cycling_halflife              ={1,300,0,1000};       //Parameter_format PPF_fast_cycling_halflife              ={{8},{ 2,10} };
Parameter_range PPR_slow_cycling_halflife              ={1,300,0,1000};       //Parameter_format PPF_slow_cycling_halflife              ={{8},{ 2,10} };
Parameter_range PPR_lignified_halflife                 ={1,300,0,1000};       //Parameter_format PPF_lignified_halflife                 ={{8},{ 2,10} };
Parameter_range PPR_detrition_halflife                 ={1,300,0,1000};       //Parameter_format PPF_detrition_halflife                 ={{8},{ 1,10} };
Parameter_range PPR_carbon_fraction                    ={0,1,0,1};            //Parameter_format PPF_carbon_fraction                    ={{8},{ 2,10} };
Parameter_range PPR_stubble_area_covered_to_mass_ratio ={1.0,10.0,0.0,15.0};  //Parameter_format PPF_stubble_area_covered_to_mass_ratio  ={{8},{ 2,10} };
Parameter_range PPR_surface_area_covered_to_mass_ratio ={1.0,10.0,0.0,15.0};  //Parameter_format PPF_surface_area_covered_to_mass_ratio  ={{8},{ 2,10} };
Parameter_range PPR_max_decomposition_rate_single_pool ={0.0,10.0,0.0,20.0};  //Parameter_format PPF_max_decomposition_rate_single_pool  ={{8},{ 0,10} };
#endif
//_parameter_properties________________________________________________________/
Residue_decomposition_parameters::Residue_decomposition_parameters()
//200220:halflife_check_used             (DEFAULT_DECOMPOSITION_TIME_50)               //040826
/*200220 obsolete
#if (CS_VERSION==4)
,decomposition_time_63             (30)
#endif
*/
:RUSLE2_type_clad                  (UNKNOWN_RESIDUE_TYPE)                        //060731
,fast_cycling_top_fraction         (  0.15)                                      //190220
,slow_cycling_top_fraction         (  0.7)                                       //190220
,lignified_top_fraction            (  0.15)                                      //190220
// For testing only

/*
,fast_cycling_root_fraction        (  0.0)                                       //060217
,slow_cycling_root_fraction        (  1.0)                                       //060217
,lignified_root_fraction           (  0.0)                                       //060217
*/

,fast_cycling_root_fraction        (  0.15)                                      //190220
,slow_cycling_root_fraction        (  0.7)                                       //190220
,lignified_root_fraction           (  0.15)                                      //190220

,fast_cycling_halflife             (  3.0)                                       //060217
,slow_cycling_halflife             (  6.0)                                       //060217
,lignified_halflife                ( 30.0)                                       //060217
,detrition_halflife                (720.0)                                       //090702
,fast_cycling_C_N_ratio            (straw_carbon_nitrogen_ratio_V4_4)            //060217
,slow_cycling_C_N_ratio            (straw_carbon_nitrogen_ratio_V4_4)            //060217
,lignified_C_N_ratio               (straw_carbon_nitrogen_ratio_V4_4)            //060217
,carbon_fraction(default_straw_residue_fraction_carbon)                          //060217
,stubble_area_covered_to_mass_ratio(2)                                           //060217
,surface_area_covered_to_mass_ratio(4)                                           //060217
,max_decomposition_rate_single_pool(0.1)                                         //070707
#if (CS_VERSION==4)
//200220obs ,v_decomposition_time_63             (decomposition_time_63     ,LABEL_decomposition_time,60,1,100,  1,1000,"Days" /*T_date_Days*/,"Time to reach 63% decomposition"/*TL_Decomposition_time_constant*/)
//200220 ,v_halflife             (halflife_check_used     ,"halflife"/*LABEL_decomposition_time_50*/,DEFAULT_DECOMPOSITION_TIME_50,1,100,  1,1000,"Days" /*T_date_Days*/,"Time to reach 50% decomposition")
,v_fast_cycling_top_fraction         (fast_cycling_top_fraction          ,UC_fraction,"fast_cycling_top_fraction"         ,2,  0.5                        ,0,1,0,1             ,0,"Fast cycling top fraction")
,v_slow_cycling_top_fraction         (slow_cycling_top_fraction          ,UC_fraction,"slow_cycling_top_fraction"         ,2,  0.4                        ,0,1,0,1             ,0,"Slow cycling top fraction")
,v_lignified_top_fraction            (lignified_top_fraction             ,UC_fraction,"lignified_top_fraction"            ,2,  0.1                        ,0,1,0,1             ,0,"Lignified top fraction")
,v_fast_cycling_root_fraction        (fast_cycling_root_fraction         ,UC_fraction,"fast_cycling_root_fraction"        ,2,  0.5                        ,0,1,0,1             ,0,"Fast cycling root_fraction")
,v_slow_cycling_root_fraction        (slow_cycling_root_fraction         ,UC_fraction,"slow_cycling_root_fraction"        ,2,  0.3                        ,0,1,0,1             ,0,"Slow cycling root_fraction")
,v_lignified_root_fraction           (lignified_root_fraction            ,UC_fraction,"lignified_root_fraction"           ,2,  0.2                        ,0,1,0,1             ,0,"Lignified root_fraction")
,v_fast_cycling_halflife             (fast_cycling_halflife              ,UM_day     ,"fast_cycling_halflife",2,  3.0                        ,1,300,0,1000        ,0,"Fast cycling time to 50%")
,v_slow_cycling_halflife             (slow_cycling_halflife              ,UM_day     ,"slow_cycling_halflife",2,  6.0                        ,1,300,0,1000        ,0,"Slow cycling time to 50%")
,v_lignified_halflife                (lignified_halflife                 ,UM_day     ,"lignified_halflife"   ,2, 30.0                        ,1,300,0,1000        ,0,"Lignified biomass time to 50%")
,v_detrition_halflife                (detrition_halflife                 ,UM_day     ,"detrition_halflife",2,720.0                        ,1,300,0,1000        ,0,"Time to 50% detrition" /*Accounts for decomposition due to exposure to the elements.*/)
,v_carbon_fraction                   (carbon_fraction                    ,UC_fraction,"carbon_fraction"                   ,2,default_straw_residue_fraction_carbon,0,1,0,1             ,0,"Top and root carbon fraction")
,v_stubble_area_covered_to_mass_ratio(stubble_area_covered_to_mass_ratio ,UC_m2_kg   ,"stubble_area_covered_to_mass_ratio",2,2                            , 1.0,10.0,0.0,15.0 ,0,"Stubble area covered to mass ratio")
,v_surface_area_covered_to_mass_ratio(surface_area_covered_to_mass_ratio ,UC_m2_kg   ,LABEL_area_covered_to_mass_ratio    ,2,4                            , 1.0,10.0,0.0,15.0 ,0,"Flattened surface residue area covered to mass ratio")
,v_max_decomposition_rate_single_pool(max_decomposition_rate_single_pool ,0         ,"max_decomposition_rate"           ,0.1,4                            , 0.0,10.0,0.0,20.0 ,"1/day","Maximum decomposition rate")
#else
//200220,p_halflife             (halflife             ,PP_halflife              ,PPR_halflife              ,PPF_decomposition_time_50)
,p_fast_cycling_top_fraction         (fast_cycling_top_fraction         ,PP_fast_cycling_top_fraction          ,PPR_fast_cycling_top_fraction          ,PPF_F) // PPF_fast_cycling_top_fraction)
,p_slow_cycling_top_fraction         (slow_cycling_top_fraction         ,PP_slow_cycling_top_fraction          ,PPR_slow_cycling_top_fraction          ,PPF_F) // PPF_slow_cycling_top_fraction)
,p_lignified_top_fraction            (lignified_top_fraction            ,PP_lignified_top_fraction             ,PPR_lignified_top_fraction             ,PPF_F) // PPF_lignified_top_fraction)
,p_fast_cycling_root_fraction        (fast_cycling_root_fraction        ,PP_fast_cycling_root_fraction         ,PPR_fast_cycling_root_fraction         ,PPF_F) // PPF_fast_cycling_root_fraction)
,p_slow_cycling_root_fraction        (slow_cycling_root_fraction        ,PP_slow_cycling_root_fraction         ,PPR_slow_cycling_root_fraction         ,PPF_F) // PPF_slow_cycling_root_fraction)
,p_lignified_root_fraction           (lignified_root_fraction           ,PP_lignified_root_fraction            ,PPR_lignified_root_fraction            ,PPF_F) // PPF_lignified_root_fraction)
,p_fast_cycling_halflife(fast_cycling_halflife,PP_fast_cycling_halflife ,PPR_fast_cycling_halflife,PPF_F) // PPF_fast_cycling_halflife)
,p_slow_cycling_halflife(slow_cycling_halflife,PP_slow_cycling_halflife ,PPR_slow_cycling_halflife,PPF_F) // PPF_slow_cycling_halflife)
,p_lignified_halflife   (lignified_halflife   ,PP_lignified_halflife    ,PPR_lignified_halflife,PPF_F) // PPF_lignified_halflife)
,p_detrition_halflife                 (detrition_halflife               ,PP_detrition_halflife                  ,PPR_detrition_halflife                ,PPF_F) // PPF_detrition_halflife)
,p_carbon_fraction                   (carbon_fraction                   ,PP_carbon_fraction                    ,PPR_carbon_fraction                    ,PPF_F) // PPF_carbon_fraction)
,p_stubble_area_covered_to_mass_ratio(stubble_area_covered_to_mass_ratio,PP_stubble_area_covered_to_mass_ratio ,PPR_stubble_area_covered_to_mass_ratio ,PPF_F) // PPF_stubble_area_covered_to_mass_ratio)
,p_surface_area_covered_to_mass_ratio(surface_area_covered_to_mass_ratio,PP_surface_area_covered_to_mass_ratio ,PPR_surface_area_covered_to_mass_ratio ,PPF_F) // PPF_surface_area_covered_to_mass_ratio)
,p_max_decomposition_rate_single_pool(max_decomposition_rate_single_pool,PP_max_decomposition_rate_single_pool ,PPR_max_decomposition_rate_single_pool ,PPF_F) // PPF_max_decomposition_rate_single_pool)
#endif
{}
//_Residue_decomposition_parameters:constructor_____________________2004-04-08_/
bool Residue_decomposition_parameters::setup_structure(CORN::Data_record &data_rec,bool for_write)
{  data_rec.set_current_section(LABEL_residue);                                  //020408
   data_rec.expect_enum("RUSLE2_type",RUSLE2_type_clad);                         //060731

   if (!for_write)
   {
      data_rec.expect_float32("fast_cycling_decomposition_time_50",fast_cycling_halflife);
      data_rec.expect_float32("slow_cycling_decomposition_time_50",slow_cycling_halflife);
      data_rec.expect_float32("lignified_decomposition_time_50"   ,lignified_halflife);
      data_rec.expect_float32("detrition_time_50"                 ,detrition_halflife);
      //200220obs data_rec.expect_float32("LABEL_decomposition_time_50"       ,halflife);

   }
   #if ((CS_VERSION>1) && (CS_VERSION<5))
   //200220obs    data_rec.expect_valid_int16  (v_decomposition_time_63); //  eventually obsolete
   //200220obs data_rec.expect_valid_int16  (v_halflife);                        //040826
   data_rec.expect_valid_float32(v_fast_cycling_top_fraction);                   //060217
   data_rec.expect_valid_float32(v_slow_cycling_top_fraction);                   //060217
   data_rec.expect_valid_float32(v_lignified_top_fraction);                      //060217
   data_rec.expect_valid_float32(v_fast_cycling_root_fraction);                  //060217
   data_rec.expect_valid_float32(v_slow_cycling_root_fraction);                  //060217
   data_rec.expect_valid_float32(v_lignified_root_fraction);                     //060217
   data_rec.expect_valid_float32(v_fast_cycling_halflife);                       //060217
   data_rec.expect_valid_float32(v_slow_cycling_halflife);                       //060217
   data_rec.expect_valid_float32(v_lignified_halflife);                          //060217
   data_rec.expect_valid_float32(v_detrition_halflife);                          //060217
   data_rec.expect_valid_float32(v_carbon_fraction);                             //060217
   data_rec.expect_valid_float32(v_stubble_area_covered_to_mass_ratio);          //020408
   data_rec.expect_valid_float32(v_surface_area_covered_to_mass_ratio);          //020408
   #else

   //200220 data_rec.expect_parameter(p_decomposition_time_50);                  //040826
   data_rec.expect_parameter(p_fast_cycling_top_fraction);                       //060217
   data_rec.expect_parameter(p_slow_cycling_top_fraction);                       //060217
   data_rec.expect_parameter(p_lignified_top_fraction);                          //060217
   data_rec.expect_parameter(p_fast_cycling_root_fraction);                      //060217
   data_rec.expect_parameter(p_slow_cycling_root_fraction);                      //060217
   data_rec.expect_parameter(p_lignified_root_fraction);                         //060217
   data_rec.expect_parameter(p_fast_cycling_halflife);                           //060217
   data_rec.expect_parameter(p_slow_cycling_halflife);                           //060217
   data_rec.expect_parameter(p_lignified_halflife);                              //060217
   data_rec.expect_parameter(p_detrition_halflife);                              //060217
   data_rec.expect_parameter(p_carbon_fraction);                                 //060217
   data_rec.expect_parameter(p_stubble_area_covered_to_mass_ratio);              //020408
   data_rec.expect_parameter(p_surface_area_covered_to_mass_ratio);              //020408
   #endif
   return true;
}
//_setup_structure__________________________________________________2006-02-19_/
/*200220 obsolete
#if (CS_VERSION==4)
float64 Residue_decomposition_parameters::get_decomposition_constant_pre_V4_3() const
{  //float64 decomposition_const = 0.0;
   float64 decomposition_time_residue_50 = decomposition_time_50_check_used;
   // prior to V 4.2.2 the constant was 63% bassis, now now using 50% decomposition basis to import if it appears to be and old version file.
   if (((decomposition_time_50_check_used == DEFAULT_DECOMPOSITION_TIME_50)) &&(decomposition_time_63 > 0))      //040830
         decomposition_time_residue_50 = convert_decomposition_time_63_to_50(decomposition_time_63);  //040830
   return  convert_halflife_to_constant(decomposition_time_residue_50);
}
//_get_decomposition_constant_pre_V4_3______________________________2006-02-19_/
#endif
*/
float64 Residue_decomposition_parameters::get_decomposition_constant(Organic_matter_cycling OM_cylcing) const
{  float64 decomp_const = 0.0;
   switch (OM_cylcing)
   {
   /*200220 obsolete
#if ((CS_VERSION > 0) && (CS_VERSION < 5))
      case OM_ORIGINAL_CYCLING   : decomp_const = get_decomposition_constant_pre_V4_3();                                         break;
      // Note that  OM_ORIGINAL_CYCLING refers to plant residues in this context
      // OM_SINGLE_CYLING is not applicable since that is for manure
#endif
      */
      case OM_FAST_CYCLING      :  decomp_const = convert_halflife_to_constant(fast_cycling_halflife); break;
      case OM_SLOW_CYCLING       : decomp_const = convert_halflife_to_constant(slow_cycling_halflife); break;
      case OM_LIGNIFIED_CYCLING  : decomp_const = convert_halflife_to_constant(lignified_halflife);    break;
      default : /*  N/A to single cycling and intrinsic cycling? */ break;       //2009-04-16
   }
   return decomp_const;
}
//_get_decomposition_constant_______________________________________2006-02-19_/
float64 Residue_decomposition_parameters::get_detrition_constant()         const
{  float64 detrition_const = convert_halflife_to_constant(detrition_halflife);
   return detrition_const;
}
//_get_detrition_constant___________________________________________2006-07-02_/
float64 Residue_decomposition_parameters::get_area_covered_to_mass_ratio(bool stubble) const
{  float64 area_to_mass_ratio = (stubble) ? stubble_area_covered_to_mass_ratio : surface_area_covered_to_mass_ratio;
   return area_to_mass_ratio;
}
//_get_area_covered_to_mass_ratio___________________________________2006-02-19_/
float64 Residue_decomposition_parameters::get_carbon_nitrogen_ratio
(Organic_matter_cycling OM_cylcing)                                        const
{  float64 CN_ratio = 50.0;
   switch (OM_cylcing)
   {
#if ((CS_VERSION > 0) && (CS_VERSION < 5))
      case OM_ORIGINAL_CYCLING   : CN_ratio = 50.0;                   break;
#endif
      case OM_FAST_CYCLING       : CN_ratio = fast_cycling_C_N_ratio; break;
      case OM_SLOW_CYCLING       : CN_ratio = slow_cycling_C_N_ratio; break;
      case OM_LIGNIFIED_CYCLING  : CN_ratio = lignified_C_N_ratio;   break;
      default : /*  N/A to single cycling and intrinsic cycling? */ break;
   }
   return CN_ratio;
}
//_get_carbon_nitrogen_ratio________________________________________2006-02-25_/

