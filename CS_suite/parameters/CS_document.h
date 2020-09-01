#ifndef CS_documentH
#define CS_documentH

#include "corn/format/structural.h"
#include "corn/container/text_list.h"
#include "corn/parameters/parameters_datarec.h"
#include "corn/parameters/parameter.h"
#include "corn/math/numerical/interpolation/interpolator_temporal.h"
#include "corn/OS/uniform_resource_locator.h"
#include "common/simulation/synchronization.h"

//______________________________________________________________________________
namespace CS {
enum Crop_submodels
{  unknown_submodels
,  model_TUE_efficiency
,  GENERALLY_APPLICABLE
};
//______________________________________________________________________________
class Parameter_adjustment_settings
: public implements_ CORN::Item
{
   modifiable_ float32       &normalization;
   CORN::Interpolation_curve_type   interpolation_curve_type;
   CORN::Unidirectional_list *interpolation; // optional list of observations
   CORN::Unidirectional_list *observations;  // optional list of observations
 public:
   inline Parameter_adjustment_settings
      (modifiable_ float32 &_normalization)
      : normalization            (_normalization)
      , interpolation_curve_type (CORN::step_curve)
      , interpolation            (0)
      , observations             (0)
      {}
};
//_class Parameter_adjustment_settings______________________________2015-09-04_/
#define ADJUSTABLE       true
#define NOT_ADJUSTABLE  false
#define POT_ADJUSTABLE   true
   // potentially ajustable

#define OBSERVABLE       true
#define NOT_OBSERVABLE  false
#define POT_OBSERVABLE   true
   // potentially observable
//______________________________________________________________________________
struct Parameter_association
: public implements_ CORN::Item
{
 public:
   bool encountered;
 public:
   Parameter_association()
      : CORN::Item()
      , encountered(false)
      {}
};
//_struct Parameter_association_________________________________________________
struct Parameter_association_numeric
: public extends_ Parameter_association
, public extends_ CORN::Number_keyed
{
   bool adjustable;
   bool observable;
   Crop_submodels applicable_submodels[];
      // submodels this parameter is known to be applicable
      // 0 if generally applicable

 public:
   inline virtual bool is_key_string(const std::string &key)              affirmation_  //180820
      { return CORN::Number_keyed::is_key_string(key); }

/*180820  was probably only used for find_cstr now using is_key
   inline virtual const char *get_key()                                    const
      { return CORN::Number_keyed::get_key(); }
*/      

   inline Parameter_association_numeric(int8    &_value,const char *_structure,bool _adjustable, bool _observable, Crop_submodels *_applicable_submodels = 0) : CORN::Number_keyed(_value), adjustable(_adjustable), observable(_observable) { know_key_cstr(_structure);}
   inline Parameter_association_numeric(nat8    &_value,const char *_structure,bool _adjustable, bool _observable, Crop_submodels *_applicable_submodels = 0) : CORN::Number_keyed(_value), adjustable(_adjustable), observable(_observable) { know_key_cstr(_structure);}
   inline Parameter_association_numeric(int16   &_value,const char *_structure,bool _adjustable, bool _observable, Crop_submodels *_applicable_submodels = 0) : CORN::Number_keyed(_value), adjustable(_adjustable), observable(_observable) { know_key_cstr(_structure);}
   inline Parameter_association_numeric(nat16   &_value,const char *_structure,bool _adjustable, bool _observable, Crop_submodels *_applicable_submodels = 0) : CORN::Number_keyed(_value), adjustable(_adjustable), observable(_observable) { know_key_cstr(_structure);}
   inline Parameter_association_numeric(int32   &_value,const char *_structure,bool _adjustable, bool _observable, Crop_submodels *_applicable_submodels = 0) : CORN::Number_keyed(_value), adjustable(_adjustable), observable(_observable) { know_key_cstr(_structure);}
   inline Parameter_association_numeric(nat32   &_value,const char *_structure,bool _adjustable, bool _observable, Crop_submodels *_applicable_submodels = 0) : CORN::Number_keyed(_value), adjustable(_adjustable), observable(_observable) { know_key_cstr(_structure);}
   inline Parameter_association_numeric(float32 &_value,const char *_structure,bool _adjustable, bool _observable, Crop_submodels *_applicable_submodels = 0) : CORN::Number_keyed(_value), adjustable(_adjustable), observable(_observable) { know_key_cstr(_structure);}
   inline Parameter_association_numeric(float64 &_value,const char *_structure,bool _adjustable, bool _observable, Crop_submodels *_applicable_submodels = 0) : CORN::Number_keyed(_value), adjustable(_adjustable), observable(_observable) { know_key_cstr(_structure);}

   inline Parameter_association_numeric(const int8    &_value,const char *_structure,bool _adjustable, bool _observable, Crop_submodels *_applicable_submodels = 0) : CORN::Number_keyed(_value), adjustable(_adjustable), observable(_observable) { know_key_cstr(_structure);}
   inline Parameter_association_numeric(const nat8    &_value,const char *_structure,bool _adjustable, bool _observable, Crop_submodels *_applicable_submodels = 0) : CORN::Number_keyed(_value), adjustable(_adjustable), observable(_observable) { know_key_cstr(_structure);}
   inline Parameter_association_numeric(const int16   &_value,const char *_structure,bool _adjustable, bool _observable, Crop_submodels *_applicable_submodels = 0) : CORN::Number_keyed(_value), adjustable(_adjustable), observable(_observable) { know_key_cstr(_structure);}
   inline Parameter_association_numeric(const nat16   &_value,const char *_structure,bool _adjustable, bool _observable, Crop_submodels *_applicable_submodels = 0) : CORN::Number_keyed(_value), adjustable(_adjustable), observable(_observable) { know_key_cstr(_structure);}
   inline Parameter_association_numeric(const int32   &_value,const char *_structure,bool _adjustable, bool _observable, Crop_submodels *_applicable_submodels = 0) : CORN::Number_keyed(_value), adjustable(_adjustable), observable(_observable) { know_key_cstr(_structure);}
   inline Parameter_association_numeric(const nat32   &_value,const char *_structure,bool _adjustable, bool _observable, Crop_submodels *_applicable_submodels = 0) : CORN::Number_keyed(_value), adjustable(_adjustable), observable(_observable) { know_key_cstr(_structure);}
   inline Parameter_association_numeric(const float32 &_value,const char *_structure,bool _adjustable, bool _observable, Crop_submodels *_applicable_submodels = 0) : CORN::Number_keyed(_value), adjustable(_adjustable), observable(_observable) { know_key_cstr(_structure);}
   inline Parameter_association_numeric(const float64 &_value,const char *_structure,bool _adjustable, bool _observable, Crop_submodels *_applicable_submodels = 0) : CORN::Number_keyed(_value), adjustable(_adjustable), observable(_observable) { know_key_cstr(_structure);}
};
//_struct Parameter_association_numeric_____________________________2015-09-07_/
struct Parameter_association_enum
: public extends_ Parameter_association
{
   Enum_abstract &enum_abst;                                                     //190605
   //190605  Labeled_enum &labeled_enum;
   std::string key_ontological;
 public:
   inline virtual bool is_key_string(const std::string &key)              affirmation_  //180820
      { return key_ontological == key; }

/*180820  was probably only used for find_cstr now using is_key
    inline virtual const char *get_key()                                   const
      { return key_ontological.c_str(); }
*/      
   inline Parameter_association_enum
      (Enum_abstract &enum_abst_                                                 //190605
      ,const char *structure_)
      : enum_abst(enum_abst_)
      , key_ontological(structure_)
      {}
};
//_struct Parameter_association_enum________________________________2015-11-22_/
struct Parameter_association_bool
: public extends_ Parameter_association
{
   bool &bool_value;
   std::string key_ontological;
 public:
   inline virtual bool is_key_string(const std::string &key)              affirmation_  //180820
      { return key_ontological == key; }

/*180820  was probably only used for find_cstr now using is_key
    inline virtual const char *get_key()                                    const
      { return key_ontological.c_str(); }
*/      
   inline Parameter_association_bool
      (bool &bool_value_
      ,const char *structure_)
      : bool_value(bool_value_)
      , key_ontological(structure_)
      {}
};
//_struct Parameter_association_bool________________________________2015-11-22_/
class Parameter_adjustment
: public extends_ structural::Mapping
, public extends_interface_ structural::Pair_key_value
{
   Parameter_adjustment_settings &settings;
 public:
   inline Parameter_adjustment(Parameter_adjustment_settings &_settings)
      : settings(_settings)
      {}
   virtual structural::Pair_key_value *provide_key_value_pair
      (structural::Construct *key_given) ;
};
//_class Parameter_adjustment_______________________________________2015-09-04_/
class Document
: public extends_ structural::Document
// moved to structural::Document: public extends_ structural::Mapping_clad
// A document is considered a key value pair (The key would be the document file name)
// Note that documents (in a directory are essentially a mapping)
// The directory itself and thus the entire file system is effectively a mapping!
// Directory entry names are keys in this mapping.
{



   Document();
};
//_class Document___________________________________________________2015-09-19_/
#ifdef abandoned

Parameter pertinence was an idea to associate parameters
relevent to specific models.

Now there will be model definitions (similar to ontology directory)
The model has a defining document
or document.aglomeration directory with parameter files,
which specify the parameters of the model.

class Document_configured
: public extends_ Document
{ public:
   //___________________________________________________________________________
   class Parameter_pertinence
   : public implements_ CORN::Item
   {public:
      std::wstring structure;
      const Enum_abstract &enumeration;
      // const Labeled_enum &enumeration;
      nat32       enum_option;
    public:
      inline Parameter_pertinence
         (const char *structure_     // ontology
         ,const Enum_abstract &enumeration_
         //190605 ,const Labeled_enum &enumeration_
         ,nat32       enum_option_)
         : CORN::Item()
         , structure    ()
         , enumeration  (enumeration_)
         , enum_option  (enum_option_)
         {  CORN::ASCIIZ_to_wstring(structure_,structure);
         }
   };
   //_class Parameter_pertinence____________________________________2016-06-30_/
 public:
   CORN::Unidirectional_list parameter_pertinencies;                             //160630
   CORN::Text_list           parameter_essentials;                               //160630
 protected:
   CORN::Unidirectional_list parameter_associations;
 public: // construction
   inline Document_configured()
      : Document()
      {}
   virtual bool initialize()                                    initialization_;
   virtual bool initialize_parameter_associations()             initialization_ = 0; //160630
   virtual bool initialize_relevencies()                        initialization_;
   virtual bool initialize_parameter_pertinencies()             initialization_  { return true; } //160630
   virtual bool initialize_parameter_essentials()               initialization_  { return true; } //160630
 public: // Mapping implementations
   //virtual structural::Pair_key_value *provide_key_value_pair(structural::Construct *key_given) ;

 public: // Priming with configured structures
   bool know(modifiable_ Parameter_association *param_assoc)      appropriation_;
   //ol take(modifiable_ Parameter_association_numeric *param_assoc_given) appropriation_;
      // Actually we should now be able to process any type but NYI

   bool is_pertinent_wstring(const std::wstring &parameter_or_propery_qualified) affirmation_; //160630
   bool is_pertinent_cstr(const char *parameter_or_propery_qualified)            affirmation_; //160630
       /// \returns true if the specified element is pertinent according to
       /// selected parameter file options
   bool is_essential_wstring(const std::wstring &parameter_or_propery_qualified) affirmation_; //160630
       /// \returns true if the specified element is essential
       /// (critical to the model).
/*190606 check still needed
   bool get_value_as_string                                                      //160701
      (const std::wstring &parameter_or_propery_qualified
      ,modifiable_ std::string &value
      ,modifiable_ std::string &units)                                    const;
*/
 private:
   bool compose_structure()                                     initialization_;
   bool know_parameter_association
      (modifiable_ structural::Mapping &in_mapping
      ,modifiable_ Parameter_association *param_assoc_know
      ,modifiable_ CORN::Text_list &key_list_given);
/*
   bool take_parameter_association_numeric
      (modifiable_ structural::Mapping &in_mapping
      ,modifiable_ Parameter_association_numeric *param_assoc_given
      ,modifiable_ CORN::Text_list &key_list_given);
*/
   virtual nat32 count()                                                   const
      {
         // I don't remember what would the the mappings is this context
         // so I need to check this
         return parameter_associations.count();
      }
};
//_class Document_configured________________________________________2015-09-19_/
#endif
} // namespace CS
#endif
// CS_YAML_DOCUMENT_H

