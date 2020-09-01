#ifndef soil_IH
#define soil_IH

/* 031111
This class provides an interface for working with soil objects
for various crop models (CropSyst)

This class presumes a layering system has been established for the
soil profile as implemented in the derived class.

For example in CropSyst there are data available both for both
a soil horizon profile and a profile system subdividing the
soil horizons, CropSyst uses the soil sublayers.

*/

#ifndef primitiveH
#  include "corn/primitive.h"
#endif
#include "corn/const.h"
#include "soil/layering.h"
#include "corn/chronometry/time_types.hpp"
#include "soil/layers_I.h"

#if ((CS_VERSION >= 5) || (REACCH_VERSION >= 2) || defined(OFOOT_VERSION))
#  include "CS_suite/simulation/CS_simulation_element.h"
#endif
#include "CS_suite/observation/CS_observation.h"

namespace CORN
{
   class Enumeration_list;
}
using namespace CORN;

#define ENTIRE_PROFILE_DEPTH 0.0
//______________________________________________________________________________
namespace Soil                                                                   //181206
{
   class Structure;                                                              //181206
   class Texture_interface;                                                      //181206
   class Temperature_profile;                                                    //181206
   class Salt;                                                                   //181206
   class Nitrogen;                                                               //181206
   class Layers_interface;                                                       //181206
   class Hydraulic_properties_interface;                                         //181206
   class Hydrology_interface;                                                    //181206
   class Salt_interface;                                                         //181206
}//_namespace_Soil_____________________________________________________________/
class Soil_temperature_profile_hourly_interface;
class Soil_erosion_RUSLE;
namespace Soil {
class Evaporator;                                                               //160606
//______________________________________________________________________________
interface_ Properties_sublayers_interface
{
 public:
   virtual unmodifiable_ Soil::Layers_interface *ref_layers()           const=0; //160412
   virtual   modifiable_ Soil::Layers_interface *mod_layers()   modification_=0; //160412
      // Returns the soil layering system.
      // This is required in derived classes.
      // It should never return 0.
   virtual unmodifiable_ Soil::Texture_interface *ref_texture()         const=0; //160412
   virtual   modifiable_ Soil::Texture_interface *mod_texture() modification_=0; //160412
      // Returns the soil texture parameters object.
      // 160412 was get_texture_modifiable
   virtual unmodifiable_ Hydraulic_properties_interface
      *ref_hydraulic_properties()                                       const=0; //160412
      // Returns the soil properties that are commonly derived from soil survey data
      // and/or pedo transfer functions.
   virtual   modifiable_ Hydraulic_properties_interface
      *mod_hydraulic_properties()                               modification_=0; //160412
      // Returns the soil properties that are commonly derived from soil survey data
      // and/or pedo transfer functions.
};
//_Properties_sublayers_interface___________________________________2010-11-04_/
interface_ Soil_interface  // rename this to I.e. Components or Coordinator
: public virtual Properties_sublayers_interface                                  //181206_101104
#if (CROPSYST_VERSION == 5)
, public implements_ CS::Simulation_element                                      //150724
#endif
{
 public:  // Status
   inline virtual float64 get_albedo()                     const { return 0.1; }
      // Returns the current albedo of the soil surface.
      // You may return a designated constant or a user specified soil parameter.
      // Often this value is adjusted for soil moisture
      // This default value is for a slightly wet typical soil.
   inline virtual float64 get_SLPF()                       const { return 1.0; }
      // This is used by the CropGro submodel.
      // If never using the CropGro model, simply return 1.0.
      // SLPF is both a soil and a crop specific parameter.
      // It reflects the combined effects on crop growth of various soil-related
      // parameters which are not modeled directly, such as aluminum toxicity or nematode damage.
      // This sounds like an adjustment parameter that should go in simulation control
 public:
   inline virtual bool take_erosion_submodel(Soil_erosion_RUSLE *erosion_submodel_)    //130328_111108
      { UNUSED_arg(erosion_submodel_); return false; }
   virtual unmodifiable_ Structure *ref_structure()                     const=0;
      //160412 was get_structure_unmodifiable
   virtual   modifiable_ Structure *mod_structure()             modification_=0;
      // Returns the soil structure (bulk density)
      //160412 was get_structure_modifiable
   virtual unmodifiable_ Hydrology_interface *ref_hydrology()           const=0; //160412
      // Returns the current soil hydrology state and process object.
      // Generally this is required in derived classes.
      // Generally derived classes will return the object.
      // 160412 was get_hydrology_constant
   virtual modifiable_ Hydrology_interface *mod_hydrology()     modification_=0;
      // Returns the current soil hydrology state and process object.
      // Generally this is required in derived classes.
      // Generally derived classes will return the object.
      // 160412 was get_hydrology_modifiable
   inline virtual unmodifiable_ Hydrology_interface *ref_hydrology_at(nat16 seconds) const
      { UNUSED_arg(seconds); return 0; }
      // Returns the soil hydrology state at the specified number of seconds into the day.
      // For example, if the hydrology model is running at an hourly time step.
      // passing second 0 to 3600 will return the hydrology of the first hour.
      // If the model is running at a daily time step any value 0 to 86399 will return
      // the daily hydrology.
   virtual const       Soil::Evaporator *ref_evaporator()               const=0; //160606
   virtual modifiable_ Soil::Evaporator *mod_evaporator()       modification_=0; //160606
   virtual bool submit_evaporator(Soil::Evaporator *evaporator_)  submission_=0; //160606
   inline virtual modifiable_ Nitrogen *mod_nitrogen()            modification_  //160412
      { return 0; }
      // This returns the soil nitrogen object that can be modified.
      // This is not required in derived classes.
      // Derived classes may return 0 indicating there is no nitrogen submodel
      // or the nitrogen simulation is not enabled.
      // 160412 was get_nitrogen_modifiable
   inline virtual unmodifiable_ Nitrogen *ref_nitrogen()                   const //160412
      { return 0; }
      // This returns the soil nitrogen object not intended for modification.
      // This is not required in derived classes.
      // Derived classes may return 0 indicating there is no nitrogen submodel
      // or the nitrogen simulation is not enabled.
      // 160412 was get_nitrogen_constant
   inline virtual unmodifiable_ Soil_temperature_profile_hourly_interface
      *provide_temperature_profile_hourly()                           provision_  //150424_161104LML added the first unmodifiable
      { return 0; }
      // Returns the hourly soil temperature profile object.
      // May return 0 if this soil does not provide a hourly soil temperature profile
      // or if hourly simulation not enabled.

   inline virtual unmodifiable_ Soil_temperature_profile_hourly_interface
      *ref_temperature_profile_hourly() const
      { return provide_temperature_profile_hourly(); }                           //150424
      // Returns the hourly soil temperature profile object.
      // May return 0 if this soil does not provide a hourly soil temperature profile
      // or if hourly simulation not enabled.

   inline virtual unmodifiable_ Temperature_profile *ref_temperature_profile() const//141212
      { return 0; }
   inline virtual unmodifiable_ Salt_interface *ref_salinity()             const //160412
      // not const because caller can do anything with the salinity
      { return 0; }
      // Returns the soil salinity object.
      // May return 0 if this soil does not provide a soil salinity submodel
      // or salinity simulation is not enabled.
      // 160412 was get_salinity_constant
   inline virtual modifiable_ Salt_interface *mod_salinity() modification_ //160412
      { return 0; }
      // Returns the soil salinity object.
      // May return 0 if this soil does not provide a soil salinity submodel
      // or salinity simulation is not enabled.
      // 160412 was get_salinity_modifiable
   inline virtual   modifiable_ bool set_salinity
      (/*Soil::*/Salt *salinity,bool keep_it = true)               modification_
      {  UNUSED_arg(salinity); UNUSED_arg(keep_it); return 0; }
      // Returns true if salinity was successfully set up.
      // returns false if could not be set
      // Pass 0 to clear the salinity submodel.
      // If keep_it is true then ownership of the salinity object
      // is passed to the soil object and it will be responsible for deleting it.
      // Note: if you relinquish ownership, then it is possible that
      // the salinity object is deleted at any time by the soil object;
      // so from this point on always use get_salinity().

      // Derived soil class may provide the salinity model by default
      // In this case override set_salinity to simply return false;

   inline virtual float64 get_pH(nat8 layer) const {UNUSED_arg(layer); return 7.0;} //150415
 public: // Soil profile state accessors  (May eventually move to a Soil_temperature_interface
   virtual float64 get_temperature_C                   (nat8 layer)     const=0;
 public: // Soil plant parameters  // Not sure where to put this.
   inline virtual float64 get_root_hospitality_factor(nat8 layer) const { UNUSED_arg(layer); return 1.0;}
      // This is used by the CropGro submodel.
      // If never using the CropGro model, simply return 1.0.
 public:
   virtual float64  get_act_water_entering_soil()                       const=0;
   virtual modifiable_ float64 &mod_act_water_entering_soil()   modification_=0; //160412_150930
 public:
   #if (CROPSYST_VERSION == 5)
   // This can actually be all versions
   #else
   virtual bool start_day()                                     modification_=0;
         // should return bool;
   virtual bool end_day()                            modification_{return true;} //130911
   virtual bool process_day()                                   modification_=0; //150611
   #endif
   virtual Seconds   get_hydrology_timestep_resolution_today()          const=0;
      // Returns the number of seconds in the hydrology timestep
      // Usually this is the timestep of the infiltration model that was used today.
      // The timestep may be variable.
      // I.e. in CropSyst the finite difference could run at daily, hourly or half hour
      // interval on any given day as deemed necessary to handle the water flux.
      // There should be a corresponding soil profile hydrology record for each
      // timestep retrieved with get_hydrology_at().
   virtual bool reinitialize_for_crop(float64 wilt_leaf_water_pot) modification_
      {  UNUSED_arg(wilt_leaf_water_pot); return true;}
      // 140926, I am not sure if reinitialize_for_crop is still needed, check with Claudio
      // If it is then probably would be start_season()

 public: // The following are functions (valid for interfaces)
   virtual float64 get_mass_kg_m2(nat8 layer)                             const;
   virtual float64 distribute_amount
      (const soil_horizon_array32(from_horizon_amount)
      ,soil_sublayer_array_64(to_layer_amount))                         const=0; //150424
   RENDER_INSPECTORS_DECLARATION_PURE;                                           //150929
};
//_Properties_sublayers_interface______________________________________________/
}//_namespace_Soil_____________________________________________________________/
#endif

