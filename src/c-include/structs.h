#ifndef STRUCTS_H
#define STRUCTS_H

#include <node_api.h>

#include "driver_ina219.h"

napi_value ina219_info_to_js(napi_env env, const ina219_info_t *info);

#endif
