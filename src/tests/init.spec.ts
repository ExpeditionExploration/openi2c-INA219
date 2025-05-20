import { Config, INA219 } from '../..';
import { sleep } from '../utils';
import { ADCSetting } from '../constants';

const bus = parseInt(process.env.I2C_BUS ?? '5');
const address = parseInt(process.env.I2C_ADDRESS ?? '0x40');
const mocked = (process.env.OPENI2C_MOCKED ?? 'false') === 'true';


test('Tests init required first', async () => {
    // Create ina219 instance with non-default settings
    const conf: Config = {
        address: address,
        busADCResolution: ADCSetting.Reso12bSamples128,
        shuntADCResolution: ADCSetting.Reso12bSamples128,
    };
    const ina219 = new INA219(bus, conf);

    await expect(() => ina219.calibrate()).rejects.toThrow("The module has not been initialised, call init() first.");
});
