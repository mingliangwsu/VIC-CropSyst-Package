#ifndef textual_workbookH
#define textual_workbookH
#include "corn/spreadsheet/workbook.h"
#include "corn/tabular/tabular_format.h"
#include "corn/container/binary_tree.h"
#include "corn/container/text_list.h"

// methods that assert(false) have not yet been implemented
namespace CORN
{
namespace textual
{
extern std::string  dummy_string;
extern std::wstring dummy_wstring;
   // These dummy values are only for not yet implemented methods
//______________________________________________________________________________
class Workbook
: public implements_ CORN::Workbook
{
 public:
   //___________________________________________________________________________
   class Sheet
   : public implements_ CORN::Workbook::Sheet
   {
      std::string name; // sheet name
      const Tabular_format &format;
      std::ostream *stream;
      std::ofstream *file_stream; //owned
         // if sheet is named (not stdout)
         // then we instanciate and own the stream
    public:
      //________________________________________________________________________
      class Field
      : public implements_ Item_string
      {
       public:
         nat16 start_ndx;  // will either be column or position
       public:
         inline Field
            (nat16 start_ndx_)
            : Item_string                                                   ("")
            , start_ndx                                             (start_ndx_)
            {}
         inline virtual bool is_key_nat32(nat32 key)                affirmation_
            { return key == start_ndx;}
         inline virtual int   compare(const CORN::Item &other)             const
            {  const Field &other_field = dynamic_cast<const Field &>(other);
               return start_ndx - other_field.start_ndx;
            }
      };
      //_Field_________________________________________________________________/
      class Line
      : public implements_ Item
      {public:
         nat16 row; // index
         provided_ CORN::Binary_tree fields_tree;  //should be binary tree, but need to implement Item compare to handle nat32
         const Delineation &delineation;
       protected:
         contribute_ CORN::Dynamic_array<nat16> sat; //bool
            // bool indicating if the corresponding column/field has been
            // assigned/encountered (sat with a value)
       public:
         inline Line(nat16 row_, nat16 expected_columns_
            ,const Delineation &delineation_)
            : Item                                                            ()
            , row                                                         (row_)
            , delineation                                         (delineation_)
            {}
         Field &provide_field
            (nat16 col,nat16 start_ndx_)                             provision_;
         virtual bool write(std::ostream &)                          stream_IO_;
         bool is_ready(nat16 last_column_ndx)                      affirmation_;
         inline virtual bool is_key_nat32(nat32 key)               affirmation_
            { return key == row; }
         inline virtual int compare(const CORN::Item &other)               const
            {  const Line &other_line = dynamic_cast<const Line &>(other);
               return row - other_line.row;
            }
      };
      //_Line__________________________________________________________________/
      provided_ CORN::Bidirectional_list lines;
      // should be binary tree, but need to implement Item compare to handle nat32
      contribute_ int32 line_flushed_ndx;
         // the last row of the sheet written
         // will only match line_ultimate_ndx when none pending
      contribute_ int32 line_ultimate_ndx;
         // the last row of the sheet with data (flushed or pending)
   public:
      Sheet
         (const std::string                  &name_
         ,const Tabular_format               &format_
         ,std::ostream                       *stream_ = 0 );
      virtual ~Sheet();                                                          //190422
      // Rows and columns are indexed from 0

      // I haven't implemented all these methods yet NYI = not yet implemented.

      // The following are for setting data in the sheet
      inline virtual sint32    set_cell_date     (nat16 col, nat32 row, const CORN::Date_const &date_value)      { assert(false); return 0; } // NYI
      inline virtual bool      set_cell_bool     (nat16 col, nat32 row, bool value)                              { assert(false); return 0; } // NYI
      inline virtual sint32    set_cell_integer30(nat16 col, nat32 row, sint32 value);
         // Note: Excel only stores 30bits of integers.
         // 16bit integer records were only in BIFF2 so support for 16bit is abandoned 120105
      inline virtual float64   set_cell_number   (nat16 col, nat32 row, float64 value,nat8 precision);
      inline virtual int32     set_range_date    (const char *range, const CORN::Date_const &date_value  )       { assert(false); return 0; } // NYI
      inline virtual bool      set_range_bool    (const char *range, bool value)                                 { assert(false); return 0; } // NYI
      inline virtual sint32    set_range_integer30 (const char *range, sint32 value)                             { assert(false); return 0; } // NYI
         // Note: Excel only stores 30bits of integers.
         // 16bit integer records were only in BIFF2 so support for 16bit is abandoned 120105
      inline virtual float64   set_range_number  (const char *range, float64 value,nat8 precision)               { assert(false); return 0; } // NYI
      inline virtual bool      set_range_color   (const char *range,nat8 red,nat8 green, nat8 blue)              { assert(false); return 0; } // NYI

      inline virtual const std::string &set_cell_label (nat16 col, nat32 row, const std::string &value);
      inline virtual const std::wstring &set_cell_label_wstring(nat16 col, nat32 row, const std::wstring &value); //190113
      inline virtual const std::string &set_range_label (const char *range, const std::string &value)            { assert(false); return dummy_string; }    // NYI
      inline virtual const std::wstring &set_range_label_wstring (const char *range, const std::wstring &value)  { assert(false); return dummy_wstring; }  // NYI
      inline virtual const std::string &set_range_formula(const char *range, const std::string &formula)         { assert(false); return dummy_string; }   // NYI

          // Note I am not sure precision was used in cell records (maybe on of the attributes)
          // In later versions of Excel there is a reference to XF records
      // The following are for getting data in the sheet
      inline virtual sint32    get_cell_integer30  (nat16 col, nat32 row)      const                        { assert(false); return 0; } // NYI
      inline virtual sint32    get_range_integer30 (const char *range)         const                        { assert(false); return 0; } // NYI
         // Note: Excel only stores 30bits of integers.
         // 16bit integer records were only in BIFF2 so support for 16bit is abandoned 120105
      inline virtual bool       get_cell_bool(nat16 col, nat32 row)            const                        { assert(false); return 0; } // NYI
      inline virtual bool       get_range_bool(const char *range)              const                        { assert(false); return 0; } // NYI
      inline virtual float64   get_cell_number   (nat16 col, nat32 row)        const                        { assert(false); return 0; } // NYI
      inline virtual float64   get_range_number  (const char *range)           const                        { assert(false); return 0; } // NYI
      inline virtual bool      set_cell_color    (nat16 col, nat32 row,nat8 red,nat8 green, nat8 blue)      { assert(false); return 0; } // NYI
      inline virtual const std::wstring &get_cell_label  (nat16 col, nat32 row,std::wstring &label) const   { assert(false); return dummy_wstring; } // NYI
      inline virtual const std::wstring &get_range_label (const char *range,std::wstring &label)    const   { assert(false); return dummy_wstring; } // NYI
      inline virtual const std::string  &set_cell_formula(nat16 col, nat32 row, const std::string &formula) { assert(false); return dummy_string; }  // NYI
      inline virtual bool clear_cell(nat16 col, nat32 row)                                                  { assert(false); return 0; } // NYI
      inline virtual bool clear_range(const char *range)                                                    { assert(false); return 0; } // NYI
      inline virtual bool is_just_added_to_workbook()                     const                             { assert(false); return 0; }// NYI
      // Sheet attributes:
      inline virtual void freeze_pane(nat16 left_most_column,nat32 top_most_row )                           { assert(false);           }  // NYI
      inline virtual const std::string &get_name(std::string &sheet_name) const                             { assert(false); return sheet_name; } // NYI
      //NYI      uint8       get_index() = 0;  // the sheet tab index (0 based).

      inline virtual nat32 get_first_define_row()                         const                              { assert(false); return 0; } // NYI
      inline virtual nat32 get_first_define_column()                      const                              { assert(false); return 0; } // NYI
      inline virtual nat32 get_last_define_row()                          const  { return line_ultimate_ndx; }
      inline virtual nat32 get_last_define_column()                       const                              { assert(false); return 0; }  // NYI
    public:
      inline virtual bool is_key_string(const std::string &key)     affirmation_  //180820
         { return key == name; }
    public: // specific to this class
      virtual nat32 flush
         (nat32 up_to_row_inclusive=0xFFFFFFFF,nat16 column_count=0) stream_IO_;
         // The caller has identified that a block of lines are ready to commit.
         // The text block is probably not data columns or detail lines
         // Cells with data values are likely to be spare and
         // Lines that have been set so far (since the beginning or the last flush)
         // may have some rows and fields that blank.
    protected:
      Line &provide_line(nat16 row)                                  provision_;
      Field &provide_line_field
         (nat32 row
         ,nat16 col
         ,nat16 col_ndx)                                             provision_;
      Tabular_column *get_column(nat16 col)                               const;

      /* Conceptual: when using workbook composition mode
      when workbook saves, each sheet is written to a separate file
      as the workbook closes
      know_stream(std::ofstream &stream)
      */
   };
   //_class_Sheet______________________________________________________________/
 private:
   const Tabular_format &format;
   provided_ Unidirectional_list        sheets;
   std::ostream                        *stream;  // Used in single sheet streaming mode
 public:
   inline Workbook(const Tabular_format &format_)
      : CORN::Workbook                                                        ()
      , format                                                         (format_)
      , stream                                                               (0)
      {}
      // This constructor is used when we compose the workbook in memory
      // then save it.
   inline Workbook
      (const Tabular_format &format_
      ,std::ostream &stream_)
      : CORN::Workbook                                                        ()
      , format                                                         (format_)
      , stream                                                        (&stream_)
      {}
      // This constructor is used when streaming output
      // (generating a report or converting formats to a single stream).
   // Conceptual need need construtor for streaming input
   // Workbook(const char *filename);                                               //191104
   virtual ~Workbook();
 public:
   virtual Sheet *provide_sheet(const std::string &sheet_name)       provision_;
   // Sets the specified sheet as active (Create if necessary).
   // For spreadsheet formats that only support single sheets,
   // this may be the name of the file for the sheet.
   // Returns a pointer to the sheet.
   // The sheet is owned by the workbook.
   inline virtual const std::string &get_name(std::string &return_name)   const
      {  UNUSED_arg(return_name);
         assert(false); return dummy_string; }
   // Derived classes should return the name of the workbook
   // (This may be the fully qualified filename.
   /*110725 conceptual:
   virtual bool copy_sheet
      (const std::string &from_sheet_name
      ,const std::string &to_sheet_name)                           modification_=0; //110711
   */
   inline virtual bool save(bool display_alerts=false) stream_IO_ {return true;}
   inline virtual bool save_as
      (const std::wstring &filename,bool display_alerts = false)     stream_IO_
      { assert(false); return 0; }
          virtual bool flush()                                       stream_IO_; //190107
   virtual bool close_sheet(const std::string &sheet_name)           stream_IO_; //190422
};
//_class_Workbook_textual___________________________________________2019-01-06_/
}}//_namespace_CORN:textual____________________________________________________/
#endif

