#include <node/node_api.h>

#include "errors.h"
#include "fn-bindings.h"

// Initialize the module
napi_value Init(napi_env env, napi_value exports) {
    napi_value info_fn, basic_init_fn;
    napi_status status;

    napi_create_function(env, NULL, 0, ina219_info_wrapper, NULL, &info_fn);
    napi_set_named_property(env, exports, "getSensorInfo", info_fn);

    // Create the basic_init function
    status =
        napi_create_function(env, NULL, 0, basic_init, NULL, &basic_init_fn);
    status |= napi_set_named_property(env, exports, "init", basic_init_fn);

    // Create getShuntVoltage function
    napi_value read_shunt_voltage_fn;
    status |=
        napi_create_function(env, NULL, 0, ina219_read_shunt_voltage_wrapper,
                             NULL, &read_shunt_voltage_fn);
    status |= napi_set_named_property(env, exports, "getShuntVoltage",
                                      read_shunt_voltage_fn);

    // Create getBusVoltage function
    napi_value read_bus_voltage_fn;
    status |=
        napi_create_function(env, NULL, 0, ina219_read_bus_voltage_wrapper,
                             NULL, &read_bus_voltage_fn);
    status |= napi_set_named_property(env, exports, "getBusVoltage",
                                      read_bus_voltage_fn);

    // Create getCurrent function
    napi_value read_current_fn;
    status |= napi_create_function(env, NULL, 0, ina219_read_current_wrapper,
                                   NULL, &read_current_fn);
    status |=
        napi_set_named_property(env, exports, "getCurrent", read_current_fn);

    // Create getPower function
    napi_value read_power_fn;
    status |= napi_create_function(env, NULL, 0, ina219_read_power_wrapper,
                                   NULL, &read_power_fn);
    status |= napi_set_named_property(env, exports, "getPower", read_power_fn);

    // Create softReset function
    napi_value soft_reset_fn;
    status |= napi_create_function(env, NULL, 0, ina219_soft_reset_wrapper,
                                   NULL, &soft_reset_fn);
    status |= napi_set_named_property(env, exports, "softReset", soft_reset_fn);

    // Create ina219_set_mode_wrapper function
    napi_value set_mode_fn;
    status |= napi_create_function(env, NULL, 0, ina219_set_mode_wrapper, NULL,
                                   &set_mode_fn);
    status |= napi_set_named_property(env, exports, "setMode", set_mode_fn);

    // Check if any of the NAPI calls failed
    if (status != napi_ok) {
        napi_throw_error(env, ERROR_CREATING_NAPI_VALUE,
                         "Failed to initialize INA219 NODE extension");
        return NULL;
    }

    return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)
