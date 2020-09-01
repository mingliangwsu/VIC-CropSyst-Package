/******************************************************************************
 * @section DESCRIPTION
 *
 * Header file for vic_driver_shared_all routines
 *
 * @section LICENSE
 *
 * The Variable Infiltration Capacity (VIC) macroscale hydrological model
 * Copyright (C) 2016 The Computational Hydrology Group, Department of Civil
 * and Environmental Engineering, University of Washington.
 *
 * The VIC model is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *****************************************************************************/

#ifndef VIC_DRIVER_SHARED_H
#define VIC_DRIVER_SHARED_H

#include <vic_run.h>
#include <vic_version.h>

// Define maximum array sizes for driver level objects
#define MAX_FORCE_FILES 2
#define MAX_OUTPUT_STREAMS 20

// Output compression setting
#define COMPRESSION_LVL_UNSET -1
#define COMPRESSION_LVL_DEFAULT 5

// Default ouput values
#define OUT_MULT_DEFAULT 0  // Why is this not 1?
#define OUT_ASCII_FORMAT_DEFAULT "%.4f"

// Default snow band setting
#define SNOW_BAND_TRUE_BUT_UNSET 99999

/******************************************************************************
 * @brief   File formats
 *****************************************************************************/
enum
{
    UNSET_FILE_FORMAT,
    ASCII,
    BINARY,
    NETCDF3_CLASSIC,
    NETCDF3_64BIT_OFFSET,
    NETCDF4_CLASSIC,
    NETCDF4
};

/******************************************************************************
 * @brief   endian flags
 *****************************************************************************/
enum
{
    LITTLE,  /**< little-endian flag */
    BIG      /**< big-endian flag */
};

/******************************************************************************
 * @brief   Veg param sources
 *****************************************************************************/
enum
{
    FROM_DEFAULT,
    FROM_VEGLIB,
    FROM_VEGPARAM,
    FROM_VEGHIST
};

/******************************************************************************
 * @brief   Forcing Variable Types
 *****************************************************************************/
enum
{
    AIR_TEMP,    /**< air temperature per time step [C] */
    ALBEDO,      /**< surface albedo [fraction] */
    CATM,        /**< atmospheric CO2 concentration [ppm] */
    CHANNEL_IN,  /**< incoming channel flow [m3] */
    FCANOPY,     /**< fractional area covered by plant canopy [fraction] */
    FDIR,        /**< fraction of incoming shortwave that is direct [fraction] */
    LAI_IN,      /**< leaf area index [m2/m2] */
#ifndef VIC_CROPSYST_VERSION
    LWDOWN,      /**< incoming longwave radiation [W/m2] */
#endif
    PAR,         /**< incoming photosynthetically active radiation [W/m2] */
    PREC,        /**< total precipitation (rain and snow) [mm] */
    PRESSURE,    /**< atmospheric pressure [kPa] */
    VP,          /**< vapor pressure [kPa] */
#ifndef VIC_CROPSYST_VERSION
    SWDOWN,      /**< incoming shortwave [W/m2] */
#endif
    WIND,        /**< wind speed [m/s] */
    SKIP,        /**< place holder for unused data columns */
#ifdef VIC_CROPSYST_VERSION
    CRAINF,      /* convective rainfall [mm] (ALMA_INPUT: [mm/s]) */
    CSNOWF,      /* convective snowfall [mm] (ALMA_INPUT: [mm/s]) */
    LSRAINF,     /* large-scale rainfall [mm] (ALMA_INPUT: [mm/s]) */
    LSSNOWF,     /* large-scale snowfall [mm] (ALMA_INPUT: [mm/s]) */
    RAINF,       /* rainfall (convective and large-scale) [mm] (ALMA_INPUT: [mm/s]) */
    SNOWF,       /* snowfall (convective and large-scale) [mm] (ALMA_INPUT: [mm/s]) */
    TMAX,        /* maximum daily temperature [C] (ALMA_INPUT: [K]) */
    TMIN,        /* minimum daily temperature [C] (ALMA_INPUT: [K]) */
    //171114LML The following is VIC 4.1.2e forcing variables
    DENSITY,     /* atmospheric density [kg/m3] */
    QAIR,        /* specific humidity [kg/kg] */
    REL_HUMID,   /* relative humidity [fraction] */
    TSKC,        /* cloud cover fraction [fraction] */
    WIND_E,      /* zonal component of wind speed [m/s] */
    WIND_N,      /* meridional component of wind speed [m/s] */
    //171114LML new added
    RHUM_MAX,    /* relative humidity [%] */
    RHUM_MIN,    /* relative humidity [%] */
    LONGWAVE,    /* incoming longwave radiation [W/m2] */
    SHORTWAVE,   /* incoming shortwave [W/m2] */
#endif
    // Last value of enum - DO NOT ADD ANYTHING BELOW THIS LINE!!
    // used as a loop counter and must be >= the largest value in this enum
    N_FORCING_TYPES  /**< Number of forcing types */
};


/******************************************************************************
 * @brief   Output Variable Types
 *****************************************************************************/
enum
{
    // Water Balance Terms - state variables
    OUT_ASAT,             /**< Saturated Area Fraction */
    OUT_LAKE_AREA_FRAC,   /**< lake surface area as fraction of the grid cell area [fraction] */
    OUT_LAKE_DEPTH,       /**< lake depth (distance between surface and deepest point) [m] */
    OUT_LAKE_ICE,         /**< moisture stored as lake ice [mm over lake ice area] */
    OUT_LAKE_ICE_FRACT,   /**< fractional coverage of lake ice [fraction] */
    OUT_LAKE_ICE_HEIGHT,  /**< thickness of lake ice [cm] */
    OUT_LAKE_MOIST,       /**< liquid water and ice stored in lake [mm over grid cell] */
    OUT_LAKE_SURF_AREA,   /**< lake surface area [m2] */
    OUT_LAKE_SWE,         /**< liquid water equivalent of snow on top of lake ice [m over lake ice area] */
    OUT_LAKE_SWE_V,       /**< volumetric liquid water equivalent of snow on top of lake ice [m3] */
    OUT_LAKE_VOLUME,      /**< lake volume [m3] */
    OUT_ROOTMOIST,        /**< root zone soil moisture  [mm] */
    OUT_SMFROZFRAC,       /**< fraction of soil moisture (by mass) that is ice, for each soil layer */
    OUT_SMLIQFRAC,        /**< fraction of soil moisture (by mass) that is liquid, for each soil layer */
    OUT_SNOW_CANOPY,      /**< snow interception storage in canopy  [mm] */
    OUT_SNOW_COVER,       /**< fractional area of snow cover [fraction] */
    OUT_SNOW_DEPTH,       /**< depth of snow pack [cm] */
    OUT_SOIL_ICE,         /**< soil ice content  [mm] for each soil layer */
    OUT_SOIL_LIQ,         /**< soil liquid content  [mm] for each soil layer */
    OUT_SOIL_ICE_FRAC,    /**< soil ice content fraction of column volume [1] for each soil layer */
    OUT_SOIL_LIQ_FRAC,    /**< soil liquid content fraction of column volume [1] for each soil layer */
    OUT_SOIL_MOIST,       /**< soil total moisture content  [mm] for each soil layer */
    OUT_SOIL_WET,         /**< vertical average of (soil moisture - wilting point)/(maximum soil moisture - wilting point) [mm/mm] */
    OUT_SURFSTOR,         /**< storage of liquid water and ice (not snow) on surface (ponding) [mm] */
    OUT_SURF_FROST_FRAC,  /**< fraction of soil surface that is frozen [fraction] */
    OUT_SWE,              /**< snow water equivalent in snow pack (including vegetation-intercepted snow)  [mm] */
    OUT_WDEW,             /**< total moisture interception storage in canopy [mm] */
    OUT_ZWT,              /**< water table position [cm] (zwt within lowest unsaturated layer) */
    OUT_ZWT_LUMPED,       /**< lumped water table position [cm] (zwt of total moisture across all layers, lumped together) */
    // Water Balance Terms - fluxes
    OUT_BASEFLOW,         /**< baseflow out of the bottom layer  [mm] */
    OUT_DELINTERCEPT,     /**< change in canopy interception storage  [mm] */
    OUT_DELSOILMOIST,     /**< change in soil water content  [mm] */
    OUT_DELSURFSTOR,      /**< change in surface liquid water storage  [mm] */
    OUT_DELSWE,           /**< change in snow water equivalent  [mm] */
    OUT_EVAP,             /**< total net evaporation [mm] */
    OUT_EVAP_BARE,        /**< net evaporation from bare soil [mm] */
    OUT_EVAP_CANOP,       /**< net evaporation from canopy interception [mm] */
    OUT_INFLOW,           /**< moisture that reaches top of soil column [mm] */
    OUT_LAKE_BF_IN,       /**< incoming baseflow from lake catchment [mm] */
    OUT_LAKE_BF_IN_V,     /**< incoming volumetric baseflow from lake catchment [m3] */
    OUT_LAKE_BF_OUT,      /**< outgoing baseflow from lake [mm] */
    OUT_LAKE_BF_OUT_V,    /**< outgoing volumetric baseflow from lake [m3] */
    OUT_LAKE_CHAN_IN,     /**< channel inflow into lake [mm] */
    OUT_LAKE_CHAN_IN_V,   /**< volumetric channel inflow into lake [m3] */
    OUT_LAKE_CHAN_OUT,    /**< channel outflow from lake [mm] */
    OUT_LAKE_CHAN_OUT_V,  /**< volumetric channel outflow from lake [m3] */
    OUT_LAKE_DSTOR,       /**< change in lake moisture storage (liquid plus ice cover) [mm] */
    OUT_LAKE_DSTOR_V,     /**< volumetric change in lake moisture storage (liquid plus ice cover) [m3] */
    OUT_LAKE_DSWE,        /**< change in swe on top of lake ice [mm] */
    OUT_LAKE_DSWE_V,      /**< volumetric change in swe on top of lake ice [m3] */
    OUT_LAKE_EVAP,        /**< net evaporation from lake surface [mm] */
    OUT_LAKE_EVAP_V,      /**< net volumetric evaporation from lake surface [m3] */
    OUT_LAKE_PREC_V,      /**< volumetric precipitation over lake surface [m3] */
    OUT_LAKE_RCHRG,       /**< recharge from lake to surrounding wetland [mm] */
    OUT_LAKE_RCHRG_V,     /**< volumetric recharge from lake to surrounding wetland [m3] */
    OUT_LAKE_RO_IN,       /**< incoming runoff from lake catchment [mm] */
    OUT_LAKE_RO_IN_V,     /**< incoming volumetric runoff from lake catchment [m3] */
    OUT_LAKE_VAPFLX,      /**< outgoing sublimation from snow on top of lake ice [mm] */
    OUT_LAKE_VAPFLX_V,    /**< outgoing volumetric sublimation from snow on top of lake ice [m3] */
    OUT_PET,              /**< Potential evapotranspiration (= area-weighted sum of potential transpiration and potential soil evaporation).  Potential transpiration is computed using the Penman-Monteith eqn with architectural resistance and LAI of the current veg cover. [mm] */
    OUT_PREC,             /**< incoming precipitation [mm] */
    OUT_RAINF,            /**< rainfall  [mm] */
    OUT_REFREEZE,         /**< refreezing of water in the snow  [mm] */
    OUT_RUNOFF,           /**< surface runoff [mm] */
    OUT_SNOW_MELT,        /**< snow melt  [mm] */
    OUT_SNOWF,            /**< snowfall  [mm] */
    OUT_SUB_BLOWING,      /**< net sublimation of blowing snow [mm] */
    OUT_SUB_CANOP,        /**< net sublimation from snow stored in canopy [mm] */
    OUT_SUB_SNOW,         /**< total net sublimation from snow pack (surface and blowing) [mm] */
    OUT_SUB_SURFACE,      /**< net sublimation from snow pack surface [mm] */
    OUT_TRANSP_VEG,       /**< net transpiration from vegetation [mm] */
    OUT_WATER_ERROR,      /**< water budget error [mm] */
    // Energy Balance Terms - state variables
    OUT_ALBEDO,           /**< average surface albedo [fraction] */
    OUT_BARESOILT,        /**< bare soil surface temperature [C] */
    OUT_FDEPTH,           /**< depth of freezing fronts [cm] */
    OUT_LAKE_ICE_TEMP,    /**< temperature of lake ice [C] */
    OUT_LAKE_SURF_TEMP,   /**< lake surface temperature [C] */
    OUT_RAD_TEMP,         /**< average radiative surface temperature [K] */
    OUT_SALBEDO,          /**< snow pack albedo [fraction] */
    OUT_SNOW_PACK_TEMP,   /**< snow pack temperature [C] */
    OUT_SNOW_SURF_TEMP,   /**< snow surface temperature [C] */
    OUT_SNOWT_FBFLAG,     /**< snow surface temperature fallback flag */
    OUT_SOIL_TEMP,        /**< soil temperature [C] */
    OUT_SOIL_TNODE,       /**< soil temperature [C] */
    OUT_SOIL_TNODE_WL,    /**< soil temperature [C] */
    OUT_SOILT_FBFLAG,     /**< soil temperature flag for each soil thermal node */
    OUT_SURF_TEMP,        /**< average surface temperature [C] */
    OUT_SURFT_FBFLAG,     /**< surface temperature flag */
    OUT_TCAN_FBFLAG,      /**< Tcanopy flag */
    OUT_TDEPTH,           /**< depth of thawing fronts [cm] */
    OUT_TFOL_FBFLAG,      /**< Tfoliage flag */
    OUT_VEGT,             /**< average vegetation canopy temperature [C] */
    // Energy Balance Terms - fluxes
    OUT_ADV_SENS,         /**< net sensible flux advected to snow pack [W/m2] */
    OUT_ADVECTION,        /**< advected energy [W/m2] */
    OUT_DELTACC,          /**< rate of change in cold content in snow pack [W/m2] */
    OUT_DELTAH,           /**< rate of change in heat storage [W/m2] */
    OUT_ENERGY_ERROR,     /**< energy budget error [W/m2] */
    OUT_FUSION,           /**< net energy used to melt/freeze soil moisture [W/m2] */
    OUT_GRND_FLUX,        /**< net heat flux into ground [W/m2] */
    OUT_IN_LONG,          /**< incoming longwave at ground surface (under veg) [W/m2] */
    OUT_LATENT,           /**< net upward latent heat flux [W/m2] */
    OUT_LATENT_SUB,       /**< net upward latent heat flux from sublimation [W/m2] */
    OUT_MELT_ENERGY,      /**< energy of fusion (melting) in snowpack [W/m2] */
    OUT_LWNET,            /**< net downward longwave flux [W/m2] */
    OUT_SWNET,            /**< net downward shortwave flux [W/m2] */
    OUT_R_NET,            /**< net downward radiation flux [W/m2] */
    OUT_RFRZ_ENERGY,      /**< net energy used to refreeze liquid water in snowpack [W/m2] */
    OUT_SENSIBLE,         /**< net upward sensible heat flux [W/m2] */
    OUT_SNOW_FLUX,        /**< energy flux through snow pack [W/m2] */
    // Miscellaneous Terms
    OUT_AERO_COND,        /**< "scene" aerodynamic conductance [m/s] (tiles with overstory contribute overstory conductance; others contribute surface conductance) */
    OUT_AERO_COND1,       /**< surface aerodynamic conductance [m/s] */
    OUT_AERO_COND2,       /**< overstory aerodynamic conductance [m/s] */
    OUT_AERO_RESIST,      /**< "scene"canopy aerodynamic resistance [s/m]  (tiles with overstory contribute overstory resistance; others contribute surface resistance)*/
    OUT_AERO_RESIST1,     /**< surface aerodynamic resistance [s/m] */
    OUT_AERO_RESIST2,     /**< overstory aerodynamic resistance [s/m] */
    OUT_AIR_TEMP,         /**< air temperature [C] */
    OUT_CATM,             /**< atmospheric CO2 concentrtaion [ppm]*/
    OUT_DENSITY,          /**< near-surface atmospheric density [kg/m3]*/
    OUT_FCANOPY,          /**< fractional area covered by plant canopy [fraction] */
    OUT_FDIR,             /**< fraction of incoming shortwave that is direct [fraction]*/
    OUT_LAI,              /**< leaf area index [m2/m2] */
    OUT_LWDOWN,           /**< incoming longwave [W/m2] */
    OUT_PAR,              /**< incoming photosynthetically active radiation [W/m2] */
    OUT_PRESSURE,         /**< near surface atmospheric pressure [kPa] */
    OUT_QAIR,             /**< specific humidity [kg/kg] */
    OUT_REL_HUMID,        /**< relative humidity [%]*/
    OUT_SWDOWN,           /**< incoming shortwave [W/m2] */
    OUT_SURF_COND,        /**< surface conductance [m/s] */
    OUT_VP,               /**< near surface vapor pressure [kPa] */
    OUT_VPD,              /**< near surface vapor pressure deficit [kPa] */
    OUT_WIND,             /**< near surface wind speed [m/s] */
    // Band-specific quantities
    OUT_ADV_SENS_BAND,    /**< net sensible heat flux advected to snow pack [W/m2] */
    OUT_ADVECTION_BAND,   /**< advected energy [W/m2] */
    OUT_ALBEDO_BAND,      /**< average surface albedo [fraction] */
    OUT_DELTACC_BAND,     /**< change in cold content in snow pack [W/m2] */
    OUT_GRND_FLUX_BAND,   /**< net heat flux into ground [W/m2] */
    OUT_IN_LONG_BAND,     /**< incoming longwave at ground surface (under veg) [W/m2] */
    OUT_LATENT_BAND,      /**< net upward latent heat flux [W/m2] */
    OUT_LATENT_SUB_BAND,  /**< net upward latent heat flux due to sublimation [W/m2] */
    OUT_MELT_ENERGY_BAND, /**< energy of fusion (melting) in snowpack [W/m2] */
    OUT_LWNET_BAND,       /**< net downward longwave flux [W/m2] */
    OUT_SWNET_BAND,       /**< net downward shortwave flux [W/m2] */
    OUT_RFRZ_ENERGY_BAND, /**< net energy used to refreeze liquid water in snowpack [W/m2] */
    OUT_SENSIBLE_BAND,    /**< net upward sensible heat flux [W/m2] */
    OUT_SNOW_CANOPY_BAND, /**< snow interception storage in canopy [mm] */
    OUT_SNOW_COVER_BAND,  /**< fractional area of snow cover [fraction] */
    OUT_SNOW_DEPTH_BAND,  /**< depth of snow pack [cm] */
    OUT_SNOW_FLUX_BAND,   /**< energy flux through snow pack [W/m2] */
    OUT_SNOW_MELT_BAND,   /**< snow melt [mm] */
    OUT_SNOW_PACKT_BAND,  /**< snow pack temperature [C] */
    OUT_SNOW_SURFT_BAND,  /**< snow surface temperature [C] */
    OUT_SWE_BAND,         /**< snow water equivalent in snow pack [mm] */
    // Carbon-Cycling Terms
    OUT_APAR,             /**< absorbed PAR [W/m2] */
    OUT_GPP,              /**< gross primary productivity [g C/m2d] */
    OUT_RAUT,             /**< autotrophic respiration [g C/m2d] */
    OUT_NPP,              /**< net primary productivity [g C/m2d] */
    OUT_LITTERFALL,       /**< flux of carbon from living biomass into soil [g C/m2d] */
    OUT_RHET,             /**< soil respiration (heterotrophic respiration) [g C/m2d] */
    OUT_NEE,              /**< net ecosystem exchange (=NPP-RHET) [g C/m2d] */
    OUT_CLITTER,          /**< Carbon density in litter pool [g C/m2] */
    OUT_CINTER,           /**< Carbon density in intermediate pool [g C/m2] */
    OUT_CSLOW,            /**< Carbon density in slow pool [g C/m2] */
    // Timing and Profiling Terms
    OUT_TIME_VICRUN_WALL, /**< Wall time spent inside vic_run [seconds] */
    OUT_TIME_VICRUN_CPU,  /**< Wall time spent inside vic_run [seconds] */
    // Last value of enum - DO NOT ADD ANYTHING BELOW THIS LINE!!
    // used as a loop counter and must be >= the largest value in this enum
    N_OUTVAR_TYPES        /**< used as a loop counter*/
};

/******************************************************************************
 * @brief   Output state variable.
 *****************************************************************************/
enum
{
    STATE_SOIL_MOISTURE,               /**<  total soil moisture */
    STATE_SOIL_ICE,                    /**<  ice content */
    STATE_CANOPY_WATER,                /**<  dew storage: tmpval = veg_var[veg][band].Wdew; */
    STATE_ANNUALNPP,                   /**<  cumulative NPP: tmpval = veg_var[veg][band].AnnualNPP; */
    STATE_ANNUALNPPPREV,               /**<  previous NPP: tmpval = veg_var[veg][band].AnnualNPPPrev; */
    STATE_CLITTER,                     /**<  litter carbon: tmpval = cell[veg][band].CLitter; */
    STATE_CINTER,                      /**<  intermediate carbon: tmpval = cell[veg][band].CInter; */
    STATE_CSLOW,                       /**<  slow carbon: tmpval = cell[veg][band].CSlow; */
    STATE_SNOW_AGE,                    /**<  snow age: snow[veg][band].last_snow */
    STATE_SNOW_MELT_STATE,             /**<  melting state: (int)snow[veg][band].MELTING */
    STATE_SNOW_COVERAGE,               /**<  snow covered fraction: snow[veg][band].coverage */
    STATE_SNOW_WATER_EQUIVALENT,       /**<  snow water equivalent: snow[veg][band].swq */
    STATE_SNOW_SURF_TEMP,              /**<  snow surface temperature: snow[veg][band].surf_temp */
    STATE_SNOW_SURF_WATER,             /**<  snow surface water: snow[veg][band].surf_water */
    STATE_SNOW_PACK_TEMP,              /**<  snow pack temperature: snow[veg][band].pack_temp */
    STATE_SNOW_PACK_WATER,             /**<  snow pack water: snow[veg][band].pack_water */
    STATE_SNOW_DENSITY,                /**<  snow density: snow[veg][band].density */
    STATE_SNOW_COLD_CONTENT,           /**<  snow cold content: snow[veg][band].coldcontent */
    STATE_SNOW_CANOPY,                 /**<  snow canopy storage: snow[veg][band].snow_canopy */
    STATE_SOIL_NODE_TEMP,              /**<  soil node temperatures: energy[veg][band].T[nidx] */
    STATE_FOLIAGE_TEMPERATURE,         /**<  Foliage temperature: energy[veg][band].Tfoliage */
    STATE_ENERGY_LONGUNDEROUT,         /**<  Outgoing longwave from understory: energy[veg][band].LongUnderOut */
    STATE_ENERGY_SNOW_FLUX,            /**<  Thermal flux through the snow pack: energy[veg][band].snow_flux */
    STATE_LAKE_SOIL_MOISTURE,          /**<  total soil moisture */
    STATE_LAKE_SOIL_ICE,               /**<  ice content */
    STATE_LAKE_CLITTER,                /**<  litter carbon: tmpval = lake_var.soil.CLitter; */
    STATE_LAKE_CINTER,                 /**<  intermediate carbon: tmpval = lake_var.soil.CInter; */
    STATE_LAKE_CSLOW,                  /**<  slow carbon: tmpval = lake_var.soil.CSlow; */
    STATE_LAKE_SNOW_AGE,               /**<  snow age: lake_var.snow.last_snow */
    STATE_LAKE_SNOW_MELT_STATE,        /**<  melting state: (int)lake_var.snow.MELTING */
    STATE_LAKE_SNOW_COVERAGE,          /**<  snow covered fraction: lake_var.snow.coverage */
    STATE_LAKE_SNOW_WATER_EQUIVALENT,  /**<  snow water equivalent: lake_var.snow.swq */
    STATE_LAKE_SNOW_SURF_TEMP,         /**<  snow surface temperature: lake_var.snow.surf_temp */
    STATE_LAKE_SNOW_SURF_WATER,        /**<  snow surface water: lake_var.snow.surf_water */
    STATE_LAKE_SNOW_PACK_TEMP,         /**<  snow pack temperature: lake_var.snow.pack_temp */
    STATE_LAKE_SNOW_PACK_WATER,        /**<  snow pack water: lake_var.snow.pack_water */
    STATE_LAKE_SNOW_DENSITY,           /**<  snow density: lake_var.snow.density */
    STATE_LAKE_SNOW_COLD_CONTENT,      /**<  snow cold content: lake_var.snow.coldcontent */
    STATE_LAKE_SNOW_CANOPY,            /**<  snow canopy storage: lake_var.snow.snow_canopy */
    STATE_LAKE_SOIL_NODE_TEMP,         /**<  soil node temperatures: lake_var.energy.T[nidx] */
    STATE_LAKE_ACTIVE_LAYERS,          /**<  lake active layers: lake_var.activenod */
    STATE_LAKE_LAYER_DZ,               /**<  lake layer thickness: lake_var.dz */
    STATE_LAKE_SURF_LAYER_DZ,          /**<  lake surface layer thickness: lake_var.surfdz */
    STATE_LAKE_DEPTH,                  /**<  lake depth: lake_var.ldepth */
    STATE_LAKE_LAYER_SURF_AREA,        /**<  lake layer surface areas: lake_var.surface[ndix] */
    STATE_LAKE_SURF_AREA,              /**<  lake surface area: lake_var.sarea */
    STATE_LAKE_VOLUME,                 /**<  lake volume: lake_var.volume */
    STATE_LAKE_LAYER_TEMP,             /**<  lake layer temperatures: lake_var.temp[nidx] */
    STATE_LAKE_AVERAGE_TEMP,           /**<  vertical average lake temperature: lake_var.tempavg */
    STATE_LAKE_ICE_AREA_FRAC,          /**<  lake ice area fraction: lake_var.areai */
    STATE_LAKE_ICE_AREA_FRAC_NEW,      /**<  new lake ice area fraction: lake_var.new_ice_area */
    STATE_LAKE_ICE_WATER_EQUIVALENT,   /**<  lake ice water equivalent: lake_var.ice_water_eq */
    STATE_LAKE_ICE_HEIGHT,             /**<  lake ice height: lake_var.hice */
    STATE_LAKE_ICE_TEMP,               /**<  lake ice temperature: lake_var.tempi */
    STATE_LAKE_ICE_SWE,                /**<  lake ice snow water equivalent: lake_var.swe */
    STATE_LAKE_ICE_SNOW_SURF_TEMP,     /**<  lake ice snow surface temperature: lake_var.surf_temp */
    STATE_LAKE_ICE_SNOW_PACK_TEMP,     /**<  lake ice snow pack temperature: lake_var.pack_temp */
    STATE_LAKE_ICE_SNOW_COLD_CONTENT,  /**<  lake ice snow coldcontent: lake_var.coldcontent */
    STATE_LAKE_ICE_SNOW_SURF_WATER,    /**<  lake ice snow surface water: lake_var.surf_water */
    STATE_LAKE_ICE_SNOW_PACK_WATER,    /**<  lake ice snow pack water: lake_var.pack_water */
    STATE_LAKE_ICE_SNOW_ALBEDO,        /**<  lake ice snow albedo: lake_var.SAlbedo */
    STATE_LAKE_ICE_SNOW_DEPTH,         /**<  lake ice snow depth: lake_var.sdepth */
    // Last value of enum - DO NOT ADD ANYTHING BELOW THIS LINE!!
    // used as a loop counter and must be >= the largest value in this enum
    N_STATE_VARS                       /**< used as a loop counter*/
};


/******************************************************************************
 * @brief   Output BINARY format types
 *****************************************************************************/
enum
{
    OUT_TYPE_DEFAULT,  /**< Default data type */
    OUT_TYPE_CHAR,     /**< char */
    OUT_TYPE_SINT,     /**< short int */
    OUT_TYPE_USINT,    /**< unsigned short int */
    OUT_TYPE_INT,      /**< int */
    OUT_TYPE_FLOAT,    /**< single-precision floating point */
    OUT_TYPE_DOUBLE    /**< double-precision floating point */
};

/******************************************************************************
 * @brief   Output aggregation method types
 *****************************************************************************/
enum
{
    AGG_TYPE_DEFAULT, /**< Default aggregation type */
    AGG_TYPE_AVG,     /**< average over agg interval */
    AGG_TYPE_BEG,     /**< value at beginning of agg interval */
    AGG_TYPE_END,     /**< value at end of agg interval */
    AGG_TYPE_MAX,     /**< maximum value over agg interval */
    AGG_TYPE_MIN,     /**< minimum value over agg interval */
    AGG_TYPE_SUM      /**< sum over agg interval */
};

/******************************************************************************
 * @brief   Frequency flags for raising alarms/flags
 *****************************************************************************/
enum
{
    FREQ_NEVER,      /**< Flag for never raising alarm */
    FREQ_NSTEPS,     /**< Flag for raising alarm every nsteps */
    FREQ_NSECONDS,   /**< Flag for raising alarm every nseconds */
    FREQ_NMINUTES,   /**< Flag for raising alarm every nminutes */
    FREQ_NHOURS,     /**< Flag for raising alarm every nhours */
    FREQ_NDAYS,      /**< Flag for raising alarm every ndays */
    FREQ_NMONTHS,    /**< Flag for raising alarm every nmonths */
    FREQ_NYEARS,     /**< Flag for raising alarm every nyears */
    FREQ_DATE,       /**< Flag for raising alarm on a specific date */
    FREQ_END         /**< Flag for raising alarm at the end of a simulation */
};

/******************************************************************************
 * @brief   Codes for displaying version information
 *****************************************************************************/
enum
{
    DISP_VERSION,
    DISP_COMPILE_TIME,
    DISP_ALL
};

/******************************************************************************
 * @brief   Codes for calendar option.
 *****************************************************************************/
enum calendars
{
    CALENDAR_STANDARD,
    CALENDAR_GREGORIAN,
    CALENDAR_PROLEPTIC_GREGORIAN,
    CALENDAR_NOLEAP,
    CALENDAR_365_DAY,
    CALENDAR_360_DAY,
    CALENDAR_JULIAN,
    CALENDAR_ALL_LEAP,
    CALENDAR_366_DAY
};

/******************************************************************************
 * @brief   Codes for time units option.
 *****************************************************************************/
enum time_units
{
    TIME_UNITS_SECONDS,
    TIME_UNITS_MINUTES,
    TIME_UNITS_HOURS,
    TIME_UNITS_DAYS
};

/******************************************************************************
 * @brief   Codes for timers
 *****************************************************************************/
enum timers
{
    TIMER_VIC_ALL,
    TIMER_VIC_INIT,
    TIMER_VIC_RUN,
    TIMER_VIC_FINAL,
    N_TIMERS
};

void initialize_parameters(void);
#endif
