#ifndef evaporation_without_vapor_pathH
#define evaporation_without_vapor_pathH
#include "soil/soil_evaporator.h"
#include "soil/hydrology_finite_diff.h"
namespace Soil {
//______________________________________________________________________________
class Evaporation_without_vapor_path
: public Evaporator

// 160525  I should not derived this from CS::Evaporator


{  // This class works with millimeter
 private:  // references
   modifiable_ Hydrology_finite_difference &hydrology_FD;    // Owned by the soil class
   cognate_ float64  atmospheric_water_potential;           // kg.s/m^4   Maintained by infiltration model.
      // atmospheric_water_potential should actually be a biometeorology element   160606
 private:  //
   bool     first_day;                                                           //090417
   bool     period_evaporation_done;                                             //090417
   float64  evaporation_depth;                                                   //090504
   float64  sum_evaporation_mm_ts;             // needs units ?units? I am pretty sure this is for the current wetted period (it is not cummulative evaporation) 090417
   float64  max_current_period_evaporation;                                      //090417
   float64  absolute_max_evaporation;                                            //090417
   float64  old_WC;                                                              //090417
   float64  mulch_cover_fraction;                                                //100610
 public :
   Evaporation_without_vapor_path
      (modifiable_ CORN::Dynamic_array<float64> &evaporation_potential_remaining_, //160717
       const Soil::Layers_interface             &soil_layers
      ,const Soil::Hydraulic_properties_interface   *soil_hydraulic_props
      ,Soil::Hydrology_finite_difference        &hydrology);
 public : // processing
   void initialize(uint8 number_real_plus_extended_layer)         modification_;
   inline void know_mulch_cover_fraction                                         //100610
      (float64 new_mulch_cover_fraction)                          modification_
      { mulch_cover_fraction = new_mulch_cover_fraction; }
   float64 evaporate
      (float64 &flux_mm                // modified
      , Seconds time_step
      , Hour    hour_0based
      , float64 pot_evap_mm_day /* mm/day */ )                    modification_;
      // Determine evaporation for the timestep and
      // deducts the water from the soil.
      // Sets up vapor_path
      // Returns actual evaporation in mm
      // Note that pot_evap_mm_day should be set to 0 if ponding or snow
      void know_atmospheric_water_potential(float64 atmospheric_water_potential_) cognition_
         { atmospheric_water_potential = atmospheric_water_potential_; }
 private:
   float64 calc_actual
      ( Seconds time_step   /* May need to be integer */
      , Hour    hour
      , float64 daily_pot_evap_m_ts /* m/hr */ )                          const;
   void subtract_water_from_layer
      ( float64 &evaporation_to_subtract
      , nat8 i)                                                   modification_;
      /*appears to be layer but check that it's not node */
};
//_Soil_evaporation_without_vapor_path______________________________2009-01-06_/
}//_namespace_Soil_____________________________________________________________/
#endif

