#ifndef parameterH
#define parameterH
//______________________________________________________________________________
//These classes are replacing Valid_type  classes
#include <string>
#ifndef constH
#  include "corn/const.h"
#endif

#ifndef unitsH
#  include "corn/measure/units.h"
#endif

#ifndef units_enumH
#include "corn/measure/units_enum.h"
#endif
#include "corn/container/item.h"
#ifndef number_keyedH
#include "corn/parameters/number_keyed.h"
#endif
#include "corn/format/field.h"
#include "corn/chronometry/date_format.h"
#include "corn/chronometry/time_format.h"

namespace CORN {

class Data_record;
extern const char *HTML_validation_colors[];

//enum RUI_validation_stage {RUI_valid_cleanup,RUI_valid_allow_error_display,RUI_valid_error_displayed,RUI_valid_stop_error_display};
//          OWL calls the validator many many times, we actually only want to display the error message once
//          also we don't wan't some little precision loss to indicate a modification.

#define NO_KEY 0
//______________________________________________________________________________
interface_ Parameter_properties
: public extends_interface_ Field
{
public:
   virtual ~Parameter_properties();                                              //130527
public:
   virtual bool is_key_string(const std::string &key)              affirmation_;  //180820

   virtual const char* get_key()                                          const;

      // The implementation of get_key is common and only uses virtual methods
      // so Parameter_properties_interface is still an interface
   //200302obs? virtual const char* get_name_subsection()                            const=0; //130915
   virtual const char* get_date_time_format()                             const
      { return get_format(); }                                                   //161013
      // The units caption may be used to encode the date format.
      // units_caption specified, will override output of unit_code.
      // Use NULL if no units.
      // For RNAutoControls if units_code is 0 and units_caption is NULL
      // then the form designed units caption
      // will be displayed (with no options for alternate units)
      // even if alt_units_codes are set for the parameter.
   virtual const Date_format *provide_format_date()                provision_=0;
   virtual const Time_format *provide_format_time()                provision_=0;
   // provide format may return 0 if not format is provided or not applicable
};
//_Parameter_properties________________________________________________________/
struct Parameter_properties_literal
{  // This version of the parameter properties uses char arrays to store string data member and is used to implement literal parameter property structures.
   const char* name_long;
      // Must be under 128 characters
      // (this is a limit of the latest version of SQL)
      // But it is recommended to be under 64 characters
   const char* name_short;
      // This is a text code used when importing from or exporting to other databases
      // that typically allow for field names less than 30 characters
      // (In this case Oracle or other SQL)
   //200203 const char* name_subsection;  // name_subsection as it appears under a section (data structure)
   const char* caption;
      // Description of the property/variable as it would appear
      // in users interfaces or reports.
      // So that  caption would be a some what abbreviated
      // to fit in user intput captions (typically less than 80 or preferably less than 60 characters)
   const char* hint;
   const char* ontology;
      // URL to a more thorough description of the property/value
      // actually this would be an ontology
   Units_code  units_code;
   const char *prefered_units_caption_or_date_time_format;                       //130515
};
//_Parameter_properties_literal________________________________________________/
/*conceptual
struct Parameter_property_ontological
{  // This version is used when the properties definition is not stored
   // in the program but rather an eternal data source
   // such as some where in the application suite's directory structure
   // or at the specified URL
   const char* property;
      // The property is a path in the form   dir/dir/name
      // The path is not fully qualified and the file name extension is not
      // specified here as the full URL/filename will be composed when needed.
      // This effectively indexes a property definition filename or a directory
      // containing additional propery definition files.
   Units_code  units_code;
   nat32 descriptor;  // Currently statistical descriptor optional
};
//_Parameter_property_ontological___________________________________2016-05-20_/
*/
//template for listener description metadata:
//CORN::Parameter_properties_literal XXX_meta{"long_name"          , "short_name",SUBSECTION_UNUSED,"caption",HINT_UNUSED,"https://en.wikipedia.org/wiki/XXXX"       ,UC_XXXXX,0};
#define SUBSECTION_UNUSED  0
#define HINT_UNUSED        0
#define URL_UNUSED         0
#define prefered_units_caption prefered_units_caption_or_date_time_format
#define prefered_date_time_format prefered_units_caption_or_date_time_format
//______________________________________________________________________________
struct Parameter_format
{
   Delineation_field delineation;
   Format_field      format;
/*
 public:
   inline Parameter_format(const Parameter_format &from)
      : delineation(from.delineation)
      , format(from.format)
      {}
*/
};
//200224 Probably deprecated



/*200224
{
   nat16   field_width;    //should be nat8 but using 16 for AutoControls
      // This is a recommended field width when storing data to
      // databases or file formats that specify field widths
   nat16    precision;     //should be nat8 but using 16 for AutoControls
   nat16    radix;         //should be nat8 but using 16 for AutoControls
};
*/
//_Parameter_format_________________________________________________2014-01-31_/
// Example template
// Parameter_properties_literal PP_xxxxxx = {"Key","ShortName","namesubsection",CORN_default_field_width,"Caption","Hint","HTTP:\\descriptionURL",UC_unitless};
//______________________________________________________________________________
class Parameter_properties_abstract
: public implements_ Parameter_properties //_interface
{
 protected:
   struct Format_temporal
   {  // These are composed formats that are not stored in (TDF) files
      // currently only the
      // Parameter_number_properties:::prefered_units_caption_or_date_time_format
      // is stored (overriding the default format
      Date_format *date;
      Time_format *time;
      Format_temporal()
      : date(0)
      , time(0)
      {}
   } provided_ format_temporal;
 public:
   inline Parameter_properties_abstract()
      : format_temporal()
      {}
   virtual const Date_format *provide_format_date()                  provision_;
   virtual const Time_format *provide_format_time()                  provision_;

};
//_class_Parameter_properties_abstract______________________________2019-01-10_/
class Parameter_properties_c_str
: public implements_ Parameter_properties_abstract                               //190110_40131
, public Parameter_properties_literal
{  // This version of the parameter properties uses char arrays to store string data member and is used to implement literal parameter property structures.
public:
   inline virtual ~Parameter_properties_c_str() {}                               //130515
      // Compilers complain when base class with virtual does not have virtual destructor
   inline virtual const char* get_name_long()         const { return strlen(name_long)  ? name_long : name_short; }
   inline virtual const char* get_name_short()        const { return strlen(name_short) ? name_short : name_long; }
   //200302obs? inline virtual const char* get_name_subsection()   const { return strlen(name_subsection) ? name_subsection : name_long; }
   inline virtual const char* get_caption_SDF()       const { return caption; }
   inline virtual const char* get_hint()              const { return hint; }
   inline virtual const char* get_ontology()          const { return ontology; }
   //NYI inline virtual const std::string &get_ontology()   const { return std::string(ontology); }
   inline virtual Units_code  get_units_code()        const { return units_code; }
   inline virtual const char* get_units_caption()     const { return prefered_units_caption; }
              // NYI if  units_caption.length() then return units_caption, otherwise get the caption from the smart_units code
   inline virtual const char* get_date_time_format()  const { return prefered_date_time_format; }
};
//______________________________________________________________________________
class Parameter_properties_string
// Note that string here does not mean that this applies only to strings,
// it means the metadata members of this struct are stored as strings
: public implements_ Parameter_properties_abstract //190110 interface
{  // This version uses strings to store the data members and would be used when reading parameter properties from a file
 public:
   std::string name_long;                 // rename this to name     (AKA key)
   std::string name_short; //                         (AKA label and key)
   //200302obs? std::string name_subsection;                                                  //130915
   std::string caption;
   std::string hint;
   std::string ontology;                                                         //190105
   Labeled_units_enum  units;
   std::string units_caption;     // ?rename this to alt or preferred units caption
   std::string &date_time_format;  // format reference is alias to units_caption for date time variables

   #if ((CS_VERSION >= 1) && (CS_VERSION <= 5))
   nat32 UED_variable;                                                           //299898
      // eventually obsolete, replacing with ontology.
   #endif
#define property_URL description_URL
// When using ontological model the parameter 'path/URL' will be stored in description_URL
public:
   inline Parameter_properties_string()
      : name_long       ("")
      , name_short      ("")
      , caption         ("")
      , hint            ("")
      , ontology        ("")
      , units           (0)
      , units_caption   ("")
      , date_time_format(units_caption)
      #if ((CS_VERSION >= 1) && (CS_VERSION <= 5))
      , UED_variable    (0)
      #endif
      {}
   inline virtual ~Parameter_properties_string() {}
      // Compilers complain when base class with virtual does not have virtual destructor
   inline virtual const char* get_name_long()         const { return name_long.length() ? name_long.c_str() : name_short.c_str(); }
   inline virtual const char* get_name_short()        const { return name_short.length() ? name_short.c_str() :name_long.c_str(); }
   //200302obs? inline virtual const char* get_name_subsection()   const { return name_subsection.length() ? name_subsection.c_str() :name_long.c_str(); }
   inline virtual const char* get_caption_SDF()       const { return caption.c_str(); }
   inline virtual const char* get_hint()              const { return hint.c_str(); }
   inline virtual const char* get_ontology_cstr()      const { return ontology.c_str(); } //190105
   inline virtual const std::string &get_ontology_string()   const { return ontology;} //190105
   virtual        Units_code  get_units_code()        const;                     //111206
   inline virtual const char* get_units_caption()     const { return units_caption.c_str(); }
   inline virtual const std::string &ref_units_caption() const { return units_caption; }
              // NYI if  units_caption.length() then return units_caption, otherwise get the caption from the smart_units code

   inline virtual const char* get_date_time_format()  const { return date_time_format.c_str(); }
   bool setup_structure(Data_record &data_rec,bool for_write );
};
//______________________________________________________________________________
interface_ Parameter_number_properties_interface
{  // Note that we use char arrays because  Parameter_number_properties are almost always set up as literals
   inline virtual ~Parameter_number_properties_interface() {}                    //130527
      // Compilers complain when base class with virtual does not have virtual destructor
   virtual float32 get_min_warning()   const = 0;  // The waring and error range values will be in the units of Unis_code
   virtual float32 get_max_warning()   const = 0;
   virtual float32 get_min_error()     const = 0;
   virtual float32 get_max_error()     const = 0;
   virtual nat8    get_precision()     const = 0;  // Applies only to floats
   virtual nat8    get_radix()         const = 0;  // applies only to naturals
};
//______________________________________________________________________________
struct Parameter_range
{
   float32  min_warning;
   float32  max_warning;
   float32  min_error;
   float32  max_error;
};
// example template
// Parameter_number_properties_literal PPN_xxxx={min_warn,max_warn,min_error,max_error, precision,10 };
extern Parameter_range  parameter_range_arbitrary;                               //130730
//200318 extern Parameter_format parameter_format_arbitrary;                              //130730

extern Parameter_format PPF_N;
extern Parameter_format PPF_F;
extern Parameter_format PPF_8_1;

/*
extern Parameter_format PPF_3N;
extern Parameter_format PPF_6_1;
//extern Parameter_format PPF_6_2;

extern Parameter_format PPF_8N;
extern Parameter_format PPF_8_1;
extern Parameter_format PPF_8_2;
extern Parameter_format PPF_8_4;
*/
//______________________________________________________________________________
class Parameter_number_properties
: public implements_ Parameter_number_properties_interface
, public Parameter_range                                                         //140131
, public Parameter_format                                                        //140131
{  // This version uses Note that we use char arrays because  Parameter_number_properties are almost always set up as literals
   // I don't have default value here, the parameter value is always already
   // set to an initial value in constructors, this will be assumed to be the default value.
   // The number parameter sets up and maintains the default value
 public:
   Parameter_number_properties();
   inline virtual ~Parameter_number_properties() {}                              //130515
      // Compilers complain when base class with virtual does not have virtual destructor
   inline virtual float get_min_warning()   const { return min_warning; }
   inline virtual float get_max_warning()   const { return max_warning; }
   inline virtual float get_min_error()     const { return min_error; }
   inline virtual float get_max_error()     const { return max_error; }

   inline virtual nat16 get_field_width()   const { return delineation.width; }  //200224 field_width
   inline virtual nat8  get_precision()     const { return format.precision; }   //20024
   inline virtual nat8  get_radix()         const { return format.radix; }       //20024
   bool setup_structure(Data_record &data_rec,bool for_write );
};
//______________________________________________________________________________
class Parameter_abstract          // abstract class
{
public:
   enum Validation_status
   { inviolate_status      // was unavailable_status  (Inviolate matches UPSILSON violation codes
   , ready_status       = 0x01
   // 0x02 to 0x0D  reserved for future use
   , disabled_status    = 0x0E
   , unknown_status     = 0x0F
   // The following status value have the same order as CORN::Quality_code error
   , not_valid_status   = 0x10
   , RESERVED_status       // reserved for future use not currently used (used to be out of range, but now we have the following low and high status
   , missing_status
   , warning_status
   , warning_low_status
   , warning_high_status
   , pot_error_status
   , pot_error_low_status
   , pot_error_high_status
   , error_status
   , error_low_status
   , error_high_status
   , STATUS_COUNT};
   contribute_ Validation_status  status;                                        //130731
 public:
   const Parameter_properties_literal &properties;   // owned by caller (actually should will normally be static literal constants)
   const Parameter_format             &format;                                   //140131
 public:
   Parameter_abstract
      (const Parameter_properties_literal &properties
      ,const Parameter_format             &format);                              //140131
   inline virtual ~Parameter_abstract()                                      {}  //130515
      // Compilers complain when base class with virtual does not have virtual destructor
   virtual Validation_status validate
      (std::string &message_text
      ,bool HTML_format) = 0; // pure
   inline bool is_valid()
      {  // Currently accept anything that is not an error as valid.
         return  (status < error_low_status) || (status > error_high_status);
      }
   inline Validation_status set_status(Validation_status new_status)
      { return status = new_status;}
   virtual const char *append_VV_to(std::string &buffer)                  const; //170523
   virtual const char *log(std::string &buffer)                           const;
   virtual const char *append_value(std::string &buffer)                const=0;
   virtual bool set_str(const char *value_cstr)                 modification_=0;
      ///< Set the value for the respective type from a C style null terminated string representation.
      ///  Must be overridden by derived classes.
      /// \return true if the value could be set given the string (Usually returns true)
   inline virtual bool set_string(const std::string &value_string) modification_
      { return set_str(value_string.c_str()); }
      ///< Set the value for the respective type string representation.
      /// \return true if the value could be set given the string (Usually returns true)
   inline virtual bool is_key_string(const std::string &key)        affirmation_  //180820
      { return key == properties.name_long;; }

   inline virtual const char *get_key()                                   const
      { return properties.name_long; }

   private:
   void set_key(const char *i_key);
 #ifdef __GNUC__
 public: // Not sure why GCC is giving non-static reference member ... cant use default assignment operator
    Parameter_abstract& operator= (const Parameter_abstract &/*pa*/) {return *this;} //150317
 #endif
};
//______________________________________________________________________________
//______________________________________________________________________________

class Parameter_number
: public extends_ Parameter_abstract
, public extends_ Number_keyed
{
public:
   float64 initial_value;
public:
   const Parameter_range&  range;
      // owned by caller (actually should normally be static literal constants)
   // There are multiple options for displaying units in the user
   // interface that override
   const Units_code*       alt_units_codes;
      // Null terminated array of units codes that can be user selected for showing units.
      // Usually will be a constant array, must remain in scope throughout existance of the parameter.
      // May be 0 if not applicable, or no alternate units are specified
      // For metric units the user interface may provide an option to
      // allow automatic input/output of any of the metric powers of 10.
      // alt_units_codes non null alt_units_codes overrides the automatic option
      // so it can be used to limit or suggest the acceptable alternate units.
   Units_clad              shown_units_code;
      // This is used to show the value in alternate units that the user may
      // have selected for display in the user interface input/edit fields or other data viewers.
      // The data value is NOT changed to these units, only it the input and/or displayed value.
      // The value is converted to/from these units.
public:
   Parameter_number(int8    &_value,const Parameter_properties_literal &_properties,const Parameter_range &_range,const Parameter_format &_format);
   Parameter_number(nat8    &_value,const Parameter_properties_literal &_properties,const Parameter_range &_range,const Parameter_format &_format);
   Parameter_number(int16   &_value,const Parameter_properties_literal &_properties,const Parameter_range &_range,const Parameter_format &_format);
   Parameter_number(nat16   &_value,const Parameter_properties_literal &_properties,const Parameter_range &_range,const Parameter_format &_format);
   Parameter_number(int32   &_value,const Parameter_properties_literal &_properties,const Parameter_range &_range,const Parameter_format &_format);
   Parameter_number(nat32   &_value,const Parameter_properties_literal &_properties,const Parameter_range &_range,const Parameter_format &_format);
   Parameter_number(float32 &_value,const Parameter_properties_literal &_properties,const Parameter_range &_range,const Parameter_format &_format);
   Parameter_number(float64 &_value,const Parameter_properties_literal &_properties,const Parameter_range &_range,const Parameter_format &_format);

   Parameter_number(const int8    &_value,const Parameter_properties_literal &_properties,const Parameter_range &_range,const Parameter_format &_format);
   Parameter_number(const nat8    &_value,const Parameter_properties_literal &_properties,const Parameter_range &_range,const Parameter_format &_format);
   Parameter_number(const int16   &_value,const Parameter_properties_literal &_properties,const Parameter_range &_range,const Parameter_format &_format);
   Parameter_number(const nat16   &_value,const Parameter_properties_literal &_properties,const Parameter_range &_range,const Parameter_format &_format);
   Parameter_number(const int32   &_value,const Parameter_properties_literal &_properties,const Parameter_range &_range,const Parameter_format &_format);
   Parameter_number(const nat32   &_value,const Parameter_properties_literal &_properties,const Parameter_range &_range,const Parameter_format &_format);
   Parameter_number(const float32 &_value,const Parameter_properties_literal &_properties,const Parameter_range &_range,const Parameter_format &_format);
   Parameter_number(const float64 &_value,const Parameter_properties_literal &_properties,const Parameter_range &_range,const Parameter_format &_format);

   // NYI include constructors for Date Time and Date_time
   virtual Parameter_abstract::Validation_status validate
      (std::string &message_text,bool HTML_format);
   const char* get_units_label(std::string& units_description)            const;
   virtual const char* log(std::string& buffer)                           const;
   inline void set_alt_units_codes(const Units_code* _alt_units_codes)
      { alt_units_codes = _alt_units_codes; }
   void reset_to_initial_value()                                  modification_;
   const char *append_value(std::string &buffer)                          const;
   inline virtual bool set_str(const char *value_cstr)            modification_
      { return Number_keyed::set_str(value_cstr); }
private:
   Parameter_abstract::Validation_status validate_value
      (std::string &message_text,bool HTML_format);
   void compose_range_message
      (std::string        &message_text  // Caller must provide large enough buffer
      ,bool                HTML_format
      ,bool                must
      ,float64             low
      ,float64             high
      ,float64             value
      ,const char *        units
      ,const char *        label1
      ,const char *        label2
      ,const char *        label3
      );
 #ifdef __GNUC__
 public: // Not sure why GCC is giving non-static reference member ... cant use default assignment operator
    Parameter_number & operator= (const Parameter_number &/*pa*/){return *this;} //150317
 #endif
};
//_class Parameter_number______________________________________________________/
}//_namespace_CORN_____________________________________________________________/
#endif

