#include "STATSGO_SSURGO_database.h"
#include "corn/parameters/parameter.h"
#include "corn/container/enumlist.h"
#include "corn/string/strconv.hpp"
#include "corn/OS/directory_entry_name.h"
#include "corn/OS/file_system_engine.h"
#include "soil/survey/database/tabular/chorizon.h"
#include "soil/survey/database/tabular/component.h"
#include "soil/survey/database/tabular/mapunit.h"
#include "soil/survey/database/tabular/chtexturegrp.h"
#include "corn/format/binary/binary_file_fstream.h"
#include "corn/application/URL_link.h"
#include "corn/data_source/VV_file.h"
#include <assert.h>

/*
Need to implement 170907

When setting up index

STATSGO database argument
option to create/use
master index that includes all records
(use this when working throughout the country)

or index only seclusions
(use this when working with various parts of the country).
*/

using namespace CORN;
namespace USDA_NRCS { namespace STATSGO2_SSURGO {
//______________________________________________________________________________
CORN::Parameter_number *chorizon_fields;
CORN::Parameter_number *component_fields;
CORN::Parameter_number *mapunit_fields;
//______________________________________________________________________________
Database::Database
(const Arguments &arguments_                                                     //170831
,const CORN::Seclusion &mapunit_seclusion_)                                      //170903
 // I.e. C:\Simulation\Database\STATSGO2\gsmsoil_us
:arguments(arguments_)
,mapunit_seclusion(mapunit_seclusion_)                                           //170903
, table_index_directory_name                                                     //170904
   (arguments_.table_index_directory_name.has_name()
    ? arguments_.table_index_directory_name
    : (arguments_.database_directory_name))
, chorizon_table     (0)   ,chorizon_indexer(0)
, component_table    (0)   ,component_indexer(0)
, mapunit_table      (0)   ,mapunit_indexer(0)
, chtexturegrp_table (0)   ,chtexturegrp_indexer(0)                              //110324
{}
//_Database:constructor________________________________________________________/
Database::~Database()
{  if (chorizon_table)     delete chorizon_table;     if (chorizon_indexer)      delete chorizon_indexer;
   if (component_table)    delete component_table;
   if (component_indexer)  delete component_indexer;
   if (mapunit_table)      delete mapunit_table;      if (mapunit_indexer)       delete mapunit_indexer;
   if (chtexturegrp_table) delete chtexturegrp_table; if (chtexturegrp_indexer)  delete chtexturegrp_indexer;
}
//_Database:destructor_________________________________________________________/
#define provide_XXXX_table(provide_NAME_table,NAME_table,table_filename)       \
CORN::Data_table &Database::provide_NAME_table()              provision_       \
{  if (!NAME_table)                                                            \
   {  CORN::OS::File_name_concrete qualified_filename_txt((arguments.database_directory_name),table_filename);\
      NAME_table = new CORN::MicroSoft::Access::Import_export_table(qualified_filename_txt.c_str(),false,0,0 );\
   };                                                                          \
   return *NAME_table;                                                         \
};
provide_XXXX_table(provide_chorizon_table    ,chorizon_table      ,L"chorizon.txt");
provide_XXXX_table(provide_component_table   ,component_table     ,L"comp.txt"    );
provide_XXXX_table(provide_mapunit_table     ,mapunit_table       ,L"mapunit.txt" );
provide_XXXX_table(provide_chtexturegrp_table,chtexturegrp_table  ,L"chtexgrp.txt");
//_provide_XXXX_table__________________________________________________________/
#define provide_XXXX_indexer(provide_NAME_indexer,NAME_indexer,key_field_name, context_key_field_name,   NAME_table,provide_NAME_table,record_type_for_indexing_given,relevent_context_keys_,key_seclusion_)  \
const Table_indexer &Database::provide_NAME_indexer(bool persistent) provision_ \
{  if (!NAME_indexer)                                                           \
   {  if (!NAME_table) provide_NAME_table();                                    \
      {  NAME_indexer = new Table_indexer(*NAME_table,key_field_name,context_key_field_name, new USDA_NRCS::record_type_for_indexing_given(),persistent?&arguments.table_index_directory_name:0,relevent_context_keys_,key_seclusion_);              \
      }                                                                         \
   }                                                                            \
   return *NAME_indexer;                                                        \
}
provide_XXXX_indexer(provide_mapunit_index      ,mapunit_indexer     ,"mukey"  ,"lkey" , mapunit_table       ,provide_mapunit_table     ,Mapunit_record     ,0    ,&mapunit_seclusion)
provide_XXXX_indexer(provide_component_index    ,component_indexer   ,"cokey"  ,"mukey", component_table     ,provide_component_table   ,Component_record   ,&provide_mapunit_index  (true),0)
provide_XXXX_indexer(provide_chorizon_index     ,chorizon_indexer    ,"chkey"  ,"cokey", chorizon_table      ,provide_chorizon_table    ,CHorizon_record    ,&provide_component_index(true),0)
provide_XXXX_indexer(provide_chtexturegrp_index ,chtexturegrp_indexer,"chtgkey","chkey", chtexturegrp_table  ,provide_chtexturegrp_table,CHtexturegrp_record,&provide_chorizon_index (true),0)
                                                                     // for chtexturegrp we index on chkey
//_provide_XXXX_indexer_____________________________________________2011-11-07_/
Table_indexer::Table_indexer
(CORN::Data_table   &indexed_table_
,const char         *key_field_name_
,const char         *context_key_field_name_                                     //190926
,CORN::Data_record  *record_for_indexing_owned_
,const CORN::OS::Directory_name  *persistent_index_files_directory_
//190930 ,const CORN::Enumeration_list    *relevent_context_keys_                         //190927
,const CORN::Container           *relevent_context_keys_                         //190930
,const CORN::Seclusion           *key_seclusion_)                                //170903
: CORN::Indexer                                                               ()
, key_field_name                                               (key_field_name_)
, context_key_field_name                               (context_key_field_name_) //190926
, curr_key_item                                                              (0)
, indexed_table                                                 (indexed_table_)
, persistent                           (persistent_index_files_directory_!=NULL)
, record_for_indexing_owned                         (record_for_indexing_owned_)
{  setup_index(persistent_index_files_directory_,relevent_context_keys_,key_seclusion_);
}
//_Table_indexer:constructor________________________________________2011-10-24_/
bool Table_indexer::setup_index
(const CORN::OS::Directory_name  *persistent_index_files_directory
,const CORN::Container           *relevent_context_keys                          //190930
,const CORN::Seclusion           *key_seclusion)
{
   std::string buffer;                                                           //140629
   std::string index_name(record_for_indexing_owned->get_primary_section_name());
   index_name.append("_"); index_name.append(key_field_name);
   CORN::OS::file_system_engine.provide_directory(*persistent_index_files_directory); //140929
   CORN::OS::File_name_concrete index_file_name(*persistent_index_files_directory,index_name.c_str(),"index");
   nat32 row = 0;
   nat32 exclude_count = 0; // just informational                                //141211
   nat32 include_count = 0;                                                      //141211
   std::clog << "Index:" << std::endl << index_file_name.c_str() << std::endl;
   if (persistent && CORN::OS::file_system_engine.exists(index_file_name))
   {
      std::clog << "Loading" << std::endl;
      std::clog << "row\tincl\texcl\tKEY" << std::endl;
      std::string last_unique_key;
      std::ifstream index_file(index_file_name.c_str());                         //191001

      index_file.seekg(0,std::ios_base::end);
      nat32 index_size = index_file.tellg();
      index_file.seekg(0,std::ios_base::beg);
      while (!index_file.eof())
      {
         row++;
         Indice *indice = new Indice; // 190930 ("");
         nat32 found_in_filter_index_unused = 0;
         indice->read (index_file);
         bool include_it = true;
         include_it = index_file.good() && !index_file.eof();                    //191001
         if (include_it)
         {
            if (key_seclusion)
            {
               std::string key_buffer;
               indice->key_string(key_buffer);
               include_it &= key_seclusion->is_included_string(key_buffer);         //170903
            } else if (relevent_context_keys)
               {  bool found = relevent_context_keys->find_string(indice->key_context); //190930 unique_key);               //190926
                  include_it = found;
               }
            const char *sort_state = "";
            if (indice->key_unique < last_unique_key)
                  sort_state       = "unsorted" ;
            {
               nat32 curr_pos = index_file.tellg();
               float32 percent_done = (float32)curr_pos/(float32)index_size*100.0;
               std::clog
               << row  << '\t'
               << (int)include_count   << '\t'
               << (int)exclude_count   << '\t'
               << indice->key_unique   << '\t'
               << percent_done << "%"  << '\t'
               << sort_state           ;
            }

            if (include_it)
            {  include_count++;                                                     //141211
               append(indice);
               std::clog << std::endl;
            } else
            {  exclude_count++;
               delete indice; indice = 0;
               std::clog << '\r';
            }
//         if (row%100 == 0)
         } // if include it
         last_unique_key = indice->key_unique;
      } // while eof
   } else
   {  std::clog << "Indexing" << std::endl;
      std::clog << "row\tincl\texcl\tKEY" << std::endl;
      std::ofstream *index_file = persistent
         ? new std::ofstream (index_file_name.c_str())
         : 0;
      if (record_for_indexing_owned)
      {  for (bool ok = indexed_table.goto_first();ok;)
         {  nat32 file_pos = dynamic_cast<Tabular_text_file &>(indexed_table).get_file_position();
            ok = indexed_table.get(*record_for_indexing_owned);                  //111106

            curr_key_item = new Indice;                                          //190930
            if (ok)                                                              //190926
            {
            row++;
            FOR_EACH_IN(section,VV_Section,record_for_indexing_owned->sections,each_section)
            { // This format doesn't really have sections, but the expected data record layout may
                  FOR_EACH_IN(entry,VV_abstract_entry,section->entries,each_entry)
                  {  // I could check the field name but they should be in sequence
                     buffer.clear();                                             //140926
                     entry->append_to_string(buffer);                            //181115
                     std::string variable; entry->append_variable(variable);     //181115

                     const char *variable_as_cstr= variable.c_str();             //181115
                     const char *section_as_cstr = section->get_label().c_str(); //181115

                     if (entry->is_key_string(context_key_field_name))           //190930
                     {
                        curr_key_item->key_context = buffer;
                     }
                     if (entry->is_key_string(key_field_name))                   //190930
                        curr_key_item->key_unique = buffer;                      //190930
                  } FOR_EACH_END(each_entry)
            } FOR_EACH_END(each_section)
            know_position(file_pos);

            // Write all indices to the index files.
            // Only keep in memory the items of interest.

            // While the original index would likely have already secluded keys,
            // the user can apply further subsequent seclusions

            // nat32 index_at = 0;                                                  //190926
            bool include_it = false;
/*
if (curr_key_item->key_unique == "806441:1580761") // component
std::clog << "reached" << std::endl;
*/
            if (key_seclusion)
            {
               std::string key_buffer;
               curr_key_item->key_string(key_buffer);
               include_it |= key_seclusion->is_included_string(key_buffer);      //170903
            } else if (relevent_context_keys)
               {  if (
                      relevent_context_keys->find_string                         //190930
                        (curr_key_item->key_context))                            //190930
                  include_it = true;
               }
            {
               std::clog << (int)row << '\t'
               << (int)include_count << '\t'
               << (int)exclude_count << '\t'
               << curr_key_item->key_unique << '\t';
            }
            if (include_it)
                  std::clog << std::endl;
            else  std::clog << '\r';                                             //141211
            if (!include_it && curr_key_item)
               {  remove(curr_key_item);
                  curr_key_item = 0;
               }

            if (curr_key_item)
            {
               append(curr_key_item);
               curr_key_item->write(*index_file);
               include_count++;                                                  //141211
            } else  exclude_count++;                                             //141211
            } // if ok
         } // for index table
      } // else index creating
      std::clog << std::endl << "Indexed." << std::endl;
      delete index_file;
   }
   std::clog << std::endl << "Sorting...." << std::endl;
   sort();
   std::clog << std::endl << "Sorted." << std::endl;
   indexed_table.reset();     // We read to the end of file which will make rdstate not 'good'
   indexed_table.goto_first();// We want to be in a good state ready to read.
   return true;
}
//_Table_indexer::setup_index_______________________________________2011-11-07_/
const char *Table_indexer::get_key_name(nat8 /*key_number*/) const
{  return key_field_name.c_str();   // there is only one key field
}
//_Table_indexer::get_key_name______________________________________2011-10-24_/
nat8 Table_indexer::get_key_count() const
{  return 1; // there is only a primary key
}
//_Table_indexer::get_key_count_____________________________________2011-10-24_/
bool  Table_indexer::know_position(nat32 position)                    cognition_
{  assert(curr_key_item); // The caller must previously call know_value
   curr_key_item->file_pos = position;
   return true;
}
//_Table_indexer::know_position_____________________________________2011-10-24_/
bool  Table_indexer::know_value
(const std::string &value                                                        //150730
,const char *field , const char *section )                            cognition_ //150731
{
assert(false);

// This is actually abandonend


   bool is_key_field = true;
   if (!field)
      return false;                                                     //181115
   if (key_field_name == field)
   {  curr_key_item = provide_indice(value.c_str());                             //150730
      // We assume the caller will next (after calling know_value for all the fields)
      // call know_position where by curr_mukey_item will have been set by this.
   } else is_key_field = false;
   return is_key_field;
}
//_Table_indexer::know_value________________________________________2011-10-24_/
bool  Table_indexer::commit()
{  //NYN?
   return false;
}
//_Table_indexer::commit____________________________________________2011-10-24_/
nat8 Table_indexer::get_key_sequence(const char *field, const char */*section*/) const
{  return (key_field_name == field) ? 1 : 0;
}
//_Table_indexer::get_key_sequence__________________________________2011-10-24_/
Table_indexer::Indice *Table_indexer::provide_indice(const char *key) provision_
{
assert(false);
return 0;
}
//_Table_indexer::provide_indice____________________________________2011-10-24_/
bool Table_indexer::Indice::write
(std::ostream &strm)                                                  stream_IO_
{
   strm << file_pos << '\t' << key_unique << '\t' << key_context << std::endl;
   return true;
}
//_Table_indexer::Indice::write_binary______________________________2019-09-30_/
bool Table_indexer::Indice::read
(std::istream &strm)                                                  stream_IO_
{  strm >> file_pos >> key_unique >> key_context;
   return true;
}
//_Table_indexer::Indice::read_binary_______________________________2019-09-30_/
const char *Table_indexer::Indice::key_string (std::string &buffer)        const
{  buffer = key_unique;
   return buffer.c_str();
}
//_Indice::key_string_______________________________________________2019-09-27_/
bool Database::Arguments::setup_structure
(CORN::Data_record &data_rec,bool for_write)                       modification_
{  data_rec.expect_directory_entry_name("--tabular"  ,database_directory_name);
   data_rec.expect_directory_entry_name("--index"    ,table_index_directory_name);
   if (!for_write)
   {  //190925 the following are deprecated because also applies to SSURGO
      data_rec.expect_directory_entry_name("--STATSGO2-tabular"  ,database_directory_name);
      data_rec.expect_directory_entry_name("--STATSGO2-index"    ,table_index_directory_name);
   }
   return true;
}
//_Database::Arguments::setup_structure_____________________________2019-02-17_/
bool Database::Arguments::get_end()                                modification_
{  if (!CORN::OS::file_system_engine.exists(database_directory_name))            //190217
   {  URL_Link tabular_link;
      CORN::OS::File_name_concrete URL_filename
         ((*CORN.get_program_directory()),"STATSGO2-tabular.url");
      CORN::VV_File URL_file(URL_filename.c_str());
      URL_file.get(tabular_link);
      if (tabular_link.WorkingDirectory.empty())
         database_directory_name.set_cstr                                        //190217
            ("C:\\Simulation\\Database\\STATSGO2\\gsmsoil_us\\tabular");
      else
         database_directory_name.set_string                                      //190217
         (tabular_link.WorkingDirectory);
   }
   return true;
}
//_Database::Arguments::get_end_____________________________________2018-01-05_/
bool Database::get_mapunit
(const std::string &mukey
,Mapunit_record &mapunit)                                                  const
{  bool got = false;
   const Table_indexer &mapunit_index = provide_mapunit_index(true);
   Table_indexer::Indice *MUindice = dynamic_cast<Table_indexer::Indice *>
      (mapunit_index.find_string(mukey));
   if (MUindice)
   {
      CORN::MicroSoft::Access::Import_export_table &mapunit_table
      =  dynamic_cast<CORN::MicroSoft::Access::Import_export_table &>
         (provide_mapunit_table());
      got
      =  mapunit_table.goto_file_position(MUindice->file_pos)
      && mapunit_table.get(mapunit);
   }
   return got;
}
//_get_mapunit_________________________________________________________________/
bool Database::get_component
(const std::string &cokey
,Component_record &component)                                              const
{  bool got = false;

   const Table_indexer &component_index = provide_component_index(true);
   CORN::MicroSoft::Access::Import_export_table &component_table
   =  dynamic_cast<CORN::MicroSoft::Access::Import_export_table &>
      (provide_component_table());
   const Table_indexer::Indice *co_indice
         = dynamic_cast<Table_indexer::Indice *>
            (component_index.find_string(cokey.c_str()));
   if (co_indice)
   {  bool component_indexed = component_table.goto_file_position(co_indice->file_pos);
      if (component_indexed)
          component_table.get(component);
   }
   return got;
}
//_get_component_______________________________________________________________/
 bool Database::get_chorizon
(const std::string &chkey
,CHorizon_record &chorizon)                                                const
{  bool got = false;
   const Table_indexer &chorizon_index = provide_chorizon_index(true);
   CORN::MicroSoft::Access::Import_export_table &chorizon_table
      = dynamic_cast<CORN::MicroSoft::Access::Import_export_table &>
      (provide_chorizon_table());
   Table_indexer::Indice *CHindice = dynamic_cast<Table_indexer::Indice *>
      (chorizon_index.find_string(chkey));
   if (CHindice)
   {
      bool horizon_found = got = chorizon_table.goto_file_position(CHindice->file_pos);
      if (horizon_found)
         got &= chorizon_table.get(chorizon);
   }
   return got;
}
//_get_chorizon________________________________________________________________/
}}//_namespace  USDA_NRCS STATSGO2_SSURGO______________________________________/

