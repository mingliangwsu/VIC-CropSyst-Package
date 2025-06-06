#ifndef oldbinrec_sH
#define oldbinrec_sH

#ifndef primitiveH
#  include "corn/primitive.h"
#endif
#include "corn/container/bilist.h"
#include "corn/format/binary/binary_file_fstream.h"

namespace CORN {

// Binary record format class is the primitive binary file
// I/O used for both Lotus and Biff file formats.
// Now allows for record body length of either 16 or 32 bits
// Spreadsheets use 16 bit, UED uses 32bits                  990107
//______________________________________________________________________________
class Old_binary_record_header
{
protected:
   nat32 position; // MUST BE FIRST The file position (of the header) (to beginning or rec_code)
public:
   nat16 rec_code;
   nat32 length;    // there is the option of using 16 or 32 bits for length field, in memory it is 32bits
   inline Old_binary_record_header
      (Binary_file_interface &in_file                                            //070622
      ,bool big_body_sizes)
      : position(0)
      ,rec_code(0)
      ,length(0)
      {  read_header(in_file,big_body_sizes);
      }
   inline Old_binary_record_header
      (nat16 i_rec_code
      ,nat32 i_length
      ,nat32 i_position)
      : rec_code(i_rec_code)
      , length(i_length)
      , position(i_position)
      {}
   inline virtual ~Old_binary_record_header() {}                                 //151204
   bool read_header(
      Binary_file_interface &in_file ,bool big_body_sizes);                      //070622_000129
public: // accessors
   inline nat16 get_code()                            const { return rec_code; }
   inline nat32 get_length()                          const { return length; }
   inline nat32 get_position()                        const { return position; }
};
//_Old_binary_record_header_________________________________________1999-01-07_/
class Old_binary_record_file
: public Old_binary_record_header // of the latest record read                   //990107
, public Binary_file_fstream                                                     //070622
{
protected:
   bool    is_at_eof;
   bool    big_body_sizes;                                                       //990106
public:
   Old_binary_record_file
      (const char *i_filename
      ,bool i_big_body_sizes   // True if record header size field is 32bits
      ,bool i_big_endian       // Most MS-DOS or Windows base software files are not big_endian
      ,std::ios_base::openmode _openmode);
   virtual ~Old_binary_record_file();
   void write_rec_header(nat16 code,nat32 len);
public:  // reading methods
   virtual nat16 read_rec_header();
   virtual void skip_to_record_following(const Old_binary_record_header &record_info); // header with length already read           //981016
   virtual void read_unknown_record_data(); // header with length already read                                                      //981016
   virtual bool eof()                                              performs_IO_; // cannot be const because Binary_fstream::eof is not const  //030207
public:  // identification methods
   virtual nat16 get_BOF_code() const = 0; // PURE
   virtual nat16 get_EOF_code() const = 0; // PURE
};
//_Old_binary_record_file___________________________________________1998-10-15_/
class Old_binary_header_item
: public Item
, public Old_binary_record_header
{public:
   inline Old_binary_header_item
      (Binary_file_interface &in_file
      ,bool big_body_sizes)                                                      //070622_000129
      :Item()
      ,Old_binary_record_header(in_file,big_body_sizes)
      {}
   inline Old_binary_header_item
      (nat16 i_rec_code
      ,nat32 i_length
      ,nat32 i_position)
      : Item()
      , Old_binary_record_header(i_rec_code,i_length,i_position)
      {}
   inline virtual int compare(const Item &other)                           const
   {   Old_binary_header_item &other_binary_header = (Old_binary_header_item &)other;
       return (rec_code < other_binary_header.get_code())
       ? -1 : (rec_code < other_binary_header.get_code()) ? 1 : 0;
   }
};
//_Old_binary_header_item___________________________________________1999-01-07_/
class Old_indexed_binary_record_file
:public Old_binary_record_file
{  Bidirectional_list rec_index;                                                 //001114
 public:
   Old_indexed_binary_record_file
      (const char *i_filename                                                    //000211
      ,bool i_big_body_sizes   // True if record header size field is 32bits
      ,bool i_big_endian       // Most MS-DOS or Windows base software files are not big_endian
      ,std::ios_base::openmode _openmode                                         //111108
      );
   virtual void initialize();
};
//_Old_indexed_binary_record_file___________________________________1999-01-07_/
}//_namespace_CORN_____________________________________________________________/
#endif

