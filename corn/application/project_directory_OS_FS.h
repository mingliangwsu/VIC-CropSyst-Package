#ifndef project_directory_OS_FSH
#define project_directory_OS_FSH
#include "corn/parameters/parameters_directory_OS_FS.h"
#include "corn/OS/directory_entry_name_concrete.h"
namespace CORN {                                                                 //081130
namespace OS_FS {
//______________________________________________________________________________
class  Project_directory
: public extends_ Parameters_directory                                           //120202
{
 protected:
   OS::Directory_name_concrete *projects_directory;                              //120320
      // The projects directory is usually the parent directory of the project
 public:
   Project_directory
      (const CORN::OS::Directory_entry_name &project_dir_name
      ,bool create_now_if_needed);                                               //130116
 public:
   virtual const CORN::OS::File_system::Directory &projects()        provision_;
      // May eventually have a specialized Projects_directory class.0
      // The projects directory is usually the parent of the project directory.
   bool  set_projects_as_current_working_directory();                            //050531
   virtual bool is_valid()                                         affirmation_; //081130
      // Derived classes will typically override this class
      // to determine if the directory contains the full compliment of required directory entries
   inline virtual const wchar_t *get_associated_file_extension() const { return L"project";} //120325
};
//______________________________________________________________________________
class Known_projects
: public Bidirectional_list
{
   //This class stores a list of known projects
   //(projects that may potentially be used or otherwise referenced by the current element context)
   //The element properties editor will keep a this list so it can quickly
   //look up element give an element name qualified
   //
   //Note that this difference from known_packages because known packages
   //may keep full element definitions in memory (typically classes and their substructure).
   //
   //known projects only keeps Element_directories in memory.
 public:
   Known_projects();
   Project_directory *recognize_context(const OS::Directory_entry_name &directory_element_name) modification_;
      // Give any arbitrary directory name traces up the  path
      // until it find a parent directory that is a  project
      // and adds that project to the projects list (if not already listed).
      // Returns true if the project has been added to this projects list
      // (Either now or if it was already listed)
      // Returns 0 if the directory_element_name is not actually under a project context
      // (I.e. if the path traced up to a root directory)

      // This method is typically called at the start of UPSILON element properties editor
      // passing the UPSILON_home_directory (or the fully qualified UPSILON executable filename)
      // For example UPSILON_home_directory  ("C:\UPSILON\program\properties_editor.exe")
      // will add UPSILON_project_directory for "C:\UPSILON" to projects.

      // This method is also typically called by the UPSILON element properties editor
      // for the properties element file being edited, since this
      // will typically be an element within a project and typically
      // that project will be frequently referenced by the element.

      // This method is also called by restore()for each of the 'remembered' projects
      // stored between sessions.
   nat16 save   (const OS::Directory_name &users_APPLICATION_working_directory) modification_;
   nat16 restore(const OS::Directory_name &users_APPLICATION_working_directory) modification_;
      // opens the recognized projects save and restore file in the user's working directory
      // and loads the known packages
      // eventually users_APPLICATION_working_directory may be member optional reference
 protected:
   virtual Project_directory *create_project_directory
      (const OS::Directory_name   &directory_to_create)                 const=0;
      // given the fully qualified directory name
      // create a new project directory
      // Derived classes will typically override this to
      // instanciate a specialized project directory
};
//______________________________________________________________________________
}}//_namespace_CORN::OS_FS_____________________________________________________/
#endif

