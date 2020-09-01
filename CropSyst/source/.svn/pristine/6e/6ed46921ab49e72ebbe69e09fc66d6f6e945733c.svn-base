#ifndef canopy_growth_portionedH
#define canopy_growth_portionedH
#include "crop/canopy_growth.h"
namespace CropSyst {                                                             //140318
class Crop_parameters;
class Crop_nitrogen_interface;                                                   //151006
}
namespace CropSyst {
interface_ Thermal_time_immutable;                                               //151106
//______________________________________________________________________________
class Canopy_growth_portioned
:public Canopy_leaf_growth
{  // This is actually Canopy_leaf_growth_portioned
 protected:
   const Crop_parameters         &crop_parameters;                               //020409_140318
      // Will be owned by this crop object
 protected: // Classes
   class Canopy_accumulation
   : public extends_ CORN::Bidirectional_list // list of portions                //120803
   {
   public:
      class Portion
      : public extends_ CORN::Item
      {
      protected:
         float64 biomass_produced;
         float64 biomass_current;
         CORN::Days  days_dead;                                                  //060824
      public:  // structors
         Portion(float64 _biomass);
         Portion();
         inline virtual ~Portion() {};                                           //120217
      public:
      public: // overrides
         virtual int compare(const CORN::Item &other)                     const;//970701
         inline virtual const char *label_string(std::string &buffer)     const  //030123
                                     { buffer.clear(); return buffer.c_str(); }
      public: // Accessors
         virtual bool    is_valid()                                       const;                                                                                                       //050823
         virtual bool    is_dead()                                      const=0; //   { return dead_AI > 0.000001; };                                                              //050823
         virtual float64 get_GAI()                                      const=0;                                                                                                   //050823
         virtual float64 get_living()                                   const=0; // Gets the living green portion GAI or fract_canopy_cover_green                                  //080806
         inline virtual float64 get_biomass_produced() const { return biomass_produced; } //060601
         inline virtual float64 get_biomass_current()  const { return biomass_current; }  // Originally we did not have any loss (I.e. from decomposition)         //070801
      public: // Processing
         virtual void die()                                                  =0; //050823
         virtual void die_back_to_GAI(float64 target_GAI)                    =0; //130626
         CORN::Days inc_days_dead()                               modification_;  //060911
            // returns true if the portion reached senescence today and just died . 060316
         inline CORN::Days  get_days_dead()           const {return days_dead; } //060824
      public:
         void increment_by(const Portion &addend)                 modification_;
         virtual float64 dec_biomass_current(float64 subtrahend_biomass);        //060911
         virtual float64 dec_living(float64 subtrahend_area_index)           =0; //060530
            // Decrements the area_index by the subtrahend value.
            // Returns the resulting area index.
         virtual float64  slough()                                renunciation_; //060824
            // This invalidates the canopy portion the biomass amount is relinquished to the caller.
            // Returns the biomass sloughed
         virtual bool  dormancy();                                               //060825
            // In senesced_biomass_shed mode,
            // at dormancy GAI_vital is transferred to GAI_effete
            // This is to prevent any GAI from the previous season to influence new growth
      }; // _________________________________________________Portion
   public: // Canopy_accumulation structors
      Canopy_accumulation
         (Abscission abscission_                                                 //110808
         ,bool       from_previous_season_
         ,CORN::Days linger_time_);                                              //080910
      inline virtual ~Canopy_accumulation()                                   {} //151204
   protected: // Canopy_accumulation members
      bool                    from_previous_season;                              //060911
      Abscission abscission;                                                     //110808
// moved to derived class /*_______*/       public : mutable Canopy_accumulation::Portion  *accum_today;     // Accumulated values todate.
   private:
      mutable float64 sloughed_biomass;
      CORN::Days linger_time;                                                    //080910
   public: // Canopy_accumulation methods
      virtual bool invalidate()                                 modification_=0;
      virtual bool update()                                      rectification_;
      virtual bool process_day()                                  modification_; //180111 was process()
      virtual Portion *get_accum_today()                           provision_=0;
      virtual bool is_valid()                                    affirmation_=0;
      virtual float64 remove_biomass                                             //040623
            (float64 living_remaining_available //080806 GAI or fCCg GAI_remaining_available
            ,float64 biomass_to_remove
            ,float64 retain_vital_living       // biomass won't be removed if it takes the GAI or fCCg below this level 060323_
            ,bool remove_newest_first
            ,bool reserve_todays_history )                        modification_;
      float64 relinquish_sloughed_biomass()                       modification_; //080808
      virtual float64 get_biomass_produced()                              const; //080808
      virtual float64 get_biomass_current()                               const;
      virtual float64 get_living()                                        const; //080808
   };//_Canopy_accumulation____________________________________________________/
 private: // Canopy_growth_portioned members
   CropSyst::Crop_nitrogen_interface  *nitrogen;                                 //151006
      // may be 0 if not running nitrogen)
   Residues_interface                 *residues;  // may be 0 if no residue submodel
 public: // Canopy_growth_portioned members
       // This is the initial green (area index or fraction_cover). It does not age or die.
       // We must keep it as long as the plant is alive.
       // After initial growth, the GAI value increases to the regrowth_GAI value 071126
 public: // Canopy_growth_portioned members
   virtual nat32 vital_becomes_effete(bool apply_dormancy)        modification_; //180718
   virtual bool end_season(bool apply_dormancy)                   modification_; //080401
 public: // structors
   Canopy_growth_portioned
      (const CropSyst::Crop_parameters    &crop_parameters
      ,Residues_interface                 *residues                              //060816
      ,CropSyst::Crop_nitrogen_interface  *nitrogen);                            //060816
   inline virtual void know_residues(Residues_interface *residues_)
                                                       { residues = residues_; } //060531
   virtual bool invalidate()                                      modification_;
   virtual bool update()                                         rectification_;
   virtual bool process_day()                                     modification_; //180111_080910
   inline virtual bool start()                     modification_ { return true;} //080811
   virtual void die_back(float64 fraction_to_die)                 modification_;
   virtual void die_back_to_GAI(float64 target_GAI)               modification_; //130624
   virtual float64 damage_GAI(float64 GAI_damage)                 modification_; //151226
      // Returns the remaining GAI                                               //160119
   virtual float64 remove_biomass
      (float64 biomass_to_remove
      ,float64 retain_living /*retain_GAI */       // biomass won't be removed if it takes the GAI or fCCg below this level. Use 0.0 for no limitation
      ,bool remove_newest_first
      ,bool reserve_todays_portion
      ,Crop_biomass &removed_biomass // Return about of biomass removed          //070412
      )                                                           modification_; //070412
   virtual float64 remove_all_biomass                                            //070627
      (Crop_biomass &removed_biomass)                             modification_; //070627
      // This removes all canopy biomass from the plant (I.e. in the case of termination) //070627
      // Returnsabout of biomass removed                                         //070627
   virtual void terminate()                                     modification_=0; //080728
   virtual modifiable_ Canopy_accumulation &mod_canopy_vital()               =0; //060911
   virtual modifiable_ Canopy_accumulation &mod_canopy_effete()              =0; //060911
   virtual unmodifiable_ Canopy_accumulation &ref_canopy_vital()        const=0; //060911
   virtual unmodifiable_ Canopy_accumulation &ref_canopy_effete()       const=0; //060911
   virtual float64 get_live_green_biomass(bool incl_vital,bool incl_effete)const;
   virtual float64 get_biomass_current(nat8 include_biomass)              const; //060601
   virtual float64 get_biomass_produced(nat8 include_biomass)             const;
      // All the biomass ever produced
      // (Warning, this is not including removed biomass!!!
      // probably should rename this to get_biomass_vital_effette)               //060601
   virtual float64 get_senescence_reduction_factor()                    const=0; //080807

//NYN/*080818_*/    virtual Canopy_accumulation::Portion &provide_untouchable_portion() = 0;
   virtual float64 increment_untouchable_portion
      (Canopy_growth_portioned::Canopy_accumulation::Portion &by_portion)    =0; //080818
   virtual bool take_untouchable_portion
      (Canopy_growth_portioned::Canopy_accumulation::Portion *untouchable_portion) modification_=0; //080818
   virtual modifiable_ Canopy_growth_portioned::Canopy_accumulation::Portion *
      mod_untouchable_portion()                                         const=0; //190816
   virtual unmodifiable_ Canopy_growth_portioned::Canopy_accumulation::Portion *
      ref_untouchable_portion()                                         const=0; //190816
 protected:
   virtual bool is_valid()                                              const;
   virtual float64 get_living_index(nat8 include_vital_effete)          const;   //080806
      // This is used by die_back for LAI based model returns GAI canopy cover returns fraction canopy cover
   bool take_portion
      (Canopy_growth_portioned::Canopy_accumulation::Portion  *todays_portion
      ,bool initial_restart_vital,bool protect_it);
   virtual float64 get_regrowth_living_parameter()                      const=0; //080807
      // In the case of canopy_growth_LAI_based() this is the regrowth_GAI
      // in the case of canopy_growth_cover_base() this is currently simply the initial fraction of canopy cover
};
//_Canopy_growth_portioned_____________________________________________________/
}//_namespace CropSyst_________________________________________________________/
#endif

