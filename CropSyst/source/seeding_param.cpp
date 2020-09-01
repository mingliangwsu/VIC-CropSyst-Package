#include "seeding_param.h"
#include "corn/data_source/datarec.h"
#define NoAltFieldName 0
#define NotYetSubsection "NotYetSubsection"
using namespace CORN;

CORN::Parameter_properties_literal PP_rate_seeds    = {"seeding_rate"   ,NoAltFieldName,"Seed application rate" ,"","",UC_count_per_m2,"seeds/m²"};
CORN::Parameter_properties_literal PP_emergence_rate= {"emergence_rate" ,NoAltFieldName,"Emergence rate"        ,"","",UC_percent};
CORN::Parameter_properties_literal PP_sowing_depth  = {"sowing_depth"   ,NoAltFieldName,"Sowing depth"          ,"","",UC_cm};

CORN::Parameter_range PPR_rate_seeds      = {    1,1000.0,  0.0,1000000.0};
CORN::Parameter_range PPR_emergence_rate  = {  5.0, 100.0,  0.0, 100.0  };
CORN::Parameter_range PPR_sowing_depth    = {  0.0,  50.0,  0.0, 100.0  };
/*200318
CORN::Parameter_format PPF_rate_seeds     = {8,0};
CORN::Parameter_format PPF_emergence_rate = {8,1};
CORN::Parameter_format PPF_sowing_depth   = {8,2};
*/
//______________________________________________________________________________
Seeding_parameters::Seeding_parameters()
: rate_seeds_m2         (300.0)               // This is for wheat (guess RLN)
, emergence_rate_percent(100.0)
, sowing_depth_cm       (5.0)
, p_rate_seeds    (rate_seeds_m2          ,PP_rate_seeds    ,PPR_rate_seeds      ,PPF_F) //PPF_rate_seeds)
, p_emergence_rate(emergence_rate_percent ,PP_emergence_rate,PPR_emergence_rate  ,PPF_F) //PPF_emergence_rate)
, p_sowing_depth  (sowing_depth_cm        ,PP_sowing_depth  ,PPR_sowing_depth    ,PPF_F) //PPF_sowing_depth)
{}
//_Seeding_parameters:constructor______________________________________________/
Seeding_parameters::Seeding_parameters(const Seeding_parameters &original)
: rate_seeds_m2            (original.rate_seeds_m2)               // This is for wheat (guess RLN)
, emergence_rate_percent   (original.emergence_rate_percent)
, sowing_depth_cm          (original.sowing_depth_cm)
, p_rate_seeds    (rate_seeds_m2          ,PP_rate_seeds    ,PPR_rate_seeds      ,PPF_F) //PPF_rate_seeds)
, p_emergence_rate(emergence_rate_percent ,PP_emergence_rate,PPR_emergence_rate  ,PPF_F) //PPF_emergence_rate)
, p_sowing_depth  (sowing_depth_cm        ,PP_sowing_depth  ,PPR_sowing_depth    ,PPF_F) //PPF_sowing_depth)
{  rate_seeds_m2          =  (original.rate_seeds_m2);
   emergence_rate_percent=   (original.emergence_rate_percent);
   sowing_depth_cm       =   (original.sowing_depth_cm);
}
//_Seeding_parameters:constructor______________________________________________/
void Seeding_parameters::setup_parameters(CORN::Data_record &data_rec,bool for_write) modification_
{
   data_rec.expect_parameter(p_rate_seeds);
   data_rec.expect_parameter(p_emergence_rate);
   data_rec.expect_parameter(p_sowing_depth);
}
//_setup_parameters____________________________________________________________/
