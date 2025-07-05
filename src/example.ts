
import { ADCMode, BusVoltageRange, I2CAddress, bindings, PGAGain } from ".";
import { Config, INA219Mode } from "./types";
import { sleep } from "./utils";

async function main() {
    try {
        const cfg: Config = {
            addr: I2CAddress.ADDRESS_0,
            // Change this to your I2C bus.
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
