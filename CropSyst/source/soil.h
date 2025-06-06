#ifndef soilH
#define soilH

#include "options.h"
#include "soil/soil_base.h"
#include "soil/soil_N_param.h"
#ifdef SOIL_INFILTRATION
#include "soil/infiltration_A.h"                                                 //150219LML
#endif
#include "weather/weather_types.h"
#include "CS_suite/observation/CS_observation.h"
#include "CropSyst/source/soil/temperature_profile.h"                            //160801LML
#ifndef CAMO_VERSION
#ifndef VIC_CROPSYST_VERSION
#define PROVIDES_SOIL_TEMPERATURE_PROFILE
#endif
#endif

class Cropsyst_model;
class       Soils_temperature_profile_hourly_interface;

namespace RUSLE { class Soil_erosion; }
class Soil_tillage_effect;
class CropSyst_UED_database;
class Water_table_curve;
namespace Soil
{
   class       Disturbance;
   interface_  Salt_interface;
   class       Abiotic_environment_profile;
   class       Temperature_profile_hourly;
   class       Temperature_profile_hourly_interface;
   class       Chemicals_profile;
}
namespace CropSyst {
//namespace Soil
//{
//______________________________________________________________________________
class Soil_components
: public Soil_base
{
  friend class Cropsyst_model; // For output access to the variables only.
 public: // public so report generator has easy access
   ::Soil::Chemicals_profile     *chemicals;          // optional  (eventually split N rename this to nitrogen);   //110808
   RUSLE::Soil_erosion         *erosion;            // optional  owned by this. 0 if no erosion submodel
   Soil_tillage_effect        *tillage_effect;     // optional owned by this.  0 if no tillage submodel, or no tillage events have occured //060929
   #ifdef PROVIDES_SOIL_TEMPERATURE_PROFILE
   const CS::Land_unit_meteorological &meteorological;                           //151022
   #endif
   const CS::Annual_temperature        &annual_temperature;                      //150424
 public: // setup and initialization
   Soil_components
      (const Smart_soil_parameters        *parameters_                           //160916
      ,bool parameters_given                                                     //160916
      ,const CORN::date32                 &simdate_                              //170524
      ,const CS::Annual_temperature       &annual_temperature                    //150424
      #ifdef PROVIDES_SOIL_TEMPERATURE_PROFILE
      ,const CS::Land_unit_meteorological &meteorological                        //151022
      #endif
      );
   virtual ~Soil_components();                                                              //020620
   ::Soil::Chemicals_profile *provide_chemicals();                                  //051120
   virtual const ::Soil::Nitrogen *ref_nitrogen()                         const; //160412_060922
   virtual ::Soil::Nitrogen *mod_nitrogen();                       modification_ //160412_060922
       // returning chemicals is only temporary, eventually will separate nitrogen and salinity
   #ifdef NITROGEN
   bool enable_nitrogen                                                          //020620
      (bool                              enable_denitrification_with_CO2         //060508
      ,Infiltration_model                infiltration_model                      //041116
      ,::Soil::Abiotic_environment_profile *soil_abiotic_environment_profile        //060510
         // may be 0 if not available
      #if (CROPSYST_VERSION==4)
      ,float64                           max_transform_depth
      ,float64                           adjust_denitrify
      ,float64                           adjust_nitrify_V4_1                     //060627
      #endif
      ,const CropSyst::Denitrification_parameters &denitrification_parameters    //150824
      ,soil_horizon_array32              (N_NO3)
      ,soil_horizon_array32              (N_NH4)
      ,Water_table_curve                *NO3_water_table_curve                   //080213
         // may be 0 if no water table consideration
      ,Water_table_curve                *NH4_water_table_curve);                 //080213
         // may be 0 if no water table consideration
   #endif
   #ifdef PHOSPHORUS
   float64 know_phosphorus_in_profile(float32 initial_P_profile);
   #endif
   virtual inline bool take_erosion_submodel(RUSLE::Soil_erosion *_erosion_submodel) appropriation_
      {  erosion = _erosion_submodel; return erosion!= 0; }                      //060712
   virtual inline RUSLE::Soil_erosion *reliquish_erosion_submodel() renunciation_ //140609
      {  RUSLE::Soil_erosion *reliquished_erosion = erosion;
         erosion = 0;
         return reliquished_erosion;
      }
   #if (CROPSYST_VERSION==4)
   virtual bool setup_structure
      (CORN::Data_record &data_rec,bool for_write);
   #endif
   // This is a high level function because it covers a lot of variables
 public: //
   bool preinitialize                                                            //150724_980801
      (Infiltration_model         infiltration_model   // default CASCADE_MODEL  //041116
      ,::Soil::Infiltration_nominal *infiltration_submodel                          //060206
      ,::Soil::Hydrology_abstract   *hydrology);                                    //060512

   virtual bool start_day()                                       modification_; //051206
   virtual bool end_day()                                         modification_; //130911
   virtual bool process_day()                                     modification_; //150611_051205
   virtual bool reinitialize
      (Moisture_characterization moisture                                        //170320
      ,soil_horizon_array32(H2O)                                                 //140311
         // not const because initialized to avgWC when 0 values
      ,bool beginning_of_simulation
      ,bool subdivide_horizons);                                                 //060229
   #ifdef CHEM_PROFILE
   virtual bool reinitialize_for_crop
      (float64 wilt_leaf_water_pot);
   #endif
   void remove_horizon_2();                                                      //990318
#ifdef EROSION
   void erode();                                                                 //131001
#endif
   bool check_automatic_irrigation                                               //960603
      (float64 max_allowable_depletion    // 0-1
      ,float64 depletion_observe_depth);  // m
   float64 determine_recharge_requirements
      (float64 irr_refill_paw
      ,float64 root_length
      ,float32 refill_point)                                              const;
   virtual float64 get_act_evaporation()                                  const; //041027
 public: // Accessor functions
   virtual float64 get_surface_temperature()                              const;
   const ::Soil::Disturbance *get_disturbance()                           const; //060807
      // May return 0 if no disturbance
   virtual float64 get_temperature(nat8 sublayer)                         const;
 public: // Soil profile state accessors for CropGro
   virtual float64 get_temperature_C(nat8  sublayer)                      const;
   inline virtual float64 get_SLPF()                                      const {return parameters_ownable->SLPF_CropGro; }
 public:   // for hourly soil temperature
#ifdef SOIL_TEMPERATURE
 public: // Soil temperature
   mutable ::Soil::Temperature_profile *temperature_profile;// optional // must be declared before freezing profile and chemical profile
      // Mutabled because we may added this at any time as needed
   provided_ ::Soil::Temperature_profile_hourly   *temperature_profile_hourly;
   // Optional but required for organic matter and nitrification models
   // For CROPSYST_PROPER owned by this
   // For MICRO_VERSION owned by clsSoil temperature_profile_hourly.             //150810RLN
   virtual float64 DeVries_thermal_conductivity    //  J / (sec m C) DeVries     //990120
      (float64 water_content_sl
      ,float64 ice_content_sl
      ,float64 bulk_density_sl)                                           const;
   ::Soil::Temperature_profile *relinquish_temperature_profile();                //140610
   ::Soil::Temperature_profile_hourly *relinquish_temperature_profile_hourly();  //140610
   virtual ::Soil::Temperature_interface  *provide_temperature_profile()  const; //141212
   virtual ::Soil_temperature_profile_hourly_interface *provide_temperature_profile_hourly() provision_;
#else
   virtual const Soil::Temperature_interface *provide_temperature_profile() const=0;//141212 160922LML added const
#endif

#ifdef SALINITY
   virtual bool set_salinity(::Soil::Salt_interface *salinity_dSm,bool keep_it = true);//051120
   virtual const ::Soil::Salt_interface *ref_salinity()                   const; //160412_051120
   virtual ::Soil::Salt_interface *mod_salinity();                               //160412_051120
      // not const because caller can do anything with the salinity
   virtual float64 get_salt_stress_function
      (nat8  sublayer,float64 osmotic_pot_50,float64 salt_tolerance_P )   const;
   virtual float64 get_osmotic_pot_solution(nat8 sublayer)                const;
#endif
   RENDER_INSPECTORS_DECLARATION;                                                //150916
};
//_Soil_components__________________________________________________1998-07-29_/
}//_namespace CropSyst_________________________________________________________/
#endif


