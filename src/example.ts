// import { INA219, Config, ShuntVoltage, BusVoltage } from '..';
// import { sleep } from './utils';

// const BUS = parseInt(process.env.I2C_BUS ?? '5');

// /**
//  * This example shows how to use the INA219 class to read shunt voltage,
//  * bus voltage, current, power, and calibration.
//  */

// async function main() {
//     const ina219 = new INA219(BUS);
//     await ina219.init();

//     let iter: number = 0;
//     while (true) {
//         console.log(`Measurement #${++iter}:`);

//         const shunt = await ina219.readShuntVoltage();
//         const bus = await ina219.readBusVoltage();
//         const power = await ina219.readPower();
//         const current = await ina219.readCurrent();
//         const calibration = await ina219.readCalibration();

//         console.log(`Shunt / mV: ${shunt}, Bus / mV: ${bus}, Current / mA: ${current}, Power / mW: ${power}`);

//         await sleep(300);
//     }
// }
// main();
