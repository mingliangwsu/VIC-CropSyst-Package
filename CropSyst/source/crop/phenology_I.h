#ifndef phenologyH
#define phenologyH
#include "corn/const.h"
#include "corn/primitive.h"
#include "corn/chronometry/date_I.h"
#include "CS_suite/simulation/CS_period.h"
#ifndef growth_stagesHPP
#  include "growth_stages.hpp"
#endif
#include "corn/container/item.h"
namespace CropSyst

#if (PHENOLOGY_VERSION==2018)
#define RELATIVE_TO_PERIODS
//relative to period is not working
#endif
{
//______________________________________________________________________________
interface_ Phenology
{
   virtual bool is_in_germination()                              affirmation_=0;
   inline virtual bool has_germinated()                          affirmation_
      { return has_emerged(); }
   virtual bool has_emerged()                                    affirmation_=0; //101030 was is_in_season
   virtual bool is_in_accrescence()                              affirmation_=0;
   virtual bool is_flowering()                                   affirmation_=0;
      // alias to is_in_anthesis
   virtual bool is_in_anthesis()                                 affirmation_=0;
   virtual bool is_in_tuberization()                             affirmation_=0; //150825
   virtual bool is_in_yield_formation()                          affirmation_=0;
   virtual bool is_in_fructescence()                             affirmation_=0; //181109
   virtual bool is_in_veraison()                                 affirmation_=0; //181109
   virtual bool is_in_rapid_fruit_development()                  affirmation_=0; //181109
   virtual bool is_in_culminescence()                            affirmation_=0; //181107
   virtual bool is_in_senescence()                               affirmation_=0; //141125
   virtual bool is_at_maturity()                                 affirmation_=0; //200509
   //200509 virtual bool is_maturity_first_day()                          affirmation_=0;
   virtual bool is_maturing()                                    affirmation_=0; //181108
      // (the crop has reached maturity initiation but not yet culmination);
      // replaced with is_maturing
      // (the crop has reached maturity initiation but not yet culmination);
   virtual bool has_matured_new()                                affirmation_=0;
       // currenty after maturity culmination  (not maturing)
   inline virtual bool is_matured()                              affirmation_    //181108
      { return is_maturing() || has_matured_new() || is_in_quiescence() ; }
   virtual bool has_expired_accrescence()                        affirmation_=0; //190628
   virtual bool is_harvested()                                   affirmation_=0; //170820
   virtual bool is_in_quiescence()                               affirmation_=0; //181108
   virtual bool is_dormant()                                     affirmation_=0;
   virtual bool is_in_growth_period()                            affirmation_=0;
   virtual bool is_in_growing_season()                           affirmation_=0; //130903
   virtual bool is_at_quiescence()                               affirmation_=0; //200404
   //___________________________________________________________________________
   virtual void clear_days_since_start_of_harvest()              assignation_=0;
   virtual nat16 day_of_season()                                        const=0;
   virtual const nat16 &ref_day_of_season()                             const=0;

   // rename these to initiate
   virtual bool activate_sowing()                                            =0;
   virtual bool activate_emergence()                                         =0;
   virtual bool activate_accrescence()                                       =0;
   virtual bool activate_culminescence()                                     =0; //181113
   virtual bool activate_senescence()                                        =0; //181113
   virtual bool activate_quiescence()                                        =0; //190424

   virtual bool activate_anthesis()                                          =0;
   virtual bool activate_yield_formation()                                   =0;
   virtual bool activate_maturity()                                          =0;

   virtual bool activate_tuberization()                                      =0;
      // rename to activate_yield_formation
   virtual bool activate_fructescence()                                      =0; //181109
   virtual bool activate_veraison()                                          =0; //181109
   virtual bool activate_rapid_fruit_development()                           =0; //181109
   virtual bool activate_restart()                                           =0;

   // Harvest and harvestable are not phenologic events so should not be in phenology
   virtual bool activate_harvest()                                           =0;
   #ifndef RELATIVE_TO_PERIODS
   virtual float64 get_thermal_time_cumulative()                        const=0;
   #endif
   #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))

   virtual bool culminate_senescence()                                       =0;
      //in 2018 the phenology would be driving period initiation and culmination.
      // although the were be something initiated by the crop.


   virtual int16 get_days_since_start_of(Normal_crop_event_sequence event) const=0;
   inline virtual bool started_today(Normal_crop_event_sequence growth_stage) affirmation_ //130903
      { return get_days_since_start_of(growth_stage) == 1; }
   virtual Normal_crop_event_sequence get_growth_stage_sequence()       const=0;
         //130425  rename this to get_event_index()
   virtual nat16 get_start_day_in_season
      (Normal_crop_event_sequence growth_stage)                         const=0; //160523
      // The number days since the start of the season() including today (1 based)
      // Typically since planting date, but may be restart date in
      // perennial/dormant crops.
   virtual nat16 get_duration_of(Normal_crop_event_sequence growth_stage)const=0;//160609
      // I beleive this is in days if so rename get_duration_days
   virtual bool is_early
      (Normal_crop_event_sequence growth_stage,nat16 days)       affirmation_=0; //181108
   #endif

   //_Period___________________________________________________________________/
   interface_ Period_thermal
   : public extends_interface_ CS::Period
   //200404 : public extends_interface_ CORN::Item
   {
      virtual bool start_day()                                  modification_=0;
      virtual bool end_day()                                    modification_=0;
      virtual bool has_expired()                                 affirmation_=0;
         // rename to has_elapsed ?
      //200409 use get_lapse virtual bool get__day_count()               const=0;
      virtual float64 get_thermal_time_accum()                          const=0; //200211

      virtual inline float64 get_thermal_time_relative_elapsed()        const=0; //200304
      virtual inline float32 get_thermal_time_relative_remaining()      const=0; //200304
      virtual bool clipping_resets()                             affirmation_=0;
      virtual float32 get_param_duration_GDDs()                         const=0;
      virtual float32 get_param_thermal_time_accum()                    const=0;
   };
   //_Period_thermal___________________________________________________________/
   // #endif
   #if ((PHENOLOGY_VERSION==2018))
   // get period methods
   virtual const Period_thermal *get_accrescence_period()               const=0; //181107
   virtual const Period_thermal *get_culminescence_period()             const=0; //181107
   virtual const Period_thermal *get_senescence_period()                const=0; //181107

   virtual const CS::Period     *get_anthesis_period_active()           const=0; //181107
   virtual const Period_thermal *get_root_elongation_period()           const=0; //181107
   virtual const Period_thermal *get_yield_formation_period()           const=0; //181111
   virtual const CS::Period     *get_maturity_period()                  const=0; //181107
   virtual bool is_early(const CS::Period *period,nat16 days)    affirmation_=0; //181108
   virtual nat8 respond_to_clipping()                                        =0; //181108
      // returns the phenologic periods that were reset if any
   virtual float64 get_growing_degree_day()                                  =0; //181115
   #ifndef RELATIVE_TO_PERIODS
   virtual inline const float64 &ref_accum_deg_days_normal()            const=0; //181231
   #endif
   #endif
   virtual nat16 get_season_duration_days()                             const=0; //181111

   virtual const CORN::date32 &ref_planting_date()                      const=0; //190710
   virtual const CORN::date32 &ref_restart_date()                       const=0; //190710
   virtual const CORN::date32 &ref_emergence_date()                     const=0; //190710
   virtual const CORN::date32 &ref_anthesis_date()                      const=0; //190710
   virtual const CORN::date32 &ref_tuberization_date()                  const=0; //190710
   virtual const CORN::date32 &ref_maturity_date()                      const=0; //190710
   virtual const CORN::date32 &ref_yield_formation_date()               const=0; //190710
   virtual const CORN::date32 &ref_culminescence_date()                 const=0; //190710
   virtual const CORN::date32 &ref_quiescence_init_date()               const=0; //190710
   virtual const CORN::date32 &ref_quiescence_culm_date()               const=0; //190710

   virtual const CORN::date32 &ref_fructescence_date()                  const=0; //190710
   virtual const CORN::date32 &ref_rapid_fruit_growth_date()            const=0; //190710

   inline virtual CORN::date32 get_planting_date()                        const { return ref_planting_date(); }
   inline virtual CORN::date32 get_restart_date()                         const { return ref_restart_date(); }
   inline virtual CORN::date32 get_emergence_date()                       const { return ref_emergence_date(); }
   inline virtual CORN::date32 get_anthesis_date()                        const { return ref_anthesis_date(); }
   inline virtual CORN::date32 get_tuberization_date()                    const { return ref_tuberization_date(); }
   inline virtual CORN::date32 get_maturity_date()                        const { return ref_maturity_date(); }
   inline virtual CORN::date32 get_yield_formation_date()                 const { return ref_yield_formation_date(); }
   inline virtual CORN::date32 get_culminescence_date()                   const { return ref_culminescence_date(); }
   inline virtual CORN::date32 get_quiescence_init_date()                 const { return ref_quiescence_init_date(); }
   inline virtual CORN::date32 get_quiescence_culm_date()                 const { return ref_quiescence_culm_date(); }
   inline virtual CORN::date32 get_fructescence_date()                    const { return ref_fructescence_date(); }
   inline virtual CORN::date32 get_rapid_fruit_growth_date()              const { return ref_rapid_fruit_growth_date(); }

   inline virtual CORN::DOY get_planting_DOY()                             const { return get_planting_date          () % 1000; } //190617
   inline virtual CORN::DOY get_restart_DOY()                              const { return get_restart_date           () % 1000; } //190617
   inline virtual CORN::DOY get_emergence_DOY()                            const { return get_emergence_date         () % 1000; } //190617
   inline virtual CORN::DOY get_anthesis_DOY()                             const { return get_anthesis_date          () % 1000; } //190617
   inline virtual CORN::DOY get_tuberization_DOY()                         const { return get_tuberization_date      () % 1000; } //190617
   inline virtual CORN::DOY get_maturity_DOY()                             const { return get_maturity_date          () % 1000; } //190617
   inline virtual CORN::DOY get_yield_formation_DOY()                      const { return get_yield_formation_date   () % 1000; } //190617
   inline virtual CORN::DOY get_culminescence_DOY()                        const { return get_culminescence_date     () % 1000; } //190617
   inline virtual CORN::DOY get_quiescence_init_DOY()                      const { return get_quiescence_init_date   () % 1000; } //190617
   inline virtual CORN::DOY get_quiescence_culm_DOY()                      const { return get_quiescence_culm_date   () % 1000; } //190617
   inline virtual CORN::DOY get_fructescence_DOY()                         const { return get_fructescence_date      () % 1000; } //190617
   inline virtual CORN::DOY get_rapid_fruit_growth_DOY()                   const { return get_rapid_fruit_growth_date() % 1000; } //190617
};
//_interface_Phenology_________________________________________________________/
}//_namespace CropSyst_________________________________________________________/
#endif

