#include "CS_suite/data/receiver_TDF.h"
#include "corn/measure/unitconv.h"
#include "corn/spreadsheet/workbook.h"

namespace CS
{
//______________________________________________________________________________
Receiver_TDF::Receiver_TDF
(
 //200616 const ED_tabular_file_format &format_
 const CORN::Table_definition &format_                                           //200618
,CORN::Workbook               &workbook_
,const std::string            &sheet_name_                                       //200706
,nat32                         collated_line_offset_)
: format                                                               (format_)
, workbook                                                           (workbook_)
, detail_timestamps                            (0,366,detail_date_default_value)
, collated_line_offset                                   (collated_line_offset_)
, current_sheet                            (workbook.provide_sheet(sheet_name_))
  //200706 (workbook.provide_sheet(""))
{}
//______________________________________________________________________________
nat32 Receiver_TDF::set
(float32          value
,nat16            offset  // I think this was intended for spread columns
,CORN::Units_code units
,CORN::datetime64 timestamp
,const std::string &ontology
,nat32            variable_code_emitted)
{  CORN::Units_convertor convertor;
   nat32 timestamp_occurs_at = 0;
   bool first_occurance_of_date = false;
   if (!detail_timestamps.occurs(timestamp,timestamp_occurs_at,0.00000001))
   {  timestamp_occurs_at = detail_timestamps.append_where(timestamp);
      first_occurance_of_date = true;
   }
   nat16 column_count = format.columns.count();
   nat16 field_number = 0;
   FOR_EACH_IN(field,CORN::Table_definition::Column,format.columns,field_iter)
   {
      /*200616 column format definition no long has column numbers
      They are now always positionally intrisically indexed

      // obsolete: Ideally field number will be intrinsic to the column number (index+1)
      // In some legacy fields the user has specified the column number
      // so we handle either case.
      if (field->number)
            field_number = field->number;
      else
      */
            field_number++;
      // Although generally we could stop
      // We do go through all the columns because the format may be setup
      // to output the same value in different units or format(date)
      // or multiple sheets
      const std::string &field_ontology_string = field->get_ontology_string();
      nat32 variable_code_field = field->get_variable_code();                    //200618
      bool field_is_record_date = field->temporal;
      bool ontology_matches= field_ontology_string.length() && (field_ontology_string == ontology);
      bool varcode_matches =
         #if ((CS_VERSION > 0) && (CS_VERSION < 6))
         (variable_code_field == variable_code_emitted);
         #else
         false;
         #endif
      if (ontology_matches
          || varcode_matches
          || field_is_record_date)                                               //200618
          //200618 ||(field->record_date))
      {  // conceptual: the user may have specifically limited the number of columns to spread
         // if offset && spread columns and spread offset+1 > spread columns
         // return
         bool emit_field = true;
         const CORN::Data_type field_data_type = field->data_type_clad.get();
         const CORN::Date_format *date_format = 0;
         const CORN::Time_format *time_format = 0;
         CORN::datetime64 value_as_datetime = 0;
         switch (field_data_type)
         {  case CORN::DT_date:
            {  date_format = field->provide_format_date();
               if (field->temporal)
               {  value_as_datetime = timestamp;
                  if (first_occurance_of_date)
                     value_as_datetime = timestamp;
                     // this was the first opportunity to write the record date
                  else emit_field = false; //  we have already set the date
               } else
               { // it is a normal column date
                  value_as_datetime = value;
               }
            } break;
            case CORN::DT_time :
            {  assert(false); // NYI
               time_format = field->provide_format_time();
               value_as_datetime = value;
            } break;
            case CORN::DT_datetime :
            {  date_format = field->provide_format_date();
               time_format = field->provide_format_time();
               value_as_datetime = value;
            } break;
            // default is normal numeric value
         } // switch data type
         nat16 sheet_col = field_number - 1;
         if (!field->sheet.name.empty())                                         //200616
            current_sheet = workbook.provide_sheet(field->sheet.name);           //200616
         if (emit_field)
         {  nat16 sheet_row = collated_line_offset + timestamp_occurs_at
               + format.detail.extent.begin_offset; // - 1;
               //200616 + format.detail_lines_delineation.start_row_number - 1;
            if (value_as_datetime != 0)
            {
               std::string date_formatted;
               CORN::Date_time_cowl_64 datetime(value_as_datetime);
               datetime.append_to_string(date_formatted,date_format,time_format);
               if (date_formatted.length())
                   current_sheet->set_cell_label(sheet_col,sheet_row,date_formatted);
            }
            else
            {  CORN::Units_code field_units = field->units.get();
               float32 value_preferred = 999999;
               convertor.convert(value,units,value_preferred,field_units);
               current_sheet->set_cell_number(sheet_col+offset,sheet_row,value_preferred,field->get_precision());
            /* Not currently flushing here because we don't have the level column yet anyway
            sheet->flush(sheet_row,column_count);
            */
            }
         }
      }
      // sheet_col++;
   } FOR_EACH_END(field_iter)
   nat32 dates_encountered = 0;
   nat32 timestamp_count = detail_timestamps.get_count();
   for (nat32 d = 0; d < timestamp_count; d++)
   {  float64 date_d = detail_timestamps.get(d);
      dates_encountered += (date_d > 0.0000000001);
   }
   return dates_encountered; //detail_dates.get_count(); //timestamp_index;
}
//_set_____________________________________________________________2019-01-04_/
nat32 Receiver_TDF::take
(float32          value
,CORN::Units_code units
,CORN::datetime64 timestamp
,const std::string &ontology
,nat32            variable_code_emitted)
{
   return set(value,0, units, timestamp, ontology, variable_code_emitted);
/*
   CORN::Units_convertor convertor;
   nat32 timestamp_occurs_at = 0;
   bool first_occurance_of_date = false;
   if (!detail_timestamps.occurs(timestamp,timestamp_occurs_at,0.00000001))
   {
      timestamp_occurs_at = detail_timestamps.append_where(timestamp);
      first_occurance_of_date = true;
   }
   nat16 column_count = format.columns_1based.count();
   FOR_EACH_IN(field,ED_tabular_file_format::Column,format.columns_1based,field_iter)
   {
      // Although generally we could stop
      // We do go through all the columns because the format may be setup
      // to output the same value in different units or format(date)
      // or multiple sheets

      const std::string &field_ontology_string = field->get_ontology_string();
      nat32 variable_code_field = field->variable_code_clad.get();
      if (  (variable_code_field  == variable_code_emitted)
          ||(field_ontology_string.length() && (field_ontology_string == ontology))
          ||(field->record_date)
          )
      {
         bool emit_field = true;
         const Data_type field_data_type = field->data_type_clad
         .get();
         const CORN::Date_format *date_format = 0;
         const CORN::Time_format *time_format = 0;
         CORN::datetime64 value_as_datetime = 0;

         switch (field_data_type)
         {  case CORN::DT_date:
            {  date_format = field->provide_format_date();
               if (field->record_date)
               {
                  //190113 value_as_datetime = detail_dates.get(timestamp_index);
                  value_as_datetime = timestamp;
                  //190103 if (value_as_datetime == detail_date_default_value)
                  if (first_occurance_of_date)
                  {  // this was the first opportunity to write the record date
                     //190110 format_date(timestamp,field_number,date_formated);
                     //190913 detail_dates.set(timestamp_index,timestamp);
                     value_as_datetime = timestamp;
                  } // else we have already set the date
                  else emit_field = false;
               }
               else
               { // it is a normal column date
                  value_as_datetime = value;
               }
            } break;
            case CORN::DT_time :
            {  assert(false); // NYI
               time_format = field->provide_format_time();
               value_as_datetime = value;
            } break;
            case CORN::DT_datetime :
            {
               date_format = field->provide_format_date();
               time_format = field->provide_format_time();
               value_as_datetime = value;
            } break;
            // default is normal numeric value
         } // switch data type

         nat16 sheet_col = field->start_number - 1;
         Workbook::Sheet *sheet = workbook.provide_sheet(field->sheet);
         // taken = true;
         if (emit_field)
         {
            nat16 sheet_row = collated_line_offset + timestamp_occurs_at
               + format.detail_lines_delineation.start_row_number - 1;
         if (value_as_datetime != 0)
         {
            std::string date_formatted;
            CORN::Date_time_cowl_64 datetime(value_as_datetime);
            datetime.append_to_string(date_formatted,date_format,time_format);
            if (date_formatted.length())
                sheet->set_cell_label(sheet_col,sheet_row,date_formatted);
         }
         else
         {  CORN::Units_code field_units = field->units.get();
            float32 value_preferred;
            convertor.convert(value,units,value_preferred,field_units);
            sheet->set_cell_number(sheet_col,sheet_row,value_preferred,field->get_precision());
            //Not currently flushing here because we don't have the level column yet anyway
            // sheet->flush(sheet_row,column_count);

         }
         }
      }
      // sheet_col++;
      //190111 abandoned field_number ++;
   } FOR_EACH_END(field_iter)

   nat32 dates_encountered = 0;
   nat32 timestamp_count = detail_timestamps.get_count();
   for (nat32 d = 0; d < timestamp_count; d++)
   {
      float64 date_d = detail_timestamps.get(d);
      dates_encountered += (date_d > 0.0000000001);
   }
   return dates_encountered; //detail_dates.get_count(); //timestamp_index;
*/
}
//_take_____________________________________________________________2019-01-04_/
nat32 Receiver_TDF::take_vector
(CORN::Dynamic_array<float32> vector_values
,CORN::Units_code units
,CORN::datetime64 timestamp
,const std::string &ontology
,nat32            variable_code_emitted)
{  nat32 dates_encountered = 0;
   for (nat16 vv_i = 0; vv_i < vector_values.get_count(); vv_i++)
      dates_encountered = set(vector_values.get(vv_i),vv_i, units, timestamp, ontology, variable_code_emitted);
   return dates_encountered;
}
//_take_vector______________________________________________________2019-01-04_/
bool Receiver_TDF::know_ID(const std::string &ID)
{  current_sheet = workbook.provide_sheet(ID);
   return current_sheet != 0;
}
//_know_ID__________________________________________________________2019-04-10_/
const char *Receiver_TDF::get_ID(std::string &ID)
{
   if (current_sheet)
      current_sheet->get_name(ID);
   return ID.c_str();
}
//_get_ID___________________________________________________________2019-09-16_/
bool Receiver_TDF::close(const std::string &ID)
{  detail_timestamps.clear();
   collated_line_offset = 0;
   return workbook.close_sheet(ID);
}
//_close_current____________________________________________________2019-04-22_/
}//_namespace_CS_______________________________________________________________/

