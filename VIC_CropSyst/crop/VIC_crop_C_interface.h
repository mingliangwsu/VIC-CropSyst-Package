#ifndef VIC_crop_C_interfaceH
#define VIC_crop_C_interfaceH
#ifdef VCS_V5
#include "vic_def.h"
#else
#include "vicNl_def.h"
#endif
#include "CropSyst/source/cs_scenario.h"

/** \file
\author Roger Nelson   Biological System Engineering  Washington State University
\email rnelson@wsu.edu
\date   2010-03-10

The functions are listed here in the general order of the sequence
they should be called in the daily loop unless otherwise indicated.
These functions provide an interface for programs written in C
to the VIC_crop crop model which is implemented in C++.
If your wish to use this model in a C++ do not uses these function,
use the C++ classes directly.

This header file is pure C, the corresponding .cpp implementation file
implements this C interface for the declaration of the function,
but has C++ implementation.

The default metric units system is generally meter-kilogram.
The timestep is implicitly daily and all variable values
are daily values unless otherwise indicated.
For example we say "water uptake for the day was 0.05 meters",
not "the water uptake rate was 0.05 meters/day".
Even though the values are numerically the same and could be
considered the same outside of the CropSyst context;
within the context of the CropSyst program, the timestep
has either been 'factored out'  (mostly) of the equations,
or equations that do still have a timestep element
have clear indication of the units.
In this case the time value or time units would be specified
as a parameter to the function.
Currently everything is a daily timestep.

This code is commented using
<A HREF="http://en.wikipedia.org/wiki/Doxygen">Doxygen</A>
<A HREF="http://www.stack.nl/~dimitri/doxygen/">comment markups</A>
 */
/* __________________________________________________________________________ */
/**
All the following functions VIC_CropSyst_XXX() apply to the current active crop.
If there is no active crop the functions returns immediatly with no effect.
 */

/**
horizon number
Number of the soil layer or soil horizon.
Soil horizons are indexed from 1.
The horizon number corresponds to the VIC layer number -1.
(horizon index 0 is reserved, if the caller indexes soil
soil layers from 0, make sure to then add 1.).
 */
#ifdef __cplusplus
extern "C" {
#endif
   // 101019 check correctedextern double VIC_CropSyst_get_return;
   /* _______________________________________________________________________ */
#ifndef USE_CROPSYST_SUBLAYERING
   void VIC_CropSyst_know_soil_layer(int horizon_number, double thickness,
      double sand_percent, double clay_percent,
      double preferred_field_capacity           // 101019
      , double preferred_permanent_wilt_point   // 101019
      , int subdivide);
   /**< \fn
   This function is used when CropSyst soil files are not available.
   This function will not be used for VIC runs, but
   it will remain available for debugging and testing (Roger 11-01-31).
   \brief Informs the crop model of the soil layer profile.
   \param [in] horizon_number See horizon number above
   \param [in] thickness Indicates the soil horizon thickness (meter)
   \param [in] percent sand soil texture
   \param [in] percent clay soil texture
   \param [in] preferred field capacity
   The volumetric (m3 water / m3 soil) field capacity of the soil layer.
   \param [in] preferred permanent wilt point
   The volumetric (m3 water / m3 soil) permanent wilting point of the soil layer.
   \param [in] subdivide
   When 0 the horizon will not be divided into sublayers.
   (0 is used when a specific soil sublayer scheme is intended or known).
   When 1 (recommended) the horizon will be automatically
   divided into sublayers (however many is deemed suitable by the model).
   (For the VIC model, 1 should be used).
   When 2 or more, [this option is not yet implemented]
   the horizon will be subdivided by the specified number of layers.
   \details
   This setup function only needs to be called once for each
   soil horizon after the crop has been created.
   This function must be called in assending horizon number sequence
   Where horizon 1 is at the top of the soil profile
   (usually the evaporative layer at the soil surface),
   horizon 2 is the next layer below layer 1 etc...)
   The largest horizon number passed in the calls to this function
   is assumed to be the number of horizons in the soil profile.

   The values for preferred field capacity and permanent wilt point
   calculated from VIC soil hydraulic properties database may be preferred
   to ensure the soil hydrology used in the crop soil water model is consistent with
   the properties used by VIC.  If 0.0 is passed for the preferred value,
   it will be calculated using pedotransfer functions by Saxton et al..
    */
#endif
   /* _______________________________________________________________________ */
   void VIC_know_soil_layer_thickness(double horizon_1_thickness_m,
      double horizon_2_thickness_m, double horizon_3_thickness_m);
   /**< \fn
   \param [in] The three parameters are the soil horizon/layer thickness (meter)
   For VIC layers 0,1 and 2 (horizon 1,2 and 3)
   See horizon number above.
   \details
   This setup function only needs to be called once for each
   soil horizon after the crop has been created.
    */
   /* _______________________________________________________________________ */
   double VIC_CropSyst_know_soil_water_content(int horizon_number,
      double water_content_volumetric);
   /**< \fn
   \brief Informs the crop model of the current soil water content (moisture)
   for the specified horizon.
   \param [in] horizon_number Number of the soil layer or soil horizon.
   Soil horizons are indexed from 1.
   (Note that VIC layers are indexed from 0)
   (horizon index 0 is reserved).
   \param [in] water_content_volumetric
   <A HREF="http://en.wikipedia.org/wiki/Water_content">Volumetric water content</A>
   of the soil layer.
   \details
   Previously, this was called everyday
   before before calling  VIC_CropSyst_process().
   Now this function is called once on the creation of a crop
   to initialize the VIC_crop soil hydrology to the current water content.
   Starting with VIC soil layer 0 (horizon 1)
   to layer 2 (horizon 3).
   (OBSOLETE:
   The method must be called for each soil layer at the beginning
   of the daily time step)
   \return If the new water content for the horizon exceeds
   the field capacities of all the crop soil sublayering system layers
   and thus cannot be fully accomodated in the crop model's
   soil this can only be considered a drainage
   (depth in millimeter).
   It is assumed this drainage always be 0 or a negligible value
   (due to rounding error).
   VIC may need to either do something with this returned drainage value
   (which should be negligable) or simply ignore it but there could be
   a water balance error.
    * */
   /* _______________________________________________________________________ */
   double VIC_CropSyst_know_water_entering_soil_mm(double water_entering_soil_mm);
   /**< \fn
   \brief Informs the crop model of the water entering the soil.
   \param [in] water_entering_soil_mm (millimeter or  kg/m^2)
   Water entering the soil at the soil surface for the day.
   (total of irrigation and precipitation, and melted snow).
   \details This function should be called daily
   before calling VIC_CropSyst_process().
   \return drainage (mm) (if any).
   Currently VIC is not necessarily doing anything with the returned drainage.
    * */
   /* _______________________________________________________________________ */
   void VIC_CropSyst_know_air_temperature(double air_temperature_maximum,
      double air_temperature_minimum);
   /**< \fn
   \brief Informs the crop model of the current weather elements and conditions.
   \param [in] air_temperature_maximum maximum daily air temperature (celcius}
   \param [in] air_temperature_minimum minimum daily air temperature (celcius}
   \details   This method must be called every day before calling VIC_CropSyst_process().
   VIC_CropSyst_know_windspeed() is usually called at the same time.
    */
   /*_2010-11-22_____________________________________________________________ */
   void VIC_CropSyst_know_solar_radiation(double daily_irradiance_MJ_m2);
   /**< \fn
   \brief Informs the crop model of the current solar radiation.
   \param [in] daily_irradiance_MJ_m2 daily global solar radiation/irradiance (short wave)
      in mega Joule per square meter  MJ/m2 (per day).
   \details   This method must be called every day before calling VIC_CropSyst_process().
   */
   /*_2011-04-13_____________________________________________________________ */
   void VIC_CropSyst_know_relative_humdity(double RelHumid_max, double RelHumid_min);
   /**< \fn
   \brief Informs the crop model of the current relative humidity.
   \param [in] RelHumid_max maximum relative humidity (percent).
   \param [in] RelHumid_min minimum relative humidity (percent).
   \details   This method must be called every day before calling VIC_CropSyst_process().
   */
   /*_2011-04-13_____________________________________________________________ */

//windspeed is always needed to calculate transpiration adjustment #ifndef CROPSYST_PROVIDED_ET
   void VIC_CropSyst_know_wind_speed_m_s(double wind_speed_m_s);
   /**< \fn
   \brief Informs the crop model of the current weather elements and conditions.
   \param [in] wind_speed_m_s average daily air speed (meter/second)
   \details   This method must be called every day before calling VIC_CropSyst_process()
   VIC_CropSyst_know_air_temperature() is usually called at the same time.
    */
   /* _2010-11-22____________________________________________________________ */
#ifndef CROPSYST_PROVIDED_ET
   double VIC_CropSyst_update_ET_coef_full_shading();
   void VIC_CropSyst_know_transpiration_max_mm(double crop_transpiration_max_mm);
   /* _2012-12-07____________________________________________________________ */
   void know_psychrometrics
      (double vapor_pressure_saturation_slope_kPa_C
      ,double psychrometric_constant_kPa_C
      ,double aerodynamic_resistance_ref_plants);
   /**< \fn
   \brief Informs the crop model of the psychrometric parameters.
   \param [in] vapor_pressure_saturation_slope_kPa_C (kPa/C)
   \param [in] psychrometric_constant_kPa_C (kPa/C)
   \param [in] reference aerodynamic resistence with plants (day/m)
   */
   /* _2014-05-09____________________________________________________________ */
#endif
   /* _______________________________________________________________________ */
   void VIC_CropSyst_know_atmospheric_CO2_concentration
      (double new_atmospheric_CO2_concentration_ppm);
   /**< \fn
   \brief Informs the crop model of changes of the atmospheric CO2 concentration.
   \param  [in] new_current_atmospheric_CO2_concentration_ppm The atmospheric CO2 concentration in parts per million
   \details    This method is optional and may be omitted if crop response to
   atmospheric CO2 concentration simulation submodel is not enabled.
   The method can be called at any time.
   It is usually adequate to call this function at the beginning of each year.
    */
   /* ____________________________________________________________________________ */
   int VIC_CropSyst_start_day(unsigned long date);
   /**< \fn
   \brief Resets any daily accumulators.
   \param [in] The date is represented as an integer computed as (year * 1000 + day_of_year)
   \details   This must be called every day at the beginning of the time step loop.
   Weather and must be provided before calling start_day().
   Generally there will be no critical variable dependencies on other objects
   but it is recommended that soil have been setup for the
   current day prior to the call to start_day() and   VIC_CropSyst_know_XXXX()
   methods should be called before calling VIC_CropSyst_start_day();
   in other words, the environmental conditions should be setup before
   the crop is setup.
   \return 1 (true) on successful start of day.
    */
   /* ____________________________________________________________________________ */
   int /* bool */ VIC_CropSyst_end_day();
   /**< \fn
   \brief invalidates calculated weather elements in preparation for the next day.
   \return 1 (true) on successful end of day.
    */
   /* ____________________________________________________________________________ */
   double VIC_CropSyst_harvest(unsigned long int harvest_options_bitmask);
#define HARVEST_TERMINATE 0x01
   /**< \fn
   \brief For annual crops this method would be called at the end of
   the season usually in response to a scheduled harvest or clipping event.
   This methods is also used for perennial crops
   (I.e. with multiple clipping events during a year).
   \param  [in] harvest_options_bitmask
   <DL>
   <DT>  bit 0 - terminate <DD> Stops growth processes
   and is usually set to true for annual crops.
   When terminate is 0, the crop will continue to grow.
   </DL>
   \details
   Biomass, leaf area index and other properties of the crop will
   be adjusted in response to the harvest.
   If the crop is not terminated the phenology and other growth parameters
   may be reset for growth recovery (of perennials).
   One would typically not call VIC_CropSyst_process for the day the crop is
   terminated and at the end of the daily timestep, the crop
   should be deleted after 'get'ting desired output varible values.
   <P>If the crop object is created with HARVEST_AUTOMATICALLY,
   the crop will automatically be harvested about 10 days after maturity.
   In this case is not necessary to call the VIC_CropSyst_harvest function.
   <P>Applies to the 'active' crop.
   \remark
   Conventionally in CropSyst simulations, management events such as harvest
   are processed near the beginning of the daily time step.
   It is recommended to delete the crop object near the end of the time step
   or at the beginning of the next day timestep so that you can
   still access crop project values for any daily output.
   \return The harvested yield is returned.
    */
   /* ____________________________________________________________________________ */
   int VIC_CropSyst_process();
   /**< \fn
   \brief The is the main daily time step function, this performs the growth functions.
   \details
   This function will be called for every day.
   Usually weather and soil water will be processed first and the
   crop processing occurs near the end of the daily loop.
   (so the crop is responding to the updated climate and soil).
   Typically crop_process() would not be called on the date when
   a crop is harvested with termination.
   \return Almost always the current crop growth stage code (a non-zero value).
   (see the Normal_crop_growth_stage enumeration defined in growth_stages.h).
   The growth stage indicator can be used to schedule management operations.
   (I.e. one might schedule an event so many days after beginning of flowering).
   The growth stage indicator should be used indicate when to initiate the
   harvest operation, I.e. a few days after NGS_MATURITY,
   or after NGS_HARVESTABLE is returned by this function.
   \return 0 is returned if the crop had been terminated (no further process occurs.)
    */
   /* ____________________________________________________________________________ */
   double VIC_CropSyst_get(unsigned long int variable_code
      /* Not Yet Implemented ,unsigned long int units_code */ );
   /**<
   \brief Retrieves the specifies state variable value.
   \param [in] variable_code  The variable value to be returned.
   See the file VIC_CropSyst_variable_coes.h for the
   current set of variable codes with units.
   \ param [in] units_code     I may eventually add the option to get the value
   in preferred units of the returned value.
   \details Currently the variable value is in the default units for the variable.
   The units code option is not yet implemented:
   If a units code is specified, then the variable value will be
   converted to the desired units if such a conversion is possible/available.
   The units codes are listed in units_code_SI.h,
   however, only a subset of these units are applicable.

   \return the current state variable value.
    */
   /* ____________________________________________________________________________ */
   /*140810 obsolete
      // VIC is now handling water uptake so if it still needs
      // to have uptake, then it needs to keep track.
      // CropSyst proper does not keep the uptake by layer.
   double VIC_CropSyst_get_uptake_mm(int horizon_number);
   */

   /**
   \param [in] horizon_number See horizon number above
   \return crop water update is the specified soil horizon.
    */

   /* ____________________________________________________________________________ */
   /** The following methods VIC_XXXX_crop
   are for the management of instances of crop objects. */
   /* ____________________________________________________________________________ */
#define HARVEST_AUTOMATICALLY 0x01
#define CO2_RESPONSE_SUBMODEL 0x02
//#error
   #if (VIC_CROPSYST_VERSION>=3)
   unsigned long int VIC_CropSyst_create_V3
   (crop_data_struct *_VIC_crop
    ,soil_con_struct *_VIC_soil_con
    ,veg_lib_struct *_VIC_veg_lib
    ,option_struct *_VIC_options
    ,veg_con_struct *_VIC_veg_con
    ,int _VIC_Crop_count_index);
   CropSyst::Scenario *generate_scenario_for_crop_count
   (crop_data_struct *_VIC_crop
    ,soil_con_struct *_VIC_soil_con
    ,veg_lib_struct *_VIC_veg_lib
    ,option_struct *_VIC_options
    ,veg_con_struct *_VIC_veg_con
    ,int _VIC_Crop_count_index);
   #else
   unsigned long int VIC_CropSyst_create
      (char *database_directory_name
      ,double latitude_dec_deg
      ,double longitude_dec_deg
      ,double elevation_meter
      ,unsigned long int optional_submodels_bitmask
      ,char *label);
   #endif
   /** \fn
   \brief This creates a crop object and returns a unique ID number
   called the 'handle' of the object.
   \param [in] WSDA_crop_type_code is corresponds to the
   Washington State Department of Agriculture crop type number.
   \param [in] optional_submodels_bitmask selection of optional submodels.
   The HARVEST_AUTOMATICALLY mode is often suitable for annual crops,
   if this mode is not enabled, one must use the VIC_CropSyst_harvest()
   The atmospheric CO2 response submodel is optional.
   \param   [in] label An optional identification mechanism to identify
   the crop object, it is not otherwise used by the model.
   Note that the crop parameters already have a label/caption identifying
   the crop type.
   The label specified here would be used to otherwise uniquely identify
   the crop object.
   \details
   <UL>
   <LI> A crop object is automatically activated when created and remains
   the active instance until deleted or the VIC_CropSyst_activate() function is called.
   <LI>    The returned handle is used to activate and identify the instance of the crop.
   <LI>    A crop can be created at any time, normally on the sowing date.
   <LI>    Each created crop should be deleted calling the VIC_CropSyst_delete() function,
   normally this will be done at some point after harvest occurs.
   </UL>
   \remark
   If HARVEST_AUTOMATICALLY model is enabled, examine the current growth
   stage returned by VIC_CropSyst_process, when NGS_HARVEST is returned,
   then one would typically use the VIC_CropSyst_get() method to obtain
   the yield, biomass and any other desired output values at the harvest.
   For annual crops, the VIC_CropSyst_object_delete() will be used to
   dispose of the harvested crop object.
   <BR>
   If the atmospheric CO2 response submodel (CO2_RESPONSE_SUBMODEL) is enabled,
   one should call the    VIC_CropSyst_know_current_atmospheric_CO2_concentration()
   function to update the current CO2 concentration.
   \return the handle (ID) of the created crop object.
    */
   /* ____________________________________________________________________________ */
   unsigned int VIC_CropSyst_is_perennial();
   /* ____________________________________________________________________________ */
   unsigned long int VIC_CropSyst_start_crop(unsigned int WSDA_crop_type_code);
   /*
   \brief This creates a crop object and returns a unique ID number
   called the 'handle' of the object.
   \param [in] WSDA_crop_type_code is corresponds to the
   Washington State Department of Agriculture crop type number.
   <BR>
   This should be called shortly before the anticipated sowing date.
   <BR>
   \return the handle (ID) of the started crop.
   **/
   /* ____________________________________________________________________________ */
   int VIC_CropSyst_stop_crop();
   /* ____________________________________________________________________________ */
   unsigned long int VIC_CropSyst_object_activate(unsigned long int crop_handle);
   /** < \fn
   \brief Activate the specifice crop object (identified by the handle).
   \param handle  [in] handle ID of the crop to be activated
   \details
   All the VIC_CropSyst_XXXX() functions are applied to the currently active
   crop object instance.
   In the case where it is desired to have multiple instances of
   crop objects it is necessary to use this method to identify which object
   the subsequent VIC_CropSyst_XXXX() functions calls will apply to.
   \remarks It is not necessary to use this function if there is only
   one instance of an object at a time.
   Attempting to activate an object which has been deleted will cause an error.
   \return That handle (same value as passed) of the specified object this is
   activated and is valid. If the object could not be activated,
   the current crop object object remains active and its handle is returned.
   If not crop is activated 0 is returned.
   It is generally assumed the specified object will be activated
   given the correct handle, but it would be a good practice for the caller
   to verify that the returned handle matches the handle parameter.
    */
   /* ____________________________________________________________________________ */
   unsigned long VIC_CropSyst_object_delete(unsigned long int crop_handle);
   /**< \fn
   \param handle  [in] handle ID of the crop to be deleted.
   \brief This deletes the specified crop object (identified by the handle).
   and its memory is deallocated.
   \details   If the specified crop object is the active object the handle
   become invalid and you must not call any other VIC_CropSyst_XXX methods
   except to activate or create another crop object.
   Normally a crop is deleted at some point after harvest
   although a crop might be deleted after some premature termination event
   (I.e. to end a cover crop).  Or if the crop is a perennial some
   management event would be needed to terminate the crop.
   \return 1 if the object was successfully deleted and
   0 if the specified handle is not a valid crop object
   (it may have already been deleted).
    */
   /* ____________________________________________________________________________ */
#ifdef __cplusplus
}
#endif

#endif

