#ifndef chemical_balanceH
#define chemical_balanceH
#ifndef NEW_N_XFER_BAL

// In any case I would like to redo chemical balances with less overhead
// using inspector like idiom and be optional.

// This is broken now because
// transformed_from_M is not currently valid.
// Now using receiver idiom and that transfer was not accomodated.
// The N balance is currently only checked in N report

#ifndef primitiveH
#  include "corn/primitive.h"
#endif
//#ifdef VERSION2
//041001 This class will eventually be replaced
// currently I am first making the balance accumulators independent
// the chemicals will now store Chemical_balance_accumulators instead of Chemical_balance_reporter
// The Chemical_balance_reporter will be moved to CS_mod
#if (CS_VERSION==4)
#  include "rprtclss.h"
#endif
#include <string>
#include "corn/const.h"

//______________________________________________________________________________
struct Chemical_balance_components
{
   const std::string &chemical_name;
   // The name of the chemical

   float64 original_profile_content;
   float64 transformed_to;
      //{ NH4 -> NO3 }
      //{ NO3 -> denitrified NO3 }
      //{ pot min. N -> NH4 }
   float64 transformed_from;
   float64 applied_irrig_soil; //Chemical applied in concentration with irrigation actually going into soil
   float64 applied;          // total applied including volatilized (for output only)
   float64 balance_leached;
   float64 reported_leached;

   float64 uptake;
   float64 residue;  // From residue. For NH4 Includes input from manure

   //190909 unused float64 total_requirements;
   float64 current_profile_content;
   float64 recalibration;
   // In the case of salinity. When soil is innundated by water table, there is a change in salinity to the observed water table salinity.
   float64 balance_error;

   float64 pending_infiltration; // Surface applied chemicals may be deferred until following days.
                     // The deferment is not accumulated.
 public:
   Chemical_balance_components
      (const std::string  &chemical_name_
      ,float64             original_profile_content_);
};
//_Chemical_balance_elements________________________________________2019-09-09_/
class Chemical_balance_accumulators_molecular
: public extends_ Chemical_balance_components                                    //190909
{public:
   // Elements are // kg chem / m^2 molecular
 public:
   float64 molecular_to_elemental;
 public:
   Chemical_balance_accumulators_molecular
      (const std::string  &chemical_name_                                       //040931
      ,float64             original_profile_content_M_
      ,float64             molecular_to_elemental_);                             //040931
   inline virtual ~Chemical_balance_accumulators_molecular() {}                  //170217
   virtual float64 balance
      (float64 current_profile_content_M_);
   inline void inc_uptake(float64 uptake_to_add_M)
      {  uptake += uptake_to_add_M; }
   inline void inc_applied_irrig_soil(float64 appl_M)
      {  applied_irrig_soil += appl_M; }
   inline void inc_applied(float64 appl_M)
      { applied += appl_M; }
   inline void inc_balance_leached(float64 leached)                              //990317
      {  balance_leached += leached; }
   inline void inc_reported_leached(float64 leached)                             //990317
      {  reported_leached += leached; }
   inline void set_reported_leached()                                            //990317
      {  reported_leached = balance_leached; }
   inline void inc_transformed_to(float64 transformed_M)                         //990317
      {  transformed_to += transformed_M; }
   inline void inc_transformed_from(float64 transformed_M)                       //990317
      {  transformed_from += transformed_M; }
   inline void dec_residue(float64 amount_M)                                     //990317
      {  residue -= amount_M; }
   inline void recalibrate(float64 adjustment_amount_M)                          //011115
      {  recalibration += adjustment_amount_M; }
   inline float64 get_original_profile_content(bool elemental) const { return elemental ? original_profile_content * molecular_to_elemental : original_profile_content;} // kg chem / m^2 molecular
   inline float64 get_transformed_to         (bool elemental)  const { return elemental ? transformed_to           * molecular_to_elemental : transformed_to;}   //{kg chem / m^2 molecular }
                             //{ NH4 -> NO3 }
                             //{ NO3 -> denitrified NO3 }
                             //{ pot min. N -> NH4 }
   inline float64 get_transformed_from       (bool elemental)  const { return elemental ? transformed_from         * molecular_to_elemental : transformed_from;}  //{kg chem / m^2 molecular }
   inline float64 get_applied_irrig_soil     (bool elemental)  const { return elemental ? applied_irrig_soil       * molecular_to_elemental : applied_irrig_soil;} // kg chem/m2 Chemical applied in concentration with irrigation actually going into soil
   inline float64 get_applied                (bool elemental)  const { return elemental ? applied                  * molecular_to_elemental : applied;}          // total applied including volatilized (for output only)
   inline float64 get_balance_leached        (bool elemental)  const { return elemental ? balance_leached          * molecular_to_elemental : balance_leached;} //981230
   inline float64 get_reported_leached       (bool elemental)  const { return elemental ? reported_leached         * molecular_to_elemental : reported_leached;}//981230

   inline float64 get_uptake                 (bool elemental)  const { return elemental ? uptake                   * molecular_to_elemental : uptake;}
   inline float64 get_residue                (bool elemental)  const { return elemental ? residue                  * molecular_to_elemental : residue;}  // From residue. For NH4 Includes input from manure

   //190909 unused inline float64 get_total_requirements     (bool elemental)  const { return elemental ? total_requirements         * molecular_to_elemental : total_requirements;}
   inline float64 get_current_profile_content(bool elemental)  const { return elemental ? current_profile_content  * molecular_to_elemental : current_profile_content;}  //990317
   inline float64 get_recalibration          (bool elemental)  const { return elemental ? recalibration            * molecular_to_elemental : recalibration;}            //011115
   inline float64 get_balance_error          (bool elemental)  const { return elemental ? balance_error              * molecular_to_elemental : balance_error;}

   inline float64 know_pending_infiltration(float64 _pending_infiltration) { return pending_infiltration = _pending_infiltration; } //071214_071016 need to increment
};
//_Chemical_balance_accumulators_molecular_____________________________________/
#endif
#endif
//chem_bal.h

