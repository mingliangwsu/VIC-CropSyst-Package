#ifndef workbookH
#define workbookH
#include "corn/container/bilist.h"
#include "corn/const.h"
#include "corn/chronometry/date_32.h"
#include "corn/string/strconv.hpp"
namespace CORN
{
//______________________________________________________________________________
interface_ Workbook
{
 protected:
   provided_ Bidirectional_list sheets;   // move this to implementing classes
      // Now should be able to use Unidirectional_list
 public:
   class Sheet  : public Item
   {public:
      inline Sheet()                                                          {}
      // CELL records were used in older versions of Excel with only 16bit row indexes
      // BIFF8 introduced new records for cell data that allowed for 32bit row indexes

      // Rows and columns are indexed from 0
      // (but note that Excel cell addressing presented to the user is 1 based)

      // The following are for setting data in the sheet
      virtual sint32    set_cell_date     (nat16 col, nat32 row, const CORN::Date_const &date_value) =0;
      virtual bool      set_cell_bool     (nat16 col, nat32 row, bool value)                         =0;
      virtual sint32    set_cell_integer30(nat16 col, nat32 row, sint32 value)=0; //120105
         // Note: Excel only stores 30bits of integers.
      /* 16bit integer records were only in BIFF2, so this is quite deprecated   120105
      virtual sint16    set_cell_integer  (nat16 col, nat32 row, sint16 value)   = 0;
      */
      virtual float64   set_cell_number   (nat16 col, nat32 row, float64 value,nat8 precision) = 0;
      virtual int32     set_range_date    (const char *range, const CORN::Date_const &date_value  ) = 0;
      virtual bool      set_range_bool    (const char *range, bool value)    =0;
      virtual sint32    set_range_integer30(const char *range, sint32 value) =0;
         // Note: Excel only stores 30bits of integers.
      /* 120105 16bit integer records were only in BIFF2, so this is quite deprecated
      virtual sint16    set_range_integer (const char *range, sint16 value)      = 0;
      */
      virtual float64   set_range_number  (const char *range, float64 value,nat8 precision) = 0;
      virtual bool      set_range_color   (const char *range,nat8 red,nat8 green, nat8 blue) = 0;  //110815

      virtual const std::string  &set_cell_label         (nat16 col, nat32 row, const std::string  &value) = 0;
      virtual const std::wstring &set_cell_label_wstring (nat16 col, nat32 row, const std::wstring &value) = 0; //130901
      virtual const std::string  &set_range_label        (const char *range   , const std::string  &value) = 0;
      virtual const std::wstring &set_range_label_wstring(const char *range   , const std::wstring &value) = 0; //130901
      virtual const std::string  &set_range_formula      (const char *range   , const std::string  &formula) = 0;    //110815

          // Note I am not sure precision was used in cell records (maybe on of the attributes)
          // In later versions of Excel there is a reference to XF records
      // The following are for getting data in the sheet
      /* 120105 16bit integer records were only in BIFF2, so this is quite deprecated

      virtual sint16    get_cell_integer  (nat16 col, nat32 row)          nst=0;
      virtual sint16    get_range_integer (const char *range)             nst=0;        //111001
      */
      virtual sint32    get_cell_integer30  (nat16 col, nat32 row)      const=0;
      virtual sint32    get_range_integer30 (const char *range)         const=0; //111001
         // Note: Excel only stores 30bits of integers.
      virtual bool      get_cell_bool(nat16 col, nat32 row)             const=0; //130328
      virtual bool      get_range_bool(const char *range)               const=0; //130328
      virtual float64   get_cell_number   (nat16 col, nat32 row)        const=0;
      virtual float64   get_range_number  (const char *range)           const=0;
      virtual bool      set_cell_color    (nat16 col, nat32 row,nat8 red,nat8 green, nat8 blue) = 0;  //110815
      virtual const std::wstring &get_cell_label  (nat16 col, nat32 row,std::wstring &label) const=0; //111001
      virtual const std::wstring &get_range_label (const char *range,std::wstring &label)    const=0; //111001
      virtual const std::string  &set_cell_formula(nat16 col, nat32 row, const std::string &formula) =0;  //110815
//Excel does not have cell type for date      virtual int32 get_cell_date(nat16 col, nat32 row) = 0;
// Probably stored as a string (label) with a specified format
#ifdef DISABLED
101127
This is dependent on get_cell which I needed to disable.
instead, use Cell_reader
      virtual bool is_cell_empty(nat16 col, nat32 row)                  const=0;
      // returns true if the cell has no data
#endif
      virtual bool clear_cell(nat16 col, nat32 row)                          =0;
      virtual bool clear_range(const char *range)                            =0;
      virtual bool is_just_added_to_workbook()                          const=0; //110711
      // Sheet attributes:
      virtual void freeze_pane(nat16 left_most_column,nat32 top_most_row )   =0;
      virtual const std::string &get_name(std::string &sheet_name)      const=0;   // the name of the sheet
//NYI      uint8       get_index() = 0;  // the sheet tab index (0 based).
//181001      virtual const char *get_key()                                     const=0;  replace with is_key()
      inline virtual bool is_key_string(const std::string &key)  affirmation_=0; //181001
      virtual nat32 get_first_define_row()                              const=0;
      virtual nat32 get_first_define_column()                           const=0;   // Excel only has 16bit column indexes
      virtual nat32 get_last_define_row()                               const=0;
      virtual nat32 get_last_define_column()                            const=0;   // Excel only has 16bit column indexes
    public:
         // Flushing methods are applicable for specializations that
         // buffer when streaming output (I.e. Textual::Workbook::Sheet)
      /*190112
      inline virtual nat32 flush_completed
         (nat32 up_to_row_inclusive,nat16 column_count)    stream_IO_{return 0;} //190107
         ///< Writes all lines that appear to be completed.
         /// Not all lines may have data expect in all cells so flushing
         /// stops at the first incomplete line.
         /// This is used to commit output where the block of cells are not
         /// sparse and it is expected that all cells will be set with values.
         /// \return latest line/row number that was complete and flushed.
      virtual nat32 flush()                               stream_IO_{ return 0;}
         ///<
      */
      inline virtual nat32 flush
         (nat32 to_row_ndx   = 0
         ,nat16 column_count = 0)     stream_IO_ { return 0; }
         ///< When column_count: write only lines that appear to be completed.
         /// In this case not all lines may have data expect in all cells so flushing
         /// stops at the first incomplete line.

         /// Specifying column count
         /// is used to commit output where the block of cells are not
         /// sparse and it is expected that all cells will be set with values.
         /// 0 indicates all cell/columns

         /// \return latest line/row number that was complete and flushed.

         /// when to_row_ndx is 0 all pending rows are written
         /// (unless expected columns are not encountered)
         /// This is used when
         /// This is used to commit output where the block of cells are sparse.
         /// and cells that will be set with values have been set and lines set
         /// so far will not have further columns set so write what is there.
         /// \return the ultimate row number (so far)
   };
 public:
   inline Workbook()
      : sheets()
      {}
   inline virtual ~Workbook()                                       { flush(); }
   inline virtual bool finalize()                                  finalization_
      { return flush(); }
   virtual const std::string &get_name(std::string &return_name)        const=0;
   // Derived classes should return the name of the workbook
   // (This may be the fully qualified filename.
   /*110725 NYN
   virtual bool copy_sheet
      (const std::string &from_sheet_name
      ,const std::string &to_sheet_name)                           modification_=0; //110711
   */
   virtual bool save(bool display_alerts = false)                  stream_IO_=0; //111215
   virtual bool save_as
      (const std::wstring &filename,bool display_alerts = false)   stream_IO_=0; //140101_111215

   inline virtual bool flush()                        stream_IO_ { return true;} //190107
      ///< flush all sheets
      /// \return if any any sheets flush any lines.
      /// false is not an error.

   virtual Sheet *provide_sheet(const std::string &sheet_name)     provision_=0;
   inline virtual Sheet *provide_sheet(const std::wstring &sheet_name)provision_ //190803
      {  std::string sheet_name_str;
         CORN::wstring_to_string(sheet_name,sheet_name_str);
         return provide_sheet(""); // (sheet_name_str);
      }
   // Sets the specified sheet as active (Create if necessary).
   // For spreadsheet formats that only support single sheets,
   // this may be the name of the file for the sheet.
   // Returns a pointer to the sheet.
   // The sheet is owned by the workbook.

   /*Conceptual
   virtual Sheet *provide_sheet_primary()                          provision_=0;
   // This returns the primary/first (and possibly only) sheet
   // In Excel this is typically, but not necessarily named Sheet1.
   */

   virtual Sheet *copy_sheet                                                     //190802
      (const std::string &from_sheet_name
      ,const std::string &to_sheet_name)
      {return 0;}
      // Derived classes should implement, but I don't currently use this
   inline virtual bool close_sheet(const std::string &sheet_name)     stream_IO_ //190422
      {return false; }
      // Currently the text workbook can close specific sheets.
      // Excel workbooks keep all sheets in memory so closing individual sheets is not applicable.
   virtual bool set_updating(bool enabled)       contribution_ { return false; }
      // This is usually only applicable to Excel OLE where
      // the application is displayed to the screen and would other
      // wise be updated continuously by default
};
//_Workbook____________________________________________________________________/
}//_namespace_CORN_____________________________________________________________/
#endif

