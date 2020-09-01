#ifndef soil_texture_IH
#define soil_texture_IH
#include "corn/const.h"
#include "corn/primitive.h"
namespace Soil
{
//______________________________________________________________________________
interface_ Texture  //181206 was Soil_texture_interface
{
public: // Texture parameters
   virtual float64 get_clay_percent                      (nat8 layer) const = 0;
   virtual float64 get_sand_percent                      (nat8 layer) const = 0;
   virtual float64 get_silt_percent                      (nat8 layer) const = 0;
public: // set
   virtual void set_sand_clay
      (nat8 layer, float64 _sand, float64 _clay)              modification_ = 0;
public: // Values for the entire profile
};
//______________________________________________________________________________
} // namespace Soil
#endif

