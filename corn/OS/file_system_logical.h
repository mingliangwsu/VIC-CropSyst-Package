#ifndef file_system_logicalH
#define file_system_logicalH
/*! \file */
//______________________________________________________________________________
#include "corn/OS/file_system.h"
#include "corn/const.h"
#include "corn/primitive.h"
#include "corn/OS/directory_entry_name.h"
#include "corn/OS/directory_entry_name_concrete.h"
#define DIRECTORY_ENTRY_IS_A_NAME
#ifdef _MSC_VER
#pragma warning( disable : 4250)
//MSC complains that inherits (implements) some member via dominance
#endif
//______________________________________________________________________________
namespace CORN {
namespace OS {
//______________________________________________________________________________
class File_system_logical     // Abstract class intented to be used to implement a logical root directory
: public File_system
{public:
   class File_type_description : public File_system::File_type_description
   {protected:
      std::string extension;  // Without the dot   These probably should be wide
      std::string filter_label;
   public:
      inline File_type_description
         (const std::string &_extension
         ,const std::string &_filter_label)
         : extension(_extension)
         , filter_label(_filter_label)
         {}
      inline File_type_description
         (const char *_extension
         ,const char *_filter_label)
         : extension(_extension)
         , filter_label(_filter_label)
         {}
      virtual const std::string &compose_file_type_pattern_for_Windows(std::string &pattern) const ;
   public: // association methods implementation
      inline virtual bool is_key_string(const std::string &key)     affirmation_ //180820
      { return extension == key; }

/*180820  was probably only used for find_cstr now using is_key

      inline virtual const char *get_key()                                 const { return extension.c_str(); } //030109
*/      
   public:
      inline const std::string &get_extension()    const { return extension;}  // Without the dot
      inline const std::string &get_filter_label() const { return filter_label;}
   };
   //_______________________________________________________________
   class Directory_entry
   : public implements_ File_system::Directory_entry
   , public extends_ Directory_entry_name_abstract /* unqualified */
   {
   protected:
      const provided_ File_system::Directory *parent_directory;
         // owned by file system (rename to parent_directory_known)
            // parent_directory may be
   private:
      contribute_ const CORN::OS::Directory_name_concrete *unresolved_parent_directory_name_owned; // not owned
   private:
      contribute_ nat8 mark;  // bitmask for user interface list selection (See CORN::Container::Item::Mark // 091215_
   public:
      Directory_entry
         (const File_system::Directory &_parent_directory
         ,const std::wstring &name ); // name includes extension (if any)
         // With this constructor, we definitely don't own the name

      Directory_entry(const CORN::OS::Directory_name &dir_name_fully_qual);      //101107
      virtual ~Directory_entry();
      virtual bool exists()                                        affirmation_;
   public: // The following methods are used with UI (I.e. listviews)            //081221
      inline virtual bool  is_selected()                           affirmation_
                                   { return mark & CORN::Item::filter_selected;} //011221
      inline virtual bool  is_checked()                            affirmation_
                                    { return mark & CORN::Item::filter_checked;} //011221
      inline virtual const CORN::OS::Directory_entry_name &ref_designation() const
         { return *this; }
      virtual const std::wstring &get_name()                              const; //110311_091114
   //inline virtual bool is_key_wstring(const std::wstring &key)     affirmation_; //180821
      
/*180820  was probably only used for find_cstr now using is_key
            virtual const char *get_key()                                       const; //030109'
*/      
      virtual const File_system::Directory *provide_parent_directory() provision_;  //140104_091115
      /// This ensures there is a parent directory in the in memory logical directory tree.
      /// filesystems may have specialized directories and may
      /// override this method to instanciate the specialized directory
      /// type. Or possibly return null if a physical parent directory is a logical root.
      ///< return null if there cannot possibly be a parent directory (this is a root)

      #ifdef NYI
      virtual const Directory_entry *provide_resolved_directory_entry
         (const Directory_entry_name &relative_directory_entry_name) provision_;
      #endif
   public: // The following methods work with the desktop environment
      virtual const CORN::OS::Directory_name *get_path_directory_name()   const; //120206
    public:
      virtual const Directory_entry_name &set_cstr
         (const char *to_copy_from)                                assignation_;
      virtual const Directory_entry_name &set_wstr
         (const wchar_t *to_copy_from)                             assignation_;
    protected:
      inline virtual bool is_relative()             affirmation_{ return false;}
      inline virtual bool is_qualified_relative()   affirmation_{ return false;}
      /**<  directory entries in the logical file system are always fully qualified.
      **/
      virtual void construction_compose()                         construction_;
   };
   //_Directory_entry__________________________________________________________/
   class Directory
   : public implements_ File_system::Directory // interface
   , public extends_    Directory_entry
   {protected:
      provided_ CORN::Bidirectional_list arbitrary_subdirectories;                //091107
    public:  // 'structors
       Directory
         (const File_system::Directory &_parent_directory
         ,const std::wstring &_name
         ,bool create_now_if_needed);                                            //130116
         // name includes extension (if any)
      Directory                                                                  //101107
         (const CORN::OS::Directory_name &directory_name_fully_qualified_name
         ,bool create_now_if_needed);                                            //130116
         // This constructor is used by Parameters_directory
      virtual bool create()                                               const;
         ///< Creates an actual directory in the actual file system.
         ///< \return true if the directory is valid (was created or it already exists).
      virtual  CORN::OS::File_name *find_file_name
         (const std::wstring   &target_file_name_unqualified_pattern
         ,File_system::Recursion recursion,nat8 recursions=255)      rendition_; //200426_150908
      virtual  CORN::OS::Directory_name *find_directory_name                     //100107
         (const std::wstring   &target_directory_name_unqualified                //150908
         ,File_system::Recursion recursion,nat8 recursions                       //200426
         ,Directory_entry_name::Qualification qualification)         rendition_; //150908

      // Note that these find_XXX_name_in_path are probably obsolete             //150908
      // because the find_XXX_name recursion option and specify the direction(s)

      /*200426 obsolete, now using find_qualified_name_in_path
      virtual  CORN::OS::File_name *find__file_name_in_path                       //110202
         (const std::wstring   &target_file_name_unqualified
         ,bool recurse_up_type_path,bool inclusive)                  rendition_;
      virtual  CORN::OS::Directory_name *find__directory_name_in_path             //110202
         (const std::wstring   &target_file_name_unqualified
         ,bool recurse_up_type_path,bool inclusive)                  rendition_;
      */
         /**< obsolete
          The find_name_in_path methods search the parent and ansestor directories.
          When recurse is true, recursively search all the parent directories in the path
          Otherwise look only in the parent directory.

          Note might want to add an option to search subdirectories
          of the ancestors in the path, but need to make sure not to
          recurse down the directory we just came up from.
         **/
      bool exist_entry_matching   // was has_directory_entry                     //100101
         (const Directory_entry_name &dir_entry_name_qualified
         ,Path_inclusion include =
              Directory_entry_name::include_name
            | Directory_entry_name::include_extension)             affirmation_;
      virtual bool exists_subdirectory(const std::wstring &name_unqual)   const; //100220
         /**<
            Derived classes may override this method if they keep file lists in memory.
         **/
      inline virtual void reset()                                  provision_ {}
      inline virtual bool is_directory()            affirmation_ { return true;} //120214
      inline virtual bool is_file()                affirmation_ { return false;} //120214
      /*200426 obsolete desktop context definition is now a function of Explorer
      const std::string &set_desktop_context
         (const std::string &suite_name
         ,const std::string &context)                                     const; //140415
      std::string get_desktop_context
         (const std::string &suite_name)                                  const; //140415
      */
    private:
      /*
      inline Directory *confirm_concrete()                                        const
         {  CORN::OS::Directory_name_concrete directory_name_fully_qualified_name(L"");
            return new Directory(directory_name_fully_qualified_name,false); }
         // This function exists to have the compiler ensure that
         // all abstract methods have been implememted
      */
     public:
      inline virtual const CORN::OS::Directory_name &get_dir_DEN() const { return *this;} //160902
   };
   //_Directory________________________________________________________________/
   class File
   : public CORN::OS::File_system::File
   {public:  // 'structors
      File(const File_name *filename_name, bool take_file_name = false);
      inline virtual bool is_directory()            affirmation_{ return false;} //120214
      inline virtual bool is_file()                 affirmation_{ return true;}  //120214
   };
   //_File_____________________________________________________________________/
public: // 'structors
   File_system_logical();
   virtual ~File_system_logical();
public:
   virtual const File_system::Directory *provide_directory                       //120207
      (const Directory_name &directory_name)                          provision_;
      /**<
      Derived classes with specialized directories will want to override
      this method to insure the specialized directory is instanciated.
      This is used in the following cases:
      - We are recursively building a list of directory and/or filenames
        list_entry_names().
      **/
protected:
   bool take_arbitrary(File_system::Directory &directory)         appropriation_;
   // For each of the roots attempt to give ownership to the
   // root directory until one takes it (That is it recognizes it as a subdirectory).
protected:
   provided_ Bidirectional_list arbitrary_directories;
      ///< Directories that have been encountered but have not special meaning
   provided_ Bidirectional_list /*180101 Association_list*/ recognized_directories;                            //120205
      ///< Specialized directories that have recognized contexts.
      ///  used only by (almost all) derived classes.

#ifdef NYI

   virtual bool take_file_type_description
      (File_type_description *file_type_description) appropriation_;
      ///< \return true if the description was added to the list of file type descriptions (not already listed).
#endif
};
//_____________________________________________________ XXXXXXXXXXXXXXXXXXXX __/
extern File_system_logical &file_system_logical;
}}//_namespace_CORN:OS_________________________________________________________/
//_2012-02-04___________________________________________________________________
#define provide_file_name_implementation(class_name,filename_method,filename_variable,filename_literal) \
const CORN::OS::File_name &class_name::filename_method()             provision_ \
{  if (!filename_variable)                                                      \
      filename_variable=new CORN::OS::File_name_concrete(*this,filename_literal);\
   return *filename_variable;                                                   \
}
//_2012-02-06___________________________________________________________________
#define provide_subdir_file_name_implementation(class_name,filename_method,filename_variable,subdir,filename_literal) \
const CORN::OS::File_name &class_name::filename_method()             provision_ \
{  if (!filename_variable)                                                      \
     filename_variable=new CORN::OS::File_name_concrete(subdir,filename_literal);\
   return *filename_variable;                                                   \
}
//_2012-04-15___________________________________________________________________
#define provide_subdir_directory_name_implementation(class_name,dirname_method,dirname_variable,subdir,dirname_literal) \
const CORN::OS::Directory_name &class_name::dirname_method()             provision_ \
{  if (!dirname_variable)                                                      \
     dirname_variable=new CORN::OS::Directory_name_concrete(subdir,dirname_literal,CORN::OS::directory_entry);\
   return *dirname_variable;                                                   \
}
//_2012-04-15___________________________________________________________________

#define provide_directory_name_implementation(class_name,directory_name_method,directory_name_variable,directory_name_literal) \
const CORN::OS::Directory_name &class_name::directory_name_method()  provision_ \
{  if (!directory_name_variable)                                                \
      directory_name_variable = new CORN::OS::Directory_name_concrete(get_dir_DEN(),directory_name_literal,CORN::OS::directory_entry); \
   return *directory_name_variable;                                             \
}
//_2012-02-06___________________________________________________________________
#define provide_specialized_directory_name_implementation(class_name,directory_type,directory_name_method,directory_name_variable,directory_name_literal) \
const directory_type &class_name::directory_name_method()            provision_ \
{  if (!directory_name_variable)                                                \
      directory_name_variable = new directory_type(get_dir_DEN(),directory_name_literal); \
   return *directory_name_variable;                                             \
}
//_2012-03-20___________________________________________________________________
#define provide_specialized_subdirectory_name_implementation(class_name,directory_type,directory_name_method,directory_name_variable,directory_name_literal) \
const directory_type &class_name::directory_name_method()             provision_\
{  if (!directory_name_variable)                                                \
      directory_name_variable = new directory_type(directory_name_literal);     \
      return *directory_name_variable;                                          \
}
//_2012-04-15___________________________________________________________________
#define provide_suite_application_directory_name_implementation(class_name,directory_type,directory_name_method,directory_name_variable,directory_name_literal) \
const directory_type &class_name::directory_name_method()             provision_\
{  if (!directory_name_variable)                                                \
      directory_name_variable = new directory_type(get_dir_DEN(),directory_name_literal);     \
      return *directory_name_variable;                                          \
}
//_2015-05-19___________________________________________________________________
#endif
