import { sleep } from './src/utils';
import { Module } from './src/module';
import {
    BUS_VOLTAGE_REGISTER,
    CALIBRATION_REGISTER,
    CONFIGURATION_REGISTER,
    CURRENT_REGISTER,
    POWER_REGISTER,
    SHUNT_VOLTAGE_REGISTER,
    ADCSetting,
    ModeSetting,
} from './src/constants'

export enum BusVoltage {
    Max16V = 0,
    Max32V = 1,
}

export enum ShuntVoltage {
    /**
     * TinyRange offers the range of +-40mV over the shunt resistor.
     * 
     * Conversly, TinyRange offers best resolution (~1.22μV). 
     */
    Max40mV = 0, // Gain = 1

    /**
     * SmallRange offers the range of +-80mV over the shunt resistor.
     * 
     * Conversly, SmallRange offers a bit worse resolution (~2.44μV).
     */
    Max80mV = 1, // Gain = 2

    /**
     * LargeRange offers the range of +-160mV over the shunt resistor.
     * 
     * Conversly, LargeRange offers worse resolution (~4.88μV).
     */
    Max160mV = 2, // Gain = 4

    /**
     * HugeRange offers the range of 320mV over the shunt resistor.
     * 
     * Conversly, HugeRange offers worst resolution (~9.766μV).
     */
    Max320mV = 3, // Gain = 8
}

export type Configuration = number;
export type Config = {
    address: number;

    // Configuration and calibration parameters

    /**
     * Set PGA gain.
     */
    maxShuntVoltage?: ShuntVoltage;

    /**
     * Maximum expected bus voltage.
     */
    maxBusVoltage?: BusVoltage;

    /**
     * Shunt resistor is 0.5ohm, but this lets you input the real value in case needed.
     */
    shuntResistance?: number;

    /**
     * Shunt ADC resolution/averaging setting. See datasheet page 27. Four bits.
     */
    shuntADCResolution?: ADCSetting;

    /**
     * Bus ADC resolution/averaging setting. See datasheet page 26/27. Four bits.
     */
    busADCResolution?: ADCSetting;

    /**
     * Operating mode setting. See datasheet Mode Setting docstring.
     */
    mode?: ModeSetting;

}

/**
 * Configuration register shift amount for SADC and BADC bits
 * 
 * - SADC: Shunt ADC resolution and averaging setting
 * - BADC: Bus ADC resolution and averaging setting
 * 
 * * See datasheet page 26/27 for more details.
 * 
 * Used internally by this file.
 */
enum ADCShiftAmount {
    SADC = 3,
    BADC = 7
}

/**
 * INA219 current measurement module
 * 
 * See the example and docstring for usage.
 * 
 * The constructor also accepts `Config` object with the following properties:
 * - `address`: I2C address of the device. Default is 0x40.
 * - `maxBusVoltage`: Maximum bus voltage. Default is 32V.
 * - `shuntResistance`: Shunt resistance in Ohms. Default is 0.1 Ohm.
 * - `maxShuntVoltage`: Maximum shunt voltage. Default is 320mV.
 * - `busADCResolution`: Bus ADC resolution. Default is 12-bit mode, 1 sample.
 * - `shuntADCResolution`: Shunt ADC resolution. Default is 12-bit mode, 1 sample.
 * - `mode`: Operating mode. Default is continuous shunt and bus voltage.
 * 
 * Providing only partial `Config` makes the rest of the properties
 * default to the values above.
 */
export class INA219 extends Module<Config> {
    private static config: Config = {
        address: 0x40, // Default slave address with no soldering
        maxBusVoltage: BusVoltage.Max32V, // Default
        shuntResistance: 0.1,
        maxShuntVoltage: ShuntVoltage.Max320mV, // Default to smaller resolution to avoid overflow.
        busADCResolution: ADCSetting.Reso12b, // Default is 12-bit mode, 1 sample
        shuntADCResolution: ADCSetting.Reso12b, // Default is 12-bit mode, 1 sample
        mode: ModeSetting.ContinuousShuntAndBusVoltage,
    }
    config: Config;

    private lsbCurrent?: number;
    private lsbPower?: number;
    private calibration: number = 0;

    constructor(busNumber?: number, config?: Partial<Config>) {
        super(busNumber);
        this.debug(`@ctor BADC: 0x${config?.busADCResolution?.toString(16)}`);
        this.debug(`@ctor SADC: 0x${config?.shuntADCResolution?.toString(16)}`);
        this.config = Object.assign({}, INA219.config, config);
        this.debug(`@ctor this.config BADC: 0x${this.config?.busADCResolution?.toString(16)}`);
        this.debug(`@ctor this.config SADC: 0x${this.config?.shuntADCResolution?.toString(16)}`);

    }

    async init() {
        super.init();

        // Init configuration register.
        // let configuration = await this.readConfiguration();
        // 0b0011100110011111; // Default
        const configuration = await this.mkConfiguration();
        await this.writeConfiguration(configuration);
        await sleep(5);

        await this.calibrate();
        await sleep(5);
    }

    /**
     * Make config based on `this.config` object.
     */
    private async mkConfiguration(): Promise<Configuration> {
        //let configuration = 0b0011100110011111;
        let configuration = 0;
        configuration |= (this.config.maxBusVoltage! & 0b1) << 13; // BRNG
        configuration |= (this.config.maxShuntVoltage! & 0b11) << 11; // PGA
        configuration = await this.setBusADCResolutionAndSampling(
            configuration, this.config.busADCResolution!);
        configuration = await this.setShuntADCResolutionAndSampling(
            configuration, this.config.shuntADCResolution!);
        configuration |= await this.setMode(configuration, this.config.mode!);
        this.debug(`0b${(configuration >>> 8).toString(2).padStart(8, '0')} ${(0xFF & (configuration >>> 0)).toString(2).padStart(8, '0')} : Configuration register value`);
        this.debug(`0b0x111001 10011111 : Default configuration register value`);
        return configuration;
    }

    /**
     * Converts between configuration register value and config object
     * @param c Configuration register value
     * @returns `Config` object
     */
    private async configurationToConfig(c: Configuration): Promise<Config> {
        const RST = (c >> 15) & 0b1;
        const D14UNUSED = (c >> 14) & 0b1;
        const BRNG = (c >> 13) & 0b1;
        const PGA = (c >> 11) & 0b11;
        const BADC = (c >> 7) & 0b1111;
        const SADC = (c >> 3) & 0b1111;
        const MODE = (c >> 0) & 0b111;

        const config: Config = {
            address: this.config.address,
            maxBusVoltage: BRNG,
            maxShuntVoltage: PGA,
            shuntResistance: this.config.shuntResistance,
            busADCResolution: BADC,
            shuntADCResolution: SADC,
            mode: MODE,
        };
        return config;
    }

    /**
     * Resets the configuration register to default value.
     * 
     * Sets the reset bit, then reinstates the configuration register value and
     * re-calibrates the device.
     */
    async resetAndReinstateSettings(): Promise<void> {
        const configuration = await this.readConfiguration();
        const calibration = await this.readCalibration();
        const reset = configuration | (1 << 15);
        await this.writeConfiguration(reset);
        this.debug(`Reseted configuration register. Reinstating this.config.`);
        while ((await this.readConfiguration() & (1 << 15)) !== 0) {
            await sleep(5);
        }
        await this.writeConfiguration(configuration);
        await this.writeCalibration(calibration);
        this.debug(`Reinstated configuration register. Calibration re-written.`);
    }

    /**
     * Resets the configuration register to default value.
     * 
     * Sets the reset bit, which will self-clear. Update this.config to reflect
     * new configuration register value.
     */
    async reset(): Promise<void> {
        const configuration = await this.readConfiguration();
        await this.writeConfiguration(configuration | (1 << 15));
        await sleep(150);
        // const resetedConfiguration = await this.readConfiguration();
        this.config = await this.configurationToConfig(0x399F);
        await sleep(100);
    }

    /**
     * Sets operating mode to given configuration register value
     * 
     * **NOTE** Does not write to configuration register.
     * 
     * @param cfg Variable with `Configuration` value
     * @param mode Variable with `ModeSetting` value
     * @returns New `Configuration` value which can be written to register
     */
    async setMode(cfg: Configuration, mode: ModeSetting)
    : Promise<Configuration> {
        return (cfg & ~3) | mode;
    }

    private async setADCResolutionAndSampling(
        cfg: Configuration, reso: ADCSetting, shift: ADCShiftAmount
    ): Promise<Configuration> {
        const ret = (cfg & ~(0xF << shift)) | (reso << shift);
        this.debug(`setting ADC reso. Shift: ${shift}, reso: 0x${reso.toString(16)}`);
        this.debug(`this.config SADC: 0x${this.config.shuntADCResolution?.toString(16)}`);
        this.debug(`this.config BADC: 0x${this.config.busADCResolution?.toString(16)}`);
        return ret;
    }
    
    /**
     * Returns new `Configuration` value with resolution and sampling set
     * 
     * **NOTE** Does not write to configuration register.
     * 
     * @param cfg Variable with `Configuration` value
     * @param reso See `ADCSettings` docstring for options
     * @returns Updated `Configuration` value
     */
    async setBusADCResolutionAndSampling(cfg: Configuration, reso: ADCSetting):
    Promise<Configuration> {
        return this.setADCResolutionAndSampling(cfg, reso, ADCShiftAmount.BADC);
    }

    /**
     * Returns new `Configuration` value with resolution and sampling set
     * 
     * **NOTE** Does not write to configuration register.
     * 
     * @param cfg Variable with `Configuration` value
     * @param reso See `ADCSettings` docstring for options
     * @returns Updated `Configuration` value
     */
    async setShuntADCResolutionAndSampling(cfg: Configuration, reso: ADCSetting):
    Promise<Configuration> {
        return this.setADCResolutionAndSampling(cfg, reso, ADCShiftAmount.SADC);
    }

    /**
     * Calibrate
     * 
     * **Note**: Datasheet describes secondary, correctional calibration step.
     * It is not implemented yet.
     * 
     * @param vBusMax Volts
     * @param vShuntMax Volts
     * @param shuntResistance Ohms
     */
    async calibrate() {
        let calibration = await this.readCalibration();

        let vBusMax = this.config.maxBusVoltage ? 32000 : 16000; // mV
        let shuntResistance = this.config.shuntResistance!; // Ohm
        let vShuntMax = 40 << this.config.maxShuntVoltage!; // mV

        // Calculate max possible/expected current
        const maxExpectedI = vShuntMax / shuntResistance; // mA

        this.debug(`Bus Max / mV: ${vBusMax}, Shunt Max / mV: ${vShuntMax}, Current Max / mA: ${maxExpectedI}, Shunt Resistance / Ohm: ${shuntResistance}`);

        // Calculate min and max LSB.
        const minimumLSB = maxExpectedI / 32767; // mA
        const maximumLSB = maxExpectedI / 4096; // mA
        this.lsbCurrent = Math.pow(10, Math.ceil(Math.log10(minimumLSB))); // Balance between most resolution and simple 10-decimal representation

        this.debug(`LSB / mA: ${this.lsbCurrent} (Min: ${minimumLSB}, Max: ${maximumLSB})`);

        // Calculate calibration register value and write it to the register.
        calibration = Number.parseInt((0.04096 / (this.lsbCurrent / 1000 * shuntResistance)).toFixed(0));

        await this.writeCalibration(calibration);

        // Calculate power LSB
        this.lsbPower = 20 * this.lsbCurrent;

        this.calibration = calibration;
    }

    /**
     * Read raw configuration register value
     * 
     * @returns Configuration register value
     */
    async readConfiguration(): Promise<Configuration> {
        const configuration = (await this.readBytes(CONFIGURATION_REGISTER, 2)).readUint16BE(0);
        this.debug(`configuration register value: ${configuration}(0b${(configuration >>> 0).toString(2).padStart(16, '0')})`);
        const RST = (configuration >> 15) & 0b1;
        const D14UNUSED = (configuration >> 14) & 0b1;
        const BRNG = (configuration >> 13) & 0b1;
        const PGA = (configuration >> 11) & 0b11;
        const BADC = (configuration >> 7) & 0b1111;
        const SADC = (configuration >> 3) & 0b1111;
        const MODE = (configuration >> 0) & 0b111;
        this.debug(`RST: ${RST}, D14UNUSED: ${D14UNUSED}, BRNG: ${BRNG}, PGA: 0b${(PGA >>> 0).toString(2).padStart(2, '0')}, BADC: 0b${(BADC >>> 0).toString(2).padStart(4, '0')}, SADC: 0b${(SADC >>> 0).toString(2).padStart(4, '0')}, MODE: 0b${(MODE >>> 0).toString(2).padStart(3, '0')} `);
        return configuration;
    }

    async readShuntVoltage(): Promise<number> {
        const shunt_voltage = (await this.readBytes(SHUNT_VOLTAGE_REGISTER, 2)).readInt16BE(0);
        this.debug(`shunt voltage register value: ${shunt_voltage} (0b${(shunt_voltage >>> 0).toString(2).padStart(32, '0').slice(16, 32)})`);
        const SIGN = (shunt_voltage >> 15) & 0b1;
        const LSB = 0.01;
        this.debug(`SIGN: ${SIGN}, LSB / mV: ${LSB} `);
        return shunt_voltage * LSB; // Device takes care of PGA-based shifting
    }

    async readBusVoltage(): Promise<number> {
        const bus_voltage = (await this.readBytes(BUS_VOLTAGE_REGISTER, 2)).readUint16BE(0);
        this.debug(`bus voltage register value: ${bus_voltage} (0b${(bus_voltage >>> 0).toString(2).padStart(16, '0')})`);
        const OVF = (bus_voltage >> 0) & 0b1; // Math Overflow Flag (for Power or Current calculations)
        const CNVR = (bus_voltage >> 1) & 0b1; // Conversion Ready
        const D2UNUSED = (bus_voltage >> 2) & 0b1;
        const LSB = 4;
        this.debug(`OVF: ${OVF}, CNVR: ${CNVR}, D2UNUSED: ${D2UNUSED}, LSB / mV: ${LSB} `);
        return (bus_voltage >> 3) * LSB;
    }

    async readPower(): Promise<number> {
        await this.writeCalibration(this.calibration);
        const power = (await this.readBytes(POWER_REGISTER, 2)).readUint16BE(0);
        this.debug(`power register value: ${power} (0b${(power >>> 0).toString(2).padStart(16, '0')})`);
        const LSB = this.lsbPower!;
        this.debug(`LSB / mW: ${LSB} `);
        return power * LSB;
    }

    async readCurrent(): Promise<number> {
        await this.writeCalibration(this.calibration);
        const current = (await this.readBytes(CURRENT_REGISTER, 2)).readInt16BE(0);
        this.debug(`current register value: ${current} (0b${(current >>> 0).toString(2).padStart(32, '0').slice(16, 32)})`);
        const CSIGN = (current >> 15) & 0b1;
        const LSB = this.lsbCurrent!;
        this.debug(`CSIGN: ${CSIGN}, LSB / mA: ${LSB} `);
        return current * LSB;
    }

    async readCalibration(): Promise<number> {
        const calibration = (await this.readBytes(CALIBRATION_REGISTER, 2)).readUint16BE(0);
        this.debug(`calibration register value: ${calibration} (0b${(calibration >>> 0).toString(2).padStart(16, '0')})`);
        const D0UNUSED = (calibration >> 0) & 0b1;
        this.debug(`D0UNUSED: ${D0UNUSED} `);
        return calibration;
    }

    async writeConfiguration(configuration: Configuration): Promise<Configuration> {
        const buf = Buffer.alloc(2);
        buf.writeUInt16BE(configuration);
        await this.writeBytes(CONFIGURATION_REGISTER, buf);
        return this.readConfiguration();
    }

    async writeCalibration(calibration: number): Promise<number> {
        const buf = Buffer.alloc(2);
        buf.writeUInt16BE(calibration);
        await this.writeBytes(CALIBRATION_REGISTER, buf);
        return this.readCalibration();
    }
}
