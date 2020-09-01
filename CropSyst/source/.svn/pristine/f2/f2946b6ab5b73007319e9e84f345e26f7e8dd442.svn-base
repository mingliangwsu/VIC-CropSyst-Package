#ifndef sim_paramH
#define sim_paramH

#ifndef CS_OPTIONS_H
// This options.h needs to be for the current program being compiled
// is should not necessarily be /cropsyst/cpp/options.h

#  include <options.h>
#endif

#ifndef primitiveH
#  include "corn/primitive.h"
#endif
#include "cs_filenames.h"
#include "common/weather/loc_param.h"
#include "chemfile.h"
#include "common/soil/layering.h"
#include "watertab.h"

#define    simulation_extension  ".SIM"
#define    LABEL_SoilR                "SoilR"
#define    LABEL_enabled              "enabled"

#define    LABEL_water_table_conc     "water_table_conc"
#define    LABEL_salt                 "salt"
#define    LABEL_years_in_rotation    "years_in_rotation"

#define    LABEL_adjustment           "adjustment"

#define    LABEL_chemicals  "chemicals"
#define    LABEL_num_chemicals  "num_chemicals"

#define    LABEL_half_life_override "half_life_override"
#define    LABEL_Freundlich_N_override "Freundlich_N_override"
#define    LABEL_activation_energy_override "activation_energy_override"
#define    LABEL_soil_moisture_influence_override "soil_moisture_influence_override"
#define    LABEL_linear_sorption_coef_override "linear_sorption_coef_override"

#define    LABEL_override             "override"

#define LABEL_aspect        "aspect"
// remember last selected

// Visualization section
#define LABEL_visualization      "visualization"
#define LABEL_generate_after_run "generate_after_run"
#define LABEL_template           "template"

using namespace CORN;

#ifdef CHEM_PROFILE
//______________________________________________________________________________
class initial_chemical_profile
: public chemical_properties
{public:
   bool enabled;
    // The following are initialization
   float32 adjustment_32;       // 0-2
   float32 water_table_conc_32 ; // kg chem/ m^3 water
   soil_horizon_array32(horizon_chemical);   // kg/ha fertilizers kg elemental form/ha, pesticies kg chem/ha
    Valid_float32 v_half_life_override;
   Valid_float32 v_Freundlich_N_override;
   Valid_float32 v_activation_energy_override;
   Valid_float32 v_soil_moisture_influence_override;
   Valid_float32 v_linear_sorption_coef_override;
   Valid_float32 v_adjustment;
   Valid_float32 v_water_table_conc;
   valid_soil_horizon_array(v_chemical);
 public:
   initial_chemical_profile(const char *i_name);
   void initialize();
   virtual const char *label_cstr_deprecated(char *buffer)                const;
   virtual const char *label_string(std::string &buffer)                  const; //170423
#ifdef NYI_XX
 private:    // The 64bit stores are not really needed because they will be store in the chemical profile
   half_life_override_64                = CORN_clean_precision(half_life,DEF_CLEAN_PRECISION);
   Freundlich_N_override_64             = CORN_clean_precision(Freundlich_N,DEF_CLEAN_PRECISION);
   activation_energy_override_64        = CORN_clean_precision(activation_energy_,DEF_CLEAN_PRECISION);
   soil_moisture_influence_override_64  = CORN_clean_precision(soil_moisture_influence,DEF_CLEAN_PRECISION);
   linear_sorption_coef_override_64     = CORN_clean_precision(linear_sorption_coef,DEF_CLEAN_PRECISION);
#endif
 public: // Accessors
    inline float64 get_adjustment()              const { return adjustment_32; }
    inline float64 get_water_table_conc()  const { return water_table_conc_32; }
};
//_initial_chemical_profile_________________________________________1999-02-25_/
#endif
#endif


