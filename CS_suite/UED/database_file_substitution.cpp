#include "CS_suite/UED/database_file_substitution.h"
#  include "UED/library/timeqry.h"
// This extends UED::Database_file_indexed to allow date substitution
#include "corn/chronometry/date_32.h"

namespace CS_UED
{
//______________________________________________________________________________
Database_file_indexed::Database_file_indexed
(const char *ued_filename
,std::ios_base::openmode openmode_
,bool auto_float16_)
: UED::Database_file_indexed(ued_filename,openmode_,auto_float16_)
, date_substituters(0)
, substitute_base_year     (0)
, substitute_reference_year(0)
, substitute_year_offset   (0)
, preclude_latest_date_time(0)
{}
//_Database_file_indexed_constructor________________________________2017-03-11_/
Database_file_indexed::~Database_file_indexed()
{  delete date_substituters;
}
//_Database_file_indexed_destructor_________________________________2017-03-23_/
datetime64 Database_file_indexed::get_latest_date_time
( Record_code  for_record_type_code
, UED::Variable_code for_var_code )                                         const
{
   datetime64 latest_date_time = UED::Database_file_indexed::get_latest_date_time
      (for_record_type_code,for_var_code);
   if (preclude_latest_date_time)
      latest_date_time = std::min<datetime64>
         (latest_date_time,preclude_latest_date_time);
   return latest_date_time;
}
//_get_latest_date_time_____________________________________________2017-03-11_/
bool Database_file_indexed::enable_substitution
(CORN::Year substitute_base_year_
,CORN::Year substitute_reference_year_                                           //170331
,int8       substitute_year_offset_)                               contribution_
{  substitute_base_year      = substitute_base_year_;
   substitute_reference_year = substitute_reference_year_;                       //170331
   substitute_year_offset    = substitute_year_offset_;
   if (!date_substituters)
        date_substituters = new CORN::Enumeration_list;
   return date_substituters != 0;
}
//_enable_substitution______________________________________________2017-03-26_/
bool Database_file_indexed::enable_preclusion
(datetime64 preclude_latest_date_time_)                            contribution_
{  bool enabled = true;
   preclude_latest_date_time = preclude_latest_date_time_;
   if (!substitute_base_year)
   {
      // If the substitute year base has not been specified
      // use the earliest year in the file.
      CORN::Date_clad_32 earliest_date(get_earliest_date_time());
      enabled &= enable_substitution
         (earliest_date.get_year()
         ,substitute_reference_year                                              //170331
         ,substitute_year_offset);
   }
   return enabled;
}
//_enable_preclusion________________________________________________2017-03-26_/
Date_substituter::Date_substituter
(UED::Variable_code variable_code_
,CORN::Year base_year_                                                           //170323
,CORN::Year reference_year_
,CORN::Year latest_year_
,int8              year_offset_)                                                 //161207
: variable_code   (variable_code_)
, base_year       (base_year_)                                                   //170323
, reference_year  (reference_year_)                                              //170331
, latest_year     (latest_year_)                                                 //170311
, year_offset     (year_offset_)                                                 //161207
, years_range     (latest_year_ - base_year_)
{}
//_Date_substituter_constructor_____________________________________2016-03-31_/
Time_query *Date_substituter::substitute
(const Time_query &time_query)                                        rendition_
{
   CORN::Year query_year    = time_query.get_year();
   CORN::Year substitute_year = query_year;
   while (substitute_year < base_year)
      substitute_year += years_range + year_offset;                              //161207
      // year_offset was added for FlexCropping project                          //161207
   if (substitute_year >= latest_year)                                           //170311
      substitute_year = base_year + year_offset                                  //170312
         + (time_query.get_year() - reference_year);                             //170331
   Time_query *subst_time_query = time_query.clone();
   subst_time_query->offset_year(substitute_year - query_year);
   while (subst_time_query->get_DOY() > subst_time_query->days_in_this_year())
      subst_time_query->dec_day();                                               //170525
      // if source year is a leap year the subsituted year might not be a leap year
      // The use of while loop will accomodate alternate calendar systems.

//std::cout << "Bryan verify substitute:" << time_query.as_string() << "->" << subst_time_query->as_string() << std::endl;

   return subst_time_query;
}
//_substitute_______________________________________________________2016-03-31_/
Time_query *Database_file_indexed::substitute_time_query
(Record_code       rec_type
,Variable_code     variable_code
,const Time_query &time_query)                                      //provision_
{  Time_query *replacement_time_query = 0;
   if (!date_substituters)                                                       //160525
      return 0;
      //There might not be substituters if not such variable exists in the file.
   Date_substituter *substituter
      = dynamic_cast<Date_substituter*>(date_substituters->find(variable_code));
   if (!substituter)
   {  // provide substituter
      if (!substitute_base_year)
      {  // The user did not specify the base year, use earliest year in file.
         CORN::Date_clad_32 earliest_date(get_earliest_date_time(rec_type,variable_code));
         substitute_base_year = earliest_date.get_year();

         if (earliest_date.get_DOY() != 1)                                       //170331
         {  substitute_base_year += 1;
         // 170331 warning, this is assuming that if the earliest year is not complete
         // the next year will be, but that is not necessarily the case
         // (There might not even be the next year
         // I should find the next earliest complete year!!!!
         }
      }
      CORN::Date_clad_32 latest_date(get_latest_date_time(rec_type,variable_code));
      CORN::Year latest_year   = latest_date.get_year();
      if (!((substitute_base_year == 9999) || (latest_year == -9999)))
      {  substituter = new Date_substituter
            (variable_code
            ,substitute_base_year                                                //170323
            ,substitute_reference_year                                           //170331
            ,latest_year
            ,substitute_year_offset);
         date_substituters->append(substituter);
      } // else there are no such dated records so cant instanciated substituter
   } // substituter was found so we can subsitute
   if (substituter) // found or created
      replacement_time_query = substituter->substitute(time_query);
   return replacement_time_query;
}
//_substitute_time_query____________________________________________2016-03-31_/
Binary_data_record_cowl *Database_file_indexed::locate_data_record
(Record_code          rec_type
,UED::Variable_code   var_code
,const Time_query    &time_query)                                     stream_IO_
{  // Since the current data record is not what we are working with,
   // try to activate the data record.
   return
      (preclude_latest_date_time &&
       (time_query.get_target_date_time() >= preclude_latest_date_time))
   ? 0
   : UED::Database_file_indexed::locate_data_record(rec_type,var_code,time_query);
}
//_locate_data_record_______________________________________________2017-03-11_/
float32 Database_file_indexed::get_no_create
(Record_code          rec_type
   // We always must specify the record type code because we may
   // have different types of records with the same variable
,UED::Variable_code   variable_code
,const Time_query    &time_query
,UED_units_code      &requested_units
,CORN::Quality       &attribute)                                      stream_IO_
{  float32 result = 0;
   attribute.assume_code(CORN::missing_quality);
   const Time_query *adj_time_query = &time_query;
   const Time_query *subst_time_query = 0;
   Binary_data_record_cowl *found_record
      = locate_data_record(rec_type ,variable_code,time_query);
   if (!found_record)
      // This is a special mode to provided data during periods that don't actually exist in the file
      subst_time_query = adj_time_query
         = substitute_time_query(rec_type,variable_code,time_query);
   result
      = adj_time_query
      ? UED::Database_file_indexed::get_no_create
         (rec_type,variable_code,*adj_time_query,requested_units,attribute)
      : 0;
   if (subst_time_query)
      attribute.assume_code(quality_estimated_from_quality(attribute));
   delete subst_time_query;
   return result;
}
//_get_no_create____________________________________________________2017-03-11_/
bool Database_file_indexed::get_parameter_on
(modifiable_ CS::Parameter &parameter
,Variable_code     variable_code
,const Time_query &time_query
,Record_code       rec_type)                                          stream_IO_
{
   const Time_query *adj_time_query = &time_query;
   const Time_query *subst_time_query = 0;
   Binary_data_record_cowl *found_record
      = locate_data_record(rec_type ,variable_code,time_query);
   if (!found_record)
      // need substituting the target date
      subst_time_query = adj_time_query
         = substitute_time_query(rec_type,variable_code,time_query);
   bool result
      = adj_time_query
      ? UED::Database_file_indexed::get_parameter_on
         (parameter,variable_code,*adj_time_query,rec_type)
      : false;
   if (subst_time_query)
      parameter.assume_code(quality_estimated_from_quality(parameter));
   delete subst_time_query;
   return result;
}
//_get_parameter_on_________________________________________________2017-03-11_/
}//_namespace CS_UED___________________________________________________________/

