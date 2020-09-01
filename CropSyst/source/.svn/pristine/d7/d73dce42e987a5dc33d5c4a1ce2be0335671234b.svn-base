#ifndef thermal_timeH
#define thermal_timeH

#ifndef primitiveH
#  include "corn/primitive.h"
#endif
#include "corn/const.h"
#include "options.h"
#include "crop/crop_param_class.h"
#include "corn/dynamic_array/dynamic_array_T.h"
//190812 #include "weather/parameter/WP_air_temperature.h"
#include "CS_suite/observation/CS_observation.h"

#define TT_TODAY     0
#define TT_YESTERDAY 1

namespace CropSyst {
//______________________________________________________________________________
interface_ Thermal_time_immutable  // was Crop_thermal_time_interface_immutable
{
   virtual float64 get_accum_degree_days
     #ifdef THERMAL_TIME_OBSOLETE_2020
      #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
      (bool adjusted_for_clipping
      ,bool for_yesterday = false)                                      const=0; //140619
      #endif
      #if ((PHENOLOGY_VERSION==2018))
      (bool adjusted_for_clipping_obsolete = false
      ,bool for_yesterday_obsolete = false)                             const=0; //140619
      #endif
     #else
      () const = 0; //200211
     #endif
   #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
   virtual const float64 &ref_thermal_time_cumulative()                 const=0;
   #endif
   // status accessors
   virtual float64 get_growing_degree_day()                             const=0;
};
//_interface_Thermal_time_immutable_________________________________2008-08-05_/
interface_ Thermal_time
: public extends_interface_ Thermal_time_immutable
{
   virtual double accumulate_degree_days
      #ifdef THERMAL_TIME_OBSOLETE_2020
      (
            // now member ref  CORN::Dynamic_array<float32> *stress_adj_hourly_temperature_optional,
      // The following are optional for unstress crop mode
      #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
      Normal_crop_event_sequence growth_stage_sequence
      #endif
      )                                                                      =0;
      #else
      ()=0; //200211
      #endif
   #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
   // in 2018 version we delete the affected periods
   virtual void reset_to(float64 new_thermal_time)              modification_=0; //040719
      // Some crops such as alfalfa we need to reset thermal time                //040719
      // (I.e. to emergence) when cut so they regrow.                            //040719
   #endif
   #ifdef THERMAL_TIME_OBSOLETE_2020
   virtual void respond_to_clipping(float64 new_thermal_time)   modification_=0; //040823
      // When annuals crops are clipped the thermal time accumulation is delayed //040823
   #endif
 public: // modifiers
   virtual void clear_accum_degree_days()                       modification_=0;
   virtual bool end_day()                                       modification_=0; //140619
};
//_interface_Thermal_time___________________________________________2015-11-06_/
}//_namespace_CropSyst_________________________________________________________/
#endif


