#include "crop/canopy_growth_LAI_based_V5.h"
#include "crop/thermal_time.h"
//______________________________________________________________________________
namespace CropSyst {
Canopy_growth_leaf_area_index_based_V5::Canopy_growth_leaf_area_index_based_V5
(const Crop_parameters                                 &crop_parameters_
,const Crop_parameters_struct::Canopy_growth_LAI_based &parameters_
,Residues_interface              *residues_
,Crop_nitrogen_interface         *nitrogen_
,float64                         &reported_peak_LAI_
,CropSyst_NS Crop_CO2_response   *CO2_response_)
:Canopy_growth_leaf_area_index_based
   (crop_parameters_
   ,parameters_
   ,residues_
   ,nitrogen_
   ,reported_peak_LAI_
   ,CO2_response_
   )
, senesce_LAI_unit_per_deg_day(0.0) // computed at senescence                    //141106
{}
//_constructor______________________________________________________2013-06-10_/
void Canopy_growth_leaf_area_index_based_V5::age_by
(float64 daily_age_deg_day
,float64 leaf_duration_deg_day_f32)                                modification_
{}
//_age_by___________________________________________________________2008-05-15_/
bool Canopy_growth_leaf_area_index_based_V5::age_leaf
(float64 thermal_time_growing_degree_day
,float64 water_stress_index)
{
   // Note that senescence should not be instantiated if not has leaf
   if (crop_parameters.has_leaf_senescence() &&senescence)                       //190630
   {
      float64 old_GAI = get_GAI(include_vital|include_effete);
      float64 new_GAI = old_GAI
         - (senesce_LAI_unit_per_deg_day * thermal_time_growing_degree_day);     //140430
      die_back_to_GAI(new_GAI);                                                  //140430
   }
   // Do not call Canopy_growth_leaf_area_index_based::age_leaf                  //150419
   /*
   #ifdef APPLY_DURATION_BEFORE_SENESCENCE
   else
      Canopy_growth_leaf_area_index_based::age_leaf( thermal_time_growing_degree_day, water_stress_index);
   #endif
   */
   return true;
}
//_age_leaf_________________________________________________________2013-06-10_/
bool Canopy_growth_leaf_area_index_based_V5::know_senescence
(const Phenology::Period_thermal *senescence_)                     modification_
{
   bool known=Canopy_growth_leaf_area_index_based::know_senescence(senescence_); //190630
   //is_in_senescence = true;
   //NYN senescence = senescence_;
   float64 GAI_at_senescence = get_GAI(include_vital);                           //130636
   if (senescence_)                                                              //191218
   {  // would be 0 when clearing at end of senescence
      float64 senescence_duration_GDDs = senescence_->get_param_duration_GDDs();
      std::clog << "senescence_duration_GDDs:" << senescence_duration_GDDs << std::endl;
      senesce_LAI_unit_per_deg_day                                               //141106
         = GAI_at_senescence / senescence_duration_GDDs;                         //181109
   }
   return known;                                                                 //190630
}
//_know_senescence__________________________________________________2018-11-11_/
}//_namespace CropSyst_________________________________________________________/

