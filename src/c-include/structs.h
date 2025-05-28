#ifndef STRUCTS_H
#define STRUCTS_H

#include <node_api.h>

#include "driver_ina219.h"

napi_value ina219_info_to_js(napi_env env, const ina219_info_t *info);

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
napi_value ina219_handle_t_to_js(napi_env env, ina219_handle_t *handle);

#endif
