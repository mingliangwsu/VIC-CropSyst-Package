#ifndef CS_contextualizationH
#define CS_contextualizationH
#include "corn/application/contextualization.h"
namespace CS
{
//______________________________________________________________________________
class Contextualization
: public extends_ CORN::Contextualization
{
   bool context_is_scenarios;                                                    //181218
 public:
   inline Contextualization(bool context_is_scenarios_ = false)                  //181218
      : CORN::Contextualization()
      , context_is_scenarios(context_is_scenarios_)                              //181218
      {}
   bool is_scenario_level(const CORN::OS::Directory_name *dir = 0) affirmation_; //171227
      // underneath Scenarios but not under Output
   bool is_scenario_ultimate
      (const CORN::OS::Directory_name *dir = 0)                    affirmation_; //171227
   bool has_any_subscenario
      (const CORN::OS::Directory_name *dir = 0)                    affirmation_; //171227
   nat32 list_ultimate_scenarios                                                 //180110
      (const CORN::OS::Directory_name &level_or_ultimate_scenario
      ,CORN::Container &ultimates_list
      ,bool exclude_percent_dirs);

};
//______________________________________________________________________________
} // namespace CS
#endif
