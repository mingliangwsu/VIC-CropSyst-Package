/******************************************************************************
 * @section DESCRIPTION
 *
 * Definition header file
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

#ifndef VIC_DEF_H
#define VIC_DEF_H
//170926LML #ifndef _BSD_SOURCE
//170926LML #define _BSD_SOURCE
//170926LML #endif
//170926LML #ifndef __USE_XOPEN
//170926LML #define __USE_XOPEN
//170926LML #endif
//170926LML #ifndef _GNU_SOURCE
//170926LML #define _GNU_SOURCE
//170926LML #endif

#include <float.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <time.h>
#include <sys/types.h>
#ifndef __BORLANDC__
#include <strings.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/time.h>
#endif
#ifdef __linux__
#include <pwd.h>
#endif


#include <VIC_CropSyst_common/VCS_Nl_def.h>
#include <vic_physical_constants.h>
//#include <vic_log.h>

/***** Model Constants *****/
//170926LML #define MAXSTRING    2048
#define MISSING      -99999.   /**< missing value */
#define MISSING_USI  99999.    /**< missing value for unsigned ints */
#define MISSING_S    "MISSING"    /**< missing value for strings */
#define NODATA_VH    -1        /**< missing value for veg_hist inputs */
#define NODATA_VEG   -1        /**< flag for veg types not in grid cell */
#define ERROR        -999      /**< Error Flag returned by subroutines */

/***** Define maximum array sizes for model source code *****/
//170926LML #define MAX_VEG         12     /**< maximum number of vegetation types per cell */
#ifdef VIC_CROPSYST_VERSION
#define MAX_LAYERS      17
#else
#define MAX_LAYERS      3      /**< maximum number of soil moisture layers */
#endif
#define MAX_NODES       50     /**< maximum number of soil thermal nodes */
#define MAX_BANDS       10     /**< maximum number of snow bands */
#define MAX_FRONTS      3      /**< maximum number of freezing and thawing front depths to store */
#define MAX_FROST_AREAS 10     /**< maximum number of frost sub-areas */
#define MAX_LAKE_NODES  20     /**< maximum number of lake thermal nodes */
#define MAX_ZWTVMOIST   11     /**< maximum number of points in water table vs moisture curve for each soil layer; should include points at lower and upper boundaries of the layer */

/***** Define minimum values for model parameters *****/
#define MINSOILDEPTH    0.001  /**< Minimum layer depth with which model can work (m) */
#define MIN_FCANOPY    0.0001  /**< Minimum allowable canopy fraction */
#define MIN_SNOW_WETFRAC 0.01  /**< Minimum fraction of snow depth to be considered wet */

/***** Define minimum and maximum values for model timesteps *****/
#define MIN_SUBDAILY_STEPS_PER_DAY  4
#define MAX_SUBDAILY_STEPS_PER_DAY  1440

#ifndef WET
#define WET 0
#define DRY 1
#endif

#ifndef SNOW
#define RAIN 0
#define SNOW 1
#endif

//170406LML conflict with stdard C library #define min(a, b) (a < b) ? a : b
//170406LML conflict with stdard C library #define max(a, b) (a > b) ? a : b

extern size_t NR;       /**< array index for force struct that indicates
                             the model step avarage or sum */
extern size_t NF;       /**< array index loop counter limit for force
                             struct that indicates the SNOW_STEP values */
extern char          vic_run_ref_str[MAXSTRING];

/******************************************************************************
 * @brief   Snow Density parametrizations
 *****************************************************************************/
enum
{
    DENS_BRAS,
    DENS_SNTHRM
};

/******************************************************************************
 * @brief   Baseflow parametrizations
 *****************************************************************************/
enum
{
    ARNO,
    NIJSSEN2001
};

/******************************************************************************
 * @brief   Aerodynamic Resistance options
 *****************************************************************************/
enum
{
    AR_406,
    AR_406_LS,
    AR_406_FULL,
    AR_410
};

/******************************************************************************
 * @brief   Ground Flux options
 *****************************************************************************/
enum
{
    GF_406,
    GF_410
};

/******************************************************************************
 * @brief   Canopy resistance parametrizations
 *****************************************************************************/
enum
{
    RC_JARVIS,
    RC_PHOTO
};

/******************************************************************************
 * @brief   Photosynthesis parametrizations
 *****************************************************************************/
enum
{
    PS_FARQUHAR,
    PS_MONTEITH
};

/******************************************************************************
 * @brief   Photosynthetic pathways
 *****************************************************************************/
enum
{
    PHOTO_C3,
    PHOTO_C4
};

/***** Data Structures *****/

/******************************************************************************
 * @brief    This structure holds the model parameters.
 *****************************************************************************/
typedef struct {
    // Lapse Rate
    double LAPSE_RATE;  /**< temperature lapse rate (C/m) */

    // Precipitation Guage Height
    double GAUGE_HEIGHT;   /**< precipitation gauge height (m) */

    // Huge Resistance Term
    double HUGE_RESIST;  /**< Extermely large resistance term (s/m) */

    // Surface Albedo Parameters
    double ALBEDO_BARE_SOIL;  /**< Broadband albedo of bare soil */

    // Surface Emissivities
    double EMISS_GRND;  /**< Emissivity of bare soil */
    double EMISS_VEG;  /**< Emissivity of vegetation */
    double EMISS_ICE;  /**< Emissivity of bare ice */
    double EMISS_SNOW;  /**< Emissivity of snow */
    double EMISS_H2O;  /**< Emissivity of open water surface */

    // Soil Constraints
    double SOIL_RARC;  /**< Architectural resistance (s/m) of soil when computing soil evaporation via Penman-Monteith eqn */
    double SOIL_RESID_MOIST;  /**< Default residual moisture content of soil colum */
    double SOIL_SLAB_MOIST_FRACT;  /**< Volumetric moisture content (fraction of porosity) in the soil/rock below the bottom soil layer; this assumes that the soil below the bottom layer has the same texture as the bottom layer. */
    double SOIL_WINDH;  /**< Default wind measurement height over soil (m) */

    // Vegetation Parameters
    double VEG_LAI_SNOW_MULTIPLIER;  /**< multiplier to calculate the amount of available snow interception as a function of LAI (m) */
    double VEG_LAI_WATER_FACTOR;  /**< Coefficient multiplied by the LAI to determine the amount of water that can be stored in the canopy */
    double VEG_MIN_INTERCEPTION_STORAGE;  /**< the amount of snow on the canopy that can only be melted off. (m) */
    double VEG_RATIO_DH_HEIGHT;  /**< Ratio of displacement height (m) to vegetation height (m) */
    double VEG_RATIO_RL_HEIGHT;  /**< Ratio of roughness length (m) to vegetation height (m) */

    // Canopy Parameters
    double CANOPY_CLOSURE;  /**< Threshold vapor pressure deficit for stomatal closure (Pa) */
    double CANOPY_RSMAX;  /**< Maximum allowable resistance (s/m) */
    double CANOPY_VPDMINFACTOR;  /**< Minimum allowable vapor pressure deficit factor */

    // Lake Parameters
    double LAKE_TMELT;
    double LAKE_MAX_SURFACE;  /**< max. surface layer thickness for E-B (m) */
    double LAKE_BETA;  /**< Curve shape parameter for lake profile. */
    double LAKE_FRACMIN;  /**< min ice thickness in meters */
    double LAKE_FRACLIM;  /**< lower limit on fractional ice cover */
    double LAKE_DM;  /**< molecular diffusivity of water */
    double LAKE_SNOWCRIT;  /**< for albedo (m) */
    double LAKE_ZWATER;
    double LAKE_ZSNOW;
    double LAKE_RHOSNOW;  /**< density of snow (kg m-3) */
    double LAKE_CONDI;  /**< thermal conductivity of ice */
    double LAKE_CONDS;  /**< thermal conductivity of snow */
    double LAKE_LAMISW;  /**< attenuation of shortwave radiation through ice (1/m) */
    double LAKE_LAMILW;  /**< attenuation of longwave radiation through ice (1/m) */
    double LAKE_LAMSSW;  /**< attenuation of shortwave radiation through snow (1/m) */
    double LAKE_LAMSLW;  /**< attenuation of longwave radiation through snow (1/m) */
    double LAKE_LAMWSW;  /**< attenuation of shortwave radiation through water (1/m) */
    double LAKE_LAMWLW;  /**< attenuation of longwave radiation through water (1/m) */
    double LAKE_A1;  /**< Percent of radiation in visible band. */
    double LAKE_A2;  /**< Percent of radiation in infrared band. */
    double LAKE_QWTAU;  /**< D. Pollard sub-ice time constant. */
    int LAKE_MAX_ITER;

    // Saturation Vapor Pressure Parameters
    double SVP_A;  /**< constant for saturated vapor pressure curve (kPa) */
    double SVP_B;  /**< constant for saturated vapor pressure curve (kPa) */
    double SVP_C;  /**< constant for saturated vapor pressure curve (kPa) */

    // Photosynthesis Parameters
    double PHOTO_OMEGA;  /**< single leaf scattering albedo */
    double PHOTO_LAIMAX;  /**< Maximum LAI in nitrogen scaling */
    double PHOTO_LAILIMIT;  /**< Minimum LAI in nitrogen scaling and maximum LAI in PAR computation */
    double PHOTO_LAIMIN;  /**< Minimum LAI in PAR computation */
    double PHOTO_EPAR;  /**< Energy content of PAR [J/mol photons] = (4.6 mol/MJ PAR)^-1 */
    double PHOTO_FCMAX;  /**< Maximum fractional veg cover; (1-FcMax) = min amount of ground visible */
    double PHOTO_FCMIN;  /**< Minimum fractional veg cover; (1-FcMin) = max amount of ground visible */
    double PHOTO_ZENITHMIN;  /**< Check for solar zenith angle > 89 deg */
    double PHOTO_ZENITHMINPAR;  /**< Cosine of the minimum solar zenith angle for photosynthesis to take place */
    double PHOTO_ALBSOIPARMIN;  /**< Minimum soil reflectivity in PAR range */
    double PHOTO_MINMAXETRANS;  /**< Minimum of maximum electron transport rate [10e-12 mol/(m^2 s)] */
    double PHOTO_MINSTOMCOND;  /**< Minimum stomatal conductance [mol H2O/m2s] */
    double PHOTO_FCI1C3;  /**< C3 Plants factor that relate leaf internal CO2 concentration to ambient CO2 concentration */
    double PHOTO_FCI1C4;  /**< C4 Plants factor that relate leaf internal CO2 concentration to ambient CO2 concentration */
    double PHOTO_OX;  /**< OXYGEN CONCENTRATION [MOL(O2) / MOL(AIR)] */
    double PHOTO_KC;  /**< MICHAELIS-MENTEN CONSTANT FOR CO2 AT 25C [MOL(CO2) / MOL(AIR)] */
    double PHOTO_KO;  /**< MICHAELIS-MENTEN CONSTANT FOR O2 AT 25C [MOL(O2) / MOL(AIR)] */
    double PHOTO_EC;  /**< ACTIVATION ENERGY FOR KC [J / MOL] */
    double PHOTO_EO;  /**< ACTIVATION ENERGY FOR KO [J / MOL] */
    double PHOTO_EV;  /**< ACTIVATION ENERGY FOR VCMAX [J / MOL] */
    double PHOTO_ER;  /**< ACTIVATION ENERGY FOR DARK RESPIRATION [J / MOL] */
    double PHOTO_ALC3;  /**< EFFICIENCY OF OF PHOTON CAPTURE */
    double PHOTO_FRDC3;  /**< RATIO OF DARK RESPIRATION TO "PVM" AT 25C for C3 */
    double PHOTO_EK;  /**< = Q10=2 (Collatz et al. 1992) */
    double PHOTO_ALC4;  /**< EFFECTIVE QUANTUM EFFICIENCY */
    double PHOTO_FRDC4;  /**< RATIO OF DARK RESPIRATION TO "PVM" AT 25C for C4 */
    double PHOTO_THETA;  /**< CURVATURE PARAMETER */
    double PHOTO_FRLEAF;  /**< Ratio of canopy leaf respiration to whole plant maintenance respiration */
    double PHOTO_FRGROWTH;  /**< Ratio of plant growth respiration to NPP */

    // Soil Respiration Parameters
    double SRESP_E0_LT;  /**< Lloyd-Taylor E0 parameter [K] */
    double SRESP_T0_LT;  /**< Lloyd-Taylor T0 parameter [K] */
    double SRESP_WMINFM;  /**< minimum soil moisture (fraction) at which soil respiration can occur */
    double SRESP_WMAXFM;  /**< maximum soil moisture (fraction) at which soil respiration can occur */
    double SRESP_WOPTFM;  /**< soil moisture (fraction) at which maximum soil respiration occurs */
    double SRESP_RHSAT;  /**< ratio of soil respiration rate under saturated conditions (w=wmaxFM) to that under optimal conditions (w=woptFM) */
    double SRESP_RFACTOR;  /**< scaling factor to account for other (non-moisture) sources of inhibition of respiration */
    double SRESP_TAULITTER;  /**< Litter pool turnover time [y] */
    double SRESP_TAUINTER;  /**< Intermediate pool turnover time [y] */
    double SRESP_TAUSLOW;  /**< Slow pool turnover time [y] */
    double SRESP_FAIR;  /**< Fraction of respired carbon from litter pool that is lost to atmosphere */
    double SRESP_FINTER;  /**< Fraction of [respired carbon from litter pool that goes to soil] that goes to intermediate pool */

    // Snow Parameters
    double SNOW_MAX_SURFACE_SWE;  /**< maximum depth of the surface layer in water equivalent (m) */
    double SNOW_LIQUID_WATER_CAPACITY;  /**< water holding capacity of snow as a fraction of snow-water-equivalent */
    double SNOW_NEW_SNOW_DENSITY;  /**< density of new fallen snow */
    double SNOW_DENS_DMLIMIT;  /**< Density limit used in calculation of destructive metamorphism (kg/m^3) */
    double SNOW_DENS_DMLIMIT_FACTOR;  /**< Density limit factor used in calculation of destructive metamorphism (kg/m^3) */
    double SNOW_DENS_MAX_CHANGE;  /**< maximum change in snowfall depth (fraction of swe) */
    double SNOW_DENS_ETA0;  /**< viscosity of snow at T = 0C and density = 0 used in calculation of true viscosity (Ns/m2) */
    double SNOW_DENS_C1;  /**< Constant in snow density computation */
    double SNOW_DENS_C2;  /**< Constant in snow density computation */
    double SNOW_DENS_C3;  /**< Constant in snow density computation */
    double SNOW_DENS_C3_CONST;  /**< Constant in snow density computation */
    double SNOW_DENS_C4;  /**< Constant in snow density computation */
    double SNOW_DENS_C4WET;  /**< Constant in snow density computation */
    double SNOW_DENS_C5;  /**< constant used in snow viscosity calculation, taken from SNTHRM.89 (/C) */
    double SNOW_DENS_C6;  /**< constant used in snow viscosity calculation, taken from SNTHRM.89 (kg/m3) */
    double SNOW_DENS_F;  /**< internal compaction rate coefficient */
    double SNOW_DENS_EXP;  /**< exponent in snow density compaction equation [Bras pg. 257 ]*/
    double SNOW_DENS_DENOM;  /**< denomenator in snow density compaction equation [Bras pg. 257] */
    double SNOW_NEW_SNT_C1; /**< Constant in Sntherm new snow density computation. */
    double SNOW_NEW_SNT_C2; /**< Constant in Sntherm new snow density computation. */
    double SNOW_NEW_SNT_C3; /**< Constant in Sntherm new snow density computation. */
    double SNOW_NEW_BRAS_DENOM;  /**< Constant in Bras new snow density computation. */
    double SNOW_MIN_SWQ_EB_THRES;  /**< Minimum SWQ for which the snowpack energy balance is computed independent of the soil surface temperature */
    double SNOW_A1;  /**< Attenuation coefficient for shortwave in a snowpack. Value and equation taken from Patterson and Hamblin, 1988 */
    double SNOW_A2;  /**< Attenuation coefficient for shortwave in a snowpack. Value and equation taken from Patterson and Hamblin, 1988 */
    double SNOW_L1;  /**< Attenuation coefficient for shortwave in a snowpack. Value and equation taken from Patterson and Hamblin, 1988 (1/m) */
    double SNOW_L2;  /**< Attenuation coefficient for shortwave in a snowpack. Value and equation taken from Patterson and Hamblin, 1988 (1/m) */
    double SNOW_NEW_SNOW_ALB;  /**< Snow albedo curve parameters. */
    double SNOW_ALB_ACCUM_A;  /**< Snow albedo curve parameters. */
    double SNOW_ALB_ACCUM_B;  /**< Snow albedo curve parameters. */
    double SNOW_ALB_THAW_A;  /**< Snow albedo curve parameters. */
    double SNOW_ALB_THAW_B;  /**< Snow albedo curve parameters. */
    double SNOW_TRACESNOW;  /**< Defines the minimum amount of new snow (mm) which will reset the snowpack albedo to new snow */
    double SNOW_CONDUCT;  /**< conductivity of snow (W/mK) */
    double SNOW_MAX_SNOW_TEMP;  /**< maximum temperature (C) at which snow can fall */
    double SNOW_MIN_RAIN_TEMP;  /**< minimum temperature (C) at which rain can fall */

    // Blowing Snow Parameters
    double BLOWING_KA;  /**< thermal conductivity of air (W/mK) */
    double BLOWING_CSALT;  /**< saltation constant m/s */
    double BLOWING_UTHRESH;  /**< threshold shear velocity m/s */
    double BLOWING_KIN_VIS;  /**< Kinemativ viscosity of air (m2/s) */
    int BLOWING_MAX_ITER;     /**< Max. iterations for numerical integration */
    int BLOWING_K;
    double BLOWING_SETTLING;  /**< Particle settling velocity m/s */
    int BLOWING_NUMINCS;     /**< Number of prob intervals to solve for wind. */

    // Treeline temperature
    double TREELINE_TEMPERATURE;  /**< Number of prob intervals to solve for wind. */

    // Iteration Bracket Widths
    double SNOW_DT;  /**< Used to bracket snow surface temperatures while computing the snow surface energy balance (C) */
    double SURF_DT;  /**< Used to bracket soil surface temperatures while computing energy balance (C) */
    double SOIL_DT;  /**< Used to bracket soil temperatures while solving the soil thermal flux (C) */
    double CANOPY_DT;  /**< Used to bracket canopy air temperatures while computing energy balance (C) */
    double CANOPY_VP;  /**< Used to bracket canopy vapor pressures while computing moisture balance (Pa) */

    // Convergence Tolerances
    double TOL_GRND;
    double TOL_OVER;

    // Frozen Soil Parameters
    int FROZEN_MAXITER;

    // Newton-Raphson Solver Parameters
    int NEWT_RAPH_MAXTRIAL;
    double NEWT_RAPH_TOLX;
    double NEWT_RAPH_TOLF;
    double NEWT_RAPH_R_MAX;
    double NEWT_RAPH_R_MIN;
    double NEWT_RAPH_RELAX1;
    double NEWT_RAPH_RELAX2;
    double NEWT_RAPH_RELAX3;
    double NEWT_RAPH_EPS2;

    // Root-Brent parameters
    int ROOT_BRENT_MAXTRIES;
    int ROOT_BRENT_MAXITER;
    double ROOT_BRENT_TSTEP;
    double ROOT_BRENT_T;
} parameters_struct;

#endif
