#ifndef structure_factory_IH
#define structure_factory_IH
#include "corn/format/structural.h"

#define L_COMPACT_SEPARATOR L'.'
#define COMPACT_SEPARATOR '.'
//______________________________________________________________________________
namespace structural
{
template <typename Char_T>
struct Compact_valuation
{  const Char_T *compact;
   const Char_T *value;
};
// This is used for setting default values

/*
Compact key has two forms
- qualified
   Ie.  "xxx.yyy.zzz"

- indented
   I.e. "  zzz"
   indentation is single space so the number of spaces represent the
   number of sublevels (submaps)
   Where where "  zzz"
   would have previously had summitted "xxx" and " yyy"
   or "xxx.yyy"

The compact keys allow one to describe simple structures with basic keys.
This is not as powerful as one could express the complex structure
with complex key constructs afforded by YAML for example.
*/
//______________________________________________________________________________
/*
   The factory allows a program to associated compact structure
   associated with data (in memory).

   Generally a structural factory would be implemented from
   Factory_abstract and this would be associated with an object class.

   [ Consider a generator with takes a schema (or model domain)
     and generates automatically a factor (and class?)
   ]

*/
interface_ Factory
{
 public:
   virtual Construct *render
      (const char *type_tag = 0)                                             =0;
      // The tag is the type of structure to be rendered
      // tag does not have introducer (I.e. str not !!str)

   virtual Pair_key_value *render_key_value_compact
      (const char *key_compact
      ,const char *type_tag = 0)                                             =0;
       // The key may be compact xxx:yyy or ultimate sclar xxx
      // The tag is the type of structure to be rendered

   virtual Pair_key_value &provide_key_value_compact
      (const char *key_compact
      ,const char *type_tag = 0)                                             =0;
       // The key may be compact xxx:yyy or ultimate sclar xxx
      // The tag is the type of structure to be rendered

   virtual Pair_key_value &provide_key_value_scalar
      (const char *key_unqual
      ,const char *type_tag = 0)                                             =0;
       // The key may be simple xxx
      // The tag is the type of structure to be rendered

/*
   virtual Pair_key_value &provide_key_value
      (const char *key_compact
      ,const char *type_tag = 0)                                             =0;
       // The key may be simple xxx  or compact xxx:yyy
      // The tag is the type of structure to be rendered
*/
   virtual bool expect_map
      (const char *compact
      ,const char *type_tag=0)                                   expectation_=0;
   virtual bool expect(const char *compact,std::string &as_str)  expectation_=0;
   virtual bool expect(const char *compact,char    &as_char)     expectation_=0;
   virtual bool expect(const char *compact,bool    &as_bool)     expectation_=0;
   virtual bool expect(const char *compact,nat8    &as_nat8)     expectation_=0;
   virtual bool expect(const char *compact,nat16   &as_nat16)    expectation_=0;
   virtual bool expect(const char *compact,nat32   &as_nat32)    expectation_=0;
   virtual bool expect(const char *compact,int8    &as_int8)     expectation_=0;
   virtual bool expect(const char *compact,int16   &as_int16)    expectation_=0;
   virtual bool expect(const char *compact,int32   &as_int32)    expectation_=0;
   virtual bool expect(const char *compact,float32 &as_float32)  expectation_=0;
   virtual bool expect(const char *compact,float64 &as_float64)  expectation_=0;
   virtual bool expect(const char *compact,Enum_abstract&as_enum)expectation_=0;
};
//_Structure_factory________________________________________________2020-03-15_/
}//_namespace_structural_______________________________________________________/
#endif

