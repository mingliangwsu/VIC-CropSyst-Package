// Rename this file to enum_clad

#ifndef enumH
#define enumH

#ifndef primitiveH
#  include "corn/primitive.h"
#endif
#include "corn/const.h"
#include <string>
class VV_enum_entry; // for friendship
//namespace CORN {
//______________________________________________________________________________
//#ifndef labeled_enumH
struct Label_description
{
   const char *label;
   const char *description;
};
//_Label_description________________________________________________2005-09-29_/
//#endif

// Implement using cowl and clad idiom  190109

class Enum_abstract
{friend class VV_enum_entry;
 protected:
 public: // temporarily public for debugging
   // Label tables may either be a simple array of strings specifying the labels in relative sequence.
   // Or an array of label and descriptions Label_description.
   // On or the other modes is used.
   const char **simple_label_table;
   Label_description *descriptive_label_table;                                   //050929
 public:
   Enum_abstract();                                                              //050929
   Enum_abstract(const char **label_table);
   // Constructor for simple_label_table_mode
   Enum_abstract(Label_description *descriptive_label_table);                    //050929
      //
   // Constructor for descriptive label_table mode.
   //190108 virtual const char *get_label(std::string &return_buffer)              const; //190108
   virtual const char *append_label(std::string &return_buffer)           const; //190109

      //190109 name to append_label  (implemented as append)

   // Returns the character string label associated with the
   // current enumeration value.
   // This must be overridden to provide the label for the current enumeration

   virtual std::ostream &stream_label(std::ostream &stream)          stream_IO_;

   virtual int32 set_label(const char *_label);
   // This must be overridden to set the enumeration value for the
   // specified label.
   virtual int32 get_int32()                                          const = 0;
   // This will return (casting) the current value to 32bit integer.
   // This must be overridden derived classes
   // RLN: Note if I had my druthers, enums would ALWAYS BE unsigned 32bits
   // But not all compiler vendors do this
   // Also using int32 will keep this class compatible with C# which damnably used signed 32bit integers ack!
   virtual int32 set_int32(int32 value_)                                   = 0 ;
   // This sets the value (casting)
   // This must be overridden derived classes
   virtual bool is_label(const char *_label)                       affirmation_;
   // return true if the label is in the label table
   // (used by CropSyst Crop parameter editor.
   //190109 virtual const char *get_description(std::string &return_buffer)        const; //190205_050929
   virtual const char *append_description(std::string &return_buffer)     const; //190109

      // probably implement as append_description

   // Returns the character string description associated with the current
   // enumeration value.
   // If it is a simple label table, the label is simply returned.
   inline virtual bool is_descriptive()                             affirmation_
      { return descriptive_label_table != 0; }                                   //050929
   // Returns true if this is a descriptive enumeration
   virtual nat16 get_max_label_length()                                   const; //050929
   // Returns the longest label length.
   // This is used to determine the field size for Data_records enumeration attributes.
};
//_Enum_abstract_______________________________________________________________/
//}//_namespace_CORN_____________________________________________________________/

#define DECLARE_ENUM_CLAD(labeled_enum_class,enumeration,i_label_table)        \
class labeled_enum_class                                                       \
: public Enum_abstract                                                   \
{                                                                              \
   enumeration value;                                                          \
public:                                                                        \
   inline labeled_enum_class(enumeration default_value = (enumeration)0)       \
      : Enum_abstract((const char **)i_label_table)                      \
      , value(default_value)                                                   \
      {}                                                                       \
   inline labeled_enum_class(const char *default_value_label)                  \
      : Enum_abstract((const char **)i_label_table)                      \
      , value((enumeration)0)                                                  \
      { set_label(default_value_label);}                                       \
   inline virtual ~labeled_enum_class() {}                                     \
   inline virtual int32 get_int32()       const {return (int32)value; }        \
   inline enumeration get()               const {return value; }               \
   inline virtual int32 set_int32(int32 value_)                                \
      {value = (enumeration)value_; return value_;}                            \
   inline enumeration set(enumeration value_) {value = value_; return value_;} \
   inline bool is(enumeration value_)     const { return value_ == value; }    \
};
//_DECLARE_ENUM_CLAD___________________________________________________________/
#define DECLARE_DESCRIPTIVE_ENUM_CLAD(labeled_enum_class,enumeration,descriptive_label_table_) \
class labeled_enum_class                                                       \
: public Enum_abstract                                                   \
{                                                                              \
   enumeration value;                                                          \
public:                                                                        \
   inline labeled_enum_class(enumeration default_value = (enumeration)0)       \
      : Enum_abstract((Label_description *)descriptive_label_table_) \
      , value(default_value)                                                   \
      {}                                                                       \
   inline labeled_enum_class(const char *default_value_label)                  \
      : Enum_abstract((Label_description *)descriptive_label_table_) \
      , value((enumeration)0)                                                  \
      { set_label(default_value_label);}                                       \
   inline virtual ~labeled_enum_class() {}                                     \
   inline virtual int32 get_int32()       const {return (int32)value; }        \
   inline enumeration get()               const {return value; }               \
   inline virtual int32 set_int32(int32 value_)                                \
      {value = (enumeration)value_; return value_;}                            \
   inline enumeration set(enumeration value_)  {value = value_; return value_;}\
   inline bool is(enumeration value_)     const { return value_ == value; }    \
};
//_DECLARE_DESCRIPTIVE_ENUM_CLAD_______________________________________________/
#define DECLARE_ENUM_COWL(labeled_enum_class,enumeration,_label_table)         \
class labeled_enum_class                                                       \
: public Enum_abstract                                                   \
{                                                                              \
   enumeration &value;                                                         \
public:                                                                        \
   inline labeled_enum_class                                                   \
      (enumeration &value_,enumeration default_value = (enumeration)0)         \
      : Enum_abstract((const char **)_label_table)                       \
      , value(value_)                                                          \
      { value = default_value; }                                               \
   inline labeled_enum_class(enumeration &value_,const char *default_value_label)\
      : Enum_abstract((const char **)_label_table)                       \
      , value(value_)                                                          \
      {  value = (enumeration)0;                                               \
         set_label(default_value_label);}                                      \
   inline virtual ~labeled_enum_class() {}                                     \
   inline virtual int32 get_int32()       const {return (int32)value; }        \
   inline enumeration get()               const {return value; }               \
   inline virtual int32 set_int32(int32 value_)                                \
      {value = (enumeration)value_; return value_;}                            \
   inline enumeration set(enumeration value_) {value = value_; return value_;} \
   inline bool is(enumeration value_)     const { return value_ == value; }    \
};
//_DECLARE_ENUM_COWL___________________________________________________________/
#define DECLARE_DESCRIPTIVE_ENUM_COWL                                          \
(labeled_enum_class,enumeration,descriptive_label_table_)                      \
class labeled_enum_class                                                       \
: public Enum_abstract                                                   \
{                                                                              \
   enumeration &value;                                                         \
public:                                                                        \
   inline labeled_enum_class                                                   \
      (enumeration &value_,enumeration default_value = (enumeration)0)         \
      : Enum_abstract((Label_description *)descriptive_label_table_) \
      , value(value_)                                                          \
      { value = default_value; }                                               \
   inline labeled_enum_class(enumeration &value_,const char *default_value_label) \
      : Enum_abstract((Label_description *)descriptive_label_table_) \
      , value(value_)                                                          \
      { value = (enumeration)0;                                                \
        set_label(default_value_label);}                                       \
   inline virtual ~labeled_enum_class()                                    {}  \
   inline virtual int32 get_int32()       const {return (int32)value; }        \
   inline enumeration get()               const {return value; }               \
   inline virtual int32 set_int32(int32 value_)                                \
      {value = (enumeration)value_; return value_;}                            \
   inline enumeration set(enumeration value_) {value = value_; return value_;} \
   inline bool is(enumeration value_)     const { return value_ == value; }    \
};
//_DECLARE_DESCRIPTIVE_ENUM_COWL_______________________________________________/
#endif

