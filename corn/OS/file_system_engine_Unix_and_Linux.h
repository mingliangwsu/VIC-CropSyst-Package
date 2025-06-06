#ifndef file_system_engine_Unix_and_LinuxH
#define file_system_engine_Unix_and_LinuxH
#include "corn/OS/directory_entry_name.h"
#include "corn/OS/file_system_engine_common.h"
#ifdef _QT_
#include "corn/OS/file_system_engine_Qt.h"
#endif

//______________________________________________________________________________
namespace CORN { namespace OS {
class File_system_engine_Unix
: public extends_
#ifdef _QT_
   File_system_engine_Qt
#else
   File_system_engine_common
   // The file system engine must implement all the methods of the file system
   // even though file systems specific for OS's are not themselves 'engines'
#endif
{
public:
   virtual bool create_directory
      (const OS::Directory_name &directory_name)                          const; //121222

public: // The following methods are not yet implemented for Linux
 public:
   File_system_engine_Unix();
   virtual bool
      create_directory_link
      (const Directory_name &source_directory_name
      ,const Directory_name &target_directory_name
      ,bool symbolic = LINK_SYMBOLIC         )                            const;

      ///< Create a hard link to a directory
      ///\return the created directory element or null if the OS was unable to create the directory.
   virtual bool
      create_file_link
      (const File_name &source_directory_name
      ,const File_name &target_directory_name
      ,bool symbolic = LINK_SYMBOLIC         )                            const;

      ///< Create a hard link to a directory
      ///\return the created directory element or null if the OS was unable to create the directory.
//   virtual bool create()                                                   const;
   virtual bool recycle
      (const Directory_entry_name &entry_name, bool delete_forever)       const;
      ///< The file is not deleted unless delete_forever is true;
      ///<\return true if the file was successfully recycled, false if it failed (nothing happened).
      ///
   #ifndef _QT_
   virtual nat32 list_names_in                                                   //161029
      (const Directory_name & directory_name
      ,CORN::Text_list *file_name_list_unqual
      ,CORN::Text_list *directory_name_list_unqual)                       const;
   virtual nat32 list_entry_names_in_deprecated //161019 list_entry_names_in                      //151212
      (const Directory_name & directory_name
      ,CORN::Text_list *file_name_list_unqual
      ,CORN::Text_list *directory_name_list_unqual
      ,const wchar_t *name_wild_card      = 0
      ,const wchar_t *extension_filter    = 0
      ,const CORN::Text_list *exclude_directories = 0)                    const;
      // This the wildcard and filter implementation  is becoming obsolete, now use Seclusion version
   #endif
/* 181030 moved to graphical_shell
   virtual bool browse_directory
      (const Directory_name &dir_name
      ,bool open_new_window
      ,bool show_directory_tree_view
      ,bool show_as_apparent_root
      ,bool select)                                                       const;
      ///< Declared in and documented File_system
   virtual bool edit_file  (const File_name &file_name, HANDLE_OR_ERROR &handle_or_error)                     const;
      ///< Edit the file with a suitable  associated program using ShellExecute
   virtual bool view_file  (const File_name &file_name, HANDLE_OR_ERROR &handle_or_error)                   const;
   virtual bool print_file (const File_name &file_name, HANDLE_OR_ERROR &handle_or_error)                   const;
   virtual bool open_file  (const File_name &file_name, HANDLE_OR_ERROR &handle_or_error)                   const;
*/
   virtual bool copy_file
      (const File_name &source, const File_name &destination
      ,bool fail_if_exists)                                               const; //120209
   virtual bool copy_directory                                                   //130505
      (const Directory_name &source, const Directory_name &destination
      ,bool fail_if_exists)                                               const;

   virtual bool move                                                             //171216
      (const Directory_entry_name &source_DEN
      ,const Directory_entry_name &destination_DEN
      ,bool force
      ,bool interactive
      ,bool update
      ,bool verbose)                                                      const;
   /*conceptual
   bool are_in_same_file_system
      (const Directory_entry_name &DEN1
      ,const Directory_entry_name &DEN2)                            affirmation;
      // Returns true if both Directory entries are on the same file system
   */

   virtual bool set_current_working_directory(const Directory_name &dir_name) const;//120217
 public:
   virtual bool exists(const Directory_entry_name &directory_entry_name)  const;
   virtual datetime64 get_modification_date_time(const Directory_entry_name &directory_entry_name) const;
   virtual Directory_name *render_CSIDL_directory_name(nat16 CSIDL)       const; //120319
      /**< As far as I know, Unix/Linux does not have CSIDL directory codes
         we render here directory names that are analogous in various desktops.
           see http://msdn.microsoft.com/en-us/library/windows/desktop/bb762494(v=vs.85).aspx
      **/
    /*
   virtual Directory_name *render_current_working_directory()              const;//120614
       // Instanciates a directory name corresponding to the
       // the current working directory.
       // The directory name object is relinquished to the caller.
   */
   virtual Directory_entry_name *render_resolved_shortcut_or_assume_actual
       (const Directory_entry_name &actual_directory_entry_or_shortcut_LNK) rendition_;
   virtual Directory_entry_name *render_resolved_symbolic_link_or_shortcut
      (const Directory_entry_name &file_name
      ,bool relative)                                                     const;
   virtual bool is_symbolic_link(const Directory_entry_name &file_name)   const; //121001
   inline virtual bool is_shortcut(const Directory_entry_name &file_name) const  //151207
      {return false; } //Currently Windows shortcuts are not recognized in Linux
   virtual nat32 get_file_size_bytes(const File_name &file_name)          const; //121108
   virtual CORN::OS::Directory_name   *render_home()                 rendition_; //161025
   virtual std::wstring *render_environment_variable_value                       //180906
      (const std::wstring &envvar)                                   rendition_;
   inline virtual char get_envvar_list_separator()        const { return ':'; }; //200611
};
//_File_system_engine_Unix________________________________________________2014_/
}}//_namespace_CORN:OS_________________________________________________________/
#endif

