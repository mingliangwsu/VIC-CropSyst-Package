#ifndef yield_responseH
#define yield_responseH
#include "corn/const.h"
#include "corn/dynamic_array/dynamic_array_T.h"
#include "corn/math/statistical/sample_dataset_T.h"
#include "corn/math/statistical/sample_cumulative_T.h"
#include "common/weather/parameter/WP_air_temperature.h"

/*
Water and heat stress during flowering
Only applicable to grain crops
*/
namespace CropSyst
{
//______________________________________________________________________________
class Observations_yield
{public:
   const float64 &observation_ref;
   nat8  wing_size;
   CORN::Dynamic_array<float32>  before;
   CORN::Dynamic_array<float32>  during;
   CORN::Dynamic_array<float32>  after;
   CORN::Dynamic_array<float32> *current;  // dont delete
   //CORN::statistical::Sample_cumulative<float32,float32> *stats;
   CORN::statistical::Sample_dataset_clad<float64,float32,float32> *stats;
 private:
   bool advance_flowering;
   bool advance_filling;
 public:
   inline Observations_yield
      (const float64 &observation_ref_
      ,nat8  wing_size_=3)
      : observation_ref                                       (observation_ref_)
      , wing_size                                                   (wing_size_)
      , before                                                                ()
      , during                                                                ()
      , after                                                                 ()
      , current                                                        (&before)
      , stats                                                                (0)
      , advance_flowering                                                (false)
      , advance_filling                                                  (false)
      {}
   inline virtual ~Observations_yield()
      {  delete stats;
      }
   virtual bool is_acheived()                                      affirmation_;
   virtual bool is_satisfied()                                     affirmation_;
   virtual bool know_flowering()                                               ;
   virtual bool know_filling()                                                 ;
   virtual bool end_day()                                         modification_;
   virtual bool commit()                                          modification_;
};
//______________________________________________________________________________
class Yield_response
{
 public:
   Yield_response()                                                           {}
   virtual bool know_flowering()                                             =0;
   virtual bool know_filling()                                               =0;
   virtual bool end_day()                                         modification_;
   virtual bool is_satisfied()                                   affirmation_=0;
   virtual bool is_acheived()                                    affirmation_=0;
   virtual bool commit()                                        modification_=0;
   virtual float64 get_factor()                                         const=0;
};
//__________________________________________________________________2020-04-22_/
#define BEFORE_WING -1
#define DURING_WING  0
#define AFTER_WING  +1
class Yield_grain_water_response
: public extends_ Yield_response
{
   float64 stress_sensitivity;
   Observations_yield stress_obs;
 public:
   inline Yield_grain_water_response
      (float64 stress_sensitivity_
      ,const float64 &water_stress_ref_
      , nat8 wing_size_=3)
      : Yield_response                                                        ()
      , stress_sensitivity                                 (stress_sensitivity_)
      , stress_obs                                (water_stress_ref_,wing_size_)
      {}
   virtual bool know_flowering()                                               ;
   virtual bool know_filling()                                                 ;
   virtual bool end_day()                                         modification_;
   virtual bool is_satisfied()                                     affirmation_;
   virtual bool is_acheived()                                      affirmation_;
   virtual float64 get_factor()                                           const;
 protected:
   virtual bool commit()                                          modification_;
};
//__________________________________________________________________2020-04-22_/
class  Yield_grain_temperature_response
: public extends_ Yield_response
{
   float64 stress_sensitivity;
   float64 temperature_optimum;  // Celcius   (parameter)
   float64 temperature_damaging; // Celcius
   Observations_yield Tmax_obs;
   Observations_yield Tmin_obs;
      // The stats is not used by this model.
   float64 factor;
 public:
   Yield_grain_temperature_response
      (float64 stress_sensitivity_
      ,float64 temperature_optimum_
      ,float64 temperature_damaging_
      ,const Air_temperature_maximum &air_temperature_max_
      ,const Air_temperature_minimum &air_temperature_min_
      ,nat8 wing_size_=3);
 public:
   virtual bool end_day()                                         modification_;
   virtual bool know_flowering()                                               ;
   virtual bool know_filling()                                                 ;
   virtual bool is_satisfied()                                     affirmation_;
   virtual bool is_acheived()                                      affirmation_;
   virtual float64 get_factor()                                           const;
 protected:
   virtual bool commit()                                          modification_;
};
//__________________________________________________________________2020-04-22_/
}//_namespace CropSyst_________________________________________________________/
#endif

