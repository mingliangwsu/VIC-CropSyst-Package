#ifndef synchronizationH
#define synchronizationH
/*
   This class implements a date/phenologic Synchronization.

   A date/phenologic Synchronization may either be an actual date,
   a relative date, or an offset to some phenologic event.
*/
#include "corn/enum.h"
#include "corn/chronometry/date_32.h"

#ifndef growth_statesH
#  include "crop/growth_stages.hpp"
#endif
#ifndef Phrase_ID_or_label
#  define Phrase_ID_or_label const char *
#endif
#ifndef constH
#include "corn/const.h"
#endif

#define LABEL_no_sync                "no_sync"

// The following are replaced by growth stages label table
#define LABEL_after_planting         "after_planting"
#define LABEL_after_bud_break        "after_bud_break"
#define LABEL_after_emergence        "after_emergence"
#define LABEL_after_flowering        "after_flowering"
#define LABEL_after_grain_filling    "after_grain_filling"
#define LABEL_after_maturity         "after_maturity"
#define LABEL_after_dormancy         "after_dormancy"
#define LABEL_after_harvest          "after_harvest"
#define LABEL_actual_date            "actual_date"
#define LABEL_relative_to_sow_date   "relative_to_sow_date"
#define LABEL_relative_to_start_date "relative_to_start_date"
#define LABEL_relative_to_end_date   "relative_to_end_date"

#define LABEL_relative_to_simulation_event "relative_to_simulation_event"
#define LABEL_begin                    "begin"
#define LABEL_end                      "end"

#define LABEL_synchronization         "synchronization"
#define LABEL_offset                  "offset"
#define LABEL_date                    "date"
#define LABEL_event_date              "event_date"
#define LABEL_phenologic              "phenologic"
#define LABEL_repeat_each_season      "repeat_each_season"
#define LABEL_before_planting         "before_planting"
#define LABEL_thermal_time				"thermal_time"
#define LABEL_after_normal_crop_growth_stage "after_normal_crop_growth_stage"
#define LABEL_after_fruit_tree_growth_stage  "after_fruit_tree_growth_stage"

//Aliases
#define LABEL_sync_old                 "sync"
#define LABEL_relative_date           "relative_date"
#define LABEL_phen_sync_obsolete      "phen_sync"
#define LABEL_synchronisation         "synchronisation"

//using namespace CORN;

namespace CORN
{
   class Data_record;
};

class Tsync_form;

#define phen_sync_date_styles (CORN_date_DEFAULT_STYLES | D_relative)

//NYI efine NEWPSYNC _COMMENT_
//______________________________________________________________________________
enum Synchronization_mode
{  ACTUAL_DATE_MODE
,  RELATIVE_TO_SIMULATION_EVENT_MODE
,  RELATIVE_TO_SOWING_MODE // rename to RELATIVE_TO_PLANTING_MODE
,  BEFORE_PLANTING_MODE
,  AFTER_NORMAL_CROP_GROWTH_STAGE_MODE
,  AFTER_FRUIT_TREE_GROWTH_STAGE_MODE
,  THERMAL_TIME_MODE
,  SYNC_MODE_COUNT
};
#define        ANY_SYNC_MODE SYNC_MODE_COUNT
extern const char *synchronization_mode_label_table[];
DECLARE_ENUM_CLAD(Synchronization_mode_clad,Synchronization_mode ,synchronization_mode_label_table)
//190113 DECLARE_LABELED_ENUM(Synchronization_mode_labeled,Synchronization_mode ,synchronization_mode_label_table)
//_Synchronization_mode_____________________________________________2003-06-05_/
enum Simulation_event   // These are general events to the simulation framework
{  SIMULATION_BEGIN_DATE
,  SIMULATION_END_DATE
};
extern const char *simulation_event_label_table[];
//190113 DECLARE_LABELED_ENUM(Simulation_event_labeled,Simulation_event ,simulation_event_label_table)
DECLARE_ENUM_CLAD(Simulation_event_clad,Simulation_event ,simulation_event_label_table)
//_Simulation_event_________________________________________________2003-06-05_/
   #if ((CS_VERSION > 0) && (CS_VERSION <= 3))
// Syncronizations is the old form
enum  Synchronizations_V3  // was phenologic_syncs
{  V3_NOSYNC
,  V3_BEFORE_PLANTING
,  V3_AFTER_PLANTING
,  V3_AFTER_BUD_BREAK
,  V3_AFTER_EMERGENCE
,  V3_AFTER_FLOWERING               //(beginning of)
,  V3_AFTER_GRAIN_FILLING           //(beginning of)                             //000120
,  V3_AFTER_INITIAL_FRUIT_GROWTH    //(beginning of)                             //030521
,  V3_AFTER_VERAISON                                                             //030521
,  V3_AFTER_RAPID_FRUIT_GROWTH                                                   //030521
,  V3_AFTER_MATURITY
,  V3_AFTER_HARVEST
,  V3_AFTER_DORMANCY  // Start of dormancy                                       //021216
,  V3_AFTER_TERMINATION                                                          //030516
,  V3_THERMAL_TIME                                                               //010716
,  V3_ACTUAL_DATE,V3_RELATIVE_TO_SOW_DATE
,  V3_RELATIVE_TO_START_DATE, V3_RELATIVE_TO_END_DATE                            //011204
,  V3_SYNC_COUNT // Count of syncronizations                                     //020309
};
extern const char *synchronizations_V3_label_table[];
DECLARE_LABELED_ENUM(Synchronizations_V3_labeled,Synchronizations_V3,synchronizations_V3_label_table)
#endif
//_Synchronizations_V3______________________________________________2001-12-04_/
enum Reschedule_mode { NO_RESCHEDULE,ONE_RESCHEDULE,MULTIPLE_RESCHEDULE,RESCHEDULE_COUNT};
extern const char *reschedule_label_table[];
DECLARE_ENUM_CLAD(Reschedule_mode_clad,Reschedule_mode,reschedule_label_table)
//_Reschedule_______________________________________________________2006-04-13_/
#define AFTER_RESTART    AFTER_PLANTING
#define ANY_SYNC SYNC_COUNT
// ANY_SYNC can be used when querying event lists for any syncronization.

//011204 The following defines are for previous version compatilibity, eventually remove.
#define RELATIVE_DATE    RELATIVE_TO_SOW_DATE

// Warning ACTUAL_DATE must preceed RELATIVE_DATE in this enumeration
// (see in_irrigation_period).

extern Phrase_ID_or_label synchronizations_stringsx[];
//______________________________________________________________________________
#define  synchronization_mode_bitmask(sync_mode)    (1 << sync_mode)
//______________________________________________________________________________
class Synchronization
: public CORN::Date_clad_32
{
 public: // public so event editor can access

   Synchronization_mode_clad        sync_mode_clad;                        //030605
   int16                               days_offset;                              //991130
      // Should not be negative Could be CORN_days but we dont have editor for unit32
   Simulation_event_clad            simulation_event_clad;                 //030605
   int16                               thermal_time;                             //010716
   Normal_crop_event_sequence_clad normal_crop_event_sequence_clad;        //130425_030605
   Fruit_tree_event_sequence_clad  fruit_tree_event_sequence_clad;         //130425_030605
 public: // temporarily public, need and accessor
   Reschedule_mode_clad             reschedule_mode_clad;                  //060413
   int16                               reschedule_start_season;                  //030613
      // 0 indicates that the syncronization is not postponed otherwise repetitions doe occur until the specified season is reached
   int16                               reschedule_end_season;                    //030613
      // This is the season that reschedule is stopped.
   // For phenologically synchronized and thermal time events,
   // the event can be marked as repeatable for each season. (Applied to perennials)
 public:
   Synchronization(const CORN::Date_const &actual_date);
   // This constructor is used for rotation, etc.
   // This is used for actual and relative dates
   Synchronization(Simulation_event sim_event,CORN::Days days_offset);
   Synchronization                                                               //991122
      (Normal_crop_event_sequence normal_crop_event_sequence                     //130425_991130
      ,CORN::Days offset_days);
   Synchronization(Synchronization_mode mode);                                   //030609
   // This constructor is used to create a synchronization type (for searches)
   Synchronization(Simulation_event sync_to_sim_event);                          //030609
   // This constructor is used to create a synchronization relative to simulation event
   // Eventually move this to CropSyst_synchronization class
   //130903 Synchronization(bool fruit_crop_model, nat32 growth_stage_bitmask);          //030606
   Synchronization(bool fruit_crop_model, Normal_crop_event_sequence growth_stage_sequence); //130903
   void copy(const Synchronization &from_copied)                  modification_; //020916
   virtual CORN::datetime64 set(const CORN::Temporal &new_date)    assignation_; //170524
   void set_normal_growth_stage_offset                                           //040802
      (Normal_crop_event_sequence normal_crop_event_sequence,CORN::Days offset_days);  //130425
   void set_relative_to_simulation_mode(int16 days_offset)        modification_; //030623
   virtual const std::string& append_to_string                                   //140614
      (std::string &target
      ,const CORN::Date_format *target_date_format/*=0*/                             //180624
      ,const CORN::Time_format *target_time_format/*=0*/)                    const;  //180624
   void validate();
   bool is_valid()                                                 affirmation_; // Used by the event parameter editor to make sure the user remembered to schedule and event //060726
   bool is_date()                                                  affirmation_; //011207
   virtual bool   is_synchronized_relatively()                     affirmation_; //140126_020322
   // Is relative currently isn't using date relative bit.
   virtual void set_as_actual(const CORN::Date_const &new_date)   modification_; //020720
   inline Synchronization_mode get_sync_mode()                            const  //030608
      { return sync_mode_clad.get() ; }
   inline int16 get_thermal_time()                 const { return thermal_time;} //030609
   Reschedule_mode get_reschedule_mode()                                  const; //060413
   bool is_before_planting()                                       affirmation_; //000406
      // relative or phenologically
   bool is_growing_season_synchronization()                        affirmation_; //030606
   //             returns true if sync mode is
      // RELATIVE_TO_SOWING_MODE
      // BEFORE_PLANTING_MODE
      // AFTER_NORMAL_CROP_GROWTH_STAGE_MODE
      // AFTER_FRUIT_TREE_GROWTH_STAGE_MODE
      // THERMAL_TIME_MODE
   bool is_phenologic_synchronization()                            affirmation_; //030607
   // check if this is obsolete or that is_growing_season_synchronization is more appropriate
   bool is_actual_date_synchronization()                           affirmation_; //030606
      // returns true if the date has been set (indicating it has been synchronized).
   #if ((CS_VERSION > 0) && (CS_VERSION <= 3))
   bool convert_from_CropSystV3();                                               //030605
      // This is provided for compatibility of CropSyst V3 and V4.1 files
      // returns true if it was CropSyst V3 synchronization.
   #endif
   virtual bool setup_structure
      (CORN::Data_record &data_rec,bool for_write,const char *prefix) /*modification_*/;//030605
      // This is used for data record I/O.
      // Prefix is usually one of the following  event_,  begin_,  end_
 public: // Actual date determination.
   // These all return an actual date if it could be determined
   // Otherwise returns 0 if no synchronization could be made.

   //030610       Syncronization is now considered read only during the sim run
   //             the actual date is now held by the event we no longer convert or synchronize to
   CORN::date32 determine_from_relative
      (const CORN::Date_const &reference_date
      , Synchronization_mode relative_sync = RELATIVE_TO_SOWING_MODE)     const; //011204
   CORN::date32 determine(const CORN::Date_const &date_to_synchronize_to) const; //030610
   //             returns the datenum (0 if not synced)
   CORN::date32 determine_from_thermal_time
      (int16 thermal_time
      , const CORN::Date_const &date_to_synchronize_to)                   const; //030610
   virtual bool sync_type_matches(const Synchronization &other_sync)affirmation_;//030605
   // Returns true if the other_sync has the same mode, and
   //    if a phen sync or simulation event, the same sub sync mode.
   //    Derived classes can override this if they add their own modes.
   //    The syncs only have to have the same mode and sub mode, not identical
};
//_Synchronization_____________________________________________________________/
#endif


