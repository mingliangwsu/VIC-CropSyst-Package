#include "vic_crop_land_unit_simulation.h"
#include "common/weather/day/sun_days.h"
#include "common/weather/database/weather_database.h"
//#include "common/weather/weather.h"
#include "CropSyst/source/soil.h"
#include "CropSyst/source/soil/hydrology_cascade.h"
#include "CropSyst/source/soil/temperature_profile.h"
//#include "CropSyst/source/soil/soil_param.h"

using namespace VIC_crop;

//_2014-10-22___________________________________________________________________
VIC_crop_land_unit_simulation::VIC_crop_land_unit_simulation
(crop_data_struct *_VIC_crop
,soil_con_struct *_VIC_soil_con
,veg_lib_struct *_VIC_veg_lib
,option_struct *_VIC_options
,veg_con_struct *_VIC_veg_con
,int _VIC_Crop_count_index
,sint32                        _LBF_ID   // negative values indicate offsite
,CropSyst::Scenario           &_scenario_control_and_model_options
,const CORN::Date             &_today
,Common_simulation_log        &_event_log
,const CropSyst::Scenario_directory *_scenario_directory)
:Land_unit_simulation_VX
   (_LBF_ID
   ,_scenario_control_and_model_options
   ,_today
   ,_event_log
   ,_scenario_directory)
,VIC_crop(_VIC_crop)
,VIC_soil_con(_VIC_soil_con)
,VIC_veg_lib(_VIC_veg_lib)
,VIC_options(_VIC_options)
,VIC_veg_con(_VIC_veg_con)
,VIC_Crop_count_index(_VIC_Crop_count_index)
,database_valid(true)
{
    Weather_database *weather_database = 0;
    Weather *loc_weather = new Weather
    (weather_database          //weather_database
    ,0          //sun_days   // relinquished to weather
    ,true
    ,false      //hourly
    ,true       // treat all valid data as real
    );

    Smart_soil_parameters *smart_soil = new Smart_soil_parameters;
    soil = new CropSyst::Soil(smart_soil,today,*loc_weather);

    soil_hydrology_cascade = new Soil_hydrology_cascade
            (*soil->get_layers()
             ,(const Soil_hydraulic_properties &)(*soil->get_hydraulic_properties_constant())
             ,*soil->get_structure_unmodifiable());
    soil->hydrology = soil_hydrology_cascade;

    soil->provide_temperature_profile();
}
//_2014-10-23___________________________________________________________________
VIC_crop_land_unit_simulation::~VIC_crop_land_unit_simulation()
{
    delete(loc_weather);
    delete(soil_hydrology_cascade);
    delete(soil);
}
//_2014-10-22___________________________________________________________________
void VIC_crop_land_unit_simulation::CropSyst_know_VIC_environment
    (crop_data_struct *_VIC_crop,
    soil_con_struct *_VIC_soil_con,
    veg_lib_struct *_VIC_veg_lib,
    option_struct *_VIC_options,
    veg_con_struct *_VIC_veg_con,
    int _VIC_Crop_count_index)
{
    VIC_crop = _VIC_crop;
    VIC_soil_con = _VIC_soil_con;
    VIC_veg_lib = _VIC_veg_lib;
    VIC_options = _VIC_options;
    VIC_veg_con = _VIC_veg_con;
    VIC_Crop_count_index = _VIC_Crop_count_index;
}
//_2014-10-22___________________________________________________________________
void VIC_crop_land_unit_simulation::CropSyst_know_VIC_layer(layer_data_struct *_VIC_layer)
{
    VIC_layer = _VIC_layer;
}
//_2014-10-23___________________________________________________________________
void VIC_crop_land_unit_simulation::CropSyst_know_VIC_atmos(atmos_data_struct *_VIC_atmos)
{
    VIC_atmos = _VIC_atmos;
}
//_2014-10-22___________________________________________________________________
void VIC_crop_land_unit_simulation::CropSyst_know_VIC_cell(cell_data_struct *_VIC_cell)
{
    VIC_cell = _VIC_cell;
}
//_2014-10-22___________________________________________________________________
bool VIC_crop_land_unit_simulation::start_year()                            modification_
{
    return CropSyst::Land_unit_simulation_VX::start_year();
}
//_2014-10-23___________________________________________________________________
bool VIC_crop_land_unit_simulation::setup_soil_property_from_VIC()
{
    /*LML 141023 need check with Roger */
    int VIC_i;
    double bottom_layer_depth = 0.0;
    for(int i=1;i<=VIC_options->Nlayer;i++){
        VIC_i = i-1;

        soil->structure_defined = true;
        soil->structure->set_bulk_density_g_cm3
                (i,kg_m3_to_g_cm3(VIC_soil_con->bulk_density[VIC_i]));
        soil->layers->thickness[i] = VIC_soil_con->depth[VIC_i];
        bottom_layer_depth += soil->layers->thickness[i];
        soil->layers->depth[i] = bottom_layer_depth;                    //Bottom of layer

        soil->layers->in_horizon_of[i] = i;
        soil->parameters_ownable->horizon_clay[i] = VIC_soil_con->clay[VIC_i];
        soil->parameters_ownable->horizon_silt[i] = VIC_soil_con->silt[VIC_i];
        soil->parameters_ownable->horizon_sand[i] = 100.0 - VIC_soil_con->clay[VIC_i] - VIC_soil_con->silt[VIC_i];
        assert(soil->parameters_ownable->horizon_sand[i] >= 0);

        soil->hydraulic_properties->set_Campbell_b_parameter
                (i,VIC_soil_con->b_campbell[VIC_i]);
        soil->hydraulic_properties->set_saturated_hydraulic_conductivity_kg_s_m3
                (i,VIC_soil_con->Ksat[VIC_i] / (9.81 * 3600.0 * 24.0));                             //mm/day -> kgs/m3
        soil->hydraulic_properties->set_air_entry_potential_parameter(i,VIC_soil_con->AE[VIC_i]);   //(J/kg) LML 141023 need check unit
        soil->hydraulic_properties->set_saturation_water_content_volumetric
                (i,VIC_soil_con->max_moist[VIC_i] / m_to_mm(VIC_soil_con->depth[VIC_i]));
        soil->hydraulic_properties->set_field_capacity_volumetric
                (i,VIC_soil_con->Wcr[VIC_i] / m_to_mm(VIC_soil_con->depth[VIC_i]));
        soil->hydraulic_properties->set_permanent_wilt_point_volumetric
                (i,VIC_soil_con->Wpwp[VIC_i] / m_to_mm(VIC_soil_con->depth[VIC_i]));

    }
    soil->parameters_owned = true;
    return true;
}
//_2014-10-23___________________________________________________________________
bool VIC_crop_land_unit_simulation::setup_weather_from_VIC()
{
    /*NR_index: array index for atmos struct that indicates
    the model step avarage or sum */

    int NR_index = VIC_options->NR;
    float64 longitude_radians = VIC_soil_con->lng;
    float64 standard_meridian_radians = longitude_radians; // Currently we do not have standard meridian in geolocation so simply using the longitude. 071116

    //Weather_database *weather_database = 0;
    //Sun_days *sun_days = new Sun_days( longitude_radians, standard_meridian_radians);

    Weather_provider *loc_provider = &(loc_weather->provider);

    loc_provider->air_temperature_avg.set_Celcius(VIC_atmos->air_temp[NR_index],measured_quality);
    loc_provider->air_temperature_max.set_Celcius(VIC_atmos->tmax,measured_quality);
    loc_provider->air_temperature_min.set_Celcius(VIC_atmos->tmin,measured_quality);
    loc_provider->relative_humidity_avg.set(VIC_atmos->relative_humidity[NR_index],measured_quality);
    loc_provider->relative_humidity_max.set(VIC_atmos->relative_humidity_max,measured_quality);
    loc_provider->relative_humidity_min.set(VIC_atmos->relative_humidity_min,measured_quality);
    loc_provider->wind_speed.set_m_s(VIC_atmos->wind[NR_index],measured_quality);
    loc_provider->precipitation.set_mm(VIC_atmos->prec[NR_index],measured_quality);
    loc_provider->atmospheric_pressure.set(VIC_atmos->pressure[NR_index],measured_quality);     //(kPa)
    loc_provider->solar_radiation.set_MJ_m2(J_m2_to_MJ_m2(VIC_atmos->shortwave[NR_index])*seconds_per_day,measured_quality);   //MJ/m2
    loc_provider->vapor_pressure_deficit_fullday_avg.set_kPa(VIC_atmos->vpd[NR_index],measured_quality);
    loc_provider->location->geolocation->set_elevation_meter(VIC_soil_con->elevation);
    loc_provider->location->geolocation->set_latitude_dec_deg_32(VIC_soil_con->lat);
    loc_provider->location->geolocation->set_latitude_dec_deg_32(VIC_soil_con->lng);

    CS::weather::Storms *loc_storms = 0;
    return CropSyst::Land_unit_simulation_VX::set_weather(loc_weather,loc_storms);

}
//_2014-10-23___________________________________________________________________
bool VIC_crop_land_unit_simulation::update_soil_mositure_from_VIC()
{
    int VIC_i;
    for(int i=1;i<=VIC_options->Nlayer;i++){
        VIC_i = i-1;
        soil_hydrology_cascade->set_water_content_volumetric_layer
                (i,get_VIC_liq_water_content_volumetric_layer(VIC_i));
        soil_hydrology_cascade->ice_water_content[i] = get_VIC_liq_water_content_volumetric_layer(VIC_i);
    }
    return true;
}
//_2014-10-23___________________________________________________________________
bool VIC_crop_land_unit_simulation::update_soil_T_from_VIC()
{
    int VIC_i;
    for(int i=1;i<=VIC_options->Nlayer;i++){
        VIC_i = i-1;
        soil->temperature_profile->set_temperature(i,VIC_layer[VIC_i].T);
    }
    return true;
}
//_2014-10-23___________________________________________________________________
double VIC_crop_land_unit_simulation::get_VIC_liq_water_content_volumetric_layer(int VIC_layer_index)
{
    return VIC_layer[VIC_layer_index].moist / m_to_mm(VIC_soil_con->depth[VIC_layer_index]);
}
//_2014-10-23___________________________________________________________________
double VIC_crop_land_unit_simulation::get_VIC_ice_water_content_volumetric_layer(int VIC_layer_index)
{
    return VIC_layer[VIC_layer_index].ice / m_to_mm(VIC_soil_con->depth[VIC_layer_index]);
}
//_2014-10-23___________________________________________________________________
bool VIC_crop_land_unit_simulation::start_day_VIC_CropSyst
(atmos_data_struct *_VIC_atmos
,layer_data_struct *_VIC_layer
,cell_data_struct *_VIC_cell
)
{
    CropSyst_know_VIC_layer(_VIC_layer);
    CropSyst_know_VIC_atmos(_VIC_atmos);
    CropSyst_know_VIC_cell(_VIC_cell);

    setup_weather_from_VIC();
    #ifndef CROPSYST_PROVIDED_SOIL_HYDROLOGY
    update_soil_mositure_from_VIC();
    #endif
    update_soil_T_from_VIC();
    return CropSyst::Land_unit_simulation_VX::start_day();
}
//_2014-10-23___________________________________________________________________
bool VIC_crop_land_unit_simulation::start()
{
    bool bset_soil_property;
    bset_soil_property = setup_soil_property_from_VIC();
    assert(bset_soil_property==true);
    return CropSyst::Land_unit_simulation_VX::start();
}
