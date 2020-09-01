#ifndef VIC_crop_phenologyH
#define VIC_crop_phenologyH
#if (VIC_CROPSYST_VERSION >= 3)
#  error This class is obsolete in version 3
/*
Version 3 now uses VIC_CropSyst_proper_crop
where the Crop class is based on CropSyst proper.
*/
#else
#ifndef primitiveH
#  include "corn/primitive.h"
#endif
#ifndef constH
#  include "corn/const.h"
#endif
#ifndef growth_stagesHPP
#  include "crop/growth_stages.hpp"
#endif
/*140810
#ifndef VIC_crop_commonH
#  include "VIC_crop_common.h"
#endif
*/
#ifndef VIC_crop_paramH
#  include "VIC_crop_param.h"
#endif
/* 140317 already included by "crop/phenology_common.h"
#ifndef phenology_interfaceH
#  include "crop/phenology_interface.h"
#endif
*/
#ifndef phenology_commonH
#  include "crop/phenology_common.h"
#endif

//_Forward declarations_________________________________________________________
class Air_temperature_average;
class Vapor_pressure_deficit_max;
namespace VIC_crop
{
class Canopy_actual;
class Dormancy;
class Dormancy_herbacious;
class Dormancy_fruit_tree;
//_____________________________________________________________________________/
class Phenology_common     // rename this to Crop_phenology_common
//130902 : public virtual Phenology_interface                                             //110824
: public extends_ CropSyst::Phenology_common
{
   friend class Dormancy;
   friend class Dormancy_herbacious;
   friend class Dormancy_fruit_tree;
 protected: // external references
   const Thermal_time_parameters   &thermal_time_parameter;
   const Season_correction         &season_correction;
   const CORN::Date &today;
   const Inactive_period_parameters *perennial_inactive_period_parameter;        //110303
   const Dormancy_parameters        *fruit_tree_dormancy_parameter;              //110303
   bool  is_root_crop;
   const Air_temperature_average      &air_Tavg;
   const Vapor_pressure_deficit_max   &VPD_max;
 public:
   float64        thermal_time_cumulative;
   float64        thermal_time_cumulative_yesterday;
   float64        thermal_time_today;
   Dormancy       *dormancy;                                                     //110302
      // dormancy will be 0 if crop is annual
 public:  // constructor
   Phenology_common
      (const Phenology_parameters        &_phenology_parameter
      ,const Thermal_time_parameters     &_thermal_time_parameter
      ,const Season_correction           &_season_correction                     //110203
      ,bool                               _is_root_crop
      ,const CORN::Date                  &_today
      ,const Inactive_period_parameters  * perennial_inactive_period_parameter     //110303
      ,const Dormancy_parameters         * fruit_tree_dormancy_parameter           //110303
      ,const Air_temperature_average     &_air_Tavg
      ,const Vapor_pressure_deficit_max  &_VPD_max
      );
 public:
   virtual bool start_day()                                        modification_;
   void develop()                                                  modification_;
   void know_harvest(nat16 _harvest_DOY)                           modification_ { harvest_DOY = _harvest_DOY; growth_stage = NGS_HARVEST; }
   //140314 virtual Normal_crop_growth_stage get_growth_stage_index()               const { return growth_stage; }  //110903
   bool perform_clipping_and_ensure_end_of_pseudodormancy()        modification_;
   bool respond_to_clipping()                                      modification_;

 public: // accessors
   inline virtual const float64 */*140810  Ref_to_const_float*/  ref_thermal_time_cumulative()         const { return &thermal_time_cumulative;}
   inline virtual float64 get_thermal_time_cumulative(bool adjusted_for_clipping = false) const { return thermal_time_cumulative;}
   //140314 inline virtual nat16 get_days_since_start_of(Normal_crop_growth_stage stage)    const { return days_since_start_of[stage]; }
// moved to phenology common     inline virtual nat16 get_days_since_start_of(Normal_crop_event_sequence stage)   const { return days_since_start_of[stage]; }

// moved to phenology common   inline nat16 get_duration_of(Normal_crop_growth_stage stage)            const { return duration_of[stage]; }
 protected:
   // RLN NYN nothing significant here virtual Growth_stage start_preemergence()                            modifies_{ return growth_stage = PREEMERGENCE; }
   virtual Normal_crop_event_sequence start_vegetative_growth()      modification_{ return growth_stage = NGS_VEGETATIVE_GROWTH; }
   virtual Normal_crop_event_sequence start_flowering()              modification_{ return growth_stage = NGS_ANTHESIS; }
   virtual Normal_crop_event_sequence start_bulking_or_filling()     modification_;
   virtual Normal_crop_event_sequence start_maturity()               modification_{ return growth_stage = NGS_MATURITY; }
 private:
   //110906 inline virtual float64 get_plant_temperature_elevation_during_stress()                       const = 0;
   /* RLN now using daily resolution
   float64 calc_thermal_time_hourly
      (float64 air_temperature)                                            const;
   */
   float64 calc_growing_degree_day_daily_resolution(float64 plant_temperature_elevation_during_stress) const;
   virtual bool start_season()                                      modification_;
   virtual bool reset()                                             modification_;//110506
   void dormancy_processing()                                       modification_;
};
//______________________________________________________________________________
class Phenology_reference    // rename to Crop_deveopment_reference
: public Phenology_common
{
 public:
   inline Phenology_reference
      (const Phenology_parameters       &_phenology_parameter
      ,const Thermal_time_parameters    &_thermal_time_parameter
      ,const Season_correction          &_season_correction                      //110203
      ,bool _is_root_crop
      ,const CORN::Date &_today
      ,const Inactive_period_parameters *perennial_inactive_period_parameter     //110303
      ,const Dormancy_parameters        *fruit_tree_dormancy_parameter           //110303
      ,const Air_temperature_average    &_air_Tavg
      ,const Vapor_pressure_deficit_max &_VPD_max
      )
      :Phenology_common(_phenology_parameter,_thermal_time_parameter,_season_correction,_is_root_crop
      ,_today
      ,perennial_inactive_period_parameter,fruit_tree_dormancy_parameter                                              //110303
      ,_air_Tavg, _VPD_max)
      {}
};
//______________________________________________________________________________
class Phenology_actual
: public Phenology_common
{
   const float64 */*140810  Ref_to_const_float*/  water_stress_index_yesterday;                              // reference to  transpiration_actual
 public:
   Phenology_actual
      (const Phenology_parameters        &_phenology_parameter
      ,const Thermal_time_parameters     &_thermal_time_parameter
      ,const Season_correction           &_season_correction                     //110203
      ,bool _is_root_crop
      ,const CORN::Date &_today
      ,const Inactive_period_parameters *_perennial_inactive_period_parameter    //110303
      ,const Dormancy_parameters        *_fruit_tree_dormancy_parameter          //110303
      ,const Air_temperature_average    &_air_Tavg
      ,const Vapor_pressure_deficit_max   &_VPD_max);
   void assign_references
      (const float64 */*140810  Ref_to_const_float*/  _water_stress_index_yesterday
      )modification_; // initialization
 private:
   //110906 virtual float64 get_plant_temperature_elevation_during_stress()                              const;
#ifdef NYN
 protected:
// no difference   virtual Growth_state start_preemergence()       modification_;
   virtual Normal_crop_growth_stage start_vegetative_growth()      modification_;
   virtual Normal_crop_growth_stage start_flowering()              modification_;
#endif
};
//______________________________________________________________________________
} // namespace VIC_crop
#endif
#endif

