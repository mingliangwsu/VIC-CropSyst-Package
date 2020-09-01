#ifndef explainationsH
#define explainationsH
#include "corn/container/bilist.h"
#include "corn/application/explaination/explaination_location.h"
namespace CORN
{
//______________________________________________________________________________
class Explainations
: public Bidirectional_list // List of Explaination_location
, public extends_    Documentation_paths_fully_qualified
{
/*
 private: // References to documentation locations (not owned, don't delete)
   cognate_ const CORN::OS::Directory_name *program_documentation_directory_fully_qual;
   cognate_ const CORN::OS::Directory_name *application_documentation_directory_fully_qual;
   cognate_ const CORN::OS::Directory_name *suite_documentation_directory_fully_qual;
   cognate_ const CORN::OS::Uniform_resource_identifier *Web_root_URI;
   cognate_ const CORN::OS::Directory_name *local_root_path_fully_qual;
*/
   bool find_unqualified_filenames;
 public:
   Explainations
      (bool  _find_unqualified_filenames = false
         ///< When resolving file names on the local machine
         /// search the documents' subdirectories for unqualified filenames.
         /// Note that this may slow because it searches the file system.
      ,const CORN::OS::Directory_name *_program_doc_dir_fully_qual     = 0
      ,const CORN::OS::Directory_name *_application_doc_dir_fully_qual = 0
      ,const CORN::OS::Directory_name *_suite_doc_dir_fully_qual       = 0
      ,const CORN::OS::Uniform_resource_identifier *_Web_root_URI      = 0
      ,const CORN::OS::Directory_name *_local_root_path_fully_qual     = 0
      );
   inline virtual ~Explainations() {}                                            //151204
   virtual Item *take(Item *new_item)                            appropriation_; //151013
   virtual nat32 write(std::ostream &explaination_stream)          performs_IO_;
      ///< \return number of explainations written
      // (Explainations will be resolved)
   inline bool know_program_documentation_directory
      (const CORN::OS::Directory_name &program_doc_dir_fully_qual_)   cognition_
      { program = &program_doc_dir_fully_qual_; return true;}
   inline bool know_application_documentation_directory
      (const CORN::OS::Directory_name &application_doc_dir_fully_qual_)cognition_
      { application  = &application_doc_dir_fully_qual_; return true;}
   inline bool know_suite_documentation_directory
      (const CORN::OS::Directory_name  &suite_doc_dir_fully_qual_)    cognition_
      { suite        = &suite_doc_dir_fully_qual_; return true; }
   inline bool know_Web_root_URI
      (const CORN::OS::Uniform_resource_identifier &_Web_root_URI
      ,const CORN::OS::Directory_name              *_local_root = 0)  cognition_
      {  Web_root_URI = &_Web_root_URI;
         local_root = _local_root;
         return true; }
      /**<
         Web root
      */
};
//______________________________________________________________________________
extern Explainations &provide_explainations_global();                            //151008
}//_namespace_CORN_____________________________________________________________/
#endif
