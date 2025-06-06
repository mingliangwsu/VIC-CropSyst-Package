#ifndef phenology_2018H
#define phenology_2018H
#include "crop/phenology_A.h"

namespace CropSyst
{
//______________________________________________________________________________
#if (PHENOLOGY_VERSION==2018)
class Phenology_2018
: public extends_ Phenology_abstract
{
 private:
   CORN::Unidirectional_list periods[4];
   enum Period_enum {pending,active,completed,clipped};
      // Clipped indicates period was removed because
      // the growth stage was removed to reset for clipping.

   float64 GDDs;
/*200211 obsolete because now we delete the 'clipped' periods
and we didnt really use the normal period anymore
   float64 GDDs[2];
      // 0 normal
      // 1 clipped
*/
 private: // observation
   cognate_ CORN::Container      *inspectors;   // not owned
   cognate_ const CS::Emanator   *emanator;     // not owned
 public:
   CORN::Unidirectional_list periods_to_activate;
   contribute_ CORN::Unidirectional_list events_realized;
      // This of events (ontology) activated today.
 protected: // quick references to active periods
   contribute_ const Period_thermal *germination;
   contribute_ const Period_thermal *emergence_event;
   contribute_ const Period_thermal *anthesis;
   contribute_ const Period_thermal *tuberization;
   contribute_ const Period_thermal *yield_formation;
   contribute_ const Period_thermal *fructescence; // alias to yield_formation
   contribute_ const Period_thermal *veraison;
   contribute_ const Period_thermal *rapid_fruit_development;
   contribute_ const Period_thermal *maturity;
   contribute_ const Period_thermal *quiescence;
   contribute_ const Period_thermal *most_active_period;                         //181230
   //_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
 public: // structors
   Phenology_2018
      (const Crop_parameters_struct::Phenology    &parameters_
      ,const float64 &stress_adjusted_temperature_C_                             //190812
      ,const float64 &air_temperature_min_C_                                     //190812
       ,Thermal_time_common::Vernalization        *vernalization_given
       ,Thermal_time_common::Photoperiodization   *photoperiodization_given
       ,bool  is_perennial_
       ,bool  is_fruit_tree_
       ,const CORN::date32 &simdate_raw_
      );
   virtual ~Phenology_2018();
 public: // simulation element implementation
   //200509 moved to abstract virtual const CORN::date32 &ref_planting_date()                        const; //190710
   virtual const CORN::date32 &ref_restart_date()                         const; //190710
   virtual const CORN::date32 &ref_emergence_date()                       const; //190710
   virtual const CORN::date32 &ref_anthesis_date()                        const; //190710
   virtual const CORN::date32 &ref_tuberization_date()                    const; //190710
   virtual const CORN::date32 &ref_maturity_date()                        const; //190710
   virtual const CORN::date32 &ref_yield_formation_date()                 const; //190710
   virtual const CORN::date32 &ref_culminescence_date()                   const; //190710
   virtual const CORN::date32 &ref_quiescence_init_date()                 const; //190710
   virtual const CORN::date32 &ref_quiescence_culm_date()                 const; //190710

   virtual const CORN::date32 &ref_fructescence_date()                    const; //190710
   virtual const CORN::date32 &ref_rapid_fruit_growth_date()              const; //190710
 public:
   virtual bool start_day()                                       modification_;
   virtual bool start_season()                                    modification_;
   virtual bool process_day()                                     modification_;
   virtual bool end_day()                                         modification_;
   virtual bool is_in_germination()                                affirmation_; //181108
   virtual bool has_emerged()                                      affirmation_; //101030 was is_in_season
   virtual bool is_in_accrescence()                                affirmation_;
   virtual bool is_in_senescence()                                 affirmation_;
   virtual bool is_flowering()                                     affirmation_;
   virtual bool is_in_anthesis()                                   affirmation_;
   virtual bool is_in_fructescence()                               affirmation_; //181109
   virtual bool is_in_veraison()                                   affirmation_; //181109
   virtual bool is_in_rapid_fruit_development()                    affirmation_; //181109

   virtual bool is_at_maturity()                                   affirmation_; //200509_181108
   //200509 virtual bool is_maturity_first_day()                            affirmation_; //181108
   virtual bool is_maturing()                                      affirmation_; //181108
      // (the crop has reached maturity initiation but not yet culmination);
   virtual bool has_matured_new()                                  affirmation_; //181108
   virtual bool is_in_quiescence()                                 affirmation_; //181108
   virtual bool is_dormant()                                       affirmation_;
   virtual bool is_in_growing_season()                             affirmation_;
   virtual bool is_in_yield_formation()                            affirmation_;
   virtual bool is_in_growth_period()                              affirmation_;
   virtual bool is_in_culminescence()                              affirmation_; //181107

   virtual bool is_in_tuberization()                               affirmation_;
   virtual bool is_early(const CS::Period *period,nat16 days)affirmation_; //181108
   virtual bool has_expired_accrescence()                          affirmation_; //190628

   virtual bool is_at_quiescence()                                 affirmation_; //200404

   virtual bool reset
      (Normal_crop_event_sequence from = NGS_RESTART
      ,Normal_crop_event_sequence to   = NGS_TERMINATED)          modification_;
   virtual void clear_days_since_start_of_harvest()                assignation_;
         // I dont think clear_days_since_start_of_harvest is applicable
         // any more because we now simply create new (active) harvest period
   // get period methods
   inline virtual const CS::Period *get_anthesis_period_active()          const  { return anthesis; }       //181108
   inline virtual const Period_thermal *get_yield_formation_period()      const  { return yield_formation;} //181111
   inline virtual const CS::Period *get_maturity_period()                     const  { return maturity; }       //181114
   Period_thermal *activate_period(const char *period_ontology)           modification_; //181111
   // These know_methods should actually return Period
   virtual bool activate_sowing()                                              ; //181108
   virtual bool activate_emergence()                                           ; //181108
   virtual bool activate_accrescence()                                         ; //181108
   virtual bool activate_culminescence()                                       ; //181113
   virtual bool activate_senescence()                                          ; //181113
   virtual bool activate_quiescence();

   virtual bool activate_anthesis()                                            ; //181108
   virtual bool activate_yield_formation()                                     ; //181108
   virtual bool activate_maturity()                                            ; //181108
   virtual bool activate_tuberization()                                        ; //181108
   virtual bool activate_fructescence()                                        ; //181109
   virtual bool activate_veraison()                                            ; //181109
   virtual bool activate_rapid_fruit_development()                             ; //181109
//200115   virtual bool /*Phenology::Period **/ activate_root_elongation()                       ; //181118
   virtual Period_thermal *activate_root_elongation()                       ; //200115_191121_181118

   virtual bool activate_restart()                                             ; //181108
   virtual nat8 respond_to_clipping()                                          ; //181108
   virtual bool know_observation
      (CORN::Container      *inspectors_
      ,const CS::Emanator   *emanator_)                              cognition_;
   virtual float64 get_growing_degree_day()                                    ; //181115
   virtual float64 get_accum_degree_days
      (/*200211 bool adjusted_for_clipping*/)                             const; //140619
   virtual inline const float64 &ref_accum_deg_days_normal()        const { return GDDs/*200508 [0]*/;}
   inline const float64 &ref_accum_deg_days_adjusted_for_clipping() const { return GDDs/*200508 [1]*/;}
      // Actually we no longer have adjusted for clipping
      // because we now get the thermal time of the period which may be reset

   bool was_realized(const char *phenology_ontology)            affirmation_;

   #ifndef RELATIVE_TO_PERIODS
   inline virtual float64 get_thermal_time_cumulative
      (/*200211obs bool adjusted_for_clipping*/) const { return GDDs[0]; }
   #endif
 protected:
   bool update_GDDs()                                             modification_; //190104
   nat8 activate_pending_periods
      (CORN::Unidirectional_list &periods_to_activate);
 protected:
   bool register_key_period(Period_thermal *potential_key_period);
   nat16 register_key_periods(CORN::Container &potential_key_periods);
 private:
   nat8 realize_events(const CORN::Container &activating_periods) contribution_;
};
//_Phenology_2018___________________________________________________2018-08-11_/
#endif
}//_namespace_CropSyst_________________________________________________________/
#endif

