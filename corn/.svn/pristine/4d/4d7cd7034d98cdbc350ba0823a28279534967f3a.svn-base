#ifndef workbook_Excel_OLEH
#define workbook_Excel_OLEH
#include <vcl.h>
#pragma hdrstop
#include "corn/spreadsheet/workbook.h"
#ifdef Excel_SRVR_XP
//as of 191016 I Dont have XP version working yet
// some of the XP function calls have have slightly different parameters
#include "Excel_XP_SRVR.h"
#define Excel_NS Excel_XP
// Link bcbofficeexp.lib
#else
#include "Excel_2K_SRVR.h"
#define Excel_NS Excel_2k
// Link bcboffice2k.lib
#endif
#include "corn/OS/directory_entry_name_concrete.h"

/*
To use these Excel_OLE classes (with BC++ Builder without VCL),
it is necessary to link:
embarcadero/radstudio/7.0
rtl.lib
cp32mt.lib
vclstd.lib
vclx.lib
*/

// Rows and columns are all 0 base indexed
namespace CORN {
namespace Excel_OLE {
//______________________________________________________________________________
class Workbook // early or late binding abstract
: public implements_ CORN::Workbook
{
 public:
   class Sheet
   : public CORN::Workbook::Sheet
   {
   public:
      Sheet();
      // The following are for setting data in the sheet
      virtual int32     set_cell_integer30  (nat16 col, nat32 row, int32 value) ;
      virtual bool      set_cell_bool(nat16 col, nat32 row, bool value) ;

      virtual float64  set_cell_number   (nat16 col, nat32 row, float64 value,uint8 precision) ;
      virtual const std::string &set_cell_label (nat16 col, nat32 row, const std::string &value) ;
      virtual const std::wstring &set_cell_label_wstring(nat16 col, nat32 row, const std::wstring &value); //130905
      virtual sint32   set_cell_date(nat16 col, nat32 row, const CORN::Date_const &date_value);

      // Range can be either cell coded coordinate i.e A1, named cell, or coded range i.e. A1..Z999
      // In the Excel OLD API cell coordinates are actually implemented as range

      // The range setting appear to be specific of late or early binding
      // The following are for getting data in the sheet
      virtual int32        get_cell_integer30  (nat16 col, nat32 row)     const;
      virtual bool         get_cell_bool(nat16 col, nat32 row)            const; //130328
      virtual float64      get_cell_number   (nat16 col, nat32 row)       const;
      inline virtual int32 get_cell_date(nat16 col, nat32 row) const {return 0;} // NYI!!!
      virtual const std::wstring &get_cell_label  (nat16 col, nat32 row,std::wstring &label_return)   const;          //130402_111001
      virtual bool set_cell_color(nat16 col, nat32 row,nat8 red,nat8 green, nat8 blue);  //110815
      virtual const std::string &set_cell_formula(nat16 col, nat32 row, const std::string &formula);  //110815
      inline virtual bool is_cell_empty(nat16 col, nat32 row)              const
      {  assert(false);
         return false;
        // Note this is not yet implemented.
      }
      virtual bool clear_cell(nat16 col, nat32 row);
      // Sheet attributes:
      virtual void freeze_pane(nat16 left_most_column,nat32 top_most_row ) ;
      //NYI  uint8       get_index() ;  // the sheet tab index (0 based).
      char *cell_name(nat16 col, nat32 row,char *name)                    const;
   };
 public:
   inline Workbook() : CORN::Workbook()                                       {}
   virtual bool save_as
      (const std::wstring &filename, bool display_alerts = false)  performs_IO_;
   virtual bool save_as_format
      (const std::wstring &filename
      ,XlFileFormat format,bool display_alerts = false)          performs_IO_=0;
};
//_Workbook____________________________________________________________________/
}}//_namespace CORN Excel_OLE__________________________________________________/
#endif

