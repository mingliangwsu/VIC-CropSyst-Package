#ifndef temperatureH
#define temperatureH
#include "corn/primitive.h"
namespace Soil
{
//______________________________________________________________________________
interface_ Temperature // was Soil_temperature
{
 public:  // Accessors
   inline virtual ~Soil_temperature_interface()                               {}
   virtual float64 get_temperature(nat8 layer)                        const = 0;
};
//______________________________________________________________________________
} // namespace Soil
#endif
