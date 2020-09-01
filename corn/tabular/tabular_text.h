#ifndef tabular_textH
#define tabular_textH
#include "corn/container/text_list.h"
#include "corn/tabular/file.h"
#include "corn/tabular/delineation.h"
#include "corn/tabular/tabular_format.h"
#ifndef dynamic_array_TH
#  include "corn/dynamic_array/dynamic_array_T.h"
#endif
#include <ostream>
#define CORN_MAX_DELINEATED_COLUMNS 100
namespace CORN
{
//______________________________________________________________________________
interface_ Data_stream
{  inline Data_stream(){}
   virtual const char *read_raw_line()                              = 0;
   virtual bool write_raw_line(const char *raw_line)                = 0;//050612
   inline virtual bool at_end()                      stream_IO_ { return false;}
   inline virtual bool good()                             const { return false;} //110822
   virtual bool remember_start_of_data()                                    = 0; //040510
   virtual bool remember_data_line_position()                               = 0; //050410
   virtual bool goto_start_of_data()                                        = 0; //050410
   virtual bool goto_data_line(nat32  zero_based_index)                     = 0; //040510
   virtual std::streampos get_file_position()                    stream_IO_ = 0; //111107
   virtual bool goto_file_position(std::streampos file_pos)      stream_IO_ = 0; //111107
   inline virtual bool reset()                      stream_IO_ { return false; } //141210_111113
   inline virtual bool clear_EOF()             stream_IO_=0;// { return false; } //190228
   inline virtual bool deallocate()                 stream_IO_ { return true;  } //150505
};
//_Data_stream______________________________________________________2005-03-27_/
class File_data_stream
: public Data_stream
{protected:
   std::string    filename;   //currently only used for debugging                //111111
   std::string    raw_data_line;
   bool           record_data_positions;                                         //111110
   CORN::Dynamic_array<nat32> data_record_position;                              //111110
   // This array records the file position of the start of each data record
   // Note this does not include any pretext or header lines, it is the start of the tabular data
   // it is used to implemente Data_source::goto() methods
public:
   File_data_stream
      (bool _record_data_positions);                                             //141009_111110
   inline virtual bool write_raw_line(const char */*raw_line*/) { return false;} //141009
   virtual bool remember_start_of_data();                                        //040510
   virtual bool remember_data_line_position();                                   //050410
   virtual bool goto_start_of_data();                                            //050410
   virtual bool goto_data_line(nat32  zero_based_index);                         //040510
   inline virtual std::streampos get_file_position() stream_IO_{return tellX();} //111107
   virtual bool goto_file_position(std::streampos file_pos)          stream_IO_;
 protected:
   virtual std::streampos seekX
      (std::streamoff off, std::ios_base::seekdir way = std::ios_base::beg) = 0; //141009
   virtual std::streampos tellX()=0;                                             //141009
};
//_File_data_stream____________________________________________________________/
class File_data_istream
: public File_data_stream
{
   std::istream *raw_data_xistream;
 public:
   inline File_data_istream
      (std::istream *in_stream, bool /*owns_raw_data_stream_*/
      ,bool          record_data_positions_)
      : File_data_stream(record_data_positions_)
      , raw_data_xistream(in_stream)
      {}
   File_data_istream
      (const char *_filename
      ,bool        _record_data_positions);
   virtual const char *read_raw_line();
 protected:
   inline virtual std::streampos seekX
      (std::streamoff offset, std::ios_base::seekdir way = std::ios_base::beg)
      { raw_data_xistream->seekg(offset,way); return tellX(); }                  //141009
   inline virtual std::streampos tellX()   { return raw_data_xistream->tellg();} //141009
   inline virtual bool at_end() performs_IO_ { return raw_data_xistream->eof();}
   inline virtual bool good()         const { return raw_data_xistream->good();} //110822
   inline virtual bool deallocate()                                   stream_IO_ //150505
      { delete raw_data_xistream; return true; }
   inline virtual bool clear_EOF()                                    stream_IO_
      { raw_data_xistream->clear(); return true; };                              //190228
};
//_File_data_istream________________________________________________2014-10-09_/
class File_data_ostream
: public File_data_stream
{
   std::ostream *raw_data_ostream;
   bool owns_raw_data_stream;                                                    //160306
 public:
   inline File_data_ostream
      (std::ostream *out_stream
      ,bool          owns_raw_data_stream_
      ,bool          record_data_positions_)
      : File_data_stream      (record_data_positions_)
      , raw_data_ostream      (out_stream)
      , owns_raw_data_stream  (owns_raw_data_stream_)
      {}
   inline File_data_ostream
      (const char *_filename
      ,bool        _record_data_positions);

   virtual ~File_data_ostream();
   inline virtual const char *read_raw_line()                       { return 0;} //150327 can't read output
   virtual bool write_raw_line(const char *raw_line) ;                           //050612
  protected:
   inline virtual std::streampos seekX
      (std::streamoff offset, std::ios_base::seekdir way = std::ios_base::beg)
      { raw_data_ostream->seekp(offset,way); return raw_data_ostream->tellp(); } //141009
   inline virtual std::streampos tellX()
      { return raw_data_ostream->tellp(); }                                      //141009
   inline virtual bool good()         const { return raw_data_ostream->good(); } //110822
   inline virtual bool clear_EOF()                                    stream_IO_ //190228
      { raw_data_ostream->clear(); return true; }
   inline virtual bool deallocate()                                   stream_IO_ //150505
      { delete raw_data_ostream; return true; }
};
//_File_data_ostream________________________________________________2014-10-09_/
class C_str_data_stream
: public Data_stream
, public CORN::Text_list
{public:
   class Iterator : public CORN::Text_list::Iterator
   {  // This iterator keeps two to head references
      // the actual head and the head of the data line
    private:
      CORN::Container::Node *start_of_data_node;
    public:
      Iterator( CORN::Text_list    &container);
      CORN::Item *Ith_data_line(nat32  i) ;                                      //991216
      void remember_start_of_data_node();                                        //050410
   };
   friend class C_str_data_stream::Iterator;                                     //030603
   Iterator *line_iterator;
public:
   C_str_data_stream(const char *raw_data_c_str);
   virtual ~C_str_data_stream();
   virtual const char *read_raw_line();
   virtual bool write_raw_line(const char *raw_line) ;                           //050612

   virtual bool at_end()                                             stream_IO_; // was { return curr_line == 0; }
   virtual bool good()                                                    const; //110822
   virtual bool goto_start_of_data();                                            //050410
   virtual bool remember_start_of_data() ;                                       //040510
   virtual bool remember_data_line_position();                                   //050410
   virtual bool goto_data_line(nat32  zero_based_index);                         //040510
public:
   virtual Container::Iterator *iterator() const;                                //030603
   inline virtual std::streampos get_file_position()     stream_IO_{ return -1;} //111107
   inline virtual bool goto_file_position(std::streampos) stream_IO_{ return false; } // NYI
   inline virtual bool reset()                         stream_IO_{ return true;} //111111
   inline virtual bool clear_EOF()                     stream_IO_{ return true;} //190228
};
//_C_str_data_stream___________________________________________________________/

class Tabular_text_file : public Tabular_file
{
 private:
   const Tabular_format /*200220 CORN::Common_tabular_file__format*/ &format;  //170327
   int data_line_count;
   bool at_end_data_marker;                                                      //050322
 protected:
   bool record_data_positions;                                                   //111110
   Data_stream *data_stream;                                                     //050407
   std::ios_base::openmode openmode;                                             //141111
 private: // The following are used only when reading
   const char *raw_data_c_str;    // raw_data_c_str is always 0 when reading text file mode  //050327
 private: // The following are used only when writing
   std::string write_buffer;                                                     //070203
 public:
   Tabular_text_file
      (const std::string &filename
      ,std::ios_base::openmode    openmode                                       //141111
      ,bool                       keep_read_records_in_memory
      ,const Tabular_format/*200220 Common_tabular_file__format*/ &format                                  //170327
      ,bool record_data_positions                                                //111110
      ,Indexer          *optional_indexer = 0);                                  //111024
       ///< Constructor for raw data read from file
   Tabular_text_file                                                             //050322
      (const char          *_raw_data_c_str
      ,bool                 _keep_read_records_in_memory
      ,const Tabular_format/*200220 Common_tabular_file__format*/ &format                                  //170327
      , bool _record_data_positions                                              //141008_111110
      ,Indexer *optional_indexer = 0);                                           //111024
      ///< Constructor for raw data to be read is is passed in char array
   Tabular_text_file                                                             //141008
      (std::ostream *_output_stream
      ,const Tabular_format/*200220 Common_tabular_file__format*/ &format                                  //170327
      , bool _record_data_positions);
         // Constructor when writing to a stream
   virtual ~Tabular_text_file();                                                 //070203
 public: // Data_source overrides
   virtual bool get(Data_record &data_rec);                                      //050406
   virtual bool get_next(Data_record &data_rec);                                 //050406
   virtual bool goto_first();                                                    //050410
      ///< Goto the first line in the text file with detail line data (not header or column header)  //050410
   virtual bool goto_index(nat32  zero_based_index);                             //050410
   inline virtual bool   goto_last()                           { return false; } // NYI (/NYN)                                  //050410
   inline virtual bool   goto_next(int32 /*skip_records = 1*/) { return false; } // NYI (/NYN)     //050410
      ///< skip records is allowed to be signed                                    //050410
/*161013 field type is actually format specification,
   inline virtual nat8  get_field_type(const char * field_name)              { return false; } // NYI (/NYN) //050410
      ///< returns the filed type corresponding to VV_entry types                  //050410
*/
   inline virtual nat32  get_current_index()                   { return false; } // NYI  // 050410
      ///< \returns the index value for the table's current position.                  //050410
      /// For database tables this is the record number                              //050410
      /// For memory tables, this is the memory location or object pointer);         //050410
   inline virtual  std::streampos get_file_position()                 stream_IO_
      { return data_stream?data_stream->get_file_position():(std::streampos)-1;} //111107
   inline virtual bool reset()                                        stream_IO_
      {  bool resat = data_stream ? data_stream->reset() : false;
         data_stream->clear_EOF();                                               //190228
            // If previously reached the end of file, the EOF bit is set.
            // reset() does not clear this bit.
         at_end_data_marker = false;                                             //191001
         return resat; }                                                         //111111
   inline virtual nat32  append(Data_record & /*data_rec*/)    { return false; } // NYI (/NYN)         //050410
      ///< Create a new record at the end of the table and sets data_rec.
      /// \returns the record index of the new record that was appended.              //050410
   inline virtual nat32  delete_record()                       { return false; } // NYI (/NYN)                       //050410
      ///< Mark the current record as deleted.                                   //050410
      /// Usually only a flag is set in the record indicating it is deleted      //050410
      /// so the record is not actually physically deleted.                      //050410
   virtual const char *label_string(std::string &buffer)                  const;
protected:
   virtual bool set_data(Data_record &data_rec,bool append);                     //150505_050705
protected:
   Data_stream* provide_data_stream();                                           //141111
   virtual bool redelineate_if_necessary();
public:
   virtual const std::string *get_Nth_header(int header_row_number);
               // 1 indexed
               // returns a pointer to the specified header row string.
               // return 0 if no header at the specified row.
   virtual int get_column_count();
   // returns the number of columns identified in the table.
   virtual int get_column_width(int column);
   // returns the width (in characters of the specified column
   inline virtual Relative_to get_relative_to_labeled()                    const //060823
      { return format.document.extent.relative_to_clad.get(); }                  //200220
      //200220 { return format.relative_to_clad.get(); }

      //200220 check if caller can just access the delineation directly (obsolete?)

   inline virtual int get_start_data_row_1_based()                         const
      { return format.detail/*200220 detail_lines_delineation*/.extent.begin_offset+1; }          //200220
      //200220 { return format.detail_lines_delineation.extent.start_row_number; }        //171110
   // returns the starting row (1 based )
   inline virtual nat32  get_record_count()                     // should be const need to check ancestors and decendents  //050407
      { return data_line_count; }
      // returns the number of lines of data currently known in the file;
      // Returns the number of records in the table                              //050410
      // Includes only data lines, not any headers or pretext.                   //050410
 protected: // The following methods apply to writing files (overrides Tabular_file);
   virtual bool goto_raw_line(nat32 line_number_1_based);                        //070203
   virtual bool goto_field_column(nat32 column_number_1_based);                  //070203
   virtual nat32 write_delimiter_string(const char *c_str);                      //070203
   virtual nat32 write_raw_string(const char *c_str);                            //070203
   virtual nat32 write_field_string                                              //070203
      (const char *c_str
      //200220 ,CORN::Delineation::Text_quote_mode quote_mode
      ,const std::string &quotation);                                            //200220
 public:
   void dump_column_header_labels(Data_record &data_rec);                        //070307
   // This isn't currently used
};
//_Tabular_text_file________________________________________________1998-06-12_/
}//_namespace CORN_____________________________________________________________/
#endif

