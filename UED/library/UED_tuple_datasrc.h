#ifndef UED_tuple_datasrcH
#define UED_tuple_datasrcH

// This is a Data_source class that provides a table oriented interface
// to UED database using Data_records.

#include "UED/library/database_file.h"
#include "corn/data_source/datasrc.h"
#include "corn/chronometry/date_time_64.h"

#include "corn/container/unilist.h"
namespace UED {                                                                  //110911
//______________________________________________________________________________
class Tuple_data_source
: public CORN::Data_source_abstract                                              //110911
{
   //___________________________________________________________________________
   class Field_creation
   : public Item
   {
   public:
      std::string section;
      std::string entry;
      const Data_record_creation_layout *creation_layout;
      bool  owns_creation_layout;
      Units_code preferred_units;
   public:
      inline Field_creation
       (  const char *section_name_
       ,  const char *entry_name_
       ,  const Data_record_creation_layout *creation_layout_
       ,  bool owns_creation_layout_
       ,  Units_code preferred_units_)
       : Item()
       , section               (section_name_)
       , entry                 (entry_name_)
       , creation_layout       (creation_layout_)
       , owns_creation_layout  (owns_creation_layout_)
       , preferred_units       (preferred_units_)
       {}
   };
   //_Field_creation___________________________________________________________/
protected:
   Database_file_indexed    *database;
   Unidirectional_list      field_creations;
   CORN::Date_time_clad_64  active_date_time;
private: // the following are common to all records
    Record_code             record_type_code;
    UED_units_code          time_step_units_code;
    CORN::Quality_code      default_attribute_code;
public: // Initialization
   Tuple_data_source                                                             //040924
      (Database_file_indexed *database
      ,Record_code            record_type_code            // NYI = 0
      ,Units_code             time_step_units_code        // NYI = 0
      ,Quality_code           default_attribute_code);    // NYI = 0
   void add_field
      (const char *section_name
      , const char *entry_name
      ,const Data_record_creation_layout *creation_layout
      , bool owns_creation_layout
      ,Units_code preferred_units);
      // This establishes a field with the specified name.
      // This field will be used to create UED data records having the specified layout.
      // add_field should be called for each section/entry in Data_records encountered by this database.
      // When creating a database, fields should be set up (calling this function) before calling set()
      // If owns_creation layout is true, the Field_creation will own the creation layout and will
      // delete it when Field_creation is destroyed.
   void add_field2                                                               //040924
      (const char          *section_name
      ,const char          *entry_name
      ,UED::Variable_code   variable_code
      ,Units_code           UED_stored_units_code // Units as stored in the database
      ,Units_code           UED_timestamp_step_units_code);
public: // Data_source overrides
   virtual bool get(CORN::Data_record &data_rec) ;
      // This reads the data record from the current record position in the database
      // (I.e. current record in the database)
      // If the database is not in a state where is can read any data it returns false.
      // If the database could be read, but there were not maching entries (fields)
      // then it returns false.
      // Returns true if any record entries could be read.
      // Examine each data record entry to determine if it was encountered
      // in the data source.
   virtual const char *label_string(std::string &buffer)                  const; //170423
protected:
   virtual bool set_data(CORN::Data_record &data_rec,bool append);               //150503_050705
      // This writes data from data_rec to this data source.
      // Classes derived from Data_record may provide information such
      // as the position of the record in the data file an the Data_source
      // may then need to position the file pointer before writing the record.
      // In classes such as VV_file, a Data_record constitutes all the data in
      // a file, consequently the data can be written out to the file.
      // Returns true if the record could be written.
      // Returns true if there was a problem.
      // This abstract method must be provided by a derived class.
public: //
   void set_date(datetime64 date_time);
      // this should be renamed set_datetime64
      // This must be called before get() or set() to
      // specify the date to store or retrieve from.
      // Or if a date field is specified.
private:
   Field_creation *get_field_creation
      (const char *section_name
      ,const char *entry_name);
};
//_class Tuple_data_source_____________________________________________________/
}//_namespace UED______________________________________________________________/
#endif

