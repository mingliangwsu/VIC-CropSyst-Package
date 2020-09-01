#  include "common/simulation/event.h"
#  include "common/simulation/operation.h"
#  include "corn/string/strconv.h"
#  include "corn/data_source/datarec.h"
#  include "corn/data_source/vv_entry.h"
#  include "corn/data_source/vv_file.h"
#     include "corn/OS/file_system_engine.h"
#  ifndef USE_CORN_TRANSLATOR_CLASS
#     define L_WARNING_Event_not_reached "Event not reached"
#     define common_translate(x) x
#  endif

using namespace std;
using namespace CORN;

extern bool LADSS_mode;   // For LADSS this must be true, for all else false
//______________________________________________________________________________
Simple_event::Simple_event()
: Item()
, date(0)                                                                        //120314
, begin_sync(SIMULATION_BEGIN_DATE,0)                                            //030608
   // was NOSYNC, now default to start date (mainly for CANMS default operations)
, enabled(true)                                                                  //020602
, checked(true)                                                                  //021012
, selected(false)                                                                //130430
{}
//_Simple_event:constructor_________________________________________2012-07-16_/
Simple_event::Simple_event(const Simple_event &from_copy)
: Item                                                               (from_copy)
, date                                                          (from_copy.date) //120314
, begin_sync                                              (from_copy.begin_sync)
, enabled                                                    (from_copy.enabled) //020602
, checked                                                    (from_copy.checked) //021012
, selected                                                               (false) //130430
{}
//_Simple_event:constructor_________________________________________2012-07-16_/
Simple_event::Simple_event(const Synchronization &sync_)
: Item()
, date(0)                                                                        //170525_120314
, begin_sync(sync_)  // was NOSYNC, now default to start date (mainly for CANMS default operations)
, enabled(true)                                                                  //020602
, checked(true)                                                                  //021012
, selected(false)                                                                //130430
{  date.prefer_date_format();                                                    //170525
}
//__Simple_event:constructor________________________________________2012-07-16_/
Common_event::Period::Period()
: end_sync                                               (SIMULATION_END_DATE,0) //030605
// Default period at end of simulation
, repeat_event                                                           (false) //021204
, event_repetitions                                            (MAX_REPETITIONS)
// By default we repeat an operation every day.
//191007, interval_timestep_clad(TS_DAY)  // Day
, interval                                                                   (1) //021103
, interval_timestep                                                     (UT_day) //191007
, initialized                                                            (false) //030515
, interval_countdown_days                                                    (0)
, period_repetitions                                                         (1) //030605
, period_cycle_years                                                         (1) //030605
// This is the number of years in the repetition cycle
, event_repetitions_countdown                                  (MAX_REPETITIONS) //030606
, period_repetitions_countdown                                 (MAX_REPETITIONS) //030606
, end_date                                                                   (0) //030610
{}
//_Common_event::Period:constructor____________________________________________/
Common_event::Period::Period(const Period &from_copy)
: end_sync                             (from_copy                     .end_sync)
, repeat_event                         (from_copy                 .repeat_event) //021204
, event_repetitions                    (from_copy            .event_repetitions)
//191007, interval_timestep_clad(from_copy.interval_timestep_clad)  // Day
, interval                             (from_copy                     .interval) //021103
, interval_timestep                    (from_copy      .interval_timestep.get())
, initialized                          (from_copy                  .initialized) //030515
, interval_countdown_days              (from_copy      .interval_countdown_days)
, period_repetitions                   (from_copy           .period_repetitions) //030605
, period_cycle_years                   (from_copy           .period_cycle_years) //030605
   // This is the number of years in the repetition cycle
, event_repetitions_countdown          (from_copy. event_repetitions_countdown)  //030606
, period_repetitions_countdown         (from_copy.period_repetitions_countdown)  //030606
, end_date                                                                   (0) //030610
{}
//_Common_event::Period:constructor____________________________________________/
Common_event::Common_event()
: Simple_event                                                                () //120716
, contingency_clad                                            (NONE_CONTINGENCY) //040806
, period                                                            (new Period) //021103
, deleted                                                                (false)
, operation                                                                  (0)
, operation_filename                                                          () //041215
, owns_operation                                                         (false) //020305
, resource_set_ID                                                            (0) //030810
, logging                                                                (false) //041015
{  assign_unique_ID();                                                           //021118
   date.prefer_date_format();                                                    //170525
}
//_Common_event:constructor____________________________________________________
Common_event::Common_event(Common_operation *operation_,bool owns_operation_)
: Simple_event                                                                () //120716
, contingency_clad                                            (NONE_CONTINGENCY)
, period                                                            (new Period)
, deleted                                                                (false)
, operation                                                         (operation_)
, operation_type_label                            (operation_->get_type_label())
, owns_operation                                               (owns_operation_)
, resource_set_ID                                                            (0)
, operation_filename                                                          ()
, logging                                                                (false)
{  assign_unique_ID();
   date.prefer_date_format();                                                    //170525
}
//_Common_event:____________________________________________________2002-01-01_/
//020101_      This operation is used if scenario editor to add events.
Common_event::Common_event(const char *operation_type_label_)
: Simple_event                                                                () //120716
, contingency_clad                                            (NONE_CONTINGENCY)
, period                                                            (new Period)
, deleted                                                                (false)
, logging                                                                (false)
, operation                                                                  (0)
, operation_filename                                                          ()
, operation_type_label                                   (operation_type_label_)
, owns_operation                                                             (0)
, resource_set_ID                                                            (0)
{  assign_unique_ID();
   date.prefer_date_format();                                                    //170525
}
//020101_      This operation is used if scenario editor to add events.
//_Common_event:constructor_________________________________________2002-01-01_/
Common_event::Common_event(const Synchronization &sync_)
: Simple_event                                                           (sync_) //120716
, contingency_clad                                            (NONE_CONTINGENCY)
, period                                                            (new Period)
, operation                                                                  (0)
, deleted                                                                (false)
, logging                                                                (false)
, owns_operation                                                         (false)
, resource_set_ID                                                            (0)
, operation_filename()
{  assign_unique_ID();
}
//_Common_event:_______________________________________________________________/
Common_event::Common_event(const Common_event &from_copy)
: Simple_event                                                       (from_copy)
, contingency_clad                                            (NONE_CONTINGENCY)
, period                  (from_copy.period ? new Period(*(from_copy.period)):0)
, operation                                          (from_copy.get_operation())
, deleted                                                    (from_copy.deleted)
, logging                                                                (false)
, owns_operation                                                         (false)
, operation_type_label                          (from_copy.operation_type_label)
, operation_name                                      (from_copy.operation_name)
, operation_filename                                                          ()
, resource_set_ID                                    (from_copy.resource_set_ID)
{  assign_unique_ID();
}
//_Common_event:constructor_________________________________________2002-01-27_/
Common_event::Common_event
(const Date_const &act_date
,Common_operation *operation_,bool owns_operation_ )
: Simple_event                                                                () //120716
, contingency_clad                                            (NONE_CONTINGENCY)
, period                                                            (new Period)
, operation                                                         (operation_)
, deleted                                                                (false)
, logging                                                                (false)
, operation_filename                                                          ()
, owns_operation                                               (owns_operation_)
, resource_set_ID                                                            (0)
{  assign_unique_ID();
   date.set(act_date);                                                           //121119
   begin_sync.set_as_actual(act_date);                                           //121119
}
//_Common_event:constructor_________________________________________2002-03-05_/
void Common_event::assign_unique_ID()                               assignation_
{  // unused static Seconds last_seconds;
   if (ID.empty() == 0) // Keep any existing ID.
   {  Date_time_clad_64 now;                                                     //170525
      now.append_to_string(ID _ISO_FORMAT_DATE_TIME);                            //170525
      ID.append(" (default)");
   }
}
//_assign_unique_ID_________________________________________________2002-11-18_/
Common_event::~Common_event()
{  if (period)         delete period; period = 0;
   if (operation && owns_operation) delete operation; operation = 0;
}
//_Common_event:destructor_____________________________________________________/
void Common_event::no_period()                                      assignation_
{  if (period)         delete period; period = 0;
}
//_no_period________________________________________________________2002-01-27_/
void Simple_event::copy(const Simple_event &from_copy)          modification_
{  date.set(from_copy.date);                                                     //120314
   begin_sync.copy(from_copy.begin_sync);
   enabled = from_copy.enabled;
   checked = from_copy.checked;                                                  //021012
}
//_copy_____________________________________________________________2013-04-28_/
void Common_event::copy(const Common_event &from_copy)
{  Simple_event::copy(from_copy);                                                //130428
   // do not copy association
   ID = from_copy.ID;
   operation = from_copy.operation;
   owns_operation = false;
   resource_set_ID = from_copy.resource_set_ID;                                  //030810
   deleted = from_copy.deleted;
   if (from_copy.period) period = new Period(*(from_copy.period));
}
//_copy_____________________________________________________________2002-01-27_/
int Common_event::compare(const CORN::Item &other)                         const
{  const CORN::Item  &other_assoc
      = dynamic_cast<const Item &>(other);                                       //120314
   const Common_event &other_event = dynamic_cast<const Common_event &>(other_assoc);//120314
   const CORN::Date_const &other_date = other_event.date;                        //120314
   int8 order = 0;
   CORN::date32 this_date_num  = get_date().get_date32();                        //170525_120518
   CORN::date32 other_date_num = other_date.get_date32();                        //170525120518
   if ((this_date_num  == 0) && (other_date_num  == 0))                          //120518
   {  // check synchronization start period
      CORN::date32 sync_start = begin_sync.get_date32();                         //170525
      CORN::date32 other_sync_start = other_event.begin_sync.get_date32();       //170525
      order = (sync_start == other_sync_start) ? 0 : (sync_start < other_sync_start) ? -1 : 1;
   } else
      order = (get_date() == other_date) ? 0 : (get_date() < other_date) ? -1 : 1;
   return order;
}
//_compare__________________________________________________________2010-11-25_/
bool Common_event::disenable()                                     modification_
{ enabled = false;
  return enabled;
}
//_disenable________________________________________________________2002-03-12_/
bool Simple_event::setup_structure
(Data_record &data_rec ,bool for_write)                            modification_ //020617
{  bool satup = begin_sync.setup_structure(data_rec,for_write,"event_");         //030609
//   data_rec.expect_bool("enabled",enabled);                                    //130430
   data_rec.expect_bool(LABEL_checked,checked,VV_bool_entry::form_true);         //021012
   //   #if (CS_VERSION==4)
   //   enabled is used only at runtime (not a user selection)
   //  Version 4 was saving this to the file, but only check needs to be saved.
   data_rec.expect_bool(LABEL_enabled,enabled,VV_bool_entry::form_true);         //130430
//   #endif
   return satup;
}
//_setup_structure__________________________________________________2012-07-16_/
bool Common_event::setup_structure                                               //161129
(Data_record &data_rec ,bool for_write)                            modification_ //020617
{  data_rec.expect_string("ID",ID,255);                                          //021111
   data_rec.expect_int32("res_set_ID",resource_set_ID);                          //030810
   data_rec.expect_enum("contingency",contingency_clad);                         //040806
   data_rec.expect_string("operation_type",operation_type_label,255);            //030902
   data_rec.expect_string("operation_name",operation_name,255);                  //030902
   data_rec.expect_file_name("operation_filename",operation_filename,255);       //041215
   // Warning, if you rename operation_name entry here,
   // dont forget to modify schedule viewer.
   //191010 data_rec.expect_enum("enable_for_model",enable_for_model_clad_obsolete);      //190527_041215
   if (!logging                                                                  //041015
   #ifdef LADSS_MODE
         || LADSS_mode
   #endif
         ) // We include everything in LADSS mode to verify importing
   {  // For CropSyst, we now no longer output the input parameters when logging //041015
      if (is_period(false) || !for_write)                                        //030609
         begin_sync.setup_structure(data_rec,for_write,"begin_");                //030609
      if (!is_period(false) || !for_write)                                       //030609
         Simple_event::setup_structure(data_rec,for_write);                      //120716
      if (operation)                                                             //041015
      {  if (operation->parameters_to_be_stored_in_event_file())                 //030808
            operation->setup_parameters(data_rec,for_write);
      }
      if (is_period(false) || !for_write)  // shouldn't need to log period       //030606
         period->setup_structure(data_rec,for_write);                            //021103
   }
   return true;
}
//_setup_structure_____________________________________________________________/
bool Simple_event::set_enabled(bool enabled_)                       assignation_
{ enabled = enabled_; return enabled; }
//_set_enabled______________________________________________________2001-12-21_/
bool Simple_event::is_enabled()                                     affirmation_
{ return enabled; }
//_is_enabled_______________________________________________________2001-12-21_/
bool Simple_event::is_checked()                                     affirmation_
{ return checked; }
//_is_checked_______________________________________________________2001-12-21_/
bool Simple_event::set_checked(bool checked_)                       contribution_ //200406 assignation_
{ checked = checked_ ; return checked_; }
//_set_checked______________________________________________________2002-01-34_/
bool Simple_event::set_selected(bool selected_)                     assignation_
{ selected == selected_; return selected; }                                      //011221 101129
//_set_selected_____________________________________________________2002-01-34_/
bool Simple_event::is_selected()                                    affirmation_
{ return selected; }                                                             //011221
//_is_selected______________________________________________________2002-01-34_/
bool Common_event::date_matches
(const CORN::Date_const &act_date_to_match)                         affirmation_
{  bool  dates_identical = (date == act_date_to_match);
   return  (dates_identical);
}
//_date_matches________________________________________________________________/
const char *Common_event::label_string(std::string &buffer)                const
{  // event is a date get_date().
   /*170525 now using ISO format
   date.set_format(CORN_date_DEFAULT_ORDERING,D_YYYY|D_Mmm|D_lead_zero,CORN_date_DEFAULT_SEPARATOR,true); //141120
   */
   begin_sync.append_to_string(buffer _ISO_FORMAT_DATE_TIME);
   buffer += ": ";
   append_operation_description_to(buffer);
   buffer += "(";
   buffer.append(ID); // ID is not interesting to the user
   buffer += ")";
   return buffer.c_str();
}
//_label_string_______________________________2017-04-24_2014-06-16_2002-12-07_/
const std::string &Common_event::append_to_string(std::string &target)     const
{  if (is_synchronized())
      date.append_to_string(target _ISO_FORMAT_DATE_TIME);
   else
   {  begin_sync.append_to_string(target _ISO_FORMAT_DATE_TIME);
      if (period)
      {  target.append(" until ");
         period->end_sync.append_to_string(target _ISO_FORMAT_DATE_TIME);
      }
   }
   return target;
}
//_append_to_string_________________________________________________2014-06-16_/
const char *Common_event::get_operation_name(std::string &result)          const
{  if (CORN::OS::file_system_engine.exists(operation_filename))
      operation_filename.append_components_to_string
         (result,CORN::OS::Directory_entry_name::include_name);
   else result.assign(operation_name);
   return result.c_str();
}
//_get_operation_name_______________________________________________2003-08-08_/
const std::string &Common_event::append_operation_description_to(std::string &buffer) const
{  if (operation)
      buffer.append(operation->get_detailed_description());
   return buffer;
}
//_append_operation_description_to__________________________________2014-06-16_/
void Common_event::log(std::ostream &log_file,bool not_processed ) // decendents must first call this }
{  logging = true;                                                               //041015
   /*170525 now using ISO format
   date.set_format(CORN_date_DEFAULT_ORDERING,D_YYYY|D_Mmm|D_lead_zero,CORN_date_DEFAULT_SEPARATOR,true); //141120_120314_020912
   */
   std::string sync_date_buffer;                                                 //140616
   append_to_string(sync_date_buffer);                                           //140616
   sync_date_buffer.append(operation_type_label);                                //140616
   append_operation_description_to(sync_date_buffer);                            //140616
   if (sync_date_buffer[0] != 0)                                                 //120924
      log_file << "description=" <<  sync_date_buffer << endl;                   //120924_041015
   if (not_processed && ! deleted)
      log_file << "status=" << common_translate(L_WARNING_Event_not_reached) << '!'<<  endl;
   //030610 The print the details I use data_record and setup the structure to dump the details.
   Data_record log_rec("details");                                               //030610
   setup_structure(log_rec,true);                                                //030610
   VV_Section *details_sect = log_rec.get_current_section();                     //030610
   if (details_sect) details_sect->entries.write(log_file);                      //030610
   if (operation)                                                                //020812
      operation->log(log_file);                                                  //110831_020812

}
//_log_________________________________________________________________________/
Common_event_list::Common_event_list()
: Bidirectional_list()                                                           //021101
{}
//_Common_event_list:constructor____________________________________2001-12-29_/
void Common_event_list::transfer_from
(Common_event_list &from_list
,Synchronization_mode what_to_transfer  // was Synchronizations                  //030605
,const CORN::Date_const *act_ref_date)
{  Common_event_list not_transferred;
   while (!from_list.is_empty())                                                 //030707
   {
      Simple_event *event = dynamic_cast<Simple_event  *>(from_list.pop_first());
      if ((event->begin_sync.get_sync_mode() == what_to_transfer)                //030609
          || (what_to_transfer == ANY_SYNC_MODE))                                //120719
      {
         CORN::date32 specified_date = event->begin_sync.get_date32();           //200211
         //if (event->date.is_relative() && act_ref_date)                          //170525_141120_120314_030609

         if (specified_date)
         {
            if  (specified_date < 1000) //relative                               //200218
            {

               if (act_ref_date)                                                 //200218
                  event->date.set_YD(act_ref_date->get_year(),specified_date);   //200212
               /* dont sync the date yet
               else
                  event->date.set_date32(specified_date); // sync will be updated later //200218
               */

//                event->date.set_date32
//                  (event->begin_sync.determine_from_relative(*act_ref_date));    //120314_030609

            }
            else
                event->date.set_date32(specified_date);                          //200212
         }
         append(event);                                                          //030609
      } else not_transferred.append(event);                                      //030609
   }
   // put ops not yet transferred back on the list
   from_list.transfer_all_from(not_transferred);                                 //030609
}
//_transfer_from____________________________________________________2001-03-20_/
CORN::date32 Simple_event::synchronize
(const Synchronization  &sync                                                    //030605
,const CORN::Date_const &date_to_synchronize_to)                                 //030609_020301
   // we can optionally syncronized only if the associated object matches
{  // returns the actual synchronized date (datenum)
   {  bool sync_type_matches = begin_sync.sync_type_matches(sync);
      if (sync_type_matches && !is_synchronized())                               //030609
            date.set_date32(begin_sync.determine(date_to_synchronize_to));       //170525_120314_030610
   }
   return date.get_date32();  // Date will be set if synced                      //120314
}
//_Simple_event::synchronize________________________________________2012-07-16_/
CORN::date32 Common_event::synchronize
(const Synchronization &sync                                                     //030605
,const CORN::Date_const &date_to_synchronize_to)                                 //020301
   // we can optionally synchronized only if the associated object matches
{  // returns the actual synchronized date (datenum)
   {  bool sync_type_matches = begin_sync.sync_type_matches(sync);
      if (sync_type_matches && !is_synchronized())                               //030609
            date.set_date32(begin_sync.determine(date_to_synchronize_to));       //120314_030610
      if (period
          && period->end_sync.sync_type_matches(sync)
          && (period->end_date.get_date32() == 0))
      {
         period->end_date.set_date32                                             //170523
            (period->end_sync.determine(date_to_synchronize_to));                //030610
#ifdef NYI
May need to do the following instead.
         int32 end_sync_date = period->end_sync.determine(date_to_synchronize_to);
         if (end_sync_date)                                                      //040516
         {  period->end_date.set(end_sync_date);
            period->end_sync.set(CORN::Date(end_sync_date));
         }
#endif
      }
   }
   return date.get_date32();  // Date will be set if synced                      //170525_120314
}
//_Common_event::synchronize________________________________________2002-03-01_/
nat16 Common_event_list::synchronize
(const Synchronization &sync                                                     //030103
,const Date_const &date_to_synchronize_to
) // we can optionally syncronized only if the associated object matches
{ // This synchronize is provided for before planting and reletive to sowing events.
   nat16 sync_count = 0;
   FOR_EACH(event,Simple_event,sync_for)                                         //030609
      sync_count += (nat16)(event->synchronize(sync,date_to_synchronize_to)!=0);
   FOR_EACH_END(sync_for)
   return sync_count;
}
//_Common_event_list::synchronize___________________________________2002-03-10_/
void Common_event_list::set_event_filename(const char *filename)
{  // This is used by CropSyst to identify the original management
   // filename containing the event/operation parameters.
   FOR_EACH(event,Simple_event,each_event)                                       //030609
   {  Common_event *operation_event = dynamic_cast<Common_event *>(event);       //120719
      if (operation_event)
      {
      // Don't erase any existing entry
      if (!CORN::OS::file_system_engine.exists(operation_event->operation_filename)) //120207
         operation_event->operation_filename.set_cstr(filename);                 //041215
      }
   } FOR_EACH_END(each_event)                                                    //030609
}
//_set_event_filename_______________________________________________2004-10-15_/
bool Common_event::Period::setup_structure
(Data_record &data_rec, bool for_write)                            modification_ //020617
{  bool satup = end_sync.setup_structure(data_rec,for_write,"end_");             //030608
   data_rec.expect_bool("repeat_event",repeat_event);                            //021204
   if (!for_write || repeat_event)                                               //021204
   {  data_rec.expect_int16("event_repetitions",event_repetitions);              //020313
      // a 0 value is used to disable repetitions,
      // but the event will be performed at least one time.
      //191007 data_rec.expect_enum("interval_time_step",interval_timestep_clad); //021103
      data_rec.expect_units("interval_timestep",interval_timestep);              //191007
      data_rec.expect_int16("interval",interval);        // Days, weeks, years
   }
   data_rec.expect_int16("period_repetitions",period_repetitions);               //030605
   data_rec.expect_int16("period_cycle_years",period_cycle_years);               //030605
   return satup;
}
//_Period::setup_structure_____________________________________________________/
void Common_event::Period::initialize(const Date_const&starting_from_date)
{  // Assume that operations will be repeated to the end of the
   // simulation when the actual end date is determined.
   // Once the end date is reached, the repeated operation will
   // be disabled and the events will not be repeated and will be deleted.
   Date_clad_32 date(starting_from_date.get_date32());
   interval_countdown_days = 0;
   int interval_days = 0;

   CORN::Units_code interval_TS = interval_timestep.get();                       //191010
   // Here we normalize the interval from the user specified timestep a day time step.
   if (interval_TS == UT_day)                                                    //191010
      interval_days = interval;                                                  //191010
   else if (interval_TS == UT_week) interval_days = 7 * interval;                //191010

      /* I had these for CANMS scheduling but never really used these, just use week
   else if     || if (interval_timestep == UT_sunday)
        || if (interval_timestep == UT_monday)
        || if (interval_timestep == UT_tuesday)
        || if (interval_timestep == UT_wednesday)
        || if (interval_timestep == UT_thursday)
        || if (interval_timestep == UT_friday)
        || if (interval_timestep == UT_saturday)
      {  interval_days = 7 * interval;
         Interval_timestep interval_timestep = interval_timestep_clad.get();
            if ((interval_timestep >= TS_SUNDAY) && (interval_timestep <= TS_SATURDAY))
            {  // if interval_timestep is a day of the week
               // scoot the date ahead until we reach a day of the week
               // that matches the selected day of the week
               // Note that in CORN::Date sunday = 1
               uint8 interval_DOW = (uint8)  interval_timestep - TS_SUNDAY + 1;
               while (date.day_of_week() != interval_DOW )
                     interval_countdown_days ++;
            }
      }
   interval_timestep = "day";
      */
   else if (interval_TS == UT_fortnight)interval_days = 14 * interval;
   else if (interval_TS == UT_month)    interval_days = interval *30;
   else if (interval_TS == UT_quarter)  interval_days = 3 * interval *30;
   else if (interval_TS == UT_trimester)interval_days = 4 * interval *30;
   else if (interval_TS == UT_biannum)  interval_days = 6 * interval *30;
   else if (interval_TS == UT_year)
      {  interval_days = interval *365;
         interval_days += interval / 4; // to account for leap years (more or less)
      }

   // Now switch interval to day timestep
   //191007 interval_timestep_clad.set(TS_DAY);
   interval_timestep.set(UT_day);                                                //191007


/*191007 now using units code

   switch (interval_timestep_clad.get())
   {  // Here we normalize the interval from the user specified value to a day time step.
      case TS_DAY : interval_days = interval; break;
      case TS_SUNDAY : case TS_MONDAY : case TS_TUESDAY : case TS_WEDNESDAY : case TS_THURSDAY : case TS_FRIDAY : case TS_SATURDAY : case TS_WEEK :
      {  interval_days = 7 * interval;
         Interval_timestep interval_timestep = interval_timestep_clad.get();
            if ((interval_timestep >= TS_SUNDAY) && (interval_timestep <= TS_SATURDAY))
            {  // if interval_timestep is a day of the week
               // scoot the date ahead until we reach a day of the week
               // that matches the selected day of the week
               // Note that in CORN::Date sunday = 1
               uint8 interval_DOW = (uint8)  interval_timestep - TS_SUNDAY + 1;
               while (date.day_of_week() != interval_DOW )
                     interval_countdown_days ++;
            }
      }break;
      case TS_FORTNIGHT :  interval_days = 14 * interval;     break;
      case TS_MONTH :      interval_days = interval *30;      break;
      case TS_QUARTER :    interval_days = 3 * interval *30;  break;
      case TS_TRIMESTER :  interval_days = 4 * interval *30;  break;
      case TS_BIANNUM :    interval_days = 6 * interval *30;  break;
      case TS_YEAR : // Year
               interval_days = interval *365;
               interval_days += interval / 4; // to account for leap years (more or less)
      break;
   }
   // Now switch interval to day timestep
   interval_timestep_clad.set(TS_DAY);
*/

   interval = (int16)interval_days;                                              //030120
   interval_countdown_days = interval_days;                                      //050729
   // This is needed because the first event of first event period was being repeated (interval_count_down was zero the next day)
   // The event should still be performed today, because initialized events are performed.

   // max_repetitions = 0 is used to disable the repetition parameters
   // in the event editor.  We guarentee that the event is performed at least once.
   event_repetitions_countdown = event_repetitions;                              //040901
   if (event_repetitions_countdown == 0)                                         //040901
       event_repetitions_countdown = 1;                                          //040901
   initialized = true;
}
//_Period::initialize_______________________________________________2003-05-15_/
bool Common_event::Period::update_event_repetition()
{  // This checks if the event is initialized and repeatable
   // If so it decremtns the repeat count and return true indicating the event is to be repeated.
   bool repeat_today = false;
   if (!initialized) return false;     // The event period has not been initialized yet so it is not ready to start repeating
   if (!repeat_event) return false;    // The event is not repeatable
   {  interval_countdown_days  -= 1;                                             //030515
      repeat_today   = interval_countdown_days <= 0;                             //030515
      if (repeat_today)                                                          //030515
      {  interval_countdown_days = interval; // interval will have been normalized to day timestep when period is initialized
         event_repetitions_countdown -=1;                                        //040901
   }  }
   return repeat_today;
}
//_Period::update_event_repetition__________________________________2003-06-09_/
bool Common_event::Period::update_end_period(const CORN::Date_const &today)
{  // This checks if the event is initialized and if the period synchronized to end today
   // This function should be called after update_repetition()
   bool period_ends = false;
   if (!end_date.get_date32()) return false;                                     //170525
   if (today > end_date) period_ends = true;
      // Periods end when the synchronized date is reached.
   // The end date is included                                                   //041221
   // Periods may also end on the date where the repeat count goes to 0;
   if (event_repetitions_countdown == 0) period_ends = true;                     //030614
   if (period_ends)
   {  // Set up for next period
      event_repetitions_countdown  = event_repetitions;
      if (event_repetitions_countdown == 0)                                      //040901
         event_repetitions_countdown = 1;
   }
   return period_ends;
}
//_Period::update_end_period________________________________________2003-06-09_/
bool Common_event::period_repeated_each_season() const
{  return period &&
      (   (begin_sync.get_reschedule_mode()       >= ONE_RESCHEDULE)
       || (period->end_sync.get_reschedule_mode() >= ONE_RESCHEDULE));           //080821
}
//_period_repeated_each_season______________________________________2003-06-09_/
bool Common_event::reinitialize_for_period()
{  bool seasonal = false;
   if (period)
   {  period->event_repetitions_countdown  = period->event_repetitions;
      //040901 Note: may need to check for special case
      // when event_repetitions is 0 (as in initialize)
      period->period_repetitions_countdown -= 1;
#ifdef VIC_CROPSYST_VERSION
      period->initialized = false;                                               //170314LML
      //170314LML If not being set false, the event will be triggered without initilization
      //since the deactivited event has been put into pending event list.
#endif
      if (period->period_repetitions_countdown > 0)
      {  if (period_repeated_each_season())                                      //080821
         {
            {  // reset the synchronization for the next year.
               date.set_date32(0);
               seasonal = true;
            }
            {  // reset the synchronization for the next year.
               period->end_date.set_date32(0);
               seasonal = true;
            }
         }
         if (is_synchronized())
            date.inc_years(period->period_cycle_years);                          //170525_120314
         else
            date.set_date32(0);
            // the should clear the synchronization so the phenologic synchronizations can be redetermined. //120314
   }  }
   return seasonal;
}
//_reinitialize_for_period__________________________________________2003-06-08_/
bool  Common_event::is_period(bool delete_period_if_not_valid)     modification_
{  bool is_a_period = false;
   if (period)
   {  if (period->repeat_event > 0)    return true;   // An event with repetitions is a period.
      if (is_operation_mode())         return true;   // An operation mode is a period.
      if (delete_period_if_not_valid)  no_period();
   }
   return is_a_period;
}
//_is_period________________________________________________________2002-12-04_/
bool  Common_event::is_operation_mode()                             affirmation_
{  Common_operation *op = get_operation();
   return (op && op->is_mode());
}
//_is_operation_mode________________________________________________2003-06-06_/
bool  Common_event::is_repeatable_period()                          affirmation_
{  // Periods synchronized relative to sowing date or start/end of simulation are not repeatable.
   bool is_repeatable_period = period;  // first check if it is a period
   if (period)
   {  Synchronization_mode begin_sync_mode = begin_sync.get_sync_mode();
      Synchronization_mode end_sync_mode = period->end_sync.get_sync_mode();
      if (begin_sync_mode == RELATIVE_TO_SIMULATION_EVENT_MODE)   is_repeatable_period = false;
      if (end_sync_mode   == RELATIVE_TO_SIMULATION_EVENT_MODE)   is_repeatable_period = false;
      if (begin_sync_mode == RELATIVE_TO_SOWING_MODE)             is_repeatable_period = false;
      if (end_sync_mode   == RELATIVE_TO_SOWING_MODE)             is_repeatable_period = false;
   }
   return is_repeatable_period;
}
//_is_repeatable_period_____________________________________________2003-06-05_/
bool Common_event::period_synchronized_to_growing_season()                 const
{  // Returns true if either the period start date or end date is synchronized
   // to the growing season (used for setting up period repetitions).
   bool synced_to_gs = period; // first check if it is a period (returns false if not a period)
   if (period)
      synced_to_gs =  begin_sync.is_growing_season_synchronization()
              ||period->end_sync.is_growing_season_synchronization();
   return synced_to_gs;
}
//_period_synchronized_to_growing_season____________________________2003-06-05_/
bool Common_event::period_synchronized_to_actual_date()                    const
{  // Returns true if either the period start date or end date is synchronized
   // to an actual date (used for setting up period repetitions).
   bool synced_to_act_date = period; // first check if it is a period (returns false if not a period)
   if (period)
      synced_to_act_date = begin_sync.is_actual_date_synchronization()
                         ||period->end_sync.is_actual_date_synchronization();
   return synced_to_act_date;
}
//_period_synchronized_to_actual_date_______________________________2003-06-07_/
Common_operation *Common_event::set_operation
(Common_operation *_operation
,bool _owns_operation)
{ operation = _operation; owns_operation = _owns_operation; return operation;}
//_set_operation____________________________________________________2002-11-01_/

