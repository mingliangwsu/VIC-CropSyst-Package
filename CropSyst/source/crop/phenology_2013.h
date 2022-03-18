#ifndef phenology_2013H
#define phenology_2013H
#include "crop/phenology_A.h"
namespace CropSyst
{
//______________________________________________________________________________
class Phenology_2013
: public extends_ Phenology_abstract
{
 protected: // external references
   const Crop_parameters_struct::Phenology &phenology_parameter;
   const float64 &thermal_time_cumulative;
 public:
   Normal_crop_event_sequence growth_stage;
 public:
   mutable CORN::date32 initialization_date32[NGS_COUNT];                        //190710
   mutable CORN::date32 finalization_date32  [NGS_COUNT];                        //190710
   sint16 days_since_start_of_0  [NGS_COUNT];
   nat16 duration_of          [NGS_COUNT];
   nat16 start_day_in_season  [NGS_COUNT];                                       //140620
   nat16 start_day_in_season_senescence;                                         //160211
   nat16 start_day_in_season_senescence_full;                                    //160211
   CORN::Date_clad_32 start_date_tuber_initiation;                               //141217
      // This could be a reference to   start_date[filling]
 public:
   Phenology_2013
      (const Crop_parameters_struct::Phenology &_phenology_parameter
      ,const float64 &thermal_time_cumulative_ref
      ,const float64 &stress_adjusted_temperature_C                              //190812
      ,const float64 &air_temperature_min_C                                      //190812
      ,Thermal_time_common::Vernalization         *vernalization_given           //190701
      ,Thermal_time_common::Photoperiodization    *photoperiodization_given      //190701
      ,bool  is_perennial
      ,bool  is_fruit_tree
      ,const CORN::date32 &simdate_raw);
   virtual ~Phenology_2013();                                                    //191022
 public: // simulation element implementation
   virtual bool start_day()                                       modification_;
   virtual bool end_day()                                         modification_;
   virtual bool restart_season()                                  modification_;

   virtual bool is_in_germination()                                affirmation_; //181108
   virtual bool has_emerged()                                      affirmation_; //101030 was is_in_season
   virtual bool is_in_accrescence()                                affirmation_;
   virtual bool is_flowering()                                     affirmation_;
   virtual bool is_in_anthesis()                                   affirmation_;
   virtual bool is_in_tuberization()                               affirmation_; //150825
   virtual bool is_in_yield_formation()                            affirmation_;
   inline virtual bool is_in_fructescence()                        affirmation_  { assert(false); return false;} //181109
   inline virtual bool is_in_veraison()                            affirmation_  { assert(false); return false;} //181109
   inline virtual bool is_in_rapid_fruit_development()             affirmation_  { /* assert(false);*/ return growth_stage == FGS_RAPID_FRUIT_DEVELOPMENT;} //LML 220318 false;} //181109

   virtual bool is_in_culminescence()                              affirmation_; //181107
   virtual bool is_in_senescence()                                 affirmation_; //141125
   virtual bool is_at_maturity()                                   affirmation_; //200509_181108
   //200509 virtual bool is_maturity_first_day()                            affirmation_; //181108
   virtual bool is_maturing()                                      affirmation_; //181108
      // (the crop has reached maturity initiation but not yet culmination);
   virtual bool has_matured_new()                                  affirmation_; //181108
   virtual bool is_harvested()                                     affirmation_; //170820
   virtual bool is_in_quiescence()                                 affirmation_; //181108
   virtual bool is_dormant()                                       affirmation_;
   virtual bool is_in_growth_period()                              affirmation_;
   virtual bool is_in_growing_season()                             affirmation_; //130903
   virtual bool is_early
      (Normal_crop_event_sequence growth_stage
      ,nat16 days)                                                 affirmation_; //181108

   virtual bool is_at_quiescence()                                 affirmation_; //200404

   virtual nat8 respond_to_clipping()                                          ; //200210

   virtual bool reset
      (Normal_crop_event_sequence from = NGS_RESTART
      ,Normal_crop_event_sequence to   = NGS_TERMINATED)          modification_;
   virtual bool has_expired_accrescence()                          affirmation_; //190628
 public:
   inline virtual int16 get_days_since_start_of
      (Normal_crop_event_sequence growth_stage)                            const
      { nat8 GS_index = (nat8)growth_stage;
        int16 days_since = days_since_start_of_0[GS_index];
        return days_since; }
   inline virtual nat16 get_duration_of
      (Normal_crop_event_sequence growth_stage)                            const
      { return duration_of[growth_stage]; }
   inline virtual CORN::date32 get_finalization_date32                           //170529_151005
      (Normal_crop_event_sequence growth_stage)                            const
      {
         CORN::Date_cowl_32 finalization_date(finalization_date32[growth_stage]);
         finalization_date.inc_days(duration_of[growth_stage]);
         return finalization_date32[growth_stage];
      }
   inline virtual nat16 get_start_day_in_season
      (Normal_crop_event_sequence growth_stage)                            const //160523
      { return start_day_in_season[growth_stage]; }

   inline virtual void clear_days_since_start_of_harvest()          assignation_
      { days_since_start_of_0[NGS_HARVEST] = -29999; /*0; */}
   inline virtual float64 get_thermal_time_cumulative
      (/*200211 bool adjusted_for_clipping*/) const
      {  //200211 UNUSED_arg(adjusted_for_clipping);
         return thermal_time_cumulative; }     //130904

    //virtual Normal_crop_event_sequence get_growth_stage_sequence()   const;
   inline virtual Normal_crop_event_sequence get_growth_stage_sequence()   const
      { return growth_stage; }

   bool activate_stage(Normal_crop_event_sequence sequence)                    ; //190701
   virtual bool activate_sowing()                                              ; //181108
   virtual bool activate_emergence()                                           ; //181108
   virtual bool activate_accrescence()                                         ; //181108
   virtual bool activate_culminescence()                                       ; //181108
   virtual bool activate_senescence()                                          ; //181108
   virtual Phenology::Period_thermal *activate_root_elongation()               ; //191121_181118
   virtual bool activate_tuberization()                                        ; //181108
   inline virtual bool activate_fructescence()             { growth_stage = FGS_FRUCTESCENCE; return true; } //190214
   inline virtual bool activate_veraison()                 { growth_stage = FGS_VERAISON; return true; } //190214
   inline virtual bool activate_rapid_fruit_development()  { growth_stage = FGS_RAPID_FRUIT_DEVELOPMENT; return true; } //190214
   virtual bool activate_anthesis()                                            ; //181108
   virtual bool activate_yield_formation()                                     ; //181108
   virtual bool activate_maturity()                                            ; //181108
   virtual bool activate_quiescence();                                           //190424
   virtual bool activate_restart()                                             ; //181108
   virtual bool activate_harvest()                                             ; //181108
/*200503
   virtual nat16 get_season_duration_days()                               const; //181111
*/

   inline virtual CORN::date32 get_restart_date()                          const { return initialization_date32[NGS_RESTART];       } //190710
   inline virtual CORN::date32 get_emergence_date()                        const { return initialization_date32[NGS_EMERGENCE];     } //190710
   inline virtual CORN::date32 get_anthesis_date()                         const { return initialization_date32[NGS_ANTHESIS];      } //190710
   inline virtual CORN::date32 get_tuberization_date()                     const { return start_date_tuber_initiation.get_date32(); } //190710
   inline virtual CORN::date32 get_maturity_date()                         const { return initialization_date32[NGS_MATURITY];      } //190710
   inline virtual CORN::date32 get_yield_formation_date()                  const { return initialization_date32[NGS_FILLING];       } //190710
   inline virtual CORN::date32 get_culminescence_date()                    const { return get_finalization_date32(NGS_ACCRESCENCE); } //190710
   inline virtual CORN::date32 get_quiescence_init_date()                  const { return initialization_date32[NGS_QUIESCENCE];    } //190710
   inline virtual CORN::date32 get_quiescence_culm_date()                  const { return get_finalization_date32(NGS_QUIESCENCE);  } //190710
   inline virtual CORN::date32 get_fructescence_date()                     const { return initialization_date32[FGS_FRUCTESCENCE];  } //190710
   inline virtual CORN::date32 get_rapid_fruit_growth_date()               const { return initialization_date32[FGS_RAPID_FRUIT_DEVELOPMENT];} //190710
   inline virtual const CORN::date32 &ref_planting_date()                   const { return initialization_date32[NGS_PLANTING];      }//190710
   inline virtual const CORN::date32 &ref_restart_date()                    const { return initialization_date32[NGS_RESTART];       }//190710
   inline virtual const CORN::date32 &ref_emergence_date()                  const { return initialization_date32[NGS_EMERGENCE];     }//190710
   inline virtual const CORN::date32 &ref_anthesis_date()                   const { return initialization_date32[NGS_ANTHESIS];      }//190710
   inline virtual const CORN::date32 &ref_tuberization_date()               const { return start_date_tuber_initiation.ref_date32(); }//190710
   inline virtual const CORN::date32 &ref_maturity_date()                   const { return initialization_date32[NGS_MATURITY];      }//190710
   inline virtual const CORN::date32 &ref_yield_formation_date()            const { return initialization_date32[NGS_ACCRESCENCE];   }//190710
   inline virtual const CORN::date32 &ref_quiescence_init_date()            const { return initialization_date32[NGS_QUIESCENCE];    }//190710

   inline virtual const CORN::date32 &ref_fructescence_date()               const { return initialization_date32[FGS_FRUCTESCENCE];           } //190710
   inline virtual const CORN::date32 &ref_rapid_fruit_growth_date()         const { return initialization_date32[FGS_RAPID_FRUIT_DEVELOPMENT];} //190710

   // For these finalization in 2013 version these are currently calculated when needed
   inline virtual const CORN::date32 &ref_culminescence_date()              const { get_finalization_date32(NGS_ACCRESCENCE);  return finalization_date32[NGS_ACCRESCENCE]; }//190710
   inline virtual const CORN::date32 &ref_quiescence_culm_date()            const { get_finalization_date32(NGS_QUIESCENCE);   return finalization_date32[NGS_QUIESCENCE];  }//190710
};
//_class Phenology_2013_____________________________________________2018-08-11_/
}//_namespace_CropSyst_________________________________________________________/
#endif

