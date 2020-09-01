#ifndef chemical_profileH
#define chemical_profileH

// Organic matter is needed even if not using Nitrogen (needed for exchange of pesticides)

#include "common/soil/layering.h"
#include "model_options.h"
#include "corn/chronometry/time_types.hpp"
#include "corn/dynamic_array/dynamic_array_T.h"

#ifndef chemical_typesH
#  include "chemical_types.h"
#endif
#include "CS_suite/simulation/CS_simulation_element.h"
#if (CROPSYST_VERSION==5)
#include "CS_suite/observation/CS_observation.h"
#endif

#ifdef CROPSYST_PROPER
class Chemical_balance_accumulators_molecular;
class Chemical_balance_accumulators_elemental;                                   //190909
#endif
class Water_table_curve;

class Chemical_mass_profile_interface;
class Organic_matter_residues_profile_abstract;

#define   NO_MOISTURE_FUNCTION    1.0
#define   ALWAYS_MOLECULAR        1.0

#define   LABEL_NO3               "NO3"
#define   LABEL_NH4               "NH4"
#define   LABEL_POT_MINERALIZABLE_N  "PMN"

//   CHEMICAL_TXT      = 'chemical.txt'; get from cs_glob

struct     Langmuir;                                                             //190404

namespace Soil {
interface_ Soil_interface;                                                       //060503
/*
class Soil_texture_interface;                                                    //060503
*/
interface_ Layers_interface;                                                     //060503
interface_ Hydrology_interface;                                                  //060503
interface_ Hydraulic_properties_interface;                                       //060503
interface_ Structure;                                                            //070726
 //     Report accumulators contains some accumulator variables
 //     that are used only for output.

    // Chemical pool is the base class for Chemical_profile
    //  For each soil layer, there is storage of the chemical.
    //  The chemical may also undergo various transformations,
    //  but generally doesn't move between layers and is not uptaken.
    //  The chemical may denature or be lost to the atmosphere.
    //
    //  Potential mineralizable nitrogen is an example of a simple
    //  chemical pool because it doesn't move between layers
    //  (It is bound to organic matter).
//______________________________________________________________________________
class Chemical_pool
#ifdef CROPSYST_PROPER
: public extends_    CS::Simulation_element_abstract                             //170425
#else
: public implements_ CORN::Item                                                  //181212
#endif
{public:
#ifndef CROPSYST_PROPER
const CORN::Date_clad_32 simdate;
#endif
   class Surface
   {public:
      class Pending_application
      : public CORN::Item
      {public:
         CORN::datetime64  time; // date and time
         float64           mass; //kg chem / m^2 soil molecular
       public:
         inline Pending_application
            (CORN::datetime64  time_
            ,float64           mass_)
            : CORN::Item()
            , time(time_)
            , mass(mass_)
            {}
      };
      //_Pending_application________________________________________2017-04-25_/
    private:
      CORN::Unidirectional_list pending_applications;
      float64 mass; //kg chem / m^2 soil molecular
    public:
      inline Surface():mass(0) {}
      nat8 schedule_application
         (float64    mass_applied //kg chem / m^2 soil molecular
         ,CORN::datetime64 start_time
         ,CORN::Seconds operation_duration
         ,CORN::Seconds interval_duration)                        modification_;
      bool commit_pending_applications
         (CORN::datetime64 by_now)                                modification_;
         // Returns the mass applied now.
      inline bool is_exhausted()                                   affirmation_
         { return mass <= 0.000000001; }
      inline float64 get_mass()                                           const
         { return mass; }
      float64 deduct_mass(float64 mass_deduction)                 modification_;
         // Removed the specified mass (must limited to the mass available).
         // Returns the actual mass removed.
      float64 get_pending_application()                                   const;
   };
   //_Surface_______________________________________________________2017-04-25_/
 protected:                                                                      //060503
   const Layers_interface               &soil_layers;                            //060503
   const Hydrology_interface            &soil_hydrology;                         //060503
   const Hydraulic_properties_interface &soil_hydraulic_properties;              //060503
   const Structure                      &soil_structure;                         //070725
 public:
   std::string name;// Name of the chemical
   Chemical_mass_profile_interface  *mass_M; //kg chem / m^2 soil molecular   // owned by this and guarenteed //070116
      // mass could be renamed to subsurface_mass
   Surface *surface;                                                             //170425
      // owned by this
      // may be 0 if no surface mass typically this would be in ponded water.
   //200312#ifndef OLD_N_XFER
   modifiable_ Chemical_pool *receiver;                                          //170502
   //200312#endif
 public:
   float64 recalibration;                                                        //011115
   // when recalibration events occur we need to update the balance.
/*200312
   #ifdef OLD_N_XFER
   #error check_obsolete
   // This is obsolete in V5
   soil_layer_array64(transformed_from_M); //kg chem / m^2] transformed from something else
   float64 profile_transformed_from_M ;   // kg / m2 used only for output
                             // NH4 -nitrified-> NO3             NH4
                             // NO3 -denitrified-> denitrified NO3
                             // pot min. N -mineralized_> NH4
   #else
*/
 public:
//200312   #endif
   CORN::Dynamic_array<float64>transformed_E_output; //170502 Need to check if daily or cumulative

   #if (CROPSYST_VERSION == 4)
   float64 control_transformation_adjustment; // simulation control adjustment parameter
   #endif
 public:
   float64 molecular_to_elemental;
   float64 elemental_to_molecular;
 public:      // For balance:
   #ifdef CROPSYST_PROPER
   #ifndef NEW_N_XFER_BAL

   // as of 170503 this is still used
   Chemical_balance_accumulators_molecular  *ANNL_bal_accums_M;                  //040931
      //{ The report accumulators are only for output and could be ommited.}
   Chemical_balance_accumulators_molecular  *GS_bal_accums_M;                    //040931
   Chemical_balance_accumulators_molecular  *CONT_bal_accums_M;                  //040931
   #else
   #error no replacement yet
   #endif
   #endif
 public:
   Chemical_pool
      (const CORN::date32 &simdate_                                              //170525
      ,const std::string &chem_name                                              //001116
      ,float64 molecular_to_elemental
      ,float64 element_to_chem_factor
      #if (CROPSYST_VERSION == 4)
      ,float64 control_transformation_adjustment
      #endif
      ,soil_layer_array64(simulation_amount_E)                                   //051120
      //200312#ifndef OLD_N_XFER
      ,Chemical_pool *receiver_                                                  //170502
      //200312#endif
      ,const Layers_interface               &soil_layers_                        //150925
      ,const Hydrology_interface            &soil_hydrology_                     //150925
      ,const Hydraulic_properties_interface &soil_hydraulic_properties_          //150925
      ,const Structure                      &soil_structure_                     //150925
      ,Infiltration_model infiltration_model);                                   //080117
   virtual ~Chemical_pool();                                                     //080116
 public: //Item implementations
   inline virtual bool is_key_string(const std::string &key)        affirmation_ //180820
      { return name == key; }
   inline virtual const char *label_string(std::string &buffer)            const //170425
      { buffer.append(name); return buffer.c_str();}
 public: // Simulation_element implementations
   virtual bool start_day()                                       modification_; //071015
 public:
   bool reinitialize_for_sowing_or_restart();                                    //950806
   bool reinitialize_at_end_season();   /*was reinitialize_for_harvest*/         //120704_950902
   void remove_sublayer_2(nat8 num_sublayers);
   virtual float64 moisture_function
      (float64 water_filled_porosity
      ,float64 function_value_at_saturation_not_applicable);                     //060504
   virtual float64 water_content_correction
      (float64 sublayer_saturation_water_content
      ,float64 sublayer_water_content
      ,float64 sublayer_reference_water_content
      ,float64 sublayer_bulk_density); // this is used by derived classes        //000515
   virtual float64 transformation_rate
      (float64 soil_temperature
      ,float64 water_content_correct);
   virtual void transformation
      (nat8  sublayer
      ,float64 sublayer_water_content
      ,float64 sublayer_saturation_water_content
      ,float64 sublayer_reference_water_content
      ,float64 sublayer_temperature
      ,float64 sublayer_bulk_density
      ,Seconds preferred_transformation_time_step                                //060510
      // The original transformation time step is 86400 seconds (1 day)
      // When using  Organic_matter_residues_profile_multiple_cycling,
      // the nitrification and denitrification transformation timestep is 1 hour
      // (or the timestep of the infiltration model).
      //200312#ifdef OLD_N_XFER
      //200312,float64 &transformed_to_M
      //200312#endif
      );
   //200312#ifndef OLD_N_XFER
   virtual float64 take_elemental_from_transformation
      (nat8   layer
      ,float64 elemental_given);
   //200312#endif
   float64 get_current_profile_content_M(); // also for output                   //981006
   virtual float64 balance(nat8  soil_num_sublayers);                            //990317
   void inc_transformed_to(float64 transformed_M);                               //990317
   void inc_transformed_from(float64 transformed_M);                             //990317
   #ifdef CROPSYST_PROPER
   void dec_residue(float64 amount_M);                                           //990318
      // This is only used for balance
   #endif
   virtual void recalibrate                                                      //011115
      (soil_layer_array64(simulation_amount_E)                                   //011115
      , bool for_initialization);
   inline virtual bool has_uptake()                       const { return false;} //990317
   inline virtual const char *get_transformation_name()            { return "";}   // probably can be const //990317
   #ifdef CROPSYST_PROPER
   void instanciate_accumulators()                            /*contribution_*/; //111207
   #ifndef NEW_N_XFER_BAL
   // as of 170503 this is still used
   //170503 Probably obsolete in all versions
   void update_YLY_bal_reporter();                                               //990319
   #endif
   #endif

   nat8 apply_surface_mass                                                       //170425
      (float64          mass_applied //kg chem / m^2 soil molecular
      ,CORN::datetime64 start_time
      ,CORN::Seconds    operation_duration
      ,CORN::Seconds    interval_duration);
      // returns the number of events generated
   float64 get_surface_mass()                                             const; //170425
   float64 deduct_surface_mass(float64 mass_deduction)            modification_; //170425
   bool commit_surface_pending_applications                                      //170503_170425
      (CORN::datetime64 by_now)                                   modification_;
protected:
   void recalibrate_set_mass                                                     //080403
      (soil_layer_array64(new_mass_layers),bool for_initialization);
};
//______________________________________________________________________________
// Chemical profile is derived from chemical pool
//     in addition to storage within the layer and transformation,
//     The chemical may be trasported between layers, there may
//     be applications or may leach out.
//
//     The chemical is not uptaken
class Chemical_profile
: public extends_ Chemical_pool
{
 public:
   //___________________________________________________________________________
#ifdef WATERTABLE
protected:
   Water_table_curve *water_table_curve;  // owned by this                       //000504
#endif
 public: // Association pure overrides
   inline virtual bool is_key_string(const std::string &key)       affirmation_  //180820
      { return name == key; }
 public:
   chemical_types chemical_type;
   float64 balance_leached_molecular;   //moved to chemical_balance_accumulators
   float64 reported_leached_molecular;  //moved to chemical_balance_accumulators

    //  if irrigation with chemical added during the day,
    //   then this is set up by the management.
   float64 daily_requirements;
   float64 total_requirements;
   float64 applied_irrig_soil_M; // kg chem/m2_soil for output only!
      // rename to applied_with_irrigation_to_soil_M
   #ifdef WATERTABLE
   float64 water_table_concentration;                                            //071214
   #endif
   float64 nu;                                                                   //080606
 protected: // References from soil parameters (in layering system)
   cognate_ const Langmuir *langmuir;                                            //190404
      // Langmuir values may not be applicable to all chemicals
 public: // structors
   Chemical_profile
      (const CORN::date32 &today_                                                //170525
      ,const std::string &chem_name
      ,chemical_types _chemical_type
      ,float64 _molecular_to_elemental
      ,float64 _element_to_chem_factor
      #if (CROPSYST_VERSION==4)
      ,float64 _control_transformation_adjustment
      #endif
      ,soil_layer_array64(simulation_amount_E)
      //200312#ifndef OLD_N_XFER
      ,Chemical_pool *receiver_                                                  //170502
      //200312#endif
      ,const Layers_interface               &soil_layers_                        //150925
      ,const Hydrology_interface            &soil_hydrology_                     //150925
      ,const Hydraulic_properties_interface &soil_hydraulic_properties_          //150925
      ,const Structure                      &soil_structure_                     //150925
      ,Infiltration_model infiltration_model                                     //080117
      ,Water_table_curve *water_table_curve);                                    //000504
   virtual ~Chemical_profile();                                                  //000504
   bool provide_surface_mass_pending_infiltration();                             //071214
 public:
   inline bool know_Langmuir(const Langmuir &langmuir_)               cognition_ //190404
      { return langmuir = &langmuir_; }                                          //190404
 public:
   virtual bool start_day()                                       modification_; //071015
 public:
   void application
      (float64  addition  // <- need convert to molecular in caller
      ,nat8     sublayer
      ,nat8  split_into_sublayers                                                //990301
      ,nat8  soil_num_sublayers                                                  //990301
      ,bool is_elemental);                                                       //990301
   void application_at_layer                                                     //160120
      (float64  addition  // <- need convert to molecular in caller
      ,nat8     at_layer
      ,bool is_elemental);
   void application_concentration
      (float64 addition_concentration   // kg/m3<- need convert to molecular in caller
       ,float64 irrigation);
   #ifdef FINITE_DIFF
   void transport_bidirectional_by_node    // uses system of equations
      (nat8  num_nodes_eff
      ,nat8  number_of_real_plus_extended_nodes                                  //080219
      ,bool water_table_condition                                                //080206
      ,soil_layer_array64(water_flow) // m3 water/m2 soil (m) in the day  flow out of the sublayer //070117
      ,soil_node_array64(old_node_water_mass)
      ,soil_node_array64(new_node_water_mass)
      ,float64 water_entering  // total water entering (irrig + precip)
      ,Seconds                at_time                                            //071015
      ,Seconds                duration // seconds of duration                    //071015
      ,bool override_drainage_depth                                              //981230
      ,nat8  drainage_sublayer);                                                 //981230
      // transport_bidirectional() is used with infiltration models
      // that move water up and down between layers or only down
      // (Intended for finite difference )
   #endif
   void transport_unidirectional                                                 //080605_041103
      (nat8  num_sublayers_eff
      ,soil_layer_array64(flow)  // m3 water/m2 soil (m water depth)   flow out of the sublayer
      ,soil_layer_array64(old_liquid_water_content)
      ,soil_layer_array64(new_liquid_water_content)
      ,float64 water_entering
      ,Seconds                at_time
      ,Seconds                duration // seconds of duration
      ,bool override_drainage_depth
      ,nat8  drainage_sublayer);
      // transport_unidirectional() is used with infiltration models
      // that move only move water downward such as the cascade model.
   void transport_unidirectional_original                                        //071001
      (nat8  num_sublayers_eff
      ,soil_layer_array64(flow)  // m3 water/m2 soil (m water depth)   flow out of the sublayer
      ,soil_layer_array64(old_liquid_water_content)
      ,soil_layer_array64(new_liquid_water_content)
      ,float64 water_entering
      ,Seconds                at_time                                            //071015
      ,Seconds                duration // seconds of duration                    //071015
      ,bool override_drainage_depth
      ,nat8  drainage_sublayer);
      // transport_unidirectional() is used with infiltration models
      // that move only move water downward such as the cascade model.
#ifdef WATERTABLE
   virtual float64 get_water_table_concentration();                              //000504
   bool adjust_water_table_concentration(
       nat8  number_FD_soluation_layers                                          //080220
      ,nat8  number_of_real_plus_extended_layers);                               //071214
      // In water table conditions, the chemical concentration in soil layers under watertable
      // will be set to the water table concentration.
#endif
   inline virtual void setup_for_transformations()                            {} //060608 Currently only overridden by  NO3_with_CO2_Profile  but could be used by other
   float64 calc_Langmuir_solute_equilibrium_concentration                        //080115
      (float64 chemical_mass
      ,float64 soil_bulk_density
      ,float64 layer_thickness
      ,float64 water_content_volumetric
      ,float64 &chem_mass_fraction_in_solution
      ,float64 K                                                                 //190403
      ,float64 Q                                                                 //190403
      ) const;
   bool inc_applied(float64 addition_molecular);                                 //990317
   bool inc_applied_irrig_soil(float64 addition_molecular);                      //990317
   bool inc_applied_and_applied_irrig_soil_subsurface(float64 addition_molecular);//071015

   // Rename _M to _molecular
   void inc_balance_leached_M(float64 leached_molecular);                        //990317
   void inc_reported_leached_M(float64 leached_molecular);                       //990317
   void set_reported_leached_M();                                                //990317
   #ifdef CROPSYST_PROPER
   void update_balance_pending_infiltration();                                   //071016
   #endif
   inline virtual void update_concentration_for_output(nat8 num_layers_eff) {}   //080403
      // Salinity_profile will override.                                         //080606
 private:
   void resolution_of_Thomas_agorithm
      (nat8  num_slots_eff    // This will be either the number of layers or nodes
      ,soil_node_array64(a_term)    // layer or nodal array
      ,soil_node_array64(b_term)    // layer or nodal array
      ,soil_node_array64(c_term)    // layer or nodal array
      ,soil_node_array64(d_term)    // layer or nodal array
      ,soil_node_array64(new_water_depth_slotal)
      ,soil_layer_array64(slot_concentration)
      ,soil_node_array64(sorbed_chemical_mass) // layer or nodal array           //080606
      ,soil_layer_array64(slot_chemical_mass));                                  //080606
};
//______________________________________________________________________________
   // chemical uptake is derived from chemical profile
   //  In addition to transformation and movement, the chemical
   //  may also be uptaken by the plant or moved by the plant
   //  within the profile.
class Chemical_uptake_profile : public Chemical_profile
{
  inline virtual bool has_uptake()                         const { return true;} //990317
 public: // structors
   Chemical_uptake_profile
      (const CORN::date32 &simdate_                                              //170525
      ,const std::string &chem_name
      ,chemical_types _chemical_type
      ,float64 molecular_to_elemental
      ,float64 element_to_chem_factor
      #if (CROPSYST_VERSION==4)
      ,float64 control_transformation_adjustment
      #endif
      ,soil_layer_array64(simulation_amount_E)  //const
      //200312#ifndef OLD_N_XFER
      ,Chemical_pool *receiver_                                                  //170502
      //200312#endif
      ,const Layers_interface               &soil_layers_                        //150925
      ,const Hydrology_interface            &soil_hydrology_                     //150925
      ,const Hydraulic_properties_interface &soil_hydraulic_properties_          //150925
      ,const Structure                      &soil_structure_                     //150925
      ,Infiltration_model infiltration_model                                     //080117
      ,Water_table_curve *_water_table_curve);                                   //000504
 public:
  virtual float64 uptake_elemental(nat8  sublayer, float64 requested_elemental_N_uptake); //980730
};
//______________________________________________________________________________
} // namespace Soil
#endif
//cs_chem.h

