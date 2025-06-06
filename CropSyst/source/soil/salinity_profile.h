#ifndef salinity_profileH
#define salinity_profileH

#include "soil/salt_I.h"
#include "soil/chemical_profile.h"
//______________________________________________________________________________
class Soil_interface;
class Soil_profile_spreadsheet_set;                                              //070927
namespace Soil {                                                                 //181206
class Layers_interface;
class Chemicals_profile; // for friendship
//______________________________________________________________________________
class Salinity_profile
: public Chemical_profile
{
 public: // For Salinity only
   soil_layer_array64(salinity_ECw_dSm_input_output);                            //080207
         // dS/m This is only for the salt pool!
   float64 uninundated_ECw_to_TDS;                                               //080402
      // Conversion of dS/m to kg/m3 (g/l) (used only for input/output)
 public:
   Salinity_profile
      (
       const CORN::date32 &simdate_                                              //170525
      ,const std::string &chem_name
      ,chemical_types   chemical_type_
      ,float64          molecular_to_elemental_
      ,float64          elemental_to_molecular_
      ,float64          control_transformation_adjustment_
      ,soil_layer_array64(simulation_amount_E)
      ,const Layers_interface               &soil_layers                         //150925
      ,const Hydrology_interface            &soil_hydrology                      //150925
      ,const Hydraulic_properties_interface &soil_hydraulic_properties           //150925
      ,const Structure                      &soil_structure                      //150925
      ,Infiltration_model infiltration_model                                     //080117
      ,Water_table_curve *water_table_curve                                      //000504
      ,float64           uninundated_ECw_to_TDS_);
 public:
   virtual void update_concentration_for_output(uint8 num_layers_eff);
   virtual void recalibrate
      (soil_layer_array64(simulation_amount_E)
      ,bool for_initialization);
};
//_Salinity_profile_________________________________________________2008-04-03_/
class Soil_salt  // rename this
: public implements_ Salt_interface                                              //170425
, public extends_ CS::Simulation_element_abstract
{protected:  friend class  Chemicals_profile;
             friend class Soil_profile_spreadsheet_set;                          //051118
   const Layers_interface               &soil_layers;                            //060503
   const Hydrology_interface            &soil_hydrology;                         //060503
   const Hydraulic_properties_interface &soil_hydraulic_properties;              //060503
   Salinity_profile  *salinity_profile; //  Created here but owned by chemical_profiles  don't delete //080403
 protected:
   soil_layer_array64(osmotic_pot_solution);
 public:
   Soil_salt
      (const CORN::date32 &today_                                                //170525
      ,const Layers_interface               &soil_layers                         //150925
      ,const Hydrology_interface            &soil_hydrology                      //150925
      ,const Hydraulic_properties_interface &soil_hydraulic_properties           //150925
      ,const Structure                      &soil_structure                      //150925
      , Infiltration_model infiltration_model                                    //080117
      , soil_layer_array64(salt)                                                 //080117
      , Water_table_curve  *water_table_curve // may be 0                        //051120
      , float64 ECw_to_TDS_soil);                                                //080213
   virtual bool start_day()                                       modification_; //061009
   virtual float64 get_salt_stress_function
      (nat8 layer,float64 osmotic_pot_50,float64 salt_tolerance_P )       const;
      // Derived classes can return 1.0 if not interested in salinity
   inline virtual float64 get_osmotic_pot_solution(uint8 layer)           const
      { return  osmotic_pot_solution[layer]; };
   virtual float64 calc_osmotic_pot_solution(uint8 layer)                 const; //061010
   virtual void set_daily_irrig_salinity                                         //000713
      (float64 concentration
      ,float64 ECw_to_total_dissolved_solids  // Typically 0.64 (used to be a constant)   //080402
      ,float64 irr_water_amount
      ,Hour    event_start_hour                                                  //081212
      ,Hours   event_duration);                                                  //071015
   virtual bool initialize()                                      modification_; //051118
   inline virtual float64 get_concentration_dSm(nat8 layer)               const  //080207
      { return salinity_profile->salinity_ECw_dSm_input_output [layer]; }        //080402
   RENDER_INSPECTORS_DECLARATION;                                                //190121
};
//_Soil_salt___________________________________________________________________/
}//_namespace_Soil______________________________________________________________
#endif

