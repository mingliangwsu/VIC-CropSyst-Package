#ifndef CS_InspectionH
#define CS_InspectionH
#include "corn/container/item.h"
#include "CS_suite/observation/CS_inspector.h"
#include "CS_suite/simulation/CS_period.h"
#include "corn/math/statistical/sample_cumulative_T.h"
#include "corn/math/statistical/sample_dataset_T.h"
#include "corn/metrology/units_clad.h"
//______________________________________________________________________________
namespace CS
{
   enum Inspection_level
      { continuous_inspection
      , annual_inspection
      , COUNT_inspection
      };

   #define ANY_INSPECTION COUNT_inspection
   extern char  inspection_level_codes[];
   extern char *inspection_level_labels[];
//______________________________________________________________________________
class Inspection
: public extends_ CORN::Item
{
 public:
   class Period
   : public extends_ CS::Period_clad
   {public:
      const Emanator   *context;
      // need reference to emanator
      // so when the period is completed we only complete periods for the emanator which is completed
      // 0 context (not specified) indicates global context

      // conceptual period probably should offer all the elements of
      // UED periods.
    public:
      inline Period
         (CORN::datetime64 /*200409 const CORN::Temporal   &*/temporality_
         ,nat8                    level_or_period_
         ,const std::string      &period_name_
         ,const Emanator         *context_)
         : CS::Period_clad          (temporality_,level_or_period_,period_name_)
         , context                                                    (context_)
         {}

   };

 public:
int32 debug_ID;
 public:
   const Inspector_abstract  &inspector;
   const Period              &period;                                            //171115
   CORN::Units_clad  timestep;                                                   //170319
      // The timestep may be better suited in the inspector
      // on the otherhand Inspection deals with time (period)
 private:
   CORN::statistical::Sample *tally;                                             //200701
   CORN::statistical::Sample_dataset_clad<float64,float32,float32> *tally_as_float;
 public:
   Inspection
      (const Inspector_abstract &inspector
      ,const Period             &period                                          //171115
      ,CORN::Units_code          timestep                                        //170319
      ,nat32                     applicable_descriptive_summary_elements);
      // The instanciator of the inspector list
      // may specify the types of statistics that will be tallied.
      // However certain statistics may only be applicable to a variable
      // This inspector's instanciator will further filter
      // the desired descriptive summary elements to what is actually applicable.
      // The record dataset if provided will recording all the commited values.
      // This is useful to apply additional analysis or output collected data.
   virtual ~Inspection();
   virtual int   compare(const CORN::Item &)                              const;

   virtual bool commit()                                          modification_;
   /*conceptual write desired tally elements
   virtual bool write(std::ostream &)                              performs_IO_;
      // Currently this write simply identifies the qualified string
      // Eventually I may have a write 'mode'
      // to optionally write the observed value (or values) and statistics.
   */
   inline virtual bool reset()
      { bool sat = true; if (tally) sat &= tally->reset(); return sat; }         //170404_170323
   inline bool records_dataset()                                    affirmation_
      { return tally_as_float != 0; }                                            //170318
   CORN::Dynamic_array<float32> *mod_tally_as_float32()           modification_; //160609
   bool is_tally_statistic_applicable(CORN::Statistic statistic)   affirmation_; //160617
   float64 get_tally_statistic(CORN::Statistic statistic)                 const; //160617
};
//_Inspection_______________________________________________________2016-06-13_/
class Inspections
: public CORN::Unidirectional_list // of Inspection (Inspector_tally)
{
 public:
   nat32 desired_descriptive_summary_elements;                                   //150818
 public:
   inline Inspections()
      : CORN::Unidirectional_list(true)                                       {}
   bool reset()                                                   modification_; //150819
   bool commit()                                                 rectification_;
};
//_Inspections______________________________________________________2016-06-13_/
}//_namespace CS_______________________________________________________________/

#endif
