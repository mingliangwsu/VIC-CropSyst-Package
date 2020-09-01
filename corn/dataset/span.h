#ifndef spanH
#define spanH
#include "corn/primitive.h"
namespace CORN {
//_______________________________________________________________________________/
class Span
{  nat32 max_index;
   nat32 from_index;
   nat32 to_index;
   nat32 first_count;
   nat32 last_count;
 public:
   Span(nat32 count_ = 0xFFFFFFFF);
   Span &from   (nat32 index);
   Span &to     (nat32 index);
   Span &first  (nat32 first_count);
   Span &last   (nat32 last_count);
   Span &resize(nat32 count_);
   inline nat32 get_from()                            const {return from_index;}
   inline nat32 get_to()                              const {return to_index;}
};
//__________________________________________________________________2020-04-26_/
}//_namespace_CORN_____________________________________________________________/
#endif
