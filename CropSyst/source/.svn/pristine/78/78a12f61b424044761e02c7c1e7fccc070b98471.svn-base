#ifndef chemicals_profileH
#define chemicals_profileH

/* This virtual soil profile superclass adds soil chemistry
   to a derived soil class
*/
#include "options.h"
#include "soil/nitrogen_common.h"
#include "common/soil/layering.h"
#include "model_options.h"
#include "soil/nitrogen_profile.h"
extern bool quick_and_dirty_disable_chem_balance_when_watertable;                //LML 150427 need talk with Roger
#include "CS_suite/observation/CS_observation.h"

class Soil_base;                                                                 //051201

namespace CropSyst { class Land_unit_simulation_VX; }                            //020628
namespace Soil
{                                                                                //181206
class Temperature_interface;                                                     //181206
class Salt_interface;                                                            //090821
class Soil_salt;                                                                 //181206
class Abiotic_environment_profile;                                               //090821
//______________________________________________________________________________
class Chemicals_profile
: public Nitrogen_common
{
 public: // public so the soil profile spreadsheet can access                    //981230
   #ifdef NITROGEN
   #if (CROPSYST_VERSION==5)
   NH4_Profile_with_pH *NH4;
   #else
   NH4_Profile *NH4;                    // Ammonium soil profile
   #endif
   NO3_Profile *NO3;                    // Nitrate soil profile
   #endif
   #ifdef SALINITY
   Soil::Soil_salt    *salinity_dSm;   bool owns_salinity;                       //051118
   #endif
#ifdef PHOSPHORUS
   float64 phosphorus;                                                           //110801
      // Not currently distributed by layer because
      // I don't know uptake dynamics.
   //NYI soil_layer_array64(phosphorus);
#endif
   float64 reported_daily_N_leached; // kgN/ha                                   //130911
 public:
   const Soil_interface     &soil;                                               //150424_020527
   const Layers_interface   &soil_layers;                                        //061004
 public:                                                                         //130925
   CORN::Bidirectional_list chemical_profiles;                                   //170425_980731
      // Unidirectional list may be adequate, but need to check  181031
 protected:
   const Temperature_interface &soil_temperature;                                //020527
   bool enable_denitrification_with_CO2;                                         //080818
#ifdef NITROGEN
   float64 ACC_NH4_released_to_atmosphere; // elemental N from NH4 from manure decomposition may be lost to the atmosphere.
#endif
   #if (CROPSYST_VERSION==4)
   float64 max_transform_depth;
   #endif
              /* phosphorus : chemical_uptake_profile;
              potassium : chemical_uptake_profile;
              pesticide1: chemical_uptake_profile;
              pesticide2: chemical_uptake_profile;
              */
 public: // so soil profile spreadsheet can see it.
   nat8 trans_layers;
#ifdef NITROGEN
   //130930 Note that layer index 0 is reserved for the profile value
   // (sum of the layers)
   // N elemental output only
   soil_layer_array64(denitrification_N_E_output);
   soil_layer_array64(nitrification_N_E_output);
   soil_layer_array64(mineralization_N_E_output);
   soil_layer_array64(NO3_N_E_output);
   soil_layer_array64(NH4_N_E_output);
   soil_layer_array64(total_N_E);
      // NO3 + NH4 (elemental N) in each layer.
 public:
   soil_layer_array64(carbon_E);                                                 //040206
#endif
 public:
   Chemicals_profile                                                             //980724
      (const Soil_interface         &soil                                        //181213
      ,const Layers_interface       &soil_layers                                 //061004
      ,const Temperature_interface  &soil_temperature);                          //020527
   inline virtual ~Chemicals_profile() {}                                        //150709
 public:
   bool enable_nitrogen                                                          //020620
      (bool                               enable_denitrification_with_CO2        //060508
      ,Infiltration_model                 infiltration_model                     //041116
      ,Abiotic_environment_profile       *soil_abiotic_environment_profile       //060510
      #if (CROPSYST_VERSION==4)
      ,float64                            max_transform_depth
      , float64                           adjust_denitrify
      , float64                           adjust_nitrify_V4_1
      #endif
      , const CropSyst::Denitrification_parameters &denitrification_parameters   //150824
      , const float64          &_ref_water_flux_in_m                             //150925
      , soil_horizon_array32             (horizon_N_NO3)
      , soil_horizon_array32             (horizon_N_NH4)
      , Water_table_curve                *NO3_water_table_curve // may be 0 if no water table consideration //080213
      , Water_table_curve                *NH4_water_table_curve // may be 0 if no water table consideration   //080213
      , const CORN::date32 &today_);
#ifdef PHOSPHORUS
   //NYI bool enable_phosphorus
   //NYI   (  soil_horizon_array32           (horizon_P));                       //110801
   float32 know_phosphorus_in_profile(float32 phosporus_profile)     cognition_; //110801
#endif
#ifdef SALINITY
   virtual bool set_salinity(Salt_interface *salinity_dSm,bool keep_it = true);  //051120
   virtual const       Salt_interface *ref_salinity()                     const; //160412_051120
   virtual modifiable_ Salt_interface *mod_salinity()             modification_; //160412_051120
#endif
   void setup_chemical_profiles                                                  //980730
      ( Bidirectional_list &initial_chemical_profiles
      , bool run_chemical
#ifdef SALINITY
                  , float64 water_table_salinity
                  , soil_horizon_array32(salt)
#endif
#ifdef WATERTABLE
         , const char * water_table_filename
         , const CORN::Date_const &start_date
         , const CORN::Date_const &end_date
#endif
      );
   bool know_NH4_Langmuir(const Langmuir &NH4_Langmuir)              cognition_; //190404
   bool initialize();                                                            //981029
   void chemical_balances();
   void chemical_transformations
      (Seconds preferred_transformation_timestep); // This option is because the hourly moisture function calculation can not be resolved with daily infiltration
   bool reinitialize_at_end_season();  //was reinitialize_for_harvest            //120704
      // conceptual: probably rename to end_season()
   bool reinitialize_for_crop();
      // conceptual: probably rename to start_season()
   bool reinitialize();
   virtual void chemical_transport_and_exchange                                  //980724
      (Infiltration_model infiltration_model                                     //080114
      ,nat8               num_layers_or_nodes_eff  // pass number of effective nodes for F.D.
      ,nat8               number_of_real_plus_extended_nodes // only for F.D.    //080219
      ,Seconds            at_time                                                //071015
      ,Seconds            duration // seconds of duration                        //071015
      ,soil_layer_array64 (old_liquid_water_content)
      ,soil_layer_array64 (new_liquid_water_content)
      ,float64             water_entering  // total water entering (irrig + precip)
      ,soil_layer_array64 (water_flow) //node based for F.D.otherwise layer base //080208
      ,float64 deferment_fraction                                                //071016
      ,bool    override_leaching_depth                                           //981230
      ,nat8    leaching_layer                                                    //981230
      ,bool water_table_conditions=false);                                       //080207
   virtual bool start_day()                                       modification_; //151113_150925_151006_071015
   virtual bool end_day()                                         modification_; //130911
   #ifdef WATERTABLE
   void adjust_water_table_concentration                                         //080218
      (nat8  number_FD_soluation_layers                                          //080220
      ,nat8  number_of_real_plus_extended_layers);                               //080220
   #endif
 private: // converstions for CropGro
   float64 kg_per_m2_to_micrograms_per_g(nat8  layer, float64 amount_kg_ha) const;   // May want to put this in soil base, but so far it is only used here
   float64 micrograms_per_g_to_kg_per_m2(nat8  layer, float64 amount_micrograms_per_g) const;   // May want to put this in soil base, but so far it is only used here
 public: // Soil_chemicals_profile_abstract overrides
                  // Not sure but these may be the same as ppm

   virtual float64 get_NO3_molecular_mass_kg_m2(nat8  layer)              const; //040520
   virtual float64 get_NH4_molecular_mass_kg_m2(nat8  layer)              const; //040520
   virtual float64 get_NO3_N_mass_kg_m2        (nat8  layer)              const; //040520
   virtual float64 get_NH4_N_mass_kg_m2        (nat8  layer)              const; //040520
   virtual float64 add_NO3_molecular_mass      (nat8  layer,float64 NO3_mass_M); //040520
   virtual float64 add_NH4_molecular_mass      (nat8  layer,float64 NH4_mass_M); //040520
   virtual float64 subtract_NO3_molecular_mass (nat8  layer,float64 NO3_mass_M); //040520
   virtual float64 subtract_NH4_molecular_mass (nat8  layer,float64 NH4_mass_M); //040520

   virtual float64 set_NO3_N_mass_kg_m2        (nat8  layer,float64 NO3_N_mass_kg_m2);  //040520
   virtual float64 set_NH4_N_mass_kg_m2        (nat8  layer,float64 NH4_N_mass_kg_m2);  //040520

   #ifdef CROPSYST_PROPER
   // The following are for balancing soil residue N in CropSyst only
   virtual void NO3_dec_residue(float64 NO3_amount_change_M);                    //040521
   virtual void NH4_dec_residue(float64 NH4_amount_change_M);                    //040521
   #endif
 public:
#ifdef NITROGEN
//040520 Note the following get_elemental_N_from_NO3 may now be obolsete
// replaced by Soil_interface  get_NO3_N_mass_kg_m2 and  get_NH4_N_mass_kg_m2
   float64 get_elemental_N_from_NO3(nat8  layer)                          const;
   float64 get_elemental_N_from_NH4(nat8  layer)                          const;
   inline virtual float64 &ref_ACC_NH4_released_to_atmosphere()                  //060122
      {return ACC_NH4_released_to_atmosphere;}
   // 080116 Can no longer reference these values directly because layer values could be distributed by node.
   float64 get_NO3_molecular_mass_kg_m2(nat8  layer);                            //060122
   float64 get_NH4_molecular_mass_kg_m2(nat8  layer);                            //060122
   float64 set_NO3_molecular_mass_kg_m2(nat8  layer,float64 new_mass);           //060122
   float64 set_NH4_molecular_mass_kg_m2(nat8  layer,float64 new_mass);           //060122
   virtual float64 get_NH4_N_mass_mineralized_from_PMN_kg_m2_profile()    const; //060123
   virtual float64 get_NH4_N_mass_mineralized_from_PMN_kg_m2(nat8  layer) const; //060123
   float64 sum_NO3_to_depth                                                      //990208
      (float64 a_given_depth
      ,bool    elemental)                                                 const;
   float64 sum_NH4_to_depth                                                      //120710
      (float64 a_given_depth
      ,bool    elemental)                                                 const;
   float64 sum_N_to_depth                                                        //170630
      (float64 a_given_depth)                                             const;
   bool is_below_critical_N                                                      //170630
      (float64 critical_N_kg_m2
      ,float64 sampling_depth)                                     affirmation_;
   virtual float64 uptake_N_mass_from_NO3
      (nat8  layer, float64 requested_pot_elemental_N_uptake_kg_m2);
   // Attempts to removed the requested elemental N from the NO3 pool.
   // Returns the actual elemental N uptaken from the pool.

   virtual float64 uptake_N_mass_from_NH4
      (nat8  layer, float64 requested_pot_elemental_N_uptake_kg_m2);
   // Attempts to removed the requested elemental N from the NH4 pool.
   // Returns the actual elemental N uptaken from the pool.

   // Although organic matter exists for all chemicals,
   // It only changes for with nitrogen so no need to output it unless N active.
#endif
   bool setup_structure
      (CORN::Data_record &data_rec,bool for_write);                                                                         //030801
   virtual void commit_diffs()                                    modification_;
   RENDER_INSPECTORS_DECLARATION;                                                //150916
};
//_Chemicals_profile________________________________________________1998-07-29_/
}//_namespace_Soil_____________________________________________________________/
#endif

