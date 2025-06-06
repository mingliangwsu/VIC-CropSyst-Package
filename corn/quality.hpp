#ifndef qualityHPP
#define qualityHPP
#include "corn/enum.h"
#include "corn/quality.h"
//______________________________________________________________________________
namespace CORN
{
//______________________________________________________________________________
interface_ Quality
{
 public:
   inline virtual ~Quality() {}                                                  //151204
   virtual inline bool is_measured()                               affirmation_
      { return get_quality_code() == measured_quality; };
   virtual inline bool is_valid()                                  affirmation_
      { return get_quality_code() < not_valid_quality; };
   virtual bool is_optimal()                                       affirmation_;
   virtual inline bool is_real()          affirmation_  { return is_optimal(); }
      ///< includes measures, calculated, interpolated, or other real
   virtual bool is_unknown()                                     affirmation_=0;
   inline virtual Quality_code invalidate(bool /*absolutely*/)    rectification_
      { return not_valid_quality; }
      ///< Sets the the quality to invalid
      ///< if absolutely is true the quality is forced to invalid
      ///< otherwise it is only invalidated if the value is not optimal
      ///< \return the set quality (if it was indeed invalidated)
   inline virtual Quality_code get_quality_code()                          const
      { return not_valid_quality; }
      ///< These two function are aliases to disabiguate
      ///< the getter when in classes derived from Physcial_property
   virtual const Quality &get_quality()                                 const=0;
   virtual       Quality &ref_quality()                                      =0;

   virtual const CORN::Quality &assume_code(CORN::Quality_code quality) rectification_ =0;
   virtual const CORN::Quality &assume     (const Quality     &quality) rectification_ =0;

   ///<\return the longest label length.
   ///< This is used to determine the field size for Data_records enumeration attributes.
   bool is_worse_than(const Quality      &that)                    affirmation_;
   bool is_worse_than(Quality_code that)                           affirmation_;
      ///<\return true if this has worse quality than that.
   bool is_better_than(const Quality      &that)                   affirmation_;
      ///<\return true if this has better quality than that.
   bool is_better_than( Quality_code that)                         affirmation_;
      ///<\return  true if this has better quality than that.
   bool is_same_or_better_than( Quality_code that)                 affirmation_;
   bool is_same_or_better_than(const Quality      &that)           affirmation_;
   const Quality        &worst(const Quality      &that)                  const;
   const Quality         &best(const Quality      &that)                  const;
      ///<\return  this or that which ever is the worst quality. 081024_//
};
//_Quality__________________________________________________________2015-01-27_/
class Quality_cowl
: public Enum_abstract
, public implements_ Quality                                                     //150128
{
protected:
    Quality_code  &ref_code;                                                     //150120
public:
   Quality_cowl (Quality_code  &_code);                                          //150120
      ///< includes measures, calculated, interpolated, or other real
   inline virtual ~Quality_cowl() {}                                             //151204
   virtual inline bool is_unknown()                                 affirmation_
      { return ref_code == unknown_quality; };
   virtual Quality_code invalidate(bool absolutely)              rectification_;
      ///< Sets the the quality to invalid
      ///< if absolutely is true the quality is forced to invalid
      ///< otherwise it is only invalidated if the value is not optimal
      ///< \return the set quality (if it was indeed invalidated)
public: // Labeled_enum overrides
   virtual int32 set_label(const char *_label);
      ///< This must be overridden to set the enumeration value for the
      ///< specified label.
   inline virtual int32 get_int32()             const {return (int32)ref_code; }
      ///< This will return (casting) the current value to 32bit integer.
      ///< This must be overridden derived classes
   inline virtual Quality_code get_quality_code()      const {return ref_code; } //150121
      ///< These two function are aliases to disabiguate
      ///< the getter when in classes derived from Physcial_property
   inline virtual const Quality &get_quality()           const { return *this; } //150121
   inline virtual       Quality &ref_quality()                 { return *this; } //150121
   virtual int32 set_int32(int32 _value);
      ///< This sets the value (casting)
      ///< This must be overridden derived classes
   virtual const CORN::Quality &assume_code(CORN::Quality_code quality) rectification_;
   virtual const CORN::Quality &assume     (const Quality &quality)     rectification_;

//NYI    bool is_label(const char *_label) const;
// Eventually override so we can check invalid and 'from' code (haven't decided on labels yet)
// so currently this only does the basic quality codes
   ///< \return true if the label is in the label table
   ///< (used by CropSyst Crop parameter editor.
   virtual const char *get_description()                                  const; //050929
   ///< \return the character string description associated with the current
   ///< enumeration value.
   ///< If it is a simple label table, the label is simply returned.
   inline virtual const char *get_label()                                  const
      { return get_description(); }
   ///<\return the character string label associated with the
   ///< current enumeration value.
   ///< This must be overridden to provide the label for the current enumeration
//NYI          Currently get_label() has not been needed
//             so simply calling get_description()
//             Eventually will want to provide a single word code for all qualities.
   inline virtual bool is_descriptive()                             affirmation_ //050929
      { return descriptive_label_table != 0; }
   ///<\return true if this is a descriptive enumeration
   virtual uint16 get_max_label_length()                                  const; //050929
   ///<\return the longest label length.
   ///< This is used to determine the field size for Data_records enumeration attributes.
};
//_Quality_cowl_____________________________________________________2005-11-29_/
extern Quality_code quality_calculated_from_quality   (const Quality &qual);
extern Quality_code quality_interpolated_from_quality (const Quality &qual);
extern Quality_code quality_estimated_from_quality    (const Quality &qual);
extern Quality_code quality_other_real_from_quality   (const Quality &qual);
extern Quality_code quality_forecasted_from_quality   (const Quality &qual);
extern Quality_code quality_generated_from_quality    (const Quality &qual);
//_quality_from_quality_____________________________________________2014-05-08_/
class Quality_clad
: public extends_ Quality_cowl
{
 private:
   Quality_code code;
 public:
   inline Quality_clad
      (Quality_code _initial_code = unknown_quality)
      : Quality_cowl(code)
      , code(_initial_code)                                                   {}
   inline Quality_clad
      (const Quality &copy_quality)
      : Quality_cowl(code)
      , code(copy_quality.get_quality_code())                                 {}
   inline virtual ~Quality_clad()                                             {} //151204
 private:
   inline Quality_clad *check_abstract()
                           { Quality_code test; return new Quality_clad(test); }
};
//_Quality_cowl_____________________________________________________2015-01-20_/
// I need also some other qualities, these could be a separate value
//    - not measured but presumed zero (I.e. solar radiation at night)
//    - impreceptiable (essentially 0)
}//_namespace_CORN_____________________________________________________________/

#define is_measured_quality(qual) (qual == CORN::observed_quality)
#define is_valid_quality(qual) (qual < CORN::not_valid_quality)
#define is_real_quality(qual) CORN::is_optimal_quality(qual)
#endif

