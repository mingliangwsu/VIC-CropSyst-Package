#ifndef CS_simulation_rotatorH
#define CS_simulation_rotatorH
/* This class is used by CropSyst (for OFoot and REACCH)
This repeatedly runs the same rotation for a land unit.
In each run, the sequence of crops (plantings) in the rotation is
shifted by one year.
The number of runs corresponds to the number of years in the rotation.

In the cycler there is a common weather and soil parameters
shared by all cropping system simulations.
The simulations advance through time simultaneously
There may be some events that are common to all cropping system simulations
*/
#  include "CS_suite/simulation/CS_simulation_engine.h"
#  include "CS_suite/simulation/CS_land_unit_meteorological.h"
#if (CS_VERSION==4)
#  include "common/simulation/log.h"
#endif
#include "corn/OS/directory_entry_name.h"
#include "rot_param.h"
#include "land_unit_with_output.h"
#define LAND_UNIT_SIMULATION_SUPERCLASS Land_unit_with_output
#include "common/weather/database/CS_weather_daily_UED.h"
#include "common/geodesy/geolocation_item.h"
//______________________________________________________________________________
namespace CropSyst
{  class Crop_interfaced;
}
//______________________________________________________________________________
namespace CS
{
//______________________________________________________________________________
class Cycler
: public extends_ Simulation_engine
{
 protected:
   const CS::Identification &ID;                                                 //151026
   CropSyst::Scenario template_scenario;                                         //160514
   //200116   Geolocation    geolocation;                                        //160513
   geodesy::Geocoordinate_item geolocation;                                      //200117
   provided_ CS::Weather_database_daily_UED *weather_DB_daily;                   //160708
 public:
   //___________________________________________________________________________
   class Cycled_scenario_simulation // was Rotated_simulation_scenario
   : public extends_ CropSyst:: LAND_UNIT_SIMULATION_SUPERCLASS
   {protected:
      const CORN::OS::Directory_name &rotator_output_directory;                  //160311
      provided_ CORN::OS::Directory_entry_name_concrete *provided_cycle_directory; //160311_160219
         // In REACCH now there is output directory for each cycle.              //160219
      #if (CS_VERSION==4)
      Common_simulation_log       event_log_currently_unused;
      #endif
         // Null when not in cropping season
         // will probably need a list of these for intercropping
    #ifdef _DEBUG
    CORN::OS::File_name_concrete debug_format_filename;                          //141220
    #endif
    public:
      Cycled_scenario_simulation                                                 //131020
         (const CS::Identification  &LBF_ID_                                     //151026
         ,nat32                      cycle_shift_                                //151026
         ,CropSyst::Scenario        &scenario_control_and_model_options
         ,const CORN::date32        &today_                                      //170525
         ,const Geocoordinate       &geocoordinate                               //151116
         ,const Land_unit_meteorological     &meteorology
         ,const CropSyst::Scenario_directory *scenario_directory
         ,bool                       want_detail_output                          //151227
         ,const CORN::OS::Directory_name &rotator_output_directory_              //160311
         );                      //170225
            // scenario directory is not const because it is being updated
      virtual Common_operation            *get_operation
         (const char                      *operation_type_str
         ,const std::string               &operation_ID  // filename may be updated with qualification so not const
         ,modifiable_ CORN::OS::File_name &operation_filename    // the fully qualified operation filename may be returned (to be output to the schedule)
         ,int32                            resource_set_ID
         ,bool                            &relinquish_op_to_event)
         { return 0; }
         // Not applicable to REACCH because this was actually only used for CropSyst LADSS
      virtual bool end_day()                                       modification_;
      virtual bool setup_cropping_system();                                      //130830
      virtual const CORN::OS::Directory_name &get_output_directory_name()  const;
      const CORN::OS::Directory_name &provide_cycle_directory()      provision_; //160310
   };
   //_class Cycled_scenario_simulation_________________________________________/
 public:
   Cycler
      (CS::Arguments &arguments_                                                //200815
      //200815       ( Command_options_data_source    &command_line_options                      //160829
      ,const CS::Identification       &ID_     // ID of the land unit            //151026
      ,bool                            CPF_mode);                                //160513
   virtual bool initialize()                                     initialization_;//150427
   virtual bool stop()                                             modification_;
   virtual const CORN::OS::File_name &provide_template_rotation_file_name() provision_ = 0;
   virtual modifiable_ Cycled_scenario_simulation *render_scenario_simulation(nat32 shifted_sow_ID) /*rendition_*/; //160514
   virtual bool start_day()                                        modification_;
   virtual bool end_day()                                          modification_;
   virtual bool end_year()                                         modification_;
   inline virtual bool is_rotation_cycling_enabled()               affirmation_  { return true; }
   inline virtual const Simulation_control &ref_simulation_control()       const { return template_scenario; } //160222
   inline virtual const Simulation_control &ref_simulation_scenario()      const { return template_scenario; } //160222
 public:
   virtual bool is_detailed_output_cell(const CS::Identification &ID_) affirmation_ = 0;
 protected:
   cognate_ const Land_unit_meteorological *meteorological_ref;                  //151023
   CORN::Unidirectional_list &land_unit_simulations;
      // list of Rotated_scenario_simulation probably should rename this
      // reference to simulation_units
   inline virtual bool know_sowing_for_phenology_adjustment
      (const CropSyst::Sowing_operation   &gen_sow_op
      ,const CORN::date32                   &planting_date)             cognition_
      { return true; }
      // This method is overridden by OFoot Simulation_engine
   inline bool know_meteorological
      (const Land_unit_meteorological *meteorological_)               cognition_
      {  meteorological_ref = meteorological_;
         return true;
      }
   virtual const CropSyst::Scenario_directory *provide_scenario_directory() provision_ =0;     //160530
   virtual CS::Weather_database_daily &provide_weather_database_daily() provision_;
   virtual CS::Weather_database_daily &provide_weather_database_daily_with_meteorology() provision_; //170311
};
//_class Cycler_____________________________________________________2013-06-17_/
}//_namespace_CS_______________________________________________________________/
#endif

