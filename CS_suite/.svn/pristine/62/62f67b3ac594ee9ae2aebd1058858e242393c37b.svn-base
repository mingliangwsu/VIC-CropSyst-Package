#ifndef CS_optationH
#define CS_optationH
#include "corn/dynamic_array/dynamic_array_T.h"
#include "corn/container/enumlist.h"
#include "corn/container/binary_tree.h"
#   include "corn/metrology/units_code.hpp"
#include "corn/OS/directory_entry_name.h"
#if ((CS_VERSION > 0) &&(CS_VERSION < 6))
#  define USE_VAR_CODES
#  include "UED/library/varcodes.h"
#endif

namespace CS {
//______________________________________________________________________________
class Optation_temporal
: public implements_ CORN::Item
{
 public:
   CORN::Units_code  timestep;                                                   //170319
   //not currently used bool              desire_everything;                     //111221
 public:
   CORN::Binary_tree          ontologies;
   #ifdef USE_VAR_CODES
   // By version 6 variable codes will be obsolete
   CORN::Dynamic_array<nat32> variable_codes;
   #endif
      // List of variables by code
 public:
   inline Optation_temporal
      (CORN::Units_code timestep_
      // not currently used ,bool              desire_everything_=false
      )
      : timestep                                                     (timestep_)
      , ontologies                                                        (true) // unique
      #ifdef USE_VAR_CODES
      , variable_codes                                                  (0,50,0) // essentially unique
      #endif
      //190119 , desire_everything  (desire_everything_)
      {}
 public: // Item implementation
   inline virtual bool is_key_nat32(nat32 key)                     affirmation_
      { return key=timestep; }
 public:
   bool is_desired(const std::string &ontology)                    affirmation_;
   bool is_desired(const char *ontology)                           affirmation_;
   #ifdef USE_VAR_CODES
   bool is_desired(nat32 variable_code)                            affirmation_;
   #endif

   bool know_ontology(const std::string &ontology)                modification_;
   bool know_variable_code(nat32 variable_code)                   modification_;

   //190119 unusedbool is_anything_desired()                                      affirmation_;
      // returns true if the specified variable is listed as a desired variable.
   /* just expose the ontologies and variable_codes lists
   bool desire    (UED_variable_code var_code)                  initialization_; //170224
   */
   /*obsolete but don't delete
     // This was for Harsimran's project, now desired variables
     // would be listed in TDF files for output
   nat32 include_from_files(const CORN::Container &file_list)   initialization_; //170224
      // Loads variables from each of the listed files.
      // \returns the number of variables ultimately listed
      // (including prexisting and all newly included variables)
      // The file is a simple format with one variable on each line.
      // The variable codes are 32bit hexadecimal (8 digit) numbers
      // The variable code may be followed by an inline name/description comment
      // indicated by # character (padded with at least one space).
      // I.e.
      // 0xFFFFFFFF # description
      // Note that this format is valid YAML.
   */

};
//_class_Optation_temporal__________________________________________2019-01-19_/
class Optation
//: public extends_ CORN::Unidirectional_list
// list of Optation_temporal
{
 protected:
   provided_ CORN::Unidirectional_list temporals;
 public:
   bool is_desired(const std::string &ontology, CORN::Units_code timestep = 0) affirmation_;
   bool is_desired(const char *ontology       , CORN::Units_code timestep = 0) affirmation_;
   #ifdef USE_VAR_CODES
   bool is_desired(nat32 variable_code,CORN::Units_code timestep=0)affirmation_;
   #endif
   bool has_any_daily()                                            affirmation_;
 public:
   bool desire(nat32 variable_code, CORN::Units_code timestep);
 public:
   nat32 optate_TDF
      (const CORN::OS::File_name &TDF_filename);
      // Doesn't necessarily have to be File_name
   nat32 optate_TDFs(const CORN::Container &TDF_filenames);
      // return the number ontologies/properties/variables added
 public:
   Optation_temporal &provide_temporal(CORN::Units_code timestep)    provision_;
};
//_class Optation___________________________________________________2019-01-19_/
extern Optation optation_global;
}//_namespace_CS_______________________________________________________________/
#endif

