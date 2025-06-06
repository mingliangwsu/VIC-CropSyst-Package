#include "CS_adjustment.h"
#include "corn/data_source/vv_file.h"
#include "CS_suite/observation/CS_examination.h"
namespace CS {
//____________________________________________________________________________
bool Adjustment_properties::expect_structure(bool for_write)
{
   bool expected = CORN::Data_record::expect_structure(for_write);
   set_current_section("adjustment");
      expect_string("alteration",alteration);
      expect_string("assignment",assignment_operator);

   set_current_section("domain");
      expect_units   ("units"    ,domain.units);
   set_current_section("codomain");
      expect_units   ("units"    ,codomain.units);
      expect_string  ("ontology" ,codomain.ontology);
      #ifdef USE_VAR_CODES
      expect_nat32   ("code"     ,codomain.UED_variable,16);
      #endif

   // the adjustment list items are currently take in get_end()
   // NYI set_current_section("assignments");

   return expected;
};
//_expect_structure_________________________________________________2018-10-15_/
/*
bool Adjustment_properties::get_end()
{

   assignments_section = set_current_section("assignments");
   FOR_EACH_IN(assignment_raw,VV_unexpected_entry,assignments_section->values,each_assignment_raw)
   {
      std::string assignment_abscissa = assignment_raw->variable;
      std::string assignment_ordinate = assignment_raw->value;

      switch (domain.units)
      {
         case : UT_year
         {

         } break;
         default: assert(false);
            // units not yet provided
         break;

      }

   get the assigments (unexpected section)
   and convert to assigment entry  // which might be interpolator
   } FOR_EACH_END(each_assignment_raw)

}
//_get_end__________________________________________________________2018-10-15_/
*/

Adjustment_properties::Adjustment_properties()
: CORN::Data_record                                               ("adjustment")
, alteration                                                      ("assignment")
, domain                                                                      ()
, codomain                                                                    ()
, adjustments                                                                 ()
, assignment_operator                                                      ("=")
{
/*moved to respective constructors
   domain.units            = UT_date;
   Codomain.unit           = UC_unitless;
   Codomain.ontology       = "";
   Codomain.UED_variable   = 0;
*/
}
//Adjustment_properties:constructor_________________________________2018-10-15_/
bool Adjustment_abstract::commit()
{
   assert(false); // NYI
   return true;
}
//_Adjustment_abstract::commit______________________________________2018-10-15_/
bool Adjustment_abstract::date_matches
(CORN::date32 simdate_raw
,CORN::date32 start_date_raw)
{
   /* check that the properties year (which may be relative) matches

   */
   assert(false); // NYI
   return true;
}
//_Adjustment_abstract::date_matches________________________________2018-10-15_/
bool Adjustment_abstract::year_matches
(CORN::Year this_year_raw
,CORN::Year start_year_raw)
{
   return (start_year_raw + temporal) == this_year_raw;
}
//_Adjustment_abstract::year_matches________________________________2018-10-15_/
}//_namespace_CS_______________________________________________________________/

