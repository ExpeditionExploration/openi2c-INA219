
/**
 * - All-register reset
 * - settings for bus voltage range
 * - PGA gain
 * - ADC resolution/averaging
 */
export const CONFIGURATION_REGISTER = 0x00;

/**
 * Conversion times:
 * 
 * - Reso9b: 84μs
 * - Reso10b: 148μs
 * - Reso11b: 276μs
 * - Reso12b: 532μs
 * - Reso12bSamples2: 1.06ms
 * - Reso12bSamples4: 2.13ms
 * - Reso12bSamples8: 4.26ms
 * - Reso12bSamples16: 8.51ms
 * - Reso12bSamples32: 17.02ms
 * - Reso12bSamples64: 34.05ms
 * - Reso12bSamples128: 68.10ms
 */
export enum ADCSetting {
    Reso9b = 0b0000,
    Reso10b = 0b0001,
    Reso11b = 0b0010,
    Reso12b = 0b0011,
    Reso12bSamples2 = 0b1001,
    Reso12bSamples4 = 0b1010,
    Reso12bSamples8 = 0b1011,
    Reso12bSamples16 = 0b1100,
    Reso12bSamples32 = 0b1101,
    Reso12bSamples64 = 0b1110,
    Reso12bSamples128 = 0b1111,
}

/**
 * Operating mode setting
 * 
 * - `PowerDown` Triggered modes use as much power as continuous modes. If you
 *   prefer to shave off a few mW and use triggered sampling/conversions,
 *   then this should probably be included to the cycle.
 * - Writing `TriggeredShuntVoltage` or `TriggeredBusVoltage` will cause the
 *   device to take a shunt voltage measurement or a bus voltage.
 * - `ADCOff` turns off the ADC.
 * - `ContinuousShuntVoltage` will cause the device to take a shunt voltage
 *   measurement continuously.
 * - `ContinuousBusVoltage` will cause the device to take a bus voltage
 *   measurement continuously.
 * - `ContinuousShuntAndBusVoltage` will cause the device to take a shunt
 *   voltage and bus voltage measurement continuously. This is the **default
 *   mode**.
 */
export enum ModeSetting {
    PowerDown,
    TriggeredShuntVoltage,
    TriggeredBusVoltage,
    TriggeredShuntAndBusVoltage,
    ADCOff,
    ContinuousShuntVoltage,
    ContinuousBusVoltage,
    ContinuousShuntAndBusVoltage
}

/**
 * Shunt voltage measurement data.
 * 
 * *Read only*
 */
export const SHUNT_VOLTAGE_REGISTER = 0x01;

/**
 * Bus voltage
 * 
 * *Read only*
 */
export const BUS_VOLTAGE_REGISTER = 0x02;

/**
 * Power measurement data
 * 
 * Yields 0 value until `CALIBRATION_REGISTER` is programmed.
 * 
 * *Read only*
 */
export const POWER_REGISTER = 0x03;

/**
 * Current over the shunt resistor
 * 
 * Yields 0 value until `CALIBRATION_REGISTER` is programmed.
 * 
 * *Read only*
 */
export const CURRENT_REGISTER = 0x04;

/**
 * - Set full-scale range
 * - Set LSB of current and power measurements
 * - Overall system calibration
 * 
 * Current and power calibration are set by bits D15 to D1. D0 is not used in
 * the calculation.
 * 
 * This register sets the current that corresponds to a full-scale drop across
 * the shunt. Full-scale range and the LSB of the current and power
 * measurement depend on the value entered in this register.
 * 
 * **Defaults** to `0x00`, and it is not possible to write 1 to the LSB; It is
 * always 0.
 * 
 * See the datasheet for specifics.
 */
export const CALIBRATION_REGISTER = 0x05;
