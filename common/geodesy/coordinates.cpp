#include "coordinates.h"
#include "corn/OS/file_system_engine.h"
#include "common/geodesy/geolocation_item.h"
#include "corn/seclusion.h"
#include <assert.h>

using namespace CORN;
using namespace CORN::OS;
using namespace CORN::OS::FS;

CORN::OS::FS::Path_discovery_struct point_file_discovery_raw
= { file_bit,{(ascendent),{to_extreme,from_CWD}},{0,0},"*.DBF"};


namespace geodesy
{
//______________________________________________________________________________
Coordinates::Arguments::Arguments
(CORN::Data_record &data_rec_)                                                   //191014
: data_rec                                                           (data_rec_) //191014
//200810 , geocoordinates_filename                                                    (0)
, ID_fieldname                                                            ("ID") //171111
//NYI , predefined_extent()
, cell_dimension                                                              ()
, precision_latitude                                                         (2)
, precision_longitude                                                        (2)
, CS_geocoordinates_dir_raw                                                   () //191103
, zone                                                                       (0) //180816
, zone_designator                                                            (0) //180816
, geocoordinates_file_discovery                                              (0) //200810
{}
//_Coordinates::Arguments:constructor_______________________________2016-08-25_/
/*200810
Coordinates::Arguments::~Arguments()
{  delete geocoordinates_filename;
}
//_Coordinates::Arguments:destructor________________________________2016-08-25_/
*/
bool Coordinates::Arguments::get_start()
{  bool started = true;
   geocoordinates_file_discovery = new Path_discovery_cowl(point_file_discovery_raw,"points");
   geocoordinates_file_discovery->seclusion.include_pattern("*.txt");
   geocoordinates_file_discovery->seclusion.include_pattern("*.dat");
   geocoordinates_file_discovery->seclusion.include_pattern("*.csv");
   if (global_arguments)
      global_arguments->path_discoveries.append(geocoordinates_file_discovery);
   return started;
}
//_get_start________________________________________________________2020-08-10_/
bool Coordinates::Arguments::expect_structure(bool for_write)
{  bool expected
   =  data_rec.expect_nat8("--precision_latitude" ,precision_latitude,10)
   && data_rec.expect_nat8("--precision_longitude",precision_longitude,10)
   && data_rec.expect_string("--ID",ID_fieldname)
   && data_rec.expect_string("--UTMzone",UTMzone)
   && data_rec.expect_string("--CS_geocoordinates",CS_geocoordinates_dir_raw);
   //&& data_rec.expect_directory_entry_name("--CS_geocoordinates",CS_geocoordinates_dir);

/*NYI need Extent using Coordinate::Item
   && data_rec.epxect_string("--extent",predefined_extent);                      //180318
*/
   ;
   return true;
}
//_expect_structure_________________________________________________2019-10-14_/
bool Coordinates::Arguments::take(CORN::Item_string *arg)
{  bool taken = false;
   if (arg)
   {  if (arg->find("km") != std::string::npos)
      {  std::string length_km_str((*arg),0,arg->length()-2);
         cell_dimension.length_km = CORN::cstr_to_float32(length_km_str.c_str());
         taken = cell_dimension.length_km > 0;
      }
      else if (arg->find("m") != std::string::npos)
      {  std::string length_m_str((*arg),0,arg->length()-1);
         float32 length_m = CORN::cstr_to_float32(length_m_str.c_str());
         cell_dimension.length_km = m_to_km(length_m);
         taken = length_m > 0;
      }
/*200810 now using Path_discovery
      else
      {
         // continue here use new Path_discovery
         if (!geocoordinates_filename)
         {  geocoordinates_filename = new CORN::OS::File_name_concrete (*arg);
            if (CORN::OS::file_system_engine.exists(*geocoordinates_filename))
               taken = true;
            else
            {  // It is not a filename so can't be geocoorder_filename
               delete geocoordinates_filename;
               geocoordinates_filename = 0;
            }
         }
      }
*/
      if (taken) { delete arg; arg = 0;}
   }
   return taken;
}
//_take_____________________________________________________________2020-02-15_/

bool Coordinates::Arguments::get_end()
{  bool ended = true;                                                            //191014

   /* conceptual provide option to reference coordinate to
      centroid or SW NW  NE WE corner of grid cell
      (Note that ESRI grids are SW corner oriented
      currently always centroid
   else if (paramstring == "centroid")
   {  centroid = true
      recognized = true;
   }
   */
   /* 100810 obsolete now using Path_discovery
   if (!geocoordinates_filename) // discover
   {
      CORN::Seclusion recognizable_point_file_types(true);
      recognizable_point_file_types.include_pattern("*.DBF");
      recognizable_point_file_types.include_pattern("*.dat");
      recognizable_point_file_types.include_pattern("*.txt");
      recognizable_point_file_types.include_pattern("*.csv");               //200331
      assert(CORN::global_arguments);                                            //200331
      geocoordinates_filename = dynamic_cast<CORN::OS::Directory_entry_name *>
         (CORN::global_arguments->discover_one
            (recognizable_point_file_types
            ,CORN::OS::file_entry
            ,CORN::OS::File_system::superdirectory_recursion_inclusive));
   }
   */
   File_name *geocoordinates_filename = geocoordinates_file_discovery->provide_primary(); //100810
   if (!geocoordinates_filename)
   {
      if (CORN::is_zero<float32>(cell_dimension.length_km))
      {  std::cerr << "A points file or cell dimension must be specified" << std::endl;
         ended = false;
      }
   }
   if (!UTMzone.empty())
   {  std::string zone_str(UTMzone,0,3);
      zone = CORN::cstr_to_int16(zone_str.c_str(),10);
      zone_designator = UTMzone[3];
      ended &= true;
   }
   return ended;                                                                 //161023
}
//_Coordinates::Arguments::get_end_____________________________________________/
bool Coordinates::Arguments::is_specified_geocoordinates()          affirmation_
{  return
      //200810 geocoordinates_filename
      // soil points file specified
      geocoordinates_file_discovery->provide_primary()   //200810
      || specific_geocoordinates.count();
}
//_Coordinates::Arguments::is_specified_geocoordinates______________2019-09-25_/
Coordinates::Item::Item()
: CORN::Item()
, geocoordinate(0)
, UTM_coordinate(0)
{}
//______________________________________________________________________________
Coordinates::Item::Item(Geocoordinate     *geocoordinate_)
: CORN::Item()
, geocoordinate(geocoordinate_)
, UTM_coordinate(0)
{}
//______________________________________________________________________________
Coordinates::Item::Item(UTM_Coordinate    *UTM_coordinate_)
: CORN::Item()
, geocoordinate(0)
, UTM_coordinate(UTM_coordinate_)
{}
//______________________________________________________________________________
Geocoordinate &Coordinates::Item::geo()                               provision_
{  if (!geocoordinate)
      geocoordinate = new Geocoordinate();
   return *geocoordinate;
}
//_Coordinate::Item::geo________________________________________________________
UTM_Coordinate &Coordinates::Item::UTM()                              provision_
{  if (!UTM_coordinate)
      UTM_coordinate = new UTM_Coordinate();
   return *UTM_coordinate;
}
//_Coordinate::Item::UTM________________________________________________________
bool Coordinates::Item::is_in_northern_hemisphere()                 affirmation_
{  return
      geocoordinate
      ? geocoordinate->is_in_northern_hemisphere()
      : UTM_coordinate
         ? UTM_coordinate->is_in_northern_hemisphere()
         : false;
}
//_Coordinate::Item::is_in_northern_hemisphere__________________________________
}//_namespace geodesy__________________________________________________________/

