#include "common/geodesy/geolocation_item.h"
namespace geodesy {                                                              //190924
//______________________________________________________________________________
Geolocation_item::Geolocation_item()
: CORN::Item()
{}
//_Geolocation_item:constructor_____________________________________2013-05-29_/
const char *Geolocation_item::label_string(std::string &buffer) const
{  return Geolocation::label_string(buffer);
}
//_label_string_____________________________________________________2017-04-23_/
int Geolocation_item::compare(const CORN::Item &other_item)              const
{
   int compare = 0;
   const Geolocation_item *other_geocoord
      = dynamic_cast<const Geolocation_item *> (&other_item);
   if (other_geocoord)
   {  compare = Geolocation::compare(*other_geocoord);
   } else
      compare = -1;
   return compare;
}
//_Geocoordinate_item::compare______________________________________2020-04-01_/
Geocoordinate_item::Geocoordinate_item()
: CORN::Item()
, Geocoordinate()
{}
//_Geocoordinate_item:constructor______________________________________________/
bool Geocoordinate_item::is_key_string(const std::string &key)      affirmation_
{  std::string label; Geocoordinate::label_string(label);
   return label == key;
}
//_is_key___________________________________________________________2018-08-20_/
const char *Geocoordinate_item::label_string(std::string &buffer)          const
{  return Geocoordinate::label_string(buffer);
}
//_is_key_string____________________________________________________2017-04-23_/
int Geocoordinate_item::compare(const CORN::Item &other_item)              const
{
   int compare = 0;
   const Geocoordinate_item *other_geocoord
      = dynamic_cast<const Geocoordinate_item *> (&other_item);
   if (other_geocoord)
   {  compare = Geocoordinate::compare(*other_geocoord);
   } else
      compare = -1;
   return compare;
}
//_compare__________________________________________________________2016-09-02_/
Geocoordinate_item *Geocoordinate_item::clone()                            const
{  return new Geocoordinate_item(*this);
}
//_clone____________________________________________________________2017-08-23_/
bool Geocoordinate_record::expect_structure(bool for_write)
{  bool expected
   =  expect_float32("latitude"     , geocoordinate_CS->latitude_dec_deg)
   && expect_float32("longitude"    , geocoordinate_CS->longitude_dec_deg)
   && expect_float32("elevation"    , geocoordinate_CS->elevation)
   && expect_string ("coordinate"   , geocoordinate_CS->coordinate)
   ;
   if (!for_write)
   {  // conceptual provided mechanize for derived classes to specify default labels
      expect_string ("COORDINATE"   , geocoordinate_CS->coordinate);
      expect_string ("ISO6709"      , geocoordinate_CS->coordinate);

      expect_float32("LATITUDE"     , geocoordinate_CS->latitude_dec_deg);
      expect_float32("LAT"          , geocoordinate_CS->latitude_dec_deg);
      expect_float32("lat"          , geocoordinate_CS->latitude_dec_deg);
      expect_float32("LAT_Y"        , geocoordinate_CS->latitude_dec_deg); // QGIS              //190919
      expect_float32("YCOORD"       , geocoordinate_CS->latitude_dec_deg); // QGIS (or GRASS) uses this
      expect_float32("Y_COORD"      , geocoordinate_CS->latitude_dec_deg); // QGIS (or GRASS) uses this
      expect_float32("Y"            , geocoordinate_CS->latitude_dec_deg); // QGIS (or GRASS) uses this

      expect_float32("LONGITUDE"    , geocoordinate_CS->longitude_dec_deg);
      expect_float32("LONG"         , geocoordinate_CS->longitude_dec_deg);
      expect_float32("long"         , geocoordinate_CS->longitude_dec_deg);
      expect_float32("LONG_X"       , geocoordinate_CS->longitude_dec_deg); // QGIS 190919
      expect_float32("LON"          , geocoordinate_CS->longitude_dec_deg);
      expect_float32("lon"          , geocoordinate_CS->longitude_dec_deg);
      expect_float32("XCOORD"       , geocoordinate_CS->longitude_dec_deg); // QGIS (or GRASS) uses this
      expect_float32("X_COORD"      , geocoordinate_CS->longitude_dec_deg); // QGIS (or GRASS) uses this
      expect_float32("X"            , geocoordinate_CS->longitude_dec_deg); // QGIS (or GRASS) uses this

      expect_float32("ELEVATION"    , geocoordinate_CS->elevation);
      expect_float32("ELEV"         , geocoordinate_CS->elevation);
   }
   return expected && CORN::Data_record::expect_structure(for_write);
}
//_expect_structure_________________________________________________2019-09-23_/
}//_namespace geodesy___________________________________________________________


