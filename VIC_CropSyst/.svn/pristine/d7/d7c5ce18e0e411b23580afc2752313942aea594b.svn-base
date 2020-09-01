
#include <stdio.h>
#include <stdlib.h>
#include "vicNl.h"
#include <vector>                                                               //LML 150323

#include "VCS_Nl_def.h"
//190130 RLN needed for XXXX_CROP_START

#ifdef VIC_CROPSYST_VERSION

#include <math.h>
#include "VIC_crop_variable_codes.h"  /** 08132010 KJC the codes were not recognized without this*/
#include "crop/growth_stages.h" /**08172010 KJC **/
#include "VIC_crop_C_interface.h"
#include "crop/VIC_soil_hydrology.h"

#if (VIC_CROPSYST_VERSION>=3)
#include "assert.h"
#endif

extern double VIC_CropSyst_get_return;
static char vcid[] = "$Id: call_crop_model.c,v 5.12.2.17 2010/08/18 21:39:21 vicadmin Exp $";

//#define MATCH_VB_TEST
#ifdef MATCH_VB_TEST
///*RLN 110128 This is used to compare against the VB version, don't delete yet Othello 2001. (precip is wrong)                    Adam, Jennifer C (jcadam@wsu.edu); Stockle, Claudio Osvaldo
double weather [][10] =
{ //For the next test need to reload weather data as exported in CropSyst 4 text file format. - Roger
};
#endif
#ifdef UNIT_TEST
#   include "unit_tests/VIC_crop_test_weather.h"
#endif

//110615 Not currently used: static int active_growth_started = 0;
/* These accumulators were for debugging eventually delete
float ppt_accum = 0;
float evap_accum = 0;
float transp_accum = 0;
float drain_accum = 0;
*/

char *getCropname(int griven_crop_code);

extern soil_con_struct   *soil_con_global;   //120811
crop_data_struct  *crop_global = 0; //120814
layer_data_struct *layer_wet_global = 0; //120814

#if (VIC_CROPSYST_VERSION == 2)
int call_crop_model(crop_data_struct       *crop,
//#ifdef CROPSYST_PROVIDED_ET
                   soil_con_struct          *soil_con,
//#endif
                   atmos_data_struct        *atmos,
                   layer_data_struct        *layer_wet, //layer is passed on instead of cell_wet in 4.0.7 KJC 08022011
                   dmy_struct               *dmy,
                   int                      rec,
                   int                      veg_class,
                   cell_data_struct         *cell_wet,
                   //121217 unused   double                   *ppt,
                   //121217 obsolete double                   *pot_evap,
                   int                      is_irrigation
                    #ifdef CROPSYST_PROVIDED_SOIL_HYDROLOGY
                   ,double                   infiltration   //infiltration added for 4.0.7.  This is within the cell_data_struct in 4.1.1 KJC 08022011
                    #endif
                    #if (VIC_CROPSYST_VERSION>=3)
                    ,int current_crop_counter
                    ,veg_con_struct *veg_con
                    #endif
   )
/*******************************************************************************************
call_crop_model()	Kiran J Chinnayakanahalli, kiran@wsu.edu	August 18, 2010

This function calls the crop model (CropSyst) interfaces and controls the conditions for growing the crop.

********************************************************************************************/
{
   // printf("%d\n call_crop_model", veg_class);
    extern veg_lib_struct *veg_lib;
    extern option_struct   options;
    crop_data_struct crop_local;
//soil_con_global  = soil_con; //120811
   layer_wet_global = layer_wet; //120814

    int lidx; //generally used as a counter for soil layers.
    int Error_value = 0;
    int VIC_CropSyst_process_return=0;
    int vpd_hour = 0;
#ifdef CROPSYST_PROVIDED_SOIL_HYDROLOGY
    int VIC_CropSyst_know_soil_water_content_return[options.Nlayer];
#endif
    int VIC_CropSyst_start_day_return=0;
    int VIC_delete_object_return=0;
    int forStopping;//for debugginh
    crop_local = (crop[0]);
    crop_global      = &crop_local; //120814
    for (lidx=0;lidx<options.Nlayer;lidx++)
        crop_local.transpiration[lidx] =0.0;
    if (crop_local.biomass_yield>0)crop_local.biomass_yield =0.0; //initialize_crop(&crop_local);
#ifdef CROPSYST_PROVIDED_SOIL_HYDROLOGY
           //2012-08-02  obsolete, the crop model no longer maintains a separate soil hydrology
    for (lidx=0;lidx<options.Nlayer;lidx++)//This part of the code makes two model integrated and reports water content of the soil to the cropsyst every day.  Keyvan Malek, April2012
        {  //4.1.1 - crop_local.soil_vol_wat_content[lidx] = cell_wet->layer[lidx].moist/(soil_con->depth[lidx]*1000); //used for testing
           crop_local.soil_vol_wat_content[lidx] = layer_wet[lidx].moist/(soil_con->depth[lidx]*1000); //used for testing
           //4.1.1 VIC_CropSyst_know_soil_water_content_return[lidx]=VIC_CropSyst_know_soil_water_content(lidx+1, cell_wet->layer[lidx].moist/(soil_con->depth[lidx]*1000));/** 08102010 KJC FUNCTION # */ //cell_wet->layer[lidx].moist);
           VIC_CropSyst_know_soil_water_content_return[lidx]=VIC_CropSyst_know_soil_water_content(lidx+1, layer_wet[lidx].moist/(soil_con->depth[lidx]*1000));
        }
#endif


    //char *CropName = getCropname(crop_local.code);//*****************
    //crop_local.crop_name = CropName;
    if (rec == 0)
    {
      crop_local.CropSystHandle =
        #if (VIC_CROPSYST_VERSION>=3)
        VIC_CropSyst_create_V3(crop
              ,soil_con
              ,veg_lib
              ,&options
              ,veg_con
              ,current_crop_counter);
        #else
        VIC_CropSyst_create(options.crop_specfic_param_dir
 //   #ifdef CROPSYST_PROVIDED_ET
        ,soil_con->lat,soil_con->lng, soil_con->elevation
 //   #else
  //  ,46.3, 120.0,320.0
 //   #endif
        ,HARVEST_AUTOMATICALLY|CO2_RESPONSE_SUBMODEL,crop_local.crop_name);
        #endif

#ifdef CROPSYST_PROVIDED_SOIL_HYDROLOGY
        for (lidx=0;lidx<options.Nlayer;lidx++)//This loop is according to the dcoupling idea where the hydrology is decoupled from the VIC and the water content is not updated everyday
        {   crop_local.soil_vol_wat_content[lidx] = layer_wet[lidx].moist/(soil_con->depth[lidx]*1000); //used for testing
           //2012-08-02  obsolete, the crop model no longer maintains a separate soil hydrology
           //4.1.1 - crop_local.soil_vol_wat_content[lidx] = cell_wet->layer[lidx].moist/(soil_con->depth[lidx]*1000); //used for testing
           //4.1.1 VIC_CropSyst_know_soil_water_content_return[lidx]=VIC_CropSyst_know_soil_water_content(lidx+1, cell_wet->layer[lidx].moist/(soil_con->depth[lidx]*1000));/** 08102010 KJC FUNCTION # */ //cell_wet->layer[lidx].moist);
           VIC_CropSyst_know_soil_water_content_return[lidx]=VIC_CropSyst_know_soil_water_content(lidx+1, layer_wet[lidx].moist/(soil_con->depth[lidx]*1000));

        }
      VIC_know_soil_layer_thickness(soil_con->depth[0],soil_con->depth[1],soil_con->depth[2]);
#else
           //2012-08-02 Now we are using VIC hydrology, the crop model now calls as needed VIC_soil_hydrology_get_water_content.
#endif


        /** Tell Crop model the CO2 concentration in ppm.  This information is passed on the first day of each year.  Right
        now VIC does not know the changing C02 and therefore a sonstant value of 350ppm is passed. -08102010 KJC**/
        #if (VIC_CROPSYST_VERSION==2)
        VIC_CropSyst_know_atmospheric_CO2_concentration(options.CO2_PPM);
        #endif
        crop_local.days_in_Gseason =0;
    } else
        VIC_CropSyst_object_activate(crop_local.CropSystHandle);


    /**08052010 Kiran.  The following code checks creates a crop object on the crop creation day, a paramter set up in the crop apramter file.
    If true it passes the soil texture conditions to Crop modelfile:///home/keyvan/dev/Xcommon/soil/txtrhydr.cpp and creates a handle to the crop**/
    if (dmy[rec].day_in_year == crop_local.crop_creation_DOY ){ //08052010 Kiran  This is the code for corn , a crop type in the present library
        // Kiran: Eventually you will need to setup the crop closer to the sowing date.
        // Also, this won't work for winter crops which will have been established in July/August - Roger
        if(!VIC_CropSyst_is_perennial())
        {   initialize_crop(&crop_local);
                    #if (VIC_CROPSYST_VERSION>=3)
                    crop_local.code = get_current_veg_type(crop_local.code_cropping_type,dmy);      //LML 141020
                    #endif
                    if(crop_local.code == 218)
                        forStopping=0;
        //crop_local.CropSystHandle = VIC_crop_object_create("/media/RAID/Data/Kiran/Data/VIC",crop_local.code,soil_con->lat,soil_con->lng,soil_con->elevation,0x01, crop_local.crop_name);
            VIC_CropSyst_start_crop(crop_local.code);
            crop_local.growth_stage = NGS_SOWING;
        };
    }/*08052010 KJC*/
    if (crop_local.CropSystHandle )
    //110215 if (dmy[rec].day_in_year >= crop_local.crop_creation_DOY /*WHEAT_SPRING_SOW_DAY_IN_YEAR/*POTATO_SOW_DAY_IN_YEAR*/ && crop_local.growth_stage != NGS_HARVEST)
    {
        //110617 VIC_CropSyst_object_activate(crop_local.CropSystHandle);//activate the crop that has to be processed.
        /**To add irrigation water to the rainfall. If the inflow is less than the refill_water_depth,
        add the amount given by the difference divided by the irrigation
        efficiency accounting for losses **/
       // crop_local.irrigation_water =0.0;//assume no irrigation.
       #if (VIC_CROPSYST_VERSION==2)
       if(is_irrigation){
        VIC_CropSyst_know_air_temperature (crop->new_tmax,crop->new_tmin);
       }
       else{
        VIC_CropSyst_know_air_temperature (atmos->tmax,atmos->tmin);
       }

            //130219 (atmos->tmax[NR],atmos->tmin[NR]);


/*********** here we get the PET from VIC***********/
/*121217 obsolete
        VIC_CropSyst_know_potential_reference_evapotranspiration_mm(pot_evap[PET_SHORT]);
*/

        VIC_CropSyst_know_wind_speed_m_s(atmos->wind[NR]);  // 2010-12-28 added -Roger
        //printf("%d\t%d\t%f\n",dmy[rec].year, dmy[rec].day_in_year, atmos->wind[NR]);

        VIC_CropSyst_know_solar_radiation((0.0864 *atmos->shortwave[NR]));
        VIC_CropSyst_know_relative_humdity(atmos->relative_humidity_max, atmos->relative_humidity_min);
        #endif
        /**Pass on the day of the year information **/
        VIC_CropSyst_start_day_return = VIC_CropSyst_start_day(dmy[rec].year*1000 +dmy[rec].day_in_year);
        /**Pass the amount of water that infiltrates */
#ifdef CROPSYST_PROVIDED_SOIL_HYDROLOGY
//2012-08-02  obsolete, the crop model no longer maintains a separate soil hydrology
        //4.1.1 if (cell_wet->infiltration > 0.0)
        if (infiltration > 0.0)
            //4.1.1 VIC_CropSyst_know_water_entering_soil_mm(cell_wet->infiltration);
            VIC_CropSyst_know_water_entering_soil_mm(infiltration);
#else
//2012-08-02  now we are now using VIC hydrology.
#endif
        //if (dmy[rec].year == 1997 && dmy[rec].month == 7 && dmy[rec].day == 29)
        //    forStopping = 1;
        if (crop_local.growth_stage < NGS_TERMINATED)
        {   /**Ask the crop model to start the process for the day**/
            crop_local.growth_stage =VIC_CropSyst_process();
            if(crop_local.growth_stage > NGS_SOWING && crop_local.growth_stage < NGS_DORMANT_or_INACTIVE)
            {   crop_local.days_in_Gseason ++;

#ifdef CROPSYST_PROVIDED_SOIL_HYDROLOGY
//2012-08-02  obsolete, the crop model no longer maintains a separate soil hydrology
                crop_local.refill_water_depth = VIC_CropSyst_get(VC_refill_water_depth_mm);
                crop_local.refill_water_depth = fabs(VIC_CropSyst_get_return);
                crop_local.refill_water_depth = VIC_CropSyst_get_return;
#else
//printf("%d\n, %d\n", rec, veg_class);
/*if(rec==100){
  int  keyvan=0;
}
double actual_irrigation_requirment=VIC_get_soil_layer_thickness_m(1);//refill_water_depth_mm();
actual_irrigation_requirment=VIC_CropSyst_get_return;
actual_irrigation_requirment=10;
if(actual_irrigation_requirment>0.000001){
    int keyvan=0;
}
crop_local.refill_water_depth=actual_irrigation_requirment;*/
//2012-08-03pt
//Keyvan must implement something to determine the irrigation amount.
//This would not go here,  it would probably be done in surface_fluxes.
#endif
            }
            else
            {   crop_local.days_in_Gseason = 0;
                crop_local.refill_water_depth =0;
            }

            switch (crop_local.growth_stage )
            {   case NGS_HARVEST:
                {   if (VIC_CropSyst_is_perennial())
                    {   crop_local.biomass_yield = VIC_CropSyst_get(VC_biomass_yield_t_ha);
                        crop_local.biomass_yield = VIC_CropSyst_get_return;
                        crop_local.biomass_after_harvest = VIC_CropSyst_get(VC_biomass_after_harvest_t_ha);
                        crop_local.biomass_after_harvest = VIC_CropSyst_get_return;
                    } else
                    {   //crop_local.biomass_yield = VIC_CropSyst_harvest(0);  not needed anymore.  02052011 Kiran
                        crop_local.biomass_yield = VIC_CropSyst_get(VC_biomass_yield_t_ha);
                        crop_local.biomass_yield = VIC_CropSyst_get_return;
                        crop_local.biomass_after_harvest = VIC_CropSyst_get(VC_biomass_after_harvest_t_ha);
                        crop_local.biomass_after_harvest = VIC_CropSyst_get_return;
                        if(crop_local.code == 218)
                            forStopping=0;
                        VIC_CropSyst_stop_crop();

                       for (lidx=1;lidx<options.Nlayer;lidx++)
                        {   layer_wet[lidx].evap = 0.0;
                        }/* 08102010 KM*/

                        #ifdef CROPSYST_PROVIDED_SOIL_HYDROLOGY
                       for (lidx=0;lidx<options.Nlayer;lidx++)
                        {   layer_wet[lidx].evap = 0.0;
                        }/* 08102010 KJC*/
                        crop_local.evaporation=0.0;
                        #endif
                        //initialize_crop(&crop_local);
                        crop_local.irrigation_water = 0.0;//reset the irrigation water
                        crop_local.water_stress_index =0;
                        crop_local.biomass_current =0.0;//reset the biomass current

                        for (lidx=0;lidx<options.Nlayer;lidx++){
                            crop_local.transpiration[lidx] =0.0;//No uptake after harvest
                        }
                        crop_local.growth_stage = NGS_TERMINATED;
                        //crop_local.first_day_of_the_Gseason = 0;
                        crop_local.days_in_Gseason = 0;
                    }  /*endif is perenial*/
                } break;
                case NGS_ACTIVE_GROWTH: //added on 08062011 KJC
                {   if(crop_local.code == 1824)
                       forStopping=0;
                    /*110615 this was to set the initial soil water content at the start of growth
                    currently the CropSyst initial water content is set at the start of the simulation.
                    if (!active_growth_started)
                    {
                        for (lidx=0;lidx<options.Nlayer;lidx++)//This loop is according to the dcoupling idea where the hydrology is decoupled from the VIC and the water content is not updated everyday
                        {
                            crop_local.soil_vol_wat_content[lidx] = cell_wet->layer[lidx].moist/(soil_con->depth[lidx]*1000); //used for testing
                            VIC_CropSyst_know_soil_water_content_return[lidx]=VIC_CropSyst_know_soil_water_content(lidx+1, cell_wet->layer[lidx].moist/(soil_con->depth[lidx]*1000));
                        }
                        ppt_accum = 0.0;
                        evap_accum = 0.0;
                        transp_accum = 0.0;
                        drain_accum = 0.0;
                    }
                    active_growth_started = 1;
                    */

                }//Should not have a break and the default case must follow
                default :
                {
                if(crop_local.code == 1824)
                       forStopping=0;
                /**Get current biomass**/
                crop_local.biomass_current =VIC_CropSyst_get(VC_biomass_current_t_ha);
                crop_local.biomass_current = VIC_CropSyst_get_return;
                if (dmy[rec].day_in_year > 280)
                            forStopping =0.0;
#ifdef CROPSYST_PROVIDED_SOIL_HYDROLOGY
//120803 the crop model now provides VIC with the transpiration by calling VIC_soil_hydrology_extract_water
                double total_transp = 0.0;
                /**Get current uptake **/
                for (lidx=0;lidx<options.Nlayer;lidx++){
                    crop_local.transpiration[lidx] =VIC_CropSyst_get_uptake_mm(lidx+1);
                    crop_local.transpiration[lidx] = VIC_CropSyst_get_return;
                    total_transp += VIC_CropSyst_get_return;
                }
#endif
                } break;
            }; // end switch

        }// if crop
        else
           crop_local.biomass_current = 0.0;
        /** Get current stress condition and the refill needed* */
        crop_local.water_stress_index =VIC_CropSyst_get(VC_water_stress_index);
        crop_local.water_stress_index = VIC_CropSyst_get_return;
        crop_local.crop_leaf_area_index = VIC_CropSyst_get(VC_leaf_area_index);
        crop_local.crop_leaf_area_index = VIC_CropSyst_get_return;
        /**Update the varaibles after the execution of the crop model **/
        /**Update layer evap **/
        for (lidx=1;lidx<options.Nlayer;lidx++)
        {   //4.1.1 cell_wet->layer[lidx].evap = crop_local.transpiration[lidx];//"uptakeXXX";//cell_wet.layer[lidx].moist);// 08102010 KJC FUNCTION CALL#3
            layer_wet[lidx].evap = crop_local.transpiration[lidx];
        }// 08102010 KJC
       /* crop_local.evaporation = VIC_CropSyst_get(VC_evaporation_mm );
        crop_local.evaporation = VIC_CropSyst_get_return;
        layer_wet[0].evap=crop_local.evaporation;*/
#ifdef CROPSYST_PROVIDED_SOIL_HYDROLOGY
//120803   VIC  now provides all soil hydrology including soil evaporation
        crop_local.evaporation = VIC_CropSyst_get(VC_evaporation_mm );
        crop_local.evaporation = VIC_CropSyst_get_return;
        //4.1.1 cell_wet->layer[0].evap += crop_local.evaporation;
       // layer_wet[0].evap += crop_local.evaporation;
//       layer_wet[0].evap+=cell_wet.evap_from_soil; //km
#endif
        /*110615  // for debugging only
            evap_accum += VIC_CropSyst_get_return;
            transp_accum += total_transp;
            drain_accum += cell_wet->baseflow;
        */
        VIC_CropSyst_end_day(); // 110125 added by RLN
    }
    else  /**for the crop fraction of the area, use bare soil calculations when the the crop is not grown -outside the growing season*/
    {   //Do nothing.  Might add things later
    }
    crop[0]=crop_local;
    return Error_value;
}

char *getCropname(int given_crop_code)
/**  This routine returns the crop name string dending on the crop code.
This should be changed to be read from a file.  Maybe from crop library.*/
{   char *crop_names[] ={"Wheat(spring)","Wheat(winter)","Potato"};
    int crop_codes[]={210,218,1827};
    int i, Max_no_crops = 3;
    for( i=0; i<Max_no_crops;i++){
        if (given_crop_code == crop_codes[i]) return crop_names[i];
    }
    return "No_name";
}
#endif
#endif

#if (VIC_CROPSYST_VERSION>=3)
//__LML 150323_________________________________________________________________/
int find_index_in_sorted_vector(const std::vector<int> &data,const int key, int imin, int imax)
{
    // continue searching while [imin,imax] is not empty
    while (imax >= imin) {
        // calculate the midpoint for roughly equal partition
        int imid = (imin + imax) / 2;
        if(data[imid] == key)
          // key found at index imid
          return imid;
          // determine which subarray to search
        else if (data[imid] < key)
          // change min index to search upper subarray
          imin = imid + 1;
        else
          // change max index to search lower subarray
          imax = imid - 1;
    }
    // key was not found
    return -1;
}
//_____________________________________________________________________________/
int get_crop_counter(const veg_con_struct *veg_con/*150930LML crop_data_struct *pcrops*/,const int target_cropping_code)   //LML 141015
{
    int index = -1;
    int num_veg = veg_con->vegetat_type_num;                                     //150930LML /*LML 150427 num_of_cropping_types_in_cell*/;
    int count = -1;
    for (int j = 0; j < num_veg; j++) {
        if (iscrop(veg_con[j].VCS.veg_class_code)) {
            count++;
            if(target_cropping_code == veg_con[j].VCS.veg_class_code){
                index = count;
                break;
            }
        }
    }
    return index;
}
//_____________________________________________________________________________/
int is_cropping_type_exist(const int cropping_type)
{
    //*LML 141020 check if the croping type in the veg parameter list is valid or not.
    // *This version is for test only!!!
    //if((cropping_type >= VEG_CLASS_CODE_START_CROP) && (cropping_type<=VEG_CLASS_CODE_END_CROP))
    std::clog << "TEST for is_cropping_type_exist()!!!\n";
    if((cropping_type >= 101) && (cropping_type <= 103))
        return 1;
    else return 0;
}
//_____________________________________________________________________________/
int get_current_veg_type(const int veg_class,const dmy_struct *dmy)    //LML 141015
{
    std::clog << "TEST for get_current_veg_type()!!!\n";
    int current_veg_type(-1);
    if(iscrop(veg_class)){ //LML 141020 assert(false);
        //LML 141020 for test only!!!
        switch(veg_class){
        case 101:
            current_veg_type = 218;
            break;
        case 102:
            current_veg_type = 210;//1208;
            break;
        case 103:
            current_veg_type = 1515;//210;
            break;
        default:
            current_veg_type = -1;
        }
    } else {
      current_veg_type = veg_class;
    }
    return current_veg_type;
}
#endif

#if (VIC_CROPSYST_VERSION==2)
//150413 obsolete in V3 because MAD is provided by land unit
int get_crop_lib_index(int veg_class_code)      //LML 141015
/**This funtion returns -1 if the veg class is not a crop type.*/
{
    extern crop_lib_struct *crop_lib;
    //crop_data_struct crop_local;
    int index = -1;
    #if ((VIC_CROPSYST_VERSION>=3) && (!defined (__BCPLUSPLUS__)))
    std::vector<int> crops(crop_lib[0].no_of_crops_in_lib);
    for (int i = 0; i < crop_lib[0].no_of_crops_in_lib; i++)
        crops[i] = crop_lib->crop_code;
    //index = search_croplib(crop_lib,veg_class_code,0,crop_lib[0].no_of_crops_in_lib-1);
    index = find_index_in_sorted_vector(crops,veg_class_code,0,crop_lib[0].no_of_crops_in_lib-1);
    #else
    for (int j = 0; j < crop_lib[0].no_of_crops_in_lib; j++) {
        if (veg_class_code == crop_lib[j].crop_code) {
            index = j;
            break;
        }
    }
    #endif
    return index;
}
#endif
#ifdef VIC_CROPSYST_VERSION
//_____________________________________________________________________________/
int iscrop(int veg_class_code_in_veg_con_file)
/*This funtion returns 1 if the veg class (in veg con file) is a crop type.*/
{
    //#if (VIC_CROPSYST_VERSION>=3)
    //if((veg_class_code_in_veg_con_file >= PERENNUIAL_CROP_START &&
    //    veg_class_code_in_veg_con_file <= PERENNUIAL_CROP_END)
    //   || (veg_class_code_in_veg_con_file >= VEG_CLASS_CODE_START_SPECIFIC_SOWING))      //LML 150427
    //    return 1;
    //#else
    //if(get_crop_lib_index(veg_class_code_in_veg_con_file) != -1)
    //    return 1;
    //#endif
    //else return 0;
    if ((veg_class_code_in_veg_con_file < PERENNUIAL_CROP_START) ||
        (veg_class_code_in_veg_con_file > MAX_VEGCLASS_LIB_CODE))
        return 0;
    else return 1;


}
//_iscrop_______________________________________________________________150526_/
bool IsRotationWithMultipleCycles(int veg_class_code_in_veg_con_file)
{
    int real_veg_class_code = veg_class_code_in_veg_con_file > 10000 ?
                              veg_class_code_in_veg_con_file % 10000 :
                              veg_class_code_in_veg_con_file;                    //180713LML the code greater than 10000 is for seperating irrighated and non-irrigated crops in one grid cell
    if (real_veg_class_code >= ANNUAL_CROP_START &&
        real_veg_class_code <= ANNUAL_CROP_END)
        return true;
    else
        return false;
}
//_IsRotationWithMultipleCycles_________________________________________150526_/
void DecomposeVegclassCode(int veg_class_code_runtime,
                           int &rotation_or_crop_veg_class_code,
                           int &rotation_cycle_index)
{
    if (veg_class_code_runtime > VEG_CLASS_CODE_START_SPECIFIC_SOWING) {
        div_t temp                      = div(veg_class_code_runtime,MULTIPLIER_FOR_SOWING_CODE);
        rotation_or_crop_veg_class_code = temp.quot;
        rotation_cycle_index            = temp.rem;
    } else {
        rotation_or_crop_veg_class_code = veg_class_code_runtime;
        rotation_cycle_index            = 0;
    }
}
//_DecomposeVegclassCode_______________________________________________________/
#if (VIC_CROPSYST_VERSION == 2)
double return_MAD(int veg_class_code)
{
    /*LML 141015
    double MAD=0;
   extern crop_lib_struct *crop_lib;
   crop_data_struct crop_local;
   int j;
   for(j=0;j<crop_lib[0].no_of_crops_in_lib;j++)
            if(veg_class_code == crop_lib[j].crop_code){
               MAD=crop_lib[j].MAD;
               return MAD;
            }
    return MAD;
    */
    #if (VIC_CROPSYST_VERSION==2)
    extern crop_lib_struct *crop_lib;
    int crop_lib_index = get_crop_lib_index(veg_class_code);
    if (crop_lib_index == -1) return 0;
    else return crop_lib[crop_lib_index].MAD;
    #else
    assert(false);          //LML 150413 NIY get MAD from CropSyst management file
    #endif
}
#endif
//_return_MAD__________________________________________________________________/
#endif
