#include "CS_suite/file_system/CS_databases.h"
#include "corn/OS/file_system_engine.h"
#include "corn/container/bilist.h"
namespace CS
{
//______________________________________________________________________________
Databases::Databases
(const CORN::OS::Directory_name &context_dir
,nat8 verbose_)
: verbose(verbose_)
{  // context_dir would typically be a scenario level,  but could be a project level
   const CORN::OS::Directory_name *potential_dir_containing_Database = 0;        //170627
   for (potential_dir_containing_Database = &context_dir                         //170627
       ;potential_dir_containing_Database
       ;potential_dir_containing_Database
         =  potential_dir_containing_Database->get_path_directory_name())
   {  CORN::OS::Directory_name_concrete *potential_Database
         = new CORN::OS::Directory_name_concrete
            (*potential_dir_containing_Database
            ,"Database"
            ,CORN::OS::directory_entry);
      if (CORN::OS::file_system_engine.exists(*potential_Database))
            append(new Database_directory(potential_Database));                  //170628
      else  delete potential_Database;
   }
   sort();
}
//_Databases:constructor____________________________________________2016-09-02_/
CORN::OS::Directory_entry_name *Databases::render_find_in_context_wstring
(const std::wstring &context_name
,const std::wstring &name
,const CORN::OS::Extension &extension)                                rendition_
{
   CORN::OS::File_name *selected_file = 0;
   FOR_EACH(DB_dir,Database_directory,each_DB)
   {  CORN::OS::Directory_name_concrete database_context_subdir(*DB_dir->name,context_name);
      CORN::OS::Directory_entry_name_concrete *potential_filename_qual
         = new CORN::OS::Directory_entry_name_concrete (database_context_subdir,name,extension);
      if (CORN::OS::file_system_engine.exists(*potential_filename_qual))
      {  delete selected_file;
         selected_file = potential_filename_qual;
      }
   } FOR_EACH_END(each_DB)
   return selected_file;
}
//_render_find_in_context_wstring___________________________________2016-09-02_/
CORN::OS::Directory_entry_name *Databases::render_find_in_context_string
(const std::wstring &context_name
,const std::string &name
,const CORN::OS::Extension &extension)                                rendition_
{  std::wstring name_wide;
   CORN::string_to_wstring(name,name_wide);
   return render_find_in_context_wstring(context_name,name_wide,extension);
}
//_render_find_in_context_string____________________________________2016-09-02_/
CORN::OS::Directory_entry_name *Databases::render_find_in_context_string_any_extension
(const std::wstring &context_name
,const std::string &file)                                             rendition_
{  CORN::OS::File_name *selected_file = 0;
   CORN::Bidirectional_list matching_DENs;
   FOR_EACH(DB_dir,Database_directory,each_DB)
   {  CORN::OS::Directory_name_concrete database_context_subdir
         (*DB_dir->name,context_name);
      std::string pattern(file); pattern.append(".*");
      CORN::Inclusion inclusion(pattern,true);
      CORN::OS::file_system_engine.list_DENs                                     //161013_160814
      (database_context_subdir
      ,&matching_DENs
      ,0
      ,inclusion
      ,CORN::OS::File_system::none_recursion_inclusive);
        // may need to be subdirectory_recursion_inclusive
   } FOR_EACH_END(each_DB)
   matching_DENs.sort(CORN::Container::DESCENDING);
   selected_file = dynamic_cast<CORN::OS::File_name *>(matching_DENs.pop_first());
      // this should be in the closest context
   return selected_file;
}
//_render_find_in_context_string_any_extension______________________2016-11-22_/
CORN::OS::Directory_entry_name *Databases::render_find_in_context_int
(const std::wstring &context_name
,int number
,const CORN::OS::Extension &extension)                                rendition_
{  std::wstring name_wide;
   CORN::append_int32_to_wstring(number,name_wide,10);
   return render_find_in_context_wstring(context_name,name_wide,extension);
}
//_render_find_in_context_int_______________________________________2016-09-02_/
CORN::OS::Directory_entry_name *Databases::render_find_under_context_wstring
(const std::wstring &context_name
,const std::wstring &undercontext_name
,const std::wstring &name
,const CORN::OS::Extension &extension)                                rendition_
{  CORN::OS::Directory_entry_name *selected_DEN = 0;
   FOR_EACH(DB_dir,Database_directory,each_DB)
   {  CORN::OS::Directory_name_concrete database_context_subdir
         (*DB_dir->name,context_name);
      // Note that the context might not necessarily be
      // in the level's Database directory.
      CORN::OS::Directory_name *undercontext_subdir =
         CORN::OS::file_system_engine.find_file_name
            (undercontext_name,database_context_subdir
            ,CORN::OS::File_system::subdirectory_recursion_inclusive,255,0,true);
      if (undercontext_subdir)
      {  CORN::OS::Directory_entry_name_concrete *potential_DEN
            = new CORN::OS::Directory_entry_name_concrete
               (*undercontext_subdir,name,extension);
         if (CORN::OS::file_system_engine.exists(*potential_DEN))
         {  delete selected_DEN;
            selected_DEN = potential_DEN;
         }
      }
      delete undercontext_subdir;
   } FOR_EACH_END(each_DB)
   return selected_DEN;
}
//_render_find_under_context_wstring________________________________2016-09_19_/
CORN::OS::Directory_entry_name *Databases::render_find_under_context_string
(const std::wstring &context_name
,const std::wstring &undercontext_name
,const std::string &name
,const CORN::OS::Extension &extension)                                rendition_
{  std::wstring name_wide;
   CORN::string_to_wstring(name,name_wide);
   return render_find_under_context_wstring
      (context_name,undercontext_name,name_wide,extension);
}
//_render_find_under_context_string_________________________________2016-09_19_/
CORN::OS::Directory_entry_name *Databases::render_find_under_context_int
(const std::wstring &context_name
,const std::wstring &undercontext_name
,int number
,const CORN::OS::Extension &extension)                                rendition_
{  std::wstring name_unqual_wide;
   CORN::append_int32_to_wstring(number,name_unqual_wide,10);
   return render_find_under_context_wstring
      (context_name,undercontext_name,name_unqual_wide,extension);
}
//_render_find_under_context_int____________________________________2016-09_19_/
CORN::OS::Directory_name *Databases::render_closest_context
(const std::wstring &context_name)                                    rendition_
{  CORN::OS::Directory_name *found = 0;
   FOR_EACH(database_dir,Database_directory,each_database)
   {  CORN::OS::Directory_name_concrete *context
        = new CORN::OS::Directory_name_concrete
            (database_dir->get_dir_DEN(),context_name,L"");
      if (CORN::OS::file_system_engine.exists(*context))
      {  if (found) delete found; // we found a closer directory
         found = context;
      }
      /* returns the context directory (I.e. Soil)
         scans the list of Databases and returns the
         Database context directory that exists closest to the
         project/scenario directory.
      */
   } FOR_EACH_END(each_database)
   return found;
}
//_render_closest_context___________________________________________2016-09-29_/
CORN::Unidirectional_list *Databases
::render_specialization_files_qualified_in_context_string
(const std::string &context_name)                                     rendition_
{  std::wstring context_name_wide;
   CORN::string_to_wstring(context_name,context_name_wide);
   return render_specialization_files_qualified_in_context_wstring(context_name_wide);
}
//_render_specialization_files_qualified_in_context_string__________2016-10-10_/
CORN::Unidirectional_list *Databases
::render_specialization_files_qualified_in_context_wstring
(const std::wstring &context_name)                                    rendition_
{  CORN::Bidirectional_list *specialization_files = new CORN::Bidirectional_list;
   // List of fully qualified filename
   FOR_EACH(database_dir,Database_directory,each_database)
   {  CORN::OS::Directory_name *context_in_this_database =
         CORN::OS::file_system_engine.find_qualified_name_in_path
            (context_name,database_dir->get_dir_DEN());
      if (context_in_this_database)
      {
      CORN::Unidirectional_list context_filenames_qual;
      CORN::OS::file_system_engine.list_DENs                                     //161013_160814
         (*context_in_this_database // context_dir_qual
         ,&context_filenames_qual
         ,0
         ,CORN::Inclusion("*.*",true)  // parameter file names will have an extension
         ,CORN::OS::File_system::none_recursion_inclusive);
      for (CORN::OS::File_name *context_filename
            = dynamic_cast<CORN::OS::File_name *>
               (context_filenames_qual.pop_first())
          ;context_filename
          ;context_filename = dynamic_cast<CORN::OS::File_name *>
            (context_filenames_qual.pop_first()))
      {  FIND_FOR_EACH_IN(generalization_filename_found,other_context_filename
            ,CORN::OS::File_name,*specialization_files,false,each_other)
         {  if (other_context_filename->get_name() == context_filename->get_name())
            {  generalization_filename_found = other_context_filename;
               each_other->delete_current();
               specialization_files->append(context_filename);
            }
         } FOR_EACH_END(each_other)
         if (!generalization_filename_found)
            specialization_files->append(context_filename);
      } // for
      }
   } FOR_EACH_END(each_database)
   specialization_files->sort();
   return specialization_files;
}
//_render_specialization_files_qualified_in_context_wstring_________2016-10-10_/
nat32 Databases::render_subdirectories
(CORN::Container       &subdirectories
,const CORN::Seclusion &seclusion
,CORN::OS::File_system::Recursion recursion)                          rendition_
{  FOR_EACH(database,Database_directory,each_database)
   {  // currently Databases doesn't have seclusion so we check the seclusion here.
      // probably would want to pass secusion to Databases constructor
      if (verbose>2) //discovery
         std::clog << "database: " << database->get_dir_DEN();
      if (seclusion.is_included_DEN(database->get_dir_DEN()))
      {  if (verbose>2) std::clog << " (included)" << std::endl;
            CORN::OS::file_system_engine.list_DENs(database->get_dir_DEN()
            ,0,&subdirectories,seclusion,recursion);                             //200426
      }
      else if (verbose>2) std::clog << " (excluded)" << std::endl;
   } FOR_EACH_END(each_database)
   if (verbose>3)   // discovery
   {  std::clog << "info: database subdirs=" << std::endl;
      subdirectories.write(std::clog);
   }
   return subdirectories.count();
   // Returns the number of included subdirectories from all databases
}
//_render_subdirectories____________________________________________2016-10-31_/
nat32 Databases::render_subdirectories_with_name
(CORN::Container        &subdirectories
,const std::string      &name)                                        rendition_
{  nat32 database_subdir_count = 0;
   FOR_EACH(database,Database_directory,each_database)
   {
      // currently Databases doesn't have seclusion so we check the seclusion here.
      // probably would want to pass secusion to Databases constructor
      if (verbose) // recursion
         std::clog << "database :" << database->get_dir_DEN();
         //if (verbose) std::clog << " (included)" << std::endl;
         database_subdir_count += CORN::OS::file_system_engine.list_DENs
            (database->get_dir_DEN(),0,&subdirectories,CORN::Inclusion(name,true)
            ,CORN::OS::File_system::subdirectory_recursion_inclusive);           //200426
   } FOR_EACH_END(each_database)
   return database_subdir_count; // Returns the number of included subdirectories from all databases
}
//_render_subdirectories_with_name__________________________________2016-11-01_/
}//_namespace_CS_______________________________________________________________/

