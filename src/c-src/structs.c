#include <node/node_api.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

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
    status |= napi_create_double(env, info->max_current_ma, &jsMaxCurrent);

    status |= napi_set_named_property(env, jsObject, "supplyVoltageMinV",
                                      jsSupplyVoltageMin);
    status |= napi_set_named_property(env, jsObject, "supplyVoltageMaxV",
                                      jsSupplyVoltageMax);
    status |=
        napi_set_named_property(env, jsObject, "maxCurrentMilliA", jsMaxCurrent);

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

