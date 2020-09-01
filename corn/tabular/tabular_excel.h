#ifndef tabular_excelH
#define tabular_excelH

//190107  USE_CORN_Excel_workbook
#define USE_HOERL_EXCEL
#ifdef USE_HOERL_EXCEL
   #include "corn/spreadsheet/Excel_xlslib/workbook_Excel_Hoerl.h"
#endif

#include "corn/tabular/file.h"
#include "corn/format/excel/workbook_Excel_BIFF.h"
#include "corn/spreadsheet/workbook_datasrc.h"
#include "corn/tabular/tabular_format.h"
namespace CORN {
//______________________________________________________________________________
class Tabular_file_Excel
: public Tabular_file
{
 private:
   #ifdef USE_HOERL_EXCEL
   Hoerl::Workbook workbook;                                                     //190107
   #else
   Excel_BIFF::Workbook workbook;
   #endif                                                                        //190107

   Workbook_data_source data_source;
   const Tabular_format /*200429 Tabular_file_format*//*200220 Common_tabular_file__format*/ &format;
   Workbook::Sheet      *active_sheet;
   std::ios_base::openmode openmode;                                             //130401
public:
   Tabular_file_Excel
      (const std::string &filename_                                              //190506
      ,bool _keep_read_records_in_memory
      ,std::ios_base::openmode _openmode                                         //130401
      ,const Tabular_format/*200429 Tabular_file_format*/ /*200220 Common_tabular_file__format*/ &_format);


   ~Tabular_file_Excel();
public: // Item reimplementation
   virtual const char *label_string(std::string &buffer)                  const; //170423
public: // Data_source reimplementation
   inline virtual bool get(Data_record &data_rec)
      { return data_source.get(data_rec); }
   inline virtual bool get_next(Data_record &data_rec)
      { return data_source.get_next(data_rec); }
   inline virtual bool goto_first()
      { return data_source.goto_first(); }
   // Goto the first line in the text file with detail line data (not header or column header)
   inline virtual bool goto_index(nat32 zero_based_index)
      { return data_source.goto_index(zero_based_index); }
   inline virtual bool goto_last()                             { return false; } // NYI (/NYN)
   inline virtual bool   goto_next(int32 skip_records = 1)
      { return data_source.goto_next(skip_records); } // NYI (/NYN)
   // skip records is allowed to be signed
   inline virtual nat32 get_current_index()                    { return false; } // NYI (/NYN)
   // returns the index value for the table's current position.
   // For database tables this is the record number
   // For memory tables, this is the memory location or object pointer);
   inline virtual nat32 append(Data_record &data_rec)          { return false; } // NYI (/NYN)
   // This creates a new record at the end of the table and sets data_rec.
   // Returns the record index of the new record that was appended.
   inline virtual nat32 delete_record()                        { return false; } // NYI (/NYN)
   // This marks the current record as deleted.
   // usually only a flag is set in the record indicating it is deleted
   // so the record is not actually physically deleted.
   inline virtual bool set_data(Data_record &data_rec,bool append)
      { return data_source.set_data(data_rec,append); }                          //150503
   virtual nat32 get_record_count();                                             //050407
      // should be const need to check ancestors and decendents
   virtual bool reset()                               stream_IO_{ return true; } //111118 //NYN
public:  // Tabular_file reimplementation
   virtual int get_column_count()                                  { return 0; } // Not currently needed for import utility
   virtual int get_column_width(int column)                        { return 0; } // Not applicable to Excel
   virtual bool goto_sheet(const std::string &sheet_name);                       //100110
   virtual nat32 write_raw_string(const char *c_str)               { return 0; } //070203
   // This is used in text files to write static header text, it is not applicable to spreadsheets or other file formats
   // This writes the specified string to the current position.
   // returns the actual number of characters written including quotes
   virtual nat32 write_field_string
      (const char *c_str                                                         //070203
      ,const std::string &quotation                                              //200429
      //200429 ,Delineation::Text_quote_mode quote_mode
      );
   virtual nat32 write_delimiter_string(const char *c_str)         { return 0; } //070203
      // delimiters are not applicable to spreadsheets
   modifiable_ CORN::Workbook &get_workbook()                     modification_;
 public:
   virtual const std::string &compose_formatted_string_from_cstr                 //140922
      (std::string &formatted_string
      ,const char *c_str
      ,const std::string &quotation                                              //200429
      //200429 ,CORN::Delineation::Text_quote_mode quote_mode
      )              const;
      // problably should pass delimit mode
};
//_Tabular_file_Excel__________________________________________________________/
}//_namespace_CORN_____________________________________________________________/
#endif

