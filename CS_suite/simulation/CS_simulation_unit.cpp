#include "CS_suite/simulation/CS_simulation_unit.h"
#include "corn/OS/directory_entry_name_concrete.h"
#ifdef CROPSYST_PROPER
#include "common/simulation/event_types.h"
#endif
#ifdef MICROBASIN_VERSION
#include "common/simulation/operation.h"                                         //170321LML
#endif
namespace CS {
//______________________________________________________________________________
DECLARE_CONTAINER_ENUMERATED_SECTION(Common_event,comment_event_section_VV,true);
//______________________________________________________________________________
Simulation_unit::Simulation_unit
(modifiable_ Simulation_control  &simulation_control_
,const CORN::date32              &simdate_)
: Simulation_element_composite                                        (simdate_) //140930
, simulation_control                                       (simulation_control_) //140714
, balance_file_daily                                                         (0) //090409
, balance_file_annual                                                        (0) //090409
, balancers                                                       (sub_elements)
, event_count                                                                (0) //160713
, CS_event_log                                                               (0) //161005
{}
//_Simulation_unit:constructor______________________________________2013-06-18_/
bool Simulation_unit::initialize()                               initialization_
{  bool initialized = true;
#ifndef MICROBASIN_VERSION
   CORN::OS::File_name_concrete schedule_txt
      (get_output_directory_name(),"schedule.txt");                              //160713
   CS_event_log = new Common_simulation_log(schedule_txt.c_str());               //161005
#endif
   event_scheduler = new Event_scheduler_simulation_unit
      (simdate,simulation_control.start_date,simulation_control.stop_date
      ,performable_operations);
   initialized &= Simulation_element_composite::initialize();                    //151203
   return initialized;
}
//_initialize_______________________________________________________2015-10-01_/
Simulation_unit::~Simulation_unit()
{
   delete event_scheduler;                                                       //201129LML
   balancers.delete_all();
   delete balance_file_daily;
   delete balance_file_annual;
//NYI  if (balance_file_seasonal) delete balance_file_seasonal;
}
//_~Simulation_unit_________________________________________________2013-06-18_/
bool Simulation_unit::start()                                      modification_
{
   /**<\fn This function can be overridden to do any special
      setup at the start of the simulation.
      This will be called one time at the beginning of run().
      \return true if start was ok (and run loop can begin).
      I.e. CropSyst Start recording to output file.
   **/
   if (balancers.count())
   {
      CORN::OS::File_name_concrete balance_daily_filename
         (get_output_directory_name(),"balance_daily.dat");                      //120320
      CORN::OS::File_name_concrete balance_annual_filename
         (get_output_directory_name(),"balance_annual.dat");                     //120320
//NYI      CORN::File_name balance_seasonal_filename ("balance_daily.dat");   balance_seasonal_filename.set_path(output_dirname);
      balance_file_daily   = new std::ofstream(balance_daily_filename.c_str());
      balance_file_annual  = new std::ofstream(balance_annual_filename.c_str());
//NYI      balance_file_seasonal= new std::ofstream(balance_seasonal_filename.c_str());
      (*balance_file_daily) << "\"date\"\t";
      (*balance_file_annual) << "\"year\"\t";
//NYI      balance_file_seasonal << "\"season\"\t";

      FOR_EACH_IN(balancer, Simulation::Balancer, balancers,each_balancer)
      {  balancer->write_column_headers(*balance_file_daily);
         (*balance_file_daily) << "\t";
         balancer->write_column_headers(*balance_file_annual);
         (*balance_file_annual) << "\t";
//NYI         balancer->write_column_headers(*balance_file_seasonal);
//NYI          (*balance_file_seasonal) << "\t";
      } FOR_EACH_END(each_balancer);
      (*balance_file_daily)    << std::endl;
      (*balance_file_annual)   << std::endl;
//NYI       (*balance_file_seasonal) << std::endl;
   }
   return Simulation_element_composite::start();
}
//_start____________________________________________________________2013-06-18_/
bool Simulation_unit::end_year()                                   modification_
{
   // Balancer end_year() must be done after end_year() to make sure the components have added any
   // last minute accumulations.
   if (balance_file_annual && balancers.count()) (*balance_file_annual)
      << Simulation_element_composite::simdate.get_year() << '\t';               //150616RLN
   bool ended = Simulation_element_composite::end_year();
   if (balance_file_annual) (*balance_file_annual)   << std::endl;               //090409
   return ended;
}
//_end_year_________________________________________________________2013-06-18_/
bool Simulation_unit::end_day()                                    modification_
{  if ( balance_file_daily&&balancers.count())
      (*balance_file_daily) << Simulation_element_composite::simdate.as_string() //150616RLN
      << '\t';
   bool ended = Simulation_element_composite::end_day();
   if (balance_file_daily) (*balance_file_daily)    << std::endl;                //090409
   performable_operations.delete_all();                                          //151003
      // Note that the operations themselves are not deleted, only the
      // items
   return ended;
}
//_end_day__________________________________________________________2013-06-18_/
nat32 Simulation_unit::take_balancer(Simulation::Balancer *balancer)
{  // This gives the balancer to the simulation to maintain accumulators.
   balancers.append(balancer);                                                   //070227
   return (nat32)balancer;
}
//_take_balancer____________________________________________________2013-06-18_/
bool Simulation_unit::process_day()                                modification_
{
   FOR_EACH_IN(performable_op
      ,Event_scheduler_common::Performable_operation
      ,performable_operations,each_op)
   {
      Common_operation &op = *(performable_op->operation_event.get_operation()); //170310
      performable_op->event_status                                               //170310
      = (performable_op->terminate)                                              //170310
      ? terminate_operation(op)                                                  //170310
      :   perform_operation(op);                                                 //170310
      #ifdef CROPSYST_PROPER
      event_count++;
      (*CS_event_log) << "[event_" << event_count << "]" << std::endl;
      performable_op->operation_event.write(*CS_event_log);
      (*CS_event_log)
         << "status="
         << ((performable_op->event_status <= 8)
            ? event_status_label_table[performable_op->event_status]
            : "unknown" )
         << std::endl;
      #endif
   } FOR_EACH_END(each_op)
   return Simulation_element_composite::process_day();
      // scheduler is now member of Simulation_element_abstract
      // so scheduling will be handled by Simulation_element_abstract
      // Events to be scheduled are setup in start_day().
}
//_process_day______________________________________________________2013-08-30_/
}//_namespace_CS_______________________________________________________________/

