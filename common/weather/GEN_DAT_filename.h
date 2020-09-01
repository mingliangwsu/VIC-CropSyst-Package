#ifndef GEN_DAT_filenameH
#define GEN_DAT_filenameH
#ifndef date_typesHPP
#  include "corn/chronometry/date_types.hpp"
#endif
using namespace CORN;

#include "corn/OS/directory_entry_name_concrete.h"
//_____________________________________________________________________________
class GEN_DAT_UED_Filename
: public CORN::OS::File_name_concrete
{public:
    inline GEN_DAT_UED_Filename(const char *_filename)
      : CORN::OS::File_name_concrete(_filename ? _filename : "noname.ued")       //030730
      {}
   inline GEN_DAT_UED_Filename(const CORN::OS::File_name &_filename)
      : CORN::OS::File_name_concrete(_filename)
      {}
    void set_year(Year year); // for filename code                               //000124
    void set_prefix_year(const std::string &prefix,Year year);                   //000124
    Year get_year();                                                             //000124
    const std::wstring &get_prefix(std::wstring &prefix_return);                 //170111_000124
    void set_ext(const char *new_ext);                                           //150315
};
//_GEN_DAT_UED_filename_____________________________________________1999-02-28_/
#endif



