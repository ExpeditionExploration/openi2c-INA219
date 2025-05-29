
import { ADCMode, BusVoltageRange, I2CAddress, ina219, PGAGain } from ".";
import { sleep } from "./utils";

async function main() {
    try {
        // Initialize the INA219 sensor
        await ina219.basicInit(
            I2CAddress.ADDRESS_0,
            "/dev/i2c-1",
            0.5,
            BusVoltageRange.VBUS_RANGE_32V,
            ADCMode.ADC_MODE_12_BIT_128_SAMPLES,
            ADCMode.ADC_MODE_12_BIT_128_SAMPLES,
            PGAGain.GAIN_1_DIV_8
        )
        console.log("INA219 initialized successfully.");
        console.log("Sensor information:");
        const info = await ina219.getSensorInfo();
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

        while (true) {
            const shuntVoltage = await ina219.getShuntVoltage();
            const busVoltage = await ina219.getBusVoltage();
            const current = await ina219.getCurrent();
            console.log(`Shunt Voltage: ${shuntVoltage} mV`);
            console.log(`Bus Voltage: ${busVoltage} mV`);
            console.log(`Current: ${current} mA`);
            console.log(`Power: ${ina219.getPower()} mW`);
            await sleep(1000); // Wait for 1 second before the next reading
            console.log("-------------------------------");
            // await ina219.softReset();
            // await sleep(20);
        }

    } catch (error) {
        console.error("Error initializing INA219:", error);
    }
}

main().then(() => {
    console.log("INA219 initialized successfully.");
}).catch((error) => {
    console.error("Failed to initialize INA219:", error);
});
