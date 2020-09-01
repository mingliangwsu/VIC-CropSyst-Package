#ifndef infiltration_AH
#define infiltration_AH

#include "soil/infiltration_I.h"
#include "mgmt_types.h"
#include "corn/chronometry/date_I.h"
#include "corn/dynamic_array/dynamic_array_T.h"
#include "soil/hydrology_I.h"
#include "soil/layers_I.h"
#include "corn/chronometry/time_types.hpp"

class Dynamic_water_entering_soil;
class Soil_interface;
class Soil_wetting;
namespace CropSyst
{  class Soil_runoff;                                                            //130308
   class Pond;                                                                   //141206
}
namespace Physical
{  class Water_depth;                                                            //160412
}
namespace Soil
{                                                                                //181206
class Chemicals_profile;
//______________________________________________________________________________
class Infiltration_nominal
: public Infiltration_interface
{  // This infiltration model does nothing,
   // so neither infiltration nor evaporation
   // It is used when we need to test the model without
 protected: friend class Soil_base; // friendship for recording
   float64   balance_drainage;   // m water depth                                //070118
   float64   reported_drainage;  // m water depth                                //070118
   float64   infiltration;       // m water depth  This is the water draining out of the bottom of the soil profile (usually the same as balance_drainage) 070118_
   float64 act_water_entering_soil;// meter                                      //991204 moved from Soil_base
      // This is the actual infiltration (including runoff, ponding ect..)
 public:  // 'structors and initialization
   inline Infiltration_nominal()                                                 //131001
      : balance_drainage         (0.0)                                           //070118
      , reported_drainage        (0.0)                                           //070118
      , infiltration             (0.0)                                           //070118
      ,act_water_entering_soil   (0.0)                                           //991204
      {}
   virtual Infiltration_nominal *copy(float64 fraction)                    const //140609
      {  UNUSED_arg(fraction); return new Infiltration_nominal();                //160525
      }
   inline virtual bool initialize()                 modification_{ return true;} //131221
   inline virtual bool reinitialize(const soil_horizon_array32(H2O)) modification_ //131221
      { UNUSED_arg(H2O); return true;}
   inline virtual void know_chemicals                                            //070207
      (Soil::Chemicals_profile *chemicals_)  mutation_{ UNUSED_arg(chemicals_);}
#ifdef WATERTABLE
   // The water table is optional and only used by F.D. model
   inline virtual void set_water_table                                           //020620
      (const char *water_table_filename
      ,const CORN::date32 &today                                                 //170525_151128
      ,const CORN::Date_const & first_date
      ,const CORN::Date_const & last_date)                       modification_{}
   inline virtual void water_table_limitations()                 modification_{} //131001
#endif
 public: // Processes
   inline virtual bool infiltrate                                                //990409
      (float64               water_entering_soil_not_irrigation                  //070109
      ,float64               management_irrigation
      ,float64               management_auto_irrigation_concentration
      ,Dynamic_water_entering_soil *non_runoff_entering_in_intervals_mm          //990204
      ,Dynamic_water_entering_soil *irrigation_entering_in_intervals_mm //option //070119
      ,const CORN::Dynamic_array<float32>    &runon_in_intervals                 //990409
      ,CORN::Dynamic_array<float32>          &runoff_in_intervals) modification_
      {  UNUSED_arg(water_entering_soil_not_irrigation);
         UNUSED_arg(management_irrigation);
         UNUSED_arg(management_auto_irrigation_concentration);
         UNUSED_arg(non_runoff_entering_in_intervals_mm);
         UNUSED_arg(irrigation_entering_in_intervals_mm);
         UNUSED_arg(runon_in_intervals); UNUSED_arg(runoff_in_intervals);
         return true;
      }
 public: // Get accessor
   inline virtual float64 get_act_water_entering_soil()                          //150925_060508
      const { return act_water_entering_soil;}
   inline virtual modifiable_ float64 &mod_act_water_entering_soil()modification_//160412_150925_060508
      { return act_water_entering_soil;}
   inline virtual float64 get_water_table_input()             const{return 0.0;}
   inline virtual float64 get_water_table_depth()             const{return 0.0;} //070309
   inline virtual float64 get_balance_drainage()              const{return 0.0;} //070118
   inline virtual float64 get_microirrigation_excess()        const{return 0.0;} //200309
   inline virtual float64 get_reported_drainage()             const{return 0.0;} //070118
   inline virtual float64 get_infiltration()                  const{return 0.0;} //070118
   inline virtual bool is_water_table_applicable()          const{return false;}
   inline virtual float64 get_daily_error()                   const{return 0.0;}
   inline virtual float64 get_deferred()                      const{return 0.0;} //07102
 public: // Set accessors
   inline virtual void set_override_leaching_depth                               //020620
      (float64 leaching_depth_)    modification_ { UNUSED_arg(leaching_depth_);}
   virtual bool know_directed_irrigation
      (float64 wetted_areac_fract)                    cognition_ {return false;} //200226

   RENDER_INSPECTORS_DECLARATION;                                                //150916
 private: // check abstraction
   inline Infiltration_nominal *test_abstraction()
      { return new Infiltration_nominal(); }
};
//_Infiltration_nominal_____________________________________________2006-05-12_/
class Infiltration_abstract
: public extends_ Infiltration_nominal                                           //071012
{
 protected:
   const Layers_interface &soil_layers;                                          //070316
   #ifndef __GNUC__
   mutable
   #endif
      Hydrology_interface  &soil_hydrology;                                      //071012
   const Hydraulic_properties_interface &soil_hydraulic_properties;              //071012
   modifiable_ CropSyst::Pond         &pond;                                     //130308
   CropSyst::Soil_runoff              *runoff;                                   //130308
      // runoff (optional) is owned by land_unit_sim
   mutable Soil::Chemicals_profile     *chemicals;
      // optional may be 0 if not chemical simulation
      // (eventually split N rename this to nitrogen)
      // eventually this should be an interface
      // Actually should ask soil for chemicals, but I haven't finished
      // making an interface for it.
   float32   daily_error;
      // was daily_finite_diff_error
      // This stores any slight error in the integration
   cognate_ float64 wetting_area_surface_fract; // as specified in irrigation operation
 protected:  // Parameters
   bool     override_leaching_depth;                                             //020527
   float64  leaching_depth;                                                      //020527
   Layer    layer_at_reported_drainage_depth;                                    //071013
 public: // 'structors and initialization
   Infiltration_abstract                                                         //131001
      (const Layers_interface &soil_layers_                                      //140424
      ,const Hydraulic_properties_interface &soil_hydraulic_properties_          //140423
      ,Hydrology_interface &soil_hydrology_
      ,CropSyst::Soil_runoff     *_runoff                                        //130308
      ,CropSyst::Pond            &_pond);                                        //130308
   virtual bool initialize()                                    modification_=0;
   virtual bool reinitialize(const soil_horizon_array32(H2O))   modification_=0;
   virtual inline void know_chemicals                                            //070207
      (Soil::Chemicals_profile *_chemicals)                             mutates_
      { chemicals = _chemicals; }
      // Makes known the soil profile chemical
      // and enables the soil chemical transport submodel.
 public: // process
      // Returns true if the infiltration submodel uses
      // water table information.
      // Derived classes should override and return true if the
      // water table is applicable
 public: // Processes
   virtual bool infiltrate
      (float64               water_entering_soil_not_irrigation
      ,float64               management_irrigation
      ,float64               management_auto_irrigation_concentration
      ,Dynamic_water_entering_soil *non_runoff_water_entering_in_intervals_mm    //990204
      ,Dynamic_water_entering_soil *irrigation_water_entering_in_intervals_mm    //070119
      ,const CORN::Dynamic_array<float32> &runon_in_intervals                    //990409
      ,CORN::Dynamic_array<float32> &runoff_in_intervals)         modification_; //990409
      // all derived classes should call this at the beginning of the infiltrate
      // routine to clear accumulators
      // Returns the amount of infiltration (m of water depth?)                  //030711
#ifdef NOT_YET_IMPLEMENTED
   virtual const Soil_hydrology_interface &get_hydrology_at                      //070117
      (Seconds timestep)                                                const=0;
      // Returns the recorded soil water hydrology at the specified timestep (seconds after midnight) of the current date
      // Derived infiltration models must override.
      // if the infiltration model does not resolution of the time step
      // then the nearest timestep recorded should be returned.
      // I.e. if the infiltration model is daily and water_content for 1:00am (timestep=3600) then the daily water_content is simply returned.
#endif
 public: // Get accessor
   virtual inline float64 get_balance_drainage() const{return balance_drainage;} //070118
   virtual inline float64 get_reported_drainage()const{return reported_drainage;}//070118
   virtual inline float64 get_infiltration()       const { return infiltration;} //070118
   virtual float64 get_water_table_input()                              const=0;
   virtual float64 get_water_table_depth()                              const=0;//070309
   virtual inline  float64 get_daily_error()       const { return daily_error; }
 public: // Set accessor
   virtual void set_override_leaching_depth
      (float64 leaching_depth)                                    modification_; //020620
 protected: // was in Soil_infiltration_cascade_hourly                           //071012
   float64 get_field_capacity_ice_corrected(Layer layer)                  const; //070125
   float64 redistribute_water // V.B.  SoilWaterRedistribution                   //070125
      (Layer first_redistribution_layer                                          //070125
      ,Layer last_redistribution_layer                                           //070125
      ,soil_layer_array64(water_flow)                                            //080208
         // works with either layer or node based depending on the infiltration model.
      ,float64 hours_to_field_capacity                                           //080311
      ,CORN::Seconds time_step)                                   modification_;
   inline virtual bool know_directed_irrigation
      (float64 wetting_area_fract_)                                   cognition_ //200226
      { wetting_area_surface_fract = wetting_area_fract_; return true; }         //200317
};
//_Infiltration_common______________________________________________2018-12-06_/
class Infiltration_none
: public Infiltration_nominal
{  // This infiltration model does nothing,
   // so neither infiltration nor evaporation
   // It is used when we need to test the model without
 public:  // 'structors and initialization
   inline Infiltration_none()
      : Infiltration_nominal()
      {}
   inline virtual bool initialize()                  modification_{return true;}
   inline virtual bool reinitialize
      (const soil_horizon_array32(H2O))                            modification_
      {UNUSED_arg(H2O); return true;}
 public: // Processes
   inline virtual bool infiltrate
      (float64               water_entering_soil_not_irrigation                  //070109
      ,float64               management_irrigation
      ,float64               management_auto_irrigation_concentration
      ,Dynamic_water_entering_soil *non_runoff_water_entering_in_intervals_mm    //990204
      ,Dynamic_water_entering_soil *irrigation_water_entering_in_intervals_mm    //070119
             // irrigation_water_entering_in_intervals_mm is optional
      ,const CORN::Dynamic_array<float32>    &runon_in_intervals                 //990409
      ,CORN::Dynamic_array<float32>          &runoff_in_intervals) modification_ //990409
      {  UNUSED_arg(water_entering_soil_not_irrigation);
         UNUSED_arg(management_irrigation);
         UNUSED_arg(management_auto_irrigation_concentration);
         UNUSED_arg(non_runoff_water_entering_in_intervals_mm);
         UNUSED_arg(irrigation_water_entering_in_intervals_mm);
         UNUSED_arg(runon_in_intervals); UNUSED_arg(runoff_in_intervals);
         return 0.0; }
 public: // Get accessor
   inline virtual float64 get_water_table_input()          const { return 0.0; }
   inline virtual float64 get_water_table_depth()          const { return 0.0; } //070309
};
//_class:Soil_infiltration_none_____________________________________2006-05-12_/
}//_namespace_Soil______________________________________________________________
#endif

