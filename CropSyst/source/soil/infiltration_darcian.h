#ifndef SOIL_INFILTRATION_DARCIAN_H
#define SOIL_INFILTRATION_DARCIAN_H

#include "soil_infiltration.h"
//______________________________________________________________________________
class Soil_infiltration_cascade_darcian
: public  Soil_infiltration_cascade
{
public:
   Soil_infiltration_cascade_darcian(Soil_base &i_soil);

   virtual float64 infiltrate
   (float64                       management_irrigation
   ,float64                       management_auto_irrigation_concentration
   ,Pond                         &pond                                           //991102
   ,const CORN::Date             &today
   ,Dynamic_water_entering_soil  &water_entering_in_intervals_mm                 //990204
   ,const Dynamic_float_array    &runon_in_intervals                             //990409
   ,Dynamic_float_array          &runoff_in_intervals                            //990409
   );
      // Returns the amount of infiltration (m of water depth?)
 protected: // Darcian working methods
   float64 infiltration_darcian_cascade                                          //981214
      (float64    management_irrigation                                          //990216
      ,float64   &additional_runoff); // m any additional computed runoff

   float64 determine_flow_to_move                                                //990214
      (uint8      sublayer
      ,float64    entering_flow
      ,float64    reservoir
      ,float64    storage_content_limit);
   float64 determine_flow_out                                                    //990214
      (uint8      sublayer
      ,float64    total_flow_to_move
      ,float64   &reservoir);
   float64 determine_flux  // kg/(m2 sec)                                        //990214
      (uint8      sublayer
      ,float64    water_pot_adj_sublayer);
   void no_water_entering_soil_case();                                           //990214
};
//_Soil_infiltration_cascade_darcian___________________________________________/
#endif

