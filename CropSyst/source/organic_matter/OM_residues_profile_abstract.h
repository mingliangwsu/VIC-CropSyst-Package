#ifndef OM_residues_profile_abstractH
#define OM_residues_profile_abstractH
#include "organic_matter/OM_types.h"
#include "corn/primitive.h"
#include "model_options.h"
#include "USDA/NRCS/RUSLE2/SDR_field_ops.h"
#include "common/soil/layering.h"
#include "CS_suite/simulation/CS_simulation_element.h"
#include "CS_suite/observation/CS_observation.h"
#define ENTIRE_PROFILE_DEPTH_as_0 0.0
#define ENTIRE_PROFILE_DEPTH_as_9999 9999
// actually currently defined in soil profile
//______________________________________________________________________________
class Residues_interface;
class Pot_mineralizable_N_profile;
namespace Soil
{
   class Soil_interface;
   interface_ Texture_interface;
   interface_ Layers_interface;
   interface_ Hydrology_interface;
   interface_ Nitrogen;
   interface_ Structure;
   class      Disturbance;
}
//______________________________________________________________________________
class Organic_matter_residues_profile_interface
#ifdef CROPSYST_PROPER
: public extends_interface_ CS::Simulation_element
#endif
{
 public: // Event processing
   virtual bool respond_to_field_operation                                       //060726
      (float32 tillage_op_depth_m
      ,float32 tillage_op_mixing_fraction
      ,USDA::NRCS::RUSLE2::Soil_disturbance_effect operation_soil_disturbance_effect // VB Operation_Soil_Disturbance_Effect
      ,const Soil::Disturbance *soil_disturbance_                                //060807
      ,const float32  residue_flatten_ratio[]
      ,const float32  residue_burial_ratio[]) = 0;
         // ManagementState.SurfaceResidueFractionRemainingAfterTillage
      // Note that these parameters are float32 bit because the in memory table uses float32
 public:
   virtual modifiable_ Residues_interface *mod_residues()                        //160627
      { return const_cast<Residues_interface *>(ref_residues()); }
   virtual const       Residues_interface *ref_residues()               const=0; //160627
   virtual bool clear_residues()                              initialization_=0; //160126
      // This deletes are residue pools (used for initialization and recalibration);
 public: // Specifically organic matter
   virtual float64 get_percent_organic_matter(nat8 sublayer)            const=0;
   virtual float64 get_organic_matter_mass(nat8 sublayer)               const=0;  //060401
 public: // carbon
   virtual float64 get_organic_carbon
      (nat8 sublayer, nat8 include_OM)                                  const=0;
   virtual float64 sum_carbon_to_depth_kg_m2
      (nat8 include_OM
      ,float64 to_depth_m // To depth of soil
      ,bool include_surface)                                            const=0;
   #if (defined(MICROBASIN_VERSION) || defined (VIC_CROPSYST_VERSION))
   virtual float64 sum_nitrogen_to_depth_kg_m2
      (nat8 include_OM
      ,float64 to_depth_m // To depth of soil
      ,bool include_surface)                                            const=0; //150722LML
      // ENTIRE_PROFILE_DEPTH defaults to the entire profile
   #endif
   virtual float64 sum_N_mineralization_to_depth
      (nat8 include_OM
      ,float64 to_depth_m   // To depth of soil
      ,bool include_surface )                                           const=0;//061116
   virtual float64 sum_N_immobilization_to_depth
      (nat8 include_OM
      ,float64 to_depth_m   // To depth of soil
      ,bool include_surface )                                           const=0;//061116
   virtual float64 sum_C_decomposition_to_depth
      (nat8 include_OM
      ,float64 to_depth_m)                                              const=0;//090520
 public: // The following are for the Nitrogen model return 0 if not simulating nitrogen
   virtual float64 get_total_straw_mineralization()                     const=0;
   virtual float64 get_total_manure_mineralization()                    const=0;
   virtual float64 get_N_immobilization_profile()                       const=0;  // only
   virtual float64 get_soil_organic_matter_mineralization_profile_N()   const=0;
   virtual float64 get_soil_organic_matter_immobilization_profile_N()   const=0; //110918
   virtual float64 get_NH4_mineralization()                             const=0;
   virtual float64 get_soil_organic_matter_mineralization_N(nat8 layer) const=0;
   virtual Pot_mineralizable_N_profile *get_pot_mineralizable_N_profile()    =0;
      // get_pot_mineralizable_N_profile is applies only to V 4.1.xx
      // everything else can simply return 0;
   virtual float64 get_gaseous_CO2_C_loss_reported_daily(nat8 include_OM)const=0;//060426
   virtual float64 *get_CO2_C_loss_profile_residue_and_OM()                  =0; //060504
   virtual float64 *get_CO2_C_loss_profile_OM()                              =0; //080902
   virtual float64 *get_CO2_C_loss_profile_residue()                         =0; //080902
      // This is used by NO3_with_CO2_Profile
      // Currently only available in Organic_matter_residues_profile_multiple_cycling
      // All other derived classes can simply return 0 if not available
      // 0 will disabled NO3_with_CO2_Profile transformation and default to NO3_Profile::transformation
   virtual float64 get_C_decomposed_from_residue
      (Organic_matter_position from_position)                           const=0; //060817
   virtual float64 get_C_incorporated_by_tillage()                      const=0; //060817
   virtual float64
      get_mass_from_layer_to_layer
      (Mass_total mass_total
      ,uint32 include_organic_matter_types
      ,nat8 from_layer
      ,nat8 to_layer
      ,float64 prorate_fraction_bottom_layer)                           const=0; //071214
 public: // The following are for everything
   virtual const Soil::Disturbance *know_soil_disturbance                        //070801
      (const Soil::Disturbance *_soil_disturbance)              modification_=0; //070801
   virtual bool  read(std::istream &stream)                      performs_IO_=0; //120912
};
//_Organic_matter_residues_profile_interface________________________2016-06-26_/
class Organic_matter_residues_profile_abstract
: public implements_ Organic_matter_residues_profile_interface
#ifdef CROPSYST_VERSION
// This condition was put in for OM compare
, public extends_ CS::Simulation_element_abstract
#endif
{
 protected: // These are all pointers/references owned by the soil do not delete
   const Soil::Texture_interface &soil_texture;    // required                //060122
   Soil::Nitrogen                *soil_nitrogen;   // optional                //060122
   const Soil::Structure         &soil_structure;                             //070725
   const Soil::Layers_interface  &soil_layers;     // required                //060122
   const Soil::Hydrology_interface   &soil_hydrology;  // required                //060122
      // soil_hydrology Doesn't appear to be used, need to check if obsolete
 public: // structors
   Organic_matter_residues_profile_abstract
      (Soil::Soil_interface   *_soil
      ,const CORN::date32 &simdate_bare_);                                       //170524_140507
 public:
   virtual float64 get_percent_organic_matter(nat8 sublayer)              const;
   virtual bool clear_residues()                                initialization_; //160126

      // Even though we implement this, it qualifies as an interface function
};
//_Organic_matter_residues_profile_abstract_________________________2005-07-26_/
#endif

