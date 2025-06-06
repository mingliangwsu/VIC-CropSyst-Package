#ifndef directory_entry_name_abstractH
#define directory_entry_name_abstractH
#include <string>
#include "corn/OS/directory_entry_name.h"
#include "corn/OS/file_system_types.h"
#include "corn/string/strconv.hpp"
//_Forward declarations_________________________________________________________
namespace CORN {
class Text_list;
namespace OS {
//______________________________________________________________________________
class Extension
: public std::wstring
{public:
   inline Extension(const char *ext)    {  CORN::string_to_wstring(ext,*this); }
   inline Extension(const wchar_t *ext)
      : std::wstring(ext)                                                     {}
   inline Extension(const std::string &ext)
      : std::wstring()
                                        {  CORN::string_to_wstring(ext,*this); }
   inline Extension(const std::wstring &ext)
      : std::wstring(ext)                                                     {}
};
//_Extension________________________________________________________2012-03-21_/
class Drive_designator
: public std::wstring
{public:
   inline Drive_designator(const char *drive_designator)
      {  CORN::string_to_wstring(drive_designator,*this); }
   inline Drive_designator(const wchar_t *drive_designator)
      : std::wstring(drive_designator)
      {}
   inline Drive_designator(const std::string &drive_designator)
      : std::wstring()
      {  CORN::string_to_wstring(drive_designator,*this); }
};
//_Drive_designator_________________________________________________2012-04-30_/
class Directory_entry_name_abstract
: public implements_ Directory_entry_name
, public extends_ std::wstring
{  // Implements/Recognizes file and directory names from various OS.
/*
consider making this a recursive definition
where the directory will either have a directory entry name path (owned or unowned), or
it will have a designator and rooted or not
*/
 protected:
   // In the case of the derived class Directory_entry_name_concrete
   // The name and ext  will be 0 until the path is extracted (they are contributes)
   // And the superclass wstring is the store of the fully qualified name.
   // In the case of Directory_entry_name these are the store of the name
   // and extension (contributes) and the superclass wstring is composition buffer.
   contribute_ std::wstring  name; // A valid filename or a special indicator I.e. <unnamed>
   contribute_ std::wstring  extension; // without .
   contribute_ bool           root;
   provided_   wchar_t        preferred_separator;                               //140202
   contribute_ Directory_entry_type identified_type;                             //131120
 protected:
   contribute_ std::string return_buffer_ASCII;
 protected:
   Directory_entry_name_abstract();
      // The default constructor is used only by derived class
 public: // 'structors
   Directory_entry_name_abstract
      (const char         *raw_name
      ,Directory_entry_type type = unknown_entry);
   Directory_entry_name_abstract
      (const wchar_t      *raw_name
      ,Directory_entry_type type = unknown_entry);                               //120202
   Directory_entry_name_abstract
      (const std::string  &raw_name
      ,Directory_entry_type type = unknown_entry);
   Directory_entry_name_abstract
      (const std::wstring &raw_name
      ,Directory_entry_type type = unknown_entry);
//NYN   Directory_entry_name_abstract(const std::string  *raw_name);
//NYN   Directory_entry_name_abstract(const std::wstring &raw_name);
   Directory_entry_name_abstract                                                 //091107
      (const std::wstring &_name
      ,const std::wstring &_extension
      ,Directory_entry_type = file_entry);
   Directory_entry_name_abstract                                                 //100214
      (const wchar_t *_name
      ,const wchar_t *_extension
      ,Directory_entry_type = file_entry);
   Directory_entry_name_abstract                                                 //100214
      (const char *_name
      ,const char *_extension
      ,Directory_entry_type = file_entry);
   Directory_entry_name_abstract                                                 //130905
      (const char *_name
      ,const std::wstring &_extension
      ,Directory_entry_type = file_entry);
   Directory_entry_name_abstract
      (const Directory_entry_name &raw_name
      ,Path_inclusion inclusion = CORN::OS::Directory_entry_name::include_copy
      ,Directory_entry_type type = unknown_entry);
   Directory_entry_name_abstract                                                 //120321
      (const Directory_entry_name &raw_name
      ,const Extension &extension
      ,Directory_entry_type = file_entry);
      //< Use this constructor to create an copy of the name replacing the extension.
   virtual ~Directory_entry_name_abstract();
public:
   virtual const std::wstring &get_Windows_representation(std::wstring &buffer) const;
   virtual const std::wstring &get_Unix_representation
      (std::wstring &buffer,bool represent_any_drive_designator_to_root)  const;
   virtual const char *c_str()                                            const;
   virtual const wchar_t *w_str()                                         const;
   inline virtual const char *get_key()                       const { return 0;} //161204
      // key as char* is not valid for DENs which are wide.
   inline virtual bool is_key_wstring(const std::wstring &key)      affirmation_  //180820
      { return key == this->w_str(); }

/*180820  was probably only used for find_cstr now using is_key
      
   inline virtual const wchar_t  *get_key_wide()       const { return w_str(); } //161204
*/   
public: // queries
   virtual bool has_name(bool must_be_valid = false)               affirmation_; //160121_980919
   virtual bool has_extension_any(bool must_be_valid = false)      affirmation_; //160121_980919
   virtual bool has_path()                                         affirmation_; //980919
      ///< Returns true if the filename has a path
   virtual bool has_extension_ASCII
      (const_ASCIIZ valid_ext,bool case_insensitive=false)         affirmation_; //990726
   virtual bool has_extension_Unicode
      (const_UnicodeZ valid_ext,bool case_insensitive=false)       affirmation_; //120209
   virtual bool is_valid_name_component
      (const std::wstring &name_component)                         affirmation_; //160121
   virtual bool is_valid()                                         affirmation_; //180201
public: // Queries
   virtual bool is_in_root()                                       affirmation_; //090925
   virtual bool is_root()                                          affirmation_; //090925
   virtual bool has_parent_directory()                             affirmation_; //120529
   virtual bool is_qualified                                                     //020503
      (bool and_fully_qualified_with_drive_designator = true)      affirmation_;
      ///< Returns true if the filename has a path with drive designator
   virtual bool has_designator_special()                           affirmation_; //171204
      /**< \return true if the first path item is a special designator
         // I.e. ~  or ^  or $envvar
         // where envvar is and environment variable
      **/

   virtual int compare_including
      (const Directory_entry_name &other
      ,Path_inclusion inclusion = include_all )                           const;
      /// < lexicorigraphically compares the respective selected elements specified in the inclusion.
   virtual Directory_entry_type get_type()                                const;
   virtual bool is_directory()                                     affirmation_; //120214
   virtual bool is_file()                                          affirmation_; //120212
   inline virtual nat16 get_length()                                      const  //170225
      { return length(); }
public: // name composition  (implements Directory_entry_name)
   virtual CORN::Container & append_path_list_to
      (CORN::Container &buffer                                                   //181030
      ,Path_inclusion inclusion = include_qualification_designated)       const; //091103
      // This is used by File_systems to find and Directories in memory.
   virtual const std::wstring &append_components_to
      (std::wstring &buffer
      ,Path_inclusion inclusion = include_all
      ,wchar_t preferred_seperator = DEFAULT_DIRECTORY_SEPARATORw_chr
      ,bool append_trailing_directory_separator = false)                  const; //091103
   virtual const std::string &append_components_to_string
      (std::string &buffer
      ,Path_inclusion inclusion = include_all
      ,wchar_t preferred_seperator = DEFAULT_DIRECTORY_SEPARATORw_chr            //140202
      ,bool append_trailing_directory_separator = false)                  const; //120209
   virtual const std::wstring &append_name_with_extension_to(std::wstring &buffer) const;
   virtual const std::wstring &append_name_to(std::wstring &result)       const; //131007
   virtual const std::wstring &append_designator_to(std::wstring &buffer) const; //140202_980919
   virtual const std::wstring &append_separator_to(std::wstring &buffer)  const; //140202_120824
   virtual wchar_t get_separator()                                        const; //140202
   virtual const std::wstring &get_name()                                 const;
   virtual const std::wstring &get_extension()                            const;
   virtual const std::wstring &get_name_with_extension(std::wstring &result)const;
public:  //
   virtual const CORN::OS::Directory_name &get_parent_directory_name_qualified()  const;
   bool compose_relative_to
      (const Directory_name &ref_dir_qual
         // Qualification does not need to be absolute (fully qualified)
         // but is is assumed this is similarly qualified
      ,std::wstring &composed
      ,Qualification qualification)                                       const; //190114
   /*
      composes a string representingthis directory entry name relative to the
         specified directory
         \return true if the composed path is sensible
         the rendered relative file name if the filename could be rendered.
         NULL is returned if this name is already relative,
         or if the relative path is not valid in the file system.
   */
   virtual Directory_entry_name *render_with_respect_to                          //180104
      (const Directory_entry_name &respect_DEN)                      rendition_;
 public:
   Directory_entry_type type_determination_raw_char
      (char last_char)                                                    const;
   Directory_entry_type type_determination_raw_str
      (const char *raw_name)                                              const;
   Directory_entry_type type_determination_raw_string
      (const std::string &raw_name)                                       const;
   Directory_entry_type type_determination_raw_wstr
      (const wchar_t *raw_name)                                           const;
   Directory_entry_type type_determination_raw_wstring
      (const std::wstring &raw_name)                                      const;
 protected:
   virtual const CORN::OS::Directory_name *get_path_directory_name()    const=0;
 protected: // name assignment
   // These sets should only be used in construction.
   virtual bool setup_is_root()                                   modification_;
   virtual const Directory_entry_name & set_DEN
      (const Directory_entry_name &to_copy_from)                  modification_;
   size_t find_right_most_directory_separator_pos()                       const; //120327
   size_t find_name_start_pos()                                           const; //120327
   virtual void  decompose()                                      contribution_; //120327
 protected:
   virtual void construction_compose()                            construction_;
};
//_Directory_entry_name_abstract_______________________________________________/
#if (defined (__MSDOS__) || defined(_WIN32)  || defined(_WIN64))
#  define directory_entry_name_is_case_sensitive false
#else
#  ifdef AMIGADOS
#     define directory_entry_name_is_case_sensitive false
#  else
#     define directory_entry_name_is_case_sensitive true
#  endif
#endif
//______________________________________________________________________________
}}//_namespace_CORN:OS_________________________________________________________/
#endif

