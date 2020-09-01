#ifndef cs_report_V4H
#define cs_report_V4H

#if (CROPSYST_VERSION >= 6)
#  error The report writer class is obsolete in version 6
#endif
#include "cs_accum.h"
// cs_accum must be first
#include "rptoptns.h"
#include "rprtclss.h"
#include "soil/chemical_balance.h"

enum detail_types {detail_INTEGER,detail_REAL,detail_STRING};

//   { This is an abstract class }
//______________________________________________________________________________
class CropSyst_report
: public Report_writer
{
public:
  CropSyst::Land_unit_simulation_VX &simulation;                                 //010202
  report_types report_type;
public:
  CropSyst_report
      (CropSyst::Land_unit_simulation_VX& simulation                             //010202
      ,const char*               report_filename_                                //030110
      ,const Report_options&     options_to_copy                                 //981203
      ,report_types              report_type_
      ,nat16                     max_detail_lines);                              //981203
  ~CropSyst_report();
  void write_headers();
  void write_detail_line();
  virtual void get_detail
     (nat16          dc
     ,std::string&   str_value
     ,int16&         int_value
     ,float64&       num_value
     ,detail_types&  detail_type
     ,int8&          width
     ,int8&          precision)                                              =0; // pure
};
//_class CropSyst_report_______________________________________________________/
#ifdef NYI
class CropSyst_chem_balance_report
: public Report_writer
{  // This class is a base class for pesticide and N balance reports
 public:
   CropSyst_N_balance_report                                                     //990318
     (Land_block_fragment_simulation &_simulation);                              //010202
};
//_class CropSyst_chem_balance_report_______________________________1999-03-18_/
#endif

class CropSyst_N_balance_report
: public Report_writer
{
   Chemical_balance_accumulators_molecular *NO3_accums_M;
   Chemical_balance_accumulators_molecular *NH4_accums_M;
   CropSyst::Period_accumulators *accums;
 public: // The input and output are special for Luca's VATE model.
   float32 VATE_soil_N_input;                                                    //041001
   float32 VATE_soil_N_output;                                                   //041001
 public:
   CropSyst_N_balance_report
       (const char             *report_filename                                  //030110
       ,const Report_options   &options_to_copy                                  //981203
       ,nat16                   max_detail_lines);                               //981203
   void write_headers(bool with_crop_description);                               //160326
   void write_detail_line
      (const CORN::Date_const &today
      ,const char *crop_description);                                            //160326
      //            In the case of Annual, GP and Fallow reports,
      //            when the accumular (report) object goes out of focus
      //            the reporter needs to be updated
  void set_reporters
     (Chemical_balance_accumulators_molecular *NO3_accums                                  //041001
     ,Chemical_balance_accumulators_molecular *NH4_accums                                  //041001
     ,CropSyst::Period_accumulators *accums);
};
//_class CropSyst_N_balance_report__________________________________1999-03-18_/
class CropSyst_DLY_report : public CropSyst_report
{public:
   CropSyst_DLY_report
      (CropSyst::Land_unit_simulation_VX &i_simulation
      ,const Report_options &options_to_copy
      ,const char *DLY_report_filename // const File_name &DLY_report_filename
      ,nat16 max_detail_lines);
   virtual void get_detail
      (nat16    dc
      ,std::string &str_value
      ,int16    &int_value
      ,float64  &num_value
      ,detail_types &detail_type
      ,int8    &width
      ,int8    &precision);
};
//_class CropSyst_DLY_report___________________________________________________/
//200122 #ifndef SOILR
 class CropSyst_YLY_report : public CropSyst_report
 {
   CropSyst::Annual_accumulators *accumulators;
 public:
   CropSyst_YLY_report
      (CropSyst::Land_unit_simulation_VX &i_simulation
      ,const Report_options &options_to_copy
      ,nat16 max_detail_lines);
   virtual void get_detail
      (nat16    dc
      ,std::string &str_value
      ,int16    &int_value
      ,float64  &num_value
      ,detail_types &detail_type
      ,int8    &width
      ,int8    &precision);
};
//_ class CropSyst_YLY_report__________________________________________________/
class CropSyst_GS_report : public CropSyst_report    // Was CropSyst_HRV_report
{CropSyst::Period_accumulators *accumulators;
 public:
   CropSyst_GS_report
     (CropSyst::Land_unit_simulation_VX &i_simulation
      ,const Report_options &options_to_copy
      ,nat16 max_detail_lines);
   virtual void get_detail
      (nat16    dc
      ,std::string &str_value
      ,int16    &int_value
      ,float64  &num_value
      ,detail_types &detail_type
      ,int8    &width
      ,int8    &precision);
};
//_class CropSyst_GS_report_________________________________________2003-05-16_/
//200122 #endif
//   {At this time the fallow report will use the same file format as the harvest report }
#endif
//cs_rprt.h

