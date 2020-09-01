#ifndef VIC_crop_interfaceH
#define VIC_crop_interfaceH
#include "corn/const.h"
#include "corn/primitive.h"
//#define Ref_to_const_float         const float64 *
#include "soil/soil_interface.h"
#include "crop/growth_stages.hpp"
#include "crop/phenology_interface.h"
//______________________________________________________________________________
namespace VIC_crop
{
//______________________________________________________________________________
class Crop_interface
{

 public: // methods interfacing with VIC
   virtual bool know_transpiration_max_mm(double transpiration_max_mm) modification_ = 0;

   inline virtual bool is_perennial()                                      const = 0;
   inline virtual float64 update_ET_coef_full_shading()            modification_ = 0;

   /* used only by VIC_crop
   virtual void know
      (const Soil_layers_interface                 *_soil_layers
      ,modifiable_ Soil_hydrology_interface        *_soil_hydrology) = 0;
   */      
   virtual bool reinitialize_CO2_response()                        modification_ =0; //110628
   virtual bool start_day(/*int32 date*/)                          modification_ =0;
   virtual bool process()                                          modification_ =0;
   virtual Normal_crop_event_sequence get_crop_event_sequence()    modification_ =0;//140810
   virtual float64 get
      (nat32 variable_code
      /*NYI ,nat32 units_code*/)                                           const = 0;
   /*140810 obsolete
      // VIC is now handling water uptake so if it still needs
      // to have uptake, then it needs to keep track.
      // CropSyst proper does not keep the uptake by layer.
   virtual float64 get_uptake_mm(nat8 layer)                               const = 0;
   */
   virtual bool process_harvest()                                  modification_ =0;
   virtual float64 harvest_annual(bool terminate)                  modification_ =0;
   virtual float64 harvest_fruit_tree()                            modification_ =0;
   virtual const CropSyst::Phenology_interface &ref_phenology()            const =0;

};
//_2014-05-09_____________________________________________________________________________
} // namespace VIC_crop
#endif
