#ifndef VIC_crop_output
#define VIC_crop_output
#include "VIC_CropSyst_common/VCS_Nl_def.h"
#include "VCS_user_def.h"

typedef struct {
    int cell_id;
    double lon;
    double lat;
    int Year;
    int Month;
    int Day;
    int DOY;
    int Dist;
    int Band;
    double Cell_fract;
    int CroppingSyst_code;
    int Crop_code;
    char Crop_name[MAXSTRING];
    double Accum_DD;
    double Accum_DD_clip;
    char Grow_Stage[MAXSTRING];
    double GAI;
    double Green_Canopy_Cover;
    double Biomass_kg_m2;
    double Yield_kg_m2;
    double Root_depth_mm;
    double VIC_PET_shortgrass_mm;
    double CropSyst_refET_mm;
    double crop_evapotransp_max_mm;                                             //20201021LML
    double Act_Transp_mm;
    double VIC_final_transp_mm;
    double irrig_netdemand_mm;
    double irrig_total_mm;
    double irrig_evap_mm;
    double irrig_runoff_mm;
    double irrig_intcpt_mm;
    double Soil_E_mm;
    double Crop_Canopy_E_mm;
    double snow_sublimation;
    double ET_mm;
    double VIC_ET_mm;
    double water_stress_index;
    double Runoff;
    double Baseflow;
    double PPT;
    double Tair_avg;
    double Tmax;
    double Tmin;
    double SWRAD_w_m2;
    double VP_kPa;
    double SPH_kg_kg;
    double RHUM_avg_per;
    double RHUM_max_per;
    double RHUM_min_per;
    double Snow_dep_mm;
    double Crop_Biomass_kg_m2;
    double profile_liq_water;
} Crop_output_struct;
#ifdef OUTPUT_FULLIRRIGATION_PATTERN
typedef struct {
    int cell_id;
    double lon;
    double lat;
    int Year;
    int DOY;
    int rotation_code;
    double irrigation_amount;
} Out_irrigation_pattern_struct;
#endif

#endif
