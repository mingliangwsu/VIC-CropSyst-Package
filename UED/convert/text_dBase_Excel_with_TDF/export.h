#error 160516 This class is obsolete.  It has been replaced with tabular_file_with_TDF_convertor
#ifndef exportH
#define exportH

#include "corn/format/spreadsheet/biff.h"
#include "corn/string/strconv.h"
#include "corn/datetime/datetime64.h"
#include "UED/convert/text_dBase_Excel_with_TDF/tuple_index.h"

#include "UED/convert/text_dBase_Excel_with_TDF/imp_exp.h"

class UED_export_file;
class UED_export_database;
class UED_export_column;
//______________________________________________________________________________
/*_______*/ class UED_export_database
/*000901_*/ : public UED::Imp_exp_database                                                    //000901
/*_______*/ {
/*040928_*/    Association_list export_columns;                                              //040928
/*000717_*/     UED::Tuple_index export_tuples;                                               //000717
/*_______*/ public:
/*_______*/    UED_export_database
/*_______*/    (const char *database_filename // const File_name &database_filename
/*050404_*/    ,ED_tabular_file_format &_format // caller instanciates                       //050404
/*_______*/    );
/*_______*/    virtual bool initialize();                                                    //140918

/*041003_*/     void set_date_ranges(
/*_______*/     CORN::datetime64 start_datetime64 //const Date_time_64 &start_date_time
/*_______*/    ,CORN::datetime64 end_datetime64);// const Date_time_64 &end_date_time

/*_______*/    virtual void export_to
/*020512_*/    (const char *output_filename // const File_name &output_filename
// start time moved to initialize
///*______*/    ,datetime64 start_datetime64 //const Date_time_64 &start_date_time
///*______*/    ,datetime64 end_datetime64  // const Date_time_64 &end_date_time
/*______*/    );
/*______*/
/*______*/
/*______*/    UED_export_column *get_export_column(int16 column_number);
/*______*/
/*______*/    Date_time_64 start_date_time;
/*______*/    Date_time_64 end_date_time;
/*______*/ private:
#ifdef OLD_STANDALONE
//050901  I am trying now to use   CORN tabular derived classes
//so I can use the new Datasource Data_record model
//Don't delete this until I get Text_dBase_Excel_with_TDF_convertor working
/*040928_*/    void output_column_header_row(int row_index, int column_number,int rows, const char *description_SDF,ED_tabular_file_format::Column &column);
/*040928*/     void output_header_fields();
/*040928_*/    void output_column_headers();
/*040829*/     void write_tuple_column(CORN::Date_time_64 &tuple_date_time, float32 value,uint16 export_column_number, UED_export_column  *export_column);
/*______*/     void write_tuples();
#else
/*070305_*/    uint32 write_tuple_records();
#endif
/*040928_*/    void setup_export_columns();
/*______*/     void tuplize_database(CORN::datetime64 filter_date_time_low,CORN::datetime64 filter_date_time_high);

#ifdef OLD_STANDALONE
//050901  I am trying now to use   CORN tabular derived classes
//so I can use the new Datasource Data_record model
//Don't delete this until I get Text_dBase_Excel_with_TDF_convertor working
/*______*/     UED_export_file *output_file;
#endif
/*______*/ };
//______________________________________________________________________________
#ifdef OLD_STANDALONE
//050901  I am trying now to use   CORN tabular derived classes
//so I can use the new Datasource Data_record model
//Don't delete this until I get Text_dBase_Excel_with_TDF_convertor working
/*______*/ class UED_export_file
/*______*/ {
/*______*/    // This is an abstract base class for specifically formatted files
/*______*/ public:
/*______*/    UED_export_file() {};
/*______*/
/*______*/    virtual void export_string
/*_______*/    (const Ustring &str
/*______*/     ,  const ED_tabular_file_format::Column &column_format
/*050901_*/    , Delineation::Text_quote_mode  quote_mode
//               ,int16 col // 1 based (may be 0 or -1 if not in a specific column for text files)
,const char *delimiter
               ) = 0;


   virtual void export_integer
      ( int16  value
      , const ED_tabular_file_format::Column &column_format
      , const char *delimiter) = 0;                                              //050901
   virtual void export_float
      ( float32  value
      , const ED_tabular_file_format::Column &column_format
      , const char *delimiter ) = 0;                                             //050901
   virtual void export_date
      ( const CORN::Date &date
      , const ED_tabular_file_format::Column &column_format
      , const char *delimiter) = 0;                                              //050901


   virtual void goto_next_row()=0;
   // End the current record line

   virtual void export_blank() = 0;

   virtual void initialize() = 0;

   virtual void goto_row(int16 target_row) = 0; // target row is 1 based
   virtual void start_data() {}                                                     //020512
    // This is used by the text file format
    // This function call notifies the export format class
    // the the following output are detail line.
    // This will dump the header to the text file.
};
//______________________________________________________________________________
/*020512*/  class UED_text_export_file
/*______*/  : public UED_export_file
/*______*/  {
               ED_tabular_file_format &format;
//             This class is used when exporting to text file format.
/*020512*/     Ustring delimiter;
//
/*______*/     Ustring header_line[100];  // Not including column headers  (index 0 not used)
//             The text export class buffers exported data in
//             the header lines until start_data() is called.
//050902 /*______*/     Ustring detail_line;  // could also be column header
//             buffer for the current row of output.

/*______*/     Ustring current_line;
//             // This is buffer where the current row is output to.
/*______*/     // Initially this will point to one of the header lines.
//             // After start_data() is called, current line point to detail_line
//             // and this will be used for the remainer of the output.

/*______*/     bool  data_started;  // Set to true when start_data is called
/*______*/     std::ofstream  tout;

/*______*/     uint32   row;  // current row number 1 based number;  (0 indicates no row)
/*______*/     uint32   col;  // current column 1 based number (0 indicate no column)
/*______*/  public:
/*______*/     UED_text_export_file(const char *text_filename,ED_tabular_file_format &_format)
/*______*/     : UED_export_file()
/*______*/     , tout(text_filename)
/*050902_*/    , current_line("")
//050902obs/*_______*/    , current_line(&header_line[1])
/*_______*/    , data_started(false)
/*_______*/    , row(0) // rows are 1 based, but we don't want to print headers unless row is set.
/*050902_*/    , format(_format)
/*______*/     {};
/*______*/
/*______*/    virtual void export_string
/*_______*/    (const Ustring &str
/*______*/    ,  const ED_tabular_file_format::Column &column_format
/*050901_*/    , Delineation::Text_quote_mode  quote_mode
//               ,int16 col // 1 based (may be 0 or -1 if not in a specific column for text files)
,const char *delimiter
/*_______*/    );

/*______*/

/*______*/
/*______*/    virtual void export_integer
/*______*/    (  int16  value
/*______*/    ,  const ED_tabular_file_format::Column &column_format
/*050901*/     ,const char *delimiter

/*______*/    );
/*______*/
/*______*/
/*______*/    virtual void export_float
/*______*/    (  float32  value
/*______*/    ,  const ED_tabular_file_format::Column &column_format
/*050901*/     ,const char *delimiter
/*______*/    );
/*______*/
/*______*/    virtual void export_date
/*______*/    (  const CORN::Date &date
/*______*/    ,  const ED_tabular_file_format::Column &column_format
/*050901*/     ,const char *delimiter
/*______*/    );
/*______*/
/*______*/    virtual void goto_next_row();
/*______*/    // End the current record line
/*______*/
/*______*/    virtual void export_blank() { /*N/A to text files*/ };
/*______*/
/*______*/    virtual void initialize() { /* nothing to initialize */} ;
/*______*/
/*______*/    virtual void goto_row(int16 target_row); // target row is 1 based
/*020512*/     virtual void start_data();
/*_______*/    // This function call notifies the export format class
/*_______*/    // the the following output are detail line.
/*_______*/    // This will dump the headers to the text file.
/*_______*/
/*020512_*/    inline virtual void set_delimiter(Ustring i_delimiter)
/*020512_*/    { delimiter = i_delimiter;};
/*______*/ };
//______________________________________________________________________________
/*______*/ class UED_Excel_export_file
/*______*/ : public UED_export_file
/*______*/ , public BIFF234_File
/*______*/ {
/*______*/ public: // Constructor
/*020512*/    UED_Excel_export_file(const char *excel_filename)                              //020512
/*______*/    :UED_export_file()
/*020512*/    ,BIFF234_File(excel_filename,0,1000,0,100)
/*______*/    {};
/*______*/    virtual ~UED_Excel_export_file();
/*______*/    virtual void initialize()
/*______*/    {
/*______*/    };
/*______*/ public:
/*______*/    virtual void export_integer
/*______*/    (  int16  value
/*______*/    ,  const ED_tabular_file_format::Column &column_format
/*050901*/     ,const char *delimiter
/*______*/    );
/*______*/    virtual void export_float
/*______*/    (  float32  value
/*______*/    ,  const ED_tabular_file_format::Column &column_format
/*050901*/     ,const char *delimiter

/*______*/    );
/*______*/
/*______*/    virtual void export_date
/*______*/    (  const CORN::Date &date
/*______*/    ,  const ED_tabular_file_format::Column &column_format
/*050901*/     ,const char *delimiter
/*______*/    );
/*______*/
               //020512 note that I don't think export_blank is used
/*______*/    inline virtual void export_blank()
/*______*/    { col++; };
/*______*/
/*_______*/    virtual void export_string
/*_______*/    (const Ustring &str
/*______*/     , const ED_tabular_file_format::Column &column_format
/*050901_*/    , Delineation::Text_quote_mode  quote_mode                             //050901

               ,const char *delimiter
/*_______*/    );
/*______*/    inline virtual void goto_next_row()
/*______*/    {  BIFF234_File::endl();
/*______*/    }
/*______*/    // End the current record line
/*______*/    inline virtual void goto_row(int16 target_row) // target row is 1 based
/*______*/    { Spreadsheet_abstract::row = target_row-1;
/*______*/      Spreadsheet_abstract::col = 0;
/*______*/    }
/*______*/ };
//______________________________________________________________________________
 #ifdef NYI
class UED_dBase_export_file
: public UED_export_file
{  goto_row() { goto_record }
}
//______________________________________________________________________________
class UED_Lotus_123_export_file
: public UED_export_file
{  }
//______________________________________________________________________________
class UED_text_export_file
: public UED_export_file
{
   // will need to keep track of the current row (text tabular may do this)
}
//______________________________________________________________________________
#endif
#endif
//end of OLD_STANDALONE
#endif

