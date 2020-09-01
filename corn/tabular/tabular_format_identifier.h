#ifndef tabular_fomrat_identiferH
#define tabular_fomrat_identiferH

#include <corn/std/std_fstream.h>
#include <corn/tabular/tabular_format.h>
#include <AdvGridWorkbook.hpp>

namespace CORN
{
//______________________________________________________________________________
class Text_format_auto_identifier
{  STD_NS ifstream  text_file;
   Tabular_file_format/*200220 Common_tabular_file__format*/  &tabular_file_format; //

      200220 probably only needs to be Delineation_tabular


public:


   Text_format_auto_identifier
      (modifiable_ /*not_const*/ Common_tabular_file__format  &_tabular_file_format
      ,const char *sample_filename);
   bool identify();
};
//_Text_format_auto_identifier_________________________________________________/
#ifdef NYI
class dBase_format_auto_identifier
{  Tabular_file_format/*200220 Common_tabular_file__format*/  &tabular_file_format; //
public:
   dBase_format_auto_identifier
      (not_const Tabular_file_format/*200220 Common_tabular_file__format*/  &_tabular_file_format
      ,const char *sample_filename);
   bool identify();
};
//_dBase_format_auto_identifier________________________________________________/
#endif
class Excel_format_auto_identifier
{  Tabular_file_format/*200220 Common_tabular_file__format*/  &tabular_file_format; //
public:
   Excel_format_auto_identifier
      (modifiable_  Tabular_file_format/*200220 Common_tabular_file__format*/  &_tabular_file_format
      ,const char *sample_filename);
   bool identify(TAdvGridWorkbook *sample_workbook);
};
//_Excel_format_auto_identifier________________________________________________/
class Tabular_format_identifier
{private:
   Format_file_type identify_file_type_by_extension(const char *_sample_filename) const;
   // call identify_file_type_by_extension first
   // if it can't identify or it is ambiguous extension, then call identify_file_type which will preread
   Format_file_type identify_file_type(STD_NS ifstream &datafile) const;
 public:
#ifdef NYI
   bool identify
      (not_const Tabular_file_format/*200220 Common_tabular_file__format*/  &tabular_file_format
      ,const char *sample_filename
      ,Tsample_form *sample_form
      ) const ;
#endif
};
//_Tabular_format_identifier___________________________________________________/
}//_namespace_CORN_____________________________________________________________/
#endif

