#include "soil_database_to_VIC_engine.h"

#include <iomanip>
#include <math.h>
#include "corn/math/moremath.h"
#include "corn/measure/measures.h"
#include "USDA/NRCS/soil/generator/soil_generator.h"
#include "corn/data_source/VV_file.h"

#include "common/geomatics/elevation_service_Google.h"
#include "corn/application/project_directory_OS_FS.h"
#include <assert.h>

#if (VIC_VERSION<412)
soil_con_struct read_soilparam(FILE *soilparam,
			       int   RUN_MODEL);
#else
extern soil_con_struct read_soilparam(FILE *soilparam,
			       char *soilparamdir,
			       int  *cell_cnt,
			       char *RUN_MODEL,
			       char *MODEL_DONE);
#endif

#define VIC_layers_original 3
//______________________________________________________________________________
class Item_soil_con
: public implements_ geodesy::Geocoordinate_item
//, public extends_    soil_con_struct
{  // Warning there would be two lat lon and elev values in this record
   // (I currently dont have Geocoordinate (item) interface)
public:
   soil_con_struct soil_con;

};
//_Item_soil_con____________________________________________________2019-02-22_/

//option_struct options;
//______________________________________________________________________________
char *latitude_fieldname = "LAT";
char *longitude_fieldname = "LON";
char *cell_fieldname = "CELLNO";
//______________________________________________________________________________
bool Cell_DB_VIC_point_record::expect_structure(bool for_write)
{  bool expected =
      CS::Soil_point_record::expect_structure(for_write);
      //191002 abandoned geodesy::Geocoordinate_record::expect_structure(for_write);
   //191002 CORN::Data_record::expect_structure(for_write);
   expect_nat32("cell"        , cell_DB_VIC_point->gridcel, 10); //alias                                    //190218
   expect_nat32("CELL"        , cell_DB_VIC_point->gridcel, 10); //alias                                    //190218
   expect_nat32("gridcel"     , cell_DB_VIC_point->gridcel, 10); //alias                                    //190218
   expect_nat32("GRIDCEL"     , cell_DB_VIC_point->gridcel, 10); //alias                                    //190218
   expect_nat32("CELLNO"      , cell_DB_VIC_point->gridcel, 10);
   expect_float32(latitude_fieldname   , cell_DB_VIC_point->latitude_dec_deg);                         //150903
   expect_float32(longitude_fieldname  , cell_DB_VIC_point->longitude_dec_deg);                       //150903
   expect_float32("ELEV_M"    , cell_DB_VIC_point->elevation);                                          //150903
   expect_string("MUKEY"      , cell_DB_VIC_point->mukey_or_polyID, 30); // For STATSGO2
   expect_string("POLY_ID"    , cell_DB_VIC_point->mukey_or_polyID, 30); // For CanSIS
   expect_bool("IRRIGATION"   , cell_DB_VIC_point->irrigation);
   expect_bool("DRYLAND"      , cell_DB_VIC_point->dryland);
   return expected;
}
//______________________________________________________________________________
namespace VIC
{
//______________________________________________________________________________
Soil_database::Soil_database
(std::ostream                    &soil_stream_
,nat8 VIC_layer_count_preferred_
,geodesy::Coordinates            &VIC_cells_available_
,geodesy::Coordinates            &DB_cells_desired_
,const CORN::OS::Directory_name  &database_soil_directory_
,const char                      *soil_file_extension_
, nat8                            VIC_CropSyst_Version_
)
: CropSyst::Soil_database        (database_soil_directory_,soil_file_extension_)
, soil_stream                                                     (soil_stream_)
, VIC_cells_available                                     (VIC_cells_available_)
, DB_cells_desired                                           (DB_cells_desired_)
, VIC_layer_count_preferred                         (VIC_layer_count_preferred_)
, VIC_layer_thickness_preferred                                            (0.1) // This should be an argument

, VIC_CropSyst_Version                                   (VIC_CropSyst_Version_)
{}
//_Soil_database::constructor_______________________________________2019-02-22_/
bool Soil_database::know_component_composition
(const std::string &mukey_or_ID
         // this is used to remove completed (mapunits or polygon IDs)
,const std::string &component_ID
         // The component ID could be mukey_or_ID when writing predominate component
         // when writing each component (currently only STATSGO2)
         // this is the component ID (which also includes the MUKEY)
,CS::Component_composition &component_comp)                  recognition_
{
/*190225 this wont work because the key field is gridcel
   Cell_DB_VIC_point *desired_cell = dynamic_cast<Cell_DB_VIC_point *>
      (DB_cells_desired.find_string(mukey_or_ID));
*/

// File grid files are wasteful/redundence
// Multiple cells may have the exact same soil description.
// We must output a copy the soil description for each cell having the mapunit

//   nat32 match_cell_count = 0;

 FOR_EACH_IN(cell,Cell_DB_VIC_point,DB_cells_desired,cell_iter)
 {
   if ((cell->mukey_or_polyID == mukey_or_ID) && !cell->completed)
   {
      //match_cell_count ++;

      Item_soil_con *avail_VIC_cell =
         dynamic_cast<Item_soil_con *>
         (VIC_cells_available.find_comparable(*cell));
      if (avail_VIC_cell)
      {

         cell->viable = true; // (component_comp); //190225 (map_unit->predominate_component_composition);
         if (cell->viable)
         {
            Smart_soil_parameters &soil_params =
               //190225 map_unit->predominate_component_composition->
               component_comp.
               soil_params;
            write_VIC_cell(
               //190225 map_unit->get_soil_parameters()
               &soil_params
               ,avail_VIC_cell->soil_con);
            know(component_ID,soil_params,cell); // Generate CropSyst soil file
            cell->slopeX = soil_params.steepness_percent; //150410
            float32 depth_profile = soil_params.get_profile_depth();
               //190225 map_unit->predominate_component_composition->profile_depth_m;
            soil_depth_estimator.know_slope_depth                                //150410
               (soil_params.steepness_percent
               ,depth_profile);
         }
         else
         {
            std::cerr << "Non viable map unit found" << std::endl;
            assert(false); // temporary to see if this occurs

            // It is not necessarily an error yet but NYI

            //NYI if the map unit is not viable, need to
            // add to a list of non-viable cells to be
            // accomodated after generating

            // Essentially any remaining DB_cells_desired
            // would be not viable and will need to be
            // instanciated (from nearby cell)

         }
         VIC_cells_available.remove(avail_VIC_cell);
         // Once we have written the cell's available data, we don't need it any more
      } // else desired cells that have not available VIC cell remain not viable
      else
      {
         std::clog << "warning: unable to find available calibrated VIC data for cell: "
            << cell->gridcel << std::endl;
      }
      cell->completed = true;
         // One we have written the cell we dont need it anymore
   }
   } FOR_EACH_END(cell_iter)
   //std::clog << "DB cell count:" << (int)DB_cells_desired.count() << std::endl;
   return true;
}
//_Soil_database::take_map_unit_____________________________________2019-02-22_/
} // namespace VIC

//______________________________________________________________________________
bool Soil_database_to_VIC::Arguments::expect_structure(bool for_write)
{
   bool expected = CS::Soil_convertor_engine::Arguments::expect_structure(for_write);
   expect_nat8    ("layers",VIC_layer_count_preferred,10);
   expect_string  ("VIC",VIC_grid_source_filename);
   return expected;
}
//_Arguments:expect_structure_______________________________________2019-02-17_/
class Viable_point
: public extends_ geodesy::Geocoordinate_item                                    //170831
{
public:
   std::string mukey_or_polyID;
   inline Viable_point
      (const std::string & _mukey_or_polyID
      ,float64 _latitude_dec_deg, float64 _longitude_dec_deg)
      : geodesy::Geocoordinate_item(_latitude_dec_deg,_longitude_dec_deg)
      // Not currently using elevation
      , mukey_or_polyID(_mukey_or_polyID)
      {}
};
//_Viable_point________________________________________________________________/
bool Soil_database_to_VIC::write_layers_column_headers
(const char *header_prefix, nat8 layers)
{
   for (nat8 layer = 1; layer <= layers; layer++)
      VIC_soil_header_stream << header_prefix << (int)layer << '\t';
   return true;
}
//_write_layers_column_headers_________________________________________________/
Soil_database_to_VIC::Soil_database_to_VIC
( const Soil_database_to_VIC::Arguments      &arguments_                              //170831
, nat8             VIC_CropSyst_Version_
, std::ostream    &VIC_soil_detail_stream_
, std::ostream    &VIC_soil_header_stream_ // std::clog
, nat8             VIC_layers_preferred_ // 17
, float32          VIC_layer_thickness_preferred_ // 0.1 // meter
)
: CS::Soil_convertor_engine                      (arguments_)
                                                 //191002 ,&cells_encountered)
, arguments                                                         (arguments_)
,VIC_layer_thickness_preferred                  (VIC_layer_thickness_preferred_)
,VIC_CropSyst_Version                           (VIC_CropSyst_Version_)
,STATSGO2_index_directory
   ("C:\\temp\\CRB\\forecast_V2\\from_STATSGO2\\deletable_index")
,VIC_soil_header_stream                         (VIC_soil_header_stream_)
,VIC_soil_detail_stream                         (VIC_soil_detail_stream_)
,NLDAS_database                                                              (0)
,NLDAS_soil_provider                                                         (0)
,cells_encountered                                                 (&arguments_) //190224
,scope                                                                        () //190224
,geometry                                                                     () //190224
,target_database
   (VIC_soil_detail_stream_
   ,arguments.VIC_layer_count_preferred
   ,VIC_grid_available
   ,cells_encountered // I think cells encountered is the MUKEY-polygonID geoloc assoc
   ,arguments.start_directory
   ,"sil"
   ,VIC_CropSyst_Version
   ) // CropSyst soil file
, VIC_grid_available                                                         (0)
   //loaded in initialize
{
   soil_points = &cells_encountered;                                             //191002
}
//______________________________________________________________________________
Cell_DB_VIC_point
*Soil_database_to_VIC::find_nearest_viable_mukey_or_polyID
(const Geocoordinate &to_point)
{
   float64 nearest_distance = 9999.9; // km
   Cell_DB_VIC_point /* Viable_point */ *nearest_point;

   FOR_EACH_IN(point, Cell_DB_VIC_point, cells_encountered, each_VP)             //150204
   {  float64 distance = geometry.distance_between_km(to_point, *point);
      if ((distance < nearest_distance) && point->viable)                        //150204
      {
         nearest_distance = distance;
         nearest_point = point;
      }
   } FOR_EACH_END(each_VP)return nearest_point;
}
//_Soil_database_to_VIC:constructor____________________________________________/
namespace VIC
{
nat8 component_percent = 100;
//______________________________________________________________________________
class Mapunit_composition
: public CS::Mapunit_composition
{
 public:
   CS::Component_composition component_composition;
 public:
   Mapunit_composition()
      : CS::Mapunit_composition()
      , component_composition(component_percent)
      {}
   virtual bool commit() { return true; }
};
}
//_Mapunit_composition______________________________________________2015-04-11_/
namespace VIC
{
bool Soil_database::setup_VIC_layer_thickness
(float32 VIC_layer_thickness[], float32 actual_profile_depth)
{  float32 surface_layer_thickness = 0.10; // meter  surface layer is always 10cm (Keyvan)
   float32 mid_layer_thickesses = VIC_layer_thickness_preferred;
   VIC_layer_thickness[1] = surface_layer_thickness;
   float32 profile_depth_preferred_VIC = VIC_layer_count_preferred * VIC_layer_thickness_preferred;
   if (actual_profile_depth < profile_depth_preferred_VIC)
   { // shrink the VIC profile to the known soil depth by adjusting
      // the mid layer thickness (the last layer will also have the adjusted thickness)
      float32 actual_profile_depth_minus_surface =
         actual_profile_depth -
         surface_layer_thickness;
      nat8 shrunk_layers = VIC_layer_count_preferred - 1;
      float32 shrunk_layer_thickness =
         actual_profile_depth_minus_surface / (float32)shrunk_layers;
      for (nat8 layer = 2; layer <= VIC_layer_count_preferred; layer++)
         VIC_layer_thickness[layer] = shrunk_layer_thickness;
   }
   else
   { // adjust the last VIC layer to accomodate the known profile depth.
      float32 last_layer_depth = actual_profile_depth -
         surface_layer_thickness - mid_layer_thickesses * (float64)
         (VIC_layer_count_preferred - 2);
      for (nat8 layer = 2; layer < VIC_layer_count_preferred; layer++)
         VIC_layer_thickness[layer] = VIC_layer_thickness_preferred;
      VIC_layer_thickness[VIC_layer_count_preferred] = last_layer_depth;
   }
   return true;
}
//_setup_VIC_layer_thickness___________________________________________________/
}//_namespace VIC______________________________________________________________/

namespace VIC
{
//______________________________________________________________________________
nat8 calc_horizon_at_depth
(float32 depth_m
,const soil_con_struct & soil_con
,nat8 soil_con_layers)
{  nat8 horizon = 1;
   float32 bottom_depth = 0;
   for (nat8 h = 0; h < soil_con_layers; h++)
   {
      bottom_depth += soil_con.depth[h];
      if (depth_m <= bottom_depth) return horizon;
      horizon++;
   }
   if (horizon > soil_con_layers) horizon = soil_con_layers;
   return horizon;
}
//_calc_horizon_at_depth____________________________________________2018-03-08_/
bool Soil_database::write_VIC_cell     // rename to write_VIC_cell
(const Smart_soil_parameters *params
   // params may be 0 indicating the soil parameters were not provided
   // by soil database.
,const soil_con_struct & soil_con // original VIC file
)
{
   float32 profile_depth  = params ? params->get_profile_depth()
      : 0.3; // it we are falling back to original VIC file then this must not be an actual available soil (I.e. water, bedrock)


   nat8     horizon_at_VIC_layer   [MAX_soil_horizons_alloc]; // index 0 not used
   float32  VIC_layer_thickness_m  [MAX_soil_horizons_alloc]; clear_horizon_array32(VIC_layer_thickness_m, 0);
   setup_VIC_layer_thickness(VIC_layer_thickness_m,profile_depth);

   /* Disabled because currently using the original UW elevations.
   CS::Elevation_service_Google elevation_service;
   if (!elevation_service.lookup_elevation(point))
      std::cerr << "Unable to lookup elevation at Website:" <<
         elevation_service.host << std::endl;
   */

   //   REACCH_engine.know_target_soil_directory(&target_directory); // 150202

   // The following a values in units as stored in the file according to:
   // http://www.hydro.washington.edu/Lettenmaier/Models/VIC/Documentation/SoilParam.shtml

   float32 expt               [MAX_soil_horizons_alloc]; clear_horizon_array32(expt);                 // ? unitless exponent ?
   float32 ksat_mm_day        [MAX_soil_horizons_alloc]; clear_horizon_array32(ksat_mm_day);
   float32 phi_s              [MAX_soil_horizons_alloc]; clear_horizon_array32(phi_s);                // (mm/mm)
   float32 density_particle_mnrl_kg_m3 [MAX_soil_horizons_alloc]; clear_horizon_array32(density_particle_mnrl_kg_m3);
   float32 density_bulk_mnrl_kg_m3 [MAX_soil_horizons_alloc]; clear_horizon_array32(density_bulk_mnrl_kg_m3);   // kg/m^3
   float32 porosity           [MAX_soil_horizons_alloc]; clear_horizon_array32(porosity);             // fract
   float32 organic_fract      [MAX_soil_horizons_alloc]; clear_horizon_array32(organic_fract); //190201
   float32 density_particle_org_kg_m3  [MAX_soil_horizons_alloc]; clear_horizon_array32(density_particle_org_kg_m3);   // kg/m^3
   float32 density_bulk_org_kg_m3 [MAX_soil_horizons_alloc]; clear_horizon_array32(density_bulk_org_kg_m3); // kg/m^3
   float32 init_moist_mm      [MAX_soil_horizons_alloc]; clear_horizon_array32(init_moist_mm);        // mm
   float32 bubble_cm          [MAX_soil_horizons_alloc]; clear_horizon_array32(bubble_cm);
   float32 sand_fract         [MAX_soil_horizons_alloc]; clear_horizon_array32(sand_fract);
   float32 clay_fract         [MAX_soil_horizons_alloc]; clear_horizon_array32(clay_fract);
   float32 sand               [MAX_soil_horizons_alloc]; clear_horizon_array32(sand);
   float32 clay               [MAX_soil_horizons_alloc]; clear_horizon_array32(clay);
   // float32 density_kg_m3      [MAX_soil_horizons_alloc]; clear_horizon_array32(density_kg_m3);        // kg/m^3
   float32 wcr_fract          [MAX_soil_horizons_alloc]; clear_horizon_array32(wcr_fract);
   float32 wpwp_fract         [MAX_soil_horizons_alloc]; clear_horizon_array32(wpwp_fract);
   //float32 VIC_layer_depth_m  [MAX_soil_horizons_alloc]; clear_horizon_array32(VIC_layer_depth_m);

   float32 layer_top_depth_m = 0.0;                                              //141001
   float32 layer_center_depth_m = 0.0;                                           //141001

   for (nat8 layer = 1; layer < MAX_soil_horizons_alloc; layer++)
   {  float32 half_thickness_m = VIC_layer_thickness_m[layer] / 2.0;
      float32 layer_center_depth_m = layer_top_depth_m + half_thickness_m;
      nat8 DB_horizon = params
         ? params->get_horizon_at(layer_center_depth_m)            //141001
         : calc_horizon_at_depth(layer_center_depth_m,soil_con,VIC_layers_original);
            // The original US dataset has all layers copied from layer 1
      horizon_at_VIC_layer[layer] = DB_horizon;
      if (params)
      {  sand[layer] = params->get_horizon_sand(DB_horizon);
         clay[layer] = params->get_horizon_clay(DB_horizon);
         sand_fract[layer] = sand[layer] / 100.0;
         clay_fract[layer] = clay[layer] / 100.0;
      }
      else
      {  sand_fract         [layer]= soil_con.quartz[DB_horizon];
         clay_fract         [layer]=     0.3; // Data unavailable
      }
      float32 density_bulk_mnrl_kg_m3_layer =
      density_bulk_mnrl_kg_m3[layer] = params
         ? params->get_horizon_bulk_density_kg_m3(DB_horizon)
         : soil_con.bulk_density[DB_horizon];
      static const float density_particle_mnrl_g_cm3 = 2.65;  // g/cm3
      float32 density_particle_mnrl_kg_m3_layer = g_cm3_to_kg_m3(density_particle_mnrl_g_cm3);
      porosity[layer] = (1.0 - (density_bulk_mnrl_kg_m3_layer / density_particle_mnrl_kg_m3_layer));
      density_particle_mnrl_kg_m3[layer] = params
         ? g_cm3_to_kg_m3(density_particle_mnrl_g_cm3)
         : soil_con.soil_density[DB_horizon];

      organic_fract[layer] = params
         ? params->get_organic_matter(DB_horizon)                                //190201
         : 0.0; // offsite, water, bedrock would have no OM
      density_particle_org_kg_m3[layer] = 1300.0; // kg/m^3                      //190301
      density_bulk_org_kg_m3[layer]
         = density_particle_org_kg_m3[layer] * (1.0 - porosity[layer]);
         // We don't have porosity of OM so assume it is the same as the soil.

      #define water_density 1000.0
      init_moist_mm[layer] = params
         ? (params->get_horizon_field_capacity_volumetric(DB_horizon)
            * porosity[layer] * VIC_layer_thickness_m[layer]
            * water_density)
            // converts to kg/m^2 which is mm water depth
         : soil_con.init_moist[DB_horizon];
      //150208 init moist is different but right order of mag.

      #define CRITICAL 0.8
      wcr_fract[layer] = params
         ? (params->get_horizon_field_capacity_volumetric(DB_horizon)
            * CRITICAL / porosity[layer])
         : soil_con.Wcr[DB_horizon];
      wpwp_fract[layer] = params
         ? (params->get_horizon_permanent_wilt_point_volumetric(DB_horizon)
            / porosity[layer])
         : soil_con.Wpwp[DB_horizon];
      if (params)
      {
         float32 b_campbell_Saxton = params->get_horizon_Campbell_b(DB_horizon);
         // provided above sand_fract[layer] = sand[layer]/100.0;
         // provided above       clay_fract[layer] = clay[layer]/100.0;
         expt[layer] = 3.0 + 2.0 * b_campbell_Saxton; // params.get_horizon_Campbell_b(DB_horizon);
         // lamda is 1/b
      } else expt[layer] = soil_con.expt[DB_horizon];
      bubble_cm[layer] = params
         ? (0.32 * expt[layer] + 4.3)    // from Keyvan's worksheet (latest)
         : soil_con.bubble[DB_horizon];
         // http://www.hydro.washington.edu/Lettenmaier/Models/VIC/Documentation/Info/bubble.shtml
      float32 ksat_m_d = params
         ? params->get_horizon_sat_hydraul_cond_m_d(DB_horizon)
         : soil_con.Ksat[DB_horizon];
      ksat_mm_day[layer] = m_to_mm(ksat_m_d) ;  // convert m/day to mm/day
      /*RLN keyvan's code was wrong units of texture
      10.0 * 24.0
         * exp(12.012
               - 0.0755 *            sand[layer]
               + (-3.895 + 0.03671 * sand_fract
                          -0.1103  * clay_fract
                          + 0.00087546 * pow((double)clay_fract, (double)2.0))
                 / porosity[layer]);
      */
      /*
      float sat = get_saturation();
      float clay_2 = clay * clay;
      float sat_hydraul_cond_cm_hr =  (sat != 0.0)
      ?	  exp((12.012 - 0.0755 * (float)sand)  +
             (- 3.895 + 0.03671 * (float)sand -
                        0.1103  * (float)clay +
                        8.7546e-4 * clay_2)
             / sat)
      : 0.0;

      */

/* from keyvan's spreadsheet
temp.Ksat[layer]= 10*24*exp(12.012-0.0755* temp.quartz[layer]*100 + (-3.895+0.03671*temp.quartz[layer]-0.1103*temp.clay[layer]
+ 0.00087546* pow (temp.clay[layer], 2.0))/temp.porosity[layer]);
*/

      phi_s[layer] = params ? -999 // not used, applies to arctic conditions
         : soil_con.phi_s[DB_horizon];
      density_particle_mnrl_kg_m3[layer] = g_cm3_to_kg_m3(density_particle_mnrl_g_cm3);   // Warning check units

      layer_top_depth_m += VIC_layer_thickness_m[layer];                         //141001
      //VIC_layer_depth_m[layer] = layer_top_depth_m;
   } // for

   //190222 std::ostream &soil = VIC_soil_detail_stream;
   // std::ostream &soil_stream = target_database.soil_stream;                      //190222


     soil_stream
        << std::fixed << "1" // run_cell enabled indicator
        << ' ' << std::setw(6) << soil_con.gridcel // gridcel 240493 241420 241421 241422          not known need to be taken from other sources
        << ' ' << std::setw(8) << std::setprecision(5) << soil_con.lat /*point.latitude_dec_deg*/    // lat 41.21875 41.281 41.2813 41.2813
        << ' ' << std::setw(8) << std::setprecision(5) << soil_con.lng /*point.longitude_dec_deg*/   // lon -116.219 -116.28 -116.22 -116.16
        << ' ' << std::setw(5) << std::setprecision(5) << soil_con.b_infilt         // 0.1      // bi  Infiltration parameter (? appears to be either 1 or 3, presuming will be calibrated)
        << ' ' << std::setw(8) << std::setprecision(6) << soil_con.Ds               // 0.5      // ds   Subsurface flow rate   (? appears to range 0 to 1 ?)
        << ' ' << std::setw(8) << std::setprecision(6) << soil_con.Dsmax            // 10.0     // dsMax mm/day [Maximum subsurface flow rate]
        << ' ' << std::setw(8) << std::setprecision(6) << soil_con.Ws
     // 0.5      // Ws  FRACTION OF BOTTOM SOIL LAYER MOISTURE
        << ' ' << std::setw(1) << soil_con.c; // "2"      // soil c  appears to be always 2

     nat8 layer;
     // All remaining value has precision of 4
     soil_stream << ' ' << std::setprecision(4);

#define stream_soil(element) { soil_stream << ' ' << element; }
#define stream_soil_layers(layer_element) \
      {  for (layer = 1; layer <= VIC_layer_count_preferred; layer++) \
         soil_stream << ' ' << layer_element[layer];}

      stream_soil_layers(expt);
      stream_soil_layers(ksat_mm_day);
      stream_soil_layers(phi_s);
      stream_soil_layers(init_moist_mm);
      stream_soil       (soil_con.elevation); //  stream_soil       (REACCH_engine.geolocation.elevation);
      stream_soil_layers(VIC_layer_thickness_m);

      stream_soil       (soil_con.avg_temp);
      stream_soil       (soil_con.dp);

      stream_soil_layers(bubble_cm);
      stream_soil_layers(sand_fract);


      {for (layer = 1; layer <= VIC_layer_count_preferred; layer++)
         soil_stream << ' ' << density_bulk_mnrl_kg_m3[layer];}
      stream_soil_layers(density_particle_mnrl_kg_m3);

      stream_soil_layers(organic_fract);                                         //190201
      stream_soil_layers(density_bulk_org_kg_m3);                                //190301
      stream_soil_layers(density_particle_org_kg_m3);                            //190301

      stream_soil       (-8); // offGMT

      stream_soil_layers(wcr_fract);
      stream_soil_layers(wpwp_fract);

      stream_soil       (soil_con.rough);
      stream_soil       (soil_con.snow_rough);
      stream_soil       (soil_con.annual_prec);
      {for (layer = 1; layer <= VIC_layer_count_preferred; layer++)
         soil_stream << ' ' << 0.0;}
         // resid_moist is 0 because 0 in campbell formula
         // If using Brooks-Corey Model you would need a non-zero value.

      stream_soil       (0); // flag for frozen soil (0=disabled)


#if SPATIAL_SNOW
   stream_soil(max_snow_distrib_slope); // NYI
#endif
#if SPATIAL_FROST
   stream_soil(frost_slope); // NYI
#endif
#if EXCESS_ICE
   stream_soil_layers(init_ice_fract); // NYI
#endif

      stream_soil       (soil_con.avgJulyAirTemp);
      //float32 avgJulyAirTemp = REACCH_engine.july_temperature_avg;
      //stream_soil(avgJulyAirTemp);

      if (VIC_CropSyst_Version >= 2)
         stream_soil_layers(clay_fract);
/*190222 Not sure if this is currently being used in VIC CropSyst
      if (VIC_CropSyst_Version >= 3)
         soil_stream << ' ' << mukey_or_polyID;
*/
   // The following were added for the VIC CropSyst integration

   soil_stream << std::endl;
/*190222 now  moved to/handled by CropSyst::Soil_database
   {  CORN::OS::File_name_concrete soil_filename
         (arguments.destination_directory                                        //170831
         ,point_derived_from.mukey_or_polyID, "sil");
      CORN::VV_File CS_soil_file(soil_filename.c_str());
      CORN::Data_record &params_as_DR =
               dynamic_cast<CORN::Data_record&>(params);
      CS_soil_file.set(params_as_DR);
   }
*/
   return true;
}
//_write_VIC_cell______________________________________________________________/
}//_namespace VIC______________________________________________________________/
bool Soil_database_to_VIC::write_headers()
{  VIC_soil_header_stream << "enable\tgridcel\tlat\tlon\tbi\tDs(mm/day)\tDsmax(mm/day)\tWs\tc\t";
   nat8 VIC_layer_count_preferred = arguments.VIC_layer_count_preferred;                   //190207
   write_layers_column_headers("expt"                 ,VIC_layer_count_preferred); // << "expt1	expt2	expt3	"
   write_layers_column_headers("ksat(mm/day)"         ,VIC_layer_count_preferred); // << "ksat1	ksat2	ksat3	"
   write_layers_column_headers("phi_s"                ,VIC_layer_count_preferred); // << "ksat1	ksat2	ksat3	"
   write_layers_column_headers("initmoist"            ,VIC_layer_count_preferred); // << "ksat1	ksat2	ksat3	"
   VIC_soil_header_stream <<   "elev\t";
   write_layers_column_headers("dz"                   ,VIC_layer_count_preferred); // << "sand1	sand2	sand3	"
   VIC_soil_header_stream <<   "T_avg\t";
   VIC_soil_header_stream <<   "dampdpth\t";
   write_layers_column_headers("bubble(cm)"           ,VIC_layer_count_preferred); // << "bubble1	bubble2	bubble3	"
   write_layers_column_headers("quartz(frct)"         ,VIC_layer_count_preferred); // << "quartz1	quartz2	quartz3	"
   write_layers_column_headers("density_bulk_soil(kg/m^3)" ,VIC_layer_count_preferred); // << "bulk density1	bulk density2	bulk density3	"
   write_layers_column_headers("density_particle_soil(kg/m^3)"         ,VIC_layer_count_preferred); // << "bulk density1	bulk density2	bulk density3	"
   write_layers_column_headers ("organic(fract)"      ,VIC_layer_count_preferred); // organic fraction
   write_layers_column_headers ("density_bulk_organic(kg/m^3)",VIC_layer_count_preferred); // organic fraction
   write_layers_column_headers ("density_particle_soil(kg/m^3)",VIC_layer_count_preferred); // organic fraction
   VIC_soil_header_stream <<   "offGMT\t";
   write_layers_column_headers("wcr(fract)"           ,VIC_layer_count_preferred); // << "wcr Fract1	wcr Fract2	wcr Fract3	"
   write_layers_column_headers("wpwp(fract)"          ,VIC_layer_count_preferred); // << "wpwp fract1	wpwp fract2	wpwp fract3	"
   VIC_soil_header_stream <<   "rough\t";
   VIC_soil_header_stream <<   "rough_snow\t";
   VIC_soil_header_stream <<   "prcp_avg(mm)\t";
   write_layers_column_headers("residmoist"           ,VIC_layer_count_preferred); // << "initmoist1	initmoist2	initmoist3	"
   VIC_soil_header_stream <<   "FS\t";
   VIC_soil_header_stream <<   "T_jly\t";
   if (VIC_CropSyst_Version >= 2)
      write_layers_column_headers("clay(frct)"        ,VIC_layer_count_preferred); // << "clay1	clay2	clay3	"
/*Not sure if used
   if (VIC_CropSyst_Version >= 3)
      VIC_soil_header_stream <<   "source_ID\t";
*/
   VIC_soil_header_stream << std::endl;
   return true;
}
//_write_headers_______________________________________________________________/
bool Soil_database_to_VIC::set_scope
(float64 latitude_minimum
,float64 longitude_minimum
,float64 latitude_maximum
,float64 longitude_maximum )
{
   scope.minimum.set_latitude_dec_deg_f64  (latitude_minimum);
   scope.minimum.set_longitude_dec_deg_f64 (longitude_minimum);
   scope.maximum.set_latitude_dec_deg_f64  (latitude_maximum);
   scope.maximum.set_longitude_dec_deg_f64 (longitude_maximum);
   // Delete records that have been loaded into memory
   return true;
}
//_set_scope___________________________________________________________________/
bool Soil_database_to_VIC::initialize()                          initialization_
{  bool inited = CS::Soil_convertor_engine::initialize();

   // Currently NLDAS is specific to this VIC convertor
   // May want to eventually moved to Soil_convertor_engine

   NLDAS_database = new NLDAS::Database((arguments.LDAS_NLDAS_dirname));         //150927

   //170902 instead of scope now is available extent

   NLDAS_database->set_scope                                                     //151010
      (scope.minimum.get_latitude_dec_deg()
      ,scope.minimum.get_longitude_dec_deg()
      ,scope.maximum.get_latitude_dec_deg()
      ,scope.maximum.get_longitude_dec_deg());
   NLDAS_soil_provider = new NLDAS::Soil_provider(*NLDAS_database);              //150927

   inited &= load_available_VIC_cells();                                         //190222
   inited &= write_headers();                                                    //190222
   return inited;
};
//_initialize__________________________________________________________________/
bool Soil_database_to_VIC::process()
{  bool processed = CS::Soil_convertor_engine::process();
   /*
   There may yet be cells that could not be processed because
   because there is no viable soil data as the cell may be in water
   or marginal land or simply unknown/unavailable.
   In these cases we simply uses the original VIC data
   */

   Smart_soil_parameters missing_soil_params;
   missing_soil_params.horizon_count = 1;
   missing_soil_params.set_horizon_thickness(1,0.30);
   nat32 missing_count = 0;
   FOR_EACH_IN(cell,Cell_DB_VIC_point,cells_encountered,cell_iter)
   {
      if (!cell->completed)
      {  missing_count ++;
         Item_soil_con *avail_VIC_cell =
            dynamic_cast<Item_soil_con *>
               (VIC_grid_available.find_comparable(*cell));
         if (avail_VIC_cell)
         {
            target_database.write_VIC_cell(0,avail_VIC_cell->soil_con);          //190308
            //190308 target_database.write_original_VIC_cell(avail_VIC_cell->soil_con);
         }
         cell->completed = true;
      }
   }
   FOR_EACH_END(cell_iter)
   std::clog << "cells missing:" << missing_count << std::endl;
   return true;
}
//_process_____________________________________________________________________/
nat32 Soil_database_to_VIC::load_available_VIC_cells()
{
   nat32 read_count = 0;
   nat32 corresponding_count = 0;
   //soil_con_struct soil_con;
   int cell_cnt = 0;
   char RUN_MODEL = 1;
   char MODEL_DONE= 0;
   FILE *VIC_soil_grid_original = fopen
      (arguments.VIC_grid_source_filename.c_str(), "r");
      //190217 (VIC_soil_grid_original_filename.c_str(), "r");
   if (VIC_soil_grid_original)
   {  while (!feof(VIC_soil_grid_original))
      {
         Item_soil_con *available_con = new Item_soil_con;

         available_con->soil_con = read_soilparam
         (VIC_soil_grid_original
         #if  (VIC_VERSION < 412)
         ,RUN_MODEL
         #else
         , 0 // filenames.soil_dir only used when reading Arc/Info formatted files
         , &cell_cnt, &RUN_MODEL, &MODEL_DONE
         #endif
         );
         available_con->set_latitude_dec_deg_f32 (available_con->soil_con.lat);
         available_con->set_longitude_dec_deg_f32(available_con->soil_con.lng);

         cell_cnt++;

std::clog << " " << available_con->soil_con.gridcel;

         //190225 const Geocoordinate_item *
         const Cell_DB_VIC_point *desired_point
            =
            dynamic_cast<const Cell_DB_VIC_point *>(
            cells_encountered.find_nearest_item_km
            (*available_con,0.0,5.0));// within 5 km
         if (desired_point)
         {
            available_con->soil_con.gridcel =
               desired_point->gridcel;
               //190225 desired_point->get_key_nat32();

            // The desired_point key is VIC gridcel in
            // with the preferred enumeration.
            // We change the VIC_grid_available to this preferred enumeration.
            VIC_grid_available.take(available_con);
std::clog << "+";
            corresponding_count++;
         }
         else
         {
std::clog << "-";
            delete available_con;
         }
std::clog << std::endl;
         read_count++;
      }
   } else
      std::cerr << "error: unable to open "
         << arguments.VIC_grid_source_filename.c_str()
         << std::endl;
   std::clog << "corresponding points:" << corresponding_count << std::endl;
   return read_count;
}
//_load_available_VIC_cells____________________________________________________/
void nrerror(char error_text[]) {}
//______________________________________________________________________________
int read_arcinfo_info
(char    *filename
,double **lat
,double **lng
,int    **cellnum) { return 0;}
//_read_arcinfo_info___________________________________________________________/
double read_arcinfo_value
(char *filename
,double lat
,double lng) { return 0; }
//_read_arcinfo_value__________________________________________________________/

#define END '\0'
#define NEW '\n'
void ttrim( char *c )
{
  while( (*c++ != END) );
    --c;
  for( ; *--c == NEW; *c = END );
}
//_ttrim_______________________________________________________________________/
/*
For areas with no detailed soil profile data

http://www.isric.org/
WISE revised legend (FAO 1988)
8 tables

SOTER database

WISsat9_a_ACGY  Acrisols to Gypsisols  stats majors groupings
WISsat9_a_HSVR  Histosols to Vertisols
WISsat9_b_ACGY

file:///C:/Documents%20and%20Settings/rnelson/My%20Documents/Downloads/A_taxotransfer_rule_based_approach_for_filling_ga.pdf
*/
//______________________________________________________________________________
namespace VIC {

nat32 Soil_depth_estimator_regression::know_slope_depth(float32 slope, float32 depth)
{  if (slope > 0.0001)
   {  slopes.append(slope);
      profile_depths.append(depth);
   }
   return slopes.get_count();
}
//_know_slope_depth____________________________________________________________/
float32 Soil_depth_estimator_regression::get_depth(float32 given_slope_steepness)
{
   float32 prof_depth = 2.0; // default to 2 meter
   CORN::Linear_regression<float32> slope_depth_regr(slopes,profile_depths);
   float32 reg_slope = 0.0;
   float32 reg_intr  = 0.0;
   bool reg_valid = slope_depth_regr.get_methodA(reg_slope,reg_intr);
   prof_depth = (given_slope_steepness * reg_slope) +  reg_intr;
   return prof_depth;
}
//_get_depth___________________________________________________________________/
}//_namespace VIC______________________________________________________________/

geodesy::Geocoordinate_item *Cell_DB_VIC_points
::render_geocoordinate()                                              rendition_
{
   Cell_DB_VIC_point *point = new Cell_DB_VIC_point;
   point->mukey_or_polyID = CORN::strip_string
            (point->mukey_or_polyID, CORN::Both);
   //moved to constructor point->viable = false; // Viability not yet identified
   return point;
}
//_render_geocoordinate_____________________________________________2017-09-02_/
/*191002 obsolete now using Data_record
bool Cell_DB_VIC_points::setup_DBF_fields(CORN::Data_source_dBase &DBF_file)  provision_
{
   bool sat_up = CS::Soil_points::setup_DBF_fields(DBF_file);
      //CS::Soil_points::provide_fields_dBase(DBF_file);
   if (!gridcel_field) gridcel_field = DBF_file.lookup_field("gridcel");
   if (!gridcel_field) gridcel_field = DBF_file.lookup_field("GRIDCEL");
   if (!gridcel_field) gridcel_field = DBF_file.lookup_field("cell");
   if (!gridcel_field) gridcel_field = DBF_file.lookup_field("CELL");
   return sat_up;
}
//_setup_DBF_fields_________________________________________________2019-02-22_/
*/
/*191002 obsolete now using Data_record
bool Cell_DB_VIC_points::set_from_record
(CORN::Generic_data_record &soil_point_rec
,Geocoordinate &geocoord_)
{

   Cell_DB_VIC_point *soil_point = dynamic_cast<Cell_DB_VIC_point *>(&geocoord_);
   CORN::VV_Section *section = soil_point_rec.get_current_section();
   if (gridcel_field)
   {
      CORN::VV_int32_entry *gridcel_entry = dynamic_cast<CORN::VV_int32_entry *>
         (section->entries.find_cstr(gridcel_field->get_name(),false));
      if (gridcel_entry)
      {
         soil_point->gridcel = gridcel_entry->get_value();
      }
   }
   return CS::Soil_points::set_from_record(soil_point_rec,geocoord_);
}
//_set_from_record__________________________________________________2019-02-22_/
*/
double calc_Maximum_sorptivity
(double sand, double clay, double porosity,double b_campbell)
{
   // not currently needed for convertor
   return 99999.9;
}
//_calc_Maximum_sorptivity______________________________________2019-02-02_RLN_/

/*
expt
Ksat
phi_s
init_moist
depth
bubble
quartz
bulk_dens_min
soil_dens_min
organic
bulk_dens_org
soil_dens_org
Wcr_FRACT
Wpwp_FRACT
resid_moist residual moisture
init_ice_fract
clay
porosity
bulk_density
soil_density
b_campbell
Wcr
Wpwp
water_pot_at_FC
S_max  max sorptivity
AE  ?air entry pot?
*/
