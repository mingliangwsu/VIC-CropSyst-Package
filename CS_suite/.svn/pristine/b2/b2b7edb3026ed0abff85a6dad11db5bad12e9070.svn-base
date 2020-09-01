#include "CS_suite/file_system/CS_context_discovery.h"
#include "corn/data_source/vv_file.h"
#include "corn/OS/file_system_engine.h"
#include "corn/container/bilist.h"
#include "corn/container/text_list.h"
#include "common/geodesy/geolocation.h"
#include "corn/seclusion.h"
namespace CS
{
//______________________________________________________________________________

/* conceptual
File_discovery::File_discovery
(directory_level leaf_
,directory_level root_)
{

      // The root is a top most directory to search for files.
      // Typically this logical root would be either a project directory (super project)
      // the user's home
      // (For both Linux/Unix and Windows ~ is recognized as home)
      // or less likely to root of the file system,
      // or the same as the leaf to limit discovery to one directory
      // The leaf would typically be the CWD.


   continue here
}
*/
//_2015-12-06_____________________________________________________constructor__/

Context_discovery::Context_discovery()
: Context_discovery_common()
, Scenarios(0)
{
   // root_level currently always going to be the root of the drive.
}
//_Context_discovery:constructor____________________________________2015-12-06_/
Context_discovery::Context_discovery
(const CORN::OS::Directory_name &leaf_
,const CORN::OS::Directory_name *root_)
: Context_discovery_common(leaf_,root_)
, Scenarios(0)
{
   // root_level currently always going to be the root of the drive.
}
//_Context_discovery:constructor____________________________________2016-11-02_/
CORN::OS::Directory_name *Context_discovery::provide_Scenarios()      provision_
{
   if (!Scenarios)
   {
      Scenarios = CORN::OS::file_system_engine.find_qualified_name_in_path
         (L"Scenarios",provide_leaf());
   }
   return Scenarios;
}
//_provide_Scenarios________________________________________________2016-11-16_/
}//_namespace_CS_______________________________________________________________/

