#include "common/simulation/operation.h"
#include "corn/data_source/datarec.h"
#include "options.h"
using namespace CORN;
//______________________________________________________________________________
Common_operation::Common_operation
(Associated_directory_mode associated_directory_mode_                            //060620
,const char *              primary_section_)
: Common_parameters_data_record
   (associated_directory_mode_,primary_section_,CS_VERSION)                      //180626
      // If I do this then need to have is_associated_directory_required() return true in that case (if not cropsyst etc).
,start_hour(0)                                                                   //081212
,duration_hours(6)                                                               //071029
{}
//______________________________________________________________________________
Common_operation::Common_operation()
: Common_parameters_data_record(NO_ASSOCIATED_DIRECTORY,"",CS_VERSION)           //060620
   // Need to check if NO_ASSOCIATED_DIRECTORY is a sensible default //030808
,start_hour    (0)
,duration_hours(6)
{}
//_Common_operation:constructor________________________________________________/
Common_operation::Common_operation(const Common_operation &copy_from)
: Common_parameters_data_record(NO_ASSOCIATED_DIRECTORY,"",CS_VERSION)           //060620
   // Need to check if NO_ASSOCIATED_DIRECTORY is a sensible default //030808
,start_hour    (copy_from.start_hour)
,  duration_hours(copy_from.duration_hours)
{}
//_Common_operation:constructor_____________________________________2014-08-28_/
void Common_operation::setup_parameters(Data_record &data_rec,bool  for_write) modification_
{  // This mode is provided for CropSyst type operation lists where all operations are in one file
   Common_parameters_data_record::setup_parameters(data_rec,for_write);          //040218
   data_rec.expect_string(LABEL_name,name,255);   // 041020 Note I am not sure if I want to store the name, I believe I want the name to match the name of the file, the name is provided as a convienience (although I am checking if it this conveninece is necessary)
   data_rec.expect_int16("start_hour"  ,start_hour);                             //170619
   data_rec.expect_int16("duration"    ,duration_hours);                         //170619
   //NYI     data_rec.expect_valid_int16("duration_minutes",v_duration_minutes); //071029
}
//_setup_parameters_________________________________________________1998-07-09_/
bool Common_operation::expect_structure(bool for_write)            modification_
{  bool expected = Common_parameters_data_record::expect_structure(for_write);   //161023_030808
   structure_defined = false;                                                    //120314
   setup_parameters(*this,for_write);
   structure_defined = true;                                                     //120314
   return expected;
}
//_expect_structure_________________________________________________1997-07-09_/
void Common_operation::log(std::ostream &log_file)                         const
{  //030611 currently logged using event datarec
   log_ID_and_type(log_file);                                                    //050110
}
//_Common_operation_________________________________________________2002-08-10_/
void Common_operation::log_ID_and_type(std::ostream &log_file)             const
{ }
//_log_ID_and_type__________________________________________________2005-01-10_/
const char* Common_operation::label_string(std::string &buffer)            const
{  buffer.append(name);
   return buffer.c_str();
}
//_label_string_____________________________________________________2017-04-24_/
void Common_operation::copy(const Common_operation &from)
{  // this copy could be moved to simulation parameters?
   description.brief.assign(from.description.brief);
}
//_copy_____________________________________________________________2002-03-15_/

