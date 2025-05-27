#include <node/node_api.h>

#include "driver_ina219.h"
#include "structs.h"

napi_value ina219_info_wrapper(napi_env env, napi_callback_info info) {
    size_t argc = 0; // No arguments expected
    napi_get_cb_info(env, info, &argc, NULL, NULL, NULL);

    // Create an instance of ina219_info_t
    ina219_info_t ina_info;

    // Call the ina219_info function
    uint8_t result = ina219_info(&ina_info);

    // Check success or failure
    if (result == 0) {
        // Convert ina_info to JavaScript object
        napi_value jsObj = ina219_info_to_js(env, &ina_info);
        return jsObj;
    } else if (result == 2) {
        napi_throw_error(env, NULL, "Failed to retrieve ina219 information");
        return NULL;
    }

    // In case of other unexpected values
    napi_throw_error(env, NULL, "Unexpected result from ina219_info");
    return NULL;
}
