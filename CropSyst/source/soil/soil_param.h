#ifndef soil_paramH
#define soil_paramH
#if (CS_VERSION == 4)
#  include "CropSyst/source/soil/soil_param_V4.h"
#else
#  include "CropSyst/source/soil/soil_param_V5.h"
#endif
// NYI
#include "common/soil/hydrology/pedotransfer_saxton_1986.h"
#include "common/soil/txtrhydr.h"
//______________________________________________________________________________
class Smart_soil_parameters : public Soil_parameters
{
 public:
    Soil_texture_hydraulics  *layer_texture_hydraulics[MAX_soil_horizons_alloc];
    contribute_ bool texture_available_in_layer[MAX_soil_horizons_alloc];        //111204
      // texture_available_in_layer added when composing from STATSGO2
      // There are some cases where a layer may be missing so we
      // assume that layer will be the same/similar an adjacent layer and copy
      // the parameters from that layer.
 public:
   Smart_soil_parameters();
   virtual ~Smart_soil_parameters();                                             //980722
   virtual bool set_start();                                                     //161023_110218
   virtual bool get_end();                                                       //161023_010315
   virtual bool delete_horizon(nat8 horizon); // This is intended for the parameter editor  //041214
   virtual bool insert_horizon(nat8 horizon); // This is intended for the parameter editor  //041214
 private:
   void delete_layer_texture_hydraulics();                                       //020402
 public:                                                                         //150411
   void setup_texture_hydraulics_layer(nat8 layer);                              //170904
   void setup_layer_texture_hydraulics();                                        //020402
 public:
   virtual void fix_bulk_density         (nat8 horizon,bool fix)  contribution_;
   virtual void fix_perm_wilt_point      (nat8 horizon,bool fix)  contribution_;
   virtual void fix_field_capacity       (nat8 horizon,bool fix)  contribution_;
   virtual void fix_water_pot_at_FC      (nat8 horizon,bool fix)  contribution_; //130326
   virtual void fix_sat_hydraul_cond_m_d (nat8 horizon,bool fix)  contribution_;
   virtual void fix_saturation_WC        (nat8 horizon,bool fix)  contribution_; //080313
   virtual float64 get_horizon_bulk_density_g_cm3  (nat8 horizon)         const; // g/cm3 same as tonne/m3
   virtual float64 get_horizon_sat_hydraul_cond_m_d(nat8 horizon)         const; // m/day ? //980528p
   virtual float64 get_horizon_saturation_WC       (nat8 horizon)         const; //080313
   virtual float64 get_horizon_air_entry_pot       (nat8 horizon)         const; //990106
   virtual float64 get_horizon_Campbell_b          (nat8 horizon)         const; //980528
   virtual float64 get_profile_depth()                                    const; //981217

   virtual float64 get_water_holding_capacity(float64 to_depth_m)         const; // depth in meter //981217
   virtual float64 get_water_holding_capacity()                           const; // of whole profile //981217
   virtual float64 get_organic_matter(float64 to_depth_m)                 const; //111208
      // This is only available when organic matter (historical?)
      // has been stored (I.e. loaded from STATSGO
   void get_avg_water_content(soil_horizon_array32(avg_WC))               const; //020504
 public:
   virtual void copy_horizon(nat8 from_horizon,nat8 to_horizon);
   bool is_agricultural(bool include_dryland,bool include_irrigated)      const; //080729
   modifiable_ Soil_texture_hydraulics  *mod_texture_hydraulics(nat8 layer)      //150411
      { return layer_texture_hydraulics[layer];}
};
//_Smart_soil_parameters____________________________________________1998-10-30_/

#endif
//soilfile.h

