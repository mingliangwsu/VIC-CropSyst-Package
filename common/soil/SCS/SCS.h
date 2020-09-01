#ifndef SCSH
#define SCSH

#include "USDA/NRCS/soil/survey/domains.h"
#include "corn/enum.h"

extern const char *land_use_label_table[];
extern const char *land_treatment_label_table[];
extern const char *hydrologic_condition_label_table[];
//______________________________________________________________________________
enum Land_use {FALLOW,ROW_CROPS,SMALL_GRAIN,CLOSE_SEED_LEGUMES,
               PASTURE,MEADOW,FOREST,FARMSTEAD,DIRT_ROAD,PAVED_ROAD};
DECLARE_ENUM_CLAD(Land_use_clad,Land_use,land_use_label_table)
//______________________________________________________________________________
enum Land_treatment {NO_TREATMENT,STRAIGHT,CONTOUR,TERRACE};
#define NATURAL STRAIGHT
DECLARE_ENUM_CLAD(Land_treatment_clad,Land_treatment,land_treatment_label_table)
//______________________________________________________________________________
enum Hydrologic_condition {POOR,FAIR,GOOD};
#define GENERAL POOR
DECLARE_ENUM_CLAD(Hydrologic_condition_clad,Hydrologic_condition,hydrologic_condition_label_table)
//______________________________________________________________________________
enum CNs {CN1,CN2,CN3};
//______________________________________________________________________________
struct classify
{  Land_use             land_use;
   Land_treatment       land_treatment;
   Hydrologic_condition hydro_condition;
   USDA_NRCS::Hydrologic_group     hydro_group;                                  //110216
   nat8                 curve_number[3]; // Index with CNs (currently stored as nat8 but these are actually real numbers).
};
#define lu   land_use
#define ltr  land_treatment
#define hyc  hydro_condition
#define hyg  hydro_group
#define cn   curve_number

#define MAX_classifications  129

extern classify condition_classifications[MAX_classifications+1];
//_classify____________________________________________________________________/
float64 SCS_lookup_curve_number
(Land_use              land_use
,Land_treatment        treatment
,Hydrologic_condition  hydro_cond
,USDA_NRCS::Hydrologic_group      hydro_group                                    //110216
,nat8                  antecedent_moisture_content);
//_SCS_lookup_curve_number_____________________________________________________/
#endif

