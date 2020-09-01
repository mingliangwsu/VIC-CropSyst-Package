#ifndef units_enumH
#define units_enumH
#include "corn/enum.h"
#include "corn/measure/units.h"
namespace CORN {
//______________________________________________________________________________
class Labeled_units_enum // Rename?
: public Enum_abstract
{
public:  // smart_units_code is temporarily public so we can access for UI it in data records,
   mutable Units_clad   units_clad;                                              //190105_111206
/*190105 functions now take buffer arg
private: // return buffer
   mutable std::string return_buffer;                                            //170423
*/
public: // Constructors
   Labeled_units_enum();
      // Note that this class does not use a fixed label table
   Labeled_units_enum(Units_code _units);
   virtual ~Labeled_units_enum();                                                //130515
public: // Labeled_enum overrides
   //190109 virtual const char *get_label(std::string &return_buffer)              const; //190105
   virtual const char *append_label(std::string &append_to)               const; //190109

   inline Units_code set(Units_code _code)                                       //060627
      { units_clad.set(_code); return _code;}
   inline Units_code get()                                                const  //060627
      { return units_clad.get(); }
      // Returns the units code abbreviation.
   virtual int32 set_label(const char *_label);
      // Sets the units based on the abbreviation
   virtual int32 get_int32()                                              const;
      // Returns the units code caste to a 32bit integer.
   virtual int32 set_int32(int32 _value) ;
      // This sets the value (casting)
      // This must be overridden derived classes
   virtual bool is_label(const char *_label)                       affirmation_;
      // return true if the label is in the label table
      // (used by CropSyst Crop parameter editor.
   //190109 virtual const char *get_description(std::string &return_buffer)        const;//050929
   virtual const char *append_description(std::string &return_buffer)     const; //190109

      // implement

      // Returns the character string description associated with the current
      // enumeration value.
      // If it is a simple label table, the label is simply returned.
   virtual bool is_descriptive()                                          const;//050929
      // Returns true if this is a descriptive enumeration
   virtual uint16 get_max_label_length()                                  const;//050929
      // Returns the longest label length.
      // This is used to determine the field size for Data_records enumeration attributes.
};
//_Labeled_units_enum_______________________________________________2006-06-26_/
}//_namespace_CORN_____________________________________________________________/
#endif

