#ifndef structuralH
#define structuralH

#include "corn/parameters/number_keyed.h"
#include "corn/container/unilist.h"
#include "corn/string/strconv.hpp"
#include "corn/labeled_bitmask.h"
#include "corn/container/text_list.h"
#include "corn/enum.h"
#include "corn/data_type_enum.h"
#include <assert.h>
#ifdef __GNUC__
#include <typeinfo>
#endif
//______________________________________________________________________________
namespace structural
{
//______________________________________________________________________________
class Pair_key_value;
class Pair_key_map;
class Construct;
Construct *render_construct_raw(const char *type_tag);
// May be prefixed with !!
//_forward_declarations________________________________________________________/
interface_ Construct
: public extends_interface_ CORN::Item
{
   CORN::Unidirectional_list comment;                                            //200218
   virtual Pair_key_value *provide_key_value_pair
      (given_ Construct *key_given
      //200623 ,const std::string *type_tag=0
      ,const std::string &type_tag
      //200623       ,const char *type_tag = 0
      )
      { return 0; }
      // I think node is abstract class so this should never get called.

   //200621 inline virtual const wchar_t *get_text_wstr()              const { return 0;}
/*200621 replace with append_to
   inline virtual const wchar_t *get_text(std::wstring &buffer)            const
      { return buffer.c_str(); }
   inline virtual const char *get_text(std::string &buffer)                const
      { return buffer.c_str(); }
*/
   inline virtual bool set_text(const wchar_t *)  modification_ { return false;}
   inline virtual bool set_text(const std::wstring &text_) modification_
                                              { return set_text(text_.c_str());}
   virtual CORN::Container &provide_comment()                { return comment; }
      // list of Item_wstring (may be multiline!)
 public: // Item implementations
   inline virtual bool is_case_sensitive()                 const { return true;}
   inline virtual bool is_scalar()                  affirmation_ { return true;}
   inline virtual bool resolved()                   affirmation_ { return true;}
   virtual Construct *render_construct
      (const std::string &type_tag)
      //200623(const std::string *type_tag)
      rendition_
      { return render_construct_raw(type_tag.c_str());}
      //200623 { return render_construct_raw(type_tag ? type_tag->c_str() : 0);}
};
//_Construct___________________________________________________________________/
class Construct_arbitrary
: public implements_ Construct
{
 public:
   std::wstring arbitrary;
 public:
   inline virtual bool resolved()                   affirmation_ {return false;}
   //200621 inline virtual const wchar_t *get_text_wstr()const{return arbitrary.c_str();}
   inline virtual const wchar_t *append_to(std::wstring &buffer)           const
      {  buffer += arbitrary;
         return buffer.c_str();
      }
   inline virtual const char *append_to(std::string &buffer)               const
      {  CORN::append_wstring_to_string(arbitrary,buffer);
         return buffer.c_str();
      }
   inline virtual bool set_text(const wchar_t *raw)                modification_
      { arbitrary = raw; return true;}
};
//_Construct_arbitrary_________________________________________________________/
class Scalar_string_cowl
: public implements_ Construct
{  // Simple key as text
 protected:
   bool as_Unicode;
   union
   {  std::string *ASCII;
      // Note that this could be an Text_list::Item
      std::wstring *Unicode;
   } referenced;
 public:
   inline Scalar_string_cowl(std::string &text_ASCII)
      : Construct()
      , as_Unicode(false)
      { referenced.ASCII = &text_ASCII;
      }
   inline Scalar_string_cowl(std::wstring &text_Unicode_given)
      : Construct()
      , as_Unicode(true)
      {  referenced.Unicode = &text_Unicode_given;
      }
   inline virtual const char *append_to(std::string &buffer)               const
      {  if (as_Unicode)
            CORN::append_wstring_to_string(*(referenced.Unicode),buffer);
         else buffer += *(referenced.ASCII);
         return buffer.c_str();
      }
   inline virtual const wchar_t *append_to(std::wstring &buffer)   const
      {  if (as_Unicode)
            buffer += *(referenced.Unicode);
         else CORN::string_to_wstring(*(referenced.ASCII),buffer);
         return buffer.c_str();
      }
/* replaced
   inline virtual const std::string &append_to_string(std::string &buffer) const
      {  if (as_Unicode)
            CORN::append_wstring_to_string(*(referenced.Unicode),buffer);
         else buffer += *(referenced.ASCII);
         return buffer;
      }
   inline virtual const std::wstring &append_to_wstring(std::wstring &buffer)const
      {  if (as_Unicode)
            buffer += *(referenced.Unicode);
         else CORN::string_to_wstring(*(referenced.ASCII),buffer);
         return buffer;
      }

*/
   inline virtual bool is_scalar()                  affirmation_ { return true;}
};
//_Scalar_string____________________________________________________2020-03-15_/
class Scalar_string_clad
: public extends_ Scalar_string_cowl
{  // Simple key as text
 protected:
   union
   {  std::string *ASCII;
      // Note that this could be an Text_list::Item
      std::wstring *Unicode;
   } owned; //owned
 public:
   inline Scalar_string_clad(std::string *text_ASCII_given)
      : Scalar_string_cowl(*text_ASCII_given)
      {  owned.ASCII = text_ASCII_given;
      }
   inline Scalar_string_clad(std::wstring *text_Unicode_given)
      : Scalar_string_cowl(*text_Unicode_given)
      {  owned.Unicode = text_Unicode_given;
      }
   inline Scalar_string_clad(const std::string &text_ASCII)
      : Scalar_string_cowl(*(owned.ASCII = new std::string(text_ASCII)))
      {}
   inline Scalar_string_clad(const std::wstring &text_Unicode)
      : Scalar_string_cowl(*(owned.Unicode = new std::wstring(text_Unicode)))
      {}
   inline Scalar_string_clad()
      : Scalar_string_cowl(*(owned.ASCII=new std::string))
      {}
   inline virtual ~Scalar_string_clad()
      {  if (as_Unicode)   delete owned.Unicode;
         else              delete owned.ASCII;
      }
};
//_Scalar_string____________________________________________________2020-03-15_/
#define Key_string Scalar_string_clad
//__________________________________________________________________2020-03-15_/
class Scalar_bool_cowl
: public implements_ Construct
{
   bool &value_ref;
//   Construct *key_given;             // owned by this
 public:
   Scalar_bool_cowl(bool &value_ref_);
  // inline virtual bool is_value_set_with_remaining_text()        affirmation_{ return true; };
 public: // Pair_key_value implementations
   virtual bool set_text
      (const wchar_t *value_as_wstr)                              modification_;
   implmt virtual bool  write(std::ostream &)                        stream_IO_;
   implmt virtual const char *append_to(std::string &buffer)              const;

};
//_Scalar_bool_cowl_________________________________________________2020-03-15_/
class Scalar_bool_clad
: public extends_ Scalar_bool_cowl
{
   bool value;
 public:
   Scalar_bool_clad()
      : Scalar_bool_cowl(value)
      {}
  // inline virtual bool is_value_set_with_remaining_text()        affirmation_{ return true; };
};
//_Scalar_bool_clad_________________________________________________2020-03-15_/
class Scalar_number_clad
: public implements_ Construct
{
   CORN::Numeric_type mode;    // Not only the primitive numeric (and date time types are considered here)
   union
   {
      nat8    *as_nat8;
      nat16   *as_nat16;
      nat32   *as_nat32;     // Time32 times may also be stored as nat32
      int8    *as_int8;
      int16   *as_int16;
      int32   *as_int32;      // Date32 dates may also be stored as int32
      float32 *as_float32;
      float64 *as_float64;    // time and datetime are stored as float64

      void    *as_any_number;                                                    //170305
      /*NYN
      const nat8    *as_const_nat8;
      const nat16   *as_const_nat16;
      const nat32   *as_const_nat32;     // Time32 times may also be stored as nat32
      const int8    *as_const_int8;
      const int16   *as_const_int16;
      const int32   *as_const_int32;      // Date32 dates may also be stored as int32
      const float32 *as_const_float32;
      const float64 *as_const_float64;    // time and datetime are stored as float64
      */

      //CORN::Date         *as_date;
      //CORN::Time         *as_time;
      //CORN::Date_time_64 *as_date_time;
   } value;
 public:
   /*implmt*/ virtual bool  write(std::ostream &)                        stream_IO_;
   inline Scalar_number_clad(CORN::Numeric_type mode_ = CORN::NT_float32)
      : mode(mode_)
      , value()
      {
         value.as_any_number = 0;
      }
/*
   template <typename Value_T>
   Scalar_number(Value_T  &value_)
      {
         value.as_any_number = (void *)(&value_);
      }
*/
   template <typename Value_T>
   bool associate_value(Value_T &value_)
      {
         value.as_any_number = (void *)(&value_);
         return true;
      }
   virtual bool set_text(const wchar_t *value_as_wstr)            modification_;
   float64 set_value_as_float64(float64 new_value)                modification_;
   const char *append_value_in_radix_to_string
      (std::string &buffer,nat8 precision,nat8 radix)                     const;
   virtual const char *append_to(std::string &buffer)                     const;
};
//_Scalar_number____________________________________________________2020-03-15_/
class Scalar_enum_cowl
: public implements_ Construct
{
   Enum_abstract &labeled_enum; // rename to enum_ref
 public:
   inline Scalar_enum_cowl(Enum_abstract &enum_ref_)
      :  labeled_enum(enum_ref_)
      {}
   virtual bool set_text(const wchar_t *value_as_wstr)            modification_;
   implmt virtual bool  write(std::ostream &)                        stream_IO_;
   //NYN virtual const char *append_to(std::string &buffer)               const;
};
//_Scalar_enum_cowl_________________________________________________2020-03-15_/
interface_ Collection
: public extends_interface_ Construct
{
 public:
   virtual nat32 count()                                                const=0;
   inline virtual bool is_scalar()                 affirmation_ { return false;}
   virtual CORN::Container &get_items()                                      =0;
};
//_Collection__________________________________________________________________/
class Collection_clad  // actually clad or cowl
: public extends_interface_ Collection
{
 private:
   mutable CORN::Container *contents; bool contents_owned;
      // this of constructs
 public:
   mutable CORN::Container &items; // reference to contents
 public:
   inline Collection_clad()
      : contents(new CORN::Unidirectional_list)
      , contents_owned(true)
      , items(*contents)
      {}
   inline Collection_clad(modifiable_ CORN::Container &items_)
      : contents(&items_)
      , contents_owned(false)
      , items(items_)
      {}
   virtual inline ~Collection_clad()
      {  if (contents_owned)
            delete contents;
      }
   inline virtual nat32 count()                                            const
      { return items.count(); }
   inline virtual CORN::Container &get_items()                   {return items;}

};
//_Collection_clad__________________________________________________2017-02-12_/

interface_ Sequence
: public extends_interface_ Collection  // of Constructs
{
/*abandoned
   class Item
      :  public extends_interface_ Construct
      {};
*/
 public:
   virtual nat32 append_text_list(CORN::Text_list &values)     appropriation_=0;
      // Simple assignment of values

   virtual Construct *render_item
      (const std::string &type_tag)
      //200623          (const char *type_tag)
         rendition_=0;
      // The type tag is optional.
      // It can be used in drived classes to
};
//_Sequence____________________________________________________________________/
// There would be no sequence cowl because this is actually clad or cowl rename this
class Sequence_clad
: public extends_ Collection_clad                                                //200218
, public implements_ Sequence                                                    //200218
{
 public:
   inline Sequence_clad()
      : Collection_clad()
      {}
   inline Sequence_clad(modifiable_ CORN::Container &items_)
      : Collection_clad(items_)
      {}
 public:
   virtual nat32 append_text_list(CORN::Text_list &values)       appropriation_;
   virtual Construct *render_item
      (const std::string &type_tag) // may be empty when type no known yet
      //200623 (const char *type_tag)
      rendition_;
      // could actually be implemented in Collection_clad
      // but sequences really are the only constructs with items.

};
//_Sequence_clad____________________________________________________2020-06-19_/
class Pair_key_value
: public implements_ Construct
{
 protected:
   structural::Construct *specifier; // owned  (key)
   structural::Construct *value; //owned
 public:
   Pair_key_value
      (structural::Construct *specifier_given=0
      ,structural::Construct *value_given=0);
   inline virtual ~Pair_key_value()
      { delete specifier; }
 public: // construct implementation
   virtual bool  write(std::ostream &strm)                         performs_IO_;

 public:
   inline virtual const Construct *get_specifier()                         const
      { return specifier; }
   implmt virtual bool is_key_string(const std::string &key)       affirmation_;
   inline virtual       Construct *get_value()                   {return value;}
   inline virtual       Construct &provide_value()        {return *get_value();}
   implmt virtual const Construct &ref_value()            const {return *value;}
   implmt virtual bool set_value_wstring
      (const std::wstring &value_as_wstring)                      modification_;
   implmt virtual bool set_value_wchr
      (const wchar_t *value_as_wstring)                           modification_;
   implmt virtual int compare(const CORN::Item &key_to_compare)           const;
   implmt virtual bool /*const structural::Construct* */ take_value
      (structural::Construct *value_given)                       appropriation_;
   /*200630 obsolete
   inline virtual bool is_value_set_with_remaining_text()
      affirmation_                                               {return false;}
   */
   inline virtual bool is_scalar()                  affirmation_ {return false;}
};
//_Pair_key_value______________________________________________________________/
interface_ Map // was mapping Mapping  // Actually just Map
: public extends_interface_ Collection // of Pair_key_value
{
 public:
   inline virtual ~Map() { }
   virtual modifiable_ CORN::Container &get_key_value_pairs()           const=0; //151018
   virtual modifiable_  Pair_key_value *find_key_value_pair
      (const structural::Construct &key_construct)                      const=0;
   virtual bool take(Pair_key_value *pair_KV)                  appropriation_=0;
};
//_Map_____________________________________________________________________/
class Map_clad
: public extends_interface_ structural::Map
, public extends_ Collection_clad
{protected:
   mutable CORN::Unidirectional_list key_value_pairs;
 public:
   inline Map_clad()
      : Map()
      , Collection_clad()
      {}
   inline Map_clad
      (modifiable_ CORN::Container &items_)
      : Collection_clad(items)
      {}
   inline virtual ~Map_clad() { /* specifier not owned dont delete */ }
 public: // Item implementations
   inline virtual modifiable_ CORN::Container &get_key_value_pairs()       const //151018
      { return key_value_pairs;}
   virtual modifiable_  Pair_key_value *find_key_value_pair
      (const structural::Construct &key_construct)                        const;
   virtual Pair_key_value *provide_key_value_pair                                //200619
      (given_ Construct *key_given
      ,const std::string &type_tag
      //200623 ,const std::string *type_tag=0
      );
   virtual int compare(const CORN::Item &mapping_to_compare)              const; //151018
   inline virtual nat32 count()         const { return key_value_pairs.count();}
   virtual bool  write(std::ostream &strm)                         performs_IO_;
   inline virtual Construct *get_value()                         { return this;}
   inline virtual bool take(Pair_key_value *pair_KV)              appropriation_
                                  { key_value_pairs.take(pair_KV); return true;}
 public: // parameter providers
   template <typename KV_pair_scalar_T, typename Key_stringT, typename Value_T>
   KV_pair_scalar_T &provide_KV_pair_scalar(const Key_stringT key,Value_T &value) provision_
      {  KV_pair_scalar_T *specific_KV_pair = 0;
         std::wstring *key_wstring = new std::wstring;
         if      (typeid(key) == typeid(std::wstring))key_wstring->assign(key);
         else if (typeid(key) == typeid(std::string)) CORN::string_to_wstring(key,key_wstring);
         else if (typeid(key) == typeid(wchar_t *) )   key_wstring = key;
         else if (typeid(key) == typeid(char *) )      CORN::ASCIIZ_to_wstring(key,key_wstring);
         Key_string *key_construct = new Key_string(key_wstring);
         Pair_key_value *KV_pair = find_key_value_pair(*key_construct);
         if (KV_pair)
         {
            specific_KV_pair = dynamic_cast<KV_pair_scalar_T *>(KV_pair);
            delete key;
         } else
         {  specific_KV_pair = new KV_pair_scalar_T(key_construct,value);
         }
         assert (specific_KV_pair);
         return *specific_KV_pair;
      }
};
//_Mapping_abstract_________________________________________________2015-10-18_/
class Pair_key_map
: public extends_ Pair_key_value
{public:
   Map *map;
 protected:
   // already in Pair_key_value_abstract   structural::Construct *ref_specifier;
      // not owned (owned by key value pair of which this mapping is the value).
      // may be 0 if uppermost structure
      // usually a Key_string for CS_Documents.
      // Note that the specifier for a document could be the
      // path/URL of the document.
 public:
   Pair_key_map
      (structural::Construct *specifier_given = 0
      ,Map *map_given = 0);
   virtual bool  write(std::ostream &strm)                         performs_IO_;
};
//_Pair_key_map_____________________________________________________2020-03-03_/
}//_namespace_structural_______________________________________________________/
#endif

