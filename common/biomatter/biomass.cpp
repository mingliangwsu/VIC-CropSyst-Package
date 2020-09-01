#include "common/biomatter/biomass.h"
#include <algorithm>
#include "corn/measure/measures.h"
//______________________________________________________________________________
float64 Biomass::subtract_mass(float64 mass_dry_to_subtract)
{  return add_mass(-mass_dry_to_subtract);                                       //100303
}
//_subtract_mass_______________________________________________________________/
float64 Biomass::add_mass(float64 mass_dry_to_add)                 modification_
{  float64 orig_mass = kg();                                                     //100303
   float64 new_mass = orig_mass + mass_dry_to_add;                               //100303
   if (orig_mass != 0.0 )                                                        //100303
   {  float64 multiplier = (orig_mass != 0) ? new_mass / orig_mass : 1;          //100303
      return multiply_by(multiplier);                                            //100303
   }                                                                             //100303
   return orig_mass;                                                             //100303
}
//_add_mass____________________________________________________________________/
float64 Biomass::divide_by(float64 divisor)                        modification_
{  float64 new_mass = (divisor > 0.000001)
      ? multiply_by(1.0/divisor)
      : kg();
   return new_mass;
}
//_divide_by________________________________________________________2009-07-29_/
float64 Biomass::calc_concentration_mass_fraction(float64 component_mass)  const
{  float64 mass = kg();
   return is_empty() ? 0.0 : (component_mass/mass);
}
//_calc_concentration_mass_fraction_________________________________2010-08-20_/
float64 Biomass::calc_concentration_mass_percent(float64 component_mass)   const
{  return calc_concentration_mass_fraction(component_mass) * 100.0;
}
//_calc_concentration_mass_percent__________________________________2010-08-20_/
float64 Biomass::get_carbon_fraction()                                     const
{  float64 carbon_fraction =
      is_empty() ? 1.0 : get_carbon_kg() / kg();
   return carbon_fraction;
}
//_get_carbon_fraction_________________________________________________________/
