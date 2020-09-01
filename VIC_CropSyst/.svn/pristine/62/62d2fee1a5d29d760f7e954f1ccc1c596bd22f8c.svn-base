#ifndef VIC_soil_hydrologyH
#define VIC_soil_hydrologyH
//struct layer_data_struct;
#ifdef __cplusplus
extern "C" {
#endif
#ifdef VCS_V5
#include "vic_def.h"
#else
#include "vicNl_def.h"
#endif

double cal_soil_evap_from_total_evap(double ET_reference_crop);
bool need_irrigation(double, double, int, double, layer_data_struct *, const soil_con_struct &
                     ,bool fill_water_to_fc_exclude_top = false
                     ,double max_capacity = 0                                   //190724LML
                     ,double *real_added_water = 0                              //190724LML
                     ,double top_layer_to_fc_fraction = 1.0);                   //190805LML control drip irrigations over fruit trees (vine in this case)
double calc_wetted_percentage(const Irrigation_Type irrigation_code,
                              const soil_con_struct &soil_con,
                              const double irrigation_amount,
                              bool isPrennial
                              );
double refill_water_depth_mm(layer_data_struct *layer,
    soil_con_struct *soil_con, size_t frost_index);                              //150701LML added argument
#if (VIC_CROPSYST_VERSION < 3)
double VIC_soil_hydrology_get_water_content(unsigned char layer);
double VIC_soil_hydrology_extract_water(double water_uptake_m[] ,unsigned char start_layer );
int    VIC_get_number_soil_layers();
double VIC_get_soil_layer_thickness_m(unsigned char layer);
//int    VIC_get_soil_depth_m(unsigned char layer);
int    VIC_get_max_number_soil_layers();
double VIC_get_water_potential(unsigned char layer);
double VIC_calc_soil_evaporation_actual_mm(
                                                  double soil_evap_pot_mm
                                                  ,double c);
double VIC_calc_ET_pot(double ET_reference_crop);                                //121207
double amount_of_deficit_irrigation(int);
double LAI_from_CropSyst();
double crop_coefficient();
double canopy_fraction();
double radiation_interception_fraction();
double calc_irrigation_runoff(double time_of_irrigation,
    double irrigation_amount, double ks, double porosity, double S_max,
    double moist);
double calc_Maximum_sorptivity (double sand, double clay, double porosity);
int closest_layer_at(const soil_con_struct &soil_con, float64 a_given_depth);

//double VIC_CropSyst_update_ET_coef_full_shading();
//void VIC_CropSyst_know_transpiration_max_mm(double crop_transpiration_max_mm);
//double evaporation_from_irrigation_systems(double ET0, int irrigation_index);
#ifdef CROPSYST_PROVIDED_SOIL_HYDROLOGY
double VIC_get_irrigation_droplet_evaporation_mm();
double VIC_get_canopy_interception_mm();
#endif

//,double canopy_intercepted_water_evaporation_mm

//,double irrigation_droplet_evaporation_mm);                                    //121129

   // irrigation droplet evaporation will be 0 if no irrigation
   // or not a spray, drip, or sprinkler irrigation system.
#endif


#ifdef __cplusplus
}
#endif

//_2012-06-15_____________________________VIC_soil_hydrology_get_water_content_/

#endif
