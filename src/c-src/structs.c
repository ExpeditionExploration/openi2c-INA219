#include <node/node_api.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "driver_ina219.h"
#include "errors.h"

/**
 * Convert ina219_info_t into a JS object.
 */
napi_value ina219_info_to_js(napi_env env, const ina219_info_t *info) {
    // Create a new JavaScript object
    napi_value jsObject;
    napi_create_object(env, &jsObject);

    napi_status status;

    // Set properties on the JavaScript object
    napi_value jsChipName, jsManufacturerName, jsInterface;
    status = napi_create_string_utf8(env, info->chip_name, NAPI_AUTO_LENGTH,
                                     &jsChipName);
    status |= napi_create_string_utf8(env, info->manufacturer_name,
                                      NAPI_AUTO_LENGTH, &jsManufacturerName);
    status |= napi_create_string_utf8(env, info->interface, NAPI_AUTO_LENGTH,
                                      &jsInterface);

    status |= napi_set_named_property(env, jsObject, "chipName", jsChipName);
    status |= napi_set_named_property(env, jsObject, "manufacturerName",
                                      jsManufacturerName);
    status |= napi_set_named_property(env, jsObject, "interface", jsInterface);

    napi_value jsSupplyVoltageMin, jsSupplyVoltageMax, jsMaxCurrent;
    status |= napi_create_double(env, info->supply_voltage_min_v,
                                 &jsSupplyVoltageMin);
    status |= napi_create_double(env, info->supply_voltage_max_v,
                                 &jsSupplyVoltageMax);
    // The driver actually defines this as amps but names the field as
    // milliAmps. Thus, multiply by 1000 to convert to mA.
    status |=
        napi_create_double(env, info->max_current_ma * 1000, &jsMaxCurrent);

    status |= napi_set_named_property(env, jsObject, "supplyVoltageMinV",
                                      jsSupplyVoltageMin);
    status |= napi_set_named_property(env, jsObject, "supplyVoltageMaxV",
                                      jsSupplyVoltageMax);
    status |= napi_set_named_property(env, jsObject, "maxCurrentMilliA",
                                      jsMaxCurrent);

    napi_value jsTemperatureMin, jsTemperatureMax, jsDriverVersion;
    status |= napi_create_double(env, info->temperature_min, &jsTemperatureMin);
    status |= napi_create_double(env, info->temperature_max, &jsTemperatureMax);
    status |= napi_create_uint32(env, info->driver_version, &jsDriverVersion);

    status |= napi_set_named_property(env, jsObject, "temperatureMin",
                                      jsTemperatureMin);
    status |= napi_set_named_property(env, jsObject, "temperatureMax",
                                      jsTemperatureMax);
    status |= napi_set_named_property(env, jsObject, "driverVersion",
                                      jsDriverVersion);

    if (status != napi_ok) {
        napi_throw_error(env, ERROR_CREATING_NAPI_VALUE,
                         "fn: ina219_info_to_js");
        return NULL;
    }

    return jsObject;
}

static void finalize_handle(napi_env env, void *data, void *hint) {
    ina219_handle_t *handle = (ina219_handle_t *)data;
    if (handle != NULL) {
        // Deinitialize the handle if necessary
        ina219_deinit(handle);
        free(handle);
    }
}
/**
 * @brief Make a Buffer object from an ina219_handle_t pointer.
 *
 * - From calling this function, the JavaScript side will garbage collect, free,
 * and deinit the ina219_handle_t pointer when the Buffer is no longer needed.
 * - This function assumes the `handle` to have been allocated using `malloc()`.
 *
 * @return A Buffer object convertible to `ina219_handle_t` in C.
 *
 * In case of an error, it throws `error creating napi value` error and returns
 * `NULL`.
 */
napi_value ina219_handle_t_to_js(napi_env env, ina219_handle_t *handle) {
    napi_value jsHandle;
    napi_status status = napi_create_object(env, &jsHandle);
    if (status != napi_ok) {
        napi_throw_error(env, ERROR_CREATING_NAPI_VALUE,
                         "fn: mk_ina219_handle_t");
        return NULL;
    }

    status = napi_create_external_buffer(env, sizeof(*handle), handle,
                                         finalize_handle, NULL, &jsHandle);
    if (status != napi_ok) {
        napi_throw_error(env, ERROR_CREATING_NAPI_VALUE,
                         "fn: mk_ina219_handle_t");
        return NULL;
    }

    return jsHandle;
}
