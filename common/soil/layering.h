#ifndef layeringH
#define layeringH
#ifndef primitiveH
#  include "corn/primitive.h"
#endif

typedef nat8 Layer;

#define MAX_soil_horizons_alloc    21
#define MAX_soil_horizons          (nat8)(20)
#define soil_horizon_array32(x)            float32 x[MAX_soil_horizons_alloc]
#define soil_horizon_array64(x)            float64 x[MAX_soil_horizons_alloc]
// The  valid_soil_horizon_array appears to be obsolete (in all versions)
// but it is still used by sim_param.h  (but I need to check if I can make it obsolete there)
#define valid_soil_horizon_array(x)         Valid_float32 *x[MAX_soil_horizons_alloc]

// Replacing sublayers with layers
// (layers replaced with horizons)
#define MAX_soil_sublayers_alloc 49
#define MAX_soil_sublayers       (nat8)(48)

#define MAX_soil_layers_alloc 49
#define MAX_soil_layers       (nat8)(48)


#define LAYERS_SUM_INDEX                     0
// For the soil layer arrays (which are indexed from 1)
// Index 1 is reserved for the sum
#define soil_layer_array32(x)                float32 x[MAX_soil_layers_alloc]
#define soil_layer_array64(x)                float64 x[MAX_soil_layers_alloc]
//051120 Now use soil_layer_array64 where the soil layering system has been abstracted
#define soil_sublayer_array_64(x)            float64 x[MAX_soil_sublayers_alloc]

#define soil_layering(x)                  nat8 x[MAX_soil_sublayers_alloc]
#define soil_sublayer_bool_array(x)       bool x[MAX_soil_sublayers_alloc]

#define valid_double_soil_layer_array(x)  Valid_float64 *x[MAX_soil_horizons_alloc]
#define valid_soil_sublayer_array(x)      Valid_float32 *x[MAX_soil_sublayers_alloc]

void clear_layer_array32(soil_layer_array32(target),float32 default_value = 0);
void clear_layer_array64(soil_layer_array64(target),float64 default_value = 0);
void clear_sublayer_array(soil_sublayer_array_64(target),float64 default_value = 0);  // replacing with clear_layer_array64()
void clear_horizon_array32(soil_horizon_array32(target),float32 default_value = 0);

// rename these to xxx_horizon_at
bool delete_horizon_at(soil_horizon_array32(target),nat8 layer);
bool insert_horizon_at(soil_horizon_array32(target),nat8 layer);

void copy_layer_array64(soil_layer_array64(target),const soil_layer_array64(source));  //060219
void copy_layer_array32(soil_layer_array32(target),const soil_layer_array32(source));  //060219
void copy_layer_array32_to_array64(soil_layer_array64(target),const soil_layer_array32(source));  //150814LML
void multiply_by_layer_array64(soil_layer_array64(target),float64 multiplier);   //060219
void copy_horizon_array32(soil_horizon_array32(target),const soil_horizon_array32(source)); //111120
void copy_horizon_array32_sized                                                  //160916
   (soil_horizon_array32(target)
   ,const soil_horizon_array32(source)
   ,nat32 size);
#define MAX_nodes  MAX_soil_layers_alloc
#define soil_node_array64(x) float64 x[MAX_nodes]
#define clear_node_array64(x)  clear_layer_array64(x)

#endif

