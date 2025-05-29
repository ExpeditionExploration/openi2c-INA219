
import { ADCMode, BusVoltageRange, I2CAddress, bindings, PGAGain } from ".";
import { sleep } from "./utils";

async function main() {
    try {
        // Initialize the INA219 sensor
        // The interface has shotInit for triggered measurements
        // and basicInit for continuous measurements.
        await bindings.basicInit(
            I2CAddress.ADDRESS_0,
            "/dev/i2c-1",
            0.1,
            BusVoltageRange.VBUS_RANGE_32V,
            ADCMode.ADC_MODE_12_BIT_128_SAMPLES,
            ADCMode.ADC_MODE_12_BIT_128_SAMPLES,
            PGAGain.GAIN_1_DIV_8
        )
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
        await bindings.softReset();
        await sleep(20); // Wait for 20 ms after reset
        await bindings.basicInit(
            I2CAddress.ADDRESS_0,
            "/dev/i2c-1",
            0.1,
            BusVoltageRange.VBUS_RANGE_32V,
            ADCMode.ADC_MODE_12_BIT_128_SAMPLES,
            ADCMode.ADC_MODE_12_BIT_128_SAMPLES,
            PGAGain.GAIN_1_DIV_8
        );
        console.log("INA219 re-initialized successfully.");
        console.log("-------------------------------");

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
