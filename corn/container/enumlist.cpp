#  ifndef enumlistH
#     include "corn/container/enumlist.h"
#  endif
#  ifndef strconvHPP
#     include <stdlib.h>
#     include "corn/string/strconv.hpp"
#  endif

namespace CORN {
//______________________________________________________________________________
Item *Enumeration_list::find(nat32 search_key)                             const
{  FIND_FOR_EACH(found_enumeration,enumeration,Item,true,enum_finder)            //030710
   {  nat32 enumeration_key = enumeration->get_key_nat32();                      //060511
      if (search_key == enumeration_key)
         found_enumeration = enumeration;
   } FOR_EACH_END(enum_finder)                                                   //030710
   return found_enumeration;
}
//_find_____________________________________________________________2002-03-19_/
nat32 Enumeration_list::find_index(nat32 search_key)                       const
{  nat32 found_index = std::string::npos;
   nat32 index = 0;
   FIND_FOR_EACH(found_enumeration,enumeration,Item,true,enum_finder)
   {  nat32 enumeration_key = enumeration->get_key_nat32();
      if (search_key == enumeration_key)
      {  found_enumeration = enumeration;
         found_index = index;
      }
      else index++;
   } FOR_EACH_END(enum_finder)
   return found_index;
}
//_find_index_______________________________________________________2013-01-25_/
nat32 Enumeration_list::remove_all_matching(nat32 search_key) modification_
{  // Remove removes all matching entries from the list
   // and deletes the associated data!
   // Returns true if there was a matching deletion.
   //bool at_least_one_item_removed = false;
   nat32 matching_deletions = 0;
   for (Item *matching_item = find(search_key)
       ;matching_item // search_key
       ;matching_item = find(search_key))
   {  remove(matching_item);
      matching_deletions++;                                                      //140716
   }
   return matching_deletions;                                                    //140716
}
//_remove_all_matching______________________________________________2002-03-19_/
Enumeration_list::Enumeration_list(bool unique_keys_)                            //160615
: Bidirectional_list(unique_keys_)
{}
//_Enumeration_list:constructor_____________________________________2005-10-10_/
Enumeration_list::Enumeration_list()
: Bidirectional_list(false)
{}
//_Enumeration_list:constructor__________________________2016-07-13_2005-10-10_/
Enumeration_list::~Enumeration_list() {}
//__Enumeration_list:destructor_____________________________________2008-11-25_/
}//_namespace CORN_____________________________________________________________/

