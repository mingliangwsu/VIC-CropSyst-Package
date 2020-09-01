#ifndef emitter_AH
#define emitter_AH

#include "corn/data/emitter.h"
#include "CS_suite/observation/CS_optation.h"
namespace CORN
{
//____________________________________________________________________________
class Emitter_abstract
: public implements_ CORN::Emitter
{
 protected: // these could be moved to an abstract class
   nat16 date_time_column_index;
   const CS::Optation *optation; //optional
 public:
   Emitter_abstract
      (const CS::Optation *optation/*=0*/);
};
//____________________________________________________________________________
class Emitter_abstract_text
: public extends_ Emitter_abstract
{
 protected: // these could be moved to an abstract_text class
   std::ifstream source_stream_owned;
   std::istream &source_stream;
 public:
   Emitter_abstract_text
      (const char *source_filename
      ,const CS::Optation *optation/*=0*/);
   Emitter_abstract_text
      (std::istream &source_stream
      ,const CS::Optation *optation/*=0*/);
};
//_____________________________________________________________________________/
}//_namespace CORN_____________________________________________________________/
#endif
