#include "soil/chemical_balance.h"
#include <iostream>
#include "rptoptns.h"
#include <math.h>
#ifndef compareHPP
#  include "corn/math/compare.hpp"
#endif
#include "static_phrases.h"
#include "corn/measure/measures.h"
//_____________________________________________________________________________/
Chemical_balance_components::Chemical_balance_components
(const std::string  &chemical_name_
,float64             original_profile_content_)
:chemical_name                                                  (chemical_name_) //041001
,original_profile_content                            (original_profile_content_)
,transformed_to                                                              (0)
,transformed_from                                                            (0)
,applied_irrig_soil                                                          (0)
,applied                                                                     (0)
,balance_leached                                                             (0)
,reported_leached                                                            (0)
,uptake                                                                      (0)
,residue                                                                     (0)
,current_profile_content                                                     (0)
,recalibration                                                               (0)
,pending_infiltration                                                        (0)
,balance_error                                                               (0)
{}
//_Chemical_balance_components______________________________________2019-09-09_/
Chemical_balance_accumulators_molecular::Chemical_balance_accumulators_molecular
(const std::string  &chemical_name_                                              //040931
,float64             original_profile_content_M_
,float64             molecular_to_elemental_)
: Chemical_balance_components(chemical_name_,original_profile_content_M_)
,molecular_to_elemental                                (molecular_to_elemental_)
{}
//_Chemical_balance_accumulators:constructor___________________________________/
float64 Chemical_balance_accumulators_molecular::balance
(float64 current_profile_content_M_)
{
   current_profile_content = current_profile_content_M_;
   balance_error =
         + original_profile_content
         + transformed_from
         + applied_irrig_soil
         + residue
         - pending_infiltration                                                  //071016
         - recalibration                                                         //011115
         - current_profile_content                                               //981006
         - transformed_to
         - balance_leached
         - uptake;
   float64 acceptable_error =                                                    //991205
      CORN::must_be_greater_or_equal_to<float64>
      (0.5 *         //110825  was 0.05
       (+ original_profile_content + transformed_from),0.00001);                 //990311

//std::cerr << std::endl << chemical_name << " balance error:" << balance_error << std::endl;

   if (fabs(balance_error) < acceptable_error)                                   //990311
      balance_error = 0.0;                                                       //990311

/*170503  This is temporarily disabled because not accomodating new receiver idiom

if (!CORN::is_zero<float64>(balance_error,0.01))
{
std::cerr << std::endl << chemical_name << " balance error:" << balance_error
<< " acceptable:" << acceptable_error
<< " chemical load:" << (+ original_profile_content_M + transformed_from_M)
<< std::endl;
std::cerr
<< "OPC:" << original_profile_content_M << std::endl
<< "TRf:" << transformed_from_M << std::endl
<< "app:" << applied_irrig_soil_M << std::endl
<< "res:" << residue_M << std::endl
<< "PIn:" << pending_infiltration << std::endl
<< "rec:" << recalibration_M << std::endl
<< "CPC:" << current_profile_content_M << std::endl
<< "TRt:" << transformed_to_M <<std::endl
<< "BlL:" << balance_leached_M <<std::endl
<< "upt:" << uptake_M << std::endl;
return balance_error;
}
*/
//if (chemical_name == "salt")
//cout << balance_error << endl;

   return balance_error;
}
//_balance_____________________________________________________________________/



