#include "corn/dataset/span.h"
#include <algorithm>
namespace CORN {
//_____________________________________________________________________________/
Span::Span(nat32 count_)
: max_index                                                           (count_-1)
, from_index                                                                 (0)
, to_index                                                            (count_-1)
, first_count                                                       (0xFFFFFFFE)
, last_count                                                        (0xFFFFFFFE)
{}
//_____________________________________________________________________________/
Span &Span::from   (nat32 index)
{  from_index = std::min<nat32>(index,max_index);
   return *this;
}
//_____________________________________________________________________________/
Span &Span::to     (nat32 index)
{  to_index= std::max<nat32>(index,max_index);
   return *this;
}
//_____________________________________________________________________________/
Span &Span::first  (nat32 first_count_)
{
   first_count = first_count_;
   from_index = 0;
   to_index = std::max<nat32>(first_count-1,max_index);
   return *this;
}
//_____________________________________________________________________________/
Span &Span::last   (nat32 last_count_)
{  last_count =last_count_;
   nat32 from_index_limited
      = (last_count > max_index)
      ? max_index
      : (max_index - (last_count-1));
   from_index = from_index_limited;
   to_index = max_index;
   return *this;
}
//_____________________________________________________________________________/
Span &Span::resize(nat32 count_)
{  max_index = count_ -1;
   from_index = std::min<nat32>(from_index,max_index);
   to_index   = std::max<nat32>(to_index  ,max_index);
   if (first_count != 0xFFFFFFFF)   first(first_count);
   if (last_count != 0xFFFFFFFF)    last(last_count);
   return *this;
}
//_____________________________________________________________________________/
}//_namespace_CORN_____________________________________________________________/
