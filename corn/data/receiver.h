#ifndef receiverH
#define receiverH
#include <string>
#include "corn/const.h"
#include "corn/chronometry/date_types.hpp"
#include "corn/metrology/units_code.hpp"
#include "corn/dynamic_array/dynamic_array_T.h"
#include "corn/container/binary_tree.h"
namespace CORN
{
//____________________________________________________________________________
class Detail_line // probably rename, not necessarily a line
: public implements_ CORN::Item
{
 private:
   contribute_ bool ready;
 public:
   CORN::datetime64              timestamp;
   CORN::Dynamic_array<float32>  values;
 public:
   Detail_line(CORN::datetime64  timestamp)
      : values(0,10,std::numeric_limits<float32>::quiet_NaN())
      {}
   inline bool is_ready()                           affirmation_
   {  if (!ready)
      {  ready = true;
         nat32 found_at = 0;
         if (values.occurs
               (std::numeric_limits<float32>::quiet_NaN(),found_at,0))
            ready = false;
      }
      return ready;
   }
};
//_Detail_line______________________________________________________2019-01-05_/
class Receiver
{
 protected:
   mutable CORN::Binary_tree pending; // Detail_line
 public:
   virtual nat32 take
      (float32          value
      ,CORN::Units_code units
      ,CORN::datetime64 timestamp
      ,const std::string &ontology
      ,nat32            variable_code)                                       =0;
   virtual nat32 take_vector
      (CORN::Dynamic_array<float32> vector_values
      ,CORN::Units_code units
      ,CORN::datetime64 timestamp
      ,const std::string &ontology
      ,nat32            variable_code)                                       =0;
   virtual bool know_ID(const std::string &ID)                               =0;
   // Some files may have multiple dataset
   // identified by some ID (I.e. weather station ID)
   // In such cases the reciever may need the ID to identify the target (file).
   //abandonedvirtual const std::string &get_ID()                                       =0;
   virtual const char *get_ID(std::string &ID)                               =0; //190916

   virtual bool close(const std::string &ID)                                 =0;
   // Indicates there will be no more data emitted for the current output file,
   // it can be closed.

   /*190410
   NYI
   The emitting file may have multiple sets of data
   I.e. multiple weather stations
   so when there is a change in dataset identified by whatever ID
   Ideally such IDs would have a single specification at the start of
   the time series data.
   Some file formats have such ID redundently repeated on each line.
   */
/*190715 abandoned (I don't think I ended up using this idiom)
 protected:
   virtual nat32 commit_detail_line(const Detail_line &detail_line);
 public:
      // derived classes will override if they are some sort
      // of report format that provides meta data (headers)
      // Returns the number of metadata items written.
 protected:
   virtual bool write_detail(const Detail_line &detail_line)       stream_IO_=0;
 protected:
   Detail_line &provide_detail(datetime64 timestamp)                 provision_;
*/

};
//_Receiver____________________________________________________________________/
}//_namespace_CORN_____________________________________________________________/
#endif