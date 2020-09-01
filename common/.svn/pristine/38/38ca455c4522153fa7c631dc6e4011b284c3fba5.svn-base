// #error check where used, may be obsolete  try using inspectors

#ifndef recorderH
#define recorderH
#include "UED/library/UED.h"
#include "UED/library/datasetrec_creation.h"
#include "corn/data_source/vv_entry.h"
#include "corn/measure/unitconv.h"

// These classes are used by simulation models to record daily results to
// Data sources

// The Recorder is not UED database specific,
// but we will use UED data structures, variable and units codes to
// store recorded data in memory.

namespace CORN {
class Recorder_data_record; // forward dec
//______________________________________________________________________________
class Recorder_base
: public UED::Data_record_creation_layout                                        //010213
, public Units_convertor
{
   CORN::Units_clad value_units;
   // The units the value has may be different than the
   // units desired to store in the database.
 public:
   Recorder_base
      (UED::Variable_code  variable_code_                                          //010213
      ,UED_units_code      database_units_code_
      ,UED_units_code      value_units_
      ,Record_code         record_type_code_);
   inline UED_units_code get_value_units_code() const{return value_units.get();}
};
//_Recorder_base____________________________________________________2001-04-21_/
class Recorder_float64
: public Recorder_base                                                           //010213
, public VV_float64_entry
{public:
   Recorder_float64
      (float64                  &value_to_record_                               //010213
      ,const char               *descriptive_var_
      ,UED::Variable_code        variable_code_
      ,UED_units_code            database_units_code_
      ,UED_units_code            value_units_
      ,Record_code               record_type_code_);
};
//_Recorder_float64____________________________________________________________/
class Recorder_float32
: public Recorder_base                                                           //010213
, public VV_float32_entry
{public:
   Recorder_float32
      (const float32          &value_to_record_                                  //010213
      ,const char             *descriptive_var_
      ,UED::Variable_code      variable_code_
      ,UED_units_code          database_units_code_
      ,UED_units_code          value_units_
      ,Record_code             record_type_code_);
};
//_Recorder_float32____________________________________________________________/
class  Recording_object
{public:
   Recording_object();
   virtual const char *get_record_section_label()                       const=0; //010215
     // Derived classes must override and return a label for the section the data is stored in.
   virtual void add_desired_variables(Recorder_data_record &to_data_record) = 0;
     // The recording object will scan the list of desired variables
     // If a variable is maintained by the object, it will create a recorder for it
     // and add it to the database.

     // A database may request this action at any time
     // (Usually at the beginning of the year)
  virtual uint32 compose_variable_code                                           //010215
     (CORN::Quality_code  quality
     , nat8               class_group // 4 bits
     , nat32              class_code  // 0 to 20 bits
     , nat32              variable_num); // 0 to 20 bits
};
//_Recording_object____________________________________________________________/
}//_namespace CORN_____________________________________________________________/

#endif
// recorder.h

