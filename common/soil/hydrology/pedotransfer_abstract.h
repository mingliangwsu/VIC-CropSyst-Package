#ifndef pedotransfer_abstractH
#define pedotransfer_abstractH
#include "corn/quality.hpp"
using namespace CORN;
#define fixed_quality measured_quality
#define normal_particle_density_g_cm3 2.65
//______________________________________________________________________________
class Pedotransfer_functions_abstract
{
 public:   // probably actually protected
   CORN::Quality_clad texture_quality;
   CORN::Quality_clad saturation_quality;
   CORN::Quality_clad vol_WC_at_1500_quality;
   CORN::Quality_clad vol_WC_at_FC_quality;                                      //140416
   CORN::Quality_clad sat_hydraul_cond_m_d_quality;
   CORN::Quality_clad bulk_density_quality;
/*140416 Claudio decided that B and AEP are always to be calculated
 and would never (rarely be a known value
   CORN::Quality_clad soil_b_quality;
   CORN::Quality_clad air_entry_pot_quality;
*/
   CORN::Quality_clad FC_quality;   // <- not currently used, but needed by           //020509
   CORN::Quality_clad PWP_quality;  // <- the SoilProfile float edit                  //020509
   CORN::Quality_clad porosity_quality;                                               //080419
   CORN::Quality_clad water_pot_at_FC_quality;
   // add porosity_status
 protected:
   float32 &sand;
   float32 &clay;
   float32 &silt;
 protected:
   CORN::Quality_clad GM_particle_size_quality;        float32 geometric_mean_particle_size;
 protected: // Communicated with external application
   float32 &saturation;             // m3/m3 not sure units ???
   float32 &vol_WC_at_1500;         // m3/m3
   float32 &vol_WC_at_FC;           // m3/m3                                     //140406
   /*   float32 &vol_WC_at_33;           // m3/m3
      Now  WP at FC is calculated from pedotransfer functions.
      so not using -33

   */
   float32 &sat_hydraul_cond_m_d;   // m/day
   float32 &bulk_density;           // grams/cm3
   float32 &soil_b;
   float32 &air_entry_pot;          // J/kg
   float32 &porosity;                                                            //080419
   float32 &water_pot_at_FC;                                                     //140416
 protected:
   virtual void  reset_questionable_states();
 public:

   //140416  These get methods should be renamed to provide_XXX and be provisions

   virtual float get_porosity_with_quality(CORN::Quality &quality_got) ; //981030
   virtual float get_porosity();                                                 //981030
   virtual float get_bulk_density(CORN::Quality &quality_returned); // gm/cm3
   virtual float get_saturation(CORN::Quality &quality_returned);
 public: // structors
   Pedotransfer_functions_abstract
   (float32 & _sand, float32 &_clay, float32 &_silt
   ,float32 & _saturation                  ,bool saturation_fixed
   ,float32 & _vol_WC_at_1500              ,bool vol_WC_at_1500_fixed
   ,float32 & _vol_WC_at_FC                ,bool vol_WC_at_FC_fixed
   ,float32 & _sat_hydraul_cond_m_d        ,bool sat_hydraul_cond_m_d_fixed
   ,float32 & _bulk_density                ,bool bulk_density_fixed
   ,float32 & _soil_b
   ,float32 & _air_entry_pot
   ,float32 & _porosity                    ,bool porosity_fixed
   ,float32 &_water_pot_at_FC              ,bool water_pot_at_FC_fixed           //140416
   );
 protected:
   float calc_bulk_density_based_on_best_saturation(CORN::Quality &calced_bulk_density_quality);   //080313
 protected: // Derived classes must override.
   virtual float calc_saturation_based_on_texture(CORN::Quality &returned_quality) = 0;            //981030
   virtual float est_saturation_as_porosity(CORN::Quality &saturation_quality) /*const*/;              //080430
 protected:
   float32 set_porosity             (float32 new_porosity            ,CORN::Quality &new_quality); //080419
   float32 set_bulk_density         (float32 new_bulk_density        ,CORN::Quality &new_quality); //080419
   float32 set_saturation           (float32 new_saturation          ,CORN::Quality &new_quality); //080419
   float32 set_air_entry_pot        (float32 new_air_entry_pot);                 //140416
   float32 set_sat_hydraul_cond_m_d (float32 new_sat_hydraul_cond_m_d,CORN::Quality &new_quality);
   float32 set_vol_WC_at_1500       (float32 new_vol_WC_at_1500        ,CORN::Quality &new_quality); // m3/m3
   float32 set_vol_WC_at_FC         (float32 new_vol_WC_at_FC          ,CORN::Quality &new_quality); // m3/m3

};
//______________________________________________________________________________
#endif
