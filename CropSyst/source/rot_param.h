#ifndef rot_paramH
#define rot_paramH
//______________________________________________________________________________
#include "cs_operation.h"
#include "corn/parameters/parameters.h"
#include "common/simulation/event.h"
#include "cs_filenames.h"
#include "corn/OS/directory_entry_name_concrete.h"
#include "corn/math/statistical/sample_dataset_T.h"
#if (defined (CROPSYST_PROPER) || defined(OFOOT_VERSION))
#  include "seeding_param.h"
#endif
#include "CropSyst/source/rot_param.h"
//______________________________________________________________________________
#define LABEL_rotation                 "rotation"
#define LABEL_sowing                   "sowing"
#define LABEL_delay_after_precip       "delay_after_precip"
#define LABEL_appreciable_precip       "appreciable_precip"
#define LABEL_avg_temp                 "avg_temp"
#define LABEL_min_PAW                  "min_PAW"
#define LABEL_max_PAW                  "max_PAW"
#define LABEL_expected_days_to_emerge  "expected_days_to_emerge"
#define LABEL_mode                     "mode"
#define LABEL_crop                     "crop"
#define LABEL_management               "management"
//______________________________________________________________________________
// Forward declaration
namespace CORN { class Date; }                                                   //130513

class Weather_spanning_interface;                                                //151130_041104
#ifdef CAMO_VERSION
namespace Liu_Xiao { class Crop_parameters_struct; }                             //180106
#else
#if (CROPSYST_VERSION == 4)
class Crop_parameters;
#endif
#endif
namespace CropSyst {
#ifdef CAMO_VERSION
#define Crop_parameters Liu_Xiao::Crop_parameters_struct
#else
class Crop_parameters;
#endif
class Management ;                                                               //020530
//______________________________________________________________________________
extern const char *planting_date_mode_label_table[];
enum Sowing_date_mode{FIXED_DATE,MIN_TEMP_REQ,DAYS_TO_EMERGE};
DECLARE_ENUM_CLAD(Sowing_date_mode_clad,Sowing_date_mode,planting_date_mode_label_table)
//190113DECLARE_LABELED_ENUM(Sowing_date_mode_clad,Sowing_date_mode,planting_date_mode_label_table)
//_Sowing_date_mode_________________________________________________2002-03-22_/
class Sowing_operation
: public CropSyst_field_operation_VX
, public CORN::Item                                                              //180326
{
 public: // for access by parameter editor
   Sowing_date_mode_clad         sowing_date_mode_clad;                          //020322
   int32                            LADSS_land_use_ID;  // Corresponds to Crop ID for crop rotations                             //020811
   int32                            LADSS_mgt_ID;                                //020811
   CORN::OS::File_name_concrete     crop_filename;                               //000426
   CORN::OS::File_name_concrete     management_filename;                         //000426

   #if (defined (CROPSYST_PROPER) || defined(OFOOT_VERSION))
   Seeding_parameters               seeding_parameters;                          //041201
   #endif
 public:
   float32  sowing_avg_temp;
   float32  sowing_min_PAW;            // PAW has units, I think mm water/m soil depth???  //991116
   float32  sowing_max_PAW;            //991116
   int16    expected_days_to_emerge;   // for a planting mode  //000330
   int16    delay_after_precip;        // Days                 //000330
   float32  appreciable_precip_mm;     // mm                   //000330

   CORN::Parameter_number p_sowing_avg_temp;                                     //180326
   CORN::Parameter_number p_sowing_min_PAW;                                      //180326
   CORN::Parameter_number p_sowing_max_PAW;                                      //180326
   CORN::Parameter_number p_expected_days_to_emerge;                             //180326
   CORN::Parameter_number p_delay_after_precip;                                  //180326
   CORN::Parameter_number p_appreciable_precip_mm;                               //180326
   CORN::date32 plant_by_date; // relative                                       //170331
      // In computed planting mode, if the planting date is postponed
      // make sure to plant by this date.
      // Normally a DOY in the sowing year, but it could be in the next year.
 #ifdef OFOOT_VERSION
 public: // Parameters for OFoot phenology adjustment
   CORN::Date typical_relative[NGS_COUNT];                                       //140625
      // Currently only emergence, max canopy, and maturity date are used.
 #endif
 public: // Not parameters, we just store here the status of the operation
   // Once we load the management and crop files
   // we can store the description for event logging
   std::string crop_description;
 public: // Not parameters, this stores the management at the start of run time
   // The management files are now loaded once
   // events relative to the planting date and actual dated events are now
   // transferred at the time the rotation is loaded, the remainder now
   // remain in memory and are transferred when the planting event occurs
   Management *management;                                                       //020528
 private:
 #ifdef CAMO_VERSION
   Liu_Xiao::Crop_parameters_struct *crop_params;
 #else
  Crop_parameters *crop_params;   // owned by this until relinquished then we just keep the pointer reference //030611
 #endif
   bool             crop_params_relinquished_to_crop;                            //030711
 public:
   bool adjusted_for_weather_conditions;                                         //170220
 public: // constructors/destructors
   Sowing_operation();
   // Constructor for rotation lists
   Sowing_operation(const char *crop_filename,const char *management_file_name); //060103
   Sowing_operation(const Sowing_operation &original);                           //020614
   // Copy constructor for build (don't need to copy status management etc)
   virtual ~Sowing_operation();                                                  //020528
 public: // Common operation overrides
   virtual const char *label_string(std::string &buffer)                  const; //170423
   virtual const wchar_t *label_wstring(std::wstring &buffer)             const; //180326
   inline virtual int32       get_type()      const { return CS_OP_ROT_SOWING; } //011207
   inline virtual const char *get_primary_section_name() const{return "sowing";} //030810
   virtual void setup_parameters                                                 //020621
      (CORN::Data_record &data_rec,bool for_write)                modification_;
      // I now simply dump any failure the reason to the log when performing sowing

   const char         *get_management_description()                       const; //020528
//   #ifdef CAMO_VERSION
//   bool                take_crop_params(Liu_Xiao::Crop_parameters_struct *_crop_params); //180106
//   Liu_Xiao::Crop_parameters_struct    *relinquish_crop_params();                //180106
//   #else
   bool                take_crop_params(Crop_parameters *_crop_params);          //030711
   Crop_parameters    *relinquish_crop_params();                                 //030711
//   #endif
      // Returns pointer to the crop parameters which are loaded
      // when the rotation is loaded.
      // The Crop parameters are usually relinquished to the Crop.
   #ifndef REACCH_PROCEDURES
   bool good_planting_conditions                                                 //020220
      (const CORN::Date_const &today
      , float64         soil_plant_avail_water_1
      , float64         soil_plant_avail_water_2
      , float64         crop_est_degday_days_from_now                            //020220
      , float64         crop_deg_day_emerge                                      //020220
      /*NYI
      //170220 #ifdef OLD_ROTATION
      #if (CROPSYST_VERSION >=5 )
      ,modifiable_ CORN::statistical::Sample_dataset_clad<float32,float32,float32>
         &sowing_precipitation_window_meter                                      //141105
      #endif
      */
      ,const Weather_spanning_interface &weather                                 //151130
      //170220 #endif
      );                                                                         //010202
   #endif
};
//_Sowing_operation____________________________________________________________/
class Sowing_event      // In V5 This should be renamed planting event because it will include planting of trees and transplanting
: public Common_event
, public CORN::Data_record
{
public:
   provided_ Sowing_operation *sowing_op;  // simply references Common_event::operation, don't delete this
   #ifdef PLANTING_SCHEDULER
   nat16 delay;
      // days determined to delay planting for suitable conditions
   #endif
public:
   inline Sowing_event()
      : Common_event()
      , CORN::Data_record("sowing")
      , sowing_op(0)
      #ifdef PLANTING_SCHEDULER
      , delay(0)
      #endif
      {}
   inline Sowing_event(const Sowing_event &copy_from)                            //130130
      : Common_event(copy_from)
      , CORN::Data_record("sowing")
      , sowing_op(0)
      #ifdef PLANTING_SCHEDULER
      , delay(0)
      #endif
      {  operation = sowing_op = new Sowing_operation(*(copy_from.sowing_op));   //160512
         owns_operation = true;                                                  //160512
      }
   inline virtual const char *label_string(std::string &buffer)           const  //180326
      {  append_to_string(buffer);
         buffer += L' ';
         return sowing_op->label_string(buffer); }
   inline virtual const wchar_t *label_wstring(std::wstring &buffer)      const  //180326
      {  append_to_wstring(buffer);
         buffer += L' ';
         return sowing_op->label_wstring(buffer);
      }
   virtual bool setup_structure(CORN::Data_record &data_rec,bool for_write) modification_;
   virtual bool expect_structure(bool for_write);                                //161025
   Sowing_operation &provide_sowing_operation()                  /*provision_*/;
   virtual Common_operation *set_operation                                       //041203
      (Common_operation *_operation,bool _owns_operation)          assignation_;
   // Will create if necessary
   // These are only used by the parameter editor, and I might redo this
};
//_Sowing_event_____________________________________________________2003-10-13_/
class Rotation_parameters
: public Common_parameters_data_record
{
 public:
   Common_event_list planting_events; //180326 sowing_events; // or sowing events?
   int16             end_year;      // This is used only by the rotation builder
   int16             years;         // This now used by both the rotation builder and rotation shifting.
 public:
   Rotation_parameters();
   virtual bool expect_structure(bool for_write = false);                        //161025
   inline virtual const char *get_primary_section_name()                  const  //020911
      { return LABEL_rotation; }
   inline virtual const char *get_type_label()                            const  //051011
      { return "CropSyst rotation"; }
   nat16 count_computed_plantings()                                       const; //170310
};
//_Rotation_parameters_________________________________________________________/
}//_namespace CropSyst_________________________________________________________/
#endif
// rot_param.h

