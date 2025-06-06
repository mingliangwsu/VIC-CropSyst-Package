//______________________________________________________________________________
#include "biomatter_abstract.h"
#include "corn/math/compare.hpp"
#include "corn/measure/unitconv.h"
#include "environment/situation.h"
#include "corn/parameters/parameter.h"
#include "CS_suite/observation/CS_inspector.h"
//______________________________________________________________________________
Biomatter_abstract::Biomatter_abstract(const Biomatter_abstract &from_copy)
: Biomass_abstract(from_copy)
, water(from_copy.water)                                                         //100905
, volume_m3(from_copy.get_volume_m3())                                           //100905
, pH(7.5)   // 7.5 because Nicole is usingthis throughout her model.             //100901
{}
//_Biomatter_abstract:copy_constructor_________________________________________/
Biomatter_abstract::Biomatter_abstract(const Biomatter &from_copy)
: Biomass_abstract(dynamic_cast<const Biomass&>(from_copy))
, water(from_copy.get_water())                                                   //110630
, volume_m3(from_copy.get_volume_m3())                                           //100905
, pH(7.5)   // 7.5 because Nicole is usingthis throughout her model.             //100901
{  // It is possible that
}
//_Biomatter_abstract:copy_constructor______________________________2001-03-03_/
Biomatter_abstract::Biomatter_abstract
(float64 _carbon_fraction                                                        //070726
,float64 _carbon_nitrogen_ratio)                                                 //070726
: Biomass_abstract
   (0.0,_carbon_fraction,_carbon_nitrogen_ratio,0.0)                             //090807
   // The carbon fraction is most plants is about 0.5 (Carbon inventory methods By N. H. Ravindranath, Madelene Ostwald) //090807
, pH(7.5)   // Nicole is using 7.5 throughout her model.                         //100901
, water  (0.0,UC_kg_mass,0.0,UC_Celcius)                                         //110630
, volume_m3 (0.0)                                                                //100905
{}
//_Biomatter_abstract:constructor___________________________________2006-03-16_/
Biomatter_abstract *Biomatter_abstract::clone()                            const
{  return new Biomatter_abstract(*this);
}
//_clone____________________________________________________________2010-08-18_/
bool Biomatter_abstract::clear()                                   modification_
{  Biomass_abstract::clear();
   volume_m3 = 0.0;                                                              //100906
   water.clear();                                                                //110630_100906
   return true;                                                                  //160620
}
//_clear____________________________________________________________2007-07-26_/
float64 Biomatter_abstract::set_moisture_percent
(float64 new_moisture)                                             modification_
{
   water.set_kg((new_moisture * dry_kg()) / (1.0 - new_moisture));               //150316_150121_110730_RLN
   return new_moisture;
}
//_set_moisture_percent_____________________________________________2009-07-12_/
float64 Biomatter_abstract
::set_mass_wet_at_percent_moisture
(float64 new_mass_wet
,CORN::Units_code new_units                                                      //100930
,float64 at_moisture_percent)                                      modification_
{  // NYI - why is dry weight not set for dry mass
   float64 dry_weight = new_mass_wet * (100.0 - at_moisture_percent) / 100.0;
   float64 new_water  = new_mass_wet - dry_weight;
   water.update_value_with_units(new_water,new_units);                           //150121_110730
   return new_mass_wet;
}
//_set_mass_wet_at_percent_moisture_________________________________2010-09-30_/
float64 Biomatter_abstract::multiply_by(float64 multiplier)        modification_
{  volume_m3 *= multiplier;
   water.multiply_by(multiplier);
   return Biomass_abstract::multiply_by(multiplier)+ water;
}
//_multiply_by_________________________________________________________________/
Biomatter_abstract *Biomatter_abstract::extract_volume_m3(float64 volume_to_extract_m3) modification_
{  Biomatter_abstract *extraction = 0;
   float64 volume = get_volume_m3();
   if ((volume > 0.000001) && (volume_to_extract_m3 > 0.0))
   {  float64 fraction = volume_to_extract_m3 / volume;
      extraction = dynamic_cast<Biomatter_abstract *>(extract_fraction(fraction));
      extraction->set_pH(this->pH);                                              //160105BRC
   }
   return extraction;
}
//_extract_volume_m3________________________________________________2010-02-26_/
Biomatter *Biomatter_abstract::extract_mass_wet_kg(float64 mass_wet_to_extract)
{  Biomatter *extraction = 0;
   float64 fract_of_avail = 0.0;
   float64 mass_available = wet_kg();
   float64 act_extr_mass = CORN::must_be_less_or_equal_to<float64>(mass_wet_to_extract,mass_available);
   fract_of_avail = act_extr_mass/ mass_available;
   extraction = dynamic_cast<Biomatter *>(extract_fraction(fract_of_avail));
   return extraction;
}
//_extract_mass_wet_kg______________________________________________2003-02-27_/
float64 Biomatter_abstract::subtract_water_vol(float64 water_volume_m3)
{  // Amount subtracted is limited to available water.
   float64 water_mass_subtrahend_kg = water_volume_m3 * water_density_kg_m3;
//NYN    float64 water_mass_subtrahend= 0;
//NYN    convertor.convert(water_subtrahend_kg,UC_kg_mass,water_mass_subtrahend_kg,mass_units);
//050222       First take water avail associated with free water and urine
//050222       Then take fecal water
//             currently I am just taking free first then urine then fecal
//             But probably should take free and urine simultaneously.
   float64 water_mass_to_be_satisfied_kg = water_mass_subtrahend_kg;             //050228
   float64 taken_from_free_kg = std::min<float64>(water_mass_to_be_satisfied_kg  //100819_050228
      ,get_water().kg());                                                        //110727
   float64 taken_from_free_my_units = taken_from_free_kg;                        //100101
   if (get_units() != UC_kg_mass)                                                //100101
      CORN::units_convertor.convert(taken_from_free_kg,UC_kg_mass,taken_from_free_my_units,get_units());     //100101
   water.decrement_by(taken_from_free_my_units);                                 //150226_150215_110630_100101
   water_mass_to_be_satisfied_kg -= taken_from_free_kg;                          //050228
   float64 act_water_kg = taken_from_free_kg;
//NYN    convertor.convert(taken_from_free,mass_units,act_water_kg,UC_kg_mass);
   float64 act_vol =  act_water_kg / water_density_kg_m3;
   return act_vol;
}
//_subtract_water_vol_______________________________________________2002-08-15_/
float64 Biomatter_abstract::add_similar_biomatter(const Biomatter &addend) modification_
{  pH  = merge_intensive_property(pH,addend.get_pH(),addend.kg());
   volume_m3 += addend.get_volume_m3();
   water.add(addend.get_water());                                                //110727
   return Biomass_abstract::add_similar_mass(addend);
}
//_add_similar_biomatter____________________________________________2010-09-01_/
float64 Biomatter_abstract::add(const Physical::Mass &addend)                modification_
{  const Biomatter_abstract *addend_as_same_type = dynamic_cast<const Biomatter_abstract *>(&addend);
   if (addend_as_same_type)
   {  pH  = merge_intensive_property(pH,addend_as_same_type->get_pH(),addend_as_same_type->kg());
      volume_m3 += addend_as_same_type->get_volume_m3();
      water.add(addend_as_same_type->get_water());                               //110727
   }
   return Biomass_abstract::add(addend);
}
//_add______________________________________________________________2010-09-08_/
Situation &Biomatter_abstract::provide_situation()                    provision_
{  if (!situation)
   {  situation = new Situation;
        unsigned long ID_long=(unsigned long)this;
        situation->ID = ID_long&0xFFFF;
   }
   return *situation;
}
//_provide_situation________________________________________________2011-08-04_/
RENDER_INSPECTORS_DEFINITION(Biomatter_abstract)
{
   #ifdef CS_OBSERVATION
   inspectors.append(new CS::Inspector_scalar(water.datum,*context,"water_H2O",CORN::value_statistic));
   #endif
   return 0;
}
//_RENDER_INSPECTORS_DEFINITION(Biomatter_abstract)______________2015-10-20BRC_/

