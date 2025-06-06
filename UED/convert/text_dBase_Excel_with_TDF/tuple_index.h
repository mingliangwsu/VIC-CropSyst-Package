#ifndef tuple_indexH
#define tuple_indexH
#include "corn/container/item.h"
#include "corn/chronometry/date_time_64.h"
#include "UED/library/UED_types.h"
#	include "corn/dynamic_array/dynamic_array_T.h"
using namespace CORN;

namespace CORN
{
   class Bidirectional_list;
}
namespace UED
{
class Export_tuple;
//______________________________________________________________________________
class Searchable_tuple
: public CORN::Item
{public:
    CORN::Date_time_clad_64 date_time;                                           //170531
 public:
   Searchable_tuple(CORN::datetime64 date_time_)                                 //170531
   :Item()
   ,date_time(date_time_)                                                        //170531
   {}
  inline virtual int compare(const Item &other)                            const //980511
   { return date_time.temporal_compare(((Searchable_tuple &)other).date_time);
   }
};
//_1999-05-06___________________________________________________________________
class Export_tuple
: public Searchable_tuple
{
    // The tuple represents one row of data
    // When dates and times are to go in a column,
    // the corresponding values array will not be set to a value.
 public:
   Dynamic_array<float32> data;   // previously was superclass                   //090709
 public:
   inline Export_tuple(CORN::datetime64 date_time_)                              //170531
      :Searchable_tuple(date_time_)
      ,data(0.0,20,0.0)                                                          //090709
      {}
   inline nat32 get_count()                    const { return data.get_count();} //090714
};
typedef Export_tuple *  Export_tuple_pointer;
//_1999-05-06___________________________________________________________________
class Tuple_index
{  Units_code              level_time_step;
   bool                    tuple_level;  // True if tuple level, else it is a subindex level
   Tuple_index  *          sublevel_index;
   Export_tuple_pointer *  tuples;
   nat16                   count;
   Year                    first_year;
   bool                    established;
 public:
   // This creates an index for a specified level and all levels underneath
   // If the time_steps match, then this level will contain tuples not sublevels.
   Tuple_index();
   virtual ~Tuple_index();
   Export_tuple *find_matching
      (const CORN::Date_time_clad_64 &index_date_time);
      // This probably could be Date_time_interface
   void setup
      (Units_code   tuple_time_step     // This is the timestep of the output file detail lines
      ,Units_code   level_time_step   // This is the timestep of this index level
      ,Year         year = 0
      ,nat16        years= 0);               // Optional
   nat32 extract_tuples(CORN::Bidirectional_list &sorted_tuples)  renunciation_;
   // This traverses all the levels and removes all the tuples
   // appending them to the sorted_tuples list.
   // This sublevel indexes are all destroyed.
};
//______________________________________________________________________________
}//_namespace_UED______________________________________________________________/
#endif

