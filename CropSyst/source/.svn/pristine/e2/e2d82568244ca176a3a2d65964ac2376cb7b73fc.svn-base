#include "wshedio.h"
#include "corn/dynamic_array/dynamic_array_T.h"
#define Dynamic_float_array Dynamic_array<float32>
#include "cs_vars.h"
#include "csvc.h"
#include "UED/library/datasetrec_creation.h"
#include "UED/library/timeqry.h"
#include "UED/library/varrec.h"
#include "corn/chronometry/date_32.h"

using namespace std;
// There is a separate database dedicated for cell inputs and one for outputs

// Currently runoff and runon, flow_in and flow_out is stored in the databases in 30minute intervals
// One data record is stored for each day, records are only stored for days with runon/runoff.
// The data records are various lengths depending on the  actual number of intervals of runon/runoff
// All values are stored with CS_UED_U_m  (meters of depth).
//______________________________________________________________________________
// Eventually move to cs_vars.cpp
UED::Variable_format_0 CS_VD_soil_surface_water_runon  (CSVC_soil_runoff_surface_water_runon_time_step_sum,UC_meter,0.0,1000.0,0.0,10000.0,"water runon","runon");
UED::Variable_format_0 CS_VD_soil_surface_water_runoff (CSVC_soil_runoff_surface_water_runoff_time_step_sum,UC_meter,0.0,1000.0,0.0,10000.0,"water runoff","runoff");
UED::Variable_format_0 CS_VD_soil_lateral_in_flow      (CSVC_soil_base_lateral_in_flow_depth_time_step_sum,UC_meter,0.0,1000.0,0.0,10000.0,"lateral in flow","inflowlat");
UED::Variable_format_0 CS_VD_soil_lateral_out_flow     (CSVC_soil_base_lateral_out_flow_depth_time_step_sum,UC_meter,0.0,1000.0,0.0,10000.0,"lateral out flow","outflowlat");

char ID_str_buf[10];
//______________________________________________________________________________
class run_XX_event_search : public UED::Time_search
{public:
   inline run_XX_event_search(datetime64 i_target_date_time)                     //030714
   :UED::Time_search(i_target_date_time,i_target_date_time,i_target_date_time
     ,UT_day,UED::Time_query::at)                                                //050714
   {  low_date_time = (nat32)i_target_date_time;  // Take only the year!
      Date_time_clad_64 smart_ending_date_time(low_date_time);                        //030714
      smart_ending_date_time.set_S(86399);                                       //030714
      high_date_time = smart_ending_date_time.get_datetime64();                  //030714
      // There may already be 30minute precip generated, this new storm event
      // might have more or less values old one in the file, so we will replace it.
      // Warning, this is not save for real data in the database, we must do something different.
      // to prevent unexpected erase.
   }
};
//_run_XX_event_search______________________________________________1999-04-28_/
Enumerated_database::Enumerated_database(const char *_filename)
: UED::Database_file_indexed(_filename ,(std::ios_base::in | std::ios_base::out),false)
{}
//_Enumerated_database:constructor_____________________________________________/
void Enumerated_database::get_XX_in_intervals
(UED::Variable_code    var_code
,UED_units_code    // unused_units_code
,const CORN::Date_const &target_date
,Dynamic_float_array &run_XX_intervals)
{  UED::Time_query_single_date query(target_date.get_datetime64());
   UED::Binary_data_record_cowl *run_XX_rec = locate_data_record
      (UED::Record_type_distinguisher_interface::UED_data_set_date_var_units,var_code,query);
   if (run_XX_rec)
   {  Dynamic_float_array &run_XX_data = run_XX_rec->ref_data_values();          //151111
      run_XX_intervals.copy(run_XX_data);
      run_XX_rec->set_modified();
   }
}
//_get_XX_in_intervals_________________________________________________________/
void Enumerated_database::set_XX_in_intervals
(UED::Variable_code var_code
,UED_units_code    units_code
,const CORN::Date_const &target_date
,Dynamic_float_array &run_XX_intervals)
{  nat32 num_intervals_with_runoff = run_XX_intervals.get_count();
   if (num_intervals_with_runoff)
   {  UED::Data_record_creation_layout creation_layout
         (UED::Record_type_distinguisher_interface::UED_data_set_date_var_units
         ,var_code ,units_code ,UT_30_minutes,UT_year ,simulated_quality);
      creation_layout.allow_arbitrary_time_stamp();
      UED::Binary_data_record_clad *daily_XX_record = new UED::Binary_data_record_clad
         (creation_layout,target_date.get_datetime64(),false);
      Dynamic_float_array &XX_data = daily_XX_record->ref_data_values();         //151111
      XX_data.copy(run_XX_intervals);
      daily_XX_record->set_modified();
      append_record(daily_XX_record,true);
   }
}
//_set_XX_in_intervals_________________________________________________________/
void Enumerated_database::set_by_layer
(UED::Variable_code var_code
,UED_units_code    units_code
,const CORN::Date_const &target_date
,nat8             layer_count
,soil_sublayer_array_64(layer_data))
{  UED::Time_query_single_date time_query(target_date.get_datetime64());
   for (nat8 sl = 1; sl <= layer_count; sl++)
   {  UED::Data_record_creation_layout creation_layout
         (UED::Record_type_distinguisher_interface::UED_data_set_year_var_units
         ,var_code+sl ,units_code ,UT_day ,UT_year,simulated_quality);
      creation_layout.allow_arbitrary_time_stamp();
      bool rec_created = true;
      CORN::Quality_clad attr(simulated_quality);
      set_value((float32)layer_data[sl],time_query,units_code,attr,creation_layout,rec_created);
   }
}
//_set_by_layer________________________________________________________________/
void Enumerated_database::get_by_layer
(UED::Variable_code var_code
,UED_units_code    units_code
,const CORN::Date_const &target_date
,nat8             layer_count
,soil_sublayer_array_64(layer_data))                               modification_
{
   UED::Time_query_single_date time_query(target_date.get_datetime64());
   for (nat8 sl = 1; sl <= layer_count; sl++)
   {  UED::Data_record_creation_layout creation_layout(UED::Record_type_distinguisher_interface::UED_data_set_year_var_units,var_code+sl ,units_code ,UT_day ,UT_year,simulated_quality);
      creation_layout.allow_arbitrary_time_stamp();
      UED_units_code preferred_units = units_code;
      CORN::Quality_clad
       preferred_attribute(UED_give_me_best_quality);
      layer_data[sl] =
         get_no_create(creation_layout.get_code(),var_code+sl,time_query,preferred_units,preferred_attribute);
   }
}
//_get_by_layer________________________________________________________________/
bool Enumerated_database::increment_from
(Enumerated_database &contributing_database
,UED::Variable_code   Xfrom_var_code
,UED::Variable_code   Xto_var_code
,UED_units_code      units_code
,float32               fraction_contributed
,float64             &total_received)
{  contributing_database.goto_BOF();
   UED::Variable_code_clad smart_from_var_code(Xfrom_var_code);
   nat8 to_layer = 0;
   if (smart_from_var_code.is_layered())  to_layer=31;
   else                                   to_layer=0;
   cout << endl;

if (fraction_contributed > 1.0)
cerr << "fract too big! " << fraction_contributed << endl;

total_received = 0;        // warning, this is the total of all water received
float64 total_to_m = 0;
float64 total_from = 0;

   for (nat8 layer = 0; layer <= to_layer; layer++) // For non layered variables, we just look at the variable
   {
      UED::Variable_code from_var_code = Xfrom_var_code + layer;
      UED::Variable_code to_var_code = Xto_var_code + layer;

CORN::Date_clad_32 latest_date;

      contributing_database.goto_BOF();

      UED::Binary_data_record_cowl *last_from_record = 0; // need to make sure not to duplicate last record

      for (UED::Binary_data_record_cowl * from_record = contributing_database.goto_next_data_record()
       ; from_record
       ; from_record = contributing_database.goto_next_data_record())
      {
         if ((from_record->get_variable_code() == from_var_code)
             && (from_record != last_from_record))  // <- might not be needed, trying to debug
         { // records will only have been put in the database when there are values for the day.
            uint16 from_data_count = from_record->get_num_values();

            if (from_data_count)
            {
               Dynamic_float_array from_data(1,20,0); // probably should used copy constructor
               from_data.copy(from_record->ref_data_values());                   //151111

latest_date.set_datetime64(from_record->get_time_stamp());

float64 from_sum_before = from_data.sum();

total_from += from_sum_before;


               from_data.multiply_by(fraction_contributed);

float64 from_sum_after = from_data.sum();

total_received += from_sum_after;

               // Try to find the corresponding record in the target data set.
               run_XX_event_search  time_query(from_record->get_time_stamp());

               UED::Data_record_creation_layout creation_layout(UED::Record_type_distinguisher_interface::UED_data_set_date_var_units,to_var_code  ,units_code ,UT_30_minutes ,UT_year,simulated_quality);
      creation_layout.allow_arbitrary_time_stamp();

               bool record_created;
               UED::Binary_data_record_cowl *to_record = locate_or_create_data_record(time_query,creation_layout,record_created);
               if (to_record)
               {
                 Dynamic_float_array &to_data = to_record->ref_data_values();

float64 to_sum_before =  to_data.sum();

float64 to_sum_after =                 to_data.increment_by(from_data);


total_to_m += to_sum_after;

if (to_sum_after >  1000000.0)
{
cout << "from before:" << from_sum_before << " * " << fraction_contributed << " = " << from_sum_after;
cout << "to before:" << to_sum_before << " + " << from_sum_after << " = " << to_sum_after;
cout << endl;
}
                 to_record->set_modified();
               }
            }
         }
         last_from_record = from_record;
      }

cout << "total from:" << total_from << endl;
cout << "from:" << total_received << endl;
cout << "to  :" << total_to_m << endl;

cout << latest_date.get_date32() << endl;

   }
   // Every single record for the contributing_database is read,
   // the values are multiplied by the fraction_contributed
   // and added to the corresponding values of records in this database.
   // records will created if there is no corresponding record.
   return true;
}
//_increment_from______________________________________________________________/
bool Enumerated_database::initialize()
{  bool inited = UED::Database_file_indexed::initialize();                       //140119
   include_units_code_def(UC_meter);
   include_var_code_def(CS_VD_soil_surface_water_runon);
   include_var_code_def(CS_VD_soil_surface_water_runoff);
   include_var_code_def(CS_VD_soil_lateral_in_flow);
   include_var_code_def(CS_VD_soil_lateral_out_flow);
   return inited;                                                                //140119
}
//_initialize__________________________________________________________________/
Cell_outlet_database::Cell_outlet_database()
: Enumerated_database("output/watershed/outlet.ued")
{}
//_Cell_outlet_database:constructor____________________________________________/
bool Cell_outlet_database::initialize()
{  bool inited = Enumerated_database::initialize();
   include_var_code_def(CS_VD_soil_surface_water_runon);
   return inited;
}
//_initialize__________________________________________________________________/
Channel_runin_database::Channel_runin_database
(const CORN::OS::Directory_name &output_watershed_channel_dir
,const char *channel_ID_str)
: Enumerated_database
   (CORN::OS::Directory_entry_name_concrete(output_watershed_channel_dir,channel_ID_str).c_str())
{}
//_Channel_runin_database:constructor__________________________________________/
bool Channel_runin_database::initialize()
{  bool inited = Enumerated_database::initialize();
   include_var_code_def(CS_VD_soil_surface_water_runon);
   return inited;
}
//_initialize_______________________________________________________2014-01-19_/
void Enumerated_database::get_runon_in_intervals
(const CORN::Date_const &target_date
,Dynamic_float_array &runon_intervals)
{  get_XX_in_intervals(CSVC_soil_runoff_surface_water_runon_time_step_sum,UC_meter,target_date,runon_intervals);
}
//_get_runon_in_intervals______________________________________________________/
void Enumerated_database::get_runoff_in_intervals
(const CORN::Date_const &target_date
,Dynamic_float_array &runoff_intervals)
{  get_XX_in_intervals(CSVC_soil_runoff_surface_water_runoff_time_step_sum,UC_meter,target_date,runoff_intervals);
}
//_get_runoff_in_intervals_____________________________________________________/
void Enumerated_database::set_runoff_in_intervals
(const CORN::Date_const &target_date
,Dynamic_float_array &runoff_intervals)
{  set_XX_in_intervals(CSVC_soil_runoff_surface_water_runoff_time_step_sum,UC_meter,target_date,runoff_intervals);
}
//_set_runoff_in_intervals_____________________________________________________/
void Enumerated_database::get_lateral_flow_in_by_layer
(const CORN::Date_const &target_date
, nat8 num_layers   // sublayer
, soil_sublayer_array_64(layer_data))
{  get_by_layer(CSVC_soil_base_lateral_out_flow_depth_time_step_sum,UC_meter,target_date,num_layers,layer_data);
}
//_get_lateral_flow_in_by_layer________________________________________________/
void Enumerated_database::set_lateral_flow_out_by_layer
(const CORN::Date_const &target_date
, nat8 num_layers   // sublayer
, soil_sublayer_array_64(layer_data))
{  get_by_layer(CSVC_soil_base_lateral_out_flow_depth_time_step_sum,UC_meter,target_date,num_layers,layer_data);
}
//_set_lateral_flow_out_by_layer_______________________________________________/
void Cell_outlet_database::set_outlet_in_intervals
(const CORN::Date_const &target_date
,Dynamic_float_array &runoff_intervals)
{  set_XX_in_intervals(CSVC_soil_runoff_surface_water_runon_time_step_sum,UC_meter,target_date,runoff_intervals);
}
//_set_outlet_in_intervals_____________________________________________________/

