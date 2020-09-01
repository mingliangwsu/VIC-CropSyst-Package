#ifndef path_discoveryH
#define path_discoveryH
#include "corn/seclusion.h"
//200818 #include "corn/OS/file_system.h"
#include "corn/OS/FS/recursion.h"
#include "corn/OS/FS/directory_entry.h"

namespace CORN {
namespace OS {
namespace FS {
//______________________________________________________________________________
const nat32 require_specifying_optional =0;
const nat32 require_specifying_1        =1;
const nat32 require_existing_optional   =0;
const nat32 require_existing_1          =1;

#define REQUIRE_SPECIFYING_EXISTING_OPTIONAL {0,0}
#define REQUIRE_SPECIFYING_EXISTING_1        {1,1}
//______________________________________________________________________________
// The raw can be used for autoinitialized aggregates
struct Path_discovery_struct
{
   /*200818 OS::FS::*/Directory_entry_bitmask DE_type_mask;
      // So can search for relevent directory entry types and/or file etc..

   /*200818 OS::FS::*/Recursion recursion;
      // bitmask of CORN::OS::File_system_recursion
      // Actually use OS::recursion
   /* moved to Recursion
   bool depth_first;
      // Applicable when searching into subdirectory/
      // true indicates that subdirectory recursion
      // first recurses down into the directory structure, then
      // searchs for matching files as the recursion comes out,
      // otherwise the search occurs as directories are entered.

   //nat8 recursion_depth;
   nat8 depth_max;
      // Descend at most levels (a non-negative integer)
      // levels of directories below the starting-points.
      // -maxdepth 0 means only apply the tests and actions to the starting-points themselves.
   nat8 depth_min;
      // Do not apply any tests or actions at levels less than levels
      // (a non-negative integer).
      // -mindepth 1 means process all files except the starting-points.
   // NYI conceptual
   */

   /* Abandoned. If there is to be separate seach based on recursion direction
      have a separate descoverty
   const char *descendents_pattern;
   const char *ascendents_pattern;
   */
   struct Require
   {
      // The number of DEN occurances matching the specified pattern
      // that are required.
      // 0 indicates the a DEN matching that pattern may be omitted.
      // 1 indicates processing a single file
      // an arbitrary number indicates only a limited number of files should be accepted/listed/discovered
      // 0xFFFFFFFF indicates unlimited
      nat32 specifying;
         // Indicates occurances on the command line
      nat32 existing;
         // Indicates occurances in the file system.
   } require;
   const char *include_pattern;
      // conceptual perhaps have Seclusion
      // a method to take SDF list if
      // seclusion patterns (with include/exclude) as in
      // lines of include/exclude file

};
//______________________________________________________________________________
interface_ Path_discovery
: public extends_interface_ CORN::Item                                           //200807
{
   // Many utilities can take input from stdin and output to stdout.

   // Programs may alternatively specify source and/or target file names
   // on the command line
   // Either with command line options such as
   // source=xxxx source_filename=xxxx
   // target=xxxx target_filename=xxxx
   // with or without  -- or - introducer

   // The source and/or target filename specified on the command line
   // could be simply identifiable by extension (or any pattern).

   // The program may specify multiple matching files.
   // (Although Linux shell may perform pattern match before
   // the command line is is processed.

   // Filenames are typially in the current working directory
   // but some programs might research for matching files recursively
   // (either up or down the directory path from the CWD).

   // This class is intended to provide for options
   // of discovering path names either on the command line and/or
   // discovered in the CWD context.

   // The may be source file(s) and target file(s)
   // Typically filenames are specified on the command line.

   // If there are multiple paths, ideally the source filename and target filename will have
   // different extensions to beable to distinguish,
   // Or the user only specifies the source file and a matching
   // target file (with a specific extension) would be created
   // if not outputing to stdout.

   virtual bool get_end()                                                    =0;
      // get_require_xxxx returns the number of paths expected to be discovered
      // Typically either 0 or 1, but may be some other minimum number.
   virtual nat32 get_require_specifying()                     const { return 0;}
      // specified on command line.
   virtual nat32 get_require_existing()                       const { return 0;}
      // existing in file system.
   virtual File_name *provide_primary()                            provision_=0;
   virtual bool setup_structure
      (Data_record &data_rec,bool for_write)                    modification_=0;
   virtual bool take(Item_string *arg)                            submission_=0;
   virtual const std::string &ref_prefix()                              const=0;
};
//_Path_discovery___________________________________________________2020-08-01_/
class Path_discovery_cowl
: public implements_ Path_discovery
{public:
   Path_discovery_struct &options_ref;
 protected:
   std::string prefix;
      // I.e. source or target
      // either argument prefix or argument section name
      /* conceptual
      // in importer/exporter utilities this could be operation import/export
      */

 public:
   Seclusion seclusion;   // specified
      // Subdirectory names to be included/excluded.
      // In depth first descendent search,
      // excluded directories with not be searched further.
   /*200804 abandoned
   Seclusion descendents_seclusion;
      // Subdirectory names to be included/excluded.
      // In depth first search, excluded directories with not be searched further.
   Seclusion ascendents_seclusion;
      // Superdirectory names to be included/excluded.
   */
   /*abaondoned moved to options_ref.required
   bool must_exist;
      // If true in get end search the directory context
      // (with optional recursion decendents and/or ascendents)
      // for the DEN if not specified on the command line
   */
 private: // the following are used when reading command line/data record
   std::string recursion_raw;
   bool depth_first_raw;

   std::string filename_raw;
   std::string filenames_SDF_raw;
      // The UED convertor had option to
      // specify multiple filenames  UED_filenames=...

 /*abandoned  there are no occurances where I have search for
   both file and directorys to be keep separately.
   If it is necessary to have separate lists, search then separate as needed
 public:
   providable_ CORN::Unidirectional_list *discovered[directory_entry_type_count];
   // This could contain path specified on the command line.
   // The array items are index directory entery types specified
   // by the DE_type_mask.
   // The discovered DE names are Item_wstring.
 */
 public:
   providable_ Unidirectional_list discovered;
   // This could contain path specified on the command line.
   // The array items are index directory entry types specified
   // by the DE_type_mask.
   // The discovered DE names are Item_wstring.
   providable_ File_name *primary;   bool primary_owned;
 public:
   Path_discovery_cowl
      (Path_discovery_struct &options_ref_
      ,const char *prefix_
      );
   virtual bool setup_structure(Data_record &data_rec,bool for_write) modification_;
   virtual bool take(Item_string *arg)                              submission_;
   virtual bool get_end();
   virtual nat32 get_require_specifying() const {return options_ref.require.specifying;}
      // specified on command line.
   virtual nat32 get_require_existing()   const {return options_ref.require.existing;}
      // existing in file system.
   nat32 discover_paths()                                            provision_;
      //abandoned(Directory_entry_bitmask preferred_DE_type_mask=0)  provision_;
      // if preferred is 0 get the mask from the options
   virtual File_name *provide_primary()                              provision_;
      // This will be the first discovered.
      // The filename may be omitted in cases where
      // the source input in stdin target output is stdout
   virtual const std::string &ref_prefix()                 const{return prefix;}
};
//_Path_discovery_cowl______________________________________________2020-08-01_/
/*conceptual
class Path_discovery_clad
: public extends_ Path_discovery_cowl
{protected
   Path_discovery_struct options;
 public:
   Path_discovery_clad
      (const char *section_=0
      ,...
      )
};
//_Path_discovery_clad______________________________________________2020-08-04_/
*/
}}}//_namespace_CORN::OS::FS___________________________________________________/
#endif

