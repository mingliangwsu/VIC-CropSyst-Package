#include <math.h>
#include "corn/math/compare.hpp"
#include "soil/disturbance.h"
#include "soil/texture_I.h"
#include "soil/hydrology_A.h"

#include <fstream>
#ifdef DEBUG_DECOMP
ofstream debug_decomp_adjust("debug_decomp_adjust.dat");
extern CORN::Date simulation_today;
#endif
namespace Soil {                                                                 //18120
//______________________________________________________________________________
Disturbance::Disturbance
(const Soil::Hydrology_interface  &_hydrology
,const Soil::Texture_interface    &_texture)                                      //131001
: hydrology (_hydrology)
, texture   (_texture)
, deepest_layer_affected(0)
, oxidation_effect()                                                             //100121
{  clear_layer_array64(rating);
   for (uint8 layer = 0; layer <= MAX_soil_layers; layer++)
      decomposition_adjustment_factor[layer] = 1.0; // 1.0 No effect
}
//_Disturbance:constructor_____________________________________________________/
float64 Disturbance::add_rating(float64 added_rating, nat8 affected_layers_now)
{  for (uint8 layer = 1; layer <= affected_layers_now; layer++)
      rating[layer] += added_rating;
   deepest_layer_affected = std::max<nat8>(deepest_layer_affected,affected_layers_now);
   return added_rating;
}
//_add_rating__________________________________________________________________/
bool Disturbance::process()
{  float64 rating_check_sum = 0.0;                                               //070801
   for (uint8 layer = 1; layer <= deepest_layer_affected; layer++)
   {  float64 available_water = hydrology.calc_available_water_fraction(layer,false); //070801
      rating[layer] *= decay(available_water);                                     // original VB: SDRDecay(SoilState.WaterContent(Year, DOY, i) / SoilProfile.GetSaturationWaterContent(i));
      if (rating[layer] < 1.0) rating[layer] = 0.0;                              //070801
      rating_check_sum += rating[layer];                                         //070801
         // Once the rating which is typically significantly above 1 (I.e. initially 25-50)
         // falls below 0 we discontinue this effect
      float64 clay = texture.get_clay_percent(layer);
      float64 decomposition_adjustment_factor_layer = 1.0 + calc_tillage_intensity(clay / 100.0, rating[layer]);
      decomposition_adjustment_factor[layer] = decomposition_adjustment_factor_layer;
   }
   // This is an optimization, once the deepest layer effected goes to 1
   // we can ignore any more decay.
   if (deepest_layer_affected && CORN::is_approximately<float64>(decomposition_adjustment_factor[deepest_layer_affected],1.0,0.0001))
   {  decomposition_adjustment_factor[deepest_layer_affected] = 1.0;
      rating[deepest_layer_affected] = 0.0;
      deepest_layer_affected -= 1;
   }
   bool disturbance_still_significant =
         (deepest_layer_affected > 0)
         && !CORN::is_zero<float64>(rating_check_sum);
#ifdef DEBUG_DECOMP
debug_decomp_adjust << simulation_today.c_str();
for (uint8 layer = 1; layer <= MAX_soil_layers; layer++)
{
debug_decomp_adjust << '\t' << decomposition_adjustment_factor[layer];
}
debug_decomp_adjust << '\n';
#endif
   return disturbance_still_significant;
}
//_process_____________________________________________________________________/
void Disturbance::set_oxidation_effect
(const CropSyst::Tillage_operation_struct::Oxidation_effect &oxidation_effect_)  //100121
{  oxidation_effect.sand = oxidation_effect_.sand;
   oxidation_effect.clay = oxidation_effect_.clay;
}
//_set_oxidation_effect_____________________________________________2010-01-21_/
float64 Disturbance::calc_tillage_intensity(float64 clay_fract, float64 SDR) const
{
// This function computes EMPIRICALLY a multiplier for soil decomposition rate based on
// tillage intensity and soil type for a single year

// clay is expressed fractionally
// SDR is the sum for the year of NRCS' Soil Disturbance Rating
   static const float64 k_clay = 5.5; // a curvature factor
   static const float64 k_tillage = (1.0 / 40.0);  // the curvature of the response to the Cumulative Soil Disturbance Rating for a single year
   float64 Term1 = oxidation_effect.clay + (oxidation_effect.sand - oxidation_effect.clay) * exp(-k_clay * clay_fract);  // clay dependent term
   float64 Term2 = 1.0 - exp(-k_tillage * SDR); // SDR dependent term
   float64 tillage_intensity = Term1 * Term2;
   return tillage_intensity;
}
//_calc_tillage_intensity___________________________________________2010-01-21_/
float64  Disturbance::decay(float64 available_water)                       const
{  static const float64 decay_Rate = 0.02;
   float64 SDRDecay = 0.98 * exp(-decay_Rate * available_water);
   return SDRDecay;
}
//_decay____________________________________________________________2007-08-01_/
}//_namespace_Soil_____________________________________________________________/
