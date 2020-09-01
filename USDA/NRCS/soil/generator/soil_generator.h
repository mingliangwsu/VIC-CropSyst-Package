#ifndef soil_generatorH
#define soil_generatorH
//______________________________________________________________________________
#include "USDA/NRCS/soil/survey/database/STATSGO_SSURGO_database.h"
#include "USDA/NRCS/soil/survey/database/tabular/mapunit.h"
#include "USDA/NRCS/soil/survey/database/tabular/component.h"
#include "common/geodesy/geolocation.h"
#include "common/soil/texture.h"
#include "corn/data_source/datarec.h"
#include "common/database/soil/provider/soil_provider_common.h"

#define SEARCH_CONTAINER CORN::Association_search_tree
namespace CORN
{  class Data_table;
   class Container;
   class Association_search_tree;
}
extern char *latitude_fieldname;
extern char *longitude_fieldname;
extern char *cell_fieldname;
namespace USDA_NRCS
{
//190928   class CHorizon;
   class CHorizon_struct;
//______________________________________________________________________________
namespace STATSGO2_SSURGO
{
class Mapunit_composition;
// ______________________________________________________________________________
class Component_composition
: public CS::Component_composition                                               //141015
{
 public:
   USDA_NRCS::Component_record component;
   USDA_NRCS::Capability_class_clad &irrcapcl; // for debugging
   USDA_NRCS::Capability_class_clad &nirrcapcl; // for debugging
   inline Component_composition()
      : CS::Component_composition                       (component.comppct.repr) //141015
      , irrcapcl                                            (component.irrcapcl)
      , nirrcapcl                                          (component.nirrcapcl)
      {}
   bool compose(const USDA_NRCS::CHorizon_struct &chorizon);
   bool compose_meta(const USDA_NRCS::Mapunit_struct &map_unit);
   USDA_texture::Texture_code lookup_texture_description
      (const std::string chorizon_chkey
      ,float32 &horizon_sand
      ,float32 &horizon_clay
      ,float32 &horizon_silt);
   inline virtual bool is_key_string(const std::string &key)        affirmation_ //180820
      { return component.cokey == key; }
   inline virtual const char    *key_string (std::string &buffer)          const //190222
      {  buffer=component.cokey; return buffer.c_str(); }
};
// ______________________________________________________________________________
class Mapunit_composition
: public CS::Mapunit_composition
{public:
      // Geolocation may be optionally specified in points of filter_intersection table.
   USDA_NRCS::Mapunit_record mapunit;
   Component_composition *predominate_component;
      // This is the same as predominate_component_composition but
      // as STATSGO::Component_composition.
   //190929 obs bool find_by_component;
 public:
   inline Mapunit_composition()
      : CS::Mapunit_composition                                               ()
      , mapunit                                                               ()
      //190929 obs , find_by_component                                                (false)
      , predominate_component                                                (0)
      {}

   bool take_component_composition                                               //190927
      (Component_composition *Component_composition_given);

   inline virtual bool is_key(const std::string &key)              affirmation_  //180820
      { return
         //190929 find_by_component && predominate_component
         //190929    ? predominate_component->is_key_string(key) // 180820 get_key()
         //190929    :
            mapunit.mukey == key;
      }
   inline virtual const char    *key_string (std::string &buffer)          const
      {
         //190929 if (find_by_component && predominate_component)
         //190929       predominate_component->key_string(buffer);
         //190929 else
         buffer.assign(mapunit.mukey);
         return buffer.c_str();
      }
 public:
   /*190927
   Component_composition *provide_component_composition(const std::string &cokey);
   */
   Component_composition *find_component_composition(const std::string &cokey);  //190927

   /*190927 never used
   inline void key_on_component()                  { find_by_component = true; }
   */
   /*190927 never used
   inline void key_on_mapunit()                   { find_by_component = false; }
   */
   /*190927 obsolete
   bool compose(USDA_NRCS::CHorizon &chorizon);
   */
   //190929 virtual bool commit();
 public:
   bool is_more_agricultural_than_predominant
      (const USDA_NRCS::Component_struct &new_component)                  const;
};
// ________________________________________________________Mapunit_composition_/
class Soil_generator
: public CS::Soil_provider                                                       //141001
{
/* This is an abstract class
A derived class needs to implement a method to write the soil data
to a target file format.
*/
   USDA_NRCS::STATSGO2_SSURGO::Database STATSGO2_database;   // database because we are already in STATSGO2 context
   CS::Soil_database &target_soil_database;                                      //190222
 protected:
   const USDA_NRCS::STATSGO2_SSURGO::Database::Arguments &arguments;
   /*moved to arguments
   nat8 aggregation_option;
      //   0=Extract map unit soil components
      //   1=Aggregate soil sequences in each map unit to create an aggregated representitive pseudopedon
      //   2=Use primary component as a representitive pedon
   */
/*obsolete now using seclusion
   contribute_ nat32 searching_for_specific_mapunit;
   contribute_ Mapunit_composition *found_specific_mapunit;
*/
 private:
   mutable Mapunit_composition *curr_mapunit_composition;                        //190927
      // currently presuming processing one map unit a time time
      // (make sure components are sorted also by MUkey
      // May want/need to have list in memory if not sorted
 public:
   Soil_generator                                                                //170903
      ( const USDA_NRCS::STATSGO2_SSURGO::Database::Arguments &arguments_
      ,const CORN::Seclusion &mapunit_seclusion
      ,CS::Soil_database &target_soil_database);                                 //190222
 public:
   virtual nat32 generate()                                          rendition_;
      /**<
      This method will render mapunits for all filtered mapunits.
      For each mapunit, the take() method will be called.
      Derived classes should override the take method
      to further process and/or output the map unit composition.
      The derived class is responsible for eventually deleted the
      rendered mapunit composition
              I.e. the take() method may delete it after writing it,
        , or store the composition and a directory structure which will
      be deleted later, or give it to another object for disposal   )
      **/
   /*190927 obsolete
   virtual Mapunit_composition *render_selected_mapunit
      (const std::string &MUkey)                                     rendition_; //150927
   virtual Mapunit_composition *render_selected_mapunit(nat32 MUkey)rendition_; //150927
   */
      /**< These method can be used to to render a single mapunit composition
       for the specified mapunit key.
      **/
   virtual Mapunit_composition *render_at_geocoordinate
      (const Geocoordinate &geocoordinate)                           rendition_; //150927
   /*190929
   bool commit_mapunit( Mapunit_composition &map_unit)            contribution_; //150927   modification_;
   */
      /**<
      Derived classed may optionally reimplement commit
      to add or augment data to the map_unit composition.
      In this case, the reimplementation will probably want
      to call this superclass's commit() method.
      **/
   bool give_to_database(Mapunit_composition *map_unit_comp)         rendition_; //190225
 protected:
   Mapunit_composition *provide_mapunit_composition                              //190927
      (const std::string &MUkey)                                     provision_;
   //      (nat32 MUkey)     provision_; //190927
   Component_composition *provide_component_composition                          //190927
      (const std::string &cokey)                                     provision_;
};
//_Soil_generator___________________________________________________2011-10-21_/
}}//_namespace STATSGO2_SSURGO_namespace USDA_NRCS_____________________________/
#endif

