#include "soil/soil_I.h"
#include "soil/hydrology_A.h"
#include "soil/hydraulic_properties.h"
#include "soil/freezing_I.h"
#include "soil/structure_I.h"
//200810 #include "physics_constants.h"
#include "corn/math/compare.h"
#include <math.h>
#ifdef CS_OBSERVATION
#include "csvc.h"
#include "CS_suite/observation/CS_inspector.h"
#include "CS_suite/observation/CS_optation.h"
#endif
#define CSVP_UNKNOWN 0
namespace Soil {                                                                 //181206
//______________________________________________________________________________
Hydrology_abstract::Hydrology_abstract
(const Layers_interface                &soil_layers_ref_
, const Hydraulic_properties_interface &hydraulic_properties_ref_
, const Structure                      &soil_structure_)
:Hydrology_interface()
,soil_layers_ref                                              (soil_layers_ref_)
,hydraulic_properties_ref                            (hydraulic_properties_ref_)
,freezing_profile_optional                                                   (0)
      // set by know_freezing_profile
,/*liquid_*/water_content_hourly_available                               (false) //071017
,soil_structure                                                (soil_structure_) //071001
{  clear_layer_array64(ice_water_content);
   clear_layer_array64(ice_content_yesterday);
   clear_layer_array64(plant_avail_water);
   clear_layer_array64(reference_water_content);
   clear_layer_array64(real_wilt_point);
   for (Hour hour = 0; hour < 24; hour++)                                        //070117
         clear_layer_array64(water_content_hourly[hour]);                        //070813
   for (int i = 0; i < 2; i++)                                                   //150123
   {
      depth_profile_water[i] = std::numeric_limits<float64>::quiet_NaN();
      depth_profile_water_quality[i] = CORN::not_valid_quality;
   }
}
//_Hydrology_abstract:constructor______________________________________________/
Hydrology_abstract::Hydrology_abstract
(const Hydrology_abstract   &copy_from
,float32                     fraction)
: soil_layers_ref                          (copy_from.soil_layers_ref)
, freezing_profile_optional                (copy_from.freezing_profile_optional)
, soil_structure                           (copy_from.soil_structure)
, hydraulic_properties_ref                 (copy_from.hydraulic_properties_ref)
{
   // All these members are volumetric so do not need to be fractioned.
   copy_layer_array64(ice_water_content        ,copy_from.ice_water_content);
   copy_layer_array64(ice_content_yesterday    ,copy_from.ice_content_yesterday);
   copy_layer_array64(water_flux_moved_to_FD   ,copy_from.water_flux_moved_to_FD);
   copy_layer_array64(plant_avail_water        ,copy_from.plant_avail_water);
   copy_layer_array64(reference_water_content  ,copy_from.reference_water_content);
   copy_layer_array64(real_wilt_point          ,copy_from.real_wilt_point);
   // The liquid water content by hour is recorded for moisture function used in denitrification, and soil temperature hourly
   water_content_hourly_available = copy_from.water_content_hourly_available;
      for (Hour hour = 0; hour < 24; hour++)
         copy_layer_array64(water_content_hourly[hour],copy_from.water_content_hourly[hour]);
   // This will be updated as used depth_profile_water[2];                       //130930
}
//_Hydrology_abstract:constructor___________________________________2014-06-10_/
void Hydrology_abstract::expand_last_layer_for_boundary_conditions
( nat8 number_layers, nat8 number_expanded_layers)                 modification_
{  for ( nat8 extended_layer = number_layers+1; extended_layer <= number_expanded_layers; extended_layer++)
   {  ice_water_content       [extended_layer] = ice_water_content         [number_layers];
      ice_content_yesterday   [extended_layer] = ice_content_yesterday     [number_layers];
      water_flux_moved_to_FD  [extended_layer] = water_flux_moved_to_FD    [number_layers];
      plant_avail_water       [extended_layer] = plant_avail_water         [number_layers];
      reference_water_content [extended_layer] = reference_water_content   [number_layers];
      real_wilt_point         [extended_layer] = real_wilt_point           [number_layers];
   // Don't need to do this one, it is more of an output water_content_hourly[24][MAX_soil_layers_alloc];
   }
   invalidate();                                                                 //130930
}
//_expand_last_layer_for_boundary_conditions___________________________________/
float64 Hydrology_abstract::get_water_plus_ice_content_volumetric(nat8 layer) const
{  return get_liquid_water_content_volumetric(layer) + get_ice_content_volumetric(layer);   //080128
}
//_get_water_plus_ice_content_volumetric____________________________2007-08-27_/
float64 Hydrology_abstract::get_water_plus_ice_content_gravimetric(nat8 layer) const
{  float64 all_WC_vol = get_water_plus_ice_content_volumetric(layer);
   float64 bulk_density_kg_m3 = soil_structure.get_bulk_density_kg_m3(layer);
   float64 all_WC_grav = all_WC_vol * water_density / bulk_density_kg_m3;
   return all_WC_grav;
}
//_get_water_plus_ice_content_gravimetric___________________________2007-10-01_/
bool Hydrology_abstract::is_saturated( nat8 layer)                        const
{  // used only in old lateral flow model
   float64 liquid_WC = get_liquid_water_content_volumetric(layer);                //070813
   float64 sat_WC =
         hydraulic_properties_ref.
         get_saturation_water_content_volumetric(layer,ICE_WATER_CONTENT_RELEVENCE_UNKNOWN_140822);
   return liquid_WC >= sat_WC;
}
//_is_saturated_____________________________________________________2006-05-08_/
float64 Hydrology_abstract::get_water_filled_porosity
( nat8 layer, Hour for_hour)                                               const
{  float64 wc =
      (for_hour == 24)                                                           //070117
      ? get_water_plus_ice_content_volumetric(layer) // claudio conf             //070827
      : get_water_plus_ice_content_volumetric_at(layer,for_hour); //claudio conf //070827
   return  wc / hydraulic_properties_ref.get_saturation_water_content_volumetric
      (layer,ICE_WATER_CONTENT_RELEVENCE_UNKNOWN_140822);
}
//_get_water_filled_porosity________________________________________2004-05-18_/
float64  Hydrology_abstract::get_air_entry_pot        // This could be moved to Hydrology_abstract .  Normally it is a hydraulic_property (constant), but it is adjusted for ice content.
( nat8 sublayer,bool consider_ice_content,bool FC_PWP_based)               const //990321

{  float64 air_entry_pot_sl = hydraulic_properties_ref.get_air_entry_potential   //990321
      (sublayer,FC_PWP_based);
   nat8 num_sublayers = soil_layers_ref.count();                                 //060505
   if (num_sublayers && // dont check this condition when initializing the soil //991030
       (sublayer > num_sublayers))
   // Special case for F.D. which looks a sublayers + 1                          //991030
         air_entry_pot_sl = get_air_entry_pot(sublayer-1,consider_ice_content,FC_PWP_based);//991030
   else                                                                          //991030
   {  float64 soil_b_sl = hydraulic_properties_ref.get_Campbell_b(sublayer,FC_PWP_based);  //990321
      float64 ice_content_sl =                                                   //990321
         get_ice_content_volumetric(sublayer);                                   //060505
      if ((ice_content_sl > 0.0) && consider_ice_content )                       //990321
      {  float64 sat_WC_ice_adjusted = hydraulic_properties_ref                  //140822_051204
            .get_saturation_water_content_volumetric(sublayer,ice_content_sl);
         float64 sat_WC_total =        hydraulic_properties_ref                  //051204
            .get_saturation_water_content_volumetric(sublayer,0.0);
         air_entry_pot_sl =                                                      //990321
            (ice_content_sl > sat_WC_ice_adjusted)                               //140822_990331
            ? 0.0 /*air_entry_pot_sl*/                                           //990331
            :  // 050729 Was returning 0.0, but this causes div/0 in caller.     //990331
               // WARNING not sure what to return in this case, but it can't be 0.0
              air_entry_pot_sl *                                                 //990321
              pow (sat_WC_ice_adjusted / sat_WC_total,- soil_b_sl);              //140822_990321
      }
   }
   return air_entry_pot_sl;
}
//_get_air_entry_pot________________________________________________1998-10-30_/
float64 Hydrology_abstract::calc_water_potential
   ( nat8 layer,bool consider_ice,bool FC_PWP_based)                       const
{ // Water retention curve.
   float64 water_content_sublayer = get_liquid_water_content_volumetric(layer);  //990316
   float64 saturation_water_content_sublayer                                     //980730
           = hydraulic_properties_ref.get_saturation_water_content_volumetric
            (layer,ICE_WATER_CONTENT_RELEVENCE_UNKNOWN_140822);                  //051204
   float64 air_entry_pot_sublayer = get_air_entry_pot(layer,consider_ice,FC_PWP_based);     //980730
   float64 b_sublayer = hydraulic_properties_ref.get_Campbell_b(layer,FC_PWP_based);       //980730
   float64 ice_content_sl = consider_ice                                         //990315
     ?get_ice_content_volumetric(layer)                                          //060505
      : 0.0;                                                                     //990315
   float64 sat_water_cont = saturation_water_content_sublayer - ice_content_sl;  //990331
   float64 result =
     (  (water_content_sublayer < saturation_water_content_sublayer)
      &&(water_content_sublayer > 0.0000001)  //prevent FP overflow              //990606
      &&(sat_water_cont > 0.0)) // prevent div 0                                 //041111
     ? (air_entry_pot_sublayer * pow((water_content_sublayer/ sat_water_cont),-b_sublayer)) //990331
     : air_entry_pot_sublayer;
   /*std::clog<<"L:"<<(int)layer
            <<"\tsat_w_iceadj:"<<sat_water_cont
            <<"\tsat_w:"<<saturation_water_content_sublayer
            <<"\tb:"<<b_sublayer
            <<"\tvwc:"<<water_content_sublayer
            <<"\tvwc/sat:"<<water_content_sublayer/sat_water_cont
            <<std::endl;*/
   return result;
}
//_calc_water_potential________________________________________________________/
float64 Hydrology_abstract::calc_available_water_fraction
(nat8 sublayer , bool real_wilt_based)                              calculation_
{   // 051204 May move this to hydraulic_properties but now sure about real_wilt_point
   float64 field_capacity_sl=hydraulic_properties_ref.get_field_capacity_volumetric(sublayer);  //051204
   float64 perm_wilt_sl     =hydraulic_properties_ref.get_permanent_wilt_point_volumetric(sublayer);  //051204
   float64 wilt_point_sl = real_wilt_based
      ? real_wilt_point[sublayer]
      : perm_wilt_sl;
   float64 avail_water_sl =  CORN::must_be_between<float64>                      //990217
   (((get_liquid_water_content_volumetric(sublayer) - wilt_point_sl)             //010523
    /(field_capacity_sl       - wilt_point_sl))
   ,0.0,1.0);                                                                    //990217
// WARNING this is just a stop gap, the liquid water content under water table must not be setting to saturation.
   if (sublayer > soil_layers_ref.count_uninundated())
      avail_water_sl = 1.0;
   return avail_water_sl;                                                        //990217
}
//_calc_available_water_fraction____________________________________2006-05-09_/
float64 Hydrology_abstract ::calc_available_water_depth_to_layer
(nat8 to_layer)                                                     calculation_
{  float64 depth_layer_bottom = soil_layers_ref.get_depth_m(to_layer);
   return calc_available_water_depth_to_depth(depth_layer_bottom);
}
//_calc_available_water_depth_to_depth______________________________2016-06-01_/
float64 Hydrology_abstract ::calc_available_water_depth_to_depth
(float64 depth)                                                     calculation_
{  float64 AW_to_depth = 0;
   float64 remaining_depth = depth;
   nat8 to_layer = soil_layers_ref.get_layer_at_depth_or_last_layer(depth);      //160722
   for (nat8 lyr = 1; lyr < to_layer; lyr++)
   {  float64 lyr_thickness = soil_layers_ref.get_thickness_m(lyr);
      float64 perm_wilt_lyr = hydraulic_properties_ref.get_permanent_wilt_point_volumetric(lyr);  //051204
      float64 WC_lyr        = get_liquid_water_content_volumetric(lyr);
      float64 AW_volumetric = WC_lyr - perm_wilt_lyr;
      float64 AW_depth_layer = AW_volumetric * lyr_thickness;
      AW_to_depth += AW_depth_layer;
      remaining_depth -= lyr_thickness;
   }
   // add bit in final layer
   float64 perm_wilt_last     = hydraulic_properties_ref.get_permanent_wilt_point_volumetric(to_layer);  //051204
   float64 WC_last            = get_liquid_water_content_volumetric(to_layer);
   float64 AW_volumetric_last = WC_last - perm_wilt_last;
   float64 lyr_thickness_last = soil_layers_ref.get_thickness_m(to_layer);       //170626brc

   float64 AW_depth_layer_last= AW_volumetric_last * lyr_thickness_last;         //170626brc
   AW_to_depth += AW_depth_layer_last;
   return AW_to_depth;
}
//_calc_available_water_depth_to_depth______________________________2016-06-01_/
float64 Hydrology_abstract::calc_ice_depth_to(float64 depth)        calculation_
{  return 0;
}
//_calc_ice_depth_to________________________________________________2016-07-24_/
float64 Hydrology_abstract::get_ice_content_volumetric_yesterday(nat8 layer) const
{  return freezing_profile_optional ? ice_content_yesterday[layer] : 0.0;
}
//______________________________________________________________________________
float64 Hydrology_abstract::get_ice_content_volumetric(nat8 layer)         const
{  return ice_water_content[layer];
}
//_get_ice_content_volumetric__________________________________________________/
bool Hydrology_abstract::is_frozen( nat8 layer)                            const
{  return freezing_profile_optional
   ? freezing_profile_optional->is_frozen(layer) : false;
}
//_is_frozen________________________________________________________2006-10-05_/
void Hydrology_abstract::record_water_content_for_hour(Hour hour) // Note that water content should have been previously stored.
{  water_content_hourly_available = true;
   for ( nat8 layer = 0; layer <= MAX_soil_layers; layer++)                      //080127
       water_content_hourly[hour][layer] = get_liquid_water_content_volumetric(layer); //080127
   // Do also for Ice.
}
//_record_water_content_for_hour____________________________________2007-01-17_/
bool Hydrology_abstract::start_day()
{  /*liquid_*/water_content_hourly_available = false;
   soil_layers_ref.copy_array64( ice_content_yesterday,ice_water_content);       //070825
   invalidate();                                                                 //130930
   return true;                                                                  //131125
}
//_start_day________________________________________________________2007-01-17_/
void Hydrology_abstract::update_liquid_and_ice_water_content()modification_
{  // this used to be done by soil_freezing
   if (!freezing_profile_optional) return;      // Note once I add freezing_and_thawing method (moved from land unit simulation, this check will be moved
   for ( nat8 layer = 1; layer <= soil_layers_ref.count(); layer++)
   {  float64 old_liquid_water_WC = get_liquid_water_content_volumetric(layer);  //080128
      float64 new_liquid_water_WC = get_liquid_water_content_volumetric(layer);  //080128
      freezing_profile_optional
         ->calc_liquid_and_ice_water_content_volumentric
         (layer,new_liquid_water_WC,ice_water_content[layer]);                   //080128_
      float64 change_WC = new_liquid_water_WC - old_liquid_water_WC;             // 080127 RLN warning need to check this not sure if old diff new or new diff old
      change_liquid_water_content_by_volumetric(layer, change_WC) modification_; //080128
   }
   invalidate();                                                                 //130930
}
//_update_liquid_and_ice_water_content______________________________2007-08-25_/
float64 Hydrology_abstract::get_water_plus_ice_content_volumetric_at
(nat8 layer,Hour hour)                                                     const
{  float64 result = 0;
   result =  ((water_content_hourly_available)
      ? water_content_hourly[hour][layer]
      : get_liquid_water_content_volumetric(layer) )
    +  ice_water_content[layer];
   return result;
}
//_get_water_plus_ice_content_volumetric_at_________________________2009-08-13_/
#ifdef UNUSED_code
080125 This currently appears to be unused but it may be needed for future use..
float64 Soil_base::get_hydraulic_conductivity( nat8 sublayer )
{  float64 sat_hydraul_cond_kg_s_m3_sl =
      hydraulic_properties->get_sat_hydraul_cond_kg_s_m3(sublayer);              //051204
   float64 air_entry_pot_sl = hydrology->get_air_entry_pot(sublayer,false,false);//990321
   float64 water_pot_sl =
      hydrology->get_water_pot(sublayer);                                        //060505
   float64 n_sl = 2.0 + 3.0 / get_Campbell_b(sublayer,false);                    //990321
   return (water_pot_sl < air_entry_pot_sl)
   ?  sat_hydraul_cond_kg_s_m3_sl  * pow(air_entry_pot_sl/water_pot_sl,n_sl)
   :  sat_hydraul_cond_kg_s_m3_sl;
}
//_get_hydraulic_conductivity_______________________________________1998-01-16_/
#endif
float64 Hydrology_abstract ::calc_water_depth_to_layer
(bool all_water ,nat8 to_layer)                                            const
{  float64 water_depth_profile = 0.0;
   if (to_layer == PROFILE_LAYER_INDEX)                                          //181105
      to_layer = soil_layers_ref.count();                                        //181105
   for (nat8 i = 1 ; i <= to_layer; i++)
   {  float64 wc_sl = all_water
            ? get_water_plus_ice_content_volumetric(i)
            : get_liquid_water_content_volumetric(i);
        water_depth_profile += wc_sl * soil_layers_ref.get_thickness_m(i);
   }
   return water_depth_profile;
}
//_calc_water_depth_to_layer________________________________________2018-11-05_/
float64 Hydrology_abstract ::calc_water_depth_to_depth(bool all_water, float32 depth) const
{  nat8 to_layer = get_soil_layers().get_layer_at_depth_or_last_layer(depth);
   return calc_water_depth_to_layer(to_layer);
}
//_calc_water_depth_to_depth________________________________________2018-11-05_/
float64 Hydrology_abstract::provide_water_depth_profile(bool all_water) provision_
{
   if (!is_valid_quality(depth_profile_water_quality[all_water]))                //150123
   {  depth_profile_water[all_water] =
         calc_water_depth_to_layer(all_water,PROFILE_LAYER_INDEX);               //181105
      depth_profile_water_quality[all_water] = simulated_quality;                //150123
   }
   return depth_profile_water[all_water];
}
//_provide_water_depth_profile______________________________________2013-09-30_/
void Hydrology_abstract::invalidate()                              contribution_
{  for (int i = 0; i < 2; i++) depth_profile_water_quality[i] = CORN::not_valid_quality;  //150124
}
//_invalidate_______________________________________________________2013-09-30_/
double Hydrology_abstract::get_K_Theta
(nat8 layer
,double Theta, double Theta_s)                                             const
// RLN these should be members
//, double K_Sat, double Campbell_b,double Theta_b, double MacPoFac

{
    //with the optional arguments the equation could still be used for calculating the original Campbell
    //Theta, Theta_s, Theta_b [cm/cm]
    //K_sat [Length/Time]
    //Campbell b [unitless]
    //MacPoFac = macropore factor, which is used as a multiplyer to increase Ks to account for macro-pore abundance [unitless]

    float64 K_sat      = hydraulic_properties_ref.get_sat_hydraul_cond_m_hr(layer);
    float64 Campbell_b = hydraulic_properties_ref.get_Campbell_b(layer,false);
    float64 Theta_b    = calc_Theta_b(layer,Campbell_b);
    float64 K_Theta_b  = K_sat *
      pow(Theta_b / Theta_s
         ,(2.0 * Campbell_b + 3.0));
    if(CORN::is_approximately<float64>(Theta_b,0.0,1E-8)) Theta_b = Theta_s;
   //140305 claudio says we won't use this
   // macro_pore_factor = hydraulic_properties_ref.get_macro_pore_factor();
    float64 K_Theta =
      (Theta < Theta_b)
      ? K_sat * pow(Theta / Theta_s, (2.0 * Campbell_b + 3.0))
      : CORN::is_approximately<float64>(Theta_b,Theta_s,1E-8)
         ?  /*macro_pore_factor * */ K_sat
         :  (Theta - Theta_b) / (Theta_s - Theta_b)
          * (/*macro_pore_factor * */ K_sat - K_Theta_b) + K_Theta_b;
            //analogous to the approach described in Mehta et al. 2004 Environmental Modeling and Assessment 9: 77-89, 2004.
    return K_Theta;
}
//_get_K_Theta______________________________________________________2014-03-05_/
float64 Hydrology_abstract::calc_Theta_b
(nat8 layer
,float64 Campbell_b)                                                       const
{
   float64 air_entry_potential = hydraulic_properties_ref
      .get_air_entry_potential(layer,false);
   float64 saturation_water_content_volumetric = hydraulic_properties_ref
      .get_saturation_water_content_volumetric
         (layer,ICE_WATER_CONTENT_RELEVENCE_UNKNOWN_140822);
   float64 Theta_b =
      calc_Theta_h_Campbell
         ( calc_h_Theta_b(air_entry_potential)
         , air_entry_potential
         , saturation_water_content_volumetric
         ,Campbell_b);
   return Theta_b;
}
//_calc_Theta_b_____________________________________________________2014-03-05_/
float64 Hydrology_abstract::calc_h_Theta_b
(float64 air_entry_potential)                                              const
{  // air_entry_potential J/kg
   //using empirical function to calculate h(Theta_b)
   float64  h_Theta_b = (air_entry_potential < -7.7)
       ? -10.0
       : -(-0.0179 * pow((air_entry_potential * (-10.0)),2.0)
           + 2.67  *     (air_entry_potential * (-10.0))) / 10.0;
   return h_Theta_b;
}
//_calc_h_Theta_b______________________________________________________________/
float64 Hydrology_abstract::calc_Theta_h_Campbell
(float64 h           // J/kg
,float64 Air_Entry   // J/kg
,float64 Theta_s     // cm/cm
,float64 Campbell_b)                                                       const
{   float32 Theta_h_Campbell = (h > Air_Entry)
      ? Theta_s
      : Theta_s * pow((Air_Entry / h),(1.0 / Campbell_b));
    return Theta_h_Campbell;
}
//_calc_Theta_h_Campbell_______________________________________________________/
float64 Hydrology_abstract::extract_water_mm
(float64 water_uptake_mm[] , nat8 start_layer)                     modification_
{  soil_layer_array64(water_uptake_m);
   for (nat8 l = 0; l < MAX_soil_layers_alloc; l++)
      water_uptake_m[l] = mm_to_m(water_uptake_mm[l]);
   return m_to_mm(extract_water(water_uptake_m,start_layer));
}
//_extract_water_mm_________________________________________________2014-06-24_/
float64 Hydrology_abstract::initialize_moisture
(nat8 layer
,Moisture_characterization moisture
,float64 WC_initial)                                               modification_
{
   if (CORN::is_zero<float64>(WC_initial) && !moisture)                          //170530brc
      moisture = average;
   switch (moisture)
   {
      case dry: WC_initial
         = hydraulic_properties_ref.get_permanent_wilt_point_volumetric(layer);  break;
      case wet: WC_initial
         = hydraulic_properties_ref.get_field_capacity_volumetric(layer);        break;
      case saturated: WC_initial
         = hydraulic_properties_ref.get_saturation_water_content_volumetric(layer,0.0); break;
      case average  : WC_initial
         = (hydraulic_properties_ref.get_field_capacity_volumetric(layer)
          + hydraulic_properties_ref.get_permanent_wilt_point_volumetric(layer))
         / 2.0;
      break;
      case specific : default : break;                                           //170829
   }
   return set_water_content_volumetric_layer(layer,WC_initial);
}
//_initialize_moisture______________________________________________2017-03-20_/
RENDER_INSPECTORS_DEFINITION(Hydrology_abstract )
{
   #ifdef CS_OBSERVATION
   // uses soil emanator                                                         //170317
   // Since currently the emanator is the soil (domain) the
   // ontology includes 'hydrology'
   inspectors.append(new CS::Inspector_scalar(depth_profile_water[1],UC_meter
      ,*context,"hydrology/water/depth&LEQ",value_statistic,CSVP_UNKNOWN));      //170319
      // Need to define ontology
   if (
      #if ((CS_VERSION > 0) &&(CS_VERSION < 6))
        (CS::optation_global.is_desired(CSVC_soil_base_plant_avail_water)) ||
      #endif
        (CS::optation_global.is_desired("hydrology/water/PAW")))
      inspectors.append(new CS::Inspector_vector
      (plant_avail_water,soil_layers_ref.count()
      ,UC_decimal_percent,*context,"hydrology/water/PAW",value_statistic
      ,CSVC_soil_base_plant_avail_water));

   /* NYI waterpotential is currently calculated as needed, could save profile values for output
   if (
      #if ((CS_VERSION > 0) &&(CS_VERSION < 6))
        (CS::optation_global.is_desired(CSVC_soil_base_water_potential)) ||
      #endif
        (CS::optation_global.is_desired("hydrology/water/potential")))
      inspectors.append(new CS::Inspector_vector
      (water_potential,soil_layers_ref.count()
      ,UC_m3_per_m3,*context,"hydrology/water/potential",value_statistic
      ,CSVP_soil_base_water_potential));
   */

   //170319 inspect the value for the profile, the tally will handle
   // initial and final
   /*170319
   // for start of period
   inspectors.append(new CS::Inspector_scalar(depth_profile_water[1],UC_meter
      ,*context,"water_depth/profile/LEQ",initial_statistic,CSVP_UNKNOWN));
   //for end of period
   inspectors.append(new CS::Inspector_scalar(depth_profile_water[1],UC_meter
      ,*context,"water_depth/profile/LEQ",final_statistic,CSVP_UNKNOWN));
   */
   #endif
   return 0; // emanator_known; // NYI
}
//_RENDER_INSPECTORS_DEFINITION_____________________________________2013-09-30_/
}//_namespace_Soil__________________________________________________2018-12-06_/

