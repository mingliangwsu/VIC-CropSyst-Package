#ifndef temperature_interfaceH
#define temperature_interfaceH
#include "corn/primitive.h"
namespace Soil
{
//______________________________________________________________________________
class Temperature_interface
{
 public:  // Accessors
   inline virtual ~Temperature_interface()                                    {} //181206
   virtual float64 get_temperature(nat8 layer)                          const=0;
};
//______________________________________________________________________________
} // namespace Soil
#endif
