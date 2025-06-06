#  include "common/weather/GEN_DAT_filename.h"
#  include <stdlib.h>
#  include "corn/math/compare.hpp"
#  include "string.h"
#  include "corn/string/strconv.h"
//______________________________________________________________________________
void GEN_DAT_UED_Filename::set_year(Year year)
{  char year_str[10];
   CORN::int16_to_cstr(year,year_str,10);
   // 970221  Need to pad year_str with leading 0's
   while (strlen(year_str) < 4)
   {
     for (int i = 3; i > 0; i--)
        year_str[i] = year_str[i-1];
     year_str[0] = '0';
   }
   year_str[4] = 0;
   std::wstring filenameonly(get_name());                                        //050207
   if (filenameonly.length() > 4)
      filenameonly.resize(filenameonly.length() - strlen(year_str));             //971018
   CORN::append_ASCIIZ_to_wstring(year_str,filenameonly);
   name = filenameonly;                                                          //131121
   CORN::OS::File_name_concrete new_name(get_parent_directory_name_qualified(),name,get_extension());//170113
   set_DEN(new_name);                                                            //170113
 }
//_set_year_________________________________________________________2000-01-24_/
void GEN_DAT_UED_Filename::set_prefix_year(const std::string &prefix,Year year)
{  char year_str[10];
   CORN::int16_to_cstr(year,year_str,10);
   std::string prefix_year(prefix);
   prefix_year.append("1999");
   CORN::string_to_wstring(prefix_year,name);                                    //131121
   construction_compose();                                                       //140422
   set_year(year);
}
//_set_prefix_year__________________________________________________2000-01-24_/
Year GEN_DAT_UED_Filename::get_year()
{  std::string filename_only;
   append_components_to_string
      (filename_only,CORN::OS::Directory_entry_name::include_name);              //130322
   int start_pos = CORN::must_be_greater_or_equal_to<int>(filename_only.length()-4,0); //040427
   start_pos = CORN::must_be_0_or_greater<int>(start_pos);                       //040427
   std::string year_only(filename_only.substr(start_pos));
   Year file_year = (Year)atoi(year_only.c_str());
   return file_year;
}
//_get_year_________________________________________________________2000-01-24_/
const std::wstring &GEN_DAT_UED_Filename::get_prefix(std::wstring &prefix_return) //170111
{  std::wstring filename_only;
   append_components_to(filename_only,CORN::OS::Directory_entry_name::include_name); // 130322
   int prefix_length = filename_only.length() -4 ;                               //061206
   prefix_return.assign(filename_only.substr(0,prefix_length));                  //061206
   return prefix_return;
}
//_get_prefix_______________________________________________________2000-01-24_
#ifdef NYI
void GEN_DAT_UED_Filename::set_prefix(const std::string &prefix_code)
{  char filenameonly[40];
   char year_str[10];
   CORN_int16_to_str(year,year_str,10);
   // 970221  Need to pad year_str with leading 0's
   while (strlen(year_str) < 4)
   {  for (int i = 3; i > 0; i--)
         year_str[i] = year_str[i-1];
      year_str[0] = '0';
   }
   year_str[4] = 0;
   std::string filenameonly;  append_name_to(filenameonly);                      //120724
   if (filenameonly.length() > 4)
      filenameonly[strlen(filenameonly) - strlen(year_str)] = 0;                 //971018
   strcat(filenameonly,year_str);
   set_file(filenameonly);
}
//_set_prefix_______________________________________________________2000-01-24_/
#endif
void GEN_DAT_UED_Filename::set_ext(const char *new_ext)
{
   CORN::OS::File_name_concrete with_new_ext(*this,CORN::OS::Extension(new_ext));
   set_DEN(with_new_ext);
}
//_set_ext__________________________________________________________2015-03-15_/
