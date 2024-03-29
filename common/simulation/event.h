#ifndef eventH
#define eventH
#ifndef event_typesH
#  include "common/simulation/event_types.h"
#endif
#ifndef synchronizationH
#include "common/simulation/synchronization.h"
#endif
#ifndef constH
#include "corn/const.h"
#endif
#include "corn/OS/directory_entry_name_concrete.h"
#include "corn/chronometry/date_32.h"
#include "corn/metrology/units_clad.h"
//______________________________________________________________________________
#define LABEL_enabled               "enabled"
#define LABEL_checked               "checked"
#define SYNCHRONIZE_ALL_OBJECTS 0
#define MAX_REPETITIONS 32767

//______________________________________________________________________________
// Forward declarations
class Toperation_event_form;  // for friendship when editing events
class Tevent_editor_form;     // for friendship when editing events
class TScenario_form;
class Common_operation;       // references
//______________________________________________________________________________
namespace CORN
{  class Data_record;
}
//______________________________________________________________________________
class Simple_event
: public implements_ CORN::Item
{public:
   CORN::Date_clad_32 date;      // if needed should reference begin_sync date   //170524_120314
   Synchronization      begin_sync;                                              //030608
      ///< If this is a period, this is the start of the period, otherwise it is the single event date

 public: // The following are assigned at runtime so we dont save these as parameters
   contribute_ bool checked;                                                     //021012
      /**< The checked value indicates if the user has selected to
            perform an event, usally by clicking on a check box.
      **/
   contribute_ bool enabled;                                                     //020312
      /**<  Enabled has two functions:
            1. This is available for the program to enable/disable the operation
               for operations that can be disabled
            2. This is also used in the case of event period and or repeatable
               operations, the simulation event scheduler can simply disable the
               operation when the period ends.
      This is used only at runtime (not a user selection)
      **/
   contribute_ bool selected;                                                    //020312
 public:
   virtual bool setup_structure
      (CORN::Data_record &data_rec,bool for_write)                 modification_;//120716
   void copy(const Simple_event &to_be_copied)                     modification_;//130428
   virtual CORN::date32 synchronize                                              //020301
      (  const Synchronization &sync                                             //030605
      ,const CORN::Date_const &date_to_synchronize_to);                          //020301
      ///< \return the actual synchronized date.
   inline virtual bool is_synchronized()                                   const
      { return date.get_date32() != 0; }                                         //170525_030608
      ///<An event is synced if the date has been assigned.
   inline virtual bool reset()                                                   //180702
      {  date.set_date32(0); return true; }
 public:
   Simple_event();
   Simple_event(const Simple_event & to_be_copied);
   Simple_event(const Synchronization &_sync);

   virtual bool set_enabled(bool _enabled)                         assignation_; //011221 101129
   virtual bool is_enabled()                                       affirmation_; //011221
   //200406 virtual bool set_checked(bool _checked)                         assignation_; //020124 101129
   virtual bool set_checked(bool)                                 contribution_; //200406
   virtual bool is_checked()                                       affirmation_; //011221
   virtual bool set_selected(bool _selected)                       assignation_;
   virtual bool is_selected()                                      affirmation_;

      /**< For event check list boxes we use the checkbox to enable or disable
         the events we need to leave enabled true because it will otherwise be
         grayed out.
         We will use enabled in derived class to indicate the operation
         event is not currently implemented or enabled based on current
         project or scenario simulation model mode.
      **/
};
//_Simple_event_____________________________________________________2012-07-16_/
class Common_event                         //120716 rename to operational event
: public extends_ Simple_event
   // when 0 indicates the event has not been synchronized to an actual date
{
 public:                                                                         //030214
      ///< If this is a period, this is the start of the period, otherwise it is the single event date
   Contingency_clad  contingency_clad;                                           //040806
      ///< What to do if the event cannot be processed on a date
   //_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
 public:
   class Period
   {
    /**< In the parameter editor, the period parameters are always available. **/
    public:  // Currently public for CANMS_sim event generation
      friend class Common_event;                                                 //030609
      friend class Common_simulation;
      friend class Tevent_editor_form;                                           //030609
      int16  event_repetitions;                                                  //020313
         /**< MAX_REPETITIONS=32767 indicates the event has unlimited repetitions
               0 is no longer used to disable, instead, use repeat_eventl
               The event will be performed at least one time (even if 0 is used).
         **/
    public: // Event repetition:
      bool              repeat_event;                                            //021204
      int16             interval;   // Days, weeks, years                        //021103
      CORN::Units_clad   interval_timestep; //units code                         //191007
      //  (Durning simulation run interval is normalized to days)
    public:
      Synchronization   end_sync;
    public:  // period repetition
      int16             period_repetitions;                                      //030605
      int16             period_cycle_years;                                      //030605
         ///< This is the number of years in the repetition cycle
    public:  // The following are used when running the simulation they are not recorded
      bool              initialized;                                             //030515
         ///< Set to true when the period has been initialize for the run
      int16              interval_countdown_days;                                //021203
         ///< Days until next event repetition  (must be signed so not using Days)
         /// When count_down is reached, repetitions will be decremented.
      int16             event_repetitions_countdown;                             //030608
         ///< Each time an event it repeated  this is decremented.
      int16             period_repetitions_countdown;                            //030608
         ///< Each time a period is repeated, this is decremented.
      CORN::Date_clad_32 end_date;                                               //170525_030610
         ///< will be 0 until the actual end date is determined.
    public:
      Period();
      Period(const Period &from_copy);
      virtual bool setup_structure
         (CORN::Data_record &data_rec
         ,bool for_write) modification_;                                         //020617
      void initialize(const CORN::Date_const &starting_from_date);               //030515
      bool update_event_repetition();                                            //030609
         /**< This checks if the event is initialized and repeatable
               If so it decremtns the repeat count and return true indicating the event is to be repeated.
         **/
      bool update_end_period(const CORN::Date_const &today);                     //030609
         /** This checks if the event is initialized and if the period synchronized to end today
            Periods may also end on the date where the repeat count goes to 0;
            This function should be called after update_repetition()
         **/
    };
   //_Period_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _/
protected:
   friend class Toperation_event_form; // So we can bind_to                      //020306
   friend class Tevent_editor_form;                                              //020306
   friend class TScenario_form;                                                  //030903
 public:                                                                         //020916
   std::string ID;                                                               //030810
      /**< This is a unique identifier for the event assigned either by
         the user or event generator when actual events are generated from
         potential events, the generated event ID can composed by the
         potential event's ID appended with : and sequence number (count) (1 based).
         Do not confuse event ID with the operation description.
         In LADSS, this is the event ID (an integer value)
      */
#ifdef LADSS_MODE
 public: // The following are assigned at runtime runtime so we don'
// It appears in UNIX I can't set operation to itself in the constructor for CropSyst operations.
#else
 protected: // The following are assigned at runtime runtime so dont save/write
#endif
   Common_operation *operation;  bool owns_operation;
      /**< Operation might not be owned by the event, so we dont delete it in that case
       Is the disposition of the operation the responsibility of this event else is it stored some where else
       **/
 public:
   std::string   operation_type_label;                                           //030808
 public:
   std::string   operation_name;                                                 //030808
      /**<For LADSS, the operation_name_or_filename stores the operation ID
         For CANMS, the operation name will be the unqualified and the full filename
         can be determined from the current project's operations directory,
         parameter subdirectory path for the operation type,
         the operation (file) name and the ".op" extension
         For CropSyst, this is the name of the management file containing the corresponding operation parameters
         this is currently only used by the schedule viewer since we no longer output the
         original parameters.
      **/
   CORN::OS::File_name_concrete operation_filename;                              //121126_041215
      /**< For CropSyst and CANMS.  When the CANMS schedule option is selected
            this is the CANMS operation file name (.op) to be associated with the CropSyst scheduled event.
         Currently CANMS always composes the operation filename from the operation_name
         so operation_filename is not used for native  CANMS operations itself,
         only when when a CropSyst managaement event is associated with a CANMS operation operation
         for functional parameter, is the operation_filename used on input
         On output, when the operation is recorded in the schedule,
         the fully qualified operation_filename is then set.
      **/
   int32             resource_set_ID;                                            //030810
      /**< At runtime event can optionally be associated with a particular object.
            The associated_object pointer will be 0 if no association.
            For CANMS/CropSyst cooperation associated object
               is initially the land block fragement, later
               events may be associated with a crop.
      **/
 public:
   Period *period;                                                               //021103
   bool deleted;
   bool logging; // this is used at runtime to prevent operation parameters from being logged //041015
 public:
   Common_event();
   Common_event(Common_operation *_operation,bool _owns_operation );             //020101
   Common_event(const char *_operation_type_string);                             //020101
   Common_event(const Synchronization &_sync);
   Common_event(const Common_event &to_be_copied);                               //020127
      // Copy constructor (the operation is not copied, only the pointer value;
   Common_event(const CORN::Date_const &act_date
      ,Common_operation *operation ,bool _owns_operation = false);               //020305
      /**< This constructor is used when creating actual events from potential events.
            If there will only be one instance of an event with an operation
            (if the operation will not be shared with other events),
            you can set owns_operation to true and the operation will be deleted when the
            event is deleted.  Owns operation must be false for repeated events and
            event periods
      **/
   virtual ~Common_event();
   void copy(const Common_event &to_be_copied)                    modification_; //020127
   inline const char *get_operation_type_label()                          const
      { return operation_type_label.c_str(); }                                   //030808
   const char *get_operation_name(std::string &result)                    const; //030808
   inline const char *set_operation_name(const char *new_name)
      { return operation_name.assign(new_name).c_str(); }                        //041215
 public:
   virtual bool date_matches(const CORN::Date_const &act_date)     affirmation_;
   inline virtual const CORN::Date_const &get_date()       const { return date;}
   virtual const char  *label_string(std::string &buffer)                 const; //030123
   virtual const std::string &append_to_string(std::string &target)       const; //140614
   virtual int          compare(const CORN::Item &other)                  const;
   virtual bool setup_structure(CORN::Data_record &data_rec,bool for_write) modification_; //970709
   virtual void log(std::ostream &log_file,bool  not_processed = false);
   inline virtual bool write(std:: ostream &strm)                  performs_IO_  //131028
      {  // strm << "[___________]" << std::endl;  // This is just for debugging
         log(strm);
         return true;
      }
   bool reinitialize_for_period()                                 modification_;  // returns true if seasonal //030608
   inline Common_operation *get_operation()           const { return operation;} //020225
   virtual Common_operation *set_operation                                       //021101
      (Common_operation *_operation,bool _owns_operation);
   const std::string &append_operation_description_to(std::string &buffer)const; //140616
      /**< If the event has an operation, appends the operation description to the buffer.
            appends empty string.
            \return the buffer;
            See also label_string which includes also the event schedule.
      **/
   virtual CORN::date32 synchronize                                              //020301
      (const Synchronization &sync                                               //030605
      ,const CORN::Date_const &date_to_synchronize_to);                          //020301

      // we can optionally synchronize only if the associated object matches    //030609
      ///< \return the actual synchronized date.
   inline virtual bool is_synchronized()  const { return date.get_date32()!=0; } //170525_030608
      ///<An event is synced if the date has been assigned.
   virtual bool  is_period(bool delete_period_if_not_valid)       modification_; //021204
      /**< Returns true if the event is a period.
         If delete_period_if_not_valid is true, will also deallocate the period if it is not valid.
         For simulation runs we dont need to keep around the period if not used or invalid, so pass true.
         In the parameter editor, we keep the period in case the user switches it on/off so pass false.
      **/
   bool is_operation_mode()                                        affirmation_; //030606
   bool is_repeatable_period()                                     affirmation_; //030605
      ///< Periods synchronized relative to sowing date or start/end of simulation are not repeatable.
   bool period_synchronized_to_growing_season()                           const; //030605
      /**< \returns true if either the period start date or end date is
            synchronized to the growing season. \return false if not a period.
      **/
   bool period_synchronized_to_actual_date()                              const; //030607
   bool period_repeated_each_season()                                     const; //030609
   void assign_unique_ID()                                         assignation_; //021118
   void no_period()                                                assignation_; //021203
      ///< deallocates and clears the period making it a single event.
 public: // Association overrides
/* may be only needed for key comparison
   inline virtual const char *get_key()              const { return ID.c_str();} //030117
*/
   inline virtual bool is_key_string(const std::string &key)              affirmation_  //180820
      { return ID == key; }
   bool                 disenable()                               modification_; //020312
      ///< disable appears to be used or predefined macro somewhere
   std::string return_buffer;                                                    //050207
};
//_Common_event_____________________________________________________2001-12-07_/
class Common_event_list
: public CORN::Bidirectional_list // indexed by ID                               //120620_021101
{
 public:
   Common_event_list();                                                          //011229
   nat16 remove_between
      (const CORN::Date_const &low_date
      ,const CORN::Date_const &high_date);                                       //000317
      /** Returns the number of events removed/deleted. (dates are inclusive)
         If the management event is to always be applied, it will not be removed.
      **/
   void transfer_from                                                            //010320
      (Common_event_list &from_list
      ,Synchronization_mode what_to_transfer = ANY_SYNC_MODE                     //030605
      ,const CORN::Date_const *act_ref_date = 0);
      // Only specified if what to transfer is relative
      /**<      Removes events event from the from_list appending to
             to this list if the event matches the specified condition
      **/
   virtual nat16 synchronize                                                     //020301
      (const Synchronization &sync                                               //030605
      ,const CORN::Date_const &date_to_synchronize_to);                          //020301
      /**  This synchronize is provided for 'before planting' and 'reletive to sowing' events.
            It should not be used for anything else at this time
          I can't remember why it can be used for anything else, I would like to move Simulation::synchronize to here.
      **/
   void set_event_filename(const char *filename);                                //041015
      /** This is used by CropSyst to identify the original management
          filename containing the event/operation parameters.**/
};
//_Common_event_list________________________________________________2001-12-07_/
#ifdef CROP_PARAM_ADJUST
#define DECLARE_EVENT_PARAMETERS(event_class, operation_class)                \
class event_class                                                             \
: public Simple_event                                                         \
, public operation_class                                                      \
{public:                                                                      \
   inline event_class()                                                       \
      : Simple_event()                                                        \
      , operation_class()                                                     \
      { operation = this; }                                                   \
   inline event_class(const Synchronization &date_phen_sync_)                 \
      : Simple_event(date_phen_sync_)                                         \
      , operation_class()                                                     \
      { operation = this; }                                                   \
};
//_DECLARE_EVENT_PARAMETERS_________________________________________2012-07-17_/
#else
#define DECLARE_EVENT_PARAMETERS(event_class, operation_class)                \
class event_class                                                             \
: public Common_event                                                         \
, public operation_class                                                      \
{public:                                                                      \
   inline event_class()                                                       \
      : Common_event()                                                        \
      , operation_class()                                                     \
      { operation = this; }                                                   \
   inline event_class(const Synchronization &date_phen_sync_)                 \
      : Common_event(date_phen_sync_)                                         \
      , operation_class()                                                     \
      { operation = this; }                                                   \
};

#endif
//_DECLARE_EVENT_PARAMETERS____________________________________________________/
/* Now we use Common_event */
// This is actually now obsolete, we can just use DECLARE EVENT PARAMETERS
// but I am keeping this incase I need to switch back in CropSyst.
#define DECLARE_EVENT_PERIOD_PARAMETERS(event_class, operation_class)         \
class event_class           \
: public Common_event       \
, public operation_class    \
{public:                    \
   inline event_class()     \
      : Common_event()      \
      , operation_class()   \
      { operation = this; } \
};
//_DECLARE_EVENT_PERIOD_PARAMETERS_____________________________________________/
#endif

