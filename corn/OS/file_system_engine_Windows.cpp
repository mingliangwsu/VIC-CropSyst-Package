#if (defined(_WIN32)  || defined(_WIN64))
#include <assert.h>
#include "windows.h"
#include "corn/OS/file_system_engine_Windows.h"
#include "corn/string/strconv.h"
#include "corn/OS/directory_entry_name_concrete.h"
#include "corn/container/text_list.h"
#include "corn/OS/OS.h"
#include "corn/seclusion.h"

#include <sys/stat.h>
// sys/stat needed for get_file_size_bytes

#ifdef __BCPLUSPLUS__
#  if ((__BCPLUSPLUS__ > 0) && (__BCPLUSPLUS__ < 0x0570))
#     include <shellapi.h>
#  endif
#endif

#ifdef _MSC_VER
#include <afxwin.h>
// for CFileFind
#else
#  include <dir.h>
   // for findfirst and findnext
#  include <stdio.h>
#endif
#include <direct.h>
#include <io.h>
#include <shlobj.h>
#include <fstream>

namespace CORN { namespace OS {

//______________________________________________________________________________
Directory_name *File_system_engine_Windows::render_CSIDL_directory_name(nat16 CSIDL) const
{  Directory_name *CSIDL_dir_name = 0;
   LPITEMIDLIST  pidl;
   LPMALLOC      pShellMalloc;
   wchar_t       szDir[MAX_PATH];                                                //121024
   // SHGetSpecialFolderLocation generates a PIDL. The memory for the PIDL
   // is allocated by the shell, and should be freed using the shell
   // mallocator COM object. Use SHGetMalloc to retrieve the malloc object
   if(SUCCEEDED(SHGetMalloc(&pShellMalloc)))
   {   // if we were able to get the shell malloc object, then
       // proceed by fetching the pidl for the windows desktop
       if(SUCCEEDED(SHGetSpecialFolderLocation(NULL, (int)CSIDL, &pidl)))
       { // return is true if success
           if(SHGetPathFromIDListW(pidl, szDir))                                 //121024
           {   // set one caption to the directory path
               CSIDL_dir_name = new Directory_entry_name_concrete(szDir);
           }
           pShellMalloc->Free(pidl);
       }
       pShellMalloc->Release();
   }
   return CSIDL_dir_name;
}
//_2010-01-01_____________________________________render_CSIDL_directory_name__/
bool File_system_engine_Windows::recycle
(const Directory_entry_name &entry_name
,bool delete_forever)                                                      const
{
      ///< Moves the directory item to the recycle bin.
      ///< For operating systems/shells that don't have recycle bin.
      ///< The file is not deleted unless delete_forever is true;
      ///< (Currently only implemented in Windows.  Under other OS, this does nothing.
      ///< Opens the directory entry with a File manager/browser (Window Explorer) window.
      ///<\return true if the file was successfully recycled, false if it failed (nothing happened).
   if (!exists(entry_name))                                                      //130211_091215
      return false;
   // Note, with some modifications this can be used to copy a directory.
   // Moves the directory to the recycle bin.


   /* May need to be implemented : Deleting Hard Link
   In order to delete a hard link, call the Windows API function DeleteFile,
   specifying the link path.
   The DeleteFile function removes the link and decreases the link count
   in the file's MFT record. If the link count becomes zero,
   the system deletes the file, that is frees its MFT entry and disk space.
   Note that there is no difference between the original file name and an
   additional hard link. Both are just pointers to the file's MFT entry.
   */
   std::wstring name_string;
   entry_name.get_Windows_representation(name_string);
   int FileNameLen = name_string.length();
   SHFILEOPSTRUCTW shfos;
   shfos.lpszProgressTitle = L"";
   shfos.pTo = L"";
   wchar_t *fn = new wchar_t[FileNameLen + 2];
   wcscpy(fn,name_string.c_str());
   *(fn + FileNameLen +1 ) =0;
   shfos.hwnd = NULL;
   shfos.wFunc = FO_DELETE;
   shfos.pFrom = fn;
   shfos.fFlags =  FOF_SILENT | FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR;
   if (!delete_forever) shfos.fFlags |= FOF_ALLOWUNDO;
   shfos.fAnyOperationsAborted = false;
   shfos.hNameMappings = NULL;
   int ret = SHFileOperationW(&shfos);
   delete [] fn;
   return (ret == 0) && (!shfos.fAnyOperationsAborted);
}
//____________________________________________________________________recycle__/

#ifdef __BCPLUSPLUS__
   //150320 Actually the new implmentation as I have it in File_system_engine_common
   // should replace this, but I need to make sure it is working
   // as before for BC++

bool File_system_engine_Windows::delete_directory
      (const Directory_name & directory_name_to_delete
      ,bool recurse_subdirectories
      ,CORN::Container *deleted_file_names         //optional
      ,CORN::Container *not_deleted_file_names     //optional
      ,CORN::Container *deleted_directory_names    //optional
      ,CORN::Container *not_deleted_directory_name //optional
      ,CORN::Text_list *error_log                  //optional
      ,bool delete_only_file_and_preserve_folder_and_subfolders )   const
{   // problem_files_SDF   is in System Data Format
   bool directory_deleted = true;
   CORN::OS::Directory_name_concrete CWD;                                        //160622
   // CWD because the following directory check uses chdir.
   if (_wchdir(directory_name_to_delete.w_str()) == 0)
      // donot delete anything if we did not change directory!!!  //990612
   {
      std::wstring qualified_find_filename(directory_name_to_delete.w_str());
      qualified_find_filename.append(L"\\");                                      //140422
      qualified_find_filename.append(L"*.*");
      #if (defined(_MSC_VER) || defined(__GNUC__))
      CFileFind finder;
      bool found = finder.FindFile(qualified_find_filename.c_str());
      #define found_filename_unqualified finder.GetFileName()
      while (found)
      #else
      #ifdef __BCPLUSPLUS__
      _wffblk fileblck;
      const unsigned findAttr = FA_RDONLY | FA_ARCH |FA_DIREC;
      int result = _wfindfirst
         (qualified_find_filename.c_str(),&fileblck,findAttr);
      while (result == 0)
      #define found_filename_unqualified fileblck.ff_name
      #else
      WIN32_FIND_DATA data;
      HANDLE handle = (FindFirstFile(qualified_find_filename.c_str(),&data)) != INVALID_HANDLE_VALUE;    //130203
      #error NYI
      assert(false);  // This has not yet been fully implement for gcc
         // See http://cygwin.com/ml/cygwin/1998-11/msg00165.html
      #endif
      #endif
      { // There was a matching directory entry
         #ifdef _MSC_VER
         if (finder.IsNormal())                                                   //121024
         #else
         #ifdef __BCPLUSPLUS__
         if ((fileblck.ff_attrib &FA_DIREC) == 0)
         #else
         if (handle)
         #endif
         #endif
         {  // Delete                                                             //020323

            Directory_entry_name_concrete *found_filename_qualified = new Directory_entry_name_concrete(directory_name_to_delete,found_filename_unqualified);
            int unlink_error =
               _wunlink(found_filename_qualified->w_str());                      //140422
            if (unlink_error)
            {  if (error_log) log_any_delete_error(unlink_error,*error_log);
               if (not_deleted_file_names)
                   deleted_file_names->append(found_filename_qualified);         //140422 new Directory_entry_name_concrete (directory_name_to_delete,found_filename_unqualified));
            } else
            {  if (deleted_file_names)
                   deleted_file_names->append(found_filename_qualified);
            }
            errno = 0; // Clear error code.
          } else if (recurse_subdirectories)
          { if (found_filename_unqualified[0] != '.') // Don't go to parent directory!
            {  Directory_name_concrete *sub_dir_to_del_raw
                  = new Directory_name_concrete
                  (directory_name_to_delete,found_filename_unqualified,CORN::OS::directory_entry);
               Directory_name_concrete *sub_dir_to_del = sub_dir_to_del_raw;
               if (delete_directory
                   ( *sub_dir_to_del
                     ,recurse_subdirectories
                     ,deleted_file_names
                     ,not_deleted_file_names
                     ,deleted_directory_names
                     ,not_deleted_directory_name
                     ,error_log
                     ,delete_only_file_and_preserve_folder_and_subfolders))
               {  if (deleted_directory_names)
                      deleted_directory_names->append(sub_dir_to_del_raw);
                  else delete sub_dir_to_del;
               } else
               {  if (not_deleted_directory_name)
                     not_deleted_directory_name->append(sub_dir_to_del_raw);
                  else delete sub_dir_to_del;
               }
            }
          }
         #ifdef __BCPLUSPLUS__
         result = _wfindnext(&fileblck);
         #else
         // VS
         found = finder.FindNextFile();                                          //121024
         #endif
       }
      // Don't need to chhdir because using fully qualified filename in findfirst//091219
      if (!delete_only_file_and_preserve_folder_and_subfolders)                  //070514
      {  // rmdir Can only delete the directory itself only after all members have been successfully deleted
         // rmdir won't delete root
         // Trying rmdir first
         int rmdir_error = _wrmdir(directory_name_to_delete.w_str());
         if (rmdir_error)
         {  // unlink might work  (but it may apply only to files)
            int unlink_error = _wunlink(directory_name_to_delete.w_str());
            if (unlink_error)
               if (error_log) log_any_delete_error(unlink_error,*error_log);
                        else directory_deleted = true;
         } else directory_deleted = true;
      }
    }
   _wchdir(CWD.w_str());                                                         //160622
   return directory_deleted;
}
#endif
//_2009-12-19_______________________________________________delete_completely__/
bool File_system_engine_Windows::exists
(const Directory_entry_name &directory_entry_name)                         const
{  const wchar_t *w_array = directory_entry_name.w_str();
   bool dir_entry_exists = _waccess(w_array, 0) == 0;
   // Note that access may not be available in all OS's (works for Windows and POSIX)
   return dir_entry_exists;
}
//_____________________________________________________________________exists__/
datetime64 File_system_engine_Windows::get_modification_date_time
(const Directory_entry_name &directory_entry_name)                         const
{
   datetime64 result = 0;
#ifdef __BCPLUSPLUS__
const wchar_t *DEN_wstr =directory_entry_name.w_str();
   FILE *stream;
   if ((stream != _wfopen(DEN_wstr ,L"rt")) == NULL)                              //001109
   {
      struct ftime ft;
      getftime(fileno(stream), &ft);
      Date_time_clad_64 file_date_time;
      file_date_time.set_YMD((Year)(ft.ft_year+1980),(CORN::Month)ft.ft_month,(CORN::DOM)ft.ft_day);
      file_date_time.set_HMS((CORN::Hour)(ft.ft_hour),(CORN::Minute)(ft.ft_min),(CORN::Second)(ft.ft_tsec * 2));
      result = file_date_time.get_datetime64();
      fclose(stream);
   }
#else

#endif
   return result;
}
//_2003-07-17__1998-01-29__________________________get_modification_date_time__/
#ifndef _QT_
nat32 File_system_engine_Windows::list_names_in
(const Directory_name & directory_name
,CORN::Text_list *file_name_list_unqual
,CORN::Text_list *directory_name_list_unqual)                              const
{
   std::wstring path_wildcard(directory_name.w_str());
   path_wildcard.append(L"\\*");
   struct _wffblk FIB;
   char included_attributes = 0;  // rename to scan_attributes
   if (directory_name_list_unqual) included_attributes |= FA_DIREC;
   int find_status = _wfindfirst(path_wildcard.c_str(),&FIB,included_attributes);
   while (find_status == 0)
   {  std::wstring unqualified_name(FIB.ff_name);
      bool is_dir =( FIB.ff_attrib & FA_DIREC);
      if (is_dir)
      {
         if (FIB.ff_name[0] != '.') // Exclude .. and . directory entries.
         {
            {
                  if (directory_name_list_unqual)
                      directory_name_list_unqual->add_wstring(unqualified_name);
            } // exclude dir
         } // not .
      } else // the entry is a file
            if (file_name_list_unqual)
                file_name_list_unqual->add_wstring(unqualified_name);
      find_status = _wfindnext(&FIB);
   }
   return (file_name_list_unqual      ? file_name_list_unqual->count()      :0)
        + (directory_name_list_unqual ? directory_name_list_unqual->count() :0);
}
//_list_names_in____________________________________________________2016-10-29_/
#endif
//_2015-12-12___________________________________________________________________
bool File_system_engine_Windows::create_directory
(const Directory_name &directory_name)                                     const //130126
{
   bool created = CreateDirectoryW(directory_name.w_str(),0);                    //180321
   return created;                                                               //180321
}
//_create_directory____________________________________________________________/
bool File_system_engine_Windows::create_shortcut
(const Directory_entry_name   &target_file   // I think will also work for directory but I haven't tried it
,const Directory_name         &short_cut_location // where the shortcut will be created
,const std::wstring           &caption  // short cuts unqualified name (no ext)
,const std::wstring           &description
// NYI const File_name *icon_filename shortcuts can have an optional icon
)                                                                          const
{  bool successful = false;
#ifdef __GNUC__
assert(false);  // This has not yet been fully implement for gcc
// See http://cygwin.com/ml/cygwin/1998-11/msg00165.html
#else

   Directory_entry_name_concrete link_file_name(short_cut_location,caption,L"lnk");
/*
http://www.flexhex.com/docs/articles/hard-links.phtml

Shortcuts can also point to non-file objects like printers.
   and can have icons

Windows Shortcuts
A more commonly used name for Windows symbolic link is shortcut.
Shortcut is a much more advanced version of Unix soft link -
in addition to the basic link-to-file relationship you can also set
a description, an icon, and so on. In addition,

A shortcut is actually a file, usually with the .lnk extension (any other extension can also be registered as a shortcut). Like any other file it can be copied, renamed, or deleted; file operations on a shortcut don't affect the object being pointed to.

Programming Considerations
In order to improve readability, the code examples below don't include any error processing. You should add some error checks if you want to use this code in your program. See the sources in the download section for an example of error handling.

Creating New Shortcut


   /* In order to set up a Windows shortcut programmatically first create it
   using the IShellLink interface (don't forget to call CoInitialize before
   calling any COM function).
   */
   CoInitialize(NULL);
   HRESULT hres = NULL;
   IShellLinkW* shell_link = NULL;
   // Get a pointer to the IShellLink interface.
   hres = CoCreateInstance(CLSID_ShellLink, NULL,
            CLSCTX_INPROC_SERVER// CLSCTX_INPROC_SERVER
            , IID_IShellLinkW,
            //reinterpret_cast<void**>
            (void**)(&shell_link));
   if (SUCCEEDED(hres))
   {  /* If you want to create a shortcut for a non-file object,
      use the SetIDList function instead of SetPath.
      In this case you should use a pointer to the object's
      ITEMIDLIST structure, or PIDL.
      PIDL is a more universal way of object addressing so you can use the
      SetIDList function for both file and non-file objects.
      You can obtain PIDLes for the standard shell objects by calling the
      SHGetFolderLocation or SHGetSpecialFolderLocation function.
      To convert a PIDL to the corresponding file path use the shell function
      SHGetPathFromIDList.
      */

      /*
      To set additional parameters like a description or an icon location use
      the appropriate IShellLink functions:
      */

      // When the shortcut is set up completely, save it using the IPersistFile interface
      IPersistFile* ppf = NULL;
      // Set the path to the shortcut target
      shell_link->SetPath(target_file.w_str());
      //Please note that unlike IShellLink::SetPath, and like most COM/OLE
      // functions, IPersistFile::Save requires a UNICODE path string.
      shell_link->SetDescription(description.c_str());
      // Query IShellLink for the IPersistFile interface for
      // saving the shortcut in persistent storage.
      hres = shell_link->QueryInterface(IID_IPersistFile,reinterpret_cast<void**>(&ppf));
      if (SUCCEEDED(hres))
      {  WCHAR wsz[MAX_PATH];
         // Ensure that the string is ANSI.
         //120209 MultiByteToWideChar(CP_ACP, 0, link_file_name.c_str(), -1,wsz, MAX_PATH);
         //120209 hres = ppf->Save(wsz, TRUE);    // Save the link by calling IPersistFile::Save.
         hres = ppf->Save(link_file_name.w_str(), TRUE);    // Save the link by calling IPersistFile::Save.
         ppf->Release();    //commit I/O by releasing the interface pointers,
         successful = true;
      }
      shell_link->Release(); // commit I/O by releasing the interface pointers,
   }
#endif
   return successful;
}
//_create_short_cut____________________________________________________________/
bool File_system_engine_Windows::is_shortcut(const Directory_entry_name &file_name) const
{  return  file_name.has_extension_ASCII("lnk",true);                            //170224
}
//_is_shortcut______________________________________________________2015-12-07_/


/* conceptual method:

edit_shortcut

Modifying Existing Shortcut
An existing shortcut can be modified using the same code; the only difference is that you should first read the shortcut using the IPersistFile::Load call. For example, a function for changing the path to the shortcut target would look as follows:

//  wszLinkPath - a UNICODE string containing the path to the shortcut (for
//      example 'C:\Documents and Settings\All Users\Desktop\MyShortcut.lnk')
//
//  szNewTarget - an ANSI string containing a new path to the target.

IShellLink *pShellLink;
IPersistFile *pPersistFile;

// Obtain IShellLink and IPersistFile pointers
::CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_ALL,
                   IID_IShellLink, (void**)&pShellLink);
pShellLink->QueryInterface(IID_IPersistFile, (void**)&pPersistFile);

// Load existing shortcut
pPersistFile->Load(wszLinkPath, STGM_READWRITE | STGM_SHARE_EXCLUSIVE);

// Modify shortcut (set a new target path)
pShellLink->SetPath(szNewTarget);

// Save the modified shortcut
pPersistFile->Save(wszLinkPath, TRUE);

// Commit I/O and release interface pointers
pPersistFile->Release();
pShellLink->Release();

Deleting Shortcut
Simply delete the shortcut file using the standard DeleteFile API function.

*/
//_____________________________________________________________________________/
Directory_entry_name *File_system_engine_Windows::render_resolved_shortcut
(const File_name &shortcut_LNK)             rendition_
{
   Directory_entry_name *resolved_dirent_name = 0;
   //151207 shortcut_LNK.has_extension_ASCII("lnk");
   if (is_shortcut(shortcut_LNK))                                                //151207
   {
#ifdef __GNUC__
assert(false);  // This has not yet been fully implement for gcc
#else

      IShellLinkW* psl;
      wchar_t* tempStr = new wchar_t[MAX_PATH];
      CoInitialize(NULL);
      std::wstring path(shortcut_LNK.w_str());
      HRESULT hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLinkW, (LPVOID*) &psl);
      if (SUCCEEDED(hr))
      {
          IPersistFile* ppf;
          hr = psl->QueryInterface( IID_IPersistFile, (LPVOID *) &ppf);
          if (SUCCEEDED(hr))
          {
              hr = ppf->Load(shortcut_LNK.w_str(), STGM_READ);
              if (SUCCEEDED(hr))
              {
                  WIN32_FIND_DATAW wfd;
#ifdef __GNUC__
assert(false);  // This has not yet been fully implement for gcc
// See http://cygwin.com/ml/cygwin/1998-11/msg00165.html
#else
                  hr = psl->GetPath(tempStr, MAX_PATH, &wfd, SLGP_UNCPRIORITY | SLGP_RAWPATH);
#endif
                  if (SUCCEEDED(hr))
                     resolved_dirent_name = new Directory_entry_name_concrete(tempStr);   //111212
              }
          }
      }
      delete tempStr;
   #endif
   }
   return resolved_dirent_name;
}
//_render_resolved_shortcut_________________________________________2011-12-12_/
Directory_entry_name *File_system_engine_Windows::render_resolved_shortcut_or_assume_actual
(const Directory_entry_name &actual_directory_entry_or_shortcut_LNK) rendition_
{  Directory_entry_name *rendered = render_resolved_shortcut(actual_directory_entry_or_shortcut_LNK);
   if (!rendered)
        rendered = new Directory_entry_name_concrete(actual_directory_entry_or_shortcut_LNK);
   return rendered;
}
//_render_resolved_shortcut_or_assume_actual________________________2011-12-12_/
Directory_name *File_system_engine_Windows::render_current_working_directory_on_drive
(const Drive_designator &logical_drive_name)                               const //120430
{  wchar_t CWD[2048];
   _wgetcwd( CWD, 2048 ); // remember where we are
   _wchdir(logical_drive_name.c_str());
   wchar_t CWD_on_drive[2048];
   _wgetcwd(CWD_on_drive, 2048 ); //
   Directory_name_concrete *target_WD = new Directory_name_concrete
      (CWD_on_drive,CORN::OS::directory_entry);                                  //161029
   _wchdir(CWD); // go back to where we were
   return target_WD;
}
//_render_current_working_directory_on_drive________________________2012-04-30_/

/* this isn't right
Directory_name *File_system_engine_Windows::render_directory_name_relative_to
(const Directory_name &qualified_reference_directory_name)            rendition_
{
   // http://msdn.microsoft.com/en-us/library/windows/desktop/aa364963(v=vs.85).aspx



    DWORD  retval=0;
    BOOL   success;
    TCHAR  buffer[BUFSIZE]=TEXT("");
    TCHAR  buf[BUFSIZE]=TEXT("");
    TCHAR** lppPart={NULL}



// Retrieve the full path name for a file.
// The file does not need to exist.

    retval = GetFullPathNameW (argv[1],
                 BUFSIZE,
                 buffer,
                 lppPart);

    if (retval == 0)
    {
        // Handle an error condition.
        printf ("GetFullPathName failed (%d)\n", GetLastError());
        return;
    }
    else
    {}
    ;

}
//_render_directory_name_relative_to________________________________2012-04-30_/
*/
bool File_system_engine_Windows::create_file_link
      (const File_name &file_name
      ,const File_name &link_name
      ,bool symbolic)                                                      const
{  ///< Create a hard link to a directory
   ///\return the created directory element or null if the OS was unable to create the directory.
   /*
   http://msdn.microsoft.com/en-us/library/aa363860(VS.85).aspx
   Establishes a hard link between an existing file and a new file. This function is only supported on the NTFS file system, and only for files, not directories.
   Note, if CreateHardLink doesn't work, try using command line command fsutil
   http://www.microsoft.com/resources/documentation/windows/xp/all/proddocs/en-us/fsutil.mspx?mfr=true
    Usage : fsutil hardlink create <new filename> <existing filename> Eg : fsutil hardlink create c:\foo.txt c:\bar.txt � bdukes Oct 21 '08 at 16:12
   */
#ifdef __GNUC__
assert(false);  // This has not yet been fully implement for gcc

return false;
#else
   return
      (symbolic == LINK_SYMBOLIC)
      //? false  // Windows need to look up function to create soft link.
      ? CreateSymbolicLinkW(link_name.w_str(),file_name.w_str(),NULL)
      : CreateHardLinkW    (link_name.w_str(),file_name.w_str(),NULL);
#endif
}
//______________________________________________________________________________
bool File_system_engine_Windows::copy_file
(const File_name &source, const File_name &destination,bool fail_if_exists)const
{  return CopyFileW(source.w_str(),destination.w_str(),fail_if_exists);
}
//_copy_file________________________________________________________2012-02-05_/
bool File_system_engine_Windows::copy_directory
(const Directory_name &source, const Directory_name &destination
,bool fail_if_exists)                                                      const
{
   ///< copy the source directory item to the destination directory
   if (!exists(source))                         return false;
   if (exists(destination) && fail_if_exists)   return false;
   SHFILEOPSTRUCTW shfos;
   std::wstring src_name_string;
   source.get_Windows_representation(src_name_string);
   int src_FileNameLen = src_name_string.length();
   shfos.lpszProgressTitle = L"";
   shfos.pTo = L"";
   wchar_t *dn_src = new wchar_t[src_FileNameLen + 2];
   wcscpy(dn_src,src_name_string.c_str());
   *(dn_src + src_FileNameLen +1 ) =0;
   std::wstring dst_name_string;
   destination.get_Windows_representation(dst_name_string);
   int dst_FileNameLen = dst_name_string.length();
   shfos.lpszProgressTitle = L"";
   shfos.pTo = L"";
   wchar_t *dn_dst = new wchar_t[dst_FileNameLen + 2];
   wcscpy(dn_dst,dst_name_string.c_str());
   *(dn_dst + dst_FileNameLen +1 ) =0;

   shfos.hwnd = NULL;
   shfos.wFunc = FO_COPY;
   shfos.pFrom = dn_src;
   shfos.pTo = dn_dst;
   shfos.fFlags =  FOF_SILENT | FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR;

   shfos.fAnyOperationsAborted = false;
   shfos.hNameMappings = NULL;
   int ret = SHFileOperationW(&shfos);
/* Now using wide characters
   int ret = SHFileOperation(&shfos);
*/
   delete [] dn_src;
   delete [] dn_dst;
   return (ret == 0) && (!shfos.fAnyOperationsAborted);
}
//_copy_directory___________________________________________________2013-02-15_/
bool File_system_engine_Windows::move
(const Directory_entry_name &source_DEN
,const Directory_entry_name &destination_DEN
,bool force
,bool interactive
,bool update
,bool verbose)                                                             const
{
   /*
   Using system command rather than function because
   I don't know of such a Windows API function
   WARNING when moving a directory the CWD must be (source's) parent
   see
   https://technet.microsoft.com/en-us/library/bb490935.aspx
   */
   std::string flags;
   if (force) // && !interactive)
   {
      if (CORN::OS::file_system_engine.exists (destination_DEN))
          CORN::OS::file_system_engine.recycle(destination_DEN,true);
   }
   if (!interactive)  flags.append(" /Y ");
   if (update)
   {
      datetime64 source_time = CORN::OS::file_system_engine
         .get_modification_date_time(source_DEN);
      datetime64 destination_time = CORN::OS::file_system_engine
         .get_modification_date_time(destination_DEN);
      if (source_time <= destination_time)
         return true; // move canceled but function performed as requested
   }
   /*
   if (verbose)
      may want to output exact information as linux verbose
   */
   return system_command_flags_source_destination
      ("move",flags,source_DEN,destination_DEN);
}
//_move_____________________________________________________________2017-12-15_/
/*conceptual
bool File_system_engine_Windows::are_in_same_file_system
      (const Directory_entry_name &DEN1
      ,const Directory_entry_name &DEN2)                            affirmation
{
      // Returns true if both Directory entries are on the same file system
      // which would be the same drive designator
      // If either is unqualified need to get fully qualified name.
}
*/
//_2017-12-15__________________________________________are_in_same_file_system_/
bool File_system_engine_Windows::set_current_working_directory
(const Directory_name &dir_name)                                           const
{  return _wchdir(dir_name.w_str()) == 0;
}
//_set_current_working_directory____________________________________2012-02-17_/

/*conceptual
   virtual modifiable_ CORN::OS::Directory *create_directory_link_soft
      (const Directory_name &source_directory_name
      ,const Directory_name &target_directory_name
      , bool take_target_directory_name = false)                           const
{

Create Synbolic link
This has been on my list to try, from my notes:

The API:

http://msdn.microsoft.com/en-us/library/aa363866(VS.85).aspx

BOOLEAN WINAPI CreateSymbolicLink(  __in  LPTSTR lpSymlinkFileName,  __in  LPTSTR lpTargetFileName,  __in  DWORD dwFlags);Some C# examples:

2D00-Creating-symbolic-links-with-C_2300_.aspx">http://community.bartdesmet.net/blogs/bart/archive/2006/10/24/Windows-Vista-2D00-Creating-symbolic-links-with-C_2300_.aspx

A C++ Example, this is cnp from another article I was reading. I have not tested it so use it with caution.

typedef BOOL (WINAPI* CreateSymbolicLinkProc) (LPCSTR, LPCSTR, DWORD);
void main(int argc, char *argv[])
{  HMODULE h;  CreateSymbolicLinkProc CreateSymbolicLink_func;
  LPCSTR link = argv[1];
    LPCSTR target = argv[2];
    DWORD flags = 0;  h = LoadLibrary("kernel32");
      CreateSymbolicLink_func =    (CreateSymbolicLinkProc)GetProcAddress(h,
      if (CreateSymbolicLink_func == NULL)
      {     fprintf(stderr, "CreateSymbolicLinkA not available\n");  }
       else
      {     if ((*CreateSymbolicLink_func)(link, target, flags) == 0)
      {        fprintf(stderr, "CreateSymbolicLink failed: %d\n",        GetLastError());  } else   {     printf("Symbolic link created.");  }}}


   return 0;
}
*/
//______________________________________________________________________________
bool File_system_engine_Windows::create_directory_link
(const Directory_name &source_directory_name
,const Directory_name &target_directory_name
,bool symbolic)                                                            const
{  ///<
   ///\return the created directory element or null if the OS was unable to create the directory.
/*

http://www.flexhex.com/docs/articles/hard-links.phtml
NTFS Hard Links, Directory Junctions, and Windows Shortcuts
NTFS Hard Links
Reparse Points and Directory Junctions
Windows Shortcuts
Downloads


--------------------------------------------------------------------------------

NTFS Hard Links
Hard link uses the same MFT entry as the original file.
Adding a hard link creates a new name attribute and increases the hard link count
(for a newly created file this count equals to one).
Deleting a hard link removes the appropriate name and decreases the hard link count.
When the count goes to zero, the system deletes the file,
freeing up its allocated disk space and releasing its MFT record.
All the name attributes are independent, so deleting, moving, or renaming the file don't affect other hard links.

In order to get the value of the hard link count,
open the file in the FlexHEX editor and select the File Properties
page in the Streams pane (the lower left one).
If there is any hard link to the file, the Hard Links value will be greater than
one (the normal directory record is also counted, so one means no hard links).

Because all the links must refer to the same MFT entry, you cannot create a
hard link on a different drive.
Another limitation is that only files can be hard-linked, not directories
(however soft links to directories are allowed).

Programming Considerations
To get the number of links use the Win32 API function GetFileInformationByHandle.
The nNumberOfLinks member of the returned BY_HANDLE_FILE_INFORMATION structure
contains the total number of links (a value of 1 means there are no hard links).


--------------------------------------------------------------------------------

Reparse Points and Directory Junctions
Reparse points provide another way of creating links.
If a file or a directory has a reparse point attached,
the system calls a file system filter, indicated by the reparse point tag.
The filter may implement any method of accessing the actual data,
including quite complicated ones.
HSM (Hierarchical Storage Management) is a good example how useful reparse points can be.

Unfortunately, although we can link to directories using the reparse points
mechanism (such links are called junctions), there is no way of linking to
files short of writing a custom file system filter driver.
Even if using reparse points is a natural way of implementing soft links,
the gain is hardly worth the pain.

However reparse points are not all that useless for our purpose:
junctions can supplement NTFS file-only hard links.
Just keep in mind that they are, in fact, soft links,
and if you move or delete the referred directory,
the junction will point to nowhere. On the other hand, being a soft link,
a junction is not limited to objects residing on the same drive;
it can point to a directory on a different drive, or even to the drive itself
(although a junction is not allowed to point to a mapped network object).

Programming Considerations
In order to improve readability, the code examples below don't include any error processing. You should add some error checks if you want to use this code in your program. See the sources in the download section for an example of error handling.

Aside from a couple of overview articles, reparse points are very scantily documented, and the Platform SDK does not include the required definitions:

#define REPARSE_MOUNTPOINT_HEADER_SIZE   8

typedef struct {
  DWORD ReparseTag;
  DWORD ReparseDataLength;
  WORD Reserved;
  WORD ReparseTargetLength;
  WORD ReparseTargetMaximumLength;
  WORD Reserved1;
  WCHAR ReparseTarget[1];
} REPARSE_MOUNTPOINT_DATA_BUFFER, *PREPARSE_MOUNTPOINT_DATA_BUFFER;

The header file WinNT.h included in the VC++ 6.0 distributive contains another definition (Microsoft removed it in the later versions of the Platform SDK):

typedef struct _REPARSE_DATA_BUFFER {
    DWORD  ReparseTag;
    WORD   ReparseDataLength;
    WORD   Reserved;
    union {
        struct {
            WORD   SubstituteNameOffset;
            WORD   SubstituteNameLength;
            WORD   PrintNameOffset;
            WORD   PrintNameLength;
            WCHAR PathBuffer[1];
        } SymbolicLinkReparseBuffer;
        struct {
            WORD   SubstituteNameOffset;
            WORD   SubstituteNameLength;
            WORD   PrintNameOffset;
            WORD   PrintNameLength;
            WCHAR PathBuffer[1];
        } MountPointReparseBuffer;
        struct {
            BYTE   DataBuffer[1];
        } GenericReparseBuffer;
    }
} REPARSE_DATA_BUFFER, *PREPARSE_DATA_BUFFER;

#define REPARSE_DATA_BUFFER_HEADER_SIZE   \
           FIELD_OFFSET(REPARSE_DATA_BUFFER, GenericReparseBuffer)

We will use the former structure as defined in the Junctions example on the Systems Internals Web site.

Helper Functions
We will need a couple of helper functions. First one opens a directory and returns its handle:

// Returns directory handle or INVALID_HANDLE_VALUE if failed to open.
// To get extended error information, call GetLastError.

HANDLE OpenDirectory(LPCTSTR pszPath, BOOL bReadWrite) {
// Obtain backup/restore privilege in case we don't have it
  HANDLE hToken;
  TOKEN_PRIVILEGES tp;
  ::OpenProcessToken(::GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken);
  ::LookupPrivilegeValue(NULL,
                         (bReadWrite ? SE_RESTORE_NAME : SE_BACKUP_NAME),
                         &tp.Privileges[0].Luid);
  tp.PrivilegeCount = 1;
  tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
  ::AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL);
  ::CloseHandle(hToken);

// Open the directory
  DWORD dwAccess = bReadWrite ? (GENERIC_READ | GENERIC_WRITE) : GENERIC_READ;
  HANDLE hDir = ::CreateFile(pszPath, dwAccess, 0, NULL, OPEN_EXISTING,
                     FILE_FLAG_OPEN_REPARSE_POINT | FILE_FLAG_BACKUP_SEMANTICS, NULL);

  return hDir;
}

The second helper function checks if the directory has an associated junction point:

#define DIR_ATTR  (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_REPARSE_POINT)

BOOL IsDirectoryJunction(LPCTSTR pszDir) {
  DWORD dwAttr = ::GetFileAttributes(pszDir);
  if (dwAttr == -1) return FALSE;  // Not exists
  if ((dwAttr & DIR_ATTR) != DIR_ATTR) return FALSE;  // Not dir or no reparse point

  HANDLE hDir = OpenDirectory(pszDir, FALSE);
  if (hDir == INVALID_HANDLE_VALUE) return FALSE;  // Failed to open directory

  BYTE buf[MAXIMUM_REPARSE_DATA_BUFFER_SIZE];
  REPARSE_MOUNTPOINT_DATA_BUFFER& ReparseBuffer = (REPARSE_MOUNTPOINT_DATA_BUFFER&)buf;
  DWORD dwRet;
  BOOL br = ::DeviceIoControl(hDir, FSCTL_GET_REPARSE_POINT, NULL, 0, &ReparseBuffer,
                                      MAXIMUM_REPARSE_DATA_BUFFER_SIZE, &dwRet, NULL);
  ::CloseHandle(hDir);
  return br ? (ReparseBuffer.ReparseTag == IO_REPARSE_TAG_MOUNT_POINT) : FALSE;
}

Creating Directory Junction
The first step is creating a directory if it does not exists. If the directory exists, it must be empty - you cannot associate a reparse point with a non-empty directory.

::CreateDirectory(szJunction, NULL)

The next step is obtaining the directory handle:

// Open for reading and writing (see OpenDirectory definition above)
HANDLE hDir = OpenDirectory(szJunction, TRUE);

And the last step is the black magic:

// Take note that buf and ReparseBuffer occupy the same space
BYTE buf[sizeof(REPARSE_MOUNTPOINT_DATA_BUFFER) + MAX_PATH * sizeof(WCHAR)];
REPARSE_MOUNTPOINT_DATA_BUFFER& ReparseBuffer = (REPARSE_MOUNTPOINT_DATA_BUFFER&)buf;

// Prepare reparse point data
memset(buf, 0, sizeof(buf));
ReparseBuffer.ReparseTag = IO_REPARSE_TAG_MOUNT_POINT;
int len = ::MultiByteToWideChar(CP_ACP, 0, szTarget, -1,
                                ReparseBuffer.ReparseTarget, MAX_PATH);
ReparseBuffer.ReparseTargetMaximumLength = (len--) * sizeof(WCHAR);
ReparseBuffer.ReparseTargetLength = len * sizeof(WCHAR);
ReparseBuffer.ReparseDataLength = ReparseBuffer.ReparseTargetLength + 12;

// Attach reparse point
DWORD dwRet;
::DeviceIoControl(hDir, FSCTL_SET_REPARSE_POINT, &ReparseBuffer,
         ReparseBuffer.ReparseDataLength+REPARSE_MOUNTPOINT_HEADER_SIZE,
         NULL, 0, &dwRet, NULL);

Note that szTarget string must contain the path prefixed with the "non-parsed" prefix "\??\", and terminated with the backslash character, for example "\??\C:\Some Dir\".

You can save some effort by using the CreateJunction function you can find in the download section.

Deleting Directory Junction
If the directory is empty, you can use the RemoveDirectory function to remove both the directory and the associated junction point. Another way is to use DeviceIoControl, which does not require the directory to be empty and removes the junction only, leaving the directory and its content intact.

if (!IsDirectoryJunction(szJunction)) {
  // Error: no junction here
}

// Open for reading and writing (see OpenDirectory definition above)
HANDLE hDir = OpenDirectory(szJunction, TRUE);

BYTE buf[REPARSE_MOUNTPOINT_HEADER_SIZE];
REPARSE_MOUNTPOINT_DATA_BUFFER& ReparseBuffer = (REPARSE_MOUNTPOINT_DATA_BUFFER&)buf;
DWORD dwRet;

memset(buf, 0, sizeof(buf));
ReparseBuffer.ReparseTag = IO_REPARSE_TAG_MOUNT_POINT;

if (::DeviceIoControl(hDir, FSCTL_DELETE_REPARSE_POINT, &ReparseBuffer,
                          REPARSE_MOUNTPOINT_HEADER_SIZE, NULL, 0, &dwRet, NULL)) {
  // Success
  ::CloseHandle(hDir);
}
else {  // Error
  DWORD dr = ::GetLastError();
  ::CloseHandle(hDir);
  // Some error action (throw or MessageBox)
}

Querying Directory Junction
The following code returns the path the junction point is linked to:

TCHAR szPath[MAX_PATH];  // Buffer for returned path

if (!IsDirectoryJunction(szJunction)) {
  // Error: no junction here
}

// Open for reading only (see OpenDirectory definition above)
HANDLE hDir = OpenDirectory(szJunction, FALSE);

BYTE buf[MAXIMUM_REPARSE_DATA_BUFFER_SIZE];  // We need a large buffer
REPARSE_MOUNTPOINT_DATA_BUFFER& ReparseBuffer = (REPARSE_MOUNTPOINT_DATA_BUFFER&)buf;
DWORD dwRet;

if (::DeviceIoControl(hDir, FSCTL_GET_REPARSE_POINT, NULL, 0, &ReparseBuffer,
                                 MAXIMUM_REPARSE_DATA_BUFFER_SIZE, &dwRet, NULL)) {
  // Success
  ::CloseHandle(hDir);

  LPCWSTR pPath = ReparseBuffer.ReparseTarget;
  if (wcsncmp(pPath, L"\\??\\", 4) == 0) pPath += 4;  // Skip 'non-parsed' prefix
  ::WideCharToMultiByte(CP_ACP, 0, pPath, -1, szPath, MAX_PATH, NULL, NULL);
}
else {  // Error
  DWORD dr = ::GetLastError();
  ::CloseHandle(hDir);
  // Some error action (throw or MessageBox)
}


--------------------------------------------------------------------------------
Notifying the Explorer
After you created or deleted a shortcut, you may need to use the SHChangeNotify function to notify the Windows Explorer about the change in the shortcut directory and to ensure the directory view is properly updated.
*/
   return 0; // NYI
}
//______________________________________________________________________________
bool File_system_engine_Windows::is_symbolic_link
(const File_name &file_name)                                               const
{  WIN32_FILE_ATTRIBUTE_DATA file_attrs;
   bool is_sym_link = GetFileAttributesExW(file_name.w_str(),GetFileExInfoStandard,&file_attrs);
   if (is_sym_link)
      is_sym_link = (file_attrs.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT);
   return is_sym_link;
}
//_is_symbolic_link_________________________________________________2012-10-01_/
Directory_entry_name *File_system_engine_Windows
::render_resolved_symbolic_link_or_shortcut
(const Directory_entry_name &file_name
,bool relative)                                                            const
{  // First check if the file is a short cut
   Directory_entry_name *rendered_dereference =render_resolved_shortcut(file_name);
      // Note might want to have render_resolved_shortcut take relative option
      // but shortcuts will probably already be relative
   if (!rendered_dereference) // maybe it is a symbolic link (junction
   {
      assert(0);
      // currently windows only has directory junction points
      // I haven't yet implemented resolving junction points
      // Use DeviceIoControl to do this
   }
   return rendered_dereference;
}
//_dereference_symbolic_link_or_shortcut____________________________2012-10-01_/
nat32 File_system_engine_Windows::get_file_size_bytes
(const File_name &file_name)                                               const
{  struct _stat filestatus;
   _wstat( file_name.w_str(), &filestatus );
   return filestatus.st_size;
}
//_open_file_with_CORN_file_name____________________________________2012-11-01_/
CORN::OS::Directory_name   *File_system_engine_Windows::render_home() rendition_
{
   CORN::OS::Directory_name *rendered = 0;
   WCHAR path[MAX_PATH];
   if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_PROFILE, NULL, 0, path)))
      rendered = new CORN::OS::Directory_name_concrete(path,CORN::OS::directory_entry);  //161029
   return rendered;
}
//_std::wstring_____________________________________________________2016-10-25_/
std::wstring *File_system_engine_Windows::render_environment_variable_value      //180906
(const std::wstring &envvar)                                          rendition_
// Must be implemented for specific OS in derived file_system_engine class.
{
   wchar_t *envvar_paths_raw = _wgetenv(envvar.c_str());
   return envvar_paths_raw ? new std::wstring(envvar_paths_raw) : 0;
}
//_render_environment_variable_value___________________________________2018-08-06_/

#ifdef NYI

120529 This will be needed to override the File_system_engine_common
because File_system_engine_common only composes from relative filenames
of the form:
relative:../../x/y/z.exe
renders->C:/A/B/C/x/y/z.ext

it cannot handle

relative:../../x/../y/z.exe
renders->C:/A/B/C/



Directory_entry_name *File_system_engine_Windows::render_resolved_absolute_directory_entry_name
(const Directory_entry_name &relative_or_absolute_actual_directory_entry_or_shortcut_LNK
,const Directory_name &with_respect_to_directory_name_qualified)                rendition_
{
   wchar_t working_path[2048];
   wcscpy(working_path,relative_or_absolute_actual_directory_entry_or_shortcut_LNK.w_str());
   const WCHAR *dirs[2];
   dirs[1] = 0;                                                                  //130909
   dirs[0] = with_respect_to_directory_name_qualified.w_str();                   //130909
   int resolved = PathResolve(working_path,dirs,0/*PRF_REQUIREABSOLUTE*/);
      //I think it may need PRF_REQUIREABSOLUTE I wan't to return an absolute path
   Directory_name_concrete *rendered =
      resolved ? new Directory_name_concrete(working_path,CORN::OS::directory_entry) : 0; //161029
   return rendered;
}
//_render_resolved_absolute_directory_entry_name____________________2012-05-29_/
#endif

File_system_engine_Windows file_system_engine_Windows;
File_system_engine &file_system_engine  = file_system_engine_Windows;
}}//_namespace_CORN_OS_________________________________________________________/


#ifdef NYI
/*
bool Smart_directory_name::set_special_folder( int csidl)
{  bool success = false;
#ifdef __WIN32__
   LPITEMIDLIST  pidl;
   LPMALLOC      pShellMalloc;
   char          szDir[MAX_PATH];
   // SHGetSpecialFolderLocation generates a PIDL. The memory for the PIDL
   // is allocated by the shell, and should be freed using the shell
   // mallocator COM object. Use SHGetMalloc to retrieve the malloc object
   if(SUCCEEDED(SHGetMalloc(&pShellMalloc)))
   {   // if we were able to get the shell malloc object, then
       // proceed by fetching the pidl for the windows desktop
       if(SUCCEEDED(SHGetSpecialFolderLocation(NULL, csidl, &pidl)))
       { // return is true if success
           if(SHGetPathFromIDList(pidl, szDir))
           {   // set one caption to the directory path
               set(szDir);
               success = true;
           }
           pShellMalloc->Free(pidl);
       }
       pShellMalloc->Release();
   }
#endif
// under UNIX, the conventional application data local settings folder is typically $HOME/.local

// For other operating systems
// we do not currently designate special folders matching the Windows CSIDL codes.

   return success;
}
//_2004-02-16___________________________________________________________________
*/
#endif

#endif

