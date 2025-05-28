#include <node/node_api.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "driver_ina219.h"
#include "errors.h"
#include "interface.h"
#include "js_native_api.h"
#include "js_native_api_types.h"
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

/**
 * Initialize the INA219 sensor
 *
 * Parameters when calling from JavaScript:
 * - `addr`: The I2C address of the INA219 sensor. Possible values are
 *           enumerated in `I2CAddress` enum.
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
napi_value basic_init(napi_env env, napi_callback_info info) {
    size_t argc = 6; // Two arguments expected
    napi_value argv[argc], this;

    napi_get_cb_info(env, info, &argc, argv, &this, NULL);

    if (argc != 6) {
        napi_throw_error(env, WRONG_NUMBER_OF_ARGUMENTS,
                         "Check number of arguments for fn: basic_init(..)");
        return NULL;
    }

    napi_status status;
    double r;
    ina219_address_t addr;
    ina219_bus_voltage_range_t voltage_range;
    ina219_adc_mode_t bus_adc_mode;
    ina219_adc_mode_t shunt_adc_mode;
    ina219_pga_t pga;

    status = napi_get_value_uint32(env, argv[0], &addr);
    status |= napi_get_value_double(env, argv[1], &r);
    status |= napi_get_value_uint32(env, argv[2], &voltage_range);
    status |= napi_get_value_uint32(env, argv[3], &bus_adc_mode);
    status |= napi_get_value_uint32(env, argv[4], &shunt_adc_mode);
    status |= napi_get_value_uint32(env, argv[5], &pga);
    if (status != napi_ok) {
        napi_throw_error(env, ERROR_CREATING_NAPI_VALUE,
                         "Failed to create NAPI value for arguments");
        return NULL;
    }

    ina219_handle_t* handle = malloc(sizeof(ina219_handle_t));

    DRIVER_INA219_LINK_INIT(handle, ina219_handle_t);
    DRIVER_INA219_LINK_IIC_INIT(handle, ina219_interface_iic_init);
    DRIVER_INA219_LINK_IIC_DEINIT(handle, ina219_interface_iic_deinit);
    DRIVER_INA219_LINK_IIC_READ(handle, ina219_interface_iic_read);
    DRIVER_INA219_LINK_IIC_WRITE(handle, ina219_interface_iic_write);
    DRIVER_INA219_LINK_DELAY_MS(handle, ina219_interface_delay_ms);
    DRIVER_INA219_LINK_DEBUG_PRINT(handle, ina219_interface_debug_print);

    /* set addr */
    int res;
    res = ina219_set_addr_pin(handle, addr);
    if (res != 0) {
        ina219_interface_debug_print("ina219: set addr pin failed.\n");
        free(handle);
        napi_throw_error(env, INIT_ERROR, "Failed to set i2c address");
        return NULL;
    }

    /* set the r */
    res = ina219_set_resistance(handle, r);
    if (res != 0) {
        ina219_interface_debug_print("ina219: set resistance failed.\n");
        free(handle);
        napi_throw_error(env, INIT_ERROR, "Failed to set resistance value");
        return NULL;
    }

    /* init */
    res = ina219_init(handle);
    if (res != 0) {
        ina219_interface_debug_print("ina219: init failed.\n");
        free(handle);
        napi_throw_error(env, INIT_ERROR, "Failed to initialize INA219");
        return NULL;
    }

    /* set bus voltage range */
    res = ina219_set_bus_voltage_range(handle, voltage_range);
    if (res != 0) {
        ina219_interface_debug_print("ina219: set bus voltage range failed.\n");
        ina219_deinit(handle);
        free(handle);
        napi_throw_error(env, INIT_ERROR, "Failed to set bus voltage range");
        return NULL;
    }

    /* set bus voltage adc mode */
    res = ina219_set_bus_voltage_adc_mode(handle, bus_adc_mode);
    if (res != 0) {
        ina219_interface_debug_print(
            "ina219: set bus voltage adc mode failed.\n");
        ina219_deinit(handle);
        free(handle);
        napi_throw_error(env, INIT_ERROR, "Failed to set bus voltage ADC mode");
        return NULL;
    }

    /* set shunt voltage adc mode */
    res = ina219_set_shunt_voltage_adc_mode(handle, shunt_adc_mode);
    if (res != 0) {
        ina219_interface_debug_print(
            "ina219: set shunt voltage adc mode failed.\n");
        ina219_deinit(handle);
        free(handle);
        napi_throw_error(env, INIT_ERROR,
                         "Failed to set shunt voltage ADC mode");
        return NULL;
    }

    /* set shunt&bus voltage continuous */
    res = ina219_set_mode(handle, INA219_MODE_SHUNT_BUS_VOLTAGE_CONTINUOUS);
    if (res != 0) {
        ina219_interface_debug_print("ina219: set mode failed.\n");
        ina219_deinit(handle);
        free(handle);
        napi_throw_error(env, INIT_ERROR, "Failed to set INA219 mode");
        return NULL;
    }

    /* set pga */
    res = ina219_set_pga(handle, pga);
    if (res != 0) {
        ina219_interface_debug_print("ina219: set pga failed.\n");
        ina219_deinit(handle);
        free(handle);
        napi_throw_error(env, INIT_ERROR, "Failed to set PGA");
        return NULL;
    }

    /* calculate calibration */
    uint16_t calibration;
    res = ina219_calculate_calibration(handle, &calibration);
    if (res != 0) {
        ina219_interface_debug_print("ina219: calculate calibration failed.\n");
        ina219_deinit(handle);
        free(handle);
        napi_throw_error(env, INIT_ERROR, "Failed to calculate calibration");
        return NULL;
    }

    /* set calibration */
    res = ina219_set_calibration(handle, calibration);
    if (res != 0) {
        ina219_interface_debug_print("ina219: set calibration failed.\n");
        ina219_deinit(handle);
        free(handle);
        napi_throw_error(env, INIT_ERROR, "Failed to set calibration");
        return NULL;
    }

    // Create a JavaScript object to return the handle
    napi_value jsHandleObject, handle_buffer;
    handle_buffer = ina219_handle_t_to_js(env, handle);
    if (handle_buffer == NULL) {
        ina219_interface_debug_print("ina219: handle to js failed.\n");
        napi_throw_error(env, ERROR_CREATING_NAPI_VALUE,
                         "Failed to create handle buffer");
        return NULL;
    }

    // Set the handle in the JavaScript context
    status = napi_create_object(env, &jsHandleObject);
    status |= napi_set_named_property(env, this, "handle", jsHandleObject);
    if (status != napi_ok) {
        ina219_interface_debug_print(
            "ina219: creating handle object failed.\n");
        napi_throw_error(env, ERROR_CREATING_NAPI_VALUE,
                         "Failed to create handle object");
        return NULL;
    }
    return jsHandleObject;
}
