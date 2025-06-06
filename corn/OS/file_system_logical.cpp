#include "corn/OS/file_system_logical.h"
#include "corn/OS/file_system_engine.h"

#include "corn/OS/directory_entry_name_concrete.h"
#include "corn/container/text_list.h"
#include "corn/string/strconv.hpp"
#include "corn/seclusion.h"
#include <assert.h>
#if (defined(_WIN32)  || defined(_WIN64))
#  include "windows.h"
#endif
//______________________________________________________________________________
namespace CORN { namespace OS {
//______________________________________________________________________________
File_system_logical::File_system_logical()
:File_system()
, arbitrary_directories()
, recognized_directories()
{
#ifdef NYI
   // These are file types defined by CORN
   take_file_type_description(new File_type_description("TDF","Tabular Data Format file"));
   take_file_type_description(new File_type_description("XLS","MicroSoft Excel"));
#endif
}
//_File_system_logical:constructor_____________________________________________/
File_system_logical::~File_system_logical()
{}
//_File_system_logical:destructor______________________________________________/
File_system_logical::Directory_entry::Directory_entry
(const File_system::Directory &_parent_directory
,const std::wstring &_name)
: Directory_entry_name_abstract(_name)
, parent_directory(&_parent_directory)                                           //110306
, unresolved_parent_directory_name_owned(0)
, mark(0)
{  assign(_parent_directory.w_str());
   append(DEFAULT_DIRECTORY_SEPARATORw_str);
   append(_name);
}
//_Directory_entry:constructor_________________________________________________/
File_system_logical::Directory_entry::Directory_entry
(const CORN::OS::Directory_name &directory_name_fully_qualified_name)            //101107
:Directory_entry_name_abstract(directory_name_fully_qualified_name,include_all)
, parent_directory(0)
, unresolved_parent_directory_name_owned(0)
, mark(0)
{
   const Directory_name *parent_dir_name = directory_name_fully_qualified_name.get_path_directory_name();
   unresolved_parent_directory_name_owned
      = parent_dir_name
      ? new CORN::OS::Directory_name_concrete                                    //131212
         (parent_dir_name->w_str())                                              //160126
      : 0;
}
//_Directory_entry:constructor______________________________________2002-02-05_/
File_system_logical::Directory_entry::~Directory_entry()
{  delete unresolved_parent_directory_name_owned; unresolved_parent_directory_name_owned = 0;
}
//_Directory_entry:destructor_______________________________________2002-02-10_/
const Directory_entry_name &File_system_logical::Directory_entry::set_cstr
(const char *to_copy_from)                                          assignation_
{  parent_directory = 0;  // this will be provided as needed
   CORN::OS::Directory_entry_name_concrete entry_name(to_copy_from);
   return Directory_entry_name_abstract::set_DEN(entry_name);
}
//_set_cstr_________________________________________________________2012-02-09_/
const Directory_entry_name &File_system_logical::Directory_entry::set_wstr
(const wchar_t *to_copy_from)                                       assignation_
{  parent_directory = 0;  // this will be provided as needed
   CORN::OS::Directory_entry_name_concrete entry_name(to_copy_from);
   return Directory_entry_name_abstract::set_DEN(entry_name);
}
//_set_wstr_________________________________________________________2012-02-09_/
void File_system_logical::Directory_entry::construction_compose()   construction_
{  File_system_logical::Directory_entry *this_modifiable = const_cast<File_system_logical::Directory_entry *>(this);
   if (this_modifiable)                                                          //140422
   {
      std::wstring composed_name;
      if (parent_directory)                                                      //140422
         composed_name.assign(parent_directory->w_str());
      else if (unresolved_parent_directory_name_owned)
         composed_name.assign(unresolved_parent_directory_name_owned->w_str());
      composed_name.append(DEFAULT_DIRECTORY_SEPARATORw_str);
      this_modifiable->assign(composed_name);                                    //140422
   }
   #ifdef NYI
   std::wstring name_without_extension(_name);
   if (name has dot)
      assume we want to replace the extension in the name with
      name_without_extension then trim from right most dot

   append(name_without_extension);
   #endif
   Directory_entry_name_abstract::construction_compose();
}
//_construction_compose_____________________________________________2012-02-10_/
const std::wstring &File_system_logical::Directory_entry::get_name()       const
{  return Directory_entry_name_abstract::get_name();
}
//_get_name_________________________________________________________2009-11-14_/
bool File_system_logical::Directory_entry::exists()                 affirmation_
{  return file_system_engine.exists(ref_designation());
}
//_exists______________________________________________________________________/
bool File_system_logical::Directory::create()                              const
{
   if (CORN::OS::file_system_engine.exists(*this)) return true;                  //160127
   const File_system::Directory *parent_directory = provide_parent_directory();  //140104
   if (parent_directory && !parent_directory->is_root())                         //140104_120306
        parent_directory->create();                                              //140104
   // In order to create the directory we must ensure the parent directory
   // already exists.  Also ensures this is in the logical directory tree.
   // There may be no parent if this is a root directory.
   if (file_system_engine.exists(ref_designation()))
      return true; // The directory already exists and is presumably valid.
   return file_system_engine.create_directory(ref_designation());
}
//_create______________________________________________________________________/
const File_system::Directory *File_system_logical::Directory_entry
::provide_parent_directory()                                          provision_
{  if (!parent_directory)
   {  if (unresolved_parent_directory_name_owned)
      {  parent_directory = file_system().provide_directory(*unresolved_parent_directory_name_owned);
         assert(parent_directory); // assert is for debugging
         delete unresolved_parent_directory_name_owned;                          //131212
// I should delete unresolved_parent_directory_name_owned but it was crashing
         unresolved_parent_directory_name_owned = 0;                             //131212
      }
   }
   return parent_directory;                                                      //140104
#ifdef OBSOLETE
101024  don't delete yet, may want to move to Directory_entry_name_concrete

   if (!parent_directory_known)                                                  //100103
   {   //100103_
      const File_system::Directory *is_this_a_directory = dynamic_cast<const File_system::Directory *>(this);
      if (is_this_a_directory && (is_this_a_directory->is_root()))
         parent_directory  = 0; // roots have no parent directory                //100103
      else
      {  // I don't know my parent directory, but maybe my designation's path does //100103
         const Path &path = designation->ref_path();                             //100103
         parent_directory  = path.directory_in_file_system;                      //100103
         if (!parent_directory)                                                  //100103
         {  // My designation might know my name                                 //100103
            if (path.directory_name)                                             //100103
            {  // Get my parent directory from the file system, (don't own the directory name, my designation or whoever owns my designation owns it)  //100103_
               parent_directory = &file_system().provide_directory_with_known(*(path.directory_name)); //100103
            }
         }
      }
      parent_directory_known = true;                                             //100103
   } // else we already known the parent directory, just return it.              //100103
   return parent_directory;
#endif
}
//_provide_parent_directory_________________________________________2009-11-15_/
const CORN::OS::Directory_name *File_system_logical::Directory_entry
::get_path_directory_name()                                                const
{  return provide_parent_directory();                                            //140104
}
//_Directory_entry__________________________________________________2012-02-06_/
File_system_logical::Directory::Directory
(const File_system::Directory &_parent_directory
,const std::wstring &_name
,bool create_now_if_needed)                                                      //130106
: Directory_entry(_parent_directory,_name)
{  // name includes extension (if any)
   if (create_now_if_needed) create();                                           //130106
}
//______________________________________________________________________________
File_system_logical::Directory::Directory
(const CORN::OS::Directory_name &directory_name_fully_qualified_name
,bool create_now_if_needed)                                                      //130116
: Directory_entry(directory_name_fully_qualified_name)
{  if (create_now_if_needed) create();                                           //130106
}
//_Directory::constructor___________________________________________2010-11-07_/
bool File_system_logical::Directory::exist_entry_matching   // was has_directory_entry
(const Directory_entry_name &dir_entry_name_qualified
,Path_inclusion inclusion)                                          affirmation_
{  Directory_entry_name_concrete local_dir_entry_name
      (dir_entry_name_qualified,inclusion);
   return file_system_engine.exists(local_dir_entry_name);
}
//_exist_entry_matching_____________________________________________2010-01-20_/
bool File_system_logical::Directory::exists_subdirectory
(const std::wstring &unqualified_name)                              affirmation_
{  Directory_entry_name_concrete local_dir_entry_name
      (*this,unqualified_name,L"");
   return file_system_engine.exists(local_dir_entry_name);
}
//_exists_subdirectory______________________________________________2010-01-20_/
CORN::OS::File_name *File_system_logical::Directory::find_file_name
(const std::wstring  &target_file_name_pattern
,File_system::Recursion recursion,nat8 recursions)                    rendition_ //200426_150908
{  Unidirectional_list matching_filename_list;
   CORN::Inclusion target_filename_seclusion(target_file_name_pattern,true);
   CORN::OS::file_system_engine.list_DENs                                        //161103
      (*this,&matching_filename_list,0
      ,target_filename_seclusion,recursion,recursions);                          //200426
   return  dynamic_cast<CORN::OS::File_name *>(matching_filename_list.pop_at_head());
}
//_find_file_name______________________________________________________________/
CORN::OS::Directory_name *File_system_logical::Directory::find_directory_name
(const std::wstring  &target_directory_name_unqualified
,File_system::Recursion recursion
,nat8 recursions
,Directory_entry_name::Qualification qualification)                   rendition_ //150908
{  Unidirectional_list matching_dir_name_list;
   CORN::OS::file_system_engine.list_DENs                                        //161106
      (*this,0,&matching_dir_name_list
      ,CORN::Inclusion(target_directory_name_unqualified,false)					 //150908
	  ,recursion,recursions);                                                    //200426
   return  dynamic_cast<CORN::OS::Directory_name *>(matching_dir_name_list.pop_at_head());
}
//_find_directory_name______________________________________________2010-01-07_/
/*200426 obsolete now using find_qualified_name_in_path
CORN::OS::File_name *File_system_logical::Directory::find__file_name_in_path
(const std::wstring  &target_file_name_unqualified
,bool recurse_up_the_path
,bool inclusive) rendition_
{
   //150908  This method is probably obsolete because because find_file_name now can search down or up the directory tree.
   CORN::OS::File_name *found_filename
      = inclusive
         ? find_file_name
            (target_file_name_unqualified
            ,none_recursion_inclusive)
         :0;
   if (!found_filename && recurse_up_the_path && !is_root())
   {  const File_system::Directory *parent = provide_parent_directory();
       if (parent)                                                               //140104
          found_filename = parent->find_file_name_in_path
             (target_file_name_unqualified,recurse_up_the_path,true);            //140104
   }
   return  found_filename;
}
//_find_file_name_in_path___________________________________________2012-02-02_/
CORN::OS::Directory_name *File_system_logical::Directory::find__directory_name_in_path
(const std::wstring   &target_dir_name_unqualified,bool recurse_up_the_path, bool inclusive)  rendition_
{
   //150908  This method is probably obsolete because because find_file_name now can search down or up the directory tree.
   if (get_name() == target_dir_name_unqualified) return this;                   //160127
   CORN::OS::Directory_name *found_dirname
      = inclusive ?find_directory_name
         (target_dir_name_unqualified
         ,none_recursion_inclusive
         ,255
         ,relative_partial_qualification                                         //150908
               //I think in logical filesystem we would always want relative filename
         ) : 0;   //130825
         // we don't find_directory_name recurvive because that will start
         // searching down the path we just came up (infinite loop)
   if (!found_dirname && recurse_up_the_path && !is_root())
   {  const File_system::Directory *parent = provide_parent_directory();
       if (parent)                                                               //140114
         found_dirname = parent->find_directory_name_in_path                     //140104
            (target_dir_name_unqualified,recurse_up_the_path,false);
   }
   return  found_dirname;
}
//_find_directory_name_in_path______________________________________2012-02-08_/
*/
/*200426 obsolete desktop context definition is now a function of Explorer

const std::string &File_system_logical::Directory::set_desktop_context
(const std::string &suite_name
,const std::string &context)                                               const
{
   Directory_name_concrete desktop_ini_filename(*this,L"desktop.ini");
   #if ((defined(_Windows)  || defined(_WIN32)) && !defined(__GNUC__) )
      #ifdef _WCHAR_T_DEFINED
      {
         std::wstring suite_nameW; CORN::string_to_wstring(suite_name, suite_nameW);
         std::wstring contextW; CORN::string_to_wstring(context, contextW);
         WritePrivateProfileString("context",suite_name.c_str(),context.c_str(),desktop_ini_filename.c_str());
         WritePrivateProfileStringW(L"context",suite_nameW.c_str(),contextW.c_str(),desktop_ini_filename.w_str());
      }
      #else
         WritePrivateProfileString("context",suite_name.c_str(),context.c_str(),desktop_ini_filename.c_str());
      #endif
   #else
      assert(false);  // NYI for other OS
   #endif
   return context;
}
//_set_desktop_context______________________________________________2014-04-15_/
*/
/*200426 obsolete desktop context definition is now a function of Explorer

std::string File_system_logical::Directory::get_desktop_context
(const std::string &suite_name)                                            const
{  std::string context_return;
   Directory_name_concrete desktop_ini_filename(*this,L"desktop.ini");
   #ifdef WINAPI
      char context_cstr[1024];
      GetPrivateProfileStringA("context",suite_name.c_str(),"unknown",context_cstr,1024,desktop_ini_filename.c_str());
      context_return =  context_cstr;
   #else
      assert(false);  // NYI for other OS
   #endif
   return context_return;
}
//_get_desktop_context______________________________________________2014-04-15_/
*/
const File_system::Directory *File_system_logical::provide_directory             //140104
(const Directory_name &directory_name)                                provision_
{  File_system::Directory *found_directory =
      dynamic_cast<File_system::Directory *>                                     //140716
      (arbitrary_directories.find_cstr(directory_name.c_str()));
   if (!found_directory)
   {  found_directory = new Directory(directory_name,false);
      arbitrary_directories.append(found_directory);
   }
   return found_directory;
}
//_provide_directory________________________________________________2012-02-07_/

#if (defined(_WIN32)  || defined(_WIN64))
File_system_logical file_system_Windows;
File_system_logical &file_system_logical = file_system_Windows;
#else
File_system_logical file_system_Unix;
File_system_logical &file_system_logical = file_system_Unix;
#endif
//______________________________________________________________________________
const std::string &File_system_logical::File_type_description
::compose_file_type_pattern_for_Windows(std::string &buffer)               const
{  buffer.assign(filter_label);
   buffer.append("(");
   buffer.append("*.");
   buffer.append(extension);
   buffer.append(")");
   buffer.append("|");
   buffer.append("*.");
   buffer.append(extension);
   return buffer;
}
//_compose_file_type_pattern_for_Windows____________________________2013-03-10_/
}}//_namespace_CORN_OS_________________________________________________________/

