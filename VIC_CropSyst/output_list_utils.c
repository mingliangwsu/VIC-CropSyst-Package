#include <stdio.h>
#include <stdlib.h>
#include <vicNl.h>
#include <string.h>

static char vcid[] = "$Id: output_list_utils.c,v 5.4.2.22 2012/02/08 00:56:33 vicadmin Exp $";
//150930LML
#ifdef VIC_CROPSYST_VERSION
#include "VCS_Nl.h"                                                              //170413LML
#define MAX_CROPROTATIONS_IN_ONE_GRIDCELL 50
//170413LML int set_output_for_crop_numbers(out_data_struct *out_data,const int numcrops);   //150930LML
#endif
out_data_struct *create_output_list() {
/*************************************************************
  create_output_list()      Ted Bohn     September 08, 2006

  This routine creates the list of output variables.

  Modifications:
  2006-Sep-14 Implemented ALMA-compliant input and output;
              now more variables are tracked.				TJB
  2006-Sep-18 Implemented aggregation of output variables.		TJB
  2006-Oct-10 Shortened the names of variables whose names were
	      too long; fixed typos in other names; added
	      OUT_IN_LONG.						TJB
  2006-Nov-07 Changed default precision from %.1f to %.4f.		TJB
  2006-Nov-07 Added OUT_SOIL_TNODE.					TJB
  2006-Nov-30 Added OUT_DELSURFSTOR.					TJB
  2007-Feb-28 Corrected AGG_TYPE definitions for miscellaneous
	      output variables; re-organized the code to make
	      it easier to debug.					TJB
  2007-Aug-17 Added EXCESS_ICE variables to output list.        	JCA
  2007-Aug-22 Added OUTPUT_WATER_ERROR as output variable.      	JCA
  2008-Sep-09 Added SOIL_TNODE_WL as an output variable, the
	      soil temperature in the wetland fraction of the
	      grid cell.						LCB via TJB
  2009-Jan-16 Added AERO_COND1&2 and AERO_RESIST1&2 to track
	      surface and overstory values; changed AERO_COND
	      and AERO_RESIST to track "scene" values.			TJB
  2009-Feb-22 Added OUT_VPD.						TJB
  2009-May-17 Added OUT_ASAT.						TJB
  2009-Jun-09 Added OUT_PET_*, potential evap computed for
	      various landcover types.					TJB
  2009-Jun-19 Added T flag to indicate whether TFALLBACK occurred.	TJB
  2009-Jul-07 Fixed nelem assignments for some band-specific vars.	TJB
  2009-Sep-19 Changed "*_FLAG" to "*_FBFLAG".				TJB
  2009-Oct-08 Extended T fallback scheme to snow and ice T.		TJB
  2010-Feb-14 Added OUT_LAKE_AREA_FRAC.					TJB
  2010-Mar-31 Added OUT_RUNOFF_IN.					TJB
  2010-Sep-24 Renamed RUNOFF_IN and OUT_RUNOFF_IN to CHANNEL_IN and
	      OUT_LAKE_CHAN_IN, respectively.  Renamed OUT_EVAP_LAKE
	      to OUT_LAKE_EVAP.  Added other lake water balance terms
	      to set of output variables.  Added volumetric versions
	      of these too.						TJB
  2010-Nov-02 Added OUT_LAKE_RO_IN and OUT_LAKE_RO_IN_V for reporting
	      overland runoff input to lake.  Added OUT_LAKE_RCHRG and
	      OUT_LAKE_RCHRG_V for reporting lake recharge of
	      surrounding wetland.  Added OUT_LAKE_VAPFLX and
	      OUT_LAKE_VAPFLX_V.					TJB
  2010-Nov-21 Added OUT_LAKE_DSTOR, OUT_LAKE_DSTOR_V, OUT_LAKE_DSWE,
	      OUT_LAKE_DSWE_V, OUT_LAKE_SWE, and OUT_LAKE_SWE_V.	TJB
  2010-Dec-01 Added OUT_ZWT.						TJB
  2011-Mar-01 Added OUT_ZWT2, OUT_ZWT3, and OUT_ZWTL.			TJB
  2011-Nov-04 Added OUT_TSKC.						TJB
  2012-Feb-07 Removed OUT_ZWT2 and OUT_ZWTL; renamed OUT_ZWT3 to
	      OUT_ZWT_LUMPED.						TJB
  2012-Nov-13 added cropsyst related output variables          KM
*************************************************************/

  extern option_struct options;
  int v;
  out_data_struct *out_data;

  out_data = (out_data_struct *)calloc(N_OUTVAR_TYPES,sizeof(out_data_struct));

  // Build the list of supported output variables

  // Water Balance Terms - state variables
  strcpy(out_data[OUT_ASAT].varname,"OUT_ASAT");                       /* saturated area fraction */
  strcpy(out_data[OUT_LAKE_AREA_FRAC].varname,"OUT_LAKE_AREA_FRAC");   /* lake surface area as fraction of grid cell area [fraction] */
  strcpy(out_data[OUT_LAKE_DEPTH].varname,"OUT_LAKE_DEPTH");           /* lake depth [m] */
  strcpy(out_data[OUT_LAKE_ICE].varname,"OUT_LAKE_ICE");               /* moisture stored as lake ice [mm] */
  strcpy(out_data[OUT_LAKE_ICE_FRACT].varname,"OUT_LAKE_ICE_FRACT");   /* fractional coverage of lake ice [fraction] */
  strcpy(out_data[OUT_LAKE_ICE_HEIGHT].varname,"OUT_LAKE_ICE_HEIGHT"); /* thickness of lake ice [cm] */
  strcpy(out_data[OUT_LAKE_MOIST].varname,"OUT_LAKE_MOIST");           /* liquid water stored in lake [mm over lake area?] */
  strcpy(out_data[OUT_LAKE_SURF_AREA].varname,"OUT_LAKE_SURF_AREA");   /* lake surface area [m2] */
  strcpy(out_data[OUT_LAKE_SWE].varname,"OUT_LAKE_SWE");               /* liquid water equivalent of snow on top of lake ice [m over lake ice] */
  strcpy(out_data[OUT_LAKE_SWE_V].varname,"OUT_LAKE_SWE_V");           /* volumetric liquid water equivalent of snow on top of lake ice [m3] */
  strcpy(out_data[OUT_LAKE_VOLUME].varname,"OUT_LAKE_VOLUME");         /* lake volume [m3] */
  strcpy(out_data[OUT_ROOTMOIST].varname,"OUT_ROOTMOIST");             /* root zone soil moisture [mm] */
  strcpy(out_data[OUT_SMFROZFRAC].varname,"OUT_SMFROZFRAC");           /* fraction of soil moisture (by mass) that is ice, for each soil layer */
  strcpy(out_data[OUT_SMLIQFRAC].varname,"OUT_SMLIQFRAC");             /* fraction of soil moisture (by mass) that is liquid, for each soil layer */
  strcpy(out_data[OUT_SNOW_CANOPY].varname,"OUT_SNOW_CANOPY");         /* snow interception storage in canopy [mm] */
  strcpy(out_data[OUT_SNOW_COVER].varname,"OUT_SNOW_COVER");           /* fractional area of snow cover [fraction] */
  strcpy(out_data[OUT_SNOW_DEPTH].varname,"OUT_SNOW_DEPTH");           /* depth of snow pack [cm] */
  strcpy(out_data[OUT_SOIL_ICE].varname,"OUT_SOIL_ICE");               /* soil ice content [mm] for each soil layer */
  strcpy(out_data[OUT_SOIL_LIQ].varname,"OUT_SOIL_LIQ");               /* soil liquid moisture content [mm] for each soil layer */
  strcpy(out_data[OUT_SOIL_MOIST].varname,"OUT_SOIL_MOIST");           /* soil total moisture content [mm] for each soil layer */
  strcpy(out_data[OUT_SOIL_WET].varname,"OUT_SOIL_WET");               /* vertical average of (soil moisture - wilting point)/(maximum soil moisture - wilting point) [mm/mm] */
  strcpy(out_data[OUT_SURFSTOR].varname,"OUT_SURFSTOR");               /* storage of liquid water on surface (ponding) [mm] */
  strcpy(out_data[OUT_SURF_FROST_FRAC].varname,"OUT_SURF_FROST_FRAC"); /* fraction of soil surface that is frozen [fraction] */
  strcpy(out_data[OUT_SWE].varname,"OUT_SWE");                         /* snow water equivalent in snow pack [mm] */
  strcpy(out_data[OUT_WDEW].varname,"OUT_WDEW");                       /* total moisture interception storage in canopy [mm] */
  strcpy(out_data[OUT_ZWT].varname,"OUT_ZWT");                         /* water table position [cm] (zwt within lowest unsaturated layer) */
  strcpy(out_data[OUT_ZWT_LUMPED].varname,"OUT_ZWT_LUMPED");           /* lumped water table position [cm] (zwt of total moisture across all layers, lumped together) */

  // Water Balance Terms - fluxes
  strcpy(out_data[OUT_BASEFLOW].varname,"OUT_BASEFLOW");               /* baseflow out of the bottom layer [mm] (ALMA_OUTPUT: [mm/s]) */
  strcpy(out_data[OUT_DELINTERCEPT].varname,"OUT_DELINTERCEPT");       /* change in canopy interception storage [mm] */
  strcpy(out_data[OUT_DELSOILMOIST].varname,"OUT_DELSOILMOIST");       /* change in soil water content [mm] */
  strcpy(out_data[OUT_DELSWE].varname,"OUT_DELSWE");                   /* change in snow water equivalent [mm] */
  strcpy(out_data[OUT_DELSURFSTOR].varname,"OUT_DELSURFSTOR");         /* change in surface liquid water storage  [mm] */
  strcpy(out_data[OUT_EVAP].varname,"OUT_EVAP");                       /* total net evaporation [mm] (ALMA_OUTPUT: [mm/s]) */
  strcpy(out_data[OUT_EVAP_BARE].varname,"OUT_EVAP_BARE");             /* net evaporation from bare soil [mm] (ALMA_OUTPUT: [mm/s]) */
  strcpy(out_data[OUT_EVAP_CANOP].varname,"OUT_EVAP_CANOP");           /* net evaporation from canopy interception [mm] (ALMA_OUTPUT: [mm/s]) */
  strcpy(out_data[OUT_INFLOW].varname,"OUT_INFLOW");                   /* moisture that reaches top of soil column [mm] (ALMA_OUTPUT: [mm/s]) */
  strcpy(out_data[OUT_LAKE_BF_IN].varname,"OUT_LAKE_BF_IN");           /* incoming baseflow from lake catchment [mm] (ALMA_OUTPUT: [mm/s]) */
  strcpy(out_data[OUT_LAKE_BF_IN_V].varname,"OUT_LAKE_BF_IN_V");       /* incoming volumetric baseflow from lake catchment [m3] (ALMA_OUTPUT: [m3/s]) */
  strcpy(out_data[OUT_LAKE_BF_OUT].varname,"OUT_LAKE_BF_OUT");         /* outgoing baseflow lake [mm] (ALMA_OUTPUT: [mm/s]) */
  strcpy(out_data[OUT_LAKE_BF_OUT_V].varname,"OUT_LAKE_BF_OUT_V");     /* outgoing volumetric baseflow from lake [m3] (ALMA_OUTPUT: [m3/s]) */
  strcpy(out_data[OUT_LAKE_CHAN_IN].varname,"OUT_LAKE_CHAN_IN");       /* channel inflow into lake [mm] (ALMA_OUTPUT: [mm/s]) */
  strcpy(out_data[OUT_LAKE_CHAN_IN_V].varname,"OUT_LAKE_CHAN_IN_V");   /* volumetric channel inflow into lake [m3] (ALMA_OUTPUT: [m3/s]) */
  strcpy(out_data[OUT_LAKE_CHAN_OUT].varname,"OUT_LAKE_CHAN_OUT");     /* channel outflow from lake [mm] (ALMA_OUTPUT: [mm/s]) */
  strcpy(out_data[OUT_LAKE_CHAN_OUT_V].varname,"OUT_LAKE_CHAN_OUT_V"); /* volumetric channel outflow from lake [m3] (ALMA_OUTPUT: [m3/s]) */
  strcpy(out_data[OUT_LAKE_DSTOR].varname,"OUT_LAKE_DSTOR");           /* change in lake moisture storage (liquid plus ice cover) [mm] (ALMA_OUTPUT: [mm/s]) */
  strcpy(out_data[OUT_LAKE_DSTOR_V].varname,"OUT_LAKE_DSTOR_V");       /* volumetric change in lake moisture storage (liquid plus ice cover) [m3] (ALMA_OUTPUT: [m3/s]) */
  strcpy(out_data[OUT_LAKE_DSWE].varname,"OUT_LAKE_DSWE");             /* change in snowpack on top of lake ice [mm] (ALMA_OUTPUT: [mm/s]) */
  strcpy(out_data[OUT_LAKE_DSWE_V].varname,"OUT_LAKE_DSWE_V");         /* volumetric change in snowpack on top of lake ice [m3] (ALMA_OUTPUT: [m3/s]) */
  strcpy(out_data[OUT_LAKE_EVAP].varname,"OUT_LAKE_EVAP");             /* net evaporation from lake surface [mm] (ALMA_OUTPUT: [mm/s]) */
  strcpy(out_data[OUT_LAKE_EVAP_V].varname,"OUT_LAKE_EVAP_V");         /* net volumetric evaporation from lake surface [m3] (ALMA_OUTPUT: [m3/s]) */
  strcpy(out_data[OUT_LAKE_PREC_V].varname,"OUT_LAKE_PREC_V");         /* volumetric precipitation over lake surface [m3] (ALMA_OUTPUT: [m3/s]) */
  strcpy(out_data[OUT_LAKE_RCHRG].varname,"OUT_LAKE_RCHRG");           /* recharge from lake to surrounding wetland [mm] (ALMA_OUTPUT: [mm/s]) */
  strcpy(out_data[OUT_LAKE_RCHRG_V].varname,"OUT_LAKE_RCHRG_V");       /* volumetric recharge from lake to surrounding wetland [m3] (ALMA_OUTPUT: [m3/s]) */
  strcpy(out_data[OUT_LAKE_RO_IN].varname,"OUT_LAKE_RO_IN");           /* incoming runoff from lake catchment [mm] (ALMA_OUTPUT: [mm/s]) */
  strcpy(out_data[OUT_LAKE_RO_IN_V].varname,"OUT_LAKE_RO_IN_V");       /* incoming volumetric runoff from lake catchment [m3] (ALMA_OUTPUT: [m3/s]) */
  strcpy(out_data[OUT_LAKE_VAPFLX].varname,"OUT_LAKE_VAPFLX");         /* sublimation from lake snow pack [mm] (ALMA_OUTPUT: [mm/s]) */
  strcpy(out_data[OUT_LAKE_VAPFLX_V].varname,"OUT_LAKE_VAPFLX_V");     /* volumetric sublimation from lake snow pack [m3] (ALMA_OUTPUT: [m3/s]) */
  strcpy(out_data[OUT_PET_SATSOIL].varname,"OUT_PET_SATSOIL");         /* potential evap from saturated bare soil [mm] (ALMA_OUTPUT: [mm/s]) */
  strcpy(out_data[OUT_PET_H2OSURF].varname,"OUT_PET_H2OSURF");         /* potential evap from open water [mm] (ALMA_OUTPUT: [mm/s]) */
  strcpy(out_data[OUT_PET_SHORT].varname,"OUT_PET_SHORT");             /* potential evap from short reference crop (grass) [mm] (ALMA_OUTPUT: [mm/s]) */
  strcpy(out_data[OUT_PET_TALL].varname,"OUT_PET_TALL");               /* potential evap from tall reference crop (alfalfa) [mm] (ALMA_OUTPUT: [mm/s]) */
  strcpy(out_data[OUT_PET_NATVEG].varname,"OUT_PET_NATVEG");           /* potential evap from current vegetation and current canopy resistance [mm] (ALMA_OUTPUT: [mm/s]) */
  strcpy(out_data[OUT_PET_VEGNOCR].varname,"OUT_PET_VEGNOCR");         /* potential evap from current vegetation and 0 canopy resistance bare soil [mm] (ALMA_OUTPUT: [mm/s]) */
  strcpy(out_data[OUT_PREC].varname,"OUT_PREC");                       /* incoming precipitation [mm] (ALMA_OUTPUT: [mm/s]) */
  strcpy(out_data[OUT_RAINF].varname,"OUT_RAINF");                     /* rainfall [mm] (ALMA_OUTPUT: [mm/s]) */
  strcpy(out_data[OUT_REFREEZE].varname,"OUT_REFREEZE");               /* refreezing of water in the snow [mm] (ALMA_OUTPUT: [mm/s]) */
  strcpy(out_data[OUT_RUNOFF].varname,"OUT_RUNOFF");                   /* surface runoff [mm] (ALMA_OUTPUT: [mm/s]) */
  strcpy(out_data[OUT_SNOW_MELT].varname,"OUT_SNOW_MELT");             /* snow melt [mm] (ALMA_OUTPUT: [mm/s]) */
  strcpy(out_data[OUT_SNOWF].varname,"OUT_SNOWF");                     /* snowfall [mm] (ALMA_OUTPUT: [mm/s]) */
  strcpy(out_data[OUT_SUB_BLOWING].varname,"OUT_SUB_BLOWING");         /* net sublimation of blowing snow [mm] (ALMA_OUTPUT: [mm/s]) */
  strcpy(out_data[OUT_SUB_CANOP].varname,"OUT_SUB_CANOP");             /* net sublimation from snow stored in canopy [mm] (ALMA_OUTPUT: [mm/s]) */
  strcpy(out_data[OUT_SUB_SNOW].varname,"OUT_SUB_SNOW");               /* net sublimation from snow pack (surface and blowing) [mm] (ALMA_OUTPUT: [mm/s]) */
  strcpy(out_data[OUT_SUB_SURFACE].varname,"OUT_SUB_SURFACE");         /* net sublimation from snow pack surface [mm] (ALMA_OUTPUT: [mm/s]) */
  strcpy(out_data[OUT_TRANSP_VEG].varname,"OUT_TRANSP_VEG");           /* net transpiration from vegetation [mm] (ALMA_OUTPUT: [mm/s]) */

  // Energy Balance Terms - state variables
  strcpy(out_data[OUT_ALBEDO].varname,"OUT_ALBEDO");                   /* albedo [fraction] */
  strcpy(out_data[OUT_BARESOILT].varname,"OUT_BARESOILT");             /* bare soil surface temperature [C] (ALMA_OUTPUT: [K]) */
  strcpy(out_data[OUT_FDEPTH].varname,"OUT_FDEPTH");                   /* depth of freezing fronts [cm] (ALMA_OUTPUT: [m]) for each freezing front */
  strcpy(out_data[OUT_LAKE_ICE_TEMP].varname,"OUT_LAKE_ICE_TEMP");     /* lake ice temperature [K] */
  strcpy(out_data[OUT_LAKE_SURF_TEMP].varname,"OUT_LAKE_SURF_TEMP");   /* lake surface temperature [K] */
  strcpy(out_data[OUT_RAD_TEMP].varname,"OUT_RAD_TEMP");               /* average radiative surface temperature [K] */
  strcpy(out_data[OUT_SALBEDO].varname,"OUT_SALBEDO");                 /* snow albedo [fraction] */
  strcpy(out_data[OUT_SNOW_PACK_TEMP].varname,"OUT_SNOW_PACK_TEMP");   /* snow pack temperature [C] (ALMA_OUTPUT: [K]) */
  strcpy(out_data[OUT_SNOW_SURF_TEMP].varname,"OUT_SNOW_SURF_TEMP");   /* snow surface temperature [C] (ALMA_OUTPUT: [K]) */
  strcpy(out_data[OUT_SNOWT_FBFLAG].varname,"OUT_SNOWT_FBFLAG");       /* snow surface temperature flag */
  strcpy(out_data[OUT_SOIL_TEMP].varname,"OUT_SOIL_TEMP");             /* soil temperature [C] (ALMA_OUTPUT: [K]) for each soil layer */
  strcpy(out_data[OUT_SOIL_TNODE].varname,"OUT_SOIL_TNODE");           /* soil temperature [C] (ALMA_OUTPUT: [K]) for each soil thermal node */
  strcpy(out_data[OUT_SOIL_TNODE_WL].varname,"OUT_SOIL_TNODE_WL");     /* soil temperature [C] (ALMA_OUTPUT: [K]) for each soil thermal node in the wetland */
  strcpy(out_data[OUT_SOILT_FBFLAG].varname,"OUT_SOILT_FBFLAG");       /* soil temperature flag for each soil thermal node */
  strcpy(out_data[OUT_SURF_TEMP].varname,"OUT_SURF_TEMP");             /* average surface temperature [C] (ALMA_OUTPUT: [K]) */
  strcpy(out_data[OUT_SURFT_FBFLAG].varname,"OUT_SURFT_FBFLAG");       /* surface temperature flag */
  strcpy(out_data[OUT_TCAN_FBFLAG].varname,"OUT_TCAN_FBFLAG");         /* Tcanopy flag */
  strcpy(out_data[OUT_TDEPTH].varname,"OUT_TDEPTH");                   /* depth of thawing fronts [cm] (ALMA_OUTPUT: [m]) for each thawing front */
  strcpy(out_data[OUT_TFOL_FBFLAG].varname,"OUT_TFOL_FBFLAG");         /* Tfoliage flag */
  strcpy(out_data[OUT_VEGT].varname,"OUT_VEGT");                       /* average vegetation canopy temperature [C] (ALMA_OUTPUT: [K]) */

  // Energy Balance Terms - fluxes
  strcpy(out_data[OUT_ADV_SENS].varname,"OUT_ADV_SENS");               /* net sensible heat advected to snow pack [W/m2] */
  strcpy(out_data[OUT_ADVECTION].varname,"OUT_ADVECTION");             /* advected energy [W/m2] */
  strcpy(out_data[OUT_DELTACC].varname,"OUT_DELTACC");                 /* rate of change in cold content in snow pack [W/m2] */
  strcpy(out_data[OUT_DELTAH].varname,"OUT_DELTAH");                   /* rate of change in heat storage [W/m2] */
  strcpy(out_data[OUT_ENERGY_ERROR].varname,"OUT_ENERGY_ERROR");       /* energy budget error [W/m2] */
  strcpy(out_data[OUT_WATER_ERROR].varname,"OUT_WATER_ERROR");         /* water budget error [mm] */
  strcpy(out_data[OUT_FUSION].varname,"OUT_FUSION");                   /* net energy used to melt/freeze soil moisture [W/m2] */
  strcpy(out_data[OUT_GRND_FLUX].varname,"OUT_GRND_FLUX");             /* net heat flux into ground [W/m2] */
  strcpy(out_data[OUT_IN_LONG].varname,"OUT_IN_LONG");                 /* incoming longwave flux at surface (under veg) [W/m2] */
  strcpy(out_data[OUT_LATENT].varname,"OUT_LATENT");                   /* net upward latent heat flux [W/m2] */
  strcpy(out_data[OUT_LATENT_SUB].varname,"OUT_LATENT_SUB");           /* net upward latent heat flux from sublimation [W/m2] */
  strcpy(out_data[OUT_MELT_ENERGY].varname,"OUT_MELT_ENERGY");         /* energy of fusion (melting) [W/m2] */
  strcpy(out_data[OUT_NET_LONG].varname,"OUT_NET_LONG");               /* net downward longwave flux [W/m2] */
  strcpy(out_data[OUT_NET_SHORT].varname,"OUT_NET_SHORT");             /* net downward shortwave flux [W/m2] */
  strcpy(out_data[OUT_R_NET].varname,"OUT_R_NET");                     /* net downward radiation flux [W/m2] */
  strcpy(out_data[OUT_RFRZ_ENERGY].varname,"OUT_RFRZ_ENERGY");         /* net energy used to refreeze liquid water in snowpack [W/m2] */
  strcpy(out_data[OUT_SENSIBLE].varname,"OUT_SENSIBLE");               /* net upward sensible heat flux [W/m2] */
  strcpy(out_data[OUT_SNOW_FLUX].varname,"OUT_SNOW_FLUX");             /* energy flux through snow pack [W/m2] */

  // Miscellaneous Terms
  strcpy(out_data[OUT_AERO_COND].varname,"OUT_AERO_COND");             /* "scene" aerodynamic conductance [m/s] (tiles with overstory contribute overstory conductance; others contribue surface conductance) */
  strcpy(out_data[OUT_AERO_COND1].varname,"OUT_AERO_COND1");           /* surface aerodynamic conductance [m/s] */
  strcpy(out_data[OUT_AERO_COND2].varname,"OUT_AERO_COND2");           /* overstory aerodynamic conductance [m/s] */
  strcpy(out_data[OUT_AERO_RESIST].varname,"OUT_AERO_RESIST");         /* "scene" aerodynamic resistance [s/m] (tiles with overstory contribute overstory resistance; others contribue surface resistance)*/
  strcpy(out_data[OUT_AERO_RESIST1].varname,"OUT_AERO_RESIST1");       /* surface aerodynamic resistance [m/s] */
  strcpy(out_data[OUT_AERO_RESIST2].varname,"OUT_AERO_RESIST2");       /* overstory aerodynamic resistance [m/s] */
  strcpy(out_data[OUT_AIR_TEMP].varname,"OUT_AIR_TEMP");               /* air temperature [C] */
  strcpy(out_data[OUT_DENSITY].varname,"OUT_DENSITY");                 /* near-surface atmospheric density [kg/m3] */
  strcpy(out_data[OUT_LONGWAVE].varname,"OUT_LONGWAVE");               /* incoming longwave [W/m2] */
  strcpy(out_data[OUT_PRESSURE].varname,"OUT_PRESSURE");               /* near surface atmospheric pressure [kPa] */
  strcpy(out_data[OUT_QAIR].varname,"OUT_QAIR");                       /* specific humidity [kg/kg] */
  strcpy(out_data[OUT_REL_HUMID].varname,"OUT_REL_HUMID");             /* relative humidity [fraction]*/
  strcpy(out_data[OUT_SHORTWAVE].varname,"OUT_SHORTWAVE");             /* incoming shortwave [W/m2] */
  strcpy(out_data[OUT_SURF_COND].varname,"OUT_SURF_COND");             /* surface conductance [m/s] */
  strcpy(out_data[OUT_TSKC].varname,"OUT_TSKC");                       /* cloud cover fraction [fraction] */
  strcpy(out_data[OUT_VP].varname,"OUT_VP");                           /* near surface vapor pressure [kPa] */
  strcpy(out_data[OUT_VPD].varname,"OUT_VPD");                         /* near surface vapor pressure deficit [kPa] */
  strcpy(out_data[OUT_WIND].varname,"OUT_WIND");                       /* near surface wind speed [m/s] */

  // Dynamic Soil Layer Terms - EXCESS_ICE option
#if EXCESS_ICE
  strcpy(out_data[OUT_SOIL_DEPTH].varname,"OUT_SOIL_DEPTH");             /* soil moisture layer depths [m] */
  strcpy(out_data[OUT_SUBSIDENCE].varname,"OUT_SUBSIDENCE");             /* subsidence of soil layer [mm] */
  strcpy(out_data[OUT_POROSITY].varname,"OUT_POROSITY");                 /* porosity [mm/mm] */
  strcpy(out_data[OUT_ZSUM_NODE].varname,"OUT_ZSUM_NODE");               /* depths of thermal nodes [m] */
#endif

  // Band-specific quantities
  strcpy(out_data[OUT_ADV_SENS_BAND].varname,"OUT_ADV_SENS_BAND");               /* net sensible heat flux advected to snow pack [W/m2] */
  strcpy(out_data[OUT_ADVECTION_BAND].varname,"OUT_ADVECTION_BAND");             /* advected energy [W/m2] */
  strcpy(out_data[OUT_ALBEDO_BAND].varname,"OUT_ALBEDO_BAND");                   /* albedo [fraction] */
  strcpy(out_data[OUT_DELTACC_BAND].varname,"OUT_DELTACC_BAND");                 /* change in cold content in snow pack [W/m2] */
  strcpy(out_data[OUT_GRND_FLUX_BAND].varname,"OUT_GRND_FLUX_BAND");             /* net heat flux into ground [W/m2] */
  strcpy(out_data[OUT_IN_LONG_BAND].varname,"OUT_IN_LONG_BAND");                 /* incoming longwave flux at surface (under veg) [W/m2] */
  strcpy(out_data[OUT_LATENT_BAND].varname,"OUT_LATENT_BAND");                   /* net upward latent heat flux [W/m2] */
  strcpy(out_data[OUT_LATENT_SUB_BAND].varname,"OUT_LATENT_SUB_BAND");           /* net upward latent heat flux from sublimation [W/m2] */
  strcpy(out_data[OUT_MELT_ENERGY_BAND].varname,"OUT_MELT_ENERGY_BAND");         /* energy of fusion (melting) [W/m2] */
  strcpy(out_data[OUT_NET_LONG_BAND].varname,"OUT_NET_LONG_BAND");               /* net downward longwave flux [W/m2] */
  strcpy(out_data[OUT_NET_SHORT_BAND].varname,"OUT_NET_SHORT_BAND");             /* net downward shortwave flux [W/m2] */
  strcpy(out_data[OUT_RFRZ_ENERGY_BAND].varname,"OUT_RFRZ_ENERGY_BAND");         /* net energy used to refreeze liquid water in snowpack [W/m2] */
  strcpy(out_data[OUT_SENSIBLE_BAND].varname,"OUT_SENSIBLE_BAND");               /* net upward sensible heat flux [W/m2] */
  strcpy(out_data[OUT_SNOW_CANOPY_BAND].varname,"OUT_SNOW_CANOPY_BAND");         /* snow interception storage in canopy [mm] */
  strcpy(out_data[OUT_SNOW_COVER_BAND].varname,"OUT_SNOW_COVER_BAND");           /* fractional area of snow cover [fraction] */
  strcpy(out_data[OUT_SNOW_DEPTH_BAND].varname,"OUT_SNOW_DEPTH_BAND");           /* depth of snow pack [cm] */
  strcpy(out_data[OUT_SNOW_FLUX_BAND].varname,"OUT_SNOW_FLUX_BAND");             /* energy flux through snow pack [W/m2] */
  strcpy(out_data[OUT_SNOW_MELT_BAND].varname,"OUT_SNOW_MELT_BAND");             /* snow melt [mm] (ALMA_OUTPUT: [mm/s]) */
  strcpy(out_data[OUT_SNOW_PACKT_BAND].varname,"OUT_SNOW_PACKT_BAND");           /* snow pack temperature [C] (ALMA_OUTPUT: [K]) */
  strcpy(out_data[OUT_SNOW_SURFT_BAND].varname,"OUT_SNOW_SURFT_BAND");           /* snow surface temperature [C] (ALMA_OUTPUT: [K]) */
  strcpy(out_data[OUT_SWE_BAND].varname,"OUT_SWE_BAND");                         /* snow water equivalent in snow pack [mm] */

#ifdef VIC_CROPSYST_VERSION //keyvan 11132012
  strcpy(out_data[OUT_CROP_BIOM_CUR].varname,"OUT_CROP_BIOM_CUR");    /*Daily biomass from the crop model, t/ha */
  strcpy(out_data[OUT_CROP_TRANSPIR].varname,"OUT_CROP_TRANSPIR");        /*Crop model total transpiration from 3 layers */
  strcpy(out_data[OUT_CROP_IRRI_WAT].varname,"OUT_CROP_IRRI_WAT");  /*Crop model irrigation water that accounts for irrigation efficiency*/
  strcpy(out_data[OUT_CROP_BIOM_AHA].varname,"OUT_CROP_BIOM_AHA");  /*Biomass after harvest */
  strcpy(out_data[OUT_CROP_BIOMYELD].varname,"OUT_CROP_BIOMYELD"); /* Yield*/
  strcpy(out_data[OUT_CROP_WSI].varname,"OUT_CROP_WSI");/* Crop water stress index 0- "no stress" and 1- "stress"*/
  strcpy(out_data[OUT_CROP_LAI].varname,"OUT_CROP_LAI");/* Crop leaf area index*/
  strcpy(out_data[OUT_CROP_GROWTHST].varname,"OUT_CROP_GROWTHST");/*Crop growth stage. See growth_stages.h for enumeration */

  strcpy(out_data[OUT_TMIN].varname,"OUT_TMIN");/*For testing the crop model */
  strcpy(out_data[OUT_TMAX].varname,"OUT_TMAX");/* For testing the crop model*/
  strcpy(out_data[OUT_DAY_VPD].varname,"OUT_DAY_VPD");/*For testing the crop model */
  strcpy(out_data[OUT_SHRT_WAVE].varname,"OUT_SHRT_WAVE");/* For testing the crop model*/

  strcpy(out_data[OUT_LYR0_VW ].varname,"OUT_LYR0_VW");/* For testing the crop model*/
  strcpy(out_data[OUT_LYR1_VW ].varname,"OUT_LYR1_VW");/* For testing the crop model*/
  strcpy(out_data[OUT_LYR2_VW ].varname,"OUT_LYR2_VW");/* For testing the crop model*/
  strcpy(out_data[OUT_CROP_TRANS0 ].varname,"OUT_CROP_TRANS0");/* For testing the crop model*/
  strcpy(out_data[OUT_CROP_TRANS1 ].varname,"OUT_CROP_TRANS1");/* For testing the crop model*/
  strcpy(out_data[OUT_CROP_TRANS2 ].varname,"OUT_CROP_TRANS2");/* For testing the crop model*/

  //for testing
  strcpy(out_data[OUT_RHMAX ].varname,"OUT_RHMAX");/* For testing the crop model*/
  strcpy(out_data[OUT_RHMIN ].varname,"OUT_RHMIN");/* For testing the crop model*/
  strcpy(out_data[OUT_WS ].varname,"OUT_WS");/* For testing the crop model*/

  strcpy(out_data[OUT_SWD0 ].varname,"OUT_SWD0");
  strcpy(out_data[OUT_SWD1 ].varname,"OUT_SWD1");
  strcpy(out_data[OUT_SWD2 ].varname,"OUT_SWD2");
  strcpy(out_data[OUT_CROP_EVAP ].varname,"OUT_CROP_EVAP");
  strcpy(out_data[OUT_INFILT ].varname,"OUT_INFILT");
  ///irrigation outputs 130503 Keyvan
  strcpy(out_data[OUT_EVAP_FROM_IRRIG ].varname,"OUT_EVAP_FROM_IRRIG");
  strcpy(out_data[OUT_EVAP_CANOP_IRRIG ].varname,"OUT_EVAP_CANOP_IRRIG");
  strcpy(out_data[OUT_EVAP_SOIL ].varname,"OUT_EVAP_SOIL");
  strcpy(out_data[OUT_RUNOFF_IRRIG ].varname,"OUT_RUNOFF_IRRIG");
  strcpy(out_data[OUT_ET_POT_SHORT ].varname,"OUT_ET_POT_SHORT");
  strcpy(out_data[OUT_CROP_COEF_KC ].varname,"OUT_CROP_COEF_KC");
  strcpy(out_data[OUT_CROP_SOIL_MOIST ].varname,"OUT_CROP_SOIL_MOIST"); //02072013 keyvan added to print soil moisture in cropping areas
  strcpy(out_data[OUT_CROP_SOIL_POTEN ].varname,"OUT_CROP_SOIL_POTEN"); //02072013 keyvan added to print soil potential in cropping areas
  strcpy(out_data[OUT_CROP_TRANS_POT ].varname,"OUT_CROP_TRANS_POT"); //02072013 keyvan added to print soil potential in cropping areas
  strcpy(out_data[OUT_CANOPY_FRACT ].varname,"OUT_CANOPY_FRACT");
  strcpy(out_data[OUT_DEL_T ].varname,"OUT_DEL_T");
  strcpy(out_data[OUT_DEL_VPD ].varname,"OUT_DEL_VPD");
  strcpy(out_data[OUT_IRRIG_INTERCEPTION ].varname,"OUT_IRRIG_INTERCEPTION");
  //170924LML strcpy(out_data[OUT_IRRIGATION].varname,"OUT_IRRIGATION");                      //150930LML
  //170924LML strcpy(out_data[OUT_CELL_EVAP_FROM_IRRIG ].varname,"OUT_CELL_EVAP_FROM_IRRIG"); //150930LML
  //170924LML strcpy(out_data[OUT_CELL_EVAP_CANOP_IRRIG].varname,"OUT_CELL_EVAP_CANOP_IRRIG");//150930LML
  //170924LML strcpy(out_data[OUT_CELL_RUNOFF_IRRIG    ].varname,"OUT_CELL_RUNOFF_IRRIG");    //150930LML
  //170924LML strcpy(out_data[OUT_CELL_IRRIGATION      ].varname,"OUT_CELL_IRRIGATION");      //150930LML
#endif
  // Set number of elements - default is 1
  for (v=0; v<N_OUTVAR_TYPES; v++) {
    out_data[v].nelem = 1;
  }
  if (options.FROZEN_SOIL) {
    out_data[OUT_FDEPTH].nelem = MAX_FRONTS;
    out_data[OUT_TDEPTH].nelem = MAX_FRONTS;
  }
  out_data[OUT_SMLIQFRAC].nelem = options.Nlayer;
  out_data[OUT_SMFROZFRAC].nelem = options.Nlayer;
  out_data[OUT_SOIL_ICE].nelem = options.Nlayer;
  out_data[OUT_SOIL_LIQ].nelem = options.Nlayer;
  out_data[OUT_SOIL_MOIST].nelem = options.Nlayer;
  out_data[OUT_SOIL_TEMP].nelem = options.Nlayer;
#if EXCESS_ICE
  out_data[OUT_SOIL_DEPTH].nelem = options.Nlayer;
  out_data[OUT_SUBSIDENCE].nelem = options.Nlayer;
  out_data[OUT_POROSITY].nelem = options.Nlayer;
  out_data[OUT_ZSUM_NODE].nelem = options.Nnode;
#endif
  out_data[OUT_SOIL_TNODE].nelem = options.Nnode;
  out_data[OUT_SOIL_TNODE_WL].nelem = options.Nnode;
  out_data[OUT_SOILT_FBFLAG].nelem = options.Nnode;
  out_data[OUT_ADV_SENS_BAND].nelem = options.SNOW_BAND;
  out_data[OUT_ADVECTION_BAND].nelem = options.SNOW_BAND;
  out_data[OUT_ALBEDO_BAND].nelem = options.SNOW_BAND;
  out_data[OUT_DELTACC_BAND].nelem = options.SNOW_BAND;
  out_data[OUT_GRND_FLUX_BAND].nelem = options.SNOW_BAND;
  out_data[OUT_IN_LONG_BAND].nelem = options.SNOW_BAND;
  out_data[OUT_LATENT_BAND].nelem = options.SNOW_BAND;
  out_data[OUT_LATENT_SUB_BAND].nelem = options.SNOW_BAND;
  out_data[OUT_MELT_ENERGY_BAND].nelem = options.SNOW_BAND;
  out_data[OUT_NET_LONG_BAND].nelem = options.SNOW_BAND;
  out_data[OUT_NET_SHORT_BAND].nelem = options.SNOW_BAND;
  out_data[OUT_RFRZ_ENERGY_BAND].nelem = options.SNOW_BAND;
  out_data[OUT_SENSIBLE_BAND].nelem = options.SNOW_BAND;
  out_data[OUT_SNOW_CANOPY_BAND].nelem = options.SNOW_BAND;
  out_data[OUT_SNOW_COVER_BAND].nelem = options.SNOW_BAND;
  out_data[OUT_SNOW_DEPTH_BAND].nelem = options.SNOW_BAND;
  out_data[OUT_SNOW_FLUX_BAND].nelem = options.SNOW_BAND;
  out_data[OUT_SNOW_MELT_BAND].nelem = options.SNOW_BAND;
  out_data[OUT_SNOW_PACKT_BAND].nelem = options.SNOW_BAND;
  out_data[OUT_SNOW_SURFT_BAND].nelem = options.SNOW_BAND;
  out_data[OUT_SWE_BAND].nelem = options.SNOW_BAND;

#ifdef VIC_CROPSYST_VERSION //keyvan 11132012
  set_output_for_crop_numbers(out_data,MAX_CROPROTATIONS_IN_ONE_GRIDCELL);       //150930LML
/**nelem in the crop paramters are generally equal to the number of crops in the crop library.
However not every crop will be present in all the cells.  The output should account for this.

The array size of the crop varaible in the output_data cannot be made equal thr he number of crops
in the grid cell because the dimension is decaled befoer we go into the grid cell loop and is
common to all the grid cells being run.

Note that even though the array length of a crop output variable equlas the number of crop in the
library it will store values for only those crops in the current grid cell and in the order of the
crop occurence in the vegparam file.
For example, if there are 10 crops in the crop library,
out_data[OUT_CROP_BIOM_CUR].nelem = 10 andOUT_CROP_BIOM_CUR out_data[OUT_CROP_BIOM_CUR].data[0 to 9]

If the current cell has only 3 crops, 210, 218 and 1827 (crop codes in crop library) in that order, only
out_data[OUT_CROP_BIOM_CUR].data[0 to 2] will have values respectively to the crops 210,218 and 1827.
KJC 02162011
*/
  /*out_data[OUT_CROP_BIOM_CUR].nelem=MAX_VEG;
  out_data[OUT_CROP_TRANSPIR].nelem=MAX_VEG;
  out_data[OUT_CROP_IRRI_WAT].nelem=MAX_VEG;
  out_data[OUT_CROP_BIOM_AHA].nelem=MAX_VEG;
  out_data[OUT_CROP_BIOMYELD].nelem=MAX_VEG;
  out_data[OUT_CROP_WSI].nelem=MAX_VEG;
  out_data[OUT_CROP_LAI].nelem=MAX_VEG;
  out_data[OUT_CROP_GROWTHST].nelem=MAX_VEG;

  //out_data[OUT_TMIN].nelem=MAX_VEG;
  //out_data[OUT_TMAX].nelem=MAX_VEG;
 // out_data[OUT_DAY_VPD].nelem=MAX_VEG;
  //out_data[OUT_SHRT_WAVE].nelem=MAX_VEG;

  out_data[OUT_LYR0_VW ].nelem=MAX_VEG;
  out_data[OUT_LYR1_VW ].nelem=MAX_VEG;
  out_data[OUT_LYR2_VW ].nelem=MAX_VEG;
  out_data[OUT_CROP_TRANS0 ].nelem=MAX_VEG;
  out_data[OUT_CROP_TRANS1 ].nelem=MAX_VEG;
  out_data[OUT_CROP_TRANS2 ].nelem=MAX_VEG;

  out_data[OUT_SWD0 ].nelem=MAX_VEG;
  out_data[OUT_SWD1 ].nelem=MAX_VEG;
  out_data[OUT_SWD2 ].nelem=MAX_VEG;
  out_data[OUT_CROP_EVAP ].nelem=MAX_VEG;
  out_data[OUT_INFILT ].nelem=MAX_VEG;//this need not be based on the crops and should actually be for each land cover
  ///130507 Keyvan added this
  out_data[OUT_EVAP_CANOP_IRRIG ].nelem=MAX_VEG;
  out_data[OUT_EVAP_FROM_IRRIG ].nelem=MAX_VEG;
  out_data[OUT_EVAP_SOIL ].nelem=MAX_VEG;
  out_data[OUT_RUNOFF_IRRIG ].nelem=MAX_VEG;
  out_data[OUT_ET_POT_SHORT ].nelem=MAX_VEG;
  out_data[OUT_CROP_COEF_KC ].nelem=MAX_VEG;
  out_data[OUT_CROP_SOIL_MOIST ].nelem=MAX_VEG;
  out_data[OUT_CROP_SOIL_POTEN ].nelem=MAX_VEG;
  out_data[OUT_CROP_TRANS_POT ].nelem=MAX_VEG;
  out_data[OUT_CANOPY_FRACT ].nelem=MAX_VEG; //130709 keyvan
  out_data[OUT_DEL_T ].nelem=MAX_VEG;
  out_data[OUT_DEL_VPD ].nelem=MAX_VEG;

*/
#endif

  // Set aggregation method - default is to average over the interval
  for (v=0; v<N_OUTVAR_TYPES; v++) {
    out_data[v].aggtype = AGG_TYPE_AVG;
  }
  out_data[OUT_ASAT].aggtype = AGG_TYPE_END;
  out_data[OUT_LAKE_AREA_FRAC].aggtype = AGG_TYPE_END;
  out_data[OUT_LAKE_DEPTH].aggtype = AGG_TYPE_END;
  out_data[OUT_LAKE_ICE].aggtype = AGG_TYPE_END;
  out_data[OUT_LAKE_ICE_FRACT].aggtype = AGG_TYPE_END;
  out_data[OUT_LAKE_ICE_HEIGHT].aggtype = AGG_TYPE_END;
  out_data[OUT_LAKE_MOIST].aggtype = AGG_TYPE_END;
  out_data[OUT_LAKE_SURF_AREA].aggtype = AGG_TYPE_END;
  out_data[OUT_LAKE_SWE].aggtype = AGG_TYPE_END;
  out_data[OUT_LAKE_SWE_V].aggtype = AGG_TYPE_END;
  out_data[OUT_LAKE_VOLUME].aggtype = AGG_TYPE_END;
  out_data[OUT_ROOTMOIST].aggtype = AGG_TYPE_END;
  out_data[OUT_SMFROZFRAC].aggtype = AGG_TYPE_END;
  out_data[OUT_SMLIQFRAC].aggtype = AGG_TYPE_END;
  out_data[OUT_SNOW_CANOPY].aggtype = AGG_TYPE_END;
  out_data[OUT_SNOW_COVER].aggtype = AGG_TYPE_END;
  out_data[OUT_SNOW_DEPTH].aggtype = AGG_TYPE_END;
  out_data[OUT_SOIL_ICE].aggtype = AGG_TYPE_END;
  out_data[OUT_SOIL_LIQ].aggtype = AGG_TYPE_END;
  out_data[OUT_SOIL_MOIST].aggtype = AGG_TYPE_END;
  out_data[OUT_SOIL_WET].aggtype = AGG_TYPE_END;
  out_data[OUT_SURFSTOR].aggtype = AGG_TYPE_END;
  out_data[OUT_SURF_FROST_FRAC].aggtype = AGG_TYPE_END;
  out_data[OUT_SWE].aggtype = AGG_TYPE_END;
  out_data[OUT_WDEW].aggtype = AGG_TYPE_END;
  out_data[OUT_ZWT].aggtype = AGG_TYPE_END;
  out_data[OUT_ZWT_LUMPED].aggtype = AGG_TYPE_END;
  out_data[OUT_SNOW_CANOPY_BAND].aggtype = AGG_TYPE_END;
  out_data[OUT_SNOW_COVER_BAND].aggtype = AGG_TYPE_END;
  out_data[OUT_SNOW_DEPTH_BAND].aggtype = AGG_TYPE_END;
  out_data[OUT_SWE_BAND].aggtype = AGG_TYPE_END;
#if EXCESS_ICE
  out_data[OUT_SOIL_DEPTH].aggtype = AGG_TYPE_END;
  out_data[OUT_POROSITY].aggtype = AGG_TYPE_END;
  out_data[OUT_ZSUM_NODE].aggtype = AGG_TYPE_END;
  out_data[OUT_SUBSIDENCE].aggtype = AGG_TYPE_SUM;
#endif
  out_data[OUT_BASEFLOW].aggtype = AGG_TYPE_SUM;
  out_data[OUT_DELINTERCEPT].aggtype = AGG_TYPE_SUM;
  out_data[OUT_DELSOILMOIST].aggtype = AGG_TYPE_SUM;
  out_data[OUT_DELSWE].aggtype = AGG_TYPE_SUM;
  out_data[OUT_DELSURFSTOR].aggtype = AGG_TYPE_SUM;
  out_data[OUT_EVAP].aggtype = AGG_TYPE_SUM;
  out_data[OUT_EVAP_BARE].aggtype = AGG_TYPE_SUM;
  out_data[OUT_EVAP_CANOP].aggtype = AGG_TYPE_SUM;
  out_data[OUT_INFLOW].aggtype = AGG_TYPE_SUM;
  out_data[OUT_LAKE_BF_IN].aggtype = AGG_TYPE_SUM;
  out_data[OUT_LAKE_BF_IN_V].aggtype = AGG_TYPE_SUM;
  out_data[OUT_LAKE_BF_OUT].aggtype = AGG_TYPE_SUM;
  out_data[OUT_LAKE_BF_OUT_V].aggtype = AGG_TYPE_SUM;
  out_data[OUT_LAKE_CHAN_IN].aggtype = AGG_TYPE_SUM;
  out_data[OUT_LAKE_CHAN_IN_V].aggtype = AGG_TYPE_SUM;
  out_data[OUT_LAKE_CHAN_OUT].aggtype = AGG_TYPE_SUM;
  out_data[OUT_LAKE_CHAN_OUT_V].aggtype = AGG_TYPE_SUM;
  out_data[OUT_LAKE_DSTOR].aggtype = AGG_TYPE_SUM;
  out_data[OUT_LAKE_DSTOR_V].aggtype = AGG_TYPE_SUM;
  out_data[OUT_LAKE_DSWE].aggtype = AGG_TYPE_SUM;
  out_data[OUT_LAKE_DSWE_V].aggtype = AGG_TYPE_SUM;
  out_data[OUT_LAKE_EVAP].aggtype = AGG_TYPE_SUM;
  out_data[OUT_LAKE_EVAP_V].aggtype = AGG_TYPE_SUM;
  out_data[OUT_LAKE_PREC_V].aggtype = AGG_TYPE_SUM;
  out_data[OUT_LAKE_RCHRG].aggtype = AGG_TYPE_SUM;
  out_data[OUT_LAKE_RCHRG_V].aggtype = AGG_TYPE_SUM;
  out_data[OUT_LAKE_RO_IN].aggtype = AGG_TYPE_SUM;
  out_data[OUT_LAKE_RO_IN_V].aggtype = AGG_TYPE_SUM;
  out_data[OUT_LAKE_VAPFLX].aggtype = AGG_TYPE_SUM;
  out_data[OUT_LAKE_VAPFLX_V].aggtype = AGG_TYPE_SUM;
  out_data[OUT_PET_SATSOIL].aggtype = AGG_TYPE_SUM;
  out_data[OUT_PET_H2OSURF].aggtype = AGG_TYPE_SUM;
  out_data[OUT_PET_SHORT].aggtype = AGG_TYPE_SUM;
  out_data[OUT_PET_TALL].aggtype = AGG_TYPE_SUM;
  out_data[OUT_PET_NATVEG].aggtype = AGG_TYPE_SUM;
  out_data[OUT_PET_VEGNOCR].aggtype = AGG_TYPE_SUM;
  out_data[OUT_PREC].aggtype = AGG_TYPE_SUM;
  out_data[OUT_RAINF].aggtype = AGG_TYPE_SUM;
  out_data[OUT_REFREEZE].aggtype = AGG_TYPE_SUM;
  out_data[OUT_RUNOFF].aggtype = AGG_TYPE_SUM;
  out_data[OUT_SNOW_MELT].aggtype = AGG_TYPE_SUM;
  out_data[OUT_SNOWF].aggtype = AGG_TYPE_SUM;
  out_data[OUT_SUB_BLOWING].aggtype = AGG_TYPE_SUM;
  out_data[OUT_SUB_CANOP].aggtype = AGG_TYPE_SUM;
  out_data[OUT_SUB_SNOW].aggtype = AGG_TYPE_SUM;
  out_data[OUT_SUB_SURFACE].aggtype = AGG_TYPE_SUM;
  out_data[OUT_TRANSP_VEG].aggtype = AGG_TYPE_SUM;
  out_data[OUT_DELTACC_BAND].aggtype = AGG_TYPE_SUM;
  out_data[OUT_SNOW_MELT].aggtype = AGG_TYPE_SUM;
  out_data[OUT_SNOWT_FBFLAG].aggtype = AGG_TYPE_SUM;
  out_data[OUT_SOILT_FBFLAG].aggtype = AGG_TYPE_SUM;
  out_data[OUT_SURFT_FBFLAG].aggtype = AGG_TYPE_SUM;
  out_data[OUT_TCAN_FBFLAG].aggtype = AGG_TYPE_SUM;
  out_data[OUT_TFOL_FBFLAG].aggtype = AGG_TYPE_SUM;
  //150930LML
  #ifdef VIC_CROPSYST_VERSION
  out_data[OUT_CROP_BIOM_CUR   ].aggtype = AGG_TYPE_END;
  out_data[OUT_CROP_TRANSPIR   ].aggtype = AGG_TYPE_SUM;
  out_data[OUT_CROP_IRRI_WAT   ].aggtype = AGG_TYPE_SUM;
  out_data[OUT_CROP_BIOM_AHA   ].aggtype = AGG_TYPE_END;
  out_data[OUT_CROP_BIOMYELD   ].aggtype = AGG_TYPE_END;
  out_data[OUT_CROP_LAI        ].aggtype = AGG_TYPE_END;
  out_data[OUT_CROP_GROWTHST   ].aggtype = AGG_TYPE_END;
  out_data[OUT_LYR0_VW         ].aggtype = AGG_TYPE_END;
  out_data[OUT_LYR1_VW         ].aggtype = AGG_TYPE_END;
  out_data[OUT_LYR2_VW         ].aggtype = AGG_TYPE_END;
  out_data[OUT_CROP_TRANS0     ].aggtype = AGG_TYPE_SUM;
  out_data[OUT_CROP_TRANS1     ].aggtype = AGG_TYPE_SUM;
  out_data[OUT_CROP_TRANS2     ].aggtype = AGG_TYPE_SUM;
  out_data[OUT_SWD0            ].aggtype = AGG_TYPE_END;
  out_data[OUT_SWD1            ].aggtype = AGG_TYPE_END;
  out_data[OUT_SWD2            ].aggtype = AGG_TYPE_END;
  out_data[OUT_CROP_EVAP       ].aggtype = AGG_TYPE_SUM;
  out_data[OUT_INFILT          ].aggtype = AGG_TYPE_SUM;
  out_data[OUT_EVAP_FROM_IRRIG ].aggtype = AGG_TYPE_SUM;
  out_data[OUT_EVAP_CANOP_IRRIG].aggtype = AGG_TYPE_SUM;
  out_data[OUT_EVAP_SOIL       ].aggtype = AGG_TYPE_SUM;
  out_data[OUT_RUNOFF_IRRIG    ].aggtype = AGG_TYPE_SUM;
  out_data[OUT_ET_POT_SHORT    ].aggtype = AGG_TYPE_SUM;
  out_data[OUT_CROP_TRANS_POT  ].aggtype = AGG_TYPE_SUM;
  out_data[OUT_IRRIG_INTERCEPTION].aggtype = AGG_TYPE_SUM;                       //170924LML
  //170924LML out_data[OUT_IRRIGATION      ].aggtype = AGG_TYPE_SUM;
  //170924LML out_data[OUT_CELL_EVAP_FROM_IRRIG ].aggtype = AGG_TYPE_SUM;
  //170924LML out_data[OUT_CELL_EVAP_CANOP_IRRIG].aggtype = AGG_TYPE_SUM;
  //170924LML out_data[OUT_CELL_RUNOFF_IRRIG    ].aggtype = AGG_TYPE_SUM;
  //170924LML out_data[OUT_CELL_IRRIGATION      ].aggtype = AGG_TYPE_SUM;
  #endif

  // Allocate space for data
  for (v=0; v<N_OUTVAR_TYPES; v++) {
    //150930LML #if (VIC_CROPSYST_VERSION>=3)
    //150930LML const int max_no_crops_in_cell = 50;                                        //LML 150407 need double check
    //150930LML out_data[v].data = (double *)calloc(max_no_crops_in_cell, sizeof(double));    //LML 150407
    //150930LML out_data[v].aggdata = (double *)calloc(max_no_crops_in_cell, sizeof(double)); //LML 150407
    //150930LML #else
    out_data[v].data = (double *)calloc(out_data[v].nelem, sizeof(double));
    out_data[v].aggdata = (double *)calloc(out_data[v].nelem, sizeof(double));
    //150930LML #endif
  }

  // Initialize data values
  init_output_list(out_data, FALSE, "%.4f", OUT_TYPE_FLOAT, 1);

  return out_data;

}


void init_output_list(out_data_struct *out_data, int write, char *format, int type, float mult) {
/*************************************************************
  init_output_list()      Ted Bohn     September 08, 2006

  This routine initializes the output information for all output variables.

*************************************************************/
  int varid, i;

  for (varid=0; varid<N_OUTVAR_TYPES; varid++) {
    out_data[varid].write = write;
    strcpy(out_data[varid].format,format);
    out_data[varid].type = type;
    out_data[varid].mult = mult;
    for(i=0; i<out_data[varid].nelem; i++) {
      out_data[varid].data[i] = 0;
    }
  }

}


int set_output_var(out_data_file_struct *out_data_files,
                    int write,
                    int filenum,
                    out_data_struct *out_data,
                    char *varname,
                    int varnum,
                    char *format,
                    int type,
                    float mult) {
/*************************************************************
  set_output_var()      Ted Bohn     September 08, 2006

  This routine updates the output information for a given output variable.

*************************************************************/
  int varid;
  int found=FALSE;
  int status=0;

  for (varid=0; varid<N_OUTVAR_TYPES; varid++) {
    if (strcmp(out_data[varid].varname,varname) == 0) {
      found = TRUE;
      out_data[varid].write = write;
      if (strcmp(format,"*") != 0)
        strcpy(out_data[varid].format,format);
      if (type != 0)
        out_data[varid].type = type;
      if (mult != 0)
        out_data[varid].mult = mult;
      out_data_files[filenum].varid[varnum] = varid;
    }
  }
  if (!found) {
    status = -1;
    fprintf(stderr, "Error: set_output_var: \"%s\" was not found in the list of supported output variable names.  Please use the exact name listed in vicNl_def.h.\n", varname);
  }
  return status;

}


void zero_output_list(out_data_struct *out_data) {
/*************************************************************
  zero_output_list()      Ted Bohn     September 08, 2006

  This routine resets the values of all output variables to 0.

*************************************************************/
  int varid, i;

  for (varid=0; varid<N_OUTVAR_TYPES; varid++) {
    for(i=0; i<out_data[varid].nelem; i++) {
      out_data[varid].data[i] = 0;
    }
  }

}

void free_out_data_files(out_data_file_struct **out_data_files) {
/*************************************************************
  free_out_data_files()      Ted Bohn     September 08, 2006

  This routine frees the memory in the out_data_files array.

*************************************************************/
  extern option_struct options;
  int filenum;

  for (filenum=0; filenum<options.Noutfiles; filenum++) {
// if((out_data_files)[filenum]) ///check to see if this one is correct and see what we have in other versions 01102013- keyvan
    free((char*)(*out_data_files)[filenum].varid);
  }
  free((char*)(*out_data_files));

}

void free_out_data(out_data_struct **out_data) {
/*************************************************************
  free_out_data()      Ted Bohn     April 19, 2007

  This routine frees the memory in the out_data array.

*************************************************************/

  int varid;

  for (varid=0; varid<N_OUTVAR_TYPES; varid++) {
    free((char*)(*out_data)[varid].data);
    free((char*)(*out_data)[varid].aggdata);
  }
  free((char*)(*out_data));

}

/*170413LML moved to VCS_Nl.c
#ifdef VIC_CROPSYST_VERSION
//150928LML_____________________________________________________________________
int set_output_for_crop_numbers(out_data_struct *out_data,const int numcrops)
{
    out_data[OUT_CROP_BIOM_CUR].nelem       = numcrops;
    out_data[OUT_CROP_TRANSPIR].nelem       = numcrops;
    out_data[OUT_CROP_IRRI_WAT].nelem       = numcrops;
    out_data[OUT_CROP_BIOM_AHA].nelem       = numcrops;
    out_data[OUT_CROP_BIOMYELD].nelem       = numcrops;
    out_data[OUT_CROP_WSI].nelem            = numcrops;
    out_data[OUT_CROP_LAI].nelem            = numcrops;
    out_data[OUT_CROP_GROWTHST].nelem       = numcrops;
    out_data[OUT_LYR0_VW ].nelem            = numcrops;
    out_data[OUT_LYR1_VW ].nelem            = numcrops;
    out_data[OUT_LYR2_VW ].nelem            = numcrops;
    out_data[OUT_CROP_TRANS0 ].nelem        = numcrops;
    out_data[OUT_CROP_TRANS1 ].nelem        = numcrops;
    out_data[OUT_CROP_TRANS2 ].nelem        = numcrops;
    out_data[OUT_SWD0 ].nelem               = numcrops;
    out_data[OUT_SWD1 ].nelem               = numcrops;
    out_data[OUT_SWD2 ].nelem               = numcrops;
    out_data[OUT_CROP_EVAP ].nelem          = numcrops;
    out_data[OUT_INFILT ].nelem             = numcrops;//this need not be based on the crops and should actually be for each land cover
    ///130503 Keyvan added
    out_data[OUT_EVAP_CANOP_IRRIG ].nelem   = numcrops;
    out_data[OUT_EVAP_FROM_IRRIG ].nelem    = numcrops;
    out_data[OUT_EVAP_SOIL ].nelem          = numcrops;
    out_data[OUT_RUNOFF_IRRIG ].nelem       = numcrops;
    out_data[OUT_ET_POT_SHORT ].nelem       = numcrops;
    out_data[OUT_CROP_COEF_KC ].nelem       = numcrops;
    out_data[OUT_CROP_SOIL_MOIST ].nelem    = numcrops;
    out_data[OUT_CROP_SOIL_POTEN ].nelem    = numcrops;
    out_data[OUT_CROP_TRANS_POT].nelem      = numcrops;
    out_data[OUT_CANOPY_FRACT].nelem        = numcrops; //130709 keyvan
    out_data[OUT_DEL_T].nelem               = numcrops;
    out_data[OUT_DEL_VPD].nelem             = numcrops;
    out_data[OUT_IRRIGATION].nelem          = numcrops;                          //150930LML
    return numcrops;
}
#endif
*/
