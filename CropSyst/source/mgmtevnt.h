#ifndef mgmtevntH
#define mgmtevntH
#include "options.h"
#include "cs_mgmt.h"
namespace CropSyst {

DECLARE_EVENT_PERIOD_PARAMETERS(Irrigation_event, CropSyst::Irrigation_operation)          //020311
DECLARE_EVENT_PERIOD_PARAMETERS(Tillage_event, CropSyst::Tillage_operation)                //020311
DECLARE_EVENT_PERIOD_PARAMETERS(Harvest_or_clipping_event, CropSyst::Harvest_or_clipping_operation) //020311
//020311    Note, it does not appear that Chemical_event is currently used
DECLARE_EVENT_PERIOD_PARAMETERS(Chemical_event, CropSyst::Chemical_operation)              //020311

DECLARE_EVENT_PERIOD_PARAMETERS(N_application_soil_observation_mode_event, CropSyst::N_application_soil_observation_mode) //060809

// This is actually obsolete, we can just use DECLARE_EVENT_PARAMETERS
#define DECLARE_MANAGEMENT_MODE_PARAMETERS(event_period_class, operation_class) \
class event_period_class \
: public operation_class \
, public Common_event \
{  \
public: \
   event_period_class() \
   : operation_class() \
   , Common_event()  \
   { operation = this; } \
};

DECLARE_MANAGEMENT_MODE_PARAMETERS(Automatic_irrigation_mode_event,CropSyst::Automatic_irrigation_mode)
DECLARE_MANAGEMENT_MODE_PARAMETERS(Automatic_clip_biomass_mode_event,CropSyst::Auto_clip_biomass_mode)

DECLARE_EVENT_PERIOD_PARAMETERS(Inorganic_nitrogen_event, CropSyst::Inorganic_nitrogen_operation)
#if (MANAGEMENT==4)
DECLARE_EVENT_PERIOD_PARAMETERS(Residue_event, CropSyst::Residue_operation)
#else
// I was going to eliminate residue (the are essentially tillage)
// I may need to resurect Residue_operation
// I now won't do this until I replace the management system (V6)
DECLARE_EVENT_PERIOD_PARAMETERS(Residue_event, CropSyst::Tillage_operation)

#endif
#if ((CROPSYST_VERSION > 0) && (CROPSYST_VERSION < 5))
DECLARE_EVENT_PERIOD_PARAMETERS(Organic_nitrogen_event, CropSyst::Organic_nitrogen_operation)
DECLARE_EVENT_PERIOD_PARAMETERS(V3_clipping_event, CropSyst::Harvest_or_clipping_operation)
#endif
DECLARE_EVENT_PERIOD_PARAMETERS(Biomatter_application_event, CropSyst::Biomatter_application_operation)
DECLARE_MANAGEMENT_MODE_PARAMETERS(Automatic_N_event,CropSyst::N_application_soil_observation_mode::Split_application)
//DECLARE_MANAGEMENT_MODE_PARAMETERS(N_application_soil_observation_mode_event,N_application_soil_observation_mode_class)
}//_namespace CropSyst_________________________________________________________/
#endif
//mgmtevnt.h


