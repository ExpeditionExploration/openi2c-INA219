
const binding = require('bindings');
import {     
    I2CAddress,
    PGAGain,
    ADCMode,
    BusVoltageRange,
    INA219Mode,
    INA219Info,
    Config,
} from "./types"
import { type INA219 } from "./types";
import * as utils from './utils';

const bindings: INA219 = binding('ina219_native');

// Bindins
export {
    bindings,
    utils,
    I2CAddress, PGAGain, ADCMode, BusVoltageRange, 
    INA219Mode, INA219Info
}
