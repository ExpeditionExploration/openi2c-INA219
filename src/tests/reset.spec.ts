import { Config, INA219 } from '../..';
import { sleep } from '../utils';
import { ADCSetting } from '../constants';

const bus = parseInt(process.env.I2C_BUS ?? '5');
const address = parseInt(process.env.I2C_ADDRESS ?? '0x40');
const mocked = (process.env.OPENI2C_MOCKED ?? 'false') === 'true';

test('Tests resetting resets configuration', async () => {
    // Create ina219 instance with non-default settings
    const conf: Config = {
        address: address,
        busADCResolution: ADCSetting.Reso12bSamples128,
        shuntADCResolution: ADCSetting.Reso12bSamples128,
    };
    const ina219 = new INA219(bus, conf);
    await ina219.init();

    // Read the configuration and check the adc settings have been reset
    await ina219.reset();
    const resetedConf = await ina219.readConfiguration();
    if (!mocked) {
        expect((resetedConf >> 3) & 0xF).toEqual(ADCSetting.Reso12b); // SADC
        expect((resetedConf >> 7) & 0xF).toEqual(ADCSetting.Reso12b); // BADC
    }
});


test('Tests resetting device resets calibration', async () => {
    // Create ina219 instance with non-default settings
    const conf: Config = {
        address: address,
        busADCResolution: ADCSetting.Reso12bSamples128,
        shuntADCResolution: ADCSetting.Reso12bSamples128,
    };
    const ina219 = new INA219(bus, conf);

    // Initialize the device to set the calibration value
    await ina219.init();

    const calibAfterInit = await ina219.readCalibration();
    expect(calibAfterInit).not.toEqual(0x0);
    // Read the configuration and check the adc settings have been reset
    await ina219.reset();
});
