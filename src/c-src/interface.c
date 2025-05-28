/**
 * Copyright (c) 2015 - present LibDriver All rights reserved
 *
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * @file      driver_ina219_interface_template.c
 * @brief     driver ina219 interface template source file
 * @version   1.0.0
 * @author    Shifeng Li
 * @date      2021-06-13
 *
 * <h3>history</h3>
 * <table>
 * <tr><th>Date        <th>Version  <th>Author      <th>Description
 * <tr><td>2021/06/13  <td>1.0      <td>Shifeng Li  <td>first upload
 * </table>
 */

#include "interface.h"

#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define IIC_BUS "/dev/i2c-1"
static int iic_fd = -1;

/**
 * @brief  interface iic bus init
 * @return status code
 *         - 0 success
 *         - 1 iic init failed
 * @note   none
 */
uint8_t ina219_interface_iic_init(void) {
    if ((iic_fd = open(IIC_BUS, O_RDWR)) < 0) {
        perror("Failed to open i2c bus");
        return 1;
    }
    return 0;
}

/**
 * @brief  interface iic bus deinit
 * @return status code
 *         - 0 success
 *         - 1 iic deinit failed
 * @note   none
 */
uint8_t ina219_interface_iic_deinit(void) {
    if (close(iic_fd) < 0) {
        perror("Failed to close i2c bus");
        return 1;
    }
    iic_fd = -1; // Reset the file descriptor
    return 0;
}

/**
 * @brief      interface iic bus read
 * @param[in]  addr iic device write address
 * @param[in]  reg iic register address
 * @param[out] *buf pointer to a data buffer
 * @param[in]  len length of the data buffer
 * @return     status code
 *             - 0 success
 *             - 1 read failed
 * @note       none
 */
uint8_t ina219_interface_iic_read(uint8_t addr, uint8_t reg, uint8_t *buf,
                                  uint16_t len) {
    struct i2c_rdwr_ioctl_data i2c_rdwr_data;
    struct i2c_msg msgs[2];

    /* clear ioctl data */
    memset(&i2c_rdwr_data, 0, sizeof(struct i2c_rdwr_ioctl_data));

    /* clear msgs data */
    memset(msgs, 0, sizeof(struct i2c_msg) * 2);

    /* set the param */
    msgs[0].addr = addr >> 1;
    msgs[0].flags = 0;
    msgs[0].buf = &reg;
    msgs[0].len = 1;
    msgs[1].addr = addr >> 1;
    msgs[1].flags = I2C_M_RD;
    msgs[1].buf = buf;
    msgs[1].len = len;
    i2c_rdwr_data.msgs = msgs;
    i2c_rdwr_data.nmsgs = 2;

    /* transmit */
    if (ioctl(iic_fd, I2C_RDWR, &i2c_rdwr_data) < 0) {
        perror("iic: read failed.\n");

        return 1;
    }

    return 0;
}

/**
 * @brief     interface iic bus write
 * @param[in] addr iic device write address
 * @param[in] reg iic register address
 * @param[in] *buf pointer to a data buffer
 * @param[in] len length of the data buffer
 * @return    status code
 *            - 0 success
 *            - 1 write failed
 * @note      none
 */
uint8_t ina219_interface_iic_write(uint8_t addr, uint8_t reg, uint8_t *buf,
                                   uint16_t len) {
    struct i2c_rdwr_ioctl_data i2c_rdwr_data;
    struct i2c_msg msgs[1];

    /* clear ioctl data */
    memset(&i2c_rdwr_data, 0, sizeof(struct i2c_rdwr_ioctl_data));

    /* clear msgs data */
    memset(msgs, 0, sizeof(struct i2c_msg) * 1);

    /* set the param */
    msgs[0].addr = addr >> 1;
    msgs[0].flags = 0;
    msgs[0].buf = buf;
    msgs[0].len = len;
    i2c_rdwr_data.msgs = msgs;
    i2c_rdwr_data.nmsgs = 1;

    /* transmit */
    if (ioctl(iic_fd, I2C_RDWR, &i2c_rdwr_data) < 0) {
        perror("iic: write failed.\n");

        return 1;
    }

    return 0;
}

/**
 * @brief     interface delay ms
 * @param[in] ms time
 * @note      none
 */
void ina219_interface_delay_ms(uint32_t ms) { usleep(ms * 1000); }

/**
 * @brief     interface print format data
 * @param[in] fmt format data
 * @note      none
 */
void ina219_interface_debug_print(const char *const fmt, ...) {}
