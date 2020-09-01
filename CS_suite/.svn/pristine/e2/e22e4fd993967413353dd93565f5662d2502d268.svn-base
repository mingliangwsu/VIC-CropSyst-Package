#ifndef stress_ascertainationH
#define stress_ascertainationH

#include "CS_suite/simulation/CS_simulation_element.h"
#include "crop/growth_stages.h"
#include "crop/phenology_I.h"
#include "corn/dynamic_array/dynamic_array_T.h"

namespace CS
{
//_________________________________________________________________________
class Stress_ascertaination
: public CS::Simulation_element_abstract
{public:
    struct Parameters
    { nat16    window_size_days;  // I.e. 14 use 0xFFFF to indicate unlimited
      int16  extreme_temperature; // may be negative
         // using whole numbers for variable code composition
      ; bool high;
      Normal_crop_event_sequence stage_initiation;
    };
 private:
   const Parameters &parameters;
   nat32 variable_code;
 protected: // references
   bool                                 window_size_days_acheived;
   const CropSyst::Phenology           &phenology;
   const CORN::Dynamic_array<float32>  &hourly_temperature;
      // The array extent represents the period
      // with each entry the daily degree hours in extreme range.
 public:
   CORN::Dynamic_array<float32>         degree_hours_of_extreme;
 public: // structors
   Stress_ascertaination
      (const Parameters                      &parameters_
      ,const CropSyst::Phenology             &phenology_
      ,const CORN::Dynamic_array<float32>    &hourly_temperature_
         // For CropSyst flowering heat stress this is stress adjusted hourly
         // plant temperature, otherwise it can be simply air temperature.
      ,const CORN::date32                    &today_bare_);
 public:
   float32 degree_hours_of_extreme_today; // potential output (otherwise could be local).
 protected:
   virtual bool end_day()                                         modification_;
 public:
   RENDER_INSPECTORS_DECLARATION;
};
//_Stress_ascertaination____________________________________________2018-09-14_/
}//_namespace_CS_______________________________________________________________/
#endif

