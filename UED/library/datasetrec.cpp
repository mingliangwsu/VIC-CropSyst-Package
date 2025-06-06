#include "UED/library/datasetrec.h"
#include "UED/library/datasetrec_creation.h"
#include "UED/library/varcodes.h"
#ifndef compareHPP
#  include "corn/math/compare.hpp"
#endif
#include "corn/math/moremath.h"
#include "corn/chronometry/date_32.h"
#include "corn/chronometry/date_time_64.h"
#include "corn/chronometry/date_const.hpp"
#include "corn/format/binary/binary_file_interface.h"
#include "corn/float16.h"
#include "corn/string/strconv.h"

namespace UED {
//______________________________________________________________________________
Data_record_header::Data_record_header
(Record_code rec_code_)
: Record_base(rec_code_)
,timestep_units_code(0)
,variable_code(0)
,timestamp(0)                                                                    //030714
//,UED_num_values_in_data_record(0)                                              //140612
{}
//_Data_record_header:constructor___________________________________1999-01-08_/
Data_record_header::Data_record_header(const Data_record_header& to_be_copied)
:Record_base(to_be_copied.get_code())
,IO_num_values (to_be_copied.IO_num_values)
,IO_year       (to_be_copied.IO_year)
,IO_date       (to_be_copied.IO_date)
,IO_time       (to_be_copied.IO_time)
,IO_timestep_units_code(to_be_copied.IO_timestep_units_code)
,IO_variable_code(to_be_copied.IO_variable_code)
,timestep_units_code(to_be_copied.timestep_units_code)
,variable_code(to_be_copied.variable_code)
,timestamp(to_be_copied.timestamp)                                               //030714
//,UED_num_values_in_data_record(0)                                              //140612
{}
//_Data_record_header:constructor___________________________________1999-11-21_/
Data_record_header::Data_record_header
(const Data_record_creation_layout& creation_layout
,datetime64 date_time_)                                                          //030714
:Record_base(creation_layout.get_code())
,timestep_units_code(creation_layout.options.timestep_units_code)
,variable_code(creation_layout.variable_code)
,timestamp(date_time_)                                                           //030714
//,UED_num_values_in_data_record(0)                                              //140612
{  if (creation_layout.options.time_stamp_step_units_code != UT_arbitrary_datetime)
   {
      //180219Date_time_clad_64 conventional_time_stamp(date_time_);                          //030714
      Date_time_cowl_64 conventional_time_stamp(date_time_);                          //030714
   // NYI may need this  if  _time_stamp_step_units_code >= year then set_doy(1)
      switch (creation_layout.options.timestep_units_code)
      {  // They following timesteps are normally indexed from the beginning of the year
         case UT_year         :
         case UT_decade       :
         case UT_century      :
         case UT_millenium    :
         case UT_24_hours     :
            conventional_time_stamp.set_time64(0);                               //030714
            conventional_time_stamp.set_DOY(1);
         break;
         case UT_week         :
         case UT_fortnight    :
         case UT_month        :
         case UT_quarter      :
         case UT_trimester    :
         case UT_biannum      :
         case UT_day          :
            conventional_time_stamp.set_time64(0);                               //030714
            conventional_time_stamp.set_DOY(1);
         break;
         // The following time steps are usually indexed begining at a given second
         case UT_millisecond  :
            // NYI
         break;
#ifdef NYI
         // The following time steps are usually indexed begining at a given minute
         case UT_second       :
         case UT_0_seconds    :
         case UT_1_second     :
         // :
         case UT_30_seconds   :
         // :
              // NYI  I think just need to remove take the 24*60 modulus of the time
              xxxx = yyyyy;
              conventional_time_stamp.Time::set(xxxx);
         break;
#endif
         // The following time steps are usually indexed begining at a given hour

         case UT_60_seconds   :
         case UT_0_minutes    :
         case UT_1_minute     :
         // :
         case UT_10_minutes   :
         // :
         case UT_15_minutes   :

         case UT_30_minutes   : // UT_half_hour

         case UT_60_minutes   :
         case UT_minute       :
             // NYI  I think just need to remove take the 24 modulus of the time
               if (creation_layout.options.time_stamp_step_units_code == UT_day)
                  conventional_time_stamp.set_time64(0);                         //050413
         break;
         // The following time steps are usually indexed begining at a given day
         case UT_0_hours      :
         case UT_1_hour       :
         // :
         case UT_hour         :
            conventional_time_stamp.set_time64(0);  break;                       //030714
      }
      timestamp = conventional_time_stamp.get_datetime64();                      //030714
   }
}
//_Data_record_header:constructor___________________________________1999-01-08_/
Data_record_header* Data_record_header::clone()                            const
{  return new Data_record_header(*this);
}
//_clone____________________________________________________________2014-01-18_/
void Data_record_header::identify_IO()
{  IO_num_values          = false;
   IO_year                = false;
   IO_date                = false;
   IO_time                = false;
   IO_timestep_units_code = false;
   IO_variable_code       = false;
   IO_num_values = (is_data_set() || is_data_vector());                          //980927
   switch (get_time_stamp_code())
   {  case time_stamp_year : IO_year = true; break;
      case time_stamp_date : IO_date = true; break;
      case time_stamp_time : IO_time = true; break;
      case time_stamp_date_time : IO_date = true; IO_time= true;break;
   }
   IO_timestep_units_code = (is_data_set() || is_data_vector());                 //980927
   if (has_variable_code())
      IO_variable_code = true;
}
//_identify_IO______________________________________________________1999-01-08_/
void Data_record_header::write_body(Binary_file_interface& parent_database)
{  //This method must only be called by derived Binary_data_record
   Date_time_clad_64 timestamp_clad(timestamp);                                 //030714
   if (IO_num_values)
   {  nat16 num_values = get_num_values();                                       //000517
      parent_database.write_nat16(num_values);
   }
   if (IO_year)
      parent_database.write_nat16(timestamp_clad.get_year()); // For year, we the year;
   else
      if (IO_date && IO_time)                                                    //030714
         parent_database.write_float64(timestamp);                               //030714
      else                                                                       //030714
      {  if (IO_date)
           parent_database.write_nat32(timestamp_clad.get_date32());            //030714
         if (IO_time)
            parent_database.write_nat32(timestamp_clad.get_HHMMSS());           //990336
      }
     if (IO_timestep_units_code)
        parent_database.write_nat32(timestep_units_code);
     if (IO_variable_code)
        parent_database.write_nat32(variable_code);
   // Don't write a generic body, that will be done by derived classes
}
//_write_body__________________________________________________________________/
nat32  Data_record_header::compute_record_body_size()                      const
{  // We use a local length for Data_record_header because
   // when we call read_body directly we are only reading the header
   // part, we must not override the length field.
  nat32  header_length = 0;
     if (IO_num_values)               header_length += sizeof(nat16);
     if (IO_year)                     header_length += sizeof(int16);
     if (IO_date)                     header_length += sizeof(int32);
     if (IO_time)                     header_length += sizeof(int32);
     if (IO_timestep_units_code)      header_length += sizeof(int32);
     if (IO_variable_code)            header_length += sizeof(int32);
  return header_length;
}
//_compute_record_body_size_________________________________________1997-11-03_/
nat32 UED_num_values_in_data_record = 0;
bool Data_record_header::read_body(Binary_file_interface& parent_database) // Binary_file_interface_replacing_Binary_fstream
{  identify_IO() ;
   UED_num_values_in_data_record = (IO_num_values)                               //000517
     ? (nat16)parent_database.read_nat16()
     : (nat16)1;
   if (IO_year)
   {  Date_time_clad_64 timestamp_clad(0);                                      //030714
      Year year = parent_database.read_nat16();
      timestamp_clad.set_YD(year,1); // store single years in date
      timestamp = timestamp_clad.get_datetime64();                              //030714
   } else
   if (IO_date && IO_time)                                                       //030714
      timestamp = parent_database.read_float64();                                //030714
   else                                                                          //030714
   {  Date_time_clad_64 timestamp_clad(0);                                      //030714
      if (IO_date) timestamp_clad.set_date32(parent_database.read_nat32());     //030714
      if (IO_time) timestamp_clad.set_HHMMSS(parent_database.read_nat32());     //030714
      timestamp = timestamp_clad.get_datetime64();                              //030714
   }                                                                             //030714
   if (IO_timestep_units_code) timestep_units_code = parent_database.read_nat32();
   if (IO_variable_code)       variable_code = parent_database.read_nat32();
  // Data_record_header by itself should never do a generic read.
   return true;                                                                  //100128
}
//_read_body________________________________________________________2000-07-05_/
datetime64 Data_record_header::get_final_time_series_date()
{
   Date_time_clad_64 timestamp_clad(timestamp);                                 //030714
   Date_time_clad_64 final_time_series_date(timestamp_clad.get_date32());       //170424_030714
   final_time_series_date.set_datetime64(timestamp);                             //030714
   Year year = timestamp_clad.get_year();
   // data scalars and data vectors only have a single date associated
   if (is_data_set())
   { switch (timestep_units_code)
     { case UT_year :
       {  year += (Year)get_num_values(); //Unum_values;                         //000517
          final_time_series_date.set_year(year);
       } break;
       case UT_biannum:
       {  year += (Year)(get_num_values() / 2);                                  //000517
          final_time_series_date.set_year(year);
          final_time_series_date.inc_days((int16)((get_num_values() % 2)*183));  //000517
          // Warning this should be either Jan 1 or July 1
       }
       break;
       case UT_quarter :
       {  year += (Year)(get_num_values() / 4);                                  //000517
          final_time_series_date.set_year(year);
          final_time_series_date.inc_days((int16)((get_num_values() % 4) * 91)); //000517
          // Warning this should be either Jan 1, Apr 1, July 1, or Oct 1
       }
       break;
       case UT_month :
       {  CORN::Month  month;                                                    //990301
          CORN::DOM    dom;                                                      //990301
          timestamp_clad.gregorian(year,month,dom);                             //990301
          for (CORN::Month m = 0; m < get_num_values(); m++)                     //000517
          {  final_time_series_date.inc_days(CORN::date_get_days_in_month(year,month)); //170526_130226
             month ++;
             if (month > 12)
             {  month = 1;
                year ++;
       } } }
       break;                                                                    //170628
       case UT_week :
       { for (nat16 w = 0; w < get_num_values(); w++)                            //000517
           final_time_series_date.inc_days(7);
       }
       break;
       case UT_day     :
       { // num_values should be 365 or 366 for leap years.
          CORN::Days days_in_year =
             final_time_series_date.days_in_year(year);                          //170525
         final_time_series_date.inc_days(days_in_year -1);
       }
       break;
   }}
   return final_time_series_date.get_datetime64();                               //030714
}
//_get_final_time_series_date__________________________________________________/
datetime64 Data_record_header::get_final_time_series_time()
{
   Date_time_clad_64 timestamp_clad(timestamp);                                 //030714
   Date_time_clad_64 final_time_series_time(timestamp);                          //170424_030714
   final_time_series_time.set_datetime64(timestamp_clad.get_time64());          //030714
   nat16 num_values = get_num_values();
   switch (timestep_units_code)
   {   case UT_hour   : {  timestamp_clad.inc_hours(num_values);  }  break;     //000517
          // num_values should be 60
       case UT_minute : {  timestamp_clad.inc_minutes(num_values);}  break;     //990306
       case UT_second : {  timestamp_clad.inc_seconds(num_values);}  break;     //990306
   }
   return final_time_series_time.get_time64();                                   //030714
}
//_get_final_time_series_time__________________________________________________/
bool Data_record_header::contains_time_step
(Variable_code   check_variable_code
,datetime64          check_date_time)                                            //030714
{  Date_time_clad_64 check_date_time_clad(check_date_time);                      //030714
   Date_time_clad_64 timestamp_clad(timestamp);                                  //030714
   if (check_variable_code != get_variable_code())
     return false;  // This record doesn't contain that data
  // This selection should work for either a data set or a single value
   if (has_date_time_stamp() && !CORN::is_approximately<float64>(check_date_time,0.0,0.0000000000000000001))
   {  if ((check_date_time_clad.get_date32() < timestamp_clad.get_date32())      //030714
       || (check_date_time_clad.get_date32() > (int32)get_final_time_series_date()))
        return false;  // The date is out of range
   } else //we don't check the year (it is built into the date)
   if (has_year_time_stamp())
   {  Year rec_time_stamp_year = timestamp_clad.get_year();                      //030714
      Year check_year      = check_date_time_clad.get_year();                    //040930
      if (get_time_step_units_code() == UT_year)                                 //040930
         // for annual time series, the data array size is unlimited only need
      {  // to check if the year to set is greater than the record time stamp
         // Actually we could adjust the time stamp and to the new lower value
         // and resize and adjust the data array
         if (rec_time_stamp_year > check_year)                                   //040930
            return false;                                                        //040930
      } else                                                                     //040930
         if (rec_time_stamp_year != check_year)
            return false; // These are different years.
   }
  // else we don't care about the date
  if (has_time_time_stamp() )
  {   bool check_datetime_less_than_this_datetime = (check_date_time < timestamp);
      datetime64 final_TS_time = get_final_time_series_time();                                  //030714
      bool check_date_greater_than_final_TS_time = (check_date_time > final_TS_time);           //030714
     return !(check_datetime_less_than_this_datetime || check_date_greater_than_final_TS_time); //980923
  }
  // else we don't care about the time.
  return true;
}
//_contains_time_step___________________________________________________________
sint8 Data_record_header::compare_header(const Data_record_header &other)  const
{
   return (timestamp < other.timestamp)        ? -1 : (timestamp > other.timestamp)         ? 1
// Can't check IO because it isn't set until Read or writing
//   : IO_variable_code   ?
     :   (variable_code < other.variable_code) ? -1 : (variable_code > other.variable_code) ? 1 : 0;
//      :  0;
}
//_compare_header___________________________________________________2014-01-18_/
Binary_data_record_cowl::Binary_data_record_cowl
(Dynamic_array<float32>        &data_values_
,Record_code _rec_code)
:Data_record_header  (_rec_code)
,data_values_ref     (data_values_)                                              //151111
,index_maintained    (false)
,IO_units_code       (true)
,units_code          (0)
,attribute_homogeneous(CORN::not_valid_quality)  // cannot be initially valid, we will either read the attributes or they will be set when data is set
,single_value_32     (0)
,D_attributes        (0,10,CORN::missing_quality)                                //000804
,can_consolidate_attributes(true)                                                //060807
,add_count           (0)                                                         //140118
,auto_float16        (false)                                                     //140130
,write_float16_status_known(false)                                               //140612
,use_float16         (false)                                                     //140612
{}
//_Binary_data_record_cowl:constructor______________________________1999-01-07_/
Binary_data_record_clad::Binary_data_record_clad
(Record_code _rec_code)
:Binary_data_record_cowl(data_values_owned,_rec_code)
,data_values_owned(0,10,0)                                                       //100321
{
};
//_Binary_data_record_clad:constructor______________________________2015-11-11_/
Binary_data_record_clad::Binary_data_record_clad
//(const Binary_data_record_cowl& to_be_copied)                                    //200311
(const Binary_data_record_clad& to_be_copied)
// :Binary_data_record_cowl(data_values_owned,to_be_copied)
:Binary_data_record_cowl(data_values_owned,to_be_copied)
,data_values_owned      (to_be_copied.data_values_owned)
{
   // Warning, I think pass the
}
//_Binary_data_record_clad:constructor______________________________2015-11-11_/
Binary_data_record_cowl::Binary_data_record_cowl
(Dynamic_array<float32>  &data_values_
,const Binary_data_record_cowl &to_be_copied)
:Data_record_header (to_be_copied)
,data_values_ref        (data_values_)                                           //151111
,index_maintained       (to_be_copied.index_maintained)                          //000701
,IO_units_code          (to_be_copied.IO_units_code)                             //000701
,units_code             (to_be_copied.units_code)
,attribute_homogeneous  (to_be_copied.attribute_homogeneous)
,single_value_32        (to_be_copied.single_value_32)
,D_attributes           (to_be_copied.get_attributes_const())                    //000517
,can_consolidate_attributes(true) // because this is creatin a new record that has not been in file
,add_count              (0)                                                      //140118
,auto_float16           (to_be_copied.auto_float16)                              //140130
,write_float16_status_known(false)                                               //140612
,use_float16            (false)                                                  //140612
{}
//_Binary_data_record_cowl__________________________________________1999-11-12_/
Binary_data_record_cowl::Binary_data_record_cowl
(Dynamic_array<float32>        &data_values_,
 const Data_record_creation_layout& creation_layout                              //990107
,datetime64 date_time_
,bool auto_float16_)                                                             //140612_030714
:Data_record_header(creation_layout,date_time_)                                  //990107
,data_values_ref     (data_values_)                                              //151111
,index_maintained    (false)                                                     //151111
,IO_units_code       (false)                                                     //151111
,units_code(creation_layout.options.stored_units_code)
,attribute_homogeneous(CORN::not_valid_quality) // presumed to be invalid because there is no data yet
,single_value_32(0)
,D_attributes(0,10,CORN::missing_quality)                                        //000804
,can_consolidate_attributes(true)                                                //060807
,add_count           (0)                                                         //140118
,auto_float16        (auto_float16_)                                             //140612_140130
,write_float16_status_known(false)                                               //140612
,use_float16(false)                                                              //140612
{}
//_Binary_data_record_cowl:constructor______________________________2015-11-11_/
Binary_data_record_clad::Binary_data_record_clad
(const Data_record_creation_layout& creation_layout                              //990107
,datetime64 date_time_
,bool       auto_float16_)                                                       //140612_030714
: Binary_data_record_cowl
   (data_values_owned,creation_layout,date_time_,auto_float16_)                  //151111
,data_values_owned(0,10,0)  // will be set latter                                //020530
{  if (creation_layout.options.time_stamp_step_units_code != UT_arbitrary_datetime)
   {  int fixed_num_values = 0;
      switch (creation_layout.options.timestep_units_code)
      {  // They following timesteps are normally indexed from the beginning of the year
         case UT_year         : // These can have an indeterminate number of values
         case UT_decade       :
         case UT_century      :
         case UT_millenium    :
         case UT_24_hours     : fixed_num_values = 0; break;
         case UT_week         :  fixed_num_values = 52;break;
         case UT_fortnight    :  fixed_num_values = 26;break;
         case UT_month        :  fixed_num_values = 12;break;
         case UT_quarter      :  fixed_num_values = 4;break;
         case UT_trimester    :  fixed_num_values = 3;break;
         case UT_biannum      :  fixed_num_values = 2;break;
         case UT_day          :
         {  Date_time_clad_64 smart_datetime(date_time_);                        //030714
            fixed_num_values = smart_datetime.days_in_this_year();               //030714_130225
         } break;
         // The following time steps are usually indexed begining at a given second
         case UT_millisecond  :
            // NYI
            fixed_num_values = 1000;
         break;
         // The following time steps are usually indexed begining at a given minute
            // NYI  I think just need to remove take the 24*60 modulus of the time
         case UT_second       :
         case UT_0_seconds    :
         case UT_1_second     : fixed_num_values = 60; break;
         // :
         case UT_30_seconds   : fixed_num_values = 2;
         // :
         break;
         // The following time steps are usually indexed begining at a given hour
            // NYI  I think just need to remove take the 24 modulus of the time
         case UT_60_seconds   :
         case UT_0_minutes    :
         case UT_1_minute     : fixed_num_values = (creation_layout.options.time_stamp_step_units_code == UT_day)?1440:60; break;
         // :
         case UT_10_minutes   : fixed_num_values = (creation_layout.options.time_stamp_step_units_code == UT_day)?144:6; break;
         // :
         case UT_15_minutes   : fixed_num_values = (creation_layout.options.time_stamp_step_units_code == UT_day)?96:4; break;

         case UT_30_minutes    // UT_half_hour
                              : fixed_num_values = (creation_layout.options.time_stamp_step_units_code == UT_day)?48:2; break;
         case UT_minute       : fixed_num_values = (creation_layout.options.time_stamp_step_units_code == UT_day)?1440:60; break;

         // The following time steps are usually indexed begining at a given day
         case UT_60_minutes   :
         case UT_0_hours      :
         case UT_1_hour       :
         case UT_hour         : fixed_num_values = 24; break;
      } // switch
      if (fixed_num_values)
      {  data_values_owned.set(fixed_num_values-1,0.0);                          //151111
         D_attributes.set(fixed_num_values-1,CORN::missing_quality);
         for (int i = 0; i < fixed_num_values; i++)
         {  data_values_owned.set(i,0.0);                                        //151111
            D_attributes.set(i,CORN::missing_quality);
}  }  }  }
//______________________________________________________________________________
Binary_data_record_clad *Binary_data_record_clad::clone()                  const
{  return new Binary_data_record_clad(*this);
}
//_clone____________________________________________________________2014-01-16_/
bool Binary_data_record_cowl::has_attribute_markers()                      const
{  return (attribute_homogeneous & has_attribute_markers_mask) != 0;
}
//______________________________________________________________________________
void Binary_data_record_cowl::identify_IO()
{  consolodate_attributes();
   Data_record_header::identify_IO();                                            //990108
   IO_units_code          = false;
   if (has_units_code())
      IO_units_code = true;
}
//_identify_IO_________________________________________________________________/
nat32  Binary_data_record_cowl::compute_record_body_size()                 const
{  nat32  body_size = Data_record_header::compute_record_body_size();
   if (IO_units_code)    body_size += sizeof(units_code);
                         body_size += sizeof(attribute_homogeneous);
   if (is_data_scalar()) body_size += sizeof(single_value_32);                   //990427
   else
   {   // attribute_homogeneous marker apply only to dataset and vectors
      nat16 num_value = get_num_values();                                        //000517
      nat8 item_size = provide_write_float16_status()                            //140612
            ? sizeof(int16)                                                      //140612
            : sizeof(float32);
      body_size += num_value * item_size;
      if (has_attribute_markers())
         body_size += sizeof(attribute_homogeneous) * num_value;
   }
   return body_size;                                                             //971104
}
//_compute_record_body_size_________________________________________1997-11-03_/
void Binary_data_record_cowl::consolodate_attributes()
{  if (modified)                                                                 //091221
   {
   bool all_attributes_same = true;
   if ( D_attributes.get_count())                                                //000707
   {  nat8 last_attribute = D_attributes.get(0);
      for (nat32  i = 0; i< D_attributes.get_count();i++)                        //000517
      {  if (D_attributes.get(i)!= last_attribute)
               all_attributes_same = false;
         last_attribute = D_attributes.get(i);
      }
      if (all_attributes_same && can_consolidate_attributes)
      {  attribute_homogeneous = last_attribute;
            D_attributes.clear();                                                //000517
      } else attribute_homogeneous = has_attribute_markers_mask;
} } }
//_consolodate_attributes___________________________________________1998-10-02_/
nat16   Binary_data_record_cowl::get_num_values() const
{  return is_data_scalar()
      ? (nat16)1
      : data_values_get_count();
}
//_get_num_values___________________________________________________2000-05-17_/
bool Binary_data_record_cowl::get_creation_layout(Data_record_creation_layout &creation_layout)
{  creation_layout.options.record_type          =get_code();
   creation_layout.variable_code                =variable_code;
   creation_layout.options.stored_units_code    =units_code;
   creation_layout.options.timestep_units_code  =timestep_units_code;
   creation_layout.options.time_stamp_step_units_code=0;
   {  switch (timestep_units_code)
      {  // The following timesteps are normally indexed from the beginning of the year
         case UT_year         :
         case UT_decade       :
         case UT_century      :
         case UT_millenium    :
         case UT_24_hours        :
            creation_layout.options.time_stamp_step_units_code=UT_year;
         break;
         case UT_week         :
         case UT_fortnight    :
         case UT_month        :
         case UT_quarter      :
         case UT_trimester    :
         case UT_biannum      :
         case UT_day          :
            creation_layout.options.time_stamp_step_units_code=UT_year;
         break;
         // The following time steps are usually indexed begining at a given second
         case UT_millisecond  :
            creation_layout.options.time_stamp_step_units_code=UT_minute;//?
         break;
         // The following time steps are usually indexed begining at a given minute
         case UT_second       :
         case UT_0_seconds    :
         case UT_1_second     :
         // :
         case UT_30_seconds   :
         // :
            creation_layout.options.time_stamp_step_units_code=UT_minute;//?
         break;
         // The following time steps are usually indexed begining at a given hour
         case UT_60_seconds   :
         case UT_0_minutes    :
         case UT_1_minute     :
         // :
         case UT_10_minutes   :
         // :
         case UT_15_minutes   :
         case UT_30_minutes   : // UT_half_hour
         case UT_60_minutes   :
         case UT_minute       :
             // NYI  I think just need to remove take the 24 modulus of the time
            creation_layout.options.time_stamp_step_units_code=UT_date;
         break;
         // The following time steps are usually indexed begining at a given day
         case UT_0_hours      :
         case UT_1_hour       :
         // :
         case UT_hour         :
            creation_layout.options.time_stamp_step_units_code=UT_date;
         break;
   } }
   creation_layout.options.default_attribute_code    =attribute_homogeneous;
   return true;
}
//_get_creation_layout__________________________________2005-10-10__2005-05-03_/
bool Binary_data_record_cowl::is_valid()                            affirmation_ //140114
{  // This is used during write to prevent writing records where all the data is invalid
   bool all_invalid_or_missing = true;
   if  (!((attribute_homogeneous == CORN::not_valid_quality) || (attribute_homogeneous == CORN::missing_quality)))
       //we still want to record things out of range (I.e. when set in the UED weather editor 060229
      all_invalid_or_missing = false;
   for (nat32  i = 0; i< D_attributes.get_count() ;i++)
   {  CORN::Quality_code quality = (CORN::Quality_code)D_attributes.get(i);
      // We currently count only missing or not valid as not valid
      // presumably we would allow recording of warning or error range values
      bool invalid_or_missing = (quality == CORN::not_valid_quality) || (quality == CORN::missing_quality);
      if (!invalid_or_missing) all_invalid_or_missing = false;
   }
   return !all_invalid_or_missing;
}
//_is_valid_________________________________________________________2005-06-03_/
Record_code Binary_data_record_cowl::get_code_for_write()                  const
{  bool write_float16 = provide_write_float16_status();
   return (write_float16)
      ? rec_code | data_set_float16_subcode
      : rec_code;
}
//_2014-06-12___________________________________________________________________
void Binary_data_record_cowl::write_body(Binary_file_interface &parent_database)
{ // Size format and IO_xx will have been setup when the record size is computed in write()
   bool write_float16 = provide_write_float16_status();                          //140512
   // 140613 although this is moved to get_code() I also set it here
   // to show the value as it is actually stored.
   if (write_float16)                                                            //140130
      rec_code |= data_set_float16_subcode;                                      //140130
   Data_record_header::write_body(parent_database);
   if (IO_units_code)
        parent_database.write_nat32(units_code);
   parent_database.write_nat8(attribute_homogeneous);                            //990107
   if (is_data_scalar())                                                         //990427
        parent_database.write_float32(single_value_32);
   else
   {  // attribute_homogeneous markers only apply to dataset and vectors         //990428
      nat16 num_values = get_num_values();                                       //000517
      if (is_data_set_float16() || write_float16)
         write_data_values_float16 (parent_database,num_values);                 //140129
      else write_data_values_float32(parent_database,num_values);                //140129
      if (has_attribute_markers())
         parent_database.write_nat8_array(D_attributes.get_array(),num_values);  //081212
} }
//_write_body__________________________________________________________________/
bool Binary_data_record_cowl::write_data_values_float32
   (Binary_file_interface &parent_database,nat16 num_values)        performs_IO_
{  parent_database.write_float32_array(data_values_ref.get_array(),num_values);  //151111
   return true;
}
//_write_data_values_float32________________________________________2014-01-29_/
bool Binary_data_record_cowl::write_data_values_float16
(Binary_file_interface &parent_database,nat16 )                     performs_IO_
{  nat32 UED_num_values_in_data_record = data_values_get_count();
   Dynamic_array<float16_bits>values_f16(UED_num_values_in_data_record);
   for (nat32 v = 0; v < UED_num_values_in_data_record; v++)
   {  float16 value_f16(data_values_ref.get(v));                                 //151111
      /*unused float32value_f32 =*/  data_values_ref.get(v);                     //151111
      values_f16.set(v,value_f16.get_bits());
   }
   parent_database.write_int16_array
      (values_f16.get_array(),UED_num_values_in_data_record);                    //081212
   return true;
}
//_write_data_values_float16________________________________________2014-01-29_/
bool Binary_data_record_cowl::do_all_float16_values_have_acceptable_precision()    const
{
   bool all_have_acceptable_precision = true;
   nat32 UED_num_values_in_data_record = data_values_ref.get_count();            //151111_140612
   for (nat32 v = 0; (v < UED_num_values_in_data_record) && all_have_acceptable_precision; v++) //151215
   {  float16 value_f16(data_values_ref.get(v));                                 //151111
      float32 value_f32 = data_values_ref.get(v);                                //151111
      bool this_has_acceptable_precision = CORN::has_acceptable_precision(value_f32,3);
      all_have_acceptable_precision &= this_has_acceptable_precision;
      if (!this_has_acceptable_precision)
      {
//This if for testing/debugging
   char var_code_hex_str[10];
   CORN::nat32_to_cstr(get_variable_code(),var_code_hex_str,16);
   float32 f16_to_f32 = value_f16;
   std::cerr << "output inprecision::"
      << " value="   << value_f32
      << " written=" << f16_to_f32
      << " var="     << var_code_hex_str
      << std::endl;
      }
   }
   return all_have_acceptable_precision;
}
//_do_all_float16_values_have_acceptable_precision__________________2014-01-29_/
bool Binary_data_record_cowl::read_body(Binary_file_interface &parent_database)
{  // Size format and IO_xx will have been setup when the record size is computed in write()
   Data_record_header::read_body(parent_database);
   identify_IO() ;
   if (IO_units_code)
          units_code = parent_database.read_nat32();                             //040715
   attribute_homogeneous = parent_database.read_nat8();                          //990107
      // general attribute of dataset
   if (is_data_scalar())                                                         //990427
        single_value_32 = parent_database.read_float32();
   else
   {  data_values_ref.resize(UED_num_values_in_data_record);                     //151111_990427
      if (is_data_set_float16())                                                 //140129
      {  use_float16 = true;                                                     //161223
         write_float16_status_known = true;                                      //161223
         read_data_values_float16(parent_database);                              //140129
      }
      else  read_data_values_float32(parent_database);                           //140129
      data_values_ref.clear_changed();                                           //161222
      // Once the data has been read, it is not considered to be change.
      if (has_attribute_markers())
         {  can_consolidate_attributes = false;                                  //060807
            D_attributes.resize(UED_num_values_in_data_record);                  //081212
            parent_database.read_uint8_array(D_attributes.get_array(),UED_num_values_in_data_record);
            D_attributes.clear_changed();                                        //161222
         } // If we latter add or change values, then we need to make sure the D_array will be filled in with the overriding attribute
         else D_attributes.set_default_value(attribute_homogeneous);             //001103
   }
   return true;                                                                  //100128
}
//_read_body___________________________________________________________________/
bool Binary_data_record_cowl::read_data_values_float32(Binary_file_interface &parent_database) performs_IO_
{  parent_database.read_float32_array(data_values_ref.get_array(),UED_num_values_in_data_record);   //151111_081212
   return true;
}
//_read_data_values_float32_________________________________________2014-01-29_/
bool Binary_data_record_cowl::read_data_values_float16(Binary_file_interface &parent_database) performs_IO_
{
   Dynamic_array<float16_bits>values_f16(UED_num_values_in_data_record);
   parent_database.read_int16_array(values_f16.get_array(),UED_num_values_in_data_record);   //081212
   for (int v = 0; v < UED_num_values_in_data_record; v++)
   {  float16 value_f16;
      int16 value_bits = values_f16.get(v);
      value_f16.set_bits(value_bits);
/*
//150610 this if statement is just a temporary fix because
// the HADgem data sets only have 360 days.
//Delete this if statement because I should fix the NETCDF to UED convertor to handle 360 day datasets
      if      (value_bits == infinity_positive_bits)
         D_attributes.set(v,out_of_range_high_error_quality);
      else if (value_bits == infinity_negative_bits)
         D_attributes.set(v,out_of_range_low_error_quality);
      else
*/
      data_values_ref.set(v,value_f16);                                          //151111
   }
   return true;
}
//_read_data_values_float16_________________________________________2014-01-29_/
datetime64 Binary_data_record_cowl::get_date_time_for_index(nat16 index)        const
{   // given an index to the values array, computes the date/time for that entry.
   Date_time_clad_64 result(timestamp);
     //if data set, at this time we assume this is the correct record
     // otherwize index will be out of range which we can also catch below.
   switch (timestep_units_code)
   {  case UT_year    : result.inc_years(index);              break;
      case UT_biannum : result.inc_months((int16)(index * 6));break;
      case UT_quarter : result.inc_months((int16)(index *3)); break;
      case UT_month   : result.inc_months(index);             break;
      case UT_week    : result.inc_days((int16)(index *7));   break;
      case UT_day     : result.inc_days(index);               break;
      case UT_hour    : result.inc_hours(index);              break;             //030714
      case UT_minute  : result.inc_minutes(index);            break;             //030714
      case UT_second  : result.inc_seconds(index);            break;             //030714
      default :                                                                  //990507
      {  if ((timestep_units_code >= UT_1_second) && (timestep_units_code <= UT_60_seconds))  //990507
            result.inc_seconds(index * (timestep_units_code - UT_1_second ));   //030714
         if ((timestep_units_code >= UT_1_minute) && (timestep_units_code <= UT_60_minutes))  //990507
            result.inc_minutes(index * (timestep_units_code - UT_1_minute ));   //030714
         if ((timestep_units_code >= UT_1_hour) && (timestep_units_code <= UT_24_hours))      //990507
            result.inc_hours(index * (timestep_units_code - UT_1_hour ));       //030714
      } break;
    }
    return result.get_datetime64();                                              //030714
}
//_get_date_time_for_index__________________________________________1998-10-02_/
datetime64 Binary_data_record_cowl::get_earliest_date_time(datetime64 earliest_date_time_yet) const
{  datetime64 record_earliest_date_time = earliest_date_time_yet;                //030714
   if (has_attribute_markers())
   {
      nat16 value_count =get_num_values();                                       //181204
      bool attribute_is_valid = false;                                           //181204
      for (nat16 index = 0
      ;(index <= value_count) &&
        (attribute_is_valid = attribute_valid(D_attributes.get(index)))          //181204_000505
      ;index++)
      {  datetime64  index_date_time = get_date_time_for_index((nat16)index);
         if (index_date_time < record_earliest_date_time)                        //990909
               record_earliest_date_time = index_date_time;                      //030714
      }
   } else
      record_earliest_date_time = get_time_stamp();                              //030714
   if (record_earliest_date_time < earliest_date_time_yet)                       //030714
      earliest_date_time_yet = record_earliest_date_time;                        //030714
   return earliest_date_time_yet;                                                //030714
}
//_get_earliest_date_time___________________________________________1998-10-02_/
datetime64 Binary_data_record_cowl::get_latest_date_time(datetime64 latest_date_time_yet) const
{  datetime64 record_latest_date_time = latest_date_time_yet;
   if (has_attribute_markers())
   {  for ( int32 index = (nat32 )((int32)get_num_values() - 1)                  //001027
          ; (index >=0) && attribute_valid(D_attributes.get((nat32 )index))      //181204
          ; index--)
      {  datetime64  index_date_time =get_date_time_for_index((nat16)index);
            if (index_date_time > record_latest_date_time )                      //181204_990909
               record_latest_date_time = index_date_time;
      }
   } else if (get_num_values())
   {  nat16 index = (nat16)((int16)get_num_values() - 1);                        //001027
      record_latest_date_time = get_date_time_for_index(index);                  //030714
   }
   return record_latest_date_time;                                               //030714
}
//_get_latest_date_time_____________________________________________1998-10-02_/
nat16 Binary_data_record_cowl::count_data_values()                         const
{  return
   is_data_scalar()
   ? (int16)1
   : (nat16)data_values_ref.get_count();                                         //151111
}
//_count_data_values________________________________________________2000-07-07_/
int16 Binary_data_record_cowl::time_step_index(datetime64 date_time_idx)   const
{  Date_time_clad_64 date_time_index(date_time_idx);                             //030714
   CORN::Year date_time_year = date_time_index.get_year();                       //200106
   Date_time_clad_64 timestamp_clad(timestamp);                                 //030714
   CORN::Year timestamp_year = timestamp_clad.get_year();                       //200106
   int16 index = -1;
     //if data set, at this time we assume this is the correct record
     // otherwize index will be out of range which we can also catch below.
   switch (timestep_units_code)
   { case UT_year :
      {  // In this case we simply index by year
         index =(int16)(date_time_year-timestamp_year);                          //200106_030714
      } break;
      case UT_biannum:                                                           //030714
      {  index =(int16)((date_time_year-timestamp_year)*2                        //200106_030714
          + (date_time_index.get_month() >= 7 ? 1 : 0));
      } break;
      case UT_quarter :
      {  index = (int16)((date_time_year-timestamp_year)*4);                     //200106_030714
         int quarter = 0;
         switch (date_time_index.get_month())
         { case  1 : case  2 : case  3 : quarter = 1; break;
           case  4 : case  5 : case  6 : quarter = 2; break;
           case  7 : case  8 : case  9 : quarter = 3; break;
           case 10 : case 11 : case 12 : quarter = 4; break;
         }
         index += (int16)(quarter -1);
      } break;
      case UT_month :
      {
         index = (int16)((date_time_year - timestamp_year) * 12);                //200106_030714
         index += (int16)(date_time_index.get_month() - 1);
      } break;
      case UT_week :
      { index = (int16)((date_time_year - timestamp_year) * 52);                 //200106_030714
        index += (int16)(date_time_index.get_DOY() * 52 / 366);
      } break;
      case UT_day     :
      { // num_values should be 365 or 366 for leap years.
         Date_clad_32 ts_date(timestamp_clad.get_date32());                      //030714
         Date_clad_32 index_date(date_time_index.get_date32());                  //030714
         index = (int16)ts_date.days_between(index_date,false);                  //030714
      } break;
      case UT_hour    :
      { // num_values should be 60
        index = (int16)(date_time_index.get_hour()); // hours are 0 to 23
      } break;
      case UT_minute :
      { index = (int16)(60 * date_time_index.get_hour()
              + date_time_index.get_minute()); // minutes are 0 to 59
      } break;
      case UT_second :
      { index =(int16)( 24 * date_time_index.get_hour()
              + 60 * date_time_index.get_minute()
              + date_time_index.get_second()); // seconds are 0 - 59
      } break;
    }
    if ((timestep_units_code >= UT_0_seconds) && (timestep_units_code <= UT_60_seconds))   //050413
    {    Date_time_clad_64 time_diff( date_time_idx - timestamp);
         nat32  seconds_diff  = time_diff.get_seconds();
         nat32  step = timestep_units_code - UT_0_seconds;
         index = (int16)( seconds_diff / step);
    }
    if ((timestep_units_code >= UT_0_minutes) && (timestep_units_code <= UT_60_minutes))   //050413
    {    Date_time_clad_64 time_diff( date_time_idx - timestamp);
         nat32  minutes_diff  = time_diff.get_minutes(); // Plus 1 is for rounding error (I think it may be needed)
         nat32  step = timestep_units_code - UT_0_minutes;
         index = (int16) CORN_round_int((double)minutes_diff / (double)step);
    }
    if ((timestep_units_code >= UT_0_hours) && (timestep_units_code <= UT_24_hours)) //050413
    {    Date_time_clad_64 time_diff( date_time_idx - timestamp);
         nat32  hours_diff  = time_diff.get_hours();
         nat32  step = timestep_units_code - UT_0_hours;
         index = (int16)( hours_diff / step);
    }
  return index;
}
//_time_step_index_____________________________________________________________/
float32 Binary_data_record_cowl::get_value
(CORN::Quality &datum_attribute
,datetime64 get_date_time)                                                 const //030714
{  float32 result = 0;
   if (is_data_set())
   {  int16 index = time_step_index(get_date_time);
      result = get_at(index,datum_attribute);
   } else
     if (!is_data_set()) // single value if date and time don't match return false
     { if (get_date_time == timestamp)
       {  result = single_value_32;
          datum_attribute.assume_code((CORN::Quality_code)attribute_homogeneous);//150127
       } else
          datum_attribute.assume_code(not_valid_quality);                        //150127
     }
   return result;
}
//_get_value___________________________________________________________________/
bool Binary_data_record_cowl::get_parameter_at
(modifiable_ CS::Parameter &parameter
,nat32  index)                                                             const
{  bool got = false;
   nat32  attribute_count = D_attributes.get_count();
   parameter.assume_code((CORN::Quality_code)
      (attribute_count ? D_attributes.get(index) : attribute_homogeneous));      //150120
   got = parameter.update_value_with_units
         (data_values_ref.get(index,false)
         ,units_code);
   return got;
}
//_get_parameter_at_________________________________________________2015-11-10_/
bool Binary_data_record_cowl::set_parameter_at
(const CS::Parameter &parameter
,nat32  index)                                                     modification_
{  bool sat = false;
   CORN::Units_clad param_units(CORN::unknown_quality);
   float32 param_value = parameter.get_with_units(param_units);
   CORN::Quality_clad param_quality(parameter.get_quality());
   set_at
      (index
      ,param_value
      ,param_quality
      ,true);
   assert(param_units.get() == units_code);
   sat = true;
   return sat;
}
//_set_parameter_at_________________________________________________2015-12-24_/
bool Binary_data_record_cowl::get_parameter_on
(modifiable_ CS::Parameter &parameter
,datetime64 get_date_time)                                                 const
{  bool got = false;
   if (is_data_set())
   {  int16 index = time_step_index(get_date_time);
      got = get_parameter_at(parameter,index);
   } else
     if (!is_data_set()) // single value if date and time don't match return false
     { if (get_date_time == timestamp)
       {  got = parameter.update_value_with_units(single_value_32,units_code);   //151124
          parameter.assume_code((CORN::Quality_code)attribute_homogeneous);      //150127
       } else
          parameter.assume_code(not_valid_quality);                              //150127
     }
   return got;
}
//_get_parameter_on_________________________________________________2015-11-22_/
bool Binary_data_record_cowl::set_parameter_on
(const CS::Parameter &parameter
,datetime64 set_date_time)
{  bool sat = false;
   if (is_data_set())
   {  int16 index = time_step_index(set_date_time);
      sat = set_parameter_at(parameter,index);
   } else
     if (!is_data_set()) // single value if date and time don't match return false
     { if (set_date_time == timestamp)
       {  sat = true;
          single_value_32 = parameter.in_prescribed_units();
          attribute_homogeneous = parameter.get_quality_code();
       } else
       {
          single_value_32 = parameter.in_prescribed_units();
          attribute_homogeneous = // parameter.assume_code(not_valid_quality);
          sat = false;
       }
     }
   return sat;
}
//_set_parameter_on_________________________________________________2015-12-21_/


/*151122 Conceptual
bool Binary_data_record_cowl::setup_parameter_reference_at
(modifiable_ CS::Parameter_cowl_T<float32> &parameter
,nat32  index)                                                             const
{  bool satup = false;
   if (get_units_code() == parameter.get_units())
   {
      nat32  attribute_count = D_attributes.get_count();
      parameter.assume_code((CORN::Quality_code)(attribute_count ? D_attributes.get(index) : attribute_homogeneous)); //150120
      parameter.reset_amount_pointer(&(data_values_ref[index]));
   } else
   {
      assert(false);
      // So if this assert is reached

      // I may need to have the parameter property take the units of the
      // stored value but currently that is a reference not a pointer
      // and I don't think I have the ability to do units conversion
      // where the units are variable.

   }
   return satup;
}
//_setup_parameter_reference_at_____________________________________2015-11-10_/
bool Binary_data_record_cowl::setup_parameter_reference
(modifiable_ CS::Parameter_cowl_T<float32> &parameter
,datetime64 get_date_time)                                                 const
{  //CORN::Quality &datum_attribute;
   bool result = false;
   if (is_data_set())
   {  int16 index = time_step_index(get_date_time);
      result = setup_parameter_reference_at(parameter,index);
   } else
     if (!is_data_set()) // single value if date and time don't match return false
     { if (get_date_time == timestamp)
       {  parameter.reset_amount_pointer(&single_value_32);
          parameter.assume_code((CORN::Quality_code)attribute_homogeneous);//150127
       } else
          parameter.assume_code(not_valid_quality);                        //150127
     }
   return result;
}
//_setup_parameter_reference________________________________________2015-11-19_/
*/
//151122 set_value should actually return bool (if success)
float32 Binary_data_record_cowl::set_value
(float32 value  // The value units at this point must be in the record specified units
,CORN::Quality &new_attribute  // was a returned value                           //081024_971104
   // The program can specify the desired attribute,
   //   UED will try to give a value for the attribute
   //   attribute will be set to the attributes available
   // UED will check the current record attribute; if the attribute
   // doesn't match it will check if individual timestep attributes
   // exist and will not overwrite values of higher quality unless
   // override safty is enabled.
,datetime64 set_date_time                                                        //970909
   // UED will try to give the value for the specified date and time.
   // If the date and time is not available, UED will try to estimate
   // a value (the returned attribute will be set accordingly
   // If the value cannot be estimated, the attribute will also be set accordingly
   //   If a record doesn't already exist for the time period matching the date and time,
   //   A record will be created having the specified units.
,bool override_safety)
{  float32 result = 0;
   if (is_data_set())
   {  if (timestep_units_code > 0x1F00) // This is a bug fix, not sure what is causing the timestep_units_code to become invalid
         timestep_units_code = UT_day;
      nat16 index = time_step_index(set_date_time);
      result = set_at(index,value,new_attribute,override_safety);                //000710
   } else
       if (set_date_time == timestamp)                                           //030714
       { // WARNING need to set/check existing attribute override safty ect
         if (new_attribute.is_same_or_better_than((Quality_code)attribute_homogeneous) || override_safety)  //000710
         {  attribute_homogeneous = (UED_quality_attribute)new_attribute.get_quality_code();  //150121_081024
            single_value_32 = value;
            set_modified(true);                                                  //000705
         } else                                                                  //000710
         {  // We couldn't override value, return current condition.             //000710
             new_attribute.assume_code((CORN::Quality_code)attribute_homogeneous); //150123_000710
             result = single_value_32;                                           //000710
         }
       }
   return result;                                                                //000710
}
//_set_value_____________________________________________________________1997?_/
nat32 Binary_data_record_cowl::set_missing_as(float32 valid_value,CORN::Quality_code valid_quality)
{  nat32 missing_count = 0;
   for (nat32 index = 0; index < data_values_ref.get_count(); index++)           //151111
   {  CORN::Quality_code quality_i = (CORN::Quality_code)D_attributes.get(index);//140611
      if ((quality_i == CORN::missing_quality) || (quality_i == CORN::not_valid_quality))
      {  CORN::Quality_clad validated_quality(valid_quality);
         set_at(index,valid_value,validated_quality,true);
         missing_count++;
      }
   }
   return missing_count;
}
//_set_missing_as___________________________________________________2014-06-09_/
float32 Binary_data_record_cowl::get_at(nat32  index, CORN::Quality &datum_attribute ) const
{  nat32  attribute_count = D_attributes.get_count();
   CORN::Quality_code quality_code = attribute_count
      ? (CORN::Quality_code)D_attributes.get(index)                              //190130
      : (CORN::Quality_code)attribute_homogeneous;                               //190130_180326
   datum_attribute.assume_code(quality_code);                                    //180326_150120
   return data_values_ref.get(index);                                            //151111
}
//_get_at___________________________________________________________2005-05-10_/
float32 Binary_data_record_cowl::set_at
( nat16 index
, float32 value  // The value units at this point must be in the record specified units
, CORN::Quality &new_attribute // was a return value                             //000710
   // The program can specify the desired attribute,
   //   UED will try to give a value for the attribute
   //   attribute will be set to the attributes available
   // UED will check the current record attribute_homogeneous; if the attribute
   // doesn't match it will check if individual timestep attributes
   // exist and will not overwrite values of higher quality unless
   // override safty is enabled.
, bool override_safety)                                                          //0000710
{  float32 result = value;
   if (is_data_set())
   // This method applies only to data sets
   // Don't set modified to false because before this point because the record
   // can be modified by other previous operations.
   {  // set/check existing attribute override safty ect
      bool first_entry = data_values_ref.get_count() == 0;                       //151111_130305
      if (first_entry)                                                           //130305
         attribute_homogeneous = new_attribute.get_quality_code();               //150121_130305
      CORN::Quality_code curr_attribute = (CORN::Quality_code)
         (((CORN::Quality_code)attribute_homogeneous == has_attribute_markers_mask) //000708
         ? D_attributes.get(index) : attribute_homogeneous);                     //000708
      if (new_attribute.is_same_or_better_than(curr_attribute)||override_safety) //000708
      {  result = data_values_ref.force(index,value);                            //180308_151111
         set_modified(true);
         if (curr_attribute!=new_attribute.get_quality_code())                   //150121
         {  if (attribute_homogeneous  != has_attribute_markers_mask)
            {  // This is the first occurance of a different quality in the dataset
               for (int32 a_i = 0; a_i < index; a_i++)
               {  // All the earlier qualities need to be set to the previously set homogeneous quality
                  D_attributes.set(a_i,attribute_homogeneous);
               }
               // The quality is no longer homogeneous
               attribute_homogeneous =has_attribute_markers_mask;                //000720
            } // else we have already found that there are different qualities in the dataset
            nat8 new_attr_qual_code = new_attribute.get_quality_code();          //200226
            D_attributes.set(index,new_attr_qual_code);                          //200226
         }
      } else
      {  // We didn't set the new value, we return the unmodified current
         // values and quality.
         result = data_values_ref.get(index);                                    //151111_000710
         new_attribute.assume_code(curr_attribute);                              //150123_081028
   } }
   return result;
}
//_set_at___________________________________________________________2000-03-23_/
CORN::Quality_code Binary_data_record_cowl::set_quality_at
(nat16 index
,const CORN::Quality &new_attribute)
{  if (is_data_set())
   // This method applies only to data sets
   // Don't set modified to false because before this point because the record
   // can be modified by other previous operations.
   {  if (attribute_homogeneous  != has_attribute_markers_mask)
      {  // This is the first occurance of a different quality in the dataset
         for (int32 a_i = 0; a_i < index; a_i++)
         {  // All the earlier qualities need to be set to the previously set homogeneous quality
            D_attributes.set(a_i,attribute_homogeneous);
         }
         // The quality is no longer homogeneous
         attribute_homogeneous =has_attribute_markers_mask;                      //000720
      } // else we have already found that there are different qualities in the dataset
      D_attributes.set(index,(nat8)new_attribute.get_quality_code());            //150121
   }
   return new_attribute.get_quality_code();                                      //150121_140902
}
//_set_quality_at___________________________________________________2014-06-15_/
float32 Vector_data_record::set(nat16 index, float32 value, CORN::Quality &new_attribute)
{  set_modified(true);                                                           //990427
   attribute_homogeneous=(UED_quality_attribute)new_attribute.get_quality_code();//150121_050721
   return data_values_owned.set(index,value);                                    //151119
}
//_set______________________________________________________________2000-07-07_/
Vector_data_record::Vector_data_record(const Vector_data_record &to_be_copied)
:Binary_data_record_clad((Binary_data_record_clad)to_be_copied)
{ }
//_Vector_data_record:constructor___________________________________1999-11-21_/
Vector_data_record *Vector_data_record::clone()                            const
{  return new Vector_data_record(*this);
}
//_Vector_data_record::clone________________________________________2014-01-16_/
int8 Binary_data_record_cowl::compare_meta
   (const Binary_data_record_cowl &other)                                  const //151111
{  int8 header_compare = compare_header(other);
   header_compare = (header_compare == 0)
   ?(   (units_code    < other.units_code)    ? -1 : (units_code    > other.units_code)    ? 1
      // Variable code is already checked in the header : (variable_code < other.variable_code) ? -1 : (variable_code > other.variable_code) ? 1
      : (data_values_ref.get_count() - other.data_values_ref.get_count()))       //151111
   : header_compare;
   return header_compare;
}
//_compare_meta_____________________________________________________2014-01-18_/
nat32 Binary_data_record_cowl::add_corresponding
(const Binary_data_record_cowl &addend_record)                     modification_ //151111
{  nat32 count = 0;
   if (compare_meta(addend_record) == 0)
   {  // The records appear to be the same form
      // Currently the records must
      // - have the same timestamp
      // - cover the same period of time (eventually I may relax this requirement allowing different periods to be added, possibly resizing augend array)
      // - have the same units  (eventually I may perform units conversion as needed
      // - have the same number of items
      provide_add_count();
      if (is_data_scalar()) // At this point we know both records are the same form
      {  if (is_valid_quality(addend_record.attribute_homogeneous))
         {  single_value_32 += addend_record.single_value_32;
            nat8 count_0 = add_count->get(0) + 1;
            add_count->set(0,count_0);
            count += 1;
         }
      }
      else
      {  for (nat16 i = 0; i < get_num_values(); i++)                            //170525
         {  CORN::Quality_clad augend_attribute;
            CORN::Quality_clad addend_attribute;
            float32 augend = get_at(i,augend_attribute);
            float32 addend = addend_record.get_at(i,addend_attribute);
            if (addend_attribute.is_valid())  // (I.e. some corresponding items in both records may be invalid)
            {  augend += addend;
               set_at(i,augend,addend_attribute,true);
               nat8 count_i = add_count->get(i) + 1;
               add_count->set(i, count_i);
               // The augend keeps track of the count of additions or each corresponding item.
               count += 1;
            }
      // If set_variable_code_sum is true and
      // if the variable code (if specified by this record) is UED standard,
      // the code qualifier flag/indicator is set to 'sum'.
         }
      }
   }
   return count; // the number of elements that were actually added.
      // 0 if the records do not correspond.
}
//_add_corresponding________________________________________________2014-01-18_/
nat32 Binary_data_record_cowl::finalize_average()                  modification_
{  nat32 count = 0;
   if (add_count) // There has previous additions, so we can set averages
   {  if (is_data_scalar())
      {  nat32 added_count_0 =  add_count->get(0);
         if (added_count_0)
         {  single_value_32 /= (float32)added_count_0;
            count += 1;
         }
      } else
      {  for (nat16 i = 0; i < get_num_values(); i++)                            //170523
         {  nat32 add_count_i = add_count->get(i);
            CORN::Quality_clad sum_attribute;
            float32 sum_i = get_at(i,sum_attribute);
            if (add_count_i)
            {  float32 average_i = sum_i / (float32)add_count_i;
               set_at(i,average_i,sum_attribute);
               count += 1;
            }
      // If set_variable_code_sum is true and
      // if the variable code (if specified by this record) is UED standard,
      // the code qualifier flag/indicator is set to 'sum'.
         }
      }
      delete add_count; add_count = 0;
   }
   // else the variable remains average

      // Uses the counts and the current values accumulated from previous
      // calls to calculate average values for each corresponding item.
      // The record's data values are replaced with the corresponding average value.
      // After the averages a computed, the count array is deleted.
      // If there had been no previous calls to add_corresponding
      // (and thus no add_count array) this method does nothing.
      // returns the number of items averaged.
      // The add_count array is reset.  Because the count is reset,
      // any subsequent additions will result in skewed averages
      // in any subsequent calls to 'finalize_average'.
      // So use  finalize_average only once after completing all averages.
   return count;
}
//_finalize_average_________________________________________________2014-01-18_/
Dynamic_array<nat32>* Binary_data_record_cowl::provide_add_count() contribution_
{  if (!add_count)
   {  nat32 data_values_count = get_num_values();
      add_count = new Dynamic_array<nat32>(data_values_count);
      for (nat32 i = 0; i < data_values_count; i++)
      {  // Initially these counts are 1
         // (if this record's corresponding initial value is valid).
         CORN::Quality_clad attribute_i
            (has_attribute_markers()
             ? Quality_code(D_attributes.get(i))
             : Quality_code(attribute_homogeneous));
         add_count->set(i,attribute_i.is_valid() ? 1:0);
      }
   }
   return add_count;
}
//_provide_add_count________________________________________________2014-01-18_/
bool Binary_data_record_cowl::provide_write_float16_status()          provision_
{  if (!write_float16_status_known)
   {  use_float16 =
         is_auto_float16() &&
         do_all_float16_values_have_acceptable_precision();                      //140612
      write_float16_status_known = true;
   }
   return use_float16;
}
//_provide_write_float16_status_____________________________________2014-06-12_/
using namespace std;
#ifdef UED_DUMP
void Binary_data_record_cowl::dumpX(std::ostream &txt_strm)
{  Record_base::dumpX(txt_strm);
   CORN::Date_clad_32 date_part((int32)(timestamp));
   std::string date_label(date_part.as_string());
   txt_strm << date_label << " | date_time" << endl;
   CORN::Units_clad smart_TS_units_code(timestep_units_code);
   std::string TS_units_code_label_buffer;                                       //170423
   smart_TS_units_code.label_string(TS_units_code_label_buffer);                 //170423
   txt_strm << hex << timestep_units_code << "h | timestep_units_code (" << TS_units_code_label_buffer << ')' <<endl;
   Variable_code_clad smart_var_code(variable_code);
   txt_strm << hex << variable_code << "h | ";
   std::string var_code_label_buffer;                                            //170423
   smart_var_code.label_string(var_code_label_buffer);                           //170423
   txt_strm << var_code_label_buffer << endl;
   CORN::Units_clad smart_units_code(units_code);
   std::string units_code_label_buffer;smart_units_code.label_string(units_code_label_buffer);  //170423
   txt_strm << hex << units_code << "h | units_code (" << units_code_label_buffer << ')' << endl;
   txt_strm << hex << (int) attribute_homogeneous << "h | attribute" << endl;
   txt_strm << get_num_values() << " | num_values" << endl;
   if (is_data_scalar())
   {  txt_strm << "value:";
      txt_strm << single_value_32 << endl;
   } else
   {  { txt_strm << "values:";
         for (int i = 0; i < get_num_values(); i++)
            txt_strm << data_values_ref.get(i) << ' ';
         txt_strm << endl;
      }
      if (has_attribute_markers())
      {  txt_strm << "attributes:";
         for (int i = 0; i < get_num_values(); i++)
            txt_strm << hex << (int)(D_attributes.get(i)) << "h ";
         txt_strm << endl;
      }
   }
}
//_dumpX____________________________________________________________1998-08-05_/
#endif
void Binary_data_record_cowl::write_YAML(std::ostream &YAML_strm,nat16 indent_level)
{
   std::string indent(indent_level,' ');
   Record_base::write_YAML(YAML_strm,indent_level);

   CORN::Date_clad_32 timestamp_clad((int32)(timestamp));
   switch (timestep_units_code)
   {
      case UT_year :
         timestamp_clad.prefer_date_format(&year_only_date_format);
      break;
      default : break; // use default format (ISO)
   }

   YAML_strm<<indent << "timestamp: " << timestamp_clad.as_string()  << std::endl; // may want to have timestamp

   CORN::Units_clad smart_TS_units_code(timestep_units_code);
   std::string TS_units_code_label_buffer;                                       //170423
   smart_TS_units_code.label_string(TS_units_code_label_buffer);                 //170423

   YAML_strm<<indent << "timestep_units: " << TS_units_code_label_buffer
      << " # " << "0x"  << hex << timestep_units_code << std::endl;

   Variable_code_clad smart_var_code(variable_code);
   std::string var_code_label_buffer;                                            //170423
   smart_var_code.label_string(var_code_label_buffer);                           //170423
   YAML_strm<<indent << "variable: " << "0x" << hex << variable_code <<  " # " << var_code_label_buffer << std::endl;

   CORN::Units_clad smart_units_code(units_code);
   std::string units_code_label_buffer;smart_units_code.label_string(units_code_label_buffer);  //170423
   YAML_strm<<indent << "units: " << units_code_label_buffer << " # "  << "0x" << hex << units_code  << std::endl;

   bool attribute_is_homogeneous = true;

   YAML_strm<<indent << "# count " << dec << get_num_values()<< std::endl;
   YAML_strm<<indent << "value: ";

   if (is_data_scalar())
   {
      YAML_strm << single_value_32 << endl;
   } else
   {
      YAML_strm << "[";
      YAML_strm << data_values_ref.get(0);
      for (int i = 1; i < get_num_values(); i++)
         YAML_strm << ", " << data_values_ref.get(i);
      YAML_strm << "]" << std::endl;
      if (has_attribute_markers())
      {  attribute_is_homogeneous = false;
      }
   }

   YAML_strm<<indent << "attribute: {";
   if (attribute_is_homogeneous)
   {   YAML_strm<<indent << "homogeneous: " << "0x" << hex << (int)attribute_homogeneous;
   } else
   {
         YAML_strm << "heterogeneous: [";
         YAML_strm << "0x" <<hex << (int)(D_attributes.get(0));
         for (int i = 1; i < get_num_values(); i++)
            YAML_strm << "0x" << hex << (int)(D_attributes.get(i));
         YAML_strm << "]" ;
   }

   YAML_strm<<indent << "}" << std::endl;
}
//_write_YAML_______________________________________________________2018-01-08_/
}//_namespace UED______________________________________________________________/

