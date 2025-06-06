#ifndef delineationH
#define delineationH
#include "corn/enum.h"
#ifndef primitiveH
#  include "corn/primitive.h"
#endif
#include "corn/const.h"
#include <string>
#include "corn/enum.h"
#include "corn/measure/units_enum.h"
#include "corn/chronometry/date_32.h"
#include "corn/tabular/tabular.h"
#include "corn/container/bilist.h"

namespace CORN {
extern const char *tabulation_label_table[];                                     //040925

class Data_record;
//______________________________________________________________________________
enum Relative_to      // Relative to top of
   {FILE_RELATIVE
      ///< Use this option when we know something
      /// is always going to be at a specified offset
      /// from the beginning of the file (or first cell)
      /// This implies file will always have the same
      /// delineation (No extraneous text)
   ,DATA_RELATIVE
      ///< was BEGIN_MARKER_RELATIVE 070923_
      /// Use this option when there may be extraneous text
      /// at the begining of a file and there is some marker
      /// that specifies the actual beginning of the data.
   ,PAGE_RELATIVE
      ///< Use this options for text files that are paginated
      /// And we know that something always occurs
      /// on a particular line of the page
      /// (Used for text file reports)
      /// Pagination is denoted with FormFeed character ASCII 12
   ,BLOCK_RELATIVE
      ///< Use this when the line row is relative to the
      /// current section (I.e. the Units row might be the 4 row of the column header).
      /// [This may not be applicable to detail lines, I don't know a situation where it might be used at this time. ].
      //070925     ,LAST_HEADER_ROW_RELATIVE
      /// This is obsolete
      /// Now use section.
      ///  Also header sections are obsolete, now
      ///  fields are floating and can appear anywhere in the file (on the page)
      //070925    ,LAST_COLUMN_HEADER_ROW_RELATIVE
      /// This is obsolete because I never really used it
      /// it was intended to determine the start of detail lines
      /// But it have never been used
   };
extern const char *relative_to_label_table[];
DECLARE_ENUM_CLAD(Relative_to_clad,Relative_to,relative_to_label_table);
//_Relative_to______________________________________________________2006-08-23_/
struct Extent
{
 public:
   Relative_to_clad relative_to_clad;
      ///< In the case of the file extent, only BEGIN_FILE_RELATIVE and BEGIN_MARKER_RELATIVE
      /// are relevent (this is the start of all the data including: header fields (if any) column headers (if any) and detail lines)
      /// The relative modes LAST_HEADER_ROW_RELATIVE,LAST_COLUMN_HEADER_ROW_RELATIVE
      /// are relevent to the respective potential sections: header fields, column headers, and detail line extents.
   //200220 int16 start_row_number; // 1 based  reimplement as offset rename this to offset
   int16 begin_offset;
      ///< offset from the relative to point
      /// This is the row number relative to the bounding extent
      /// For example the column headers may start 5 rows after the end position
      /// of the begin_marker for the file
      /// so the offset would be 4
      /// I don't think we will encounter a case were the data or column headers start at a huge
      /// number although I suppose they could be at the end of a file (footers)
      /// Column:
      /// When 0 this indicates the column number will be the positional as encountered.
      /// Values will be used overriding positional occurance
      //  when columns are omitted, or when using spreading.
      /// Row:
      /// 0 indicates it doesn't apply.
      /// offset to where the data starts relative
      /// (below any header and column header rows/lines).

   std::string begin_marker;
      ///< This is a character string (usually on its own line) that marks the beginning of the
      /// header or data detail lines
      /// Empty string indicates no marker
      /// data everything before the marker (including this line) is ignored
      /// There will often be an end_data_marker
      /// The reading of the data (headers and detail lines) begins after this line
      /// Header and/or data record number (1 based) will be relative to
      /// To the point following the begin_data_marker
      /// (This will usually be the line following the line containing the begin_data_marker,
      /// although I do see it possible that the data may start on the same line.
   std::string end_marker;
      ///< This is a character string (usually on its own line) that marks the end of the data
      /// Empty string indicates no marker
      /// Everything after this line (including the marker) is ignored.
      /// With no additional parsing
   nat16 rows;                                                                   //200226
};
//_Extent___________________________________________________________2020-02-20_/
/*200220 obsolete now use Extent_map
class Extent__layout
: public Extent
{
public:

public:
   inline Extent__layout()
      : relative_to_clad(FILE_RELATIVE)
      , start_row_number(0)
      , begin_marker("")
      , end_marker("")
      {}
   virtual bool setup_structure(CORN::Data_record &data_rec,bool for_write) modification_;
};
//_Extent__layout____________________________________________________2005-04-11_/
*/
/*200220 NYI
class Extent_map
: public extends_
{
   Extent &extent;

};
*/
//_Extent_map_______________________________________________________2020-02-20_/

struct Separation
{
   std::string delimiter;
      // space comma tab semicolon or other using YAML escape sequence.
      // May be omitted when fixed width tabulation
      // Conceptual: There may be multiple characters indicating that
      // any of these may be a delimiter
   bool continuous;
      // (optional) (default false)
   // Conceptual
   // std::string other_delimiter_string;    // Other extended delimiter (I.e. HTML table <TH><TD>) //050830
   std::string quotation;
      // Usually specified as a single character
      // either ' " or empty or any other quote character
      // Conceptual: '"  or "' indicates alternating quotation
      // Conceptual: French style guillemets parenthesis brackets and braces
   char decimal;
      // either . or , omitted will use native localization

};
//_Separation_______________________________________________________2020-02-20_/
enum Tabulation

         // rename these to xxxx_TABULATION

{intrinsic_TABULATION
,fixed_TABULATION
,delimit_TABULATION           // was DELIMIT_character
/*200220
#if ((CS_VERSION > 0) && (CS_VERSION < 6))
,DELIMIT_character_renamed // <- rename this to delimit
#endif
*/
};
DECLARE_ENUM_CLAD(Tabulation_clad,Tabulation,tabulation_label_table)
//_Tabulation___________________________________________________________________
class Delineation
//200202 : public Extent__layout
{ ///< \class This class does not support header fields or column headers other than specifying the number of rows to skip to locate the first data row
 public:
   Tabulation_clad tabulation_clad;
   Extent      extent;
   Separation  separation;                                                       //200220
   bool        paginate;                                                         //200225
   std::string missing;                                                          //200220

    /*200220 obsolete now specified as a string in separation
    enum Text_quote_mode   { NO_QUOTE, SINGLE_QUOTE, DOUBLE_QUOTE, ALTERNATING_QUOTE};
    DECLARE_ENUM_CLAD(Text_quote_mode_clad,Text_quote_mode,text_quote_mode__label_table)

    // 180508 rename to Quoting

   // In alternating quote mode, single quote will quote double quotes
   // I.e.  'He said "yes"'
   // or    "It's mine not sara's"
   */

   /*200220 replaced with separation
   // If not fixed width columns, the following delimited options are active
   bool           continuous_delimiter; // Treat continuous delimiters as one
   bool           delimiter_tab;
   bool           delimiter_space;
   bool           delimiter_comma;
   bool           delimiter_semicolon;
   bool           delimiter_other;    // also allow other_delimiter_16 as a delimiter
   int16          other_delimiter_16; // Other character to use as a delimiter
   bool           delimiter_string;                                              //050830
   std::string other_delimiter_string;    // Other extended delimiter (I.e. HTML table <TH><TD>) //050830
   Text_quote_mode_clad  text_quote_mode_clad;                                   //020505

   std::string decimal_mark;  // . for US  , for other countries
      // consider move to Tabular file since it is a property of the file format, not delineation //050402
      // However it is delineating the digits
      // conceptual: if omitted default to OS setting.
   */

   /*200220 moved to extent
   std::string notes;        // Commentary information stored in the format file
   */

   //conceptual: justification option


 public:
   inline Delineation()
      /*200220
      : Extent__layout() ,
      , continuous_delimiter(false)
      , delimiter_tab(false)
      , delimiter_space(false)
      , delimiter_comma(false)
      , delimiter_semicolon(false)
      , delimiter_other(false)
      , other_delimiter_16(32)
      , delimiter_string(false)                                                  //090305
      , other_delimiter_string("")                                               //090305
      , text_quote_mode_clad(NO_QUOTE)                                           //010815
      , decimal_mark(".")                                                        //050402
      , notes("")
      */
      : tabulation_clad(delimit_TABULATION) // (DELIMIT_character)
      {
         separation.delimiter = " ";                                             //200220
         separation.quotation = "";                                              //200220
         separation.decimal   = '.';                                             //200220
         separation.continuous= false;                                           //200220
      }
   bool is_delimiter(char potential_delimiter)                     affirmation_;
      //200220 probably should be moved to separator

   bool has_delimiter()                                            affirmation_; //190107

   const char *append_delimiter(std::string &return_buffer)               const; //190106
      // When reading files, multiple potential delimiters could be optionally specified as delimiters
      // When writing files, only a single delimiter is available.
   virtual bool default_R_text()                                  modification_;
      // Sets up delimit for R import/export text files.
   bool copy_from(const Delineation &from)                 modification_; //141008
};
//_Delineation______________________________________________________2005-04-11_/

class Delineation_tabular //200220 was Common_tabular_file__format

   // Probably should not be derived from Common_parameters_data_record
   // because it probably is to used to get the field names.
   // Instead, should have fields class (List of fields)

//200222 , public Extent         //200220 replace this with document delineation
{public:
   //______________________________________________________________________________
   // Column headers can be delineated different from the data rows/detail lines
   class Delineation_header_footer //200220  Column_headers__delineation   //200220 rename to Delineation_header
   : public Delineation
   {public:
      //200220 moved to extent int16 rows;                                       //990507
         // Does not include the units rows
      int16 units_row_offset; // Number of rows below the column header start row where units start (relative to column header start row_ //010725
      int16 units_rows;                                                          //010725
      bool  units_parenthesis;  // True if the units are surrounded by parenthesis //060323
         // conceptual change this to char where one would specify ( or [ or {.
      // May need an options to omit parenthesis if units are not specified for a column I.e. don't print ()
      bool  paginate;                                                            //200220
    public: // constructor
      inline Delineation_header_footer() //200220 Column_headers_delineation()                                        //060626
         : Delineation                                                        ()
         //200220 , rows(0)
         , units_row_offset                                                  (0)
         , units_rows                                                        (0)
         , units_parenthesis                                             (false)
         , paginate                                                      (false) //200220
         {
            extent.rows=0;                                                       //200220
         }
      inline virtual bool default_R_text()                         modification_
         {  extent.rows = 1;                                                     //200220
            return Delineation::default_R_text();
         }
      inline bool copy_from(const Delineation_header_footer &from)modification_
         {
            //200220 moved to extent rows              = from.rows;
            units_row_offset  = from.units_row_offset;
            units_rows        = from.units_rows;
            units_parenthesis = from.units_parenthesis;
            paginate          = from.paginate;                                   //200220
            return Delineation::copy_from(from);
         }
      //_Column_headers_delineation_________________________________2014-10-08_/
   };
   //___________________________________________________________________________
   class Delineation_detail
   : public Delineation
   { public:
      int16                   timestep;  // Not sure if I need this I thought I had removed it may have tried to do this with units only but not sure I can do that   //060223
      Labeled_units_enum      timestep_units_enum; // The time step between each record  may be time step units
     public:
      inline Delineation_detail()
         : Delineation()
         ,timestep (1)
         ,timestep_units_enum(UT_day)                                           //060626
         {}
   };
   //_Detail_lines_delineation_________________________________________________/

 public:
    // The following are  information:  but I haven't defined such a structure
   std::string          definer; // user who created the format file       //990307
   CORN::Date_clad_32   inception; // version date                         //990307
      // 200220 can be raw and cowl
   std::string          description;
   std::string          notes;  // Commentary information stored in the format file

   Delineation          document;                                       //200220
      // These are actually properties of document delineation
      // But I havent specialized a Delineation_document structure.
      std::string pagination;                                                    //200220
         // ["none", "FF", "ASA" ]
      std::string lineation;                                                     //200220
         // ["native", "CRLF", "LF", "CR", "ASA" ]
   Delineation_header_footer     header;                                         //200220
   Delineation_header_footer     footer;                                         //200220
   Delineation_detail            detail;                                         //200220


   /*200220 replaced with paginate (for header and footer each)
   bool                          column_headers_every_page; // True if headers/footers appear on every paginated page   //070925
   //200220 olumn_headers_delineation    column_headers_delineation;


   std::string                   column_header_static/*[10]*/;   // Some times is is preferable to override the column header captioning with a static text (this is necessary when the column headers don't match with columns, where the caption is not fully within the fixed column width of the data) //100104
   //200220 Detail_lines_delineation      detail_lines_delineation;                       //050411

      // rename this to delineation_detail

/*200220 moved to Tabular_file_format
       //             column_headers can have a delineation layout different from the detail lines, but the same options are available
   Bidirectional_list            columns;          // list of Column      //040925
*/
   /*200131 obsolete
   enum Variable_order {fixed_order,variable_order};                             //020505
   DECLARE_ENUM_CLAD(Variable_order_clad,Variable_order,variable_order_label_table)
   Variable_order_clad variable_order_clad;                                      //990507
   */

   sint16 year_offset_for_dates_with_only_two_digits;

   /*200220 Quite old and never really used
   //200220 these are probably obsolete, use data source idiom
   //The following are used for download options
   std::string                cgi_form_server;                                   //050516
   std::string                cgi_form_action;                                   //050516
   Request_method_clad        cgi_form_method;            // I.e. POST GET?      //050516
   */

 public: // Constructors
   /*200220
   Common_tabular_file__format
      (Format_file_type format_file_type);
   */
   /* probably not needed
   Delineation_tabular //200220 Common_tabular_file__format
       (const std::string &description                                           //040909
       //200220 ,Format_file_type   format_file_type
       );
       // This constructor is for loading from a data file
   */
    Delineation_tabular();                                                       //200220
    //200220 Common_tabular_file__format();
       // This constructor is used by UED_export file
   bool copy_from(const Delineation_tabular &source);                            //141008
 public:

   /*200220  need to reimplement this. I think  set_delineation() with some sort of bitmask
   void reset_for_file_type();                                                   //060626
   */

   // In the user interface, when the file type is change
   // we can set up some options that we know are not relevent for the file type.
   virtual bool default_R_text()                                  modification_;
      // Sets up format for R import/export text files.
   /*200226
   virtual bool get_end();                                                       //190325
      // Not sure yet if I need to reimplement this
   */

};
//_Delineation_tabular_was Common_tabular_file__format_______________2020-02-20_/
class Table_definition
: public extends_ Delineation_tabular
{
 public:
   class Column
   : public implements_ Item
   , public extends_ CORN::Tabular_column
   {

    public:
      /*200808 moved to Tabular_column  (Parameter_parameters_string)
      nat32 UED_variable;
      */
      /* NYI this would be used when writing UED file,
         but may want to have another way of specifying UED creation
      nat8  UED_data_type;
      nat32 UED_units;
      nat8  UED_rectype;
      */
      bool record_date;  // check needed may be obsolete

/*
    &&sf. expect     ( "variable"      ,column. xxxxxx) // !!int 0 # UED variable code (deprecated)
    &&sf. expect     ( "units"         ,xxxxxx) // !units  '' # units the value as stored in UED data record
    &&sf. expect     ( "rectype"       ,xxxxxx) // !!int 0 # record type code
    &&sf. expect     ( "type"          ,column.data_type_clad) // !enum date # [date, .... ]  # data type for the binary file
*/

    public:
      inline Column()
        : Item                ()
        , CORN::Tabular_column()                                                 //040925
        //200808 , UED_variable        (0)
        /*NYI
        , UED_data_type       (0)
        , UED_units           (0)
        , UED_rectype         (0)
        */
        {}
      virtual bool setup_structure
         (CORN::Data_record &data_rec,bool for_write)             modification_;
         // setup_structure is needed to prevent conflists where declaring enumerated section in VV files
      virtual bool is_key_string(const std::string &key)           affirmation_; //180820
      inline virtual const char    *key_string(std::string &buffer)        const
         { buffer.append(get_key()); return buffer.c_str(); }                    //190107
      #if ((CS_VERSION >= 1) && (CS_VERSION <= 5))
      inline virtual nat32 get_variable_code()      const {return UED_variable;} //200707
      #endif

   };
   //_Column___________________________________________________________________/
 public:
   provided_ Bidirectional_list columns; // list of Column                                 //040925
 public:
   virtual nat16 get_width_column(nat16 column_index)                     const; //170327
   virtual nat16 get_position(nat16 column_index)                         const; //190325
   virtual nat16 get_column_number(nat16 column_index)                    const; //190325
   virtual std::string *render_blank
      (nat16 column_index
      ,nat32 row_index)                                              rendition_; //190108
 public:
   //abandoned Column *render_column()                                           rendition_; //200302
   Column &provide_column(nat16 column_index)                        provision_; //200302
   nat32 position_columns();                                                     //200306
 public:
   #if ((CS_VERSION >= 1) && (CS_VERSION <= 5))
   virtual void list_required_variables()                                 const; //200813 170225
   // This is used with optation
   #endif
 public:
   inline Table_definition()
      : Delineation_tabular()
      , columns()
      {}
//   virtual bool is_column_record_date(const Column &column)               const; //200618
};
//_Table_definition_________________________________________________2020-03-06_/
extern const std::string &compose_formatted_string_from_cstr
(std::string &formatted_string
,const char *c_str
,std::string quotation
//200220 ,CORN::Delineation::Text_quote_mode quote_mode
,nat16 field_width
);
//__________________________________________________________________2019-01-06_/
}//_namespace CORN_____________________________________________________________/
#endif

