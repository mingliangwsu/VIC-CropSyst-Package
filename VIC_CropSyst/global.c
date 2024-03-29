/**************************************************
 *LML 141105 created for define global variables
 **************************************************/

#include "global.h"

 /*190201RLN check needed
#ifdef VCS_V5
#include "vic_def.h"
#else
#include "vicNl_def.h"
#endif
*/
#if (VIC_CROPSYST_VERSION>=3)
#include "corn/chronometry/date_32.h"
//190130 CORN::Date_clad_32 global_today;                                                 //150723LML note: used for CropSyst and include the spinup period
//200827RLN CORN::date32 global_simdate_raw;                                                 //190130
//200827RLN CORN::Date_clad_32 global_today(global_simdate_raw);                             //190130
CORN::Date_clad_32 global_simdate;                                               //200827RLN_190130
int global_rec;
filenames_struct filenames;
CropCodeLib *cropcode_lib;                                                       //161102LML
int account_crop_types;                                                          //161102LML
#endif
#if VIC_CROPSYST_VERSION==2
crop_lib_struct *crop_lib; //keyvan 11132012
#endif

global_param_struct global_param;
veg_lib_struct *veg_lib;
option_struct options;
Error_struct Error;
param_set_struct param_set;

char *version = "4.1.2.e bug fix update 2012-Nov-01";
#ifdef SEPERATE_SOIL_PARAMETER_AND_CROP_OUTPUT
char *optstring = "g:vos:c:f:";
#else
char *optstring = "g:vo";
#endif
#if QUICK_FS
double   temps[] = { -1.e-5, -0.075, -0.20, -0.50, -1.00, -2.50, -5, -10 };
#endif
int flag;

char   ref_veg_over[]        = { 0, 0, 0, 0 };
double ref_veg_rarc[]        = { 0.0, 0.0, 25, 25 };
double ref_veg_rmin[]        = { 0.0, 0.0, 100, 100 };
double ref_veg_lai[]         = { 1.0, 1.0, 2.88, 4.45 };
double ref_veg_albedo[]      = { BARE_SOIL_ALBEDO, H2O_SURF_ALBEDO, 0.23, 0.23 };
double ref_veg_rough[]       = { 0.001, 0.001, 0.0148, 0.0615 };
double ref_veg_displ[]       = { 0.0054, 0.0054, 0.08, 0.3333 };
double ref_veg_wind_h[]      = { 10.0, 10.0, 10.0, 10.0 };
double ref_veg_RGL[]         = { 0.0, 0.0, 100, 100 };
double ref_veg_rad_atten[]   = { 0.0, 0.0, 0.0, 0.0 };
double ref_veg_wind_atten[]  = { 0.0, 0.0, 0.0, 0.0 };
double ref_veg_trunk_ratio[] = { 0.0, 0.0, 0.0, 0.0 };
/* One element for each PET type (non-natural or natural) */
char   ref_veg_ref_crop[]    = { FALSE, FALSE, TRUE, TRUE, FALSE, FALSE };
