#ifndef canopy_growth_LAI_basedH
#define canopy_growth_LAI_basedH
#include "crop/canopy_growth_portioned.h"
#include "crop/crop_param.h"

#if (CROPSYST_VERSION >= 5)
#define  CropSyst_NS CropSyst::
#else
#define  CropSyst_NS  
#endif

//______________________________________________________________________________
namespace CropSyst {
class Crop_CO2_response;
//______________________________________________________________________________
class Canopy_growth_leaf_area_index_based
: public Canopy_growth_portioned
{  //190612 This should be renamed Canopy_leaf_growth_leaf_area_index_based
 protected: // Classes
   //_________________________________________________________________
   class Canopy_accumulation
   : public CropSyst_NS Canopy_growth_portioned::Canopy_accumulation
   {public:
      class Portion
      : public CropSyst_NS Canopy_growth_portioned::Canopy_accumulation::Portion
      {protected:
         float64  age;        // In degree days
         float64  GAI;        // Area Index vigorous new growth of this season   //060911
         float64  dead_AI;    // Area Index             //970305
         float64  GAI_related_biomass; // kg/m2 GAI related canopy biomass.  It is used for GAI/biomass development calculation.  // was  GAI_related_canopy_biomass   980226P
         // GAI related biomass biomass:
         // Used to calculate GAI in plant development in that
         // during times of stress leaves dont expand
         // although there is still a gain biomass
      public:
         Portion
            (float64 age
            ,float64 GAI  // Initially area_index is always green area index
            ,float64 biomass
            ,float64 GAI_biomass);  // This is the GAI related canopy biomass. It is used for development calculation //980226
         Portion();
         inline virtual ~Portion()                                            {} //120217
      // This constructor is used for accumulators
      public: // overrides
        virtual int compare(const CORN::Item &other)                      const; //970701
        inline virtual const char *label_strign(std::string &buffer)      const
                                      { buffer.clear(); return buffer.c_str(); } //030123
                                      // Never printed.
      public: // Accessors
                virtual bool    is_valid()                                const;                                                                                                  //050823
         inline virtual bool    is_dead()                                 const  { return dead_AI > 0.000001; }                                                                   //050823
         inline virtual float64 get_age()                                 const  { return age; }                                                                                          //981207
         inline virtual float64 get_GAI()                                 const  { return GAI; }                                                                                          //050823
         virtual float64 get_living()                                     const  { return GAI; } // Gets the living green portion GAI or fract_canopy_cover_green                                    //080806

         inline virtual float64 get_LAI()                                 const  { return GAI + dead_AI ; }                                                                               //060911
         inline virtual float64 get_biomass_current()                     const  { return biomass_current; }  // Originally we did not have any loss (I.e. from decomposition)            //070801
         inline virtual float64 get_biomass_produced()                    const  { return biomass_produced; }                                                                             //060601
         inline virtual float64 get_GAI_related_biomass()                 const  { return GAI_related_biomass; }                                                                          //981207
      public: // Processing
         virtual void die() ;                                                    //050823
            // obsolete: returns the amount of GAI cleared
         virtual void die_back_to_GAI(float64 target_GAI) ;                      //130626
         bool age_by(float64 daily_age_deg_day,float64 leaf_duration_deg_day);   //050823
            // returns true if the portion reached senescence today and just died.//060316
      public:
         void increment_by(const Portion &addend)                 modification_;
         virtual float64  slough()                                renunciation_; //060824
            // This invalidates the canopy portion the biomass amount is relinquished to the caller.
            // Returns the biomass sloughed
         virtual bool  dormancy();                                               //060825
            // In senesced_biomass_shed mode,
            // at dormancy GAI_vital is transferred to GAI_effete
            // This is to prevent any GAI from the previous season to influence new growth
      public: // specific to this class
         void clear_GAI_related_biomass()                         modification_; //080515
         float64 dec_living(float64 subtrahend_area_index)        modification_; //060530
            // Decrements the area_index by the subtrahend value.
            // Returns the resulting area index.
      public:
         float64 reduce_green_fraction_canopy_cover                              //080805
            (float64 canopy_cover_to_reduce_fraction)             modification_;
         virtual bool write(std::ostream &strm)                    performs_IO_; //100121
      };
      //_class_Portion_________________________________________________________/
   private:
      public : mutable Portion  *accum_today;     // Accumulated values todate.
   public: // Canopy_accumulation members
      inline Canopy_accumulation
         (Abscission abscission_                                                 //110808
         ,bool          from_previous_season_
         ,CORN::Days    linger_time_)                                            //080910
         : CropSyst_NS Canopy_growth_portioned::Canopy_accumulation
            (abscission_                                                         //110808
            ,from_previous_season_,linger_time_)
         , accum_today(0)
         {}
      virtual ~Canopy_accumulation();                                            //080808
   public:
      float64 get_GAI()                                          rectification_;
      float64 get_LAI()                                                mutates_; //060911
      inline virtual bool is_valid()                                      const
         { return accum_today != 0; }
      virtual bool invalidate()                                    modification_;
      inline virtual CropSyst_NS Canopy_growth_portioned::Canopy_accumulation::Portion
         *get_accum_today()                  provision_ { return  accum_today; }
      virtual bool update()                                      rectification_;
   private:
      virtual Portion  *provide_accum_today()                        provision_;
   };
   //_class_Canopy_accumulation________________________________________________/
 protected:
   mutable Canopy_accumulation canopy_vital;                                     //060911
   mutable Canopy_accumulation canopy_effete;                                    //060911
   const Phenology::Period_thermal *senescence;                                          //190630
 protected:
   const CropSyst_NS Crop_parameters_struct::Canopy_growth_LAI_based &parameters;
 private:
    inline virtual float64 get_regrowth_living_parameter()                 const //08080
      { return parameters.regrowth_GAI;}
      // In the case of canopy_growth_LAI_based() this is
      // in the case of canopy_growth_cover_base() this is currently simply the initial fraction of canopy cover
 private:   // Some of these will be move to common
   mutable bool GAI_has_died_off;    nat32 resets;                               //080515
 public:  // eventually private?
   mutable float64  peak_LAI;
   #ifndef __GNUC__
   mutable
   #endif
   float64 &reported_peak_LAI;  // reference to crop which uses this only for output //080725
 private:
   float64  LAI_for_self_shading_response;  // This is not the LAI reported in output or used for canopy cover //060725
   CropSyst::Crop_CO2_response   *CO2_response;                                  //020409
      // will be 0 if not simulating CO2
   #if (CO2_RESPONSE_VERSION==4)
   // actually this did not appear to be used anymore
   float64 leaf_area_growth_related_biomass_at_baseline_conc;                    //110913
   #endif
   Canopy_accumulation::Portion *untouchable_portion;                            //080818
 public: // Canopy cover fraction
   mutable float64 cover_total_fract     ;  // OUTPUT fraction of canopy cover.    May eventually rename to yesterday_fract_canopy_solrad_interception
   mutable float64 cover_green_fract      ;  // May eventually rename to yesterday_fract_green_canopy_solrad_interception
   mutable float64 orchard_shaded_area_GAI;                                      //081020
      // mutable so we can store for output this will eventually be moved to the orchard submodel
   mutable float64 canopy_porosity;                                              //081020
      // mutable so we can store for output this will eventually be moved to the orchard submodel
 public: // structors
   Canopy_growth_leaf_area_index_based
      (const CropSyst_NS Crop_parameters   &crop_parameters_      // Will be owned by this crop object
      ,const CropSyst_NS Crop_parameters_struct::Canopy_growth_LAI_based   &parameters_
      ,Residues_interface                  *residues_                            //060816
      ,CropSyst_NS Crop_nitrogen_interface *nitrogen_                            //060816
      ,float64 &_reported_peak_LAI                                               //080725
      ,Crop_CO2_response                   *CO2_response_);                      //080728
   virtual ~Canopy_growth_leaf_area_index_based();
   inline virtual bool initialize()                modification_ { return true;} // NYN
 public: // Canopy_growth_portioned actualized methods
   inline virtual modifiable_    CropSyst_NS Canopy_growth_portioned::Canopy_accumulation &mod_canopy_vital() { return canopy_vital; }             //060911
   inline virtual modifiable_    CropSyst_NS Canopy_growth_portioned::Canopy_accumulation &mod_canopy_effete(){ return canopy_effete; }            //060911
   inline virtual unmodifiable_  CropSyst_NS Canopy_growth_portioned::Canopy_accumulation &ref_canopy_vital()  const { return canopy_vital; }   //090312
   inline virtual unmodifiable_  CropSyst_NS Canopy_growth_portioned::Canopy_accumulation &ref_canopy_effete() const { return canopy_effete; } //090312
 public:
   virtual bool update()                                         rectification_;
   virtual void terminate()                                       modification_; //080728
   virtual bool emerge_cotyledon
      (float64 new_cotalydon_GAI
      ,float64 new_cotalydon_BM)                                  modification_; //080728
   virtual float64 remove_biomass
      (float64 biomass_to_remove
      ,float64 retain_GAI        // biomass won't be removed if it takes the GAI below this level. Use 0.0 for no limitation
      ,bool remove_newest_first
      ,bool reserve_todays_portion
      ,Crop_biomass &removed_biomass_returned)                    modification_; //070412
   virtual float64 update_LAI_for_self_shading_response
      (bool after_accescence
      ,bool before_maturity)
      modification_;
   virtual bool start()                                           modification_; //080811
   virtual bool restart_with
      (float64 restart_biomass,float64 restart_GAI
      ,bool use_clumping_factor = true)                           modification_;
      //190703 actually now would be start_with

   virtual bool respond_to_clipping()                             modification_; //110824_080515
   virtual float64 get_influence_on_root_activity_factor
      (bool after_end_of_vegetative_growth)                               const; //080805
   virtual float64 get_GAI(uint8 include_GAI)                             const;
   virtual float64 get_LAI()                                              const; //060824
   virtual float64 get_LAI(bool from_canopy)                              const; //080728
   inline virtual float64 get_peak_LAI()               const { return peak_LAI;} //080728
   inline virtual float64 get_reported_peak_LAI()const{return reported_peak_LAI;}//141203
   virtual bool age_leaf                                                         //070328
      (float64  thermal_time_growing_degree_day
      ,float64 water_stress_index);
   virtual bool develop_leaf
     (bool continue_leaf_growth
     ,float64 canopy_growth
     ,float64 LAI_related_growth  // Today's LAI related growth
     ,float64 thermal_time_growing_degree_day);                                  //080728
   virtual float64 get_LAI_to_LAImax_fract()                              const; //080728
      // get_LAI_to_LAImax_fract is used is Crop_CropSyst for calculating plant height
 public:
   virtual float64 get_senescence_reduction_factor()                      const; //080807
   virtual float64 calc_adjusted_ET_crop_coefficient()                    const; //080811
      //200117  was get_...
 private:
   virtual void update_fraction_of_canopy_cover()                rectification_;
   float64 clumping(float64 xAI,float64 area_factort)                     const; //000623
 public:
   virtual float64 calc_cover_fract                                              //000623
      (float64 xAI
      ,bool photosynthetic_active_rad_based
      ,bool use_clumping_factor = true                                           //080801
      ,bool unused_this_model= false )                                    const; //081104
   float64 get_interception_PAR_green()                                   const; //110901
   float64 calc_interception_global_green()                               const; //110904
   //200228 float64 get_interception_global_green()                                const; //200228
   inline virtual modifiable_ float64 &reference_fract_canopy_cover() modification_ { return cover_total_fract;} //080801
   virtual bool end_day()                                         modification_; //080801
   //200228 inline virtual float64 get_fract_canopy_cover()                         const
   inline virtual float64 get_interception_global_total()                 const
                                                    { return cover_total_fract;} // eventually rename to get_fract__canopy_cover_total
   inline virtual float64 get_interception_global_green()                  const;
   //200228                                                    { return cover_green_fract; }
   virtual float64 calc_plant_development                                        //990215
      (float64 length_max
      ,float64 last_length
      ,float64 overall_growth_stress)                                     const; //080805
   virtual float64 increment_untouchable_portion(CropSyst_NS Canopy_growth_portioned::Canopy_accumulation::Portion &by_portion) modification_;//080818
   virtual bool take_untouchable_portion(CropSyst_NS Canopy_growth_portioned::Canopy_accumulation::Portion *untouchable_portion) modification_;//080818
   inline virtual modifiable_               Canopy_growth_portioned::Canopy_accumulation::Portion *mod_untouchable_portion() const {return (CropSyst_NS Canopy_growth_portioned::Canopy_accumulation::Portion *)untouchable_portion; } //190816
   inline virtual unmodifiable_             Canopy_growth_portioned::Canopy_accumulation::Portion *ref_untouchable_portion() const {return (CropSyst_NS Canopy_growth_portioned::Canopy_accumulation::Portion *)untouchable_portion; } //190816
   inline virtual float64 know_LWP_yesterday(float64 LWP_yesterday)   cognition_ //110901
         { return 0; } // not used in this model
   inline virtual bool know_senescence (const Phenology::Period_thermal *senescence_)    //190630
      modification_ { senescence = senescence_; return senescence != NULL;}
   inline virtual bool know_maturity_initiation()                 modification_  //200409
      { return true;}                                                            //200409
      // know_maturity_initiation not used by this model;

 private:
   float64 get_GAI_related_biomass()                                      const;
   virtual void age_by(float64 daily_age_deg_day,float64 leaf_duration_deg_day);
 public:  void dump_ages(std::ofstream &tostream);                               //070705
};
//_class Canopy_growth_leaf_area_index_based___________________________________/
}//_namespace CropSyst_________________________________________________________/
#endif



