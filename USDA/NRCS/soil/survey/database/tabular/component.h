//---------------------------------------------------------------------------
#ifndef componentH
#define componentH

#include "component_struct.h"
#include "corn/parameters/parameter.h"
#include "corn/data_source/datarec.h"

#define ENUM_NYI CORN::Parameter_properties_c_str
#define BOOL_NYI CORN::Parameter_properties_c_str
//---------------------------------------------------------------------------
namespace USDA_NRCS {
#define Param_number CORN::Parameter_number
#define Param_propsC CORN::Parameter_properties_c_str
//______________________________________________________________________________
/* conceptual NYI
struct Parameter_LRH
{
   CORN::Parameter_number  l; // low
   CORN::Parameter_number  r; // representitive
   CORN::Parameter_number  h; // high
 public:
   Parameter_LRH
      (CORN::Parameter_number  &l // low
      ,CORN::Parameter_number  &r // representitive
      ,CORN::Parameter_number  &h // high
      );

};
//_Parameter_LRH________________________________________________________________
*/
class Component_record
: public Component_struct
, public CORN::Data_record
{
 public:
   //conceptual Parameter_LRH p_compptc;      // Comp %   0 100 percent                       //190927
   Param_number p_comppct_l;     // Comp %   0 100 percent
   Param_number p_comppct_r;
   Param_number p_comppct_h;

   Param_propsC p_compname;      // Component Name String No 60
   ENUM_NYI     p_compkind;      // Kind 254 Component_kind
   BOOL_NYI     p_majcompflag;   // Major Component 3 bool
   Param_propsC p_otherph;       // SIR phase No 40
   Param_propsC p_localphase;    // Local Phase   40
   Param_number p_slope_l;       // Slope Gradient (prcsn 1) 0 999, percent
   Param_number p_slope_r;
   Param_number p_slope_h;
   Param_number p_slopelenusle_l;// Slope Length USLE    0 4000, meters
   Param_number p_slopelenusle_r;
   Param_number p_slopelenusle_h;
   Param_propsC p_runoff;        //  Runoff Class   254 runoff Runoff_class
   Param_number p_tfact;         // T  (prcsn 1) 5 tons per acre per year
   ENUM_NYI     p_wei;           // Wind Erodibility Index 254, tons per acre per year wind_erodibility_index Wind_erodibility_index
   ENUM_NYI     p_weg;           // Wind Erodibility Group 254 Wind_erodibility_group
   ENUM_NYI     p_erocl;         // Erosion Class   254 Erosion_class
   ENUM_NYI     p_earthcovkind1; // Cover Kind 1  254 Earth_cover_kind_level_one
   ENUM_NYI     p_earthcovkind2; // Cover Kind 2  254 Earth_cover_kind_level_two
   ENUM_NYI     p_hydricon;      // Hydric Condition 254 Hydric_condition
   ENUM_NYI     p_hydricrating;  // Hydric Rating 254 Hydric_rating
   ENUM_NYI     p_drainagecl;    // Drainage Class 254 Drainage_class
   Param_number p_elev_l;        // Elevation   (prcsn 1) -300 8550 meters
   Param_number p_elev_r;
   Param_number p_elev_h;
   Param_number p_aspectccwise;  // Aspect Counter Clockwise   0 360 degrees
   Param_number p_aspectrep;     // Aspect Representative   0 360 degrees
   Param_number p_aspectcwise;   // Aspect Clockwise   0 360 degrees
   Param_propsC p_geomdesc;      //Geomorphic Description Vtext No Vtext
   Param_number p_albedodry_l;   // Albedo Dry  (prcsn 2) 0 1
   Param_number p_albedodry_r;
   Param_number p_albedodry_h;
   Param_number p_airtempa_l;    // MAAT  (prcsn 1) -50 50 degrees centigrade
   Param_number p_airtempa_r;
   Param_number p_airtempa_h;
   Param_number p_map_l;         // MAP  No 0 11500 millimeters
   Param_number p_map_r;
   Param_number p_map_h;
   Param_number p_reannualprecip_l; // REAP No 0 11500 millimeters
   Param_number p_reannualprecip_r;
   Param_number p_reannualprecip_h;
   Param_number p_ffd_l;      // Frost Free Days  0 365 days
   Param_number p_ffd_r;
   Param_number p_ffd_h;
   ENUM_NYI     p_nirrcapcl;  // Nirr LCC 254 Capability_class
   ENUM_NYI     p_nirrcapscl; //  Nirr Subcl Choice No 254 Capability_subclass
   Param_number p_nirrcapunit;// Nirr LCU  1 99
   //ENUM_NYI   p_irrcapcl;    // Irr LCC 254 Capability_class
   ENUM_NYI     p_irrcapscl;  // Irr Subcl Choice No 254 Capability_subclass
   Param_number p_irrcapunit;       // Irr LCU  No 1 99
   Param_number p_cropprodindex;    // Prod Index   0 100
   ENUM_NYI     p_constreeshrubgrp; //  Cons Tree Shrub Group   254 Conservation_tree_shrub_group
   ENUM_NYI     p_wndbrksuitgrp; // Windbreak Suitability (Obsolete)   254 Windbreak_suitability_group
   Param_number p_rsprod_l; // Range Prod  0 20000 pounds per acre per year
   Param_number p_rsprod_r;
   Param_number p_rsprod_h;
   ENUM_NYI     p_foragesuitgrpid; // Forage Suitability Group ID 11
   ENUM_NYI     p_wlgrain;       // Grain Habitat   254    Wildlife_rating
   ENUM_NYI     p_wlgrass;       // Grass Habitat   254   Wildlife_rating
   ENUM_NYI     p_wlherbaceous;  // Herbaceous Habitat   254  Wildlife_rating
   ENUM_NYI     p_wlshrub;       // Shrub Habitat   254  Wildlife_rating
   ENUM_NYI     p_wlconiferous;  // Conifer Habitat   254  Wildlife_rating
   ENUM_NYI     p_wlhardwood;    // Hardwood Habitat   254 Wildlife_rating
   ENUM_NYI     p_wlwetplant;    // Wetland Habitat   254  Wildlife_rating
   ENUM_NYI     p_wlshallowwat;  // Water Habitat   254    Wildlife_rating
   ENUM_NYI     p_wlrangeland;   // Rangeland Wildlife   254   Wildlife_rating
   ENUM_NYI     p_wlopenland;    // Openland Wildlife   254  Wildlife_rating
   ENUM_NYI     p_wlwoodland;    // Woodland Wildlife   254  Wildlife_rating
   ENUM_NYI     p_wlwetland;     // Wetland Wildlife   254   Wildlife_rating
   ENUM_NYI     p_soilslippot;   // Soil Slip Pot Choice  254 Soil_slippage_potential
   ENUM_NYI     p_frostact;      // Frost Action Choice  254 Potential_frost_action
   Param_number p_initsub_l;  // Init Subsid  0 999 centimeters
   Param_number p_initsub_r;
   Param_number p_initsub_h;
   Param_number p_totalsub_l; // Total Subsid  0 999 centimeters
   Param_number p_totalsub_r; // Total Subsid  0 999 centimeters
   Param_number p_totalsub_h; // Total Subsid  0 999 centimeters
   ENUM_NYI     p_hydgrp;        // Hydrologic Group   254 Hydrologic_group
   ENUM_NYI     p_corcon;        // Corrosion Concrete  254 Corrosion_concrete
   ENUM_NYI     p_corsteel;      // Corrosion Steel   254 Corrosion_uncoated_steel
   ENUM_NYI     p_taxclname;     // Taxonomic Class 120
   ENUM_NYI     p_taxorder;      // Order   254 Taxonomic_order
   ENUM_NYI     p_taxsuborder;   // Suborder 254 Taxonomic_suborder
   ENUM_NYI     p_taxgrtgroup;   // Great Group 254 Taxonomic_great_group
   ENUM_NYI     p_taxsubgrp;     // Subgroup 254 Taxonomic_subgroup
   ENUM_NYI     p_taxpartsize;   // Particle Size 254  Taxonomic_family_particle_size
   ENUM_NYI     p_taxpartsizemod;// Particle Size Mod 254 Taxonomic_family_part_size_mod
   ENUM_NYI     p_taxceactcl;    // CEC Activity Cl 254   Taxonomic_family_c_e_act_class
   ENUM_NYI     p_taxreaction;   // Reaction 254  Taxonomic_family_reaction
   ENUM_NYI     p_taxtempcl;     // Temp Class 254 Taxonomic_family_temp_class
   ENUM_NYI     p_taxmoistscl;   // Moist Subclass   254  //Taxonomic_moisture_subclass
   ENUM_NYI     p_taxtempregime; // Temp Regime 254 Taxonomic_temp_regime
   ENUM_NYI     p_soiltaxedition;// Keys to Taxonomy Edition Used 254 Soil_taxonomy_edition
   Param_number p_castorieindex; // CA Storie Index   0 100
   Param_propsC p_flecolcomnum;  // FL Ecol Comm #   5
   bool         flhe;            // FL HE  3       the boolean is current read into a string
   bool         flphe;           // FL PHE 3        the boolean is current read into a string
   ENUM_NYI     p_flsoilleachpot;// FL Leach Pot 254 FL_Soil_leaching_potential
   ENUM_NYI     p_flsoirunoffpot;// FL Runoff Pot   254 FL_Soil_runoff_potential
   bool         fltemik2use;     // FL Temik 3 bool   the boolean is current read into a string
   bool         fltriumph2use;   // FL Triumph   3          the boolean is current read into a string
   ENUM_NYI     p_indraingrp;    // IN Drainage Grp 3
   Param_number p_innitrateleachi;                 // IN NO3 Leach Index   0 99
   ENUM_NYI     p_misoimgmtgrp;  // MI Soil Mgmt Grp  254 MI_Soil_management_group
   ENUM_NYI     p_vasoimgtgrp;   // VA Soil Mgmt Grp 254 VA_Soil_management_group
   Param_propsC p_mukey;   // Mapunit Key   30
   Param_propsC p_cokey;  // Component Key   30
 public:
   Component_record();
   virtual bool expect_structure(bool for_write) ;
};
//______________________________________________________________________________
}//_namespace USDA_NRCS________________________________________________________/


#endif
