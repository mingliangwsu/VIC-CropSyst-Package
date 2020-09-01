#ifndef canopy_growth_LAI_based_V5H
#define canopy_growth_LAI_based_V5H
#include "crop/canopy_growth_LAI_based.h"
#include "canopy_cover_curve.h"

// This is actually now available in also in V4 as the preferred option
// In V5 the V4 version will be obsolete.
namespace CropSyst {
//______________________________________________________________________________
class Canopy_growth_leaf_area_index_based_V5
: public extends_ Canopy_growth_leaf_area_index_based
{private: // Parameter references
   float64 senesce_LAI_unit_per_deg_day;                                         //141106
      // Degree days per one unit of LAI scenesence
      // This used to be a parameter now this is computed
      // at the start of senescence
 public: // structors
   Canopy_growth_leaf_area_index_based_V5
      (const Crop_parameters                                 &crop_parameters_      // Will be owned by this crop object
      ,const Crop_parameters_struct::Canopy_growth_LAI_based &parameters_
      ,Residues_interface        *residues_                                      //060816
      ,Crop_nitrogen_interface   *nitrogen_                                      //060816
      ,float64                   &reported_peak_LAI_                             //080725
      ,Crop_CO2_response         *CO2_response_);                                //080728
   virtual bool age_leaf                                                         //070328
      (float64  thermal_time_growing_degree_day
      ,float64 water_stress_index);
 private:
   virtual void age_by(float64 daily_age_deg_day,float64 leaf_duration_deg_day_f32);
 public:
   virtual bool know_senescence(const Phenology::Period_thermal *senescence) modification_; //181109
};
//_Canopy_growth_leaf_area_index_based_V5______________________________________/
}//_namespace CropSyst_________________________________________________________/
#endif

