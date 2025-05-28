#include <node/node_api.h>

#include "errors.h"
#include "fn-bindings.h"

// Initialize the module
napi_value Init(napi_env env, napi_value exports) {
    napi_value info_fn, basic_init_fn;
    napi_status status;

    napi_create_function(env, NULL, 0, ina219_info_wrapper, NULL,
                         &info_fn);
    napi_set_named_property(env, exports, "INA219Info", info_fn);

    // Create the basic_init function
    status = napi_create_function(env, NULL, 0, basic_init, NULL,
                                  &basic_init_fn);
    if (status != napi_ok) {
        napi_throw_error(env, ERROR_CREATING_NAPI_VALUE,
                         "Failed to create basic_init function");
        return NULL;
    }
    napi_set_named_property(env, exports, "basicInit", basic_init_fn);

    return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)
