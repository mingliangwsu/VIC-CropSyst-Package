#ifndef CS_ontologyH
#define CS_ontologyH
#include "corn/container/unilist.h"
#include "corn/container/text_list.h"
#include "corn/metrology/units_clad.h"
#include "corn/OS/uniform_resource_locator.h"
//#include "corn/data_source/datarec.h"
#include "corn/format/structural.h"
#include "corn/format/VV/VV_document.h"


//---------------------------------------------------------------------------
/*
#define ONTOLOGY_CLASSIFICATION     "classification"
#define ONTOLOGY_PROPERTY           "property"
#define ONTOLOGY_CONCEPTUALIZATION  "conceptualization"
#define ONTOLOGY_MODEL              "model"
//#define ONTOLOGY_PARAMETER          "parameter"
//#define ONTOLOGY_XXXXXX           "xxxx"
*/
//---------------------------------------------------------------------------
namespace CS
{
//___________________________________________________________________________
struct Simulation_model_specification
{
   std::string domain;  // I.e. Crop Managament Simulation Scenario
   std::string model;   // I.e. CropSyst
   std::string version; // I.e. 5
      // The format of the version is flexible
      // and it could include subversion information,
      // but it must be lexicographically consistent to allow automatic version seclusion
   std::string version_date;  // I.e. 2017003 as it existed on Jan 3 2017
};
//_Simulation_model_specification______________________________________________/
namespace ontology
{
#define TYPE_CONCEPTUALIZATION   "conceptualization"
#define TYPE_CLASSIFICATION      "classification"
#define TYPE_PROPERTY            "proprietate"
#define TYPE_PARAMETER           "parametro"

//#define TYPE_PROPERTY            "property"
//#define TYPE_PARAMETER           "parameter"
#define TYPE_MODEL               "modello"
#define TYPE_DOMAIN              "dominio"
//___________________________________________________________________________
class Translation
: public extends_ CORN::Item
{public:
   std::string language; // ISO language code (2 character)
   std::wstring text;
 public:
   inline Translation() {}
   inline Translation
      (const std::string &language_
      ,const std::wstring &text_)
      : CORN::Item()
      , language  (language_)
      , text      (text_)
      {
         // CORN::string_to_wstring(text_,text);
      }

   Translation(const structural::Pair_key_value &item);
   inline virtual bool is_key_string(const std::string &key)        affirmation_ //180820
      { return language == key; }

};
//_Translation_________________________________________________________________/
class Translated_text
:public extends_ CORN::Unidirectional_list // list of Translation
{public:
   std::string name; // I.e. caption description intention
 public:
   Translated_text(const std::string &name_);
   bool translate(std::wstring &translated)                               const;
};
//_Translated_text_____________________________________________________________/
class Range
{
   CORN::Units_clad  units; // (metric) units of range values only!
   float32           minimum;
   float32           maximum;
 public:
   Range()
      : units(0)
      , minimum(-99999)
      , maximum( 99999)
      {}
};
//_Range_______________________________________________________________________/
class Elements
{
 public:
   Elements();
   inline virtual ~Elements() {}
   bool get_translated_text(modifiable_ Translated_text &text);
};
//_Elements____________________________________________________________________/
class Elements_conceptual
: public implements_ Elements
{public:

   // These are lists of translations
   Translated_text caption;
   Translated_text description; // brief description
   Translated_text intention;   // How the value is to be used (hint)
   Translated_text documentation;
   nat8 mesura; //  measure code (not full units)
   // NYN bool optional; probably technical
 public:
   Elements_conceptual();
};
//_Elements_conceptual______________________________________________2016-02-27_/
class Elements_theoretical
: public extends_ Elements_conceptual
{public:
   Range extremo; //  extreme
 public:
   Elements_theoretical();
   virtual bool expect_structure()                                 expectation_;
};
//_Elements_theoretical_____________________________________________2016-02-27_/
class Elements_operational
: public extends_ Elements_theoretical
{public:
   Range limitation;  // expected range

   /*
   // I think I will have application and provision documents
   // be either local files or URL links.

   Translated_text pplication; // was use Links to external documentation describing models and uses of this variable (for respective languages)
   Translated_text provision;         // URLs where to data sources where  measured and/or extrapolated data may be obtained
      // provision might actually be operational because it conserns
      // use of data values
   */
 public:
   Elements_operational();
   virtual bool expect_structure()                                 expectation_;
   //NYN virtual bool get_end();
};
//_Elements_operational_____________________________________________2016-02-27_/
class Elements_model
: public extends_ Elements_operational
{public:
   enum Model
      { none
      , selectable
      , intrisic
      , linear
      , statistical
      };
   std::string  exemplar; //  selectable or intrinsic or linear or statistical   N/A or omitted indicates no model parameters
   //NYI may want to use Model enum
 public:
   Elements_model();
      virtual bool expect_structure()                              expectation_;
};
//_Elements_model___________________________________________________2016-12-31_/
class Elements_property
: public extends_ Elements_operational
{public:
   inline Elements_property()
      : Elements_operational()
      {}
};
//_Elements_property________________________________________________2016-12-31_/
class Elements_parameter
: public extends_ Elements_property
{public:
   CORN::Units_clad units;
   bool vector;
 public:
   inline Elements_parameter()
      : Elements_property()
      , units(U_units_not_specified)
      , vector(false)
      {}
 public:
   inline bool is_scalar()                     affirmation_  { return !vector; }
   inline bool is_vector()                     affirmation_  { return vector; }
};
//_Elements_parameter_______________________________________________2016-12-31_/
class Definition
: public implements_ CORN::Item
{public:
   enum Kind
      { kind_unknown
      , kind_conceptualization
      , kind_property
      , kind_model
      , kind_parameter
      , kind_classification
      , kind_domain };
   CORN::OS::Uniform_resource_identifier generalization;
      // URI of definition file/directory, this definition extends
      // (if not or in addition to the parent??)

   Elements *elements; // Elements_complete element;
 private:
   provided_ CORN::Unidirectional_list  subdefinitions;
 protected:
   CORN::OS::Directory_name       *directory;
      // owned, may be 0 if not extended
   //CORN::OS::Directory_entry_name *filename;
   CORN::OS::File_name *filename;
      // owned
      // Definitions may be either a simple or extended
      // Simple definitions are a single file with name.type
      // where the extension is conceptualization property model
      // I.e.  C:\CS_Suite_5\ontology\Crop\xxx\yyy\zzz.type
      // Extended definitions consist of a directory which will have
      // a type definition file with the name type
      // I.e.  C:\CS_Suite_5\ontology\Crop\xxx\yyy\zzz\type
   bool extended;
   Kind kind;
   //170126 now using kind enum std::string type;
      // The type will be encoded in definition_DEN
      // either in the extension (simple) or the name (extended)
      // I.e. conceptualization classification property parameter model

   std::wstring ID;
      // The ID corresponds to the path under the domain of the definition directory
      // The ID must be a valid anchor ID so directory separators are replaced with :
      // http://stackoverflow.com/questions/70579/what-are-valid-values-for-the-id-attribute-in-html
      // The ID excludes the domain path
      // (also does not include the definition filename).
 private:
   contribute_ CORN::Text_list definition_ID_names;

 public: // structures
   Definition(CORN::OS::Directory_entry_name *DEN_given);
   virtual ~Definition();
 public: // Item implementation
   virtual const wchar_t *label_wstring(std::wstring &buffer)             const;
      // Returns the element's caption in the preferred available language
   virtual const wchar_t *key_wstring(std::wstring &buffer)               const;
      // Returns the definition ID (key) corresponding to the definition directory
   inline virtual bool is_key_wstring(const std::wstring &key)      affirmation_ //180820
      { return ID == key; }
 public:
   virtual bool initialize()                                    initialization_;
      // setup the key and load the defininition (elements) file
   const CORN::Unidirectional_list &provide_subdefinitions()         provision_;
   nat32 render_specific_definitions
      (CORN::Container &definitions
      //,const std::string &specific
      ,Definition::Kind of_kind)                                     rendition_;
   const std::wstring &get_hint(std::wstring &hint)                       const;
      // returns the element intention
   /*
   nat32 render_parameter_definitions  (CORN::Container &parameter_definitions,bool apropos) rendition_;
   nat32 render_model_definitions      (CORN::Container &model_definitions    ,bool apropos) rendition_;
   nat32 render_all_definitions        (CORN::Container &all_definitions      ,bool apropos) rendition_;
   */
   bool is_parameter()                                             affirmation_;
   bool is_property()                                              affirmation_;
   bool is_expired()                                               affirmation_;
   virtual bool is_valid()                                         affirmation_;
      /// \return true if is recongized ontological definition
   bool is_applicable()                                            affirmation_;
      /// \return true if applicable to current model version/selection options
   virtual bool is_extended()                                      affirmation_;
 private:
   CORN::OS::File_name *find_extended_definition_filename
      (CORN::OS::Directory_entry_name *DEN_given);
};
//_Definition_conceptual____________________________________________2016-02-27_/
class Definition_listable
: public extends_ CORN::Item
{private:
 public:
   const Definition &definition;
 public:  // structors
    Definition_listable(const Definition &definition_);
 public:
   inline virtual const wchar_t *label_wstring(std::wstring &buffer)       const
      { return definition.label_wstring(buffer); }
   inline virtual const wchar_t *key_wstring(std::wstring &buffer)         const
      { return  definition.key_wstring(buffer); }
};
//_Definition_listable______________________________________________2017-01-01_/
class Domain
: public extends_ Definition
{
/*
 protected: // members
   CORN::OS::Directory_name *domain_directory; //owned
*/
 public: // structors
   Domain(CORN::OS::Directory_entry_name *domain_directory_given);
   inline virtual bool is_extended() affirmation_ { return true; }
   inline virtual bool initialize()                              initialization_
      { return true; }
      // domain does not have definition file
};
//_Domain___________________________________________________________2016-02-27_/
extern std::string language_precedence;
extern CORN::Text_list *language_precedence_list;
const CORN::Text_list *provide_language_precedence_list();
//______________________________________________________________________________
//______________________________________________________________________________
class Range_map
: public implements_ structural::Map
{
   Range &range;
 public:
   inline Range_map(Range &range_)
      : structural::Map()
      , range(range_)
      {}
   virtual nat32 expect()                                          expectation_;
};
//__________________________________________________________________2020-03-04_/
class Elements_map
: public implements_ CORN::VV::Document /// structural::Map
{
   Elements &elements;
 public:
   virtual nat32 expect()                                          expectation_;
};
//_Elements_map_____________________________________________________2020-03-04_/
class Elements_conceptual_map
{
   Elements_conceptual *elements_conceptual;

};
//_Elements_conceptual_map__________________________________________2020-03-04_/
class Elements_theoretical_map
: public extends_ Elements_map
{

};
//_Elements_theoretical_map_________________________________________2020-03-04_/
}}//_namespace_CS_ontology_____________________________________________________/
#endif

