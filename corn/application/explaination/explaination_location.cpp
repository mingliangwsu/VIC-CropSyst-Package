#include "explaination_location.h"
#include "corn/application/program_directory_OS_FS.h"
#include "corn/string/strconv.hpp"
#include "corn/OS/file_system_engine.h"
//---------------------------------------------------------------------------
namespace CORN
{
using namespace OS_FS;
Label_description Explaination::severity_label_table[] =
{ {"information" , "Information" }   // 00
, {"warning"     , "Warning"     }   // 01
, {"error"       , "Error"       }   // 10
, { 0,0}
};
//______________________________________________________________________________
Explaination::Explaination
(Severity _severity
,const std::string &_document_URI
,const std::string &_additional_comment
,const std::string &_context_URI)
: Item()
, severity           (_severity)
, document_URL       (_document_URI)
, additional_comment (_additional_comment)
, context_URIs       ()
{
   context_URIs.set_unique_keys(true);                                           //180628
   context_URIs.add_string(_context_URI);                                        //140620
}
//_Explaination:constructor____________________________________________________/
Explaination::Explaination
(Severity _severity
,const std::string &_document_URI
,const std::string &_additional_comment
,const OS::File_name &_file_name)
: Item()
, severity           (_severity)
, document_URL       (_document_URI)
, additional_comment (_additional_comment)
, context_URIs       ()
{
   context_URIs.set_unique_keys(true);                                           //180628
   context_URIs.add_wstr(_file_name.w_str());                                    //140620
}
//_Explaination:constructor____________________________________________________/
Explaination::Explaination
(Severity _severity
,const std::string &_document_URI
,const std::string &_additional_comment
,const std::string &_context_URI
,const datetime64 occurance_datetime)
: Item()
, severity           (_severity)
, document_URL       (_document_URI)
, additional_comment (_additional_comment)
, context_URIs       ()
//, resolved           (false)
{
   context_URIs.set_unique_keys(true);                                           //180628
   context_URIs.add_string(_context_URI);                                        //140620
   occurance_datetimes.append(occurance_datetime);
}
//_Explaination:constructor_________________________________________2014-06-20_/
void Explaination::append_context(const std::string &_context_URI)
{  context_URIs.add_cstr(_context_URI.c_str());
}
//_append_context______________________________________________________________/
void Explaination::render(std::string &as_string,const char *delimiter) const
{  as_string.append(severity_label_table[severity].description);
   as_string.append(delimiter);
/*done by Explainations resolve_URLs()
   std::string actual_explaination_URL(explaination_URL);
   if (program_directory)
   {  size_t proghome_pos = actual_explaination_URL.find("%PROG_HOME%");
      if (proghome_pos != std::string::npos)
         actual_explaination_URL.replace(proghome_pos,(size_t)11,program_directory->c_str());
   }
   OS::File_name_concrete explaination_filename(actual_explaination_URL);
*/
   as_string.append("<A HREF=\"");
   if (document_URL.local_file_name) document_URL.local_file_name->append_to_string(as_string);            //150727
   else                 as_string.append(document_URL);                          //150727
   as_string.append("\">");
   CORN::wstring_to_string(document_URL.ref_path().get_name(),as_string);
   as_string.append("</A>");
   as_string.append(delimiter);
   as_string.append(additional_comment);
   FOR_EACH_IN(context,Item /*180119 Text_list::Item*/,context_URIs,each_context)
   {  as_string.append(delimiter);
      context->append_to_string(as_string);                                       //180115
      //180115 as_string.append(*context);
   } FOR_EACH_END(each_context)
}
//_render______________________________________________________________________/
int Explaination::compare(const Item &other)                               const //110622
{  int comparison = 0;
   const Explaination &other_explaination = dynamic_cast <const Explaination &>(other);
   comparison = (int)severity - (int)(other_explaination.severity);
   if (comparison == 0)
   {  comparison = document_URL.compare(other_explaination.document_URL);
      if (comparison == 0)
         comparison = additional_comment.compare(other_explaination.additional_comment);
     // Don't compare context_URIs
   }
   return comparison;
}
//_compare__________________________________________________________2011-06-22_/
bool Explaination::write(std::ostream &strm)                        performs_IO_ //150728
{  std::string details_HTML;
   render(details_HTML,"<BR>");
   strm << details_HTML << "<HR>" << std::endl;
   return true;
}
//_write____________________________________________________________2011-08-11_/
bool Explaination::is_resolved()                                    affirmation_ //150729
{  return document_URL.local_file_name || (document_URL.find("%") != std::string::npos);
}
//_is_resolved______________________________________________________2015-07-29_/
bool Explaination::resolve_URL
(const Documentation_paths_fully_qualified &doc_paths
,bool  find_unqualified_filenames)                                 contribution_
{  bool resolved = is_resolved();
   if (!resolved && doc_paths.program)     resolved = replace_variable("%PROG_DOC%"   ,*doc_paths.program        ,find_unqualified_filenames,doc_paths.Web_root_URI,doc_paths.local_root);
   if (!resolved && doc_paths.application) resolved = replace_variable("%APP_DOC%"    ,*doc_paths.application    ,find_unqualified_filenames,doc_paths.Web_root_URI,doc_paths.local_root);
   if (!resolved && doc_paths.suite)       resolved = replace_variable("%SUITE_DOC%"  ,*doc_paths.suite          ,find_unqualified_filenames,doc_paths.Web_root_URI,doc_paths.local_root);
   /*
   if (!is_resolved() && program_directory) resolved = replace_variable("%PROG_DOC%"   ,program_dirname_web_URL  ,doc_paths.Web_root_URI,doc_paths.local_root);
   if (!is_resolved() && program_directory) resolved = replace_variable("%APP_DOC%"    ,app_dirname_web_URL      ,doc_paths.Web_root_URI,doc_paths.local_root);
   if (!is_resolved() && program_directory) resolved = replace_variable("%SUITE_DOC%"  ,suite_dirname_web_URL    ,doc_paths.Web_root_URI,doc_paths.local_root);
   */
   return resolved;
}
//_resolve_URL______________________________________________________2015-07-29_/
bool Explaination::replace_variable
(const char *variable
,const CORN::OS::Directory_name &doc_path
,bool find_unqualified_filenames
,const CORN::OS::Uniform_resource_identifier *Web_root_URI
,const CORN::OS::Directory_name *local_root)                       contribution_
{  bool replaced = false;
   std::size_t var_len = strlen(variable);
   std::string local_document_filename(document_URL.ref_path().c_str());
   std::size_t var_pos = document_URL.find(variable);
   if (var_pos != std::string::npos)
   {
      local_document_filename.replace(var_pos,var_len,doc_path.c_str());
      // This should give local file path. Ie:
      // c:\CS_Suite_4\CropSyst\crop_editor\manual\root.htm
      // or
      // c:\CS_Suite_4\CropSyst\crop_editor\Documentation\Manual\root.htm
      document_URL.local_file_name = new CORN::OS::Directory_name_concrete(local_document_filename);
      if (CORN::OS::file_system_engine.exists(*document_URL.local_file_name))
      {
          document_URL.set_local_path_string(local_document_filename);
      } else
      {  // The file is not on the local file system.
         // so we don't want to haved the local_file_name
         delete document_URL.local_file_name; document_URL.local_file_name = 0;
         // We assume there is a Web page if there is Web_root_URI
         if (Web_root_URI)
         {
            std::string Web_path(document_URL.ref_path().c_str());
            if (local_root)
            {  std::size_t local_root_pos = Web_path.find(local_root->c_str());
               if (local_root_pos != std::string::npos)
               {  std::size_t local_root_len = strlen(local_root->c_str());
                  Web_path.replace(local_root_pos,local_root_len,*Web_root_URI);
                  document_URL.set_path_string(Web_path);
               } // else the local_root not in the path (usually not the case) don't change the URI
            } // else don't atempt to change the URI (This user of this class didn't specify the optional local root).
         } // else now Web_root_URI to set (The use of this class didn't specify the option Web root).
      }
   } else
   {
      bool document_is_qualified = document_URL.ref_path().is_qualified();
      if (find_unqualified_filenames && !document_is_qualified)
      {
         std::wstring name_and_ext;
         document_URL.local_file_name = CORN::OS::file_system_engine.
            find_file_name                                                       //150909
            (document_URL.ref_path().get_name_with_extension(name_and_ext)
            ,doc_path,CORN::OS::File_system::subdirectory_recursion_inclusive); //200518LML ,0,false);//160924_

         document_URL.set_local_path_filename(*document_URL.local_file_name);
         // if the filename is not found document_URL.local_file_name will be left set to 0.
         // (so can't resolve, do nothing).
      } // else do nothing, presume the
   }
   return replaced;
}
//_replace_variable_________________________________________________2015-07-29_/
}//_namespace_CORN_____________________________________________________________/

