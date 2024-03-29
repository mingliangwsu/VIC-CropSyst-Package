#ifdef __unix__
#include "corn/OS/file_system_engine_Unix_and_Linux.h"
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <fstream>
#include <sys/types.h>
#include <pwd.h>
#include <stdlib.h>
#include "corn/OS/directory_entry_name_concrete.h"
#include "corn/container/text_list.h"
#include "corn/OS/CSIDL.h"
#include "corn/string/strconv.hpp"
#include "corn/seclusion.h"
#ifdef __GXX_EXPERIMENTAL_CXX0X__x
#include <regex>
// Note that regex requires -std=c++0x
#endif


// This file is specifically for Unix or Linux
// It is not necessary to include this file if not targeting Unix or Linux.

namespace CORN { namespace OS {
//______________________________________________________________________________
bool dir_op_valid()
{  bool op_valid = true;
   switch (errno)
   {  case EACCES :       case EBADF :
      case EFAULT :       case ELOOP  :
      case EMFILE :       case ENAMETOOLONG :
      case ENFILE :       case ENOENT :
      case ENOMEM :       case ENOTDIR :
         op_valid = false;
   }
   return op_valid;
}
//_dir_op_valid_____________________________________________________2000-11-13_/
File_system_engine_Unix::File_system_engine_Unix()
{
//   std::clog << "File_system_engine_Unix constructor" <<std::endl;
}
//_constructor_________________________________________________________________/
bool File_system_engine_Unix::create_directory
(const OS::Directory_name &directory_name)                                 const
{
   bool created = mkdir(directory_name.c_str(), S_IRWXU|S_IRWXG|S_IRWXO) == 0;
   return created;
}
//_create_directory_________________________________________________2012-12-22_/
datetime64 File_system_engine_Unix::get_modification_date_time
(const Directory_entry_name &directory_entry_name)                         const
{  datetime64 result = 0;                                                        //030717
   assert(false); //            Needs to be implemented for UNIX but I can't find the UNIX equivelent of  getftime!
   return result;
}
//_get_modification_date_time_______________________________________1998-01-29_/
bool File_system_engine_Unix::
create_directory_link
(const Directory_name &directory_name
,const Directory_name &link_name
,bool symbolic)                                                            const
{  const char *dir_name_cstr  = directory_name.c_str();;
   const char *link_name_cstr = link_name.c_str();
   return symbolic
      ? symlink(dir_name_cstr,link_name_cstr)
      : link(dir_name_cstr,link_name_cstr)
      == EXIT_SUCCESS;
}
//_create_directory_link____________________________________________2014-10-22_/
      ///< Create a link to a directory
      ///\return the created directory element or null if the OS was unable to create the directory.
bool File_system_engine_Unix::
create_file_link
(const File_name &file_name
,const File_name &link_name
,bool symbolic)                                                            const
{  const char *file_name_cstr  = file_name.c_str();;
   const char *link_name_cstr = link_name.c_str();
   return symbolic
      ? symlink(file_name_cstr,link_name_cstr)
      : link   (file_name_cstr,link_name_cstr)
      == EXIT_SUCCESS;
   // Note linking to a directory is restricted to the superuser
   // So this will fail for standard users.
}
//_create_file_link_________________________________________________2014-10-22_/
#ifndef _QT_
nat32 File_system_engine_Unix::list_names_in
(const Directory_name & directory_name
,CORN::Text_list *file_name_list_unqual
,CORN::Text_list *directory_name_list_unqual)                              const
{
   std::wstring path_wildcard(directory_name.w_str());
   path_wildcard.append(L"/*");
   DIR *dir = opendir(directory_name.c_str());
   if (dir)
   {  class dirent *ent = 0;
      while ((ent = readdir(dir)) != NULL)
      {  // Currently Linux only has ASCII file names.
         const char *DEN_unqual_cstr = ent->d_name;
         if (DEN_unqual_cstr && DEN_unqual_cstr[0] != '.') // exclude  . and .. directories
         {  CORN::OS::Directory_entry_name_concrete DEN_qual
               (directory_name,DEN_unqual_cstr);
            // load the file status
            class stat st; int status = stat(DEN_qual.c_str(), &st);
            bool is_directory = (st.st_mode & S_IFDIR) != 0;
            if (status != -1)
            {  if (is_directory && directory_name_list_unqual)
                     directory_name_list_unqual->add_cstr(DEN_unqual_cstr);
               else // the entry is a file
                  if (file_name_list_unqual)                                     //160219
                     file_name_list_unqual->add_cstr(DEN_unqual_cstr);
            } // status
            else
            { // NYI any other hidden directories inclusion/exclusion
              // should be optional, currently they are excluded.
              // Normally such directories explicitly composed
              // by the applications that create/use them.
              // However, excluding such directories makes this
              // class currently unsuitable for general Unix system
              // directory management utilities.
            }
         } // if . ..
      } // while reading entries
      closedir(dir);
   } // dir openned
   return file_name_list_unqual      ? file_name_list_unqual->count()      :0
        + directory_name_list_unqual ? directory_name_list_unqual->count() : 0;
}
//_list_names_in____________________________________________________2016-10-29_/
nat32 File_system_engine_Unix::list_entry_names_in_deprecated
(const Directory_name & directory_name
,CORN::Text_list *file_name_list_unqual
,CORN::Text_list *directory_name_list_unqual
,const wchar_t *name_wild_card
,const wchar_t *extension_filter
,const CORN::Text_list *exclude_directories)                               const
{
   if ((name_wild_card != 0) && wcsstr(name_wild_card,L"*"))
      assert(false); // filtering with wild cards not yet implemented
   if ((extension_filter != 0) && wcsstr(extension_filter,L"*"))
      assert(false); // filtering not yet implemented
   std::wstring path_wildcard(directory_name.w_str());
   path_wildcard.append(L"/");
   path_wildcard.append(name_wild_card   ? name_wild_card   : L"*");
   if (extension_filter)
   {  path_wildcard.append(L".");
      path_wildcard.append(extension_filter ? extension_filter : L"*");
   }
   DIR *dir = opendir(directory_name.c_str());
   if (dir)
   {  class dirent *ent = 0;
      while ((ent = readdir(dir)) != NULL)
      {  // Currently Linux only has ASCII file names.
         const char *DEN_unqual_cstr = ent->d_name;
         bool include_DEN = true;
         // Check if name (either file or director)  matches
         CORN::OS::Directory_entry_name_concrete DEN_qual
               (directory_name,DEN_unqual_cstr);
         if (name_wild_card)
         {
            #ifdef __GXX_EXPERIMENTAL_CXX0X__x
            std::wregex rx(name_wild_card);
            //continue here we want to check only the name component
            std::wstring DEN_qual_wstring;
            CORN::append_ASCIIZ_to_wstring(DEN_unqual_cstr,DEN_qual_wstring);
            include_DEN &= regex_search
               (DEN_qual_wstring.begin(),DEN_qual_wstring.end(),rx);
            #else
             std::string name_wild_card_str;
             CORN::wstring_to_string(name_wild_card,name_wild_card_str);
            // warning without regex, we can only compare exact match substring
            std::string DEN_unqual(DEN_unqual_cstr);
            include_DEN &= DEN_unqual.find(name_wild_card_str) != std::string::npos;
            #endif
         }
         if (extension_filter && include_DEN)
         {
            #ifdef __GXX_EXPERIMENTAL_CXX0X__x
                    //NYI use regex to check the extension
            #else
                include_DEN &= DEN_qual.has_extension_Unicode(extension_filter,false);
            #endif
         }
         if (include_DEN) // So far file name matches
         {
            // load the file status
            class stat st; int status = stat(DEN_qual.c_str(), &st);
            bool is_directory = (st.st_mode & S_IFDIR) != 0;
            if (is_directory)
            {
               if (directory_name_list_unqual)
               {  // caller wants list of directory names
                  if (( DEN_unqual_cstr[0] != '.') && (status != -1))
                  {  bool exclude_this_directory = exclude_directories
                        && exclude_directories->find_cstr(DEN_unqual_cstr);
                     if (!exclude_this_directory)
                        directory_name_list_unqual->add_cstr(DEN_unqual_cstr);
                  } // else exclude  . and .. directories
                   // NYI any other hidden directories inclusion/exclusion
                   // should be optional, currently they are excluded.
                   // Normally such directories explicitly composed
                   // by the applications that create/use them.
                   // However, excluding such directories makes this
                   // class currently unsuitable for general Unix system
                   // directory management utilities.
               }
            }
            else // normal file
            {
               if (file_name_list_unqual&& DEN_unqual_cstr)                      //160219
                  file_name_list_unqual->add_cstr(DEN_unqual_cstr);
            }
         }
      }
      closedir(dir);
   }
   return file_name_list_unqual      ? file_name_list_unqual->count()      :0
        + directory_name_list_unqual ? directory_name_list_unqual->count() : 0;
}
#endif
//_list_entry_names_in______________________________________________2015-12-15_/
/* obsolete, these would be implemented in Shell_graphical
bool File_system_engine_Unix::browse_directory
(const Directory_name &dir_name
,bool open_new_window
,bool show_directory_tree_view
,bool show_as_apparent_root
,bool select)                                                              const
{
   assert(false);
}
//_browse_directory____________________________________________________________/
      ///< Declared in and documented File_system
bool File_system_engine_Unix::edit_file(const File_name &file_name, HANDLE_OR_ERROR &handle_or_error)        const
{
   assert(false);
}
//_edit_file___________________________________________________________________/
      ///< Edit the file with a suitable  associated program using ShellExecute
bool File_system_engine_Unix::view_file  (const File_name &file_name, HANDLE_OR_ERROR &handle_or_error)      const
{
   assert(false);
}
//_view_file___________________________________________________________________/
bool File_system_engine_Unix::print_file (const File_name &file_name, HANDLE_OR_ERROR &handle_or_error)      const
{
   assert(false);
}
//____________________________________________________________________________/
bool File_system_engine_Unix::open_file  (const File_name &file_name, HANDLE_OR_ERROR &handle_or_error)      const
{
   assert(false);
}
//____________________________________________________________________________/
*/
bool File_system_engine_Unix::copy_file
(const File_name &source, const File_name &destination
,bool fail_if_exists)                                                      const
{  if (fail_if_exists && exists(destination)) return false;
   std::string cmd ("/bin/cp -p \'");
   cmd.append(source.c_str());
   cmd.append("\' \'");
   cmd.append(destination.c_str());
   cmd.append("\'");
   return system( cmd.c_str()) == EXIT_SUCCESS;
}
//_copy_file________________________________________________________2012-02-09_/
bool File_system_engine_Unix::copy_directory
(const Directory_name &source, const Directory_name &destination
,bool fail_if_exists)                                                      const
{  if (fail_if_exists && exists(destination)) return false;
   std::string cmd ("/bin/cp -R -p \'");
   cmd.append(source.c_str());
   cmd.append("\' \'");
   cmd.append(destination.c_str());
   cmd.append("\'");
   return system( cmd.c_str()) == EXIT_SUCCESS;
}
//_copy_directory___________________________________________________2013-05-05_/
bool File_system_engine_Unix::move
(const Directory_entry_name &source_DEN
,const Directory_entry_name &destination_DEN
,bool force
,bool interactive
,bool update
,bool verbose)                                                             const
{
   /*
   Using system command rather than function because
   I don't know of such a Linux API function
   see
   https://www.rapidtables.com/code/linux/mv.html
   https://www.computerhope.com/unix/umv.htm
   https://en.wikipedia.org/wiki/Mv
   */

   std::string flags;
   if (force)        flags.append(" -f ");
   if (interactive)  flags.append(" -i ");
   if (update)       flags.append(" -u ");
   if (verbose)      flags.append(" -v ");
   return system_command_flags_source_destination
      ("/bin/mv",flags,source_DEN,destination_DEN);
}
//_move_____________________________________________________________2017-12-15_/


   /*conceptual
bool File_system_engine_Unix::are_in_same_file_system
      (const Directory_entry_name &DEN1
      ,const Directory_entry_name &DEN2)                            affirmation
{
      // Returns true if both Directory entries are on the same file system
   use stat on each file
   if the the major and minor partions are the same then its the same
   file system.

   see: https://www.linuxquestions.org/questions/linux-general-1/test-if-two-files-are-on-the-same-file-system-584241/
}
*/
//_are_in_same_file_system__________________________________________2017-12-15_/
bool File_system_engine_Unix::recycle
(const Directory_entry_name  &entry_name
,bool delete_forever)                                                      const
{  if (delete_forever)
         remove(entry_name.c_str());
   else
   {  // Moving files to trashbin is a function of the user's desktop

      // This is not currently implemented so simply deleting the file

      remove(entry_name.c_str());

      /* Solaris if running CDE does have a Trashbin
      Not sure about GNOME etc...
      Desktop environments that subscribe to the
      The FreeDesktop.org Trash specification
      http://www.ramendik.ru/docs/trashspec.html
      Will move recylcled files to
       $XDG_DATA_HOME/Trash

      if ($XDG_DATA_HOME is defined)


      The user may have install Trash_cli (http://www.gnutoolbox.com/trashcli/)
      In which case we can call
      /usr/bin/trash-put

      */
   }
      ///< The file is not deleted unless delete_forever is true;
      ///<\return true if the file was successfully recycled, false if it failed (nothing happened).

   return false;
}
//_recycle__________________________________________________________2002-12-07_/
bool File_system_engine_Unix::exists(const Directory_entry_name &directory_entry_name)   const
{  bool dir_entry_exists =
      access(directory_entry_name.c_str(), F_OK) == 0;
   // Note that access may not be available in all OS's (works for Windows and POSIX)
   return dir_entry_exists;
}
//_exists______________________________________________________________________/
bool File_system_engine_Unix::set_current_working_directory(const Directory_name &dir_name) const
{  return chdir(dir_name.c_str()) == 0;
}
//_set_current_working_directory____________________________________2012-02-17_/
enum Linux_Desktop
{ none_desktop
, KDE_desktop
, gnome_desktop
};
//_2012-03-19___________________________________________________________________
Directory_name *File_system_engine_Unix::render_CSIDL_directory_name(nat16 CSIDL) const
{
   char *CSIDL_dir_cstr = 0;
   Linux_Desktop desktop = KDE_desktop;
/* http://www.freedesktop.org/wiki/
*/

/*http://standards.freedesktop.org/basedir-spec/latest/

 XDG Base Directory Specification is based on the following concepts:

There is a single base directory relative to which user-specific data files should be written. This directory is defined by the environment variable $XDG_DATA_HOME.
There is a single base directory relative to which user-specific configuration files should be written. This directory is defined by the environment variable $XDG_CONFIG_HOME.
There is a set of preference ordered base directories relative to which data files should be searched. This set of directories is defined by the environment variable $XDG_DATA_DIRS.
There is a set of preference ordered base directories relative to which configuration files should be searched. This set of directories is defined by the environment variable $XDG_CONFIG_DIRS.
There is a single base directory relative to which user-specific non-essential (cached) data should be written. This directory is defined by the environment variable $XDG_CACHE_HOME.
There is a single base directory relative to which user-specific runtime files and other file objects should be placed. This directory is defined by the environment variable $XDG_RUNTIME_DIR.
*/
   switch (CSIDL)
   {
//NYI      case CSIDL_DESKTOP  : CSIDL_dir_cstr = "xxxxx"; break;    // <desktop>

//NYI      case xxxxxx  : CSIDL_dir_cstr = "xxxxx"; break;#define CSIDL_INTERNET                  0x0001        // Internet Explorer (icon on desktop)
//NYI      case xxxxxx  : CSIDL_dir_cstr = "xxxxx"; break;#define CSIDL_PROGRAMS                  0x0002        // Start Menu\Programs
//NYI      case xxxxxx  : CSIDL_dir_cstr = "xxxxx"; break;#define CSIDL_CONTROLS                  0x0003        // My Computer\Control Panel
//NYI      case xxxxxx  : CSIDL_dir_cstr = "xxxxx"; break;#define CSIDL_PRINTERS                  0x0004        // My Computer\Printers
      case CSIDL_PERSONAL  : CSIDL_dir_cstr = "~/Documents"; break;    //  Alias My Documents
//NYI      case xxxxxx  : CSIDL_dir_cstr = "xxxxx"; break;#define CSIDL_FAVORITES                 0x0006        // <user name>\Favorites
      case CSIDL_STARTUP  :   // Start Menu\Programs\Startup
         // In Linux autostart program are by user and specific to the Desktop environment
         switch (desktop)
         {
            case KDE_desktop : CSIDL_dir_cstr = "~/.kde/Autostart"; break;
         }
//NYI      case xxxxxx  : CSIDL_dir_cstr = "xxxxx"; break;#define CSIDL_RECENT                    0x0008        // <user name>\Recent
//NYI      case xxxxxx  : CSIDL_dir_cstr = "xxxxx"; break;#define CSIDL_SENDTO                    0x0009        // <user name>\SendTo
      case CSIDL_BITBUCKET  : CSIDL_dir_cstr = "~/.local/share/Trash"; break;   // <desktop>\Recycle Bin
      case CSIDL_STARTMENU  :
       CSIDL_dir_cstr = "xxxxx"; break;//#define                  0x000b        // <user name>\Start Menu
         // In Linux start menu program are by user and specific to the Desktop environment
         switch (desktop)
         {
            case KDE_desktop : CSIDL_dir_cstr = "~/.kde/share/apps"; break;
         }
      // case CSIDL_MYDOCUMENTS        : CSIDL_dir_cstr = "~/Documents";      break;
      case CSIDL_MYMUSIC            : CSIDL_dir_cstr = "~/Music";          break;
      case CSIDL_MYVIDEO            : CSIDL_dir_cstr = "~/Videos";         break;
      case CSIDL_DESKTOPDIRECTORY   : CSIDL_dir_cstr = "~/Desktop";        break;
      case CSIDL_DRIVES             : CSIDL_dir_cstr = "/media";           break;
         //Unix doesn't actually have drives as in Windows, however
         //Mount points for removable media such as CD-ROMs (appeared in FHS-2.3).

//NYI      case xxxxxx  : CSIDL_dir_cstr = "xxxxx"; break;#define CSIDL_NETWORK                   0x0012        // Network Neighborhood (My Network Places)
//NYI      case xxxxxx  : CSIDL_dir_cstr = "xxxxx"; break;#define CSIDL_NETHOOD                   0x0013        // <user name>\nethood
//NYI      case xxxxxx  : CSIDL_dir_cstr = "xxxxx"; break;#define CSIDL_FONTS                     0x0014        // windows\fonts
//NYI      case xxxxxx  : CSIDL_dir_cstr = "xxxxx"; break;#define CSIDL_TEMPLATES                 0x0015
//NYI      case xxxxxx  : CSIDL_dir_cstr = "xxxxx"; break;#define CSIDL_COMMON_STARTMENU          0x0016        // All Users\Start Menu

//NYI      case xxxxxx  : CSIDL_dir_cstr = "xxxxx"; break;#define CSIDL_COMMON_PROGRAMS           0X0017        // All Users\Start Menu\Programs
//NYI      case xxxxxx  : CSIDL_dir_cstr = "xxxxx"; break;#define CSIDL_COMMON_STARTUP            0x0018        // All Users\Startup
//NYI      case xxxxxx  : CSIDL_dir_cstr = "xxxxx"; break;#define CSIDL_COMMON_DESKTOPDIRECTORY   0x0019        // All Users\Desktop
//NYI      case xxxxxx  : CSIDL_dir_cstr = "xxxxx"; break;#define CSIDL_APPDATA                   0x001a        // <user name>\Application Data
//NYI      case xxxxxx  : CSIDL_dir_cstr = "xxxxx"; break;#define CSIDL_PRINTHOOD                 0x001b        // <user name>\PrintHood
      case CSIDL_LOCAL_APPDATA  : CSIDL_dir_cstr = "~/.local"; break;      // <user name>\Local Settings\Applicaiton Data (non roaming)
//NYI      case xxxxxx  : CSIDL_dir_cstr = "xxxxx"; break;#define CSIDL_ALTSTARTUP                0x001d        // non localized startup
//NYI      case xxxxxx  : CSIDL_dir_cstr = "xxxxx"; break;#define CSIDL_COMMON_ALTSTARTUP         0x001e        // non localized common startup
//NYI      case xxxxxx  : CSIDL_dir_cstr = "xxxxx"; break;#define CSIDL_COMMON_FAVORITES          0x001f
//NYI      case xxxxxx  : CSIDL_dir_cstr = "xxxxx"; break;#define CSIDL_INTERNET_CACHE            0x0020
//NYI      case xxxxxx  : CSIDL_dir_cstr = "xxxxx"; break;#define CSIDL_COOKIES                   0x0021
//NYI      case xxxxxx  : CSIDL_dir_cstr = "xxxxx"; break;#define CSIDL_HISTORY                   0x0022
//NYI      case xxxxxx  : CSIDL_dir_cstr = "xxxxx"; break;#define CSIDL_COMMON_APPDATA            0x0023        // All Users\Application Data
//NYI      case xxxxxx  : CSIDL_dir_cstr = "xxxxx"; break;#define CSIDL_WINDOWS        ls -la            0x0024        // GetWindowsDirectory()
//NYI      case xxxxxx  : CSIDL_dir_cstr = "xxxxx"; break;#define CSIDL_SYSTEM                    0x0025        // GetSystemDirectory()
//NYI      case xxxxxx  : CSIDL_dir_cstr = "xxxxx"; break;#define CSIDL_PROGRAM_FILES             0x0026        // C:\Program Files
      case CSIDL_MYPICTURES  : CSIDL_dir_cstr = "~/Pictures"; break;
//NYI      case xxxxxx  : CSIDL_dir_cstr = "xxxxx"; break;#define CSIDL_PROFILE                   0x0028        // USERPROFILE
//NYI      case xxxxxx  : CSIDL_dir_cstr = "xxxxx"; break;#define CSIDL_SYSTEMX86                 0x0029        // x86 system directory on RISC
//NYI      case xxxxxx  : CSIDL_dir_cstr = "xxxxx"; break;#define CSIDL_PROGRAM_FILESX86          0x002a        // x86 C:\Program Files on RISC
//NYI      case xxxxxx  : CSIDL_dir_cstr = "xxxxx"; break;#define CSIDL_PROGRAM_FILES_COMMON      0x002b        // C:\Program Files\Common
//NYI      case xxxxxx  : CSIDL_dir_cstr = "xxxxx"; break;#define CSIDL_PROGRAM_FILES_COMMONX86   0x002c        // x86 Program Files\Common on RISC
//NYI      case xxxxxx  : CSIDL_dir_cstr = "xxxxx"; break;#define CSIDL_COMMON_TEMPLATES          0x002d        // All Users\Templates
//NYI      case xxxxxx  : CSIDL_dir_cstr = "xxxxx"; break;#define CSIDL_COMMON_DOCUMENTS          0x002e        // All Users\Documents
//NYI      case xxxxxx  : CSIDL_dir_cstr = "xxxxx"; break;#define CSIDL_COMMON_ADMINTOOLS         0x002f        // All Users\Start Menu\Programs\Administrative Tools
//NYI      case xxxxxx  : CSIDL_dir_cstr = "xxxxx"; break;#define CSIDL_ADMINTOOLS                0x0030        // <user name>\Start Menu\Programs\Administrative Tools
//NYI      case xxxxxx  : CSIDL_dir_cstr = "xxxxx"; break;#define CSIDL_CONNECTIONS               0x0031        // Network and Dial-up Connections
//NYI      case xxxxxx  : CSIDL_dir_cstr = "xxxxx"; break;#define CSIDL_COMMON_MUSIC              0x0035        // All Users\My Music
//NYI      case xxxxxx  : CSIDL_dir_cstr = "xxxxx"; break;#define CSIDL_COMMON_PICTURES           0x0036        // All Users\My Pictures  //Version 6.0. The file system directory that serves as a repository for image files common to all users. A typical path is C:\Documents and Settings\All Users\Documents\My Pictures.
//NYI      case xxxxxx  : CSIDL_dir_cstr = "xxxxx"; break;#define CSIDL_COMMON_VIDEO              0x0037        // All Users\My Video
//NYI      case xxxxxx  : CSIDL_dir_cstr = "xxxxx"; break;#define CSIDL_RESOURCES                 0x0038        // Resource Direcotry
//NYI      case xxxxxx  : CSIDL_dir_cstr = "xxxxx"; break;#define CSIDL_RESOURCES_LOCALIZED       0x0039        // Localized Resource Direcotry
//NYI      case xxxxxx  : CSIDL_dir_cstr = "xxxxx"; break;#define CSIDL_COMMON_OEM_LINKS          0x003a        // Links to All Users OEM specific apps   This value is recognized in Windows Vista for backward compatibility, but the folder itself is no longer used.
//NYI      case xxxxxx  : CSIDL_dir_cstr = "xxxxx"; break;#define CSIDL_CDBURN_AREA               0x003b        // USERPROFILE\Local Settings\Application Data\Microsoft\CD Burning
// unused                               0x003c
//NYI      case xxxxxx  : CSIDL_dir_cstr = "xxxxx"; break;#define CSIDL_COMPUTERSNEARME           0x003d        // Computers Near Me (computered from Workgroup membership)

/*            for see http://en.wikipedia.org/wiki/Filesystem_Hierarchy_Standard

/	Primary hierarchy root and root directory of the entire file system hierarchy.

/bin
Essential command binaries that need to be available in single user mode; for all users, e.g., cat, ls, cp.
/boot
Boot loader files, e.g., kernels, initrd; often a separate partition[23]
/dev
Essential devices, e.g., /dev/null.
/etc
Host-specific system-wide configuration files
There has been controversy over the meaning of the name itself. In early versions of the UNIX Implementation Document from Bell labs, /etc is referred to as the etcetera directory,[24] as this directory historically held everything that did not belong elsewhere (however, the FHS restricts /etc to static configuration files and may not contain binaries).[25] Since the publication of early documentation, the directory name has been re-designated in various ways. Recent interpretations include Backronyms such as "Editable Text Configuration" or "Extended Tool Chest".[26]
/etc/opt
Configuration files for /opt/.
/etc/X11
Configuration files for the X Window System, version 11.
/etc/sgml
Configuration files for SGML.
/etc/xml
Configuration files for XML.
/home
Users' home directories, containing saved files, personal settings, etc.; often a separate partition.
/lib
Libraries essential for the binaries in /bin/ and /sbin/.
/mnt
Temporarily mounted filesystems.
/opt
Optional application software packages.[27]
/proc
Virtual filesystem documenting kernel and process status as text files, e.g., uptime, network. In Linux, corresponds to a Procfs mount.
/root
Home directory for the root user.
/sbin
Essential system binaries, e.g., init, ip, mount.
/srv
Site-specific data which are served by the system.
/tmp
Temporary files (see also /var/tmp). Often not preserved between system reboots.
/usr
Secondary hierarchy for read-only user data; contains the majority of (multi-)user utilities and applications.[28]
/usr/bin
Non-essential command binaries (not needed in single user mode); for all users.
/usr/include
Standard include files.
/usr/lib
Libraries for the binaries in /usr/bin/ and /usr/sbin/.
/usr/sbin
Non-essential system binaries, e.g., daemons for various network-services.
/usr/share
Architecture-independent (shared) data.
/usr/src
Source code, e.g., the kernel source code with its header files.
/usr/X11R6
X Window System, Version 11, Release 6.
/usr/local
Tertiary hierarchy for local data, specific to this host. Typically has further subdirectories, e.g., bin/, lib/, share/.[29]
/var
Variable filesfiles whose content is expected to continually change during normal operation of the systemsuch as logs, spool files, and temporary e-mail files. Sometimes a separate partition.
/var/cache
Application cache data. Such data are locally generated as a result of time-consuming I/O or calculation. The application must be able to regenerate or restore the data. The cached files can be deleted without loss of data.
/var/lib
State information. Persistent data modified by programs as they run, e.g., databases, packaging system metadata, etc.
/var/lock
Lock files. Files keeping track of resources currently in use.
/var/log
Log files. Various logs.
/var/mail
Users' mailboxes.
/var/run
Information about the running system since last boot, e.g., currently logged-in users and running daemons.
/var/spool
Spool for tasks waiting to be processed, e.g., print queues and unread mail.
/var/spool/mail
Deprecated location for users' mailboxes.
/var/tmp
Temporary files to be preserved between reboots.
[edit]



CSIDL_ADMINTOOLS

FOLDERID_AdminTools
Version 5.0. The file system directory that is used to store administrative tools for an individual user. The MMC will save customized consoles to this directory, and it will roam with the user.
CSIDL_ALTSTARTUP
FOLDERID_Startup
The file system directory that corresponds to the user's nonlocalized Startup program group. This value is recognized in Windows Vista for backward compatibility, but the folder itself no longer exists.
CSIDL_APPDATA
FOLDERID_RoamingAppData
Version 4.71. The file system directory that serves as a common repository for application-specific data. A typical path is C:\Documents and Settings\username\Application Data. This CSIDL is supported by the redistributable Shfolder.dll for systems that do not have the Internet Explorer 4.0 integrated Shell installed.
CSIDL_BITBUCKET
FOLDERID_RecycleBinFolder
The virtual folder that contains the objects in the user's Recycle Bin.
CSIDL_CDBURN_AREA
FOLDERID_CDBurning
Version 6.0. The file system directory that acts as a staging area for files waiting to be written to a CD. A typical path is C:\Documents and Settings\username\Local Settings\Application Data\Microsoft\CD Burning.
CSIDL_COMMON_ADMINTOOLS
FOLDERID_CommonAdminTools
Version 5.0. The file system directory that contains administrative tools for all users of the computer.
CSIDL_COMMON_ALTSTARTUP
FOLDERID_CommonStartup
The file system directory that corresponds to the nonlocalized Startup program group for all users. This value is recognized in Windows Vista for backward compatibility, but the folder itself no longer exists.
CSIDL_COMMON_APPDATA
FOLDERID_ProgramData
Version 5.0. The file system directory that contains application data for all users. A typical path is C:\Documents and Settings\All Users\Application Data. This folder is used for application data that is not user specific. For example, an application can store a spell-check dictionary, a database of clip art, or a log file in the CSIDL_COMMON_APPDATA folder. This information will not roam and is available to anyone using the computer.
CSIDL_COMMON_DESKTOPDIRECTORY
FOLDERID_PublicDesktop
The file system directory that contains files and folders that appear on the desktop for all users. A typical path is C:\Documents and Settings\All Users\Desktop.
CSIDL_COMMON_DOCUMENTS
FOLDERID_PublicDocuments
The file system directory that contains documents that are common to all users. A typical path is C:\Documents and Settings\All Users\Documents.
CSIDL_COMMON_FAVORITES
FOLDERID_Favorites
The file system directory that serves as a common repository for favorite items common to all users.
CSIDL_COMMON_MUSIC
FOLDERID_PublicMusic
Version 6.0. The file system directory that serves as a repository for music files common to all users. A typical path is C:\Documents and Settings\All Users\Documents\My Music.

CSIDL_COMMON_PICTURES
FOLDERID_PublicPictures
Version 6.0. The file system directory that serves as a repository for image files common to all users. A typical path is C:\Documents and Settings\All Users\Documents\My Pictures.
CSIDL_COMMON_PROGRAMS
FOLDERID_CommonPrograms
The file system directory that contains the directories for the common program groups that appear on the Start menu for all users. A typical path is C:\Documents and Settings\All Users\Start Menu\Programs.
CSIDL_COMMON_STARTMENU
FOLDERID_CommonStartMenu
The file system directory that contains the programs and folders that appear on the Start menu for all users. A typical path is C:\Documents and Settings\All Users\Start Menu.
CSIDL_COMMON_STARTUP
FOLDERID_CommonStartup
The file system directory that contains the programs that appear in the Startup folder for all users. A typical path is C:\Documents and Settings\All Users\Start Menu\Programs\Startup.
CSIDL_COMMON_TEMPLATES
FOLDERID_CommonTemplates
The file system directory that contains the templates that are available to all users. A typical path is C:\Documents and Settings\All Users\Templates.
CSIDL_COMMON_VIDEO
FOLDERID_PublicVideos
Version 6.0. The file system directory that serves as a repository for video files common to all users. A typical path is C:\Documents and Settings\All Users\Documents\My Videos.
CSIDL_COMPUTERSNEARME
FOLDERID_NetworkFolder
The folder that represents other computers in your workgroup.
CSIDL_CONNECTIONS
FOLDERID_ConnectionsFolder
The virtual folder that represents Network Connections, that contains network and dial-up connections.
CSIDL_CONTROLS
FOLDERID_ControlPanelFolder
The virtual folder that contains icons for the Control Panel applications.
CSIDL_COOKIES
FOLDERID_Cookies
The file system directory that serves as a common repository for Internet cookies. A typical path is C:\Documents and Settings\username\Cookies.
CSIDL_DESKTOP
FOLDERID_Desktop
The virtual folder that represents the Windows desktop, the root of the namespace.
CSIDL_DESKTOPDIRECTORY
FOLDERID_Desktop
The file system directory used to physically store file objects on the desktop (not to be confused with the desktop folder itself). A typical path is C:\Documents and Settings\username\Desktop.
CSIDL_DRIVES
FOLDERID_ComputerFolder
The virtual folder that represents My Computer, containing everything on the local computer: storage devices, printers, and Control Panel. The folder can also contain mapped network drives.
CSIDL_FAVORITES
FOLDERID_Favorites
The file system directory that serves as a common repository for the user's favorite items. A typical path is C:\Documents and Settings\username\Favorites.
CSIDL_FONTS
FOLDERID_Fonts
A virtual folder that contains fonts. A typical path is C:\Windows\Fonts.
CSIDL_HISTORY
FOLDERID_History
The file system directory that serves as a common repository for Internet history items.
CSIDL_INTERNET
FOLDERID_InternetFolder
A virtual folder for Internet Explorer.
CSIDL_INTERNET_CACHE
FOLDERID_InternetCache
Version 4.72. The file system directory that serves as a common repository for temporary Internet files. A typical path is C:\Documents and Settings\username\Local Settings\Temporary Internet Files.
CSIDL_LOCAL_APPDATA
FOLDERID_LocalAppData
Version 5.0. The file system directory that serves as a data repository for local (nonroaming) applications. A typical path is C:\Documents and Settings\username\Local Settings\Application Data.
CSIDL_MYDOCUMENTS
FOLDERID_Documents
Version 6.0. The virtual folder that represents the My Documents desktop item. This value is equivalent to CSIDL_PERSONAL.
CSIDL_MYMUSIC
FOLDERID_Music
The file system directory that serves as a common repository for music files. A typical path is C:\Documents and Settings\User\My Documents\My Music.
CSIDL_MYPICTURES
FOLDERID_Pictures
Version 5.0. The file system directory that serves as a common repository for image files. A typical path is C:\Documents and Settings\username\My Documents\My Pictures.
CSIDL_MYVIDEO
FOLDERID_Videos
Version 6.0. The file system directory that serves as a common repository for video files. A typical path is C:\Documents and Settings\username\My Documents\My Videos.
CSIDL_NETHOOD
FOLDERID_NetHood
A file system directory that contains the link objects that may exist in the My Network Places virtual folder. It is not the same as CSIDL_NETWORK, which represents the network namespace root. A typical path is C:\Documents and Settings\username\NetHood.
CSIDL_NETWORK
FOLDERID_NetworkFolder
A virtual folder that represents Network Neighborhood, the root of the network namespace hierarchy.
CSIDL_PERSONAL
FOLDERID_Documents
Version 6.0. The virtual folder that represents the My Documents desktop item. This is equivalent to CSIDL_MYDOCUMENTS.
Previous to Version 6.0. The file system directory used to physically store a user's common repository of documents. A typical path is C:\Documents and Settings\username\My Documents. This should be distinguished from the virtual My Documents folder in the namespace. To access that virtual folder, use SHGetFolderLocation, which returns the ITEMIDLIST for the virtual location, or refer to the technique described in Managing the File System.
CSIDL_PRINTERS
FOLDERID_PrintersFolder
The virtual folder that contains installed printers.
CSIDL_PRINTHOOD
FOLDERID_PrintHood
The file system directory that contains the link objects that can exist in the Printers virtual folder. A typical path is C:\Documents and Settings\username\PrintHood.
CSIDL_PROFILE
FOLDERID_Profile
Version 5.0. The user's profile folder. A typical path is C:\Users\username. Applications should not create files or folders at this level; they should put their data under the locations referred to by CSIDL_APPDATA or CSIDL_LOCAL_APPDATA. However, if you are creating a new Known Folder the profile root referred to by CSIDL_PROFILE is appropriate.
CSIDL_PROGRAM_FILES
FOLDERID_ProgramFiles
Version 5.0. The Program Files folder. A typical path is C:\Program Files.
CSIDL_PROGRAM_FILESX86
FOLDERID_ProgramFilesX86
CSIDL_PROGRAM_FILES_COMMON
FOLDERID_ProgramFilesCommon
Version 5.0. A folder for components that are shared across applications. A typical path is C:\Program Files\Common. Valid only for Windows XP.
CSIDL_PROGRAM_FILES_COMMONX86
FOLDERID_ProgramFilesCommonX86
CSIDL_PROGRAMS
FOLDERID_Programs
The file system directory that contains the user's program groups (which are themselves file system directories). A typical path is C:\Documents and Settings\username\Start Menu\Programs.
CSIDL_RECENT
FOLDERID_Recent
The file system directory that contains shortcuts to the user's most recently used documents. A typical path is C:\Documents and Settings\username\My Recent Documents. To create a shortcut in this folder, use SHAddToRecentDocs. In addition to creating the shortcut, this function updates the Shell's list of recent documents and adds the shortcut to the My Recent Documents submenu of the Start menu.
CSIDL_RESOURCES
FOLDERID_ResourceDir
Windows Vista. The file system directory that contains resource data. A typical path is C:\Windows\Resources.
CSIDL_RESOURCES_LOCALIZED
FOLDERID_LocalizedResourcesDir
CSIDL_SENDTO
FOLDERID_SendTo
The file system directory that contains Send To menu items. A typical path is C:\Documents and Settings\username\SendTo.
CSIDL_STARTMENU
FOLDERID_StartMenu
The file system directory that contains Start menu items. A typical path is C:\Documents and Settings\username\Start Menu.
CSIDL_STARTUP
FOLDERID_Startup
The file system directory that corresponds to the user's Startup program group. The system starts these programs whenever any user logs on. A typical path is C:\Documents and Settings\username\Start Menu\Programs\Startup.
CSIDL_SYSTEM
FOLDERID_System
Version 5.0. The Windows System folder. A typical path is C:\Windows\System32.
CSIDL_SYSTEMX86
FOLDERID_SystemX86
CSIDL_TEMPLATES
FOLDERID_Templates
The file system directory that serves as a common repository for document templates. A typical path is C:\Documents and Settings\username\Templates.
CSIDL_WINDOWS
FOLDERID_Windows


*/
   } //switch CSIDL
   Directory_name *CSIDL_dir_name = new Directory_name_concrete
      (CSIDL_dir_cstr,CORN::OS::directory_entry);                                //161029
   return CSIDL_dir_name;
}
//_render_CSIDL_directory_name______________________________________2012-03-19_/
/*
Directory_name *File_system_engine_Unix::render_current_working_directory() const
{
   // Instanciates a directory name corresponding to the
   // the current working directory.
   // The directory name object is relinquished to the caller.

   //Note I don't know how to get the CWD using wchar_t in linux

   char CWD[2048];
   getcwd( CWD, 2048 ); // remember where we are
   chdir(logical_drive_name.c_str());
   char CWD_on_drive[2048];
   getcwd(CWD_on_drive, 2048 ); //
   Directory_name_concrete *target_WD = new Directory_name_concrete(CWD_on_drive,CORN::OS::directory_entry);
   chdir(CWD); // go back to where we were
   return target_WD;
}
//_render_current_working_directory_________________________________2012-06-14_/
*/
Directory_entry_name *File_system_engine_Unix::render_resolved_shortcut_or_assume_actual
(const Directory_entry_name &actual_directory_entry_or_shortcut_LNK) rendition_
{
   Directory_entry_name *rendered = 0;
   /* Currently not applicable in Linux  (Linux doesn't have shortcuts)
    // MacOS X, however does have shortcuts.
   rendered = render_resolved_shortcut(actual_directory_entry_or_shortcut_LNK);
   if (!rendered)
   */
   rendered = new Directory_entry_name_concrete(actual_directory_entry_or_shortcut_LNK);
   return rendered;
}
//_render_resolved_shortcut_or_assume_actual________________________2012-06-14_/
bool File_system_engine_Unix::is_symbolic_link
(const File_name &file_name)                                               const
{  bool is_sym_link = false;
   struct stat p_statbuf;
   if (lstat(file_name.c_str(), &p_statbuf) >= 0)
   {  is_sym_link = S_ISLNK(p_statbuf.st_mode) == 1;
   } // else error occured so assumed not to be a link.
   return is_sym_link;
}
//_is_symbolic_link_________________________________________________2012-11-01_/
Directory_entry_name *File_system_engine_Unix::render_resolved_symbolic_link_or_shortcut
(const Directory_entry_name &file_name
,bool relative)                                                            const
{  Directory_entry_name *resolved = 0;
   char buff[2048];
   ssize_t len = ::readlink(file_name.c_str(), buff, sizeof(buff)-1);
   if (len != -1)
   {  buff[len] = '\0'; // readlink doesn't null terminate the string
      resolved = new File_name_concrete(buff);
      /* NYI
         if the link is relative and the relative option is disabled,
         then covert the resolved filename to fully qualify.


      */
   } // else not a link so not resolved.
   return resolved;
}
//_render_resolved_symbolic_link_or_shortcut________________________2012-11-01_/
nat32 File_system_engine_Unix::get_file_size_bytes(const File_name &file_name)   const
{  std::ifstream file(file_name.c_str(), std::ios::binary );
   /*nat32*/unsigned long fsize = file.tellg();
   file.seekg( 0, std::ios::end );
   fsize = (unsigned long)file.tellg() - fsize;
   return fsize; // returns the number of bytes in the file
}
//_get_file_size_bytes______________________________________________2005-09-20_/
Directory_name   *File_system_engine_Unix::render_home()              rendition_
{  struct passwd *pw = getpwuid(getuid());
   Directory_name *rendered = new Directory_name_concrete
      (pw->pw_dir,CORN::OS::directory_entry);                                    //161029
   return rendered;
}
//_render_home______________________________________________________2016-10-25_/
std::wstring *File_system_engine_Unix::render_environment_variable_value
(const std::wstring &envvar)                                          rendition_
      // Must be implemented for specific OS in derived file_system_engine class.
{
   std::string envvar_raw;
   CORN::append_wstring_to_string(envvar,envvar_raw);
   char *envvar_paths_raw = getenv(envvar_raw.c_str());
   std::wstring *envvar_paths
      =  envvar_paths_raw ? new std::wstring : 0;
   CORN::ASCIIZ_to_wstring(envvar_paths_raw ,(*envvar_paths));
   if (envvar_paths) CORN::ASCIIZ_to_wstring(envvar_paths_raw,*envvar_paths);
   return envvar_paths; // may be 0
}
//_get_environment_variable_value___________________________________2018-08-06_/
File_system_engine_Unix file_system_engine_Unix;                                 //120614
File_system_engine &file_system_engine = file_system_engine_Unix;                //120614
}}//_namespace_CORN_OS_________________________________________________________/
#endif
