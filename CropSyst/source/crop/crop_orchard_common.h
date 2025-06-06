#ifndef crop_orchard_commonH
#define crop_orchard_commonH
//---------------------------------------------------------------------------
#ifndef primitiveH
#  include "corn/primitive.h"
#endif
#include "options.h"
#include "model_options.h"
#include "crop/crop_param.h"
#include "crop/canopy_growth_cover_based.h"
#include "crop/thermal_time.h"
//______________________________________________________________________________
class Sun;
class Sun_days;
class Sun_hours;
class Weather_hours;
//______________________________________________________________________________
namespace CropSyst {
class Crop_CO2_response;                                                         //130406
//______________________________________________________________________________
class Orchard_interface
{
 public:
   interface_ Interception_interface
   {
      virtual float64 update_solid_canopy_interception()       rectification_=0;
      virtual float64 get_solid_canopy_interception()                   const=0;
   };
   //_class_Interception_interface_____________________________________________/
   virtual void expand_canopy(float64 deg_days_today)           modification_=0; //000718
   virtual float64 update_solid_canopy_interception()          rectification_=0; //151027

   virtual float64 get_solid_canopy_interception()                      const=0;
   virtual float64 get_canopy_porosity()                                const=0; //081103
   virtual float64 get_canopy_shaded_area_GAI()                         const=0; //081103
   virtual  void set_deg_days_begin_canopy_development                           //071121
      (float64 deg_days_begin_canopy_development)               modification_=0;
   virtual bool know_canopy_leaf
      (const Canopy_leaf_growth *canopy_leaf_growth)                         =0; //190625
   virtual bool drop_leaves()                                                =0; //190625
//_Orchard_interface________________________________________________2012-09-09_/
};
//_Orchard_interface___________________________________________________________/
class Orchard_common
: public Orchard_interface
{
 protected: // crop parameters
   class Tree_canopy_extent
   {
      float64 deg_days_begin_canopy_development;
      float64 deg_days_end_vegetative_growth;
      class Dimension
      {
       private:  // parameters
         float64 initial;
         float64 final;
         float64 &deg_days_begin_canopy_development;
         float64 &deg_days_end_vegetative_growth;
       public:  // state   Currently public, but should make accessors
         mutable float64 current;
       public:
         Dimension
            ( float64 initial_size
            , float64 final_size
            , float64 &deg_days_begin_canopy_development
            , float64 &deg_days_end_vegetative_growth);
         float64 expand(float64 deg_days_today)                   modification_;
         inline float64 get()                            const {return current;}
      }; //________________________________________class Dimension_/
    public: // Currently public, but should make accessors
      Dimension width;
      Dimension breadth;
      Dimension height;
    public: // constructor
      Tree_canopy_extent(CropSyst::Crop_parameters_struct
         ::Canopy_architecture &architecture
         //200403 ::Canopy_characteristics_hedge_row &canopy_characteristics
         , float64 deg_days_begin_canopy_development
         , float64 deg_days_end_vegetative_growth);
      virtual void set_deg_days_begin_canopy_development
         (float64 deg_days_begin_canopy_development)               assignation_; //071121
    public:
      void expand(float64 deg_days_today)                         modification_;
   };//_Tree_canopy_extent_____________________________________________________/
 public:
   class Interception_architectural
   : public Orchard_interface::Interception_interface
   {
    protected:
      float64 slope_radians;        // slope of soil surface
      float64 slope_aspect_from_south_radians; // aspect of the slope terrain  0 is south increasing to west north and east
      float64 row_azimuth_radians;  // 0 is south with west positive (converted from crop layout parameters)
    protected: // references
      const float64     &fract_cloud_cover;
      const CORN::Date_const &today;
    protected:
      CropSyst::Crop_parameters_struct::Layout              &layout;             //110218
      CropSyst::Crop_parameters_struct::Canopy_architecture &architecture;       //200403_110218
      //200304 CropSyst::Crop_parameters_struct::Canopy_characteristics_hedge_row  &canopy_characteristics; //110218
    protected: // aliases
      Tree_canopy_extent *tree_canopy;
    protected:
      const Sun_days   &sun_days;         // Provided by weather/meteorology
    public:  // constructor
      Interception_architectural
         (CropSyst::Crop_parameters_struct::Layout &layout                       //110218
         ,CropSyst::Crop_parameters_struct::Canopy_architecture                  //200403
            &canopy_architecture                                                 //110218
         /*200403
         ,CropSyst::Crop_parameters_struct::Canopy_characteristics_hedge_row
            &canopy_characteristics                                              //110218
         */
         ,float64 slope_percent
         ,float64 slope_aspect_N0_E90_degrees
         ,const Sun_days   &sun_days         // Provided by weather/meteorology
         ,const float64    &fract_cloud_cover
         ,const CORN::Date_const &today);
    private:
      float64 convert_aspect_from_0_north_with_east_90_to_0_south_with_east_90
         (float64 aspect_N0_E90_degrees)                                   const;
    public:
      virtual float64 update_solid_canopy_interception()       rectification_=0;
      inline virtual void know_tree_canopy(Tree_canopy_extent *tree_canopy_)
         modification_ {tree_canopy = tree_canopy_; }
   };
   //_Interception_____________________________________________________________/
 public:

   class Canopy_architectural                                                    //190613
   {
    protected:
      const Canopy_leaf_growth *canopy_leaf_growth;
      float64 light_extinction_coef; //191106 kc;
    public: // The following could be local but are stored for output
      mutable float64 shaded_area_GAI; // daily
      mutable float64 porosity;
      mutable float64 solid_canopy_interception;
    public: //construtor
      Canopy_architectural(float64 light_extinction_coef_);                      //190613
    public: // Canopy_growth_leaf_area_index_based overrides
      virtual float64 calc_cover_fract                                           //081022
         (float64 xAI    // Either LAI or  GAI
         ,bool photosynthetic_active_rad_based
         ,bool use_clumping_factor = false
         ,bool record_shading_porosity_interception = true)               const; //081104
      inline virtual float64 get_shaded_area_GAI()const{return shaded_area_GAI;} //081103
      inline virtual float64 get_porosity()       const { return porosity;}      //081103
      inline float64 know_solid_canopy_interception
         (float64 solid_canopy_interception_)                         cognition_
         { return solid_canopy_interception = solid_canopy_interception_; }      //081103
      inline virtual float64 know_LWP_yesterday(float64 LWP_yesterday) cognition_ //110901
         { UNUSED_arg(LWP_yesterday); return 0; } // not used in this model
      inline virtual bool know_canopy_leaf
         (const Canopy_leaf_growth *canopy_leaf_growth_)                         //190625
         { canopy_leaf_growth = canopy_leaf_growth_; return true; }
      inline virtual bool drop_leaves()
      {  canopy_leaf_growth = 0;
         return true;
      }
   };
   //_class Canopy_architectural____________________________________2008-11-03_/
 private:
  float64 light_extinction_coef; //191106 kc;
   Interception_architectural    *interception; // owned by this provided by main model
   Tree_canopy_extent            *tree_canopy;                                   //071117
      // tree_canopy is only applicable of architectural interception
   mutable Canopy_architectural  *canopy_architectural;   // (optional)

   //180614 mutable Canopy_growth *canopy;
   // conceptual, user should be able to select model
   //   mutable Canopy_leaf_growth *canopy;                                      //180614_081103
 public:
   Orchard_common
      (float64 kc_
      ,Interception_architectural *interception
      ,Crop_parameters_struct::Canopy_architecture &canopy_architecture          //200403_110218
      ,float64 deg_days_flowering
      ,float64 deg_days_begin_rapid_fruit_growth);
   virtual void expand_canopy(float64 deg_days_today);                           //000718
   virtual float64 update_solid_canopy_interception()            rectification_; //151027
   virtual float64 get_solid_canopy_interception()                        const;
   virtual void set_deg_days_begin_canopy_development
      (float64 deg_days_begin_canopy_development)                  assignation_; //071121
   virtual float64 get_canopy_porosity()                                  const; //081103
   virtual float64 get_canopy_shaded_area_GAI()                           const; //081103
   virtual bool know_canopy_leaf
      (const Canopy_leaf_growth *canopy_leaf_growth)                           ; //190625
   virtual bool drop_leaves()                                                  ; //190625
   #ifdef NYN
   virtual bool process_day()                                     modification_;
   #endif
};
//_Orchard_common______________________________________________________________/
}//_namespace CropSyst_________________________________________________________/
#endif

