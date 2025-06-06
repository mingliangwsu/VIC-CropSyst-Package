#ifndef CS_simulation_element_pluginH
#define CS_simulation_element_pluginH
#include "CS_suite/simulation/CS_simulation_element.h"
#include "CS_suite/observation/CS_inspector.h"
#include "corn/OS/directory_entry_name.h"
#if (defined(_WIN32)  || defined(_WIN64)|| defined(_Windows))
#include <windows.h>
#endif

// The plugin system is currently intended for version 5
// but I may add it also to V4

namespace CS {
//______________________________________________________________________________
typedef bool (*bool_func_parameterless)(void);
//______________________________________________________________________________
class Simulation_element_plugin
: public implements_ Simulation_element_abstract
{
   const CORN::Container *inspectors;
   const CORN::OS::File_name    &DL_filename;
 public: // construction
   Simulation_element_plugin
      (const CORN::OS::File_name &DL_filename
      ,const CORN::Container     *inspectors
      ,const CORN::date32        &simdate_raw);                                    //170524
      //170524 ,const CORN::Date &_today);
   virtual ~Simulation_element_plugin();

   bool load()                                                  initialization_;
   virtual bool is_valid()                                         affirmation_;
      // Should only be called after initialize.

   // Note that the unqualified filename could be used as the key
 public: // Simulation_element implementations
   virtual bool initialize()                                    initialization_;
   virtual bool start()                                           modification_;
   virtual bool start_year()                                      modification_;
   virtual bool start_day()                                       modification_;
   virtual bool process_day()                                     modification_;
   virtual bool end_day()                                         modification_;
   virtual bool end_year()                                        modification_;
   virtual bool stop()                                            modification_;
 private:
   bool_func_parameterless DL_initialize;
   bool_func_parameterless DL_start;
   bool_func_parameterless DL_start_year;
   bool_func_parameterless DL_start_day;
   bool_func_parameterless DL_process_day;
   bool_func_parameterless DL_end_day;
   bool_func_parameterless DL_end_year;
   bool_func_parameterless DL_stop;
 private:
   #if (defined(_WIN32)  || defined(_WIN64))
   HMODULE library_handle;
   #endif
   #ifdef __unix
   void *  library_handle;
   #endif
};
//_2015-09-08_RLN_______________________________________________________________
} // namespace CS
#endif

