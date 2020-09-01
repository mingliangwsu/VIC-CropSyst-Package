#ifndef canopy_growthH
#define canopy_growthH
#include "corn/const.h"
#include "crop/crop_types.h"
#include "corn/container/bilist.h"
#include "corn/chronometry/time_types.hpp"
class Canopy_growth_LAI_based_parameters;
class Residue_decomposition_parameters;
class Residues_interface;
class Crop_thermal_time_interface_immutable;
/*191025 moved below
namespace CropSyst {
class Crop_parameters;
}
*/

#if (PHENOLOGY_VERSION==2018)
#include "crop/phenology_2018.h"
#endif

#if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
#include "crop/growth_stages.hpp"
#include "crop/phenology_2013.h"
#endif
namespace CropSyst {
class Crop_parameters;
class Crop_biomass;
class Crop_nitrogen_interface;
//---------------------------------------------------------------------------
interface_ Canopy_leaf_growth
{
   inline virtual ~Canopy_leaf_growth()                                       {} //151204
   virtual bool initialize()                                    modification_=0;
   virtual bool invalidate()                                    modification_=0;
   virtual bool update()                                       rectification_=0;
   virtual bool process_day()                                   modification_=0; //080910 //180111 was process()
   virtual bool start()                                         modification_=0; //080811
   virtual bool restart_with
      (float64 restart_biomass,float64 restart_GAI
      ,bool use_clumping_factor = true)                         modification_=0;
   // Temporal processiong
   virtual bool end_day()                                       modification_=0; //190821
   virtual bool emerge_cotyledon
      (float64 new_cotyledon_GAI
      ,float64 new_cotyledon_BM)                                modification_=0; //080728
   virtual bool know_accrescence  (const Phenology::Period_thermal *accrescence)    modification_ {return true;} //181109
   virtual bool know_culminescence(const Phenology::Period_thermal *culminescence_) modification_ {return true;} //181109
   virtual bool know_senescence   (const Phenology::Period_thermal *senescence)     modification_ {return true;} //181109
   virtual bool know_maturity_initiation()                      modification_=0; //200409
   inline virtual bool know_N_leaf_stress_factor
      (float64 N_leaf_stress_factor_)                             {return true;} //200528

   virtual bool end_season(bool apply_dormancy)                 modification_=0; //080401
   // Event processing
   virtual bool respond_to_clipping()                           modification_=0; //080515
   virtual nat32 vital_becomes_effete(bool apply_dormancy)      modification_=0; //180718
   virtual void die_back(float64 fraction_to_die)               modification_=0;
   virtual float64 damage_GAI(float64 GAI_damage)               modification_=0; //151226
      ///< kills the specified GAI_damage.
      ///  \returns the amount of live GAI remaining.
   virtual void terminate()                                     modification_=0; //080728
   virtual float64 remove_biomass                                                //070412
      (float64 biomass_to_remove
      ,float64 retain_living // GAI or fCCg retain_GAI        // biomass won't be removed if it takes the GAI below this level. Use 0.0 for no limitation
      ,bool remove_newest_first
      ,bool reserve_todays_portion
      ,Crop_biomass &removed_biomass )                          modification_=0;
         // Return about of biomass removed
   virtual float64 remove_all_biomass
      (Crop_biomass &removed_biomass)                           modification_=0;
   // status query
   virtual float64 get_biomass_current(nat8 include_biomass)            const=0; // The current biomass (Some may have decomposed) 060601
   virtual float64 get_influence_on_root_activity_factor
      (bool after_end_of_vegetative_growth)                             const=0; //080805
   virtual float64 get_interception_global_total()                      const=0; //200228 get_fract_canopy_cover()
   virtual modifiable_ float64 &reference_fract_canopy_cover()  modification_=0; // only for output? 080801
   virtual void know_residues(Residues_interface *_residues)                = 0; //060531
   virtual float64 get_interception_global_green()                      const=0; //110906 Appears to be used only for CO2

   // The following will require something equivelent in the canopy based model
   virtual float64 calc_cover_fract
      (float64 xAI
      , bool photosynthetic_active_rad_based
      ,bool use_clumping_factor = true                                           //080801
      ,bool record_shading_porosity_interception = false )              const=0; //000623
      // for fruit trees the default record_shading_porosity_interception is true 081104

   virtual float64 get_senescence_reduction_factor()                    const=0; //080807
   virtual float64 calc_adjusted_ET_crop_coefficient()                  const=0; //200127_080811
   virtual float64 calc_plant_development                                        //990215
      (float64 length_max
      ,float64 last_length
      ,float64 overall_growth_stress)                                   const=0; //080805
   // probably only for output, canopy cover based will probaby not need this
   virtual float64 get_GAI(nat8 include_GAI)                            const=0;
   virtual float64 get_LAI()                                            const=0; //060824
   virtual float64 get_LAI(bool from_canopy)                            const=0; //080728
   // probably used elsewhere in the model
   virtual float64 get_live_green_biomass
      (bool include_vital,bool include_effete)                          const=0;
   // The following only apply the LAI based mode
   virtual bool age_leaf
      (float64  thermal_time_growing_degree_day
      ,float64 water_stress_index)                              modification_=0; //070328
   virtual bool develop_leaf
     (bool continue_leaf_growth
     ,float64 canopy_growth
     ,float64 LAI_related_growth  // Today's LAI related growth
     ,float64 thermal_time_growing_degree_day)                  modification_=0; //080728
   virtual float64 update_LAI_for_self_shading_response
      (bool after_accescence,bool before_maturity)              modification_=0;
   virtual float64 know_LWP_yesterday(float64 _LWP_yesterday)      cognition_=0;
   virtual float64 get_interception_PAR_green()                         const=0;
   //200228 virtual float64 get_interception_global_green()                      const=0;
   virtual float64 get_reported_peak_LAI()                              const=0; //141203
   virtual float64 get_peak_LAI()                                       const=0; //141203
};
//_Canopy_leaf_growth__________________________________________________________/
}//_namespace CropSyst_________________________________________________________/
#endif

