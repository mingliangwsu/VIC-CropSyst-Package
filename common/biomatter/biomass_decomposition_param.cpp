#include "options.h"
#include "biomass_decomposition_param.h"
#include <corn/measure/units.h>
#include <common/biomatter/decomposition_const.h>
#include <corn/parameters/parameters_datarec.h>
#include <math.h>
#ifndef moremathH
#  include <corn/math/moremath.h>
   using namespace std;
#endif
//______________________________________________________________________________
Biomass_decomposition_parameters::Biomass_decomposition_parameters
(Organic_matter_source source)                                                   //100216
: Common_parameters_data_record(NO_ASSOCIATED_DIRECTORY,"manure",CS_VERSION)     //180626 CS_VERSION_NUMBERS)

//NYN/*_______*/ ,NH3_N_content_kgN_kgDM                         (0.04)   ,v_NH3_N_content           (NH3_N_content_kgN_kgDM ,UC_kg_per_kg  ,"NH3_N_content"        ,3    ,0.04, 0.0,1.0   , 0.0,999.99,"kgN/kgDM","Ammonium (NH3) content")
,v_carbon_percent         (composition_decomposition.carbon_percent_DryWt    ,UC_percent    ,"carbon"      ,3    ,0.30, 0.0,100.0   , 0.0,100.0   ,"%","Carbon percent of dry weight biomass")
,v_nitrogen_percent       (composition_decomposition.nitrogen_percent_DryWt  ,UC_percent    ,"nitrogen"    ,3    ,0.03, 0.0,5.0   , 0.0,100.0   ,"%","Nitrogen percent of dry weight biomass")
// NYN ,area_to_mass_ratio_after_application           (0.2)    ,v_area_to_mass_ratio_after_application         (xxxx)
{  setup_default(source);
}
//_Biomass_decomposition_parameters:constructor________________________________/
void Biomass_decomposition_parameters::setup_default(Organic_matter_source source)
{  composition_decomposition.cycling_pools = default_organic_matter_composition_decomposition[source].cycling_pools;
   for (int i = 0; i < 5; i++) // 5 is cycling count
   {  composition_decomposition.nitrogen_percent_DryWt= default_organic_matter_composition_decomposition[source].nitrogen_percent_DryWt;
      composition_decomposition.carbon_percent_DryWt  = default_organic_matter_composition_decomposition[source].carbon_percent_DryWt;
      composition_decomposition.cycling_pools         = default_organic_matter_composition_decomposition[source].cycling_pools;
      composition_decomposition.fraction_of_biomass[i]              = default_organic_matter_composition_decomposition[source].fraction_of_biomass[i];
      composition_decomposition.days_to_50_percent_decomposition[i] = default_organic_matter_composition_decomposition[source].days_to_50_percent_decomposition[i];
   }
}
//_setup_default_________________________________________2011-08-17_2010-01-16_/
float64 Biomass_decomposition_parameters::get_decomposition_constant(Organic_matter_cycling OM_cylcing) const
{  float64 decomp_const = 0.0;
   decomp_const = convert_halflife_to_constant(composition_decomposition.days_to_50_percent_decomposition[OM_cylcing]);
   return decomp_const;
}
//_get_decomposition_constant_______________________________________2006-02-19_/
bool Biomass_decomposition_parameters::expect_structure(bool for_write)
{  bool expected = Common_parameters_data_record::expect_structure(for_write);   //161025
   structure_defined = false;                                                    //120314
   expect_valid_float32(v_carbon_percent);
   expect_valid_float32(v_nitrogen_percent);
   expect_bool("cycling_pools",composition_decomposition.cycling_pools);         //120318
   set_current_section("fraction_of_biomass");
                                     composition_decomposition.fraction_of_biomass[OM_SINGLE_CYCLING] = 1.0; // always 1
      expect_float32("fast_cycling" ,composition_decomposition.fraction_of_biomass[OM_FAST_CYCLING]);
      expect_float32("slow_cycling" ,composition_decomposition.fraction_of_biomass[OM_SLOW_CYCLING]);
      expect_float32("ligneous"     ,composition_decomposition.fraction_of_biomass[OM_LIGNEOUS_CYCLING]);
   set_current_section("days_to_50_percent_decomposition");
      expect_float32("single"       ,composition_decomposition.days_to_50_percent_decomposition[OM_SINGLE_CYCLING]);
      expect_float32("fast_cycling" ,composition_decomposition.days_to_50_percent_decomposition[OM_FAST_CYCLING]);
      expect_float32("slow_cycling" ,composition_decomposition.days_to_50_percent_decomposition[OM_SLOW_CYCLING]);
      expect_float32("ligneous"     ,composition_decomposition.days_to_50_percent_decomposition[OM_LIGNEOUS_CYCLING]);
   structure_defined = true;                                                     //120314
   return expected;
}
//_expect_structure_________________________________________________2008-09-08_/

