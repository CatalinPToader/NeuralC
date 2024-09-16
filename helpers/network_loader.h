#ifndef NET_LOAD_H
#define NET_LOAD_H

#ifndef NEURON_H
#include "../neuron.h"
#endif
#ifndef NET_MATH_HELP_H
#include "math.h"
#endif
#ifndef NET_CONF_HELP_H
#include "network_config_helper.h"
#endif
#include <stdlib.h>

typedef struct tuple_layer TupleLayer;

struct tuple_layer {
    Activator activator;
    int neuron_count;
};

Activator createID();
Network* load(FILE *network_file);
#endif