#ifndef tabularH
#define tabularH


/* This class defines the tabular ordering of tabular files

// used by
// ued\convert\export.h
// ICASA I/O classes

Tabular_data_column is otherwise deprecated, not for future used

*/
#include "corn/data_type_enum.h"
#include "corn/enum.h"
#include "corn/chronometry/date_time_64.h"
#include "corn/validate/validtyp.h"
#include "corn/parameters/parameter.h"

#include "corn/tabular/column.h"

namespace CORN {

// conceptual put under Tabular namespace

//______________________________________________________________________________
class Tabular_column     // meta properties  // This is actually a field, not necesarily a column

   // Replacing  this with Delineation_column


: public Parameter_properties_string                                             //100116
, public Parameter_number_properties                                             //100116
{
 public:
   Data_type_clad data_type_clad;

   Delineation_sheet sheet;                                                      //200302
   /*200302 obsolete   moved  Delineation sheet
   std::string       sheet;
      // For workbook programs (I.e. Excel, this is the worksheet the column occurs on)
      // It may be left blank if there is only one worksheet in the workbook
      // or if all worksheets have the same format.
   */
/*200402  Moved to Delineation_field
   nat16             position;  // 1 based  //was start_number                   //190325
      // Only relevent/use for text files.
      // Normally omitted (0) which indicates the position will be determined
      // from the relative position in the listed columns calculated from
      // field widths.
      // When specified (none 0) this is the absolute character position.

*/
   /*but need for */ // In the case of FORTRAN like carridge control,
   /*editor */       // carridge control character is at position 0,
                     // so data is reletive to the first character after the carriage control.
                     // Plain text file character positions are counted from 1.
                     // (there should be no column defined for the carridge control characters position.
                     // This is 1 based for excel columns
                     // start_number will be 0 if the column is not delineated by position and width
                     // (but rather delineated by delimiters)
   //180510  These have not really been used, but this has been
   // conceptually implemented for handling layered output (I.e. soil hydrology by layer)
   /*200302 obsolete   moved  Delineation sheet
   nat16             number;  // column are numbered from 1                      //190325
      // Normally omitted (0) in TDF files which indicates the position will be
      // determined from the relative position in the listed columns.
      // Used for Excel (or the new text workbook).
      // A none 0 value overrides the relative column indexing
      // Also when a none 0 value is encountered the following columns with
      // column_numbered 0 will be relative from the specified column_number.
   */
   //200224 bool              spread_over_columns;
   //200224 int16             spread_columns;
   nat16 spread;                                                                 //200224
      // This is the number of columns including this column
      // to reserve for spreading data set and layered variables
      // Usually 1.
      // May be omitted (0) indicates no spreading

      //so I think spread_over_columns can be omitted
   /*200302
   std::string       missing_value;    // For text this might be the word "missing" or "N/A", for numbers this might be "99999.99" //070524
      // The missing value is now property of Delineation
   */

   // These were added for AgMIP smallholder project to get the site and expertiment name
   nat8              parent_path;                                                //181104
      // This is most relevent to collator (but generally) applicable
      // To output the parent directory name relative
      // to the current working directory.
   std::string       static_text;                                                //181104
      // Specifies that there is no data to export, but output static text value
   bool              temporal;                                                //200618
         // True if the column denote the record date (or part of the record date (I.e. year)   //041004
         // perhaps rename record_date to temporal

 public: // constructor
   Tabular_column();
 public:
   virtual bool setup_structure(Data_record &data_rec,bool for_write);
};
//_Tabular_column______________________________________________________________/
class Tabular_data_column
// maybe rename to Tabular field (column) this is where data is stored (in memory)
: public Item
{  // This is used to store data values that are read from a record
 public:
   CORN::Tabular_column  &TC_layout;
 public: // When the file is read the record column values will be read into one of the following (expect structure needs a place to store values)
   // Consider these values as like a union, only one of these is relevent depending on the column's field type
   bool           value_bool;
   std::string    value_string;
   Date_time_clad_64   value_datetime;  // Use for date, time and datetime values.
   Date_format *date_format; // owned may be 0 (default) if not specified        //170531
   Time_format *time_format; // owned may be 0 (default) if not specified        //170531
   float32        value_float32;
   float64        value_float64;
   int16          value_int16;
   int32          value_int32;
   Validation_status validation_status;
 public: // constructor
   Tabular_data_column(CORN::Tabular_column  &_layout);
   virtual ~Tabular_data_column();                                               //170531
 public:
   inline virtual bool is_key_string(const std::string &key)       affirmation_  //180820
      { return TC_layout.is_key_string( key); }
   float32 get_value_as_float32()                                         const;
   int16 get_value_as_int16()                                             const; //050412
   int16 set_int_value(int16 from_int16)                          modification_; //070203
   // set_int_value  is used in UED export for date component fields
   // It sets each of the integer fields because at any time the user could change the data type of the field
   float64 set_float64_value(float64 from_float64)                modification_; //141021
   float32 set_float_value(float32 from_float32)                  modification_; //070203
   // set_float_value  is used in UED export for date component fields
   // It sets each of the float fields because at any time the user could change the data type of the field
   void clear_values();                                                          //100126
};
//_Tabular_column______________________________________________________________/
//Note when parsing the input record, store the int value and float value into each compatible value_xxx
//(this will be done by the data source
}//_namespace_CORN_____________________________________________________________/
#endif

