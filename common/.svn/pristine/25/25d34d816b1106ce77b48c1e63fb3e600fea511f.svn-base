#ifndef Geolocation_itemH
#define Geolocation_itemH
#include "common/geodesy/geolocation.h"
#include "corn/string/strconv.hpp"

// This class is deprecated, it is being replaced with Coordinate                //180425

namespace geodesy {                                                              //190924
//______________________________________________________________________________
class Geolocation_item
: public implements_ CORN::Item
, public extends_ Geolocation
{
public:
   Geolocation_item();                                                           //130529
   inline Geolocation_item(const Geolocation &copy_from)
      : CORN::Item()
      , Geolocation(copy_from)
      {}
   inline virtual bool is_key_string(const std::string &key)        affirmation_ //180820
      { return station_ID_code == key; }
   virtual const char *label_string(std::string &buffer)                  const; //170423
   virtual int compare(const CORN::Item &other_item)                      const; //200401
};
//_Geolocation_item____________________________________________________________/
class Geocoordinate_item
: public implements_ CORN::Item
, public extends_ Geocoordinate
{
   contribute_ std::string key_obsolete; // this is only temporary until I replace container get_key to take the buffer
public:
   Geocoordinate_item();                                                         //130529
   inline Geocoordinate_item(const Geocoordinate &copy_from)
      : CORN::Item()
      , Geocoordinate(copy_from)
      {}
   inline Geocoordinate_item(float32 latitude,float32 longitude)                 //170831
      : CORN::Item()
      , Geocoordinate(latitude,longitude)
      {}
   inline Geocoordinate_item(const std::string &from_string)                     //171103
      : CORN::Item()
      , Geocoordinate(from_string)
      {}
   inline Geocoordinate_item(const std::wstring &from_string)                    //171103
      : CORN::Item()
      , Geocoordinate(CORN::wstring_to_string(from_string,key_obsolete))
      {} // will surmise the format of from_string
   virtual bool is_key_string(const std::string &key)              affirmation_; //180820
   virtual const char *label_string(std::string &buffer)                  const; //170423
   virtual int compare(const CORN::Item &other_item)                      const;
   virtual Geocoordinate_item *clone()                                    const; //170823
};
//_Geocoordinate_item_______________________________________________2014-01-24_/
class Geocoordinate_record
: public extends_ CORN::Data_record
{
 public: //should be protected:
   mutable Geocoordinate_item *geocoordinate_CS;
 public:
   inline Geocoordinate_record
      (const char *primary_section_)
      : CORN::Data_record(primary_section_)
      , geocoordinate_CS(new Geocoordinate_item())                               //190924
      {}
   //NYN Geocoordinate_record(Geocoordinate *geocoordinate);
   // NYN Geocoordinate_record()
   virtual bool expect_structure(bool for_write);
   inline virtual bool know(Geocoordinate_item *geocoordinate_)       cognition_
      {
         // I think geocoordinate_CS needs to be deleted (replacing)
         delete geocoordinate_CS;                                                //191007
         geocoordinate_CS = geocoordinate_;
         return true;
      }
   inline virtual Geocoordinate_item *relinquish_item()
   {  Geocoordinate_item *item = geocoordinate_CS;
      geocoordinate_CS = 0;
      return item;
   }
};
//_Geocoordinate_record_____________________________________________2019-09-23_/
}//_namespace geodesy___________________________________________________________
#endif

