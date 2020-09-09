#ifndef VCS_NL_H
#define VCS_NL_H
#include <assert.h>
#include <vector>
#  include "crop/VIC_soil_hydrology.h"
#  if (VIC_CROPSYST_VERSION < 3)
#     include "crop/growth_stages.h" //keyvan
#     include "crop/VIC_crop_C_interface.h"
#  else
#     include "agronomic/VIC_land_unit_C_interface.h"
#     include "agronomic/VIC_land_unit_simulation.h"
#     include "agronomic/VIC_soil.h"
#  endif
#if VIC_CROPSYST_VERSION==2 ///Keyvan added
int call_crop_model(crop_data_struct *,
                   // #ifdef CROPSYST_PROVIDED_SOIL_HYDROLOGY
                   soil_con_struct  *,
                   //#endif
                   atmos_data_struct *,
                   //4.1.1 cell_data_struct *,  //At this time, only wet cell is sent.  Only transpiration is updated
                   layer_data_struct *,
                   dmy_struct *,
                   int, int //double *, double,
                   ,cell_data_struct *
                   ,int
                   #ifdef CROPSYST_PROVIDED_SOIL_HYDROLOGY
                   ,double
                   #endif
                   #if (VIC_CROPSYST_VERSION>=3)
                   ,int current_crop_counter
                   ,veg_con_struct *veg_con
                   #endif
                   );
void free_croplib(crop_lib_struct **);
crop_data_struct *read_cropparam(FILE *, int /*int   Nveg_type)*/);
double return_MAD(int veg_class_code);
#endif
int get_cropping_system_numbers(const veg_con_struct *veg_con_array);          //150928LML
int set_output_for_crop_numbers(out_data_struct *out_data,const int numcrops); //150928LML
int copy_veg_lib_record(const veg_lib_struct &from_veg, veg_lib_struct &to_veg);      //151001LML
int make_veg_lib_for_crops(veg_con_struct *veg_con,veg_lib_struct *veg_lib);     //151001LML
void initialize_crop(crop_data_struct *);/**08172010 KJC */
void free_crop(crop_data_struct *);
int get_crop_lib_index(int veg_class_code);      //LML 141015
int iscrop(int veg_class_code_in_veg_con_file);
bool IsRotationWithMultipleCycles(int veg_class_code_in_veg_con_file);           //LML 150526
void DecomposeVegclassCode(int veg_class_code_runtime,
                           int &rotation_or_crop_veg_class_code,
                           int &rotation_cycle_index);                           //LML 150526
//double evaporation_from_irrigation_systems(double ET0, double ET0_open_water, int irrigation_index, double crop_h = 0.5);
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
        );
#endif
bool is_surface_irrigation(const General_Irrigation_Type girrig_type);
double calc_Maximum_sorptivity(double sand, double clay, double porosity,
                               double b_campbell);
int closest_layer_at(const soil_con_struct &soil_con, float64 a_given_depth);
double calc_infiltration_capacity(const double s_philip
  ,const double irrig_time, const double ks);
double calc_runoff_from_center_pivot(const double irrigation_amount_mm,
  const double sprinkler_diameter, const double ks,
  const double s_philip, const double time_of_rotattion);
double calc_irrigation_runoff(const int irrigation_index,
    /*const*/ double irrigation_amount_mm,
    /*const*/ double soil_toplayer_moist_mm,
    /*const*/ double max_allowable_deficit,
    const soil_con_struct& soil_con);
void clear_cell_irrigation_water_fluxes(cell_data_struct *current_cell);         //150702LML
int iscrop(int veg_class);
CO2_conc_struct *read_CO2_conc(FILE *);
double solve_penman_for_T (double, va_list);
double solve_wet_bulb(double, va_list);
#if (VIC_CROPSYST_VERSION>=3)
crop_data_struct *provide_cropparam(const veg_con_struct *veg_con_array, const int gridcel);
bool find_irrigation_type(const Irrigation_Types_In_Each_Cell &irrig_lib,
                          const int crop_code, Irrigation_Type &irrigation_type, bool &full_irrigation);
CropCodeLib *read_cropcodelib(FILE *, int&);                                     //161102LML
int find_index_in_sorted_vector(const std::vector<int> &data,const int key, int imin, int imax);
int is_cropping_type_exist(const int cropping_type);              //LML 141020 NIY
int get_current_veg_type(const int veg_class, const dmy_struct *dmy);    //LML 141015
int get_crop_counter(const veg_con_struct *veg_con, const int target_cropping_code);
int get_veg_lib_index(const int veg_class_code);
void free_cropcodelib(CropCodeLib **);
void set_irrigation_efficiency_of_irrigation_library(const int management_factor);
General_Irrigation_Type identify_general_irrigation_type(const Irrigation_Type irrigation);

#ifdef USE_IRRIGATION_PARAM_FILE
void read_irrigparam(FILE *irrigparam, const int gridcel, Irrigation_Types_In_Each_Cell &irrigation_list);
#endif
#ifdef DEBUG_SPECIFIC_VEG_BAND
void print_state_flux_specific_veg_band(int dist,
                                        double mu,
                                        int band,
                                        #ifdef CHECK_WATER_BALANCE
                                        double init_total_water_mm,
                                        #endif
                                        const veg_lib_struct &veglib_rec,
                                        const cell_data_struct *cell,
                                        const snow_data_struct *snow,
                                        const veg_var_struct   *veg_var,
                                        const veg_con_struct   *veg_con,
                                        //150929LML const crop_data_struct *crop,
                                        const soil_con_struct  *soil,
                                        const atmos_data_struct *atmos);
#endif
#if (FULL_IRRIGATION==FALSE)
void alloc_irrig_pattern(int, irrigation_pattern_struct **,filep_struct *, filenames_struct *,
              soil_con_struct *);
double *read_irrig_pattern(FILE *, irrigation_pattern_struct*, int nrecs);

#endif
void copy_and_split_veg_con_element(const veg_con_struct &from,
                                    veg_con_struct &to,
                                    int split_num,
                                    int rotation_index,
                                    int set_zone);
int set_average_veglib_for_crop(veg_lib_struct &veglib,
                                const veg_var_struct * const * const *veg_var,
                                const dist_prcp_struct *prcp,
                                const soil_con_struct *soil_con,
                                const int veg_index,
                                const int current_month);                        //150929LML
#endif  //(VIC_CROPSYST_VERSION>=3)
#endif // VCS_NL_H
