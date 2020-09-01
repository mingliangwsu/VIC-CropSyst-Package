#ifndef coordinatesH
#define coordinatesH
#include "corn/container/item.h"
#include "geodesy/geocoordinate.h"
#include "geodesy/UTM.h"
#include "corn/container/bilist.h"
#include "corn/application/arguments.h"
namespace geodesy
{
//______________________________________________________________________________
// Need to implement an Extent that uses Coordinate::Item
//______________________________________________________________________________
class Coordinates
: public extends_ CORN::Bidirectional_list
{
 public:
   class Item
   : public implements_ CORN::Item
   {
    protected: // owned
      provided_ Geocoordinate     *geocoordinate;
      provided_ UTM_Coordinate    *UTM_coordinate;
    public:
      Item();
      Item(Geocoordinate     *geocoordinate_);
      Item(UTM_Coordinate    *UTM_coordinate_);
    public:
      Geocoordinate     &geo()                                       provision_;
      UTM_Coordinate    &UTM()                                       provision_;
      virtual bool is_in_northern_hemisphere()                     affirmation_;
   };
   //_Item__________________________________________________________2018-04-25_/
   struct Cell_dimension    // need name  Cell_topology?  Cell_dimensions?
   {
      float32       length_km; // currently assuming cell is square
      bool          centroid;
    public:
      inline Cell_dimension()
         : length_km(0)
         , centroid(true)
         {}
      inline Cell_dimension(const Cell_dimension &from_copy)
         : length_km(from_copy.length_km)
         , centroid (from_copy.centroid)
         {}
   };
   //_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
   class Arguments
   {
    protected:
      CORN::Data_record &data_rec;                                               //191014
    public:
      // Could be DBF or DAT or TXT currently expecting only one filename
      /*200810 now using geocoordinates_file_discovery
      CORN::OS::File_name *geocoordinates_filename;
      */
      std::string    ID_fieldname;                                               //171111
      Cell_dimension cell_dimension;
      nat8           precision_latitude;                                         //170829
      nat8           precision_longitude;                                        //170829
      std::string CS_geocoordinates_dir_raw;                                     //180318
      mutable CORN::Unidirectional_list specific_geocoordinates;                 //180318
      sint16 zone;                                                               //180816
      char   zone_designator;                                                    //180816

      CORN::OS::FS::Path_discovery_cowl *geocoordinates_file_discovery;                   //200810
         // given to global_arguments.path_discoveries
    private:
      std::string UTMzone;                                                       //191014
         // used only when setup args options
    public:
      Arguments(CORN::Data_record &data_rec_);                                   //191014
      /*200810
      virtual ~Arguments();
      */
      virtual bool is_specified_geocoordinates()                   affirmation_; //190925
      virtual bool expect_structure(bool for_write);                             //191014
      virtual bool get_start();                                                  //200810
      virtual bool get_end();                                                    //161023
      virtual bool take(CORN::Item_string *arg)                  appropriation_; //200216
   };
   //_Arguments________________________________________________________________/
 public:
   inline Coordinates(const Arguments *arguments_=0)
      : CORN::Bidirectional_list(true) // must be unique
/*NYI
      , arguments(arguments_)
      , netCDF_grid_available(false)
      , extent(0)
      , geocoord_fields_dBase (0)
*/
      {}
   inline ~Coordinates()
      {
         //NYI         delete geocoord_fields_dBase;
      }
};
//_Coordinates______________________________________________________2018-04-15_/
}//_namespace geodesy__________________________________________________________/
#endif

