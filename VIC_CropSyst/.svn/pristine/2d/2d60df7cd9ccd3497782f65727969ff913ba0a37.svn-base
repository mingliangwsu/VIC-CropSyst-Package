#ifndef VIC_CROP_LAND_UNIT_SIMULATION_H
#define VIC_CROP_LAND_UNIT_SIMULATION_H
#include "CropSyst/source/land_unit_sim.h"
#ifdef VCS_V5
#include "vic_def.h"
#else
#include "vicNl_def.h"
#endif

class Soil_hydrology_cascade;
namespace VIC_crop
{
    class VIC_crop_land_unit_simulation
    :public CropSyst::Land_unit_simulation_VX
    {
    private:
        /*LML 141022 these pointers are references, they do not change during simulation period.*/
        crop_data_struct *VIC_crop;
        soil_con_struct *VIC_soil_con;
        veg_lib_struct *VIC_veg_lib;                //Array
        option_struct *VIC_options;
        veg_con_struct *VIC_veg_con;                //Array
        int VIC_Crop_count_index;

        /*LML 141022 these pointers do change during simulation period.*/
        atmos_data_struct *VIC_atmos;
        layer_data_struct *VIC_layer;       //Array
        cell_data_struct *VIC_cell;

    private:
        Weather *loc_weather;
        Soil_hydrology_cascade *soil_hydrology_cascade;
    public:
        bool database_valid;
    private:
        bool setup_weather_from_VIC();                                                  //LML 141023
        bool setup_soil_property_from_VIC();                                            //LML 141023
        //150608LML obs bool update_soil_mositure_from_VIC();
        bool update_soil_T_from_VIC();
        //LML 150422 void CropSyst_know_VIC_layer(layer_data_struct *_VIC_layer);
        //LML 150422 void CropSyst_know_VIC_atmos(atmos_data_struct *_VIC_atmos);
        //LML 150422 void CropSyst_know_VIC_cell(cell_data_struct *_VIC_cell);
        double get_VIC_liq_water_content_volumetric_layer(int VIC_layer_index);
        double get_VIC_ice_water_content_volumetric_layer(int VIC_layer_index);
        const char *get_CropSyst_crop_rotation_parameter_filename();
        const char *get_CropSyst_crop_parameter_filename();
        const char *get_CropSyst_soil_parameter_filename();
    public:
        VIC_crop_land_unit_simulation
            (crop_data_struct *_VIC_crop
            ,soil_con_struct *_VIC_soil_con
            ,veg_lib_struct *_VIC_veg_lib
            ,option_struct *_VIC_options
            ,veg_con_struct *_VIC_veg_con
            ,int _VIC_Crop_count_index
            ,sint32                        _LBF_ID                                      //020504
            ,CropSyst::Scenario           &_scenario_control_and_model_options          //130827
            ,const CORN::Date             &_today                                       //130619
            ,Common_simulation_log        &_event_log                                   //030308
            ,const CropSyst::Scenario_directory *_scenario_directory);
        ~VIC_crop_land_unit_simulation();
        void CropSyst_know_VIC_environment
            (crop_data_struct *_VIC_crop,
            soil_con_struct *_VIC_soil_con,
            veg_lib_struct *_VIC_veg_lib,
            option_struct *_VIC_options,
            veg_con_struct *_VIC_veg_con,
            int _VIC_Crop_count_index);

        virtual bool start_year()                                      modification_;   //130620
        //LML 150422 bool start_day_VIC_CropSyst(atmos_data_struct *_VIC_atmos
        //LML 150422     ,layer_data_struct *_VIC_layer
        //LML 150422     ,cell_data_struct *_VIC_cell);
        virtual bool start();

         CropSyst::Crop_interfaced        *start_crop(int crop_code)         {assert(false);};
        float64 harvest(bool terminate_crop)                              modification_ {assert(false);};
        virtual bool start_day(int32 date)                                modification_ {assert(false);};
        virtual bool end_day()                                            modification_ {assert(false);};
        virtual bool stop_crop()                                          modification_ {assert(false);};


        inline virtual bool is_perennial()                                         const {assert(false);};
        inline virtual float64 update_ET_coef_full_shading()                modification_ {assert(false);};

        virtual bool process()                                              modification_ {assert(false);};
        virtual float64 get(nat32 variable_code)                                     const{assert(false);};
        inline bool know_transpiration_max_mm(double transpiration_max_mm)   modification_{assert(false);};

        virtual void know_pychrometrics                                               //140508
              (float64 vapor_pressure_saturation_slope_kPa_C
              ,float64 psychrometric_constant_kPa_C
              ,float64 aerodynamic_resistance_ref_plants)                    modification_{assert(false);};

        virtual Common_operation *get_operation
           (const char *operation_type_str
           ,const std::string &operation_ID                     // filename may be updated with qualification so not const
           ,modifiable_ CORN_File_name   &operation_filename    // the fully qualified operation filename may be returned (to be output to the schedule)
           ,int32 resource_set_ID
           ,bool &relinquish_op_to_event)                                               {assert(false);};
        virtual Event_status perform_operation
             (Common_operation &op
              ,const void *associated_with_obj)                        modification_    {assert(false);};
        virtual Event_status process_model_specific_event(const Simple_event *event)    {assert(false);};
        virtual const OS::Directory_name &get_output_directory_name()             const {assert(false);};


        //virtual void check_abstract(){VIC_crop_land_unit_simulation *test = new VIC_crop_land_unit_simulation(0,0,0,0,0,0,0,0,0,0,0);};
    };
} // namespace VIC_crop




#endif // VIC_CROP_LAND_UNIT_SIMULATION_H
