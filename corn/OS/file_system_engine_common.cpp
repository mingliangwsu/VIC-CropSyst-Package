#include <string>
#include "corn/OS/directory_entry_name_concrete.h"
#include "corn/OS/file_system_engine_common.h"
#include "corn/container/text_list.h"
#include "corn/string/strconv.hpp"
#include "corn/seclusion.h"

#include <sys/stat.h>
// for struct stat and stat()  (Unix, Linux, Windows)
#if (defined(_WIN32)  || defined(_WIN64) || defined(__MSDOS__))
#  include <dir.h>
#else
#  include <errno.h>
#endif
#ifdef __unix
//__MINGW_ANSI_STDIO__
#include <unistd.h>
#endif
// for getcwd()  dir.h may be Windows specific

#ifdef _MSC_VER
#include <direct.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <string.h>
   //for strchr

namespace CORN { namespace OS {
std::wstring name_ext; // This is here as a hack because the compiler doesn't like this as a declaration local for some unknown reason
//______________________________________________________________________________
File_system::Directory::Type File_system_engine_common::identify_type
(const Directory_entry_name &entry_name)                                   const
{  // This code is the same for Unix, Linux, Windows and POSIX complient OS.
#ifdef WINAPI
   struct _stat st_buf;                                                          //120209
   if ((entry_name.w_str()[1] == L':') && (entry_name.w_str()[2] == 0))          //150305
      return directory_entry;                                                    //150305
   int status = _wstat(entry_name.w_str(),&st_buf);                              //120209
#  else
   // I don't know if Unix/Linux have wide versions of these functions
   struct stat st_buf;
   int status = stat(entry_name.c_str(),&st_buf);
#  endif
   File_system::Directory::Type identified_type
      = (status == 0) ? ((File_system::Directory::Type )((st_buf.st_mode & S_IFMT) >> 12)) :unknown_entry;
   // For other OS's will likely need to reimplement this method in a derived class.
   return identified_type;
}
//_identify_type_________________________________________________identify_type_/
Directory_name *File_system_engine_common::render_current_working_directory() const
{  char cwd[1024];
#  ifdef _MSC_VER
      _getcwd
#  else
      getcwd                                                                     //981104
#  endif
      (cwd,1023);
   return new Directory_name_concrete(cwd,CORN::OS::directory_entry);            //161029_160206
}
//_render_current_working_directory_________________________________2009-12-08_/
Directory_name *File_system_engine_common::render_current_working_directory_on_drive
(const Drive_designator &logical_drive_name)                               const
{
   // Only Windows, DOS, OS2? and AmigaDOS have drive designators
   // For all others, we simply get the current working directory.
   return render_current_working_directory();
}
//_render_current_working_directory_on_drive________________________2012-04-30_/
#ifndef ENOENT
//120711 Some older compilers (I.e. BCB5) might not have ENOENT defined
#define ENOENT 2
#endif

bool File_system_engine_common::log_any_delete_error
( int unlink_or_rm_dir_error,CORN::Text_list &error_log)                   const
{  bool in_error = unlink_or_rm_dir_error > 0;
   if (in_error)                                                                 //091219
   {  std::string error_message("Error (");
      char error_no[10];
      int16_to_cstr(unlink_or_rm_dir_error,error_no,10);
      error_message.append(error_no);
      error_message.append(
                  (unlink_or_rm_dir_error == ENOENT )
                  ? ") Path or file function not found"
                  : ") Permission denied");
      error_log.add_cstr(error_message.c_str());
   }
   return in_error;
}
//_log_any_delete_error________________________________________________________/
Directory_entry_name *File_system_engine_common::render_resolved_absolute_DEN
(const Directory_entry_name &filename_abs_or_rel
// cases
// ..\..\..\X\Y\z.ext
// X\Y\z.ext
// z.ext
// .\z.ext
// <999\path\file.ext
// $designator:\path\file.ext

,const Directory_name &with_respect_to_directory_name_qualified)      rendition_
//Case A: C:\L\M\N\O
{
   Directory_entry_name_concrete *resolved_DEN=0;
   /**<
      // Returns 0 if nothing to resolve

      Respect directory
      C:/A/B/C/D/E

      relative:./filename.ext
      renders->C:/A/B/C/D/E

      relative:../../x/y/z.exe
      renders->C:/A/B/C/x/y/z.ext

      The method as implemented in this class cannot handle the form:
      relative:../../x/../y/z.exe
      This form is fairly rarely used, but
      derived classes should override this method using an OS API function
      which handles all cases properly.
   **/

   if (filename_abs_or_rel.has_designator_special())                             //171204
   { // Handle any special designator                                            //171204

      CORN::Text_list pot_designated_special_dir_names;
      filename_abs_or_rel.append_path_list_to
         (pot_designated_special_dir_names
         ,CORN::OS::Directory_entry_name::include_all);
      Item_wstring *first = dynamic_cast<Item_wstring *>                         //171204
         (pot_designated_special_dir_names.pop_first());                         //171204
      if (first)
      {
         const Item_wstring *next
            = dynamic_cast<Item_wstring *>                                       //171204
            (pot_designated_special_dir_names.get_at(0));                        //171204
         Text_list resolved_designator;                                          //171204
         bool resolved = resolve_designator                                      //171204
            (first,next
            ,resolved_designator);                                               //171204
         if (resolved)                                                           //171204
         {                                                                       //171204
            pot_designated_special_dir_names.prepend_items(resolved_designator); //171204
            resolved_DEN                                                         //171204
               = new Directory_entry_name_concrete
                  (pot_designated_special_dir_names
                  ,Directory_entry_name::include_qualification_designated);      //171204
            return resolved_DEN;                                                 //171204
         }
         delete first;
      } // if first
   } // if designator special
   else
   {
   bool stop_decomposing_relative_paths = false;
   bool appears_to_be_relative = filename_abs_or_rel.is_relative();              //160204
   bool qualified = filename_abs_or_rel.is_qualified(true);
   if (!appears_to_be_relative && qualified)                                     //160204
      return new CORN::OS::Directory_entry_name_concrete
         (filename_abs_or_rel.w_str(),CORN::OS::unknown_entry);
   CORN::Text_list relative_path_dir_names;
   filename_abs_or_rel.append_path_list_to
      (relative_path_dir_names
      ,CORN::OS::Directory_entry_name::include_qualification_designated);
   CORN::Text_list left_absolute_path_dir_names;
   with_respect_to_directory_name_qualified.append_path_list_to
      (left_absolute_path_dir_names
      , CORN::OS::Directory_entry_name::include_qualification_designated
       |CORN::OS::Directory_entry_name::include_name);
   CORN::Text_list right_absolute_path_dir_names;
   bool abort = false;
   bool found_relative = false;
   bool continue_resolving = true;
   while (!abort && continue_resolving)
   {  //Warning: currently unicode filenames are converted to ASCII
      // I need a wide version of Text list
      // (Actually I need to eliminate Text_list and add string_items to container).
      //180115 CORN::Text_list::Item *curr_rel_dirname = dynamic_cast<CORN::Text_list::Item *>
      Item_wstring *curr_rel_dirname = dynamic_cast<Item_wstring *>
         (relative_path_dir_names.pop_head());
      if (curr_rel_dirname)
      {
         if ((*curr_rel_dirname) == L".")
         {  found_relative = true;/* do nothing, eat the . */
            delete curr_rel_dirname;                                             //120615
         } else if ((*curr_rel_dirname) == L"..")
         {  found_relative = true;
            if (stop_decomposing_relative_paths)
               abort = true;
            else
            {  CORN::Item *dropped_dir = dynamic_cast<CORN::Item *>
                  (left_absolute_path_dir_names.pop_tail());
               delete dropped_dir;
            }
            delete curr_rel_dirname;                                             //120615
         }
         else // it is a directory name
         {  right_absolute_path_dir_names.append(curr_rel_dirname);
            stop_decomposing_relative_paths = true;
         }
      } else
         continue_resolving = false;
   }
   //Directory_entry_name_concrete *resolved_DEN = 0;
   if (!abort)
   {  if (found_relative)
      {  left_absolute_path_dir_names.transfer_all_from(right_absolute_path_dir_names);
         std::string resolved_path;
         char *directory_separator = DEFAULT_DIRECTORY_SEPARATORa_str;
      // For Unix/Linux filenames prepend the root directory separator.
      // warning, this has not been checked for all cases
           if (directory_separator[0] == 47 /*'//'*/)
             resolved_path.append(directory_separator);
      nat32 appended = left_absolute_path_dir_names.string_items(resolved_path,directory_separator[0]);
      if (appended)
         resolved_path.append(directory_separator);
      //std::wstring name_ext;  this should be defined here but compiler is complaining
      name_ext.clear();
      filename_abs_or_rel.get_name_with_extension(name_ext);
      CORN::append_wstring_to_string (name_ext,resolved_path);                   //120209
      resolved_DEN = new Directory_name_concrete(resolved_path);
      } else
      {  resolved_DEN = new Directory_name_concrete
            (with_respect_to_directory_name_qualified                            //160128
            ,filename_abs_or_rel);
      }
   }
   }
   return resolved_DEN;
}
//_render_resolved_absolute_DEN_____________________________________2012-04-30_/
Directory_entry_name *File_system_engine_common::render_relative_DEN             //170411
(const Directory_entry_name &DEN_absolute
,const Directory_name       &with_respect_to_dir_absolute)            rendition_
{  std::wstring DEN_relative_result;
      /** \returns a version of the directory name relatively qualified
         with respect to the specified directory.
         DEN_absolute is expected to be fully absolute qualified filename.
         If DEN_absolute  is already relative, then simply renders a copy.
         If must_exist is true, return 0 if the rendered file name does not
         exist.
      */

   CORN::Text_list DEN_absolute_path_list; // will not include ultimate name and extension
   DEN_absolute.append_path_list_to
      (DEN_absolute_path_list
      ,CORN::OS::Directory_entry_name::include_qualification_designated);
   CORN::Text_list respect_dir_path_list;
   with_respect_to_dir_absolute.append_path_list_to
      (respect_dir_path_list
      ,CORN::OS::Directory_entry_name::include_all);
   Item_wstring *DEN_absolute_item = 0;
   Item_wstring *respect_item = 0;
   do
   {  DEN_absolute_item = dynamic_cast<Item_wstring *>
         (DEN_absolute_path_list.pop_head());
      respect_item = dynamic_cast<Item_wstring *>
         (respect_dir_path_list.pop_head());
   } while ((DEN_absolute_item && respect_item)
      && (*DEN_absolute_item) == (*respect_item));
   // Put the last mismatch items back on the respective lists
   DEN_absolute_path_list.prepend(DEN_absolute_item);
   respect_dir_path_list.prepend(respect_item);
   for (nat8 p = 0; p < (nat8)respect_dir_path_list.count(); p++)
   {  DEN_relative_result += L"..";
      DEN_relative_result += DEFAULT_DIRECTORY_SEPARATORw_chr;
   }
   DEN_absolute_path_list.string_items
      (DEN_relative_result,DEFAULT_DIRECTORY_SEPARATORw_chr);
   DEN_relative_result += DEFAULT_DIRECTORY_SEPARATORw_chr;
   DEN_relative_result += DEN_absolute.get_name();
   std::wstring extension = DEN_absolute.get_extension();
   if (extension.length())
   {  DEN_relative_result += '.';
      DEN_relative_result += extension;
   }
   return new CORN::OS::Directory_entry_name_concrete(DEN_relative_result);
}
//_render_relative_DEN______________________________________________2017-04-11_/
Provision File_system_engine_common::provide_directory
(const Directory_name &directory_name)                                     const
{  Provision provision = preexist;
   if (directory_name.is_root()) return preexist;
   if (CORN::OS::file_system_engine.exists(directory_name))
      provision = preexist;
   else
   {  const CORN::OS::Directory_name *parent_path
         = directory_name.get_path_directory_name();                             //140101
      if (parent_path)
            provision = provide_directory(*parent_path);
      else  return fail;
      provision = (create_directory(directory_name)) ? create : fail;
   }
   return provision;
}
//_provide_directory_____________________________________2015-03-21_2014-08-25_/
bool File_system_engine_common::rename
(const Directory_entry_name &old_name
,const Directory_entry_name &new_name)                                     const
{
   // This function works only for filenames that are ASCII
   // The Windows version should use the Windows API function
   // supporting UNICODE
   return ::rename(old_name.c_str(),new_name.c_str()) == 0; //stdio
}
//_rename___________________________________________________________2012-12-09_/
Directory_entry_name *File_system_engine_common::find_qualified_name_in_path
(const std::wstring &unqualified_name
,const CORN::OS::Directory_name &from_directory
,Path_inclusion include_qualification_directories_in_search)          rendition_ //140210
{  Directory_entry_name_concrete *filename_in_this_dir =                         //140217
      new Directory_entry_name_concrete(from_directory,unqualified_name);        //140217
   Directory_entry_name *found_filename =  filename_in_this_dir;                 //140217
   bool filename_in_this_dir_exists = exists(*filename_in_this_dir);
   if (!filename_in_this_dir_exists)
   {  delete filename_in_this_dir;
      found_filename = filename_in_this_dir =0;
      if (include_qualification_directories_in_search)                           //140210
      {  Path_inclusion next_include_qualification_directories_in_search =       //140210
            ( include_qualification_directories_in_search
            & Directory_entry_name::include_ancestors_path)                      //140210
            ? include_qualification_directories_in_search                        //140210
            : 0; // only search the parent                                       //140210
         const Directory_name *parent_dir = from_directory.get_path_directory_name();
         if (parent_dir)
            found_filename = find_qualified_name_in_path
               (unqualified_name,*parent_dir
               ,next_include_qualification_directories_in_search);               //140210
      }
   }
   return found_filename;
}
//_find_qualified_name_in_path______________________________________2013-11-11_/
Directory_entry_name *File_system_engine_common::find_DEN
(const std::wstring             &target_name // can now have wild card           //160922
,Directory_entry_type  type
,const Directory_name &from_directory
,File_system::Recursion          recursion                                       //150908
,nat8                            recursions                                      //200426
,const Directory_name            *logical_root
,bool                            case_insensitive)                    rendition_ //160924
{
   Unidirectional_list matching_filename_list;
   Unidirectional_list matching_dirname_list;
   list_DENs // list_entry_names
      (from_directory
      ,&matching_filename_list,&matching_dirname_list
      ,CORN::Inclusion(target_name,case_insensitive)
      ,recursion,recursions
      ,logical_root);                                                  //150908
      // I think                     //150908
   Directory_entry_name *found_DEN
    = (type == CORN::OS::directory_entry)
      ?  dynamic_cast<Directory_entry_name *>(matching_dirname_list .pop_head())
      :  dynamic_cast<Directory_entry_name *>(matching_filename_list.pop_head());
   return found_DEN;
}
//_find_DEN_________________________________________________________2017-12-27_/
CORN::OS::File_name *File_system_engine_common::find_file_name
(const std::wstring             &target_filename // can now have wild card       //160922
,const CORN::OS::Directory_name &from_directory
,File_system::Recursion          recursion                                       //150908
,nat8                            recursions                                      //200426
,const Directory_name            *logical_root
,bool                            case_insensitive)                    rendition_ //160924
{

/*171228 NYI  can now use this, but it hasn't been tested yet

   return find_DEN
      (target_name
      ,CORN::OS::file_entry
      ,from_directory
      ,recursion
      ,logical_root
      ,case_insensitive);
*/
   Unidirectional_list matching_filename_list;
   list_DENs // list_entry_names
      (from_directory
      ,&matching_filename_list
      ,0 // no directory
      ,CORN::Inclusion(target_filename,case_insensitive)
      ,recursion,recursions,logical_root);                                       //200426_150908
   return  dynamic_cast<CORN::OS::File_name *>(matching_filename_list.pop_head());
}
//_find_file_name___________________________________________________2015-04-12_/
bool File_system_engine_common::delete_directory
      (const Directory_name & directory_name_to_delete
      ,bool recurse_subdirectories
      ,CORN::Container *remember_deleted_file_names               //optional
      ,CORN::Container *remember_not_deleted_file_names           //optional
      ,CORN::Container *remember_deleted_subdirectory_names       //optional
      ,CORN::Container *remember_not_deleted_subdirectory_names   //optional
      ,CORN::Text_list *error_log                                 //optional
      ,bool delete_only_file_and_preserve_folder_and_subfolders )          const
{  bool directory_deleted = true;
   CORN::Unidirectional_list file_name_list;
   CORN::Unidirectional_list directory_name_list;
   list_DENs
      (directory_name_to_delete
      ,&file_name_list
      ,&directory_name_list
      ,recurse_subdirectories
        ? File_system::subdirectory_recursion_inclusive
        : File_system::none_recursion_inclusive);                                //150908

// NYI   should  not need record the CWD so we can return to it when done
//       however, if the directory or one of its subdirectories is
//       the CWD the delete will fail on that directory.
//       May need to to temporarily jump out away.
//       I think the way I have this coded, the CWD will simply not be deleted.

   bool all_files_deleted = true;
   for (CORN::OS::File_name *filename = dynamic_cast<CORN::OS::File_name *>(file_name_list.pop_head())
       ;filename
       ;filename = dynamic_cast<CORN::OS::File_name *> (file_name_list.pop_head()))
   {  bool this_file_deleted = recycle(*filename,true);
      all_files_deleted &= this_file_deleted;
      if (this_file_deleted)
      {  if (remember_deleted_file_names)
             remember_deleted_file_names->append(filename);
      }
      else
      {  if (remember_not_deleted_file_names)
             remember_not_deleted_file_names->append(filename);
      }
   } // for each file
   directory_deleted &= all_files_deleted;
   if (recurse_subdirectories)
   {  bool all_subdirs_deleted = true;
      for (CORN::OS::Directory_name *subdir_to_del = dynamic_cast<CORN::OS::Directory_name *>(file_name_list.pop_head())
          ;subdir_to_del
          ;subdir_to_del = dynamic_cast<CORN::OS::Directory_name *>(file_name_list.pop_head()))
      {  bool subdir_deleted = delete_directory
            ( *subdir_to_del
            ,recurse_subdirectories
            ,remember_deleted_file_names
            ,remember_not_deleted_file_names
            ,remember_deleted_subdirectory_names
            ,remember_not_deleted_subdirectory_names
            ,error_log
            ,delete_only_file_and_preserve_folder_and_subfolders);
         all_subdirs_deleted &= subdir_deleted;
         if (subdir_deleted)
         {  if (remember_deleted_subdirectory_names)
                remember_deleted_subdirectory_names->append(subdir_to_del);
            else delete subdir_to_del;
         } else
         {  if (remember_not_deleted_subdirectory_names)
                remember_not_deleted_subdirectory_names->append(subdir_to_del);
            else delete subdir_to_del;
         }
      } // for subdir_to_del
      directory_deleted = all_subdirs_deleted;
   } // if recurse
   // Now delete this directory
   if (!delete_only_file_and_preserve_folder_and_subfolders
      && directory_deleted)
      directory_deleted &= recycle(directory_name_to_delete,true);
   return directory_deleted;
}
//_delete_directory_________________________________________________2015-03-19_/
nat32 File_system_engine_common::list_DENs                                       //161013
(const Directory_name & directory_name
,CORN::Container *file_DEN_list       //= 0 // Optional, pass null if file list not wanted
,CORN::Container *directory_DEN_list  //= 0 // optional, pass null if file list not wanted
,const Seclusion &seclusion
,File_system::Recursion recursion
,nat8 recursions                                                                 //200426
,const Directory_name *logical_root_preferred)                             const
{
   CORN::Text_list file_names_unqual;
   CORN::Text_list subdir_names_unqual;

   CORN::OS::Directory_name *logical_root_rendered = 0;
   const CORN::OS::Directory_name *logical_root_used = logical_root_preferred;

   if (directory_name.is_root())
      return 0;
      // We need to exit if it is root directory because file listing API
      // interprets the drive designator as the current working directory
   if ((recursion & FILE_SYSTEM_RECURSION_SUPERDIRECTORY) && recursions)         //200424
   {
      if (logical_root_preferred == 0)  // recursion stop point not specified    //161025
         logical_root_rendered = render_home(); // stop at home                  //161025
      logical_root_used = logical_root_rendered                                  //161025
         ? logical_root_rendered : logical_root_preferred;                       //161025
      if (!directory_name.is_root()
         && (logical_root_used->compare_including(directory_name) != 0))         //161025
      {
         list_DENs // entry_names
            (directory_name.get_parent_directory_name_qualified()
            ,file_DEN_list
            ,directory_DEN_list
            ,seclusion
            ,FILE_SYSTEM_RECURSION_SUPERDIRECTORY,(recursions-1) ,logical_root_used); //200426
              // Do not propogate the exclusion or both bits in recursed directories
      }
   }
   // seclusion occurs as we recurse down because seclusion
   // rules are applied to directory names as they are encountered
   // traversing down the file system.
   // For single rule inclusions we assume all subdirectories are recursed down

   if ((seclusion.count() > 1) && seclusion.is_excluded_DEN(directory_name))     //161029
      return 0;
   // get list of unqualified names actually in this current directory

   list_names_in(directory_name,&file_names_unqual,&subdir_names_unqual);
   if (file_DEN_list)
   {  // Add to requested list matching filenames in this directory
      FOR_EACH_IN(filename_item ,Item ,file_names_unqual,each_filename)
      {  std::wstring filename_unqual; filename_item->append_to_wstring(filename_unqual);  //180119
               CORN::OS::File_name_concrete *qualified_name
                  = new CORN::OS::File_name_concrete
                     (directory_name,filename_unqual,CORN::OS::file_entry);
               if (seclusion.is_included_DEN(*qualified_name))                   //161024
               {
                     file_DEN_list->take(qualified_name);
               } else delete qualified_name;                                     //161024
      } FOR_EACH_END(each_filename)
   }
   CORN::Unidirectional_list subdir_list;
   if (directory_DEN_list)
   {  // Add to requested list matching directory names in this directory
      FOR_EACH_IN(subdir_item,Item ,subdir_names_unqual,each_subdir)
      {
         std::wstring subdir_unqual; subdir_item->append_to_wstring(subdir_unqual);//181019
         // Warning currently text list items are based on std::string
               CORN::OS::Directory_name_concrete *qualified_name
                  = new CORN::OS::Directory_name_concrete
                     (directory_name,subdir_unqual,CORN::OS::directory_entry);
               if (seclusion.is_included_DEN(*qualified_name))                   //161024
               {
                  // although we have secluded name parts
                  // we also want to check the composed path/name
                     directory_DEN_list->take(qualified_name);
               } else delete qualified_name;                                     //161024
      } FOR_EACH_END(each_subdir)
   }
   if ((recursion & FILE_SYSTEM_RECURSION_SUBDIRECTORY) && recursions)           //200426
   {    // Now recurse down
      FOR_EACH_IN(subdir_item,Item,subdir_names_unqual,each_subdir2)
      {
         std::wstring subdir_unqual; subdir_item->append_to_wstring(subdir_unqual);
         if ((seclusion.count() <= 1) || !seclusion.is_excluded_wstring(subdir_unqual,true,false)) //181015
         {  CORN::OS::Directory_name_concrete subdir(directory_name,subdir_unqual,CORN::OS::directory_entry); //161029
            list_DENs
             (subdir
             ,file_DEN_list
             ,directory_DEN_list
             ,seclusion
             ,FILE_SYSTEM_RECURSION_SUBDIRECTORY
              // Do not propogate the exclusion or both bits in recursed directories
             ,recursions-1                                                       //200426
             ,0); // not recursion up
         }
      } FOR_EACH_END(each_subdir2)
   }
   delete logical_root_rendered;
   return ((file_DEN_list ? file_DEN_list->count() : 0)
          +(directory_DEN_list ? directory_DEN_list->count(): 0));
}
//_list_DENs________________________________________________________2016-08-14_/
bool File_system_engine_common::matches_any_in
(const std::wstring &name_with_ext
,const CORN::Text_list &wild_card_patterns_to_compare
,bool case_insensitive)                                             affirmation_
{
   // WARNING NYI actually would check wild card characters, I need
   // to implement.

   // When I implement this, I presume Linux and Windows would have
   // such wild card parsing in their respective API's and thus would
   // reimplement this method.
   // However if I need to implement this from scratch, I would
   // just implement here.
   return wild_card_patterns_to_compare.find_wstring(name_with_ext) != 0;
}
//_matches_any_in___________________________________________________2015-12-12_/
bool File_system_engine_common::copy_directory_contents
(const Directory_name &source, const Directory_name &target
,const Seclusion *seclusion_optional
,bool use_links
,bool fail_if_exists)                                                      const
{  bool copied = true;
   if (!exists(target))
      create_directory(target);
   Inclusion *include_all = 0;
   const Seclusion *seclusion_used = seclusion_optional;
   if (seclusion_optional == 0)
      seclusion_used = include_all = new Inclusion("*",false);
   CORN::Unidirectional_list contents_files;
   CORN::Unidirectional_list contents_diretories;

// std::clog << "source:" << source.c_str() << std::endl;

   list_DENs
      (source
      ,&contents_files
      ,0//&contents_diretories
      ,*seclusion_used
      ,CORN::OS::File_system::none_recursion_inclusive);
   list_DENs
      (source
      ,0//&contents_files
      ,&contents_diretories
      ,CORN::Inclusion("*\/",false)
      ,CORN::OS::File_system::none_recursion_inclusive);

   FOR_EACH_IN(file_name,File_name,contents_files,each_file)
   {  std::wstring name_with_ext;
      file_name->get_name_with_extension(name_with_ext);
      CORN::OS::File_name_concrete target_filename(target,name_with_ext);
      copied &= copy_file(*file_name,target_filename,false);
   } FOR_EACH_END(each_file)
   FOR_EACH_IN(dir_name,Directory_name,contents_diretories,each_dir)
   {  copied &= copy_directory(*dir_name,target,false);
   } FOR_EACH_END(each_dir)
   delete include_all;
   return copied;
}
//_copy_directory_contents__________________________________________2016-11-05_/
bool File_system_engine_common::resolve_designator
(const Item_wstring *designator
,const Item_wstring *next_path_item  // may be 0 and may be consumed by this.
,Text_list       &resolved_path_items)                                rendition_
{  bool resolved = false;
   nat32 designator_length = designator ? designator->length() : 0;
   if (!designator_length) return false; // no specified designator

   char first_char = designator->at(0);
   char last_char = designator->at(designator_length-1);
   //200426 std::wstring variable((*designator),1,designator_length-2);                   //180115
   Directory_entry_name *found_name_qual=0;                                      //180906
   std::wstring subitem_to_find; // this wont be need when I have Text_list<std::wstring> //180906
   if (next_path_item)                                                           //200426
   next_path_item->append_to_wstring(subitem_to_find);                           //1800906

   switch (first_char)
   {  case '~':
      {
         switch (last_char)  // This is deprecated  200426
         {
            case '~' : // users home directory
            {
               const char *current_home_dir(getenv("HOME"));
               Directory_name_concrete HOME_dir_name
                  (current_home_dir ? current_home_dir: DEFAULT_DIRECTORY_SEPARATORa_str
                  ,CORN::OS::directory_entry);
               HOME_dir_name.append_path_list_to
                  (resolved_path_items,CORN::OS::Directory_entry_name::include_all);
               resolved = true;
            } break;
            case ':' :  // special designator  begins with ~ ends with :
            {  std::wstring variable((*designator),1,designator_length-2);                   //180115
               if (variable == L"CONTEXT")
               {
                  if (next_path_item) // We only need to match composed DEN with the next DEN name.
                  {
                     CORN::OS::Directory_name_concrete CWD;                      //180222
                     found_name_qual
                     = CORN::OS::file_system_engine.find_qualified_name_in_path
                        (subitem_to_find,CWD); // Context is with respect to the CWD //180222
                     if (!found_name_qual)                                       //180511
                     {
                        found_name_qual = find_from_environment_variable         //180906
                           (L"CONTEXT",subitem_to_find);                         //180906
                     }
                  } else resolved = false; // nothing next to search for
               } else // Other special designators to be implementated would go here
                  resolved = false;
            } break;
         } // switch
      } break; // home directory contexts
      case '$' :
      {
         if (last_char == ':')  // The : is deprecated
         {  std::wstring variable((*designator),1,designator_length-2);
            found_name_qual = find_from_environment_variable
               (variable,subitem_to_find);
         } else                                                                  //200426
         {  std::wstring variable((*designator),1);                              //200426
            found_name_qual = find_from_environment_variable                     //200426
               (variable,subitem_to_find);                                       //200426
         }                                                                       //200426
      } break; // environment variable contexts
      case '^' :                                                                 //200426
      {
                  if (next_path_item) // We only need to match composed DEN with the next DEN name.
                  {
                     CORN::OS::Directory_name_concrete CWD;                      //180222
                     found_name_qual
                     = CORN::OS::file_system_engine.find_qualified_name_in_path
                        (subitem_to_find,CWD); // Context is with respect to the CWD //180222
                  } else resolved = false; // nothing next to search for

      } break;
      case '<' :                                                                 //200426
      {
         nat8 recursions = 255;
         std::wstring recursions_raw(*designator,1);
         if (!recursions_raw.empty())
            recursions = CORN::wstr_to_int16(recursions_raw.c_str(),10);
         CORN::OS::Directory_name_concrete CWD;
         found_name_qual = find_DEN
         (subitem_to_find
         ,unknown_entry // any type
         ,CWD
         ,File_system::superdirectory_recursion_inclusive
         ,recursions);
      } break;
      case '>' :                                                                 //200426
      {
         nat8 recursions = 255;
         std::wstring recursions_raw(*designator,1);
         if (!recursions_raw.empty())
            recursions = CORN::wstr_to_int16(recursions_raw.c_str(),10);
         CORN::OS::Directory_name_concrete CWD;
         found_name_qual = find_DEN
         (subitem_to_find
         ,unknown_entry // any type
         ,CWD
         ,File_system::subdirectory_recursion_inclusive
         ,recursions);
      } break;
   } // switch first_char

   if (!resolved && found_name_qual)                                             //180906
   {
      found_name_qual->append_path_list_to
         (resolved_path_items,CORN::OS::Directory_entry_name::include_qualification_designated);
         // found_name_qual will include the next
         // but because it is appended to resolved_path_items
         // we only include the path (designated because will be
         // fully qualified with actual filename which would be
         // drive designated under Windows)
      resolved = true;
      delete found_name_qual;
   }
   return resolved;
}
//_resolve_designator_______________________________________________2017-12-04_/
CORN::OS::Directory_name *File_system_engine_common::find_from_environment_variable
(const std::wstring &envvar,const std::wstring &subitem_to_find)      rendition_
{  // The environment variable may actually be a list of paths
   // separate by ; or : depending on the operating system
   std::wstring* envvar_paths = render_environment_variable_value(envvar);
      // raw list
   CORN::OS::Directory_name *found_name_qual = 0;
   if (envvar_paths)
   {
      // split the envvar_paths into list of (leafs)

      // if there is : then the conventional separator is as Linux
      // else it is Windows using ;
      char list_separator = get_envvar_list_separator();
         //2006       (':');
      size_t semicolon_pos = envvar_paths->find(L";");
      if (semicolon_pos!=std::string::npos) list_separator = ';';

      CORN::Text_list envvar_path_list_leafs(envvar_paths->c_str(),list_separator);
      FIND_FOR_EACH_IN
         (found_name_qual_in_leaf,envvar_dir
         ,CORN::Item_wstring,envvar_path_list_leafs,true,each_leaf)
      {
         CORN::OS::Directory_name_concrete leaf(*envvar_dir);
         found_name_qual
            = CORN::OS::file_system_engine.find_qualified_name_in_path
               (subitem_to_find,leaf); // name in envvar variable path list
         if (found_name_qual)
            found_name_qual_in_leaf = envvar_dir;
      } FOR_EACH_END(each_leaf)
   } // else envvar not specified in the systems environment variables so do nothing
   delete envvar_paths;
   return found_name_qual;
}
//_find_from_environment_variable___________________________________2018-09-06_/
bool File_system_engine_common::system_command_flags_source_destination
(const std::string &command
,const std::string &flags
,const Directory_entry_name &source_DEN
,const Directory_entry_name &destination_DEN)                              const
{
   std::string cmd(command);
   cmd += ' ';
   cmd.append(flags);
   cmd.append(" \"");
   source_DEN.append_to_string(cmd);
   cmd.append("\" \"");
   destination_DEN.append_to_string(cmd);
   cmd.append("\"");
   return system( cmd.c_str()) == EXIT_SUCCESS;
}
//_system_command_flags_source_destination__________________________2017-12-16_/
}}//_namespace_CORN_OS_________________________________________________________/

