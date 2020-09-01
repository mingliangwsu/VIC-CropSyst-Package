#ifndef event_typesH
#define event_typesH
//______________________________________________________________________________
#include "corn/enum.h"
//______________________________________________________________________________
/*191007 now use interval units code
enum Interval_timestep
{  TS_DAY
,  TS_WEEK
,  TS_FORTNIGHT
,  TS_MONTH
,  TS_TRIMESTER
,  TS_QUARTER
,  TS_BIANNUM
,  TS_YEAR
,  TS_SUNDAY
,  TS_MONDAY
,  TS_TUESDAY
,  TS_WEDNESDAY
,  TS_THURSDAY
,  TS_FRIDAY
,  TS_SATURDAY
,  TS_NEVER
, TS_COUNT
};
extern const char *interval_timestep_label_table[];
DECLARE_ENUM_CLAD(Interval_timestep_clad,Interval_timestep,interval_timestep_label_table);
*/
//_Interval_timestep___________________________________________________________/
// In the case an event cannot be performed there are optional contigencies      //040806
// NONE/IGNORE  simply does not process the event with no error and it is not rescheduled
// POSTPONE reschedules the event for the next day.
// STOP     shut down the simulation nicely (saving outputs as if the stop date had been reached), user may interpret this case as a non viable simulation.
// ABORT    immediately terminates the simulation
enum Contingency
{ NONE_CONTINGENCY        //180104 rename to CONTINUE_CONTIGENCY or IGNORE
//NYI , PARTIAL_CONTIGENCY    may need to add this for grazing model
, POSTPONE_CONTINGENCY
, STOP_CONTINGENCY // NOT_VIABLE_CONTINGENCY  //180104 rename to STOP_CONTIGENCY
, ABORT_CONTIGENCY
, CONTINGENCY_COUNT
};
extern const char *contingency_label_table[];
DECLARE_ENUM_CLAD(Contingency_clad,Contingency,contingency_label_table);
//_Contingency_________________________________________________________________/
#endif

