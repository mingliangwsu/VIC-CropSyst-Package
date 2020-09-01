#include "soil/soil_evaporator.h"
#include "corn/math/moremath.h"
namespace Soil {                                                                 //200225
//______________________________________________________________________________
Evaporator::Evaporator
(modifiable_ CORN::Dynamic_array<float64>          &evap_potential_remaining_
,const       Layers_interface                      &layers_
,const       Hydraulic_properties_interface        &hydraulic_properties_
,modifiable_ Hydrology_interface                   &hydrology_)
: CS::Evaporator_intervals                           (evap_potential_remaining_)
, layers                                                               (layers_)
, hydraulic_properties                                   (hydraulic_properties_)
, hydrology                                                         (hydrology_)
/*200225 the current value is yesterday's value where used in soil temperature
, evaporation_actual_yesterday                                                () //160804
*/
{}
//_Evaporator_soil:constructor______________________________________2016-06-06_/
float64 Evaporator
::evaporate_interval(nat8 interval)                                modification_
{
   float64 evap_requested_now = evaporation_potential_remaining.get(interval);   //160719
   // This was in Soil_infiltration_cascade_common
   float64 PWP_1 = hydraulic_properties.get_permanent_wilt_point_volumetric(1);
   float64 PWP_2 = hydraulic_properties.get_permanent_wilt_point_volumetric(2);
   float64 thickness_1 = layers.get_thickness_m(1);
   float64 thickness_2 = layers.get_thickness_m(2);
   float64 FC_2 = hydraulic_properties.get_field_capacity_volumetric(2);         //051204
   float64 mid_capacity = PWP_2 + ((FC_2 - PWP_2) * 0.75);
   float64 pot_evap = evap_requested_now;                                        //160413
   pot_evap *= (1.0-mulch_cover_fraction);                                       //100620
      // mulch here is material other than residue (i.e. plastic cover)
   evaporation_potential_remaining.deduct_at(interval,pot_evap);                 //160719
   evaporation_potential.set(interval,pot_evap);                                 //160805
   float64 air_dry_water_content = PWP_1 /3.0;
   float64 WC_liquid_1 = hydrology.get_liquid_water_content_volumetric(1);       //990311
   float64 evap_1
      = (WC_liquid_1 < PWP_1)                                                    //990311
      ? (pot_evap * CORN_sqr((WC_liquid_1 - air_dry_water_content)               //990311
        / (PWP_1 - air_dry_water_content)))
      : pot_evap;
   if ((WC_liquid_1-evap_1/thickness_1)< air_dry_water_content)                  //990311
       evap_1 = (WC_liquid_1-air_dry_water_content)*thickness_1;
   if (evap_1 < 0.0)                                                             //061205
      evap_1 = 0;  // Dust mulching  may temporarily have one of the watercontent terms out of sync
   float64 evap_volumetric = - evap_1 / thickness_1;  // Negative                //070814
   hydrology.change_liquid_water_content_by_volumetric(1,evap_volumetric);       //070814
   // During fallow periods, the soil is assumed to dry deeper                   //960530
   // into second layer, but not as dry as the first layer.
   float64 evap_2 = 0.0;
   if (fallow && summer_time)
   {
      pot_evap -= evap_1;
      evap_2 = pot_evap;                                                         //981209
      // Can dry 25% of available water
      float64 WC_liquid_2 =
         hydrology.get_liquid_water_content_volumetric(2);                       //981209
      evap_2 =
         ((WC_liquid_2 - evap_2 / thickness_2) < mid_capacity)
         ? CORN::must_be_0_or_greater<float64>
            ((WC_liquid_2 - mid_capacity) * thickness_2)
         : pot_evap;
      float64 evap_volumetric = - evap_2 / thickness_2;                          //070814
         // // Negative value
      hydrology.change_liquid_water_content_by_volumetric
         (2,evap_volumetric);                                                    //070814
   }
   if (evap_2 < 0.0)                                                             //061205
   {  evap_2 = 0;   // Normally this shouldn't happen
      // clog << "Warning negative evaporation in soil layer 2, temporary artifact of dust mulching - automatically corrected" << endl;
   }
   float64 evaporation =  (evap_1 + evap_2);                                      //980730
   evaporation_actual.set(interval,evaporation);                                 //160728LML
   return evaporation_actual.get(interval);                                      //160719
}
//_evaporate_interval_______________________________________________2016-06-06_/
bool Evaporator::know_conditions
(bool fallow_
,bool summer_time_
,float64 mulch_cover_fraction_)                                       cognition_
{
   fallow               = fallow_;
   summer_time          = summer_time_;
   mulch_cover_fraction = mulch_cover_fraction_;
   return true;
}
//_know_conditions__________________________________________________2016-06-06_/
bool Evaporator::end_day()                                         modification_
{
   /*200225
   evaporation_actual_yesterday.copy(evaporation_actual);
   */
   return CS::Evaporator_intervals::end_day();
}
//_end_day__________________________________________________________2016-08-04_/
}//_namespace_Soil_____________________________________________________________/
