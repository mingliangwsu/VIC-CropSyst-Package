#ifndef SDF_listH
#define SDF_listH
#include "corn/container/text_list.h"

/*
This container is used for working with
System Data Format (SDF) lists.

This class will probably become deprecated when I add SDF rendering to Text_list
Use instead use Text_list

*/
namespace CORN {
//191101 class SDF_List; // forward declaration for friendship
//______________________________________________________________________________
class SDF_Cowl : public Text_list
{
 protected:
   bool updated;
   /*mutable*/ std::string &SDF_string_ref;                                          //040909
      // This is only a buffer, data is stored as SDF_items
      // actually this could be a cowl idiom
   bool space_is_a_delimiter;                                                    //090901
 public:
   SDF_Cowl(std::string &SDF_string_ref_,bool space_is_a_delimiter = true);
   inline virtual ~SDF_Cowl()                                                 {}
   const char *get_cstr() const;
   void update();
   // Update refills the list based on the value buffer
   // This is used after the SDF_list_string has been read by data_record expect.
   //
   //
   std::string &get_SDF_for_data_record_expect();                                //040909
      //191101 probably could become obsolete because can
      // just use cowl and clad idiom

};
//_SDF_Cowl_________________________________________________________2019-11-01_/
class SDF_Clad
: public extends_ SDF_Cowl
{
 private:
   std::string SDF_string;
 public:
   SDF_Clad();                                                                   //200430
   SDF_Clad(const char *SDF_string_,bool space_is_a_delimiter = true);

};
//_SDF_Clad_________________________________________________________2019-11-01_/
typedef SDF_Clad SDF_List;                                                       //191101
//______________________________________________________________________________
}//_namespace CORN_____________________________________________________________/
#endif
// SDF_List

