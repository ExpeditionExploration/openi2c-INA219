import { error } from 'console';
import { Config, INA219 } from '../..';
import { ADCSetting } from '../constants';
import { sleep } from '../utils';

const bus = parseInt(process.env.I2C_BUS ?? '5');
const address = parseInt(process.env.I2C_ADDRESS ?? '0x40');
const mocked = (process.env.OPENI2C_MOCKED ?? 'false') === 'true';

test('Tests resetting and restoring previous config and calibration', async () => {
    // Create ina219 instance with non-default settings
    const conf: Config = {
        address: address,
        busADCResolution: ADCSetting.Reso12bSamples128,
        shuntADCResolution: ADCSetting.Reso12bSamples128,
    };
    const ina219 = new INA219(bus, conf);
    await ina219.init(); // Init waits internally some.

    const _defaultConfig = await ina219.readConfiguration();
    if ((process.env.DEBUG ?? 'false') === 'true') {
        console.log('conf_1', `0x${_defaultConfig.toString(16)}`);
    }
    await ina219.resetAndReinstateSettings();
    const conf_2 = await ina219.readConfiguration();
    if (!mocked) {
        expect(conf_2).toEqual(0x3fff);
    }
});


test('Tests resetting and restoring previous config and calibration', async () => {
    // Create ina219 instance with non-default settings
    const conf: Config = {
        address: address,
        busADCResolution: ADCSetting.Reso12bSamples128,
        shuntADCResolution: ADCSetting.Reso12bSamples128,
    };
    const ina219 = new INA219(bus, conf);
    await ina219.init(); // Init waits internally some.

    const calibBeforeResetting = await ina219.readCalibration();
    if (process.env.DEBUG ?? false) {
        console.log(`calibBeforeResetting: 0x${calibBeforeResetting.toString(16)}`);
    }
    await ina219.resetAndReinstateSettings();
    const calibration = await ina219.readCalibration();
    expect(calibration).toEqual(calibBeforeResetting);
});
