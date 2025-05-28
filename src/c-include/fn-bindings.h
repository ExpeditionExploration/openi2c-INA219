#ifndef FN_BINDINGS_H
#define FN_BINDINGS_H

#include <node/node_api.h>

napi_value ina219_info_wrapper(napi_env env, napi_callback_info info);

/**
 * Initialize ina219
 */
napi_value basic_init(napi_env, napi_callback_info info);

#endif
