#ifndef cs_operation_codesH
#define cs_operation_codesH
/*
#ifndef labeled_enumH
#include <corn/labeled_enum.h>
#endif
*/
#include "corn/enum.h"
//______________________________________________________________________________
enum CropSyst_Op
{
   CS_OP_NONE
,  CS_OP_ROT_SOWING
,  CS_OP_MGMT_HARVEST_OR_CLIPPING   /*Eventually rename back to CS_OP_MGMT_HARVEST */
,  CS_OP_MGMT_TILLAGE
,  CS_OP_MGMT_RESIDUE
,  CS_OP_MGMT_IRRIGATION
,  CS_OP_MGMT_INORGANIC_NITROGEN
,  CS_OP_MGMT_ORGANIC_NITROGEN
,  CS_OP_MGMT_BIOMATTER_APPLICATION
,  CS_OP_MGMT_CHEMICAL
,  CS_OP_MGMT_AUTO_IRRIGATION
,  CS_OP_MGMT_AUTO_NITRO_OPTIMAL  // check name
,  CS_OP_MGMT_AUTO_N           // <- This is a place holder for LADSS
,  CS_OP_MGMT_AUTO_N_BALANCE_BASED
#if ((CROPSYST_VERSION > 0) && (CROPSYST_VERSION <= 4))
,  CS_OP_MGMT_AUTO_N_LOCAL_CALIBRATION
#endif
,  CS_OP_MGMT_AUTO_N_APPL  // <- This is for split applications.
,  CS_OP_MGMT_SOIL_N_SAMPLING
,  CS_OP_MGMT_AUTO_CLIP_BIOMASS
,  CS_OP_RECALIBRATION
,  CS_OP_PROFILE_PRINT
,  CS_OP_COUNT
};
//______________________________________________________________________________
extern const char *CropSyst_op_text[];
extern const char *CropSyst_operation_codes[];
extern const char *CropSyst_operation_icons[];
extern const char *CropSyst_crop_event_codes[];

//190113 DECLARE_LABELED_ENUM(CropSyst_Op_labeled,CropSyst_Op,CropSyst_operation_codes)
//190113 DECLARE_LABELED_ENUM(CropSyst_Op_descriptive_labeled,CropSyst_Op,CropSyst_op_text);

DECLARE_ENUM_CLAD(CropSyst_Op_clad,CropSyst_Op,CropSyst_operation_codes)
DECLARE_ENUM_CLAD(CropSyst_Op_descriptive_clad,CropSyst_Op,CropSyst_op_text);

//______________________________________________________________________________
#endif
// cs_operations_codes.h
