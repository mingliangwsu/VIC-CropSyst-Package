#ifndef scenario_export_engineH
#define scenario_export_engineH
//______________________________________________________________________________
#include "corn/OS/directory_entry_name_concrete.h"
#include "corn/container/unilist.h"
#include "corn/container/text_list.h"
#include "CS_suite/file_system/CS_context_discovery.h"
#include <fstream.h>
namespace CropSyst
{
//______________________________________________________________________________
class Scenario_export_engine
{protected: // Inputs
   CORN::Unidirectional_list        selected_scenario_filenames_fully_qualified;
 protected:
   std::ofstream target;
   CS::Context_discovery context;                                                //190120
 public:
   //___________________________________________________________________________
   class Arguments
   {public:
      CORN::OS::Directory_name_concrete   *source_directory_name;
         // default to CWD
         // These will typically be the project directory name, or
         // a project's Scenarios, or an individual scenario directory name
      CORN::OS::File_name_concrete        *target_filename;
         // By default the generated target filename will
         // have the same name as the
      CORN::Unidirectional_list           identified_scenario_filenames;         //150427
      std::wstring target_file_extension;
    public:
      Arguments(int argc, const char *argv[],const wchar_t *_target_file_ext);
      Arguments                                                                  //150428
         (const CORN::OS::Directory_entry_name &source_directory_name
         ,const CORN::OS::File_name            &target_filename_qual
         ,const wchar_t                        *target_file_extension);
         // Constructor used when calling from CS explorer
      bool know_selected_scenarios_unqual
         (const CORN::Text_list &selected_scenario_names);

      const std::wstring compose()                                        const;
    private:
      bool take_arg_DEN
         (CORN::OS::Directory_entry_name_concrete *DE_a)          construction_;
      bool complete_construction()                                construction_;
   };
   //___________________________________________________________________________
 public:
   modifiable_ Arguments &arguments;
   contribute_ nat32 scenario_count;
   Scenario_export_engine
      (modifiable_  Arguments &_arguments);
   virtual bool initialize()                                    initialization_;
   bool include_scenario(const std::string &scenario_name_unqual) modification_;
   bool validate_source_scenarios_directory()                      affirmation_;
   virtual nat16 export_scenarios()                               modification_;
      // returns the number of scenarios exported
   virtual bool export_scenario
      (const CORN::OS::File_name &source_filename)              modification_=0;
 protected:
   virtual const char *get_target_filename_prefix()                     const=0;
   virtual const char *get_target_filename_extension()                  const=0;
   nat16 select_all_scenarios()                                   modification_;
   virtual CORN::OS::File_name *render_source_filename
      (const CORN::OS::Directory_name &scenario_dir)               rendition_=0;
   virtual CORN::OS::File_name *render_target_filename()             rendition_;
};
//_Scenario_export_engine___________________________________________2014-01-11_/
}//_namespace_CropSyst_________________________________________________________/
#endif
