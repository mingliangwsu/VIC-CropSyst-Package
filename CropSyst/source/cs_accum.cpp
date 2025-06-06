/*
   Period accumulators for variables used in output
*/

#include <math.h>
#ifndef compareHPP
#  include"corn/math/compare.hpp"
#endif
#include "cs_accum.h"
#include "cs_ET.h"
#include "cs_mgmt.h"
#include "organic_matter/OM_residues_profile_abstract.h"
#include "common/residue/residues_interface.h"
#include "common/residue/residue_decomposition_param.h"
#include "crop/crop_N_common.h"
#include "soil/runoff.h"
#include "soil.h"
#include "soil/infiltration_I.h"
#include "soil/nitrogen_profile.h"
#include "crop/crop_interfaced.h"
#if (CROPSYST_VERSION >=5)
#  include "common/weather/weather_provider.h"
#else
#  include "common/weather/weather.h"
#endif
#ifdef CHEM_PROFILE
#   include "soil/chemical_profile.h"
#   include "soil/chemicals_profile.h"
#   include "soil/chemical_balance.h"
#endif
#include "land_unit_sim.h"
#include "soil/soil_evaporator.h"
//______________________________________________________________________________
using namespace std;
/*200813
obsolete
#include <q_n_d.h>
*/
extern float64 today_OM_mineralized_kgN_m2 ;
namespace CropSyst {
//______________________________________________________________________________
float32 key_profile_sum_depths[key_profile_sum_depths_COUNT]
= {ENTIRE_PROFILE_DEPTH_as_9999,0.05,0.10,0.15,0.30,0.60,0.90};
//______________________________________________________________________________
Annual_accumulators::Annual_accumulators
   (CropSyst::Land_unit_simulation_VX & _simulation
   ,float64 _init_total_water_content
   ,float64 _initial_ponded)
: Period_accumulators
   (_simulation
   ,_init_total_water_content
   ,_initial_ponded)
{}
//_Annual_accumulators:constructor_____________________________________________/
Water_balance_accumulators::Water_balance_accumulators
(CropSyst::Land_unit_simulation_VX  &simulation_
,float64                   init_total_water_content_
,float64                   initial_ponded_)
:simulation                                                        (simulation_)
#ifndef VIC_CROPSYST_VERSION
,pond                                                          (simulation.pond) //160408
#endif
,run_nitrogen       (simulation.scenario_control_and_model_options.run_nitrogen) //190709
,init_total_water_content                            (init_total_water_content_)
,water_depletion                                                           (0.0)
,initial_ponded                                                (initial_ponded_)
,eff_precipitation                                                         (0.0)
,irrigation                                                                (0.0)
,runoff                                                                    (0.0)
,act_transpiration                                                         (0.0)
,act_uptake                                                                (0.0)
,crop_intercept                                                            (0.0) //160419
,residue_interception                                                      (0.0)
,act_residue_evaporation                                                   (0.0)
,act_soil_evaporation                                                      (0.0)
,act_pond_evaporation                                                      (0.0) //160724
,water_table_input                                                         (0.0) //981019
,balance_drainage                                                          (0.0) //990106
,balance_microirrigation_excess                                            (0.0) //200309
,reported_drainage                                                         (0.0) //990719
#ifdef WSHEDMOD
,lateral_flow_in                                                           (0.0)
,lateral_flow_out                                                          (0.0)
#endif
,finite_diff_error                                                         (0.0)
,water_balance                                                             (0.0)
,water_balance_last                                                        (0.0) //151225
,water_cont_recalib                                                        (0.0) //000313
{}
//_Water_balance_accumulators:constructor______________________________________/
Period_accumulators::Period_accumulators
   (CropSyst::Land_unit_simulation_VX &_simulation                               //010202
   ,float64                  _init_total_water_content
   ,float64                  _initial_ponded)
:Water_balance_accumulators                                                      //990311
   (_simulation
   ,_init_total_water_content
   ,_initial_ponded)
,gaseous_N_loss                                                            (0.0)
,evapotranspiration_max                                                    (0.0)
,pot_residue_evaporation                                                   (0.0)
,evapotranspiration_act                                                    (0.0)
,evapotranspiration_ref                                                    (0.0) //130723
,pot_transpiration                                                         (0.0)
,pot_soil_evaporation                                                      (0.0)
,act_water_entering_soil                                                   (0.0) //990616
#ifdef MICROBASIN_VERSION
,water_stress_index                                                        (0.0) //161101LML
,nitrogen_stress_index                                                     (0.0) //161101LML
#endif
,N_nitrification                                                           (0.0) //170331LML
,N_denitrification                                                         (0.0) //170207LML
,soil_infiltration                                                         (0.0)
,total_N_applied                                                           (0.0) //160323
,inorganic_N_applied                                                       (0.0)
,organic_N_applied                                                         (0.0)
,NH3_volatilization_loss                                                   (0.0) //021212
,inorganic_NH4_to_atmosphere                                               (0.0)
,N_fert_requirements                                                       (0.0)
,N_fixation                                                                (0.0)
,N_uptake                                                                  (0.0) //120514
,straw_mineralization_V4_1                                                 (0.0)
,manure_mineralization_V4_1                                                (0.0)
,OM_N_mineralization                                                       (0.0) //110918
,N_immobilization                                                          (0.0) //040209
,biomass_grazed                                                            (0.0) //040209
,biomass_unsatisfied                                                       (0.0) //060327
,gaseous_CO2_C_loss_OM                                                     (0.0) //060426
,gaseous_CO2_C_loss_SOM                                                    (0.0) //060811
,gaseous_CO2_C_loss_residue                                                (0.0) //060811
,C_decomposed_SOM_profile                                                  (0.0) //090520
,C_decomposed_SOM_5cm                                                      (0.0) //090520
,C_decomposed_SOM_10cm                                                     (0.0) //090520
,C_decomposed_SOM_15cm                                                     (0.0) //090520
,C_decomposed_SOM_30cm                                                     (0.0) //090520
,C_decomposed_SOM_60cm                                                     (0.0) //090520
,C_decomposed_SOM_90cm                                                     (0.0) //090520
,C_decomposed_residue_profile                                              (0.0) //090520
,C_decomposed_residue_5cm                                                  (0.0) //090520
,C_decomposed_residue_10cm                                                 (0.0) //090520
,C_decomposed_residue_15cm                                                 (0.0) //090520
,C_decomposed_residue_30cm                                                 (0.0) //090520
,C_decomposed_residue_60cm                                                 (0.0) //090520
,C_decomposed_residue_90cm                                                 (0.0) //090520
,C_decomposed_residue_flat                                                 (0.0) //060811
,C_decomposed_residue_stubble                                              (0.0) //060811
,C_decomposed_residue_attached                                             (0.0) //060811
,C_incorporated_by_tillage                                                 (0.0) //060811
,C_removed_by_harvest                                                      (0.0) //060811
,plant_C_mass_canopy                                                       (0.0) //060817
,plant_C_mass_roots                                                        (0.0) //060817
,N_mass_yield                                                              (0.0) //120117
,N2O_loss_denitrification                                                  (0.0) //06060X
,N2O_loss_nitrification                                                    (0.0) //090722
,precip_m                                                                  (0.0)
,air_temperature_stats                                (STAT_arithmetic_mean_bit) //160324
{
//180108 temp restored    #ifndef CS_INSPECTOR_VERSION
      //170326 Now mineralization vector is stored,
      // this output is now processed from collator using new strata classes
   for (nat8 i = 0; i < key_profile_sum_depths_COUNT; i++)                       //160629
   {
      N_OM_mineralization[i] = 0;
      N_OM_immobilization[i] = 0;
   }
//180108 temp restored    #endif
}
//_Period_accumulators:constructor_____________________________________________/
void Period_accumulators::accumulate()
{  Water_balance_accumulators::accumulate();
   CropSyst::Crop_model_interface *crop = simulation.get_crop();                 //200404
      //200404 (simulation.get_crop_active_or_at_end_of_season());                        //031119_131008NS
   // need to use crop_for_output because we may have already set the new
   // crop but we still need to output today's results.
   Organic_matter_residues_profile_abstract * organic_matter_residues_profile_kg_m2 = simulation.organic_matter_residues_profile_kg_m2; //060628

   // Claudio says these are crop specific
   evapotranspiration_max     += simulation.output_crop_evapotransp_max_m;       //160401
   evapotranspiration_act     += simulation.output_act_crop_evapotransp_m;       //160323
   #if ((CROPSYST_PROPER==4)  || defined(OFOOT_VERSION))
   evapotranspiration_ref     += simulation.ET_ref->get_m();                     //180416
   #else
   #ifndef VIC_CROPSYST_VERSION
   evapotranspiration_ref     += simulation.
      meteorology.ref_ET_reference().get_m();                                    //160731
   #endif
   #endif
   const Residues_interface *residues_kg_m2 = organic_matter_residues_profile_kg_m2 ? organic_matter_residues_profile_kg_m2->ref_residues() : 0;
   if (residues_kg_m2)                                                           //060628
   {  pot_residue_evaporation    += residues_kg_m2->get_pot_evaporation();       //060628
      act_residue_evaporation    += residues_kg_m2->get_act_evaporation();
      residue_interception       += residues_kg_m2->get_interception();
   }
   precip_m               += simulation.meteorology.ref_precipitation_actual().get_m(); //151029_151130
   air_temperature_stats.append(simulation.meteorology.ref_air_temperature_avg().Celcius()); //160324
   biomass_grazed             += crop && crop->get_biomass_fate_today_record() ? crop->get_biomass_fate_today_record()->get_removed_for_grazing()  : 0.0;//040306
   biomass_unsatisfied        += simulation.unsatisfied_harvest_BM_today_kg_m2;  //060327
   if (organic_matter_residues_profile_kg_m2)                                    //060817
   {
      gaseous_CO2_C_loss_OM        +=   organic_matter_residues_profile_kg_m2->get_gaseous_CO2_C_loss_reported_daily(INCLUDE_ALL_ORGANIC_MATTER) ;       //060426

//cout << "CO2 C loss:" <<  per_m2_to_per_ha(gaseous_CO2_C_loss_OM)<< endl;

      gaseous_CO2_C_loss_SOM       +=   organic_matter_residues_profile_kg_m2->get_gaseous_CO2_C_loss_reported_daily(INCLUDE_SOM_AND_MICROBIAL) ;  //060811
      gaseous_CO2_C_loss_residue   +=   organic_matter_residues_profile_kg_m2->get_gaseous_CO2_C_loss_reported_daily(INCLUDE_RESIDUE_OM) ;         //060811
      C_decomposed_SOM_profile  += organic_matter_residues_profile_kg_m2->sum_C_decomposition_to_depth(INCLUDE_SOM_ONLY,ENTIRE_PROFILE_DEPTH_as_9999);   //090520
      C_decomposed_SOM_5cm   += organic_matter_residues_profile_kg_m2->sum_C_decomposition_to_depth(INCLUDE_SOM_ONLY,0.05);                              //090520
      C_decomposed_SOM_10cm  += organic_matter_residues_profile_kg_m2->sum_C_decomposition_to_depth(INCLUDE_SOM_ONLY,0.10);                              //090520
      C_decomposed_SOM_15cm  += organic_matter_residues_profile_kg_m2->sum_C_decomposition_to_depth(INCLUDE_SOM_ONLY,0.15);                              //090520
      C_decomposed_SOM_30cm  += organic_matter_residues_profile_kg_m2->sum_C_decomposition_to_depth(INCLUDE_SOM_ONLY,0.30);                              //090520
      C_decomposed_SOM_60cm  += organic_matter_residues_profile_kg_m2->sum_C_decomposition_to_depth(INCLUDE_SOM_ONLY,0.60);                              //090520
      C_decomposed_SOM_90cm  += organic_matter_residues_profile_kg_m2->sum_C_decomposition_to_depth(INCLUDE_SOM_ONLY,0.90);                              //090520
      C_decomposed_residue_profile  += organic_matter_residues_profile_kg_m2->sum_C_decomposition_to_depth(INCLUDE_RESIDUE_OM,ENTIRE_PROFILE_DEPTH_as_9999); //090520
      C_decomposed_residue_5cm   += organic_matter_residues_profile_kg_m2->sum_C_decomposition_to_depth(INCLUDE_RESIDUE_OM,0.05);               //090520
      C_decomposed_residue_10cm  += organic_matter_residues_profile_kg_m2->sum_C_decomposition_to_depth(INCLUDE_RESIDUE_OM,0.10);               //090520
      C_decomposed_residue_15cm  += organic_matter_residues_profile_kg_m2->sum_C_decomposition_to_depth(INCLUDE_RESIDUE_OM,0.15);               //090520
      C_decomposed_residue_30cm  += organic_matter_residues_profile_kg_m2->sum_C_decomposition_to_depth(INCLUDE_RESIDUE_OM,0.30);               //090520
      C_decomposed_residue_60cm  += organic_matter_residues_profile_kg_m2->sum_C_decomposition_to_depth(INCLUDE_RESIDUE_OM,0.60);               //090520
      C_decomposed_residue_90cm  += organic_matter_residues_profile_kg_m2->sum_C_decomposition_to_depth(INCLUDE_RESIDUE_OM,0.90);               //090520

      C_decomposed_residue_flat        += organic_matter_residues_profile_kg_m2->get_C_decomposed_from_residue(OM_flat_surface_position);       //060811
      C_decomposed_residue_stubble     += organic_matter_residues_profile_kg_m2->get_C_decomposed_from_residue(OM_stubble_surface_position);    //060811
      C_decomposed_residue_attached    += organic_matter_residues_profile_kg_m2->get_C_decomposed_from_residue(OM_attached_to_plant_position);  //060811
      C_incorporated_by_tillage        += organic_matter_residues_profile_kg_m2->get_C_incorporated_by_tillage();                               //060811
   }
   const CropSyst::Soil_components *soil = simulation.ref_soil();
   if (soil)                                                                     //060803
   {  soil_infiltration          += soil->get_infiltration();
      act_water_entering_soil    += soil->get_act_water_entering_soil();         //990716
      reported_drainage          += soil->get_reported_drainage();               //990106
      #ifndef VIC_CROPSYST_VERSION
      pot_soil_evaporation       += soil->ref_evaporator()->output_evaporation_potential_daily; //160719
      act_soil_evaporation       += soil->ref_evaporator()->output_evaporation_actual_daily;    //160719
      #endif
   }
   #ifndef VIC_CROPSYST_VERSION
   act_pond_evaporation += simulation.pond.output_evaporation_actual_daily;      //160724
   #endif
   if (crop)                                                                     //060817
   {  pot_transpiration          += crop->get_pot_transpiration_m(CO2_NONE);     //060817
      #ifdef MICROBASIN_VERSION
      water_stress_index         += crop->get_water_stress_index();              //161101LML
      #endif
      // conceptual added accumulator for CO2 adjusted
      {
         #ifdef RESIDUES
         const Residue_decomposition_parameters &decomp_params = crop->get_residue_decomposition_parameters(); //160708
         float64 crop_residue_carbon_fraction = decomp_params.carbon_fraction;   //140910_060817
         #else
         float64 crop_residue_carbon_fraction = default_straw_residue_fraction_carbon; //140910
         #endif
         C_removed_by_harvest += crop->get_latest_yield_kg_m2() * crop_residue_carbon_fraction; //140910_060817
      }
      crop_intercept          += crop->get_intercepted_precipitation_m();        //160419
/*
if (crop_intercept >0 )
std::clog << "reached" << std::endl;
*/
      plant_C_mass_canopy     += crop->get_C_mass_canopy();                      //060818
      plant_C_mass_roots      += crop->get_C_mass_roots();                       //060818
      const Crop_nitrogen_interface *crop_N_kg_m2 = crop->get_nitrogen();        //050722
      #ifdef NITROGEN
      if (crop_N_kg_m2)                                                          //050722
      {  gaseous_N_loss       += crop_N_kg_m2->get_gaseous_loss();               //050722
         N_fert_requirements  += crop_N_kg_m2->get_fertilization_requirements(); //050722
         N_fixation           += crop_N_kg_m2->get_fixation();                   //050722
          {  // moved from daily report                                          //120514
   const Soil::NO3_Profile *NO3 =(soil&&soil->chemicals)?soil->chemicals->NO3:0; //120514
   const Soil::NH4_Profile *NH4 =(soil&&soil->chemicals)?soil->chemicals->NH4:0; //120514
             if (NO3 && NH4)                                                     //120514
             {
             #ifdef CROPSYST_PROPER
             //190130 Disabled by RLN, but LML may be using this for output
             float64 curr_NO3_uptake_E = NO3->molecular_to_elemental*(NO3->GS_bal_accums_M->uptake); //120514
             float64 curr_NH4_uptake_E = NH4->molecular_to_elemental*(NH4->GS_bal_accums_M->uptake); //120514
             N_uptake = curr_NO3_uptake_E + curr_NH4_uptake_E ;                  //120514
             #endif
             #ifdef MICROBASIN_VERSION
             nitrogen_stress_index += crop->get_nitrogen()->get_stress_index_mean();//161101LML
             #endif
             }
         }
         N_mass_yield += crop->ref_reported().season.N_mass_yield;               //190705_160628
      }
      #endif
   }
   #ifdef NITROGEN
   if (run_nitrogen)                                                             //050114
   {
      N2O_loss_denitrification   += soil->chemicals->NO3->get_N2O_loss();        //060606
      N2O_loss_nitrification     += soil->chemicals->NH4->get_N2O_loss();        //090722
      total_N_applied            += simulation.nitrogen_daily_output.applied_total;    //160323
      inorganic_N_applied        += simulation.nitrogen_daily_output.applied_inorganic;//160323
      organic_N_applied          += simulation.nitrogen_daily_output.applied_organic;  //131004
      NH3_volatilization_loss    += simulation.nitrogen_daily_output.volatilization_loss_NH3;         //131004
      inorganic_NH4_to_atmosphere+= simulation.nitrogen_daily_output.inorganic_atomospheric_loss_NH4; //131004
      N_nitrification            += soil->chemicals->NH4->nitrified_N_output.sum();    //170503RLN
      N_denitrification          += soil->chemicals->NO3->denitrified_N_output.sum();  //170503RLN
      if (organic_matter_residues_profile_kg_m2)
      {
         OM_N_mineralization        +=  organic_matter_residues_profile_kg_m2->get_soil_organic_matter_mineralization_profile_N(); //110918
         straw_mineralization_V4_1  +=  organic_matter_residues_profile_kg_m2->get_total_straw_mineralization();    // deprecated
         N_immobilization           +=  organic_matter_residues_profile_kg_m2->get_soil_organic_matter_immobilization_profile_N(); //110918
         manure_mineralization_V4_1 +=  organic_matter_residues_profile_kg_m2->get_total_manure_mineralization();   // deprecated
         //180108 temp restored          #ifndef CS_INSPECTOR_VERSION
         for (nat8 i = 0; i < key_profile_sum_depths_COUNT; i++)                 //160629
         {
            N_OM_mineralization[i]
               += organic_matter_residues_profile_kg_m2
                  ->sum_N_mineralization_to_depth
                     (INCLUDE_RESIDUE_OM|INCLUDE_SOM_AND_MICROBIAL
                     ,key_profile_sum_depths[i],true);                           //061116
            N_OM_immobilization[i]
               += organic_matter_residues_profile_kg_m2
                  ->sum_N_immobilization_to_depth
                     (INCLUDE_RESIDUE_OM|INCLUDE_SOM_AND_MICROBIAL
                     ,key_profile_sum_depths[i],true);                           //061116
         }
         //180108 temp restored          #endif
      }
   }
   #endif
}
//_Period_accumulators::accumulate_____________________________________________/
void Water_balance_accumulators::accumulate()
//190709 (const CORN::Date_const & today,bool run_nitrogen)
{  eff_precipitation += simulation.daily_liquid_precipitation;
   irrigation        += simulation.total_irrigation_today;                       //050428
   CropSyst::Crop_model_interface *crop = simulation.crop_active_or_intercrop;   //070627_031119 //131008NS
   if (crop)                                                                     //070627
   {
      act_transpiration += crop->get_act_transpiration_m();
      act_uptake += crop->get_act_uptake_m();                                    //160414
   }
   const CropSyst::Soil_components *soil = 0;
   #ifndef VIC_CROPSYST_VERSION
   soil = simulation.ref_soil();                                                 //060803
   #endif
   if (soil)                                                                     //060803
   {
#ifdef RUNOFF
      runoff   += simulation.runoff ? simulation.runoff->ref_estimated_runoff() : 0.0; //200316
#endif
#if (defined(CROPSYST_PROPER) || defined(OFOOT_VERSION))
      water_table_input += soil->infiltration_submodel->get_water_table_input(); //981019
      balance_drainage  += soil->infiltration_submodel->get_balance_drainage();  //070118 soil->balance_drainage;
      balance_microirrigation_excess += soil->infiltration_submodel->get_microirrigation_excess(); //200309
#endif
#ifdef WSHEDMOD
      lateral_flow_in   += soil->lateral_flow_in;                                //990525
      lateral_flow_out  += soil->lateral_flow_out;                               //990525
#endif
   }
}
//_Water_balance_accumulators::accumulate___________________________1999-03-11_/
void Water_balance_accumulators::compute_water_balance
(float64 total_soil_water_content_today
,float64 daily_finite_diff_error_today
,float64 deferred // F.D.                                                        //071024
,bool    main_water_balance                                                      //000626
,bool    show_water_balance_errors)                                              //051115
{  water_depletion = init_total_water_content - total_soil_water_content_today;
   finite_diff_error += daily_finite_diff_error_today;
   float64 water_bal = 0.0;
   #ifndef VIC_CROPSYST_VERSION
   float64 ponded             = pond.get_depth();                                //160408
   #endif
   water_bal =
         + water_cont_recalib
         + eff_precipitation
         + irrigation
         + water_table_input                                                     //981019
         + water_depletion
         #ifdef WSHEDMOD
         + lateral_flow_in                                                       //990525
         - lateral_flow_out                                                      //990525
         #endif

         + crop_intercept                                                        //160419

         - residue_interception
         - runoff
         - act_uptake                                                            //160414
         - act_soil_evaporation                                                  //051013
         - balance_drainage                                                      //990606
         - balance_microirrigation_excess                                        //200309
         - deferred                                                              //071024
         - act_pond_evaporation                                                  //170110
         - ponded;

/*
std::clog << "WB:" << water_bal << std::endl;

if (water_bal > 0.0001)
std::clog << "reached" << std::endl;



if (!CORN::is_zero<float64>(water_bal,0.0001) )
std::cout
<< std::endl
<< simulation.simdate_raw
<< "\twb:\t" << water_bal
<< "\t+wcr:\t" << water_cont_recalib
<< "\t+pre:\t" << eff_precipitation
<< "\t+irr:\t" << irrigation
<< "\t+wti:\t" << water_table_input
<< "\t+wdp:\t" << water_depletion
//<< "\t-cintS:\t" << crop_intercept_stored
<< "\t-cintT:\t" << crop_intercept
//160414 << "\t-cint:\t" << crop_intercept
<< "\t-ri:\t" << residue_interception
<< "\t-ro:\t" <<    runoff
//160414 << "\t-at:\t" <<    act_transpiration
<< "\t-au:\t" <<    act_uptake                                                   //160414
<< "\t-ase:\t" << act_soil_evaporation
<< "\t-bd:\t" << balance_drainage
<< "\t-def:\t" << deferred
<< "\t-pnd:\t" << ponded;
*/

   if (CORN::is_zero<float64>(water_bal,0.00000001) && act_uptake > 0.00001 )
          water_bal = 0;
   float64  accum_pot_water_entering_soil =
         eff_precipitation + irrigation;                                         //990309
   if (water_depletion > 0.0)
      accum_pot_water_entering_soil += water_depletion;                          //990716
   float64 acceptable_error = std::max<float64>(0.005,accum_pot_water_entering_soil*0.01);//010530
    float64 abs_balance_to_check = fabs(water_bal);
/*
if (reported_drainage > 0.0)
std::cout << "RD:" << reported_drainage << std::endl;
*/

    if (main_water_balance)
    { if ((abs_balance_to_check > acceptable_error)                              //990223
         && show_water_balance_errors
         && !CORN::is_zero<float64>(water_balance, water_balance_last))          //051115
      {
/*
{
std::cerr <<std::endl<< ("Water balance") <<  water_bal << std::endl;
std::clog
<< std::endl
<< "wb:\t" << water_bal
<< "\t+wcr:\t" << water_cont_recalib
<< "\t+pre:\t" << eff_precipitation
<< "\t+irr:\t" << irrigation
<< "\t+wti:\t" << water_table_input
<< "\t+wdb:\t" << water_depletion
//160414 << "\t-cint:\t" << crop_intercept
//160414 << "\t-cintS:\t" << crop_intercept_stored
<< "\t-cintT:\t" << crop_intercept_today
<< "\t-ri:\t" << residue_interception
<< "\t-ro:\t" <<    runoff
//160414 << "\t-at:\t" <<    act_transpiration
<< "\t-au:\t" <<    act_uptake                                                   //160414
<< "\t-ase:\t" << act_soil_evaporation
<< "\t-bd:\t" << balance_drainage
<< "\t-def:\t" << deferred
<< "\t-pnd:\t" << ponded << std::endl;;
}
*/
          std::cerr <<std::endl<<  ("Water balance:") <<  water_bal << std::endl;
      }
    }
    water_balance_last = water_bal;
    water_balance = water_bal;                                                   //990222
}
//_compute_water_balance____________________________________________1999-03-11_/
}//_namespace CropSyst_________________________________________________________/

