#ifndef soil_evaporatorH
#define soil_evaporatorH
#include "common/evaporator.h"
#include "soil/layers_I.h"
#include "soil/hydrology_I.h"
namespace Soil {                                                                 //200225
//______________________________________________________________________________
class Evaporator
: public extends_ CS::Evaporator_intervals                                       //160717
{
 protected: // conditions
   cognate_ bool fallow;
   cognate_ bool summer_time;
   cognate_ float64 mulch_cover_fraction;
 protected: //
   /*200225 In CropSyst proper we the value is actually yesterdays
      at the point we calculate soil temperature
   // Soil temperature needs  evap actual, but it is calculated before
   // actual evap so we have been using yesterdays value
   CORN::Dynamic_array<float64> evaporation_actual_yesterday;         // m
   */
   const Layers_interface                 &layers;
   const Hydraulic_properties_interface   &hydraulic_properties;

      // Warning soil_hydraulic_properties should actually be a pointer
      // because these properties could change (due to soil tillage)

   #ifndef __GNUC__
   cognate_
   #endif
         Hydrology_interface &hydrology;
 public:
   Evaporator
      (modifiable_ CORN::Dynamic_array<float64>    &evap_potential_remaining_ //160717
      ,const       Layers_interface                &layers_
      ,const       Hydraulic_properties_interface  &hydraulic_properties_
      ,modifiable_ Hydrology_interface             &hydrology_);
 public:
   virtual float64 evaporate_interval(nat8 interval)              modification_;
   /*200225 already implemented
   inline virtual float64 get_evaporation_actual_hour(CORN::Hour hour)     const //200225
      { return evaporation_actual.get(hour); }                                   //200225
   */
   /*200225
   inline virtual float64 get_evaporation_actual_hour_yesterday(CORN::Hour hour) const
      { return evaporation_actual_yesterday.get(hour); }
   */
 public:
   bool end_day()                                                 modification_;
 public:
   bool know_conditions
      (bool fallow
      ,bool summer_time
      ,float64 mulch_cover_fraction)                                 cognition_;
};
//_Evaporator_soil__________________________________________________2016-06-06_/
}//_namespace_Soil_____________________________________________________________/
#endif
