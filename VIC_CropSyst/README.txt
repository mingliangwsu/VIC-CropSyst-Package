# README.txt - VIC Model Release Notes
#
# $Id: README.txt,v 5.72.2.148 2012/10/25 23:15:57 vicadmin Exp $
#------------------------------------------------------------------------

--------------------------------------------------------------------------------
                     ***** VIC MODEL RELEASE NOTES *****
--------------------------------------------------------------------------------
README.txt - general notes on VIC model source code. For details on differences
between versions see Revisions.txt
--------------------------------------------------------------------------------

Usage:
------

	vicNl [-v | -o | -g<global_parameter_file>]

	  v: display version information
	  o: display compile-time options settings (set in user_def.h)
	  g: read model parameters from <global_parameter_file>.
	     <global_parameter_file> is a file that contains all needed model
	     parameters as well as model option flags, and the names and
	     locations of all other files.


--------------------------------------------------------------------------------
***** Description of changes from VIC 4.1.2.e to VIC 4.1.2.d *****
--------------------------------------------------------------------------------


Bug Fixes:
----------

Fixed incorrect reporting of canopy energy balance terms.

	Files Affected:

	calc_atmos_energy_bal.c
	calc_water_energy_balance_errors.c
	put_data.c
	surface_fluxes.c
	vicNl.h

	Description:

	VIC was not summing under- and over-story energy fluxes correctly for the
	case of a forest canopy with snow.  This only affected the reporting of
	energy balance terms in VIC's output; internal calculations were fine.

	In addition, a bug (sign error in flux summation) in VIC's calculation of
	the surface energy balance (for output only) was fixed.  This reduces the
	vast majority of the energy balance errors reported during the course of
	a typical energy balance simulation.




--------------------------------------------------------------------------------
***** Description of changes from VIC 4.1.2.d to VIC 4.1.2.c *****
--------------------------------------------------------------------------------


Bug Fixes:
----------

Fixed incorrect summing of rain and snow components of precipitation over grid
cell

	Files Affected:

	full_energy.c

	Description:

	The amounts of rainfall and snowfall over the lake (or inundated wetland)
	were being omitted from the grid cell totals.  This has been fixed.




Vapor pressure incorrect if user supplies (QAIR or REL_HUMID) + PRESSURE as
input forcings instead of vapor pressure.

	Files Affected:

	initialize_atmos.c

	Description:

	For the cases of the combination of (QAIR or REL_HUMID) plus PRESSURE
	supplied as input forcings instead of VP, the logic distinguishing
	between daily and sub-daily supplied PRESSURE was flawed, resulting
	in incorrect values in both cases.  This has been fixed.




--------------------------------------------------------------------------------
***** Description of changes from VIC 4.1.2.c to VIC 4.1.2.b *****
--------------------------------------------------------------------------------


Bug Fixes:
----------

Incorrect handling of user-supplied tskc (cloud fraction) for LW_CLOUD==LW_CLOUD_DEARDORFF

	Files Affected:

	calc_longwave.c
	mtclim_vic.c

	Description:

	Previous versions of VIC (before 4.1.2) used a full-sky longwave
	formulation taken from two formulas in the Bras hydrology text.  For
	the new Deardorff full-sky longwave formulation, the dependence on
	cloud fraction is different from the old Bras formulation.   In 
	4.1.2 (and 4.1.2.a-b), the new Deardorff formulation did not account
	for the possibility of user-supplied cloud fraction; if the user
	supplied cloud fraction as an input forcing, the resulting longwave
	was wrong.  This has been fixed.




Changed default settings of MTCLIM_SWE_CORR and LW_TYPE to reflect best
general settings

	Files Affected:

	initialize_global.c
	global.param.sample

	Description:

	In light of the findings of Bohn et al. (2012), we have changed the
	default setting of MTCLIM_SWE_CORR to FALSE and of LW_TYPE to
	LW_PRATA.  These settings give forcing estimates that are less biased
	in general.




Vapor pressure set to 0 if user supplies (QAIR or REL_HUMID) + PRESSURE as
input forcings instead of vapor pressure.

	Files Affected:

	initialize_atmos.c

	Description:

	For the cases of the combination of (QAIR or REL_HUMID) plus PRESSURE
	supplied as input forcings instead of VP, VIC was supposed to compute
	VP from (QAIR or REL_HUMID) and PRESSURE, then transfer the computed
	VP to the atmos data structure.  This transfer was being skipped, and
	vapor pressure was consequently set to 0 during the simulation.  This
	has been fixed.




Computed longwave sometimes is extremely large at high latitudes.

	Files Affected:

	mtclim_vic.c

	Description:

	Previously (VIC 4.1.2, 4.1.2.a, and 4.1.2.b only), when SHORTWAVE and
	VP were supplied to VIC as input forcings (and LONGWAVE was NOT
	supplied as a forcing), the incoming longwave radiation computed by
	VIC would in rare cases become extremely large.  This happens only at
	high latitudes in winter when the theoretical clear-sky potential
	solar radiation is very small.  If the supplied VP was large enough,
	it could cause the internal variable t_tmax (clear-sky transmittance)
	to go negative.  This in turn would cause the internal variable
	t_fmax (cloud transmittance) to go negative as well.  This, finally,
	would cause computed LONGWAVE values to become extremely large, if
	the LW_CLOUD method was set to DEARDORFF.  This has been fixed.




--------------------------------------------------------------------------------
***** Description of changes from VIC 4.1.2.b to VIC 4.1.2.a *****
--------------------------------------------------------------------------------


Bug Fixes:
----------

VIC was unnecessarily requiring WIND to be supplied as an input forcing.

	Files Affected:

	initialize_atmos.c
	vicNl_def.h

	Description:

	VIC was requiring WIND (or the zonal and meridional components of
	wind, WIND_E and WIND_N) to be supplied as an input forcing.  Now VIC
	allows WIND to be omitted.  If WIND is not supplied as an input
	forcing, VIC will supply a default wind speed, defined in vicNl_def.h
	as DEFAULT_WIND_SPEED and currently set to 3.0 m/s.




Cloud fraction tskc was not accounting for the case in which observed incoming
shortwave is supplied as a forcing.

	Files Affected:

	mtclim_vic.c

	Description:

	In the absence of observations, VIC's estimate of cloud fraction, tskc,
	is a function of some intermediate quantities that are computed within
	the MTCLIM algorithm (in mtclim_vic.c).  These intermediate terms
	can be computed from either observed daily shortwave radiation (if
	available) or simulated daily shortwave radiation.  The computation
	of tskc was previously taking place in a part of the code where
	only the simulated daily shortwave radiation was available.  Thus,
	tskc would not reflect the actual amount of incident shortwave, even
	if observed incident shortwave was supplied as a forcing.

	This has been fixed.  The tskc computation has been moved to another
	location in the code where the observed daily shortwave can be
	accessed (if supplied by the user as a forcing).




--------------------------------------------------------------------------------
***** Description of changes from VIC 4.1.2.a to VIC 4.1.2 *****
--------------------------------------------------------------------------------


Bug Fixes:
----------

Incorrect timing of disaggregated radiation and air temperature when daily
forcings are supplied and off_gmt is 0.

	Files Affected:

	initialize_atmos.c

	Description:

	VIC was double-shifting the timeseries of subdaily radiation and
	temperature in cases in which VIC was *not* given sub-daily incoming
	shortwave as an input forcing *and* soil parameter "off_gmt" was not
	set to the local time zone offset (i.e. not set to longitude*24/360).
	This caused incorrect timing of the diurnal cycle of radiation and
	air temperature.

	This has been fixed.




Disaggregated radiation is constant throughout the day when daily incoming
shortwave radiation is supplied as an input forcing.

	Files Affected:

	initialize_atmos.c
	mtclim_constants_vic.h
	mtclim_vic.c
	mtclim_wrapper.c

	Description:

	When VIC was supplied with *daily* incoming shortwave as an input
	forcing variable, VIC would fail to disaggregate this correctly to
	sub-daily time steps; it would simply repeat the daily average for
	every sub-daily time step.  This has been fixed.  Now VIC will
	compute a diurnal cycle whose daily average matches the supplied daily
	average.




--------------------------------------------------------------------------------
***** Description of changes from VIC 4.1.2 to VIC 4.1.1 *****
--------------------------------------------------------------------------------


New Features:
-------------

Cleaned up the sample global parameter file

	Files Affected:

	get_global_param.c
	global.param.sample
	initialize_global.c

	Description:

	The large number of options available in VIC 4.1.2 has begun
	to clutter the global parameter file, making it difficult for
	users to determine which options need to be set.

	Now the sample global parameter file groups the various options
	into several categories, making a clear distinction between
	those options that need to be changed by the user for each
	simulation, and those that rarely need to be changed.  Those
	options that rarely need to be changed are commented out, and VIC
	now sets those options to the most commonly-used default values.




Removal of obsolete model options

        Files Affected:

        arno_evap.c
        canopy_evap.c
        close_files.c
        cmd_proc.c
        compute_zwt.c
        display_current_settings.c
        dist_prec.c
        frozen_soil.c
        full_energy.c
        func_canopy_energy_bal.c
        func_surf_energy_bal.c
        get_global_param.c
        global.h
        global.param.sample
        initialize_global.c
        initialize_lake.c
        initialize_model_state.c
        initialize_snow.c
        Makefile
        open_debug.c (removed)
        put_data.c
        read_lakeparam.c
        read_soilparam_arc.c
        read_soilparam.c
        read_veglib.c
        read_vegparam.c
        runoff.c
        store_moisture_for_debug.c (removed)
        surface_fluxes.c
        user_def.h
        vicerror.c
        vicNl.c
        vicNl_def.h
        vicNl.h
        write_atmosdata.c (removed)
        write_data.c
        write_debug.c (removed)

        Description:

	We have removed some of the available model simulation options, due
	to their being unrealistic or rarely used.  These include:

	  GF_FULL value of GRND_FLUX_TYPE option.  This setting double-counted
	    the effect of canopy attenuation of radiation.
	  AR_COMBO value of AERO_RESIST_CANSNOW option.  This setting double-
	    counted the effects of canopy snow on aerodynamic resistance.
	  LINK_DEBUG option.  This user_def.h option would print out values of
	    various water balance terms for debugging purposes, but was rarely
	    used and difficult to maintain.




Overhaul of meteorological forcing processing

        Files Affected:

        alloc_atmos.c
        CalcBlowingSnow.c
	calc_air_temperature.c
	calc_longwave.c
	display_current_settings.c
        get_force_type.c
        get_global_param.c
        global.param.sample
        initialize_atmos.c
        initialize_global.c
        Makefile
        mtclim_constants_vic.h (was mtclim42_vic.h)
        mtclim_parameters_vic.h (new file)
        mtclim_wrapper.c (was mtclim42_wrapper.c)
        mtclim_vic.c (was mtclim42_vic.c)
        output_list_utils.c
        put_data.c
        read_atmos_data.c
        read_soilparam.c
        vicNl_def.h
        vicNl.h
        write_forcing_file.c

        Description:

	Numerous bug fixes and improvements to VIC's meteorological forcing
	processing.

	Bug fixes:

	Two main bugs fixed, both primarily affecting high-latitude grid
	cells:

	1. On days having only 1 hour of darkness, the diurnal cycle
	   of temperature was incorrect, skipping that day's minimum
	   temperature and smoothly transitioning to the next day's
	   maximum.

	2. The seasonal evolution of the diurnal cycle of solar radiation
	   was always 366 days long, so that the number of daylight hours got
	   increasingly out of phase with the time of year (by one day per
	   non-leap year) as the simulation progressed.  After 20-30 years,
	   the diurnal cycle was substantially different from what it should
	   be.  Above 60 N, the number of days having no sunlight was
	   substantially longer than it should be.

	Also fixed inconsistencies in the timing of meteorological variables
	when sub-daily met variables were supplied by the user.  Now, we have
	adopted the following convention:
          daily supplied forcings are assumed to start/end at midnight in
	    local time; for these forcings, off_gmt in the soil parameter file
	    is ignored
	  sub-daily supplied forcings are assumed to occur relative to the time
	    zone indicated by off_gmt in the soil parameter file

	New features:

        Updated VIC's internal version of the MTCLIM forcing disaggregation
        functions from version 4.2 (Thornton and Running, 1999) to include
	elements of version 4.3 (Thornton et al 2000).  

        This update includes 6 main changes to the forcing estimation scheme:

        1. Optional correction of downward shortwave for the effect of snow.
	   In the presence of snow, incoming shortwave radiation has been
	   observed to be higher than MTCLIM 4.2's estimates, due to
	   scattering of light from the snow back into the atmosphere (and
	   ultimately back to the ground again).  This correction is optional
	   and is controlled by the new global parameter option MTCLIM_SWE_CORR.
	   If this is set to TRUE, the correction is performed when snow is
	   present on the ground (estimated by a simple degree-day model in
	   the forcing pre-processor).  If FALSE, the correction is not
	   performed (and shortwave is estimated as in previous versions of
	   VIC).  The default value of MTCLIM_SWE_CORR is TRUE.
	   
        2. Optional changes in the iteration between shortwave and VP
	   estimates.  The algorithm for estimating shortwave requires
	   observed VP as an input; the algorithm for estimating VP requires
	   observed shortwave as an input.  Therefore, when neither quantity is
	   observed, MTCLIM 4.2 (and previous versions of VIC) used the
	   Tdew = Tmin approximation to supply the shortwave estimate with
	   an estimate of daily vapor pressure.  Then the resulting shortwave
	   estimate was used to compute a more accurate estimate of Tdew and
	   vapor pressure.  MTCLIM 4.3 imposes an annual aridity criterion for
	   performing this final step; for humid environments (annual PET <
	   2.5 * annual precip) the scheme simply assumes daily Tdew = daily
	   Tmin.  We have found that this behavior introduces a strong
	   positive bias into vapor pressure, and have chosen to make it
	   optional.  Therefore, we have introduced the new global
	   parameter option VP_ITER to control the shortwave-vp iteration.  It
	   can take on the following values:

	     VP_ITER_NEVER = never perform the final update of Tdew and VP
	     VP_ITER_ALWAYS = always perform the final update of Tdew and VP
	     VP_ITER_ANNUAL = use the annual aridity criterion (annual PET <
                              2.5 * annual precip) do decide whether to perform
	                      the final update
	     VP_ITER_CONVERGE = continue iterating between shortwave and vp
	                        until the two values stabilize (this tends
	                        to give almost identical results to
	                        VP_ITER_ALWAYS)

	   The default value of VP_ITER is VP_ITER_ALWAYS.

	3. To make the sub-daily values of VP more accurate, we have introduced
	   another global parameter option: VP_INTERP.  If set to TRUE, VIC will
	   assign the daily VP value computed by the MTCLIM functions to the
	   time of sunrise on that day, and interpolate linearly between sunrise
	   vapor pressure values of adjacent days.  If set to FALSE, VIC will
	   hold vapor pressure constant over the entire day as in previous
	   versions.  The default value of VP_INTERP is TRUE.

	4. Alternative clear-sky longwave algorithms.  We have introduced a
	   new global parameter option: LW_TYPE.  This option determines which
	   algorithm is used to compute clear-sky longwave radiation.
	   Choices are:

	     LW_TVA = algorithm of Tennessee Valley Authority (TVA, 1972)
		(This is what all previous versions of VIC have used.  Our tests
		indicate that this algorithm is still the best when observed
		cloud fractions are unavailable and are estimated by MTCLIM,
		which is the current situation for VIC.)
	     LW_ANDERSON = algorithm of Anderson (1964)
	     LW_BRUTSAERT = algorithm of Brutseart (1975)
	     LW_SATTERLUND = algorithm of Satterlund (1979)
	     LW_IDSO = algorithm of Idso (1981)
	     LW_PRATA = algorithm of Prata (1996) (Our tests indicate that
		this algorithm is best when observed cloud fractions are
		supplied as a forcing.)

	   Default is set to LW_TVA.

	5. Alternative cloud longwave emission algorithms.  We have introduced
	   a new global parameter option: LW_CLOUD.  This option
	   determines which algorithm is used to simulate the effect of
	   cloudiness on incoming longwave radiation.  Choices are:

	     LW_CLOUD_BRAS = algorithm composed of equations 2.29 and 2.43
		from the Bras textbook (Bras, R. L., "Hydrology, an
		introduction to hydrologic science", Addison-Wesley, Reading,
		Massachusetts, 1990).  This was the algorithm used in all
		previous releases of VIC. (Our tests indicate that this
		algorithm introduces substantial temperature-dependent biases
		in longwave estimates outside of the temperate zone)
	     LW_CLOUD_DEARDORFF = algorithm used in Deardorff (1978) in which
		cloud_fraction is assumed equal to
			(1 - actual_shortwave/theoretical_clear_sky_shortwave)
		and total sky emissivity is represented as the weighted average:
			cloud_fraction * cloud_emissivity
			+ (1-cloud_fraction) * clear_sky_emissivity
		(Our tests indicate that this algorithm is superior)

	   Default is set to LW_CLOUD_DEARDORFF.

	6. Optional threshold for daily precipitation to cause dimming of
	   incoming shortwave radiation.  Previously, any precipitation would
	   cause estimated daily incoming shortwave radiation to dim by 25%.
	   Because the smoothing/resampling methods used in creating gridded
	   forcings can artificially "bleed" trace amounts of precipitation
	   into neighboring grid cells, we introduced this threshold to
	   counteract any resulting low biases in shortwave.  This is
	   controlled by the global parameter file option SW_PREC_THRESH,
	   which is the minimum amount of daily precipitation (in mm) that
	   must be exceeded for shortwave to dim.  The default value is set to
	   0.1 mm.




Added the ability to simulate organic soil.

	Files Affected:

	display_current_settings.c
	frozen_soil.c
	full_energy.c
	func_surf_energy_bal.c
	get_global_param.c
	global.param.sample
	initialize_global.c
	initialize_model_state.c
	lakes.eb.c
	prepare_full_energy.c
	read_soilparam_arc.c
	read_soilparam.c
	runoff.c
	soil_conduction.c
	vicNl_def.h
	vicNl.h

	Description:

	Added logic to handle organic soil properties.  Added a new global
	parameter file option, ORGANIC_FRACT. If ORGANIC_FRACT is TRUE, VIC
	assumes that there are 3*Nlayer extra columns in the soil parameter
	file, containing for each layer the organic fraction and bulk and soil
	densities of the organic matter.  If ORGANIC_FRACT is FALSE or is
	omitted, VIC assumes that the soil param file does not contain these
	extra columns, and assumes that the soil contains 0 organic matter (as
	in previous versions of VIC).

	The organic fraction in the soil parameter file is the fraction of
	soil (by volume) that is organic matter.  If this fraction is > 0,
	then the quartz content and the original bulk and soil densities are
	assumed to apply only to the mineral portion of the soil.  Internally,
	VIC will compute the aggregate bulk and soil densities of the combined
	mineral and organic portions of the soil and compute an aggregate soil
	porosity.

	VIC now uses equations taken from Farouki 1986 to compute the thermal
	conductivity and heat capacity of the soil containing organic matter.
	As organic fraction approaches 0, the soil properties approach those
	of the mineral portion of the soil.




Extended computation of soil temperatures and ground heat flux to all modes of
operation.


	Files Affected:

	calc_soil_thermal_eqn.c
	calc_surf_energy_bal.c
	display_current_settings.c
	frozen_soil.c
	full_energy.c
	func_surf_energy_bal.c
	get_global_param.c
	initialize_global.c
	initialize_model_state.c
	soil_conduction.c
	surface_fluxes.c
	vicNl_def.h
	vicNl.h

	Description:

	Extended computation of soil temperatures and ground heat flux to all
	modes of model operation.  Previously these quantities were only
	computed when FROZEN_SOIL was TRUE.

	Now soil temperatures, ice content, and ground heat flux are computed
	as follows:

	1. If FROZEN_SOIL is TRUE, there is no change to previous model
		behavior.  By default, soil temperature profile is computed via
		the finite element method of Cherkauer and Lettenmaier (1999),
		taking account of the phase change of ice.  (To use the Liang
		et al (1999) approximation, set QUICK_FLUX to TRUE; however
		this is not recommended because it doesn't handle the phase
		change of ice.)  If FULL_ENERGY is also TRUE, the surface
		temperature is computed via iteration over the surface energy
		balance; if FULL_ENERGY is FALSE, the surface temperature is
		set to air temperature.  Soil layer average temperatures and
		ice contents are computed and available for output.

	2. If FROZEN_SOIL is FALSE, the ice phase change is not accounted for
		and soil layer ice contents are all set to 0.  Unlike previous
		versions, the soil layer temperatures and ice contents (=0)
		are now available for output.  The default method of computing
		soil temperature profile depends on the setting of the
		QUICK_FLUX option:
		a. QUICK_FLUX FALSE: use approximation of Liang et al (1999).
		   This is the default method, if QUICK_FLUX is not specified
		   in the global param file.
		b. QUICK_FLUX TRUE: use finite element method of Cherkauer et
		   al. (1999).

	Now that soil temperatures, ice contents, and ground heat fluxes are
	always computed, the GRND_FLUX option has been removed.




Added computation of water table position.


	Files Affected:

	compute_zwt.c (new)
	initialize_lake.c
	initialize_soil.c
	lakes.eb.c
	Makefile
	output_list_utils.c
	put_data.c
	read_soilparam.c
	read_soilparam_arc.c
	runoff.c
	user_def.h
	vicNl_def.h
	vicNl.c
	vicNl.h

	Description:

	Added computation of the water table position, "zwt".  Units are [cm]
	and the position is negative below the soil surface.  To monitor
	the water table position, we have added the following output variables:

	  OUT_ZWT  - water table position [cm] computed by requiring that all
		     layers below it are saturated, i.e. the water table will
		     appear only in the lowest unsaturated layer.
	  OUT_ZWT_LUMPED - water table position [cm] computed by combining all soil
		     layers into a single layer and assuming the soil below
		     is completely saturated.

	The water table's position within a soil layer is computed from soil
	water retention curves following Letts et al. (2000):

	  w(z) = { ((zwt-z)/bubble)**(-1/b), z <  zwt-bubble
	         { 1.0,                      z >= zwt-bubble
	where
	  z      = depth below surface [cm]
	  w(z)   = relative moisture at depth z given by
	           (moist(z) - resid_moist) / (max_moist - resid_moist)
	  zwt    = depth of water table below surface [cm]
	  bubble = bubbling pressure [cm]
	  b      = 0.5*(expt-3)
	Note that zwt-bubble = depth of the free water surface, i.e.
	position below which soil is completely saturated.

	This assumes water in unsaturated zone above water table
	is always in equilibrium between gravitational and matric
	tension (e.g., Frolking et al, 2002).

	So, to find the soil moisture value in a layer corresponding
	to a given water table depth zwt, we integrate w(z) over the
	whole layer:

	w_avg = average w over whole layer = (integral of w*dz) / layer depth

	Then,
	  layer moisture = w_avg * (max_moist - resid_moist) + resid_moist

	Instead of the zwt defined above, we actually report free
	water surface elevation zwt* = -(zwt-bubble).  I.e. zwt* < 0
	below the soil surface, and marks the point of saturation
	rather than pressure = 1 atm.

	To store the (zwt,moisture) pairs in this relationship for each layer,
	two new variables have been added to the soil_con_struct:
	  zwtvmoist_zwt[MAX_LAYER][MAX_ZWTVMOIST]
	  zwtvmoist_moist[MAX_LAYER][MAX_ZWTVMOIST]
	These are computed in read_soilparam().  Then, in compute_zwt() VIC
	interpolates between these points to estimate the water table depth
	in each layer given the layer's current soil moisture.




Added channel inflow from upstream into the lake to the list of forcing
variables.  Added lake water balance terms to input/output variables.

	Files Affected:

	alloc_atmos.c
	full_energy.c
	get_force_type.c
	initialize_atmos.c
	initialize_lake.c
	initialize_model_state.c
	LAKE.h
	lakes.eb.c
	output_list_utils.c
	output.PILPS-2E.ALMA.template
	output.TRADITIONAL.410.template
	set_output_defaults.c
	put_data.c
	read_lakeparam.c
	vicNl.c
	vicNl_def.h
	vicNl.h

	Description:

	Added forcing variable CHANNEL_IN to allow channel flow from
	upstream grid cells to be an input into the lake.  This variable
	is ignored if LAKES is set to FALSE or the lake/wetland tile has 0
	area.  CHANNEL_IN must be in units of cubic meters [m3] per forcing
	time step (or cubic meters per second [m3/s] if ALMA_INPUT is TRUE).

	Several output variables have been added to aid in tracking the
	lake water budget.  The complete set of lake water budget variables
	is (in mm over grid cell area):

	  OUT_LAKE_BF_IN	baseflow into lake from catchment
	  OUT_LAKE_BF_OUT	baseflow out of lake
	  OUT_LAKE_CHAN_IN	channel inflow into lake
	  OUT_LAKE_CHAN_OUT	channel outflow from lake
	  OUT_LAKE_DSTOR  	change in lake moisture storage (liquid plus
					ice cover)
	  OUT_LAKE_DSWE  	change in snowpack on top of lake ice
	  OUT_LAKE_EVAP		(was OUT_EVAP_LAKE)
	  OUT_PREC		(already existed)
	  OUT_LAKE_RCHRG	recharge from lake to wetland
	  OUT_LAKE_RO_IN	runoff into lake from catchment
	  OUT_LAKE_VAPFLX	sublimation from lake snow
	These same variables exist in units of volume (m**3):
	  OUT_LAKE_BF_IN_V	baseflow into lake from catchment
	  OUT_LAKE_BF_OUT_V	baseflow out of lake
	  OUT_LAKE_CHAN_IN_V	channel inflow into lake
	  OUT_LAKE_CHAN_OUT_V	channel outflow from lake
	  OUT_LAKE_DSTOR_V	change in lake moisture storage (liquid plus
					ice cover)
	  OUT_LAKE_DSWE_V	change in snowpack on top of lake ice
	  OUT_LAKE_EVAP_V	evap from lake
	  OUT_LAKE_PREC_V	(note the "LAKE" in the name)
	  OUT_LAKE_RCHRG_V	recharge from lake to wetland
	  OUT_LAKE_RO_IN_V	runoff into lake from catchment
	  OUT_LAKE_VAPFLX_V	sublimation from lake snow

	This change also involved changing how lake fluxes are represented
	internally in VIC.  They are now represented as volumes (m3) per step.
	This allows better handling of cases in which the lake shrinks to 0
	area.




Added the output variable OUT_LAKE_AREA_FRAC, the lake surface area as a
fraction of the grid cell area.

	Files Affected:

	output_list_utils.c
	put_data.c
	vicNl_def.h

	Description:

	Added the output variable OUT_LAKE_AREA_FRAC, which tracks the lake
	surface area as a fraction of the grid cell area.  This allows quick
	comparison of VIC outputs with lake area observations (e.g. from
	remote sensing).




Added validation of b_infilt

	Files Affected:

	read_soilparam.c

	Description:

	VIC now checks to make sure b_infilt > 0.  If the value is <= 0,
	VIC exits with an error message.




Removed MIN_LIQ option.

	Files Affected:

	arno_evap.c
	frozen_soil.c
	get_global_param.c
	global.param.sample
	initialize_global.c
	initialize_lake.c
	initialize_model_state.c
	initialize_soil.c
	lakes.eb.c
	runoff.c
	soil_conduction.c
	vicNl_def.h
	vicNl.h

	Description:

	This option was intended to allow a dynamic (temperature-dependent)
	lower bound on liquid soil moisture, based on the principle that, even
	at very low temperatures, some moisture remains unfrozen.  However,
	it ended up being unnecessary and its implementation introduced
	undesirable complexity into the code.  In addition, it contained a bug
	that allowed soil moisture to fall below residual moisture level.
	Therefore this feature has been removed.




Bug Fixes:
----------

Users can inadvertently choose an unstable soil temperature scheme

	Files Affected:

	get_global_param.c
	initialize_global.c
	initialize_model_state.c
	newt_raph_func_fast.c

	Description:

	Previously VIC's default soil temperature scheme when
	FROZEN_SOIL=TRUE was an explicit scheme whose stability
	was not guaranteed.  VIC has had an optional implicit
	scheme but this was not turned on unless the user chose
	it.  This led to users inadvertently running VIC with
	an unstable temperature scheme under certain combinations
	of model time step and thermal node spacing.

	This has been fixed.  Now the implicit scheme is turned
	on by default (IMPLICIT=TRUE by default when FROZEN_SOIL
	is TRUE).  The implicit scheme is stable for all combinations
	of model time step and thermal node spacing.  Because this
	can slow down VIC simulations in some cases, this option
	can still be overridden by setting IMPLICIT to FALSE in the
	global parameter file.  However, if the user does this,
	VIC now checks to see if the simulation's model time step
	and thermal node spacing are in the stable region; if not,
	VIC aborts with an error message.




Clarified description and variable names in SPATIAL_SNOW option

	Files Affected:

	calc_snow_coverage.c
	calc_surf_energy_bal.c
	ice_melt.c
	initialize_lake.c
	initialize_snow.c
	read_soilparam_arc.c
	read_soilparam.c
	solve_snow.c
	vicNl_def.h
	write_snow_data.c

	Description:

	The parameter for the snow pack depth distribution was previously
	named "depth_full_snow_cover".  However, this parameter's function
	was actually to represent the slope of the snow pack depth
	distribution.  If we define "depth_thresh" to be the minimum snow
	pack depth below which the coverage < 1, then
	  depth_full_snow_cover MUST = 2*depth_thresh
	This was very misleading and caused users some confusion.  Therefore
	we have changed the name of the parameter to "max_snow_distrib_slope".
	In addition, other associated variables in the snow_data structure
	have been renamed accordingly (max_swq became max_snow_depth and
	swq_slope became snow_distrib_slope).




Fixed incorrect diurnal radiation cycle at high latitudes

	Files Affected:

	See "Overhaul of meteorological forcing processing" under
	new features

	Description:

	See "Overhaul of meteorological forcing processing" under
	new features.  Two main bugs fixed, both primarily affecting
	high-latitude grid cells:
	  1. On days having only 1 hour of darkness, the diurnal cycle
	     of temperature was incorrect, skipping that day's minimum
	     temperature and smoothly transitioning to the next day's
	     maximum.
	  2. The seasonal evolution of the diurnal cycle of solar radiation
	     was always 366 days long, so that the number of daylight hours got
	     increasingly out of phase with the time of year (by one day per
	     non-leap year) as the simulation progressed.  After 20-30 years,
	     the diurnal cycle was substantially different from what it should
	     be.  Above 60 N, the number of days having no sunlight was
	     substantially longer than it should be.




Fixed incorrect computation of snow cover fraction over lake/wetland

	Files Affected:

	lakes.eb.c

	Description:

	Previously VIC didn't compute snow cover fraction correctly
	over the lake/wetland tile.  This has been fixed.




Fixed VIC's inability to handle the case where some cells have lakes and
others don't.

	Files Affected:

	full_energy.c
	read_initial_model_state.c
	read_lakeparam.c
	vicNl_def.h
	write_model_state.c

	Description:

	VIC 4.1.1 and previous versions of 4.1.2 were unable to handle the
	case in which some cells contain lakes and others don't.  If
	options.LAKES was TRUE, all cells were required to have a valid
	value of lake_idx to indicate which veg tile contained a lake,
	even if no lakes existed in the cell.

	This has been fixed.  Now, users can specify a cell that has no
	lakes by setting lake_idx to -1 in the lake parameter file.  In
	this case, VIC will ignore all other lake parameters for the cell
	and set the cell's lake area permanently to 0.




Fixed bugs in declarations of frost_fract for SPATIAL_FROST TRUE

	Files Affected:

	lakes.eb.c
	put_data.c
	vicNl.h

	Description:

	Declarations for frost_fract were wrong or missing.  This has
	been fixed.        




Fixed various lake water balance errors.

	Files Affected:

	initialize_lake.c
	initialize_model_state.c
	initialize_soil.c
	LAKE.h
	lakes.eb.c
	runoff.c
	vicNl.h

	Description:

	Now calls to get_depth() are all consistent, using *liquid* water
	volume instead of total volume (thus ldepth is always the liquid water
	depth).  Lake ice area is not allowed to increase if ice area > liquid
	area.  Ice cover is now treated like a "skin" on top of the liquid
	(regardless of its thickness).  Ice is assumed to be completely buoyant,
	i.e. none of the ice is below the water line.  If ice completely covers
	the lake and the liquid part of the lake shrinks, the ice is assumed to
	bend like a blanket, so that the outer edges now rest on land but the
	center rests on top of the liquid.  The lake "area" is still considered
	to be the ice cover in this case.  Baseflow out of the lake is only
	allowed in the area of the lake containing liquid.  The edges of the ice
	sheet are assumed to be vertical, i.e. the surface area of the top of the
	ice sheet is equal to the surface area of the bottom of the ice sheet.
	This fixes water balance errors arising from inconsistent estimates of
	lake ice area.  Also fixed bugs in computation of lake->recharge.                




Fixed incorrect snow albedo decay for southern hemisphere.

	Files Affected:

	LAKE.h
	lakes.eb.c
	solve_snow.c

	Description:

	Previously the computation of the USACE snow albedo decay would
	only switch from accumulation to melt curves for dates between
	March 1 and October 1.  These dates are only appropriate for the
	northern hemisphere.  This has been fixed so that the reverse
	condition must be met in the southern hemisphere.




Saturated area fraction incorrect for SPATIAL_FROST = TRUE.

	Files Affected:

	runoff.c

	Description:

	Previously the computation of cell.Asat occurred in the wrong
	place in runoff.c so that it was only correct for SPATIAL_FROST
	= FALSE.  This has been fixed.




State file handles are never closed.

	Files Affected:

	vicNl.c

	Description:

	Previously VIC was not closing the intial and final state files.
	This has been fixed.




VIC aborts with soil temperature node moisture > max moisture.

	Files Affected:

	soil_conduction.c

	Description:

	Previously VIC would abort the simulation if soil temperature node
	moisture ever became greater than the maximum allowable moisture.
	This behavior has been replaced with a resetting of node moisture to
	the maximum.  This is not intended to be a permanent fix; it simply
	allows simulations to continue with minimal errors.  A more robust
	fix is planned.




Initial lake depth < mindepth is reset to mindepth.

	Files Affected:

	read_lakeparam.c

	Description:

	The validation of initial lake depth was incorrectly resetting depth
	to mindepth if initial depth < mindepth.  This has been fixed.




Soil moisture falls below residual moisture level in some cases.

	Files Affected:

	(see "Removed MIN_LIQ option" under "New Features")

	Description:

	The MIN_LIQ option contained a bug that allowed soil moisture to fall
	below residual moisture level in some cases.  This feature has been
	removed (see "Removed MIN_LIQ option" under "New Features").




Incorrect calculation of grnd_flux, deltaH, and fusion for EXP_TRANS=TRUE.

	Files Affected:

	arno_evap.c
	calc_surf_energy_bal.c
	frozen_soil.c
	func_surf_energy_bal.c
	get_global_param.c
	initialize_model_state.c
	modify_Ksat.c
	solve_snow.c
	surface_fluxes.c
	vicNl.h

	Description:

	In 4.1.1, the calculation of grnd_flux, deltaH, and fusion involved
	the assumption that soil thermal nodes 1 and 2 were at depths of
	1*soil_con->depth[0] and 2*soil_con->depth[0], respectively.  This is
	not true for exponential node spacing (EXP_TRANS=TRUE) or any other
	node spacing scheme in general.  This has been fixed to allow
	aribitrary node spacing.

	The temperature profile given by the exponential node spacing is now
	interpolated to find the temperatures at the same depths as for the
	linear node spacing, i.e.  the boundary between the first and second
	soil hydrologic layers.  This makes the computation of grnd_flux,
	deltaH, and fusion consistent across all modes of operation (linear
	and exponential node spacing AND the QUICK_FLUX approximation), since
	now they all apply to the same control volume (first soil layer).



Simulation log messages appear out of order.

	Files Affected:

	display_current_settings.c
	put_data.c

	Description:

	In 4.1.1, some messages sent to the screen would appear out of order
	when saved to a log file.  This has been fixed by redirecting messages
	from stdout to stderr.



Fixed typo in setting of fallback T value for the soil T profile.

	Files Affected:

	frozen_soil.c

	Description:

	In 4.1.1, the fallback temperature was erroneously set to "oldT",
	which is the T value of the previous iteration, but not the value
	from the beginning of the time step (except on the first iteration).
	Now, "oldT" has been replaced by "T0".



Fixed typo in initialization of fallback counts for the soil T profile.

	Files Affected:

	frozen_soil.c

	Description:

	In 4.1.1, only the fallback counts for nodes 1 to Nnodes-2 were
	initialized (to 0).  Now initialization is done for nodes 0 to
	Nnodes-1.



Fixed various water balance errors in the lake model and added logic to handle
the case when lake area goes to 0.

	Files Affected:

	full_energy.c
	lakes.eb.c
	put_data.c
	vicNl_def.h

	Description:

	In 4.1.1, the lake model would generate small water balance errors
	when lake area would change, particularly in the presence of lake ice.
	In addition, lake water balance terms would become NaN when lake
	fraction went to 0.  These errors have been fixed.



Replaced "assert" statements with "if" statements.

	Files Affected:

	calc_atmos_energy_bal.c
	calc_surf_energy_bal.c
	ice_melt.c
	snow_intercept.c
	snow_melt.c
	water_energy_balance.c
	water_under_ice.c

	Description:

	Previous versions of these files contain "assert" statements, which
	cause the model to abort if the assertions are not true.  These have
	been replaced by "if" statements, so that the offending conditions can
	be remedied and the simulation can continue.



Removed save_data structure initialization from initialize_model_state().

	Files Affected:

	initialize_model_state.c
	put_data.c
	vicNl.c
	vicNl.h

	Description:

	For each time step, the initial values of the major moisture storage
	terms are stored in the save_data structure, so that the difference
	between final and initial states can be computed and written to
	output.  In earlier versions, these variables needed to be initialized
	in initialize_model_state() so that they contained valid values before
	the first call to put_data().  This is no longer necessary, as the
	model now initializes all output variables with an initial call to
	put_data() before the simulation begins.  Thus, the initialization
	has been removed from initialize_model_state().



Fixed typo in condition for reading Wdew values.

	Files Affected:

	read_initial_model_state.c

	Description:

	In 4.1.1, Wdew values were expected for even the bare soil veg tile,
	despite the fact that these values are not saved for bare soil in the
	function write_model_state().  This has been fixed; Wdew is no longer
	expected for the bare soil tile.



Fixed bug in runoff computation when soil column is completely saturated.

	Files Affected:

	runoff.c

	Description:

	runoff() first computes total runoff for the timestep, and then computes
	hourly vertical water movement through the soil column.  During the
	hourly computations, if the entire soil column becomes saturated, some
	downward-moving water may be "regurgitated" back up to the surface,
	where it is added to the total runoff for the time step.

	In 4.1.1, the hourly infiltration into the soil column is computed
	using runoff[frost_area].  However, in the case of complete saturation,
	the regurgitated excess water is added to runoff[frost_area] within
	the hourly loop, so that the next hour's infiltration is computed
	using a larger value of runoff[frost_area].  This leads to water
	balance errors.  This has been fixed in 4.1.2.  Now, hourly
	infiltration is only computed relative to the original estimate of
	runoff[frost_area], before any regurgitated excess water is added to
	it.



Fixed typo in writing of number of lake active nodes to ASCII-format state file.

	Files Affected:

	write_model_state.c

	Description:

	Fixed a typo in the writing of the number of lake active nodes to the
	statefile, for ASCII format.  The error involved omitting a space that
	would separate the active node number from the temperature of the
	deepest soil thermal node, preventing the state file from being read
	correctly in subsequent VIC runs.



Added TFALLBACK logic to soil thermal profile solution for case in which max
iterations are exceeded.

	Files Affected:

	frozen_soil.c

	Description:

	In 4.1.1, errors due to exceeding the maximum number of iterations in
	the soil temperature profile computation in calc_soil_thermal_fluxes()
	were not handled by the TFALLBACK option.  Logic to handle this case
	has now been added.



Fixed bugs in initialization of various TFallback counts and flags.

	Files Affected:

	initialize_snow.c
	frozen_soil.c

	Description:

	In 4.1.1, the number of Tfallbacks reported to the screen sometimes
	came out negative; this has been fixed for fallbacks in snow surface
	temperature and soil temperature profile.



Misc fixes to handling of aerodynamic resistance.

	Files Affected:

	func_surf_energy_bal.c
	surface_fluxes.c

	Description:

	Replaced ra_under with Ra_used[0] in func_surf_energy_bal.c - this
	should not change behavior; it simply cleans up the code a tad.

	Added logic to surface_fluxes.c to handle cases when aero_cond and
	aero_resist are 0 or very large.



Added hack to prevent runaway cold nose in soil temperature profile.  This is
only enabled when options.TFALLBACK = TRUE.

	Files Affected:

	frozen_soil.c

	Description:

	In 4.1.1, the soil temprature computation could sometimes encounter a
	runaway "cold nose", in which one node was colder than its neighbors
	and continued to grow colder than its neighbors over time, due to
	instabilities in the soil heat equation (in the presence of ice in the
	soil).  A rigorous numerical solution to this problem has not yet been
	implemented, but this hack causes the offending node to be reset to
	the average of its neighbors' temperatures as soon as the runaway
	behavior is detected.  We hope to implement a more rigorous solution
	to this problem in future releases.



Simplified arguments lists of various functions.

	Files Affected:

	arno_evap.c
	func_surf_energy_bal.c
	initialize_atmos.c
	snow_intercept.c
	solve_snow.c
	surface_fluxes.c
	vicNl.c
	vicNl.h

	Description:

	Replaced individual forcing and date/time variables in argument
	lists of solve_snow() and snow_intercept() with the dmy and atmos
	data structures.  Removed several unused variables from the arg list
	of arno_evap().  Replaced individual soil_con variables in arg list
	of initialize_atmos() with the soil_con structure.



Replaced GLOBAL_LAI option with 2 options: VEGPARAM_LAI and LAI_SRC.

	Files Affected:

	display_current_settings.c
	get_global_param.c
	global.param.sample
	initialize_global.c
	read_veglib.c
	read_vegparam.c
	vicNl_def.h

	Description:

	In previous versions, the GLOBAL_LAI option caused some confusion.
	GLOBAL_LAI=TRUE indicated that LAI values should be read from the veg
	parameter file (usually because they vary globally), and FALSE
	indicated that LAI values should be read from the veg library (meaning
	they were constant across the globe).  But the natural interpretation
	of "GLOBAL_LAI=TRUE" is that LAI values are globally constant.  In
	addition, the presence of LAI values in the veg param file was
	dictated by the setting of GLOBAL_LAI.  If the user wanted to have VIC
	read LAI values from the veg library, GLOBAL_LAI would need to be
	FALSE, and the veg param file would need to have its LAI values
	removed.  This linkage of veg param file format and where VIC was
	looking for LAI values seemed unnecesary.

	There are now 2 options controlling these 2 separate aspects of LAI.
	VEGPARAM_LAI controls the *format* of the veg param file.  Independent
	of this, LAI_SRC controls *where* VIC looks for LAI values.  This way,
	1) the user can switch between the two sources of LAI values without
	having to change the veg param file, and 2) the meanings of these
	options are more clear than the old GLOBAL_LAI option.

	The old GLOBAL_LAI setting has been maintained for backwards-
	compatibility, i.e. if VIC encounters GLOBAL_LAI in the global
	param file, it will set VEGPARAM_LAI and LAI_SRC appropriately.



Changed default number of soil layers from 2 to 3 (the most common
configuration).

	Files Affected:

	initialize_global.c

	Description:

	The default number of soil layers has been changed from 2 to 3 (the
	most common configuration).  This will only affect users who have
	omitted this setting from their global parameter files.




--------------------------------------------------------------------------------
***** Description of changes from VIC 4.1.1 to VIC 4.1.0 beta r5 *****
--------------------------------------------------------------------------------


New Features:
-------------

Modified reporting of aerodynamic resistance in output files.

	Files Affected:

	full_energy.c
	func_canopy_energy_bal.c
	func_surf_energy_bal.c
	output_list_utils.c
	put_data.c
	surface_fluxes.c
	vicNl_def.h
	wetland_energy.c

	Description:

	Replaced the existing variables aero_resist_used and Ra_used with
	arrays of two elements each; [0] corresponds to surface values
	(bare soil or non-overstory veg; with snow or snow-free) and [1]
	corresponds to overstory or non-overstory veg.  For non-overstory
	veg, elements [0] and [1] are the same.

	Added the following output variables to store the average aerodynamic
	conductance and resistance of the grid cell corresponding to elements
	[0] and [1], respectively: OUT_AERO_COND1, OUT_AERO_COND2,
	OUT_AERO_RESIST1, and OUT_AERO_RESIST2.  Changed the computation
	of OUT_AERO_COND and OUT_AERO_RESIST to be "scene" values, i.e.
	veg tiles containing overstory contribute their overstory (element
	[1]) values, and non-overstory veg tiles contribute their surface
	(element [0]) values.  Earlier versions of VIC simply output the
	values of AERO_COND and AERO_RESIST computed latest during the time
	step (whether from surface or overstory).



Added AERO_RESIST_CANSNOW option.

	Files Affected:

	display_current_settings.c
	func_canopy_energy_bal.c
	get_global_param.c
	global.param.sample
	initialize_global.c
	vicNl_def.h

	Description:

	This option allows the user to control how aerodynamic resistances
	in the overstory are corrected for the presence of snow in the canopy.

	Possible values are:
	  AR_406: Multiply aerodynamic resistance by 10 for latent heat
		  calculation but NOT for sensible heat, and do NOT apply
		  stability correction, as in VIC 4.0.6 and earlier.
		  Additionally, use surface aero_resist for ET when no
		  no snow in canopy.
	  AR_406_LS: Multiply aerodynamic resistance by 10 for both latent
		  and sensible calculations, but do NOT apply stability
		  correction.  Additionally, use surface aero_resist for
		  ET when no no snow in canopy.
	  AR_406_FULL: Multiply aerodynamic resistance by 10 for both latent
		  and sensible calculations, but do NOT apply stability
		  correction.  Always use canopy aero_resist for ET.
	  AR_410: Apply stability correction but do not multiply by 10, as in
		  VIC 4.1.0.  Always use canopy aero_resist for ET.
	  AR_COMBO: Apply stability correction and multiply resulting resistance
		  by 10.  Always use canopy aero_resist for ET.

	VIC 4.1.0 differed from VIC 4.0.6 (and earlier) in the computation
	of aerodynamic resistances in snow-filled canopy.  This new option
	allows backwards-compatibility with both of these model versions, as
	well as the new "AR_406_LS", "406_FULL" and "AR_COMBO" options, for
	comparison.  The default is set to AR_410.



Reinstated the COMPUTE_TREELINE option, with option to supply the average July
air temperature.

	Files Affected:

	compute_treeline.c
	display_current_settings.c
	get_global_param.c
	global.param.sample
	initialize_atmos.c
	initialize_global.c
	read_snowband.c
	read_soilparam_arc.c
	read_soilparam.c
	read_vegparam.c
	user_def.h
	vicNl.c
	vicNl.h
	vicNl_def.h

	Description:

	COMPUTE_TREELINE:

	This option revives the treeline elevation computation that is a
	current feature of VIC 4.0.x.  If set to TRUE this flag will
	force the VIC model to compute the elevation of the tree line,
	based on elevation at which the average annual July air temperature
	is at or below 10C.  All snowbands above this evelation are then
	assumed to be above the treeline, and vegetation types with
	overstory are removed from the snow band average variables.  If no
	non-overstory vegetation types exist in an above-treeline band,
	the band will be treated as if it consists of a default vegetation
	type, specified by the user.

	To use this option, insert the following line into the global
	parameter file:
	  COMPUTE_TREELINE	n
	where n is the id number of the desired default vegetation type to use
	when no non-overstory vegetation types exist in the band.  To specify
	bare soil, set n to a negative number.

	To deactivate treeline calculation, change the line in the global
	parameter file to
	  COMPUTE_TREELINE FALSE
	or remove the line from the global control file.

	WARNING #1: Since the model does not store default root zone
	distributions, the default vegetation type will use the
	values from the last defined vegetation type for the current
	grid cell (i.e. veg type N-1, prior to the addition of the
	new vegetation type).

	WARNING #2: If you are using GLOBAL_LAI, than the LAI and
	dew storage terms for the default vegetation type will be set
	to the values used by the last occurrence of the default
	vegetation type.

	JULY_TAVG_SUPPLIED:

	The default behavior of VIC is to compute the average July temperature
	from the meteorological input forcings, over the time span of the
	simulation.  One drawback to this behavior is that, for short
	simulations over different periods of time, the average July
	temperature can vary enough to change which elevation bands are
	considered to be above the treeline.

	Therefore, if desired, the user can specify the average July
	temperature for each grid cell by taking the following 2 steps:
	  1. Add each grid cell's average July air temperature to the soil
	     parameter file, as the final field on each line.
	  2. Set JULY_AVGT_SUPPLIED = TRUE in the global parameter file

	The JULY_AVG_SUPPLIED option is ignored if COMPUTE_TREELINE is FALSE.


Added OUT_VPD output variable.

	Files Affected:

	output_list_utils.c
	put_data.c
	vicNl_def.h
	write_forcing_file.c

	Description:

	Added the output variable OUT_VPD to track vapor pressure deficit.


Added OUT_ASAT output variable.

	Files Affected:

	full_energy.c
	output_list_utils.c
	put_data.c
	runoff.c
	surface_fluxes.c
	vicNl_def.h
	vicNl.h
	wetland_energy.c

	Description:

	Added the output variable OUT_ASAT to track saturated area.


New MIN_LIQ option, with fixes to min_liq formulation.

	Files Affected:

	arno_evap.c
	display_current_settings.c
	frozen_soil.c
	get_global_param.c
	global.param.sample
	initialize_global.c
	initialize_model_state.c
	initialize_soil.c
	lakes.eb.c
	runoff.c
	soil_conduction.c
	vicNl_def.h
	vicNl.h

	Description:

	Made use of min_liq (instead of residual moisture) optional, via
	options.MIN_LIQ.  If MIN_LIQ is FALSE (or omitted) in global
	parameter file, all equations that depend on liquid soil moisture
	use residual moisture as their absolute minimum liquid water
	content.  If MIN_LIQ is TRUE, all of these equations use min_liq
	as their absolute minimum liquid water content.  Min_liq is
	computed in soil_conduction.c as residual moisture multiplied
	by the max_unfrozen_water content at the current temperature.

	Made min_liq formulation more consistent across the model code.
	Added min_liq to the layer_struct so that this temperature-dependent
	quantity can be computed once per time step and communicated to all
	functions that look at soil moisture.			TJB


New PLAPSE option, which lapses air pressure (and density) by grid cell average elevation.

	Files Affected:

	display_current_settings.c
	global.param.sample
	initialize_atmos.c
	initialize_global.c
	vicNl_def.h

	Description:

	Previously, when air pressure was not supplied in the input forcings,
	VIC would set it equal to a constant 95.5 kPa.  Now, if options.PLAPSE
	is TRUE, and if air pressure is not supplied, VIC computes it by
	lapsing sea level pressure by grid cell average elevation.  Air
	density is handled the same way.			TJB


New GRND_FLUX_TYPE option, with fixes to deltaH and fusion equations.

	Files Affected:

	display_current_settings.c
	func_surf_energy_bal.c
	get_global_param.c
	global.param.sample
	initialize_global.c
	surface_fluxes.c
	vicNl_def.h

	Description:

	Corrected deltaH and fusion terms of surface energy balance, by taking
	surf_atten into account, as in Liang et al 1999.  Added GRND_FLUX_TYPE
	option, to allow backwards compatibility with versions 4.0.6 and
	4.1.0.

	Possible values are:
	  GF_406: use (flawed) formulas for ground flux, deltaH, and fusion
	          from VIC 4.0.6 and earlier
	  GF_410: use formulas from VIC 4.1.0 (ground flux is correct,
	          but deltaH and fusion ignore surf_atten)
	  GF_FULL: use correct ground flux formula from VIC 4.1.0 and
	          also take surf_atten into account in deltaH and fusion
	GF_FULL is the default value.


Added validation of Campbell's expt and bubble pressure.

	Files Affected:

	read_soilparam.c

	Description:

	Previously, VIC did not check to see if "nodata" values such as -9999
	were supplied in the soil parameter file for the Campbell's expt and
	bubble pressure parameters.  These values would produce "nan" in
	computations of soil hydraulic conductivity and soil ice content.  This
	is especially insidious in the case of bubble pressure, as it is not
	needed for the water balance mode, and therefore users often set it to
	"nodata" values for water balance mode runs.  Subsequently using such a
	soil parameter file for a full-energy balance or frozen soil simulation
	would cause the model to crash without the cause being clear.  Now, the
	validation in read_soilparam() prevents this case.

	Note: read_soilparam_arc() computes these quantities internally and
	already validates their values.


Added TFALLBACK option, to continue with previous temperature when energy
balance iteration fails to converge.

	Files Affected:

	calc_atmos_energy_bal.c
	calc_surf_energy_bal.c
	dist_prec.c
	display_current_settings.c
	frozen_soil.c
	func_surf_energy_bal.c
	get_global_param.c
	global.param.sample
	ice_melt.c
	initialize_global.c
	initialize_lake.c
	initialize_model_state.c
	lakes.eb.c
	output_list_utils.c
	put_data.c
	root_brent.c
	snow_intercept.c
	snow_melt.c
	solve_snow.c
	surface_fluxes.c
	vicNl_def.h
	vicNl.h

	Description:

	Added options.TFALLBACK, which can take the following values:
	  TRUE: If energy balance solution fails to converge, use previous
	        T value and continue
	  FALSE: If energy balance solution fails to converge, report an
	         error
	Default = TRUE.

	Currently, the following temperature variables are affected by the
	TFALLBACK option:
		snow.surf_temp  : snow pack surface temperature
		energy.T[]      : soil T profile nodes
		energy.Tsurf    : surface temperature
		energy.Tcanopy  : temperature of canopy air
		energy.Tfoliage : temperature of canopy snow

        For each grid cell, for each of the above variables, VIC will count
	the number of instances when the previous step's T was used.  These
	totals will be reported at the end of the grid cell's simulation.

	In addition, VIC tracks occurrences of T fallbacks through time via
	"flag" variables.  For each T variable, there is a corresponding T
	fallback flag, which is set to 1 if, in a given time step, the
	previous step's T was used in that time step, and 0 otherwise.  This
	holds for each veg tile / elevation band combination in the grid cell.

	These time series of occurrences of T fallbacks can be reported in
	output files via the following 4 new output variables:
	  OUT_SOILT_FLAG = array of flags, one for each soil T node
	  OUT_SURFT_FLAG = flag for Tsurf
	  OUT_SNOWT_FLAG = flag for snow.surf_temp
	  OUT_TCAN_FLAG = flag for Tcanopy
	  OUT_TFOL_FLAG = flag for Tfoliage
	Each of these contain the AVERAGE over the entire grid cell and over
	the output time interval (if aggregating temporally) of the flag
	values.  Thus, these are NOT integers but fractions ranging from 0 to
	1.

	Additionally modified root_brent to continue attempting to bracket
	root if one bound encounters undefined values/ERROR code from target
	function.



Added computation of 6 types of potential evaporation.

	Files Affected:

	arno_evap.c
	CalcAerodynamic.c
	canopy_evap.c
	compute_pot_evap.c (new)
	full_energy.c
	global.h
	initialize_lake.c
	initialize_model_state.c
	lakes.eb.c
	Makefile
	output_list_utils.c
	penman.c
	put_data.c
	read_soilparam_arc.c
	read_soilparam.c
	read_veglib.c
	read_vegparam.c
	surface_fluxes.c
	vicNl.c
	vicNl_def.h
	vicNl.h
	wetland_energy.c

	Description:

	Added computation of potential evaporation for 6 different reference
	land cover types, along with new output variables for reporting them:
	  OUT_PET_SATSOIL = pot evap from saturated soil
	  OUT_PET_H2OSURF = pot evap from open water surface
	  OUT_PET_SHORT   = pot evap from short reference crop (clipped grass)
	  OUT_PET_TALL    = pot evap from tall reference crop (alfalfa)
	  OUT_PET_NATVEG  = pot evap from current vegetation with canopy
			    resistance computed in the usual manner except
			    for the absence of water limitation
	  OUT_PET_VEGNOCR = pot evap from current vegetation with canopy
			    resistance set to 0

	Four "default" veg library classes were created (defined in global.h)
	so that veg characteristics for these landcover types (such as
	roughness length, etc) are present regardless of the contents of the
	externally-supplied vegetation library.  As a result, bare soil no
	longer is treated as an exception within the code, but as a legitimate
	class having its own area fraction Cv, etc.  This simplifies some of
	the loops in the code.

	In order to compute potential evaporation for these different cases,
	several different aerodynamic resistances also needed to be computed.
	Rather than store all of these in the cell_data structure, the
	"aero_resist" array of the cell_data structure was removed and
	the previously-named "aero_resist_used" array was renamed to
	"aero_resist".  Now, the pre-stability-correction values of
	aero_resist are stored in a temporary internal array and not reported.

	Finally, computation of canopy resistance was moved out of penman()
	and into a separate function calc_rc() within the same file.


Simplified argument lists of runoff() and surface_fluxes().

	Files Affected:

	full_energy.c
	runoff.c
	surface_fluxes.c
	vicNl.h
	wetland_energy.c

	Description:

	Simplified the argument lists of runoff() and surface_fluxes() by
	replacing explicit references to individual cell_data variables by
	a single reference to the cell_data structure.



Wetland portion of lake/wetland tile is now simulated in full_energy().

	Files Affected:

	free_dist_prcp.c
	frozen_soil.c
	full_energy.c
	initialize_lake.c
	initialize_model_state.c
	initialize_soil.c
	LAKE.h
	lakes.eb.c
	make_dist_prcp.c
	Makefile
	put_data.c
	read_initial_model_state.c
	read_lakeparam.c
	read_soilparam.c
	read_soilparam_arc.c
	read_vegparam.c
	soil_conduction.c
	vicNl.c
	vicNl_def.h
	vicNl.h
	wetland_energy.c (removed)
	write_model_state.c

	Description:

	In previous versions of 4.1.x, the wetland portion of the lake/wetland
	tile was stored in the (N+1)st vegetation tile, and processed in the
`	function wetland_energy(), which was an almost exact duplicate of
	full_energy().  This created several difficulties, including a) potential
	conflicts with the COMPUTE_TREELINE option, b) conflicts with the new
	implementation of bare soil and potential evap, c) hard-coding of
	wetland vegetation to be equal to the grid cell's first listed cover
	type, and d) general complexities in maintaining the code/adding new
	features.

	Now, the wetland portion of the lake/wetland tile must be listed in
	the vegetation parameter file in the same was as all other veg cover
	types.  Similarly, the wetland portion of the lake/wetland tile is
	processed in full_energy() in the exact same way as all other tiles.
	Wetland_energy() has been removed.  Lakemain() has also been removed,
	as the calls to lake-specific functions are now executed directly from
	full_energy().  To make all of this happen, the index of the veg tile
	containing the lake/wetland must be indicated in the lake parameter
	file (thus, the lake parameter file must contain an additional
	value for each grid cell).

	Users may now specify any type of desired vegetation to fill the
	wetland portion of the lake/wetland tile, simply by listing the
	desired veg cover in the veg param file and placing the index of this
	tile in the lake parameter file.  The user is also free now to create
	a new wetland vegetation class in the vegetation library.



State files now contain data for 0-area bands.

	Files Affected:

	read_initial_model_state.c
	write_model_state.c

	Description:

	In order to ensure that the value of Nbands saved in the state file
	accurately accounted for the number of bands whose states are stored
	in the state file, changed VIC to read/write data for all bands,
	whether or not their area is 0.



Bug Fixes:
----------

Fixed conflict between PRT_SNOW_BAND option and flexible output configuration.

	Files Affected:

	global.param.sample
	parse_output_info.c
	put_data.c
	vicNl_def.h

	Description:

	The PRT_SNOW_BAND option exists for backwards-compatibility with older
	versions of VIC.  If the user is using the default output cofiguration,
	and PRT_SNOW_BAND is set to TRUE, it creates "snowband" output files.
	However, this was not made clear in the documentation; it is possible
	to specify output files containing the snow-band-specific values of
	variables without setting PRT_SNOW_BAND to TRUE.  However, put_data()
	contained logic that only allowed it to record snow-band-specific
	quantities if PRT_SNOW_BAND is TRUE, even if the user had specified
	snow-band-specific output variables in the output files.

	Therefore, the logic checking the value of PRT_SNOW_BAND was removed
	from put_data().  Additionally, if the user has specified N_OUTFILES
	in the global parameter file, PRT_SNOW_BAND will now be set to FALSE
	by parse_output_info(), just to make sure that PRT_SNOW_BAND only
	applies to the default output case.  Documentation in
	global.param.sample and vicNl_def.h has been updated to reflect this
	behavior.						TJB


Fixed problems in soil temperature solution due to use of dz_node.

	Files Affected:

	calc_surf_energy_bal.c
	frozen_soil.c
	func_surf_energy_bal.c
	initialize_model_state.c
	lakes.eb.c
	soil_conduction.c
	vicNl.h
	wetland_energy.c

	Description:

	The addition of the EXCESS_ICE option introduced a new variable,
	Zsum_node, to the soil thermal solution code.  This conflicted with
	the existing variable dz_sum.  Therefore, dz_sum has been replaced
	by Zsum_node in several functions.			KAC via TJB


Fixed problem of errors from put_data() not being caught.

	Files Affected:

	dist_prec.c

	Description:

	Modified routine to store put_data() error in ErrorFlag2 and
	return a single ERROR value if an error occurs.		KAC via TJB


Fixed uninitialized value errors in parse_output_info.c.

	Files Affected:

	parse_output_info.c

	Description:

	Added default values for format, typestr, and multstr, so that they
	can be omitted from global param file.			TJB



Fixed inability to read tabs in soil/veg parameter files.

	Files Affected:

	read_soilparam.c
	read_vegparam.c

	Description:

	At some point in earlier versions, the functions that read soil/veg
	parameter files were modified to use the "strtok" function, and as a
	result, VIC lost the ability to understand tabs as field separators
	in these files.  This ability has now been restored.	TJB
	


Fixed bugs in snow bands implementation.

	Files Affected:

	output_list_utils.c
	read_snowband.c
	read_vegparam.c
	vicNl.c
	vicNl.h
	vicNl_def.h

	Description:

	Some snowband-specific output variables had an incorrect number of
	elements allocated in output_list_utils.c.  This has been fixed.  In
	addition, added BandElev[] array to soil_con_struct.  Also added logic
	to ensure that grid-cell-average elevation (from soil parameter file)
	matches the average of the band elevations.  Also added allocation of
	extra veg tile for the case of a band being above the treeline in the
	COMPUTE_TREELINE case.					TJB



Removed special logic for longwave in water balance mode

	Files Affected:

	func_surf_energy_bal.c
	intialize_atmos.c
	initialize_model_state.c

	Description:

	In the 4.0.x branch of VIC, net longwave radiation was stored in the
	atmos.longwave variable for the case of water balance mode.  This
	required that special logic be inserted into various parts of the
	code wherever the longwave variable was used.  In 4.1.x, new features
	were added and several different types of longwave flux were added.
	None of these new fluxes took the water-balance case into account, and
	as a result, the water balance case was handled incorrectly.  Now the
	special handling of the water balance case has been removed; atmos.longwave
	always contains incoming longwave.  This also simplifies the code.
	TJB



VIC now can understand indented comment lines in the global parameter file

	Files Affected:

	get_global_param.c

	Description:

	Previously, VIC only understood a line to be a comment if it began
	with a '#' character at the very beginning of the line, i.e. no
	leading white space such as spaces or tabs.  Now, any line whose first
	non-whitespace character is '#' is considered to be a comment.



Miscellaneous fixes to snow albedo aging scheme

	Files Affected:

	initialize_snow.c
	SnowPackEnergyBalance.c
	snow_melt.c
	solve_snow.c

	Description:

	Previously, VIC's last_snow counter, which keeps track of the number
	of time steps since the last fresh snowfall, and which is used to
	compute the snow albedo, was not reset to 0 at the proper times.  This
	has been fixed, along with its initialization.



Fixed errors in summing output variables

	Files Affected:

	dist_prec.c
	full_energy.c
	initialize_lake.c
	initialize_model_state.c
	initialize_snow.c
	LAKE.h
	lakes.eb.c
	put_data.c
	read_initial_model_state.c
	read_snowband.c
	read_soilparam_arc.c
	read_soilparam.c
	vicNl.c
	vicNl_def.h
	vicNl.h
	write_model_state.c

	Description:

	Previously, many lake water and energy budget terms were lumped into
	the wetland soil, snow, and energy structures.  This often led to
	water/energy balance errors both internally and in summing these terms
	for output.  To solve this problem, new cell, snow, and energy
	structures were added to the lake_var structure.  These structures
	allow all lake water and energy budget terms to be tracked
	independently of the wetland.

	In addition, previously, the initial computation of water/energy
	storages for use in the water/energy balance checks was done in
	vicNl().  This logic was not in sync with the computations in put_data().
	Therefore the logic was replaced with an initial call to put_data()
	(using rec= -Nrecs).  Within put_data(), two new functions were
	created to help ensure that summing was consistent among upland veg
	tiles, wetland, and lake: collect_wb_terms() and collect_eb_terms().  

	Finally, another source of errors arose from the allocation and
	initialization of snowband parameters in read_snowband().  These
	terms were only initialized in read_snowband(), but were in some cases
	used in put_data() for the non-snowband case.  Therefore, we have
	moved the allocation/initialization of these terms to read_soilparam()
	and read_soilparam_arc().  These parameters are further modified in
	read_snowband(), as necessary.



Added error messages for case when LAI==0 and overstory==1.

	Files Affected:

	read_veglib.c
	read_vegparam.c

	Description:

	Added error messages for case when LAI==0 and overstory==1.  Without
	this check, a user could specify LAI of 0 for an overstory type (e.g.
	a deciduous tree).  Unforutunately, the model code assumes that the
	overstory itself always contains some minimum LAI (perhaps assuming
	that the LAI contains stem area as well?), and in some cases divides
	by the LAI.  If LAI ever is 0 for an overstory type, this will lead
	to nan values in evap, soil moisture, canopy storage, etc.



--------------------------------------------------------------------------------
***** Description of changes from VIC 4.1.0 beta r4 to VIC 4.1.0 beta r5 *****
--------------------------------------------------------------------------------


New Features:
-------------

Updated lake model.

	Files Affected:

	full_energy.c
	get_dist.c (new)
	ice_melt.c
	initialize_lake.c
	lakes.eb.c
	LAKE.h
	Makefile
	output_list_utils.c
	put_data.c
	read_initial_model_state.c
	read_lakeparam.c
	read_soilparam_arc.c
	read_soilparam.c
	vicNl_def.h
	vicNl.c
	vicNl.h
	water_energy_balance.c
	water_under_ice.c
	wetland_energy.c
	write_model_state.c

	Description:

	Merged Laura Bowling's latest lake model code into UW version.
	Changes include:
	  * Fixed lake snow physics to be consistent with land snow pack
	  * Lake ice formation now takes into account availability of liquid water
	  * Lake ice now explicitly tracked in water balance
	  * Drainage now depends only on liquid water content, not total water+ice
	  * Drainage now modeled as flow over broad-crested wier
	  * New lake parameter file format - see read_lakeparam.c for details
	  * Fixes for crashes in extreme cases; fixes for water balance errors
	  * New output variable OUT_SOIL_TNODE_WL, the soil thermal node
	    temperatures in the wetland tile
										LCB via TJB


New optional snow density computation.

	Files Affected:

	display_current_settings.c
	get_global_param.c
	initialize_global.c
	snow.h
	snow_utility.c
	solve_snow.c
	user_def.h
	vicNl_def.h
	vicNl.h

	Description:

	In addition to the previous algorithm (as a function of day of year,
	based on a plot of seasonal variation of typical snow densities found
	in Bras, figure 6.10, p 258), added a new algorithm that computes the
	snow density based on swe and snow metamorphism.  The new algorithm is
	taken from SNTHERM89, adjusted for an essentially single-layer model.
	Introduced a new global parameter file option, SNOW_DENSITY, to choose
	which of these density algorithms will be used.  If SNOW_DENSITY is set
	to DENS_BRAS, the original algorithm is used.  If SNOW_DENSITY is set
	to DENS_SNTHRM, the new algorithm is used.  The default value (i.e. if
	SNOW_DENSITY is omitted) is DENS_BRAS.  Also moved snow-related
	constants from user_def.h to snow.h, for consistency.			KMA via TJB



New optional snow albedo algorithm.

	Files Affected:

	display_current_settings.c
	get_global_param.c
	initialize_global.c
	LAKE.h
	lakes.eb.c
	snow_utility.c
	solve_snow.c
	vicNl_def.h
	vicNl.h

	Description:

	In addition to the previous algorithm (based on US Army Corps of
	Engineers empirical snow albedo decay curves), added a new algorithm
	based on Sun et al., 1999.  Introduced a new global parameter file
	option, SNOW_ALBEDO, to choose which of these density algorithms will
	be used.  If SNOW_ALBEDO is set to USACE, the original algorithm is
	used.  If SNOW_ALBEDO is set to SUN1999, the new algorithm is used.
	The default value (i.e. if SNOW_ALBEDO is omitted) is DENS_BRAS.	KAC via TJB



Bug Fixes:
----------

Allow lakes to be empty or very shallow.

	Files Affected:

	initialize_lake.c
	initialize_model_state.c
	LAKE.h
	lakes.eb.c
	read_lakeparam.c
	wetland_energy.c

	Description:

	Previously, lakes could not be initialized to be empty or with volume
	below the minimum volume of 1 lake node.  This modification removes
	that constraint.  In addition, the lake-specific get_* functions were
	modified to return ErrorFlag values instead of aborting on error,
	making them more consistent with the CONTINUEONERROR option.		KAC via TJB


Miscellaneous fixes to lake model

	Files Affected:

	lakes.eb.c
	read_lakeparam.c
	wetland_energy.c

	Description:

	Fixes for swe accounting errors and incorrect parsing of lake
	parameter file.								LCB via TJB


Lake snow and ice fixes

	Files Affected:

	LAKE.h
	lakes.eb.c
	vicNl_def.h

	Description:

	Modified to set lake ice fraction to 0 if new lake fraction is 0.
	Previously lake ice was always adjusted by dividing old by new fractions,
	which causes nans when the new fraction is 0.  Modified redistribution of
	snow variables between wetland and lake ice.  Surface water is now stored
	as a lake variable and used to reestablish surf_water at the start of a
	new time step.								KAC via TJB


Better validation of global parameter file options

	Files Affected:
	get_global_param.c
	make_in_and_outfiles.c
	parse_output_info.c
	read_soilparam_arc.c
	read_vegparam.c

	Description:
	Now, all options in the global parameter file involving input and
	output files are checked to ensure that the filenames have been
	specified.  In addition, SOIL_DIR and RESULT_DIR no longer require
	a trailing "/"; a "/" is automatically appended when constructing
	the soil parameter filename or results filename.  Also, makes sure
	that N_OUTFILES matches number of output files actually defined in
	the global param file.							TJB


Incorrect soil ice fractions for the case when FROZEN_SOIL = TRUE and
options.SNOW_BAND > 1.

	Files Affected:

	full_energy.c
	prepare_full_energy.c
	surface_fluxes.c

	Description:

	Changed initial total and frozen moistures from scalars (moist and
	ice0) to arrays (moist0 and ice0).  Previously, when
	options.SNOW_BAND > 1, the values of moist and ice0 computed for
	earlier bands were always overwritten by the values of moist and
	ice0 computed for the final band (even if the final band had 0
	area).									JS and KAC via TJB


Bad results when SNOW_STEP = 24 hours

	Files Affected:

	get_global_param.c
	initialize_atmos.c

	Description:

	Fixed the criterion for deciding whether to store NET longwave or
	INCOMING longwave in atmos[rec].longwave[NR].  Previously, VIC
	stored NET lw if SNOW_STEP != global.dt, under the assumption that
	this condition was equivalent to running in 24 h water balance mode.
	However, this caused problems for users who unwittingly ran 24 h
	water balance mode with SNOW_STEP = 24.  The new criterion is that
	both FULL_ENERGY and FROZEN_SOIL must be FALSE.				TJB


Incorrect setting for NOFLUX option in calc_surf_energy_bal()

	Files Affected:

	calc_surf_energy_bal.c

	Description:

	Modified code to reset NOFLUX boundary to global option
	value before computing final soil column temperatures.
	Previously NOFLUX was set to FALSE for initial QUICK_SOLVE
	estimates, but never reset to reflect actual bottom
	boundary conditions for final pass through solver.			KAC


Removed -g from normal compiling options.

	Files Affected:

	Makefile

	Description:

	At some point, "-g" was added to the normal compiling options.
	This has been removed.  To use the -g option, users should uncomment
	the "debug" section of the makefile.					TJB


Changed "vicInterp" to "vicDisagg".

	Files Affected:

	Makefile

	Description:

	Changed the name of the makefile option to compile vic with the
	OUTPUT_FORCE option set to TRUE.  The term "disaggregate" is more
	correct than the term "interpolate", so "vicDisagg" seems more
	appropriate.  The usage of this option is as follows:

	At command prompt, type
		make disagg

	This will create the executable vicDisagg, which is simply vicNl
	compiled with OUTPUT_FORCE set to TRUE in user_def.h.			TJB


Added end-of-file check to read_lakeparam.c

	Files Affected:

	read_lakeparam.c

	Description:

	Added check on !feof(lakeparam) to loop over lake param file, to
	avoid infinite loop when current grid cell has no lakes and is not
	mentioned in the lake param file.					TJB


Compilation problems for QUICK_FS user_def.h option

	Files Affected:

	func_surf_energy_bal.c
	get_global_param.c
	initialize_model_state.c
	read_lakeparam.c
	water_energy_balance.c
	wetland_energy.c

	Description:

	Previous updates for the EXCESS_ICE option had inadvertently introduced
	bugs into the logic for the QUICK_FS option.  These included removal of
	the logic that modifies calls to distribute_node_moisture_properties()
	and estimate_layer_ice_content().					TJB


OUT_SUB_BLOWING, OUT_SUB_SURFACE, and OUT_SUB_SNOW missing from default snow_*
output files.

	Files Affected:

	set_output_defaults.c

	Description:

	Added OUT_SUB_BLOWING, OUT_SUB_SURFACE, and OUT_SUB_SNOW to default
	snow output file for case of options.BLOWING == TRUE.  This makes it
	almost the same as previous versions of 4.1.0, (r3 and earlier) with
	the exception that previous versions of 4.1.0 multiplied these terms
	by 100 when saving to the snow file.					TJB


Error in computation of initial water storage for DIST_PRCP = TRUE

	Files Affected:

	vicNl.c

	Description:

	Added dist_prcp fraction (mu) to the computation of initial water
	storage terms.  Otherwise, when DIST_PRCP = TRUE, the water storage
	terms for wet and dry fractions were added together with no weighting,
	resulting in twice the storage as actually present.			TJB


Fixed various issues with uninitialized variables, un-freed arrays, NaN's, etc.

	Files Affected:

	dist_prec.c
	put_data.c
	read_vegparam.c
	runoff.c
	surface_fluxes.c
	vicNl.h

	Description:

	put_data() was attempting to return an ErrorFlag but it was defined
	as type int.  Therefore, changed the type of put_data() and changed
	the call to put_data() in dist_prec.c.  read_vegparam() allocated
	but never freed a temporary array vegarr[].  This was fixed.
	In runoff(), top_moist could sometimes exceed top_max_moist due
	to rounding errors, leading to NaN's in i_0 and A_0.  surface_fluxes()
	was sending incorrectly-indexed displacement and roughness values to
	CalcBlowingSnow().  These have all been fixed.		LCB via TJB


--------------------------------------------------------------------------------
***** Description of changes from VIC 4.1.0 beta r3 to VIC 4.1.0 beta r4 *****
--------------------------------------------------------------------------------


New Features:
-------------

Flexible output configuration & aggregation of output variables

        Files Affected:

        Makefile
	alloc_atmos.c
	calc_water_energy_balance_errors.c (new)
	close_files.c
	conv_force_vic2alma.c (removed)
	conv_results_vic2alma.c (removed)
	display_current_settings.c
	dist_prec.c
	full_energy.c
	get_global_param.c
	global.h
	global.param.sample
	IceEnergyBalance.c
	initialize_atmos.c
	initialize_global.c
	initialize_model_state.c
	make_in_and_outfiles.c
	output.LDAS_OUTPUT.template (new)
	output.OPTIMIZE.template (new)
	output.OUTPUT_FORCE.ALMA.template (new)
	output.OUTPUT_FORCE.template (new)
	output.PILPS-2E.ALMA.template (new)
	output.TRADITIONAL.410.template (new)
	output.TRADITIONAL.template (new)
	output_list_utils.c (new)
	parse_output_info.c (new)
	put_data.c
	read_atmos_data.c
	set_output_defaults.c (new)
	SnowPackEnergyBalance.c
        solve_snow.c
	surface_fluxes.c
	user_def.h
	vicNl.c
	vicNl.h
	vicNl_def.h
	vicerror.c
	water_energy_balance.c
	wetland_energy.c
	write_data.c
	write_forcing_file.c

        Description:

        In earlier versions of VIC, the set of output files and their contents
        were hard-coded.  A few settings in user_def.h (OPTIMIZE and
	LDAS_OUTPUT) allowed the user to change the contents of the output
	files, but this has not been enough to accomodate the various needs of
	users.  Users inevitably have had to modify write_data.c to produce the
	type of output they want, and when VIC is updated, they must merge their
	changes into the new version of the code.

        VIC 4.1.0 now allows the user to specify exactly which output files to
        create and which variables to store in each file.  This way, users can
        save space by only writing those variables that are useful, and will
	be less likely to need to maintain a private version of the code to do
	this.

	In addition, VIC 4.1.0 can now aggregate the output variables to a
	user-defined output interval, via the OUT_STEP setting in the global
	parameter file.  Currently, the largest output interval allowed is 24
	hours, so this option is only useful for simulations running at sub-
	daily time steps.


        Main points:

        1. Output file names and contents can be specified in the global param
           file (see below).

        2. If you do not specify file names and contents in the global param
           file, VIC will produce the same set of output files that it has
           produced in earlier versions, namely "fluxes" and "snow" files,
	   plus "fdepth" files if FROZEN_SOIL is TRUE and "snowband" files if
           PRT_SNOW_BAND is TRUE.  These files will have the same contents and
           format as in earlier versions.

        3. The OPTIMIZE and LDAS_OUTPUT options have been removed.  These
           output configurations can be selected with the proper set of
           instructions in the global param file.  (see the output.*.template
           files included in this distribution for more information.)

        4. If you do specify the file names and contents in the global param
	   file, PRT_SNOW_BAND will have no effect.

        To specify file names and contents in the global param file, one
        should use the following format:

          (typical global param file contents here...)

          # Output File Contents
          N_OUTFILES    <n_outfiles>

          OUTFILE       <prefix>        <nvars>
          OUTVAR        <varname>       [<format>       <type>  <multiplier>]
          OUTVAR        <varname>       [<format>       <type>  <multiplier>]
          OUTVAR        <varname>       [<format>       <type>  <multiplier>]

          OUTFILE       <prefix>        <nvars>
          OUTVAR        <varname>       [<format>       <type>  <multiplier>]
          OUTVAR        <varname>       [<format>       <type>  <multiplier>]
          OUTVAR        <varname>       [<format>       <type>  <multiplier>]

        where
                <n_outfiles> = number of output files
                <prefix>     = name of the output file, NOT including latitude
                               and longitude
                <nvars>      = number of variables in the output file
                <varname>    = name of the variable (this must be one of the
                               output variable names listed in vicNl_def.h.)

                <format>, <type>, and <multiplier> are optional.  For a given
                variable, you can specify either NONE of these, or ALL of
                these.  If these are omitted, the default values will be used.

                <format>     = (for ascii output files) fprintf format string,
                               e.g.
                                 %.4f = floating point with 4 decimal places
                                 %.7e = scientific notation w/ 7 decimal
                                 %places
                                 *    = use the default format for this
                                 *    variable
                <type>       = (for binary output files) data type code.
                               Must be one of:
                                 OUT_TYPE_DOUBLE = double-precision floating
						   point
                                 OUT_TYPE_FLOAT  = single-precision floating
						   point
                                 OUT_TYPE_INT    = integer
                                 OUT_TYPE_USINT  = unsigned short integer
                                 OUT_TYPE_SINT   = short integer
                                 OUT_TYPE_CHAR   = char
                                 *               = use the default type
                <multiplier> = (for binary output files) factor to multiply
                               the data by before writing, to increase
                               precision.
                                 *    = use the default multiplier for this
                                 *    variable

        Here's an example.  To specify 2 output files, named "wbal" and
        "ebal", and containing water balance and energy balance terms,
        respectively, you could do something like this:

          N_OUTFILES    2

          OUTFILE       wbal    6
          OUTVAR        OUT_PREC
          OUTVAR        OUT_EVAP
          OUTVAR        OUT_RUNOFF
          OUTVAR        OUT_BASEFLOW
          OUTVAR        OUT_SWE
          OUTVAR        OUT_SOIL_MOIST

          OUTFILE       ebal    7
          OUTVAR        OUT_NET_SHORT
          OUTVAR        OUT_NET_LONG
          OUTVAR        OUT_LATENT
          OUTVAR        OUT_SENSIBLE
          OUTVAR        OUT_GRND_FLUX
          OUTVAR        OUT_SNOW_FLUX
          OUTVAR        OUT_ALBEDO

        Since no format, type, or multiplier were specified for any variables,
	VIC will use the default format, type, and multiplier for the variables.

        If you wanted scientific notation with 10 significant digits for
	ALBEDO, you could do the following:

          OUTVAR        OUT_ALBEDO              %.9e    *       *

        Note that even if you only want to specify the format, you must supply
	a value in the type and multiplier columns as well.  This can be "*" to
	indicate the default value.  Similarly, if you only want to specify the
	type (e.g.  as a double), you would need to do something like:

          OUTVAR        OUT_ALBEDO              *       OUT_TYPE_DOUBLE	*


        Date variables:

        For typical output files, the date is always written at the beginning
	of each record.  This will consist of the following columns:
          year month day hour
        For daily output timestep, "hour" is not written.

        If BINARY_OUTPUT is TRUE, these will all be written as type int
	(OUT_TYPE_INT).

        If OUTPUT_FORCE is TRUE, the date will NOT be written.


        Multiple-valued variables:

        Since variables like SOIL_MOIST have 1 value per soil layer, these
	variables will be written to multiple columns in the output file, one
	column per soil layer.  Other multiple-valued variables are treated
	similarly.


        Snow band output:

        To specify writing the values of variables in each snow band,  append
        "_BAND" to the variable name (this only works for some variables - see
        the list in vicNl_def.h).  If you specify these variables, the value
	of the variable in each band will be written, one band per column.  For
        example, for a cell having 2 snow bands:

          OUTVAR        OUT_SWE_BAND
          OUTVAR        OUT_ALBEDO_BAND

        will result in an output file containing:

          year month day (hour) swe[0] swe[1] albedo[0] albedo[1]

										TJB


Cleanup of structures holding filenames and file pointers

	Files Affected:

	check_files.c
	check_state_file.c
	close_files.c
	display_current_settings.c
	dist_prec.c
	get_global_param.c
	initialize_model_state.c
	make_in_and_outfiles.c
	open_state_file.c
	read_initial_model_state.c
	vicerror.c
	vicNl.c
	vicNl_def.h
	vicNl.h
	write_model_state.c

	Description:

	1. Merged infiles and outfiles structs into filep_struct.
	2. Merged builtnames into filenames struct.
	3. Renamed infiles.statefile to filep.init_state
	4. Moved global.statename to filenames.statefile.
	5. Added f_path_pfx[] to the filenames_struct, to store
	   the path and prefix of forcing files.  Now, forcing[]
	   only stores the full forcing file names.
										TJB


Soil thermal node temperature is now an output variable

	Files Affected:

	output_list_utils.c
	put_data.c
	vicNl_def.h

	Description:

	Soil thermal node temperature can now be selected for output, via the
	key "OUT_SOIL_TNODE".  If selected, the temperatures at all valid soil
	thermal nodes will be output.						TJB


Removed LAKE_MODEL compile-time option

	Files Affected:

	check_files.c
	display_current_settings.c
	dist_prec.c
	full_energy.c
	get_global_param.c
	IceEnergyBalance.c
	ice_melt.c
	initialize_lake.c
	initialize_model_state.c
	initialize_soil.c
	LAKE.h
	lakes.eb.c
	make_dist_prcp.c
	put_data.c
	read_initial_model_state.c
	read_lakeparam.c
	user_def.h
	vicNl.c
	vicNl_def.h
	vicNl.h
	water_energy_balance.c
	water_under_ice.c
	wetland_energy.c
	write_model_state.c

	Description:

	From a functional standpoint, the LAKE_MODEL compile-time option was
	redundant with the options.LAKE switch.  Meanwhile, the difference in
	the size of the executable between when the LAKE_MODEL switch was on
	or off was deemed insignificant.  Therefore, this switch was removed
	in the interest of making the code easier to maintain and reducing
	user confusion.  Now, to turn off lake functionality, simply set LAKE
	to FALSE in the global parameter file (or omit the "LAKE" line
	completely).								TJB


More complete set of supported input variables

        Files Affected:

        display_current_settings.c
        get_force_type.c
        get_force_type_pilps.c (removed)
        get_global_param.c
        initialize_atmos.c
        initialize_atmos_pilps.c (removed)
        initialize_global.c
        vicNl_def.h

        Description:

        Added REL_HUMID (relative humidity, as a fraction) to the list of
        supported met input variables.  Added the ALMA_INPUT option, which
        causes temperatures to be interpreted as Kelvin, pressures as kPa,
        and moisture fluxes as rates (mm/s) instead of accumulated fluxes
        (mm/timestep).  This allowed us to remove TAIR and PSURF from the
        list of supported met input variables, since AIR_TEMP and PRESSURE
        provide this ability when ALMA_INPUT is TRUE.  This also makes
        input variable specification more consistent with output variable
        specification.

        Since now the forcing files from PILPS can be read via the
        appropriate specification of input variables and ALMA_INPUT option
        in the global parameter file, the file initialize_atmos_pilps.c
        has been removed.							TJB


Optional headers for output and input files

        Files Affected:

        display_current_settings.c
        get_global_param.c
        global.param.sample
        initialize_global.c
        Makefile
        read_atmos_data.c
        vicNl.c
        vicNl_def.h
        vicNl.h
        write_header.c (new)

        Description:

        Added the PRT_HEADER option to the global parameter file.  If
        this is set to TRUE, VIC will insert a short header into its
        output files, describing the time step, start date/time,
        variables and units included in the file.

        For ascii files, the output header has the following format:

                # NRECS: (nrecs)
                # DT: (dt)
                # STARTDATE: yyyy-mm-dd hh:00:00
                # ALMA_OUTPUT: (0 or 1)
                # NVARS: (Nvars)
                # VARNAME    VARNAME   VARNAME   ...

        where
           nrecs       = Number of records in the file
           dt          = Time step length in hours
           start date  = Date and time of first record of file
           ALMA_OUTPUT = Indicates units of the variables; 0 = standard VIC
                         units; 1 = ALMA units
           Nvars       = Number of variables in the file, including date
                         fields

        For binary files, the output header has the following format:

            // Data        Stored As           Comment
            //
            // Identifier  (unsigned short)*4  0xFFFF, repeated 4 times
            // Nbytes      (unsigned short)*1  Number of bytes in the header,
            //                                 INCLUDING THE IDENTIFIER
            //
            // Part 1: Global Attributes
            // Nbytes1     (unsigned short)*1  Number of bytes in part 1
            // nrecs       (int)*1             Number of records in the file
            // dt          (int)*1             Time step length in hours
            // startyear   (int)*1             Year of first record
            // startmonth  (int)*1             Month of first record
            // startday    (int)*1             Day of first record
            // starthour   (int)*1             Hour of first record
            // ALMA_OUTPUT (char)*1            0 = standard VIC units; 1 = ALMA units
            // Nvars       (char)*1            Number of variables in the file,
            // including date fields
            //
            // Part 2: Variables
            // Nbytes2     (unsigned short)*1  Number of bytes in part 2
            // For each variable, the following fields: { len varname type mult }
            //   len       (char)*1            Number of characters in varname
            //   varname   (char)*len          Variable name
            //   type      (char)*1            Code identifying variable type
            //   mult      (float)*1           Multiplier for variable


        To accomodate input forcing files that might have been produced via
        VIC's OUTPUT_FORCE option, and therefore could contain a header,
        read_atmos_data.c has been modified to detect and skip headers that
        follow the formats outlined above.					TJB


Variable TYPE specifications for binary-format output files in the global parameter
file must match the strings listed in vicNl_def.h.

	Files Affected:

	parse_output_info.c

	Description:

	When listing output variables in the global parameter file, if the
	output file format is binary, the variable data TYPE must match the string
	from vicNl_def.h exactly, e.g. "OUT_TYPE_INT" rather than just "INT".	TJB


Added global option CONTINUEONERROR allowing simulation to continue on cell error.

	Files Affected:

	arno_evap.c
	CalcAerodynamic.c
	calc_atmos_energy_bal.c
	CalcBlowingSnow.c
	calc_rainonly.c
	calc_surf_energy_bal.c
	display_current_settings.c
	dist_prec.c
	frozen_soil.c
	full_energy.c
	func_surf_energy_bal.c
	get_global_param.c
	ice_melt.c
	initialize_new_storm.c
	lakes.eb.c
	redistribute_during_storm.c
	root_brent.c
	runoff.c
	snow_intercept.c
	snow_melt.c
	soil_conduction.c
	solve_snow.c
	surface_fluxes.c
	vicNl.c
	water_energy_balance.c
	water_under_ice.c
	wetland_energy.c
	global.param.sample
	LAKE.h
	vicNl_def.h
	vicNl.h

	Description:

	If the global option CONTINUEONERROR is set to TRUE then the
	simulation continues to run even if a cell fails and produces
	partial output. Default is is for CONTINUEONERROR to be FALSE,
	as in previous versions of model. The return value of some modules
	was changed from void to an int numeric return value. The ERROR
	value is defined in vicNl_def.h.					KAC via GCT


Exponential grid transformation option for soil thermal nodes in finite
difference heat equation

	Files Affected:

	calc_surf_energy_bal.c
	display_current_settings.c
	frozen_soil.c
	func_surf_energy_bal.c
	get_global_param.c
	initialize_global.c
	initialize_model_state.c
	soil_conduction.c
	soil_thermal_eqn.c
	vicNl_def.h

	Description:

	New option is now EXP_TRANS to have an exponential distribution
	of the thermal node depths in the vertical dimension.  This allows for
	closer thermal nodes near the surface of soil where the variance is
	greatest.  The transformation allows for the solution to be solved in
	linear-space, wherein the nodes are distributed linearly with depth,
	but the physical system exists in exponential space.  This change also
	involves a rearranging of the terms in the finite-difference equaton
	(equation 8 of Cherkauer et al. 1999).  Therefore the constants (A-E)
	are calculated in a new way.  These constants are equal to the constants
	in each of the terms in equation 8 multiplied by alpha^2*deltat.	JCA


Implicit solution option for finite difference frozen soils algorithm

	Files Affected:

	calc_surf_energy_bal.c
	display_current_settings.c
	frozen_soil.c
	func_surf_energy_bal.c
	get_global_param.c
	initialize_global.c
	Makefile
	surface_fluxes.c
	vicNl_def.h
	vicNl.h

	Description:

	The implicit solution uses Newton Raphson to solve the system of non-
	linear equations.  Therefore there is a new option, IMPLICIT.  If the
	implicit solution fails (which can sometimes occur at the initial
	formation of ice in the soil column), the model will use the original
	explicit solver to solve for the thermal temperatures for that time
	step only.  The implicit option is unconditionally stable and therefore
	the Courant-Friedrichs-Lewy condition does not need to be checked (as
	it should be for the explicit solver).					JCA


Zsum_node calculated once and passed in soil_con to other subroutines

	Files Affected:

	calc_surf_energy_bal.c
	func_surf_energy_bal.c
	initialize_model_state.c
	lakes.eb.c
	read_initial_model_state.c
	runoff.c
	soil_conduction.c
	vicNl_def.h
	write_debug.c
	write_model_state.c

	Description:

	Zsum_node is the depth of each of the thermal nodes needed
	for the finite difference frozen soils algorithm.  Rather than re-
	calculating this value in various places, this vector is now passed
	in the soil_con structure to the various subroutines where it is
	needed.  The one exception is set_node_parameters, because this is
	a slightly different definition of Zsum.				JCA


Patch for "cold nose" problem

	Files Affected:

	frozen_soil.c
	soil_thermal_eqn.c

	Description:

	This is an inelegant fix for "cold nose" problem that may occur in
	the finite different frozen soil algorithm in each mode of operation.
	This involves a thermal node that is of a lower temperature than the
	upper and lower thermal nodes.  Due to the first term of the heat
	conduction term (the term that accounts for a change in the thermal
	conductivity in space), the first term can become more negative
	than the second term. This causes this cold node to become
	much colder and thus breaks the second law of thermodynamics (because
	flux_term1 exceeds flux_term2 in absolute magnitude).  Therefore, a check
	is now made to set the first term to zero when this begins to happen.
	This only seems to happen in the first and second near-surface nodes.	JCA


Added EXCESS_ICE option (set in user_def.h)

	Files Affected:

	calc_surf_energy_bal.c
	calc_water_energy_balance_errors.c
	display_current_settings.c
	frozen_soil.c
	full_energy.c
	func_surf_energy_bal.c
	get_global_param.c
	initialize_model_state.c
	initialize_soil.c
	LAKE.h
	lakes.eb.c
	output_list_utils.c
	prepare_full_energy.c
	put_data.c
	read_initial_model_state.c
	read_soilparam_arc.c
	read_soilparam.c
	runoff.c
	soil_conduction.c
	soil_thermal_eqn.c
	surface_fluxes.c
	user_def.h
	vicNl.c
	vicNl_def.h
	vicNl.h
	wetland_energy.c
	write_model_state.c

	Description:

	If TRUE, VIC allows for excess ground ice, i.e. an expanded porosity
	to account for an initial volumetric ice fraction larger than
	soil porosity.  The ground subsides (therefore soil depth is no
	longer static) and the porosity decreases as the excess ice melts.
	Once porosity reaches the soil porosity (1-bulk density/soil density),
	it does not change.  The initial volumetric ice fraction for
	each soil layer must be defined in the soil file. The maximum value
	for this initial ice fraction is set by MAX_ICE_INIT in
	vicNl_def.h.  Another new parameter is ICE_AT_SUBSIDENCE, also set
	in vicNl_def.h.  This parameter controls the rate of subsidence as
	the ground warms.  Setting a larger value will cause the ground to
	subside sooner and faster.  The physical bounds of this parameter
	are 0.0 and 1.0, although setting this to a value of 1.0 will result
	in the ground continually subsiding despite very cold soil
	temperatures because there is always a small amount of unfrozen water
	in each soil layer due to the function used in maximum_unfrozen_water.
	The recommended value is 0.8.						JCA


Bare soil evap for LAI=0

	Files Affected:

	func_surf_energy_bal.c

	Description:

	Modified to use arno_evap rather than canopy_evap if LAI
	is 0, e.g. winter cropland.						KAC via TJB


Drop canopy snow if thin

	Files Affected:

	snow_intercept.c

	Description:

	Modified to drop canopy snow if it is especially thin
	(< MIN_SWQ_EB_THRES), which should improve the numeric
	stability of the canopy energy balance solution.			KAC via TJB



Bug Fixes:
----------


Various bugs in output variables

	Files Affected:

	global.param.sample
	output_list_utils.c
	output.PILPS-2E.ALMA.template
	output.TRADITIONAL.410.template
	output.TRADITIONAL.template
	put_data.c
	set_output_defaults.c
	vicNl_def.h

	Description:

	1. Shortened the names of variables whose names were too long
	2. Fixed typos in other names
	3. Added OUT_IN_LONG
										TJB


Uninitialized value of ice[] in transpiration()

	Files Affected:

	canopy_evap.c

	Description:

	Modified to initialize ice[] for all soil layers before computing
	available moisture (to avoid using uninitialized values later on).	TJB


Uninitialized value of mixdepth in solve_lake()

	Files Affected:

	lakes.eb.c

	Description:

	Now set mixdepth=0 for case of complete ice cover; this guarantees that
	it is initialized for all cases.					TJB


Incorrect sub-daily temperature interpolation when referencing GMT instead of
local time

	Files Affected:

	calc_air_temperature.c

	Description:

	Temperature interpolation didn't account for case in which min or max
	temperature could cross the boundary of the current day.  This can
	happen when referencing GMT instead of local time, for cells far away
	from 0 E longitude.  This has been fixed.				TJB


Water budget errors in snow pack on top of lake ice

	Files Affected:

	lakes.eb.c

	Description:

	Convert swq and surf_water from mm over lake to mm over ice fraction at
	beginning of solve_lake().  This was needed to avoid a water budget
	error since swq and surf_water were being converted to mm over lake at
	end of solve_lake().							TJB


Incorrect filenames in Makefile for variable output list functions

	Files Affected:

	Makefile

	Description:

	Changed ".c" to ".o" for output_list_utils.o, parse_output_info.o,
	set_output_defaults.o							TJB


Output variable OUT_DELSURFSTOR not functioning

	Files Affected:

	output_list_utils.c
	put_data.c

	Description:

	Default properties for OUT_DELSURFSTOR were not set in
	output_list_utils.c, and OUT_DELSURFSTOR was not assigned a
	value in put_data.c.  These problems have been fixed.			TJB


Pressure and vapor pressure output in wrong units

	Files Affected:

	put_data.c
	write_forcing_file.c

	Description:

	The output variables pressure and vapor pressure were output in Pa
	instead of kPa.  This has been fixed.					TJB


Changed OUT_SURF_TEMP from the average of T[0] and T[1] to exactly
equal to T[0].

	Files Affected:

	put_data.c

	Description:

	Previously, OUT_SURF_TEMP had been set to the average of node
	temperatures T[0] and T[1].  This has been changed to just T[0],
	since OUT_SURF_TEMP represents the surface skin temperature.		TJB


Added case of SPATIAL_FROST = TRUE in full_energy.c

	Files Affected:

	full_energy.c

	Description:

	This module needed to handle the case of SPATIAL_FROST = TRUE.		GCT


Fixed fread statements

	Files Affected:

	read_initial_model_state.c

	Description:

	Fixed fread checks to make sure correct number of items were read
	in rather than the size of the item read in.				JCA


Read in order incorrect in error_print_surf_energy_bal

	Files Affected:
	calc_surf_energy_bal.c
										GCT


Removed (1.-snow_coverage) from three equations where it did not belong

	Files Affected:

	func_surf_energy_bal.c
										GCT


setup_frozen_soil subroutine removed as it is never called

	Files Affected:

	frozen_soil.c
										GCT


Bug fix for previous bug fix to dt_baseflow calculation.

	Files Affected:

	runoff.c

	Description:

	Fixed bug arising from earlier fix to dt_baseflow
	calculation.  Earlier fix took residual moisture
	into account in the linear part of the baseflow eqn,
	but not in the non-linear part.  Now we take residual
	moisture into account correctly throughout the whole
	equation.								TJB


Sub-daily snow step for 24h wb mode not aggregating correctly

	Files Affected:

	snow_melt.c
	solve_snow.c
	surface_fluxes.c

	Description:

	The implementation of the canopy iteration in surface_fluxes() 
	caused the data structures containing the results
	for the sub-daily snow steps to be reset at the beginning
	of every sub-daily step, resulting in large water
	balance errors and incorrect daily results.  This has been fixed by
	creating distinct data structures to store results for 
	iterations (iter_*) and steps (step_*).  In addition,
	the units of snow melt were corrected to
	be consistent (mm) across functions.					TJB


Moved Implicit error counting above call for solve_T_profile.

	Files Affected:

	func_surf_energy_bal.c
										GCT


Moved ARNO/NIJSSEN conversion after calculation of max_moist.

	Files Affected:

	read_soilparam_arc.c
										GCT


Added ErrorFlag return value from initialize_prcp.

	Files Affected:

	lakes.eb.c
	LAKE.h
										GCT


Trap cases of T-errors matching variable ERROR for root_brent.

	Files Affected:
        
	calc_atmos_energy_bal.c
	calc_surf_energy_bal.c
	frozen_soil.c
	ice_melt.c
	root_brent.c
	snow_intercept.c
	snow_melt.c

	Description:

	Basically, what was happening was that when root_brent was called
	from calc_surf_energy_bal with func_surf_energy_bal as Function,
	then func_surf_energy_bal was ocasionally returning a value of ERROR to
	root_brent, and root_brent was then (in some cases) interpreting
	this as an actual surface temperature and continuing merrily on
	its way. So, there was an error in the model that was never
	caught. So, now root_brent checks for this and returns a value of
	ERROR if there is a problem. Because of this change, all of the
	routines that call root_brent had to be changed since they checked
	against an error of -9998 - now they check for
	values less than -998, since ERROR = -999.				JCA


Trap cases of negative soil moisture.

	Files Affected:

	full_energy.c

	Description:

	No longer resets ice content to previous time-step ice content if
	subsidence has occurred.						JCA


Fixed bug for read-in during EXCESS_ICE option.

	Files Affected:

	read_initial_model_state.c

	Description:

	Fixed bug for read-in during EXCESS_ICE option.				JCA


Memory errors for ARC_SOIL=TRUE and OUTPUT_FORCE=TRUE

	Files Affected:

	get_force_type.c
	get_global_param.c
	initialize_global.c
	read_soilparam.c
	read_soilparam_arc.c
	vicNl.c

	Description:

	Memory errors would occur when ARC_SOIL=TRUE and OUTPUT_FORCE=TRUE.
	In addition, the output files would not contain sufficient contents due
	to not closing properly.  These have been fixed.			TJB


Handling of cells missing from snowband file

	Files Affected:

	read_snowband.c

	Description:

	Previously, if SNOW_BAND = TRUE and a particular cell was not listed
	in the snowband file, VIC would abort.  Now, when VIC encounters
	cells with no information in the snowband file, VIC issues a warning
	and sets the cells to have 1 band, with Tfactor = 0 and Pfactor = 1.	KAC via TJB


Moved check for soil moisture > max to initialize_model_state.c

	Files Affected:

	read_initial_model_state.c
	initialize_model_state.c

	Description:

	For initializing the model state the check for soil
	moist > max moist was moved from read_initial_model_state.c
	to initialize_model_state.c. The check is performed on all veg types
	even if the area fraction = 0. There was a situation with
	EXCESS_ICE = TRUE when the model was trying to distribute
	moisture values that exceeded max moist to each thermal node
	in the wetland area when Cl = 0.					JCA


Incorrect limits on soil layer evap in runoff() for SPATIAL_FROST = TRUE

	Files Affected:

	runoff.c

	Description:

	Modified to correctly handle evaporation from spatially
	distributed soil frost.  Original version could produce
	negative soil moisture in fractions with high ice content
	since only total evaporation was checked versus total
	liquid water content, not versus available liquid water
	in each frost subsection.						KAC via TJB


Bug fixes in EXCESS_ICE calculations

	Files Affected:

	full_energy.c
	vicNl_def.h

	Description:

	Added MAX_SUBSIDENCE parameter to EXCESS_ICE option.
	Fixed bug in subsidence calculation.					JCA


Return ERROR instead of exiting in ice_melt.c module

	Files Affected:

	ice_melt.c

	Description:

	Return ERROR instead of exiting, if ice_melt could not converge to
	a solution in root_brent.						JCA


Force runoff() to use user-specified resid_moist

	Files Affected:

	runoff.c

	Description:

	Removed logic that reset resid_moist[i].  Previously,
	resid_moist[i] was reset to 0 for i > 0 when
	resid_moist[0] == 0.  Such resetting of soil properties
	was deemed unnecessary and confusing, since VIC would end
	up using different residual moisture values than those
	specified by the user.  If a user truly wants to specify
	residual moisture in all layers to be 0, the user should
	set these explicitly in the soil parameter file.  Also
	fixed typo in fprintf() on line 289.					TJB


Fixed error in EXP_TRANS formulation

	Files Affected:

	frozen_soil.c
	soil_thermal_eqn.c

	Description:

	Fixed error in EXP_TRANS formulation.					JCA


Updated default output file variable lists to match traditional 4.1.0
output files

	Files Affected:

	set_output_defaults.c

	Description:

	Updated to reflect variables present in traditional 4.1.0
	output files.  Previously the defaults matched the traditional
	4.0.6 output files.							TJB


Model aborts when TIME_STEP = 24 and STARTHOUR is not specified.

	Files Affected:

	get_global_param.c

	Description:

	Added validation of dt, start date, end date, and nrecs.		TJB


Miscellaneous bugs in frozen soils.

	Files Affected:

	frozen_soil.c
	soil_conduction.c

	Description:

	Fixed miscellaneous bugs (typos) in frozen soil equations.		JCA


Output file headers contain "hour" field despite output dt == 24 hours.

	Files Affected:

	write_header.c

	Description:

	Replaced all instances of global.dt with global.out_dt,
	since out_dt is the time interval used in the output files.		TJB


Liquid soil moisture sometimes falls below residual.

	Files Affected:

	runoff.c

	Description:

	Fixed the checks on the lower bound of soil moisture.
	Previously, the condition was
	  (moist[lindex]+ice[lindex]) < resid_moist[lindex]
	which led to liquid soil moisture falling below residual
	during winter conditions.  This has been changed to
	  moist[lindex] < resid_moist[lindex]
	to eliminate these errors and make the logic consistent
	with the rest of the code.						TJB


Variable "moist" in runoff() has different meaning than in other functions.

	Files Affected:

	runoff.c

	Description:

	Renamed all *moist* variables to *liq* if they only refer
	to liquid soil moisture.  This makes the logic much easier
	to understand.								TJB


Soil moisture drops below residual for sub-daily time step interval and
FULL_ENERGY or FROZEN_SOIL = TRUE.

	Files Affected:

	runoff.c

	Description:

	Fixed the checks on the lower bound of soil moisture.  Previously,
	the lower bound on soil moisture was
	  (liquid + ice >= residual moisture)
	and the way this bound was enforced was to reduce baseflow to
	bring total liquid + ice content back up to residual, but this
	compensation was limited so that the compensation would never
	be larger than baseflow (i.e. the compensation would never create
	negative baseflow).

	However, this condition had two main problems: 1. it allowed liquid
	moisture to fall to very low values (due to evap being over-estimated
	in arno_evap() and transpiration(), and/or Q12 being over-estimated
	earlier in runoff() due to bad numerics) in the presence of ice, and
	2.  it had limited ability to recover from these low values because
	baseflow (already small) wasn't allowed to be reduced below 0.

	This behavior has been replaced with the following conditions: For
	unfrozen soil, the new lower bound is
	  (liquid >= resid_moist)
	while for frozen soil, the new error condition is
	  (liquid >= min_liq_fraction * resid_moist)
	where
	  min_liq_fraction = the value returned by maximum_unfrozen_water()
	                     for a unit maximum moisture content.

	If overestimates of evap or drainage do occur, baseflow is allowed
	to be reduced below 0 to bring liquid water back up to the lower
	limit.  Then, further down in the code, if baseflow is negative,
	bottom-layer evap is reduced by (-baseflow) and baseflow is set to 0.	TJB



--------------------------------------------------------------------------------
***** Description of changes from VIC 4.1.0 beta r2 to VIC 4.1.0 beta r3 *****
--------------------------------------------------------------------------------


New Features:
-------------

Improved lake model

	Files Affected:
	CalcBlowingSnow.c, LAKE.h, IceEnergyBalance.c, ice_melt.c,
	initialize_atmos.c, initialize_lake.c, lakes.eb.c, read_lakeparam.c,
	solve_snow.c, surface_fluxes.c, vicNl.h, vicNl_def.h,
	water_energy_balance.c, water_under_ice.c

	Description:
	This fixes the following bugs:

		Lake model crashes when lakes fill up
		Lake model crashes when lakes dry out

	And introduces the following new features:

		Blowing snow sublimation computed over lakes
		Tuning of default lake profile (parabolic to square)

	The new parameter BETA in LAKE.h can be used to control the shape of the
	automatic lake profile (when the global option LAKE_PROFILE is FALSE).
	BETA=0.5 is strongly parabolic, while BETA=0.001 is almost square
	(vertical walls).

	If LAKE_PROFILE=FALSE, the lake parameter file from VIC 4.1.0 beta 2 can
	be used without any changes.  However, if LAKE_PROFILE=TRUE, the lake
	parameter file must now contain a (depth,area) pair for each lake node.
	See read_lakeparam.c for details.
	

VIC now supports ALMA input and output variables.

	Files Affected:
	Makefile, conv_force_vic2alma.c, conv_results_vic2alma.c, close_files.c,
	display_current_settings.c, dist_prec.c, get_force_type.c,
	get_global_param.c, initialize_atmos.c, initialize_global.c,
	make_in_and_outfiles.c, put_data.c, read_atmos_data.c, vicNl.h,
	vicNl_def.h, write_data.c, write_forcing_file.c

	Description:
	VIC now supports ALMA input and output variables.  To have VIC read ALMA
	input (forcing) variables, all that is required is to identify them by
	the appropriate names in the forcing section of the global parameter file.
	The appropriate names are listed in vicNl_def.h.  To have VIC write ALMA
	output variables, it is necessary to include the option ALMA_OUTPUT in the
	global parameter file and set it to TRUE.  This option does not affect
	model physics.


State file is now written at the END of the final timestep of the date indicated
in the global parameter file.

	Files Affected:
	dist_prec.c

	Description:
	Previous versions of VIC wrote the state file at the beginnning of the
	date indicated (via STATEYEAR, STATEMONTH, and STATEDAY) in the global
	parameter file.  For sub-daily model time steps, this meant that the
	simulation's end could not be aligned exactly with the writing of the
	state file, since the model would continue simluating for the remainder
	of the final day after writing the state file.  This created difficulties
	when running VIC in real-time simulations, in which the forcings are
	broken up into short time periods and the previous state file must be
	used as the initial condition for the next time period.

	Now that the state file is written at the end of the final timestep of the
	date indicated in the global parameter file, it is possible for the end of
	a forcing file, the end of the simulation, and the timing of the state file
	all to align on the same time.  This simplifies re-starting the model for
	a forcing file that begins immediately after the previous forcing file
	ended, since the state file now is equivalent to the initial condition at
	the beginning of the new forcing file.


EQUAL_AREA global parameter option

	Files Affected:
	display_current_settings.c, get_global_param.c, initialize_global.c,
	read_lakeparam.c, vicNl_def.h

	Description:
	New global parameter option.  When EQUAL_AREA is TRUE, all grid cells are
	assumed to have the same area, and the global parameter RESOLUTION is
	interpreted to store the grid cell area in km^2.  When EQUAL_AREA is FALSE,
	grid cells are assumed to have the same side length in degrees, and
	RESOLUTION is interpreted to store the grid cell side length in degrees,
	as before.



New vicInterp executable

	Files Affected:
	Makefile, check_files.c, close_files.c, get_global_param.c,
	read_soilparam.c, vicNl.c

	Description:
	Now users can create a stand-alone executable called "vicInterp" by
	typing "make interp".  This executable is a stripped-down version 
	of VIC that reads a set of forcing files and outputs diurnally-varying
	sub-daily forcings.  The interpolation scheme, based on the Thornton and
	Running (mtclim) algorithm, can convert from from daily Prec, Tmin, Tmax,
	and Wind to sub-daily Prec, AirTemp, Shortwave and Longwave radiation,
	Wind, Pressure, and air Density.  The interpolated forms of Prec, Wind,
	Pressure, and Density do not vary diurnally but are assumed constant
	during each day.

	vicInterp is vicNl, compiled with the OUTPUT_FORCE option set to TRUE.
	As such, vicInterp can read a standard VIC global parameter file.  While
	it will ignore many of the options, it understands (and requires) the
	following:
		TIMESTEP
		STARTYEAR
		STARTMONTH
		STARTDAY
		STARTHOUR
		ENDYEAR
		ENDMONTH
		ENDDAY
		GRID_DECIMAL
		FORCING1
		N_TYPES
		FORCE_TYPE
		FORCE_FORMAT
		FORCE_ENDIAN
		FORCE_DT
		FORCEYEAR
		FORCEMONTH
		FORCEDAY
		FORCEHOUR
		SOIL
		RESULT_DIR
	In addition, the following options are optional:
		BINARY_OUTPUT
		ALMA_OUTPUT

	For example, if FORCE_DT is 24 and TIMESTEP is 3, vicInterp will
	interpolate the input daily forcings to a 3-hour time step.  If
	ALMA_OUTPUT is TRUE, these output forcings will be standard ALMA
	variables.

	This new feature also involves an update to the OUTPUT_FORCE option
	that fixes a bug in which output files were not properly closed
	before exiting.  This prevented all of the output data from being
	completely written.


Bug Fixes:
----------

Large water balance errors in daily water balance mode when snow is present

	Files Affected:
	surface_fluxes.c

	Description:
	Fixed broken snow step in surface_fluxes.c.  Per-snow-step snow
	quantities were being reset at the beginning of each canopy-surface
	energy balance iteration, preventing snow characteristics from being
	accumulated over all snow steps, and resulting in large water balance
	errors.  Now per-iteration snow quantities are stored separately
	from per-snow-step quantities.


Aerodynamic resistance incorrect in output fluxes file

        Files Affected:
        IceEnergyBalance.c, LAKE.h, SnowPackEnergyBalance.c,
        calc_surf_energy_bal.c, full_energy.c, func_canopy_energy_bal.c,
        func_surf_energy_bal.c, ice_melt.c, lakes.eb.c, put_data.c,
        snow_intercept.c, snow_melt.c, solve_snow.c, surface_fluxes.c,
        vicNl.h, vicNl_def.h, wetland_energy.c

        Description:
        In 4.1.0 beta r2 and earlier, the aerodynamic resistance written to the
        output fluxes file rarely reflected the value actually used in flux
        computations.  This has been fixed.

        VIC actually computes an array of 3 different aerodynamic resistances,
        as follows:
          aero_resist[0] : over vegetation or bare soil
          aero_resist[1] : over snow-filled overstory
          aero_resist[2] : over snow pack
        VIC determines which element of the array to use depending on the
	current vegetation type and whether snow is present.  In addition, in
	most cases, VIC applies a stability correction to this aerodynamic
	resistance before using it in flux computations.  Furthermore, when the
	current vegetation tile contains overstory and snow is present on the
	ground, aero_resist[2] is used for snow pack flux computations and
	either aero_resist[1] or aero_resist[0] is used for canopy flux
	computations, meaning that two different aerodynamic resistances are in
	use in the same time step.

        However, VIC 4.1.0 beta r2 always wrote the uncorrected value of
        aero_resist[0] to the fluxes file.

        In 4.1.0 beta r3 and later, the value written to the fluxes file is the
        actual value used in flux computations, including any corrections that
        were applied.  In the case mentioned above in which two different
        aerodynamic resistances are in use at the same time, the one used for
        the snow pack is written.  In addition, the aerodynamic resistance of
        the canopy air/surrounding atmosphere interface is not tracked.


Aerodynamic resistance not correctly aggregated for output

	Files Affected:
	conv_results_vic2alma.c, put_data.c, vicNl_def.h

	Description:
	In previous releases, aerodynamic resistance (out_data->aero_resist)
	was aggregated by a simple area-weighted average over veg tiles.  This
	led to an aggregate value that was not the true effective resistance
	of the entire grid cell.  Since evaporation is proportional to
	1/aero_resist, it is (1/aero_resist), or the aerodynamic conductivity,
	that should be averaged over the grid cell.  Therefore, a new variable,
	out_data.aero_cond, was created for the purposes of aggregation.  After
	aggregation, out_data.aero_resist is computed as 1/out_data.aero_cond.

	The effect of the change is most pronounced when there is a large range
	of values of aerodynamic resistance among the veg tiles in a grid cell.
	The effective aerodynamic resistance, computed the new way, will tend
	to be smaller than the old way when the values cover a wide range.
	However, the effective aerodynamic resistance will never be smaller than
	the smallest value among the various veg tiles in the cell.
	


State variables for SPATIAL_FROST and LAKE_MODEL options not stored in state
file.

	Files Affected:
	dist_prec.c, initialize_model_state.c, read_initial_model_state.c,
	vicNl.h, write_model_state.c

	Description:
	State variables for SPATIAL_FROST and LAKE_MODEL options were not stored
	in state files, causing these variables to be reset to default values
	when reading from an initial state file.  This has been fixed.

	NOTE: this fix involves a change in the format of state files which
	makes state files used with 4.1.0 beta 3 incompatible with earlier
	releases (and vice versa).



ARNO_PARAMS global parameter option changed to BASEFLOW

        Files Affected:
        display_current_settings.c
        get_global_param.c
        global.param.sample
        initialize_global.c
        read_soilparam.c
        read_soilparam_arc.c
        vicNl_def.h

        Changed the name of the ARNO_PARAMS global parameter option to
        BASEFLOW.  The meaning of the ARNO_PARAMS option was actually
        opposite to its name: when ARNO_PARAMS was FALSE, VIC would
        interpret the first four parameters in the soil parameter file
        to be the standard ARNO soil parameters Ds, Dsmax, Ws, and c,
        while when ARNO_PARAMS was TRUE, VIC would interpret the first
        four parameters to be d1, d2, d3, and d4, the soil parameters
        used in Nijssen et al. (2001).  The new option now can take
        values of "ARNO" and "NIJSSEN2001".  When BASEFLOW == NIJSSEN2001,
        VIC assumes the soil parameter file contains d1, d2, d3, and d4.
        When BASEFLOW == ARNO, VIC assumes the soil parameter file
        contains Ds, Dsmax, Ws, and c.


Removed some snowband output

	Files Affected:
	write_data.c

	Description:
        Removed  net sw radiation, net lw, albedo, latent heat flux, 
	sensible heat flux, ground heat flux

STATE file option is now specified in global file, not in user_def.h at compile time

        Files Affected:

        display_current_settings.c
        dist_prec.c
        get_global_param.c
        initialize_global.c
        open_state_file.c
        user_def.h
        vicNl.c
        vicNl.h
        vicNl_def.h
        write_model_state.c

        Previously, to be able to read/write state files, VIC required
        users to define SAVE_STATE to be TRUE in user_def.h and
        recompile VIC, in addition to having the correct settings of
        INIT_STATE, STATENAME, STATEYEAR, STATEMONTH, and STATEDAY in
        the global parameter file.

        This was both unnecessary and confusing.  Setting SAVE_STATE
        to FALSE did not improve VIC performance noticeably, and
        having both a SAVE_STATE compile option and state file
        information in the global parameter file only led to mistakes
        in which the user made a change in one place without realizing
        a change needed to be made elsewhere.

        Now VIC behaves as follows:
        1. To read an initial state file, the following line
        must be in the global parameter file:
          INIT_STATE init_state_filename

        where init_state_filename is the name of the initial state
        file.  If this line is absent or commented out (preceded by a
        "#"), VIC will start from default initial conditions.

        2. To write a state file during the simulation, the
        following lines must be in the global parameter file:
          STATENAME  state_file_name
          STATEYEAR  state_year
          STATEMONTH state_month
          STATEDAY   state_day

        where state_file_name is the path and prefix of the file to
        save the state information in (the simulation date on which
        the state file is saved will be appended to the state file
        name), and state_year, state_month, and state_day are the
        year, month, and day of the simulation on which to save state.
        The state will be saved AFTER the FINAL time step of that
        date.  If all of these lines are absent or commented out, VIC
        will not save a state file.  If some (but not all) of these
        lines are present, VIC will give an error.


Now reads extra_veg from state file

        Files Affected:
        read_initial_model_state.c
               

Reverting from version 5.10 to 5.9 in surface_fluxes.c 

        Files Affected:
        surface_fluxes.c

        Energy balance errors had been introduced in the changes from 5.9 to 5.10.
        This is an intermittent problem that appears to depend on versions of FreeBSD
        when compiling with the optimization flag. This temporary reversion should
        be okay as long as the time steps are sub-daily.  GCT


Skip reading/writing of snow band for areafract <= 0

        Files Affected:
        read_initial_model_state.c
        write_model_state.c

        This will reduce the size of the statefile. GCT


Lake model energy terms NaN for southern hemisphere lakes

        Files Affected:
        lakes.eb.c

        The computation of ks in the eddy() function was attempting to compute
	  ks=6.6*pow(sin((double)lat*PI/180.),0.5)*pow(wind,-1.84);
	however, in the southern hemisphere, lat is negative and ks evaluates
	to NaN, causing lake temperatures to all become NaN.  This line has
	been replaced by
	  ks=6.6*pow(sin((double)fabs(lat)*PI/180.),0.5)*pow(wind,-1.84);
	This appears to fix the problem.	TJB


Miscellaneous fixes for memory leaks and uninitialized variables.

        Files Affected:

        free_dist_prcp.c
        get_global_param.c
        initialize_lake.c
        initialize_model_state.c
        lakes.eb.c
        make_dmy.c
        mtclim42_vic.c
        output_list_utils.c
        parse_output_info.c
        put_data.c
        read_veglib.c
        solve_snow.c
        vicNl.c
        vicNl.h

        Description:

        Miscellaneous fixes for memory leaks and uninitialized variables.
        Also changed comments in vicNl.h to reflect type changes introduced
        in 4.1.0._r4h   TJB


--------------------------------------------------------------------------------
***** Description of changes from VIC 4.1.0 beta r1 to VIC 4.1.0 beta r2 *****
--------------------------------------------------------------------------------


New Features:
-------------

"-v" and "-o" command-line options and display of run/compile-time options

        Files Affected:
        cmd_proc.c, display_current_settings.c, get_global_param.c, global.h,
        vicNl.c, vicNl.h, vicNl_def.h

        Description:
        In VIC 4.1.0 beta r2, if VERBOSE is TRUE, all compile-time options (from
        user_def.h) and run-time options (from your global parameter file) are
        displayed for you at the beginning of a model run.  If you are saving
        your model output to a log file, this information will be stored with
        your log, so you will have a record of the option settings that produced
        the results of that particular model run.

	The new "-v" option will display the release number of your vicNl
	executable.  For example, typing:
		vicNl -v
	gives
		***** VIC Version 4.1.0 Beta Release 2 *****

        Meanwhile, the new "-o" option displays the current compile-time
        options.  One benefit of this option is that you can see what the
        options in user_def.h were set to when vicNl was compiled, without
        having to start a model run.  Since your version of user_def.h may have
        changed since you compiled vicNl, this is the most reliable way to see
        what these options are set to.  For example:

	gen.hydro.washington.edu 175: vicNl -o

	***** VIC Version 4.1.0 Beta Release 2 - Current Model Settings *****

	COMPILE-TIME OPTIONS (set in user_def.h)
	----------------------------------------

	Output to Screen:
	OUTPUT_FORCE_STATS      FALSE
	VERBOSE                 TRUE

	Input Files:
	NO_REWIND               TRUE

	Output Files:
	LDAS_OUTPUT             FALSE
	LINK_DEBUG              FALSE
	OPTIMIZE                FALSE
	OUTPUT_FORCE            FALSE
	SAVE_STATE              FALSE

	Simulation Parameters:
	CLOSE_ENERGY            FALSE
	COMPUTE_TREELINE        FALSE
	LAKE_MODEL              FALSE
	LOW_RES_MOIST           FALSE
	QUICK_FS                FALSE
	SPATIAL_FROST           FALSE
	SPATIAL_SNOW            FALSE

	Maximum Array Sizes:
	MAX_BANDS               10
	MAX_FRONTS               3
	MAX_LAKE_NODES          20
	MAX_LAYERS               3
	MAX_NODES               18
	MAX_VEG                 12

	Snow Constants:
	NEW_SNOW_ALB            0.850000
	SNOW_ALB_ACCUM_A        0.940000
	SNOW_ALB_ACCUM_B        0.580000
	SNOW_ALB_THAW_A         0.820000
	SNOW_ALB_THAW_B         0.460000
	TraceSnow               0.030000

	Other Constants:
	LAI_WATER_FACTOR        0.200000
	LWAVE_COR               1.000000
	MAXIT_FE                25


Automatic recompilation on updates to *.h

        Files Affected:
        Makefile

        Description:
        In VIC 4.1.0 beta r1, updating a .h file and recompiling VIC would not
	result in recompilation of files that depend on the .h file, unless a
	"make clean" command was issued first.  Now, if any .h files are
        updated, all dependent .c files are recompiled on the next "make".


NEW_ARNO_TYPE global option is now ARNO_PARAMS

	Files Affected:
	get_global_param.c, read_soilparam.c, vicNl_def.h

	Description:
	Changed the name of the global option NEW_ARNO_TYPE to be ARNO_PARAMS.
	"NEW_ARNO_TYPE" is confusing, since this option will not be new
	forever, and doesn't refer to a "type" but rather a set of parameters.

	NOTE: This change requires the user to replace all occurrences of
	"NEW_ARNO_TYPE" in their global control files with "ARNO_PARAMS".


Bug Fixes:
----------

Spurious condensation at low temperatures

        Files Affected:
        arno_evap.c

        Description:
        Changed logic of evap limit check to avoid creating spurious
        condensation.  In VIC 4.1.0 beta r1, whenever evaporation > (liquid
        moisture - residual moisture), evaporation would be set to (liquid
        moisture - residual moisture).  However, at low temperatures, when most
        or all soil moisture is frozen, liquid moisture < residual moisture,
        causing (liquid moisture - residual moisture) to be negative.  Any non-
        negative evap would be greater than this, resulting in evap getting set
        to (liquid moisture - residual moisture), which would be negative (i.e.
        condensation).  This artificially created condensation in whatever
        amount necessary to bring liquid moisture up to residual, causing 1)
        large latent heat flux, 2) incorrect surface temperatures, 3) occasional
        inability for calc_surf_energy_bal to converge in root_brent, and 4)
        spuriously high runoff and baseflow.  Now there is an added condition
        that liquid moisture > residual moisture for evap to be capped at
        (liquid moisture - residual moisture).

        NOTE: This fix results in lower runoff and baseflow in unvegetated areas
        with frozen soils, and may require recalibration of soil parameters.


Incorrect baseflow limits

        Files Affected:
        runoff.c

        Description:
        In 4.1.0 beta r1, runoff.c checked for the wrong bounds on baseflow,
        allowing baseflow to become negative when liquid soil moisture < residual
        moisture.  These bounds have been fixed in 4.1.0 beta r2, as follows:
        baseflow is not allowed to exceed (liquid soil moisture - residual moisture);
        when baseflow < 0, baseflow is set to 0; when baseflow > 0 and the resulting
        soil moisture < residual moisture, water is taken out of baseflow and
        given to the soil as follows:
          if baseflow > (residual moisture - soil moisture), then
            baseflow -= (residual moisture - soil moisture);
            soil moisture += (residual moisture - soil moisture);
          else
            soil moisture += baseflow;
            baseflow = 0;

        NOTE: This fix may result in small changes in baseflow and evaporation.


Runs using initial state files starting at state file date rather than global
start date

        Files Affected:
        check_state_file.c

        Description:
        In 4.1.0 beta r1, check_state_file.c would increment the index of the
        forcing data array until it reached the record corresponding to the date
        stored in the state file.  This caused the simulation to start at the
        date at which the state file was saved rather than the start date
        specified in the global parameter file.  If the state file's date was
        earlier than the start date in the global parameter file, the index
        would be incremented until a segmentation fault occurred.  This has
        been fixed in 4.1.0 beta r2 so that the start date in the global parameter
        file is always the start date of the simulation.  The date stored in
        the initial state file is ignored.

        NOTE: If you have been relying on the state file to dictate when your
        simulations start, this fix may require you to change your global
        parameter file so that STARTYEAR, STARTMONTH, etc. reflect the start
        date/time you want.


Incorrect sublimation values for BLOWING option

	Files Affected:
	CalcBlowingSnow.c, IceEnergyBalance.c, SnowPackEnergyBalance.c,
	calc_surf_energy_bal.c, func_surf_energy_bal.c, ice_melt.c, lakes.eb.c,
	latent_heat_from_snow.c, put_data.c, snow_melt.c, solve_snow.c,
	surface_fluxes.c, vicNl.h, vicNl_def.h

	Description:
	Fixed 3 bugs in the sublimation terms: 1) sub_surface was wrong when
	snow step was not 1 hour, 2) sub_blowing was wrong under certain
	conditions, and 3) sub_blowing and sub_surface did not contain the
	contribution from lakes, even though sub_total did.  The fix
	establishes the convention that the internal variables VaporMassFlux,
	BlowingMassFlux, and SurfaceMassFlux always have units of kg/m2s; and
	that internal variables vapor_flux, blowing_flux, and surface_flux
	always have units of m/timestep.  Unnecessary terms were removed from
	the parameter lists of several functions.

	NOTE: The effects of this fix on major water balance terms such as
	runoff, baseflow, evaporation, etc. should be very small.

	NOTE 2: The lake contribution to sub_blowing is currently set to 0.


Negative incoming shortwave radiation at high latitudes

        Files Affected:
        mtclim42_vic.c

        Description:
        In 4.1.0 beta r1, when sub-daily shortwave radiation is estimated from
        daily min/max temperatures, negative values occasionally are calculated
        in coastal areas above the Arctic Circle in winter.  Now, if estimated
        sub-daily incident shortwave is negative, it is set to 0.0.


Undefined daily precipitation for deserts

        Files Affected:
        mtclim42_vic.c

        Description:
        In 4.1.0 beta r1, if a grid cell's annual precipitation (specified in
        the soil parameter file) is 0, then the adjusted daily precipitation
        calculated in mtclim42_vic.c ends up being undefined.  In 4.1.0 beta r2
        this has been fixed.

        More specifically, the MTCLIM 4.2 algorithm (which VIC uses for
        estimating sub-daily forcing values) was originally set up to expect a
        base precipitation as an input, and to translate this base precip into
        a site-specific precip via an adjustment function.  The adjustment
        function multiplies the base precip by the ratio of site_isohyet to
        base_isohyet to get site precipitation.  In calling the MTCLIM 4.2
        functions, VIC sets site_isohyet and base_isohyet to the grid cell's
        annual precipitation.  So this ratio should always = 1.  However,
        when annual precipitation is 0.0, this ratio is undefined.  So, the
        fix is to set the ratio to 1 when both site_isohyet and base_isohyet
        are 0 (or very small). (found by Liz Clark)


Snow_flux incorrectly set to Tcanopy in fluxes output file

	Files Affected:
	put_data.c

	Description:
	In 4.1.0 beta r1, the snow_flux output variable was incorrectly set to
	Tcanopy.  This has been corrected.


Incorrect value for sub_snow in fluxes output file

	Files Affected:
	write_data.c

	Description:
	Replaced output of sub_snow[0] in fluxes file with sub_total.


Special case in Penman equation

        Files Affected:
        penman.c

        Description:
        Changed
                if (vpd > 0.0 && evap < 0.0)
        to
                if (vpd >= 0.0 && evap < 0.0)
        to correctly handle evap when vpd == 0.0. (found by Justin Sheffield
        at Princeton)


Rint() function not supported on all platforms

        Files Affected:
        compute_dz.c, initialize_atmos.c, read_soilparam.c,
        read_soilparam_arc.c

        Description:
        Replaced rint(something) with (float)(int)(something + 0.5) to
        handle rounding without resorting to rint(), which isn't supported
        on all platforms.  (found by Justin Sheffield at Princeton)


Global parameter initialization

        Files Affected:
        initialize_global.c

        Description:
        Initialize ARC_SOIL, COMPRESS, and ARNO_PARAMS to FALSE.  Also changed
        limit on loop over forcing types from hard-coded 17 to variable
        N_FORCING_TYPES.  (found by Justin Sheffield at Princeton)


Bottom soil node thickness initialization

        Files Affected:
        initialize_model_state.c

        Description:
        Initialize soil_con->dz_node[Nnodes] to 0.0, since it is accessed in
        set_node_parameters().  (found by Justin Sheffield at Princeton)


Canopy evaporation and distributed precipitation

        Files Affected:
        surface_fluxes.c

        Description:
        Fixed initialization of canopyevap to initialize for every value of
        dist, rather than just dist 0. (found by Justin Sheffield at Princeton)


Output debug file error

        Files Affected:
        write_atmosdata.c

        Description:
        No longer close the debug file, since the next cell must write to it.
        (found by Justin Sheffield at Princeton)


Calculation of deltaH when FS_ACTIVE is FALSE

        Files Affected:
        func_surf_energy_bal.c

        Description:
        Added check that both FS_ACTIVE and FROZEN_SOIL are true before
        adjusting *deltaH.  This is just a safety measure; ice and ice0 should
        both be 0 when FS_ACTIVE is FALSE.  (found by Justin Sheffield at
        Princeton)


Root_brent error message clarification

        Files Affected:
        calc_atmos_energy_bal.c, calc_surf_energy_bal.c, frozen_soil.c,
	ice_melt.c, root_brent.c, snow_intercept.c, snow_melt.c, vicNl.h

        Description:
        Instead of printing warning or error messages, root_brent.c now
	passes descriptions of the errors to the functions that called it,
        leaving it to them to describe the specific error and its consequences.
	In 4.0.4, root_brent's messages sometimes were wrong.


Display current grid cell number for arc/info soil files

        Files Affected:
        read_soilparam_arc.c

        Description:
        In 4.1.0 beta r1, for arc/info-format soil files, the current grid
        cell is not displayed (for regular-format soil files, it is displayed).
        Now the current grid cell number is always displayed.


Inconsistent format in state file

	Files Affected:
	write_model_state.c

	Description:
	Removed initial space on veg/band info line in ASCII file.


Pending Issues:
---------------

Lake model crashes when lake level gets too high

Lake model crashes when lake level gets too low

COMPUTE_TREELINE option non-functional

	Description:
	This issue is waiting for completion of a treeline-computation scheme
	that allows landcover fractions to be stated explicitly as a function
	of elevation band.

Lakes smeared across multiple elevation bands

	Description:
	Currently, VIC assumes that the percentage of grid cell area covered by
	lakes is constant throughout the grid cell, and therefore when multiple
	elevation bands are specified for a grid cell, the lake percentage is
	distributed evenly across all elevations.  However, when modeling large
	lakes, this assumption does not hold; the bulk of the lake coverage may
	be due to a single large lake, which by definition cannot exist in
	multiple elevation bands.  Therefore, we need a way of explicitly
	specifying lake coverage as a function of elevation.


--------------------------------------------------------------------------------

Updates 7-30-2003: VIC 4.1.0 r1

    (1) Added support for ASCII as well as Binary state files (NOTE: ASCII
	state files were added for easier editing when trying to start the 
	model from observed state - Only Binary state files will produce
	identical runs if used to store and restart from the model state).
	Added BINARY_STATE_FILE to the global file, if TRUE state file I/O
	is in binary, if FALSE it is ASCII.
    (2) Made certain that local NOFLUX flag is set in calc_surf_energy_bal.c
	every time the routine is executed.
    (3) write_data.c: Corrected output of sub_snow variable to item [0] 
	rather than a point - will need to decide what parts of this array 
	are important to output.
    (4) Modified runoff.c so that only the top two layers are used when
	computing infiltration.  Previously, all but the bottom layer was 
	used.  Therefore this only affects you if you tried running with
	more than three layers.  This change was made as it makes more sense 
	for a situation where you are trying to improve the representation
        of the soil column by increasing the resolution (and perhaps depth)
	of the soil column.
    (5)	snow_utility.c (snow_density): Added check to keep compression from 
	aging from exceeding the actual depth of the snowpack.
    (6) solve_snow.c: Added check so that MELTING flag is only TRUE if melt
        occurs in the melt season - currently this is defined between March 1 
	and October 1.  Otherwise the MELTING flag can trigger rapid very 
	early season melt.

Updates 4-21-2003: Mering codes to form VIC 4.1.0 r0

-----------------------------------------------------------------------------

    From comparison of LatestAdminSource and LatestTestSource

    Dynamic allocation of as many arrays and structures as are currently 
    used by the VIC model cause electric fence (a memory debugging tool)
    to bog down/fail in the allocation process.  As the allocation of
    memory is unlikely to be the cause of new memory errors, hard wired 
    memory can be used instead, letting eleectric fence penetrate further
    into the code.
    alloc_atmos.c: (KAC) MEMORY_DEBUG update
	Modified so that dynamic allocation of the atmospheric data arrays
	does not occur when the debugging code is linked to the model.  
	This makes it possible to use electric fence to debug memory errors
	without changing the model code.  Previously, dynamic allocation
	of the atmospheric arrays was only skipped if VIC was configured 
	for optimization work.
    vicNl_def.h: (KAC) MEMORY_DEBUG update
        See alloc_atmos.c.

    Newest version of the model had water balance errors when using 
    distributed precipitation.  Model crashes were also witnessed.  Fixed
    errors introduced by other model updates.
    arno_evap.c: (KAC) DIST_PRCP fix
	Moved unit conversion of moist_resid outside of the distributed
	precipitation loop.  This prevents it from being multiplied by 
	D1 * 1000 twice for the DRY fraction of the grid cell.
    initialize_model_state.c: (KAC) DIST_PRCP fix
        Modified to initialize soil and vegetation parameters for the dry 
	grid cell fraction, if distributed precipitation is activated.
    snow_intercept.c: (KAC) DIST_PRCP fix
        Added check of intercepted before mass balance error calculation.  
	Since interception quantity can be greater than Wd_max when snow 
	is present, the routine could return dew values higher than maximum 
	in a time step when intercepted snow melted.  If there is no other 
	snow, and distributed precipitation is active this could cause the 
	model to crash in redistribute_during_storm as it will be unable to 
	conserve water when too much water is in the canopy.

    ** Make sure that all updates from surface_fluxes.c are accounted for
    after merge with 4.0.4beta **

-----------------------------------------------------------------------------

    From comparison of LatestAdminSource (updated above) and LaurasNewestSource

    IceEnergyBalance.c: (LCB) LAKE_WETLAND updates
        SnowFlux variable renamed to qf to match the lake-ice model 
	documentation.
    LAKE.h: (LCB) LAKE_WETLAND updates
        Modified some model parameters (need to check with Laura as to their
	applicability outside dissertation basins).  Also updated profiles
	for several subroutines.
    SnowPackEnergyBalance.c: (KAC) OTHER
        Certain constant definitions have been moved to vicNl_def.h to 
	standardize the location of model constants.  This is especially 
	useful for eliminating redundant and unused constant definitions.
    SnowPackEnergyBalance.c: (LCB) BLOWING_SNOW update
        New variables are passed to the routine and along to 
	latent_heat_from_snow to add the effects of blowing snow to the
	accumulation and ablation of the snowpack.
    calc_surf_energy_bal.c: (LCB) BLOWING_SNOW update
        Modified to include the effects of blowing snow in the surface 
	energy balance calulations.
    close_files.c: (LCB) LAKE_WETLAND updates
        Now closes the lake and wetland debugging file.
    full_energy.c: (LCB) BLOWING_SNOW updates
        Modified to handle blowing snow.
    full_energy.c: (LCB) LAKE_WETLAND updates
	Modified to output lake model variables during debugging.
    func_surf_energy_bal.c: (LCB) BLOWING_SNOW updates
        Modified to handle blowing snow.
    func_surf_energy_bal.c: (KAC) OTHER
        Modified so that the calculation of sensible heat flux so that 
	occurs in all model versions.  This eliminates a problem in
	WB mode where sensible heat flux was not set to 0, instead it
	showed the cumulative sensible heat flux from the snowpack.
    get_global_param.c: (LCB) BLOWING_SNOW update
        Added BLOWING_SNOW parameter
    get_global_param.c: (Jenny?) NEW_ARNO_TYPE
        Added parameter for reading Bart's new Arno parameters
    get_global_param.c: (LCB) LAKE_WETLAND updates
        Added PRT_LAKE parameter to output lake variables during debugging.
    ice_melt.c: (LCB) LAKE_WETLAND updates
        Modified method by which lake coverage fraction and ice height 
        are updated? ** Check with Laura **
    initialize_global.c: (LCB) LAKE_WETLAND updates
        Added the initialization of PRT_LAKE to the list of debugging flags.
    initialize_global.c: (LCB) BLOWING_SNOW update
        Added initialization of BLOWING_SNOW to the list of global parameters.
    initialize_lake.c: (LCB) LAKE_WETLAND updates
        Made improvements to the initialization process for lakes.  
        ** Check with Laura **
    initialize_model_state.c: (LCB) LAKE_WETLAND updates
        Modified to initialize lake variables.
    initialize_snow.c: (LCB) BLOWING_SNOW update
        Modified to initalize blowing_snow variable.
    initialize_soil.c: (LCB) LAKE_WETLAND updates
        Modified to initialize wetland soil moisture.
    initialize_veg.c: (LCB) LAKE_WETLAND updates
        Modified to get the maximum number of vegetation types passed to 
	it.  This allows the maximum number of vegetation types to include 
	the wetland vegetation fraction when the lake model is active.
    lakes.eb.c: (LCB) LAKE_WETLAND updates
        Modifications were made to improve handling of snow and ice and to 
	make the lake algorithm interact with the wetland algorithm.
    latent_heat_from_snow.c: (LCB) BLOWING_SNOW update
        Modified to handle the effects of blowing snow.
    make_dist_prcp.c: (LCB) LAKE_WETLAND updates
        Modified to allocate vegetation variables for the wetland 
	vegetation class.
    make_in_and_outfiles.c: (LCB) OTHER
        Modified to print notification that the output fluxes file will be 
	in a binary format.
    open_debug.c: (LCB) LAKE_WETLAND updates
	Modified to open lake model debugging file.
    put_data.c: (LCB) LAKE_WETLAND updates
        Updated output of lake variables to reflect algorithm changes.  
    put_data.c: (LCB) BLOWING_SNOW update
        Added output variables for blowing snow algorithm.
    read_lakeparam.c: (LCB) LAKE_WETLAND updates
        Modified to reflect updates to the lake and wetland algorithms.
    read_soilparam.c: (JA) OTHER
        Modified to convert from Bart's new Arno parameters into the 
        standard parameters (Dsmax, Ds, Ws, and c).
    read_vegparam.c: (LCB) BLOWING_SNOW update
        Added code to read in blowing snow parameters.
    snow_melt.c: (LCB) BLOWING_SNOW update
        Modified to handle blowing snow.
    soil_conduction.c (set_node_parameters): (KAC) OTHER
        Modified to correct differences between calculations to determine 
	maximum node moisture and node moisture, so that nodes on the 
	boundary between soil layers are computed the same way for both.
    soil_conduction.c (distribute_node_moisture_properties): (KAC) OTHER
        Modified to check that node soil moisture is less than or equal 
	to maximum node soil moisture, otherwise an error is printed to 
	the screen and the model exits.
    solve_snow.c: (LCB) BLOWING_SNOW update
        Modified to handle the effects of blowing snow.
    surface_fluxes.c: (LCB) BLOWING_SNOW update
        Modified to add the effects of blowing snow.
    user_def.h: (KAC) SPATIAL_SNOW
        Added TraceSnow to indicate the minimum depth of new snow required 
	to reset the snow surface albedo from the ablation to the 
	accumulation curve.
    vicNl.c: (LCB) LAKE_WETLAND updates
        Updated storage of lake water for water balance calculations.
    vicNl.h: (LCB,KAC,JA)
        Updated to reflect model changes.
    vicNl_def.h: (LCB,KAC,JA)
        Updated to reflect model changes.
    water_energy_balance.c: (LCB) LAKE_WETLAND updates
        Updated to reflect changes in algorithm structure.
    water_under_ice.c: (LCB) LAKE_WETLAND updates
        Updated to reflect changes in algorithm structure.
    write_data.c: (LCB) LAKE_WETLAND updates
        Updated output of model for lakes and wetlands algorithm.
        ** No lake variables are output when using LDAS format. **
    write_data.c: (KAC) BLOWING_SNOW update
        Added additional sublimation terms to LDAS and standard snow
        output files.

    Added:
    CalcBlowingSnow.c: (LCB) BLOWING_SNOW update
        Subroutine to compute the effects of blowing snow on snowpack 
	sublimation.
    wetland_energy.c: (LCB) LAKE_WETLAND updates
        Subroutine computes the surface energy balance for exposed 
	wetland vegetation.

-----------------------------------------------------------------------------

    From comparison of LatestAdminSource (updated above) and SOURCE_4.0.4beta

    CalcAerodynamic.c: (KAC) CANOPY_ENERGY_BALANCE update
        This routine was modified to store wind speed, aerodynamics
	resistance, reference height, roughness lengh and displacement
	height for three conditions: (1) snow-free, (2) snow-covered and
	(3) canopy wind speed.
    SnowPackEnergyBalance.c: No Changes Needed
    StabilityCorrection.c: (KAC) OTHER
        Moved definition of G (gravity) to vicNl_def.h to provide a 
	consistant location for all model constants.
    alloc_atmos.c: No Changes Needed
    arno_evap.c: No Changes Needed
    calc_longwave.c: No Changes Needed
    calc_rainonly.c: 
        Why does version 4.0.4 check to see if MAX_SNOW_TEMP <= MIN_RAIN_TEMP,
	rather than justt less than as the new version does?  Was this a bug
	or personal preference?  ** Check This **
    calc_surf_energy_bal.c: (KAC) CANOPY_ENERGY_BALANCE update
        Modified to work with the canopy energy balance updates.
    calc_surf_energy_bal.c: (KAC) SPATIAL_FROST update
        Modified to work with the spatial frost updates.
    calc_surf_energy_bal.c: (KAC) QUICK_SOLVE update
        Modified determine the minimum number of soil thermal nodes to 
	capture the maximum amount of the surface energy flux exchange.
	This reduces solution time with frozen soil significantly, but 
	increases energy balance errors.
    canopy_energy_bal.c: (KAC)
	This include changes for spatial snow and frost plus the new
	canopy energy balance.  No notes indicate fixes made to v4.0.4
	that need to be incorporated.
	- converted internal time step accounting to seconds from hours
	- added check to further restrict evaporative losses if ice content
	  is less than wilting point, otherwise dry soils can evaporate too
	  much if ice is present.
    check_files:
	Added lines for lake file
    check_state_file.c: (KAC) STATE_FILE updates
        Modified to work with new binary state file, fixed in version 4.0.4.
    close_files:
	Added lines for lake file
    compress_files.c: No changes made
	Modified to nice the backgrounded file compression processes.
    dist_prcp.c: (KAC) DIST_PRCP, TREELINE and STATE_FILE updates
	This will have to be carefully merged, as there are several fixes
	and improvements which should be maintained.  Some of these include 
	fixes to the accounting storms, updates for spatial frost and snow,
	updates for the state file, updates to account for the treeline.
	** This will involve some extra effort, especially since distributed
	precipitation variables are now account for in several locations. **
	--> DRY_TIME and STILL_STORM are now computed separatly for vegetation
	    types, this needs to be incorporated into state file.
    frozen_soil.c: (KAC)
	Modified so that soil layer ice content is only calculated if the 
	frozen soil algorithm is implemented and active in the current grid 
	cell.
    full_energy.c: (KAC)
	Added lake algorithm, and canopy energy balance updates.  Nothing
	appears to be needed from v4.0.4.
    func_surf_energy_bal.c: (KAC)
	updated for spatial snow and frost as well as the canopy energy 
	balance.  No important changes in v4.0.4 found.
    get_global_param.c: (KAC)
        Nothing new in v4.0.4, all additions are in the admin code.
    initialize_atmos.c: (KAC)
        Modified to initialize atmospheric pressues in Pa rather than kPa,
	all calculations now use Pa eliminating internal conversions.  Added
	treeline calculations. Added output of forcing file statistics (this
	is in addition to the output of forcing files which was in v4.0.3).
    initialize_global.c: (KAC)
        Added initialization of lake variables, blowing snow variables and
	QUICK_SOLVE for frozen soil energy fluxes.
    initialize_model_state.c: (KAC)
        Updated for canopy energy balance, new model state file, and lakes 
	and wetlands algorithm.
    initialize_new_storm.c: (KAC)
        Modified to work with spatial frozen soil.
    initialize_snow.c: (KAC)
        Modified to initialize blowing snow and spatial snow algorithm 
        variables.
    make_dist_prcp.c: (LCB)
        Modified to include wetland variables in the new structures.
    make_dmy.c:
        Only white space changes
    make_in_and_outfiles.c: (KAC)
        Lake model file control was added
    mtclim42_vic.c: (KAC) 
        Modified to compute all pressures in Pa rather than kPa.
    mtclim42_vic.h: (KAC)
        Definition of EPS constant moved to vicNl_def.h
    mtclim42_wrapper.c: (KAC)
        Changed calls to vicrerror to calls to nrerror.  Now handles 
	pressure in Pa, rather than kPa.
    open_state_file.c: (KAC)
        Updated to handle binary state file.
    penman.c: (KAC)
        Now handles pressure in Pa, rather than kPa.
    prepare_full_energy.c: (KAC)
        modified so that ice content is set to zero unless the frozen soil 
	algorithm is implemented and active in the current grid cell. 
    put_data.c: (KAC)
        modified to incorporate the effects of treeline calculations, the
	lake and wetland algorithm, spatial snow and frost algorithms, and 
	the canopy energy balance.
    read_atmos_data.c: (KAC)
        v4.0.4 multiplies nrecs by NF to check for a long enough file,
        v.4.1.0 multiples by dt.  I think new version is probably correct.
    read_initial_model_state.c: (KAC)
        Updated to handle binary state file.  Need to address storm state
	variables!
    read_snowband.c: (KAC)
	Modified to allocate treeline variable.  Will need to update to fix
        lake model/snow band problem!
    read_soilparam.c: (KAC, JA)
        Modified to read in spatial snow and frost parameters and to read in 
	Bart's new Arno parameter set.  
    read_soilparam_arc.c: (KAC)
        Modified to read in spatial snow and frost parameters and to read in 
	Bart's new Arno parameter set.
    read_vegparam.c: (DP,KAC)
        Modified code to update Wdmax based on LAI values read in for the 
	current grid cell.  If LAI is not obtained from this function, then 
	the values cacluated in read_veglib.c are left unchanged.
    redistribute_during_storm.c: (KAC)
        Modified to work with distributed snow and frozen soil.
    runoff.c: (KAC)
        Modified to handle spatial snow and soil frost.
    snow.h: (KAC)
        Added minimum SWQ for computing snow pack energy balance.  Added
        coefficients of shortwave attenuation through the snowpack.  
	Removed minimum SWQ for full coverage, now included in the soil 
	files.
    snow_intercept.c: (KAC)
        Modified to handle new variables required to close the canopy 
	energy balance.
    snow_melt.c: (KAC)
        Modified to handle partial snow cover.  Modified to assure that 
	ground heat flux is used properly in the snow surface energy 
	balance as well as imporving the handling of energy fluxes for 
	partial snow cover.  Modified to handle blowing snow.
    snow_utility.c: (KAC)
        Moved definition of G to vicNl_def.h.
    soil_conduction.c: (KAC)
	set_node_parameters:
        Modified to correct differences between calculations to determine 
	maximum node moisture and node moisture, so that nodes on the 
	boundary between soil layers are computed the same way for both.
	distribute_node_moisture_propertes:
	Modified to check that node soil moisture is less than or equal to 
	maximum node soil moisture, otherwise an error is printed to the 
	screen and the model exits.
	estimate_layer_ice_content:
	Modified to find ice content in spatial frost bands.
    solve_snow.c: (KAC)
	- Added partial snow cover and advection of sensible heat from local 
	bare patches.
	- Modified to pass the minimum depth of full snow cover as a variable 
	in soil_con rather than a globally defined constant.
	- Fixed check of new snow accumulation for setting understory flag to 
	use snowfall[WET] not snowfall.
	- Set MELTING flag to maintain melting albedo curve even during brief 
	periods of refreezing, until a snowfall exceeds SnowThres.
	- Modified to handle the effects of blowing snow.
	- Modified to handle closed canopy energy balance.
    surface_fluxes.c: (KAC)
        - Modified to handle partial snow cover.
        - Modified to iterate a solution for the exchange of energy between 
	the snowpack and the ground surface.
        - Modified to add the effects of blowing snow.
        - Fixed indexing problem for sub-daily snow model within daily water 
	balance VIC: hour (now hidx) is incremented by 1 rather than the 
	sub-daily time step, so the atmospheric forcing data is now properly 
	indexed.
	- Indexing fix sent SNOW_STEP to calc_surf_energy_bal rather than the 
	model time step, meaning that without snow the evaporation was 
	computed for SNOW_STEP hours rather than a full day.  This was fixed 
	by introducing step_inc to index the arrays, while step_dt keeps 
	track of the correct time step.
    svp.c: (KAC)
        Modified internal calculations to use kPa rather than Pa for 
	consistancy
    user_def.h: (KAC)
        Added options for lake model, closing the canopy energy balance,
	computing the treeline, computing statistics from the input 
	forcings, and controls for spatial snow and frost.
    vicNl.c: (KAC)
        - Added controls for lake model.
	- Updated storage of lake water for water balance calculations.
	- Modifed to add AboveTreeLine to soil_con_struct so that the model 
	can make use of the computed treeline.
	- Modified to initialize storm parameters using the state file.
        - Modified to start the model by skipping records until the state 
	file date is found.  This replaces the previous method of modifying 
	the global file start date, which can change the interpolation of 
	atmospheric forcing data.
	- Modified to store wet and dry fractions when intializing water 
	balance storage.  This accounts for changes in model state 
	initialization, which now stores wet and dry fractions rather than 
	just averaged values.
    vicNl.h: (KAC)
        Added definitions for treeline and forcing stats subroutines and
	made some needed modifications to subroutine definitions due to
	other changes, however, other changes still need to be made.
    vicNl_def.h: (KAC)
        Modified to handle lake algorithm, blowing snow, spatial snow and
	frost, and treeline calculations.
    write_data.c: (KAC)
	- Made hour a variable in all output data file formats even if the 
	model is run at a daily time step.  Also modified all output files 
	to account for new variables introduced by the spatial frost and 
	snow algorithms, the lake algorithm and the PILPS 2e study.
	- Added energy fluxes to snow band output files.
	- Updated output of model for lakes and wetlands algorithm.  Added 
	output of blowing snow sublimation to LDAS and standard snow output 
	files.  ** No Lake Variables are included in the LDAS output format. 
	**
	- Modified LDAS SWQ output, so that it is multiplied by 10 instead 
	of 100 before being converted to a short integer.  This reduces 
	stored value precision to 0.1, but increases the maximum storable 
	SWQ, which was exceeded in previous LDAS simulations.
        - Eliminated different formats between energy balance and water
	balance model output.
    write_debug.c: (KAC)
        Modified to work with closed canopy energy balance.
    write_forcing_file.c: (KAC)
	Modified to output pressures, which are handled internally in kPa, 
	as Pa for backward compatability.
    write_layer.c: (KAC)
        Modified to handle spatial soil frost.
    write_model_state.c: (KAC)
	- Rewritten to handle updates to vicNl_def.h and to write the file 
	as binary to minimize write time and differences with simulations 
	started with the state file.
	- Model is now restarted with the correct values for mu and 
	LAST_STORM.
        ** Still need to account for differences with distributed 
	precipitation flags, which now differ with vegetation - also need
	to check storage of lake variables, blowing snow variables and 
	MELTING flag.

-----------------------------------------------------------------------------

June 4, 2003: VIC release 4.0.4beta r2

This covers bugs found during tests with the snow algorithm.

        solve_snow.c: Bug found by KAC
	    Counter for number of days since last snowfall was 
	    incremented twice in the MELTING update.  This has been
	    fixed.
	solve_snow.c: modification by KAC
	    Added check so that MELTING flag is only TRUE if melt
	    occurs in the melt season - currently this is defined
	    between March 1 and October 1.  Otherwise the MELTING
	    flag can trigger rapid very early season melt
	write_model_state.c, read_initial_model_state.c, open_state_file.c,
	check_state_file.c: Modification by KAC
	    Modified to handle both ASCII and BINARY state files.
	    NOTE: ASCII mode is designed to make it easier to create 
	    a state file for initializing a point model.  It includes 
	    all features of the Binary state file, but values are 
	    truncated so the resulting restart will not be identical.
	    If you need an exact restart, use the Binary files.  Also
	    removed ice content from the state file as it is computed
	    at the beginning of each time step, so storing its value
	    is unnecessary.

-----------------------------------------------------------------------------

April 23, 2003: VIC release 4.0.4beta r1

This covers bug fixes found by beta testers and fixed in the version of 
the code bundled with this file.

	surface_fluxes.c: (found by Ingjerd Haddeland)
	    Indexing fix sent SNOW_STEP to calc_surf_energy_bal rather
	    than the model time step, meaning that without snow the 
	    evaporation was computed for SNOW_STEP hours rather than a
	    full day.  This was fixed by introducing step_inc to 
	    index the arrays, while step_dt keeps track of the correct
	    time step. 

-----------------------------------------------------------------------------

March 27, 2003: VIC release 4.0.4beta

This release covers patches for several bugs found in 4.0.3, which were 
never formally released (i.e. the downloadable source code was modified, 
but no major announcement was made).  It also includes other fixes and 
modifications that have been identified as being needed prior to releasing 
version 4.1.0, which will involve several significant changes (including 
lakes & wetlands, spatial snow & frost, and a closed canopy energy balance).

Modifications:

	Snow albedo update: (found by Keith)
	     In previous releases, the snow albedo function has been hyper 
	     sensitive to trace amounts of snowfall during the melt period.  
	     Whenever new snow falls or the cold content falls below 0, the 
	     albedo is switched from the ablation curve to the accumulation 
	     curve.  This curve is then followed until the cold content 
	     exceeds 0, indicating it is in the spring melt season.  This is 
	     fine when accounting for thin early season snowpacks or mid-
	     season melt events, however, a cold snap or light dusting of 
	     snow should not reset the snowpack albedo to much higher winter 
	     values for days or weeks at a time.  This release of the model 
	     monitors the state of pack with the variable MELTING.  This 
	     flag keeps the snowpack on the ablation curve unless new snow-
	     fall exceeds a threshold (TraceSnow in user_def.h) indicating 
	     that the top of the snowpack should be represented by the albedo 
	     of the new snow.
	Model initialization: (found by Ulysses and Keith)
	     In previous releases, the initialization of soil layer moist[] 
	     and ice[] was within a second set of loops for band and 
	     vegetation, using the same counters.  Because of the dual use
	     of counters, initialization was not completed correctly.  The
	     primary effect of this was that thermal node values beyond the
	     first vegetation type were not correctly initialized, which 
	     caused the model to crash during some simulations with full
             energy or frozen soils active.  Without frozen soil, most 
	     simulations would compensate for the problem within their
	     spin-up time, so it is unlikely that this bug impacts any 
	     simulations not employing frozen soil.
	Snow time step: (found by Andy, et al.)
	     The snow algorithm needs to run sub-daily for the energy balance
	     components to function properly.  This means that for daily 
	     simulations, the snow model must be solved at a finer (sub-daily)
	     time step.  In the previous release, initialize_atmos.c stored
	     sub-daily met data in each days variable using positions (e.g. 
             0,1,..8 for 3 hour data).  In surface_fluxes.c the model indexed 
	     the sub-daily time steps used by the snow algorithm with hours 
	     (e.g. 0,3,6,...21 for three hour data).  This means the arrays
	     were incorrectly indexed and the resulting model simulations 
	     would be wrong.  The fix implemented here has been tested under 
	     several model configurations and is deemed the official version.
	     WARNING: There are several versions of this fix circulating,
	     please update your code to this version - the previous fixes
	     may not work in all circumstances!
	FROZEN_SOIL active flag: (found by Ed and Justin)
	     The cause of the problem is a bug in the code that occurs when 
	     the global frozen soils flag (FROZEN_SOILS) is set to true but 
	     the individual cell frozen soil flag (FS_ACTIVE) is set to 
	     false. This causes the change in soil heat storage to be 
	     calculated incorrectly.  This was fixed by adding additional
	     conditions within frozen_soil.c and initialize_model_state.c,
	     which verify the FS_ACTIVE is TRUE before running 
	     estimate_layer_ice_content.  This avoids the problem of the 
	     soil layer ice content being set to a positive value, ignored
	     by the rest of the model.
	Vapor pressure: (Keith)
	     All internal vapor pressure calculations are now done in 
	     Pascals.  Previous release versions, switched between Pa
	     and kPa, so this simply removes the extra step.  The input
	     file format is unchanged, so there should be no change to
	     model output (model might run slightly faster, but it is
	     also unlikely that this will be witnessed by a normal user).
	Constant dew despite changing LAI: (Dave Peterson)
	     The modification of read_vegparam.c to update LAI based on 
	     a grid cell specific value did not change the values of Wdmax.  
	     Wdmax values were computed in read_veglib.c based on the 
	     default LAI values, so they did not necessarily reflect the 
	     actually LAI values used for the grid cell.  Values for Wdmax 
	     are now computed in read_vegparam.c whenever GLOBAL_LAI are 
	     provided.  The effects of this change will change in magnitude
	     based on how different the cell LAI values are from those in
	     the default library file.
	DRY_TIME error: (Reinur Schnur)
	     DRY_TIME in dist_prcp.c was incremented by the time step in 
	     hours.  Then to see if the current rain was part of the same 
	     storm or the start of a new one, DRY_TIME was checked to see 
	     if it was greater than or equal to 24/dt.  This compares 
	     DRY_TIME in hours to the model time step.  The "/dt" has been 
	     removed, so now DRY_TIME is checked versus the hours since the 
	     last storm.
	State file: (KAC)
	     *** WARNING: This may require modifications to your global file ***
	     The state file has been modified to account for model updates.
	     It has also been converted to write binary files - this makes 
	     them less convenient to edit, but means that model starts using
	     the same forcing, soil and vegetation files will produce the
	     exact same results.  There has also been a slight change in how
	     the global file is set up to restart the model.  The global
	     file should now have the same year, month, day and hour as the 
	     original global file - the VIC model will compute the number of
	     records to skip at the beginning to reach the point where the 
	     model state was saved.  This means that calculations to yield
	     sub-daily metrological forcings from daily forcings will produce 
	     the exact same forcing values -> this also means that restarted
	     simulations will be exactly the same as the original run.  Slight
	     variations in the model results were also introduced because the
	     method for storing soil node depths led to the possibility of 
	     very small differences in dz_node for the restarted model.  
	     Previous versions also did not store the snowpack cold content,
	     this meant that for restarts during winter, the snowpack albedo
	     might start on the ablation curve (as cold content was initialized
	     to 0 and not to a value less than 0) rather than the accumulation
	     curve.  This could lead to slight differences in the snowpack
	     if no new snow fell and the snowpack was not melting - but
	     after 10 years of simulations the differences were minor.  As 
	     noted above the new version of the state file should allow the
	     model to be restarted and to produce exactly the same results as
	     the original complete result.  If there are cases where this is
	     not true, please report.  If you edit the read/write model state
	     functions - BE VERY CAREFUL to edit Nbytes to reflect any changes.
	COMPUTE_TREELINE: (KAC and LCB)
	     This is an added feature which computes the treeline elevation
	     in the current grid cell and does not include vegetation 
	     fractions with overstory in the grid cell averages for snow bands
	     that exceed the treeline elevation.  This feature was added
	     to the model to reduce the appearance of "glaciers" in high
	     elevation snow bands.  It computes average annual July air
	     temperatures using the temperature data from the atmospheric
	     forcing files (WARNING - elevation of treeline may change if
	     the period of forcing data used changes).  It then lapses the
	     average annual July air temperature to locate the elevation 
	     at which it equals 10C.  This is assumed to be the treeline,
	     so vegetation types with overstory in snow bands with average 
	     elevations higher than this, are excluded from the grid cell
	     average variables in put_data.c.  For the time being full 
	     energy and water balances are still computed for these 
	     vegetation fractions, and no attempt is made to verify that
	     a snow band has a non-overstory vegetation type that can be 
	     expanded to represent the coverage area lost due to the 
	     exclusion of the overstory fraction.
	     

-----------------------------------------------------------------------------

August 15, 2000: VIC release 4.0.3

This release fixes a problem with the implementation of va_arg that 
causes run time errors on some systems.  Previous releases of the code 
worked correctly on the LINUX and freeBSD systems where it was tested. 
However on some systems (including Sun Ultra-2s) character variables 
passed with va_arg are changed into integers so reading a character from 
the argument list does not produce the value sent to the routine.  The
character flags used by VIC to indicate if there is snow present and if
the frozen soil algorithm has been activated have now been converted to 
integers, which should make the va_arg call work on all systems.

Also fixed in this release was a check in dist_prec.c to see if it is 
still raining which actually used the memory address of the 
precipitation variable rather than the daily value in the check.

MODIFIED FILES:
	read_atmos_data.c	-	Fixed input file time step check
	write_forcing_files.c	-	Added free statements for pointers
	calc_surf_energy_bal.c	-	Converted char flags to int
	dist_prec.c		-	Fixed logical statement error
	frozen_soil.c		-	Converted char flags to int
	func_surf_energy_bal.c	-	Converted char flags to int
	initialize_atmos.c	-	Added flag for output forcing
	vicNl.h			-	Converted char flags to int
	vicNl_def.h		-	Converted char flags to int

-----------------------------------------------------------------------------

July 19, 2000: VIC release 4.0.2

Two new pre-processor options have been added to VIC as well as minor 
modifications to two subroutines.  

If set to TRUE the NO_REWIND pre-processor option stops the VIC model from
rewinding the soil and vegetation parameter input files for each new grid
cell.  This reduces run times but requires that all input files are in the
same order as the soil parameter file.  

If set TRUE the OUTPUT_FORCE pre-processor option blocks off the main
model and only reads the provided forcing files. Once VIC has estimated
the missing forcing parameters the full forcing data set for the defined
simulation period is output to a series of gridded forcing files.  The
gridded forcing files are written to the RESULTS directory defined in the
global control file with the prefix "full_data_".  The new files are in
Binary or ASCII depending on the setting of BINARY_OUTPUT.

The error messages in get_global_param.c have been modified so that the
correct file is referenced when telling the user to change values found in
the model source code.

In read_soilparam.c, the soil parameters are defined only if the current
grid cell is run, otherwise the line in the file is skipped and soil_con
is returned without new data values.

-----------------------------------------------------------------------------

May 30, 2000: VIC release 4.0.1

Increased use of the released VIC model code has lead to the
discovery of a couple of minor bugs.  This release fixes those bugs as
well as introducing a improved precipitation correction algorithm based on 
Yang et al. 1998.  Unless you have encountered these problems or are
trying to correct precipitation undercatch due to wind, in the VIC
model, your results will not be impacted by these fixes.

MODIFIED FILES:
    correct_precip.c       - changed to WMO correction equation for
                             NWS 8" standard gauge.
    full_energy.c          - modified to handle WMO correction.
    initialize_atmos.c     - modified to handle WMO correction.
                             fixed error in estimating minimum daily 
                             temperature from sub-daily temperatures.
    make_in_and_outfiles.c - removed line that opened the state file
                             again for each new grid cell.
    open_state_file.c      - modified comments.
    put_data.c             - modified to handle WMO correction.
    snow_utility.c         - cleaned source code.
    solve_snow.c           - modified to handle WMO correction.
    surface_fluxes.c       - modified to handle WMO correction.
    vicNl.h                - modified to handle WMO correction.
    vicNl_def.h            - modified to handle WMO correction.

REFERENCE:

    Yang, D., B. E. Goodison, J. R. Metcalfe, V. S. Golubev, R.
    Bates, T. Pangburn, and C. L. Hanson, Accuracy of NWS 8" Standard 
    Nonrecording Precipitation Gauge: Results and Application of WMO 
    Intercomparison, J. Atmos. Oceanic Tech., 15, 54-68, 1998.
             

-----------------------------------------------------------------------------

Date: May 16, 2000

From: Keith Cherkauer

Topic: Release of VIC 4.0.0

The code for VIC release 4.0.0 has undergone several months of tests (as
VIC release 3.3.0 Beta) and has now been deemed ready for release to the
general public.  This document is designed to provide information
concerning changes in the model between the last release version (3.2.1)
and the current version.

There is no formal users manual, information about how to use the current
version can be found at
http://www.hydro.washington.edu/Lettenmaier/Models/VIC/VIChome.html.  
Information about the basic model design can be found in Liang, et al.
(1994), while the rewrite of the source code as well as the addition of
cold season processes is described in Cherkauer and Lettenmaier (1999).

The VIC macroscale hydrologic model was developed as a research tool.  As
such it is the users responsibility to verify that it is working correctly
when applied to new situations.  When using the VIC model reference should
be made to Liang, et al. (1994) and Cherkauer and Lettenmaier (1999) as
well as an acknowledgment that the code was received from the University
of Washington.  Other important papers relating to the development of the
VIC model are included on the home page and in the source code.

Possible bugs in the code should be reported to
vicadmin@hydro.washington.edu.  ALWAYS CHECK YOUR INPUT FILES!  Most
"bugs" are actually caused by trying to run the model with bad parameters
or forcing data.  The VIC model will run limited checks to find common
major errors but in most cases it will attempt to run with the bad values.  
If after checking all of your input data you still believe you have found
a bug in the model, send an e-mail including the complete output from the
model as well as a description of the problem and the files necessary to
run the model to recreate the code (if files are large please put a
compressed tar file in
ftp://ftp.ce.washington.edu/pub/HYDRO/vicadmin/TEMP).  Outdated and
modified versions of the code are the responsibility of the user to debug.  
Modifications made to the code, which may improve the general model
performance, may be submitted to vicadmin@hydro.washington.edu for
possible inclusion in future versions of the code.

VIC release 4.0.0 represents a major change to the source code from
version 3.2.1.  It is strongly recommended that if you were using version
3.2.1 or earlier versions that you update with a complete copy of the new
code.

Major changes from release version 3.2.1 to 4.0.0:

- Radiation Estimation Update: The routines to estimate shortwave and
longwave radiation as well as vapor pressure from daily minimum and
maximum temperature and precipitation have been updated to correspond to
the algorithm described by Thornton and Running (1999).  These routines
provide significantly improved radiation estimates especially in regions
outside the continental United States.

- Model Core Update: The core of the VIC model was rewritten so that all
modes of the model (water balance, energy balance, etc.) make use of the
same model code.  This makes it easier to modify the model and have
modifications apply to sll modes, it also allows the model to run with new
combinations of algorithms (i.e. full energy balance mode with the finite
difference ground heat flux solution).

- Soil Moisture Transport Update: The frozen and thawed sub-layers added
to the model for the original frozen soil algorithm have been removed.  
This makes the soil drainage routine cleaner and faster.  Frozen soils now
estimate full layer ice contents from the ice content at each soil thermal
node.  Without being confined by sub-layers, the frozen soil algorithm can
now be applied to regions of permafrost.

- Forcing File Control Added: Version 4.0.0 moves controls of the forcing
file format and data types into the global control file.  The model can
now handle most ASCII column and short int Binary files without writing
new subroutines and recompiling the source code.

- Pre-processor Options Added: There are now more option flags in the
source code headers to control which parts of the model are in fact
compiled.  This allows the model functionality to be adjusted without the
addition of computationally intensive conditional switching statements.

- Model State File: With the release of version 4.0.0 separate snow and
soil initialization files have been combined into a single model state
file.  The state file can be created outside the model for starting
simulations with prescribed initial conditions, or the model state can be
saved by VIC at a specified date.  Note that currently there will be small
differences between a full and a warm started simulation because radiation
and vapor pressure are estimated using forcing data from the simulation
period, not from the full dataset included in the forcing file.  It also
does not store wet and dry fraction information, when running with
distributed precipitation the model is restarted using average soil
conditions.

References:

Liang, X., D. P. Lettenmaier, E. F. Wood, and S. J. Burges, A simple
hydrologically based model of land surface water and energy fluxes for
GSMs, J. Geophys. Res., 99(D7), 14,415-14,428, 1994.

Cherkauer, K. A., and D. P. Lettenmaier, Hydrologic effects of frozen
soils in the upper Mississippi River basin, J. Geophys. Res., 104(D16),
19,599-19,610, 1999.

Thornton, P.E. and S.W. Running, An improved algorithm for estimating
incident daily solar radiation from measurements of temperature, humidity,
and precipitation, Ag. For. Met., 93, 211-228, 1999.

