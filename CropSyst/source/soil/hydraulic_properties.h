#ifndef hydraulic_propertiesH
#define hydraulic_propertiesH
#include "soil/hydrology_I.h"
#include "corn/math/statistical/stats.h"
#include "soil/layering.h"

class Soil_parameters_class_common;
namespace Soil {                                                                 //181206
class Layers_interface;                                                          //181206
//______________________________________________________________________________
class Hydraulic_properties                                                       //051201
: public Hydraulic_properties_interface                                          //051201
//181206 : public Soil_hydraulic_properties_interface                            //051201
{  // These are soil properties that are taken from soil parameters by horizon and distributed to sublayers(layers)
   // Generally they are constant, but can change over time or temporarily based on immediate conditions or events.
   // (I.e. freezing, tillage operations, erosion)
   const Layers_interface &layers; // Provides convenient access to soil layering
protected:
   soil_layer_array64(saturation_water_content_volumetric);                      //051205
   soil_layer_array64(field_capacity_volumetric);                                //051204
   soil_layer_array64(permanent_wilt_point_volumetric);                          //051204
   soil_layer_array64(saturated_hydraulic_conductivity_kg_s_m3);  // kg�s/m�     //051204
   // CropSyst uses two Campbell b values: one is user specified,
   // the second is calculated from FC and PWP
   // When the b value is not known, it will be calculated when the soil is initialized
   soil_layer_array64(Campbell_b_parameter);                                     //051204
   soil_layer_array64(air_entry_pot_parameter);    // J/kg                       //990321
   soil_layer_array64(Campbell_b_FC_PWP_based);    // Used for Cascade model     //051204
   soil_layer_array64(air_entry_pot_FC_PWP_based); // J/kg                       //990321P
   soil_layer_array64(bypass_coef);                                              //070117
   soil_layer_array64(water_pot_at_FC);                                          //130228
protected: // computed not save to file.
   mutable soil_layer_array64(unsaturated_conductivity_function_slope);          //080118
   mutable bool unsaturated_conductivity_function_slope_valid;                   //080118
public: // constructors
   Hydraulic_properties(const Layers_interface &_layers);
   Hydraulic_properties  // copy constructor
      (const Hydraulic_properties //181206 Soil_hydraulic_properties
         &copy_from
      ,const Layers_interface    &_layers);
   Hydraulic_properties                                                          //150926
      (const Layers_interface &_layers
      ,const Soil_parameters_class_common &_parameters);
   inline virtual ~Hydraulic_properties()                                     {} //170217
   void clear();
   void copy(const Hydraulic_properties &copy_from);
   virtual void expand_last_layer_for_boundary_conditions
      (nat8 number_layers,nat8 number_expanded_layers)            modification_; //080130
public: // public get accessors
   virtual float64 get_saturation_water_content_volumetric                       //051204
      (nat8 layer,float64 current_ice_water_content)                      const; //140822
   virtual float64 get_field_capacity_volumetric            (nat8 layer)  const; //051204
   virtual float64 get_permanent_wilt_point_volumetric      (nat8 layer)  const;
   inline virtual float64 get_lower_limit_volumetric        (nat8 layer)  const
      { return get_permanent_wilt_point_volumetric(layer); };
   inline virtual float64 get_drained_upper_limit_volumetric(nat8 layer)  const
      { return get_field_capacity_volumetric(layer); };
   // Note: Currently just returning field capacity but it is actually based on triangular distribution?
   virtual float64 get_sat_hydraul_cond_kg_s_m3             (nat8 layer)  const;  // rename to get_saturated_hydraulaulic_conductivity_kg_s_m3
   virtual float64 get_unsaturated_conductivity_function_slope(nat8 layer)const;//080118
   // Note that this air entry potential is from the soil parameters
   // In CropSyst this is adjusted for ice content.
   virtual float64 get_bypass_coef                          (nat8 layer)  const;//070117
   virtual float64 get_water_pot_at_FC                      (nat8 layer)  const;//130228
   virtual const float64 *ref_soil_water_pot_at_FC          ()            const;//170501
   virtual float64 get_Campbell_b          (nat8 layer,bool FC_PWP_based) const;//990321
   virtual float64 get_air_entry_potential (nat8 layer,bool FC_PWP_based) const;//990321
   /* Claudio decided not to use this
   virtual float64 get_macro_pore_factor                    (nat8 layer)   const //140304
      { return 1.0; }
      // NYI (this may actually be bypass coef
      // if not then need to add to parameter file).
   */
public :
   virtual float64 calc_saturation_water_content_volumetric (float64 _bulk_density_g_cm3)                                        const;
public: // set accessors (used for initialization)
   virtual float64 change_bulk_density_g_cm3                (nat8 layer,float64 _bulk_density_g_cm3);          //070725
   virtual float64 set_field_capacity_volumetric            (nat8 layer,float64 _field_capacity_volumetric);
   virtual float64 set_permanent_wilt_point_volumetric      (nat8 layer,float64 _perm_wilt_point_volumetric);
   virtual float64 set_water_pot_at_FC                      (nat8 layer,float64 _water_pot_at_FC);             //130228

   virtual float64 set_saturation_water_content_volumetric     (nat8 layer,float64 _saturation_volumetric);    //051204
   virtual float64 set_saturated_hydraulic_conductivity_kg_s_m3(nat8 layer,float64 _sat_hydraul_cond_kg_s_m3);
   virtual float64 set_saturated_hydraulic_conductivity_m_d    (nat8 layer,float64 _sat_hydraul_cond_m_day);   //150926
   virtual float64 set_Campbell_b_parameter                    (nat8 layer,float64 _Campbell_b);
   virtual float64 set_air_entry_potential_parameter           (nat8 layer,float64 _air_entry_pot);
   virtual float64 set_Campbell_b_FC_PWP_based                 (nat8 layer,float64 _Campbell_b);
   virtual float64 set_air_entry_potential_FC_PWP_based        (nat8 layer,float64 _air_entry_pot);
   virtual float64 set_bypass_coef                             (nat8 layer,float64 _bypass_coef);              //070117
};
//______________________________________________________________________________
} // namespace Soil
#define J_per_kg_to_m(J_per_kg) (J_per_kg * 0.102)
//______________________________________________________________________________
#endif

