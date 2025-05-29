#ifndef FN_BINDINGS_H
#define FN_BINDINGS_H

#include <node/node_api.h>

napi_value ina219_info_wrapper(napi_env env, napi_callback_info info);

/**
 * Initialize ina219
 */
napi_value basic_init(napi_env, napi_callback_info info);

/**
 * Read the shunt voltage from the INA219 sensor.
 *
 * This function reads the shunt voltage and returns mV as a JavaScript number.
 * It assumes that the INA219 has been initialized.
 */
napi_value ina219_read_shunt_voltage_wrapper(napi_env env,
                                             napi_callback_info info);

/**
 * Read the bus voltage from the INA219 sensor.
 *
 * This function reads the bus voltage and returns mV as a JavaScript number.
 * It assumes that the INA219 has been initialized.
 */
napi_value ina219_read_bus_voltage_wrapper(napi_env env,
                                           napi_callback_info info);

/**
 * Read the current from the INA219 sensor.
 *
 * This function reads the current and returns mA as a JavaScript number.
 * It assumes that the INA219 has been initialized.
 */
napi_value ina219_read_current_wrapper(napi_env env, napi_callback_info info);

/**
 * Read the power from the INA219 sensor.
 *
 * This function reads the power and returns mW as a JavaScript number.
 * It assumes that the INA219 has been initialized.
 */
napi_value ina219_read_power_wrapper(napi_env env, napi_callback_info info);

/**
 * Soft reset the INA219 sensor.
 *
 * This function performs a soft reset on the INA219 sensor.
 * It assumes that the INA219 has been initialized. Returns nothing and
 * throws an error if the reset fails.
 */
napi_value ina219_soft_reset_wrapper(napi_env env, napi_callback_info info);

#endif
