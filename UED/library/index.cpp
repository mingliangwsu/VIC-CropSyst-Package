#  include "UED/library/index.h"
#  ifndef UED_H
#     include "UED/library/database_file.h"
#  endif
#  include "UED/library/varcodes.h"
#  ifndef UED_TIMEQUERY_H
#     include "UED/library/timeqry.h"
#  endif

#define DBG _COMMENT_
namespace UED {
//______________________________________________________________________________
bool Time_stamped_record_item::write(std::ostream &strm)            performs_IO_
{   // Used primarily for debugging
    strm << (nat32)earliest_date_time << " ";
    strm << (nat32)index_node->get_position();
    return true;                                                                 //150729
}
//_write_______________________________________________________________________/
Binary_data_record_cowl *Time_stamped_record_item::get_record(Binary_record_file_dynamic &binary_record_file)
{ return (Binary_data_record_cowl *)(index_node? index_node->get(binary_record_file) : 0); }
//_get_record_______________________________________________________2005-05-22_/
void Variable_index_list::finish()
{
   //   When finishing the database we need to make sure that
   // latest index node pointers are cleared
   FOR_EACH(item_to_finish,Variable_index_item,each_item)                        //030709
   {    item_to_finish->latest_index_node = 0;
   }FOR_EACH_END(each_item)
}
//_finish__finish___________________________________________________2000-10-06_/
bool Variable_index_list::record_and_variable_code_matches
( Record_code      &record_type_code
, Variable_code    &var_code
, Record_code      &for_record_type_code
, Variable_code    &for_var_code )                                  affirmation_
{
   bool rec_and_var_type_matches = false;
   Record_code min_rec_type_code = (for_record_type_code == UED_FOR_ALL_RECORD_TYPES) ? (Record_code)0       : for_record_type_code;
   Record_code max_rec_type_code = (for_record_type_code == UED_FOR_ALL_RECORD_TYPES) ? (Record_code)0xFFFF  : for_record_type_code;
   UED::Variable_code min_var_code= (for_var_code         == UED_FOR_ALL_VARIABLES   ) ? (UED::Variable_code)0          : for_var_code;  //000612
   UED::Variable_code max_var_code= (for_var_code         == UED_FOR_ALL_VARIABLES   ) ? (UED::Variable_code)0xFFFFFFFF : for_var_code;  //000612
   nat32 indx_var_physical_prop = Variable_code_clad(var_code).get_physical_property(); //990820
   bool variable_code_matches =                                                  //141031
      ((indx_var_physical_prop >= min_var_code) &&                               //990820
               (indx_var_physical_prop    <= max_var_code))                      //990820
             || (for_var_code == var_code);                                      //000701
   if (variable_code_matches)                                                    //141031
   {
      bool rec_type_matches =
         (record_type_code >= min_rec_type_code) &&
         (record_type_code <= max_rec_type_code);
      if (!rec_type_matches)
      {  // Maybe it is a float16 record type
         Record_code min_rec_type_code_float16 = min_rec_type_code | data_set_float16_subcode;
         Record_code max_rec_type_code_float16 = max_rec_type_code | data_set_float16_subcode;
         rec_type_matches =
            (record_type_code >= min_rec_type_code_float16) &&
            (record_type_code <= max_rec_type_code_float16);
      }
      rec_and_var_type_matches = rec_type_matches;
   }
   return rec_and_var_type_matches;
}
//_record_and_variable_code_matches_________________________________2014-10-31_/
Variable_index_item *Variable_index_list::find_index
(Record_code         record_type_code
,UED::Variable_code   variable_code)                                modification_
{  FIND_FOR_EACH(found_item,find_item,Variable_index_item,true,find_each)        //030709
   {  if (record_and_variable_code_matches                                       //141101
            ( find_item->record_type_code , find_item->variable_code
            , record_type_code            , variable_code ))
         found_item = find_item;
   } FOR_EACH_END(find_each)                                                     //000518
   return found_item;                                                            //000518
}
//_find_index_______________________________________________________1997-10-03_/
Time_stamped_record_item *Time_stamped_record_list::find_index
 (const Time_query &time_query) modification_                                    //990118
{  FIND_FOR_EACH(found_item,find_item,Time_stamped_record_item,true,find_each)   //030709
   {
      CORN::Date_time_clad_64 query_target_date_time(time_query.get_target_date_time());
      CORN::Date_time_clad_64 rec_earliest_date_date_time(find_item->earliest_date_time);
      CORN::Date_time_clad_64 rec_latest_date_date_time(find_item->latest_date_time);
      bool date_in_range = ((query_target_date_time.get_datetime64() >= rec_earliest_date_date_time.get_datetime64()) // This comparison need identified in Fast Graph
                     &&(query_target_date_time.get_datetime64() <= rec_latest_date_date_time.get_datetime64()));
      UED_units_code time_query_time_step = time_query.get_time_step();          //040125
      UED_units_code find_item_time_step =  find_item->get_time_step();          //040922
      bool timestep_matches =                                                    //040125
               (time_query_time_step == UED_QUERY_ANY_TIME_STEP) ||  // If we don't care what the time step is within the record 050510
               (time_query_time_step == find_item_time_step);
        if (date_in_range && timestep_matches)                                   //040125
          found_item =  find_item;
   } FOR_EACH_END(find_each)                                                     //030709
   return found_item;
}
//_find_index_______________________________________________________1997-10-03_/
Time_stamped_record_item *Variable_index_item::find_record_index
(const Time_query &time_query)                                                   //990118
{
   // It is possible when creating new records that
   // we already have a data record buffer but no index for it
   // (Indexes might not be generated until the record is actually written
   //  because we don't have a file position yet.
   //  In this case we return 0 for the TS record item,
   //  but return the record buffer for this index.
   Time_stamped_record_item *record_index = find_index(time_query);              //990118
   return record_index;
}
//_find_record_index________________________________________________1999-01-11_/
Time_stamped_record_item *Time_stamped_record_list::get_earliest_record_pos_info
(datetime64 &earliest_date_time)
{  // This method returns the lowest date record in the index.
   Time_stamped_record_item *earliest_item = 0;
   FOR_EACH(item,Time_stamped_record_item,find_each)                             //030709
   {  datetime64 item_earliest_date_time  = item->get_earliest_date_time();
      datetime64 seek_earliest_date_time  = earliest_date_time;
      if (item_earliest_date_time < seek_earliest_date_time)                     //021212
        {  earliest_date_time = item->get_earliest_date_time();
           earliest_item = item;
        }
     } FOR_EACH_END(find_each) 
   return earliest_item;
}
//_get_earliest_record_pos_info_____________________________________1999-09-09_/
Time_stamped_record_item *Time_stamped_record_list::get_latest_record_pos_info
(datetime64 &latest_date_time)
{  // This method returns the lowest date record in the index.
   Time_stamped_record_item *latest_item = 0;
   FOR_EACH(item,Time_stamped_record_item,find_each)                             //030709
   {  if (item->get_latest_date_time() > latest_date_time)
        {  latest_date_time = item->get_latest_date_time();
           latest_item = item;
        }
   } FOR_EACH_END(find_each)                                                     //030709
   return latest_item;
}
//_get_latest_record_pos_info_______________________________________1999-09-09_/
Time_stamped_record_item *Variable_index_list::get_earliest_record_pos_info
( datetime64         &earliest_date_time
, Record_code        &for_record_type_code
, UED::Variable_code    &for_var_code )
{
   earliest_date_time = 9999365L;                                                //030714
   Time_stamped_record_item *earliest_record_item_pos_inf = 0;                   //990909
   FOR_EACH(indx,Variable_index_item,each_indx)
   {  if (record_and_variable_code_matches                                       //141101
         (indx->record_type_code,indx->variable_code
         ,for_record_type_code,for_var_code))
      {  Time_stamped_record_item *earliest_in_index  = indx->get_earliest_record_pos_info(earliest_date_time);
         if (earliest_in_index)
         {  earliest_record_item_pos_inf = earliest_in_index;
            // Remember which was there earliest because we are going to do
            // a detailed search for the earliest date once we've found
            // the earliest record.
            for_record_type_code = indx->record_type_code;                       //990909
            for_var_code         = indx->variable_code;                          //990909
         }
      }
   } FOR_EACH_END(each_indx)
   return earliest_record_item_pos_inf;                                          //971104
}
//_get_earliest_record_pos_info_____________________________________1999-09-09_/
Time_stamped_record_item *Variable_index_list::get_latest_record_pos_info
(datetime64        &latest_date_time
,Record_code       &for_record_type_code
,UED::Variable_code &for_var_code)
{
//This method returns the latest date record indexed.
//This function overrides the pure function in UED_database class
//because it can do the search much faster than scanning the file.
//get_latest_date_time for each of the listed indexes,
//return the maximum of these.
   latest_date_time = -9999001;                                                  //141101_030714
   //170217unuse Record_code min_rec_type_code = (for_record_type_code == UED_FOR_ALL_RECORD_TYPES) ? (Record_code)0       : for_record_type_code;
   //170217unused Record_code max_rec_type_code = (for_record_type_code == UED_FOR_ALL_RECORD_TYPES) ? (Record_code)0xFFFF  : for_record_type_code;
   Time_stamped_record_item *latest_record_item_pos_inf = 0;                     //990909
   FOR_EACH(indx,Variable_index_item,each_indx);                                 //030709
   {        if (record_and_variable_code_matches                                 //141101
            (indx->record_type_code,indx->variable_code
            ,for_record_type_code,for_var_code))
      {  Time_stamped_record_item *latest_in_index  = indx->get_latest_record_pos_info(latest_date_time);
         if (latest_in_index)
         {  latest_record_item_pos_inf = latest_in_index;
            /*150422 Not sure what this was for.
               but I am finding problems that this changes
               the search for record and type and var codes

            for_record_type_code = indx->record_type_code;
            for_var_code         = indx->variable_code;
            */
            // Remember which was there earliest because we are
            // going to do a detailed search for the earliest date
            // once we've found the earliest record.
         }
      }
   } FOR_EACH_END(each_indx)                                                     //030709
   return latest_record_item_pos_inf;                                            //971104
}
//_get_latest_record_pos_info_______________________________________1997-09-15_/
void Variable_index_item::update
(datetime64                earliest_date_time_
,datetime64                latest_date_time_
,UED_units_code            time_step_                                            //040125
,BINARY_RECORD_INDEX_NODE *new_latest_index_node
,Database_file_indexed     &/*170217unused database*/)
{  // Remember the index node for fast reaccess
   if (latest_index_node)
       latest_index_node = 0;                                                    //001026
   if (new_latest_index_node)
   {  latest_index_node = new_latest_index_node;
      // Make sure there is a TS record item for this record
      Time_stamped_record_list::update
       (earliest_date_time_,latest_date_time_,time_step_,new_latest_index_node); //040125
   }
}
//_update______________________________________________________________________/
Binary_record *Variable_index_list::append_record
(Binary_data_record_cowl *current_record_
,Database_file_indexed &parent_database)                             submission_ //990314
{  if (!current_record_) return 0; // just to be save                            //010308
   UED::Variable_code rec_to_set_var_code = current_record_->get_variable_code(); //000630
   Record_code   rec_to_set_code  = current_record_->get_code();                 //000630
   Variable_index_item *var_index =  is_empty()                                  //000630
      ?  0 : find_index(rec_to_set_code,rec_to_set_var_code);                    //000630
   if (!var_index)                                                               //990113
   {  // create an variabe index item, we don't need to create
      // a Time_stamped_record_item because in this case, the
      // record doesnt yet have a record position
      var_index = new Variable_index_item(rec_to_set_code,rec_to_set_var_code);  //990113
      append(var_index);                                                         //990113
   }
   BINARY_RECORD_INDEX_NODE *curr_rec_index_node
      = parent_database.get_index_node_for(current_record_);
   datetime64 earliest_date_time=current_record_->get_earliest_date_time(9999999);//030714
   datetime64 latest_date_time  =current_record_->get_latest_date_time(0);       //030714
   //  latest_date_time need to determine because the record may not be set with data yet
   UED_units_code time_step     =current_record_->get_time_step_units_code();    //040125
   var_index->update(earliest_date_time,latest_date_time,time_step,curr_rec_index_node,parent_database); //000706
   return current_record_;
}
//_append_record_______________________________________________________________/
void Time_stamped_record_list::update
(datetime64                earliest_date_time_                                   //030714
,datetime64                latest_date_time_                                     //030714
,UED_units_code            time_step_                                            //040125
,BINARY_RECORD_INDEX_NODE *index_node_) // probably could be &since index nodes are always in memory. //000705
{  Time_query earliest_time_query
      (earliest_date_time_,earliest_date_time_,earliest_date_time_,Time_query::at);
   Time_stamped_record_item *time_stamped_record_item = find_index(earliest_time_query);
   if (!time_stamped_record_item)
   {  time_stamped_record_item = new Time_stamped_record_item
          (earliest_date_time_,latest_date_time_,time_step_,index_node_);        //040125
      append(time_stamped_record_item);
} }
//_update___________________________________________________________1999-01-08_/
void Variable_index_list::update
(Record_code               rec_type_code_
,UED::Variable_code         variable_code_
,datetime64                earliest_date_time_
,datetime64                latest_date_time_
,UED_units_code            time_step_                                            //040125
,BINARY_RECORD_INDEX_NODE *index_node // index nodes are always in memory.       //000705
,Database_file_indexed     &database)
{  Variable_index_item * var_index_item = find_index(rec_type_code_,variable_code_);
   if (!var_index_item)
   {  var_index_item = new Variable_index_item(rec_type_code_,variable_code_);
      append(var_index_item);
   }
   var_index_item->update(earliest_date_time_,latest_date_time_
                         ,time_step_,index_node,database);                       //040125
}
//_update___________________________________________________________1999-01-08_/
Binary_data_record_cowl *Variable_index_item::get_record
(const Time_query       &time_query
,Database_file_indexed  &database)
{  bool record_contains_time_step = 0;
   Binary_data_record_cowl *latest_record_buffer = 0;
   if (latest_index_node)
   {  latest_record_buffer = (Binary_data_record_cowl *)latest_index_node->get(database);
      if (latest_record_buffer)
      {  record_contains_time_step =
           (latest_record_buffer && latest_record_buffer->
              contains_time_step(variable_code,time_query.get_target_date_time()));
   } }
   Binary_data_record_cowl *matching_record =  record_contains_time_step ? latest_record_buffer : 0;
   if (!matching_record)
   {  // The record was not the latest record accessed in this index
      Time_stamped_record_item *matching_TS_record_item = find_index(time_query);
      if (matching_TS_record_item)
      {  matching_record = matching_TS_record_item->get_record(database);
         latest_index_node = matching_TS_record_item->index_node;
   } }
   return matching_record;
}
//_get_record_______________________________________________________2007-07-07_/
Binary_data_record_cowl *Variable_index_list::get_record
(Record_code             record_type_code
,UED::Variable_code       variable_code
, const Time_query      &time_query
,Database_file_indexed  &database)
{  // Returns 0 if nothing matches in the indexes
   Variable_index_item *matching_index = find_index(record_type_code,variable_code);
   return matching_index
      ? matching_index->get_record(time_query,database)
      : 0;
}
//_get_record_______________________________________________________2000-07-07_/
int Time_stamped_record_item::compare(const Item  &other) const
{  Time_stamped_record_item &other_index = (Time_stamped_record_item &)other;
   return
      (earliest_date_time < other_index.earliest_date_time)
      ? -1
      : (earliest_date_time > other_index.earliest_date_time)
         ? 1
         : 0;
}
//__compare_____________________________________________2003-07-13__1998-02-10_/
}//_namespace UED______________________________________________________________/

