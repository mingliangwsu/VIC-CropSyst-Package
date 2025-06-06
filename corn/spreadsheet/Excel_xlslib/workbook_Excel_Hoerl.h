#ifndef workbook_Excel_HoerlH
#define workbook_Excel_HoerlH
#include "corn/spreadsheet/workbook.h"
// Hoerls source directory needs to be added to the IDE C++ include paths
// C:\dev_other\xlslib\trunk\xlslib\src
// C:\dev_other\xlslib\trunk\xlslib\build\RadStudio
#include "xlslib/workbook.h"

#if (__BCPLUSPLUS__ > 0x0600)
//#define LIBXLS_WORKS
      // NYI for BDS
#endif
#ifdef LIBXLS_WORKS
#  include "C:\dev_other\libxls\trunk\libxls\cplusplus\XlsReader.h"
#endif
#include "corn/OS/directory_entry_name_concrete.h"
namespace CORN
{
namespace Hoerl
{
//______________________________________________________________________________
class Workbook // was Workbook_Excel
: public implements_ CORN::Workbook
{
   // This version of the Excel workbook uses xlslib to implement I/O
 public:
   class Sheet
   : public CORN::Workbook::Sheet
   {private:
      std::string name;
      nat16       index;
      xlslib_core::worksheet  *worksheet_out;
      #ifdef LIBXLS_WORKS
      xls::WorkBook        *workbook_in;                                         //130401
      #endif
    public:
      Sheet(xlslib_core::worksheet *worksheet_   ,const std::string &_name);
      #ifdef LIBXLS_WORKS
      Sheet
         (xls::WorkBook       *in_workbook_                                      //130401
         ,nat16                index_
         ,const std::string   &name_);                                           //130401
      #endif
      // CELL records were used in older versions of Excel with only 16bit row indexes
      // BIFF8 introduced new records for cell data that allowed for 32bit row indexes

      // The following are for setting data in the sheet
      virtual sint32    set_cell_date     (nat16 col, nat32 row, const CORN::Date_const &date_value);
      virtual bool      set_cell_bool     (nat16 col, nat32 row, bool value);
      virtual sint32    set_cell_integer30(nat16 col, nat32 row, sint32 value);  //120105
      virtual float64   set_cell_number   (nat16 col, nat32 row, float64 value,nat8 precision);
      virtual sint32    set_range_date    (const char *range, const CORN::Date_const &date_value);
      virtual bool      set_range_bool    (const char *range, bool value);
      virtual sint32    set_range_integer30 (const char *range, sint32 value);
      virtual float64   set_range_number  (const char *range, float64 value,uint8 precision);
      virtual bool      set_range_color   (const char *range,nat8 red,nat8 green, nat8 blue);

      virtual const std::string &set_cell_label   (nat16 col, nat32 row, const std::string &value);
      virtual const std::wstring &set_cell_label_wstring(nat16 col, nat32 row, const std::wstring &value); //130905

      virtual const std::string &set_range_label  (const char *range, const std::string &value);
      virtual const std::wstring &set_range_label_wstring(const char *range, const std::wstring &value); //130901
      virtual const std::string &set_range_formula(const char *range, const std::string &formula);

      // Note I am not sure precision was used in cell records (maybe on of the attributes)
      // In later versions of Excel there is a reference to XF records.
      // The following are for getting data in the sheet.
      virtual sint32    get_cell_integer30  (nat16 col, nat32 row)        const;
      virtual sint32    get_range_integer30 (const char *range)           const;
      virtual bool      get_cell_bool(nat16 col, nat32 row)               const; //130401
      virtual bool      get_range_bool(const char *range)                 const; //130401

      virtual float64   get_cell_number   (nat16 col, nat32 row)          const;
      virtual float64   get_range_number  (const char *range)             const;
      virtual bool      set_cell_color    (nat16 col, nat32 row,nat8 red,nat8 green, nat8 blue);

      virtual const std::wstring &get_cell_label  (nat16 col, nat32 row,std::wstring &label)   const;
      virtual const std::wstring &get_range_label (const char *range,std::wstring &label)      const;
      virtual const std::string  &set_cell_formula(nat16 col, nat32 row, const std::string &formula);
//Excel does not have cell type for date      virtual int32 get_cell_date(nat16 col, nat32 row) = 0;
// Probably stored as a string (label) with a specified format
#ifdef DISABLED
101127
This is dependent on get_cell which I needed to disable.
instead, use Cell_reader
      virtual bool is_cell_empty(nat16 col, nat32 row)                    const;
      // returns true if the cell has no data
#endif
      virtual bool clear_cell(nat16 col, nat32 row);
      virtual bool clear_range(const char *range);
      virtual bool is_just_added_to_workbook()                            const;
      // Sheet attributes:
      virtual void freeze_pane(nat16 left_most_column,nat32 top_most_row );
      virtual const std::string &get_name(std::string &sheet_name)        const;
      //NYI uint8 get_index() = 0;  // the sheet tab index (0 based).
      //virtual const char *get_key()                                       const;  replace with is_key
      inline virtual bool is_key_string(const std::string &key)    affirmation_  //181001
      { return name == key; }

      virtual nat32 get_first_define_row()                                const;
      virtual nat32 get_first_define_column()                             const;
      virtual nat32 get_last_define_row()                                 const;
      virtual nat32 get_last_define_column()                              const;
   };
   //_sheet____________________________________________________________________/
 private:
   std::ios_base::openmode  openmode;                                            //130401
   xlslib_core::workbook   *workbook_out;
   #ifdef LIBXLS_WORKS
   xls::WorkBook           *workbook_in;                                         //130401
   #endif
   CORN::OS::File_name_concrete filename_XLS;                                    //151027
 public:
   Workbook
      (const char *_workbook_filename_XLS                                        //130901
      ,std::ios_base::openmode _openmode);                                       //130401
   ~Workbook();
   virtual Workbook::Sheet *provide_sheet(const std::string &sheet_name) provision_;
   inline virtual Workbook::Sheet *provide_sheet                                 //191216
      (const std::wstring &sheet_name) provision_
      { return dynamic_cast<Workbook::Sheet *>
          (CORN::Workbook::provide_sheet(sheet_name)); }                         //191228
   // Sets the specified sheet as active (Create if necessary).
   // For spreadsheet formats that only support single sheets,
   // this may be the name of the file for the sheet.
   // Returns a pointer to the sheet.
   // The sheet is owned by the workbook.
   virtual const std::string &get_name(std::string &return_name)          const;
   // Derived classes should return the name of the workbook
   // (This may be the fully qualified filename.
   virtual bool save(bool display_alerts = false)                    stream_IO_;
   virtual bool save_as
      (const std::wstring &filename,bool display_alerts = false)     stream_IO_; //140516_111215
};
//_class_Workbook_Excel_____________________________________________2011-12-09_/
}}//_namespace_CORN_Hoerl______________________________________________________/
#endif
