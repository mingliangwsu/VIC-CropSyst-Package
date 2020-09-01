#ifndef structure_factory_GH
#define structure_factory_GH
#include "format/structural.h"
#include "format/structure_factory_I.h"
namespace structural
{
//______________________________________________________________________________
class Factory_generic               // rename to Factory_generic
: public implements_ Factory
{

 protected:
   //CORN::Unidirectional_list map_stack;
   mutable Map *map_stack[128]; // arbitrary size
      /* the document may use any indentation spacing convention
         and each indentation level may have its own local number of spaces,
         it is only required that specing at each level is consistent.
      */
/*
   mutable int8 map_count; // will correspond to indentation level.
      // The items on the stack are pointers to elements of the document_map
*/
   mutable int8 map_index; // will correspond to indentation level.
      // The items on the stack are pointers to elements of the document_map
   mutable Map *document_map;
   mutable Map *current_map;
 public:
   Factory_generic(Map *document_map_);
 public:
   virtual Construct *render
      (const char *type_tag = 0);
      // The tag is the type of structure to be rendered
      // tag may have introducer (I.e. str or YAML !!str)
      // Derived factories may have more specific types, and thus would
      // override this method.
   virtual Pair_key_value *render_key_value_compact
      (const char *key_compact
      ,const char *type_tag = 0);
      // The key may be simple xxx  or compact xxx:yyy
      // The tag is the type of structure to be rendered

   virtual Pair_key_value &provide_key_value_compact
      (const char *key_compact
      ,const char *type_tag = 0);
      // The key may be simple xxx  or compact xxx:yyy
      // The tag is the type of structure to be rendered

   virtual Pair_key_value &provide_key_value_scalar
      (const char *key
      ,const char *type_tag = 0);
      // The key may be simple xxx  or compact xxx:yyy
      // The tag is the type of structure to be rendered

   virtual bool expect_map
      (const char *compact_key // with colon full qualification without is imposed structure
      ,const char *type_tag=0)                                     expectation_;
   virtual bool expect_map_end()                                   expectation_;
   virtual bool expect(const char *compact,std::string &as_str)    expectation_;
   virtual bool expect(const char *compact,char    &as_char)       expectation_;
   virtual bool expect(const char *compact,bool    &as_bool)       expectation_;
   virtual bool expect(const char *compact,nat8    &as_nat8)       expectation_;
   virtual bool expect(const char *compact,nat16   &as_nat16)      expectation_;
   virtual bool expect(const char *compact,nat32   &as_nat32)      expectation_;
   virtual bool expect(const char *compact,int8    &as_int8)       expectation_;
   virtual bool expect(const char *compact,int16   &as_int16)      expectation_;
   virtual bool expect(const char *compact,int32   &as_int32)      expectation_;
   virtual bool expect(const char *compact,float32 &as_float32)    expectation_;
   virtual bool expect(const char *compact,float64 &as_float64)    expectation_;
   virtual bool expect(const char *compact,Enum_abstract &as_enum) expectation_;

   template <typename Scalar_T, typename Value_T>
   bool expect_T(const char *compact,Value_T &as_T)
   {
      Pair_key_value *key_value_pair = provide_map_scalar(compact);
      Scalar_T *scalar = new Scalar_T(as_T);
      return key_value_pair->take_value(scalar) != 0;
   }

   template <typename Scalar_T, typename Value_N>
   bool expect_N(const char *compact,Value_N &as_N)
   {  //numeric
      Pair_key_value *key_value_pair = provide_map_scalar(compact);
      Scalar_number_clad *scalar = new Scalar_number_clad();
      scalar->associate_value<Value_N>(as_N);
      return key_value_pair->take_value(scalar) != 0;
   }
 private:
   Pair_key_value *provide_map_scalar(const char *compact) provision_;
 private:
   inline Factory_generic *check_abstract()
      { return new Factory_generic(document_map); }
};
//_Factory_generic_________________________________________________2020-03-15_/
}//_namespace_structural_______________________________________________________/
#endif

