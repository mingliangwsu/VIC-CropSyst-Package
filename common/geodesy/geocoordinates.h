#ifndef geocoordinatesH
#define geocoordinatesH

// This class is deprecated, it is being replaced with Coordinates               //180425

#include "corn/container/unilist.h"
#include "corn/container/bilist.h"
#include "corn/OS/directory_entry_name_concrete.h"
#include "common/geodesy/geocoordinate.h"
#include "common/geodesy/geolocation_item.h"
#include "corn/data_source/dbase_datasrc.h"
#include "common/geodesy/coordinates.h"
#include "corn/data_source/datarec.h"

namespace CORN
{
   class Data_table;
}
namespace geodesy
{
//______________________________________________________________________________
class Geocoordinates
: public extends_ geodesy::Coordinates                                           //180525
{
 public:
   //_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
   struct Extent
   {
      float32 north_most; // dec_deg
      float32 south_most; // dec_deg
      float32  west_most; // dec_deg
      float32  east_most; // dec_deg
      Geocoordinate corner1;
      Geocoordinate corner2;
    public:
      inline Extent()
         : north_most(-99999.0)
         , south_most( 99999.0)
         , west_most ( 99999.0)
         , east_most (-99999.0)
         , corner1()
         , corner2()
         {}
      inline Extent(const Extent &from_copy)
         : north_most(from_copy.north_most)
         , south_most(from_copy.south_most)
         , west_most (from_copy.west_most)
         , east_most (from_copy.east_most)
         , corner1(from_copy.corner1)
         , corner2(from_copy.corner2)
         {}
      Extent(const std::string &name);
      Extent(const std::wstring &name);
      Extent(const char *name);
         // These constructor takes an extent name as composed from compose_name()
         // I.e. 29.85N081.56W_29.60N081.44W

      bool set(const std::string &name);
      bool compose_query_NCSS(std::string &query)                         const;
         // appends REST query string to the query conforming to NCSS
         // https://www.unidata.ucar.edu/software/thredds/v5.0/tds/reference/services/NcssGrid.html
      bool compose_name(std::string &name_raw)                            const;
      bool contains_point(const Geocoordinate &geocoord)           affirmation_; //171011
      bool is_valid()                                              affirmation_; //171011
      bool is_single_point()                                       affirmation_; //171030
      bool is_same_latitute_or_longitude()                         affirmation_; //190602
      Extent *render_little_larger()                                 rendition_; //171030
      bool update_corners_from_extremes()                         modification_; //180216
   };
   //_Extent_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ 2017-08-17_/
   class Arguments
      : public extends_ geodesy::Coordinates::Arguments                          //180425
      {
       public:
         std::string    predefined_extent_raw;                                   //190925
            // temporary store until assigned in get end
         Extent         predefined_extent;                                       //170817
       public:
         Arguments(CORN::Data_record &data_rec_);                                //191014
         virtual ~Arguments();
         virtual bool get_start();                                               //200810
         virtual bool expect_structure(bool for_write);                          //190925
         virtual bool take(CORN::Item_string *arg);                              //200212
         virtual bool get_end();                                                 //190925
      };
   //_Arguments_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ __/
 protected:
   const Arguments *arguments; //optional
   cognate_ bool netCDF_grid_available;
   provided_ Extent *extent;
 public:
   inline Geocoordinates(const Arguments *arguments_=0)
      : geodesy::Coordinates(arguments_)                                         //180425
      , arguments(arguments_)
      , netCDF_grid_available(false)
      , extent(0)
      {}
   inline ~Geocoordinates()                                                   {}
   nat32 load();
      // returns the number of (valid) coordinates loaded
   nat32 load_from_DBF(const CORN::OS::File_name &filename_DBF);                 //170809
   nat32 load_from_text(const CORN::OS::File_name &filename_txt);                //170809
   nat32 load_from_table                                                         //170902
      (CORN::Data_table &table
      ,nat32 &records_read
      ,nat32 &records_valid);
      // geocoord_rec is just a buffer record of the type usable by the database
      // so we are not constantly instanciating one (it is merely contributory
      // is is not an output!.
   nat32 load_from_directory(const CORN::OS::Directory_name &dir);               //180105
      // Lists all DENs in the specified directory, and those
      // where the name  (or extension?) is a geocoordinate will be listed
   nat32 load_CS_geocoordinates();                                               //180316
   nat32 compose_from_predefined_extent
      (const Extent         &predefined_extent                                   //170817
      ,const Cell_dimension &cell_dimension);                                    //270817
   inline bool know_netCDF_grid_available(bool is_available)         cognition_
       { netCDF_grid_available = is_available; return is_available; }
   inline bool is_populated()                                      affirmation_
      { return count(); }
   virtual const geodesy::Geocoordinate_item *find_nearest_item_km
      (const Geocoordinate &point
      ,float32 minimum_inclusion_distance = 0.0
      ,float32 maximum_inclusion_distance = 20100.0)                      const; //171103

   virtual const Geocoordinate *find_nearest_km
      (const Geocoordinate &point
      ,float32 minimum_inclusion_distance = 0.0
      ,float32 maximum_inclusion_distance = 20100.0)                      const; //170612
      ///< The optional arguments are used to define a range of distances
      /// for considerations;
      ///< minimum_inclusion_distance exclude points near the reference point,
      /// so 0.0 would include the point itself it is listed.
      /// maximum_inclusion_distance excludes points beyond the range of the
      /// reference point the default includes all points on the Earth.
      /// \return Will always find a point, but may return 0
      /// (I.e. if the list is empty)
   const Extent &provide_extent()                                    provision_; //170817
 protected:
   virtual bool table_has_a_geocoordinate_fields()                 affirmation_; //170510
   virtual geodesy::Geocoordinate_item *render_geocoordinate()       rendition_;
   // Concept virtual Geocoordinate_item *render_DEN_geocoordinate() rendition_;
   inline virtual bool is_geocoordinates_required() affirmation_ { return true;}
   virtual geodesy::Geocoordinate_record *render_geocoordinate_record            //190924
      () rendition_;
};
//_Geocoordinates______________________________________________________________/
}//_namespace geodesy__________________________________________________________/
#endif

