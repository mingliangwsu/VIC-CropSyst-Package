
#ifdef __cplusplus
extern "C" {
#endif
#include "VIC_soil_hydrology.h"

#ifdef __cplusplus
}
#endif
#if VIC_CROPSYST_VERSION==2
#include "crop/VIC_crop_C_interface.h"
#elif VIC_CROPSYST_VERSION==3
#include "agronomic/VIC_land_unit_C_interface.h"
#include "agronomic/VIC_land_unit_simulation.h"
#include "CropSyst/source/management/management_param_struct.h"
#endif
#include <stdio.h>
#include <stdlib.h>
//#include <vicNl.h>
#include <math.h>
#include <assert.h>
#include <irrigation_lib.h> //20130502 keyvan
#ifdef VCS_V5
#include "vic_def.h"
#include "VIC_soil.h"                                                            //170831LML
#include "VIC_crop_variable_codes.h"                                             //170831LML
#include "VIC_land_unit_C_interface.h"                                           //170831LML
#include "VCS_Nl_v5.h"                                                           //170901LML
#else
#include "vicNl_def.h"
#include "VCS_Nl.h"
#endif
#include "VIC_crop_variable_codes.h"
#ifdef VIC_CROPSYST_VERSION
extern layer_data_struct  *layer_wet_global;
extern option_struct   options;
extern crop_data_struct *crop_global;
extern soil_con_struct *soil_con_global;
extern double VIC_CropSyst_get_return;
extern global_param_struct global_param;
#ifndef FULL_IRRIGATION
extern std::map<int,std::map<int,irrigation_pattern_struct>> girrig_patt;       //irrigation_pattern_struct *girrig_patt;
#endif
bool need_irrigation(double MAD_crop_specific
                        ,double depletion_obs_depth_mm                            //180510LML
                        ,int rec
                        ,double irrigation_capacity_mm_T                         //(mm/T) maximum irrigated water flow into soil root zone
                        ,layer_data_struct *layer                          //150701LML
                        ,const soil_con_struct &soil_con                         //150701LML
                        ,double &irrigation_demand
#ifndef FULL_IRRIGATION
                        ,int rotation_code
#endif
                        ,bool fill_water_to_fc                                   //190724LML
                        ,double max_capacity                                     //190724LML
                        ,double *real_added_water                                //190724LML
                        ,double top_layer_to_fc_fraction                         //190805LML
                        ,double water_stress_index
                        ,double water_stress_tolerance
                        )
{
    bool irrigation = false;
    irrigation_demand = 0;                                                       //160509LML
    //irrigation_pattern_struct *irrig_pattern;
    //#ifdef FULL_IRRIGATION
    double accumulative_soil_depth  = 0.0;
    double maximum_available_water  = 0.0;
    double actual_available_water   = 0.0;
    double water_deficit_to_FC      = 0.0;
    //160104LML moved here
    //180510LML
    //180510LMLdouble root_depth_mm = VIC_CropSyst_get(
    //180510LML            #if (VIC_CROPSYST_VERSION>=3)
    //180510LML            VIC::
    //180510LML            #endif
    //180510LML            VC_root_depth_mm);

    double root_depth_mm = VIC_CropSyst_get(
                #if (VIC_CROPSYST_VERSION>=3)
                VIC::
                #endif
                VC_root_depth_mm);

    //180820 LML the irrigation only goes to the middle layers, i.e. aboue bottom layer
    int deepest_irrigation_layer = options.Nlayer - 2;
    int recharge_to_sublayer = CORN::is_approximately<float64>(root_depth_mm, 0.0, 0.00001) ?
                                deepest_irrigation_layer : (closest_layer_at(soil_con,mm_to_m(root_depth_mm)) + 1); //171204LML from CropSyst
    recharge_to_sublayer = CORN::must_be_less_or_equal_to(recharge_to_sublayer,deepest_irrigation_layer); //180612LML

    int obs_to_sublayer = CORN::is_approximately<float64>(depletion_obs_depth_mm, 0.0, 0.00001) ?
                                deepest_irrigation_layer : (closest_layer_at(soil_con,mm_to_m(depletion_obs_depth_mm)) + 1); //171204LML from CropSyst
    obs_to_sublayer = CORN::must_be_less_or_equal_to(obs_to_sublayer,deepest_irrigation_layer); //180612LML 180703

    if (recharge_to_sublayer < obs_to_sublayer) recharge_to_sublayer = obs_to_sublayer; //180703LML 180725 reenabled
    //recharge
    int from_layer = 0; //190724LML to be more real fill top layer also    fill_water_to_fc_exclude_top ? 1 : 0;                              //190724LML if perfect irrigation, only count layer bellow top layer
    for (int i = from_layer; i <= recharge_to_sublayer; i++) {
          //if (i == 0) water_deficit_to_FC += std::max<double>(soil_con.max_moist[i] - layer[i].moist, 0.0);   //190806LML at least make the top layer saturated?
          //else
        water_deficit_to_FC       += std::max<double>(/*190806LML soil_con.Wcr[i]*/ soil_con.VCS.Field_Capacity[i] - layer[i].moist, 0.0);

        //std::clog << "i:" << i
        //          << " FC:" << soil_con.VCS.Field_Capacity[i]
        //          << " moist:" << layer[i].moist
        //          << " water_deficit_to_FC:" << water_deficit_to_FC
        //          << std::endl;

    }

    //observe depletion
    for (int i = 0; i <= obs_to_sublayer; i++) {
          if (i != 0) {                                                          //171204LML top evaporation layer is not included in the soil depletion calculation
                                                                                 //180726LML ask Caludio to decide if it is okay or not.
             maximum_available_water   += std::max<double>(/*190806LML soil_con.Wcr[i]*/ soil_con.VCS.Field_Capacity[i] - soil_con.Wpwp[i], 0.0);
             double available_water_layer = std::min<double>(layer[i].moist - soil_con.Wpwp[i], /*190806LML soil_con.Wcr[i]*/ soil_con.VCS.Field_Capacity[i] - soil_con.Wpwp[i]); //180521LML
             actual_available_water    += std::max<double>(available_water_layer, 0.0);
          }
                                                                                 //180517LML if added all moisture (negtive and positive), particulay the top layer,
                                                                                 //the soil might be under-irrigated. BUT now it seems has some overestimation.
          //std::clog << "VIC_layer i:" << i
          //          << "\tLayer_Depth:" << soil_con.depth[i]
          //          << "\tSat:" << soil_con.max_moist[i]
          //          << "\tFC:" << /*190806LML soil_con.Wcr[i]*/ soil_con.VCS.Field_Capacity[i]
          //          << "\tmoist:" << layer[i].moist
          //          << "\tDeficit_to_FC:" << std::max<double>(/*190806LML soil_con.Wcr[i]*/ soil_con.VCS.Field_Capacity[i] - layer[i].moist, 0.0)
          //          << std::endl;

    }

    double depletion = 1.0 - actual_available_water / maximum_available_water;

    //std::clog << "depletion:" << depletion
    //          << "\twater_deficit_to_FC:" << water_deficit_to_FC
    //          << "\tirrigation_capacity_mm_T:" << irrigation_capacity_mm_T
    //          << "\tactual_available_water:" << actual_available_water
    //          << "\tmaximum_available_water:" << maximum_available_water
    //          << std::endl;


    //if (((depletion > MAD_crop_specific) || (water_deficit_to_FC > irrigation_capacity_mm_T)) && water_deficit_to_FC > 0.0) {    //190918 removed the
    //if (water_stress_index > 0.05)
    //    printf("water_stress_index:%lf depletion:%lf MAD_crop_specific:%lf\n",water_stress_index,depletion,MAD_crop_specific);

    if (((depletion > MAD_crop_specific) || (water_stress_index > water_stress_tolerance)) && water_deficit_to_FC > 0.0) {          //190918 LML removed the condition for irrigation capacity
        irrigation = true;
        irrigation_demand = water_deficit_to_FC;
        //printf("irrigation_demand:%lf\n",irrigation_demand);

        if (fill_water_to_fc) {
            double available_water_to_fill = max_capacity;
            *real_added_water = 0;
            for (int i = from_layer; i <= recharge_to_sublayer; i++) {
                //double target_moisture = (i == 0) ? soil_con.max_moist[i] : soil_con.VCS.Field_Capacity[i];
                double target_moisture = soil_con.VCS.Field_Capacity[i];
                double to_moisture = (i == 0) ? (top_layer_to_fc_fraction * /*190806LML soil_con.Wcr[i]*/ target_moisture) : /*190806LML soil_con.Wcr[i]*/ target_moisture; //190805LML
                double need = (layer[i].moist < to_moisture) ? (to_moisture - layer[i].moist) : 0;
                if (available_water_to_fill > need) {
                    layer[i].moist += need;
                    available_water_to_fill -= need;
                    *real_added_water += need;
                } else {
                    layer[i].moist += available_water_to_fill;
                    *real_added_water += available_water_to_fill;
                    available_water_to_fill = 0;
                }
            }
        }
    }
    //#else
    #ifndef FULL_IRRIGATION
    //today_is_deficit_irrigation()
    double amount_def_irr = amount_of_deficit_irrigation(rec,rotation_code);
    if (amount_def_irr >= 0) {                                                  //If == -1, not irrigation pattern, assume full irrigation.
        irrigation = amount_def_irr > 0;
        irrigation_demand = irrigation ? amount_def_irr : 0;
    }
    #endif

    //if (irrigation)
    //std::clog << "\tirrigation:"        << irrigation
    //          << "\tdepletion_obs_depth_mm:"         << depletion_obs_depth_mm
    //          << "\tdepletion:" << depletion
    //          << "\tirrigation_demand:" << irrigation_demand
    //          << "\trecharge_to_sublayer:" << recharge_to_sublayer
    //          << std::endl;

    return irrigation;
}
//______________________________________________________________________________
#ifndef FULL_IRRIGATION
double amount_of_deficit_irrigation(int rec, int rotation_code)
{
    if (girrig_patt.find(rotation_code) != girrig_patt.end() ) {
        if (girrig_patt[rotation_code].find(rec) != girrig_patt[rotation_code].end()) {
            irrigation_pattern_struct &irrig_pattern = girrig_patt[rotation_code][rec];
            return irrig_pattern.proration_rate >= 0 ?
               irrig_pattern.irrig_amount * irrig_pattern.proration_rate
               : irrig_pattern.irrig_amount * global_param.VCS.basin_wide_proration;
        } else {
            return 0.0;                                                         //if crop exist but no poration record, just set no irrigation
        }
    }
    return -1.0;                                                                //200917LML if the crop not in the irrigation table
};

#endif
//______________________________________________________________________________
#if (VIC_CROPSYST_VERSION < 3)
/**************************************************************
for implementing deficit irrigation
    130710
**************************************************************/

/*************************************************************
calculate evaporation from irrigation systems           keyvan
calculates the evaporation from irrigation systems based on
formula which was developed by Keyvan Malek,

30Nov2012 for now some dummy number are presented as an input set
            to the formula later we have to put better values
            for each irrigation system in the irrigation library
            file.                                       Keyvan
**************************************************************/

/*double evaporation_from_irrigation_systems(double ET0)
{
    double cd=1.1; //nuzzle coeficient
    double h=207.0; //irrigation system operating pressure kpa
    double g=9.81;

    double D=3.0; //average droplet size mm
    double teta=35.0; //angle of the nuzzle
    double x0=10.0; //diameter of sprinkler
    double y0_z=2.0; //height of the irrigation sprinkler from ground m
    double crop_h=0.5; //crop height m it should be impelemented from CropSyst
    double K_t=-0.07527; //empirical coefficient of the t term
    double k_D=0.31866; //empirical coefficient of the D term


    double Ap=0.07;//percentage of area which is covered by irrigation system at a time
    double v0=cd*pow((2.0*g*h/100.0),0.5); //initial velocity

    ***************************
    calcualte the evaporation from
    irrigation system   keyvan 121130
    ***************************
   double t_term=pow(((tan(teta)-(y0_z-crop_h)/x0)*v0*cos(teta)/g),K_t);
    double D_term=pow(1/D, k_D);
    double Ed=ET0*Ap*D_term*t_term;




   return Ed;
    // Return the current volumetric water content for the specified soil layer> *
};*/
#endif
//______________________________________________________________________________
#if (VIC_CROPSYST_VERSION<3)
//150701LML added macro
double VIC_soil_hydrology_get_water_content(unsigned char CropSyst_layer)
{  int VIC_layer_index = CropSyst_layer-1;
   double liquid_WC = (layer_wet_global[VIC_layer_index].moist - layer_wet_global[VIC_layer_index].ice)
                      / (1000.0 * soil_con_global->depth[VIC_layer_index]); // it's in fraction
   return liquid_WC;
   // Return the current volumetric water content for the specified soil layer>
}
#endif
//_2012-06-15_____________________________VIC_soil_hydrology_get_water_content_/
#if (VIC_CROPSYST_VERSION < 3)
double VIC_soil_hydrology_extract_water(double water_uptake_m[] ,unsigned char start_layer )
{  int VIC_layer = 0;
 //   extern layer_data_struct  layer;
   double total_u


   ptake_m = 0;
   for (VIC_layer=start_layer-1;VIC_layer<options.Nlayer;VIC_layer++)
   {
      int  CropSyst_layer = VIC_layer + 1;
      crop_global->transpiration[VIC_layer] += water_uptake_m[CropSyst_layer] * 1000.0;
      total_uptake_m += water_uptake_m[CropSyst_layer];
      // it is possible that there could be multiple extract water events
      //    this is why the transpiration  is incremented here.
      // VIC should be clearing the transpiration array at the start of the day.
     // layer[VIC_layer].evap=
   };
   return total_uptake_m;

}
/* 
   This function should removed the specified water update depth amounts
   for each of the soil layers
   Soil layers in the water_uptake_m array are indexed from 1
   so water_uptake_m[0] is not used and water_uptake_m[1]
   would be the evaporative layer.
   Roots start in layer 2 so I think  water_uptake_m[1] will always be 0.
   In any case, you should extract from all layers starting with the
   layer indicated by the start_layer value (this will usually be layer 2).

   This function should update the VIC soil water contents corresponding
   to the extracted water depth.

   Note that it is possible (but unlikely) that the water uptake in the layer
   exceeds the available water so the function should check for this case
   and limit that actual water uptake amount to the available water (depth).

   The function should accumulate the the actual water update amount
   and return this sum (in meter).
   Unless there is a limiting condition, the function will simply return
   the sum of the water_uptake_m array values.
*/


#endif
//_2012-06-15_________________________________VIC_soil_hydrology_extract_water_/
#if (VIC_CROPSYST_VERSION < 3)
int VIC_get_number_soil_layers()
{  return(options.Nlayer);
};
//_2012-06-15_______________________________________VIC_get_number_soil_layers_/
int VIC_get_max_number_soil_layers()
{  return(options.Nlayer);
};
//_2012-06-15___________________________________VIC_get_max_number_soil_layers_/
double VIC_get_water_potential(unsigned char CropSyst_layer)
{  int VIC_layer_index = CropSyst_layer -1;
//   double WC_at_33_or_FC = soil_con_global->Wcr[VIC_layer_index]; //wcr is the feild capacity
   double saturation_WC =1.0-soil_con_global->bulk_density[VIC_layer_index]/soil_con_global->soil_density[VIC_layer_index];  // between 0 and 1
   double Campbell_b = 2.0/(soil_con_global->expt[VIC_layer_index]-3.0);  // expt= 3+2/b ==> b= Campbell_b
   double WC = VIC_soil_hydrology_get_water_content(CropSyst_layer); // should be checked
   double clay_fract = soil_con_global->clay  [VIC_layer_index];
   double sand_fract = soil_con_global->quartz[VIC_layer_index];
   double clay_pcrnt = clay_fract * 100.0;
   double sand_pcrnt = sand_fract * 100.0;
   double A_Value = 100.0 * exp(-4.396 - 0.0715 * clay_pcrnt -   0.000488 * pow(sand_pcrnt,2.0) - 0.00004285 * pow(sand_pcrnt,2.0) * clay_pcrnt);
   double b_value = -(-3.14 - 0.00222 * pow(clay_pcrnt ,2.0) - 0.00003484 * pow(sand_pcrnt,2.0) * clay_pcrnt);
/*121116 was  (changed by RLN)
   double A_Value = 100.0 * exp(-4.396 - 0.0715 * clay - 0.000488 * pow(soil_con_global->quartz[VIC_layer_index]*100 ,2.0) - 0.00004285 * pow(soil_con_global->quartz[VIC_layer_index]*100.0 , 2.0) * soil_con_global->clay[VIC_layer_index]);
   double b_value = -(-3.14 - 0.00222 * pow(soil_con_global->clay[VIC_layer_index] ,2) - 0.00003484 * pow((soil_con_global->quartz[VIC_layer_index]*100) ,2) * soil_con_global->clay[VIC_layer_index]);
*/
   double air_entry_potential = -A_Value * pow(saturation_WC  ,-b_value);
   double potential= air_entry_potential*pow((WC/saturation_WC), -b_value/*Campbell_b*/); ///check the units j/kg instead of meter
   return potential;
}
//_2012-06-15__________________________________________VIC_get_water_potential_/
#endif
double refill_water_depth_mm(layer_data_struct *layer,soil_con_struct *soil_con, size_t frost_index) //150701LML added arguments
{
   //Calculating water deficit (mm)
   int cropsyst_layer_number = 0;
   double refill_required_profile_mm = 0.0;
   double accumulative_soil_depth=0;
   double root_depth_mm = VIC_CropSyst_get(
               #if (VIC_CROPSYST_VERSION>=3)
               VIC::
               #endif
               VC_root_depth_mm);
   for (cropsyst_layer_number = 1; cropsyst_layer_number<=options.Nlayer-1; cropsyst_layer_number++) ///110828 why it starts from 1? it should start from 0. keyvan
   {  int VIC_layer_index = cropsyst_layer_number-1;
      double layer_thickness_mm = /*150701LML soil_con_global*/soil_con->depth[VIC_layer_index] *1000;
      accumulative_soil_depth += layer_thickness_mm;
        //  if (root_depth_mm >= accumulative_soil_depth){ ///keyvan turned it off 030614
      // meter
      double WC =
        #ifdef VCS_V5
        (layer[VIC_layer_index].moist - layer[VIC_layer_index].ice[frost_index]) / m_to_mm(soil_con->depth[VIC_layer_index]);
        #elif (VIC_CROPSYST_VERSION>=3)
        (layer[VIC_layer_index].moist - layer[VIC_layer_index].ice) / m_to_mm(soil_con->depth[VIC_layer_index]);
        #else
        VIC_soil_hydrology_get_water_content(cropsyst_layer_number);  //fraction
        #endif
      double WC_depth_mm = WC * layer_thickness_mm;
      double FC_mm = /*150701LML soil_con_global*//*190806LML soil_con->Wcr[VIC_layer_index]*/ soil_con->VCS.Field_Capacity[VIC_layer_index];//0.7;
      double refill_required_layer_mm = std::max<double>(0,FC_mm - WC_depth_mm);
      //if (refill_required_layer_mm > 0.0)                                        //110627
      refill_required_profile_mm += refill_required_layer_mm;
   //}; //if root depth< soil profile
   }; // for layer
   //LML 141015 VIC_CropSyst_get_return=refill_required_profile_mm;
   if(refill_required_profile_mm < 0) {
       assert(false);
   }
   return refill_required_profile_mm;
}
#if (VIC_CROPSYST_VERSION < 3)
//_____________________________________________________________________________/
double  VIC_get_soil_layer_thickness_m(unsigned char CropSyst_layer)
{  int VIC_layer_index =  CropSyst_layer - 1;
   return soil_con_global->depth[VIC_layer_index];
};
//_2012-08-22___________________________________VIC_get_soil_layer_thickness_m_/
#endif
double cal_soil_evap_from_total_evap(double ET_max_mm)                           //160104LML VIC_partition_crop_evaporation_and_soil_evaporation
{
   double canopy_cover_fraction_total = VIC_CropSyst_get(
            #if (VIC_CROPSYST_VERSION>=3)
               VIC::CANOPY_COVER_FRACTION_TOTAL
            #else
               VC_canopy_cover_fraction_total
            #endif
               );
   #if (VIC_CROPSYST_VERSION==2)
   double crop_transpiration_max_mm
      = std::max<double>(ET_max_mm * canopy_cover_fraction_total,1.e-5);
   VIC_CropSyst_know_transpiration_max_mm(crop_transpiration_max_mm);
   #endif
   // Determine soil evaporation
   double residue_cover_fraction = VIC_CropSyst_get(VIC::FRACT_RESIDUE_COVER);
   return ET_max_mm * (1.0 - canopy_cover_fraction_total) * (1.0 - residue_cover_fraction);
}
//_2012-11-29____________VIC_partition_crop_evaporation_and_soil_evaporation___/
#if (VIC_CROPSYST_VERSION<3)
double VIC_calc_soil_evaporation_actual_mm(
                                           double soil_evap_pot_mm
                                          ,double WC_0_mm
                                           ) ///keyvan I can not understand this function + VIC_CropSyst_get hass multiple definition
{
  // double crop_transpiration_act_mm = VIC_CropSyst_get(VC_transpiration_actual_mm);
   //crop_transpiration_act_mm = VIC_CropSyst_get_return;
double soil_evap_actual_mm=0.0;
double layer_thickness_mm = soil_con_global->depth[0] *1000;
//double WC_0 = layer_wet_global[0].moist; // as it's already in mm I removed to following functions
double WC_depth_mm_0 = WC_0_mm;// * layer_thickness_mm;
double PWP_mm_0 =soil_con_global->Wpwp[0];


if ((WC_depth_mm_0-PWP_mm_0)>soil_evap_pot_mm)
    soil_evap_actual_mm= soil_evap_pot_mm;
else{
    soil_evap_actual_mm=WC_depth_mm_0-PWP_mm_0;
}
if (soil_evap_actual_mm<0)
    soil_evap_actual_mm=0;


   // Keyvan continue here soil_evap_actual_mm = .....

   return soil_evap_actual_mm;
}
//_2012-11-16_________________________________VIC_calc_soil_evaporation_actual_/
double VIC_calc_ET_pot(double ET_reference_crop)
{
   double ET_crop_coef_full_shading = VIC_CropSyst_update_ET_coef_full_shading(); ///need to be checked

   // Warning need to verify that  VIC_CropSyst_update_ET_coef_full_shading

   // is returning the correct ET_crop_coef_full_shading.

   // If not, we will need to use use following


    ET_crop_coef_full_shading = VIC_CropSyst_get_return;

   double ET_pot_mm =

      ET_reference_crop    // short grass
         * ET_crop_coef_full_shading;//; // changes day by day, get from CropSyst
   VIC_CropSyst_get_return = ET_pot_mm;
   return ET_pot_mm;
}
#ifdef CROPSYST_PROVIDED_SOIL_HYDROLOGY
//____________________________________________________________VIC_calc_ET_pot__/
double VIC_get_irrigation_droplet_evaporation_mm()
{  /*
   Keyvan: this needs to be implemented.
   I think you have already implemented this, you can either
   moved that code here, or simply have this function return the result of
   that calculation.
   */
   return 0.0;
   // Returning a value of zero indicates no irrigation for the day.
   // or not a sprinkler/drip system.
};
//_2012-11-16________________________VIC_get_irrigation_droplet_evaporation_mm_/
extern double VIC_get_canopy_interception_mm()
{
  /*
   Keyvan: this needs to be implemented.
   simply have this function return the result of that calculation.
   */
   return 0.0;
}
//_2012-11-16___________________________________VIC_get_canopy_interception_mm_/
#endif
double LAI_from_CropSyst()
{
   double LAI_crop_area = VIC_CropSyst_get(VC_leaf_area_index);
   return LAI_crop_area;
}
//_2013-02-04_Keyvan___________________________________________________________/
double crop_coefficient()
{
    double ET_crop_coef_full_shading=0;
    ET_crop_coef_full_shading = VIC_CropSyst_update_ET_coef_full_shading(); ///need to be checked

   // Warning need to verify that  VIC_CropSyst_update_ET_coef_full_shading
   // is returning the correct ET_crop_coef_full_shading.
   // If not, we will need to use use following

    ET_crop_coef_full_shading = VIC_CropSyst_get_return;
    return ET_crop_coef_full_shading;
}
//_2013-05-03_Keyvan___________________________________________________________/
/*******************************************************************
get the amount of canopy fraction by formulation developed by
Gylon Campbell from the approach that CropSyst is uses
Keyvan 130613
********************************************************************/
double canopy_fraction()
{
    double kc=crop_coefficient();
    double LAI_CS=LAI_from_CropSyst();
    double canopy_fraction_cs=1- exp(-1*kc* LAI_CS);
    if (canopy_fraction_cs<0)
        canopy_fraction_cs=0.0;

    return canopy_fraction_cs;
}
//_2013-06-13_Keyvan___________________________________________________________/
/*******************************************************************
get the amount of canopy interception fraction by formulation exists in CropSyst
Keyvan 130710
********************************************************************/
double radiation_interception_fraction()
{
    double canopy_cover_fraction_green = VIC_CropSyst_get(VC_canopy_cover_fraction_green);
    return canopy_cover_fraction_green;
}
#endif
/*************************************************************
calculate evaporation from irrigation systems           keyvan
calculates the evaporation from irrigation systems based on
formula which was developed by Keyvan Malek,

30Nov2012 for now some dummy number are presented as an input set
            to the formula later we have to put better values
            for each irrigation system in the irrigation library
            file.                                       Keyvan
**************************************************************/
#ifdef MECHANISTIC_IRRIGATION
double evaporation_from_irrigation_systems(
    int irrigation_index,
#ifdef ED_PLAYAN
    double wind,
    double air_temp,
    double rhum,
#else
    double irrigation_amount,
    double ET0,
    double ET0_open_water,
    double crop_h,
    const soil_con_struct& soil_con,
    bool isPerennial
#endif
)
{
    double Ed = 0;
    Irrigation_Type irrigation_code =
      Irrigation_library[irrigation_index].IrrigationType_code;
    General_Irrigation_Type girrig_type =
      identify_general_irrigation_type(irrigation_code);
#ifdef ED_PLAYAN
    General_Irrigation_Type girrig_type =
      identify_general_irrigation_type(irrigation_code);
    if(girrig_type == BIG_GUN || girrig_type == SOLID_SET
       || girrig_type == WHEEL_LINE) {                                           //sprinkler
        Ed = 20.3 + 0.214 * pow(wind,2) - 2.29 * 0.001 * pow(rhum,2);
    } else if(girrig_type == CENTER_PIVOT) {                                     //center pivot (actual irrigation time
        Ed = -2.1 + 1.91 * wind + 0.231 * air_temp;
    }
    Ed = Ed / 100.0 * irrigation_amount;                                         //WORNING: 170901LML NEED TO DOUBLE CHECK WITH KEYVAN!!!
    if (Ed < 0) Ed = 0;
#else
    double wetted_percentage = calc_wetted_percentage(irrigation_code, soil_con,
      irrigation_amount, isPerennial);
#if (VIC_CROPSYST_VERSION>=3)
    const Irrigation_Facility_Charactor &ifc =
      Irrigation_library[irrigation_index].irrigation_facility_charactor;
    double cd = ifc.cd;                                                          //1.1; //nuzzle coeficient
    double h = ifc.h_op * 100.0;                                                 //207.0; //irrigation system operating pressure kpa
    double g =
#ifdef VCS_V5
            CONST_G;
#else
            GRAVITY;
#endif
    double D                = ifc.D;                                             //3.0; //average droplet size mm
    double teta             = ifc.q_nuzzle * 0.0174532925;                       //35.0*0.0174532925; //angle of the nuzzle it should be converted to radian by a function
    double x0               = ifc.x_dm_spk;                                      //10.0; //diameter of sprinkler
    double y0_0             = ifc.y0_spk;                                        //2.0; //height of the irrigation sprinkler from ground m
    double Ap               = ifc.A_Ap;                                          //0.07;//percentage of area which is covered by irrigation system at a time
    double time_of_irrig    = ifc.tirrig;
    double actual_drop_size = ifc.actual_droplet_size;
    double v0 = ifc.v;
    #else
    double cd=Irrigation_library[irrigation_index].irrigation_efficiency[4];     //1.1; //nuzzle coeficient
    double h=Irrigation_library[irrigation_index].irrigation_efficiency[2]*100;  //207.0; //irrigation system operating pressure kpa
    double g=9.81;
    double D=Irrigation_library[irrigation_index].irrigation_efficiency[1];      //3.0; //average droplet size mm
    double teta=Irrigation_library[irrigation_index].irrigation_efficiency[8]
                *0.0174532925;                                                   //35.0*0.0174532925; //angle of the nuzzle it should be converted to radian by a function
    double x0=Irrigation_library[irrigation_index].irrigation_efficiency[6];     //10.0; //diameter of sprinkler
    double y0_0=Irrigation_library[irrigation_index].irrigation_efficiency[5];   //2.0; //height of the irrigation sprinkler from ground m
    double Ap=Irrigation_library[irrigation_index].irrigation_efficiency[10];    //0.07;//percentage of area which is covered by irrigation system at a time
    #endif
    //170901LML #define crop_h 0.5
    //170920LML const double K_t = -0.07527;
    //170920LML const double k_D = 0.31866;
    //170920LML double v0 = cd*pow((2.0*g*h/100.0),0.5); //initial velocity
    //170920LML double t_term = pow(((tan(teta)-(y0_0-crop_h)/x0)*v0*cos(teta)/g),K_t);
    //170920LML double D_term = pow(1/D, k_D);
    if (girrig_type == CENTER_PIVOT || girrig_type == BIG_GUN
        || girrig_type == SOLID_SET || girrig_type == WHEEL_LINE) {
      const double K_t = 1.5707;
      const double k_D = -0.52044;
      double t_term = pow((v0 * sin(teta) / g +
                           pow(pow(v0,2.0) * pow(sin(teta),2) * g * y0_0, 0.5)
                           / g), K_t);
      double D_term = pow( h / (actual_drop_size * 10.0), k_D);
      if (girrig_type == CENTER_PIVOT) {
        Ed = Ap * ET0 * D_term * t_term * irrigation_amount / 100.0;
      } else {
        time_of_irrig = 3.0;
        Ap = 1.0;
        Ed = Ap * ET0 * D_term * t_term * irrigation_amount / 100.0
             * time_of_irrig / 10.0;
      }
    } else {
        if (is_surface_irrigation(girrig_type)) {
            time_of_irrig = 2.0;
            Ed = ET0_open_water * wetted_percentage * time_of_irrig / 10.0;
        } else if (girrig_type == DRIP) {
            wetted_percentage = must_be_between<double>(wetted_percentage,
                                                        0.2, 0.9);
            Ed = ET0_open_water * wetted_percentage * time_of_irrig / 10.0;      //mm
        }
    }
#endif
    return Ed;
};
#endif //MECHANISTIC_IRRIGATION
//_170901LML____________________________________________________________________
bool is_surface_irrigation(const General_Irrigation_Type girrig_type)
{
    return girrig_type == FLOOD || girrig_type == RILL || girrig_type == FURROW;
}
//_170901LML____________________________________________________________________
bool is_sprinkler_irrigation(const General_Irrigation_Type girrig_type)
{
    return girrig_type == BIG_GUN || girrig_type == SOLID_SET
           || girrig_type == WHEEL_LINE;
}
//_170901LML____________________________________________________________________
double calc_irrigation_runoff(
    const int irrigation_index,
    /*const*/ double irrigation_amount_mm,                                           //reached to the surface
    /*const*/ double soil_toplayer_moist_mm,
    /*const*/ double max_allowable_deficit,
    const soil_con_struct& soil_con
#ifndef FULL_IRRIGATION
    ,double basin_proration_ratio
#endif
        )
{
    //use Philip's equation
    double irrig_runoff = 0;
    Irrigation_Type irrigation_code =
      Irrigation_library[irrigation_index].IrrigationType_code;
    General_Irrigation_Type girrig_type =
      identify_general_irrigation_type(irrigation_code);
    if (girrig_type == DRIP
        || CORN::is_approximately<double>(irrigation_amount_mm,0))
        return irrig_runoff;
    Irrigation_Efficiency &irr_eff =
      Irrigation_library[irrigation_index].irrigation_efficiency;
    Irrigation_Facility_Charactor &irr_fac =
      Irrigation_library[irrigation_index].irrigation_facility_charactor;
    double ks = soil_con.Ksat[0] / 24.0 / 10.0;                                 //(mm/day -> cm/hr)
    double s_max = soil_con.VCS.S_max;
    double porosity = soil_con.porosity[0];
    double Wpwp = soil_con.Wpwp[0];
    //190806LML double Wcr = soil_con.Wcr[0];
    double Fc = soil_con.VCS.Field_Capacity[0];


    //DEBUG!!!
    //irrigation_amount_mm = 85.36;
    //ks = 632.479112 / 24.0 / 10.0;
    //s_max = 7.14555;
    //porosity = 0.45226;
    //Wpwp = 9.875;
    //Wcr = 31.2849;
    //soil_toplayer_moist_mm = 31.24;
    //max_allowable_deficit = 0.5;
    //irr_fac.Q = 1.2949;
    //irr_fac.x_dm_spk = 13.6999;



    double s_philip = 0;
    if (porosity > soil_toplayer_moist_mm / 100.0)                   //171008LML added condition
        s_philip = pow(2.0 * (porosity
                              - soil_toplayer_moist_mm / 100.0
                             ) * ks * s_max, 0.5);
    double proration_ratio = 1.0;
#ifndef FULL_IRRIGATION
    proration_ratio = basin_proration_ratio;
#endif

    double typical_irrig_amount = mm_to_cm(irr_eff.maximum_capacity
                                           * proration_ratio
                                           / irr_eff.uniformity)
                                  - 0.05 * mm_to_cm(irr_eff.maximum_capacity
                                                    * proration_ratio);          //assuming the Evaporative losses would be 5% (cm)
    double typical_WC_of_irrigation = Wpwp
                                      + (/*190806LML Wcr*/ Fc - Wpwp)
                                        * max_allowable_deficit * 0.5;           //0.5 multiplyer becasue first layer is assumed to get dried sooner
    double typical_S_philip   = pow((2.0 * (porosity
                                            - typical_WC_of_irrigation / 100.0)
                                     * ks * s_max), 0.5);
    double typical_irrig_time = pow((-typical_S_philip
                                     + pow(pow(typical_S_philip, 2.0)
                                           + 4.0 * ks * typical_irrig_amount
                                       , 0.5))
                                    / (2.0 * ks), 2.0);                          // back calculate to find a reasonable time of
                                                                                 // typical_irrig_time=0.334395; check cp
    double irrig_time = 0.0;                                                     //(hr)
    double lag_time = 0.0;
    //Calculate irrigation time
    if (is_surface_irrigation(girrig_type)) {
        irrig_time = typical_irrig_time;
    } else if (is_sprinkler_irrigation(girrig_type)) {
        double single_sprinkler_discharge_rate = m_to_mm(irr_fac.Q
          / (CONST_PI * pow((irr_fac.x_dm_spk / 2.0),2.0)));                     //(mm/hr)
        double overlap_factor = cm_to_mm(typical_irrig_amount)
                                / irr_eff.presumed_efficiency
                                / typical_irrig_time
                                / single_sprinkler_discharge_rate;
        double application_rate = single_sprinkler_discharge_rate
                                  * overlap_factor;                              //(mm/hr)
        irrig_time  = irrigation_amount_mm / application_rate;
        irrig_time += lag_time;
    }
    //Calculate irrigation runoff
    if (girrig_type == CENTER_PIVOT) {
        double cp_time_rotation = typical_irrig_time * 350.0 * CONST_PI * 2.0
                                  /irr_fac.x_dm_spk;                             //350 is the length of irrigation system?
        irrig_runoff = calc_runoff_from_center_pivot(irrigation_amount_mm,
                         irr_fac.x_dm_spk, ks, s_philip, cp_time_rotation);
    } else {
        double infiltration_mm = calc_infiltration_capacity(s_philip
                                   , irrig_time, ks);
        irrig_runoff = irrigation_amount_mm - infiltration_mm;
    }

    double max_runoff_loss = 1.2 * (1.0 - (irr_eff.uniformity
                                           * irr_eff.presumed_efficiency))
                                   * irrigation_amount_mm * proration_ratio;     //maximmum is 120% of total efficiency
    double min_runoff_loss = (1.0 - irr_eff.presumed_efficiency)
                             * irrigation_amount_mm * proration_ratio * 0.2;     //minimmum is 20% of the presumed efficiency

    //std::clog << "irrigation_amount_mm:" << irrigation_amount_mm
    //          << "\tirrig_runoff:" << irrig_runoff
    //          << "\tmin_runoff_loss:" << min_runoff_loss
    //          << "\tmax_runoff_loss:" << max_runoff_loss
    //          << "\trunoff_fraction:" << (irrig_runoff/irrigation_amount_mm)
    //          << std::endl;

    irrig_runoff = CORN::must_be_between<double>(irrig_runoff,min_runoff_loss
                                                 ,max_runoff_loss);
    return irrig_runoff;
};
//_170920LML from Keyvon________________________________________________________
double calc_wetted_percentage(const Irrigation_Type irrigation_code,
                              const soil_con_struct &soil_con,
                              const double irrigation_amount,
                              bool isPrennial
                              )
{
  General_Irrigation_Type girrig_type =
    identify_general_irrigation_type(irrigation_code);
  double wetted_percentage = 0;
  double d_malekandpeters = 0.0;
  if(girrig_type == CENTER_PIVOT || girrig_type == BIG_GUN
     || girrig_type == SOLID_SET || girrig_type == WHEEL_LINE) {
    return 0.0;
  } else if (girrig_type == DRIP) {
    double time_of_irrig = 7;
    double row_space = 0;                                                        //m
    double col_space = 0;                                                        //m
    int number_of_emitter = 0;
    if(isPrennial){
         row_space = 2.7;
         col_space = 2.5;
         number_of_emitter = 3;
    } else{
         row_space = 1;
         col_space = 1;
         number_of_emitter = 1;
    }
    double area      = row_space * col_space;
    double vol       = area * irrigation_amount;                                 // litter
    double discharge = vol / time_of_irrig / (double)number_of_emitter;
    double ks        = soil_con.Ksat[0] / 24.0 /10.0;                            //cm/hr
    double d_teta    = (soil_con.Wpwp[0] / 100.0 + /*190806LML soil_con.Wcr[0]*/ soil_con.VCS.Field_Capacity[0] / 100.0 )
                       / 2.;                                                     //cm3/cm3
    double pb        = soil_con.bulk_density[0] / 1000.0;                        //gr/cm3
    d_malekandpeters = pow(discharge,0.543) * pow(ks,0.772)
                       * pow(time_of_irrig,0.419) * pow(d_teta,-0.687)
                       * pow(pb,0.305);                                          //cm
    if (isPrennial) {
      wetted_percentage = (double)number_of_emitter * pow(d_malekandpeters / 100.0, 2)
                         * 3.14 / row_space / col_space;                         //four emitter to meet the irrigation demand
    } else{
      wetted_percentage = (double)number_of_emitter * d_malekandpeters
                          / 100.0 / row_space / col_space;                       //four emitter to meet the irrigation demand
    }
    return wetted_percentage > 1.0 ? 1.0 : wetted_percentage;
  } else { //surface irrigation
    if (girrig_type == FURROW) wetted_percentage = 0.5;
    else wetted_percentage = 1.0;                                                // for border and rill irrigation
    return wetted_percentage;
  }
};
//______________________________________________________________________________
double calc_runoff_from_center_pivot(const double irrigation_amount_mm,
  const double sprinkler_diameter, const double ks,
  const double s_philip, const double time_of_rotattion)
{   //sprinkler_diameter:(m)  ks:(cm/hr) time_of_rotattion:(hr)
    const double cp_length  = 400.0;                                             //m
    const int number_of_seg = 4;                                                 //segment
    const double lag_time   = 0.5;
    double total_area       = CONST_PI * pow(cp_length, 2.0);
    double total_runoff     = 0.0;
    double seg_length       = cp_length / (double)number_of_seg;
    for (int i = 0; i < number_of_seg; i++) {
        double seg_radius      = i * seg_length + seg_length / 2.0;              //(m)
        double seg_perimeter   = 2.0 * CONST_PI * seg_radius;                    //(m)
        double seg_speed       = seg_perimeter / time_of_rotattion
                                 / SEC_PER_HOUR;                                 //(m/sec)
        double irrigation_time = sprinkler_diameter / seg_speed / SEC_PER_HOUR;  //(hr)
        //double seg_application_rate  = irrigation_amount_mm / irrigation_time; //(mm/hr)
        double infiltration_capacity = calc_infiltration_capacity(s_philip,
                                         irrigation_time + lag_time, ks);
        double segment_runoff        = std::max<double>(irrigation_amount_mm
                                         - infiltration_capacity, 0.0);
        double segment_area          = CONST_PI * pow(seg_radius,2.0);
        total_runoff                += segment_runoff * segment_area
                                       / total_area;
    }
    return total_runoff;
}
//______________________________________________________________________________
double calc_infiltration_capacity(const double s_philip
  ,const double irrig_time, const double ks)
{
    //irrig_time: (hr)
    //ks:         (cm/hr)
    return cm_to_mm(s_philip * pow(irrig_time, 0.5) + ks * irrig_time);          //(mm)
}
//______________________________________________________________________________
double calc_Maximum_sorptivity(double sand, double clay, double porosity,
                               double b_campbell)
{   //clay and sand have to be in %.
    //it's taken from Rawls 1992, hydrological handbook
    double hb_rawls = 100.0 * (0.341 * porosity - 0.108);                        //Rawls 1983 (also in Saxton paper equation #8)
    double s_max = (2.0 * b_campbell + 3.0) / (b_campbell + 3.0) * hb_rawls;
    return s_max >= 0 ? s_max : 0;
}
//______________________________________________________________________________
int closest_layer_at(const soil_con_struct &soil_con, float64 a_given_depth)
{  float64 layer_depth = 0.0;
   bool done = false;
   nat8 layer = -1;
   while (! done)
   {  layer++;
      float64 thickness_layer = soil_con.depth[layer];
      layer_depth += thickness_layer;
      if (layer == (options.Nlayer-1)) done = true;
      if (layer_depth >= a_given_depth)
      {  done = true;
         if ((layer_depth - a_given_depth) > thickness_layer / 2.0)
         if (layer > 0) layer--;
      }
   }
   return layer;
}
/***************************************
solver
****************************************/
/*
solve_penman_for_T(double Ts, va_list ap){

double init_T;
double init_ea;
double Ec;
double Ed;
double ET_pot;
double rs;
double ra;
double Rn;
double P;
double cp;


//******* Passing arguments through va_list*********

init_T              = (double) va_arg(ap, double);
init_ea             = (double) va_arg(ap, double);
Ec                  = (double) va_arg(ap, double);
Ed                  = (double) va_arg(ap, double);
ET_pot              = (double) va_arg(ap, double);
rs                  = (double) va_arg(ap, double);
ra                  = (double) va_arg(ap, double);
Rn                  = (double) va_arg(ap, double);
P                   = (double) va_arg(ap, double);
cp                  = (double) va_arg(ap, double);


//Ts=init_T;
double C_gas_law=2.16679;

double init_A= C_gas_law*init_ea/init_T;
double new_A= init_A + Ed +Ec;

double target_ET=ET_pot-(Ed+Ec);


    double ET_num=4098*0.618*exp(17.27*Ts/(Ts+237.3))/(Ts+237.3)^2*(Rn)+pa*cp/ra*(0.618*exp(17.27*Ts/(Ts+237.3))-A/C*(Ts+273.15));
    double ET_denom=4098*0.618*exp(17.27*Ts/(Ts+237.3))+cp*P/(e*2.501-0.002361*Ts*(1+rs/ra));
    double ET=ET_num/(ET_denom*(2.501-0.002361*Ts);

    return error=ET-target_ET



*/
//170901LML #endif

#endif
