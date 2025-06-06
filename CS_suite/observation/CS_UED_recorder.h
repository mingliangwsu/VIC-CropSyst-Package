#ifndef CS_UED_recorderH
#define CS_UED_recorderH
#include "corn/OS/directory_entry_name_concrete.h"
#include "corn/container/unilist.h"
#include "CS_suite/observation/CS_examination.h"
#include "UED/library/varcodes.h"
//______________________________________________________________________________
namespace UED
{
   class Database_file;
}
//______________________________________________________________________________
namespace CS
{
//______________________________________________________________________________
class Recorder_abstract
{
 protected:
   CS::Examination &examination;
      // Currently this is always the global examination, but it
      // could be other specialized examination.
   contribute_ bool ended_year;                                                  //170413
 public:
   Recorder_abstract();
   const char *compose_conventional_name
      (std::string &composed
      ,const std::string      &ontology
      ,nat8                    level_or_period
      ,const std::string      &period_name // optional may be empty
      ,bool                    stats
      ,const CORN::Units_clad &timestep)                                  const;
 public:
   virtual bool start_year()                                      modification_;
   virtual bool end_year()                                        modification_;
 protected:
   nat16 commit_inspections (CORN::Container &inspections)              const;   //190125
   nat16 commit_tallies     (CORN::Container &tallies)                  const;   //190125
   nat16 commit_inspectors                                                       //190125
      (CORN::Container &inspectors
      ,CORN::Units_code timestep)                                       const;
      // Return the count of committed items.

   virtual bool commit_inspection(/*const*/ Inspection &inspection)     const=0; //190125
   virtual bool commit_tally
      (const Examination::Inspection_tally_store &tally_store)          const=0; //190125
   virtual bool commit_inspector                                                 //190115
      (const Inspector_abstract &inspector
      ,CORN::Units_code timestep)                                       const=0; //190312
};
//_class Recorder_abstract__________________________________________2017-03-19_/
class Recorder_UED
: public extends_ Recorder_abstract
{
 public: // currently public only because  Land_unit_output::start_year() currently uses this
   class UED_File_association
   : public implements_ CORN::Item
   {
    public:
      CORN::OS::File_name *UED_filename; //owned
      UED::Database_file  *UED_file    ; //owned
      CORN::Dynamic_array<nat32> committed_variables;
         //List of variable codes whose definitions have already been committed.
    public:
      inline UED_File_association
      (CORN::OS::File_name   *UED_filename_given
         ,UED::Database_file *UED_file_given)
         : CORN::Item()
         , UED_filename(UED_filename_given)
         , UED_file    (UED_file_given)
         {}
      inline virtual ~UED_File_association()
         {  delete UED_file;
            delete UED_filename;
         }
      inline virtual bool is_key_wstring(const std::wstring &key)   affirmation_ //180820
      { return key.compare(UED_filename->w_str()) ==0; }
      bool append_variable_definition
         (UED::Variable_code  var_code
         ,CORN::Units_code    units_code
         ,const std::string  &ontology);
   };
   //_class UED_File_association____________________________________2016-06-16_/
   const CORN::OS::Directory_name &output_directory;
 public: // could be protected.
   provided_ CORN::Unidirectional_list UED_files;
      //list of UED_File_association
 public:
   Recorder_UED(const CORN::OS::Directory_name  &output_directory);              //171114
   //virtual ~Recorder_UED();
   virtual bool end_day()                                         modification_;
   virtual bool end_year()                                        modification_;
   virtual bool stop()                                            modification_;
 protected:
   virtual bool commit_inspection(/*const*/ Inspection &inspection)       const; //190125
   virtual bool commit_tally
      (const Examination::Inspection_tally_store &tally_store)            const; //190125
   virtual bool commit_inspector                                                 //190125
      (const Inspector_abstract &inspector
      ,CORN::Units_code timestep)                                         const; //190312
 private:
   UED_File_association &provide_UED_file_association
      (CORN::OS::File_name_concrete *target_UED_filename_given)      provision_;
   UED_File_association &provide_inspection_UED_file_association
      (const Inspection &inspection)                                 provision_;
   UED_File_association &provide_tally_UED_file_association
      (const Examination::Inspection_tally_store &inspection_tally)  provision_;
};
//_class Recorder_UED_______________________________________________2016-06-14_/
extern Recorder_UED *UED_recorder_global;
extern Recorder_UED &provide_global_UED_recorder
   (const CORN::OS::Directory_name  &output_directory);                          //171114
extern Recorder_UED &ref_global_UED_recorder();
//______________________________________________________________________________
} // namespace CS
#endif

