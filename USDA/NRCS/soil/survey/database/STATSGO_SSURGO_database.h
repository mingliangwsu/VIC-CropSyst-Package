#ifndef STATSGO_SSURGO_databaseH
#define STATSGO_SSURGO_databaseH
#include <stdlib.h>
#include "corn/OS/directory_entry_name_concrete.h"
#include "corn/format/MicroSoft/Access/import_export_table.h"
#include "corn/container/indexer.h"
#include "corn/dynamic_array/dynamic_array_T.h"
#include "corn/container/enumlist.h"
#include "USDA/NRCS/soil/survey/database/STATSGO_SSURGO_types.h"
#include "corn/seclusion.h"

namespace CORN {
class Binary_file_interface;
}
//______________________________________________________________________________
namespace USDA_NRCS
{
   class Mapunit_record;
   class CHorizon_record;
   class Component_record;
namespace STATSGO2_SSURGO {
//______________________________________________________________________________
class Table_indexer
: public CORN::Indexer
//190930 , public CORN::Enumeration_list
, public CORN::Bidirectional_list                                                //190930
{
   public: struct Indice : public CORN::Item
   {  // Most STATSGO2 table keys consist two numbers separated by a colon
      // The second number is apparently always the actually unique index
      // SSURGO only and a single number

      std::string key_unique;  //nat32 key_unique;
      std::string key_context; //nat32 key_context;

      /*190926 I was previously using the property in STATSGO
         keys where the MUKEY was composed with the unique key number
         SSURGO does not have this so I abandon this.
      nat32 STATSGO2_MUkey_nat32X;
      std::string STATSGO2_MUkey_str;                                            //170903
      */
      contribute_ nat32 file_pos;
   public:
      inline Indice()
         //190930 obs(const std::string &key_)
      : CORN::Item()
      , key_unique()    // , key_unique(0)
      , key_context()   // , key_context(0)
      , file_pos(0)
      {
         //190930 set_key_value(key_);                                           //190930

/*moved to set_key_value
         size_t colonpos = key_.find(":");
         // Apparently the map unit number (MUkey) is only used to compose
         // keys in STATSGO2 (SSURGO does not use this idiom)
         // The map unit number is not actually needed to make unique
         // keys, the number following the : is itself unique.
         if (colonpos != std::string::npos)
         {
            std::string STATSGO2_MUkey_str_unused;
            STATSGO2_MUkey_str_unused = key_.substr(0,colonpos);                        //170903
            std::string unique_key_str(key_,colonpos+1);
            //190926 STATSGO2_MUkey_nat32X = atoi(STATSGO2_MUkey_strX.c_str());             //170903
            unique_key = atoi(unique_key_str.c_str());
         }else
         {
            unique_key = atoi(key_.c_str());
            //190926 STATSGO2_MUkey_nat32X = unique_key;
            //190926 STATSGO2_MUkey_strX=key_;                                             //170903
         }
*/
      }

      /*190930
      bool set_key_value(const std::string &key_);                                //190930

      inline virtual bool is_key_nat32(nat32 key)                   affirmation_  //180820
         { return key_unique == key; }
      inline virtual nat32 get_key_nat32()           const { return key_unique;}
      virtual nat32 write_binary(CORN::Binary_file_interface &bin_file) stream_IO_; //111015
      virtual nat32 read_binary(CORN::Binary_file_interface &bin_file)  stream_IO_; //111015
      */

      inline virtual bool is_key_string(const std::string &key)        affirmation_ //180820
      { return key == key_unique; }

      virtual const char    *key_string (std::string &buffer)             const;
      virtual bool  write(std::ostream &)                            stream_IO_; //190930
      virtual bool  read (std::istream &)                            stream_IO_; //190930

/*190930 abandoned
      virtual bool  know_value
         (const std::string &value
         ,const char *field, const char *section)                    cognition_; //190930
*/

   };
   //_Indice___________________________________________________________________/
 protected:
   std::string key_field_name;   // This is the actual unique key field
   std::string context_key_field_name;
      // This is the field that the record is a member of
      // For example the component is a member of (one of many in) map unit
      // or each component may have multiple multiple horizons (chorizon)
   contribute_ Indice *curr_key_item;                                            //150731
   CORN::Data_table &indexed_table;
   bool persistent;
   contribute_ CORN::Data_record       *record_for_indexing_owned;               //121228
 public:
   Table_indexer
      (CORN::Data_table                   &indexed_table
      ,const char                         *key_field_name
      ,const char                         *context_key_field_name                //190926
      ,CORN::Data_record                  *record_for_indexing_owned
      ,const CORN::OS::Directory_name     *persistent_index_files_directory
      //190930 ,const CORN::Enumeration_list       *relevent_context_keys                 //190927
      ,const CORN::Container              *relevent_context_keys                 //190930
      ,const CORN::Seclusion              *key_seclusion_ = 0                    //190927_170903
      ); // may be 0 if not persistent
 protected:
   bool setup_index
      (const CORN::OS::Directory_name  *persistent_index_files_directory
      //190930 ,const CORN::Enumeration_list    *relevent_context_keys                    //190927
      ,const CORN::Container           *relevent_context_keys                    //190930
      ,const CORN::Seclusion           *key_seclusion = 0);                      //170903
   virtual const char *get_key_name(nat8 key_number)                      const;
   virtual nat8       get_key_count()                                     const;
   virtual bool  know_position(nat32 position)                       cognition_;
   virtual bool  know_value
      (const std::string &value
      ,  const char *field, const char *section)                     cognition_;
   virtual bool  commit();
   virtual nat8 get_key_sequence(const char *field, const char *section)  const;
   Indice *provide_indice(const char *key)                           provision_;
};
//_Table_indexer____________________________________________________2011-10-24_/
class Database
{protected:
 public:
   //_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
   class Arguments
   {public:
      nat8 aggregation_option;
      /*
         0=Extract map unit soil components
         1=Aggregate soil sequences in each map unit to create an aggregated representitive pseudopedon
         2=Use primary component as a representitive pedon
      */
      CORN::Arguments &CORN;
      CORN::OS::Directory_name_concrete database_directory_name;
         // I.e. C:\\Simulation\\Database\\STATSGO2\\gsmsoil_us\\tabular
         // STATSGO2_tabular="C:\Simulation\Database\STATSGO2\gsmsoil_us\tabular"
      CORN::OS::Directory_name_concrete table_index_directory_name;
    public:
      inline Arguments
         (CORN::Arguments &CORN_)
         : CORN(CORN_)
         , aggregation_option(2) // use predominate component
         , database_directory_name     ("C:\\Simulation\\Database\\STATSGO2\\gsmsoil_us\\tabular")
         , table_index_directory_name  (CORN.start_directory,"deleteable")
         {}
      virtual bool setup_structure
         (CORN::Data_record &data_rec,bool for_write)             modification_;
      virtual bool get_end()                                      modification_;
   };
   //_Arguments_____________________________________________________2017-08-31_/
   CORN::OS::Directory_name_concrete table_index_directory_name;
   provided_ CORN::MicroSoft::Access::Import_export_table *chorizon_table;     contribute_ Table_indexer *chorizon_indexer;
   provided_ CORN::MicroSoft::Access::Import_export_table *component_table;    contribute_ Table_indexer *component_indexer;
   provided_ CORN::MicroSoft::Access::Import_export_table *mapunit_table;      contribute_ Table_indexer *mapunit_indexer;
   provided_ CORN::MicroSoft::Access::Import_export_table *chtexturegrp_table; contribute_ Table_indexer *chtexturegrp_indexer;//110324
 public:
   const Arguments &arguments;                                                   //170831
   const CORN::Seclusion &mapunit_seclusion;                                     //170903
      // Filters which mapunits to include/exclude
      // Provided by application
      // (optional when empty includes everything)
 public: // 'structors
   Database
      (const Arguments &arguments_                                               //170831
      ,const CORN::Seclusion &mapunit_seclusion_);                               //170903
      // I.e. C:\Simulation\Database\STATSGO2\gsmsoil_us
      // If the table index directory is not specified, then
      // the table index files will be stored in _database_directory_name
      // Often only a small subset of the STATSGO2 map units are needed
      // for a project so only those need to be indexed.
      // When using subsets, it is recommended to store that index files in
      // a special directory.
      // Only store that complete index in the STATSGO database directory.
   virtual ~Database();
 public:
   CORN::Data_table &provide_chorizon_table   ()                     provision_;
   CORN::Data_table &provide_component_table  ()                     provision_;
   CORN::Data_table &provide_mapunit_table    ()                     provision_;
   CORN::Data_table &provide_chtexturegrp_table()                    provision_; //110324
   const Table_indexer &provide_chorizon_index     (bool persistent) provision_;
   const Table_indexer &provide_component_index    (bool persistent) provision_;
   const Table_indexer &provide_mapunit_index      (bool persistent) provision_;
   const Table_indexer &provide_chtexturegrp_index (bool persistent) provision_;
   nat32 refilter_mapunits();                                                    //170903
      /**<
      // Often times, we are only interested in a subset of the
      // available map units.
      // This will remove the specified map units from all the current indexes.
      // Indexing will then be much faster, and more space efficient.
      // This optional function is usually called once, but it can be used
      // mulitple time, I to remove map units that have already been processed.
      // \return the total number of indices eliminated.
      **/
 public:
   bool get_mapunit  (const std::string &mukey,Mapunit_record   &mapunit) const;
   bool get_component(const std::string &cokey,Component_record &component)const;
   bool get_chorizon (const std::string &chkey,CHorizon_record  &chorizon)const;
};
//_Database_________________________________________________________2011-10-24_/
}}//_namespace USDA_NRCS STATSGO2_SSURGO_______________________________________/
#endif

