# INA219 driver

Datasheet: http://www.adafruit.com/datasheets/ina219.pdf

**You probably shouldn't use these bindings quite yet. :D**

## About

INA219 is a current, voltage and power measurement module. This TypeScript
module wraps the [LibDriver INA219](https://github.com/libdriver/ina219) driver.

This repo contains Node bindings to that driver. The LibDriver's driver isn't
Linux specific, but these bindings are.

The bindings are written in C, and need to be compiled for them to work. There
are no pre-compiled binaries. Installing by installing OpenI2C should get you
going, but if you want this driver only, see *Building* for how to build the
binary for Node.


## Usage

```ts
import { ADCMode, BusVoltageRange, I2CAddress, bindings, PGAGain } from ".";
import { Config, INA219Mode } from "./types";
import { sleep } from "./utils";

async function main() {
    try {
        const cfg: Config = {
            addr: I2CAddress.ADDRESS_0,
            i2cDevice: "/dev/i2c-1",
            // Rest of the fields are optional. Below are the defaults.
            pgaGain: PGAGain.GAIN_1_DIV_8,
            busAdcMode: ADCMode.ADC_MODE_12_BIT_128_SAMPLES,
            shuntAdcMode: ADCMode.ADC_MODE_12_BIT_128_SAMPLES,
            r: 0.1,
            voltageRange: BusVoltageRange.VBUS_RANGE_32V,
            mode: INA219Mode.SHUNT_BUS_VOLTAGE_CONTINUOUS
        }
        // Initialize the INA219 sensor
        // The interface has shotInit for triggered measurements
        // and basicInit for continuous measurements.
        await bindings.init(cfg)
        console.log("INA219 initialized successfully.");
        console.log("Sensor information:");
        const info = await bindings.getSensorInfo();
        console.log(`chipName: ${info.chipName}`);
        console.log(`manufacturerName: ${info.manufacturerName}`);
        console.log(`interface: ${info.interface}`);
        console.log(`supplyVoltageMinV: ${info.supplyVoltageMinV} V`);
        console.log(`supplyVoltageMaxV: ${info.supplyVoltageMaxV} V`);
        console.log(`maxCurrentMilliA: ${info.maxCurrentMilliA} mA`);
        console.log(`temperatureMin: ${info.temperatureMin} °C`);
        console.log(`temperatureMax: ${info.temperatureMax} °C`);
        console.log(`driverVersion: ${info.driverVersion}`);
        console.log("-------------------------------");
        console.log("Reset and re-initialize the INA219 sensor.");

        while (true) {
            const shuntVoltage = await bindings.getShuntVoltage();
            const busVoltage = await bindings.getBusVoltage();
            const current = await bindings.getCurrent();
            console.log(`Shunt Voltage: ${shuntVoltage} mV`);
            console.log(`Bus Voltage: ${busVoltage} mV`);
            console.log(`Current: ${current} mA`);
            console.log(`Power: ${bindings.getPower()} mW`);
            await sleep(1000); // Wait for 1 second before the next reading
            console.log("-------------------------------");
        }

    } catch (error) {
        console.error("Error initializing INA219:", error);
    }
}

main()
```


## Running tests

tbd


## Building

For building you need a C toolchain. I've only built this using GCC.

Building the wrapper module is done by using one of the npm build scripts
defined in the `package.json` file:

```bash
npm run build
```

or

```bash
npm run bear-build
```

[Bear](https://github.com/rizsotto/Bear) is a tool which generates the
`compile_commands.json`-file. I didn't get clangd completions to work without
it in this node-gyp project. If you need to install it, your distro's package
manager likely has it.

Node headers need to be available in one of the system include directories,
such as `/usr/local/include/node`.


## Tested boards

So far I've only tried this on *Raspberry Pi 4B* using *Node v22.14.0*.
