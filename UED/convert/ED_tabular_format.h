#ifndef ED_tabular_formatH
#define ED_tabular_formatH

// Extends tabular format to include environment data specific fields

// Adds
//  Header data including
//  concepts of variables
//  Assumes detail lines are time series

#include "corn/tabular/tabular_format.h"
#include "corn/math/statistical/stats.h"
#include "UED/library/record.h"
#include "UED/library/varcodes.h"

#define LABEL_variable_code   "variable_code"
#define LABEL_units_code      "units_code"
#define LABEL_UED_record_code "UED_record_code"

using namespace CORN;                                                            //100808
namespace CS                                                                     //170225
{
class Desired_variables; // forward declaration
}
extern const char *UED_scalar_record_code_label_table[];
DECLARE_ENUM_CLAD(UED_scalar_record_code_clad,CORN::Record_code,UED_scalar_record_code_label_table);

using namespace UED;
//______________________________________________________________________________
extern const char *field_data_label_table[];
extern const char *generating_application_vars_label_table[];
extern const char *location_var_label_table[];
//_externs__________________________________________________________2004-09-27_/
class ED_tabular_file_format
: public CORN::Tabular_format/*200220 Common_tabular_file__format*/                                        //060925
{
 public:
   class Metadata_field : public CORN::Item                                      //180101_010814
   , public CORN::Parameter_properties_string
   , public CORN::Parameter_number_properties
   {
    public:
       enum Field_data
       {FD_static_text
       ,FD_UED_general_comment
       ,FD_UED_database_description
       ,FD_UED_generating_application
       ,FD_UED_location    // see Location_var
       ,FD_UED_scalar_data
       ,FD_date_time_stamp  // The field is a time stamp date element, it is either a formatted text string or part of a date and/or time i.e, year, month, day of month or DOY
                 // For example, in an Excel table, there could be a sheet for each year of data so there may be a header cell identify the year.
                 // or perhaps a sheet for each month.
                 // Excel cell could be a complete date (Excel stores dates and number cells counting the number of days since 1900 or 1904
                 // Specifying a FD_date_time_stamp will imply
                 // that if the timestep (cell in Excel) is encountered
                 // detail lines that have separate columns for month and/or day (probably not year) or time elements
                 // will have automatically automatically added to the detail line time stamp.
                 // I.e. if the detail lines have day of month and the sheet/file is time stamped
                 // with the year and month, the detail line will be time stamped with the date composed from year month and day of month.
                 // Similarly if the sheet is time stamped with only the year
                 // and the detail line is time stamped with the day of year, the detail line will be timestamped with the composed date
       ,FD_parent_path                                                           //181030
       };
       DECLARE_ENUM_CLAD(Field_data_clad,Field_data,field_data_label_table)
       //_Field_data___________________________________________________________/
       enum Generating_application_vars
       {  GEN_APP_version,GEN_APP_description,GEN_APP_build_date};
       DECLARE_ENUM_CLAD(Generating_application_var_clad,Generating_application_vars,generating_application_vars_label_table)
       //_Generating_application_vars__________________________________________/
       enum Location_var   // Note, I might want to move this to UED
       {  LOC_VAR_station_ID
       ,  LOC_VAR_geocoordinate_ISO_6709_2008
       ,  LOC_VAR_latitude_decimal_degrees
       ,  LOC_VAR_latitude_deg_min_sec
       ,  LOC_VAR_longitude_decimal_degrees
       ,  LOC_VAR_longitude_deg_min_sec
       ,  LOC_VAR_elevation_meters
       ,  LOC_VAR_elevation_feet
       ,  LOC_VAR_country_name
       ,  LOC_VAR_state_name
       ,  LOC_VAR_county_name
       ,  LOC_VAR_description
       ,  LOC_VAR_station_name
       ,  LOC_VAR_comment                                                        //100107
       ,  LOC_VAR_station_number                                                 //100107
       };
       DECLARE_ENUM_CLAD(Location_var_clad,Location_var,location_var_label_table)
       //_Location_var__________________________________________________________
   public:
      std::string description;    // Description of the header                   //040909
      std::string worksheet;      // For spreadsheet files, this is the sheet to put the header data on //050405
      Relative_to_clad relative_to_clad;                                         //070925
      nat16      row_num;        // 1 based (0 if not applicable)                                                          // Actually unsigned
      nat16      col_num_not_pos;        // 1 based (0 if not applicable) for excel, it is the cell for column separate values it is the column
         // 171129 eventually rename this back to col_num
         // when I have located all the places where I need to distinguish
      nat16      pos_num;            // for text file this char pos 1 based index   //181030
      nat16      caption_row;   // actually unsigned but signed for UI  //060324_060627 was label_row
      nat16      caption_col;   // actually unsigned but signed for UI  //060324_060627 was label_col
      Field_data_clad field_data_clad;
      bool        every_page;    // If (text) file is paginated this option put the field on every page. //070925
      std::string static_text;
         // Static text that may be associated with a variable
         // or stand alone comment (whether or not there is a caption)
      nat8        parent_path;                                                   //181030
         // The file value is taken from the CWD path name
         // 0 is the directory name of the CWD
         // 1 is the parent directory
         // 2 is the parent's parent directory etc...
      nat16       database_description_index;                                    //060626
      nat16       database_comment_index;                                        //040927
      // The following are used to seek (when exporting) or render (when importing) UED scalar records (field_data_labeled = FD_UED_scalar_data)
      Record_code          UED_scalar_record_code;
      UED::Variable_code    UED_scalar_variable_code;
      //NYI probably will need to add a Ustring for date and/or time format it for timestamp fields.
      Generating_application_var_clad generating_application_var_clad;
      Location_var_clad               location_var_clad;                   //050405
   /*200225 obsolete the file type is now known/provided elsewhere
    private: // The following members are not I/O to the definition (TDF) file
      Format_file_type   format_file_type;                                       //050405
         // We need to know the format file type so we can exclude unneeded members in I/O
         // Cant remember what format_file_type was for, may be obsolete with new parser
    */
    public:
      std::string read_buffer;                                                   //070927
         // When importing data this is where the field data value
         // will be stored until it is commited.
    public:
       inline Metadata_field()  // We need to know the format file type so we can exclude unneeded members in I/O
       :  description("")
       ,  worksheet("Sheet 1")                                                   //050405
       ,  row_num(0)
       ,  col_num_not_pos(0)
       ,  pos_num(0)                                                             //171129
       ,  field_data_clad(FD_static_text)
       ,  static_text()
       ,  parent_path(0)                                                         //181030
       ,  database_description_index(0)
       ,  database_comment_index(0)
       ,  UED_scalar_record_code(0)
       ,  UED_scalar_variable_code(0)
       /*200225 file type is now known/provided elswere
       ,  format_file_type(FILE_TYPE_unknown)
       */
       ,  every_page(false)                                                      //070925
       ,  relative_to_clad(FILE_RELATIVE)                                        //070925
       {}
   /*200225 check obsolete
      inline void set_format_file_type(Format_file_type format_file_type_)
                                       { format_file_type = format_file_type_; } //060602
   */
      virtual bool setup_structure(CORN::Data_record &data_rec,bool for_write) modification_;
   inline virtual bool is_key_string(const std::string &key)              affirmation_  //180820
      { return description == key; }
   };
   //_Metadata_field___________________________________________________________/
   class Column
   : public Tabular_format::Column
   /*200220 Common_tabular_file__format::Column */                               //060925
   {
   public:

   //continue here:
   //   UED_variable code is now available in Tabular_format::Column (Parameter_properties_string)
   //   used variable_code_cowl

      Variable_code_clad  variable_code_clad;                                    //040925
      // already has ontology
      Record_code_clad    record_type_clad;  // rename to record_type_clad
      Statistic_clad          statistic_clad; // Use STAT_XXXX constants from stats.h

      bool                   &record_date;
         // True if the column denote the record date (or part of the record date (I.e. year)   //041004
         // perhaps rename record_date to temporal

      int16                   period; // Signed, negative value disables         //041003
      /*
       If period is specified, we dont need variable code, units, record type, statistic //041004
       period is used for CropSyst report generation. This is the period level specified in the UED (I.e. Phenologic stage).
      */
      Labeled_units_enum      UED_import_to_smart_units_code_enum;               //040925
            // Used only for UED import in the case where the smart_variable_code
            // is not a common variable code, so we have to
            // compose a creation
      bool                    missing_is_0;                                      //140608
         // There was a format where non-measured 0 values of precipitation
         // were not recorded in the source file.
         // In this case it is necessary to assume missing values are 0.
   public:
      inline Column()
         : Tabular_format::Column()                                              //200220_060925
         , variable_code_clad                                                (0)
         , record_type_clad                                                  (0) //040925
         , statistic_clad                                      (value_statistic)
         //200618, record_date                                                   (false) //041004
         , record_date                                                (temporal) //200618
         , period                                                           (-1) //041004
         , UED_import_to_smart_units_code_enum                               (0) //060626
         , missing_is_0                                                  (false) //140608
         {
            temporal = false;                                                    //200618
         }
      virtual bool setup_structure
         (CORN::Data_record &data_rec,bool for_write)             modification_;
      virtual nat32 get_variable_code() const {return variable_code_clad.get();} //200618

   };
   //_Column___________________________________________________________________/
public:  // attributes (of ED_tabular_file_format)
   // Columns section:
   // Column Scalar fields section
      std::string filename_extension; // Usually used when creating/writing export files  (without period) //080228
   int16                         records_per_UED_record;  // Used when importing (I.e. old CropSyst weather data text format) //040925
   Bidirectional_list      metadata_fields_numbered;    // List of Metadata_field. Arbitrary header fields (I think this is scalar fields)
   // It might be useful to have delineation layout for header fields, currently assuming the header lines are specifically spaced with no quotation marks and no delimiting with special characters  //100111
   // column_headers can have a delineation layout different from the detail lines, but the same options are available
   int16                   date_column_number;     // 1 based                         //050407
   int16                   ID_column_number;       // 1 based   (Station ID number?)  //050407
   int16                   name_column_number;     // 1 based   (Station name?)       //050407
   nat8                    extension_year_digits;                                            //150405
      // When data is stored in separate file, this is the number of digits
      // to denote the year (I.e. CropSyst3 used 4 digits, MONICA uses 3 digits)
      // 0 indicates data not separated into yearly files.
   bool year_filename_suffix_obsolete;
public: // Constructors
   /*200225
   ED_tabular_file_format(Format_file_type format_file_type_);
   */
   ED_tabular_file_format
      (const std::string &description_                                           //040909
      /*200225
      ,Format_file_type format_file_type_
      */
      );
      // This constructor is for loading from a data file
   ED_tabular_file_format();
       // This constructor is used by UED_export file
public:  // VV_file overrides
   virtual bool expect_structure(bool for_write = false);                        //161025
   virtual bool set_start();                                                     //161023_050405
   virtual bool get_end();                                                       //161023_150405
   const Column *find_column(Variable_code var_code);
   const Column *find_column(std::string   ontology);
         // WARNING columns are not necessarily unique according to
         // var_code/ontology because there may be similar columns in different units
   void list_required_variables()                                         const; //170225_041021
   const Metadata_field *find_metadata_field
      (const std::string &sheet_name,nat16 row, nat16 col)                const; //100115
   //abandoned virtual bool is_column_record_date(const Column &column)               const; //200618
 private:  // adjuncts The following are maintained as the file format is determined
    nat16 date_column;
};
//_ED_tabular_file_format______________________________________________________/
#endif
//filefmt

