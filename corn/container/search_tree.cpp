
#error obsolete

containers now take Item which provides comparison methods
and handles both keys (strings or nat32) or object comparison


#  include <string.h>
#  include "corn/container/search_tree.h"
#  include "corn/container/enumlist.h"
namespace CORN {
//______________________________________________________________________________
Enumeration *Enumeration_search_tree::find(nat32 search_key)               const
{  Comparable_enumeration comp_enum(search_key);
   return (Enumeration *)find_comparable(comp_enum);
}
//_find_____________________________________________________________2002-03-19_/
bool Enumeration_search_tree::remove_all_matching(nat32 search_key)
{  /* Remove removes all matching entries from the list and deletes
      the associated data! \return true if there was a matching deletion. */
   bool at_least_one_item_removed = false;
   for (Enumeration *matching_item = find(search_key)
       ;matching_item
       ;matching_item = find(search_key))
   {  remove(matching_item);
      at_least_one_item_removed = true;
   }
   return at_least_one_item_removed;
}
//_remove_all_matching______________________________________________2002-03-19_
class Comparable_association : public Item
{ private :
   std::string value;                                                            //140716
   bool case_sensitive;
 public:
   inline Comparable_association(const char *_value,bool _case_sensitive)
      :Item()
      ,case_sensitive(_case_sensitive)
      , value(_value)                                                            //140716
      {}                                                                         //140716
   inline Comparable_association()                                               //140716
      :Item()
      ,case_sensitive(true)
      , value()
      {  }
   inline virtual bool is_key_string(const std::string &key)        affirmation_ //180820
      { return value.compare(key) == 0; }

/*180820  was probably only used for find_cstr now using is_key
      
   inline virtual const char *get_key()         const { return value.c_str();}
*/   
   virtual bool is_case_sensitive()             const { return case_sensitive; }


   //180813 NYN inline virtual const std::string &append_to_string(std::string &buffer) const { return buffer; } //180813
      //not currently used
};
//_Comparable_association___________________________________________2005-11-01_/
Item *Association_search_tree::find(const char * search_key,bool case_sensitive) const
{  Comparable_association comp_enum(search_key,case_sensitive);
   return (Item *)find_comparable(comp_enum);
}
//_find_____________________________________________________________2002-03-19_/
}//_namespace CORN_____________________________________________________________/
