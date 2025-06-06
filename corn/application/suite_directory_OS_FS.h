#ifndef suite_directory_OS_FSH
#define suite_directory_OS_FSH
#include "corn/application/documentation_directory_OS_FS.h"
namespace CORN {
namespace OS_FS {
   /// Suite: a suite of applications where the seperate applications
   /// share some common directories for example DLLs or common help.
//______________________________________________________________________________
class Suite_directory
: public OS::File_system_logical::Directory
{
 protected:
   provided_ Documentation_directory    *documentation_directory;
      // Documentation common to all applications in the suite
   std::wstring prefered_documentation_dir_name;
   std::wstring prefered_manual_dir_name;
   std::wstring prefered_icons_dir_name;

   provided_ CORN::OS::Directory_name *common_directory;                         //101114
   provided_ CORN::OS::Directory_name *manual_directory;
   provided_ CORN::OS::Directory_name *icons_directory;
   provided_ CORN::OS::File_name_concrete *install_or_update_filename;           //120112
   CORN::OS::File_name_concrete        *current_program;
 public:
   Suite_directory
      (const OS::Directory_name &directory_name
      ,const wchar_t *prefered_documentation_dir_name = L"Documentation"
      ,const wchar_t *prefered_manual_dir_name        = L"Manual"
      ,const wchar_t *prefered_icons_dir_name         = L"Icons");
   // Use this constructor when we have a fully qualified filename
   // (typically a program in any subdirectory of the suite);
   void know_initiating_program(const char *initiating_program);
   inline virtual bool is_valid()                        const { return true; };
      ///< Derived classes must reimplement is_valid()
      /// This method is often used to determine if a directory is
      /// a suite directory.
      /// Various methods may be employed to signal a suite directory
      /// I.e. the directory may have 'suite' as part of the directory name
      /// or there may be a 'magic' file in the directory
      /// or one or more files or directories with a recognizable name.
      /// \return true if the
 public: // Smart_directory_name reimplementations
      ///< Derived classes should call this.
   virtual void reset()                                              provision_;
      // provision because only modifies provided contributes;
 public: // Fluent interface
   Documentation_directory        &documentation()                   provision_;
   const CORN::OS::Directory_name &manual()                          provision_;
   const CORN::OS::Directory_name &icons()                           provision_;
   const CORN::OS::Directory_name &common()                          provision_;
   const CORN::OS::File_name      &install_or_update()               provision_; //120112
      ///< Provides the same of a program that is used to install or update the application suite programs;
};
//______________________________________________________________________________
}}//_namespace_CORN::OS_FS_____________________________________________________/
#endif

