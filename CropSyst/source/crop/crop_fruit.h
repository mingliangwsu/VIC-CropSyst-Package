
// This file should actually be named Crop_orchard_fruit

#ifndef crop_fruitH
#define crop_fruitH
#include "common/weather/parameter/WP_psychrometric_const.h"
#include "corn/primitive.h"
#include "crop/crop_cropsyst.h"
#ifdef NITROGEN
#include "common/biometeorology/parameter/slope_SVP_curve_psychrometric_resistance.h"
#endif

#include "crop/yield.h"
#include "common/weather/seasons.h"
class ET_reference_interface;
class Weather_provider;
class Sun;
namespace CropSyst {

class Orchard_interface;
//______________________________________________________________________________
class Fruit_chill_requirement
{
   float32 chill_hours_remaining;  // rename name this to chill units
 public:
   inline Fruit_chill_requirement(float32 chill_hours_needed)
      : chill_hours_remaining(chill_hours_needed)
      {}
   inline bool is_satisfied()  affirmation_ { return chill_hours_remaining < 0;}
   inline set_chill_hours_reminning_negtive() {chill_hours_remaining = -1.0;}     //230707LML special cases the chill requirement never satisfied
 public:
   float32 add_hour_temperature(float64 temperature);                            //190709
      /// \returns correspoinding chill units deducted from chill_hours_remaining
   inline float32 get_chill_hours_remaining() const { return chill_hours_remaining;}
   // convert temperature to chill units and deduct
   //chill units from chill_hours remaining.
};
//_class Fruit_chill_requirement____________________________________2002-11-24_/
class Fruit_bud_break_requirement
{private:
   float32 deg_day_bud_break_chill_req_sat;     //copy from crop parameter       //030515
   float32 deg_day_bud_break_chill_req_not_sat; //copy from crop parameter       //030515
 public:
   inline Fruit_bud_break_requirement
      (float32 deg_day_bud_break_chill_req_sat_                                  //030515
      ,float32 deg_day_bud_break_chill_req_not_sat_)                             //030515
      : deg_day_bud_break_chill_req_sat(deg_day_bud_break_chill_req_sat_)
      , deg_day_bud_break_chill_req_not_sat(deg_day_bud_break_chill_req_not_sat_)
      {}
   bool is_satisfied(bool chill_req_satisfied,float32 accum_deg_days) affirmation_;
   //190708 bool satisfied(bool chill_req_satisfied,float32 accum_deg_days)     mutates_;
};
//_class Fruit_bud_break_requirement________________________________2002-11-24_/
class Fruit_harvest_requirements
{public:
   const CropSyst::Crop_parameters::Fruit &parameters;
   mutable CORN::Days consecutive_days_below_threshold_temperature;
 public:
   Fruit_harvest_requirements(const CropSyst::Crop_parameters::Fruit &parameters);
   bool is_satisfied(DOY doy,float64 min_temp,float64 avg_temp)    affirmation_;
   //190708 bool satisfied(DOY doy,float64 min_temp,float64 avg_temp)           mutates_;
};
//_class Fruit_harvest_requirements_________________________________2008-12-04_/
class Crop_orchard_fruit // was Crop_CropSyst_orchard_fruit
: public extends_ Crop_complete
{public:
   //___________________________________________________________________________
   class Yield_model   // cannot use name Yield because
   : public implements_ CropSyst::Yield_model
   {public:
      /*cognate_*/ float64 &fruit_dry_biomass;  // modified
    public:
      Yield_model(float64 &fruit_dry_biomass_);
      virtual float64 calculate(float64 canopy_biomass_kg_m2)      calculation_;
      virtual float64 get_reported_harvest_index()                        const
         { return 0; }
         // harvest index not applicable to fruit model
         // but could be calculated for completeness
   };
   //_class Yield_model_____________________________________________2017-02-20_/
 private:
   const Crop_parameters_struct::Tolerance &tolerance_parameters;                //190616
   CropSyst::Crop_parameters::Fruit *fruit_parameters_damage_adjusted;           //081103
   mutable float64 max_fruit_dry_biomass;                                        //200608
   float64 latest_harvest_fruit_dry_biomass;                                     //200608
   bool max_fruit_load_achieved;                                                 //200709

   const CS::Weather_hours     *ref_hourly_weather;                              //180612

      // deleted and reset each season.
   bool limit_max_LAI;                                                           //081103
   float64                day_length_hours_unused;                               //140812
         // day_length_hours are used for photoperiod which is not appicable to fruit crops //140812
   bool harvested;                                                               //200603
      // I harvested is now only used for status output -- checking
 protected:
   const CS::Seasons &seasons;                                                   //190705
   Orchard_interface *orchard;                                                   //071116
   struct Linger_label                                                           //200420
   {
      sint16 chill_satisfied;                                                    //030529
      sint16 budbreak;                                                           //030521
      sint16 veraison;                                                           //030521
   } linger_label;                                                               //200420
 public:
   mutable float64  fruit_dry_biomass;    // kg/m2  dry basis                    //000616
   float64  reserves_biomass;       // kg/m2  dry basis                          //071117
   //200603obs float64  fruit_root_turnover_biomass;  // kg/m2  dry basis        //071117
 public: // for fruit growth mass calculations:
   float64  current_fruit_count;                                                 //071122
      // I think this is the number of fruits per tree/plant
      //(I don't think per hectare)  (could be a crop parameter)
   CORN::Date_clad_32 initial_fruit_growth_date ;                                //030521
   CORN::Date_clad_32 veraison_date ;                                            //030521
   CORN::Date_clad_32 rapid_fruit_growth_date ;                                  //030521
   CORN::Date_clad_32 bud_break_date;                                            //081013
   CORN::Date_clad_32 chill_requirement_satisfied_date;                          //081013
 private:
   virtual bool setup_quiescence_end_requirement();                              //200402
      //200402 (CORN::DOY doy);            //080401
   Fruit_chill_requirement       *fruit_chill_requirement;                       //021125
      // This is 0 when the crop does not have chill requirement
      // (applies only to fruit crops)
   Fruit_bud_break_requirement   *fruit_bud_break_requirement;                   //021125
      // This is 0 when the crop does not have bud_break_requirement
      // (applies only to fruit crops)
   Fruit_harvest_requirements       *fruit_harvest_requirements;                 //180612
 public: // 'structor
   Crop_orchard_fruit // was Crop_CropSyst_orchard_fruit
      (CropSyst::Crop_parameters    *parameters_                                 //020409
      ,float64 param_dry_soil_root_activity_coef_                                //170518
      ,float64 param_saturated_soil_root_activity_coef_                          //170518
      ,bool                          owns_parameters_                            //040121
      ,const CORN::date32 &today_                                                //170525
      ,CORN::Unidirectional_list             &performable_operations_            //151004
      ,const CS::CO2_atmospheric_concentration *atmospheric_CO2_conc_ppm_optional//180122
      ,modifiable_ Crop_CO2_response         *Crop_CO2_response_given_optional   //151201
      ,const Air_temperature_maximum         &air_temperature_max_               //151201
      ,const Air_temperature_minimum         &air_temperature_min_               //151201
      ,const Air_temperature_average         &air_temperature_avg_               //151201
      ,const Solar_radiation                 &solar_radiation_                   //151201
      ,const Vapor_pressure_deficit_daytime  &vapor_pressure_deficitdaytime_     //151201
      ,const Vapor_pressure_deficit_max      &vapor_pressure_deficit_max_        //151201
      ,const Psychrometric_constant          &psychrometric_constant_            //140508
      ,const float64                         &aerodynamic_resistance_ref_plants_ //140508
      ,const Sun                             &sun_                               //151128
      ,const Weather_spanning_interface      &weather_                           //151201_130901
      ,Soil::Soil_interface                  *soil_                              //181206_050721
      ,Soil::Nitrogen                        *soil_chem_                         //050721
      ,modifiable_ Soil::Salt_interface      *soil_salinity_                     //181206_051118

      ,Crop_nitrogen_model           nitrogen_model_                             //050722
      #ifdef NITROGEN
      ,const Slope_saturated_vapor_pressure_plus_psychrometric_resistance        //referenced
         *slope_saturated_vapor_pressure_plus_psychrometric_resistance_reference
//190619 unused       ,ET_reference_interface       *_ET
      #endif
      ,const CS::Seasons &seasons_ref                                            //190705
      ,Orchard_interface *orchard_optional);
   virtual ~Crop_orchard_fruit();
 private :
   virtual bool initialize();
   virtual bool start();                                                         //140916_071120
   //NYN virtual bool end_season()                                      modification_; //200609
   inline virtual bool is_fruit()                     affirmation_{return true;} //190707
   virtual Canopy_leaf_growth &provide_canopy()                      provision_; //190702
   virtual bool Crop_orchard_fruit::drop_leaves();                               //190625
   virtual bool process_day();                                    modification_; //180622
   virtual bool senesce_at_dormancy();                                           //071120
   virtual bool restart_after_inactive_period();                                 //190708
   void potential_fruit_biomass_growth()                                  const; //050818
   virtual float64 distribute_todays_non_leaf_stem_growth(float64 todays_growth) modification_; //071120
      // returns todays growth that is available to goto leaf and stem           //071120
   virtual bool initiate_fruit_growth ()                          modification_; //030521
   virtual bool initiate_veraison             ()                  modification_; //000625
   virtual bool initiate_rapid_fruit_growth   ()                  modification_; //030521
   virtual bool thermal_time_event()                              modification_;
   virtual float64 get_solid_canopy_interception()                        const; //190613
 public: // accessors for output
   inline virtual float64 get_fruit_dry_biomass_kg_ha()                   const
      { return over_m2_to_ha * fruit_dry_biomass; }

   inline virtual float64 get_latest_yield_kg_m2()                         const //200608
      { return latest_harvest_fruit_dry_biomass; }

   inline virtual float64 get_live_green_canopy_biomass_kg_m2(nat8 include_biomass) const //200603
   {  return Crop_complete::get_live_green_canopy_biomass_kg_m2(include_biomass)
         + fruit_dry_biomass
         + reserves_biomass;                                                     //200608
   }
   inline virtual float64 get_canopy_biomass_kg_ha()                      const  //200608
   { return per_m2_to_per_ha(get_canopy_biomass_kg_m2()+reserves_biomass); }
   inline virtual float64 get_fruit_reserves_biomass_kg_m2()              const  //200608
      { return reserves_biomass; }
   inline virtual float64 get_fruit_reserves_biomass_kg_ha()              const  //200608
      { return over_m2_to_ha * reserves_biomass; }
 protected:
   virtual void update_lingers()                                  modification_; //071120
   virtual bool consider_inactive_period();                                      //200402_020626
   #ifdef NYI
   // Never implemented, probably abandoned
   float64 calc_fruit_biomass_growth
      (float64 available_biomass_for_fruit_growth)                 calculation_; //071120
 private: // fruit growth methods
   float64 get_colimitation_curvature()                                   const;
   #endif
   virtual float64 calc_freezing_damage()                          calculation_; //190613
 public: // accessors
   virtual bool respond_to_freezing_damage
      (float64 fract_reduction_due_to_freezing)                   modification_; //081103
   virtual bool is_limit_max_LAI()                                 affirmation_; //081103
   virtual bool is_at_max_fruit_load()                             affirmation_; //081103
   inline virtual const CORN::date32 get_bud_break_date()                 const { return bud_break_date.get_date32();}                   //081013
   inline virtual const CORN::date32 get_initial_fruit_growth_date()      const { return initial_fruit_growth_date.get_date32();}        //081013
   inline virtual const CORN::date32 &get_rapid_fruit_growth_date()       const { return rapid_fruit_growth_date.get_date32();}          //081013
   //#if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
   inline virtual const CORN::date32 &get_chill_requirement_satisfied_date() const { return chill_requirement_satisfied_date.get_date32();} //081013
   virtual float32 get_fruit_chill_hours_remaining()                      const;
   //#endif

   // for output only ?
   virtual float64 get_orchard_solid_canopy_interception()                const; //081020
   virtual float64 get_orchard_shaded_area_GAI()                          const; //081020
   virtual float64 get_orchard_canopy_porosity()                          const;
   // PHENOLOGY_2018 need to implement
   inline virtual void know_weather_hourly
      (const CS::Weather_hours *_hourly_weather)                                 //151212_080908
      { ref_hourly_weather = _hourly_weather; }
 public: // Special events and conditions
   virtual
      Crop_mass_fate_biomass_and_nitrogen * harvest                              //200603
      (float64 remove_amount_kg_m2
      ,const CropSyst::Harvest_or_clipping_operation_struct
      *harvest_or_clipping_params
      ,bool terminate);
   inline bool is_fruit_harvested()                    const {return harvested;}
   /*200420 replaced with describe_periods
   virtual const char *describe__growth_stage
      (std::string &growth_stage_phrase)                                  const;
   */
   virtual nat8 describe_periods(CORN::Text_list &descriptions)           const;
   inline virtual bool waiting_for_fruit_bud_break()                      const
      { return fruit_bud_break_requirement; }
   virtual void check_hardiness();                                               //190614
 public: // outputs
   virtual float64  get_dry_biomass_kg_m2()                               const;
 protected:
   RENDER_INSPECTORS_DECLARATION;                                                //180612
};
//_class Crop_orchard_fruit_________________________________________2007-11-20_/
}//_namespace CropSyst_________________________________________________________/
#endif

