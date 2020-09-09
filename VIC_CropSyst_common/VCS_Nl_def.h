#ifndef VCS_NL_defH
#define VCS_NL_defH
#include <stdio.h>
#include "irrigation_lib.h"
#ifndef MAXSTRING
#define MAXSTRING 4096
#endif
#ifndef MAX_LAYERS
#define MAX_LAYERS 17
#endif
#ifndef N_PET_TYPES
#define N_PET_TYPES 6
#endif

#if (VIC_CROPSYST_VERSION>=3)
#define MAXCROPTYPES 200
#define xVEG_CLASS_CODE_START_NONE_ROTATION_CROP 100
#define xVEG_CLASS_CODE_END_NONE_ROTATION_CROP   3000
#define PERENNUIAL_CROP_START 100
#define PERENNUIAL_CROP_END   3000

#define xVEG_CLASS_CODE_START_ROTATION_CROP      (VEG_CLASS_CODE_END_NONE_ROTATION_CROP + 1)
#define xVEG_CLASS_CODE_END_ROTATION_CROP        (VEG_CLASS_CODE_START_ROTATION_CROP + 5000)
#define ANNUAL_CROP_START      (PERENNUIAL_CROP_END + 1)
#define ANNUAL_CROP_END        10000
//180730LML note: if need for fractional irrigation, the crop without irrigation is coded as (10000 + irrigated code).
//180713LML added the max member
#define MAX_VEGCLASS_LIB_CODE                   20000
#define MULTIPLIER_FOR_SOWING_CODE              100
#define VEG_CLASS_CODE_START_SPECIFIC_SOWING    (ANNUAL_CROP_START * MULTIPLIER_FOR_SOWING_CODE)
#endif


//***VIC_CropSyst varibales****** //Keyvan Malek NOV072012
#define OUT_TMAX        155                                                      //* max air temperature [C] (ALMA_OUTPUT: [K])*
#define OUT_TMIN        156                                                      //* min air temperature [C] (ALMA_OUTPUT: [K])*
//Crop variable - if CROPSYST_ON TRUE --KJC

#define OUT_CROP_BIOM_CUR       157
#define OUT_CROP_TRANSPIR       158                                              //*sum of all three layers?   *
#define OUT_CROP_IRRI_WAT       159
#define OUT_CROP_BIOM_AHA       160                                              //*Biomass after harvest    *
#define OUT_CROP_BIOMYELD       161                                              //*Biomass yield            *
#define OUT_CROP_WSI            162                                              //*Water stress index       *
#define OUT_CROP_LAI            163                                              //*Crop Leaf area index     *
#define OUT_CROP_GROWTHST       164                                              //*Crop growth stage.       *
#define OUT_DAY_VPD             165                                              //*For testing the crop model, kpa *
#define OUT_SHRT_WAVE           166                                              //*ShortwAVE For testing crop model.  This should be same as OUT_NET_SHORT(82) but the units may be different *
#define OUT_LYR0_VW             167                                              //*layer0 volumetric water content   *
#define OUT_LYR1_VW             168                                              //*layer1 volumetric water content   *
#define OUT_LYR2_VW             169                                              //*layer2 volumetric water content   *
#define OUT_CROP_TRANS0         170                                              //*Crop uptake from layer 0          *
#define OUT_CROP_TRANS1         171                                              //*Crop uptake from layer 1          *
#define OUT_CROP_TRANS2         172                                              //*Crop uptake from layer 2          *
//For testing
#define OUT_RHMAX               173                                              //*daily maximum realative humidity. Percentage                                       *
#define OUT_RHMIN               174                                              //*daily minimum realative humidity. Percentage                                       *
#define OUT_WS                  175                                              //*daily average wind speed. m/s (this is input forcing data)                         *
#define OUT_SWD0                176                                              //*soil water depth in layer 0. mm                                                    *
#define OUT_SWD1                177                                              //*soil water depth in layer 1. mm                                                    *
#define OUT_SWD2                178                                              //*soil water depth in layer 2. mm                                                    *
#define OUT_CROP_EVAP           179                                              //*soil evaporation from layer 0 in mm (this is only for crop fraction of the cell)   *
#define OUT_INFILT              180                                              //*water entering the soil in mm                                                      *
#define OUT_EVAP_FROM_IRRIG     181                                              //*Evaporation from irrigation systems mm                                             *
#define OUT_EVAP_CANOP_IRRIG    182                                              //*Evaporation from the IRRIGATION water which is intercepted by canopy mm            *
#define OUT_EVAP_SOIL           183                                              //*Evaporation from soil mm                                                           *
#define OUT_RUNOFF_IRRIG        184                                              //*Irrigation runoff mm                                                               *
#define OUT_ET_POT_SHORT        185                                              //*ET potential (after applying the KC which is the ETmax mm                          *
#define OUT_CROP_COEF_KC        186                                              //*Crop coeficient (KC)                                                               *
#define OUT_CROP_SOIL_MOIST     187                                              //* 07022013 Keyvan                                                                   *
#define OUT_CROP_SOIL_POTEN     188                                              //*07022013 Keyvan added to print soil potential which is passed to the crop model    *
#define OUT_CROP_TRANS_POT      189                                              //*to print potential transpiration                                                   *
#define OUT_CANOPY_FRACT        190                                              //*canopy interception_fraction from CropSyst                                         *
#define OUT_DEL_T               191                                              //*keyvan added to store new T in an irrigated field                                  *
#define OUT_DEL_VPD             192                                              //*keyvan added to store new VPD in an irrigated field                                *
#define OUT_IRRIG_INTERCEPTION  193                                              //170924LML intercepted irrigation by canopy
//170924LML #define OUT_IRRIGATION          193                                              //*150930lml stores total amount of irrigation water                                  *
//170924LML #define OUT_CELL_EVAP_FROM_IRRIG    194                                          //*150930LML Evaporation from irrigation systems mm                                   *
//170924LML #define OUT_CELL_EVAP_CANOP_IRRIG   195                                          //*150930LML Evaporation from the IRRIGATION water which is intercepted by canopy mm  *
//170924LML #define OUT_CELL_RUNOFF_IRRIG       196                                          //*150930LML Irrigation runoff mm                                                     *
//170924LML #define OUT_CELL_IRRIGATION         197                                          //*150930LML 150930lml stores total amount of irrigation water                        *

typedef struct {
  FILE *cropparam;                                                               //*Crop fractional coverage plus anyother paramter(s) for grid cell.  KJC 02102011 *
  FILE *croplib;                                                                 //*Crop library file.  This file has the crop list and the crop code and crop name association.  KJC 02102011*
  FILE *irrigation_pattern;                                                      //*irrigation pattern for the full irrigation mode, this is added to implement the deficit irrigation 130530 Keyvan*
  FILE *CO2_PPM;                                                                 //*CO2 concentration from RCP85*
  #if VIC_CROPSYST_VERSION>=3
  #ifdef USE_IRRIGATION_PARAM_FILE
  FILE *irrigparam;                                                              //160609LML irrigation parameter for each grid cell
  #endif
  FILE *cropcodelib;                                                             //161102LML Crop name & code library for getting the right crop parameters
  #endif
} VCS_filep_struct;

typedef struct {
  char cropparam[MAXSTRING];                                                     //*primary crop parameter file that has for each grid cell:
                                                                                 //number of crops, crop code and fraction of area of cell for each crop
                                                                                 //KJC 02102011*
  char croplib[MAXSTRING];                                                       //*Crop library file that has the crop code and crop name association.
                                                                                 //Other crop specific paramters can be added. KJC 02132011 *
  char irrigation_pattern[MAXSTRING];                                            //*irrigation pattern file that has the irrigation information of full
                                                                                 //irrigation runs it's used in deficit irrigation 130530 Keyvan*
  char CO2_FILE[MAXSTRING];                                                      //*CO2 concentration file that has the CO2 information of full
                                                                                 // RCP-85 130530 Keyvan*
  char irrig_fpath_pfx[MAXSTRING];                                               //*Stores the path to the irrigation pattern folder*

  #if (VIC_CROPSYST_VERSION>=3)
  #ifdef USE_IRRIGATION_PARAM_FILE
  char irrig[MAXSTRING];                                                         //160609LML
  #endif
  char cropcodelib[MAXSTRING];                                                   //161102LML
  char CropSyst_Working_dir[MAXSTRING];                                          //* The directory for VIC_CropSyst, i.e. current directory of Database *
  char crop_output_file_name[MAXSTRING];                                         //170130LML output file name for daily crop outputs
  #endif
} VCS_filenames_struct;

typedef struct {
    char crop_specfic_param_dir[MAXSTRING];                                        //*path to the directory where the crop specific
                                                                                   //parameter files reside.  There will be a crop parameter file for eac crop. KJC 06272011*
    double CO2_PPM;                                                                //*Concentration of CO2 in PPM. This changes for the future cliamte.
                                                                                   //Values used: Current CLiamte- 388 ppm; future climate - 500 ppm.
                                                                                   //This can change depending on the scenario.   Keyvan 130605*
    int CO2_Nyear;                                                                 //*number of years in CO2 file Keyvan 130605*
    char CO2_trans;                                                                //*to check if CO2 is constant ro transient*
    char clay_input;                                                               //*TRUE means clay percentage should be provided at the end of soil file*
    bool do_irrigate_crop;                                                         //170923LML if true, irrigation is conducted over irrigated area; otherwise, always no irrigation
    #if (VIC_CROPSYST_VERSION>=3)
    int NR;                                                                        //* array index for atmos struct that indicates
                                                                                   //the model step avarage or sum *
    int NF;                                                                        //* array index loop counter limit for atmos
                                                                                   //struct that indicates the SNOW_STEP values *
    #endif
} VCS_option_struct;


#if (VIC_CROPSYST_VERSION>=3)
typedef struct {
  int    number_spinups_for_soil;                                                //*150723LML To get equilibium for SOM, residue, and soil inorganic N*
  int    spinup_years;                                                           //*years of climate data to use for each spinup run*
  int    is_spinup_run;                                                          //*150723LML*
} VCS_global_param_struct;

#define MAX_NUM_CROP_TYPES_FOR_IRRIGATION_DEFINE 50
typedef struct Irrigation_Types_In_Each_Cell {
  int               crop_code[MAX_NUM_CROP_TYPES_FOR_IRRIGATION_DEFINE];         //180730LML note: same code as veg parameter file, not the code for crop types
  Irrigation_Type   irrigation_type[MAX_NUM_CROP_TYPES_FOR_IRRIGATION_DEFINE];
  bool              full_irrigation[MAX_NUM_CROP_TYPES_FOR_IRRIGATION_DEFINE];
  double            fraction_full_irrigation[MAX_NUM_CROP_TYPES_FOR_IRRIGATION_DEFINE];
  Irrigation_Types_In_Each_Cell() {
    for (int i = 0; i < MAX_NUM_CROP_TYPES_FOR_IRRIGATION_DEFINE; i++) {
      crop_code[i]          = 0;
      irrigation_type[i]    = NO_IRRIGATION;
      full_irrigation[i]    = false;
      fraction_full_irrigation[i] = 0.0;
    }
  }
} Irrigation_Types_In_Each_Cell;
#endif //VIC_CROPSYST_VERSION>=3

typedef struct {
  double   clay[MAX_LAYERS];                                                     //* clay content-VIC-Crop related varible which provides clay content for cropsyst- keyvan*
  double   b_campbell [MAX_LAYERS];                                              //* added to store cambell b_value*
  double   AE[MAX_LAYERS];                                                       //* Air entry potential*
  double   silt[MAX_LAYERS];                                                     //* silt content*
#if (VIC_CROPSYST_VERSION>=3)
  double   S_max;                                                                //Maximum_sorptivity
  double   water_pot_at_FC[MAX_LAYERS];                                          //(J/kg or kPa) 170504LML
  double   Field_Capacity[MAX_LAYERS];                                           //(mm) 190806LML
  int CropSyst_Soil_ID;                                                          //* for CropSyst soil properties LML 141104*
  Irrigation_Types_In_Each_Cell irrigation_type_list;                            //160609LML
#endif
} VCS_soil_con_struct;

 #if (VIC_CROPSYST_VERSION>=3)
typedef struct {
  int    veg_class_code;                                                         //* LML 150413 it's the VEG code*
  char   veg_or_rotation_name[MAXSTRING];
} VCS_veg_lib_struct;
#endif

struct veg_lib_struct;
typedef struct {
  int     veg_class_code;                                                        //* stores codes related to each veg class -130226 Keyvan-added *
                                                                                 //* It's also the code including crop rotation cycle infomation *
  #if VIC_CROPSYST_VERSION>=3
  veg_lib_struct **veg_lib;                                                      //* 151001LML for each band. It's pointer array.*
  bool this_year_start_irrigated;                                                //190918LML check if this crop has the first irrigation or not
  #endif
} VCS_veg_con_struct;

typedef struct {
  double tmax;                                                                   //* max air temperature (C) * //keyvan NOV 2012 130219 RLN
  double tmin;                                                                   //* min air temperature (C) * //keyvan NOV 2012 130219 RLN
  double *relative_humidity;                                                     //*relative humidity. Percentage*
  double *sph;                                                                   //specific humidity [kg/kg]
  double relative_humidity_min;                                                  //*daily minimum relative humidity. Percentage*
  double relative_humidity_max;                                                  //*daily maximum relative humidity. Percentage*
} VCS_atmos_data_struct;

typedef struct {
  double transpiration_daily;                                                    //* 150615 daily transpiration from soil layer (mm/day)*
  double evap_bare_soil;                                                         //* 150624LML Soil surface evaporation (mm)*
} VCS_layer_data_struct;

typedef struct {
  double actual_irrigation_reach_ground_amount;                                  //(mm) *this stores the amount of calculated irrigation requirement and apply it after the seperation of the runoff-keyvan NOV2012
                                                                                 //LML 150415 it's net irrigated water after loss through evaporation, canopy interception, and direct runoff
  double deep_percolation_from_irrigation;                                       //(mm) 180531
  //double xxxtotal_irrigation_water;                                                 //LML 150415 it's total amount of irrigation
  double irrigation_runoff;
  double evap_from_irrigation_syst;                                              //LML note: from water drops or surface flow, not including canopy evaporation
  double evap_from_irrigation_intercept;                                         //160509LML note: directly used for canopy evaporation demand (not for interception)
  double irrig_canopy_loss;
  double evap_from_soil;
  double intercepted_irrigation;                                                 //Irrigation for feeding the Wdmax before and after irrigation, and canopy evaporation demand
  double potential_transpir;                                                     //crop potential transpiration 150702LML
  double infiltration;                                                           //*the amount of water that gets into the top layer.  Used to inform the crop
                                                                                 //crop model waht is getting into the water.  Added by Kiran Chinnayakanahalli 11222010*
  double irrigation_netdemand;                                                   //(mm) net irrigation demand
  double irrigation_water;                                                       //*the amount of water added as irrigation water.  KJC 03312011 *
  double aero_resist_daily[N_PET_TYPES][3];                                      //150608LML for CropSyst
  double pot_evap_daily[N_PET_TYPES];                                            //*150608LML array of different types of potential evaporation (mm/day) *
  int iscrop;                                                                    //*to identify if this is a crop paramter. 1-crop paramter, 0-not a crop parameter. Default is 0.*
  double infiltration_daily_m;                                                   //151001LML used for CropSyst chemical transport
} VCS_cell_data_struct;

#if (VIC_CROPSYST_VERSION>=3)
typedef struct Grid_cell_crop_output
{
    double cell_evap_from_irrigation;                                            //stores total evaporation from irrigation systems in a cell
    double cell_evap_from_irrigation_intercepted;                                //130329 keyvan added to store entire cell value
    double cell_irrigation_runoff;                                               //130329 keyvan
    double cell_irrig_water;                                                     //this stores total amount of irrigation water (converted to a vlaue for entire grid cell like raainfall for
                                                                                 //calculation of water balance in the cell) 20/sep/2012 keyvan
} Grid_cell_crop_output;
#endif

//*****************************************************************
//This structure holds crop relevant data when the crop model is active.
//08162010 KJC
//**********************************************************************
typedef struct crop_data_struct {
  //**Input crop information *
  int no_of_crops_in_cell;                                                       //*The total number of crops in the grid cell. This will be the same for each crop in the cell. *
  #if (VIC_CROPSYST_VERSION>=3)
  int code_cropping_type;                                                        //Ration type
  #endif
  float Cv;                                                                      //*Fraction of grid cells area associated with the crop growth *
  float Cv_sum;                                                                  //*Total fraction of the crop coverage.  Do we need this?? *
  int code;                                                                      //*Crop code that identifies the crop. For examle Spring wheat is 210 *
  #if (VIC_CROPSYST_VERSION == 2)
  //150413 In V3 we work with rotations not specific crops
  char crop_name[CROP_NAME_MAXLENGTH];                                           //*Crop name. Do we need this??*
  #endif
  int crop_creation_DOY;                                                         //*Crop should be be created on this day.  *
  int TypeCropIrrigated;                                                         //*1=if crop is irrigated in this cell or 0 = if not irrigated.
                                                                                 //03072011: Roger's STATSGO based irrigation map has values the following values:
                                                                                 //0 is no irrigation no dryland (not agricultural)
                                                                                 //1 is dryland only
                                                                                 //2 is irrigation only
                                                                                 //3 is irrigation and/or dryland.
                                                                                 //
                                                                                 //For our the initial runs, 0 & 1 means no irrigation, 2 & 3 means full irrigation.  We could potentially use this inforamtion
                                                                                 //to distribute irrigation or may go with binary*
   int isPerennial;                                                              //Added 03242011.  This inforamtion is right now in crop paramter file and identifies if the crop is perennial or annual.
                                                                                 //0- annual and 1- perennial.  This information will be used to decide if the crop object is deleted every year or not

  //**These are variables that are returned from the crop model *
  float biomass_current;                                                         //* current biomass t/ha*
  double intercepted_irrigation;                                                 //Irrigation for feeding the Wdmax before and after irrigation
  float biomass_yield;                                                           //* t/ha*
  float biomass_after_harvest;                                                   //* t/ha*
  float crop_leaf_area_index;                                                    //*unitless*
  float water_stress_index;                                                      //* 0-1 1=full stress*
  float refill_water_depth;                                                      //* mm*
  float transpiration[MAX_LAYERS];                                               //* Called uptake in the crop model. Unit = mm/dt  *
  float evaporation;                                                             //*evaporation from the top soil (0th layer) in mm. *
  float canopy_evap;                                                             //mm/dt 150630LML
  unsigned long int CropSystHandle;                                              //*CropSystHandle, one for each crop grown *
  float irrigation_netdemand;                                                    //(mm) net irrigation water demand 180327LML
  float irrigation_water;                                                        //* if inflow < refill_water_depth, = (refill_water_depth-precipitaion)/irrigation efficiency, mm *
                                                                                 //LML 150415 it's total amount of irrigation
  float irrigation_evap;                                                         //*stores evaporation from irrigation systems- Oct-1-2012 Keyvan LML note: from water drops or surface flow*
  #if (VIC_CROPSYST_VERSION == 2)
  float cell_avg_transp;                                                         //21/SEP/2012 added for checking the water balance in a cell
  float cell_evap_from_irrigation;                                               //* stores total evaporation from irrigation systems in a cell*
  double cell_evap_from_irrigation_intercepted;                                  //130329 keyvan added to store entire cell value
  double cell_irrigation_runoff;                                                 //130329 keyvan
  float cell_irrig_water;                                                        //*this stores total amount of irrigation water (converted to a vlaue for entire grid cell like raainfall for
                                                                                 //calculation of water balance in the cell) 20/sep/2012 keyvan*
  #else
  Grid_cell_crop_output *grid_cell_output_ref;                                   //150929LML
  #endif
  double delta_T;                                                                //keyvan added to store new temperature of an irrigated fields
  double delta_VPD;                                                              //Keyvan added to store updated VPD of an irrigated field
  double pot_evap_short;                                                         //130503 keyvan
  double evap_intercepted_irrig_water;                                           //*130506 keyvan.
                                                                                 // *LML note: it's the extra irrigation that should added to the canopy for evaporation demand,
                                                                                 // *i.e. not including the canopy_initial_deficit*
  double irrigation_runoff;                                                      //130507 Keyvan
  double canopy_interception_fraction;
  double soil_potential;                                                         // 02072013 keyvan added to print for making sure soil hydrology is passed correctly to the cropsyst
  double soil_moist;                                                             // 02072013 keyvan added to print for making sure soil hydrology is passed correctly to the cropsyst
  double potential_transpir;                                                     // 02072013 keyvan added to print for making sure soil hydrology is passed correctly to the cropsyst
  double soil_evap;                                                              //(mm/dt) soil evaporation
  double crop_coef;
  double new_vpd;
  double new_Temp;
  double new_tmax;
  double new_tmin;
  double new_ea;
  double new_max_vpd;
  int growth_stage;                                                              //*This tracks the growing stage of the crop.  For the enumeration see "Normal_crop_growth_stage" in growth_stages.h*
  float MAD;                                                                     //*130417 keyvan added to have different value for each indivisual crop*
#ifdef CROPSYST_PROVIDED_SOIL_HYDROLOGY
  float soil_vol_wat_content[MAX_LAYERS]; //3 values, one for each layer .  Used for outputting the variables for testing
#endif
  //int first_day_of_the_Gseason;  //This will serve as a flag to catch the first day of the growing season;
  int days_in_Gseason;                                                           //This will serve to count the days in the growing season;
  float trans_sum_of_layers;
  #ifdef USE_CROPSYST_CROP_IRRIGATION
  double total_daily_irrigation_from_CropSyst_mm;                                //150718LML
  #endif
} crop_data_struct;

//******************************************************************
//  This structure stores parameters for individual crop types.
//  ******************************************************************
#if (VIC_CROPSYST_VERSION==2)
//150413RLN Obsolete in V3 because MAD is provided by current land unit (automatic) irrigation mode parameters.
typedef struct
{
  char  crop_name[CROP_NAME_MAXLENGTH];                                          //*A character string that stores the crop name *
  int   crop_code;                                                               //*This stores a unique crop code.  Right now this is either directly from the WSDA cropland data layer.
                                                                                 //Some crops might have a derived number. For ex: Wheat in WSDA has a general code of 210 and there is difference between spring and winter wheat.
                                                                                 //Here 210 =Wheat(spring) = 210 and Wheat(winter) = 218*
  float MAD;
  int   no_of_crops_in_lib;                                                      //* Total number of crops in the libratry.  The number of crops in any cell cannot be more than this.*
} crop_lib_struct;
#endif

//********************************************************************
//Keyvan 130530
//this structure stores irrigation history from previous runs
//********************************************************************
#if (FULL_IRRIGATION==FALSE)
typedef struct {
    double irrig_amount;
} irrigation_pattern_struct;
#endif
//*******************************************************************
//KEYVAN 130605
//this structure stores CO2 concentrations for different times
//*******************************************************************
typedef struct {
    double CO2_conc;
    int year;
} CO2_conc_struct;

#if (VIC_CROPSYST_VERSION>=3)
typedef struct {
    char cropname[MAXSTRING];
    int code;
} CropCodeLib;
#endif
#endif
