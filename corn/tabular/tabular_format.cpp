
// This class is deprecated, now using YAML format

#include "corn/container/bilist.h"
#include "corn/tabular/tabular_format.h"
#include "corn/metrology/units_symbology.h"
namespace CORN {
///////////////////////////////////////////////////////////////////////////////
#define LABEL_format                "format"
//200220obs #define LABEL_header_terminator     "header_terminator"
#define LABEL_column_headers        "column_headers"
#define LABEL_rows                  "rows"
#define LABEL_units_rows            "units_rows"
#define LABEL_units_start_row       "units_start_row"
#define LABEL_detail_lines          "detail_lines"
#define LABEL_header_field          "header_field"
#define LABEL_column                "column"
//______________________________________________________________________________

#define     LABEL_data "data"


Tabular_format::Tabular_format()
: Table_definition()
#ifndef USE_MAP_IDIOM
, Common_parameters_data_record (NO_ASSOCIATED_DIRECTORY,"format")               //040925
#endif
{ }
//_Delineation_tabular:constructor______________________________________/
// This constructor is used when loading a format

//_Common_tabular_file__format:constructor______________________________________/
/*NYI
nat32 Delineation_tabular_map::expect()
{
// This constructor is used by UED_export file

/*200220 replacing constructors

// This constructor is used when creating a new format using an editor
Common_tabular_file__format::Common_tabular_file__format
(Format_file_type format_file_type_)
:Common_parameters_data_record (NO_ASSOCIATED_DIRECTORY,"format")                //040925
//200220 ,Extent_layout()
,format_file_type_clad(format_file_type_)
,big_endian(false) // assume Intel processors
,carriage_return_line_feed(false)
,paginated(false)                                                                //070925
,FORTRAN_carriage_control(false)                                                 //020525
,definer("")                                                                  //990307
,inception() // default today                                                 //990307
//200131 ,header_terminator_obsolete(0)                                          //990507
   //200130  This was never really used
   // With YAML, used header end marker, and used YAML escape character/sequence
,delineation_document()                                                          //200220
//200220 ,column_headers_every_page(false)                                                //070925
,column_headers_delineation()
,detail_lines_delineation()                                                      //050411
,columns()                                                                       //040925
//200131 obs ,variable_order_clad(fixed_order)                                   //020505
,missing_data_marker("")                                                         //090305
,year_offset_for_dates_with_only_two_digits_X(0)                                 //120331
,cgi_form_server("")                                                             //090305
,cgi_form_action("")                                                             //090305
,cgi_form_method("")                                                             //090305
{ }
//_Common_tabular_file__format:constructor______________________________________/
// This constructor is used when loading a format
Common_tabular_file__format::Common_tabular_file__format
(const std::string &unused i_description
,Format_file_type   format_file_type_)
:Common_parameters_data_record(NO_ASSOCIATED_DIRECTORY,"format")                 //040925
//200220 ,Extent_layout() // Delineation()
,format_file_type_clad(format_file_type_)
,big_endian(false) // assume Intel processors
,carriage_return_line_feed(false)
,paginated(false)                                                                //070925
,FORTRAN_carriage_control(false)                                                 //020525
,definer("")                                                                  //990307
,inception() // default today                                                 //990307
//200220 obsolete ,header_terminator(0)                                                            //990507
,delineation_document()                                                          //200220
//200220 ,column_headers_every_page(false)                                                //070925
,column_headers_delineation()
,detail_lines_delineation()                                                      //050411
,columns()                                                                       //040925
//200131obs ,variable_order_clad(fixed_order)                                    //020505
,missing_data_marker("")                                                         //090305
,year_offset_for_dates_with_only_two_digits_X(0)                                 //120331
,cgi_form_server("")                                                             //090305
,cgi_form_action("")                                                             //090305
,cgi_form_method("")                                                             //090305
{}
//_Common_tabular_file__format:constructor______________________________________/
// This constructor is used by UED_export file
Common_tabular_file__format::Common_tabular_file__format()
:Common_parameters_data_record(NO_ASSOCIATED_DIRECTORY,"format")                 //040925
//200220 ,Extent_layout() // Delineation()
,format_file_type_clad(FILE_TYPE_unknown)
,big_endian(false) // assume Intel processors
,carriage_return_line_feed(false)
,paginated(false)                                                                //070925
,FORTRAN_carriage_control(false)                                                 //020525
,definer("")                                                                  //990307
,inception() // default today                                                 //990307
//200220obsolete ,header_terminator(0)                                                            //990507
,delineation_document()                                                          //200220
//200220 ,column_headers_every_page(false)                                                //070925
,column_headers_delineation()
,detail_lines_delineation()                                                      //050411
,columns()                                                                       //040925
//200131 obs ,variable_order_clad(fixed_order)                                   //020505
,missing_data_marker("")                                                         //090305
,year_offset_for_dates_with_only_two_digits_X(0)                                 //120331
,cgi_form_server("")                                                             //090305
,cgi_form_action("")                                                             //090305
,cgi_form_method("")                                                             //090305
{}
//_Common_tabular_file__format:constructor______________________________________/


*/



const char *format_file_type_label_table[] =
{"unknown"
,"text"
,"binary"
,"dBase"
,"Lotus123"
,"Excel"
,0};
//_format_file_type_label_table________________________________________________/



#ifdef USE_MAP_IDIOM
#else


//______________________________________________________________________________
//200220 reimplement using Map idiom

#define LABEL_description      "description"
#define LABEL_time_step        "time_step"
#define LABEL_format_file_type "format_file_type"
#define LABEL_big_endian       "big_endian"
#define LABEL_carriage_return_line_feed        "carriage_return_line_feed"
#define LABEL_FORTRAN_carriage_control "FORTRAN_carriage_control"
//obsolete #define LABEL_defined_by       "defined_by"
//obsolete #define LABEL_defined_date     "defined_date"
#define LABEL_notes            "notes"
#define LABEL_time_step_units "time_step_units"
// These are 1 based indexes  (because column references (in identification need to be 1 based)
DECLARE_CONTAINER_ENUMERATED_SECTION(Tabular_format::Column,Column_section_VV,1);

//#if (CS_VERSION < 6)
bool Tabular_format::expect_structure(bool for_write )
{  bool expected = Common_parameters_data_record::expect_structure(for_write );  //161023_120314
   structure_defined = false;                                                    //120314
   set_current_section(LABEL_format);

      //200226 setup_structure(*this,for_write);                                 //050411
      //Item::setup_structure(*this,for_write);                                  //180326
      //Extent_layout::setup_structure(*this,for_write);                         //180326
      expect_string(LABEL_description
         ,Common_parameters_data_record::description.brief
         ,ARBITRARY_STRING_LENGTH_512);                                          //200225obs expect_enum(LABEL_format_file_type,format_file_type_clad);                 //020505
      //200225obs expect_bool(LABEL_big_endian,big_endian,VV_bool_entry::form_true);
      //200225obs expect_bool(LABEL_carriage_return_line_feed,carriage_return_line_feed,VV_bool_entry::form_true);

      //200225 NYI expect_bool("pagination",);                                   //070925
      //NYI expect_bool("paginated",document.);                                  //070925
      //deprecated expect_bool(LABEL_FORTRAN_carriage_control,FORTRAN_carriage_control,VV_bool_entry::form_true); //020525
      expect_string("definer",definer,ARBITRARY_STRING_LENGTH_512);
      expect_int32("inception",inception.date_num);

      //200131 obs expect_enum("column_order",variable_order_clad);              //060324
      //200220obs expect_int16(LABEL_header_terminator,header_terminator);       //040925
      expect_int16("year_offset_when_only_two_digits",year_offset_for_dates_with_only_two_digits); //120331
   set_current_section("data_markers");                                          //050516
      expect_string("missing",document.missing,ARBITRARY_STRING_LENGTH_256);     //050516
      expect_string("missing",header.missing,ARBITRARY_STRING_LENGTH_256);       //050516
      expect_string("missing",detail.missing,ARBITRARY_STRING_LENGTH_256);       //050516
   set_current_section(LABEL_column_headers);
      //200226 dont forget to reimplement header.setup_structure(*this,for_write); //040925
      expect_bool("column_headers_every_page",header.paginate);                  //070925
      expect_nat16(LABEL_rows             ,header.extent.rows,10);
      expect_int16("units_row_offset"     ,header.units_row_offset);
      expect_int16(LABEL_units_rows       ,header.units_rows);
      expect_bool("units_parenthesis"     ,header.units_parenthesis);            //060324
      /*200225
      expect_string("1",column_header_static,1024); //  column_header_static, 1024, 1 , 9 ,false);         //100104
      */
   set_current_section(LABEL_detail_lines);
      expect_int16(LABEL_time_step,detail.timestep);                             //060223
      expect_enum(LABEL_time_step_units,detail.timestep_units_enum);             //060626
      // 200225 reimplement detail.setup_structure(*this,for_write);             //050411
   set_current_section(LABEL_format);
   EXPECT_ENUMERATED_SECTION(LABEL_column,Column_section_VV,columns);            //200807
   structure_defined = true;                                                     //120314
   return expected;
}
//______________________________________________________________________________
//#endif
#endif

bool Tabular_format
::Column::setup_structure(CORN::Data_record &data_rec,bool for_write) modification_
{  bool satup = Tabular_column::setup_structure(data_rec,for_write);
   if (!for_write)
   {  //10017  For backward compatibility, eventually delete  (there should not be a lot of files with this entry and units didn't match except for dates).
         data_rec.expect_enum("units_code",   units); // I used to store the numeric units code in hex
         data_rec.expect_string("description",caption ,255); // Description is being replaced by Caption //060626_
   }
   return satup;
}
//_setup_structure__________________________________________________2004-09-26_/
bool
//200220 Common_tabular_file__format
Tabular_format
::Column::is_key_string
(const std::string &key)                                            affirmation_ //180820
{ return CORN::Tabular_column::is_key_string(key); }
//_is_key_string____________________________________________________2003-09-26_/


/*200220  need to reimplement this. I think  set_delineation() with some sort of bitmask


void Common_tabular_file__format::reset_for_file_type()
{  // In the user interface, when the file type is change
   // we can set up some options that we know are not relevent for the file type.
   switch (format_file_type_clad.get())
   {  case FILE_TYPE_binary :
         //200220 column_headers_delineation.text__quote_mode_clad.set(Delineation::NO_QUOTE);
         column_headers_delineation.separation.quotation="";                     //200220
         column_headers_delineation.tabulation_clad.set(Delineation::intrinsic_TABULATION);
         detail_lines_delineation.separation.quotation="";                       //200220
         //200220 detail_lines_delineation. text__quote_mode_clad.set(Delineation::NO_QUOTE);
         detail_lines_delineation.tabulation_clad.set(Delineation::intrinsic_TABULATION);
      break;
      case FILE_TYPE_dBase :
         //200220          column_headers_delineation.text_quote_mode_clad.set(Delineation::NO_QUOTE);
         column_headers_delineation.separation.quotation = "";                   //200220
         column_headers_delineation.tabulation_clad.set(Delineation::intrinsic_TABULATION);
         //200220          detail_lines_delineation. text_quote_mode_clad.set(Delineation::NO_QUOTE);
         detail_lines_delineation.separation.quotation = "";                    //200220
         detail_lines_delineation.tabulation_clad.set(Delineation::intrinsic_TABULATION);
      break;
      case FILE_TYPE_lotus123 : // same as excel
      case FILE_TYPE_Excel :
         //200220          column_headers_delineation.text_quote_mode_clad.set(Delineation::NO_QUOTE);
         column_headers_delineation.separation.quotation = "";                   //200220
         column_headers_delineation.tabulation_clad.set(Delineation::intrinsic_TABULATION);
         //200220          detail_lines_delineation. text_quote_mode_clad.set(Delineation::NO_QUOTE);
         detail_lines_delineation.separation.quotation = "";                     //200220
         detail_lines_delineation.tabulation_clad.set(Delineation::intrinsic_TABULATION);
      break;
      case FILE_TYPE_text :
      case FILE_TYPE_unknown :                                                   //091130
           //Should need to change anything for text user can define all
      break;
   }
}
//_reset_for_file_type______________________________________________2006-06-26_/

*/



//200220 remimplement/move this, have delineation file (available to the application)
// we specify the file to load
// (OR) use compact structure expectation



bool
//200220 Common_tabular_file__format
Delineation_tabular
::copy_from
(const Delineation_tabular & source)
{  bool copied = true;
   pagination = source.pagination;
   lineation = source.lineation;

   definer = source.definer;
   inception.set_date32(source.inception.get_date32());

   /*200302
   missing_data_marker = source.missing_data_marker;
   */
   year_offset_for_dates_with_only_two_digits = source.year_offset_for_dates_with_only_two_digits;

   /*200220 obsolete copy from handles these  Now pagination an lineation
   start_row_number          = source.start_row_number;
   format_file_type_clad.set(source.format_file_type_clad.get());
   carriage_return_line_feed  = source.carriage_return_line_feed;
   paginated                  = source.paginated;
   FORTRAN_carriage_control   = source.FORTRAN_carriage_control;
   */
   /* 200220 moved Delineation_header_footer WARNING
   column_headers_every_page  = source.column_headers_every_page;
   */


   //200131 variable_order_clad.set(source.variable_order_clad.get());
   //200302 missing_data_marker        = source.missing_data_marker;  // R can also be told to recognise  "NA" and "." and "na"

   copied
   &= document .copy_from(source.document)
   && header   .copy_from(source.header)
   && footer   .copy_from(source.footer)
   && detail   .copy_from(source.detail);
   return copied;
}
//_copy_from____________________________________________2020-02-20__2014-10-08_/
/* 200306 unused
bool
//200220 Common_tabular_file__format
Tabular_format
::copy_from
(const Tabular_format  & source)
{  bool copied = Delineation_tabular::copy_from(source);                         //200220
   assert(false);
   /200220  Need to implement
   copied &= copy_columns ..


   return copied;
}
*/
//_copy_from________________________________________________________2014-10-08_/

bool Tabular_format::get_end()
{
   position_columns();                                                           //200306
   #ifdef USE_MAP_IDIOM
   return true;
   #else
   return Common_parameters_data_record::get_end();                                        //200220
   #endif
}
//_get_end__________________________________________________________2019-03-25_/
}//_namespace_CORN_____________________________________________________________/

