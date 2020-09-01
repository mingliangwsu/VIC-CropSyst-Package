#ifndef soil_provider_commonH
#define soil_provider_commonH
#include "CropSyst/source/soil/soil_param.h"
class Smart_soil_parameters;
class Geocoordinate; // eventually move to CS namespace 150904
namespace CS {
class Soil_database;
//______________________________________________________________________________
class Component_composition
: public implements_ CORN::Item
{  // There may be multiple components in a map unit (polygon)
 public:
   Smart_soil_parameters soil_params;
   float32 profile_depth_m;
   nat8 &comppct;        // for debugging
   inline Component_composition(nat8 &_comppct)
      : CORN::Item()
      , profile_depth_m(0)  // to be calculated
      , comppct(_comppct)
      {}
};
//_Component_composition__________________________________________________
class Mapunit_composition
: public implements_ CORN::Item
{public:
   CORN::Bidirectional_list components;  //list of Component_composition
   Component_composition *predominate_component_composition;
 public:
   inline Mapunit_composition()                                                  //141230
      : CORN::Item()
      , predominate_component_composition(0)
      {}
   inline virtual const Smart_soil_parameters &get_soil_parameters()       const
      {
assert(predominate_component_composition);                                       //190222
         return predominate_component_composition->soil_params ;}
   inline const Component_composition *get_predominate_component_composition()
      { return predominate_component_composition; }
   //190929 inline virtual bool commit()                                { return false; } //150927
};
//_Mapunit_composition___________________________________________________/
class Soil_provider
: public implements_ CORN::Item
{
 public:
   /*190927 obsolete
   virtual Mapunit_composition *render_selected_map_unit
      (const std::string &MUkey_or_POLY_ID)                        rendition_=0;
   */
   /* NYI
   virtual bool done_with
      (const std::string &MUkey_or_POLY_ID) = 0;
   */
   virtual Mapunit_composition *render_at_geocoordinate
      (const Geocoordinate &geocoordinate)                         rendition_=0;
   inline virtual nat32 generate()                      rendition_ { return 0; } //170904
      // generate() is just a hack until I implement CanSIS:NLDS generator
};
//_Soil_provider_________________________________________________________/
}//_namespace CS_______________________________________________________________/
#endif
