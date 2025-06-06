#ifndef cs_operationH
#define cs_operationH

#include "options.h"
#include "common/simulation/operation.h"
#include "common/soil/layering.h"
#ifndef dynamic_array_TH
#  include "corn/dynamic_array/dynamic_array_T.h"
#  define Dynamic_float_array Dynamic_array<float32>
#endif
#ifndef cs_operation_codesH
#  include "cs_operation_codes.h"
#endif

#define UNKNOWN_NRCS_SDR_operation_number 0
#define LABEL_terminate_crop        "terminate_crop"

namespace CORN {class Data_record;}
namespace CropSyst {
class Management_parameters; // forward for friendship
}
namespace CropSyst {

//______________________________________________________________________________
class Operation
: public Common_operation
{
   friend class Management_parameters;                                           //040614
 public:
   int16                op_ID; // at this time this is used only for LADSS       //031124
   CropSyst_Op_clad  op_clad;                                              //030824
   float32 carbon_footprint_kgCO2e_ha;                                           //120419
 public: // constructors
   Operation(CropSyst_Op op_code);
   Operation(const Operation &copy_from);                                        //140828
 public:
   virtual void setup_parameters(CORN::Data_record &data_rec,bool for_write) modification_;   //970709
   virtual const char *label_string(std::string &buffer)                  const; //170423
   virtual void log(std::ostream &log_file)                               const; //020810
   virtual void log_ID_and_type(std::ostream &log_file)                   const; //050110
   inline virtual const char *get_simulation()                            const { return "CropSyst"; } //021108
   inline virtual const char *get_primary_section_name()                  const { return "CropSyst"; }  // Currently a catch all, eventually will be overridden as necessary 030810
   inline virtual const char *append_type_label(std::string &return_buffer)const { return op_clad.append_label(return_buffer); } //190109
   void set_description(const char *description)                  modification_; //040614
     // set description is currently used only to update descriptions when importing V3 operations //040614
   inline virtual bool parameters_to_be_stored_in_event_file()             const { return true;} //030808
};
//_Operation________________________________________________________2010-12-07_/
extern const char *recal_var_code_label_table[];
//_recal_var_code_label_table_______________________________________2002-08-12_/
class Recalibration_operation
: public Operation
{public:
   enum Recal_var_code {WC_RECAL,NO3_RECAL,NH4_RECAL,POM_RECAL,SNOW_RECAL,PAW_RECAL,RBM_RECAL,MBM_RECAL,CO2_RECAL}; //010723
   DECLARE_ENUM_CLAD (Recal_var_code_clad,Recal_var_code,recal_var_code_label_table)   //020812
 public:
   Recal_var_code_clad var_code_clad;                                            //020812
   CORN::Dynamic_array<float32> values_by_horizon;                               //011117
      // In the case of scalar  values (I.e. SNOW) the scalar value is stored at index 1 (Not 0 because the array reader assumes soil property which is indexed from layer 1 not 0.) 080226_
      // Recalibration values are by horizon not (sub)layers!!!!
      // The units of the values depend on the var_code
 public: // constructors
   Recalibration_operation(std::ifstream &in_file);
   // This constructor is used when reading events from a recalibration file.
   Recalibration_operation
      (Recal_var_code var_code
      ,soil_horizon_array32(values_by_horizon));                                 //011116
   // This constructor is used for adding recalibration
   // to reinitialize the soil profile.
 public: // Common operation overrides

   virtual const char *label_string(std::string &buffer)                  const; //170423
   inline virtual int32 get_type()                                        const  { return CS_OP_RECALIBRATION; } //011207
   virtual void log(std::ostream &log_file)                               const; //020810
   inline float32 get_value()                                             const  { return values_by_horizon.get(1); } // In this case the scalar value is simply stored in element 1   060605_
};
//_Recalibration_operation__________________________________________2002-03-09_/
class Field_operation
: public Operation
{  friend class Management_parameters;                                           //040614
public:
   bool    terminate_crop;                                                       //110115 moved from tillage
   std::string NRCS_operation_description;                                       //060802
      // May be 0 if no soil disturbance or not a field operation
public:
   Field_operation
      (CropSyst_Op op_code = CS_OP_NONE                                          //150428
      , int16 _NRCS_SDR_operation_number = UNKNOWN_NRCS_SDR_operation_number
      ,int16 reservered = 0);// This parameters has no function in this version of model
   Field_operation(const Field_operation &copy_from);                            //140828
   virtual void setup_parameters
      (CORN::Data_record &data_rec,bool for_write)                modification_;
   virtual void log(std::ostream &log_file)                               const; //020810
   virtual const char *label_string(std::string &buffer)                  const; //170423
};
//_Field_operation_____________________________________________________________/
#define CropSyst_field_operation_VX  CropSyst::Field_operation
//______________________________________________________________________________
}//_namespace CropSyst_________________________________________________________/
#endif
// cs_operation.h

