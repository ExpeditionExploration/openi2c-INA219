
/**
 * INA219 I2C address enumeration.
 */
export enum I2CAddress {
    /** A0 = GND, A1 = GND ; 0x40 */
    ADDRESS_0 = (0x40 << 1),
    /** A0 = VS+, A1 = GND ; 0x41 */
    ADDRESS_1 = (0x41 << 1),
    /** A0 = SDA, A1 = GND ; 0x42 */
    ADDRESS_2 = (0x42 << 1),
    /** A0 = SCL, A1 = GND ; 0x43 */
    ADDRESS_3 = (0x43 << 1),
    /** A0 = GND, A1 = VS+ ; 0x44 */
    ADDRESS_4 = (0x44 << 1),
    /** A0 = VS+, A1 = VS+ ; 0x45 */
    ADDRESS_5 = (0x45 << 1),
    /** A0 = SDA, A1 = VS+ ; 0x46 */
    ADDRESS_6 = (0x46 << 1),
    /** A0 = SCL, A1 = VS+ ; 0x47 */
    ADDRESS_7 = (0x47 << 1),
    /** A0 = GND, A1 = SDA ; 0x48 */
    ADDRESS_8 = (0x48 << 1),
    /** A0 = VS+, A1 = SDA ; 0x49 */
    ADDRESS_9 = (0x49 << 1),
    /** A0 = SDA, A1 = SDA ; 0x4A */
    ADDRESS_A = (0x4A << 1),
    /** A0 = SCL, A1 = SDA ; 0x4B */
    ADDRESS_B = (0x4B << 1),
    /** A0 = GND, A1 = SCL ; 0x4C */
    ADDRESS_C = (0x4C << 1),
    /** A0 = VS+, A1 = SCL ; 0x4D */
    ADDRESS_D = (0x4D << 1),
    /** A0 = SDA, A1 = SCL ; 0x4E */
    ADDRESS_E = (0x4E << 1),
    /**< A0 = SCL, A1 = SCL ; 0x4F */
    ADDRESS_F = (0x4F << 1)
}

/**
 * INA219 bus voltage range enumeration.
 */
export enum BusVoltageRange {
    /** ±16V */
    VBUS_RANGE_16V = 0,
    /** ±32V */
    VBUS_RANGE_32V = 1
}

/**
 * INA219 shunt voltage PGA gain enumeration.
 * The gain is set to 1, 1/2, 1/4, or 1/8.
 * 
 * Smaller gain, or rather attenuation, allows for larger shunt voltages to be
 * measured.
 */
export enum PGAGain {
    /** PGA gain of 1. Allows for maximum shunt voltage of ±40mV. */
    GAIN_1 = 0,
    /** PGA gain of 1/2. Allows for maximum shunt voltage of ±80mV. */
    GAIN_1_DIV_2 = 1,
    /** PGA gain of 1/4. Allows for maximum shunt voltage of ±160mV. */
    GAIN_1_DIV_4 = 2,
    /** PGA gain of 1/8. Allows for maximum shunt voltage of ±320mV. */
    GAIN_1_DIV_8 = 3
}

/**
 * INA219 ADC resolution and sampling enumeration.
 */
export enum ADCMode {
    /** 9 bit / 1 samples, 84μs */
    ADC_MODE_9_BIT_1_SAMPLES    = 0x0,
    /** 10 bit / 1 samples, 148μs */
    ADC_MODE_10_BIT_1_SAMPLES   = 0x1,
    /** 11 bit / 1 samples, 276μs */
    ADC_MODE_11_BIT_1_SAMPLES   = 0x2,
    /** 12 bit / 1 samples, 532μs */
    ADC_MODE_12_BIT_1_SAMPLES   = 0x3,
    /** 12 bit / 2 samples, 1.06ms */
    ADC_MODE_12_BIT_2_SAMPLES   = 0x9,
    /** 12 bit / 4 samples, 2.13ms */
    ADC_MODE_12_BIT_4_SAMPLES   = 0xA,
    /** 12 bit / 8 samples, 4.26ms */
    ADC_MODE_12_BIT_8_SAMPLES   = 0xB,
    /** 12 bit / 16 samples, 8.51ms */
    ADC_MODE_12_BIT_16_SAMPLES  = 0xC,
    /** 12 bit / 32 samples, 17.02ms */
    ADC_MODE_12_BIT_32_SAMPLES  = 0xD,
    /** 12 bit / 64 samples, 34.05ms */
    ADC_MODE_12_BIT_64_SAMPLES  = 0xE,
    /** 12 bit / 128 samples, 68.10ms */
    ADC_MODE_12_BIT_128_SAMPLES = 0xF,
}

/**
 * INA219 mode enumeration.
 */
export enum INA219Mode {
    /** power down */
    POWER_DOWN = 0x0,
    /** shunt voltage triggered */
    SHUNT_VOLTAGE_TRIGGERED = 0x1,
    /** bus voltage triggered */
    BUS_VOLTAGE_TRIGGERED = 0x2,
    /** shunt and bus triggered */
    SHUNT_BUS_VOLTAGE_TRIGGERED = 0x3,
    /** adc off */
    ADC_OFF = 0x4,
    /** shunt voltage continuous */
    SHUNT_VOLTAGE_CONTINUOUS = 0x5,
    /** bus voltage continuous */
    BUS_VOLTAGE_CONTINUOUS = 0x6,
    /** shunt and bus voltage continuous */
    SHUNT_BUS_VOLTAGE_CONTINUOUS = 0x7,
}

/**
 * INA219 sensor information.
 */
export type INA219Info = {
    /** chip name */
    chipName: string;         
    /** manufacturer name */       
    manufacturerName: string; 
    /** chip interface name */     
    interface: string;     
    /** chip min supply voltage */            
    supplyVoltageMinV: number;   
    /** chip max supply voltage */     
    supplyVoltageMaxV: number;      
    /** chip max current */  
    maxCurrentMilliA: number;
    /** chip min operating temperature */              
    temperatureMin: number;     
    /** chip max operating temperature */        
    temperatureMax: number;  
    /** driver version */           
    driverVersion: number;           
}

export type INA219 = {
    /// Basic initialization of the INA219 sensor.
    init: (conf: Config) => Promise<void>,

    /// Return the shunt voltage in millivolts.
    getShuntVoltage: () => Promise<number>,

    /// Return the bus voltage in millivolts.
    getBusVoltage: () => Promise<number>,

    /// Return the current in milliamps.
    getCurrent: () => Promise<number>,

    /// Return the power in milliwatts.
    getPower: () => Promise<number>,

    /// Do a software reset of the INA219 sensor.
    softReset: () => Promise<void>,

    /// Get the sensor information.
    getSensorInfo: () => Promise<INA219Info>,

    /// Set the mode of the INA219 sensor.
    setMode: (mode: INA219Mode) => Promise<void>,
}

export type Config = {
    addr: I2CAddress,
    i2cDevice: string,
    r?: number,
    voltageRange?: BusVoltageRange,
    busAdcMode?: ADCMode,
    shuntAdcMode?: ADCMode,
    pgaGain?: PGAGain,
    mode?: INA219Mode,
}
