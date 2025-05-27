#include <node/node_api.h>

#include "fn-bindings.h"

// Initialize the module
napi_value Init(napi_env env, napi_value exports) {
    napi_value wrap_function;
    napi_create_function(env, NULL, 0, ina219_info_wrapper, NULL,
                         &wrap_function);
    napi_set_named_property(env, exports, "INA219Info", wrap_function);
    return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)
