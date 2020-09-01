#ifndef crop_abstractH
#define crop_abstractH

#include "soil/soil_I.h"
#include "soil/nitrogen_I.h"
#include "common/weather/weather_interface.h"
#include "common/residue/residues_interface.h"
#include "crop/crop_interface.h"
#define likely_const_
// Weather should be unmodifiable but I haven't check const correctness
//______________________________________________________________________________
class Crop_abstract             // rename this to Crop_situated
: public Crop_interface
{
public:
   modifiable_    Soil::Soil_interface    &soil; // eventually class will be renamed
   unmodifiable_  Soil::Layers_interface  &soil_layers;  // eventually class will be renamed
   modifiable_    Soil::Nitrogen          &soil_chemicals;
   modifiable_    Residues_interface      *residues;
      // Residues is optional (used by the canopy decomposition submodel);
   const CORN::Date                       &today;  //200117 I think this can now bee date32 reference to simdate_raw
public:
   inline Crop_abstract
      (modifiable_   Soil::Soil_interface &soil_ref_
      ,modifiable_   Residues_interface   *residues_
      ,const CORN::Date                   &today_)
      : soil            (soil_ref_)
      , residues        (residues_)
      , soil_layers     (*(soil_ref_.ref_layers()))
      , soil_chemicals  ((*soil_ref_.mod_nitrogen()))
      , today           (today_)
      {}
   virtual const char* modname()                                        const=0;
};
//_class Crop_abstract____________________________________________________2014_/
#endif

