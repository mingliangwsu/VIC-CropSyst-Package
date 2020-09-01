#ifndef soil_database_to_VIC_engineH
#define soil_database_to_VIC_engineH
//______________________________________________________________________________
#include "common/geodesy/geolocation_item.h"
#include "common/geodesy/geometry/terrestrial_geometry.h"
#include "corn/data_source/dbase_datasrc.h"
#include "CanSIS/NSDB/soil/provider/CanSIS_NSDB_soil_provider.h"
#include "LDAS/soil/provider/NLDAS_soil_provider.h"
#include "corn/math/numerical/linear_regression_T.h"

#include "CS_suite/CropSyst/convert/soil/soil_convertor_engine.h"

#include <stdio.h>
//RLN Need to include stdio.h before vicNl_def.h because
// vicNl_def needs FILE declaration
#include <vicNl_def.h>

#include "USDA/NRCS/soil/survey/database/STATSGO_SSURGO_database.h"
#include "CS_suite/CropSyst/convert/soil/soil_convertor_engine.h"
#include "corn/data_source/generic_datarec.h"

/* VIC grid cell properties and file format documentation:
http://www.hydro.washington.edu/Lettenmaier/Models/VIC/Documentation/SoilParam.shtml
*/

namespace CS
{  class Soil_provider;
}

namespace VIC
{
/*
The VIC soil database currently consists of a single text file
which is the VIC grid for the watershed/basin.

CONCEPTUAL:
For VIC-CropSyst the database will consist of the VIC grid file
and a set of CropSyst soil files and a geocoordinate association file
which would be either a DBF or CSV (CS Suite recognized point file)
So currently the VIC::Soil_database is derived from CropSyst::Soil_Database
*/
//______________________________________________________________________________
class Soil_depth_estimator_regression
{
   CORN::Dynamic_array<float32> slopes; // independent variable
   CORN::Dynamic_array<float32> profile_depths; // dependent variable
 public:
   inline Soil_depth_estimator_regression()                                   {}
   nat32 know_slope_depth(float32 slope, float32 depth);
   float32 get_depth(float32 given_slope);
};
//_class_Soil_depth_estimator_regression____________________________2015-04-10_/
class Soil_database
: public extends_ CropSyst::Soil_database
{
 public:
   std::ostream &soil_stream;
   geodesy::Coordinates &VIC_cells_available;
      // These are VIC grid items (from the original VIC database)
      // Filtered agains desired cells;
      // items are deleted as processed
   geodesy::Coordinates &DB_cells_desired;
      // These are  Cell_DB_VIC_point (assocated soil database ID with geocoord
      // items are deleted as processed

   nat8     VIC_layer_count_preferred;
   float32  VIC_layer_thickness_preferred; // meter

   nat8 VIC_CropSyst_Version;
   Soil_depth_estimator_regression soil_depth_estimator;

 public:
   Soil_database
      (std::ostream &soil_stream
      ,nat8 VIC_layer_count_preferred
      ,geodesy::Coordinates &VIC_cells_available
      ,geodesy::Coordinates &DB_cells_desired
      ,const CORN::OS::Directory_name &database_soil_directory
         // where generated soils files are output
      ,const char *soil_file_extension
      , nat8 VIC_CropSyst_Version);
/*190222
   virtual bool know
      (const std::string            &name_or_ID
      ,const Smart_soil_parameters  &params
      ,const Geocoordinate          *geocoordinate)                  cognition_;
*/
/*190225
   virtual bool take_map_unit
      (
      //190224 abandoned const std::string            &name_or_ID,
      CS::Mapunit_composition *map_unit)                        appropriation_;
*/
   virtual bool know_component_composition
      (const std::string &mukey_or_ID
         // this is used to remove completed (mapunits or polygon IDs)
      ,const std::string &component_ID
         // The component ID could be mukey_or_ID when writing predominate component
         // when writing each component (currently only STATSGO2)
         // this is the component ID (which also includes the MUKEY)
      ,CS::Component_composition &component_comp)                    recognition_;
 public:
   virtual bool write_VIC_cell
      (const Smart_soil_parameters *params
      //190222 ,modifiable_ Cell_DB_VIC_point & point_derived_from
      ,const soil_con_struct & soil_con)                             stream_IO_;
/*obsolete  now use write_VIC_soil where params is 0
   bool write_original_VIC_cell                                                  //150215
      (//modifiable_ Cell_DB_VIC_point   *encountered_cell,
      const soil_con_struct           &soil_con);
*/
 private:
   bool setup_VIC_layer_thickness
      ( float32 VIC_layer_thickness[]
      , float32 actual_profile_depth);
};
//_Soil_Database_____________________________________________________2019-02-22_/

} // namespace VIC

//______________________________________________________________________________
class Cell_DB_VIC_point
: public extends_ CS::Soil_point // Geocoordinate_item
//191002 , public CORN::Data_record
{
 public:
   nat32 gridcel;  // cellno;
   //unused AREASYMBOL
   //unused SPATIALVER
   //unused    CORN::Ustring musym;
   float32     slopeX; // steepness percent
      // In think the slope is to accomodate non-viable cells.
   bool irrigation;
   bool dryland;
 public: //contributions
   bool viable;
   bool completed;                                                               //190228
 public:
   inline Cell_DB_VIC_point()
      : CS::Soil_point()
      //191002 : CORN::Data_record("cell" /*"Cell_DB_VIC_point"*/) //primary section irrelevelent
      , slopeX(0)
      , viable(false) // viability not yet identified
      , completed(false)                                                         //190228
      {}
   // moved to Cell_DB_VIC_point_record
   // inline virtual bool expect_structure(bool for_write);
   inline virtual bool is_key_nat32(nat32 key)                      affirmation_ //180820
      { return gridcel==key;}
   inline virtual nat32 get_key_nat32()                const { return gridcel; }
};
//______________________________________________________________________________
class Cell_DB_VIC_point_record
: public extends_  CS::Soil_point_record
{
 public:
   mutable Cell_DB_VIC_point *cell_DB_VIC_point;
 public:
   inline Cell_DB_VIC_point_record(/*const char *primary_section_*/)
      : CS::Soil_point_record                                           ("cell")
      , cell_DB_VIC_point                                (new Cell_DB_VIC_point)
      {}
   virtual bool expect_structure(bool for_write);
   inline virtual Cell_DB_VIC_point *relinquish_item()
   {
      Cell_DB_VIC_point *item = cell_DB_VIC_point;
      cell_DB_VIC_point = 0;
      return item;
   }
};
//_Cell_DB_VIC_point_record_________________________________________2019-10-02_/
class Cell_DB_VIC_points
: public extends_ CS::Soil_points
{
 private:
   provided_ CORN::Data_source_dBase::Field_item *gridcel_field;
 public:
   inline Cell_DB_VIC_points(const geodesy::Geocoordinates::Arguments *arguments_)
      : CS::Soil_points                                             (arguments_)
      , gridcel_field                                                        (0)
      {}
 protected: // Geocoordinates implementations
   virtual geodesy::Geocoordinate_item *render_geocoordinate()                rendition_;
   /*191002 obsolete now using Data_record
   virtual bool setup_DBF_fields(CORN::Data_source_dBase &DBF_file)  provision_;
   virtual bool set_from_record
      (
       CORN::Generic_data_record &soil_point_rec
      ,Geocoordinate &geocoord_
      );
   */
};
//_Cell_DB_VIC_points_______________________________________________2017-09-02_
class Soil_database_to_VIC
: public extends_ CS::Soil_convertor_engine                                      //170831
{public:
   //_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
   class Arguments
   : public extends_ virtual CS::Soil_convertor_engine::Arguments
   {
    public:
      nat8        VIC_layer_count_preferred;
      std::string VIC_grid_source_filename;                                      //190217
         // This is the file containing VIC grid cells (soil file)
         // from which non-soil values are copied from.
         // It is not the name of the VIC grid file to be generated.
         // This file is required because we need to get the non-soil cell properties.
         // The file could be for the entire region
         // (I.e. the VIC calibrated soils for the US: vic.soil.cal.us)

    public:
      inline Arguments()
      : CS::Soil_convertor_engine::Arguments                                  ()
      , VIC_layer_count_preferred                                            (3)
      , VIC_grid_source_filename
                        ("C:\\Projects\\MSLSH\\Database\\Soil\\vic.soil.cal.us")
      {}
      virtual bool expect_structure(bool for_write);                             //190217
   };
   //_2017-08-31_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _Arguments_/
 protected:
   const Arguments &arguments;                                                   //170831
 protected: // options/parameter (to be moved to Arguments)
//190207 moved to arguments   nat8     VIC_layer_count_preferred;
   float32  VIC_layer_thickness_preferred; // meter
            // the last layer will have variable thickness for deep soils
   nat8     VIC_CropSyst_Version;
   /*moved to Soil_database
   Soil_depth_estimator_regression soil_depth_estimator;
   */
 protected: // Database paths
   CORN::OS::Directory_name_concrete STATSGO2_index_directory;
      // This can be any arbitrary directory that can be deleted
      // when nolonger used.
 protected: // output streams
   std::ostream &VIC_soil_header_stream;
   std::ostream &VIC_soil_detail_stream;
 protected: // source
   /*190217 replaced with arguments.VIC_grid_source_filename
   CORN::OS::Directory_name_concrete VIC_soil_grid_original_filename;
      // The original erroneous VIC grid file 3 layer soil
      // prepared by the University of Washington.
   */
 protected:
   NLDAS::Database      *NLDAS_database;                                         //150927
   NLDAS::Soil_provider *NLDAS_soil_provider;                                    //150927
   Cell_DB_VIC_points    cells_encountered;                                      //170902
      // This lists cells to actually include in the exported VIC file.
      // It associates the selected cells geolocation with the various
      // soil database primary keys MYKEY (STATSGO2, POLY_ID CanSIS);
      // and target VIC gridcel.
      // This does not specify association with the original (available) VIC soil file
      // as those gridcel may not necessarily be the same numbering system.
      // Note that xxx references cells_encountered as the points to be included.
      // (soil_points)
   Scope scope;                                                                  //151010
   // Note that scope probably can be replaced by extent

   Terrestrial_geometry geometry;
   float32 thickness[MAX_soil_horizons_alloc];
   VIC::Soil_database target_database;                                           //190222
   geodesy::Geocoordinates VIC_grid_available;
      // This is available VIC grid loaded into memory.
      // This is an existing (calibrated) VIC grid/soil file I.e vic.soil.cal.us
      // Initially all records matching desired geocoordinates
      // are loaded into memory.
      // As (desired) cells are processed and output, the items can be removed
      // from VIC_grid_available.
 public:
   Soil_database_to_VIC
      (const Arguments &arguments_
      //190217 moved to arguments ,const CORN::OS::Directory_name &VIC_soil_grid_original_filename
      ,nat8 VIC_CropSyst_Version  = 3
      ,std::ostream &VIC_soil_detail_stream    = std::cout
      ,std::ostream &VIC_soil_header_stream    = std::clog
      ,nat8          VIC_layer_count_preferred       = 17
      ,float32       VIC_layer_thickness_preferred   = 0.1 // meter
      );
   bool set_scope                                                                //151010
      (float64 latitude_minimum  =  -90.0
      ,float64 longitude_minimum = -180.0
      ,float64 latitude_maximum  =   90.0
      ,float64 longitude_maximum =  180.0);
   virtual bool initialize()                                    initialization_;
   virtual bool process();                                                       //190301


   nat32 load_available_VIC_cells();                                             //190222
   /*190222 replaced with load_available_VIC_cells and records kept in memory
   bool process();
      //190217 (const CORN::OS::File_name       &original_VIC_soils);
      /// \return true is successed (any processed)
//      /// \return exit status <-obsolete
   */
 public:
   bool write_headers();
   bool write_layers_column_headers
      (const char *header_prefix
      , nat8 layers);
 private:
   bool provider_to_VIC
      (CS::Soil_provider &provider                                               //141001
      ,CORN::Data_source_dBase &VIC_point_MUkey_intersection_table);
 protected:
   Cell_DB_VIC_point
      *find_nearest_viable_mukey_or_polyID
      (const Geocoordinate &to_point);
   /*190222 moved to Soil_database
   bool write_VIC_soil
      (//190222 const CS::Mapunit_composition   &map_unit
       const Smart_soil_parameters &params                                       //190222
      ,modifiable_ Cell_DB_VIC_point   &point
      ,const soil_con_struct           &soil_con); // soil_con containing original VIC provided by UW
   bool setup_VIC_layer_thickness
      ( float32 VIC_layer_thickness[]
      , float32 actual_profile_depth);
   */
   bool provider_to_VIC_cell
      (modifiable_ Cell_DB_VIC_point   &encountered_cell
      ,CS::Soil_provider               &provider
      ,const soil_con_struct           &soil_con);
   //#ifdef USE_ORIGINAL_UW_SOILS
   // now providing NLDAS data  but this is fallback option
/*190301 moved to Soil_database
   bool original_UW_to_VIC_cell                                                  //150215
      (modifiable_ Cell_DB_VIC_point   *encountered_cell,
      const soil_con_struct           &soil_con);
*/
   //#endif
   bool instanciate_soil_providers
      (const CORN::Unidirectional_list & grid_database_intersect_filenames);
//190222   bool process_cell(const soil_con_struct & soil_con);
 public: // CS::Soil_convertor_engine implementations
   inline virtual VIC::Soil_database &get_target_database()                      //190222
      { return target_database; }
};
//______________________________________________________________________________
//______________________________________________________________________________
#endif

