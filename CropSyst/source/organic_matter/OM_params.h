#ifndef OM_paramsH
#define OM_paramsH
#include "corn/parameters/parameter.h"
#include "common/soil/layering.h"
#include "organic_matter/OM_types.h"
#include "corn/const.h"

#define    LABEL_POM  "%OM"
// Organic matter history
#define     LABEL_partitioning_mode          "partitioning_mode"
#define     LABEL_active_SOM_protected       "active_SOM_protected"
#define     LABEL_microbial_biomass          "microbial_biomass"
#define     LABEL_labile_SOM                 "labile_SOM"
#define     LABEL_metastable_SOM             "metastable_SOM"
#define     LABEL_passive_SOM                "passive_SOM"
//______________________________________________________________________________
namespace CORN
{  class Data_record;
};
using namespace CORN;
//______________________________________________________________________________
class Organic_matter_initialization_parameters
{
public:
   Organic_matter_partitioning_mode_clad organic_matter_partitioning_mode_clad;  //040610

   // The following are essentially the percent of the initial organic matter carbon mass that is apportioned to the respective organic matter pools
   soil_horizon_array32(microbial_biomass_percent);     // Applies to all modes                // initial_C_to_microbial_biomass_percent
   soil_horizon_array32(labile_SOM_percent);            // Applies to user defined mode only   // initial_C_to_labile_SOM_percent
   soil_horizon_array32(metastable_SOM_percent);        // Applies to user defined mode only   // initial_C_to_metastable_SOM_percent
   soil_horizon_array32(passive_SOM_percent);           // Applies to user defined mode only   // initial_C_to_passive_SOM_percent
public:
   Organic_matter_initialization_parameters();
   virtual bool setup_structure(Data_record &data_rec,bool for_write/* = false*/ ) modification_; // 120909
};
//_Organic_matter_initialization_parameters______________2006-02-15_2001-07-17_/
class Organic_matter_decomposition_parameters
{
 public: // Added in  V4_4
   float32 unadj_microbial_decomposition_const;                                  //040610
   float32 labile_active_SOM_decomposition_const;                                //040610
   float32 metastable_active_SOM_decomposition_const;                            //040610
   float32 passive_SOM_decomposition_const;                                      //040610
   float32 microbial_CN_ratio;                                                   //070221
   float32 labile_active_SOM_CN_ratio;                                           //070221
   float32 metastable_active_SOM_CN_ratio;                                       //070221
   float32 passive_SOM_CN_ratio;                                                 //070221
   float32 C_fraction_from_labile_to_passive_SOM;                                //090515
   float32 C_fraction_from_metastable_to_passive_SOM;                            //090515
   float32 single_SOM_CN_ratio;                                                  //110928

   // Valid_float is only needed for scenario editor
   // In V5 parameter properties will be loaded from ontology
   Parameter_number p_unadj_microbial_decomposition_const;
   Parameter_number p_labile_active_SOM_decomposition_const;
   Parameter_number p_metastable_active_SOM_decomposition_const;
   Parameter_number p_passive_SOM_decomposition_const;
   Parameter_number p_microbial_CN_ratio;
   Parameter_number p_labile_active_SOM_CN_ratio;
   Parameter_number p_metastable_active_SOM_CN_ratio;
   Parameter_number p_passive_SOM_CN_ratio;
   Parameter_number p_C_fraction_from_labile_to_passive_SOM;
   Parameter_number p_C_fraction_from_metastable_to_passive_SOM;
   Parameter_number p_single_SOM_CN_ratio;                                       //110928
   Parameter_number p_max_apparent_soil_C_decomposition_const;                   //070710
 public: // For single SOM pool
   float32 max_apparent_soil_C_decomposition_const;                              //070710
   // max_residue_decomposition_const_obs becoming obsolete (was actually never used)
 public:
   Organic_matter_decomposition_parameters();
   virtual bool setup_structure(Data_record &data_rec,bool for_write/* = false*/ ) modification_; // 120909
};
//_Organic_matter_decomposition_parameters__________________________2007-03-03_/

#endif

