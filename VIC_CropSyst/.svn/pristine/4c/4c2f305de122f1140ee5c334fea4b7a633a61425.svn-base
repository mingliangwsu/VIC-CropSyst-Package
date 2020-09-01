#ifndef WSDA_croptypeH
#define WSDA_croptypeH

#ifdef CROP_PARAMETERS_HARDCODED
#include "VIC_crop_param.h"
#ifdef __cplusplus
namespace  VIC_crop {
#endif
//______________________________________________________________________________
struct WSDA_Crop_type_struct
{
   nat16 OID_DoNotUse; // ArcGIS table index
   nat16 WSDA_code;
   char *caption;
//   char *notes;
   struct
#ifdef __cplusplus
   VIC_crop::
#endif
      Crop_parameters_struct parameters;
};
//______________________________________________________________________________
typedef struct WSDA_Crop_type_struct WSDA_Crop_type;
extern
      Crop_parameters* WSDA_lookup_crop_parameters(nat16 WSDA_code);
//______________________________________________________________________________
#ifdef __cplusplus
};
#endif
#endif
#endif
