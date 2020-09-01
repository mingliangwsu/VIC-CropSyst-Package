#include "import_export_table.h"
#include "corn/data_source/generic_datarec.h"
#include "corn/container/indexer.h"
#include "corn/format/structure_factory_I.h"

namespace CORN { namespace MicroSoft { namespace Access {
//______________________________________________________________________________
Import_export_table_format_map::Import_export_table_format_map
(Import_export_table_format &import_export_table_format_)
: structural::Map_clad()
, import_export_table_format                       (import_export_table_format_)
{
/*200220 obsolete now set using Compact_valuation
   detail.extent.relative_to_clad.set(FILE_RELATIVE);
   detail.extent.begin_offset = 0;                             //200220
   detail.tabulation_clad.set
      (delimit_TABULATION);
      //200131 (Delineation::DELIMIT_character);
   detail.separation.continuous=false;                         //200220
   detail.separation.delimiter = "|";                          //200220
   detail.separation.quotation = "\"";                         //200220
      // Warning, I don't know the quoting methodology,
      // so far text appears to be double quote but it could be alternating
   detail.separation.decimal   = '.';                          //200220
*/
}
//_Import_export_table_map:constructor______________________________2020-02-20_/

/*200220
Import_export_table_format::Import_export_table_format()
{
   detail_lines_delineation.relative_to_clad.set(FILE_RELATIVE);
   //200220 detail_lines_delineation.start_row_number = 0;
   detail_lines_delineation.extent.begin_offset = 0;                             //200220
   detail_lines_delineation.tabulation_clad.set
      (Delineation::delimit_TABULATION);
      //200131 (Delineation::DELIMIT_character);
   detail_lines_delineation.continuous_delimiter = false; // don't treat continuous delimiters as one
   detail_lines_delineation.delimiter_tab        = false;
   detail_lines_delineation.delimiter_space      = false;
   detail_lines_delineation.delimiter_comma      = false;
   detail_lines_delineation.delimiter_semicolon  = false;
   detail_lines_delineation.delimiter_other      = true;
   detail_lines_delineation.other_delimiter_16   = '|';
   detail_lines_delineation.other_delimiter_string = "";
   detail_lines_delineation.text__quote_mode_clad.set(Delineation::DOUBLE_QUOTE);
      // Warning, I don't know the quoting methodology,
      // so far text appears to be double quote but it could be alternating
   detail_lines_delineation.decimal_mark         = ".";
}
//_Import_export_table_format:constructor___________________________2017-03-28_/
*/


Import_export_table_format import_export_table_format_default;
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ __ _2016-10_13_/
Import_export_table::Import_export_table
(const std::string &filename_
,bool                       keep_read_records_in_memory_
,Indexer                   *optional_indexer_known_                              //111024
,CORN::Data_record         *record_for_indexing_given_)                          //111106
:CORN::Tabular_text_file
   (filename_
   ,std::ios::in                                                                 //141111
   ,keep_read_records_in_memory_
   ,import_export_table_format_default
   ,false // _record_data_positions  these files may be quite large and should be otherwise indexed
   ,optional_indexer_known_)                                                     //111024
, indexer_known                                        (optional_indexer_known_)
, record_for_indexing_owned                         (record_for_indexing_given_) //111106
{}
//_Import_export_table:constructor_____________________________________________/
Import_export_table::~Import_export_table()
{  if (record_for_indexing_owned) delete record_for_indexing_owned;
}
//_Import_export_table:destructor______________________________________________/
bool Import_export_table::goto_file_position(nat32 file_pos)        performs_IO_
{  data_stream->goto_file_position(file_pos);
   return true;
}
//_goto_file_position_______________________________________________2011-11-07_/

structural::Compact_valuation<char> delineation_format[] =
{{"information:",0}
/*
,{" description: !!str
,{" definer: !!str
,{" inception: !!timestamp
,{" notes: !!str
*/
,{"detail:",0}
/*
,{" missing: !!str
,{" timestep:
,{"  count: !!int (default 1)
,{"  units: !units_timestep (default day)
*/
,{" tabulation:","delimit"}
,{" extent:",0} // # (optional)
,{"  rows:","1"}
,{"  begin:",0}
,{"   relative:","file"}
,{"   offset:","1"}  // need to verify this (dont not if these files have header (probably does)
/*
,{"   marker:","xxxxx"} !!str
,{"  end: # (optional)
,{"   marker:","xxxxx"} !!str
*/
,{" separation:",0}
,{"  delimiter:","|"}
,{"  quotation:","\""}
,{"  decimal:","."}
/*
,{"  continuous: !!bool # (optional) (default false)   ","xxxxx"}
,{" notes:","xxxxx"}
,{"document:",0}
,{" missing:","xxxxx"} !!str
,{" lineation:","xxxxx"}!enum ["native", "CRLF", "LF", "CR", "ASA" ]","xxxxx"}
,{" pagination:","xxxxx"} !enum ["none", "FF", "ASA" ]","xxxxx"}
,{" tabulation:","xxxxx"} !enum ["intrinsic", "fixed", "delimit"]","xxxxx"}
,{" notes:","xxxxx"} !!str","xxxxx"}
,{" extent:",0}
,{"  rows:","xxxxx"} !!int  # (conceptual) (optional) the number of rows expected in the document (from the start to the end)","xxxxx"}
,{"  begin:",0}
,{"   offset:","xxxxx"} !!int # 0","xxxxx"}
,{"   relative:","xxxxx"} !enum ["file", "data", "page", "block" ] # default file","xxxxx"}
,{"   marker:","xxxxx"}
,{"  end:,0}
,{"   marker:","xxxxx"}
,{" separation:",0}
,{"  delimiter:","xxxxx"} !!str  # space comma tab semicolon or other using YAML escape sequence
,{"  quotation:","xxxxx"} !!str  # ' " or empty or any other quote character
,{"  decimal:","xxxxx"} !!str # either . or ,
,{"  continuous:","xxxxx"} !!bool # (optional) (default false)
*/
,{"header:",0}
/*
,{" paginate:","xxxxx"} !!bool  # header is repeated after every page break (optional)
*/
,{" tabulation:","delimit"}
/*nt # number of rows reserved for units (default 1)
,{"   parenthesis:","xx
,{" units:",0}
,{"   offset:","xxxxx"} !!int # offset from the start row of the column header (0 indicates units occurs in the caption text)
,{"   rows:","xxxxx"} !!ixxx"} !!bool  # indicates if units are enclose in parenthesis generally used when units are in caption text (optional)
*/
,{" extent:",0}
,{"  rows:","0"}  // No header
,{"  begin:",0}
,{"   offset:","1"}
,{"   relative:","file"}
/*
,{"   marker:","xxxxx"} !!str
*/
/*
,{"  end:",0} # (optional)
,{"   marker:","xxxxx"} !!str
*/
,{" separation:",0}
,{"  delimiter:","|"}
,{"  quotation:","\""}
,{"  decimal:","."}
,{"  continuous:","false"}
,{0,0}
};
//_delineation_format__________________________________________________________/
bool Import_export_table_format_map::register_compacts()
{
   //NYI

}
//_register_compacts___________________________________________________________/
}}}//_namespace_CORN:MicroSoft:Access__________________________________________/

