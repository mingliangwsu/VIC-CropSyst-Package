
#include "VIC_crop_param.h"
namespace VIC_crop
{
#ifdef CROP_PARAMETERS_HARDCODED
#else
//______________________________________________________________________________
void Crop_parameters::expect_structure(bool for_write)
{  CropSyst::Crop_parameters::expect_structure(for_write);
   structure_defined = false;
   structure_defined = false;                                                     //130708
   set_current_section("season");
   expect_nat16("start_DOY_WA",season_correction_WA.earliest_start_DOY,10);
   expect_nat16("start_DOY_ID",season_correction_ID.earliest_start_DOY,10);
   expect_nat16("start_DOY_OR",season_correction_OR.earliest_start_DOY,10);
   expect_nat16("duration_WA", season_correction_WA.duration,10);
   expect_nat16("duration_ID", season_correction_ID.duration,10);
   expect_nat16("duration_OR", season_correction_OR.duration,10);
   structure_defined = true;                                                     //120314
};
//______________________________________________________________________________
bool Crop_parameters
::setup_season_correction(float64 latitude_dec_deg,float64 longitude_dec_deg)  modification_
{
   // The following is special for the Columbia Basin run where
   // the weather data is bad and we need to correct the season phenology parameters.
   if (latitude_dec_deg >= 54.66666)            //use_BC_phenology
   {  season_correction.earliest_start_DOY = season_correction_BC.earliest_start_DOY;
      season_correction.duration = season_correction_BC.duration;
   }
   else if
   //110311 (longitude_dec_deg > -117.250000)    //further west than Idaho-Washington border
        /*The washington Idaho border is -117.0 */
   (longitude_dec_deg > -115.75)  // Now using a point near Idaho's east boarder so the panhandle is the same as washington
   { season_correction.earliest_start_DOY = season_correction_ID.earliest_start_DOY;
     season_correction.duration = season_correction_ID.duration;
   }
   else if (latitude_dec_deg >= 46.299999)      //use_WA_phenology
   { season_correction.earliest_start_DOY =season_correction_WA.earliest_start_DOY;
     season_correction.duration =season_correction_WA.duration;
   }
   else                                         //use_OR_phenology
   { season_correction.earliest_start_DOY =season_correction_OR.earliest_start_DOY;
     season_correction.duration =season_correction_OR.duration;
   };
   return true;
};
//______________________________________________________________________________
bool Crop_parameters
::adjust_phenology(float32 phenology_multiplier,float32 season_start_multiplier)
{
   // Multipliers may be 0 if there is no adjustment
   if (is_perennial())
   {
      if (season_start_multiplier > 0.00001)
      {
      season_correction.earliest_start_DOY    *= season_start_multiplier;
      season_correction_BC.earliest_start_DOY *= season_start_multiplier;
      season_correction_ID.earliest_start_DOY *= season_start_multiplier;
      season_correction_WA.earliest_start_DOY *= season_start_multiplier;
      season_correction_OR.earliest_start_DOY *= season_start_multiplier;
      season_correction.earliest_start_DOY    *= season_start_multiplier;
      phenology.emergence_deg_day              = season_start_multiplier * (float32)phenology.emergence_deg_day;
      }
   } else
      if (phenology_multiplier > 0.00001)
         phenology.emergence_deg_day = phenology_multiplier * (float32)phenology.emergence_deg_day;
   if (phenology_multiplier > 0.00001)
   {
      phenology.tuber_initiation_deg_day         = phenology_multiplier * (float32)phenology.tuber_initiation_deg_day ;
      //140808 phenology.end_vegetative_growth_deg_day    = phenology_multiplier * (float32)phenology.end_vegetative_growth_deg_day ;
      phenology.max_canopy_deg_day    = phenology_multiplier * (float32)phenology.max_canopy_deg_day ;
      phenology.max_root_depth_deg_day           = phenology_multiplier * (float32)phenology.max_root_depth_deg_day ;
      phenology.begin_flowering_deg_day          = phenology_multiplier * (float32)phenology.begin_flowering_deg_day ;
      phenology.begin_filling_deg_day            = phenology_multiplier * (float32)phenology.begin_filling_deg_day ;
      phenology.begin_maturity_deg_day           = phenology_multiplier * (float32)phenology.begin_maturity_deg_day ;
      phenology.begin_senescence_deg_day         = phenology_multiplier * (float32)phenology.begin_senescence_deg_day ;
      phenology.maturity_significant             = phenology_multiplier * (float32)phenology.maturity_significant ;
      phenology.begin_rapid_fruit_growth_deg_day = phenology_multiplier * (float32)phenology.begin_rapid_fruit_growth_deg_day;
   };
   return true;
}
//______________________________________________________________________________
#endif
}; // namespace VIC_crop

