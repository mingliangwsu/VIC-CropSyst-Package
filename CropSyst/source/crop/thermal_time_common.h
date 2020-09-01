#ifndef thermal_time_commonH
#define thermal_time_commonH
#include "options.h"
#include "crop/thermal_time.h"
#include "crop/crop_param_class.h"
#include "weather/parameter/WP_air_temperature.h"
#include "CS_suite/observation/CS_observation.h"

#define VERNALIZATION_NOT_APPLICABLE 0
#define PHOTOPERIOD_NOT_APPLICABLE   0

namespace CropSyst
{
//______________________________________________________________________________
class Thermal_time_common
: public implements_ Thermal_time
{
 public:
   //___________________________________________________________________________
   class Vernalization
   {
    private:
      const Crop_parameters_struct::Vernalization &parameters;                   //080605
      const Physical::Temperature   &temperature_with_est_night;                 //181116
         //actually would be temperature_with_est_night;                         //150217_140625
    protected:
      contribute_ float64 vern_days;
      contribute_ float64 Vfactor; // rename this to factor
    public:
      enum Status {VERNALIZATION_INACTIVE,VERNALIZATION_START,VERNALIZATION_ACTIVE,VERNALIZATION_END};
      contribute_ Status status;
    public:
      Vernalization
         (const Crop_parameters_struct::Vernalization             &parameters_
         ,const Physical::Temperature             &temperature_with_est_night_);
      inline bool is_satisfied()                                          const
         { return status == VERNALIZATION_END; }
      float64 update()                                           rectification_;
      inline float64 get_factor()                      const { return Vfactor; }
   };
   //_Vernalization_________________________________________________2011-11-09_/
   class Photoperiodization
   {
    private:
      const Crop_parameters_struct::Photoperiod &photoperiod_parameters;
      const float64                             &ref_day_length_hours;
    protected:
      contribute_ float64 factor;
    public:
      Photoperiodization
         (const Crop_parameters_struct::Photoperiod &photoperiod_parameters_
         ,const float64                             &ref_day_length_hours_);
      float64 update()                                           rectification_;
      inline float64 get_factor()                        const {return factor; }
   };
   //_Photoperiodization____________________________________________2018-11-16_/
 public:
   const CropSyst:: Crop_parameters_struct::Thermal_time &parameters;            //080605
 public:
   bool                        cropsyst_fruit_model;                             //041212
   const float64              &stress_adjusted_temperature_C;                    //190812
   const float64              &air_temperature_min_C;                            //190812
 public: // Limitations may be NULL if not applicable
   Vernalization        *vernalization;                                          //111109
   Photoperiodization   *photoperiodization;                                     //181116
      // In PHENOLOGY 2013 these are given/owned
      // In PHENOLOGY 2018 phenology owns these
 public:
   float64        growing_degree_day;                                            //971122
   nat32          consecutive_days_no_accumulation;                              //030515
public: // Temporarily public until  I implement add_desired variables for this class
    #ifdef THERMAL_TIME_OBSOLETE_2020
   float64        clipping_recovery_deg_days_deprecated;                         //040826

      // deprecated (appears to have been disabled for a very long time)

      // When a crop is clipped, set the clipping_recovery thermal time counter, //040826
      // this is decremented by growing_degree_day each day until < 0.           //040826
      // While clipping_recovery > 0, there is no accumulation.                  //040826
   #ifdef THERMAL_TIME_OBSOLETE_2020
   #endif
   //200211 #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
   float64        accum_degree_days_normal[2];
   float64        accum_degree_days_clipped[2];                                  //040823
      // Index 0 is today, 1 is yesterday
      // Adjusted for regrowth after clipping event.
      // When a perennial crop is clipped we reset this accumulator to emergence
      // (we may need to adjust this abit).
      // yesterday was needed to match the thermal time (thermal_time_matchs)
      // which would match consecutive days of no accumulation
      //200211 #endif
      //200211 #if ((PHENOLOGY_VERSION==2018))
   #else
   float64  GDDs_yesterday; // accumulated degree days
   float64  GDDs; // accumulated degree days
      // Yesterday is not needed because as soon as a stage is acheived
      // a new period is rendered to be the active period.
   #endif
public:
   float64 calc_photoperiod_factor()                               calculation_; //140812
   virtual float64 calc_growing_degree_day_daily_resolution                      //000330
      (float64 adjusted_max_temp                                                 //030610
      ,float64 air_temp_min)                                     calculation_=0;
      // This function calculates the normal growing degree day                  //019723
      // not limited by vernalization or photoperiod                             //019723
   virtual float64 calc_growing_degree_day_hourly_resolution                     //021124
      (const CORN::Dynamic_array<float32> &stress_adj_hourly_temperature) calculation_;//030610
   float64 calc_limited_growing_degree_day                                       //010723
      (float64 growing_degree_day
         //200211
      #ifdef THERMAL_TIME_OBSOLETE_2020
      #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
      ,Normal_crop_event_sequence growth_stage_sequence
      #endif
      #endif
      )                                                            calculation_; //130902
      // This function updates the normal calculation of growing_degree_day      //010723
      // (Growing degrees for the current date).
      // It returns the actual growing degrees that may be limited by
      // vernalization or photoperiod.
   virtual double accumulate_degree_days                                         //151110_010727
      (
      #ifdef THERMAL_TIME_OBSOLETE_2020
      // The following are optional for unstressed crop mode                     //020630
      #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
      Normal_crop_event_sequence growth_stage_sequence
      #endif
      #endif
      ); //130911
   #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
   // in 2018 version we delete the affected periods
   void reset_to(float64 new_thermal_time)                        modification_; //040719
      // Some crops such as alfalfa we need to reset thermal time                //040719
      // (I.e. to emergence) when cut so they regrow.                            //040719
   #endif
   #ifdef THERMAL_TIME_OBSOLETE_2020
   void respond_to_clipping(float64 new_thermal_time)             modification_; //040823
      // When annuals crops are clipped the thermal time accumulation is delayed //040823
   #endif
 public: // Constructor
   Thermal_time_common                                                           //080605
      (const Crop_parameters_struct::Thermal_time    &parameters_                //110218_080605
      ,const float64 &stress_adjusted_temperature_C_                             //190812
      ,const float64 &air_temperature_min_C_                                     //190802
      ,bool                  cropsyst_fruit_model_                               //041212
      ,Vernalization        *vernalization_optional                              //181116
      ,Photoperiodization   *photoperiodization_optional);                       //181116

 public: // status accessors
   inline virtual float64 get_growing_degree_day()                        const  //971122
      { return growing_degree_day;}
   virtual float64 get_accum_degree_days
    #ifdef THERMAL_TIME_OBSOLETE_2020
      #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
      (bool adjusted_for_clipping = true                                         //200211_080805
      ,bool for_yesterday = false)                                        const; //140619
      #endif
      #if ((PHENOLOGY_VERSION==2018))
      (bool adjusted_for_clipping_obsolete = false                               //080805
      ,bool for_yesterday_obsolete         = false)                       const; //140619
      #endif
     #else
     () const;                                                                   //200211
     #endif
   inline virtual float64 get_accum_degree_days_yesterday()               const  //200211
      { return GDDs_yesterday;}
   inline virtual const float64 &ref_degree_day()                         const  //160101
      { return growing_degree_day; }
   #
   // actually now unused in all cases, but may be needed for VIC
   inline virtual const float64 &ref_thermal_time_cumulative()            const  //130613
      { return
        #ifdef THERMAL_TIME_OBSOLETE_2020
         #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
         accum_degree_days_clipped[0];
         #endif
         #if ((PHENOLOGY_VERSION==2018))
         GDDs;                                                                   //181115
         #endif
        #else
         GDDs;
        #endif
      }

   virtual void clear_accum_degree_days();
   virtual bool end_day()                                         modification_; //140619
   RENDER_INSPECTORS_DECLARATION;                                                //150916
};
//_Thermal_time_common______________________________________________2002-06-28_/
}//_namespace CropSyst_________________________________________________________/
#endif

