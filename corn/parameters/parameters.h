#ifndef parametersH
#define parametersH

#if ((__BCPLUSPLUS__  >0) && (__BCPLUSPLUS__ <= 0x0550))
#include "corn/string/ustring.h"
#endif

#include <string>
#include "corn/primitive.h"
#include "corn/const.h"
#include "corn/parameters/parameters_types.h"
#include "corn/chronometry/date_32.h"

// This class provides a Data_record class that automatically records the current version of a parameter file
#define LABEL_description           "description"
#define MAX_DESCRIPTION_LENGTH 255
//______________________________________________________________________________
namespace CORN
{
   class Data_record;
}
//______________________________________________________________________________
#define CHANGE_V1   1,0,0
#define CHANGE_V2   2,0,0
#define CHANGE_V3   3,0,0
#define CHANGE_V4   4,0,0
//______________________________________________________________________________
namespace CORN
{  namespace OS
   {  class Directory_entry_name;
   }
}
//______________________________________________________________________________
// This should be in namespace CORN 180626
class Common_parameters
{protected:
   Associated_directory_mode         associated_directory_mode;                  //060620
 public:
   struct Description                                                            //150111
   {
      std::string brief/* description*/ ;
      /*190625 obsolete
      simply have a URL in the parameter files directory
      std::string details_URL;                                                   //080502
      */
   } description;
   nat32  version_code;                                                          //180626
      // Typically this would be simply the major version
      // If more detailed versioning is needed:
      // byte 0 could be the major version
      // byte 1 could be the minor version
      // byte 2 and 3 could be the release
   // Not sure needed nat32 program_date;
   CORN::Date_clad_32 version_program_date;                                      //180626
   CORN::Date_clad_32 version_program_date_curr;                                 //180626
   std::wstring  data_source_URL;                                                //140801
      // This is the source of the parameters.
      // In CropSyst this used by the crop phenology adjuster,
      // to uniquely identify crop parameters by the fully
      // qualified parameter file name.
      // Currently this is only set as needed/used.

 public:
   Common_parameters
      (Associated_directory_mode associated_directory_mode_                      //060620
      ,nat32                     version_code_ =0);                              //180626
   inline virtual const char *get_description()                            const
                                             {return description.brief.c_str();}
      // This is the user provided description
   inline virtual Associated_directory_mode get_associated_directory_mode()const
                                             {return associated_directory_mode;} //060627
   bool is_associated_directory_required()                                const;//051003
      // If you override this method to return true, then you must also
      // override provide_associated_directory;
 public:
   virtual const char *get_type_label()                       const {return "";} //140223_051011
      // Derived classes should override this method to return a literal description
      // of what the parameters represent (I.e. file type).
      // This is used by the paremeter file form to compose
      // file filter description.
   inline virtual const char *get_designation_notes()         const {return "";} //050322
      // The designation file can have some notes specific to the type of
      // parameter file.  Override get_designation_notes() to specify additional notes
   virtual nat16 resolve_relative_and_shortcut_filenames
      (const CORN::OS::Directory_entry_name &)                 const {return 0;}
};
//_2005_________________________________________________________________________
#endif
// corn/parameters.h

