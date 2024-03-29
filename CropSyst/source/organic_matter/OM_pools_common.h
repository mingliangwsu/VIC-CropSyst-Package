#ifndef OM_pools_commonH
#define OM_pools_commonH
#include "corn/primitive.h"
#include "corn/container/unilist.h"
#include "common/soil/layering.h"
#include "common/residue/residue_const.h"
#include "organic_matter/OM_types.h"
#include "soil/soil_I.h"

#include "CS_suite/observation/CS_observation.h"

namespace Soil
{
   class Structure;
   class Abiotic_environment_profile;
   class Abiotic_environment_layer;
   class Disturbance;
}
class Organic_matter_initialization_parameters;                                  //111004
class Organic_matter_decomposition_parameters;                                   //111004
class Organic_biomatter_pool_common;                                             //181214
class Organic_biomatter_common;                                                  //181214
//______________________________________________________________________________
class Organic_matter_total_common
{  // common to the V4.5 Single pool and V4.4 multipools
public:
   float64 mass[MASS_TOTAL_COUNT];
public:
   Organic_matter_total_common();
   void add(const Organic_biomatter_common &other);
   void clear();
};
//_Organic_matter_total_common_________________________________________________/
class Organic_matter_pools_totals_common
{   // common to the V4.5 Single pool and V4.4 multipools
 protected:
   friend class Organic_matter_pools_multiple;
   friend class Organic_matter_pools_common;
   friend class Residues_V4_4;
   Organic_matter_total_common masses[ORGANIC_MATTER_TYPE_COUNT][OM_POSITION_COUNT];
   // Note, I think mineralization and immobilization only occur in subsurface,
   // so these might not need to be in the array by position
   // just a single sum for each organic matter type
   //NYN    float64 N_mineralized[ORGANIC_MATTER_TYPE_COUNT];
 public:
   Organic_matter_pools_totals_common();
   void add(const Organic_biomatter_common &org_BM);
};
//_Organic_matter_pools_totals_common__________________________________________/
class Organic_matter_pools_common   // rename because this is not an abstract class
: public CORN::Unidirectional_list
{  // Lists all Organic_biomatter_V4_4 pools (including SOM and residue)
   // This table points to the listed Stable organic matter
   // pools with the new tillage it is convenient (faster) to
   // work with this table rather than looking up the pools
   // in the list.
   // This table also allows only those pools that are needed to be
   // allocated and listed (use provide_organic_matter_pool).
   // Currently plant residue and manure are not included in this table.
 protected:
   mutable Organic_biomatter_common *organic_matter[ORGANIC_MATTER_TYPE_COUNT][MAX_soil_layers_alloc]; //190215
      // References, do not delete these //060726
 protected:
   friend class Residues_V4_common;
   friend class Residues_V4_4;
   mutable Organic_matter_pools_totals_common *pools_totals;
/*190115 each pool how has their own contact fraction  and the subsurface is always 1
 public: // The following were in Organic_matter_pools_profile_V4_4
   float64                           subsurface_contact_fraction; // Always 1.0
*/
 protected:
   const Organic_matter_decomposition_parameters &OM_decomposition_parameters;
   const Soil::Texture_interface             &soil_texture;                      //070707
   Soil::Abiotic_environment_profile         &soil_abiotic_environment_profile;  //060726
   const Soil::Layers_interface              &soil_layers;                       //060726
   const Soil::Structure                     &soil_structure;                    //070725
   Soil::Nitrogen                            *soil_N;
      // Owned by simulation // May be 0 if not running nitrogen
      // when 0, no totals are available and will need to be recalculated
   const Soil::Disturbance                    *soil_disturbance;
      // optional, when 0 then no tillage effect on decomposition                //060807
   float64                              CO2_C_loss_daily[3];
      // sum of subsurface CO2_losses (kg C/m2?)                  //060811
      // 0 it the total
      // 1 is SOM
      // 2 is residue
   float64                             today_OM_mineralized_kgN_m2;
   float64                             today_N_immobilized;                      //060717
   float64                             C_fraction_transfered_to_CO2;
 protected:
   contribute_ float64                 subsurface_contact_fraction;              //190327
      // might be the same as total_daily_CO2_loss
 protected: // For process today (by layer)
   // pot_C_decomposition outputs
   soil_layer_array64(immobilized_NO3_N); // kgN/m2
   soil_layer_array64(immobilized_NH4_N); // kgN/m2
   soil_layer_array64(CO2_C_loss_subsurface_residue_and_OM);// kgC/m2            //060504
      // Used by new denitrification (NO3_with_CO2_Profile)
   soil_layer_array64(CO2_C_loss_subsurface_OM);            // kgC/m2            //080902
      // Used by new denitrification (NO3_with_CO2_Profile)
   soil_layer_array64(CO2_C_loss_subsurface_residue);       // kgC/m2            //080902
      // Used by new denitrification (NO3_with_CO2_Profile)
   soil_layer_array64(N_immobilization_demand_sum/*190513 _subsurface*/); // kgN/m2
      // Now surface and subsurface (surface is at index 0)                      //190513
   soil_layer_array64(mineralized_N_residue);       // kgN/m2 (subsurface)
      // Some models doe not have residue N mineralization
   soil_layer_array64(mineralized_N_OM);           // kgN/m2
   soil_layer_array64(mineralized_N_OM_residue);   // kgC/m2

   float64 C_incorporated_by_tillage;  // currently used only for output         //060817
   // act_C_decomposition inputs
   soil_layer_array64(act_C_decomp_subsurface);
   float64 act_C_decomposed_from_all      [OM_POSITION_COUNT];
   float64 act_C_decomposed_from_residue  [OM_POSITION_COUNT];
   soil_layer_array64(N_deficit_for_immobilization_subsurface);                  //110928
   soil_layer_array64(reduction_factor_subsurface);                              //110928
 protected: // Moved from OM_pools_multiple
   soil_layer_array64(pot_C_decomp_subsurface);
   float64 pot_C_decomp_surface  [OM_POSITION_COUNT];                            //111004
 public:
   Organic_matter_pools_common
      (const Organic_matter_decomposition_parameters &OM_decomp_params           //120912
      ,Soil::Abiotic_environment_profile  &soil_abiotic_environment_profile      //060726
      ,const Soil::Layers_interface       &soil_layers                           //060726
      ,const Soil::Structure              &soil_structure                        //070725
      ,Soil::Nitrogen                     *soil_N
      ,const Soil::Texture_interface      &soil_texture);                        //070707
   virtual ~Organic_matter_pools_common();                                       //160616
   void invalidate_totals()                                            mutates_;
   virtual bool set_modified(bool modified)                            mutates_;
   virtual  nat32 update_totals()                                    provision_;
   virtual bool start_day();                                                     //060330
   virtual bool process();
   virtual bool initialize_layer
      (nat8 horizon,nat8 layer
      ,float64 sand_fraction                                                     //060418
      ,float64 initial_organic_matter_carbon_mass
      ,float64 microbial_decomposition_const
      ,const Organic_matter_initialization_parameters &OM_historical_parameters  //060215
      ,const Soil::Abiotic_environment_layer &soil_abiotic_environment_layer_ref       //190115
         //190215 *point_soil_abiotic_environment_layer
      )=0; //111004
   virtual bool clear_residues()                                   modification_;//160126
protected: // subprocesses
   void update_soil_N(nat8 residue_horizon);                       modification_;
   bool residue_detrition()                                        modification_;//090702
      // detrition is non-microbial decomposition
public:  // Events
   bool respond_to_field_operation                                               //060726
      (float64           tillage_op_depth_m
      ,float64           tillage_op_mixing_fraction
      ,const Soil::Texture_interface              &soil_texture                   //060807
      ,const Soil::Disturbance *_soil_disturbance); // 0 = optional               //060807
   inline virtual const Soil::Disturbance *know_soil_disturbance                  //070801
      (const Soil::Disturbance *_soil_disturbance)                  modification_ //070801
      { return soil_disturbance = _soil_disturbance; };                          //070801
   virtual bool take_pools(CORN::Unidirectional_list &pools)       modification_;//111004_070808
      // This is used by respond_to_field_operation (for OM and residue pools)   //070808
      // When residues are redistributes, new pools may be created.              //070808
      // This adds the new pools.                                                //070808
      // Used to recalibrate residues.
   virtual bool take_OM_pool(Organic_biomatter_pool_common *OM_pool) appropriation_;//161116
   virtual bool specialized_reference                                            //170228
      (Organic_biomatter_pool_common *OM_pool_common)                        =0;
 public:  // Accessors get_total_xxx()
   virtual Organic_biomatter_common  *provide_organic_matter_pool                //070707
      (Organic_matter_type type,nat8 layer)                      provision_ = 0; //190215
   virtual Organic_biomatter_common *get_organic_matter_pool                     //170301
      (Organic_matter_type type,nat8  layer);
      // May return 0 if not applicable/available
   virtual Organic_biomatter_common  *render_OM_pool                             //111001
      (Organic_matter_type     type
      ,Organic_matter_position position
      ,nat8                    layer
      ,Organic_matter_cycling  cycling
      ,RUSLE2_Residue_type     RUSLE2_residue_type                              //060731
      ,float64                 biomass
      ,float64                 carbon_fraction
      ,float64                 carbon_nitrogen_ratio
      ,float64                 decomposition_constant                           //090703
      ,float64                 detrition_constant                               //090413
      ,float64                 area_per_mass      // = 0.0  stubble and surface residue_only?
      ,float64                &contact_fraction
      ,const Soil::Texture_interface &soil_texture                               //070707
      ,const Soil::Abiotic_environment_layer &soil_abiotic_env_layer
      //190215 , Soil::Abiotic_environment_layer * _soil_abiotic_env_layer
      ) rendition_=0;
   virtual Organic_biomatter_pool_common *render_OM_pool_for_read()rendition_=0;//161116
   bool  is_included                                                             //060403
      (Organic_matter_type om_type
      , nat32 include_organic_matter_types_mask)                          const;
   inline virtual bool is_applicable(Organic_matter_type OM_type)   affirmation_ //170301
      { return true; }
   virtual nat8 SOM_index_last()                                        const=0; //170301

   // These should be get_total
   virtual float64 get_mass                                                      //060331
      (Mass_total mass
      ,nat32 include_organic_matter_types
      ,nat32 include_positions)                                           const;
   virtual float64 get_mass_from_layer_to_layer                                  //071214
      (Mass_total mass_total
      ,nat32 include_organic_matter_types
      ,nat8 from_layer
      ,nat8 to_layer
      ,float64 prorate_fraction_bottom_layer)                             const;
   virtual float64 get_nitrogen_mass_including
      ( nat32 include_organic_matter_types, nat32 include_positions)      const;
   virtual float64 get_total_N_immobilized()                              const; //060403
   inline virtual float64 get_immobilization_N(nat8 layer)                const  //061218
      { return immobilized_NO3_N[layer] + immobilized_NH4_N[layer]; }
   virtual float64 get_soil_organic_matter_mineralization_N(nat8 layer)   const; //060403
   virtual float64 get_soil_organic_matter_mineralization_profile_N()     const; //060403
      // probably the same as today_OM_mineralized_kgN_m2
           float64 get_CO2_C_loss_reported_daily(nat8 include_OM)         const; //060426
   virtual float64 get_soil_organic_matter_immobilization_profile_N()     const; //110918
   float64 get_C_decomposed_from_layer_to_layer // This is used only for output carbon annual report 090520
      (nat32 include_organic_matter_types                                       //090520
      ,nat8 from_layer
      ,nat8 to_layer
      ,float64 prorate_fraction_bottom_layer)                              const;
   inline virtual float64 get_C_decomposed_from_residue                          //060817
      (Organic_matter_position from_position)                              const { return act_C_decomposed_from_residue[from_position]; }
   inline virtual float64 get_C_incorporated_by_tillage()                  const { return C_incorporated_by_tillage; }           //060817
   inline float64 *get_CO2_C_loss_profile_residue_and_OM()         modification_ { return CO2_C_loss_subsurface_residue_and_OM; }//060504
   inline float64 *get_CO2_C_loss_profile_OM()                     modification_ { return CO2_C_loss_subsurface_OM; }            //080902
   inline float64 *get_CO2_C_loss_profile_residue()                modification_ { return CO2_C_loss_subsurface_residue; }       //080902
 protected:
   virtual void clear_totals();
   virtual bool transfer_carbon_and_nitrogen_between_pools();                    //111004
 public:
   Organic_biomatter_common *find_similar_organic_biomatter
      (const Organic_biomatter_common &to_other);                                //060605
 public: // Used only by the single pool model
   inline const Organic_biomatter_common *ref_organic_biomatter
      (Organic_matter_type OM_type, nat8 layer)                            const //070712
      { return organic_matter[OM_type][layer]; }
   modifiable_ virtual Organic_biomatter_pool_common *get_metastable_SOM_ref(nat8 layer)    = 0;//110929
   modifiable_ virtual Organic_biomatter_pool_common *get_microbial_biomass_ref(nat8 layer) = 0;//110929
 protected:
   float64 determine_reduction_factor_surface
      (// member reduction_factor_subsurface
      float64 N_immobilization_demand_surface
      //member ,float64 &N_deficit_for_immobilization
      )                     modification_; //110928
      // N_deficit_for_immobilization returned
   float64 determine_reduction_factor
      (nat8   residue_horizon
      ,float64 soil_layer_NH4_N_mass
      ,float64 soil_layer_NO3_N_mass
      ,float64 N_immobilization_demand
      ,float64 &N_deficit_for_immobilization)                     modification_; //110928
      // N_deficit_for_immobilization returned
   bool determine_reduction_factor_layers()                       modification_; //110928
   virtual float64 potential_C_decomposition();                                  //111004
      ///< \return the potential carbon decomposition
      /// (the return value is not currently used probably could return bool )
   virtual float64 actual_C_decomposition();                                     //111004
      ///< \return the actual carbon decomposition
      /// (the return value is not currently used probably could return bool ) )
   virtual bool tally_immobilization()                            modification_;
 protected:
   float64 transfer_carbon_and_nitrogen                                          //111004
      (float64 &CO2_loss_SOM,float64 &CO2_loss_residue);
 public:
   //120912 this was added so we can store historical equilibrated OM for futher future scenario runs
   virtual nat32 write(std::ostream &)                             stream_IO_=0; //150728_120912
   virtual bool read(std::istream &stream)                           stream_IO_; //161116_120912
   inline const char *get_ontology()                                       const //160616
      { return "organic_matter"; }
   #ifdef CS_OBSERVATION
   contribute_ CS::Emanator *emanator_known;                                     //160616
   #endif
   RENDER_INSPECTORS_DECLARATION;
};
//_Organic_matter_pools_common_________________________________________________/
#endif

