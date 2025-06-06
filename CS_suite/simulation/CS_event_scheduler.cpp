#include "CS_suite/simulation/CS_event_scheduler.h"
#include "common/simulation/operation.h"
#include "common/simulation/log.h"
#include "common/simulation/balancer.h"
#include "corn/application/explaination/explainations.h"

namespace CS {
//______________________________________________________________________________
class Year_event_list
: public implements_ CORN::Item
, public extends_ CORN::Enumeration_list
{
   Year year;
 public:
   inline Year_event_list(Year year_)
      : year(year_)
      {}
   inline virtual bool is_key_nat32(nat32 key)                      affirmation_  //180820
      { return (nat32) year == key; }
};
//_class Year_event_list____________________________________________2015-12-03_/
Event_scheduler_common::Event_scheduler_common
(const CORN::Date_const &today_
,CORN::Unidirectional_list &performable_operations_)
: today_scheduled       (today_)                                                 //151001
, queueA                ()                                                       //090309
, queueB                ()                                                       //090309
, completed_events      ()                                                       //090309
, events_for_next_season()                                                       //090309
, active_queue           (&queueA)                                               //040820
, pending_events         (0)                                                     //151003
, performable_operations(performable_operations_)                                //151003
, aborted               (false)                                                  //040806
, viable                (true)                                                   //040816
{}
//_Event_scheduler_common:constructor__________________________________________/
Event_scheduler_simulation_unit::Event_scheduler_simulation_unit
(const CORN::Date_const &today_
,const CORN::Date_const &start_date_                                             //101221_021214
,const CORN::Date_const &stop_date_                                              //101221_021214
,CORN::Unidirectional_list &performable_operations_)                             //151003
: Event_scheduler_common
   (today_,performable_operations_)                                              //151003
, ref_start_date        (start_date_)
, ref_stop_date         (stop_date_)
, year_event_lists      ()                                                       //151203
{}
//_Event_scheduler_simulation_unit_____________________________________________/
Event_scheduler_common::~Event_scheduler_common()
{
    queueA.delete_all();                                                         //201129LML
    queueB.delete_all();                                                         //201129LML
    completed_events.delete_all();                                               //201129LML
}
//______________________________________________________________________________
Event_scheduler_simulation_unit::~Event_scheduler_simulation_unit()
{}
//_~Event_scheduler_simulation_unit_________________________________2009-04-09_/
nat32 Event_scheduler_common::take_events
(modifiable_ CORN::Container &events)                              modification_
{  return active_queue->transfer_all_from(events);
}
//_take_events______________________________________________________2015-09-30_/
bool Event_scheduler_simulation_unit::start()                      modification_
{  // Note that the start date is always the target start date
/* NYI need to write explaination
   #if (CS_VERSION==4)
   if (event_log)
   {  ref_start_date.set_format(D_YMD,D_YYYY|D_Mmm|D_lead_zero,'/',true);
      event_log->record(get_ID(),"event","start","start of simulation");         //030306
      *event_log << "status=start"<< std::endl; // This shows the start icon in the schedule viewer
      *event_log << "model=simulation"<< std::endl;
   }
   #else
   // NYI implement using Explainations
   #endif
*/
#ifndef VIC_CROPSYST_VERSION
   provide_explainations_global().take
      (new Explaination
         (CORN::Explaination::severity_information
         ,"%SUITE_DOC%/common/explaination/period.html#start"                    //151215
            // not written this yet (may actually want to refer to the documentation
         ,"" // no additional comment
         ,"" // no context
         ,today_scheduled.get_datetime64()));
#endif
   {  Synchronization rel_to_start_date_sync(SIMULATION_BEGIN_DATE,0);           //030605_110505 moved from run()
      synchronize(rel_to_start_date_sync,ref_start_date);                        //031010
   }                                                                             //030605
   {  Synchronization rel_to_end_date_sync(SIMULATION_END_DATE,0);               //030605
      synchronize(rel_to_end_date_sync,ref_stop_date);                           //031010
   }
   for (Simple_event *event = dynamic_cast<Simple_event *>
            (active_queue->pop_at_head())                                        //151203
         ;event
         ;event = dynamic_cast<Simple_event *>(active_queue->pop_at_head()))
   {  CORN::Year event_year = event->date.get_year();
      Year_event_list *year_event_list = dynamic_cast<Year_event_list *>
         (year_event_lists.find_nat32(event_year));
      if (!year_event_list)
      {  year_event_list = new Year_event_list(event_year);
         year_event_lists.append(year_event_list);
      }
      year_event_list->append(event);
   }
   return Event_scheduler_common::start();
}
//_start____________________________________________________________2009-04-09_/
bool Event_scheduler_simulation_unit::start_year()                 modification_
{  bool started =  Event_scheduler_common::start_year();                         //151203
   CORN::Year this_year = today_scheduled.get_year();                            //151203
   Year_event_list *this_year_events = dynamic_cast<Year_event_list *>
      (year_event_lists.find_nat32((nat32)this_year));
   if (this_year_events)
   {
      active_queue->transfer_all_from(*this_year_events);
      year_event_lists.remove(this_year_events);
   }
   return started;
}
//_start_year_______________________________________________________2015-10-04_/
bool Event_scheduler_simulation_unit::stop()                       modification_
{  // Note that the actual stop date may not be the desired/target stop date
// Note if we are always passing today, then simply use the today reference
/*151013 NYI need to write explaination
   #if (CS_VERSION==4)
   if (event_log)
   {  const CORN::Date &act_stop_date = get_today();                             //131020
      #if (CS_VERSION==4)
      event_log->record((nat32)
         get_ID(),"event","stop","end of simulation");                           //030306
      *event_log << "date=" <<act_stop_date.get() << " (" << act_stop_date.as_string() << ")" << std::endl;
      *event_log << "status=stop"<<std:: endl;
      *event_log << "model=simulation"<< std::endl;
      #else
      // NYI implment usng explainations
      #endif
   }
   #endif
*/
#ifndef VIC_CROPSYST_VERSION
   provide_explainations_global().take
      (new Explaination
         (CORN::Explaination::severity_information
         ,"%SUITE_DOC%/common/documentation/explaination/period.html#stop"
         ,"" // no additional comment
         ,"" // no context
         ,today_scheduled.get_datetime64()));
#endif
   return true;
}
//_stop________________________________________________________________________/
bool Event_scheduler_common
::log_contingency(Contingency contingency)                         modification_
{
/* NYI need to write explaination
   #if (CS_VERSION==4)
   if (event_log)
   {  event_log->record((nat32)
         get_ID(),"event",contingency_label_table[contingency]," contingency");  //030306
      *event_log << "status="<< contingency_label_table[contingency]<<std::endl;
      *event_log << "model=simulation"<< std::endl;
   }
   #else
   // use explaination
   #endif
*/
#ifndef VIC_CROPSYST_VERSION
   provide_explainations_global().take
      (new Explaination
         (CORN::Explaination::severity_information
         ,"%SUITE_DOC%/common/documentation/explaination/contigency.html#stop"
         ,contingency_label_table[contingency]
         ,"" // no context
         ,today_scheduled.get_datetime64()));
#endif
      return true;
}
//_log_contingency__________________________________________________2006-08-06_/
nat16  Event_scheduler_common::synchronize
(const Synchronization &sync
,const Date_const &date_to_synchronize_to)
{
   // This function scans the scenario events list for
   // matching syncronizations.
   // It creates new actual date events based on the date to
   // syncronize to.
   // The actual events operation pointer currently is normally
   // set to the potential event, because  syncronization may
   // produce multiple actual events based on the potential
   // event definition, we dont delete or convert potential
   // events to actual events, we do move the potential events
   // to the synced_pot_events so we can optimize the matching
   // process not having to scan for events that have already
   // been syncronized and probably already processed.
   // Returns the number of events added to the act_events
   // list for the syncronization.

   return active_queue->synchronize                                              //030609
      (sync,date_to_synchronize_to);
}
//_synchronize_________________________________________________________________/
bool Event_scheduler_common::legacy_schedule()                                   //151016_150611RLN
{  pending_events = (active_queue == &queueA) ? &queueB : &queueA;               //151003 now member
      // Any events not processed today get put here and then back on the event list at the end of the loop
   Simple_event *event = 0;

   //std::clog << "schedule date:" << today_scheduled.get_date32() << std::endl;

   while ((event = dynamic_cast<Simple_event  *>(active_queue->pop_first())) != 0)
   {  bool event_time_passed = (event->is_synchronized() ) && (event->date < today_scheduled); //120719_120314
/*
if (event->date == today_scheduled)
std::clog << "reached event date debug" << std::endl;
*/

      Common_event  *operation_event = dynamic_cast<Common_event  *>(event);

/*
if (operation_event && (operation_event->ID == "2019051.3870836806 (default)"))
std::clog << std::endl<<"AutoIrrig" << std::endl;
*/
      bool can_be_performed_simulation_start_date = true;
      if (operation_event)
      {  Common_operation *op = operation_event->get_operation();
         #ifdef LADSS_MODE
         if (!op)
         {  // The operation is not yet loaded or not stored in the event list
            // the scenario may know how to load the operation.
            bool relinquish_op_to_event;
            op = get_operation(operation_event->get_operation_type_label(),operation_event->operation_name,operation_event->operation_filename,operation_event->resource_set_ID,relinquish_op_to_event);
            operation_event->set_operation(op,relinquish_op_to_event);
         }
         #endif
         if (op)                                                                 //120719
            can_be_performed_simulation_start_date                               //120719
               = op->can_be_performed_before_simulation_start_date();            //120719
         else { delete event; event = operation_event = 0; }                     //120719
         // Events which the user has marked as not checked
         // can be removed since they are not to be simulated.
         // Also events must have an operation.
         // Also events that are not periods whose start date has passed.
         if (operation_event)
         {  bool is_period = operation_event->is_period(true);
            if (is_period) event_time_passed = false;                            //120719
            bool event_checked = operation_event->is_checked();
            if (!event_checked)
            { delete event; event = operation_event = 0; }
         }
      }
      if (event)                                                                 //120719
      {
         if (event_time_passed)
            delete event;
         else if (!event->is_enabled() ||
               !event->is_checked())                                             //130430
            completed_events.append(event); // Disabled events go to completed events
         else
         {  bool iniciate_event = (event->date == today_scheduled)               //120314
               || (   (event->date.get_date32() > 0)                             //120725
                   && (event->date <= today_scheduled)                           //120314
                   && (can_be_performed_simulation_start_date));
            if (operation_event)                                                 //120719
            {  bool repeat_event = false;
               bool end_event    = false;
               bool is_period = operation_event->is_period(true);                //120719
               Common_operation *op = operation_event->get_operation();
               if (is_period)
               {  if (!operation_event->period->initialized && iniciate_event)
                     operation_event->period->initialize(today_scheduled);
                  else // else is needed othewise we decrement the event repetitions countdown immediately
                     repeat_event = operation_event->period->update_event_repetition();
                  if (repeat_event)
                        operation_event->date.set(today_scheduled);
                        // Update the event date so it shows the current date.   //120314
                  end_event = operation_event->period->update_end_period(today_scheduled);
                  if (end_event)
                     operation_event->date.set(operation_event->period->end_date);
                        // This will allow the end date to show in the output log
                        // (instead of the period start date) //120314
               }
                   // Note we use >= in end event because the simulation may request
                  // the event to remain enabled a little bit longer
               nat32 my_ID = (nat32)this; // Make this a member, it only needs to be done once
               if (iniciate_event|| (repeat_event
                   && !end_event))  // I had this commented out on Nov 11 2004, a problem Luca had but this causes problems for events generated by grazing optimization model.
                                  // This is right for the optimization model.
                                  // Indeed completed periods are not moved to completed with out this.
                                  // I think the problem was with only the irrigation on the very last date of the period I fixed this
                  // I dont think I want to perform the operation
                  //                      ,on the end date (dont want to do it if it is a repeated event and the repetition was not scheduled for this day)
                  //                         the end operation period can do this if specific operations need it.

               {
/*
if (operation_event && (operation_event->ID == "2019051.3870836806 (default)"))
std::clog << "reached2" << std::endl;
*/


                  /*151013 NYI Need to write explaination
                  #if (CS_VERSION==4)
                  if (event_log)
                  {  event_log->record((nat32)
                  #if (__LP64__ || _M_X64)
                  (uint64_t)
                  #endif
                        this,"event");
                        operation_event->log(*event_log,false);
                     *event_log<< "repeated="<< (repeat_event? "true": "false") << std::endl;
                  }
                  #endif
                  */
                  std::string operation_name;
#ifndef VIC_CROPSYST_VERSION
                  provide_explainations_global().take                            //151013
                     (new Explaination
                        (CORN::Explaination::severity_information
                        ,"%SUITE_DOC%/common/documentation/explaination/repetition.html"
                        ,operation_event->get_operation_name(operation_name)
                        ,operation_event->ID
                        ,today_scheduled.get_datetime64()));                     //170525
#endif

                  // Temporarily give the event to performed event queue
                  // which is share with simulation engine drived classes
                  // to apply the operation
                  //170310 moved nat32 my_ID = (nat32)this; // Make this a member, it only needs to be done once

                  performable_operations.append
                     (new Performable_operation(*operation_event,my_ID,false));  //170310
/*200421
                  if (is_period)
                     pending_events->append(event);
                  else  // single event:
*/
                  {  switch (event->begin_sync.get_reschedule_mode())            //060412
                     {  case MULTIPLE_RESCHEDULE :                               //060412
                           event->date.set_date32(0);                            //170525
                           pending_events->append(event);
                        break;
                        case ONE_RESCHEDULE : // schedule once each season (year)
                           event->date.set_date32(0);                            //170525
                           events_for_next_season.append(event);
                        break;
                        default : //  NO_RESCHEDULE

                           if (is_period)                                        //200421
                           {
                              // I move this here from above for compatibility
                              // but I dont think it is actually relevent
                              pending_events->append(event);                     //200421
                           }
                           completed_events.append(event);
                        break;
                     }
                  }
               } else
                  if (end_event)
                  {  // we give the simulation the opportunity to close down other things.
                     performable_operations.append                               //170310
                        (new Performable_operation(*operation_event,my_ID,true));
                     std::string operation_name;
                     operation_event->get_operation_name(operation_name);        //170310
#ifndef VIC_CROPSYST_VERSION
                     provide_explainations_global().take                         //151013
                     (new Explaination
                        (CORN::Explaination::severity_information
                        ,"%SUITE_DOC%/common/documentation/explaination/period.html#event_end"
                        ,operation_name
                        ,operation_event->ID
                        ,today_scheduled.get_datetime64()));                     //170525
#endif
                     event->date.clear();                                        //120314_041117
                     bool seasonal = operation_event->reinitialize_for_period(); //041117
                     if (seasonal)  {
#ifdef VIC_CROPSYST_VERSION
                         pending_events->append(event);
                         //170314LML the "Event_scheduler_crop::end_season()" seems never being called
                         // so here to keep the repeat events "alive" in the event lists
#endif
                         events_for_next_season.append(event);                   //041117
                     }
                     else           completed_events.append(event);              //041117
                  }
                  else              pending_events->append(event);
            } // operation_event
            else
            {
               if (iniciate_event)                                               //130429
               {
                  #ifndef YAML_PARAM_VERSION
                  Event_status_indicator event_status = process_model_specific_event(event);
                  if (event_status == ES_COMPLETE)                               //130428
                  {
//180615                     completed_events.append(event);                             //130428

//This is a quick and dirty fix to ALWAYS have  perennial crop adjustments to be
//rescheduled the next season. It should check the reschedule mode!!!
event->date.set_date32(0);
//events_for_next_season.append(event);                   //180615
active_queue->prepend(event);                   //180615



                     event = 0;                                                  //130428
                  }
                  #endif
               } else                                                            //120719
                     pending_events->append(event);                              //120719
            } //else operation_event
         } // event_time_passed
      } // event
   } // while
   active_queue = (active_queue == &queueA) ? &queueB : &queueA;                 //040820
   return true;
}
//_start_day______________________________________________________________2000_/
bool Event_scheduler_common::start_day()                                         //151016_150611RLN
{
   bool started = legacy_schedule();
   return started;
}
//_start_day______________________________________________________________2000_/
bool Event_scheduler_common::end_day()                             modification_
{
   // By now performable ops should have been performed by simulation unit.
   nat32 my_ID = (nat32)this; //make this a member
   FOR_EACH_IN(performed_op,Performable_operation,performable_operations,op_iter)
   {  if (performed_op->owner_ID == my_ID)
        if (aftermath(performed_op->operation_event,performed_op->event_status))
            op_iter->delete_current();
   } FOR_EACH_END(op_iter)
   return true;
}
//_end_day__________________________________________________________2015-10-03_/
bool Event_scheduler_common::aftermath
(Common_event &operation_event
,Event_status_indicator &event_status)
// Needs to be modifiable because we may postpone or otherwise reschedule the event date
{  bool performed = true;
         // Perform operation may produce new actual events (I.e. sowing loads managements)
         // so we need to synchronize:
      if (event_status == ES_FAIL)                                               //040806
      {  switch (operation_event.contingency_clad.get())
         {  case ABORT_CONTIGENCY      : log_contingency(operation_event.contingency_clad.get()); aborted = true; break;
            case POSTPONE_CONTINGENCY  : event_status = ES_POSTPONE;  break;
            case STOP_CONTINGENCY: log_contingency(operation_event.contingency_clad.get()); viable = false; break;
            case NONE_CONTINGENCY      :
            default :  // Do nothing, the event will just be eaten
            break;
         }
      }
      if (event_status == ES_POSTPONE)                                           //040702
      {
         Date_clad_32 delay_date(today_scheduled.get_date32()); delay_date.inc_day(); //170530_170525
         Common_event  *postponed_event = new Common_event(delay_date,operation_event.get_operation());
         postponed_event->ID = operation_event.ID;
            // Make sure we keep the reference to the original event for the schedule viewer.
         postponed_event->operation_name.assign(operation_event.operation_name);
            // Make sure we keep the reference to the original event for the schedule viewer.
         postponed_event->operation_filename.assign(operation_event.operation_filename);
            // Make sure we keep the reference to the original event for the schedule viewer.
         postponed_event->contingency_clad.set(POSTPONE_CONTINGENCY);
         pending_events->append(postponed_event);
      }
      bool is_period = operation_event.is_period(true);                          //151003
      if ((event_status < ES_ERROR) && is_period)
         event_status &= event_status_started;
      // For event periods we disable the event when this date is reached.
   return performed;
}
//_aftermath________________________________________________________2015-10-03_/
}//_namespace_CS_______________________________________________________________/



