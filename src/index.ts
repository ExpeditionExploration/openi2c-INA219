
const binding = require('bindings');
import {     
    I2CAddress,
    PGAGain,
    ADCMode,
    BusVoltageRange,
    Mode,
    INA219Handle,
    INA219Info,
} from "./types"
import { type INA219 } from "./types";

const ina219: INA219 = binding('ina219_native');
export {
    I2CAddress, PGAGain, ADCMode, BusVoltageRange, 
    Mode, INA219Handle, INA219Info, ina219
}
