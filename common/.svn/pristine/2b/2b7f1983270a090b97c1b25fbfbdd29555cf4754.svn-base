#include "common/soil/layering.h"
//______________________________________________________________________________
void clear_sublayer_array(soil_sublayer_array_64(target),float64 default_value)
{  for (nat8 sublayer = 0; sublayer <= MAX_soil_sublayers; sublayer++)
      target[sublayer] = default_value;                                          //110217 added optional default value
}
//_clear_sublayer_array_____________________________________________1999-02-14_/
void clear_layer_array64(soil_layer_array64(target),float64 default_value)
{  for (nat8 sublayer = 0; sublayer <= MAX_soil_sublayers; sublayer++)
      target[sublayer] = default_value;                                          //110217 added optional default value
}
//_clear_layer_array64______________________________________________1999-02-14_/
void clear_horizon_array32(soil_horizon_array32(target),float32 default_value)
{  for (nat8 sublayer = 0; sublayer <= MAX_soil_horizons; sublayer++)
      target[sublayer] = default_value;                                          //110217 added optional default value
}
//_clear_horizon_array32____________________________________________2005-10-19_/
bool delete_horizon_at(soil_horizon_array32(target),nat8 layer)
{  if (layer < MAX_soil_horizons)
      for (nat8 l = layer; l < MAX_soil_horizons_alloc-1 ; l++)
         target[l] = target[l+1];
    // Don't clear because it will leave parameter editor in uncomputable state
   return true;
}
//_delete_horizon_at___________________________________________________________/
bool insert_horizon_at(soil_horizon_array32(target),nat8 layer)
{  if (layer == MAX_soil_horizons) return false; // can insert past end
   for (nat8 l = MAX_soil_horizons; l > layer; l--)
      target[l] = target[l-1];
   // We will keep a copy of the current values for the inserted layer
   return true;
}
//_insert_horizon_at___________________________________________________________/
void copy_layer_array64
(soil_layer_array64(target)
,const soil_layer_array64(source))
{  for (nat8 layer = 0; layer <= MAX_soil_layers; layer++)
      target[layer] = source[layer];
}
//_copy_layer_array64_______________________________________________2006-02-19_/
void copy_layer_array32
(soil_layer_array32(target)
,const soil_layer_array32(source))
{  for (nat8 layer = 0; layer <= MAX_soil_layers; layer++)
      target[layer] = source[layer];
}
//_copy_layer_array32___________________________________________2015-08-14_LML_/
void copy_layer_array32_to_array64
(soil_layer_array64(target)
 ,const soil_layer_array32(source))
{
    for (nat8 layer = 0; layer <= MAX_soil_layers; layer++)
          target[layer] = source[layer];
}
//_copy_layer_array32_to_array64____________________________________2006-02-19_/
void copy_horizon_array32(soil_horizon_array32(target),const soil_horizon_array32(source))
{  for (nat8 layer = 0; layer <= MAX_soil_horizons; layer++)
      target[layer] = source[layer];
}
//_copy_horizon_array32_____________________________________________2011-11-20_/
void copy_horizon_array32_sized
(soil_horizon_array32(target)
,const soil_horizon_array32(source)
,nat32 size)
{  for (nat8 layer = 0; layer <= size; layer++)
      target[layer] = source[layer];
}
//_copy_horizon_array32_sized_______________________________________2011-11-20_/
void multiply_by_layer_array64(soil_layer_array64(target),float64 multiplier)
{  for (nat8 layer = 0; layer <= MAX_soil_layers; layer++)
      target[layer] *= multiplier;
}//_multiply_by_layer_array64________________________________________2006-02-19_/

