#include "organic_matter/OM_residues_profile_common.h"
#include "soil/soil_I.h"
#include "organic_matter/OM_pools_common.h"
#include "organic_matter/OM_common.h"
#include "soil/abiotic_environment.h"
#include "common/residue/residue_V4_common.h"
#include "corn/math/compare.hpp"
#include "soil/texture_I.h"
#include "soil/structure_I.h"
#include "organic_matter/OM_const.h"
#include "organic_matter/OM_params.h"
#include "weather/parameter/WP_air_temperature.h"

const Soil::Soil_interface *soil_hack;                                                 //111003
#ifndef MAXDOUBLE
#define MAXDOUBLE 9999999.9
#endif
//______________________________________________________________________________
Organic_matter_residues_profile_common::Organic_matter_residues_profile_common
(Soil::Soil_interface                     *soil_
,Soil::Abiotic_environment_profile   &soil_abiotic_environment_profile_
,modifiable_ CORN::Dynamic_array<float64> &evaporation_potential_remaining_      //160717
,const Air_temperature_average      &temperature_air_avg_                        //150427
,const Wind_speed                   &wind_speed_                                 //150427
,soil_horizon_array32               (horizon_percent_organic_matter_)            //111004
,Organic_matter_pools_common        *organic_matter_pools_profile_given_         //111004
,const Organic_matter_initialization_parameters  &OM_init_parameters_            //111004_060215
,const Organic_matter_decomposition_parameters   &OM_decomp_parameters_
,const CORN::date32                 &today_)                                     //140507
: Organic_matter_residues_profile_abstract(soil_,today_)
, soil_abiotic_environment_profile        (soil_abiotic_environment_profile_)    //070707
, organic_matter_pools_profile_owned      (organic_matter_pools_profile_given_)
, residues
   (*organic_matter_pools_profile_given_,*soil_,soil_abiotic_environment_profile_
   ,evaporation_potential_remaining_,temperature_air_avg_,wind_speed_)           //160412_111004
, organic_matter_decomposition_parameters (OM_decomp_parameters_)                //120912
{  soil_hack = soil_;                                                            //111004
      // This is temporary, the single pool SOM model needs the
      // full soil profile (for nitrogen)
      // Eventually make the soil interface a member
   initialize(horizon_percent_organic_matter_,OM_init_parameters_);              //120912_111004
}
//_constructor________________________________2007-07-07_2011-10-04_2006-02-15_/
bool Organic_matter_residues_profile_common::respond_to_field_operation
(float32                   tillage_op_depth_m
,float32                   tillage_op_mixing_fraction
,USDA::NRCS::RUSLE2::Soil_disturbance_effect   operation_soil_disturbance_effect // VB Operation_Soil_Disturbance_Effect
,const Soil::Disturbance   *soil_disturbance_      // may be 0 if no disturbances //060807
,const float32             residue_flatten_ratio[RUSLE2_RESIDUE_TYPE_COUNT]
,const float32             residue_burial_ratio[RUSLE2_RESIDUE_TYPE_COUNT])      // ManagementState.SurfaceResidueFractionRemainingAfterTillage
{  bool residues_result = residues.respond_to_field_operation
      (tillage_op_depth_m,operation_soil_disturbance_effect,residue_flatten_ratio,residue_burial_ratio);
   bool OM_result =
      organic_matter_pools_profile_owned->
      respond_to_field_operation(tillage_op_depth_m,tillage_op_mixing_fraction
      ,soil_texture,soil_disturbance_);                                          //060807
   return OM_result && residues_result;
}
//_respond_to_field_operation_______________________________________2006-07-27_/
const Soil::Disturbance *Organic_matter_residues_profile_common::know_soil_disturbance
(const Soil::Disturbance *_soil_disturbance)                        modification_
{  organic_matter_pools_profile_owned->know_soil_disturbance(_soil_disturbance);
   return _soil_disturbance;                                                     //070815
}
//_know_soil_disturbance____________________________________________2007-08-01_/
float64 Organic_matter_residues_profile_common::sum_carbon_to_depth_kg_m2
(nat8    include_OM
,float64 to_depth_m   // To depth of soil
,bool    include_surface ) const
{  float64 sum = 0.0;                                                            //070828
   if (to_depth_m > 0.000001)                                                    //080226
   {  if (CORN::is_approximately<float64>(to_depth_m,ENTIRE_PROFILE_DEPTH_as_9999,0.00001)) //071214
         sum += organic_matter_pools_profile_owned->get_mass_from_layer_to_layer
            (C_MASS_TOTAL,include_OM ,1,soil_layers.count(),1.0) ;
      else                                                                       //071214
      {  nat8  layer_at_depth = soil_layers.get_layer_at_depth(to_depth_m);      //080225
         float64 bottom_layer_thickness = soil_layers.get_thickness_m(layer_at_depth); //071214
         float64 above_bottom_layer_depth = (layer_at_depth > 1) ? soil_layers.get_depth_m(layer_at_depth-1) : 0; //071214
         float64 fraction_last_layer = (to_depth_m - above_bottom_layer_depth )/bottom_layer_thickness ;          //071214
         float64 prorate_fraction_bottom_layer = fraction_last_layer;            //071214
         sum += organic_matter_pools_profile_owned->get_mass_from_layer_to_layer
            (C_MASS_TOTAL,include_OM ,1,layer_at_depth,prorate_fraction_bottom_layer) ;
      }
   } // else if 0 then we must just want the surface.                           //080225
   if (include_surface)
      sum += organic_matter_pools_profile_owned->get_mass_from_layer_to_layer
         (C_MASS_TOTAL,include_OM,0,0,1.0);
   return sum;
}
//_sum_carbon_to_depth_kg_m2___________________________________________________/
#if (defined(MICROBASIN_VERSION) || defined (VIC_CROPSYST_VERSION))
float64 Organic_matter_residues_profile_common::sum_nitrogen_to_depth_kg_m2
(nat8    include_OM
,float64 to_depth_m   // To depth of soil
,bool    include_surface ) const
{  float64 sum = 0.0;                                                            //070828
   if (to_depth_m > 0.000001)                                                    //080226
   {  if (CORN::is_approximately<float64>(to_depth_m,ENTIRE_PROFILE_DEPTH_as_9999,0.00001)) //071214
         sum += organic_matter_pools_profile_owned->get_mass_from_layer_to_layer
            (N_MASS_TOTAL,include_OM ,1,soil_layers.count(),1.0) ;
      else                                                                       //071214
      {  nat8  layer_at_depth = soil_layers.get_layer_at_depth(to_depth_m);      //080225
         float64 bottom_layer_thickness = soil_layers.get_thickness_m(layer_at_depth); //071214
         float64 above_bottom_layer_depth
           = (layer_at_depth > 1) ? soil_layers.get_depth_m(layer_at_depth-1):0; //071214
         float64 fraction_last_layer = (to_depth_m - above_bottom_layer_depth )/bottom_layer_thickness ; //071214
         float64 prorate_fraction_bottom_layer = fraction_last_layer;            //071214
         sum += organic_matter_pools_profile_owned->get_mass_from_layer_to_layer
            (N_MASS_TOTAL,include_OM ,1,layer_at_depth,prorate_fraction_bottom_layer) ;
      }
   } // else if 0 then we must just want the surface.                           //080225
   if (include_surface)
      sum += organic_matter_pools_profile_owned->get_mass_from_layer_to_layer
         (N_MASS_TOTAL,include_OM,0,0,1.0);
   return sum;
}
#endif
//_sum_nitrogen_to_depth_kg_m2___________________________________2015-07-22LML_/
float64 Organic_matter_residues_profile_common::get_organic_matter_mass(nat8  layer)    const
{  // Can't use the totals because we need this by layer
   // so need to loop through all the pools and sum  mass (INCLUDE_SOM) (all SOMs and Microbial)
   return  organic_matter_pools_profile_owned->get_mass_from_layer_to_layer
      (BIOMASS_TOTAL,INCLUDE_SOM_AND_MICROBIAL,layer,layer,1.0) ;                //060331
}
//_get_organic_matter_mass__________________________________________2006-04-01_/
float64 Organic_matter_residues_profile_common::get_organic_carbon(nat8  layer, nat8  include_OM )  const
{  return  organic_matter_pools_profile_owned->get_mass_from_layer_to_layer
      (C_MASS_TOTAL,INCLUDE_SOM_AND_MICROBIAL,layer,layer,1.0) ;
}
//_get_organic_carbon__________________________________________________________/
float64 Organic_matter_residues_profile_common::sum_N_mineralization_to_depth
(nat8  include_OM
,float64 to_depth_m   // To depth of soil
,bool include_surface ) const
{
   bool to_profile_depth = CORN::is_approximately<float64>(to_depth_m,ENTIRE_PROFILE_DEPTH_as_9999,0.00001); //160527
   if (to_profile_depth) to_depth_m = soil_layers.get_depth_profile_m();         //160527
   nat8  layer_at_depth =
      to_profile_depth                                                           //160527
      ? soil_layers.count()                                                      //080225
      : soil_layers.get_layer_at_depth(to_depth_m);                              //080225
   float64 bottom_layer_thickness = soil_layers.get_thickness_m(layer_at_depth); //071214
   float64 above_bottom_layer_depth     = (layer_at_depth > 1) ? soil_layers.get_depth_m(layer_at_depth-1) : 0;//071214
   float64 fraction_last_layer =  (to_depth_m - above_bottom_layer_depth )/bottom_layer_thickness ;            //071214
   float64 prorate_fraction_bottom_layer = fraction_last_layer;                  //071214
   float64 sum = 0.0;
   sum += organic_matter_pools_profile_owned->get_mass_from_layer_to_layer(N_MINERALIZATION_TOTAL,include_OM ,1,layer_at_depth,prorate_fraction_bottom_layer) ;
   if (include_surface)
      sum += organic_matter_pools_profile_owned->get_mass_from_layer_to_layer(N_MINERALIZATION_TOTAL,include_OM,0,0,1.0);
   return sum;
}
//_sum_N_mineralization_to_depth____________________________________2006-11-16_/
float64 Organic_matter_residues_profile_common::sum_C_decomposition_to_depth
(nat8  include_OM
,float64 to_depth_m)                                                       const
// To depth of soil
{
   bool to_profile_depth = CORN::is_approximately<float64>(to_depth_m,ENTIRE_PROFILE_DEPTH_as_9999,0.00001); //160527
   if (to_profile_depth) to_depth_m = soil_layers.get_depth_profile_m();         //160527
   nat8  layer_at_depth =
      to_profile_depth                                                           //160527
      ? soil_layers.count()
      : soil_layers.get_layer_at_depth(to_depth_m);
   float64 bottom_layer_thickness = soil_layers.get_thickness_m(layer_at_depth);
   float64 above_bottom_layer_depth     = (layer_at_depth > 1) ? soil_layers.get_depth_m(layer_at_depth-1) : 0;
   float64 fraction_last_layer =  (to_depth_m - above_bottom_layer_depth )/bottom_layer_thickness ;
   float64 prorate_fraction_bottom_layer = fraction_last_layer;
   float64 sum = 0.0;
   sum += organic_matter_pools_profile_owned->get_C_decomposed_from_layer_to_layer
      (include_OM ,1,layer_at_depth,prorate_fraction_bottom_layer) ;
#ifdef NYI
090520 Currently this is only subsurface because the carbon report already has an accumulation of the surfaced C decomposition
   if (include_surface)
      sum += organic_matter_pools_profile_owned->get_C_decomposed_from_layer_to_layer(include_OM,0,0,1.0);
#endif
   return sum;
}
//_sum_C_decomposition_to_depth_____________________________________2009-05-20_/
float64 Organic_matter_residues_profile_common::sum_N_immobilization_to_depth
(nat8  include_OM
,float64 to_depth_m   // To depth of soil
,bool include_surface ) const
{
   bool to_profile_depth = CORN::is_approximately<float64>(to_depth_m,ENTIRE_PROFILE_DEPTH_as_9999,0.00001); //160527
   if (to_profile_depth) to_depth_m = soil_layers.get_depth_profile_m();         //160527
   nat8  layer_at_depth = to_profile_depth                                       //160527
      ? soil_layers.count()                                                      //080225
      : soil_layers.get_layer_at_depth(to_depth_m);                              //080225
   float64 sum = 0.0;
   for (nat8  layer = 1; layer <= layer_at_depth; layer++)                       //061218
      sum += organic_matter_pools_profile_owned->get_immobilization_N(layer);    //061218
   return sum;
}
//_sum_N_immobilization_to_depth____________________________________2006-11-16_/
float64 Organic_matter_residues_profile_common::get_N_immobilization_profile() const
{  float64 total_N_immobilization = organic_matter_pools_profile_owned->get_total_N_immobilized();
   return total_N_immobilization;
   // 110918 not sure if this is working correctly,
   // now using get_soil_organic_matter_immobilization_profile_N
}
//_get_N_immobilization_profile________________________________________________/
float64 Organic_matter_residues_profile_common::get_soil_organic_matter_mineralization_profile_N()     const
{ return organic_matter_pools_profile_owned->get_soil_organic_matter_mineralization_profile_N(); }
//______________________________________________________________________________
float64 Organic_matter_residues_profile_common::get_soil_organic_matter_immobilization_profile_N() const
{ return organic_matter_pools_profile_owned->get_soil_organic_matter_immobilization_profile_N(); }
//_get_soil_organic_matter_immobilization_profile_N_________________2011-09-18_/
float64 Organic_matter_residues_profile_common::get_soil_organic_matter_mineralization_N(nat8  layer)  const
{ return organic_matter_pools_profile_owned->get_soil_organic_matter_mineralization_N(layer); }
//______________________________________________________________________________
Pot_mineralizable_N_profile *Organic_matter_residues_profile_common::get_pot_mineralizable_N_profile()
{  return 0;}
// this version does not have  get_pot_mineralizable_N because OM mineralization is done a different way.
//______________________________________________________________________________
float64 Organic_matter_residues_profile_common::get_gaseous_CO2_C_loss_reported_daily(nat8  include_OM)  const
{  return organic_matter_pools_profile_owned->get_CO2_C_loss_reported_daily(include_OM);   }
//_get_gaseous_CO2_C_loss_reported_daily____________________________2006-04-16_/
float64 *Organic_matter_residues_profile_common::get_CO2_C_loss_profile_residue_and_OM()
{  return organic_matter_pools_profile_owned->get_CO2_C_loss_profile_residue_and_OM(); }
//__________________________________________________________________2006-05-04_/
float64 *Organic_matter_residues_profile_common::get_CO2_C_loss_profile_OM()
{  return organic_matter_pools_profile_owned->get_CO2_C_loss_profile_OM(); }
//__________________________________________________________________2008-09-02_/
float64 *Organic_matter_residues_profile_common::get_CO2_C_loss_profile_residue()
{  return organic_matter_pools_profile_owned->get_CO2_C_loss_profile_residue();}
//__________________________________________________________________2008-09-02_/
float64 Organic_matter_residues_profile_common::get_C_decomposed_from_residue(Organic_matter_position from_position) const
{  return organic_matter_pools_profile_owned->get_C_decomposed_from_residue(from_position); }
//__________________________________________________________________2006-08-17_/
float64 Organic_matter_residues_profile_common::get_C_incorporated_by_tillage() const
{  return organic_matter_pools_profile_owned->get_C_incorporated_by_tillage(); }
//__________________________________________________________________2006-08-17_/
float64 Organic_matter_residues_profile_common::get_mass_from_layer_to_layer
(Mass_total mass_total
,nat32 include_organic_matter_types
,nat8  from_layer
,nat8  to_layer
,float64 prorate_fraction_bottom_layer
)  const
{ return organic_matter_pools_profile_owned->get_mass_from_layer_to_layer(mass_total,include_organic_matter_types,from_layer,to_layer,prorate_fraction_bottom_layer) ; }
//_get_mass_from_layer_to_layer__________________________2007-14-14_2007-03-14_/
bool Organic_matter_residues_profile_common::start_day()
{  return organic_matter_pools_profile_owned->start_day();                       //061006
}
//__________________________________________________________________2006-08-17_/
bool Organic_matter_residues_profile_common::process_day()                       //150611
{  // Update environment
   bool processed = true;                                                        //090702
   residues.update_totals_sum_pools();
   float64 residue_water_hold = residues.get_curr_water_hold();
   soil_abiotic_environment_profile.update(residue_water_hold);
      // Actually air temp should be reference to abiotic envirionment
   processed &= organic_matter_pools_profile_owned->process();                   //090702
   // Additional processing for residues
   residues.exhaust_all();
   return  processed;
}
//_process_day______________________________________________________2006-01-16_/
float64 Organic_matter_residues_profile_common::get_NH4_mineralization()   const
{ return organic_matter_pools_profile_owned->get_soil_organic_matter_mineralization_profile_N(); }
//_get_NH4_mineralization___________________________________________2009-06-22_/
bool Organic_matter_residues_profile_common::initialize(
 soil_horizon_array32(percent_organic_matter)
   ,const Organic_matter_initialization_parameters &_organic_matter_initialization_parameters) //060215
{  for (nat8  lyr = 1; lyr <= soil_layers.count(); lyr++)
   {
      float64 sand_fraction = soil_texture.get_sand_percent(lyr) / 100.0;        //060418
      float64 thickness_m = soil_layers.get_thickness_m(lyr);
      int horizon = soil_layers.get_horizon_at_layer(lyr);
      float64 percent_OM = percent_organic_matter[horizon];
      float64 bulk_density_kg_m3 = g_cm3_to_kg_m3(soil_structure.get_bulk_density_g_cm3(lyr)); //070725
      float64 initial_organic_matter_carbon_mass = (percent_OM / 100.0)
            * carbon_fraction_in_organic_matter * bulk_density_kg_m3 * thickness_m;
      organic_matter_pools_profile_owned->initialize_layer(horizon,lyr
         ,sand_fraction                                                          //060418
         ,initial_organic_matter_carbon_mass
         ,organic_matter_decomposition_parameters.unadj_microbial_decomposition_const //060418
         ,_organic_matter_initialization_parameters                              //060215
         ,soil_abiotic_environment_profile.ref_soil_abiotic_environment_layer(lyr));
   }
/*NYI
CMB_RPT           balance_report_file << "<TABLE BORDER=1>" << endl;
CMB_RPT           balance_report_file << "<TR><TH>Date";
CMB_RPT           SOM_C_balancer.write_column_headers(balance_report_file);
CMB_RPT           residue_C_balancer.write_column_headers(balance_report_file);
CMB_RPT           balance_report_file << endl;
*/
   return true;
}
//(moved from Organic_matter_residues_profile_multiple_cycling)
//_initialize____________________________________________2011-10-04_2006-02-16_/
bool Organic_matter_residues_profile_common::clear_residues()    initialization_
{  return organic_matter_pools_profile_owned->clear_residues()
      &&  residues.clear_all();
}
//_clear_residues___________________________________________________2016-01-26_/
bool // nat32
Organic_matter_residues_profile_common::write(std::ostream &stream)   stream_IO_
{
/*170302 use write_header
   stream
   << "type\t"
   << "position\t"
   << "layer\t"
   << "detrition\t"
   // The following are Biomass_abstract and superclasses
   // I dont currently have read/write for these classes
   << "C_fraction\t"
   << "C/N\t"
   << "decomposition\t"
   << "amount(" << "kg" <<")\t"
   // residue only
   << "C_cycling\t"
   << "residue_type\t"
   << "area/mass\t";
   stream << std::endl;
*/
   nat32 written = 0;                                                            //150728
   if (organic_matter_pools_profile_owned)
   FOR_EACH_IN(OM_pool, Organic_biomatter_common,(*organic_matter_pools_profile_owned),each_pool)
   {
      if (!written) { OM_pool->write_header(stream); stream << std::endl; }
      OM_pool->write(stream);
      stream << std::endl;
      written++;                                                                 //150728
   } FOR_EACH_END(each_pool)
   return written;                                                               //150728
}
//_write____________________________________________________________2012-09-09_/
bool Organic_matter_residues_profile_common::read
(std::istream &stream)                                                stream_IO_
{  // the list must exist before calling read.
   bool read_it = organic_matter_pools_profile_owned->read(stream);
   FOR_EACH_IN(OM_pool,Organic_biomatter_pool_common
              ,(*organic_matter_pools_profile_owned), each_org_BM)
   {  if (OM_pool->is_straw_or_manure_residue())
      {  // not checking if contribution is not setup yet
         // because should always be the case after reading.
          residues.setup_contribution(*OM_pool);
      }
   } FOR_EACH_END(each_org_BM)
   return read_it;
}
//_read________________________________________________________________________/
RENDER_INSPECTORS_DEFINITION(Organic_matter_residues_profile_common)
{
   #ifdef CS_OBSERVATION
   // uses LBF context                                                           //160616
   if (organic_matter_pools_profile_owned)
       organic_matter_pools_profile_owned->render_inspectors(inspectors,context,"SOM");
   residues.render_inspectors(inspectors,context,"residues");
   #endif
   return 0; // emanator;
}
//_RENDER_INSPECTORS_DEFINITION________________________________________________/

