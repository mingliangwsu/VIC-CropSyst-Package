#error this class is obsolete now use CWPF
#ifndef cropprodH
#define cropprodH
/* 990616
This analysis class was originally written for Armen
It does the following:
Given a simulation that has been run
*/
#include "corn/OS/directory_entry_name_concrete.h"
#include "CropSyst/source/cs_events.h"
#include "CropSyst/source/cs_scenario.h"
#include "CropSyst/source/cs_scenario_directory.h"

#include <vcl/ComCtrls.hpp>
#include <vcl/filectrl.hpp>
#include "corn/dynamic_array/dynamic_array_T.h"
//______________________________________________________________________________
class CPF_Irrigation_event
: public Common_event
{
public:
   float32 amount;
public:
   inline CPF_Irrigation_event
      (  const CORN::Date &_date
      ,  float32           _amount)
      : Common_event (_date)                                                     //011221
      , amount       (_amount)
      {}
   //190718 obs virtual const char *label_cstr_deprecated(char *buffer)                 const { return buffer;}
   virtual const char *label_string(std::string &buffer) const { return buffer.c_str();} //170423
   inline virtual nat32 write(std::ostream &)          modification_{ return 0;} //980511
   virtual bool process(const CORN::Date &act_date)              { return true;} // Not needed?
};
//_CPF_Irrigation_event________________________________________________________/
class Crop_production_function_simulation
{
   CropSyst::Scenario_directory primary_scenario_directory;                      //150417

   Bidirectional_list            irrig_event_list;
   CropSyst::Scenario            active_sim;                                     //131007_110725
private:
   void collect_automatic_irrigation(const char *ued_filename);
   CORN::OS::Directory_name *create_scenario
      (nat8 percent_of_optimal
      ,const CORN::Dynamic_array<float32> *irrig_adj_fraction
            // Each element in the array represents the amount to adjust
            // irrigation
            // If 0 the percent of optimal value is used
      ,const CORN::Dynamic_array<nat32>   *irrig_adj_dates
            // The corresponding dates as date32 values
      ) rendition_;
   void run_scenario(CORN::OS::Directory_name &scenario_dirname);
public:
   Crop_production_function_simulation
      (const CORN::OS::Directory_name &_primary_scenario_dirname);               //150417
   void run(bool &CPF_abort);
};
//_Crop_production_function_simulation_________________________________________/
#endif