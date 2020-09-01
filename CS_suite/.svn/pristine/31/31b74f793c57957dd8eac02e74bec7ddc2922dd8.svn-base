#include "CS_suite/observation/CS_emanator.h"
#include "CS_suite/observation/CS_examination.h"
#include <assert.h>

#include <iostream>
namespace CS {
CORN::Unidirectional_list inactive_emanators_global;
//______________________________________________________________________________
Emanator::Emanator
(nat32                ID_
,const std::string   &instance_name_
,const std::string   &ontology_
,bool                 ephemeral_
,const Emanator      *context_)
: CORN::Item() //Enumeration()
, ephemeral             (ephemeral_)                                             //160616
, ID                    (ID_)
, instance_name         (instance_name_)
, ontology_domain       (ontology_)
, context               (context_)
, status                (normal_status)
{}
//_Emanator:constructor________________________________________________________/
Emanator::~Emanator()
{
   // Since we are going out of scope make sure any
   // continuous inspections are closed.
      // In most cases the examination will have already been instanciated
      // (if an examination is instanciated now, it is just going to be a dummy
      // so the date is completely irrelevent at this point).                    //171114
   CS::get_examination_global()                                                  //171114
      .complete_period_inspection(CS::continuous_inspection,*this,true);         //170319
}
//_Emanator:destructor_________________________________________________________/
/*170322 currently just listing the current domain, although I may need to restore this
nat16 Emanator::list_ontology_path(CORN::Text_list &qualified_ontology)    const
{  if (context)
       context->list_ontology_path(qualified_ontology);
   if (ontology_domain.length())      qualified_ontology.add_string(ontology_domain);
   return qualified_ontology.count();
}
*/
//_2016-06-10___________________________________________________________________
const char *Emanator::append_ontology_domain_name_to
(std::string &append_to,char delimiter)                                    const
{  if (!ontology_domain.empty())
   {  append_to.append(ontology_domain);
      if (delimiter) append_to += delimiter;
   }
   return append_to.c_str();
}
//_append_ontology_domain_name_to___________________________________2017-03-22_/
nat16 Emanator::list_instance_path(CORN::Text_list &qualified_instance)    const
{
   if (context) context->list_instance_path(qualified_instance);
   if (instance_name.length()) qualified_instance.add_string(instance_name);
   return qualified_instance.count();
}
//_list_instance_path_______________________________________________2016-06-10_/
Emanators::Emanators
(const CORN::Temporal &temporality_curr_)                                        //171115
: CORN::Enumeration_list(true)
, temporality_curr(temporality_curr_)                                            //171115
{}
//_Emanators:constructor____________________________________________2015-08-18_/
Emanator &Emanators::know
(nat32              emanator_ID
,const std::string &instance_name
,const std::string &ontology
,bool               ephemeral                                                    //160616
,const Emanator    *context
,const CORN::Temporal &temporality                                               //171115
//abandoned ,CORN::datetime64    start_date
)                                                 //200409
{  Emanator *new_emanator = new Emanator
      (emanator_ID,instance_name,ontology,ephemeral,context);
   bool taken = take(new_emanator);
   // It is presumed that all emanators on becoming known
   // will be inspected continuously through out the duration.
   CS::Inspection::Period *complete_examination_period                           //200409_171115
      // I think the complete examination period
      = new CS::Inspection::Period
         (temporality.get_datetime64()                                           //200409
         //200409 temporality
         ,CS::continuous_inspection,"",context);        //190124_171116
   CS::get_examination_global().periods.take(complete_examination_period);       //171115
   if (taken)
   {
      CS::get_examination_global().add_period_inspection                         //170319
         (*complete_examination_period                                           //171115
         ,UT_day //timestep of the inspected data
            // Currently only daily emanators
            // but should be parameter
         ,*new_emanator
         ,STAT_BASIC_ELEMENTS_INIT_FINAL_bits);
   } else
   {
      std::clog << "error: duplicate emanator" << std::endl;
   }
   return *new_emanator;
}
//_know_____________________________________________________________2015-08-18_/
bool Emanators::forget
(nat32 emanator_ID)                                                 //cognition_
{  bool forgot = false;
   Emanator *identified_emanator = dynamic_cast<Emanator *>
      (find_nat32(emanator_ID));
   if (identified_emanator)
   {
      forgot = CS::get_examination_global()                                      //171115
         .complete_period_inspection                                             //170521
         (ANY_INSPECTION,*identified_emanator,true);                             //171222
      identified_emanator->set_status(Emanator::deleted_status);
      Emanator *detached_emanator = dynamic_cast<Emanator *>
         (detach(identified_emanator));                                          //170521
      inactive_emanators_global.take(detached_emanator);                         //170522
   }
   return forgot;
}
//_forget___________________________________________________________2015-08-18_/
Emanator::Status Emanators::get_status_of(nat32 emanator_ID)               const
{  Emanator::Status identified_status = Emanator::unknown_status;
   const Emanator *identified_emanator = dynamic_cast<const Emanator *>
      (find_nat32(emanator_ID));
   if (identified_emanator)
      identified_status = identified_emanator->get_status();
   return identified_status;
}
//_get_status_of____________________________________________________2015-08-18_/
Emanators *emanators_global;
Emanators &instantiate_emanators(const CORN::Temporal &temporality_curr)
{  if (!emanators_global)
      emanators_global = new Emanators(temporality_curr);                        //171114
   return *emanators_global;
}
//_instantiate_emanators____________________________________________2017-11-15_/
Emanators &get_emanators()
{  if (!emanators_global)
   {  //std::cerr << "error(programming): emanators have not been instanciated" << std::endl;
      //exit(1); // should use a specific standard error code
   }
   return *emanators_global;
}
//_get_emanators____________________________________________________2017-11-15_/
bool Emanators::start_year()                                       modification_
{  bool started = true;
   FOR_EACH(emanator,Emanator,each_emanator)
   {
      if (emanator->is_relevant_annually())
      {
         CS::Examination &examination = CS::get_examination_global();            //171115
         CS::Inspection::Period *annual_period = new CS::Inspection::Period                                  //171115
            (temporality_curr.get_datetime64()                                   //200409
            ,CS::annual_inspection,"",emanator);                                 //190124_171116
         examination.periods.take(annual_period);                                //171115
         started &= examination                                                  //171115
         .add_period_inspection
         (*annual_period,UT_day                                                  //171115
          //timestep of the inspected data
         ,*emanator,STAT_BASIC_ELEMENTS_INIT_FINAL_bits);
      }
   } FOR_EACH_END(each_emanator)
   return started;
}
//_start_year________________________________________________________2017-3-19_/
bool Emanators::end_year()                                         modification_
{  bool ended = true;
   FOR_EACH(emanator,Emanator,each_emanator)
   {
      if (emanator->is_relevant_annually())
        ended &= CS::get_examination_global()                                    //171115
         .complete_period_inspection
         (CS::annual_inspection,*emanator,false);                                //171222
   } FOR_EACH_END(each_emanator)
   return ended;
}
//_end_year__________________________________________________________2017-3-19_/
bool Emanators::start(CORN::datetime64 inception_date)             modification_
{  bool started = true;
   FOR_EACH(emanator,Emanator,each_emanator)
   {
      Examination &examination = get_examination_global();                       //171115
      CS::Inspection::Period *examination_period = new CS::Inspection::Period    //200409_171115
         (inception_date //200409temporality_curr
         ,CS::continuous_inspection,"",emanator);               //190124_171116
      examination.periods.take(examination_period);                              //171115
        started &= examination                                                   //171115
         .add_period_inspection
         (*examination_period
         ,UT_day //timestep of the inspected data
         ,*emanator,STAT_BASIC_ELEMENTS_INIT_FINAL_bits);
   } FOR_EACH_END(each_emanator)
   return started;
}
//_start____________________________________________________________2017-04-14_/
bool Emanators::stop()
{  bool stopped = true;
   FOR_EACH(emanator,Emanator,each_emanator)
   {
      stopped &= CS::get_examination_global()                                    //171115
         .complete_period_inspection
         (ANY_INSPECTION,*emanator,false);                                       //171222
      emanator->set_status(Emanator::stopped_status);
   } FOR_EACH_END(each_emanator)
   return stopped;
}
//_stop_____________________________________________________________2017-03-24_/
}//_namespace_CS_______________________________________________________________/

