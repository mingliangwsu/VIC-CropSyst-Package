#include "UED/convert/text_dBase_Excel_with_TDF/export_column.h"
#include "UED/convert/text_dBase_Excel_with_TDF/tuple_index.h"
#include <fstream>

/*181106 abandoned  This is special for CropSyst versions still using variable codes
#define CSVC_crop_base_name
*/
#define GROWING_SEASON_PERIOD 1
// this would match CropSyst_period defined in land_unit_output.h

namespace UED
{
//______________________________________________________________________________
bool Export_column::set_export_tupple
(nat16 column_index                                                              //020525
,float32 target_value
,const Date_time_clad_64 &index_date_time
,Tuple_index &tuples)                                                            //000717
{  // If this column is the start of a data set then we spread
   //  the data values across the record starting from this column:
    current_export_tuple = (Export_tuple *)tuples.find_matching(index_date_time);//000717
    if (current_export_tuple)
       current_export_tuple->data.set(column_index,target_value);
    return true;
 }
//_set_export_tupple________________________________________________1999-05-06_/
bool Export_column::is_date_in_season
(CORN::datetime64             target_date_time
,modifiable_ Database_file_indexed &database)                       affirmation_
// modifiable because get_period is not const
{
   nat32 consider_options =
          UED_period_start_date_option      |UED_period_end_date_option
         |UED_period_application_code_option; //|UED_period_index_option;
   const Period_record * season = database.get_period
            (consider_options
            ,target_date_time
            ,GROWING_SEASON_PERIOD);
            // enumeration index and variable code not relevent
   bool in_season = season != 0;
   return in_season;
}
//_is_date_in_season________________________________________________2018-11-06_/
bool Export_column::tuplize_data                                                 //990506
(nat16                        column_index
,Binary_data_record_cowl     &data_record                                        //151125
,Tuple_index                 &tuples                                             //000717
,streamable_ Database_file_indexed &database
,const CORN::Units_clad      &smart_preferred_units
,CORN::datetime64             filter_date_time_range_low                         //070621
,CORN::datetime64             filter_date_time_range_high
,bool                         seasonal                                           //181106
)
{  if (data_record.is_data_scalar())                                             //040930
   {  CORN::datetime64 target_date_time = data_record.get_date_time_for_index(0);//040930
      encountered = true;                                                        //040930
      bool date_pertinent                                                        //181106
           =  (target_date_time >= filter_date_time_range_low)
           && (target_date_time<=filter_date_time_range_high);
      date_pertinent &= (!seasonal || is_date_in_season(target_date_time,database)); //181106
      if (date_pertinent)                                                        //181106_070621
      {  CORN::Quality_clad attribute (UED_give_me_best_quality);                //040930
         // Convert value to prefered units:
         float32 from_value =  data_record.get_value(attribute,target_date_time);
         float32 target_value = 0.0;
         database.convert(from_value,data_record.get_units_code(),target_value,smart_preferred_units.get());
         set_export_tupple                                                       //040930
          (column_index //column_number //                                          //040930
          ,target_value                                                          //040930
          ,target_date_time                                                      //040930
          ,tuples);                                                              //040930
      }
   } else
   {  Dynamic_array<float32> &data_values = data_record.ref_data_values();       //151125_040930
      for (nat32 i = 0;i < data_values.get_count(); i++) // for each value in the data record array
      {  //  Compute the index date time:
         CORN::Date_time_clad_64 index_date_time(data_record.get_date_time_for_index(i));   //070221
         CORN::datetime64 index_datetime64 = index_date_time.get_datetime64();   //070221
         bool date_pertinent =                                                   //181106
            (index_datetime64 >= filter_date_time_range_low)
          &&(index_datetime64 <= filter_date_time_range_high);
         date_pertinent &= (!seasonal || is_date_in_season(index_datetime64,database));                                      //181106
         if (date_pertinent)                                                     //181106_070221
         {  // Convert value to prefered units:                                  //070221
            float32 from_value = data_values.get(i);
            float32 target_value = 0.0;
            database.convert(from_value,data_record.get_units_code(),target_value,smart_preferred_units.get());
            set_export_tupple
            (column_index // column_number //
            ,target_value
            ,index_date_time
            ,tuples);
   }  }  }
   return true;
}
//_tuplize_data_____________________________________________________1999-05-06_/
bool Export_column::tuplize_data_set
(nat16                              column_index                                 //020526
,Binary_data_record_cowl           &data_record                                  //151125
,Tuple_index                       &tuples                                       //000717
,streamable_ Database_file_indexed &database // Just for convert function
,const CORN::Units_clad            &smart_preferred_units
,CORN::datetime64                   date_time_range_low                          //070621
,CORN::datetime64                   date_time_range_high                         //070621
,bool                               seasonal)                                    //181106
{  bool result = true;
   encountered = true;                                                           //040930
       Dynamic_array<float32> &data_values = data_record.ref_data_values();      //151125
       if (statistic_labeled.get())                                              //990518
       { float32 stat_value = 0.0;
          bool convert = true;
          switch (statistic_labeled.get())
          {  case sum_statistic : // STAT_sum :
             { stat_value = data_values.sum();
               convert = true;
             } break;
             case  count_statistic: // STAT_count :
             {  stat_value = data_values.get_count();
                convert = false;
             } break;
/* NYI
            case mean_statistic :   // STAT_mean :
            {
               stat_value = data_set.get_mean();
               convert = true;
            } break;
            case std_dev_statistic : // STAT_std_dev :
            {
               stat_value = data_set.get_standard_deviation();
               convert = true;
            }
            case coef_var_statistic : // STAT_coef_var :
            {
               stat_value = data_set.get_coefficient_of_variation();
               convert = false;
            }
*/
          }
          float32 target_value = 0.0;
          if (convert)
             database.convert(stat_value,data_record.get_units_code(),target_value,smart_preferred_units.get());
          else
             target_value = stat_value;
          CORN::Date_time_clad_64 index_date_time(data_record.get_date_time_for_index(0));
          set_export_tupple
          (column_index
          ,target_value
          ,index_date_time
          ,tuples
          );
       } else
       {
          result = tuplize_data(column_index,data_record,tuples,database,smart_preferred_units/*,true*/
            ,date_time_range_low,date_time_range_high                            //070621
            ,seasonal);                                                          //181106
      }
    return result;
}
//_tuplize_data_set_________________________________________________1999-05-06_/
bool Export_column::tuplize
(nat16                              column_index                                 //040929
,Binary_data_record_cowl           &data_record                                  //151125
,Tuple_index                       &tuples                                       //000717
,streamable_ Database_file_indexed &database
,CORN::datetime64                   date_time_range_low                          //070621
,CORN::datetime64                   date_time_range_high                         //070621
,bool                               seasonal)                                    //181106
{   bool processed = false;
    nat32 dr_vc = data_record.get_variable_code();

    nat32 this_vc = TFFC_layout.variable_code_clad.get();   // 050830 Warning not sure if TFFC_layout or TC_layout
//std::cout << "tuplize:" << (nat32)data_record.get_time_stamp() << '\r';
   bool data_rec_VC_matches_layout_VC = dr_vc == this_vc;                        //160516
   bool data_rec_type_matches_layout_rec_type
      = data_record.get_code() == TFFC_layout.record_type_clad.get_code();      //160516
   bool match_any_rec_type = TFFC_layout.record_type_clad.get_code() == 0;      //160516
      // <- TDF format might not specify the UED_record type code (it isn't really needed for export) //070307
   if (data_rec_VC_matches_layout_VC
        &&(data_rec_type_matches_layout_rec_type || match_any_rec_type))
    {  // We can handle this record.
       current_export_tuple = 0;
      UED::Record_code_clad dr_record_code_clad(data_record.get_code());        //070307
      if (dr_record_code_clad.is_data_set())
          tuplize_data_set(column_index, data_record,tuples,database
            ,TFFC_layout.get_units_code()                                        //100117
            ,date_time_range_low,date_time_range_high,seasonal);                 //181106
      else // single value for single column.
          tuplize_data(column_index,data_record,tuples,database
            ,TFFC_layout.get_units_code()                                        //100117
            ,date_time_range_low,date_time_range_high,seasonal);                 //181106
       processed = true;
    }
    return processed;
}
//_tuplize__________________________________________________________1999-05-06_/
}//_namespace UED______________________________________________________________/

