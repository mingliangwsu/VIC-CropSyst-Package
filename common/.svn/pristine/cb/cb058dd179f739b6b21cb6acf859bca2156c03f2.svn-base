#ifndef residue_decomposition_paramH
#define residue_decomposition_paramH
#include "organic_matter/OM_types.h"
#if (CS_VERSION>=1 && CS_VERSION <=4)
#  include "corn/validate/validtyp.h"
#else
#  include "corn/parameters/parameter.h"
#endif
#include "common/residue/residue_const.h"

#define LABEL_residue            "residue"
#define  LABEL_decomposition_time                  "decomposition_time"
#define  LABEL_decomposition_time_50                  "decomposition_time_50"
#define  LABEL_residue_area_mass_obs               "residue_area_mass"
#define  LABEL_area_covered_to_mass_ratio          "area_covered_to_mass_ratio"

namespace CORN {class Data_record;}                                              //060601
using namespace CORN;                                                            //060602

//______________________________________________________________________________
class Residue_decomposition_parameters
{
 public:
   RUSLE2_Residue_type_clad             RUSLE2_type_clad;                        //060731
 public:  // The following prior to version 4.3
  #if (CS_VERSION>=1 && CS_VERSION <=4)
  int16    decomposition_time_63;                                                //040830
  #endif
   //200220obs int16    halflife;                                               //040830
public:  // Added for V4.3 Carbon residue and OM with with slow, fast and lignified cycling
   float32  fast_cycling_top_fraction;                                           //060217
   float32  slow_cycling_top_fraction;                                           //060217
   float32  lignified_top_fraction;                                              //060217
   float32  fast_cycling_root_fraction;                                          //060217
   float32  slow_cycling_root_fraction;                                          //060217
   float32  lignified_root_fraction;                                             //060217
   float32  fast_cycling_halflife;                                               //060217
   float32  slow_cycling_halflife;                                               //060217
   float32  lignified_halflife;                                                  //060217
   float32  detrition_halflife;                                                   //090702

   float32  fast_cycling_C_N_ratio;                                              //060217
   float32  slow_cycling_C_N_ratio;                                              //060217
   float32  lignified_C_N_ratio;                                                 //060217
   float32  carbon_fraction;                                                     //060217
   float32  stubble_area_covered_to_mass_ratio;                                  //020408
   float32  surface_area_covered_to_mass_ratio;                                  //020408
   bool     decompose_senesced_canopy;                                           //060316
   float32  max_decomposition_rate_single_pool;                                  //070707
      //150913  check with Claudio
      // perhaps max_decomposition_rate_single_pool (1/day) could be
      // normalized to be the same units as decomposition times

#if (CS_VERSION==4)
   //200220 obs Valid_int16    v_decomposition_time_63;   //  The number of days it takes to decompose to 63% (this is being replaced with 50% basis and is here only for import purposes)
   //200220 Valid_int16    v_halflife;   //  The number of days it takes to decompose to 50%
   Valid_float32  v_fast_cycling_top_fraction;           // These 3 fractions should sum to 1
   Valid_float32  v_slow_cycling_top_fraction;
   Valid_float32  v_lignified_top_fraction;
   Valid_float32  v_fast_cycling_root_fraction;   // These 3 fractions should sum to 1
   Valid_float32  v_slow_cycling_root_fraction;
   Valid_float32  v_lignified_root_fraction;
   Valid_float32  v_fast_cycling_halflife;  // Days to acheived 50% decomposition
   Valid_float32  v_slow_cycling_halflife;
   Valid_float32  v_lignified_halflife;
   Valid_float32  v_detrition_halflife;
   Valid_float32  v_carbon_fraction;
   Valid_float32  v_stubble_area_covered_to_mass_ratio;
   Valid_float32  v_surface_area_covered_to_mass_ratio;
   Valid_float32  v_max_decomposition_rate_single_pool;
#else
   //200220 Parameter_number p_halflife;   //  The number of days it takes to decompose to 50%
   Parameter_number p_fast_cycling_top_fraction;           // These 3 fractions should sum to 1
   Parameter_number p_slow_cycling_top_fraction;
   Parameter_number p_lignified_top_fraction;
   Parameter_number p_fast_cycling_root_fraction;   // These 3 fractions should sum to 1
   Parameter_number p_slow_cycling_root_fraction;
   Parameter_number p_lignified_root_fraction;
   Parameter_number p_fast_cycling_halflife;  // Days to acheived 50% decomposition
   Parameter_number p_slow_cycling_halflife;
   Parameter_number p_lignified_halflife;
   Parameter_number p_detrition_halflife;
   Parameter_number p_carbon_fraction;
   Parameter_number p_stubble_area_covered_to_mass_ratio;
   Parameter_number p_surface_area_covered_to_mass_ratio;
   Parameter_number p_max_decomposition_rate_single_pool;
   #endif
 public:
   Residue_decomposition_parameters();
   bool setup_structure(CORN::Data_record &data_rec,bool for_write);             //060219
   /*200220
   #if (CS_VERSION==4)
   float64 get_decomposition_constant_pre_V4_3()                          const; //060219
   #endif
   */
   float64 get_decomposition_constant(Organic_matter_cycling OM_cylcing)  const; //060219
   float64 get_detrition_constant()                                       const; //090413
   float64 get_area_covered_to_mass_ratio(bool stubble)                   const; //060219
   float64 get_carbon_nitrogen_ratio(Organic_matter_cycling OM_cylcing)   const; //060225
};
//_Residue_decomposition_parameters_________________________________2002-04-08_/
#endif

