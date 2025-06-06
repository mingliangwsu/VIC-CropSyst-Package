#  include "corn/container/SDF_list.h"
#include "corn/string/strconv.hpp"
#include <ctype.h>
namespace CORN {
//______________________________________________________________________________
SDF_Cowl::SDF_Cowl(std::string &SDF_string_ref_,bool _space_is_a_delimiter)
: SDF_string_ref(SDF_string_ref_)
, updated(false)
, space_is_a_delimiter(_space_is_a_delimiter)                                    //090901
{  //update();
}
//_SDF_Cowl:constructor_____________________________________________2019-11-01_/
SDF_Clad::SDF_Clad(const char *SDF_string_,bool space_is_a_delimiter_)
: SDF_Cowl(SDF_string,space_is_a_delimiter_)
, SDF_string(SDF_string_)
{  SDF_string.assign(SDF_string_ ? SDF_string_ : "");
   update();
}
//_SDF_Clad:constructor_____________________________________________2019-11-01_/
SDF_Clad::SDF_Clad()
: SDF_Cowl(SDF_string)
, SDF_string("")
{  update();
}
//_SDF_Clad:constructor_____________________________________________2019-11-01_/
void SDF_Cowl::update()
{
   //Parse string and create SDF_items append to list                System
   std::string item_value;                                                       //040909
   char cstr[5]; cstr[1] = 0;
   bool in_quote = false;
   bool started_value = false;
   bool in_unquoted_string = false;                                              //041003
   const char *SDF_list_string = SDF_string_ref.c_str();
   for (int i = 0; i < (int)SDF_string_ref.length(); i++)
   {
      char ci = SDF_list_string[i];
      char ci1 = SDF_list_string[i+1];
      cstr[0] = ci;
      switch (ci)
      {  case '\"'  :
         { if (!in_quote)
            {  in_quote = true;
               started_value = true;
            } else
            {  if (ci1 == '\"')
               {  // two quote marks is a quote character
                  item_value.append(cstr);
                  i += 1;
               } else
               {  // we reached the end of the string
                  append(new Item_string(item_value.c_str()));
                  item_value.assign("");
                  if (ci1 == ',') i+=1; // take care of separating comma         //040929
                  started_value = false;
                  in_quote = false;
                     in_unquoted_string = false;                                 //041003
               }
            }
         } break;
         case ' ': case ',' :
         { if (in_quote )
            {  // We are inside a quote so this is part of the string
               item_value.append(cstr);
            } else
            {  bool delimiter = (space_is_a_delimiter && (ci == ' ')) || ci == ','; //090901
               if (started_value && delimiter)
               {  append(new Item_string(item_value.c_str()));
                  item_value.assign("");
                  started_value = false;
                  in_unquoted_string = false;                                    //041003
               } else
                  if (ci == ',')
                  {  // we encountered a successive comma, so we append an empty string
                     append(new Item_string (""));
                     in_unquoted_string = false;                                 //041003
                 }
                 else  // Special case when we don't treat spaces as delimiter
                    if (!space_is_a_delimiter)                                   //200514
                       item_value.append(cstr);                                  //090901
            }
         } break;
         default:
            if (!in_quote) in_unquoted_string = true;                            //041003
            item_value.append(cstr);
            started_value = true;
         break;
      }
   }
   if (item_value.length()) append
      (new Item_string(item_value.c_str()));                                     //171125
   updated = true;
}
//_update______________________________________________________________________/
const char *SDF_Cowl::get_cstr() const
{  SDF_string_ref.assign("");
   FOR_EACH(item,Item_string,each_item)
     append_SDF_to(*item,SDF_string_ref,true);                                   //171125_130919
   FOR_EACH_END(each_item)
   return SDF_string_ref.c_str();
}
//_get_cstr____________________________________________________________________/
std::string &SDF_Cowl::get_SDF_for_data_record_expect()
{  get_cstr(); // This should ensure SDF_list_string is updated for the write
   return SDF_string_ref;
}
//_get_SDF_for_data_record_expect___________________________________2004-09-09_/
}//_namespace CORN_____________________________________________________________/
