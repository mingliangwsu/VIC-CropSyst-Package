#include "UED/library/database_file.h"
#include <math.h>
#include "corn/chronometry/date_32.h"
#include "corn/measure/measures.h"
#include "corn/string/strconv.hpp"
#include "UED/library/UED.h"
#include "UED/library/controls.h"
#include "UED/library/varrec.h"
#include "UED/library/unitsrec.h"
#include "UED/library/marker.h"
#include "UED/library/locrec.h"
#include "UED/library/timeqry.h"
#include "UED/library/datasetrec_creation.h"
#include "common/geodesy/geolocation.h"
#include "UED/library/locrec.h"
#ifndef text_listH
#  include "corn/container/text_list.h"
#endif
#define BINARY_RECORD_INDEX_NODE  Binary_record_index_node_dynamic
namespace UED {
//_____________________________________________________________________________/
Binary_record *Database_file::append_record
(Binary_record *bin_record_to_append,bool write_immediately)         submission_
{
//NYI          if (it is a variable definition) add it to var def index
//NYI          if (it is a units    definition) add it to var def index
   Record_base *record_to_append = (Record_base *)bin_record_to_append;
   if (record_to_append->has_data())
         current_data_rec = (Binary_data_record_cowl *)record_to_append;
   else  current_nondata_rec =  record_to_append;
   return Binary_record_file_dynamic_indexed::append_record
         (bin_record_to_append,write_immediately);                               //050522
}
//_2000-07-06__________________________________________________________________/
Binary_record *Database_file_indexed::append_record(Binary_record *bin_record_to_append,bool write_immediately)
{  Database_file::append_record(bin_record_to_append,write_immediately);
   Record_base *record_to_append = (Record_base *)bin_record_to_append;
   if (record_to_append->has_data())
      indexes.append_record((Binary_data_record_cowl *)record_to_append,*this);
   return bin_record_to_append;
}
//_2000-07-06__________________________________________________________________/
Database_file::Database_file
(const char *_UED_filename                                                       //971208
,std::ios_base::openmode _openmode                                               //130311
,bool _auto_float16)                                                             //140612
: Binary_record_file_dynamic_indexed(_UED_filename,true,_openmode,false)         //050522
, current_nondata_rec      (0)                                                   //070619
, geolocation_rec_in_memory(0)                                                   //070619
, current_data_rec         (0)                                                   //070619
, auto_float16(_auto_float16)                                                    //140612
{  BOF_code = UED_marker_BOF;                                                    //030208
   EOF_code = UED_marker_EOF;                                                    //030208
   free_code= UED_free_space;                                                    //030208
}
//_____________________________________________________________________________/
bool Database_file::initialize()
{  bool inited = Binary_record_file_dynamic_indexed::initialize();
   if (inited)
      if (index.count() == 0)                                                    //050522
         append_record(new Marker_BOF_record (UED_VERSION_CODE),true);           //040720
   return inited;                                                                //140119
}
//_initialize__________________________________________________________________/
Binary_record *Database_file::instanciate_record(Record_code code) const
{  Record_base *record = 0;
   switch (code)
   {  case UED_marker_BOF                    : record = new Marker_BOF_record();   break;
      case UED_marker_1                      : record = new Marker_record(1);      break;
      case UED_marker_end_definitions        : record = new Marker_record(2);      break;
      case UED_marker_3                      : record = new Marker_record(3);      break;
      case UED_marker_4                      : record = new Marker_record(4);      break;
      case UED_marker_5                      : record = new Marker_record(5);      break;
      case UED_marker_6                      : record = new Marker_record(6);      break;
      case UED_marker_EOF                    : record = new Marker_record(7);      break;
      case UED_general_comment               : record = new General_comment_record();        break;
      case UED_database_description          : record = new Database_description_record();   break;
      case UED_generating_application        : record = new Generating_application_record(); break;
      case UED_period                        : record = new Period_record("");   break; //040913
      case UED_free_space                    : record = new Free_space_record(0);break; //030208
      case UED_geolocation_information       : record = new Geolocation_record();break;
      case UED_variable_definition_format_0  : record = new Variable_definition_format_0_record();       break;
#ifdef NYI
      case UED_variable_definition_format_1  : record = new UED_variable_definition_format_1_record();   break;
      case UED_variable_definition_format_2  : record = new UED_variable_definition_format_2_record();   break;
#endif
      case UED_variable_definition_format_ontological  : record = new Variable_definition_format_ontological_record();   break;
      case UED_units_definition_format_0     : record = new Units_definition_format_0_record();          break;
#ifdef NYI
      case UED_units_definition_format_1     :
         record = new UED_units_definition_format_1_record();
         result = (Record_base *) record;
      break;
      case UED_units_definition_format_2     : record = new UED_units_definition_format_2_record();      break;
      case UED_units_definition_format_3     : record = new UED_units_definition_format_3_record();      break;
#endif
      default :
      {  Record_code_clad smart_code(code);
         if (smart_code.is_data_set())
            record = new Binary_data_record_clad(code); // handles both scalars and data sets
         else if (smart_code.is_data_scalar())
            record = new Binary_data_record_clad(code); // handles both scalars and data sets
         else if (smart_code.is_data_vector())
            record = new Vector_data_record(code);
// I disabled this, I don't remember why it should never occur
//                     else
//                        record = new Record_base(); // <- this catchall should be avoided
      }
   }
   return record;
}
//_instanciate_record_______________________________________________1999-01-14_/
const char *Database_file::get_variable_description
(Variable_code variable_code
,std::string &description)                                                       //170424
{
   const Variable_format_abstract *var_def = get_variable_definition(variable_code);
   if ((var_def == 0))
   {  description.assign("Undefined UED variable, binary:");
      CORN::append_nat32_to_string  (variable_code,description,2);
   } else description.assign(var_def->ref_description());
   return description.c_str();
}
//_get_variable_description_________________________________________1997-11-05_/
std::string &Database_file::get_units_description(UED_units_code units_code,bool abbreviated)
{  Units_definition *units_def = get_units_definition(units_code);
   static std::string undefined_units_description;                               //040908
   if (units_def == 0)
   {  undefined_units_description.assign(
      (abbreviated ? "?":"Undefined UED units, binary:"));                       //990507
      if (!abbreviated)                                                          //990507
      { char binary_code[50];
        CORN::int32_to_cstr(units_code,binary_code,2);                           //001126
        undefined_units_description.append(binary_code);
      }
   } else  // We have a special UED units definition
      if (abbreviated)
         units_def->get_abbreviation(undefined_units_description);
      else                                                                       //030125
         units_def->Units_clad::label_string(undefined_units_description);       //170423
   return undefined_units_description;
}
//_1997-11-05__________________________________________________________________/
bool Database_file::write_general_comment(const char *general_comment)
{  bool written = true;                                                          //091021
   General_comment_record *gen_cmnt_rec = new General_comment_record (general_comment);
   if (find_duplicate_record(gen_cmnt_rec))                                      //091021
   {  delete gen_cmnt_rec; written = false; }
   else append_record(gen_cmnt_rec,true);                                        //000706
   return written;
}
//_write_general_comment____________________________________________1997-12-08_/
bool Database_file::write_generating_application(nat16 version,const char *application_name)
{  bool written = true;
   Generating_application_record *app_rec = new Generating_application_record(version,application_name);
   if (find_duplicate_record(app_rec))
   {  delete app_rec; written = false; }
   else  append_record(app_rec,true);                                            //000706
   return written;
}
//_write_generating_application_________________________2009-10-21__1997-12-08_/
bool Database_file::write_database_description(const char *database_description)
{  bool written = true;
   Database_description_record *desc_rec = new Database_description_record(database_description);
   if (!find_duplicate_record(desc_rec))
   {  delete desc_rec; written = false; }
   else append_record(desc_rec,true);                                            //000706
   return written;
}
//_write_database_description___________________________2009-10-21__1997-12-08_/
nat16 Database_file::get_descriptions(CORN::Text_list &descriptions)  stream_IO_
{  int description_count = 0;
   goto_BOF();                                                                   //050523
   for (Database_description_record *desc_rec = (Database_description_record *)
         goto_next_record_of_type(UED_database_description)
       ;desc_rec
       ;desc_rec = (Database_description_record *)goto_next_record_of_type(UED_database_description))
   {  descriptions.append(new Item_string(desc_rec->get_database_description().c_str()));
      description_count++;
   }
   return description_count;
}
//_get_descriptions_________________________________________________2009-10-21_/
nat16 Database_file::get_comments(CORN::Text_list &comments)          stream_IO_
{  int comment_count = 0;
   goto_BOF();                                                                   //050523
   for (General_comment_record *desc_rec = (General_comment_record *)
         goto_next_record_of_type(UED_general_comment)
       ;desc_rec
       ;desc_rec = (General_comment_record *)goto_next_record_of_type(UED_general_comment))
   {  comments.append(new Item_string(desc_rec->get_general_comment().c_str()));
      comment_count++;
   }
   return comment_count;
}
//_get_comments_____________________________________________________2009-10-21_/
void Database_file::write_end_definitions_marker()
{  append_record(new Marker_record(UED_marker_end_definitions),true);            //000706
}
//_write_end_definitions_marker_____________________________________1997-12-07_/
float32 Database_file::get_value
(Variable_code    variable
,const Time_query    &time_query                                                 //990118
,UED_units_code      &requested_units
,CORN::Quality       &attribute)
{  //  We now only work with the current data record
   bool variable_code_available = current_data_rec
      && (current_data_rec->get_variable_code() == variable);                    //040125
   bool timestep_available      = current_data_rec
     && current_data_rec->contains_time_step                                     //040125
         (variable,time_query.get_target_date_time());
   if (! variable_code_available || ! timestep_available)                        //040125
   {  attribute.invalidate(true); //   Indicate that the requested variable is missing
      return 0.0;
   }
   // get_record always returns are record (creating a new one if necessary
   // This is why we need to find_record_date_time() to see if such a record exists.
   UED_units_code       record_actual_units = current_data_rec->get_units_code();
   CORN::Quality_clad   datum_attribute;
   float32 datum_value = current_data_rec->get_value(datum_attribute,time_query.get_target_date_time());   //990119
   // will return datum_attribute negative (bit 8 set to 1) if there is no data

   // We will try to convert the value, if the conversion cannot
   // be done, we return the datum units.
   float32 converted_value = datum_value;
   if (requested_units == UED_any_units)
      requested_units = record_actual_units;
   else
      if (!convert(datum_value,record_actual_units,converted_value,requested_units))
          requested_units = record_actual_units;
   attribute.assume(datum_attribute);                                            //150120
   // In the future, if datum_attribute is missing data, we will try
   // to produce a resonable value.
   return converted_value;
}
//_get_value________________________________________________________1997-08-05_/
bool Database_file::get_parameter_on
(modifiable_ CS::Parameter &parameter
,Variable_code     variable_code
,const Time_query &time_query
,Record_code)                                             stream_IO_ //190122 const
{  bool got = false;
   bool variable_code_available = current_data_rec
      && (current_data_rec->get_variable_code() == variable_code);
   bool timestep_available      = current_data_rec
      && current_data_rec->contains_time_step(variable_code,time_query.get_target_date_time());
   if (! variable_code_available || ! timestep_available)
   {  parameter.invalidate(true); // Indicate that the requested variable is missing
      got = false;
   } else                                                                        //151124
   got = current_data_rec->get_parameter_on(parameter,time_query.get_target_date_time());
   return got;
}
//_get_parameter_on_________________________________________________2015-11-19_/
bool Database_file::set_parameter_on
(const CS::Parameter &parameter
,Variable_code     variable_code
,const Time_query &time_query
,Record_code       /*170216unused rec_type*/)
{  bool sat = false;
   bool variable_code_available = current_data_rec
      && (current_data_rec->get_variable_code() == variable_code);
   bool timestep_available      = current_data_rec
      && current_data_rec->contains_time_step(variable_code,time_query.get_target_date_time());
   if (! variable_code_available || ! timestep_available)
   {
      sat = false;
   } else                                                                        //151124
   sat = current_data_rec->set_parameter_on(parameter,time_query.get_target_date_time());
   return sat;
}
//_set_parameter_on_________________________________________________2015-11-19_/
/*151122 conceptual
bool Database_file::setup_parameter_reference
(modifiable_ CS::Parameter_cowl_T<float32> &parameter
,Variable_code     variable_code
,const Time_query &time_query)                                             const
{  bool satup = false;

   bool variable_code_available = current_data_rec && (current_data_rec->get_variable_code() == variable_code);
   bool timestep_available      = current_data_rec && current_data_rec->contains_time_step(variable_code,time_query.get_target_date_time());
   if (! variable_code_available || ! timestep_available)
   {
      parameter.invalidate(true); // Indicate that the requested variable is missing
      satup = false;
   }
   satup = current_data_rec->setup_parameter_reference(parameter,time_query.get_target_date_time());
   return satup;
}
//_2015-11-19__________________________________________________________________/
*/
Binary_data_record_cowl *Database_file_indexed::locate_data_record
(Record_code          rec_type
,Variable_code        var_code
,const Time_query    &time_query)                                     stream_IO_
{  // Since the current data record is not what we are working with,
   // try to activate the data record.
   if (!initialized) initialize();                                               //000829
   Binary_data_record_cowl *located_record = indexes.get_record(rec_type,var_code,time_query,*this);
   return located_record;
}
//_locate_data_record_______________________________________________1999-01-11_/
Binary_data_record_cowl *Database_file_indexed::locate_or_create_data_record
(const Time_query                   &time_query                                  //990118
,const Data_record_creation_layout  &creation_layout
,bool                               &record_created)                modification_//000405
{  record_created = false;
   // Since the current data record is not what we are working with,
   // try to activate the data record.
   Binary_data_record_cowl *located_record = locate_data_record                  //990115
         (creation_layout.get_code(),creation_layout.variable_code,time_query);
   // If an index already has the record to locate in memory that's all
   if (!located_record)
   {  Record_code_clad creation_record_type(creation_layout.options.record_type);//070214
      located_record = new Binary_data_record_clad
         (creation_layout
         ,creation_record_type.is_data_set()
            ?  time_query.get_low_date_time() // low date time at this point should be the earliest date for the new record
            :  time_query.get_target_date_time()
         ,auto_float16);                                                         //140612
      record_created = true;                                                     //000405
      append_record(located_record,false);                                       //000706
   }
   current_data_rec = located_record;                                            //000706
   return located_record;
}
//_locate_or_create_data_record_____________________________________1999-01-11_/
float32 Database_file_indexed::get_no_create
(Record_code          rec_type                                                   //040125
   // We always must specify the record type code because we may
   // have different types of records with the same variable
,Variable_code    variable_code                                                  //040125
,const Time_query    &time_query                                                 //990118
,UED_units_code      &requested_units
,CORN::Quality       &attribute)                                      stream_IO_ //160331
{  float32 result = 0;                                                           //000710
   attribute.assume_code(CORN::missing_quality);                                 //160331
   Binary_data_record_cowl *found_record = locate_data_record(rec_type ,variable_code,time_query);   //000518
   if (found_record)                                                             //000518
   {  current_data_rec = found_record;                                           //000706
      result = Database_file::get_value(variable_code,time_query,requested_units,attribute);    //990902
   }
/*170311 moved to CS_UED::Database_file_indexed::get_no_create
   else if (!substituted_time_query)
      //161106now date_substituters would enabled (substituted_time_query)// Try substituting the target date                                      //160331
   {  // This is a special mode to provided data during periods that don't actually exist in the file
      Time_query *alt_time_query
         = substitute_time_query(rec_type,variable_code,time_query);
      if (alt_time_query)
      {  result = get_no_create(rec_type,variable_code,*alt_time_query,requested_units,attribute,true);
         delete alt_time_query;
      } // There could be no such variables in the database at all.
   } // dont try further date substitutions the value simply is not in the database
*/
   return result;                                                                //000710
}
//_get_no_create____________________________________________________1997-08-05_/
bool Database_file_indexed::get_parameter_on
(modifiable_ CS::Parameter &parameter
,Variable_code     variable_code
,const Time_query &time_query
,Record_code       rec_type)                                          stream_IO_ //160331
{  bool result = false;
   Binary_data_record_cowl *found_record = locate_data_record(rec_type ,variable_code,time_query);
   if (found_record)
   {  current_data_rec = found_record;
      result = Database_file::get_parameter_on(parameter,variable_code,time_query,rec_type);
   }
   return result;
}
//_get_parameter_on_________________________________________________2015-11-19_/
bool Database_file_indexed::set_parameter_on                                                 //180808
(const CS::Parameter &parameter
,const Time_query &time_query
,const Data_record_creation_layout &creation_layout
)
{  bool result=false;
   Binary_data_record_cowl *provided_record = 0;
//  provided_record = locate_data_record(rec_type ,variable_code,time_query);
//   if (!provided_record)
   {
      bool record_created = false;


      provided_record = locate_or_create_data_record    // actually just create
         (time_query,creation_layout,record_created);
   }
   if (provided_record)
   {
      current_data_rec = provided_record;
      result = Database_file::set_parameter_on
         (parameter,creation_layout.variable_code,time_query
         ,creation_layout.options.record_type);
   }
   return result;
}
//_set_parameter_on_________________________________________________2018-08-08_/
/*151122 conceptual
bool Database_file_indexed::setup_parameter_reference
(modifiable_ CS::Parameter_cowl_T<float32> &parameter
,Variable_code     variable_code
,const Time_query &time_query
,Record_code       rec_type)                                      rectification_
{  bool result = false;
   Binary_data_record_cowl *found_record = locate_data_record(rec_type ,variable_code,time_query);
   if (found_record)
   {  current_data_rec = found_record;
      result = Database_file::setup_parameter_reference(parameter,variable_code,time_query);
   }
   else if (substitute_dates)
   {
      CORN::Date earliest_date(get_earliest_date_time(rec_type,variable_code));
      CORN::Date latest_date  (get_latest_date_time(rec_type,variable_code));
      CORN::Year earliest_year = earliest_date.get_year();
      CORN::Year latest_year   = latest_date.get_year();
      if ((earliest_year == 9999) || (latest_year == -9999))
         parameter.assume_code(CORN::missing_quality);
      else // No such records exist in the database so we can't substitute
      {
         nat16 years_range        = latest_year - earliest_year;
         CORN::Year query_year    = time_query.get_year();
         CORN::Year substitute_year = query_year;
         while (substitute_year < earliest_year)
            substitute_year += years_range;
         Time_query substitute_time_query(time_query);
         substitute_time_query.offset_year(substitute_year - query_year);
         while (substitute_time_query.get_DOY() > substitute_time_query.days_in_this_year())
            substitute_time_query.dec();
            // if source year is a leap year the subsituted year might not be a leap year
            // The use of while loop will accomodate alternate calendar systems.
         found_record = locate_data_record(rec_type ,variable_code,substitute_time_query);
         if (found_record)
         {
            current_data_rec = found_record;
            result = Database_file::setup_parameter_reference(parameter,variable_code,time_query);
         }
      }
   }
//   else attribute.assume_code(CORN::missing_quality);
   return result;
}
//_2015-11-19___________________________________________________________________
 * */

bool Database_file::current_data_record_is_target
(Record_code     target_rec_code
,Variable_code   target_var_code)
{  if (!current_data_rec) return false;
   Variable_code curr_var_code = current_data_rec->get_variable_code();
   return
      (current_data_rec &&
       ((current_data_rec->get_code() == target_rec_code) &&
        (curr_var_code                == target_var_code)));
}
//_current_data_record_is_target____________________________________1999-01-18_/
float32 Database_file::set_value
(float32 value
,const Time_query &time_query                                                    //990118
,UED_units_code      &units
,CORN::Quality &attribute                                                        //000710
,const Data_record_creation_layout &creation_layout
,bool  &//record_created // not used here                                        //000405
,bool override_safety)
{  // The Database_file version of set can only work with the current
   // data record.  This method will be overridden by the UED_index_database
   // to search and load relevent records.
   float32 converted_value;
   float32 result =
    (current_data_record_is_target                                               //990118
          (creation_layout.options.record_type
          ,creation_layout.variable_code))
    ?
      // We can only set data if the record is of the target type.
       (!convert(value,units,converted_value,current_data_rec->get_units_code()))
      ? 0.0 // false This should help the programmer see when the problem occurs
      //  Couldn't convert the data don't attempt set values in the wrong units.
      : current_data_rec->set_value(converted_value,attribute,time_query.get_target_date_time(),override_safety)
   : 0.0;                                                                        //990118
   return result;
}
//_set_value________________________________________________________1997-11-01_/
float32 Database_file_indexed::set_value
(float32 value
,   const Time_query &time_query                                                 //990118
   // UED will try to give the value for the specified date and time.
   // If the date and time is not available, UED will try to estimate
   // a value (the returned attribute will be set accordingly
   // If the value cannot be estimated, the attribute will also be set accordingly
   //   If a record doesn't already exist for the time period matching the date and time,
   //   A record will be created having the specified units.
   ,UED_units_code      &units
   // The program can specify the units of the variable.
   //   UED will try to convert the value to the units the data is stored in
   //   if it can't figure out how to do that, set fails returning false.
              //   Note that units conversion is a feature of the
              //   UED database class, not the record itself.
   ,CORN::Quality &new_attribute    // must be a returned value returns          //971104
   // The program can specify the desired attribute,
   //   UED will try to give a value for the attribute
   //   attribute will be set to the attributes available
   // UED will check the current record attribute; if the attribute
   // doesn't match it will check if individual timestep attributes
   // exist and will not overwrite values of higher quality unless
   // override safty is enabled.
   ,const Data_record_creation_layout &creation_layout
   // If a time series record doesn't exist for the specified
   // variable code with the specified date and time, a new record
   // will be created using this layout.
   ,bool  &record_created                                                        //000405
   ,bool override_safety)
{  Binary_data_record_cowl *data_rec = locate_or_create_data_record(time_query,creation_layout,record_created);
   float32 converted_value;
   bool converted_ok = convert(value,units,converted_value,data_rec->get_units_code()); //000720
   float32 result =                                                              //000710
   converted_ok
   ? data_rec->set_value(converted_value,new_attribute,time_query.get_target_date_time(),override_safety)
   : 0.0; //  Couldn't convert the data don't attempt set values in the wrong units.
   return result;                                                                //000710
}
//_set_value______________________________________________________________1997_/
bool Database_file::set_vector
(const Dynamic_array<float32> &values_array
,datetime64                 date_time                                            //030714
,UED_units_code            &units
,CORN::Quality             &attribute    // must be a returned value
,const Data_record_creation_layout &creation_layout
,bool                      &record_created)                        modification_//000405
{  // The Database_file version of set can only work with the current
   // data record.  This method will be overridden by the UED_index_database
   // to search and load relevent records.
   bool result = true;
   record_created = false;                                                       //000707
   if (!current_data_rec)                                                        //000401
   {  Binary_data_record_cowl *vector_rec = new Binary_data_record_clad
         (creation_layout,date_time,auto_float16);                               //140612
      append_record(vector_rec,false);                                           //000706
      record_created = true;                                                     //000706
   }
   if (current_data_record_is_target(creation_layout.options.record_type
          ,creation_layout.variable_code) && current_data_rec->get_time_stamp() == date_time)   //030714
   {  // We can only set data if the record is of the target type.
      if (current_data_rec->get_code() != creation_layout.options.record_type )
         return false;
      Vector_data_record *vector_record=(Vector_data_record *)current_data_rec;
      for (nat32  i = 0 ; i < values_array.get_count() ; i++)                    //070226
      {  float32 converted_value;
         if (!convert(values_array.get(i),units,converted_value,vector_record->get_units_code())) //100321_991121
            return false;
         //  Couldn't convert the data don't attempt set values in the wrong units.
         vector_record->set(i,converted_value,attribute);
   }  }
   return result;
}
//_set_vector_______________________________________________________1998-09-25_/
bool Database_file_indexed::set_vector
(const Dynamic_array<float32>      &values
,datetime64                         date_time                                    //030714
,UED_units_code                    &units
,CORN::Quality                     &new_attribute    // was a returned value
,const Data_record_creation_layout &creation_layout
,bool                              &record_created)                              //000405
{  Time_query_single_date time_query
      (date_time,creation_layout.options.timestep_units_code,Time_query::at);    //040203
   bool unused= false;
   locate_or_create_data_record(time_query,creation_layout,record_created);
   return Database_file::set_vector(values,date_time,units,new_attribute,creation_layout,unused);
}
//_set_vector_______________________________________________________1998-09-25_/
int16 Database_file::get_vector
(float32             vector_data[]
,Record_code         rec_type                                                    //990118
,Variable_code       variable
,const Time_query   &time_query                                                  //990118
,UED_units_code     &requested_units
,CORN::Quality      &attribute
,bool  &/*not used record_created  */)                                           //000405
{  // sanity check:
   if (!current_data_rec) return 0;
   bool is_target = current_data_record_is_target(rec_type,variable);
   bool contains_timestep =
      (time_query.query_mode == UED::Time_query::all)
      ||
      current_data_rec->contains_time_step(variable,time_query.get_target_date_time());   //990118
   bool is_data_vector = current_data_rec->is_data_vector();
   if (!is_target || !contains_timestep || !is_data_vector)                      //970919
   {  attribute.invalidate(true); // Indicate that the requested attribute is missing  //021222
      return 0;
   }
   Vector_data_record *curr_vector_record = (Vector_data_record *)current_data_rec;
   // get_record always returns are record (creating a new one if necessary
   // This is why we need to find_record_date_time() to see if such a record exists.
   UED_units_code       record_actual_units = curr_vector_record->get_units_code();
   Dynamic_array<float32> &target_vector = curr_vector_record->ref_data_values();//151111_120804_990427
   for (nat16 i = 0 ; i < curr_vector_record->get_vector_size(); i++)
   {  float32 datum_value = target_vector.get(i);                                //990427
      // will return datum_attribute negative (bit 8 set to 1) if there is no data
      // We will try to convert the value, if the conversion cannot
      // be done, we return the datum units.
      if (requested_units == UED_any_units)
         requested_units = record_actual_units;
      else
         if (!convert(datum_value,record_actual_units,vector_data[i],requested_units))
             requested_units = record_actual_units;
    }
    // In the future, if datum_attribute is missing data, we will try
    // to produce a resonable value.
   return curr_vector_record->get_vector_size();
}
//_get_vector_______________________________________________________1998-09-28_/
int16 Database_file_indexed::get_vector
(float32              vector_data[]
,const Time_query    &time_query                                                 //990118
,UED_units_code      &requested_units
,CORN::Quality       &attribute
,const Data_record_creation_layout  &creation_layout
,bool  &record_created)                                                          //000405
{  locate_or_create_data_record(time_query,creation_layout,record_created);      //990118
   return Database_file::get_vector
      (vector_data
      ,creation_layout.options.record_type
      ,creation_layout.variable_code
      ,time_query                                                                //990118
      ,requested_units,attribute,record_created);
}
//_get_vector_______________________________________________________1998-09-28_/
int16 Database_file_indexed::get_vector
(float32              vector_data[]
,Record_code          rec_type                                                   //990118
,Variable_code        variable
,const Time_query    &time_query                                                 //990118
,UED_units_code      &requested_units
,CORN::Quality       &attribute
,bool                &record_created)                                            //000405
{  current_data_rec = locate_data_record(rec_type,variable,time_query);          //010216
   return  current_data_rec                                                      //010216
   ? Database_file::get_vector
    (vector_data,rec_type,variable
     ,time_query
     ,requested_units,attribute,record_created)
   : (int16)0;
}
//_get_vector_______________________________________________________1998-09-28_/
Record_base * Database_file::goto_BOF()                               stream_IO_
{  return (Record_base *)read_record(std::istream::beg);
}
//_goto_BOF_____________________________________________2000-07-05__1997-09-15_/
Record_base * Database_file::goto_next()                              stream_IO_
{  return (Record_base *)read_record(std::fstream::cur);
}
//_goto_next____________________________________________2000-07-05__1997-09-15_/
Record_base *Database_file::goto_next_record_of_type(Record_code record_type_code)
{  return (Record_base *)read_record_of_type(record_type_code,std::fstream::cur);
}
//_goto_next_record_of_type_________________2013-05-09__2000-07-05__1999-12-10_/
Binary_data_record_cowl * Database_file::goto_next_data_record()
{  bool at_data_record = false;
   do
   {  Record_base *currrec = goto_next();                                        //991220
     if (!currrec)                                                               //991220
         return 0;
      {  Record_code_clad rec_code(currrec->get_code());
         if (rec_code.has_data())
         {  current_data_rec = (Binary_data_record_cowl *)currrec;               //000714
            at_data_record = true;
         }
      }
   }  while (!at_data_record);
   return current_data_rec;
}
//_goto_next_data_record____________________________________________1999-04-30_/
////////////////////////////////////////////////////////////////////////////////
Database_file_indexed::Database_file_indexed
(const char *_filename                                                           //971208
,std::ios_base::openmode _openmode                                               //130311
,bool _auto_float16)                                                             //140612
: Database_file(_filename,_openmode,_auto_float16)                               //140612_130311_000711
, indexes()                                                                      //990111
{}
//_constructor_________________________________________________________________/
bool Database_file_indexed::finalize()
{  //  here we want to make sure all indexs Xlast are cleared because they may point to records that will be finished// delete_all();
   indexes.finish();
   return Database_file::finalize();
}
//_finish___________________________________________________________2000-10-26_/
bool Database_file_indexed::initialize()                                         //
{  if (initialized) return true;                                                 //140119_101009
   bool inited = Database_file::initialize();                                    //140119
  //Initially there are no indexes in memory,
  // created as record are requested.
   // We now have a list of record header indexes;
   // so scan the list for records that are time stamped data,
   // This actually only needs to be done when reading existing file.
   FOR_EACH_IN(header,BINARY_RECORD_INDEX_NODE,index,each_header)                //030709
   {  Record_code_clad rec_code(header->get_record_code());
      if (rec_code.has_data()) // Currently all records are time stamped
      {  // Read the variable and date info from the record
         Data_record_header *data_rec_header=(Data_record_header *)header->get(*this); //000705
         if (data_rec_header) //in BDS2006 I got 0, not sure if that is possible //070619
            indexes.update
            (header->get_record_code()                                           //000705
            ,data_rec_header->get_variable_code()
            ,data_rec_header->get_time_stamp()                                   //030714
            ,data_rec_header->get_final_time_series_date()
            ,data_rec_header->get_time_step_units_code()                         //040125
            ,header
            ,*this);
         header->finish(*this); // This will remove the record body from memory  //000706
      }
   } FOR_EACH_END(each_header)
   // In the case we are creating this database , the indexes will be empty.
   return inited;                                                                //140119
}
//_initialize_______________________________________________________2014-01-19_/
Dynamic_array<float32> *                                                         //120806
Database_file_indexed::get_record_buffer_array
(Variable_code        var_code
,const Time_query    &time_query                                                 //990118
,Dynamic_array<nat8> &attributes                                                 //120806_000818
,Record_type_distinguisher_interface::time_stamp_codes time_step_resolution)     //010525
{  //for each of the valid combinations of data record types matching the time step resolution
   // We want to search for various records with the same kind of variable
   nat8 code = Record_type_distinguisher_interface::UED_data_set_year_var_units;
   {  Record_code_clad smart_code(code);
      if (smart_code.is_data_set() &&
             (smart_code.get_time_stamp_code() == time_step_resolution))
      {  Binary_data_record_cowl *matching_record =
                locate_data_record(smart_code.get_code(),var_code,time_query);
         if (matching_record)
         {  attributes.set(matching_record->get_attributes());
            return &(matching_record->ref_data_values());                        //151111
   }  }  }
   return 0;
}
//_get_record_buffer_array______________________________1999-04-26__1999-01-14_/
void Database_file::include_var_code_def
(const Variable_format_0 &variable_def)
{  // Search variable definitions list for the specified variable_def.code
   const Variable_format_abstract *existing_var_def = get_variable_definition(variable_def.code_clad.get());
   bool writing_database = ! is_read_only();                                     //130311_040909
   if (!writing_database) return;                                                //990514
       // Only attempt to create an actual record if database is for output        990514
   // If it isn't already in the list create a definition record
   if (!existing_var_def)
   {  Variable_definition_format_0_record *new_var_def = new
      Variable_definition_format_0_record(variable_def);
         append_record(new_var_def,true);                                        //000706
   }
}
//_include_var_code_def_____________________________________________1999-04-26_/
void Database_file::include_var_code_def_ontological
(const Variable_format_ontological &variable_def)
{  // Search variable definitions list for the specified variable_def.code
   const Variable_format_abstract *existing_var_def = get_variable_definition(variable_def.code_clad.get());
   bool writing_database = ! is_read_only();                                     //130311_040909
   if (!writing_database) return;                                                //990514
       // Only attempt to create an actual record if database is for output        990514
   // If it isn't already in the list create a definition record
   if (!existing_var_def)
   {  Variable_definition_format_ontological_record *new_var_def = new
      Variable_definition_format_ontological_record(variable_def);
         append_record(new_var_def,true);                                        //000706
   }
}
//_include_var_code_def_ontological_________________________________1999-04-26_/
void Database_file::include_units_code_def
(const Units_definition &units_def)
{  // Search variable definitions list for the specified variable_def.code
   Units_definition *existing_units_def = get_units_definition(units_def.get_code());
   bool writing_database = !is_read_only();                                      //130311_040909
   if (!writing_database)                                                        //990514
      return; // Only attempt to create an actual record if database is for output //990514
   // If it isn't already in the list create a definition record
   if (existing_units_def == 0)
   {  Units_definition_format_0_record *new_units_def =
         new Units_definition_format_0_record(units_def);
      append_record(new_units_def,true);                                         //000706
   }
}
//_include_units_code_def___________________________________________1999-04-26_/
datetime64 Database_file_indexed::get_earliest_date_time
( Record_code   for_record_type_code
, Variable_code for_var_code )                                             const //170323
{
   datetime64 earliest_date_time(9999365);                                       //170404_030714
   const Time_stamped_record_item *earliest_record_item_pos_inf =                //170323
      indexes.get_earliest_record_pos_info                                       //990909
            ( earliest_date_time, for_record_type_code, for_var_code );          //990909
   const Binary_data_record_cowl *earliest_data_record =                         //170323_990909
      earliest_record_item_pos_inf ? earliest_record_item_pos_inf->
         ref_record(*this): 0;                                                   //170323
   if (earliest_data_record)                                                     //990909
      earliest_data_record->get_earliest_date_time(earliest_date_time);          //990909
   return earliest_date_time;                                                    //990909
}
//_get_earliest_date_time___________________________________________1997-09-15_/
datetime64 Database_file_indexed::get_latest_date_time
( Record_code  for_record_type_code
, Variable_code for_var_code )                                             const //170323
{
   datetime64 latest_date_time(-9999365);                                        //170404_030714
   const Time_stamped_record_item *latest_record_item_pos_inf =                  //170323
         indexes.get_latest_record_pos_info                                      //990909
            ( latest_date_time, for_record_type_code, for_var_code );            //990909
   const Binary_data_record_cowl *latest_data_record =                           //170323_990909
            latest_record_item_pos_inf? latest_record_item_pos_inf->
               ref_record(*this):0;
   if (latest_data_record)                                                       //990909
         latest_data_record->get_latest_date_time(latest_date_time);             //990909
   return latest_date_time;                                                      //990909
}
//_get_latest_date_time_____________________________________________1997-09-15_/
Units_definition *Database_file::get_units_definition(UED_units_code units_code,bool /*not used:abbreviated*/)
{  Units_definition *desired_units_def = 0;                                      //030716
   FIND_FOR_EACH_IN(found_node,record_node,BINARY_RECORD_INDEX_NODE,index,true,each_record_node)   //030716
   {  if (record_node->get_record_code() == UED_units_definition_format_0)       //030716
      {  Units_definition_format_0_record *units_def_record =
            (Units_definition_format_0_record *)record_node->get(*this);         //030716
         Units_code units_def_code = units_def_record->get();                    //030716
         if (units_def_code == units_code)                                       //030716
         {  found_node = record_node;                                            //030716
            desired_units_def = units_def_record;                                //030716
   }  }  } FOR_EACH_END(each_record_node)                                        //030716
   if (!desired_units_def)                                                       //030716
   {  Units_clad units_clad(units_code);
      char description[100];
      char abbreviation[100];
      if (units_clad.compose_description(description,abbreviation))              //990510
      {  Units_definition units_def(units_code,description,abbreviation);
         Units_definition_format_0_record *units_def_rec = new Units_definition_format_0_record(units_def);
         append_record(units_def_rec,true);
         return units_def_rec;                                                   //030716
         // Warning, will probably want to create a new record
      }
   }
   return desired_units_def;
}
//_get_units_definition_____________________________________________1997-11-05_/
const Variable_format_abstract *Database_file::get_variable_definition
(Variable_code variable_code)                                      modification_
{
   if (!initialized) initialize();                                               //141008
   Variable_format_abstract *desired_var_def = 0;
   {  // Search for variable definition records that have been indexed
      // and load them in memory if not already done so.
      // Note that once the record is in memory it is not necessarily reread.
      FIND_FOR_EACH_IN(found_node,record_node,BINARY_RECORD_INDEX_NODE,index,true,each_record_node)
      {  if (record_node->get_record_code() == UED_variable_definition_format_0)
         {  Variable_definition_format_0_record *var_def_record
               = (Variable_definition_format_0_record *)record_node->get(*this);
            Variable_code var_def_var_code = var_def_record->ref_smart_variable_code().get();
            if (var_def_var_code == variable_code)
            {  found_node = record_node;
               desired_var_def = var_def_record;
            }
         } else if (record_node->get_record_code() == UED_variable_definition_format_ontological) //160620
         {  Variable_definition_format_ontological_record *var_def_record
            = (Variable_definition_format_ontological_record *)record_node->get(*this);
            Variable_code var_def_var_code = var_def_record->ref_smart_variable_code().get();
            if (var_def_var_code == variable_code)
            {  found_node = record_node;
               desired_var_def = var_def_record;
            }
         }
      } FOR_EACH_END(each_record_node)
   }
   return  desired_var_def;
}
//_get_variable_definition_______________________________2003-07-16_2001-02-16_/
nat16 Database_file::get_variable_definitions
(Variable_format_abstract *variable_definitions[]                                //160619
,nat16 max_allocation)                                              //provision_
{
   nat16 vi = 0;
   nat16 variable_count = 0;
   goto_BOF();                                                                   //141027
   // Search for variable definition records that have been indexed
   // and load them in memory if not already done so.
   // Note that once the record is in memory it is not necessarily reread.
   FOR_EACH_IN(record_node,BINARY_RECORD_INDEX_NODE,index,each_record_node)
   {  if (record_node->get_record_code() == UED_variable_definition_format_0)
      {  Variable_format_0 *var_def =
            dynamic_cast <Variable_format_0 *>(record_node->get(*this));
         if (var_def && vi < max_allocation)
         {  variable_definitions[vi++] = var_def;
         }
      } if (record_node->get_record_code() == UED_variable_definition_format_ontological)
      {  Variable_format_ontological *var_def =
            dynamic_cast <Variable_format_ontological *>(record_node->get(*this));
         if (var_def && vi < max_allocation)
         {  variable_definitions[vi++] = var_def;
         }
      }
   } FOR_EACH_END(each_record_node)
   variable_count = vi;
   for (;vi < max_allocation; vi++) // zero out the rest of the array.
         variable_definitions[vi] = 0;


      // Fill the provided array with pointers to the current variable definitions.
      // The max_allocation is the maximum array size.
      // The variable definitions are provided (loaded in memory) as needed.
      // Returns the number of variable definitions filled into the array.
      // (The remaining points in the array are cleared to 0).
   return variable_count;
}
//_get_variable_definitions____________________________________________________/
/* NYI  160619
nat16 Database_file::get_variable_definitions_ontological
(Variable_format_ontological *variable_definitions[]
,nat16 max_allocation)                                                //provision_
{
continue here
}
*/
//______________________________________________________________________________
nat16 Database_file::get_defined_variables_codes
(nat32 variable_codes[],nat16 max_allocation)
{
   nat16 variable_count = 0;
   nat16 vi = 0;   //variable index
   goto_BOF();                                                                   //141027
   // Search for variable definition records that have been indexed
   // and load them in memory if not already done so.
   // Note that once the record is in memory it is not necessarily reread.
   FOR_EACH_IN(record_node,BINARY_RECORD_INDEX_NODE,index,each_record_node)
   {  if (record_node->get_record_code() == UED_variable_definition_format_0)
      {  Variable_format_0 *var_def =
            dynamic_cast <Variable_format_0 *>(record_node->get(*this));
         nat32 var_def_code =  var_def->code_clad.get();
         if (var_def && vi < max_allocation)
         {  // Variable has already been listed, avoid duplication.              //141008
            bool previously_listed = false;                                      //141008
            for (nat16 listed_index = 0; listed_index < vi && !previously_listed;listed_index++)
               if (variable_codes[listed_index] == var_def_code)
                  previously_listed = true;
            if (!previously_listed)
               variable_codes[vi++] = var_def_code;
         }
      } else if (record_node->get_record_code() == UED_variable_definition_format_ontological)  //160620
      {  Variable_format_ontological *var_def =
            dynamic_cast <Variable_format_ontological *>(record_node->get(*this));
         nat32 var_def_code =  var_def->code_clad.get();
         if (var_def && vi < max_allocation)
         {  // Variable has already been listed, avoid duplication.              //141008
            bool previously_listed = false;                                      //141008
            for (nat16 listed_index = 0; listed_index < vi && !previously_listed;listed_index++)
               if (variable_codes[listed_index] == var_def_code)
                  previously_listed = true;
            if (!previously_listed)
               variable_codes[vi++] = var_def_code;
         }
      }
   }   FOR_EACH_END(each_record_node)
   variable_count = vi;
   for (;vi < max_allocation; vi++) // zero out the rest of the array.
         variable_codes[vi] = 0;

      // Fill the provided array with pointers to the current variable definitions.
      // The max_allocation is the maximum array size.
      // The variable definitions are provided (loaded in memory) as needed.
      // Returns the number of variable definitions filled into the array.
      // (The remaining points in the array are cleared to 0).

   return variable_count;
}
//_get_defined_variables_codes______________________________________2014-09-22_/
nat16 Database_file::get_defined_variables_codes
(CORN::Dynamic_array<nat32> &variable_codes)
{
   nat16 variable_count = 0;
   nat16 vi = 0;   //variable index
   goto_BOF();                                                                   //141027
   // Search for variable definition records that have been indexed
   // and load them in memory if not already done so.
   // Note that once the record is in memory it is not necessarily reread.
   FOR_EACH_IN(record_node,BINARY_RECORD_INDEX_NODE,index,each_record_node)
   {  if (record_node->get_record_code() == UED_variable_definition_format_0)
      {  Variable_format_0 *var_def =
            dynamic_cast <Variable_format_0 *>(record_node->get(*this));
         nat32 var_def_code =  var_def->code_clad.get();
         nat32 existing_index;
         if (!variable_codes.occurs(var_def_code, existing_index))
            variable_codes.append(var_def_code);
      } else if (record_node->get_record_code() == UED_variable_definition_format_ontological)  //160620
      {  Variable_format_ontological *var_def =
            dynamic_cast <Variable_format_ontological *>(record_node->get(*this));
         nat32 var_def_code =  var_def->code_clad.get();
         nat32 existing_index;
         if (!variable_codes.occurs(var_def_code, existing_index))
            variable_codes.append(var_def_code);
      }
   } FOR_EACH_END(each_record_node)
   return variable_codes.get_count();
}
//_get_defined_variables_codes______________________________________2019-01-02_/
const Period_record * Database_file::set_period(const Period_record &copy_from_period)
{  //This modifies or adds a period with the specified description and attributes.
   //Arguments for options that are not used may be 0
   //Existing periods with the same start/end dates,
   //options and options values and will be updated,
   //will get an updated period_description.
   //Returns the either the modified matching period record, or the new period record.

   //Scan the record list for period record
   //filter for matching options,
   if (!initialized) initialize();                                               //050125
   Period_record *period = 0;
   FIND_FOR_EACH_IN(found_node,record_node,BINARY_RECORD_INDEX_NODE,index,true,each_record_node)
   {  if (record_node->get_record_code() == UED_period)
      {  Period_record *period_record =  (Period_record *)record_node->get(*this);
         if (period_record)                                                      //081129
         {  if ((period_record->options == copy_from_period.options)
             && (period_record->initiation == copy_from_period.initiation)
             && (period_record->completion == copy_from_period.completion))
            {  found_node = record_node;
               period = period_record;
   }  }  }  } FOR_EACH_END(each_record_node)
   if (period)
   {  //set the record options
      period->text = copy_from_period.text;
      // options, and dates are not modified in this case. (they index this record)
      period->application_code = copy_from_period.application_code;
      period->enumeration = copy_from_period.enumeration;
      period->index = copy_from_period.index;
      period->variable_code = copy_from_period.variable_code;
   } else  // create a new record
   {  period = new Period_record(copy_from_period);
      append_record(period,false);
   }
   return period;
}
//_set_period_______________________________________________________2004-10-11_/
#ifdef NYI
Period_record *Database_file_indexed::get_period
(datetime64 date
,nat32  application_code = 0
,nat32  enumeration      = 0
,nat32  index            = 0)   // Could be const, since this only adjusts points
{
   //Searches the database for a period the date falls with in the period
   //Returns the period record.
   //Return 0 if no matching period is found.
   //If the period's date matches and the periods options
   //specify application code, enumeration, and/or index
   //these specified parameters will must also match.
   //Note, I might want to make these signed where -1 (default) indicates
   //ignore the respective option.  (Or I could pass a bit mask specifying the corresponding options flags to evaluate).

   // NYI
}
//_get_period_______________________________________________________2004-10-03_/
#endif
const Period_record *Database_file::get_period
(nat32      consider_options
,datetime64 at_date
,nat32      application_code_
,nat32      enumeration_
,nat32      index_
,Variable_code variable_code_)
{  // This returns the first period encountered that has a date time that matches the time query,
   //  options and values for selected options.
   // Returns 0 if no matching period.
   if (!initialized) initialize();                                               //050125
   Period_record *period = 0;
   FIND_FOR_EACH_IN(found_node,record_node,BINARY_RECORD_INDEX_NODE,index,true,each_record_node)
   {  if (record_node->get_record_code() == UED_period)
      {  Period_record *period_record =  (Period_record *)record_node->get(*this);
         {  // Now check if the dates match the query
            bool matchs = true;
            Variable_code period_variable_code =  period_record->variable_code;
            if ((  (consider_options & UED_period_start_date_time_option)        //141217
                 &&(consider_options & UED_period_end_date_time_option)  )       //141217
                && (   (at_date < period_record->initiation)                     //141217
                    || (at_date > period_record->completion)))                   //141217
                matchs = false;                                                  //141217
/*180525 This seems to be excluding all periods
            else                                                                 //141217
            {  if ((consider_options & UED_period_start_date_time_option)  && (at_date != period_record->start_date_time))
                  matchs = false;                                                //141217
               if ((consider_options & UED_period_end_date_time_option)    && (at_date != period_record->end_date_time))
                  matchs = false;                                                //141217
            }
*/
            if ((consider_options & UED_period_application_code_option) && (application_code_  != period_record->application_code))  matchs = false;
            if ((consider_options & UED_period_variable_code_option)    && (variable_code_     != period_variable_code))  matchs = false;
            if ((consider_options & UED_period_enumeration_option)      && (enumeration_       != period_record->enumeration))       matchs = false;
            if ((consider_options & UED_period_index_option)            && (index_             != period_record->index))             matchs = false;

//std::clog << (int)matchs << ' ';


            if (matchs)
            {  found_node = record_node;
               period = period_record;
   }  }  }  } FOR_EACH_END(each_record_node)
   return period;
}
//_get_period_______________________________________________________2004-10-11_/
nat32  Database_file::copy_periods_to_database(Database_file &target_database)
{  // This copies all the listed periods from this database to the specified target database.
   // Returns the number of periods copied.
   nat32  period_count = 0;
   if (!initialized) initialize();
   FOR_EACH_IN(record_node,BINARY_RECORD_INDEX_NODE,index,each_record_node)
   {  if (record_node->get_record_code() == UED_period)
      {  Period_record *period_record =  (Period_record *)record_node->get(*this);
         target_database.set_period(*period_record);
   }  } FOR_EACH_END(each_record_node)
   return period_count;
}
//_copy_periods_to_database_____________________________2005-01-25__2004-10-11_/
Geolocation_record  *Database_file::mod_geolocation_record()          stream_IO_
{  if (geolocation_rec_in_memory) return geolocation_rec_in_memory;              //070619
   goto_BOF();                                                                   //050523

//   This probably should be provide because if there is no
//   geolocation record in the file we will get calling this and
//   is may keep reseting the binary record files index_node so potential infinite loop.

   geolocation_rec_in_memory = (Geolocation_record *)
      goto_next_record_of_type(UED_geolocation_information);
   return geolocation_rec_in_memory;                                             //070619
}
//_mod_geolocation_record___________________________________________2005-03-07_/
const Geolocation_record  *Database_file::ref_geolocation_record()    stream_IO_
{ return (const Geolocation_record  *)mod_geolocation_record();
}
//_ref_geolocation_record___________________________________________2005-04-29_/
const Generating_application_record *Database_file::get_generating_application_record() stream_IO_
{  goto_BOF();
   return (Generating_application_record *)goto_next_record_of_type(UED_generating_application);
}
//_get_generating_application_record_________________________________2009-10-21/
Geolocation_record *Database_file::take_geolocation_record
(Geolocation_record *geolocation_record_given) modification_
{  // This replace/updates any existing location record, or appends the location record if not present.
   // The location record is relinquished to this UED
/* This is going into infinite loop if record already exists
   for (Geolocation_record *existing_geolocation_record=get_geolocation_record() //100519
       ;existing_geolocation_record  && geolocation_record_given                 //120326
       ;existing_geolocation_record = get_geolocation_record())
*/
   Geolocation_record *existing_geolocation_record=mod_geolocation_record();     //140608
   if (existing_geolocation_record)                                              //140608
   {
      if (existing_geolocation_record->compares_exactly_to_geolocation(*geolocation_record_given)) //120326
         {  // There is already a geolocation record in this file
            // (or at least in memory) that matches the one to take.
            // In this case we don't try to replace it.
            delete geolocation_record_given; geolocation_record_given = 0;       //120326
         }
      else
      {
         if (geolocation_record_given->is_more_complete_than                     //140608
            (*existing_geolocation_record))
         {
            existing_geolocation_record->free(get_free_code(existing_geolocation_record->get_code()));
            finished_with(existing_geolocation_record);
            if (geolocation_rec_in_memory == existing_geolocation_record)
                geolocation_rec_in_memory = 0;
         } else
         {  delete geolocation_record_given; geolocation_record_given = 0;       //120326
         }
      }
   }
   if (geolocation_record_given)                                                 //120326
   {  append_record(geolocation_record_given,false);
      geolocation_rec_in_memory = geolocation_record_given;                      //100519
   }
   return geolocation_rec_in_memory;                                             //120326
}
//_take_geolocation_record__________________________________________2005-05-23_/
const Geolocation_record *Database_file::know_geolocation
(const Geolocation &geolocation)                                   modification_
{  // This replace/updates any existing location record, or appends the location record if not present.
   Geolocation_record *geoloc_rec = new Geolocation_record(geolocation);
   return take_geolocation_record(geoloc_rec);
}
//_know_geolocation_________________________________________________2009-09-22_/
Geolocation_record  *Database_file::provide_geolocation_record()      stream_IO_
{  if (!geolocation_rec_in_memory)
   {  geolocation_rec_in_memory = mod_geolocation_record();
      if (!geolocation_rec_in_memory)
      {  geolocation_rec_in_memory = new Geolocation_record;
         append_record(geolocation_rec_in_memory,false);
      }
   }
   return geolocation_rec_in_memory;
}
//_provide_geolocation_record_______________________________________2014-06-06_/
nat32  Database_file::free_records_for_period
(CORN::Year start_year, CORN::Year end_year)
{  nat32  deleted_rec_count = 0;
   goto_BOF();
   while ( Binary_data_record_cowl *data_rec = goto_next_data_record())
   {  CORN::Date_clad_32 time_stamp_date(data_rec->get_time_stamp());
      CORN::Date_clad_32 timeseries_final_date(data_rec->get_final_time_series_date());
      CORN::Year time_stamp_year       = time_stamp_date.get_year();
      CORN::Year timeseries_final_year = timeseries_final_date.get_year();
      if ( (time_stamp_year       >= start_year)
         &&(time_stamp_year       <= end_year)
         &&(timeseries_final_year >= start_year)
         &&(timeseries_final_year <= end_year))
      {  data_rec->rec_code = get_free_code(data_rec->get_code());
         data_rec->set_modified();
         deleted_rec_count++;
   }  }
   return deleted_rec_count;
}
//_free_records_for_period__________________________________________2010-01-05_/
nat32  Database_file::free_records_for_period_variable
(CORN::Year start_year, CORN::Year end_year,Variable_code variable_code)
{  nat32  deleted_rec_count = 0;
   goto_BOF();
   while ( Binary_data_record_cowl *data_rec = goto_next_data_record())
   {  CORN::Date_clad_32 time_stamp_date(data_rec->get_time_stamp());
      CORN::Date_clad_32 timeseries_final_date(data_rec->get_final_time_series_date());
      CORN::Year time_stamp_year       = time_stamp_date.get_year();
      CORN::Year timeseries_final_year = timeseries_final_date.get_year();
      if ( (time_stamp_year       >= start_year)
         &&(time_stamp_year       <= end_year)
         &&(timeseries_final_year >= start_year)
         &&(timeseries_final_year <= end_year))
      {  if (data_rec->get_variable_code() == variable_code)
         {  data_rec->rec_code = get_free_code(data_rec->get_code());
            data_rec->set_modified();
            deleted_rec_count++;
   }  }  }
   return deleted_rec_count;
}
//_free_records_for_period_variable_________________________________2011-06-01_/
bool Database_file::done_with_dates_before(const CORN::Date_const &earliest_date)
{
   FOR_EACH_IN(header,BINARY_RECORD_INDEX_NODE,index,each_header)
   {  Record_code_clad rec_code(header->get_record_code());
      if (rec_code.has_data()) // Currently all data records are time stamped
      {  // Read the variable and date info from the record
         Data_record_header *data_rec_header=dynamic_cast<Data_record_header *>
            (header->get(*this));
         if (data_rec_header)
         {  if (data_rec_header->get_time_stamp() < earliest_date.get_datetime64())
            {  finished_with(data_rec_header); // This will remove the record body from memory

         //141103  warning deleting the record will be problematic
         // because it is still in the index
         // this is just a quick and dirty test
         // should clean up the index

               each_header->delete_current();
            }
         }
      }
   } FOR_EACH_END(each_header)
   return true;
}
//_done_with_dates_before___________________________________________2014-11-03_/
}//_namespace UED______________________________________________________________/

