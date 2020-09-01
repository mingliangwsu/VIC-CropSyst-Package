#ifndef import_export_tableH
#define import_export_tableH
#include "corn/tabular/tabular_text.h"
#include "corn/OS/directory_entry_name.h"
#include "corn/format/structural.h"
//___________________________________________________________________________
namespace CORN { namespace MicroSoft { namespace Access {
//___________________________________________________________________________
struct Import_export_table_format
: public extends_ Tabular_format
{public:
};
//_Import_export_table_format_______________________________________2016-10-13_/
class Import_export_table
: public Tabular_text_file
{  CORN::Dynamic_array<nat32>    NA_column_widths; // Not applicable
   CORN::Dynamic_array<nat32>    NA_column_start;  // Not applicable
 public:
   Indexer           *indexer_known;
   CORN::Data_record *record_for_indexing_owned;
 public:
   Import_export_table
      (const std::string &filename_
      ,bool               keep_read_records_in_memory
      ,Indexer           *optional_indexer_known = 0
      ,CORN::Data_record *record_for_indexing_given = 0);                        //111024
   virtual ~Import_export_table();
   bool goto_file_position(nat32 file_pos)                         performs_IO_;
};
//_Import_export_table______________________________________________2020-02-20_/
class Import_export_table_format_map     // cowl
: public structural::Map_clad
{
   Import_export_table_format &import_export_table_format;
 public:
   Import_export_table_format_map(Import_export_table_format &import_export_table_);
   virtual bool register_compacts();
};
//_Import_export_table_map__________________________________________2020-02-20_/
}}}//_namespace_CORN::MicroSoft::Access________________________________________/
#endif

