#ifndef emitterH
#define emitterH
#include "corn/const.h"
#include "corn/primitive.h"
#include "corn/chronometry/date_types.hpp"
#include "corn/container/container.h"
#include "corn/dynamic_array/dynamic_array_T.h"
namespace CORN
{
//____________________________________________________________________________
class Receiver;
//____________________________________________________________________________
interface_ Emitter
{
 public:
//   Emitter(const char *source_filename);
   //virtual ~Emitter();
   virtual nat32 emit_temporal
      (CORN::Receiver &receiver
      ,CORN::datetime64 time_earliest
      ,CORN::datetime64 time_latest)                               stream_IO_=0;
   virtual ~Emitter()                                                         {}
};
//____________________________________________________________________________
}//_namespace_CORN_____________________________________________________________/
#endif