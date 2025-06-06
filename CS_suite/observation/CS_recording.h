#ifndef CS_recordingH
#define CS_recordingH
#include "CS_suite/simulation/CS_simulation_element.h"
#include "CS_suite/observation/CS_UED_recorder.h"
namespace CS
{
//______________________________________________________________________________
class Recording_system
: public Simulation_element_abstract
{
 private:
   Examination &examination;
 public:
   Recording_system(const CORN::date32 &simdate_raw);
 public:
   virtual bool start_year()                                      modification_;
   virtual bool end_day()                                         modification_;
   virtual bool end_year()                                        modification_;
   virtual bool stop()                                            modification_;
};
//_class_Recording_system___________________________________________2017-05-28_/
} // namespace CS
#endif
