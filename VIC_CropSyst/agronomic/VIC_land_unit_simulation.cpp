﻿#ifdef VIC_CROPSYST_VERSION
#include "VIC_land_unit_simulation.h"
#include "VIC_land_unit_C_interface.h"

#include "corn/measure/measures.h"
#include "corn/data_source/vv_file.h"
#include "corn/chronometry/date_32.h"
#include "common/weather/day/sun_days.h"
#include "common/weather/database/weather_database.h"
#include "common/weather/weather.h"
#include "CropSyst/source/soil.h"
#include "CropSyst/source/soil/hydrology_cascade.h"
#include "CropSyst/source/soil/temperature_profile.h"
#include "CropSyst/source/cs_mgmt.h"
#include "CropSyst/source/rot_param.h"
#include "CropSyst/source/crop/crop_interfaced.h"
#include "CropSyst/source/crop/phenology_I.h"
#include "organic_matter/OM_residues_profile_abstract.h"                         //150722LML
#include "residue/residues_cycling.h"                                            //150722LML
#include "residue/residues_interface.h"                                          //160509LML
#include "soil/chemicals_profile.h"                                              //150722LML
#include "global.h"                                                              //150723LML
#include "UED/library/std_codes.h"
#include "common/weather/hour/weather_hours.h"                                   //190515LML
#ifdef VCS_V5
#include "VCS_Nl_v5.h"
#include "vic_driver_shared_all.h"
#else
#include "vicNl.h"                                                               //150608
#endif
//______________________________________________________________________________
namespace VIC {
extern CORN::Enumeration_list rotations_parameters;


Atmospheric_CO2_change::Atmospheric_CO2_change(const CORN::date32 &simdate_raw_)
    :Atmospheric_CO2_change_element(simdate_raw_)
{};
//_2014-10-22___________________________________________________________________
Land_unit_simulation::Land_unit_simulation
(
 CS::Arguments *arguments_given                                                  //200817RLN_150415
//200817RLN CropSyst::Arguments *arguments_given                                            //150415
, nat32 rotation_or_crop_veg_class_code_                                        //150521RLN
, nat8  rotation_cycle_index_                                                    //150521RLN
//190130 , const CORN::Date_cowl_32  &simdate_
, const CORN::date32     &simdate_raw                                            //190130RLN
    , crop_data_struct   &VIC_crop_                                              //LML 150422
    , soil_con_struct    &VIC_soil_con_                                          //LML 150422
    , option_struct      &VIC_options_                                           //LML 150422
    , veg_con_struct     &VIC_veg_con_item_                                      //LML 150422
    , atmos_data_struct  *VIC_atmos_                                             //LML 150422
    , cell_data_struct   &VIC_cell_                                              //LML 150422
    , snow_data_struct   &VIC_snow_                                              //LML 150608
    , veg_var_struct     &VIC_veg_var_
    )                                          //LML 150609
: CropSyst::Land_unit_simulation
   (arguments_given->ref_ID() //151208 scenario_ID                               //151208RLN
   ,rotation_cycle_index_                                                        //151208RLN
   ,scenario_control_and_model_options
   //,simdate_.ref_date32()
   ,simdate_raw                                                                  //190130RLN
   ,geolocation
   ,meterology
//200325RLN    #if (CS_VERSION==4)
   //200325RLN ,log_unused //150415 RLN_event_log
//200325RLN    #endif
   ,0                                                                            //161101LML 
   )
,rotation_or_crop_veg_class_code (rotation_or_crop_veg_class_code_)              //150521RLN
,rotation_cycle_index            (rotation_cycle_index_)                         //150521RLN
,VIC_crop      (VIC_crop_)
,VIC_soil_con  (VIC_soil_con_)
,VIC_options   (VIC_options_)
,VIC_veg_con_item   (VIC_veg_con_item_)
,arguments(arguments_given)                                                      //150415RLN
,VIC_atmos     (VIC_atmos_)
,VIC_cell      (VIC_cell_)
,VIC_snow      (VIC_snow_)
,VIC_veg_var   (VIC_veg_var_)                                                    //150609LML
#ifdef USE_CROPSYST_CROP_IRRIGATION
,irrigation_operation_for_VIC_next_day(0)                                        //150720LML
#endif
,reference_canopy_resistance_d_m(/*canopy_resistance_constant_d_m*/ 0.00081)
//150521,VIC_Crop_count_index(_VIC_Crop_count_index)
,slope_saturated_vapor_pressure_plus_psychrometric_resistance_reference          //150603
   (0)

,vic_soil_for_CropSyst
   (simdate_raw //190130 .ref_date32()                                                          //170526
   ,scenario_control_and_model_options.annual_temperature                        //150424
   ,VIC_cell.layer,VIC_soil_con)
//200325RLN ,log_unused("unused.log")                                                        //150415RLN
,database_valid(true)
,scenario_control_and_model_options(false)                                       //160421RLN
,geolocation(VIC_soil_con.lat,VIC_soil_con.lng,VIC_soil_con.elevation)
//170525 ,meterology(today,geolocation,NO_SNOW_PACK)
,atmospheric_CO2(simdate_raw)                                                   //04012021
,meterology(simdate_raw /*190130 .ref_date32()*/,geolocation,NO_SNOW_PACK)
,location_params(geolocation) //190128RLN ,0,0,0) //(Need VIC version numbers)
//150707,extraterrestrial_solar_irradiance_day(UC_MJ_m2,geolocation,_today)
//190723LML ,sun_days(geolocation.get_latitude_radians(),geolocation.get_latitude_radians())
//190723LML ,sun_hours(sun_days,geolocation.get_latitude_dec_deg())
//190723LML ,extraterrestrial_solar_irradiance(geolocation,sun_days,sun_hours,today)
,weather_provider(0)
,air_temperature_min_tomorrow(0)
,air_temperature_max_yesterday(0)
,curr_irrigation_operation             (0)                                       //170221RLN_150718LML
,zone(0)                                                                         //151008
,weather_spanning
   (simdate_raw,VIC_atmos_                                                       //190130_170525
   ,air_temperature_max_yesterday
   ,air_temperature_min_tomorrow)
{
  //190723LML meterology.know_solar_irradiance_extraterrestrial(extraterrestrial_solar_irradiance); //160812LML
  //190723LML meteorology.provide_sun();                                                     //170327LML
/*141207RLN weather and soil instanciation will be done in initialize method
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
*/
    meteorology.know_Atmospheric_CO2(&atmospheric_CO2);                         //04012021
    soil = &vic_soil_for_CropSyst;                                              //190723LML
    //soil->layers = soil->ref_layers();                                          //200708LML
    //soil->hydrology = soil->ref_hydrology();                                    //200709LML



    //float64 test = soil->ref_hydrology()->provide_water_depth_profile(true);


}
//_Land_unit_simulation:constructor_________________________________2014-10-23_/
Land_unit_simulation::~Land_unit_simulation()
{
    delete weather_provider;                                                       //180402LML
    delete slope_saturated_vapor_pressure_plus_psychrometric_resistance_reference; //180402LML
    if(crop_active_or_intercrop) delete crop_active_or_intercrop;                //180402LML
}
//_2014-10-22___________________________________________________________________
/* LML 150422
void Land_unit_simulation::know_VIC_environment
    (crop_data_struct *_VIC_crop,
     soil_con_struct *_VIC_soil_con,
     veg_lib_struct *_VIC_veg_lib,
     option_struct *_VIC_options,
     veg_con_struct *_VIC_veg_con,
     atmos_data_struct *_VIC_atmos,
     cell_data_struct *_VIC_cell,
     int _VIC_Crop_count_index)
{
    VIC_crop                = _VIC_crop;
    VIC_soil_con            = _VIC_soil_con;
    VIC_veg_lib             = _VIC_veg_lib;
    VIC_options             = _VIC_options;
    VIC_veg_con             = _VIC_veg_con;
    VIC_Crop_count_index    = _VIC_Crop_count_index;
    VIC_atmos               = _VIC_atmos;                                        //LML 150422
    VIC_cell                = _VIC_cell;                                         //LML 150422
    vic_soil_for_CropSyst.VIC_soil_con = VIC_soil_con;                          //LML 150422
    vic_soil_for_CropSyst.hydrology.set_VIC_layer_array(VIC_cell->layer);        //LML 150422
}
*/
//_2014-10-22___________________________________________________________________
//LML 150422 void Land_unit_simulation::CropSyst_know_VIC_layer(layer_data_struct *_VIC_layer)
//LML 150422 {
//LML 150422     VIC_layer = _VIC_layer;
//LML 150422 }
//_2014-10-23___________________________________________________________________
//LML 150422 void Land_unit_simulation::CropSyst_know_VIC_atmos(atmos_data_struct *_VIC_atmos)
//LML 150422 {
//LML 150422     VIC_atmos = _VIC_atmos;
//LML 150422 }
//_2014-10-22___________________________________________________________________
//LML 150422 void Land_unit_simulation::CropSyst_know_VIC_cell(cell_data_struct *_VIC_cell)
//LML 150422 {
//LML 150422     VIC_cell = _VIC_cell;
//LML 150422 }
//_2014-10-22___________________________________________________________________
/*141207RLN currently nothing to override
bool Land_unit_simulation::start_year()                            modification_
{
    return CropSyst::Land_unit_simulation_VX::start_year();
}
*/
//_2014-10-23___________________________________________________________________
bool Land_unit_simulation::setup_soil_property_from_VIC()
{
    VIC::Soil_layers &layers = vic_soil_for_CropSyst.layers;
    //190723LML soil = &vic_soil_for_CropSyst;                                               //LML 150325
    modifiable_ Soil::Structure *soil_structure = vic_soil_for_CropSyst.mod_structure(); //RLN150411
    for(int i = 1; i <= VIC_options.Nlayer; i++){
        int VIC_i = i-1;
        soil->structure_defined = true;
        soil_structure->set_bulk_density_kg_m3(i,VIC_soil_con.bulk_density[VIC_i]); //150411RLN
        //141207 soil->structure->set_bulk_density_g_cm3(i,kg_m3_to_g_cm3(VIC_soil_con->bulk_density[VIC_i]));
        layers.set_thickness_m(i,VIC_soil_con.depth[VIC_i]);
        soil->mod_texture()->set_sand_clay(i,
            100. * VIC_soil_con.quartz[VIC_i],                                   //170203LML soil->get_sand_percent(i) / 100.0,
            100. * VIC_soil_con.VCS.clay[VIC_i]);                                //170203LML soil->get_clay_percent(i) / 100.0);   //LML 150422

        //161104LML read the soil organic matter percentage from VIC soil parameter file
        // to initialize CropSyst SOM pools through control file
        scenario_control_and_model_options.horizon_percent_organic_matter[i] = VIC_soil_con.organic[VIC_i] * 100.0;
    }
    layers.set_thickness_provided(true);
    layers.set_depth_by_thickness(VIC_options.Nlayer);
    soil->parameters_owned = true;
    return true;
}
//_setup_soil_property_from_VIC_____________________________________2014-10-23_/
bool Land_unit_simulation::setup_weather_from_VIC()
{
   int NR_index = VIC_options.VCS.NR;
#ifdef VCS_V5
   int tomorrow = (global_rec >= (global_param.nrecs - global_param.model_steps_per_day)) ? (global_param.nrecs - global_param.model_steps_per_day) : (global_rec + global_param.model_steps_per_day);
   int yesterday = (global_rec == 0) ? 0 : (global_rec - global_param.model_steps_per_day);
#else
   int tomorrow = global_rec == global_param.nrecs ? global_param.nrecs : (global_rec + 1);
   int yesterday = global_rec == 0 ? 0 : (global_rec - 1);
#endif

   atmos_data_struct &today_atmos = VIC_atmos[global_rec];                       //160504LML
   double tmax = today_atmos.VCS.tmax;
   double tmin = today_atmos.VCS.tmin;
   double srad =
#ifdef VCS_V5
           today_atmos.VCS.sortwave_radiation_sum;
#else
           J_m2_to_MJ_m2(today_atmos.shortwave[NR_index]) * seconds_per_day;
#endif
   double tmin_tomorrow  = VIC_atmos[tomorrow].VCS.tmin;
   double tmax_yesterday = VIC_atmos[yesterday].VCS.tmax;

   weather_provider->air_temperature_max.set_Celcius(tmax                   ,measured_quality);
   weather_provider->air_temperature_min.set_Celcius(tmin                   ,measured_quality);
   weather_provider->air_temperature_avg.set_Celcius((tmax + tmin) / 2.0,measured_quality); //160504LML
   air_temperature_min_tomorrow = tmin_tomorrow;
   air_temperature_max_yesterday = tmax_yesterday;
   weather_provider->relative_humidity_avg.update_amount_in_preset_units(today_atmos.VCS.relative_humidity[NR_index]  /*,measured_quality*/); weather_provider->relative_humidity_avg.assume_code(measured_quality);
   weather_provider->relative_humidity_max.update_amount_in_preset_units(today_atmos.VCS.relative_humidity_max        /*,measured_quality*/); weather_provider->relative_humidity_max.assume_code(measured_quality);
   weather_provider->relative_humidity_min.update_amount_in_preset_units(today_atmos.VCS.relative_humidity_min        /*,measured_quality*/); weather_provider->relative_humidity_min.assume_code(measured_quality);
#ifdef VCS_V5
   weather_provider->wind_speed.set_m_s(today_atmos.VCS.wind_avg,measured_quality);
   weather_provider->precipitation_actual.set_mm(today_atmos.VCS.prec_sum,measured_quality);
   weather_provider->atmospheric_pressure.set_kPa(today_atmos.VCS.pressure_avg); weather_provider->atmospheric_pressure    .assume_code(measured_quality);
   weather_provider->solar_radiation.set_MJ_m2(srad,measured_quality);
   weather_provider->vapor_pressure_deficit_fullday_avg.set_kPa(today_atmos.VCS.vpd_avg); weather_provider->vapor_pressure_deficit_fullday_avg.assume_code(measured_quality);
#else
   weather_provider->wind_speed.set_m_s(today_atmos.wind[NR_index],measured_quality);
   weather_provider->precipitation_actual.set_mm(today_atmos.prec[NR_index],                    measured_quality);
   weather_provider->atmospheric_pressure.set_kPa(Pa_to_kPa(today_atmos.pressure[NR_index])            /*,measured_quality*/); weather_provider->atmospheric_pressure    .assume_code(measured_quality);
   weather_provider->solar_radiation.set_MJ_m2(srad,measured_quality);
   //190724LML weather_provider->vapor_pressure_deficit_fullday_avg.set_kPa(Pa_to_kPa(today_atmos.vpd[NR_index])/*,measured_quality*/); weather_provider->vapor_pressure_deficit_fullday_avg.assume_code(measured_quality);
#endif
   CS::meteorology::Storms *loc_storms = 0;
   //190723LML bool update_weather = weather_provider->update();
   const double psychrometric_constant = weather_provider->psychrometric_constant.get_kPa_per_C();
   //201216LML double ra = VIC_cell.VCS.aero_resist_daily[PET_SHORT][0] ;                        //150608
   //201216LML weather_provider->aerodynamic_resistance_ref_plants/*(day/m)*/ = CORN::must_be_greater_or_equal_to(ra,1.0e-5) / (float64)Seconds_per_day;
   //201216LML weather_provider->aerodynamics.resistance_of_plants_reference_s_m.update_amount_in_preset_units(CORN::must_be_greater_or_equal_to(ra,1.0e-5));


#ifdef LIU_DEBUG
   /*
   std::clog << "year:"         << (int)global_today.get_year()
             << "\tmonth:"      << (int)global_today.get_month()
             << "\tDOM:"        << (int)global_today.get_DOM()
             << "\tDOY:"        << (int)global_today.get_DOY()
             << "\tglobal_rec:" << global_rec
             << "\ttomorow:"    << tomorrow
             << "\tyesterday:"  << yesterday
             << "\tNR_index:"   << NR_index
             << "\ttmax:"       << tmax
             << "\ttmin:"       << tmin
             << "\tppt:"        << today_atmos.prec[NR_index]
             << "\trad(MJ/m2):" << weather_provider->solar_radiation.get_MJ_m2()
             << std::endl;
   */
#endif
   return true;//190723LML update_weather;                                                        //141207RLN
}
//_setup_weather_from_VIC___________________________________________2015-04-06_/
bool Land_unit_simulation::start_day()                            modification_
{
    weather_provider->invalidate();
    bool started = setup_weather_from_VIC();
       //&& weather_provider->update()
       //190723LML && slope_saturated_vapor_pressure_plus_psychrometric_resistance_reference->update().is_valid()

    //04012021 update CO2
    //meteorology.atmospheric_CO2_change
    //VIC CO2 concentration: options.VCS.CO2_PPM;
    weather_provider->atmospheric_CO2_conc_ppm = options.VCS.CO2_PPM;
    atmospheric_CO2.curr_CO2_conc.set_ppm(options.VCS.CO2_PPM,CORN::measured_quality);
    //std::clog << "CO2_PPM:" << weather_provider->atmospheric_CO2_conc_ppm << std::endl;

    started &= CropSyst::Land_unit_simulation_VX::start_day();
    //190723LML if (meteorology.subdaily && meteorology.subdaily_owned)  meteorology.subdaily->update();               //190515LML
    //190723LML if (meteorology.hourly   && meteorology.hourly_owned)    meteorology.hourly->update(today.get_DOY());  //190515LML
    started &= meteorology.start_day();

    //for (int hr = 0; hr < 24; hr++)
    //    std::clog << "temp hr[" << hr << "]:" << meteorology.hourly->get_air_temperature(hr) << "\n";


   //171201LML moved to initialize_at_start_of_day()
   /*
   curr_irrigation_operation = 0;                                                //170221_150720LML
   snow_pack_thickness = VIC_snow.depth;                                         //150608LML
   */
   //170727LML if (global_param.dt != 24.0) {                                                //150616LML added the condition
     /*150616LML float64 pot_ref_evapotranspiration_m = mm_to_m(VIC_cell.pot_evap_daily[PET_SHORT]);          //150608RLN
      // Warning this is assuming the VIC model is run at daily time step.
      // When running hourly, pot_evap is the last hour of the day.
     float64 pot_crop_evapotranspiration_m                                         //150608RLN
     =  ( crop_active_or_intercrop)                                                //150608RLN
        ? crop_active_or_intercrop->update_pot_evapotranspiration(pot_ref_evapotranspiration_m) //150608RLN
        : pot_ref_evapotranspiration_m;                                            //150608RLN
        */

   //171117LML update_PET_from_VIC();                                            //150616LML 171117LML NEED check why it WAS here, for subdaily timestep?

   //170727LML }
   /*
#ifdef VCS_V5
   VIC_cell.VCS.pot_evap_total_daily    = 0.0;
   VIC_cell.VCS.pot_evap_soil_daily     = 0.0;
   VIC_cell.VCS.pot_evap_veg_daily      = 0.0;
#else
   for (int i = 0; i < N_PET_TYPES; i++)
       VIC_cell.VCS.pot_evap_daily[i] = 0.0;
#endif
   VIC_cell.VCS.infiltration_daily_m = 0;                                            //151001LML
   */

   //180523LML
   #ifdef CROPSYST_HANDLE_CANOPY_EVAP
   double canopy_dew_mm = VIC_veg_var.Wdew;
   double current_dew_mm = crop_active_or_intercrop ?
                           m_to_mm(crop_active_or_intercrop->get_intercepted_precipitation_m_stored())
                           :0;
   if (crop_active_or_intercrop) {
       crop_active_or_intercrop->set_intercepted_precipitation_m_stored(mm_to_m(canopy_dew_mm));
       //VIC_veg_var.Wdew = 0;
       //This dew is processed by CropSyst to calculate transpiration (water_uptake + intercepted_evap)

       //std::clog << "startday_canopy_dew_mm:" << canopy_dew_mm
       //          << "current_dew_mm:" << current_dew_mm
       //          << "after setting:" << m_to_mm(crop_active_or_intercrop->get_intercepted_precipitation_m_stored())
       //          << std::endl;
   }
   #endif
   return started;
}
//_start_day________________________________________________________2015-06-16_/
double Land_unit_simulation::update_PET_from_VIC()                 modification_
{
    float64 pot_ref_evapotranspiration_m =
#ifdef VCS_V5
            mm_to_m(VIC_cell.VCS.pot_evap_veg_daily);
#else
            //mm_to_m(VIC_cell.VCS.pot_evap_daily[PET_SHORT]);                   //150608RLN
            mm_to_m(VIC_cell.VCS.potential_transpir);                            //180620LML the irrigation evap has been removed
#endif


    //180620LML not sure if this potential ET should deduct the evaporation from irrigation system or not
    //std::clog << " VIC_cell.VCS.potential_transpir:" << VIC_cell.VCS.potential_transpir
    //          << "\tVCS.pot_evap_daily[PET_SHORT]:" << VIC_cell.VCS.pot_evap_daily[PET_SHORT]
    //          << std::endl;


    /*std::clog << "CropSyst_PET is used!!!\n";
    meteorology.provide_ET_reference();
    meteorology.ref_ET_reference().update();
    float64 test = meteorology.ref_ET_reference().get_m();
    std::clog << "\tVIC_PET:"       << pot_ref_evapotranspiration_m
              << "\tCropSyst_PET:"  << test
              << std::endl;
    pot_ref_evapotranspiration_m = test;
    */



#ifdef LIU_DEBUG
    //std::clog << "pot_ref_evapotranspiration_mm:"
    //          //<< VIC_cell.VCS.pot_evap_daily[PET_SHORT]
    //          << pot_ref_evapotranspiration_m * 1000.0
    //          << std::endl;
#endif
     // Warning this is assuming the VIC model is run at daily time step.
     // When running hourly, pot_evap is the last hour of the day.
     
    //double pet_CropSyst =
    //   ( crop_active_or_intercrop)                                             //150608RLN
    //   ? crop_active_or_intercrop->get_pot_transpiration_m(false)
    //   : 0;
     
    float64 pot_crop_evapotranspiration_m                                        //150608RLN
    =  ( crop_active_or_intercrop)                                               //150608RLN
       ? crop_active_or_intercrop->
            update_evapotranspiration_max(pot_ref_evapotranspiration_m)          //200208_150608RLN
       : pot_ref_evapotranspiration_m;                                           //150608RLN
    
    //std::clog << "pet_CropSyst:" << pet_CropSyst
    //          << "\tpet_VIC:" << pot_ref_evapotranspiration_m
    //          << "\tpet_selected:" << (( crop_active_or_intercrop) ? crop_active_or_intercrop->get_pot_transpiration_m(false) : 0)
    //          << "\tpot_crop_evapotranspiration_m:" << pot_crop_evapotranspiration_m
    //          << "\tpot_ref_evapotranspiration_m:" << pot_ref_evapotranspiration_m
    //          << std::endl;
       
    return pot_crop_evapotranspiration_m;
}
//_update_PET_from_VIC______________________________________________2014-02-06_/
bool Land_unit_simulation::process_day()                           modification_ //150611RLN
{
   bool processed = CropSyst::Land_unit_simulation_VX::process_day();
   /*200817RLN obsolete
   perform_triggered_synchronizations_today();
   */
#ifdef CROPSYST_HANDLE_CANOPY_EVAP
   if (crop_active_or_intercrop) {
       double canopy_dew_mm = VIC_veg_var.Wdew;
       double current_dew_mm = crop_active_or_intercrop ?
                               m_to_mm(crop_active_or_intercrop->get_intercepted_precipitation_m_stored())
                               :0;
       crop_active_or_intercrop->set_intercepted_precipitation_m_stored(0.0);
       VIC_veg_var.Wdew = current_dew_mm;
       VIC_veg_var.canopyevap = canopy_dew_mm - current_dew_mm;
       if (VIC_veg_var.canopyevap < 0) {
           std::cerr << "ERROR: Negtive canopyevap!\n";
       }
       //std::clog << "\tendday_canopy_dew_mm:" << VIC_veg_var.Wdew
       //          << "\tcanopyevap:" << VIC_veg_var.canopyevap
       //          << std::endl;
   }
#endif
   return processed;
}
//_process_day___________________________________________________2015-07-18LML_/
bool Land_unit_simulation::end_day()                               modification_
{
   #if defined(MANAGEMENT) && defined(USE_CROPSYST_CROP_IRRIGATION)
   VIC_crop.total_daily_irrigation_from_CropSyst_mm = m_to_mm(total_irrigation_today);
   irrigation_operation_for_VIC_next_day            = curr_irrigation_operation; //150720LML
   #endif
   vic_soil_for_CropSyst.set_daily_infiltrated_water_m(VIC_cell.VCS.infiltration_daily_m);
   //std::clog<<"daily_infiltrated_water_m:"<<VIC_cell.infiltration_daily_m<<std::endl;
   bool processed = CropSyst::Land_unit_simulation_VX::end_day();


#ifdef LIU_DEBUG
   /*if (crop_active_or_intercrop)
      std::clog << "Accumulate_DD[adj_true] :" << crop_active_or_intercrop->get_accum_degree_days(true)
                << "Accumulate_DD[adj_false]:" << crop_active_or_intercrop->get_accum_degree_days(false)
                << std::endl;
    */
#endif
   return processed;
}
//_end_day__________________________________________________________2015-06-09_/
float64 Land_unit_simulation::get(nat32 variable_code) const
{
    float64 outvalue = 0;
///*150411 Roger will provide these
//  150528 DEBUG ONLY!!!
    if (crop_active_or_intercrop) {
        switch (variable_code) {
        case LAI:
            outvalue = (crop_active_or_intercrop->is_fruit()/*200208RLN is_orchard()*/
              && (crop_active_or_intercrop->ref_phenology().get_growth_stage_sequence()
                  == NGS_QUIESCENCE /*200208 NGS_DORMANT_or_INACTIVE*/))
                  ? 0 : crop_active_or_intercrop->get_LAI(true);
            break;
        case GAI:
            outvalue = (crop_active_or_intercrop->is_fruit()/*200208RLN is_orchard()*/
               && (crop_active_or_intercrop->ref_phenology().get_growth_stage_sequence()
                  == NGS_QUIESCENCE /*200208 NGS_DORMANT_or_INACTIVE*/))
                   ? 0 : crop_active_or_intercrop->get_GAI(true);
            break;
        case CANOPY_HEIGHT:
            outvalue = crop_active_or_intercrop->get_plant_height_m();
            break;
        case CANOPY_ALBEDO:
            outvalue = 0.1;
            break;
        case DSPLACEMENT:
            outvalue = crop_active_or_intercrop->get_plant_height_m() * 0.67;    //150713LML EddyPro™
            break;
        case EMISSIVITY:
            outvalue = -9999999;
            break;
        case RAD_ATTEN:
            outvalue = 0.5;
            break;
        case MIN_STOMATA_CONDUCTANCE:
            outvalue = 60.0;                                                     //150713LML not used for crop
            break;
        case RA_ARCHITECTURAL:
            outvalue = 25.0;                                                     //150713LML not used for crop
            break;
        case TRUNK_RATIO:
            outvalue = 0.2;
            break;
        case WIND_ATTEN:
            outvalue = 0.5;
            break;
        case WIND_H:
            outvalue = 2.0;
            break;
        case RGL:
            outvalue = 100.0;
            break;
        case ROUGHNESS:
            outvalue = 0.01;                                                     //150713LML need other parameters to decide
            break;
        case CANOPY_COVER_FRACTION_GREEN:
            outvalue = crop_active_or_intercrop->get_canopy_interception_global_green(); //200228 get_fract_canopy_cover_green();//crop_active_or_intercrop->global_solar_rad_intercepted_green_canopy();
            break;
        case CANOPY_COVER_FRACTION_TOTAL:
            outvalue = crop_active_or_intercrop->get_canopy_interception_global_total(); //200228 get_fract_canopy_cover_total();
            break;
        case ET_COEF_FULL_SHADING:


//200208RLN continue here need to restore get_adjusted_ET_crop_coefficient

            outvalue = crop_active_or_intercrop->get_canopy_interception_global_total() //200228 get_fract_canopy_cover_total()
                        > 1e-8 ?
                       crop_active_or_intercrop->get_adjusted_ET_crop_coefficient()
                       : 1.0;
            //printf("ET_COEF_FULL_SHADING(%.5f)\n",outvalue);
            break;
        case VC_root_depth_mm:
            outvalue = m_to_mm(crop_active_or_intercrop->get_recorded_root_depth_m());
            break;
        case Water_Stress_Index:
            outvalue = crop_active_or_intercrop->get_water_stress_index();
            break;
        }
    } else {
        switch (variable_code) {
        case LAI:
            outvalue = 0.0;
            break;
        case GAI:
            outvalue = 0.0;
            break;
        case CANOPY_HEIGHT:
            outvalue = 0.0;
            break;
        case CANOPY_ALBEDO:
            outvalue = 0.1;
            break;
        case DSPLACEMENT:
            outvalue = 1.0;
            break;
        case EMISSIVITY:
            outvalue = -9999999;
            break;
        case RAD_ATTEN:
            outvalue = 0.5;
            break;
        case MIN_STOMATA_CONDUCTANCE:
            outvalue = 60.0;
            break;
        case RA_ARCHITECTURAL:
            outvalue = 25.0;
            break;
        case TRUNK_RATIO:
            outvalue = 0.2;
            break;
        case WIND_ATTEN:
            outvalue = 0.5;
            break;
        case WIND_H:
            outvalue = 2.0;
            break;
        case RGL:
            outvalue = 100.0;
            break;
        case ROUGHNESS:
            outvalue = 0.0;
            break;
        case CANOPY_COVER_FRACTION_GREEN:
            outvalue = 0.0;//crop_active_or_intercrop->global_solar_rad_intercepted_green_canopy();
            break;
        case CANOPY_COVER_FRACTION_TOTAL:
            outvalue = 0.0;
            break;
        case ET_COEF_FULL_SHADING:
            outvalue = 1.0;
            break;
        case VC_root_depth_mm:
            outvalue = 0.0;
            break;
        case Water_Stress_Index:
            outvalue = 0.0;
            break;
        }
    }

    //150722 LML
    if (organic_matter_residues_profile_kg_m2)
    switch (variable_code) {
    case FRACT_RESIDUE_COVER:                                                    //160509LML
        outvalue = organic_matter_residues_profile_kg_m2->ref_residues()->get_total_fract_ground_cover(INCLUDE_POSITION_FLAT_SURFACE| INCLUDE_POSITION_STUBBLE_SURFACE);
        break;
    case SURFACE_RESIDUE_C_MASS:
        outvalue = organic_matter_residues_profile_kg_m2->sum_carbon_to_depth_kg_m2(INCLUDE_RESIDUE_OM,0.0,true);
        break;
    case SUBSURFACE_RESIDUE_C_MASS:
        outvalue = organic_matter_residues_profile_kg_m2->sum_carbon_to_depth_kg_m2(INCLUDE_RESIDUE_OM,ENTIRE_PROFILE_DEPTH_as_9999,false);
        break;
    case PROFILE_SOM_C_MASS:
        outvalue = organic_matter_residues_profile_kg_m2->sum_carbon_to_depth_kg_m2(INCLUDE_SOM_AND_MICROBIAL,ENTIRE_PROFILE_DEPTH_as_9999,false);
        break;
    case SURFACE_RESIDUE_N_MASS:
        outvalue = organic_matter_residues_profile_kg_m2->sum_nitrogen_to_depth_kg_m2(INCLUDE_RESIDUE_OM,0.0,true);
        break;
    case SUBSURFACE_RESIDUE_N_MASS:
        outvalue = organic_matter_residues_profile_kg_m2->sum_nitrogen_to_depth_kg_m2(INCLUDE_RESIDUE_OM,ENTIRE_PROFILE_DEPTH_as_9999,false);
        break;
    case PROFILE_SOM_N_MASS:
        outvalue = organic_matter_residues_profile_kg_m2->sum_nitrogen_to_depth_kg_m2(INCLUDE_SOM_AND_MICROBIAL,ENTIRE_PROFILE_DEPTH_as_9999,false);
        break;
    case PROFILE_NO3_N_MASS:
        outvalue = soil->chemicals->sum_NO3_to_depth(ENTIRE_PROFILE_DEPTH_as_9999,true);
        break;
    case PROFILE_NH4_N_MASS:
        outvalue = soil->chemicals->sum_NH4_to_depth(ENTIRE_PROFILE_DEPTH_as_9999,true);
        break;
    case CO2_C_LOSS_RESIDUE:
        outvalue = organic_matter_residues_profile_kg_m2->get_gaseous_CO2_C_loss_reported_daily(INCLUDE_RESIDUE_OM);
        break;
    case CO2_C_LOSS_SOM:
        outvalue = organic_matter_residues_profile_kg_m2->get_gaseous_CO2_C_loss_reported_daily(INCLUDE_SOM_AND_MICROBIAL);
        break;
    /*150925LML case NITRIFICATION_N2O_N:
        outvalue = soil->chemicals->NH4->get_N2O_loss();
        break;
    case DENITRIFICATION_N2O_N:
        outvalue = soil->chemicals->NO3->get_N2O_loss();
        break;*/
    }
    return outvalue;
}
//_get______________________________________________________________2014-10-23_/
bool Land_unit_simulation::initialize()                            modification_
{
   //std::clog << "Scenario file name:" << arguments->provide_scenario_file_name().c_str() << std::endl;
   //std::clog << "Debug & Hard coded for scenario file name!!!" << std::endl;
   //CORN::VV_File scenario_file("C:\\temp\\debug_VIC_CropSyst\\Scenarios\\20150413\\.CropSyst_scenario");          //160502LML

   std::string scenario_file_name(CWD_temporary.c_str());
   scenario_file_name += "/.CropSyst_scenario";

#ifdef LIU_DEBUG
   std::cerr << "\nWarning: Temporary solution for scenario file name:" << scenario_file_name << "!!!\n\n";
#endif

   CORN::VV_File scenario_file(scenario_file_name.c_str());                      //160421
   scenario_file.get(scenario_control_and_model_options);                        //160421
   scenario_control_and_model_options.start_date.set_YMD(global_simdate /*200727RLN global_today*/.get_year(), global_param.startmonth, global_param.startday);   //160503LML

   //200827RLN  Warning: I am not sure why the year is set from the global date,
   // why not global_param.start_year ????

   scenario_control_and_model_options.stop_date.set_YMD (global_param.endyear, global_param.endmonth, global_param.endday);          //160503LML
   static const nat16 SOW_EVENTS_ARRAY_ALLOC = 512;                              //150401
   #ifdef VERBOSE
   std::clog << "Composing shifted rotation" << std::endl;
   #endif

   int rotation_base_name = (rotation_or_crop_veg_class_code >= VEG_CLASS_CODE_START_SPECIFIC_SOWING)
                            ? (rotation_or_crop_veg_class_code / MULTIPLIER_FOR_SOWING_CODE)
                            : rotation_or_crop_veg_class_code;                   //170327LML
   if (rotation_base_name > 10000) rotation_base_name -= 10000;                  //200909LML nonirrigated crops
   VIC::Rotation_parameters_enumerated *rotation =
      dynamic_cast<VIC::Rotation_parameters_enumerated *>
         (VIC::rotations_parameters.find
          (rotation_base_name));
   if (!rotation)
   {
      // This code is probably a single (established crop such as orchard)
      char rotation_or_crop_veg_class_code_cstr[10];
      if (rotation_or_crop_veg_class_code > 10000) rotation_or_crop_veg_class_code -= 10000;                  //200909LML nonirrigated crops
      CORN::int32_to_cstr(rotation_or_crop_veg_class_code,rotation_or_crop_veg_class_code_cstr,10);
      CORN::OS::Directory_name_concrete project_database_directory
         (*VIC::project_directory,"Database");
      CORN::OS::Directory_name_concrete crop_directory
         (project_database_directory,"Crop/Code");
      CORN::OS::Directory_name_concrete management_directory
         (project_database_directory,"Management/Code");
      /*170221 unused
      CORN::OS::Directory_name_concrete rotation_directory
         (project_database_directory,"Rotation");
      */
      CORN::OS::File_name_concrete crop_filename
         (crop_directory,rotation_or_crop_veg_class_code_cstr,"CS_crop");

      std::clog << "crop_name:" << crop_filename.c_str() << std::endl;

      if (CORN::OS::file_system_engine.exists(crop_filename))
      {
         scenario_control_and_model_options.single.planting_date //200208RLN sowing_date
            .set(scenario_control_and_model_options.start_date);
         scenario_control_and_model_options.single.crop_filename.set_DEN(crop_filename);
         // Presume mangement file for established crop will have the same
         // name as the crop code (the management file is optional and might not used for CRB project).
         CORN::OS::File_name_concrete management_filename(management_directory,rotation_or_crop_veg_class_code_cstr,"mgt");
         std::clog << "management_filename:" << management_filename.c_str() << std::endl;
         if (CORN::OS::file_system_engine.exists(management_filename))
            scenario_control_and_model_options.single.management_filename.set_DEN(management_filename);
      } else
      {
         std::clog << "Rotation parameters not previously loaded by VIC_get_rotation_cycle_years and not established crop: "
                << rotation_or_crop_veg_class_code <<std::endl;
         return false;
      }

      scenario_control_and_model_options.cropping_system_mode_clad.set(established_cropping_system);  //170223LML


      // Need to set the sowing date for established  crop.

   } else // we have a rotation
   {
   /* Parameters have already be loaded and added to VIC::rotations_parameters list
   const CORN::OS::File_name &template_rotation_file_name = provide_template_rotation_file_name();
   {  CORN::VV_File rotation_file(template_rotation_file_name.c_str());
      if (!rotation_file.get(rotation))
         std::cerr << "error: Unable to load rotation: " << template_rotation_file_name << std::endl;
   }
   #ifdef VERBOSE
   std::clog << "rotation file:" << template_rotation_file_name.c_str() << std::endl;
   #endif
   */

   nat8 build_years = rotation->years;
   scenario_control_and_model_options.cropping_system_mode_clad.set(rotation_cropping_system);  //170223LML

   bool rotation_schedule_relative = build_years > 0;                            //140227
   // Note that phenology adjustment could be moved to Rotator::start() but current
   nat16 i;

   bool initialized = true;                                                      //150209_140722
      // After determining phenology adjustments,
      // we need to restart the weather for the starting date because
      // the soil temperature initialization that follows needs
      // the weather setup for the first day of the simulation.
   CropSyst::Sowing_event *sow_events[SOW_EVENTS_ARRAY_ALLOC]; // an arbitrary large number
   for (i = 0; i < SOW_EVENTS_ARRAY_ALLOC-1; i++)
   { sow_events[i] = 0; }
   nat8 template_sow_index = 0;
   nat8 sowing_count = 0;
   FOR_EACH_IN(sow_event,CropSyst::Sowing_event,rotation->planting_events,each_sowing)
   {  sow_events[template_sow_index] = sow_event;
      rotation_schedule_relative &= sow_event->begin_sync.is_relative();         //140227
      template_sow_index ++;
      sowing_count++;
   } FOR_EACH_END(each_sowing)
   if (rotation_schedule_relative)                                               //140227
   {
      //150521 variation for (nat8 build_year_shift_index = 0; build_year_shift_index < build_years; build_year_shift_index++)
      nat8 build_year_shift_index = rotation_cycle_index;                                //150521`
      char build_year_shift_index_cstr[10];
      CORN::nat8_to_cstr((int)build_year_shift_index,build_year_shift_index_cstr,10);

      VIC::Land_unit_simulation *shifted_scenario = this;                        //150521
      //150521 variation  Rotated_scenario_simulation *shifted_scenario  = render_scenario_simulation(build_year_shift_index); // The shift index is used as the ID
      //150521 variation land_unit_simulations.append(shifted_scenario);
      nat8 target_seq_year_index = build_year_shift_index;
      for (Year year = simulation_control.start_date.get_year()                  //151006RLN ref_start_date.get_year()
          ;year <= simulation_control.stop_date.get_year()                       //151006RLN ref_stop_date.get_year()
          ;year ++)
      {  for (nat8 sow_index = 0; sow_index < sowing_count ;sow_index++)
         {  CropSyst::Sowing_event *template_sow_event = sow_events[sow_index];
            Year template_sow_seq_year =  template_sow_event->begin_sync.get_year();
            if ((template_sow_event) && (template_sow_seq_year == target_seq_year_index))
            {  CropSyst::Sowing_event *gen_event = new CropSyst::Sowing_event(); //130611
               CropSyst::Sowing_operation &template_sow_op =  dynamic_cast<CropSyst::Sowing_operation &>(*(template_sow_event->get_operation()));

               char sow_year_cstr[10]; CORN::nat16_to_cstr((int)year,sow_year_cstr,10);
               gen_event->ID = "Sowing:";  gen_event->ID.append(build_year_shift_index_cstr); gen_event->ID.append(":");gen_event->ID.append(sow_year_cstr);

#ifdef LIU_DEBUG
               //std::clog << "Sowing event ID:" << gen_event->ID << std::endl;
               // We have to have a copy of the sowing operation because
               // loaded events are transfer for each sowing event.
#endif

               CropSyst::Sowing_operation *gen_sow_op = new CropSyst::Sowing_operation(template_sow_op); //130909 *(dynamic_cast<const CropSyst::Sowing_operation *>(template_sow_event->get_operation())));
               gen_event->set_operation(gen_sow_op,true);                        //130906_130611
               Year gen_event_year = year;                                       //130611
               CORN::DOY gen_event_DOY =template_sow_event->begin_sync.get_DOY();//130611
               gen_event->begin_sync.set_YD(gen_event_year,gen_event_DOY);       //130611
               gen_event->begin_sync.sync_mode_clad.set(ACTUAL_DATE_MODE);       //190128RLN_130611
               gen_event->date.set_YD(gen_event_year,gen_event_DOY);             //130830
               CORN::Date_clad_32 planting_date (year,gen_event_DOY);
               const CORN::OS::Directory_name &rotation_dir =
                  rotation->rotation_filename_owned->
                  //150521 variation  template_rotation_file_name.
                     get_parent_directory_name_qualified();

               CORN::OS::File_name_concrete crop_filename_unresolved(gen_sow_op->crop_filename.c_str());
               CORN::OS::Directory_entry_name *crop_filename_reset
                 = CORN::OS::file_system_engine.render_resolved_absolute_DEN     //161031LML render_resolved_absolute_directory_entry_name
                  (CORN::OS::Directory_entry_name_concrete(gen_sow_op->crop_filename.c_str()),rotation_dir);
               if (crop_filename_reset)
                  gen_sow_op->crop_filename.set_DEN(*crop_filename_reset);
               if (!CORN::OS::file_system_engine.exists(gen_sow_op->crop_filename))
               {
                 std::cerr<<"ERROR!! Crop_file:"<<gen_sow_op->crop_filename.c_str()<<" doesn't exit!!\n";
               }
               delete crop_filename_reset;
               CORN::OS::Directory_entry_name *management_filename_reset
                  = CORN::OS::file_system_engine.render_resolved_absolute_DEN    //161031LML render_resolved_absolute_directory_entry_name
                   (CORN::OS::Directory_entry_name_concrete(gen_sow_op->management_filename.c_str()),rotation_dir);
               if (management_filename_reset)
                  gen_sow_op->management_filename.set_DEN(*management_filename_reset);
               if (!CORN::OS::file_system_engine.exists(gen_sow_op->management_filename))
               {
                 std::cerr<<"ERROR!! Management_file:"<<gen_sow_op->management_filename.c_str()<<" doesn't exit!!\n";
               }
               delete management_filename_reset;
               shifted_scenario->
                  scenario_event_list.append(gen_event);                         //130830

#ifdef LIU_DEBUG
               std::clog << "\nYear:"<<year
                         <<"\tSCHEDULE_PLANTING_EVENT for Crop:"<<rotation_or_crop_veg_class_code
                         <<"\tRotation_cycle:"<<(int)rotation_cycle_index
                         <<"\tsow:"<<(int)sow_index
                         <<"\tplanting_DOY:"<<gen_event->date.get_DOY()
                         <<"\tevent ID:" << gen_event->ID
                         <<std::endl;
#endif

               shifted_scenario->schedule_planting_event
                 (planting_date.get_date32(),*gen_sow_op,gen_event->ID.c_str());
               //150521 variation  know_sowing_for_phenology_adjustment(*gen_sow_op,planting_date);

         }  }
         target_seq_year_index ++;                                               //130611
         if (target_seq_year_index >= build_years)                               //130611
            target_seq_year_index = 0;                                           //130611
      }
/*for debugging
      if (is_detailed_output_cell(ID))
      {  // These outputs are only for debugging they are not used.
         char debug_X_csn[25];  strcpy(debug_X_csn,"X.CS_scenario");
         debug_X_csn[0] = (int)('0') + build_year_shift_index;
         CORN::OS::File_name_concrete dbg_scenario_filename(get_output_directory_name(),debug_X_csn);
         CORN::VV_File debug_scenario(dbg_scenario_filename.c_str());
         debug_scenario.set(shifted_scenario->scenario_control_and_model_options);

         char debug_X_rot[25];  strcpy(debug_X_rot,"X.scheduled_operations");
         debug_X_rot[0] = (int)('0') + build_year_shift_index;
         CORN::OS::File_name_concrete dbg_rotation_filename(get_output_directory_name(),debug_X_rot);
         //CORN::VV_File debug_rotation(dbg_rotation_filename.c_str());

         std::ofstream debug_rotation(dbg_rotation_filename.c_str());
         FOR_EACH_IN(sow_evnt,CropSyst::Sowing_event,shifted_scenario->scenario_event_list,each_SE)
            shifted_scenario->scenario_event_list.write(debug_rotation);
            //sow_evnt->log(debug_rotation);
         FOR_EACH_END(each_SE)
      }
*/
      // for
   } else // not a relative rotation schedule                                    //140126
   {  // This is the case using rotation shifting, just a single land unit

      //NYI continue here need to setup  single crop and management

   } // if rotation relative
   }

   #ifdef USE_CROPSYST_CROP_IRRIGATION
   VIC_crop.total_daily_irrigation_from_CropSyst_mm = 0.0;                       //150718LML
   #endif
   meterology.know_weather_provider(weather_provider);                           //160217RLN
   meterology.know_weather_spanning(weather_spanning);
   //if (!meterology.ET_ref) meterology.provide_ET_reference();                    //190721LML
   meterology.initialize(); //190723LML
   //161107LML initilize SOM
   for(int i = 1; i <= VIC_options.Nlayer; i++){
       int VIC_i = i-1;
       scenario_control_and_model_options.horizon_percent_organic_matter[i]
           = VIC_soil_con.organic[VIC_i] * 100.0;
   }
   return CropSyst::Land_unit_simulation::initialize();
   //RLN return true;                                                                  //LML 150528
}
//_initialize_______________________________________________________2015-04-27_/
bool Land_unit_simulation::start()
{
      weather_provider = new Weather_provider
      (ref_today()//160216LML simulation_unit_today
      ,geolocation
      //190723LML ,extraterrestrial_solar_irradiance//150707_day
      ,meterology.provide_solar_radiation_extraterrestrial()                     //200817RLN_190723LML
      ,&location_params);                                                        //150604RLN_150325LML
      //160421RLN meteorology.submit_weather_provider(weather_provider);
      initialize();                                                                //150528LML
      meterology.start(); //190723LML

      // RLN I think location params are optional

/* RLN location must be setup before instanciating Weather provider
   weather_provider->location->geolocation->set_elevation_meter(VIC_soil_con.elevation);
   weather_provider->location->geolocation->set_latitude_dec_deg_32(VIC_soil_con.lat);
   weather_provider->location->geolocation->set_latitude_dec_deg_32(VIC_soil_con.lng);

*/
   slope_saturated_vapor_pressure_plus_psychrometric_resistance_reference
      = new Slope_saturated_vapor_pressure_plus_psychrometric_resistance
        (weather_provider->vapor_pressure_saturation_slope
         ,weather_provider->psychrometric_constant
         ,weather_provider->aerodynamics
         ,reference_canopy_resistance_d_m);



   //141208 RLN moved from start_day_VICxx()
   float64 longitude_radians = weather_provider->location->geolocation->get_longitude_radians(); //141208  VIC_soil_con->lng;
   float64 standard_meridian_radians = longitude_radians; // Currently we do not have standard meridian in geolocation so simply using the longitude. 071116    //Weather_database *weather_database = 0;
   //Sun_days *sun_days = new Sun_days( longitude_radians, standard_meridian_radians);

    bool bset_soil_property;
    bset_soil_property = setup_soil_property_from_VIC();
    assert(bset_soil_property==true);


    //150609 CropSyst::Land_unit_simulation_VX::initialize();                             //150417RLN
    return CropSyst::Land_unit_simulation_VX::start();
}
//_start_______________________________________________________________________/
void Land_unit_simulation::know_pychrometrics                                          //140508
         (float64 vapor_pressure_saturation_slope_kPa_per_C
         ,float64 psychrometric_constant_kPa_per_C
         ,float64 aerodynamic_resistance_ref_plants)               modification_
{
   weather_provider->vapor_pressure_saturation_slope.set_kPa_per_C
      (vapor_pressure_saturation_slope_kPa_per_C,CORN::Quality_clad(measured_quality));
    weather_provider->psychrometric_constant.set_kPa_per_C
      (psychrometric_constant_kPa_per_C);
    weather_provider->aerodynamic_resistance_ref_plants = aerodynamic_resistance_ref_plants;
}
//_know_pychrometrics___________________________________________2014-02-08_RLN_/
const CropSyst::Irrigation_operation_struct *Land_unit_simulation::get_irrigation_parameters() const
{  return curr_automatic_irrigation_mode;
}
//_get_irrigation_parameters____________________________________2015-05_08_LML_/
const Air_temperature_average &Land_unit_simulation::ref_air_temperature_avg()  const
{  return weather_provider->air_temperature_avg;
}
//_ref_air_temperature_avg______________________________________2015-05-27_RLN_/
const Wind_speed              &Land_unit_simulation::ref_wind_speed()           const
{  return weather_provider->wind_speed;
}
//_ref_wind_speed_______________________________________________2015-05-27_RLN_/
const Slope_saturated_vapor_pressure_plus_psychrometric_resistance *
Land_unit_simulation::get_slope_saturated_vapor_pressure_plus_psychrometric_resistance_reference() const
{  return slope_saturated_vapor_pressure_plus_psychrometric_resistance_reference;
}
//_Slope_saturated_vapor_pressure_plus_psychrometric_resistance_____2015-06-02_/
/*190129RLN I think this is obsolete
float64 Land_unit_simulation::set_reference_canopy_resistance
( float64 _reference_canopy_resistance_d_m) rectification_
{ return reference_canopy_resistance_d_m = _reference_canopy_resistance_d_m;
}
*/
//_get_slope_SVP_plus_psychrometric_resistance_reference____________2015-06-08_/
float64 Land_unit_simulation::cal_psychrometric_constant()                       //(kPa_per_C) 150608LML
{
    double gamma;                 /* psychrometric constant (Pa/C) */
    /* calculate scale height based on average temperature in the column */
    double tair = weather_provider->air_temperature_avg.Celcius();
    double elevation = VIC_soil_con.elevation;
    const double lapse_rate =
#ifdef VCS_V5
            param.LAPSE_RATE;
#else
            LAPSE_PM;
#endif
    const double ps =
#ifdef VCS_V5
                    CONST_PSTD;
#else
                    PS_PM;
#endif
    double h  = 287./CONST_G * ((tair + 273.15) + 0.5 * elevation * lapse_rate);
    /* use hypsometric equation to calculate p_z, assume that virtual
       temperature is equal air_temp */
    double pz = ps * exp(-(double)elevation/h);
    /* calculate latent heat of vaporization. Eq. 4.2.1 in Handbook of
       Hydrology, assume Ts is Tair */
    double lv = 2501000. - 2361. * tair;
    /* calculate gamma. Eq. 4.2.28. Handbook of Hydrology */
    gamma = 1628.6 * pz/lv;
    return gamma / 1000.;
}
//_cal_psychrometric_constant____________________________________2015-07-20LML_/
CropSyst::Irrigation_operation *Land_unit_simulation::get_irrigation_operation_for_VIC_next_day()
{
   return
   #ifdef USE_CROPSYST_CROP_IRRIGATION
   irrigation_operation_for_VIC_next_day;
   #else
   0;
   #endif
}
//_get_irrigation_operation_for_VIC_next_day________________________2015-06-03_/
nat8 Land_unit_simulation::chemical_transport_and_exchange
(Seconds               at_time
,Seconds               duration
,soil_layer_array64   (old_liquid_water_content)
,soil_layer_array64   (new_liquid_water_content)
,float64               water_entering_soil
,soil_layer_array64   (water_flow))                                modification_
{
   nat8 layers = vic_soil_for_CropSyst.ref_layers()->count(); //190128RLN get_number_layers();
   //190128 Soil_chemicals_profile
   ::Soil::Chemicals_profile                                                     //190128
   *chemicals = vic_soil_for_CropSyst.provide_chemicals();
   if (chemicals)
   {
       chemicals->chemical_transport_and_exchange
         (CASCADE_DAILY_INFILTRATION // because VIC is layer based and can be subdaily
         ,layers
         ,layers
         ,at_time
         ,duration                  // seconds of duration
         ,old_liquid_water_content  // layer based
         ,new_liquid_water_content  // layer based
         ,water_entering_soil       // total water entering (irrig + precip)
         ,water_flow                // node based for F.D. otherwise layer base
         ,0.0                       // deferment_fraction only applied to F.D.
         ,false
         ,0                         // drainage_layer not applicable
         ,false);
       return 1;
   } else {
   return 0;
   }
}
//_chemical_transport_and_exchange______________________________2015-07-26_RLN_/
class Association_crop_irrigation_type
: public implements_ CORN::Item
{public:
   std::string crop_type_name;
   std::string irrigation_type_code;
 public:
   inline virtual bool is_key_string(const std::string &key)        affirmation_ //180820
      { return crop_type_name == key; }
/*180820  was probably only used for find_cstr now using is_key
   inline virtual const char *get_key()                                    const
      { return crop_type_name.c_str(); }
*/      
};
//_class_Association_crop_irrigation_type _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _/
/*200208RLN This appears to be obsolete

bool Land_unit_simulation::get_cell_irrigation_type_code_append_to
(const         std::string &crop_type_name
,modifiable_   std::string &irrigation_type_code)                  contribution_
{  bool recognized = false;
   Association_crop_irrigation_type *crop_irrig_type_assoc = 0;
   crop_irrig_type_assoc =
      dynamic_cast<Association_crop_irrigation_type *>
      (recognized_crop_irrig_type_assocations.find_string(crop_type_name));
   if (!crop_irrig_type_assoc) // we haven't yet loaded into memory
   {  // Try to load it from the
      crop_irrig_type_assoc = new Association_crop_irrigation_type;
      CORN::OS::File_name_concrete automatic_irrigation_filename
         (scenario_directory->Database(false).Management()
         ,L"automatic_irrigation"
         ,L"dat");
      std::ifstream automatic_irrigation_file(automatic_irrigation_filename.c_str());
      Association_crop_irrigation_type assoc_rec;

      while (!automatic_irrigation_file.eof() && !recognized)
      {
         automatic_irrigation_file
         >> crop_irrig_type_assoc->crop_type_name
         >> crop_irrig_type_assoc->irrigation_type_code;
         if (crop_irrig_type_assoc->crop_type_name == crop_type_name)
         {  recognized_crop_irrig_type_assocations.append(crop_irrig_type_assoc);
            recognized = true;
         }
      }
      if (!recognized)
      {  delete crop_irrig_type_assoc; crop_irrig_type_assoc = 0; }
   }
   if (crop_irrig_type_assoc)
   {
      recognized = true;
      irrigation_type_code.append(crop_irrig_type_assoc->irrigation_type_code);
   }
   return recognized;
}
//_get_cell_irrigation_type_code_append_to______________________2015-10-07_RLN_/
*/
Event_status_indicator Land_unit_simulation::perform_operation (Common_operation &op)
{
    /*
   Event_status_indicator event_status = ES_FAIL;                                          //030519
   CropSyst::Operation &CS_op = (CropSyst::Operation &)op;                                           //120416
   CropSyst_Op op_type = (CropSyst_Op)CS_op.get_type();
   switch (op_type)
   {
      case CS_OP_ROT_SOWING :
      {  CropSyst::Sowing_operation & sowing_op = (CropSyst::Sowing_operation &)CS_op;               //030519

         // We are not actually overriding the sowing operation.
         // The crop to sow is identified in order to setup
         // irrigation type and we then load supplementary automatic
         // irrigation management parameter file fragment.

         std::string crop_type_name;
         CORN::append_wstring_to_string(sowing_op.crop_filename.get_name(),crop_type_name);
         std::string auto_crop_irrigation_code(crop_type_name);
         auto_crop_irrigation_code.append("_");
         bool has_auto_irrig = get_cell_irrigation_type_code_append_to(crop_type_name,auto_crop_irrigation_code);
         if (has_auto_irrig)
         {

            CropSyst::Management_parameters auto_crop_irrig_management;
            CORN::OS::File_name_concrete auto_crop_irrig_filename
               (scenario_directory->Database(false).Management(),auto_crop_irrigation_code,"mgt");
            CORN::VV_File automatic_irrigation_file(auto_crop_irrig_filename.c_str());
            automatic_irrigation_file.get(auto_crop_irrig_management);
            event_scheduler->take_events(auto_crop_irrig_management.auto_irrigation_mode_operations);
         }
         // else there is no automatic irrigation associated with this crop
         // in this cell.
      } break;
   }*/
   return CropSyst::Land_unit_simulation::perform_operation(op);
}
//_perform_operation____________________________________________2015-10-07_RLN_/
nat8 Land_unit_simulation::provide_zone()                             provision_
{
/*151015 NYI I need to implement database discovery
   if (!zone)
   {
      CORN::OS::File_name_concrete cell_zone_filename
         (master_projects_directory,"crop_cells_zoning","txt");
      std::ifstream cell_zone_file(cell_zone_filename.c_str());
      int32 record_cell_ID;
      nat8  record_zone;
      while (!zone)
      {
         cell_zone_file >> record_cell_ID >> record_zone;
         if (record_cell_ID == Land_unit_simulation::Land_unit::ID)
            zone = record_zone
      }
   }
*/
   return zone;
}
//171201LML
int Land_unit_simulation::initialize_at_start_of_day()
{
    curr_irrigation_operation = 0;                                                //170221_150720LML
    snow_pack_thickness = VIC_snow.depth;                                         //150608LML
 #ifdef VCS_V5
    VIC_cell.VCS.pot_evap_total_daily    = 0.0;
    VIC_cell.VCS.pot_evap_soil_daily     = 0.0;
    VIC_cell.VCS.pot_evap_veg_daily      = 0.0;
 #else
    for (int i = 0; i < N_PET_TYPES; i++)
        VIC_cell.VCS.pot_evap_daily[i] = 0.0;
 #endif
    VIC_cell.VCS.infiltration_daily_m = 0;                                            //151001LML
    return 0;
}
//_provide_zone_________________________________________________2015-10-08_RLN_/
Event_status_indicator Land_unit_simulation::perform_irrigation
(CropSyst::Irrigation_operation   &irrig_op)
{
    curr_irrigation_operation = &irrig_op;                                        //150718LML
    return CropSyst::Land_unit_simulation::perform_irrigation(irrig_op);
}
//_perform_irrigation_______________________________________________2017-02-21_/
bool Land_unit_simulation::Weather_spanning::get_parameter_on
      (modifiable_ CS::Parameter &parameter
      ,nat32  variable_code // Typically the variable code will be UED variable code
      ,const CORN::Date_const &on_date)                           rectification_
{
    CORN::Date_cowl_const_32 simdate_cowl(simdate);                              //190130
    int offset = simdate_cowl.days_between(on_date, false);                      //190130
    //190130 int offset = today.days_between(on_date, false);
    int on_date_index = global_rec + offset;
    if (variable_code == UED::STD_VC_max_temperature.get()) {
       parameter.force_amount(VIC_atmos[on_date_index].VCS.tmax);
       parameter.assume_code(CORN::observed_quality);                            //170327LML
    }
    if (variable_code == UED::STD_VC_min_temperature.get()) {
       parameter.force_amount(VIC_atmos[on_date_index].VCS.tmin);
       parameter.assume_code(CORN::observed_quality);                            //170327LML
    }
    return true;
}
//_get_parameter_on_____________________________________________2016-02-17_RLN_/
float64 Land_unit_simulation::Weather_spanning::get_air_temperature_min_tomorrow()                  const
{  return air_temperature_min_tomorrow;
}
//_get_air_temperature_min_tomorrow_____________________________2016-02-17_RLN_/
float64 Land_unit_simulation::Weather_spanning::get_air_temperature_max_yesterday()                 const
{  return air_temperature_max_yesterday;
}
//_get_air_temperature_max_yesterday____________________________2016-02-17_RLN_/

}// namespace VIC
bool quick_and_dirty_disable_chem_balance_when_watertable = true;                //150526 150701LML set true

#ifdef __BCPLUSPLUS__
//190131 BC++ doesn't have getopt system
int getopt(int argc, char *argv[], char * optstring)
{
   assert(false);
   return 0;
}
char *optarg;
#endif

#endif //VIC_CROPSYST_VERSION
