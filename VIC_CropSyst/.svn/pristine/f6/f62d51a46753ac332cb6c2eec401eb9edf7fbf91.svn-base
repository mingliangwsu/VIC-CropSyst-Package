#-------------------------------------------------
#
# Project created by QtCreator 2014-03-04T02:09:34
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = VIC_CropSyst
CONFIG   += console
CONFIG   -= app_bundle
QMAKE_CXXFLAGS += -fpermissive  -w -g -O0
QMAKE_CXXFLAGS += -Wno-reorder -Wno-unused-parameter -Wno-unused-but-set-parameter
DEFINES += xCROPSYST_ON \
    CS_VERSION_NUMBERS="5,0,0" \
    CROPSYST_MAX_DESCRIPTION_LENGTH=128 \
    CROPSYST_VV_BOOL_FORM=CORN::VV_bool_entry::form_true \
    Normal_crop_growth_stage=Normal_crop_event_sequence \
    USE_STD_STRING \
    LIU_DEBUG \
    CS_VERSION=5 \
    VIC_CROPSYST_VERSION=3 \
    CROPSYST_VERSION=5 \
    WEATHER_PROVIDER_VERSION=5 \
    USE_WEATHER_PROVIDER \
    CHECK_WATER_BALANCE \
    EVENT_SCHEDULER_VERSION=1 \
    xUSE_CROPSYST_CROP_IRRIGATION \
    PRINT_VIC_SOIL_LAYERS_FOR_DEBUG \
    MECHANISTIC_IRRIGATION \
    IRRIGATION_FROM_DEMAND \
    USE_IRRIGATION_PARAM_FILE \
    SATURATE_FLOW_TO_LOWER_LAYER_FIRST \
    OLD_ORCHARD \
    SEPERATE_SOIL_PARAMETER_AND_CROP_OUTPUT \
    IRRIGATION_MANAGEMENT_QUALITY=1
#LML 150707 added CHECK_WATER_BALANCE
#   CROPSYST
#    NITROGEN
#   USE_OS_FS \

#LML 140915 check with roger about the macro WEATHER_PROVIDER_VERSION
#LML 140916 check if use CROPSYST macro
QMAKE_CXX = g++
QMAKE_CC = g++
# QMAKE_EXT_CPP += .c

TEMPLATE = app
#../../../../ for CORN
INCLUDEPATH += ../../../../VIC_CropSyst_common \
               ../../../../VIC_CropSyst/vic_5_emulator/vic/vic_run/include \
               ../../../../VIC_CropSyst/vic_5_emulator/vic/drivers/shared_all/include \
               ../../../ \
               ../../ \
               ../../../crop \
               ../../../../ \
               ../../../../common \
               ../../../../corn \
               ../../../../CropSyst \
               ../../../../CropSyst/source \
               ../../../../USDA/NRCS

SOURCES +=      ../../../CalcAerodynamic.c \
                ../../../CalcBlowingSnow.c \
                ../../../IceEnergyBalance.c \
                ../../../SnowPackEnergyBalance.c \
                ../../../StabilityCorrection.c \
                ../../../advected_sensible_heat.c \
                ../../../alloc_atmos.c \
                ../../../arno_evap.c \
                ../../../calc_air_temperature.c \
                ../../../calc_atmos_energy_bal.c \
                ../../../calc_cloud_cover_fraction.c \
                ../../../calc_forcing_stats.c \
                ../../../calc_longwave.c \
                ../../../calc_rainonly.c \
                ../../../calc_root_fraction.c \
                ../../../calc_snow_coverage.c \
                ../../../calc_surf_energy_bal.c \
                ../../../calc_veg_params.c \
                ../../../calc_water_energy_balance_errors.c \
                ../../../call_crop_model.c \
                ../../../canopy_evap.c \
                ../../../check_files.c \
                ../../../check_state_file.c \
                ../../../close_files.c \
                ../../../cmd_proc.c \
                ../../../compress_files.c \
                ../../../compute_dz.c \
                ../../../compute_pot_evap.c \
                ../../../compute_treeline.c \
                ../../../compute_zwt.c \
                ../../../correct_precip.c \
                ../../../display_current_settings.c \
                ../../../dist_prec.c \
                ../../../estimate_T1.c \
                ../../../free_crop.c \
                ../../../free_dist_prcp.c \
                ../../../free_vegcon.c \
                ../../../frozen_soil.c \
                ../../../full_energy.c \
                ../../../func_atmos_energy_bal.c \
                ../../../func_atmos_moist_bal.c \
                ../../../func_canopy_energy_bal.c \
                ../../../func_surf_energy_bal.c \
                ../../../get_dist.c \
                ../../../get_force_type.c \
                ../../../get_global_param.c \
                ../../../ice_melt.c \
                ../../../initialize_atmos.c \
                ../../../initialize_crop.c \
                ../../../initialize_global.c \
                ../../../initialize_lake.c \
                ../../../initialize_model_state.c \
                ../../../initialize_new_storm.c \
                ../../../initialize_snow.c \
                ../../../initialize_soil.c \
                ../../../initialize_veg.c \
                ../../../lakes.eb.c \
                ../../../latent_heat_from_snow.c \
                ../../../make_cell_data.c \
                ../../../make_dist_prcp.c \
                ../../../make_dmy.c \
                ../../../make_energy_bal.c \
                ../../../make_in_and_outfiles.c \
                ../../../make_snow_data.c \
                ../../../make_veg_var.c \
                ../../../massrelease.c \
                ../../../modify_Ksat.c \
                ../../../mtclim_vic.c \
                ../../../mtclim_wrapper.c \
                ../../../newt_raph_func_fast.c \
                ../../../nrerror.c \
                ../../../open_debug.c \
                ../../../open_file.c \
                ../../../open_state_file.c \
                ../../../output_list_utils.c \
                ../../../parse_output_info.c \
                ../../../penman.c \
                ../../../prepare_full_energy.c \
                ../../../put_data.c \
                ../../../read_arcinfo_ascii.c \
                ../../../read_atmos_data.c \
                ../../../read_croplib.c \
                ../../../read_cropparam.c \
                ../../../read_forcing_data.c \
                ../../../read_initial_model_state.c \
                ../../../read_lakeparam.c \
                ../../../read_snowband.c \
                ../../../read_soilparam.c \
                ../../../read_soilparam_arc.c \
                ../../../read_veglib.c \
                ../../../read_vegparam.c \
                ../../../redistribute_during_storm.c \
                ../../../root_brent.c \
                ../../../runoff.c \
                ../../../set_output_defaults.c \
                ../../../snow_intercept.c \
                ../../../snow_melt.c \
                ../../../snow_utility.c \
                ../../../soil_conduction.c \
                ../../../soil_thermal_eqn.c \
                ../../../solve_snow.c \
                ../../../store_moisture_for_debug.c \
                ../../../surface_fluxes.c \
                ../../../svp.c \
                ../../../vicNl.c \
                ../../../vicerror.c \
                ../../../water_energy_balance.c \
                ../../../water_under_ice.c \
                ../../../write_atmosdata.c \
                ../../../write_data.c \
                ../../../write_debug.c \
                ../../../write_forcing_file.c \
                ../../../write_header.c \
                ../../../write_layer.c \
                ../../../write_model_state.c \
                ../../../write_snow_data.c \
                ../../../write_soilparam.c \
                ../../../write_vegparam.c \
                ../../../write_vegvar.c \
                ../../../global.c \
    ../../../agronomic/VIC_soil.cpp \
    ../../../agronomic/VIC_land_unit_simulation.cpp \
    ../../../agronomic/VIC_land_unit_C_interface.cpp \
    ../../../../UED/library/UED_fname.cpp \
    ../../../../common/simulation/operation.cpp \
    ../../../../corn/math/numerical/interpolate.cpp \
    ../../../crop/VIC_soil_hydrology.c \
    ../../../../CropSyst/source/cs_scenario_directory.cpp \
    ../../../../CropSyst/source/crop/yield.cpp \
    ../../../agronomic/balanceitem.cpp \
    ../../../read_irrigparam.c \
    ../../../../CS_suite/observation/CS_inspector.cpp \
    ../../../../CS_suite/simulation/CS_simulation_element_plugin.cpp \
    ../../../../common/biometeorology/ET_reference.cpp \
    ../../../../CS_suite/observation/CS_examination.cpp \
    ../../../../corn/measure/measures.cpp \
    ../../../../corn/measure/common_units.cpp \
    ../../../vic_5_emulator/vic/vic_run/src/physics.c \
    ../../../vic_5_emulator/vic/drivers/shared_all/src/initialize_parameters.c \
    ../../../../VIC_CropSyst_common/VCS_Nl.c \
    ../../../../corn/chronometry/time_conversion.cpp \
    ../../../agronomic/irrigation_lib.c \
    ../../../../corn/chronometry/date_time_A.cpp \
    ../../../../common/weather/seasons.cpp \
    ../../../../common/weather/snow_pack.cpp \
    ../../../../common/weather/subdaily.cpp \
    ../../../../common/weather/hour/weather_hours.cpp \
    ../../../../common/weather/parameter/WC_ET_reference.cpp \
    ../../../../common/weather/atmospheric_CO2_change_recalibration.cpp \
    ../../../../CORN/application/explaination/explainations.cpp


contains(DEFINES,"VIC_CROPSYST_VERSION=3"){
    message("VIC_CROPSYST_VERSION=3...")
    SOURCES +=
}


HEADERS += \
                ../../../global.h \
                ../../../irrigation_lib.h \
                ../../../mtclim42_vic.h \
                ../../../mtclim_constants_vic.h \
                ../../../mtclim_parameters_vic.h \
                ../../../snow.h \
                ../../../user_def.h \
                ../../../vicNl.h \
                ../../../vicNl_def.h \
                ../../../wgtpar.h \
                ../../../LAKE.h \
    ../../../crop/vic_crop_land_unit_simulation.h \
    ../../../agronomic/search_veg_lib.h \
    ../../../agronomic/VIC_land_unit_C_interface.h \
    ../../../agronomic/VIC_land_unit_simulation.h \
    ../../../agronomic/VIC_soil.h \
    ../../../../VIC/vic/vic_run/include/vic_physical_constants.h \
    ../../../WSU_vicNl.h \
    ../../../WSU_vicNl_def.h \
    ../../../vic_5_emulator/vic/vic_run/include/vic_run.h \
    ../../../vic_5_emulator/vic/vic_run/include/vic_def.h \
    ../../../vic_5_emulator/vic/drivers/shared_all/include/vic_driver_shared_all.h \
    ../../../../VIC_CropSyst_common/VCS_Nl.h \
    ../../../../VIC_CropSyst_common/VCS_global.h \
    ../../../../VIC_CropSyst_common/VCS_user_def.h

contains(DEFINES,"VIC_CROPSYST_VERSION=3"){
    message("CROPSYST_ON... add head files.")
    HEADERS += \
                ../../../crop/VIC_crop_C_interface.h \
                ../../../crop/VIC_soil_hydrology.h \
                ../../../crop/biometeorology/ET_common.h \
                ../../../crop/biometeorology/ET_model.h \
                ../../../crop/biometeorology/ET_reference.h \
                ../../../crop/biometeorology/ET_reference_interface.h \
                ../../../crop/soil/hydrology_interface.h \
                ../../../crop/soil/layers_interface.h \
                ../../../crop/unit_tests/VIC_crop_test_soil_WC.h \
                ../../../crop/unit_tests/VIC_crop_test_weather.h \
                ../../../../common/weather/ET_and_plant_influential_elements.h \
                ../../../../common/weather/current_elements_interface.h \
                ../../../../common/weather/parameter/WP_ET_reference.h \
                ../../../../common/weather/parameter/WP_air_temperature.h \
                ../../../../common/weather/parameter/WP_atmospheric_pressure.h \
                ../../../../common/weather/parameter/WP_dew_point_temperature.h \
                ../../../../common/weather/parameter/WP_precipitation.h \
                ../../../../common/weather/parameter/WP_relative_humidity.h \
                ../../../../common/weather/parameter/WP_solar_radiation.h \
                ../../../../common/weather/parameter/WP_temperature.h \
                ../../../../common/weather/parameter/WP_vapor_pressure.h \
                ../../../../common/weather/parameter/WP_vapor_pressure_act.h \
                ../../../../common/weather/parameter/WP_vapor_pressure_deficit.h \
                ../../../../common/weather/parameter/WP_wind_speed.h \
                ../../../../common/weather/parameter/weather_parameter.h \
                ../../../../common/weather/parameter/WC_solar_radiation.h \
                ../../../../common/weather/parameter/WC_vapor_pressure.h \
                ../../../../common/weather/parameter/WC_dew_point_temperature.h \
                ../../../../common/weather/parameter/WC_relative_humidity.h \
                ../../../../common/weather/parameter/WC_ET_reference.h \
                ../../../../common/weather/parameter/WC_vapor_pressure_deficit.h \
                ../../../../common/weather/parameter/WC_vapor_pressure_act.h \
                ../../../../common/weather/parameter/WC_psychrometric_const.h \
                ../../../../common/weather/plant_influential_elements.h \
                ../../../../common/weather/sun.h \
                ../../../../common/weather/weather_crop_interface.h \
                ../../../../common/weather/weather_interface.h \
                ../../../../common/weather/weather_provider.h \
                ../../../../common/weather/weather_types.h \
                ../../../../CropSyst/source/crop/phenology_interface.h \
                ../../../crop/VIC_weather_provider.h \
                ../../../../CropSyst/source/land_unit_sim.h \
                ../../../../CropSyst/source/cs_scenario.h \
                ../../../../CropSyst/source/soil/chemicals_profile.h \
                ../../../../CropSyst/source/soil.h \
                ../../../../corn/container/item.h \
                #../../options.h \
                ../../../../CropSyst/source/options.h \
                ../../../../common/residue/residues_cycling.h \
                ../../../../CropSyst/source/organic_matter/OM_common.h \
                ../../../../CropSyst/source/organic_matter/OM_pools_common.h \
                ../../../../common/residue/residues_interface.h \
                ../../../../common/residue/residue_V4_common.h \
                ../../../../common/residue/residues_common.h \
                ../../../../corn/container/container.h \
                ../../../../corn/container/unilist.h \
                ../../../../CropSyst/source/crop/canopy_growth_LAI_based_V5.h \
                ../../../../CropSyst/source/cs_vars_abbrv.h \
                ../../../../CropSyst/source/management/management_param_V5.h \
                ../../../../CropSyst/source/N_sources.h \
                ../../../../CropSyst/source/cs_operation.h \
                ../../../../CropSyst/source/cs_operation_codes.h \
                ../../../../common/simulation/operation.h \
                ../../../../CropSyst/source/crop/canopy_growth_LAI_based.h \
                ../../../../CS_suite/simulation/CS_simulation_element.h \
                ../../../../CropSyst/source/soil/abiotic_environment.h \
                ../../../../CropSyst/source/soil/disturbance.h \
                ../../../../CS_suite/simulation/CS_land_unit.h \
                ../../../../CropSyst/source/pond.h \
                ../../../../common/simulation/simulation.h \
                #../../../../CropSyst/source/soil/dynamic_water_entering.h \
                ../../../../common/weather/wthr.h \
                ../../../../CropSyst/source/rot_param.h \
                ../../../../CropSyst/source/cs_ET.h \
                ../../../../UED/library/timeqry.h \
                ../../../../common/weather/daily.h \
                ../../../../common/simulation/balancer.h \
                ../../../../common/biomatter/biomass_decomposition_param.h \
                ../../../../CropSyst/source/organic_matter/multiple_pool/OM_multiple_pool.h \
                ../../../../common/simulation/event_status.h \
                ../../../../common/weather/wthrbase.h \
                ../../../../common/simulation/log.h \
                ../../../../common/weather/atmospheric_CO2_change_recalibration.h \
                ../../../../CropSyst/source/soil.h \
                ../../../../CropSyst/source/soil/tillage_effect.h \
                ../../../../CropSyst/source/soil/soil_base.h \
                ../../../../CropSyst/source/soil/chemicals_profile.h \
                ../../../../common/weather/sky.h \
                ../../../../common/weather/snow.h \
                ../../../../common/weather/any_timestep/relhumid_est.h \
                ../../../../common/weather/any_timestep/temperature_est.h \
                ../../../../CropSyst/source/seeding_param.h \
                ../../../../CropSyst/source/soil/layers.h \
                ../../../../CropSyst/source/soil/sublayers.h \
                ../../../../CropSyst/source/soil/hydraulic_properties.h \
                ../../../../CropSyst/source/soil/structure_common.h \
                ../../../../CropSyst/source/soil/structure_interface.h \
                ../../../../common/weather/any_timestep/min_max_to_timestep_estimator.h \
                ../../../../CropSyst/source/soil/nitrogen_common.h \
                ../../../../common/soil/txtrhydr.h \
                ../../../../CropSyst/source/soil/temperature_hourly.h \
                ../../../../common/weather/hour/weather_hours.h \
                ../../../../CropSyst/source/soil/temperature_functions.h \
                ../../../../CS_suite/simulation/CS_simulation_unit.h \
                #../../../../CropSyst/source/soil/soil_param.h \
                #../../../../CropSyst/source/soil/hydrology_finite_diff.h \
                ../../../../CropSyst/source/soil/hydrology_cascade.h \
                #../../../../CropSyst/source/soil/infiltration_cascade_daily.h \
                #../../../../CropSyst/source/soil/infiltration_cascade_hourly.h \
                ../../../../CropSyst/source/organic_matter/multiple_pool/OM_pools_multiple.h \
                ../../../../CropSyst/source/organic_matter/OM_residues_profile_common.h \
                ../../../../CropSyst/source/organic_matter/single_pool/OM_pools_single.h \
                ../../../../CropSyst/source/cs_event.h \
                ../../../../CropSyst/source/soil/temperature_profile.h \
                ../../../../CropSyst/source/cs_accum.h \
                #../../../../CropSyst/source/soil/infiltration_finite_diff.h \
                ../../../../CropSyst/source/perform_op.h \
                ../../../../CropSyst/source/cs_mgmt.h \
                ../../../../CropSyst/source/soil/profile_texture.h \
                ../../../../CropSyst/source/soil/nitrogen_profile.h \
                ../../../../CropSyst/source/soil/chemical_profile.h \
                ../../../../USDA/NRCS/RUSLE2/SDR_field_ops.h \
                ../../../../common/weather/hour/sun_hours.h \
                ../../../../common/weather/hour/solar_irrad_hour.h \
                ../../../../common/weather/hour/ET_irrad_hour.h \
                ../../../../CS_suite/simulation/CS_event_scheduler.h \
                #../../../../CropSyst/source/soil/soil_param_class.h \
                #../../../../CropSyst/source/soil/soil_param_V5.h \
                ../../../../CropSyst/source/soil/hydrology.h \
                #../../../../CropSyst/source/soil/infiltration_cascade_common.h \
                #../../../../CropSyst/source/soil/infiltration.h \
                #../../../../CropSyst/source/soil/surface_infiltration_green_ampt.h \
                ../../../../CropSyst/source/organic_matter/OM_residues_profile_abstract.h \
                ../../../../CropSyst/source/organic_matter/single_pool/OM_single_pool.h \
                ../../../../CropSyst/source/soil/surface_temperature.h \
                ../../../../CropSyst/source/crop/crop_orchard_Kemanian_Stockle.h \
                ../../../../CropSyst/source/crop/crop_interfaced_cropsyst.h \
                ../../../../CropSyst/source/inorganic_NH4.h \
                ../../../../CropSyst/source/crop/crop_orchard_common.h \
                ../../../../CropSyst/source/crop/crop_cropsyst.h \
                ../../../../CropSyst/source/crop/crop_interfaced.h \
                ../../../../USDA/NRCS/soil/survey/domains.h \
                ../../../../CropSyst/source/soil/chemical_balance.h \
                ../../../../CropSyst/source/soil/chemical_mass_profile.h \
                ../../../../CropSyst/source/crop/crop_orchard_Oyarzun_Stockle.h \
                #../../../../CropSyst/source/soil/evaporation_without_vapor_path.h \
                #../../../../CropSyst/source/soil/evaporation_abstract.h \
                ../../../../CropSyst/source/cs_events.h \
                ../../../../CropSyst/source/crop/crop_biomass.h \
                ../../../../CropSyst/source/crop/canopy_growth.h \
                ../../../../CropSyst/source/crop/extreme_temperature_response.h \
                ../../../crop/VIC_CropSyst_proper_crop.h \
                ../../../crop/VIC_crop_common.h \
                ../../../crop/VIC_crop_interface.h \
                ../../../../CropSyst/source/crop/phenology_sequencer.h \
                ../../../../common/biomatter/biomass_abstract.h \
                ../../../../CropSyst/source/crop/canopy_cover_continuum.h \
                ../../../../CropSyst/source/crop/canopy_growth_cover_based.h \
                ../../../../CropSyst/source/crop/canopy_growth_portioned.h \
                ../../../../CropSyst/source/crop/crop_param_V5.h \
                ../../../../CropSyst/source/crop/crop_param_class.h \
                ../../../../corn/parameters/parameters_datarec.h \
                ../../../../corn/parameters/parameters.h \
                ../../../../CropSyst/source/crop/crop_param_struct.h \
                ../../../../CropSyst/source/management/management_param_class.h \
                ../../../../CropSyst/source/management/management_param_struct.h
}



win32: LIBS += -L$$PWD/../../../../common/weather/build/Qt/Windows_MinGW/Release/release/ -lweather_provider_static

INCLUDEPATH += $$PWD/../../../../common/weather/build/Qt/Windows_MinGW/Release/release
DEPENDPATH += $$PWD/../../../../common/weather/build/Qt/Windows_MinGW/Release/release

win32: LIBS += -L$$PWD/../../../../CORN/build/Qt/Windows_MinGW/Release/release/ -lcorn_complete_static

INCLUDEPATH += $$PWD/../../../../CORN/build/Qt/Windows_MinGW/Release/release
DEPENDPATH += $$PWD/../../../../CORN/build/Qt/Windows_MinGW/Release/release

win32: LIBS += -L$$PWD/../../../../CS_suite/build/Qt/CS_suite_static/Release/release/ -lCS_suite_static

INCLUDEPATH += $$PWD/../../../../CS_suite/build/Qt/CS_suite_static/Release/release
DEPENDPATH += $$PWD/../../../../CS_suite/build/Qt/CS_suite_static/Release/release

win32: LIBS += -L$$PWD/../../../../UED/library/build/Qt/UED_static/Release/release/ -lUED_static

INCLUDEPATH += $$PWD/../../../../UED/library/build/Qt/UED_static/Release/release
DEPENDPATH += $$PWD/../../../../UED/library/build/Qt/UED_static/Release/release

win32: LIBS += -L$$PWD/../../../../CropSyst/source/build/Qt/Windows_MinGW/Debug/debug/ -lCropSyst_element_V5_for_VIC_V3

INCLUDEPATH += $$PWD/../../../../CropSyst/source/build/Qt/Windows_MinGW/Debug/debug
DEPENDPATH += $$PWD/../../../../CropSyst/source/build/Qt/Windows_MinGW/Debug/debug
