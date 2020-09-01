#ifndef file_systemH
#define file_systemH
#include "corn/const.h"
#include "corn/OS/directory_entry_name.h"
#include "corn/chronometry/date_time_64.h"

namespace CORN {
namespace OS {
class Directory_entry_name;
#define FILE_SYSTEM_RECURSION_SUBDIRECTORY   CORN::OS::File_system::subdirectory_recursion_inclusive
#define FILE_SYSTEM_RECURSION_SUPERDIRECTORY CORN::OS::File_system::superdirectory_recursion_inclusive
//#define FILE_SYSTEM_RECURSION_INCLUSIVE
#define FILE_SYSTEM_RECURSION_EXCLUSIVE      CORN::OS::File_system::none_recursion_exclusive
//______________________________________________________________________________
interface_ File_system
{public:
   enum Recursion
      { none_recursion_inclusive             = 0
      , subdirectory_recursion_inclusive     = 1
      , superdirectory_recursion_inclusive   = 2
      , both_recursion_inclusive             = 3
      , none_recursion_exclusive             = 4
      , subdirectory_recursion_exclusive     = 5
      , superdirectory_recursion_exclusive   = 6
      , both_recursion_exclusive             = 7
      };
      // Note these values are bitmasks.
      // In Linux superdirectory_recursion stops in home (inclusive).
      // (could have bits for home inclusive, home exclusive, and follow up to root).
      // When resursing superdirectory recursion only goes up in superdirectories.
      // Exclusive means the start directory file entries are not listed
      // and only once into the recursion are the entries includes.
   //_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
   class File_type_description : public Item
   {public:
      virtual const std::string &get_extension()                        const=0;
         // Without the dot
      virtual const std::string &get_filter_label()                     const=0;
      virtual const std::string &compose_file_type_pattern_for_Windows
         (std::string &pattern)                                         const=0;
   };
   //_File_type_description____________________________________________________
   class Directory_entry     // interface
   : public extends_interface_ Item  // so it can be listed.
   {public:
      typedef Directory_entry_type Type;
      //________________________________________________________________________
   public:
      virtual bool exists()                                             const=0;
      //conceptual
      virtual const Directory_entry_name &ref_designation()             const=0;
    public:
      virtual const Directory_entry_name &set_wstr
         (const wchar_t *to_copy_from)                          modification_=0;
      virtual const Directory_entry_name &set_cstr
         (const char *to_copy_from)                             modification_=0;
   };
   //_Directory_entry____________________________________________________/
   class Directory   // Interface
   : public extends_interface_ Directory_entry
   #ifdef DIRECTORY_ENTRY_IS_A_NAME
   , public extends_interface_ CORN::OS::Directory_entry_name // unqualified
   #endif
   {
   public:  // Note the following functions are actually functions of the operating system's file system, not functions of directory name.
      virtual bool create()                                             const=0;
      inline virtual bool is_directory()            affirmation_{ return  true;} // 130518
      inline virtual bool is_suite()                affirmation_{ return false;} //091109
   public:
         /** In the following methods, the rendered file or directory name
             (if found) is reliquished to to the caller.
            \returns null if no such directory entry exists.
         **/


//191103   These find_XXX methods should be replaced with a single
// method find_DEN where the DEN type is specifed, and use seclusion


      virtual CORN::OS::File_name *find_file_name
         (const std::wstring    &target_filename                                 //150908
         ,File_system::Recursion recursion
         ,nat8 recursions=255)                                     rendition_=0; //200426
         ///< The target_filename may be a wildcard pattern.
         ///< There could be multiple files that match the target filename,
         ///< we simply return the first in the list.
      virtual CORN::OS::Directory_name *find_directory_name                      //100107
         (const std::wstring  &target_directory_name
         ,File_system::Recursion recursion                                       //150908
         ,nat8 recursions                                                        //200426
         ,Directory_entry_name::Qualification qualification                      //150908
         )                 rendition_ = 0;
         ///< The rendered directory name (if found) is reliquished to to the caller.
         ///< \return null if no such directory

      /* In the following find_xxx_in_path methods
         The search extends up the to the parent directory.
         If recurse is true the search extends up the parent's parent directory.
         If inclusive is true, the this directory is searched
         (??but not subdirectories??)
      */
      /*200426 obsolete
      virtual CORN::OS::File_name *find_file_name_in_path                        //110202
         (const std::wstring   &target_file_name_unqualified
         ,bool recurse_up_the_path,bool inclusive)                 rendition_=0;
      virtual CORN::OS::Directory_name *find__directory_name_in_path              //110202
         (const std::wstring   &target_dir_name_unqualified
         ,bool recurse_up_the_path,bool inclusive)                 rendition_=0;
      */
      virtual void reset()                                         provision_=0;
         ///< Clears provided directory and file names.
      #ifdef NYI
      virtual const Directory_entry *provide_resolved_directory_entry
         (const Directory_entry_name &relative_DEN)                provision_=0;
         /**<
         \return
         **/
      #endif

    public:
      #ifdef NYI
      Directory *find_directory(const std::string &unqualified_dir_name,bool recursive) const = 0;   //081211
         ///< NYI unqualified_file_name_to_find may contain wild card characters.
         ///< creates and returns the fully qualified filename if it is found.
         ///< The ownership of the filename is reliquished to the caller.
      bool create()                                          performs_IO_const_;
         ///< Create the directory.
         ///< (although technically could be a const, it does I/O, so don't use const.)
         ///< All parent directories in the path will also be created if they don't already exist.
         ///< Returns true if the directory was newly created or already exists.
         ///< false if it could not be created
      bool set_as_current_working_directory()                performs_IO_const_;
         ///< Returns true if successful.
         ///< (although technically could be a const, it does I/O, so don't use const.)
    #endif
   };
   //_Directory__________________________________________________________/
   class File
   : public extends_interface_ Directory_entry
   {
    public: // 'structors
    public:
    private:
   };
   //_File_______________________________________________________________/
 public:
   virtual const File_system::Directory *provide_directory                       //120207
      (const CORN::OS::Directory_name &directory_name)             provision_=0;
};
//_File_system_________________________________________________________________/
File_system &file_system();
// The file_system() function is not implemented in the library.
// This method must be implemented by the calling program or derived classes.
// The calling program must include a library that implements a File_system
// (or implement one itself).
// Then implement this function which will return a reference to an
// instance of the file_system.

/* If you don't need a specialized file system, add the following lines
to your main() source file.
namespace CORN { namespace OS {
File_system &file_system() { return CORN::OS::file_system_logical; }
}}
*/
}}//_namespace_CORN:OS_________________________________________________________/
#endif

