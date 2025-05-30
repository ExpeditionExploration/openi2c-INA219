#include <node/node_api.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "driver_ina219.h"
#include "errors.h"
#include "interface.h"
#include "js_native_api.h"
#include "js_native_api_types.h"
#include "structs.h"

static ina219_handle_t ina219_iic_handle;

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
 * Initialize the INA219 sensor for continuous mode.
 *
 * Takes configuration object with following keys when calling from JavaScript:
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
 * - `mode`: Sensor mode enumerated in `INA219Mode`.
 *
 * Returns a JavaScript object containing the INA219 handle, which can be used
 * to interact with the INA219 sensor in subsequent function calls.
 */
napi_value basic_init(napi_env env, napi_callback_info info) {
    size_t argc = 1;
    napi_value argv[argc], this;

    napi_get_cb_info(env, info, &argc, argv, &this, NULL);

    if (argc != 1) {
        napi_throw_error(env, WRONG_NUMBER_OF_ARGUMENTS,
                         "Check number of arguments for fn: basic_init(..)");
        return NULL;
    }

    napi_status status;
    napi_value cfg;
    ina219_address_t addr;
    double r;
    char i2c_device[32];
    ina219_bus_voltage_range_t voltage_range;
    ina219_adc_mode_t bus_adc_mode;
    ina219_adc_mode_t shunt_adc_mode;
    ina219_pga_t pga;
    ina219_mode_t mode;

    cfg = argv[0];
    napi_value jAddr, jI2cDevice, jPgaGain, jBusAdcMode, jShuntAdcMode, jR,
        jVoltageRange, jMode;

    // Get mandatory init args from config object
    status = napi_get_named_property(env, cfg, "addr", &jAddr);
    status |= napi_get_named_property(env, cfg, "i2cDevice", &jI2cDevice);

    status |= napi_get_value_string_utf8(env, jI2cDevice, i2c_device,
                                         sizeof(i2c_device), NULL);
    status |= napi_get_value_uint32(env, jAddr, &addr);

    if (status != napi_ok) {
        napi_throw_error(env, ERROR_CREATING_NAPI_VALUE,
                         "Failed to create NAPI value for arguments");
        return NULL;
    }

    // Define defaults for the optional fields
    ina219_pga_t default_pga = INA219_PGA_320_MV;
    ina219_adc_mode_t default_bus_adc = INA219_ADC_MODE_12_BIT_128_SAMPLES;
    ina219_adc_mode_t default_shunt_adc = INA219_ADC_MODE_12_BIT_128_SAMPLES;
    float default_r = 0.1;
    ina219_bus_voltage_range_t default_voltage_range =
        INA219_BUS_VOLTAGE_RANGE_32V;
    ina219_mode_t default_mode = INA219_MODE_SHUNT_BUS_VOLTAGE_CONTINUOUS;

    // Check if the cfg object has optional init args from config object
    bool hasPgaGain, hasBusAdcMode, hasShuntAdcMode, hasR, hasVoltageRange,
        hasMode;
    status = napi_has_named_property(env, cfg, "pgaGain", &hasPgaGain);
    status |=
        napi_has_named_property(env, cfg, "hasBusAdcMode", &hasBusAdcMode);
    status |=
        napi_has_named_property(env, cfg, "hasShuntAdcMode", &hasShuntAdcMode);
    status |= napi_has_named_property(env, cfg, "hasR", &hasR);
    status |=
        napi_has_named_property(env, cfg, "hasVoltageRange", &hasVoltageRange);
    status |= napi_has_named_property(env, cfg, "hasMode", &hasMode);

    // Fetch all named properties from cfg object
    status |= napi_get_named_property(env, cfg, "pgaGain", &jPgaGain);
    status |= napi_get_named_property(env, cfg, "busAdcMode", &jBusAdcMode);
    status |= napi_get_named_property(env, cfg, "shuntAdcMode", &jShuntAdcMode);
    status |= napi_get_named_property(env, cfg, "r", &jR);
    status |= napi_get_named_property(env, cfg, "voltageRange", &jVoltageRange);
    status |= napi_get_named_property(env, cfg, "mode", &jMode);

    // Assign correct values to configuration variables
    if (hasPgaGain) // PGA Gain
        status |= napi_get_value_uint32(env, jPgaGain, &pga);
    else
        pga = default_pga;
    if (hasBusAdcMode) // Bus ADC mode
        status |= napi_get_value_uint32(env, jBusAdcMode, &bus_adc_mode);
    else
        bus_adc_mode = default_bus_adc;
    if (hasR) // Resistance
        status |= napi_get_value_double(env, jR, &r);
    else
        r = default_r;
    if (hasShuntAdcMode) // Shunt ADC mode
        status |= napi_get_value_uint32(env, jShuntAdcMode, &shunt_adc_mode);
    else
        shunt_adc_mode = default_shunt_adc;
    if (hasMode) // Sensor mode
        status |= napi_get_value_uint32(env, jMode, &mode);
    else
        mode = default_mode;
    if (hasVoltageRange) // Bus voltage range
        status |= napi_get_value_uint32(env, jVoltageRange, &voltage_range);
    else
        voltage_range = default_voltage_range;

    if (status != napi_ok) {
        napi_throw_error(env, ERROR_CREATING_NAPI_VALUE,
                         "Failed to create NAPI value for arguments");
        return NULL;
    }

    // Set the I2C bus
    set_iic_bus(i2c_device);

    DRIVER_INA219_LINK_INIT(&ina219_iic_handle, ina219_handle_t);
    DRIVER_INA219_LINK_IIC_INIT(&ina219_iic_handle, ina219_interface_iic_init);
    DRIVER_INA219_LINK_IIC_DEINIT(&ina219_iic_handle,
                                  ina219_interface_iic_deinit);
    DRIVER_INA219_LINK_IIC_READ(&ina219_iic_handle, ina219_interface_iic_read);
    DRIVER_INA219_LINK_IIC_WRITE(&ina219_iic_handle,
                                 ina219_interface_iic_write);
    DRIVER_INA219_LINK_DELAY_MS(&ina219_iic_handle, ina219_interface_delay_ms);
    DRIVER_INA219_LINK_DEBUG_PRINT(&ina219_iic_handle,
                                   ina219_interface_debug_print);

    /* set addr */
    int res;
    res = ina219_set_addr_pin(&ina219_iic_handle, addr);
    if (res != 0) {
        ina219_interface_debug_print("ina219: set addr pin failed.\n");
        napi_throw_error(env, INIT_ERROR, "Failed to set i2c address");
        return NULL;
    }

    /* set the r */
    res = ina219_set_resistance(&ina219_iic_handle, r);
    if (res != 0) {
        ina219_interface_debug_print("ina219: set resistance failed.\n");
        napi_throw_error(env, INIT_ERROR, "Failed to set resistance value");
        return NULL;
    }

    /* init */
    res = ina219_init(&ina219_iic_handle);
    if (res != 0) {
        ina219_interface_debug_print("ina219: init failed.\n");
        napi_throw_error(env, INIT_ERROR, "Failed to initialize INA219");
        return NULL;
    }

    /* set bus voltage range */
    res = ina219_set_bus_voltage_range(&ina219_iic_handle, voltage_range);
    if (res != 0) {
        ina219_interface_debug_print("ina219: set bus voltage range failed.\n");
        ina219_deinit(&ina219_iic_handle);
        napi_throw_error(env, INIT_ERROR, "Failed to set bus voltage range");
        return NULL;
    }

    /* set bus voltage adc mode */
    res = ina219_set_bus_voltage_adc_mode(&ina219_iic_handle, bus_adc_mode);
    if (res != 0) {
        ina219_interface_debug_print(
            "ina219: set bus voltage adc mode failed.\n");
        ina219_deinit(&ina219_iic_handle);
        napi_throw_error(env, INIT_ERROR, "Failed to set bus voltage ADC mode");
        return NULL;
    }

    /* set shunt voltage adc mode */
    res = ina219_set_shunt_voltage_adc_mode(&ina219_iic_handle, shunt_adc_mode);
    if (res != 0) {
        ina219_interface_debug_print(
            "ina219: set shunt voltage adc mode failed.\n");
        ina219_deinit(&ina219_iic_handle);
        napi_throw_error(env, INIT_ERROR,
                         "Failed to set shunt voltage ADC mode");
        return NULL;
    }

    /* set shunt&bus voltage continuous */
    res = ina219_set_mode(&ina219_iic_handle,
                          INA219_MODE_SHUNT_BUS_VOLTAGE_CONTINUOUS);
    if (res != 0) {
        ina219_interface_debug_print("ina219: set mode failed.\n");
        ina219_deinit(&ina219_iic_handle);
        napi_throw_error(env, INIT_ERROR, "Failed to set INA219 mode");
        return NULL;
    }

    /* set pga */
    res = ina219_set_pga(&ina219_iic_handle, pga);
    if (res != 0) {
        ina219_interface_debug_print("ina219: set pga failed.\n");
        ina219_deinit(&ina219_iic_handle);
        napi_throw_error(env, INIT_ERROR, "Failed to set PGA");
        return NULL;
    }

    /* calculate calibration */
    uint16_t calibration;
    res = ina219_calculate_calibration(&ina219_iic_handle, &calibration);
    if (res != 0) {
        ina219_interface_debug_print("ina219: calculate calibration failed.\n");
        ina219_deinit(&ina219_iic_handle);
        napi_throw_error(env, INIT_ERROR, "Failed to calculate calibration");
        return NULL;
    }

    /* set calibration */
    res = ina219_set_calibration(&ina219_iic_handle, calibration);
    if (res != 0) {
        ina219_interface_debug_print("ina219: set calibration failed.\n");
        ina219_deinit(&ina219_iic_handle);
        napi_throw_error(env, INIT_ERROR, "Failed to set calibration");
        return NULL;
    }

    return NULL;
}

napi_value ina219_read_shunt_voltage_wrapper(napi_env env,
                                             napi_callback_info info) {
    size_t argc = 0; // No arguments expected
    napi_get_cb_info(env, info, &argc, NULL, NULL, NULL);

    if (argc != 0) {
        napi_throw_error(env, WRONG_NUMBER_OF_ARGUMENTS,
                         "Check number of arguments for fn:"
                         " ina219_read_shunt_voltage_wrapper(..)");
        return NULL;
    }

    int16_t raw;
    float mV;
    uint8_t res = ina219_read_shunt_voltage(&ina219_iic_handle, &raw, &mV);
    if (res != 0) {
        napi_throw_error(env, INIT_ERROR, "Failed to read shunt voltage");
        return NULL;
    }

    napi_value jsMV;
    napi_status status = napi_create_double(env, mV, &jsMV);
    if (status != napi_ok) {
        napi_throw_error(env, ERROR_CREATING_NAPI_VALUE,
                         "Failed to create NAPI value for shunt voltage");
        return NULL;
    }

    return jsMV;
}

napi_value ina219_read_bus_voltage_wrapper(napi_env env,
                                           napi_callback_info info) {
    size_t argc = 0; // No arguments expected
    napi_get_cb_info(env, info, &argc, NULL, NULL, NULL);

    if (argc != 0) {
        napi_throw_error(env, WRONG_NUMBER_OF_ARGUMENTS,
                         "Check number of arguments for fn:"
                         " ina219_read_bus_voltage_wrapper(..)");
        return NULL;
    }

    uint16_t raw;
    float mV;
    uint8_t res = ina219_read_bus_voltage(&ina219_iic_handle, &raw, &mV);
    if (res != 0) {
        napi_throw_error(env, INIT_ERROR, "Failed to read bus voltage");
        return NULL;
    }

    napi_value jsMV;
    napi_status status = napi_create_double(env, mV, &jsMV);
    if (status != napi_ok) {
        napi_throw_error(env, ERROR_CREATING_NAPI_VALUE,
                         "Failed to create NAPI value for bus voltage");
        return NULL;
    }

    return jsMV;
}

napi_value ina219_read_current_wrapper(napi_env env, napi_callback_info info) {
    size_t argc = 0; // No arguments expected
    napi_get_cb_info(env, info, &argc, NULL, NULL, NULL);

    if (argc != 0) {
        napi_throw_error(env, WRONG_NUMBER_OF_ARGUMENTS,
                         "Check number of arguments for fn:"
                         " ina219_read_current_wrapper(..)");
        return NULL;
    }

    int16_t raw;
    float mA;
    uint8_t res = ina219_read_current(&ina219_iic_handle, &raw, &mA);
    if (res != 0) {
        napi_throw_error(env, INIT_ERROR, "Failed to read current");
        return NULL;
    }

    napi_value jsMA;
    napi_status status = napi_create_double(env, mA, &jsMA);
    if (status != napi_ok) {
        napi_throw_error(env, ERROR_CREATING_NAPI_VALUE,
                         "Failed to create NAPI value for current");
        return NULL;
    }

    return jsMA;
}

napi_value ina219_read_power_wrapper(napi_env env, napi_callback_info info) {
    size_t argc = 0; // No arguments expected
    napi_get_cb_info(env, info, &argc, NULL, NULL, NULL);

    if (argc != 0) {
        napi_throw_error(env, WRONG_NUMBER_OF_ARGUMENTS,
                         "Check number of arguments for fn:"
                         " ina219_read_power_wrapper(..)");
        return NULL;
    }

    uint16_t raw;
    float mW;
    uint8_t res = ina219_read_power(&ina219_iic_handle, &raw, &mW);
    if (res != 0) {
        napi_throw_error(env, INIT_ERROR, "Failed to read power");
        return NULL;
    }

    napi_value jsMW;
    napi_status status = napi_create_double(env, mW, &jsMW);
    if (status != napi_ok) {
        napi_throw_error(env, ERROR_CREATING_NAPI_VALUE,
                         "Failed to create NAPI value for power");
        return NULL;
    }

    return jsMW;
}

napi_value ina219_soft_reset_wrapper(napi_env env, napi_callback_info info) {
    size_t argc = 0; // No arguments expected
    napi_get_cb_info(env, info, &argc, NULL, NULL, NULL);
    if (argc != 0) {
        napi_throw_error(env, WRONG_NUMBER_OF_ARGUMENTS,
                         "Check number of arguments for fn:"
                         " ina219_soft_reset_wrapper(..)");
        return NULL;
    }
    uint8_t res = ina219_soft_reset(&ina219_iic_handle);
    switch (res) {
        case 0:
            // Success
            break;
        case 1:
            napi_throw_error(env, ERROR_RESETTING_DEVICE, "Soft reset failed");
            return NULL;
        case 2:
            napi_throw_error(env, ERROR_RESETTING_DEVICE, "Handle is NULL");
            return NULL;
        case 3:
            napi_throw_error(env, ERROR_RESETTING_DEVICE,
                             "Handle is not initialized");
            return NULL;
        default:
            napi_throw_error(env, ERROR_RESETTING_DEVICE,
                             "Unexpected error during soft reset");
            return NULL;
    }

    return NULL; // No return value needed for soft reset
}

/**
 * Set INA219 mode.
 *
 * Possible modes are enumerated in `INA219Mode` enum.
 *
 * Parameters when calling from JavaScript:
 * - `mode`: The mode to set, which is described in the `INA219Mode` enum.
 *
 * Returns nothing. Throws on error.
 *
 * JS errors that can be thrown:
 * - `INIT_ERROR`: If the INA219 handle is not initialized or if setting the
 *                 mode fails.
 * - `ERROR_CREATING_NAPI_VALUE`: If there is an error creating the NAPI value
 *                                for the mode.
 * - `WRONG_NUMBER_OF_ARGUMENTS`: If the number of arguments passed is not
 *                                correct.
 */
napi_value ina219_set_mode_wrapper(napi_env env, napi_callback_info info) {
    size_t argc = 1; // One argument expected
    napi_value argv[argc], this;

    napi_get_cb_info(env, info, &argc, argv, &this, NULL);

    if (argc != 1) {
        napi_throw_error(
            env, WRONG_NUMBER_OF_ARGUMENTS,
            "Check number of arguments for fn: ina219_set_mode_wrapper(..)");
        return NULL;
    }

    uint32_t mode;
    napi_status status = napi_get_value_uint32(env, argv[0], &mode);
    if (status != napi_ok) {
        napi_throw_error(env, ERROR_CREATING_NAPI_VALUE,
                         "Failed to create NAPI value for mode");
        return NULL;
    }

    uint8_t res = ina219_set_mode(&ina219_iic_handle, (ina219_mode_t)mode);
    if (res != 0) {
        napi_throw_error(env, INIT_ERROR, "Failed to set INA219 mode");
        return NULL;
    }

    return NULL;
}
