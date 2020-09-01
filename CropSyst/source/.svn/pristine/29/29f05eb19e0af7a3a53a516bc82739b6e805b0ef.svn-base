#ifndef canopy_cover_continuumH
#define canopy_cover_continuumH
#include <ostream>
#include <assert.h>
#include "crop/crop_param_struct.h"
#include "crop/canopy_cover_curve.h"
#include "crop/phenology_I.h"

#define Ref_to_const_float         const float64 *

namespace CropSyst
{
   class Canopy_growth_cover_based;
//______________________________________________________________________________
class Canopy_cover_abstract
{protected:  // parameters these may need to be renamed to correspond with CropSyst crop parameters
   const Crop_parameters_struct::Canopy_growth_cover_based &canopy_parameter;
   const Crop_parameters_struct::Morphology  &canopy_growth_common_parameter;
   const Phenology::Period_thermal *accrescence;
   const Phenology::Period_thermal *culminescence;                               //181109
   const Phenology::Period_thermal *senescence;                                  //181109
   nat32 maturity_lapse;                                                         //200409
   //200409 const CS::Period *maturityX;                                                  //181109

 protected: // references to other objects
   bool  is_fruit_tree;
 protected: // state maintained by this
   float64 interception_global_green;                    //fraction              //110906
      // was solar_rad_intercepted_fraction
   float64 interception_global_green_yesterday;          //fraction              //110906
      // was solar_rad_intercepted_fraction_yesterday
   float64 interception_PAR_green;                       //fraction
   //200126 float64 cover_latest_vegetative;                                     //171221
         //  cover_latest_vegetative is now obsolete (undone)
 public:
   Canopy_cover_abstract
      (const Crop_parameters_struct::Canopy_growth_cover_based &canopy_parameter_
      ,const Crop_parameters_struct::Morphology &canopy_growth_common_parameter_
      ,const Phenology  &phenology_                                              //110824
      ,bool             is_fruit_tree_);                                         //120510
 public:
   virtual bool update_cover(float64 LWP_yesterday)             modification_=0;
   inline virtual float64 get_interception_PAR_green()                     const
                                     { return interception_PAR_green; }
   inline Ref_to_const_float ref_interception_insolation_global_green()    const
                                          { return &interception_global_green; }
   inline float64 &mod_interception_insolation_global_green()      modification_
                                           { return interception_global_green; } //110824
   inline float64 get_interception_global_green()                          const
      { return interception_global_green; }
      // rename this to ref_interception_insolation_global_green_canopy
   float64 set_interception_insolation_global_green
      (float64 interception_insolation_global_green)              modification_; //130520
   virtual float64 adjust_interception_insolation_global_total                   //190611
         //200228  rename to adjust_interception_insolation_global_total (or rather  adjust_interception_global_total
      (float64 adjustment)                         modification_ { return 0.0; }
   float64 adjust_interception_insolation_global_green                           //190405
      (float64 adjustment)                                        modification_;
      // These adjustments are used in the case where the canopy has been clipped.
      // It is necessary to reduce the interception_insolation_global_green_canopy
      // for both today and yesterday according to the removal
      // So adjustment would always be between 0 and 1.
   virtual bool start_dormancy()                                  modification_; //110406
   inline virtual bool start_season()                             modification_;
   virtual bool know_accrescence  (const Phenology::Period_thermal *accrescence_);       //181109
   virtual bool know_culminescence(const Phenology::Period_thermal *culminescence_);     //181109
   virtual bool know_senescence   (const Phenology::Period_thermal *senescence_);
   inline virtual bool know_N_leaf_stress_factor(float64 N_leaf_stress_factor_) { return false; }       //200528

   inline virtual bool know_maturity_initiation()                 modification_  //200409
      {maturity_lapse = 1; return true;}                                                  //200409
   virtual bool respond_to_clipping()                             modification_; //110421
   virtual bool start_day()                          modification_{return true;} //110601
   virtual bool end_day()                                         modification_; //190821
   void dump_headers(std::ostream &strm,const char *prefix)               const;
   virtual void dump(std::ostream &strm)                                  const;
};
//_class_Canopy_cover_abstract_________________________________________________/
class Canopy_cover_reference
: public Canopy_cover_abstract
{protected:
   // In this case:
   //    solar_rad_intercepted_fraction            is PotentialFractionSolarRadiationIntercepted
   //    PAR_intercepted_fraction                        is PotentialFractionPARIntercepted
   //    solar_rad_intercepted_fraction_yesterday  is PotentialFractionSolarRadiationIntercepted(DAP - 1)
   #define unstressed_interception_insolation_global_green            interception_global_green
   #define unstressed_interception_insolation_global_green_yesterday  interception_global_green_yesterday
 public:
   Canopy_cover_curve *canopy_cover_curve;                                       //171220
 protected:
   float64 canopy_senescence_rate;
 public:
   float64 unstressed_green_fract; // cover fraction
 public:
   float64 expansion_potential_today;                                            //110601
 public:
   Canopy_cover_reference
      (const Crop_parameters_struct::Canopy_growth_cover_based &canopy_parameter_
      ,const Crop_parameters_struct::Morphology &canopy_growth_common_parameter_
      ,const Phenology                          &phenology_                      //110824
      ,bool                                      is_fruit_tree_);                //120510_101030
   virtual bool update_cover
      (float64 leaf_water_potential_yesterday_NA_this_version =0) modification_;
 private:
   float64 calculate()                                                    const;
 public:
   bool initialize()                                            initialization_; //131206_110628
      // value of 1.0 disables CO2 response
   virtual bool start_day()                                       modification_; //110601
   virtual bool end_day()                                         modification_; //190821
   virtual bool start_season();
   float64 set_interception_insolation_global_green_at_senescence
      (float64 interception_insolation_global_green)              modification_; //200206
   inline virtual bool know_accrescence(const Phenology::Period_thermal *accrescence_)
      {  accrescence    = accrescence_;
         culminescence  = 0;
         senescence     = 0;
         return canopy_cover_curve->know_accrescence(accrescence_); }            //181231
   inline virtual bool know_culminescence(const Phenology::Period_thermal *culminescence_)
      {  culminescence  = culminescence_;
         accrescence    = 0;
         senescence     = 0;
         return canopy_cover_curve->know_culminescence(culminescence_); }        //181231
   inline virtual bool know_senescence(const Phenology::Period_thermal *senescence_)
      {  senescence     = senescence_;
         accrescence    = 0;
         culminescence  = 0;
         return canopy_cover_curve->know_senescence(senescence_); }              //181118
   virtual bool start_dormancy()                                  modification_; //110901
   void dump_headers(std::ostream &strm)                                  const;
   virtual void dump(std::ostream &strm)                                  const;
};
//_class_Canopy_cover_reference________________________________________________/
class Canopy_cover_actual
: public Canopy_cover_abstract
{
 protected:
   // In this case:
   //    solar_rad_intercepted_fraction   is FractionSolarRadInterceptedTotalCanopy
   //    interception_PAR_green_canopy               is FractionPARIntercepted
//   const Transpiration_actual &transpiration_actual;
 protected: // state maintained by this.
   float64 global_solar_rad_intercepted_total;                                   //110906 // was FractionSolarRadInterceptedTotalCanopy
 protected: // References to other objects;
   Canopy_cover_reference &canopy_cover_reference;
   const float64 &water_stress_index_yesterday;                                  //110902
   float64 N_leaf_stress_factor;                                                 //200528
 protected: friend class Canopy_growth_cover_based;
   bool first;
 public:
   float64 cover_attained_max;                                                   //110825 was maximum_canopy_cover_reached
   float64 cover_to_lose_total;
 public:
   Canopy_cover_actual
      (const Crop_parameters_struct::Canopy_growth_cover_based &canopy_parameter
      ,const Crop_parameters_struct::Morphology  &canopy_growth_common_parameter
      ,const Phenology                       &phenology                          //110824
         //In VIC, this should be Phenology_actual
      ,bool                                   is_fruit_tree                      //110422
      ,const float64                         &water_stress_index_yesterday       //110902
      , Canopy_cover_reference               &canopy_cover_reference);
   virtual bool end_day()                                         modification_; //190821
   virtual bool start_season();
   virtual bool know_senescence(const Phenology::Period_thermal *senescence_);   //200125
   virtual bool update_cover(float64 LWP_yesterday)               modification_;
   virtual float64 adjust_interception_insolation_global_total                   //190611
      (float64 adjustment)                                        modification_;
   virtual bool start_dormancy()                                  modification_; //110406
   virtual bool respond_to_clipping()                             modification_;
   inline float64 get_interception_global_green_yesterday()               const
                                 { return interception_global_green_yesterday; }
   inline float64 get_interception_global_total()                         const
                                  { return global_solar_rad_intercepted_total; }
   float64 calc_leaf_area_index()                                         const;
   float64 calc_green_area_index()                                        const; //110901
   float64 calc_peak_leaf_area_index()                                    const; //110901
   void dump_headers(std::ostream &strm)                                  const;
   virtual void dump(std::ostream &strm)                                  const;
   virtual bool know_N_leaf_stress_factor(float64 N_leaf_stress_factor_);        //200528

 private:
   float64 calc_XXXX_area_index(float64 fract_intercept)                  const; //110901
};
//_class_Canopy_cover_actual___________________________________________________/
}//_namespace CropSyst_________________________________________________________/
#endif

