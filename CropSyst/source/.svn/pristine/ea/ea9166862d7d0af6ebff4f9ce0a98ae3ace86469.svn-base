// This unit should be renamed  OM_pool_single
#include <math.h>
#include "organic_matter/single_pool/OM_single_pool.h"
#include "soil/texture_I.h"
#include "soil/abiotic_environment.h"
#include "common/biomatter/organic_biomatter_balance.h"
#ifndef DETAILED_BALANCES
//130711 we dont bother with the detailed balance check
#   include "common/residue/residue_balancer.h"
#endif
#include "corn/math/compare.h"
#include "corn/measure/measures.h"

#include "soil/soil_I.h"
#include "soil/nitrogen_I.h"
#include "soil/structure_I.h"
extern const Soil::Soil_interface *soil_hack;
extern C_Balance *C_balance;                                                     //111005
//______________________________________________________________________________
Organic_biomatter_pool_single::Organic_biomatter_pool_single
(const Organic_biomatter_pool_single &from_copy)
: Organic_biomatter_pool_common(from_copy)                                       //110929
, decomposition                    (0)
, nitrogen_organic_kg(from_copy.nitrogen_organic_kg)                             //110005
{  // Warning: the following default copy copies the same receivers
   // but the copy constructor is generally used when moving OM
   // (usually residues) to different layers
   // in this case the receivers need to be for the other layer
   // so if the layer is going to be different, it is necessary
   // for the caller to call contribute_to()
   // For residues, created by Residues_V4_4, call setup_contribution()
   for (int i = 0; i <= 3; i++)
   {  receiver[i] = from_copy.receiver[i];
      C_fraction_transferred_to[i] = from_copy.C_fraction_transferred_to[i];
   }
}  // Copy constructor is used by Residues_V4_4::respond_to_field_operation
//_Organic_biomatter_pool_single:copy_constructor___________________2006-07-27_/
// 070707   check to see if this constructor is used.
// 070707   This did not set the carbon_fraction, C/N ratio, or decomposition_constant
//          Not sure how this would work with out.
//          I think this was just a place holder for accumulators
//          so I think can just use Organic_biomatter_common for the accumulators
Organic_biomatter_pool_single::Organic_biomatter_pool_single    // For stable provide
(Organic_matter_type                type_
,nat8                               layer_
,float64                           &contact_fraction_                            //190327_190215
,const Soil::Texture_interface     &soil_texture_                                //070707
,const Soil::Abiotic_environment_layer  &soil_environment_
)
:Organic_biomatter_pool_common
   (type_
   ,layer_
   ,OM_subsurface_position
   ,OM_INTRINSIC_CYCLING                                                         //111001
   ,UNKNOWN_RESIDUE_TYPE                                                         //111001
   ,0  //  biomass
   ,0  //  carbon fraction   // check that this is setup later!
   ,0  //  carbon_nitrogen_ratio    // check that this is setup later!
   ,0  //  decomposition_constant
   ,0.0  // detrition constant
   ,0.0                                                                          //111001
   ,contact_fraction_                                                            //111001
   ,soil_texture_
   ,soil_environment_)                                                           //070707
, decomposition            (0)
 , nitrogen_organic_kg(0)
{  for (int i = 0; i < 4; i++)
   {  C_fraction_transferred_to[i] = 0;
      receiver[i] = 0;
   }
/*//debug
if (carbon_nitrogen_ratio < 1)
{
         std::cerr << "This should not occur: "
         << organic_matter_type_label_table[type]
         << " init C/N < 1 (layer=" << int(layer) << ")="
         << carbon_nitrogen_ratio
         << std::endl;
}
*/
}
//_Organic_biomatter_pool_single:constructor___________________________________/
Organic_biomatter_pool_single::Organic_biomatter_pool_single    // For stable organic matter and microbial
(Organic_matter_type     type_
,nat8                    layer_
,float64                 biomass_
,float64                 carbon_fraction_
,float64                 carbon_nitrogen_ratio_
,float64                 decomposition_constant_
,float64                 area_per_mass_ // = 0.0  stubble and surface residue_only?
,float64                &contact_fraction_
,const Soil::Texture_interface &soil_texture_                                    //070707
,const Soil::Abiotic_environment_layer & soil_environment_
)
:Organic_biomatter_pool_common
(type_,layer_,OM_subsurface_position
,OM_INTRINSIC_CYCLING                                                            //111001
,UNKNOWN_RESIDUE_TYPE                                                            //111001
,biomass_,carbon_fraction_,carbon_nitrogen_ratio_,decomposition_constant_,0 /*only residues have detrition */
,area_per_mass_                                                                  //111001
,contact_fraction_                                                               //111001
,soil_texture_,soil_environment_)                                                //070707
,decomposition                                                               (0)
,nitrogen_organic_kg          (biomass_*carbon_fraction_/carbon_nitrogen_ratio_) //111005
{  for (int i = 0; i < 4; i++)
   {  C_fraction_transferred_to[i] = 0;
      receiver[i] = 0;
   }
/*
//debug
if (carbon_nitrogen_ratio < 1)
{
         std::cerr << "This should not occur: "
         << organic_matter_type_label_table[type]
         << " init C/N < 1 (layer=" << int(layer) << ")="
         << carbon_nitrogen_ratio
         << std::endl;
}
*/
}
//_Organic_biomatter_pool_single:constructor___________________________________/
Organic_biomatter_pool_single::Organic_biomatter_pool_single    // for residue
(Organic_matter_type     type_
,Organic_matter_position position_
,uint8                   layer_
,Organic_matter_cycling  cycling_
,RUSLE2_Residue_type     RUSLE2_residue_type_                                    //060721
,float64                 biomass_
,float64                 carbon_fraction_
,float64                 carbon_nitrogen_ratio_
,float64                 decomposition_constant_
,float64                 detrition_constant_                         //090702_090513 _decomposition_constant_nonmicrobial
,float64                 area_per_mass_       // = 0.0  stubble and surface residue_only?
,float64                &contact_fraction_                                       //190327
,const Soil::Texture_interface    &soil_texture_                                 //070707
,const Soil::Abiotic_environment_layer &soil_environment_)
:Organic_biomatter_pool_common
(type_,layer_,position_,cycling_,RUSLE2_residue_type_,biomass_,carbon_fraction_
,carbon_nitrogen_ratio_,decomposition_constant_,detrition_constant_
,area_per_mass_,contact_fraction_,soil_texture_,soil_environment_)
,decomposition                                                               (0)
,nitrogen_organic_kg(biomass_*carbon_fraction_/carbon_nitrogen_ratio_)           //111005
{  for (int r = 0; r < 4; r++)
   {  C_fraction_transferred_to[r] = 0;
      receiver[r] = 0;
   }
/*
//debug
if (carbon_nitrogen_ratio < 1)
{
         std::cerr << "This should not occur: "
         << organic_matter_type_label_table[type]
         << " init C/N < 1 (layer=" << int(layer) << ")="
         << carbon_nitrogen_ratio
         << std::endl;
}
*/
}
//_Organic_biomatter_pool_single:constructor___________________________________/
void Organic_biomatter_pool_single::clear_decomposition()
{  delete decomposition; decomposition = 0;
}
//_clear_decomposition______________________________________________2014-11-04_/
float64 Organic_biomatter_pool_single::get_carbon_nitrogen_ratio()    provision_
{  return carbon_nitrogen_ratio =
    ((type == STRAW_RESIDUE) || (type == MANURE_RESIDUE))
   ? Biomass_abstract::get_carbon_nitrogen_ratio()
   : calc_carbon_nitrogen_ratio();
}
//_get_carbon_nitrogen_ratio___________________________________________________/
float64 Organic_biomatter_pool_single::transfer_carbon_and_nitrogen()
//(float64 &CO2_loss_SOM,float64 &CO2_loss_residue)
{  float64 C_transfer_to_CO2 = 0;
   if (decomposition)                                                            //111003
   {  float64 decomposed_carbon =  decomposition->act_carbon;
      C_transfer_to_CO2 = decomposed_carbon * C_fraction_transfered_to_CO2;

//std::cout << (int) layer << "cfractx:" << C_fraction_transfered_to_CO2 << std::endl;

      Carbon_decomposition_residue *as_residue_decomposition =
         dynamic_cast<Carbon_decomposition_residue *>(decomposition);
      if (as_residue_decomposition
         #ifdef KEMANIAN_HUMIFICATION
         && as_residue_decomposition->humification
         #endif
         )
      {  // Should actually have a subtract method (complement of add(Mass))
         // but for now, subtract_mass(kg) should work the same
         // Do not subtract humification from residue pool,
         // simply deduct  decomposed_carbon this will take
         // care of all component masses because they are stored intensively.
         //170306 result not used float64 new_carbon =
            deduct_carbon(decomposed_carbon);
         if (receiver[1])
             receiver[1]->
             #ifdef KEMANIAN_HUMIFICATION
             add(*(as_residue_decomposition->humification));
             #else
             receive_carbon(decomposed_carbon*(this->C_fraction_transferred_to[1]));                                //190219
             #endif
         #ifdef KEMANIAN_HUMIFICATION
         #ifdef DETAILED_BALANCES
         C_balance->residue.humification_to_OM   += as_residue_decomposition->humification->get_carbon_kg();
         C_balance->OM.humification_from_residue += as_residue_decomposition->humification->get_carbon_kg();
         #endif
         #endif
      }
      else
      {
         //170306 result not used float64 new_carbon =
         deduct_carbon(C_transfer_to_CO2);
         nitrogen_organic_kg -= decomposition->N_mineralization;
         get_carbon_nitrogen_ratio();  // for update of C/N
      // ELSE warning I dont think there is anything to do for
      // SOM decomposition
      }
   }
   return C_transfer_to_CO2;
}
//_transfer_carbon_and_nitrogen________________________________________________/
Organic_biomatter_pool_common::Decomposition *Organic_biomatter_pool_single
::provide_decomposition_residue
(float64 moisture_function
,float64 temperature_function)                                        provision_ //141104 actually this is rendition
{
   delete decomposition;                                                         //141104
   return decomposition = new Organic_biomatter_pool_single::Carbon_decomposition_residue
      (*this ,temperature_function,moisture_function,true);
}
//_provide_decomposition_residue____________________________________2011-10-03_/
Organic_biomatter_pool_common::Decomposition *Organic_biomatter_pool_single
::provide_decomposition_SOM
(float64 tillage_decomposition_rate_adj
,float64 minimized_temperature_moisture_function)                     provision_ //141104 actually this is rendition
{
   delete decomposition;                                                         //141104
   return decomposition = new Organic_biomatter_pool_single::Carbon_decomposition_common
      (*this,tillage_decomposition_rate_adj,minimized_temperature_moisture_function,false);  // Not is a residue
}
//_provide_decomposition_SOM________________________________________2011-10-03_/
#ifdef KEMANIAN_HUMIFICATION
float64 Organic_biomatter_pool_single::Carbon_decomposition_residue::actual
(float64 N_deficit_for_immobilization
,float64 decomposition_reduction_factor)
{  // Begin reduction to potential decomposition amount
   if (humification && decomposition_reduction_factor > 0.0) // if there was potential humification.
   {  // Apply reduction
      humification->multiply_by(decomposition_reduction_factor);
      act_carbon = pot_carbon * decomposition_reduction_factor;
      #ifdef OBSOLETE
      // For checking.
      // the humification should have the same caracteristics as the following:
      float64 humified_C  = act_carbon * humification->factor;
      float64 humified_mass = humified_C / pool.get_carbon_fraction() ;
         // humified has the same C/N of decomposing residue
      float64 humified_N = (1.0 / pool.get_carbon_nitrogen_ratio())*act_carbon;
      //obs calculated as needed float64 humified_CN = humified_C / humified_N;
      float64 released_CO2_C = act_carbon * (1.0 - humification->factor);
      float64 humified_act =  humified_C / pool.get_carbon_fraction() ;
      #endif
   }
   return Organic_biomatter_pool_single::Carbon_decomposition_common
      ::actual(N_deficit_for_immobilization,decomposition_reduction_factor);
}
#endif

//_Carbon_decomposition_residue::actual_____________________________2011-10-01_/
Organic_biomatter_pool_single::Carbon_decomposition_residue::
Carbon_decomposition_residue
(const Organic_biomatter_pool_single &_pool                                      //111001
,float64 _temperature_function
,float64 _moisture_function
,bool    _is_a_residue)
:Organic_biomatter_pool_single::Carbon_decomposition_common
   (_pool                                                                        //111001
   , 1.0 // 070707 Claudio decided tillage adjustment should only affect SOM pools, so 1.0 is used for residues
   , 1.0  // _minimized_temperature_moisture_function  // does not apply to residue V4.3
   ,_is_a_residue)
, temperature_function  (_temperature_function)
, moisture_function     (_moisture_function)
#ifdef KEMANIAN_HUMIFICATION
, humification(0)                                                                //111001
#endif
{}
//_Carbon_decomposition_residue::Carbon_decomposition_residue__________________/
Organic_biomatter_pool_single::Carbon_decomposition_residue::~Carbon_decomposition_residue()
{
#ifdef KEMANIAN_HUMIFICATION
   delete humification; //humification = 0;
#endif
}
//_Carbon_decomposition_residue:destructor__________________________2011-10-04_/
Organic_biomatter_pool_single::Carbon_decomposition_common::Carbon_decomposition_common
(const Organic_biomatter_pool_common &_pool                                      //111001
,float64 _tillage_decomposition_rate_adj
,float64 _minimized_temperature_moisture_function
,bool    _is_a_residue)
: Organic_biomatter_pool_common::Decomposition
(_pool,_tillage_decomposition_rate_adj,_minimized_temperature_moisture_function
,_is_a_residue)
{}
//_Carbon_decomposition_common:constructor_____________________________________/
float64 Organic_biomatter_pool_single::Carbon_decomposition_common::N_mineralization_or_immobilization_demand
(float64 carbon_nitrogen_ratio)
{  // In VB this is done for each organic matter by
   //SoilOrganicCarbonClass::MineralizationAndImmobilizationDemand
   N_immobilization_demand = 0;                                                  //061219
   if (is_a_residue)
   {  if (net_N_mineralization < 0)
           N_immobilization_demand = -net_N_mineralization;
      else N_mineralization = net_N_mineralization;    //Residue N mineralization
   } else
      N_mineralization = pot_carbon / carbon_nitrogen_ratio;
   return N_immobilization_demand;
}
//_Carbon_decomposition_common::N_mineralization_or_immobilization_demand______/
#ifdef KEMANIAN_HUMIFICATION
float64 Organic_biomatter_pool_single::Carbon_decomposition_residue::potential()
{
   // Normally we decompose residues with a separate mechanism.
   // This is provided to match Armen's implementation of the single pool decomposition.

   float64 max_decomposition_rate_residue = pool.get_decomposition_constant();   //111001
   float64 decomposition_rate
         =  max_decomposition_rate_residue * moisture_function
            * temperature_function
            * pool.get_contact_fraction();
   if (decomposition_rate > 1.0)
   {  std::cerr << "warning: residue decomposition rate > 1, limited to 1" << std::endl;
      decomposition_rate = 1.0;
   }

   #ifdef KEMANIAN_HUMIFICATION
   delete humification; humification =0;                                         //141103
   #endif
   if (decomposition_rate > 0.000001)
   {  // The humified mass will have the same composition as the decomposing pool.
      #ifdef KEMANIAN_HUMIFICATION
      Organic_biomatter_pool_common *pool_decomposition = pool.clone();
      pool_decomposition->multiply_by(decomposition_rate);
      humification = new Humification(*pool_decomposition,pool.C_fraction_transferred_to[1]);
      // input-dependent constant to correct CN ratio = 110 (fix this to accommodate  manure)
      nat8 contribute_to_layer = pool.layer == 0 ? 1 : pool.layer;               //111001
      unmodifiable_ Soil::Nitrogen *soil_N = soil_hack->ref_nitrogen();          //160412
      float64 humification_CN_ratio = 9.3;                                       //131216
         // This is represented value
         // (it appears to fluctuate from  8.9 to 10.5, tending more to 9.46)
         // added by Roger to allow the model
         // to be run when the nitrogen model is not enabled.
         // Normally the nitrogen model is enabled for residue decomposition.
      if (soil_N)
      {
         static const float64 CN_ratio_correction = 110.0;
         float64 soil_NO3_kg_m2 = soil_N
            ->get_NO3_N_mass_kg_m2(contribute_to_layer);                         //111001
         float64 soil_NO3_conc = soil_NO3_kg_m2 / soil_hack->ref_structure()->get_mass_kg_m2(contribute_to_layer);  //160412_111001
         //190207 soil_NO3_conc += 0.0000001; // to avoid pow overflows when 0.0?

         if (CORN::is_zero<float64>(soil_NO3_conc,0.0000000000001))              //190207
            // needs a lot of precision
            soil_NO3_conc = 0.00000000000001;
            // to avoid pow overflows when 0.0?                                  //190207
            // needs a lot of precision because the denominator below is rather small.

         float64 YY2 //correction factor based on CN of decomposing pool
            = 5.5 * (1.0 - 1.0 / (1.0 + pow((pool.get_carbon_nitrogen_ratio() / CN_ratio_correction) , 3.0)));
         // float64      //170217
         humification_CN_ratio =  8.5 + YY2 * (0.5 + 0.5 / (1.0 + pow((soil_NO3_conc / 0.000008) , 3.0)));
      }
      humification->set_carbon_nitrogen_ratio(humification_CN_ratio);
      delete pool_decomposition; pool_decomposition = 0;
      #endif
      float64 residue_decomposition = pool.kg() * decomposition_rate; // from the decomposing residue pool
      pot_carbon = residue_decomposition * pool.get_carbon_fraction();
      // Unlike SOM there is no tillage decomposition rate adjustment (Claudio decided tillage adjustment should only affect SOM pools 070707)
      // Actual carbon decomposition will be the potential unless it is reduced
   }
   act_carbon = pot_carbon;  // actual will be the potential unless reduction in needed
   return pot_carbon;
}
//_Carbon_decomposition_residue::potential__________________________2011-10-01_/
#else
float64 Organic_biomatter_pool_single::Carbon_decomposition_residue::potential()
{
   // This is now actually identical to mulitple pool (should just have a common residue potential decomposition

   float64 temperature_term = (1.0 - exp(-pool.get_decomposition_constant()
       * temperature_function));                                                 //040806
   float64 microbial_term  = pool.get_carbon_kg()
      * pool.get_contact_fraction()                                              //190327
      * temperature_term * moisture_function;                                    //090413
   // Now nonmicrobial decomposition has been replaced by detrition              //090702
   pot_carbon  = microbial_term;                                                 //090409
   //printf("dec_const(%f)\ttemperature_function(%f)\ttemperature_term(%f)\tmicrobial_term(%f)\n",
   //       pool.get_decomposition_constant(),temperature_function,temperature_term,microbial_term);
   // Unlike SOM there is no tillage decomposition rate adjustment (Claudio decided tillage adjustment should only affect SOM pools 070707)
   // Actual carbon decomposition will be the potential unless it is reduced
   act_carbon = pot_carbon/*pot_carbon_nonmicrobial*/;
   return pot_carbon;
}
//_Carbon_decomposition_residue::potential__________________________2019-03-27_/
#endif
Organic_biomatter_pool_single::~Organic_biomatter_pool_single()
{  clear_decomposition();
}
//_Organic_biomatter_pool_single:destructor____________________________________/
float64 Organic_biomatter_pool_single::add(const Physical::Mass &addend) modification_
{
   Physical::Mass_common::add(addend);                                           //111010

//150203 I dont know why I didn't use:
//150203    Organic_biomatter_pool_common::add(addend);                          //150203

   const Biomass &as_biomass = dynamic_cast<const Biomass &>(addend); // Added must be some type of biomass
   float64 humification_N =  as_biomass.get_nitrogen_organic();
   // humified_CN = humified_carbon / humification_N; // Just a check to verify humification.get_carbon_nitrogen_ratio() above
   nitrogen_organic_kg +=humification_N;
   //float CN_ratio =
   get_carbon_nitrogen_ratio();  // For C/N to be updated.
   // Do not merge intensive properties defined in this class
   return dry_kg();
}
//_add______________________________________________________________2011-10-03_/
std::istream &Organic_biomatter_pool_single::read(std::istream &stream) stream_IO_
{
   Organic_biomatter_pool_common::read(stream);
   // I have verified that the computed nitrogen_organic_kg_calc is the same
   // at the end of the simulation so don't need to record
   if (carbon_nitrogen_ratio > 0.000001)
      nitrogen_organic_kg = kg() *carbon_fraction / carbon_nitrogen_ratio;
   // check see if need to setup C_fraction_transferred_to
   // probably not because there are not other pools
   return stream;
}
//_read_____________________________________________________________2017-02-28_/


