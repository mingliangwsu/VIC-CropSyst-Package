#ifndef VCS_GLOBAL_H
#define VCS_GLOBAL_H
//170628LML #include "corn/datetime/date.hpp"
#include "corn/chronometry/date_32.h"
#ifdef VCS_V5
#include "vic_driver_classic.h"
#endif
//200827RLN extern CORN::Date_clad_32 global_today;
extern CORN::Date_clad_32 global_simdate;
extern int global_rec;
extern filenames_struct filenames;
#if (VIC_CROPSYST_VERSION==2)
//LML 150413 extern crop_rotation_lib_struct *crop_rotation_lib;    //LML 141104
//#else
extern crop_lib_struct *crop_lib; //keyvan 11132012
#endif
#endif // VCS_GLOBAL_H
