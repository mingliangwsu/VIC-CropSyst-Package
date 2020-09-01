#ifndef URL_linkH
#define URL_linkH
#include "corn/data_source/datarec.h"
class URL_Link
: public CORN::Data_record
{public:
   std::string URL;
   std::string WorkingDirectory;
   /* NYI These are additional settings used by Windows
   nat8        ShowCommand;
      // empty(0) Normal
      // 7 = Minimized
      // 3 = Maximized
   nat32 IconIndex;
   std::string IconFile;
   std::string Modified;
   nat32 HotKey;
   */
 public:
   URL_Link()
      : CORN::Data_record("InternetShortcut")
      , URL()
      , WorkingDirectory()
      {}
   virtual bool expect_structure(bool for_write);
};
//_2018-01-05___________________________________________________________________
#endif
