#if (VIC_CROPSYST_VERSION >= 3)
#  error This class is obsolete in version 3
/*
Version 3 now uses VIC_CropSyst_proper_crop
where the Crop class is based on CropSyst proper.
*/
#else
#ifndef VIC_crop_transpirationH
#define VIC_crop_transpirationH
#if (!VB_VERSION_DATE || (VB_VERSION_DATE >= 20110110))
#include "corn/primitive.h"
#include "corn/const.h"
#include "soil/layering.h"
#include "VIC_crop_param.h"
//#include "VIC_crop_common.h"
#include "corn/measure/measures.h"
#include "corn/math/statistical/stats.h"
#include "crop/transpiration_interface.h"
#include "weather/parameter/WP_ET_reference.h"
//140810 class ET_reference; // _interface;

class Soil_layers_interface;
class Soil_hydrology_interface;
class Wind_speed/*_parameter*/ ;
//140810 class ET_Reference;                                                              //140808_110113
//140808 class ET_Reference_parameter;                                                    //110113
class Relative_humidity/*_minimum_parameter*/ ;

namespace CropSyst
{
class Crop_CO2_response;
}
namespace VIC_crop
{
//__forward_declarations_________________________________________________________
class Phenology_actual;
class Root;
//_______________________________________________________forward_declarations__/
class Transpiration_abstract
: public virtual Transpiration_interface
{
protected: // parameters ( These should be renamed to match CropSyst or vica versa
   const Transpiration_parameters &parameter;
   const Canopy_parameters        &canopy_parameter;
   const Canopy_growth_common_parameters &canopy_growth_common_parameter;
   float32 phenology_parameter_end_vegetative_growth_deg_day;
protected:  // external references
   const float64 */*140810  Ref_to_const_float*/             canopy_solar_rad_intercepted_fraction;         //110111  PotentialFractionSolarRadiationIntercepted or FractionSolarRadInterceptedGreenCanopy
   const float64 */*140810  Ref_to_const_float*/             thermal_time_cumulative;
   #ifdef CROPSYST_PROVIDED_ET
   const ET_Reference/*140810 _parameter */       *ET_ref;                                        //110113
   #endif
   CropSyst::Crop_CO2_response             *CO2_response; // optional reference, not owned //110903
   // Although these crop heights are canopy states , they are only used in the transpiration model.
   float64                        crop_height;                                   //110111
   float64                        crop_height_max_attained;                      //110111 was Max_Reached_Height_Potential_Crop Max_Reached_Height_Actual_Crop
   const Wind_speed/*_parameter*/                 *wind_speed_element;                //110111
   const Relative_humidity/*_minimum_parameter*/  *RH_min_element;                    //110111
protected:
   float64 ET_crop_coefficient;                                                  //110111 Current_Pot_Crop_ET_Coefficient Current_Crop_ET_Coefficient
   float64 ET_crop_coef_full_shading;                                            //110111   CurrentETCropCoefficientFullShading(DACE) Current_Crop_ET_Coeff_Full_Shading
   float64 potential_cum_mm;                                                     // CumulativePotentialCropPotentialTranspiration CumulativeCropPotentialTranspiration
   float64 crop_potential_ET;       /*used only for output*/                     // PotentialCropPotentialET   CropPotentialET
   float64 crop_potential_ET_cum;   /*used only for output?*/
   float64 ET_reference_crop_CO2_adjusted;
   float64 ET_crop_coef_full_shading_yesterday;                                  //110111    // was Previous_Day_Potential_Crop_ET_Coeff_Full_Shading
   float64 ET_crop_coef_unadj_max;                                               //110111    // was Max_Unadj_Crop_ET_Coeff_Potential_Crop and Max_Unadj_Crop_ET_Coeff_Actual_Crop
           // rename Previous_Day_Potential_Crop_ET_Coeff_Full_Shading to pot_crop_ET_coef_yesterday
public:
   float64 potential_mm;

public:
   Transpiration_abstract
      (const Transpiration_parameters &_transpiration_parameters
      ,const Canopy_parameters        &_canopy_parameters
      ,const Canopy_growth_common_parameters &_canopy_growth_common_parameter
      ,float32 phenology_parameter_end_vegetative_growth_deg_day);
   virtual bool start_day()                                        modification_;//110601
   virtual bool start_season()                                     modification_;//110624
   virtual float64 update_potential()                              modification_;
   inline virtual  float64 get_potential_mm()                              const { return potential_mm; }
   inline virtual float64 get_water_stress_factor_yesterday()              const { return 1.0; }
   void assign_references
      (
      const float64 */*140810  Ref_to_const_float*/             _canopy_solar_rad_intercepted_fraction
      ,const float64 */*140810  Ref_to_const_float*/             _thermal_time_cumulative
      ,CropSyst::Crop_CO2_response  *_CO2_response                               //110903
      #if (!VB_VERSION_DATE || (VB_VERSION_DATE >= 20110110))
      ,const Wind_speed             *_wind_speed_element                         //110111
      ,const Relative_humidity      *_RH_min_element
      #ifdef CROPSYST_PROVIDED_ET
      ,const ET_Reference           *_ET_ref
      #endif
      #endif
      );        //110111
   inline float64 get_ET_ref_CO2_output_mm()                               const { return ET_reference_crop_CO2_adjusted;}
   inline float64 get_crop_potential_ET()                                  const { return crop_potential_ET;}
   #ifdef CROPSYST_PROVIDED_SOIL_HYDROLOGY
   #else
   inline float64 set_potential_mm(float64 _potential_mm)                        { potential_mm = _potential_mm; return potential_mm; }
   #endif
   float64 update_crop_ET_coef_full_shading()                     modification_;
private:
   float64 CalculateFullShadingCropETCoeff(float64 crop_height)            const;
   float64 calc_weather_adjusted_crop_ET_coef
      (float64 crop_height, float64 Unajusted_Crop_ET_Full_Shading)        const;
};
//_____________________________________________________Transpiration_abstract__/
class Transpiration_unstressed
: public Transpiration_abstract
{
   // In this case:
   // Transpiration_reference::potential           is  PotentialCropPotentialTranspiration
   // Transpiration_reference::potential_cum       is  CumulativePotentialCropTranspiration
   // Transpiration_reference::ET_crop_coefficient is PotentialCurrentETCropCoefficient
   // Transpiration_reference::canopy_solar_rad_intercepted_fraction is CropState.PotentialFractionSolarRadiationIntercepted
   // Transpiration_reference::thermal_time_cumulative is CropState.PotentialCumulativeThermalTime
public:
   inline Transpiration_unstressed
      (const Transpiration_parameters  &_transpiration_parameters
      ,const Canopy_parameters         &_canopy_parameters
      ,const Canopy_growth_common_parameters &_canopy_growth_common_parameter
      ,float32             _phenology_parameter_end_vegetative_growth_deg_day)
      : Transpiration_abstract
         (_transpiration_parameters
         ,_canopy_parameters
         ,_canopy_growth_common_parameter
         ,_phenology_parameter_end_vegetative_growth_deg_day
         )
      {}
};
//____________________________________________________Transpiration_reference__/
class Transpiration_actual
: public Transpiration_abstract
{
protected: // parameters ( These should be renamed to match CropSyst or vica versa
   // Transpiration_actual::potential           is  ActualCropPotentialTranspiration
   // Transpiration_actual::ET_crop_coefficient is CurrentETCropCoefficient
   // Transpiration_actual::canopy_solar_rad_intercepted_fraction is canopy_actual.solar_rad_intercepted_green_canopy
   // Transpiration_actual::thermal_time_cumulative  is CropState.CumulativeThermalTime
protected: // external references
   const Phenology_actual                 *phenology;
   const Soil_layers_interface            *soil_layers;
   modifiable_ Soil_hydrology_interface   *soil_hydrology;
   const Root                             *root;
protected:  //friend class xxxx;
   float64 water_stress_factor;          float64 water_stress_factor_yesterday;  // was WaterStressFactor(DAP) and WaterStressFactor(DAP-1)
   float64 water_stress_index;           float64 water_stress_index_yesterday;   // was WaterStressIndex

   CORN::Statistical_running_tally water_stress_flowering_index;                 //110524
                                                                                 // unused CumulativeWaterStressIndex
   float64 actual_mm;                                                            // was ActualCropTranspiration
   float64 actual_cum_mm;                                                        // was CumulativeActualCropTranspiration
   float64 deficiency_mm;
   float64 leaf_water_potential; float64 leaf_water_potential_yesterday;         // water
   soil_layer_array64(water_uptake_m); // meter
public:
   Transpiration_actual
      (const Transpiration_parameters     &_transpiration_parameters
      ,const Canopy_parameters        &_canopy_parameters
      ,const Canopy_growth_common_parameters &_canopy_growth_common_parameter
      ,float32 phenology_parameter_end_vegetative_growth_deg_day
/*
      ,const         ET_reference_interface    &_ET_ref_model
      ,const         Soil_layers_interface     &_soil_layers
      ,modifiable_   Soil_hydrology_interface  &_soil_hydrology
      ,const         Phenology_actual          &_phenology
      ,const         Root                      &_root
*/
      );
   inline void know
      (const         Soil_layers_interface     *_soil_layers
      ,modifiable_   Soil_hydrology_interface  *_soil_hydrology
      ,const         Phenology_actual          *_phenology
      ,const         Root                      *_root)
      {  soil_layers       = _soil_layers;
         soil_hydrology    = _soil_hydrology;
         phenology         = _phenology;
         root              = _root;
      }
   float64 update_water_uptake()                                   modification_;
   virtual bool start_day()                                        modification_;//110601
   virtual bool start_season()                                     modification_;//110624

      ///\return actual update in mm
   inline float64 get_water_stress_factor()                                const { return water_stress_factor ; }
   inline const float64 */*140810  Ref_to_const_float*/  ref_water_stress_factor_yesterday()           const { return &water_stress_factor_yesterday ; }
   inline virtual float64 get_water_stress_factor_yesterday()              const { return water_stress_factor_yesterday; }
   inline float64 get_water_stress_index()                                 const { return water_stress_index; }
   inline const float64 */*140810  Ref_to_const_float*/  ref_water_stress_index()                      const { return &water_stress_index; }
   inline const float64 */*140810  Ref_to_const_float*/  point_water_stress_index_yesterday()          const { return &water_stress_index_yesterday; }
   inline const float64& ref_water_stress_index_yesterday()                const { return water_stress_index_yesterday; }
   inline const float64 */*140810  Ref_to_const_float*/  ref_leaf_water_potential_yesterday()          const { return &leaf_water_potential_yesterday; }
   inline float64 get_leaf_water_potential_yesterday()                     const { return leaf_water_potential_yesterday; }
   inline const float64 get_water_stress_flowering_index()                 const { return water_stress_flowering_index .get_mean(); } //110524
   inline float64 get_uptake_mm(nat8 layer)                                const { return m_to_mm(water_uptake_m[layer]); }
public: // accessors for output
   inline float64 get_actual_mm()                                          const { return actual_mm; }
   inline float64 get_leaf_water_potential()                               const { return leaf_water_potential; }
   inline bool respond_to_clipping()                                modification_{water_stress_flowering_index.reset(); return true;}
};
//_______________________________________________________Transpiration_actual__/
} // namespace VIC_crop
#endif
#endif
#endif
