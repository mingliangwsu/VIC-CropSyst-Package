#ifndef infiltration_cascade_dailyH
#define infiltration_cascade_dailyH
#include "soil/infiltration_cascade_common.h"
namespace Soil {
//______________________________________________________________________________
class Infiltration_cascade_daily
: public  Infiltration_cascade_common                                            //070116
{public:
   Infiltration_cascade_daily
      (const Soil::Layers_interface                &soil_layers                  //140424
      ,const Soil::Hydraulic_properties_interface  &soil_hydraulic_properties    //140423
      ,Soil::Hydrology_cascade                     &hydrology                    //060504
      ,CropSyst::Soil_runoff                       *runoff_unused                //130308
      ,CropSyst::Pond                              &pond_unused);                //131001
   #if (DIRECTED_IRRIGATION==2016)
   Infiltration_cascade_daily
      (const Infiltration_cascade_daily &from_copy, float32 fraction);
   virtual Soil_infiltration_nominal *copy(float32 fraction)              const; ///140609
   #endif
   virtual bool infiltrate
      (float64  total_water_entering_soil_pot_not_irrigation
      ,float64  management_irrigation                                            //070107
      ,float64  management_auto_irrigation_concentration
      ,Dynamic_water_entering_soil  *non_runoff_water_entering_in_intervals_mm   //990204
      ,Dynamic_water_entering_soil  *irrigation_water_entering_in_intervals_mm   // optional (may be 0) //070119
      ,const Dynamic_array<float32>    &runon_in_intervals                       //990409
      ,Dynamic_array<float32>          &runoff_in_intervals)      modification_; //990409
      // Returns the amount of infiltration (m of water depth?)                  //030711
 private:                                                                        //030710
   virtual void cascade_pulse                                                    //981119
      (           float64 pulse_water_entering_soil_not_irrigation               //981119
      ,           float64 pulse_irrigation                                       //981119
      ,modifiable_ float64 old_weater_content[])                  modification_; //981119
   virtual float64 infiltration_cascade
      (float64 total_water_entering_soil_daily_not_irrigation                    //070107
      ,float64 irrigation = 0.0)                                  modification_; //980801
      // Default will be 0 if no management
};
//_Soil_infiltration_cascade_daily_____________________________________________/
}//_namespace_Soil_____________________________________________________________/
#endif

