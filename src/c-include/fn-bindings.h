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

/**
 * Initialize the INA219 sensor for shot mode.
 *
 * Parameters when calling from JavaScript:
 * - `addr`: The I2C address of the INA219 sensor. Possible values are
 *           enumerated in `I2CAddress` enum.
 * - `i2c_device`: The I2C device file path, e.g., `/dev/i2c-1`.
 * - `r`: The shunt resistor value in ohms.
 * - `voltage_range`: The bus voltage range, which is described in the
 *                    `BusVoltageRange` enum.
 * - `bus_voltage_adc_mode`: The ADC mode for bus voltage, described in the
 *                           `ADCMode` enum.
 * - `shunt_voltage_adc_mode`: The ADC mode for shunt voltage, described in the
 *                             `ADCMode` enum.
 * - `pga`: The programmable gain amplifier setting, described in the
 *          `PGAGain` enum.
 *
 * Returns a JavaScript object containing the INA219 handle, which can be used
 * to interact with the INA219 sensor in subsequent function calls.
 */
napi_value shot_init(napi_env env, napi_callback_info info);

/**
 * Set INA219 mode.
 *
 * Possible modes are enumerated in `INA219Mode` enum.
 */
napi_value ina219_set_mode_wrapper(napi_env env, napi_callback_info info);

#endif
