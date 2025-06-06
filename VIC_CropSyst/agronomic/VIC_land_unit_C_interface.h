#ifdef VIC_CROPSYST_VERSION
#ifndef VIC_land_unit_C_interfaceH
#define VIC_land_unit_C_interfaceH
#include <list>
#include "VIC_crop_output.h"
/*190201RLN check needed
#ifdef VCS_V5
#include "vic_def.h"
#else
#include "vicNl_def.h"
#endif
*/
#include "CropSyst/source/cs_scenario.h"
#include "irrigation_lib.h"
#include "CropSyst/source/rot_param.h"
#include "agronomic/VIC_land_unit_simulation.h"

#ifdef OUTPUT_FULLIRRIGATION_PATTERN
extern std::list<Out_irrigation_pattern_struct> out_irrigation_pattern_output_list;
#endif
/** \file
\author Roger Nelson  Biological System Engineering  Washington State University
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

 */
/* __________________________________________________________________________ */
/**
All the following functions VIC_land_unit_XXX() apply to the current active
land unit. If there is no active land unit the functions returns immediately
with no effect.

VIC_CropSyst_object_XXXX functions are alias to VIC_land_unit_XXXX
They are provided to use in VIC code compatibility either V2 or V3.
The VIC_land_unit_XXXX functions are preferable for V3.
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
namespace CropSyst                                                               //150413RLN
{
   class Irrigation_operation_struct;                                            //150413RLN
}

//___________________________________________________________________________
namespace VIC
{
   extern CORN::OS::Directory_name *project_directory;                           //150526RLN
   extern VIC::Land_unit_simulation *active_land_unit;                           //150710LML
class Rotation_parameters_enumerated
: public extends_ CropSyst::Rotation_parameters
, public extends_ CORN::Item
{
   nat32 rotation_or_crop_veg_class_code;
 public:
   CORN::OS::File_name *rotation_filename_owned;
 public:
   inline Rotation_parameters_enumerated
      (nat32 _rotation_or_crop_veg_class_code
      ,CORN::OS::File_name *_rotation_filename_owned)
      :CropSyst::Rotation_parameters()
      ,rotation_or_crop_veg_class_code(_rotation_or_crop_veg_class_code)
      ,rotation_filename_owned(_rotation_filename_owned)
      {}
   inline ~Rotation_parameters_enumerated()
      { delete rotation_filename_owned; }
   inline virtual bool is_key_nat32(const nat32 key)                affirmation_ //190128
      { return rotation_or_crop_veg_class_code == key; }
   inline virtual nat32 get_key_nat32()                                 const
       { return rotation_or_crop_veg_class_code; }
};
}
//___________________________________________________________________________
extern "C" {
#endif
/* _______________________________________________________________________ */
void VIC_land_unit_know_psychrometrics
(double vapor_pressure_saturation_slope_kPa_C
,double psychrometric_constant_kPa_C
,double aerodynamic_resistance_ref_plants);
/*_2012-12-08_________________________________________________________________*/
void VIC_land_unit_know_transpiration_max_mm
(double crop_transpiration_max_mm);
/*_2014-12-08_________________________________________________________________*/
unsigned long int VIC_land_unit_start();
/*_2016-05-04_________________________________________________________________*/
unsigned long int VIC_land_unit_start_year();
/*_2014-12-08_________________________________________________________________*/
int VIC_land_unit_start_day();
//171201LML
int VIC_land_unit_before_start_day();
/*_2015-07-18LML______________________________________________________________*/
nat16 VIC_land_unit_get_irrigation_type();
/*_2015-06-16LML______________________________________________________________*/
double VIC_land_unit_update_PET();
bool is_digits(const std::string &str);
/*_2014-12-08_________________________________________________________________*/
int VIC_land_unit_end_day(const double cell_fraction, int dist, int band);
/*_2015-07-22LML______________________________________________________________*/
int VIC_land_unit_print_end_day_outputs(int growth_season_only,
                                        const double cell_fraction,
                                        int dist, int band);
/*_2014-12-08_________________________________________________________________*/
int VIC_land_unit_process_day();                                                 //150611RLN
/*_2014-12-08_________________________________________________________________*/
const CropSyst::Irrigation_operation_struct *VIC_land_unit_get_irrigation_parameters(); //190128RLN
/*_2020-10-28_________________________________________________________________*/
double VIC_land_unit_get_irrigation_target();
/*_2020-10-28_________________________________________________________________*/
/*_VIC_land_unit_get_irrigation_parameters_____________________2014-04-13_RLN_*/
int VIC_land_unit_chemical_transport_and_exchange
   (unsigned long int at_time
   ,unsigned long int duration
   ,double           *old_liquid_water_content
   ,double           *new_liquid_water_content
   ,double            water_entering_soil
   ,double           *water_flow);
/*_2015-07-26_RLN_____________________________________________________________*/
double VIC_land_unit_get
(unsigned long int variable_code
/*NYI ,unsigned long int units_code*/);
/*_2014-12-08_________________________________________________________________*/
double VIC_CropSyst_get
(unsigned long int variable_code
/*NYI ,unsigned long int units_code*/);
/*_2015-04-15_________________________________________________________________*/
//LML 150423 int VIC_CropSyst_get_transpiration(double layerevap[MAX_LAYERS]);
//NYI, get water uptake demands from each layer
/*_2015-03-18_________________________________________________________________*/
unsigned long int VIC_land_unit_create
(int cell_ID
, int
, int rotation_cycle_index                                                        //150521RLN
, crop_data_struct &_VIC_crop
, soil_con_struct  &_VIC_soil_con
, option_struct    &_VIC_options
, veg_con_struct   &_VIC_veg_con
, atmos_data_struct *_VIC_atmos
, cell_data_struct  &_VIC_cell
, snow_data_struct  &_VIC_snow/*150608LML*/
, veg_var_struct    &_VIC_veg_var/*150609LML*/);
/*
   VIC should call VIC_get_rotation_cycle_years
   The call VIC_land_unit_create for each out the rotation cycle years
   where rotation_shift_index is indexed from 0.
*/
/*_________________________________________________________________2014-12-08_*/
unsigned long int VIC_land_unit_activate
(unsigned long int land_unit_handle);
/*_________________________________________________________________2015-07-14_*/
unsigned long int VIC_land_unit_deactivate();
/*_________________________________________________________________2014-12-08_*/
unsigned long int VIC_CropSyst_object_activate
(unsigned long int land_unit_handle);
/*_________________________________________________________________2014-12-08_*/
unsigned long int VIC_land_unit_delete
(unsigned long int land_unit_handle);
/*_________________________________________________________________2014-12-08_*/
unsigned long int VIC_CropSyst_object_delete
(unsigned long int land_unit_handle);
/*_________________________________________________________________2015-03-18_*/
extern int find_index_in_sorted_array(const int *data,const int key, int imin,
                                      int imax);
//170412LML extern int get_veg_lib_index(const int veg_class_code);
extern int get_irrig_lib_index(const int irrig_code);

extern int get_years_in_rotation(int rotation_code);
extern int VIC_land_unit_get_current_crop_code();
extern int VIC_land_unit_get_current_crop_name(char *crop_name);
/* Crop name is set to the name of the crop file
 for the current active crop (null terminated).
 The land unit may be in fallow, in which case crop_name is empty string.
 In VIC files prepared for the CRB project, the crop
 files are associated with a numeric code  and these are symbolic
 links to files with descriptive file names.
 This function dereferences the symbolic link to get the descriptive file name.
 The function returns the number of characters set in crop_name
 (not counting null terminator).
*/
extern int VIC_land_unit_is_current_crop_fruit_tree();                           //150720RLN
extern int VIC_get_rotation_cycle_years(int rotation_or_crop_veg_class_code);    //150526LML
extern int VIC_know_project_directory(const char *project_directory_name);       //150527
#ifdef __cplusplus
}
#endif

#endif
/*____________________________________________________________________________*/
#endif
//VIC_CROPSYST_VERSION
